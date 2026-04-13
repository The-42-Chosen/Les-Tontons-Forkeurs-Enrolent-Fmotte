/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 13:15:18 by erpascua          #+#    #+#             */
/*   Updated: 2026/04/13 21:10:05 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

#include <sstream>
#include <stdexcept>

// =====================
// == Canonical Form  ==
// =====================

static method_http parseMethodToken(const std::string &method)
{
    if (method == "GET")
        return (GET);
    if (method == "POST")
        return (POST);
    if (method == "DELETE")
        return (DELETE);
    if (method == "HEAD")
        return (HEAD);
    throw std::runtime_error("Unsupported HTTP method: " + method);
}

HttpRequest::HttpRequest() : _keepAlive(false), _contentLength(0)
{
}

HttpRequest::HttpRequest(std::string requestRawContent) : _keepAlive(false), _contentLength(0)
{
    parseHeader(requestRawContent);
}

HttpRequest::HttpRequest(const HttpRequest &cpy)
    : _method(cpy._method), _uri(cpy._uri), _protocol(cpy._protocol), _headers(cpy._headers), _body(cpy._body),
      _keepAlive(cpy._keepAlive), _contentLength(cpy._contentLength)
{
}

HttpRequest &HttpRequest::operator=(const HttpRequest &cpy)
{
    if (this != &cpy)
    {
        _method = cpy._method;
        _uri = cpy._uri;
        _protocol = cpy._protocol;
        _headers = cpy._headers;
        _body = cpy._body;
        _keepAlive = cpy._keepAlive;
        _contentLength = cpy._contentLength;
    }
    return (*this);
}

HttpRequest::~HttpRequest()
{
}

// =====================
// == Getter & Setter ==
// =====================

method_http HttpRequest::getMethod() const
{
    return (_method);
}

const std::string &HttpRequest::getUri() const
{
    return (_uri);
}

const std::string &HttpRequest::getProtocol() const
{
    return (_protocol);
}

const char *HttpRequest::methodToString(method_http method)
{
    if (method == GET)
        return ("GET");
    if (method == POST)
        return ("POST");
    if (method == DELETE)
        return ("DELETE");
    if (method == HEAD)
        return ("HEAD");
    return ("UNKNOWN");
}

// =====================
// ==     Method      ==
// =====================

HttpRequest &HttpRequest::parseHeaderMethod(const std::string &headerContent)
{
    _headers.clear();

    // _method | _uri | _protocol
    std::string requestLine;
    std::string::size_type headerEnd = headerContent.find("\r\n\r\n");
    std::string::size_type lineEnd = headerContent.find("\r\n");

    if (lineEnd == std::string::npos)
        requestLine = headerContent;
    else
        requestLine = headerContent.substr(0, lineEnd);

    std::stringstream ssMethod(requestLine);
    std::string method;

    if (!(ssMethod >> method >> _uri >> _protocol))
        throw std::runtime_error("Invalid HTTP request line");

    _method = parseMethodToken(method);

    // _headers
    if (headerEnd == std::string::npos)
        headerEnd = headerContent.size();
    else
        headerEnd += 2;

    std::string::size_type current = (lineEnd == std::string::npos) ? headerContent.size() : lineEnd + 2;
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
                throw std::runtime_error("Invalid HTTP header line");

            std::string key = requestLine.substr(0, colon);
            std::string value = requestLine.substr(colon + 1);
            std::string::size_type first = value.find_first_not_of(" \t");
            if (first != std::string::npos)
                value = value.substr(first);
            else
                value.clear();

            _headers[key] = value;
            if (key == "Content-Length")
            {
                std::stringstream ssLength(value);
                if (!(ssLength >> _contentLength))
                    throw std::runtime_error("Invalid Content-Length header");
            }
            else if (key == "Connection" && value == "keep-alive")
                _keepAlive = true;
        }
        current = next + 2;
    }

    // TODO: Verif parsing _headers + faire parsing _body;

    // Body
    // requestLine = headerContent.substr(current, next - current)

    // if (!requestLine.empty())
    // {
    // 	_body = requestLine.substr()
    // }

    return (*this);
}

HttpRequest &HttpRequest::parseHeader(const std::string &headerContent)
{
    return (parseHeaderMethod(headerContent));
}
