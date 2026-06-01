/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 16:52:26 by erpascua          #+#    #+#             */
/*   Updated: 2026/06/01 18:50:22 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"

#include "Client.hpp"
#include "CorrectResponse.hpp"
#include "DeleteMethod.hpp"
#include "ErrorResponse.hpp"
#include "GetMethod.hpp"
#include "HeadMethod.hpp"
#include "Header.hpp"
#include "HttpRequest.hpp"
#include "PostMethod.hpp"
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

    ErrorResponse error(this, statusCode);
    RedirResponse redir(this, statusCode);
    CorrectResponse correct(this, statusCode);

    AResponse *response = selectResponse(statusCode, error, redir, correct);

    response->applyResponse();

    std::cout << "\n\nRESPONSE\n";
    std::cout << getResponseContent() << "\n";
}

AResponse *HttpResponse::selectResponse(int statusCode, ErrorResponse &error, RedirResponse &redir,
                                        CorrectResponse &correct)
{
    if (400 <= statusCode && statusCode < 600)
        return &error;
    else if (300 <= statusCode && statusCode < 400)
        return &redir;
    else
        return &correct;
}

void HttpResponse::sendToClient()
{
    int clientFd = getRequest()->getClient()->getClientFd();
    send(clientFd, _responseContent.c_str(), _responseContent.size(), 0);
}
