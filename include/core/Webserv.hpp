/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 17:09:20 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/25 10:29:26 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "struct.hpp"
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
    std::vector<Server *> _vectorServer;
    std::vector<Client *> _vectorClient;
    std::map<int, std::set<Server *> > _mapFdToServer;
    int _webserEpoll;

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
    const std::vector<Client *> &getClients(void) const;
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
