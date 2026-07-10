/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 13:15:18 by erpascua          #+#    #+#             */
/*   Updated: 2026/07/06 05:50:26 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

#include "Body.hpp"
#include "Client.hpp"
#include "Cookie.hpp"
#include "Header.hpp"
#include "RequestContext.hpp"

#include "DeleteMethod.hpp"
#include "GetMethod.hpp"
#include "HeadMethod.hpp"
#include "PostMethod.hpp"

// =====================
// == Canonical Form  ==
// =====================

HttpRequest::HttpRequest(RequestContext *requestContext) : _header(NULL), _body(NULL)
{
    setRequestContext(requestContext);
}

HttpRequest::~HttpRequest()
{
    delete getHeader();
    delete getBody();
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

RequestContext *HttpRequest::getRequestContext() const
{
    return _requestContext;
}

void HttpRequest::setRequestContext(RequestContext *requestContext)
{
    if (requestContext == NULL)
        throw ExecptionErrorUninitializedVariable("*requestContext", "HttpRequest");

    _requestContext = requestContext;
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
    Header *header = new Header(); // what if fail ?
    setHeader(header);
    header->initialisationHeader(headerContent);
}

void HttpRequest::initBody()
{
    Body *body = new Body(*this); // what if fail ?
    setBody(body);
    body->initialisationBody();
}

CookieMap HttpRequest::getCookies() const
{
    HeaderContent headers = getHeader()->getHeaderContent();
    HeaderContent::const_iterator it = headers.find("cookie");

    if (it == headers.end())
        return CookieMap();

    return parseCookieHeader(it->second);
}
