/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 17:09:20 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/13 17:17:05 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Server.hpp"
#include "struct.hpp"

#define MAX_CLIENT 10
#define MAX_EVENTS 10
#define SIZE_BUFFER 1024

class Webserv
{
  private:
    std::vector<Server> _vector_server;
    std::vector<int> _vector_client_fd;
    std::vector<int> _vector_server_fd;
    
  public:
    // =====================
    // == Canonical Form  ==
    // =====================

    Webserv();
    ~Webserv();
    Webserv(const Webserv &other);
    Webserv &operator=(const Webserv &other);

    // =====================
    // == Getter & Setter ==
    // =====================

    // SERVERS
    Server *get_server(size_t i);
    size_t get_servers_count(void);
    std::vector<Server> *get_all_servers(void);

    // =====================
    // ==     Method      ==
    // =====================

    bool initialisation_webserv(std::vector<std::string> &tokens);
    bool splitServers(std::vector<std::string> &tokens);

    bool initialisation_connection();
    void initialisation_socket(int epoll_fd);
    void webserv_listen(int epoll_fd);
    void manage_connection(int epoll_fd, int event_fd);
    void get_new_client(int epoll_fd, int server_fd);
    void get_message_from_client(int clientSocket, unsigned int size_buffer);
    void close_connection(int epoll_fd);
};