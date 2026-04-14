/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 17:09:17 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/15 20:18:09 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

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

    try
    {
        while (!tokens.empty())
        {
            Server server;
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

void Webserv::initialisation_socket(int epoll_fd)
{
    int serverSocket;
    s_listen *listen;
    Server *server;

    std::map<s_listen, int> map_socket_fd;
    std::map<s_listen, int>::iterator it;

    // For each server
    for (size_t i = 0; i < get_servers_count(); i++)
    {
        // For each listen
        server = get_server(i);

        for (size_t j = 0;; ++j)
        {
            listen = server->get_listen(j);
            if (!listen)
                break;

            it = map_socket_fd.find(*listen);

            if (it == map_socket_fd.end())
            {
                serverSocket = create_server_socket(listen->ip, listen->port, MAX_CLIENT);
                add_socket_to_event(epoll_fd, serverSocket, NULL);

                map_socket_fd.insert(std::make_pair(*listen, serverSocket));

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

void Webserv::get_new_client(int epoll_fd, int server_fd)
{
    int clientSocket;
    Client *client = new Client;

    if ((clientSocket = accept(server_fd, NULL, NULL)) == -1)
    {
        delete client;
        throw ExecptionErrorFunction("accept");
    }

    add_socket_to_event(epoll_fd, clientSocket, client);
    client->set_client_fd(clientSocket);
    client->set_server_fd(server_fd);

    std::cout << "Nouveau client connecté: fd=" << clientSocket << "\n";
}

void Webserv::get_message_from_client(Client *client)
{
    int bytes;
    char buffer[SIZE_BUFFER];

    if ((bytes = recv(client->get_client_fd(), buffer, sizeof(buffer), 0)) == -1)
        throw ExecptionErrorFunction("recv");

    if (bytes == 0)
    {
        close(client->get_client_fd());
        delete client;
        std::cout << "Client is disconnected\n";
        return;
    }

    std::string s;
    s.assign(buffer, buffer + bytes);
    client->append_request(s);

    if (bytes == SIZE_BUFFER)
        return;
    
	if (client->get_request().find("\r\n\r\n") == std::string::npos)
        return;
    
	std::cout << "Final Message from client: " << client->get_request() << "\n";
    HttpRequest request(client->get_request());
    client->clear_request();

    std::string reply = "Message received\n";
    send(client->get_client_fd(), reply.c_str(), reply.size(), 0);
}

void Webserv::manage_connection(int epoll_fd, struct epoll_event &events)
{
    std::string reply = "Message received\n";
    int server_fd = events.data.fd;

    if (server_fd < static_cast<int>(_map_fd_to_serv.size() + 4))
        get_new_client(epoll_fd, server_fd);

    else
        get_message_from_client(static_cast<Client *>(events.data.ptr));
}

void Webserv::webserv_listen(int epoll_fd)
{
    int nfds;
    struct epoll_event events[MAX_EVENTS];

    std::cout << "Serveur en attente..." << std::endl;

    while (1)
    {
        if ((nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1)) == -1)
            throw ExecptionErrorFunction("epoll_wait");

        if (stop_webserv)
            return;

        for (int i = 0; i < nfds; ++i)
            manage_connection(epoll_fd, events[i]);
    }
}

bool Webserv::initialisation_connection()
{
    int epoll_fd;
    bool res = false;

    struct sigaction sa;
    init_signal(sa);
    sigaction(SIGINT, &sa, NULL);

    try
    {
        if ((epoll_fd = epoll_create(1)) == -1)
            throw ExecptionErrorFunction("epoll_create");

        initialisation_socket(epoll_fd);
        webserv_listen(epoll_fd);
    }
    catch (const std::exception &e)
    {
        if (!stop_webserv)
        {
            std::cerr << e.what() << '\n';
            std::cout << "More info: " << strerror(errno) << "\n";
            res = true;
        }
    }
    close_connection(epoll_fd);
    return res;
}

void Webserv::close_connection(int epoll_fd)
{
    // Close fd client

    std::map<int, std::set<Server*> >::iterator it = _map_fd_to_serv.begin();
    for (; it != _map_fd_to_serv.end(); ++it)
        close((*it).first);
    _map_fd_to_serv.clear();

    close(epoll_fd);
}
