/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 17:09:17 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/23 13:35:39 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

namespace
{

std::string toLowerCopy(const std::string &value)
{
    std::string lowered = value;
    for (std::string::size_type i = 0; i < lowered.size(); ++i)
        lowered[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(lowered[i])));
    return lowered;
}

std::string trimSpaces(const std::string &value)
{
    std::string::size_type begin = value.find_first_not_of(" \t");
    if (begin == std::string::npos)
        return ("");
    std::string::size_type end = value.find_last_not_of(" \t");
    return (value.substr(begin, end - begin + 1));
}

std::string getHeaderValue(const std::string &request, const std::string &headerName)
{
    std::string::size_type headerEnd = request.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return ("");

    std::string loweredHeaderName = toLowerCopy(headerName);
    std::string::size_type current = request.find("\r\n");
    if (current == std::string::npos)
        return ("");
    current += 2;

    while (current < headerEnd)
    {
        std::string::size_type lineEnd = request.find("\r\n", current);
        if (lineEnd == std::string::npos || lineEnd > headerEnd)
            break;
        std::string line = request.substr(current, lineEnd - current);
        std::string::size_type colon = line.find(':');
        if (colon != std::string::npos)
        {
            std::string key = toLowerCopy(trimSpaces(line.substr(0, colon)));
            if (key == loweredHeaderName)
                return (trimSpaces(line.substr(colon + 1)));
        }
        current = lineEnd + 2;
    }
    return ("");
}

bool parseDecimalLength(const std::string &value, size_t &contentLength)
{
    std::stringstream ss(value);
    size_t parsed = 0;
    if (!(ss >> parsed))
        return (false);
    if (!ss.eof())
        return (false);
    contentLength = parsed;
    return (true);
}

bool hasChunkedEncoding(const std::string &transferEncoding)
{
    std::string lowered = toLowerCopy(transferEncoding);
    std::stringstream ss(lowered);
    std::string token;

    while (std::getline(ss, token, ','))
    {
        if (trimSpaces(token) == "chunked")
            return (true);
    }
    return (false);
}

bool isCompleteChunkedBody(const std::string &request, std::string::size_type bodyStart)
{
    std::string::size_type current = bodyStart;

    while (current < request.size())
    {
        std::string::size_type lineEnd = request.find("\r\n", current);
        if (lineEnd == std::string::npos)
            return (false);

        std::string sizeToken = request.substr(current, lineEnd - current);
        std::string::size_type semicolon = sizeToken.find(';');
        if (semicolon != std::string::npos)
            sizeToken = sizeToken.substr(0, semicolon);
        sizeToken = trimSpaces(sizeToken);

        std::stringstream ss(sizeToken);
        size_t chunkSize = 0;
        ss >> std::hex >> chunkSize;
        if (sizeToken.empty() || ss.fail() || !ss.eof())
            return (true);

        current = lineEnd + 2;
        if (chunkSize == 0)
            return (true);

        if (current + chunkSize + 2 > request.size())
            return (false);
        if (request.substr(current + chunkSize, 2) != "\r\n")
            return (true);

        current += chunkSize + 2;
    }
    return (false);
}

bool isCompleteHttpRequest(const std::string &request)
{
    std::string::size_type headerEnd = request.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return (false);

    std::string::size_type bodyStart = headerEnd + 4;
    std::string transferEncoding = getHeaderValue(request, "transfer-encoding");
    if (!transferEncoding.empty() && hasChunkedEncoding(transferEncoding))
        return (isCompleteChunkedBody(request, bodyStart));

    std::string contentLengthValue = getHeaderValue(request, "content-length");
    if (!contentLengthValue.empty())
    {
        size_t contentLength = 0;
        if (!parseDecimalLength(contentLengthValue, contentLength))
            return (true);
        return ((request.size() - bodyStart) >= contentLength);
    }
    return (true);
}

} // namespace

// =====================
// == Canonical Form  ==
// =====================

Webserv::Webserv() : _vector_server(0), _webser_epoll(-1)
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
    this->_vector_server = other._vector_server;
    return (*this);
}

// =====================
// == Getter & Setter ==
// =====================

// SERVERS
const std::vector<Server *> &Webserv::getServers(void) const
{
    return _vector_server;
}

const std::map<int, std::set<Server *> > &Webserv::getFdToServersMap(void) const
{
    return _map_fd_to_serv;
}

void Webserv::setEpollFd(const int epoll)
{
    _webser_epoll = epoll;
}

int Webserv::getEpollFd(void)
{
    return _webser_epoll;
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
    _vector_server.clear();

    try
    {
        while (!tokens.empty())
        {
            Server *server = new Server(this);
            _vector_server.push_back(server);
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

void Webserv::initializeSocket()
{
    int serverSocket;
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
            {
                serverSocket = createServerSocket(listenConfig->ip, listenConfig->port, MAX_CLIENT);
                addSocketToEvent(getEpollFd(), serverSocket, NULL);

                map_socket_fd.insert(std::make_pair(*listenConfig, serverSocket));

                // Création directe dans la map
                _map_fd_to_serv.insert(std::make_pair(serverSocket, std::set<Server *>()));
                _map_fd_to_serv[serverSocket].insert(server);
            }
            else
            {
                serverSocket = it->second;
                std::set<Server *> &set_server = _map_fd_to_serv[serverSocket];
                if (set_server.find(server) == set_server.end())
                    set_server.insert(server);
            }
        }
    }
}

void Webserv::handleNewClient(int server_fd)
{
    int clientSocket;

    if ((clientSocket = accept(server_fd, NULL, NULL)) == -1)
        throw ExecptionErrorFunction("accept");

    Client *client = new Client;

    addSocketToEvent(getEpollFd(), clientSocket, client);
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
    std::cout << "Client is disconnected\n";
}

void Webserv::receiveMessageFromClient(Client *client)
{
    int bytes;
    char buffer[SIZE_BUFFER];

    if ((bytes = recv(client->getClientFd(), buffer, sizeof(buffer), 0)) == -1)
        return;

    if (bytes == 0)
    {
        deleteClient(client);
        return;
    }

    std::string s;
    s.assign(buffer, buffer + bytes);
    client->appendRequest(s);

    if (!isCompleteHttpRequest(client->getRequest()))
        return;

    std::cout << "Final Message from client: " << client->getRequest() << "\n";
    HttpRequest request(client);
    client->clearRequest();

    std::string reply = "Message received\n";
    send(client->getClientFd(), reply.c_str(), reply.size(), 0);
}

void Webserv::handleConnection(struct epoll_event &events)
{
    std::string reply = "Message received\n";
    int server_fd = events.data.fd;

    if (server_fd < static_cast<int>(_map_fd_to_serv.size() + 4))
        handleNewClient(server_fd);

    else
        receiveMessageFromClient(static_cast<Client *>(events.data.ptr));
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

    struct sigaction sa;
    initializeSignal(sa);
    sigaction(SIGINT, &sa, NULL);
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
    std::map<int, std::set<Server *> >::iterator it_fd = _map_fd_to_serv.begin();
    for (; it_fd != _map_fd_to_serv.end(); ++it_fd)
        close((*it_fd).first);
    _map_fd_to_serv.clear();

    // Free instance server
    std::vector<Server *>::iterator it_server = _vector_server.begin();
    for (; it_server != _vector_server.end(); ++it_server)
        delete (*it_server);
    _vector_server.clear();

    close(getEpollFd());
}
