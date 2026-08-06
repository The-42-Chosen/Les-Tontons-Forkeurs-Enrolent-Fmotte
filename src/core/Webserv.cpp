/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 17:09:17 by fmotte            #+#    #+#             */
/*   Updated: 2026/08/06 19:39:46 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

#include "ARequest.hpp"
#include "CGIRequest.hpp"
#include "Client.hpp"
#include "Header.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "RequestContext.hpp"
#include "ResponseContext.hpp"
#include "Server.hpp"
#include "StaticRequest.hpp"

#include "colors.hpp"
#include "execption.hpp"
#include "utilsConnection.hpp"
#include "utilsRequest.hpp"

#include <cstring>
#include <sstream>
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
        _setEventData = other._setEventData;
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

std::set<EventData *> Webserv::getSetEventData(void) const
{
    return _setEventData;
}

void Webserv::addSetEventData(EventData *eventData)
{
    if (eventData == NULL)
        throw ExecptionErrorUninitializedVariable("*eventData", "Webserv");

    _setEventData.insert(eventData);
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

    if (_vectorServer.empty())
    {
        std::cerr << "Error: No server defined in the config file\n";
        return (true);
    }
    return (false);
}

void Webserv::registerNewSocket(std::map<Listen, int> &map_socket_fd, Listen *listenConfig, Server *server)
{
    int serverSocket = createServerSocket(listenConfig->ip, listenConfig->port, MAX_CLIENT);
    EventData *eventData = addFdToEvent(getEpollFd(), serverSocket, EPOLLIN, SERVER, server);

    server->addEventData(eventData);
    server->setWebserv(this);
    addSetEventData(eventData);

    map_socket_fd.insert(std::make_pair(*listenConfig, serverSocket));

    _mapFdToServer.insert(std::make_pair(serverSocket, std::set<Server *>()));
    _mapFdToServer[serverSocket].insert(server);
}

void Webserv::registerExistingSocket(int serverSocket, Server *server)
{
    std::set<Server *> &set_server = _mapFdToServer[serverSocket];

    server->setWebserv(this);

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

    client->setClientFd(clientSocket);
    client->setServerFd(server_fd);
    client->setWebserv(this);

    try
    {
        EventData *eventData = addFdToEvent(getEpollFd(), clientSocket, EPOLLIN, CLIENT, client);
        client->setEventData(eventData);
        addSetEventData(eventData);
    }
    catch (const std::exception &e)
    {
        deleteClient(client);
        throw;
    }

    std::cout << "Nouveau client connecté: fd=" << client->getClientFd() << "\n";
}

void Webserv::deleteClient(Client *client)
{
    _vectorClient.erase(std::find(_vectorClient.begin(), _vectorClient.end(), client));
    _setEventData.erase(client->getEventData());
    delete client;

    std::cout << "Client is disconnected\n";
}

void Webserv::handleDisconnect(Client *client)
{
    // Already disconnected: no event may be handled for this client anymore
    if (client->isPendingDelete())
        return;

    // Explicit DEL before close: a CGI child forked in the same loop turn
    // still holds a copy of the fd, so close() alone would leave a stale
    // registration in epoll
    epoll_ctl(getEpollFd(), EPOLL_CTL_DEL, client->getClientFd(), NULL);
    _setEventData.erase(client->getEventData());

    if (client->isCGIProcessing())
        client->setPendingDelete(true);
    else
        deleteClient(client);
}

size_t Webserv::largestConfiguredMaxBodySize()
{
    size_t largest = 0;

    for (size_t i = 0; i < _vectorServer.size(); ++i)
    {
        Server *server = _vectorServer[i];
        if (server == NULL)
            continue;

        if (server->getClientMaxBodySize() > largest)
            largest = server->getClientMaxBodySize();

        for (size_t j = 0; server->getLocation(j) != NULL; ++j)
        {
            if (server->getLocation(j)->getClientMaxBodySize() > largest)
                largest = server->getLocation(j)->getClientMaxBodySize();
        }
    }
    return largest;
}

RequestState Webserv::readAndCheckRequestCompletion(Client *client)
{
    int bytes;
    char buffer[SIZE_BUFFER];

    if ((bytes = recv(client->getClientFd(), buffer, sizeof(buffer), 0)) < 0)
        return REQUEST_DISCONNECTED;

    if (bytes == 0)
    {
        client->setPeerClosed(true);
        if (isCompleteRequest(client->getContentRequest()))
            return REQUEST_COMPLETE;
        return REQUEST_DISCONNECTED;
    }

    std::string s;
    s.assign(buffer, buffer + bytes);
    client->appendContentRequest(s);

    if (!isCompleteRequest(client->getContentRequest()))
    {
        if (isDeclaredBodySizeExceeding(client->getContentRequest(), largestConfiguredMaxBodySize()))
            return REQUEST_COMPLETE;
        return REQUEST_INCOMPLETE;
    }

    return REQUEST_COMPLETE;
}

// If error code not set, everything else becomes a 500.
static int statusCodeFromException(const std::exception &e)
{
    std::stringstream stream(e.what());
    int statusCode = 0;

    if (!(stream >> statusCode) || !stream.eof() || statusCode < 100 || statusCode > 599)
        return 500;
    return statusCode;
}

void Webserv::applyErrorToResponse(Client *client, const std::exception &e)
{
    int statusCode = statusCodeFromException(e);

    std::cerr << "Request failed with status " << statusCode << " (" << e.what() << ")\n";

    // response sent through the STATIC path, even for a failed CGI
    client->setTypeRequest(STATIC);

    if (client->getARequest() != NULL && client->getARequest()->getResponseContext() != NULL)
        client->getARequest()->getResponseContext()->setStatusCode(statusCode);
}

// Build the response and arm EPOLLOUT: the actual send happens when epoll
// tells us the socket is writable (sendPendingDataToClient).
// Return TRUE if client has been deleted
bool Webserv::sendResponseToClient(Client *client)
{
    if (client->isPendingDelete())
    {
        deleteClient(client);
        return true;
    }

    if (client->getARequest() == NULL || client->getARequest()->getResponseContext() == NULL)
        return false;

    HttpResponse response(client->getARequest());
    response.initialisationHttpResponse();

    client->setSendBuffer(response.getResponseContent());
    client->setCloseAfterSend(response.getShouldCloseConnection());

    size_t requestLength = completeRequestLength(client->getContentRequest());
    if (requestLength == 0)
        client->clearContentRequest();
    else
        client->consumeContentRequest(requestLength);

    updateClientEpollEvents(client, EPOLLOUT);
    return false;
}

// One send per EPOLLOUT event; -1 or 0 -> the client is removed,
// partial send -> keep EPOLLOUT armed and wait for the next event
void Webserv::sendPendingDataToClient(Client *client)
{
    if (!client->hasPendingSend())
    {
        updateClientEpollEvents(client, EPOLLIN);
        return;
    }

    const std::string &buffer = client->getSendBuffer();
    ssize_t bytes = send(client->getClientFd(), buffer.c_str() + client->getSendOffset(),
                         buffer.size() - client->getSendOffset(), 0);

    if (bytes <= 0)
    {
        handleDisconnect(client);
        return;
    }

    client->setSendOffset(client->getSendOffset() + bytes);
    if (client->hasPendingSend())
        return;

    client->clearSendState();

    if (client->getCloseAfterSend())
    {
        handleDisconnect(client);
        return;
    }

    if (isCompleteRequest(client->getContentRequest()))
        executeBufferedRequest(client);
    else if (client->isPeerClosed())
        handleDisconnect(client); // FIN received and nothing left to answer
    else
        updateClientEpollEvents(client, EPOLLIN);
}

void Webserv::updateClientEpollEvents(Client *client, uint32_t epollEvents)
{
    struct epoll_event ev;

    ev.events = epollEvents;
    ev.data.ptr = client->getEventData();

    if (epoll_ctl(getEpollFd(), EPOLL_CTL_MOD, client->getClientFd(), &ev) == -1)
        handleDisconnect(client);
}

void Webserv::processClient(EventData *eventData)
{
    Client *client = static_cast<Client *>(eventData->ptr);

    RequestState state = readAndCheckRequestCompletion(client);

    if (state == REQUEST_DISCONNECTED)
    {
        handleDisconnect(client);
        return;
    }
    if (state == REQUEST_INCOMPLETE)
        return;

    if (client->isCGIProcessing())
    {
        if (client->isPeerClosed())
            updateClientEpollEvents(client, 0);
        return;
    }

    executeBufferedRequest(client);
}

// Parse and execute the first request buffered in the client: STATIC
// requests are answered right away, CGI answers later through readToChild
void Webserv::executeBufferedRequest(Client *client)
{
    try
    {
        processClientResponse(client);
    }
    catch (const std::exception &e)
    {
        applyErrorToResponse(client, e);
    }

    if (client->getTypeRequest() == STATIC)
        sendResponseToClient(client);
}

void Webserv::writeToChild(EventData *eventData)
{
    CGIRequest *cgiRequest = static_cast<CGIRequest *>(eventData->ptr);

    // One write per EPOLLOUT event; once the whole body has been fed to the
    // child, its stdin pipe is closed and the event is not watched anymore
    if (cgiRequest->sendDataToChild())
        _setEventData.erase(cgiRequest->geteventDataWrite());
}

void Webserv::readToChild(EventData *eventData)
{
    CGIRequest *cgiRequest = static_cast<CGIRequest *>(eventData->ptr);
    Client *client = cgiRequest->getRequestContext()->getClient();

    try
    {
        if (!cgiRequest->receivedDataFromChild())
            return; // pas encore EOF -> on rendra la main à epoll

        // EOF atteint : l'enfant a fermé stdout, il se termine.
        // Il sera récolté par le waitpid de la boucle principale.
        cgiRequest->closeStdoutPipe();

        cgiRequest->processDataFromChild();
    }
    catch (const std::exception &e)
    {
        applyErrorToResponse(client, e);
    }

    client->setCGIProcessing(false);

    _setEventData.erase(cgiRequest->geteventDataWrite());
    _setEventData.erase(cgiRequest->geteventDataRead());

    sendResponseToClient(client);
}

static std::string selectCgiInterpreter(const std::string &scriptName)
{
    std::string::size_type pos = scriptName.find_last_of('.');
    std::string extension = (pos != std::string::npos) ? scriptName.substr(pos) : "";

    if (extension == ".php")
        return "/usr/bin/php-cgi";
    return "/usr/bin/python3";
}

void Webserv::processClientResponse(Client *client)
{
    client->initialisationClient();
    client->selectTypeRequest();

    client->getARequest()->validateRequest();

    if (client->getTypeRequest() == STATIC)
    {
        StaticRequest *staticRequest = dynamic_cast<StaticRequest *>(client->getARequest());
        staticRequest->selectMethodHttp();
    }
    else
    {
        CGIRequest *cgiRequest = dynamic_cast<CGIRequest *>(client->getARequest());
        std::string scriptName = cgiRequest->getRequestContext()->getHttpRequest()->getHeader()->getScriptName();
        cgiRequest->initializationCGIRequest(selectCgiInterpreter(scriptName));
        client->setCGIProcessing(true);

        addSetEventData(cgiRequest->geteventDataWrite());
        addSetEventData(cgiRequest->geteventDataRead());
    }
}

void Webserv::handleConnection(struct epoll_event &events)
{
    EventData *eventData = static_cast<EventData *>(events.data.ptr);

    if (_setEventData.find(eventData) == _setEventData.end())
        return;

    switch (eventData->type)
    {

    case (SERVER):
        handleNewClient(eventData->fd);
        break;
    case (CLIENT):
        if (events.events & EPOLLOUT)
            sendPendingDataToClient(static_cast<Client *>(eventData->ptr));
        else
            processClient(eventData);
        break;
    case (WRITECHILD):
        writeToChild(eventData);
        break;
    case (READCHILD):
        readToChild(eventData);
        break;
    }
}

void Webserv::listenToWebserv()
{
    int nfds;
    struct epoll_event events[MAX_EVENTS];

    std::cout << "Serveur en attente..." << std::endl;

    while (1)
    {
        nfds = epoll_wait(getEpollFd(), events, MAX_EVENTS, 1000);

        if (stop_webserv)
            return;

        // A signal (EINTR) must not kill the server: retry on the next loop turn
        if (nfds == -1)
            continue;

        // Collect every finished CGI child so none is left as a zombie
        while (waitpid(-1, NULL, WNOHANG) > 0)
            ;

        if (nfds == 0)
        {
            checkTimeOut();
            continue;
        }

        for (int i = 0; i < nfds; ++i)
        {
            try
            {
                handleConnection(events[i]);
            }
            catch (const std::exception &e)
            {
                std::cerr << "Unhandled event error: " << e.what() << "\n";
            }
        }
    }
}

bool Webserv::initializeConnection()
{
    int epoll_fd;
    bool res = false;

    initializeSignal();

    try
    {
        if ((epoll_fd = epoll_create(1)) == -1)
            throw ExecptionErrorFunction("epoll_create");

        if (fcntl(epoll_fd, F_SETFD, FD_CLOEXEC) == -1)
            throw ExecptionErrorFunction("fcntl");

        setEpollFd(epoll_fd);
        initializeSocket();
        listenToWebserv();
    }
    catch (const std::exception &e)
    {
        if (!stop_webserv)
        {
            std::cerr << e.what() << '\n';
            res = true;
        }
    }
    closeConnection();
    return res;
}

void Webserv::checkTimeOut()
{
    std::set<EventData *>::iterator it;
    it = _setEventData.begin();

    for (; it != _setEventData.end(); it++)
    {
        if (getCurrentTime() > (*it)->time + DELAY && (*it)->type == READCHILD)
        {
            std::cout << "KILL process\n";
            CGIRequest *cgiRequest = static_cast<CGIRequest *>((*it)->ptr);

            kill(cgiRequest->getPid(), 9);

            applyErrorToResponse(cgiRequest->getRequestContext()->getClient(), std::runtime_error("504"));

            Client *client = cgiRequest->getRequestContext()->getClient();
            client->setCGIProcessing(false);

            _setEventData.erase(cgiRequest->geteventDataWrite());
            _setEventData.erase(cgiRequest->geteventDataRead());

            // 504 is an error status: the connection closes once it is sent
            sendResponseToClient(client);
            break;
        }
    }
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

    // Free instance client: the destructor closes the still connected sockets
    std::vector<Client *>::iterator it_client = _vectorClient.begin();
    for (; it_client != _vectorClient.end(); ++it_client)
        delete (*it_client);
    _vectorClient.clear();

    close(getEpollFd());
}
