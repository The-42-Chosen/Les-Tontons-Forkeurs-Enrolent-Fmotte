/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/17 17:37:08 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/26 17:33:57 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Header.hpp"
#include "HttpRequest.hpp"
#include "struct.hpp"
#include "utilsDuplicate.hpp"

Header::Header() : _method(NONE), _uri(""), _protocol(""), _host("")
{
}

Header::~Header()
{
}

Header::Header(const Header &other)
{
    *this = other;
}

Header &Header::operator=(const Header &other)
{
    if (this != &other)
    {
        _method = other._method;
        _uri = other._uri;
        _protocol = other._protocol;
        _headerContent = other._headerContent;
        _host = other._host;
    }
    return (*this);
}

// =====================
// ==     Getters     ==
// =====================
HttpMethod Header::getMethod(void) const
{
    return _method;
}

void Header::setMethod(const std::string &method)
{
    _method = parseMethodToken(method);
}

std::string Header::getUri(void) const
{
    return _uri;
}

void Header::setUri(const std::string &uri)
{
    if (uri[0] != '/')
        throw std::runtime_error("400");

    if (uri.size() > 8192)
        throw std::runtime_error("414");

    _uri = uri;
}

std::string Header::getProtocol(void) const
{
    return _protocol;
}

void Header::setProtocol(const std::string &protocol)
{
    if (protocol != "HTTP/1.1")
        throw std::runtime_error("505");

    _protocol = protocol;
}

HeaderContent Header::getHeaderContent(void) const
{
    return _headerContent;
}

void Header::addHeaderContent(std::string key, std::string value)
{
    value = trimSpaces(value);
    value = toLowerString(value);

    key = toLowerString(key);

    _headerContent[key] = value;
}

void Header::setHeaderContent(HeaderContent headerContent)
{
    _headerContent = headerContent;
}

std::string Header::getHost(void) const
{
    return _host;
}

void Header::setHost(const std::string &host)
{
    _host = host;
}

// =====================
// == 	  Member	  ==
// =====================
void Header::initialisationHeader(const std::string &headerContent)
{
    parseHeaderInfo(headerContent);
    parseHeaderContent(headerContent);

    if (getHost() == "")
        throw std::runtime_error("400");
}

HttpMethod Header::parseMethodToken(const std::string &method)
{
    if (method == "GET")
        return (GET);
    if (method == "POST")
        return (POST);
    if (method == "DELETE")
        return (DELETE);
    if (method == "HEAD")
        return (HEAD);

    throw std::runtime_error("501");
}

void Header::parseHeaderInfo(const std::string &headerContent)
{
    _headerContent.clear();

    std::string::size_type lineEnd = headerContent.find("\r\n");

    const std::string requestLine = (lineEnd == std::string::npos) ? headerContent : headerContent.substr(0, lineEnd);

    std::stringstream stream(requestLine);

    std::string method;
    std::string uri;
    std::string protocol;
    std::string extra;
    if (!(stream >> method >> uri >> protocol) || (stream >> extra))
        throw std::runtime_error("400");

    setMethod(method);
    setUri(uri);
    setProtocol(protocol);
}
std::string::size_type Header::findEnd(const std::string &headerContent, const std::string &end)
{
    std::string::size_type headerEnd = headerContent.find(end);

    if (headerEnd == std::string::npos)
        return headerContent.size();

    else
        return headerEnd += 2;
}

void Header::parseHeaderContent(const std::string &headerContent)
{
    std::string requestLine;
    std::string::size_type headerEnd = findEnd(headerContent, "\r\n\r\n");
    std::string::size_type current = findEnd(headerContent, "\r\n");

    while (current < headerEnd)
    {
        std::string::size_type next = headerContent.find("\r\n", current);
        if (next == std::string::npos || next > headerEnd)
            next = headerEnd;

        requestLine = headerContent.substr(current, next - current);
        if (!requestLine.empty())
        {
            std::string::size_type colon = requestLine.find(':');
            if (colon == std::string::npos)
                throw std::runtime_error("400");

            std::string key = requestLine.substr(0, colon);
            std::string value = requestLine.substr(colon + 1);
            addHeaderContent(key, value);

            if (toLowerString(key) == "host")
                setHost(value);
        }
        current = next + 2;
    }
}
