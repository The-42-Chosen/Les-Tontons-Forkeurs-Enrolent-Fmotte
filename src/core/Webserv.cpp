/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 17:09:17 by fmotte            #+#    #+#             */
/*   Updated: 2026/06/30 17:32:47 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

#include "Client.hpp"
#include "Header.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Request.hpp"
#include "Server.hpp"

#include "colors.hpp"
#include "execption.hpp"
#include "utilsConnection.hpp"
#include "struct.hpp"

#include <cstring>
#include <errno.h>

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

    if (bytes == 0)
    {
        deleteClient(client);
        return true;
    }

    std::string s;
    s.assign(buffer, buffer + bytes);
    client->appendContentRequest(s);

    if (!isCompleteRequest(client->getContentRequest()))
        return true;

    return false;
}

void Webserv::processClientRequest(Client *client)
{
    if (readAndCheckRequestCompletion(client))
        return;

    std::cout << "Final Message from client: " << client->getContentRequest() << "\n";

    processClientResponse(client);
}

void Webserv::processClientResponse(Client *client)
{
    HandleRequest request;
    if (request.initialisationRequest(client))
    {
        request.processRequest();
        
        HttpResponse response(&request);
        response.initialisationHttpResponse();
        response.sendToClient();

        client->clearContentRequest();
    }
}

void Webserv::handleConnection(struct epoll_event &events)
{
    std::string reply = "Message received\n";
    
    EventData *eventData =  static_cast<EventData *>(events.data.ptr);
    switch (eventData->type)
    {
        case SERVER:  handleNewClient(eventData->fd); break;
        case CLIENT:  processClientRequest(static_cast<Client *>(eventData->ptr)); break;
        // case PIPEIN:  writeToCgi(static_cast<Client>(data->ptr)); break;
        // case PIPEOUT: readFromCgiAndSend(static_cast<Client *>(data->ptr)); break;
        case PIPEIN:  break;
        case PIPEOUT:  break;
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
