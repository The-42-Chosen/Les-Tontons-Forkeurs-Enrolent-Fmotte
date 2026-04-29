/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 14:43:09 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/21 14:33:12 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

// =====================
// == Canonical Form  ==
// =====================

Client::Client() : _client_fd(-1), _server_fd(-1), _server(0), _webserv(0), _request("")
{
}

Client::~Client()
{
}

Client::Client(const Client &other)
{
    *this = other;
}

Client &Client::operator=(const Client &other)
{
    this->_client_fd = other._client_fd;
    this->_server_fd = other._server_fd;
    this->_server = other._server;
    this->_request = other._request;

    return (*this);
}

// =====================
// == Getter & Setter ==
// =====================

// CLIENT-FD
int Client::getClientFd(void)
{
    return _client_fd;
}
void Client::setClientFd(int client_fd)
{
    _client_fd = client_fd;
}

// SERVER-FD
int Client::getServerFd(void)
{
    return _server_fd;
}
void Client::setServerFd(int server_fd)
{
    _server_fd = server_fd;
}

// SERVER-PTR
Server *Client::getServerPtr(void)
{
    return _server;
}
void Client::setServerPtr(Server *server)
{
    _server = server;
}

// REQUEST
std::string &Client::getRequest(void)
{
    return _request;
}
void Client::clearRequest(void)
{
    _request.clear();
}
void Client::appendRequest(std::string &request)
{
    _request.append(request);
}

Webserv *Client::getWebserv(void)
{
    return _webserv;
}

void Client::setWebserv(Webserv *webserv)
{
    if (webserv == NULL)
        throw ExecptionErrorUninitializedVariable("*webserv", "Client");
    _webserv = webserv;
}