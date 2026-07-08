/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 17:09:17 by fmotte            #+#    #+#             */
/*   Updated: 2026/07/08 22:01:30 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

#include "Client.hpp"
#include "Header.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "ARequest.hpp"
#include "Server.hpp"
#include "RequestContext.hpp"
#include "StaticRequest.hpp"
#include "CGIRequest.hpp"

#include "colors.hpp"
#include "execption.hpp"
#include "utilsConnection.hpp"
#include "utilsRequest.hpp"

#include <cstring>
#include <errno.h>
#include <sys/wait.h>

// =====================
// == Canonical Form  ==
// =====================

Webserv::Webserv() : _vectorServer(0), _vectorClient(0), _webserEpoll(-1)
{
}

Webserv::~Webserv()
{
}

Webserv::Webserv(const Webserv &other)
{
    *this = other;
}

Webserv &Webserv::operator=(const Webserv &other)
{
    if (this != &other)
    {
        _vectorServer = other._vectorServer;
        _vectorClient = other._vectorClient;
        _mapFdToServer = other._mapFdToServer;
        _sessions = other._sessions;
        _webserEpoll = other._webserEpoll;
    }
    return (*this);
}

// =====================
// == Getter & Setter ==
// =====================

// SERVERS
const std::vector<Server *> &Webserv::getServers(void) const
{
    return _vectorServer;
}

const std::vector<Client *> &Webserv::getClients(void) const
{
    return _vectorClient;
}

const std::map<int, std::set<Server *> > &Webserv::getFdToServersMap(void) const
{
    return _mapFdToServer;
}

void Webserv::setEpollFd(const int epoll)
{
    _webserEpoll = epoll;
}

int Webserv::getEpollFd(void)
{
    return _webserEpoll;
}

int Webserv::touchSession(const std::string &sessionId)
{
    cleanupSessions();

    SessionInfo &session = _sessions[sessionId];
    session.lastSeen = time(NULL);
    return ++session.visits;
}

void Webserv::cleanupSessions(void)
{
    time_t now = time(NULL);

    std::map<std::string, SessionInfo>::iterator it = _sessions.begin();
    while (it != _sessions.end())
    {
        if (now - it->second.lastSeen > SESSION_TTL)
            _sessions.erase(it++); // post-increment keeps a valid iterator (C++98)
        else
            ++it;
    }
}

// =====================
// ==     Method      ==
// =====================

bool Webserv::initializeWebserv(std::vector<std::string> &tokens)
{
    return splitIntoServers(tokens);
}

bool Webserv::splitIntoServers(std::vector<std::string> &tokens)
{
    _vectorServer.clear();

    try
    {
        while (!tokens.empty())
        {
            Server *server = new Server(this);
            _vectorServer.push_back(server);
            server->initializeServer(tokens);
            server->initializeCheck();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return (true);
    }
    return (false);
}

void Webserv::registerNewSocket(std::map<Listen, int> &map_socket_fd, Listen *listenConfig, Server *server)
{
    int serverSocket = createServerSocket(listenConfig->ip, listenConfig->port, MAX_CLIENT);
    addFdToEvent(getEpollFd(), serverSocket, EPOLLIN, SERVER, server);

    map_socket_fd.insert(std::make_pair(*listenConfig, serverSocket));

    _mapFdToServer.insert(std::make_pair(serverSocket, std::set<Server *>()));
    _mapFdToServer[serverSocket].insert(server);
}

void Webserv::registerExistingSocket(int serverSocket, Server *server)
{
    std::set<Server *> &set_server = _mapFdToServer[serverSocket];

    if (set_server.find(server) == set_server.end())
        set_server.insert(server);
}

void Webserv::initializeSocket()
{
    Listen *listenConfig;
    Server *server;

    std::map<Listen, int> map_socket_fd;
    std::map<Listen, int>::iterator it;

    std::vector<Server *> vector_server = getServers();

    for (size_t i = 0; i < vector_server.size(); ++i)
    {
        server = vector_server[i];

        for (size_t j = 0;; ++j)
        {
            listenConfig = server->getListen(j);

            if (!listenConfig)
                break;

            it = map_socket_fd.find(*listenConfig);

            if (it == map_socket_fd.end())
                registerNewSocket(map_socket_fd, listenConfig, server);

            else
                registerExistingSocket(it->second, server);
        }
    }
}

void Webserv::handleNewClient(int server_fd)
{
    int clientSocket;

    if ((clientSocket = accept(server_fd, NULL, NULL)) == -1)
        throw ExecptionErrorFunction("accept");

    Client *client = new Client;

    _vectorClient.push_back(client);

    addFdToEvent(getEpollFd(), clientSocket, EPOLLIN, CLIENT, client);
    
    client->setClientFd(clientSocket);
    client->setServerFd(server_fd);
    client->setWebserv(this);

    std::cout << "Nouveau client connecté: fd=" << client->getClientFd() << "\n";
}

void Webserv::deleteClient(Client *client)
{
    if (epoll_ctl(getEpollFd(), EPOLL_CTL_DEL, client->getClientFd(), NULL) == -1)
        throw ExecptionErrorFunction("epoll_ctl");

    close(client->getClientFd());
    delete client;
    _vectorClient.erase(std::find(_vectorClient.begin(), _vectorClient.end(), client));

    std::cout << "Client is disconnected\n";
}

bool Webserv::readAndCheckRequestCompletion(Client *client)
{
    int bytes;
    char buffer[SIZE_BUFFER];

    if ((bytes = recv(client->getClientFd(), buffer, sizeof(buffer), 0)) == -1)
        return true;

    // if (bytes == 0)
    // {
    //     deleteClient(client);
    //     return true;
    // }

    std::string s;
    s.assign(buffer, buffer + bytes);
    client->appendContentRequest(s);

    if (!isCompleteRequest(client->getContentRequest()))
        return true;

    return false;
}

void Webserv::processClient(EventData *eventData)
{
    Client *client = static_cast<Client *>(eventData->ptr);
    
    if (processClientRequest(client))
        processClientResponse(client);
}

bool Webserv::processClientRequest(Client *client)
{
    if (readAndCheckRequestCompletion(client))
        return false;

    //std::cout << "Final Message from client: " << client->getContentRequest() << "\n";
    return true;
}

void  Webserv::writeToChild(EventData *eventData)
{
    CGIRequest *cgiRequest = static_cast<CGIRequest *>(eventData->ptr);
    cgiRequest->sendDataToChild();
}

void Webserv::readToChild(EventData *eventData)
{
    CGIRequest *cgiRequest = static_cast<CGIRequest *>(eventData->ptr);
    
    cgiRequest->receivedDataFromChild();
    waitpid(cgiRequest->getPid(), NULL, 0);
    cgiRequest->processDataFromChild();
}

void Webserv::processClientResponse(Client *client)
{
    client->initialisationClient();
    client->selectTypeRequest();
    
    if (client->getTypeRequest() == STATIC)
    {
        StaticRequest *staticRequest = dynamic_cast<StaticRequest *>(client->getARequest());
        staticRequest->selectMethodHttp();
    }
    else
    {
        CGIRequest *cgiRequest = dynamic_cast<CGIRequest *>(client->getARequest());
        cgiRequest->initializationCGIRequest("/usr/bin/python3");
    }
}

void Webserv::handleConnection(struct epoll_event &events)
{
    EventData *eventData = static_cast<EventData *>(events.data.ptr);
    
    switch (eventData->type)
    {
    
        case(SERVER): handleNewClient(eventData->fd); return;
        case(CLIENT): processClient(eventData); break;
        case(WRITECHILD): writeToChild(eventData); break;
        case(READCHILD): readToChild(eventData); break;
    }
    
    if (eventData->type == CLIENT)
    {
        Client *client = static_cast<Client *>(eventData->ptr);
        
        if (client->getTypeRequest() == STATIC)
        {
            HttpResponse response(client->getARequest());
            response.initialisationHttpResponse();
            response.sendToClient();
            client->clearContentRequest(); 
        }
    }
    else if (eventData->type == READCHILD)
    {
        CGIRequest *cgiRequest = static_cast<CGIRequest *>(eventData->ptr);
        Client *client = cgiRequest->getRequestContext()->getClient();
        
        HttpResponse response(cgiRequest);
        response.initialisationHttpResponse();
        response.sendToClient();
        client->clearContentRequest();
    }
}

void Webserv::listenToWebserv()
{
    int nfds;
    struct epoll_event events[MAX_EVENTS];

    std::cout << "Serveur en attente..." << std::endl;

    while (1)
    {
        if ((nfds = epoll_wait(getEpollFd(), events, MAX_EVENTS, -1)) == -1)
            throw ExecptionErrorFunction("epoll_wait");

        if (stop_webserv)
            return;

        for (int i = 0; i < nfds; ++i)
            handleConnection(events[i]);
    }
}

bool Webserv::initializeConnection()
{
    int epoll_fd;
    bool res = false;

    initializeSignal();
    errno = 0;

    try
    {
        if ((epoll_fd = epoll_create(1)) == -1)
            throw ExecptionErrorFunction("epoll_create");

        setEpollFd(epoll_fd);
        initializeSocket();
        listenToWebserv();
    }
    catch (const std::exception &e)
    {
        if (!stop_webserv)
        {
            std::cerr << e.what() << '\n';
            if (errno != 0)
                std::cout << "More info: " << strerror(errno) << "\n";
            res = true;
        }
    }
    closeConnection();
    return res;
}

void Webserv::closeConnection()
{
    // Close fd client
    // Close fd server
    std::map<int, std::set<Server *> >::iterator it_fd = _mapFdToServer.begin();
    for (; it_fd != _mapFdToServer.end(); ++it_fd)
        close((*it_fd).first);
    _mapFdToServer.clear();

    // Free instance server
    std::vector<Server *>::iterator it_server = _vectorServer.begin();
    for (; it_server != _vectorServer.end(); ++it_server)
        delete (*it_server);
    _vectorServer.clear();

    // Free instance client
    std::vector<Client *>::iterator it_client = _vectorClient.begin();
    for (; it_client != _vectorClient.end(); ++it_client)
        delete (*it_client);
    _vectorClient.clear();

    close(getEpollFd());
}
