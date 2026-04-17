/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 14:43:09 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/17 17:37:14 by fmotte           ###   ########.fr       */
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
int Client::get_client_fd(void)
{
    return _client_fd;
}
void Client::set_client_fd(int client_fd)
{
    _client_fd = client_fd;
}

// SERVER-FD
int Client::get_server_fd(void)
{
    return _server_fd;
}
void Client::set_server_fd(int server_fd)
{
    _server_fd = server_fd;
}

// SERVER-PTR
Server *Client::get_server_ptr(void)
{
    return _server;
}
void Client::set_server_ptr(Server *server)
{
    _server = server;
}

// REQUEST
std::string &Client::get_request(void)
{
    return _request;
}
void Client::clear_request(void)
{
    _request.clear();
}
void Client::append_request(std::string &request)
{
    _request.append(request);
}

Webserv *Client::get_webserv(void)
{
    return _webserv;
}

void Client::set_webserv(Webserv *webserv)
{
    if (webserv == NULL)
        throw ExecptionErrorUninitializedVariable("webserv", "Client");
    _webserv = webserv;
}