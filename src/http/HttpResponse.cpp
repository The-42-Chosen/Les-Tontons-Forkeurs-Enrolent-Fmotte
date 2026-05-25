/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 16:52:26 by erpascua          #+#    #+#             */
/*   Updated: 2026/05/25 11:37:12 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"

#include "HttpRequest.hpp"
#include "Request.hpp"
#include "Header.hpp"
#include "Client.hpp"

#include "execption.hpp"

// =====================
// == Canonical Form  ==
// =====================
   
HttpResponse::HttpResponse(Request *request): _responseContent(""), _request(NULL)
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

Request *HttpResponse::getRequest(void) const
{
    return _request;
}

void HttpResponse::setRequest(Request *request)
{
    if (request == NULL)
        throw ExecptionErrorUninitializedVariable("*request", "HttpResponse");

    _request = request;
}

// =====================
// == 	  Methods	  ==
// =====================
void HttpResponse::initialisationHttpResponse(std::string payload)
{   
    int statusCode = getRequest()->getStatusCode();

    if (400 <= statusCode && statusCode <= 599)
        handleError();

    else if (300 <= statusCode && statusCode <= 399)
        handleRedirection();

    else
    {
        HttpMethod method = getRequest()->getHttpRequest()->getHeader()->getMethod();
        handleCorrect(method, payload);
    }  
}

void HttpResponse::handleError()
{
    _responseContent = "Error";
}

void HttpResponse::handleRedirection()
{
    _responseContent = "Redirection";
}

void HttpResponse::handleCorrect(HttpMethod method, std::string payload)
{
    (void) method;
    (void) payload;
    _responseContent = "Correct";
}

void HttpResponse::sendToClient()
{
    int clientFd = getRequest()->getClient()->getClientFd();
    send(clientFd, _responseContent.c_str(), _responseContent.size(), 0);
}
