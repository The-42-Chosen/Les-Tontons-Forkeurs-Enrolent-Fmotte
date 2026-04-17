/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 17:09:17 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/20 16:17:37 by fmotte           ###   ########.fr       */
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
const std::vector<Server *> &Webserv::get_server(void) const
{
    return _vector_server;
}

const std::map<int, std::set<Server *>> &Webserv::get_map(void) const
{
    return _map_fd_to_serv;
}

const std::map<int, std::set<Server *> > &Webserv::get_map(void) const
{
    return _map_fd_to_serv;
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
            Server *server = new Server(this);
            _vector_server.push_back(server);
            server->initialisation_server(tokens);
            server->initialisation_check();
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
    std::vector<Server *> vector_server = get_server();

    for (size_t i = 0; i < vector_server.size(); ++i)
    {
        server = vector_server[i];

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

void Webserv::had_new_client(int epoll_fd, int server_fd)
{
    int clientSocket;

    if ((clientSocket = accept(server_fd, NULL, NULL)) == -1)
        throw ExecptionErrorFunction("accept");

    Client *client = new Client;

    add_socket_to_event(epoll_fd, clientSocket, client);
    client->set_client_fd(clientSocket);
    client->set_server_fd(server_fd);
    client->set_webserv(this);

    std::cout << "Nouveau client connecté: fd=" << client->get_client_fd() << "\n";
}

void Webserv::received_message_from_client(Client *client)
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
    HttpRequest request(client);
    client->clear_request();

    std::string reply = "Message received\n";
    send(client->get_client_fd(), reply.c_str(), reply.size(), 0);
}

void Webserv::manage_connection(int epoll_fd, struct epoll_event &events)
{
    std::string reply = "Message received\n";
    int server_fd = events.data.fd;

    if (server_fd < static_cast<int>(_map_fd_to_serv.size() + 4))
        had_new_client(epoll_fd, server_fd);

    else
        received_message_from_client(static_cast<Client *>(events.data.ptr));
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
    errno = 0;

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
            if (errno != 0)
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
    // Close fd server
    std::map<int, std::set<Server *> >::iterator it_fd = _map_fd_to_serv.begin();
    for (; it_fd != _map_fd_to_serv.end(); ++it_fd)
        close((*it_fd).first);
    _map_fd_to_serv.clear();
    
    // Free instance server
    std::vector<Server*>::iterator it_server = _vector_server.begin();
    for (; it_server != _vector_server.end(); ++it_server)
        delete (*it_server);
    _vector_server.clear();
    

    // Free instance server
    std::vector<Server *>::iterator it_server = _vector_server.begin();
    for (; it_server != _vector_server.end(); ++it_server)
        delete (*it_server);
    _vector_server.clear();

    close(epoll_fd);
}
