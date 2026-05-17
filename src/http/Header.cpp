/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/17 17:37:08 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/17 21:02:05 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Header.hpp"
     
Header::Header(const std::string &headerContent): _method(NONE), _uri(""), _protocol(""), _host("")
{
    initialisationHeader(headerContent);
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

void Header::setUri(std::string uri)
{
     if (_uri[0] != '/')
        throw std::runtime_error("400 Bad Request");
        
    if (_uri.size() > 8192)
        throw std::runtime_error("414 URI Too Long");
        
    _uri = uri;
}

std::string Header::getProtocol(void) const
{
    return _protocol;
}
void Header::setProtocol(std::string protocol)
{
    if (_protocol != "HTTP/1.1")
        throw std::runtime_error("505 HTTP Version Not Supported");

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

void Header::setHost(std::string host)
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
        throw std::runtime_error("400 Bad Request");
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
        
    throw std::runtime_error("501 Not Implemented");
}

void Header::parseHeaderInfo(const std::string &headerContent)
{
    _headerContent.clear();

    std::string::size_type lineEnd = headerContent.find("\r\n");
    
    const std::string requestLine =
            (lineEnd == std::string::npos)
                ? headerContent
                : headerContent.substr(0, lineEnd);

    std::stringstream stream(requestLine);
    
    std::string method;
    std::string uri;
    std::string protocol;
    std::string extra;
    if (!(stream >> method >> uri >> protocol) || (stream >> extra))
        throw std::runtime_error("400 Bad Request");
    
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
                throw std::runtime_error("400 Bad Request e");

            std::string key = requestLine.substr(0, colon);
            std::string value = requestLine.substr(colon + 1);
            addHeaderContent(key, value);

            if (toLowerString(key) == "host")
                setHost(value);
        }
        current = next + 2;
    }
}

