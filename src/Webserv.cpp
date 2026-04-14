/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 17:09:17 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/14 18:46:23 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"


// =====================
// == Canonical Form  ==
// =====================

Webserv::Webserv() : _vector_server(0), _vector_client(0)
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
    
    //For each server
    for (size_t i = 0; i < get_servers_count(); i++)
    {
        //For each listen
        server = get_server(i);
        for (size_t j = 0; (listen = server->get_listen(j)) != NULL; ++j)
        {
            it = map_socket_fd.find(*listen);
            
            //If new fd 
            if (it == map_socket_fd.end())
            {
                serverSocket = create_server_socket(listen->ip, listen->port, MAX_CLIENT);
                add_socket_to_event(epoll_fd, serverSocket);
                
                map_socket_fd.insert(std::make_pair(*listen, serverSocket));            //Syntaxe for cpp98++
                std::set<Server*> set_server;
                _map_fd_to_serv.insert(std::make_pair(serverSocket, set_server));        //Syntaxe for cpp98++
                _map_fd_to_serv[serverSocket].insert(server);
            }
            else
            {
                serverSocket = (*it).second;
                std::set<Server*> set_server = _map_fd_to_serv[serverSocket];

                //Avoid doublons
                if (std::find(set_server.begin(), set_server.end(), server) == set_server.end())
                    _map_fd_to_serv[serverSocket].insert(server);
            }
        }
    }
}

void Webserv::get_new_client(int epoll_fd, int server_fd)
{
    int clientSocket;
    Client client;

    if ((clientSocket = accept(server_fd, NULL, NULL)) == -1)
        throw ExecptionErrorFunction("accept");

    add_socket_to_event(epoll_fd, clientSocket);
    client.set_client_fd(clientSocket);
    client.set_server_fd(server_fd);
    _vector_client.push_back(client);

    std::cout << "Nouveau client connecté: fd=" << clientSocket << "\n";
}

void Webserv::get_message_from_client(int clientSocket)
{
    int bytes;
    char buffer[SIZE_BUFFER];
    std::string s;
    
    if ((bytes = recv(clientSocket, buffer, sizeof(buffer), 0)) == -1)
        throw ExecptionErrorFunction("recv");

    buffer[bytes] = '\0';
    s = buffer;
    
    // Think to find a better way to find the correct client
    // Perhaps replace vector by a set ?
    std::vector<Client>::iterator it = _vector_client.begin();
    for (; it != _vector_client.end(); ++it)
    {
        if (it->get_client_fd() == clientSocket)
            break;
    }
    
    if (bytes == 0)
    {
        close(clientSocket);
        if (it != _vector_client.end())
            _vector_client.erase(it);
        std::cout << "Client is disconnected\n";
        return;
    }

    it->append_request(s);
        
    if (bytes == SIZE_BUFFER)
        return;
        
    std::cout << "Message from client: " << it->get_request() << "\n";
    s = "";
    it->set_request(s);

    std::string reply = "Message received\n";
    send(clientSocket, reply.c_str(), reply.size(), 0);
}

void Webserv::manage_connection(int epoll_fd, int event_fd)
{
    std::string reply = "Message received\n";

    if (event_fd < static_cast<int>(_map_fd_to_serv.size() + 4))
        get_new_client(epoll_fd, event_fd);

    else
        get_message_from_client(event_fd);
}

void Webserv::webserv_listen(int epoll_fd)
{
    int nfds;
    struct epoll_event events[MAX_EVENTS];

    _vector_client.clear();
    std::cout << "Serveur en attente..." << std::endl;

    while (1)
    {
        if ((nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1)) == -1)
            throw ExecptionErrorFunction("epoll_wait");

        if (stop_webserv)
            return;

        for (int i = 0; i < nfds; ++i)
            manage_connection(epoll_fd, events[i].data.fd);
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
    for (size_t i = 0; i < _vector_client.size(); i++)
        close(_vector_client[i].get_client_fd());
    _vector_client.clear();


    std::map<int, std::set<Server*> >::iterator it = _map_fd_to_serv.begin();
    for (; it != _map_fd_to_serv.end(); ++it)
        close((*it).first);
    _map_fd_to_serv.clear();
    
    close(epoll_fd);
}
