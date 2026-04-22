/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 14:43:11 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/21 14:24:55 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Server.hpp"
#include "struct.hpp"

class Server;
class Webserv;

class Client
{
  private:
    int _client_fd;
    int _server_fd;
    Server *_server;
    Webserv *_webserv;
    std::string _request;

  public:
    // =====================
    // == Canonical Form  ==
    // =====================
    Client();
    ~Client();
    Client(const Client &other);
    Client &operator=(const Client &other);

    // =====================
    // == Getter & Setter ==
    // =====================

    // CLIENT-FD
    int getClientFd(void);
    void setClientFd(int client_fd);

    // SERVER-FD
    int getServerFd(void);
    void setServerFd(int server_fd);

    // SERVER-PTR
    Server *getServerPtr(void);
    void setServerPtr(Server *server);

    // REQUEST
    std::string &getRequest(void);
    void clearRequest(void);
    void appendRequest(std::string &request);

    // WEBSERV
    Webserv *getWebserv(void);
    void setWebserv(Webserv *webserv);
};
