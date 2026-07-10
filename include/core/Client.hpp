/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 14:43:11 by fmotte            #+#    #+#             */
/*   Updated: 2026/07/06 02:33:54 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Server.hpp"
#include "struct.hpp"

class Server;
class Webserv;
class ARequest;

class Client
{
  private:
    // split between link and info
    int _client_fd;
    int _server_fd;
    Server *_server;
    Webserv *_webserv;
    ARequest *_ARequest;
    TypeRequest _typeResquest;
    std::string _contentRequest;
    std::string _sessionId;

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
    std::string &getContentRequest(void);
    void clearContentRequest(void);
    void appendContentRequest(std::string &request);

    // WEBSERV
    Webserv *getWebserv(void);
    void setWebserv(Webserv *webserv);

    // SESSION
    std::string getSessionId(void);
    void setSessionId(const std::string &sessionId);
    bool hasSession(void);

    ARequest *getARequest() const;
    void setARequest(ARequest *ARequest);

    TypeRequest getTypeRequest() const;
    void setTypeRequest(TypeRequest typeRequest);

    // =====================
    // ==     Method      ==
    // =====================
    void initialisationClient();
    void selectTypeRequest();
};
