/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 17:09:20 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/20 16:18:03 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"
#include "HttpRequest.hpp"
#include "Server.hpp"
#include "execption.hpp"
#include "struct.hpp"
#include "utils_connection.hpp"
#include <cstring>
#include <errno.h>
#include <map>
#include <set>

#define MAX_CLIENT 10
#define MAX_EVENTS 10
#define SIZE_BUFFER 1024

class Server;
class Client;

class Webserv
{
  private:
    std::vector<Server *> _vector_server;
    std::map<int, std::set<Server *> > _map_fd_to_serv;

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
    const std::vector<Server *> &get_server(void) const;
    const std::map<int, std::set<Server *> > &get_map(void) const;

    // =====================
    // ==     Method      ==
    // =====================

    bool initialisation_webserv(std::vector<std::string> &tokens);
    bool splitServers(std::vector<std::string> &tokens);

    bool initialisation_connection();
    void initialisation_socket(int epoll_fd);
    void webserv_listen(int epoll_fd);
    void manage_connection(int epoll_fd, struct epoll_event &events);
    void had_new_client(int epoll_fd, int server_fd);
    void received_message_from_client(Client *client);
    void close_connection(int epoll_fd);
};
