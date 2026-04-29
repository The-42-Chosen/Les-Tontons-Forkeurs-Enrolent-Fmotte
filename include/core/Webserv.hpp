/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 17:09:20 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/23 13:48:15 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"
#include "HttpRequest.hpp"
#include "Server.hpp"
#include "colors.hpp"
#include "execption.hpp"
#include "struct.hpp"
#include "utilsConnection.hpp"
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
    int _webser_epoll;

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
    const std::vector<Server *> &getServers(void) const;
    const std::map<int, std::set<Server *> > &getFdToServersMap(void) const;
    void setEpollFd(const int epoll);
    int getEpollFd(void);

    // =====================
    // ==     Method      ==
    // =====================

    bool initializeWebserv(std::vector<std::string> &tokens);
    bool splitIntoServers(std::vector<std::string> &tokens);

    bool initializeConnection();
    void initializeSocket();
    void listenToWebserv();
    void handleConnection(struct epoll_event &events);
    void handleNewClient(int server_fd);
    void receiveMessageFromClient(Client *client);
    void deleteClient(Client *client);
    void closeConnection();
};
