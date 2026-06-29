/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 13:15:18 by erpascua          #+#    #+#             */
/*   Updated: 2026/06/29 02:19:32 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include "Body.hpp"
#include "Client.hpp"
#include "Cookie.hpp"
#include "Header.hpp"
#include "Request.hpp"

#include "DeleteMethod.hpp"
#include "GetMethod.hpp"
#include "HeadMethod.hpp"
#include "PostMethod.hpp"

// =====================
// == Canonical Form  ==
// =====================

HttpRequest::HttpRequest(HandleRequest *request) : _header(NULL), _body(NULL)
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

HandleRequest *HttpRequest::getRequest() const
{
    return _request;
}

void HttpRequest::setRequest(HandleRequest *request)
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
void HttpRequest::initHeader(const std::string &headerContent)
{
    Header *header = new Header();
    setHeader(header);
    header->initialisationHeader(headerContent);
}

void HttpRequest::initBody()
{
    Body *body = new Body(*this);
    setBody(body);
    body->initialisationBody();
}

std::string HttpRequest::selectMethodHttp(Location *location)
{
    HttpMethod httpMethod = getHeader()->getMethod();

    AMethod *method = NULL;

    GetMethod get(this);
    DeleteMethod del(this);
    PostMethod post(this);
    HeadMethod head(this);

    if (httpMethod == GET)
        method = &get;
    else if (httpMethod == DELETE)
        method = &del;
    else if (httpMethod == POST)
        method = &post;
    else if (httpMethod == HEAD)
        method = &head;

    return method->applyMethod(location);
}

CookieMap HttpRequest::getCookies() const
{
    HeaderContent headers = getHeader()->getHeaderContent();
    HeaderContent::const_iterator it = headers.find("cookie");

    if (it == headers.end())
        return CookieMap();

    return parseCookieHeader(it->second);
}
