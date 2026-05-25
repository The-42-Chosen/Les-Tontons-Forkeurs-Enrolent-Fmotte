/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 13:15:18 by erpascua          #+#    #+#             */
/*   Updated: 2026/05/25 11:36:50 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "HttpRequest.hpp"
#include "Request.hpp"
#include "Header.hpp"
#include "Body.hpp"

#include "DeleteMethod.hpp"
#include "GetMethod.hpp"
#include "HeadMethod.hpp"
#include "PostMethod.hpp"

// =====================
// == Canonical Form  ==
// =====================

HttpRequest::HttpRequest(Request *request): _header(NULL), _body(NULL)
{
    setRequest(request);
}

HttpRequest::~HttpRequest()
{
    delete getHeader();
    delete getBody();
}

HttpRequest::HttpRequest(const HttpRequest &other)
{
    *this = other;
}

HttpRequest &HttpRequest::operator=(const HttpRequest &other)
{
    if (this != &other)
    {
        _header = other._header;
        _body = other._body;
    }
    return (*this);
}

// =====================
// == Getter & Setter ==
// =====================
Header *HttpRequest::getHeader() const
{
    return _header;
}

void HttpRequest::setHeader(Header *header)
{
   if (header == NULL)
        throw ExecptionErrorUninitializedVariable("*header", "HttpRequest");

    _header = header; 
}

Request *HttpRequest::getRequest() const
{
    return _request;
}

void HttpRequest::setRequest(Request *request)
{
    if (request == NULL)
        throw ExecptionErrorUninitializedVariable("*request", "HttpRequest");

    _request = request; 
}

Body *HttpRequest::getBody() const
{
    return _body;
}

void HttpRequest::setBody(Body *body)
{
   if (body == NULL)
        throw ExecptionErrorUninitializedVariable("*body", "HttpRequest");

    _body = body; 
}


// =====================
// ==     Method      ==
// =====================
void HttpRequest::parseHttpRequest(const std::string &headerContent)
{
    Header *header = new Header();
    setHeader(header);
    header->initialisationHeader(headerContent);

    Body *body = new Body(*this);
    setBody(body);
    body->initialisationBody();
}

std::string HttpRequest::selectMethodHttp(Location *location)
{
    // Which method -> different behavior
    HttpMethod httpMethod = getHeader()->getMethod();

    AMethod *method = NULL;
    std::string payload;

    if (httpMethod == GET)
        method = new GetMethod(this);

    else if (httpMethod == DELETE)
        method = new DeleteMethod(this);

    else if (httpMethod == POST)
        method = new PostMethod(this);

    else if (httpMethod == HEAD)
        method = new HeadMethod(this);
    
    
    payload = method->applyMethod(location);
    delete method;
    return payload;
}