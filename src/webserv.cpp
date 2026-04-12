/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 17:09:17 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/11 18:40:45 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "execption.hpp"
#include "utils_connection.hpp"
#include <cstring>
#include <errno.h>

// =====================
// == Canonical Form  ==
// =====================

Webserv::Webserv() : _vector_server(0)
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
Server *Webserv::get_server(size_t i)
{
    if (i < _vector_server.size())
        return &_vector_server[i];
    else
        return NULL;
}

size_t Webserv::get_servers_count(void)
{
    return _vector_server.size();
}
std::vector<Server> *Webserv::get_all_servers(void)
{
    return &_vector_server;
}

// =====================
// ==     Method      ==
// =====================

bool Webserv::initialisation_webserv(std::vector<std::string> &tokens)
{
    return splitServers(tokens);
}

bool Webserv::splitServers(std::vector<std::string> &tokens)
{
    _vector_server.clear();
    Server server;

    try
    {
        while (!tokens.empty())
        {

            server.initialisation_server(tokens);
            server.initialisation_check();
            _vector_server.push_back(server);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return (true);
    }
    return (false);
}

void Webserv::initialisation_socket(int epoll_fd, int serverSocket)
{
    s_listen *listen;
    Server *srv;
    size_t j;

    for (size_t i = 0; i < get_servers_count(); i++)
    {
        srv = get_server(i);
        j = 0;
        while ((listen = srv->get_listen(j)) != NULL)
        {
            create_server_socket(listen->ip, listen->port, serverSocket, MAX_CLIENT);
            set_nonblocking(serverSocket);
            add_socket_to_event(epoll_fd, serverSocket);
            ++j;
        }
    }
}

void Webserv::manage_connection(int epoll_fd, int serverSocket, int event_fd)
{
    int bytes;
    int clientSocket;
    std::string reply = "Message received\n";

    std::vector<int> &v = _vector_client_fd;
    // New Client
    if (event_fd == serverSocket)
    {
        clientSocket = get_new_client(epoll_fd, serverSocket);
        v.push_back(clientSocket);
    }
    else
    {
        // Perhaps do a function for the message reciced
        clientSocket = event_fd;
        bytes = get_message_from_client(clientSocket, SIZE_BUFFER);

        if (bytes)
            send(clientSocket, reply.c_str(), reply.size(), 0);
        else
            v.erase(find(v.begin(), v.end(), clientSocket));
    }
}
void Webserv::webserv_listen(int epoll_fd, int serverSocket)
{
    int nfds;
    struct epoll_event events[MAX_EVENTS];

    _vector_client_fd.clear();
    std::cout << "Serveur en attente..." << std::endl;

    while (1)
    {
        if ((nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1)) == -1)
            throw ExecptionErrorFunction("epoll_wait");

        if (stop_webserv)
            return;

        for (int i = 0; i < nfds; ++i)
            manage_connection(epoll_fd, serverSocket, events[i].data.fd);
    }
}

bool Webserv::initialisation_connection()
{
    int epoll_fd;
    int serverSocket;
    bool res = false;

    struct sigaction sa;
    init_signal(sa);
    sigaction(SIGINT, &sa, NULL);

    try
    {
        if ((epoll_fd = epoll_create(1)) == -1)
            throw ExecptionErrorFunction("epoll_create");

        if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
            throw ExecptionErrorFunction("socket");

        initialisation_socket(epoll_fd, serverSocket);
        webserv_listen(epoll_fd, serverSocket);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        std::cout << "More info: " << strerror(errno) << "\n";
        res = true;
    }
    close_connection(epoll_fd, serverSocket);
    return res;
}

void Webserv::close_connection(int serverSocket, int epoll_fd)
{
    if (_vector_client_fd.empty())
        return;

    for (size_t i = 0; i < _vector_client_fd.size(); i++)
        close(_vector_client_fd[i]);
    _vector_client_fd.clear();

    close(serverSocket);
    close(epoll_fd);
}