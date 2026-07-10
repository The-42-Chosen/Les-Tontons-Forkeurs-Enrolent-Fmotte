/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 16:52:26 by erpascua          #+#    #+#             */
/*   Updated: 2026/07/08 21:28:15 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"

#include "ARequest.hpp"
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
#include "RequestContext.hpp"
#include "ResponseContext.hpp"

#include "execption.hpp"

// =====================
// == Canonical Form  ==
// =====================

HttpResponse::HttpResponse(ARequest *arequest) : _responseContent(""), _arequest(NULL)
{
    setARequest(arequest);
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

ARequest *HttpResponse::getARequest(void) const
{
    return _arequest;
}

void HttpResponse::setARequest(ARequest *arequest)
{
    if (arequest == NULL)
        throw ExecptionErrorUninitializedVariable("*arequest", "HttpResponse");

    _arequest = arequest;
}

// =====================
// == 	  Methods	  ==
// =====================
void HttpResponse::initialisationHttpResponse()
{
    int statusCode = getARequest()->getResponseContext()->getStatusCode();

    ErrorResponse error(this, statusCode);
    RedirResponse redir(this, statusCode);
    CorrectResponse correct(this, statusCode);

    AResponse *response = selectResponse(statusCode, error, redir, correct);

    response->applyResponse();

    // std::cout << "\n\nRESPONSE\n";
    // std::cout << getResponseContent() << "\n";
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
    int clientFd = getARequest()->getRequestContext()->getClient()->getClientFd();
    send(clientFd, _responseContent.c_str(), _responseContent.size(), 0);
}
