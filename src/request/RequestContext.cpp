/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestContext.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/05 21:33:57 by fmotte            #+#    #+#             */
/*   Updated: 2026/07/22 15:11:06 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestContext.hpp"

#include "ARequest.hpp"
#include "Client.hpp"
#include "Header.hpp"
#include "HttpRequest.hpp"
#include "ResponseContext.hpp"

#include "execption.hpp"
#include "utilsRequest.hpp"

// =====================
// == Canonical Form  ==
// =====================

RequestContext::RequestContext(ARequest *arequest)
    : _client(NULL), _server(NULL), _location(NULL), _ARequest(NULL), _httpRequest(NULL)
{
    setARequest(arequest);
}

RequestContext::~RequestContext()
{
    delete getHttpRequest();
}

RequestContext::RequestContext(const RequestContext &other)
: _client(other._client), 
_server(other._server),
_location(other._location),
_ARequest(other._ARequest),
_httpRequest(new HttpRequest(*other._httpRequest))
{
    _httpRequest->setRequestContext(this);
}

// =====================
// == Getter & Setter ==
// =====================

Client *RequestContext::getClient(void) const
{
    return _client;
}

void RequestContext::setClient(Client *client)
{
    if (client == NULL)
        throw ExecptionErrorUninitializedVariable("*client", "RequestContext");

    _client = client;
}

Server *RequestContext::getServer(void) const
{
    return _server;
}

void RequestContext::setServer(Server *server)
{
    if (server == NULL)
        throw ExecptionErrorUninitializedVariable("*server", "RequestContext");

    _server = server;
}

Location *RequestContext::getLocation(void) const
{
    return _location;
}

void RequestContext::setLocation(Location *location)
{
    if (location == NULL)
        throw ExecptionErrorUninitializedVariable("*location", "RequestContext");

    _location = location;
}

ARequest *RequestContext::getARequest(void) const
{
    return _ARequest;
}

void RequestContext::setARequest(ARequest *arequest)
{
    if (arequest == NULL)
        throw ExecptionErrorUninitializedVariable("*arequest", "RequestContext");

    _ARequest = arequest;
}

HttpRequest *RequestContext::getHttpRequest(void) const
{
    return _httpRequest;
}

void RequestContext::setHttpRequest(HttpRequest *httpRequest)
{
    if (httpRequest == NULL)
        throw ExecptionErrorUninitializedVariable("*httpRequest", "RequestContext");

    _httpRequest = httpRequest;
}

// =====================
// ==     Method      ==
// =====================
void RequestContext::initialisationRequestContext()
{
    HttpRequest *httpRequest = new HttpRequest(this); // check fail
    setHttpRequest(httpRequest);

    getHttpRequest()->initHeader(getClient()->getContentRequest());
    linkToServer();
    setLocation(findLocation());
    getHttpRequest()->initBody();
}

void RequestContext::linkToServer(void)
{
    int fd_server = getClient()->getServerFd();
    std::set<Server *>::iterator it;
    std::set<Server *> set_server = getClient()->getWebserv()->getFdToServersMap().find(fd_server)->second;

    for (it = set_server.begin(); it != set_server.end(); ++it)
    {
        for (size_t i = 0;; ++i)
        {
            if ((*it)->getNameServer(i) == "")
                break;

            // std::cout << "\n";
            // std::cout << "Name: " << (*it)->getNameServer(i) << "\n";
            // std::cout << "HOST: " << getHttpRequest()->getHeader()->getHost() << "\n";

            if ((*it)->getNameServer(i) == getHttpRequest()->getHeader()->getHost())
            {
                setServer(*it);
                getClient()->setServerPtr(*it);
                return;
            }
        }
    }
    setServer(*(set_server.begin()));
    getClient()->setServerPtr(*(set_server.begin()));
}

Location *RequestContext::findLocation(void)
{
    Location *location;
    Location *best_location = NULL;
    int best_score = 100000;
    int score;

    for (int i = 0; (location = getClient()->getServerPtr()->getLocation(i)) != NULL; ++i)
    {
        score = longestPrefixMatch(getHttpRequest()->getHeader()->getScriptName(), location->getName());

        if (score < best_score)
        {
            best_location = location;
            best_score = score;
        }
    }
    return best_location;
}