/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 16:52:26 by erpascua          #+#    #+#             */
/*   Updated: 2026/05/30 18:40:40 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"

#include "Client.hpp"
#include "CorrectResponse.hpp"
#include "ErrorResponse.hpp"
#include "Header.hpp"
#include "HttpRequest.hpp"
#include "RedirResponse.hpp"
#include "Request.hpp"

#include "execption.hpp"

// =====================
// == Canonical Form  ==
// =====================

HttpResponse::HttpResponse(HandleRequest *request) : _responseContent(""), _request(NULL)
{
    setRequest(request);
}

HttpResponse::~HttpResponse()
{
}

// =====================
// ==     Getters     ==
// =====================
std::string HttpResponse::getResponseContent()
{
    return _responseContent;
}

void HttpResponse::setResponseContent(std::string responseContent)
{
    _responseContent = responseContent;
}

void HttpResponse::addResponseContent(std::string responseContent)
{
    _responseContent += responseContent;
}

HandleRequest *HttpResponse::getRequest(void) const
{
    return _request;
}

void HttpResponse::setRequest(HandleRequest *request)
{
    if (request == NULL)
        throw ExecptionErrorUninitializedVariable("*request", "HttpResponse");

    _request = request;
}

// =====================
// == 	  Methods	  ==
// =====================
void HttpResponse::initialisationHttpResponse()
{
    int statusCode = getRequest()->getStatusCode();

    if (400 <= statusCode && statusCode < 600)
        handleError(statusCode);

    else if (300 <= statusCode && statusCode < 400)
        handleRedirection(statusCode);

    else
        handleCorrect(statusCode);
}

void HttpResponse::handleError(int statusCode)
{
    ErrorResponse response(this, statusCode);

    std::string statusLine = response.makeStatusLine();
    response.makeHeader();
    std::string body = response.makeErrorPage();
    response.addHeaderContent("Content-Length", intToString(static_cast<int>(body.size())));

    addResponseContent(statusLine);
    addResponseContent(response.headerToString());
    addResponseContent("\n\n");
    addResponseContent(body);

    std::cout << "\n\nRESPONSE\n";
    std::cout << getResponseContent() << "\n";
}

void HttpResponse::handleRedirection(int statusCode)
{
    RedirResponse response(this, statusCode);

    std::string statusLine = response.makeStatusLine();
    response.makeHeader();
    response.addHeaderContent("Location", getRequest()->getPayload());

    addResponseContent(statusLine);
    addResponseContent(response.headerToString());
    addResponseContent("\n\n");

    std::cout << "\n\nRESPONSE\n";
    std::cout << getResponseContent() << "\n";
}

void HttpResponse::handleCorrect(int statusCode)
{
    CorrectResponse response(this, statusCode);

    std::string statusLine = response.makeStatusLine();
    response.makeHeader();
    std::string body = getRequest()->getPayload();
    response.addHeaderContent("Content-Length", intToString(static_cast<int>(body.size())));

    addResponseContent(statusLine);
    addResponseContent(response.headerToString());
    addResponseContent("\n\n");
    addResponseContent(body);

    std::cout << "\n\nRESPONSE\n";
    std::cout << getResponseContent() << "\n";
}

void HttpResponse::sendToClient()
{
    int clientFd = getRequest()->getClient()->getClientFd();
    send(clientFd, _responseContent.c_str(), _responseContent.size(), 0);
}
