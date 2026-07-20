/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 14:43:09 by fmotte            #+#    #+#             */
/*   Updated: 2026/07/20 02:56:31 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

#include "ARequest.hpp"
#include "CGIRequest.hpp"
#include "Header.hpp"
#include "HttpRequest.hpp"
#include "RequestContext.hpp"
#include "StaticRequest.hpp"

// =====================
// == Canonical Form  ==
// =====================

Client::Client()
    : _client_fd(-1), _server_fd(-1), _server(0), _webserv(0), _ARequest(NULL), _typeResquest(STATIC),
      _contentRequest(""), _sessionId(""), _CGIProcessing(false), _pendingDelete(false)
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
    this->_contentRequest = other._contentRequest;
    this->_sessionId = other._sessionId;
    this->_CGIProcessing = other._CGIProcessing;
    this->_pendingDelete = other._pendingDelete;

    return (*this);
}

const std::string Client::_listCGI[2] = {".py", ".php"};

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
    if (server == NULL)
        throw ExecptionErrorUninitializedVariable("*server", "Client");

    _server = server;
}

// REQUEST
std::string &Client::getContentRequest(void)
{
    return _contentRequest;
}
void Client::clearContentRequest(void)
{
    _contentRequest.clear();
}
void Client::appendContentRequest(std::string &request)
{
    _contentRequest.append(request);
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

// SESSION
std::string Client::getSessionId(void)
{
    return _sessionId;
}

void Client::setSessionId(const std::string &sessionId)
{
    _sessionId = sessionId;
}

bool Client::hasSession(void)
{
    return !_sessionId.empty();
}

ARequest *Client::getARequest() const
{
    return _ARequest;
}

void Client::setARequest(ARequest *ARequest)
{
    if (ARequest == NULL)
        throw ExecptionErrorUninitializedVariable("*ARequest", "Client");

    _ARequest = ARequest;
}

TypeRequest Client::getTypeRequest() const
{
    return _typeResquest;
}

void Client::setTypeRequest(TypeRequest typeRequest)
{
    _typeResquest = typeRequest;
}

// CGI / DISCONNECT STATE
bool Client::isCGIProcessing() const
{
    return _CGIProcessing;
}

void Client::setCGIProcessing(bool CGIProcessing)
{
    _CGIProcessing = CGIProcessing;
}

bool Client::isPendingDelete() const
{
    return _pendingDelete;
}

void Client::setPendingDelete(bool pendingDelete)
{
    _pendingDelete = pendingDelete;
}

// =====================
// ==     Method      ==
// =====================
void Client::initialisationClient()
{
    ARequest *arequest = new ARequest(); // check if fail

    setARequest(arequest);
    getARequest()->initialisationARequest();
    getARequest()->getRequestContext()->setClient(this);
    getARequest()->getRequestContext()->initialisationRequestContext();
}

void Client::selectTypeRequest()
{
    std::string uri = getARequest()->getRequestContext()->getHttpRequest()->getHeader()->getScriptName();

    size_t pos = uri.find_last_of('.');
    std::string extension;
    if (pos != std::string::npos)
        extension = uri.substr(pos);

    if (extension.empty() || std::find(_listCGI, _listCGI + 2, extension) == _listCGI + 2)
    {
        setTypeRequest(STATIC);
        ARequest *arequest = getARequest();
        StaticRequest *staticRequest = new StaticRequest(*arequest); // check if fail
        delete arequest;
        setARequest(staticRequest);
    }
    else
    {
        setTypeRequest(CGI);
        ARequest *arequest = getARequest();
        CGIRequest *cgiRequest = new CGIRequest(*arequest);
        delete arequest;
        setARequest(cgiRequest);
    }
}
