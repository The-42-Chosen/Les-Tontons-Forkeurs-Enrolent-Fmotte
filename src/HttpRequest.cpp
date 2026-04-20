/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 13:15:18 by erpascua          #+#    #+#             */
/*   Updated: 2026/04/20 16:16:17 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

#include <sstream>
#include <stdexcept>

// =====================
// == Canonical Form  ==
// =====================

HttpRequest::HttpRequest() : _keepAlive(false), _contentLength(0)
{
}

HttpRequest::HttpRequest(Client *client) : _keepAlive(false), _contentLength(0)
{
    try
    {
        setClient(client);
        parseHttpRequest(client->get_request());
        interpretation();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return;
    }
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

void HttpRequest::setClient(Client *client)
{
    if (client == NULL)
        throw ExecptionErrorUninitializedVariable("client", "HttpRequest");
    else
        _client = client;
}

const char *HttpRequest::methodToString(method_http method)
{
    switch (method)
    {
    case 0:
        return ("GET");
    case 1:
        return ("POST");
    case 2:
        return ("DELETE");
    case 3:
        return ("HEAD");
    }
    return ("UNKNOWN");
}

// =====================
// == 	Validations   ==
// =====================

bool HttpRequest::isValidURI(void)
{
    if (_uri[0] != '/')
    {
        throw std::runtime_error("400 Bad Request");
        return (false);
    }
    if (_uri.size() > 8192)
    {
        throw std::runtime_error("414 URI Too Long");
        return (false);
    }
    return (true);
}

bool HttpRequest::isValidProtocol(void)
{
    if (_protocol != "HTTP/1.1")
    {
        throw std::runtime_error("505 HTTP Version Not Supported");
        return (false);
    }
    return (true);
}

bool HttpRequest::isHostPresentAndValid(void)
{
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); it++)
    {
        if (it->first == "Host")
        {
            if (!it->second.empty())
                return (true);
            else
                throw std::runtime_error("400 Bad Request");
        }
    }
    throw std::runtime_error("400 Bad Request");
    return (false);
}

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
    throw std::runtime_error("501 Not Implemented");
}

// =====================
// ==     Method      ==
// =====================

// _method | _uri | _protocol
void HttpRequest::parseHeaderMethod(const std::string &headerContent)
{
    _headers.clear();

    std::string requestLine;
    std::string::size_type lineEnd = headerContent.find("\r\n");

    if (lineEnd == std::string::npos)
        requestLine = headerContent;
    else
        requestLine = headerContent.substr(0, lineEnd);

    std::stringstream ssMethod(requestLine);
    std::string method;
    std::string extraToken;

    if (!(ssMethod >> method >> _uri >> _protocol))
        throw std::runtime_error("400 Bad Request");
    if (ssMethod >> extraToken)
        throw std::runtime_error("400 Bad Request");

    _method = parseMethodToken(method);
    if (!isValidURI())
        throw std::runtime_error("400 Bad Request");
    if (!isValidProtocol())
        throw std::runtime_error("505 HTTP Version Not Supported");
}

// _headers
void HttpRequest::parseHeader(const std::string &headerContent)
{
    // _headers
    std::string requestLine;
    std::string::size_type headerEnd = headerContent.find("\r\n\r\n");
    std::string::size_type lineEnd = headerContent.find("\r\n");
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
                throw std::runtime_error("400 Bad Request");

            std::string key = requestLine.substr(0, colon);
            std::string value = requestLine.substr(colon + 1);
            std::string::size_type first = value.find_first_not_of(" \t");
            if (first != std::string::npos)
                value = value.substr(first);
            else
                value.clear();

            _headers[key] = value;
            // _contentLength
            if (key == "Content-Length")
            {
                std::stringstream ssLength(value);
                if (!(ssLength >> _contentLength))
                    throw std::runtime_error("400 Bad Request");
            }
            else if (key == "Connection" && value == "keep-alive")
                _keepAlive = true; // _keepAlive
        }
        current = next + 2;
        if (!isHostPresentAndValid())
            throw std::runtime_error("400 Bad Request");
    }
}

// _body
void HttpRequest::parseBody(const std::string &headerContent)
{
    std::string::size_type bodyStart = headerContent.find("\r\n\r\n");
    if (bodyStart != std::string::npos)
    {
        bodyStart += 4;
        std::string bodyContent = headerContent.substr(bodyStart);
        for (std::string::size_type i = 0; i < bodyContent.size(); ++i)
            _body.push_back(static_cast<__uint8_t>(bodyContent[i]));
    }
}

void HttpRequest::parseHttpRequest(const std::string &headerContent)
{
    parseHeaderMethod(headerContent);
    parseHeader(headerContent);
    // parseBody(headerContent);
}

void HttpRequest::interpretation(void)
{
    std::cout << "Method : |" << this->getMethod() << "| - Uri |" << this->getUri() << "| - Protocol |"
              << this->getProtocol() << "|" << std::endl;

    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); it++)
        std::cout << it->first << " | " << it->second << std::endl;

    link_to_server();
    std::cout << "Test: " << _client->get_server_ptr()->get_name_server(0) << "\n";
}

void HttpRequest::link_to_server(void)
{
    int fd_server = _client->get_server_fd();
    std::set<Server *>::iterator it;
    std::set<Server *> set_server = _client->get_webserv()->get_map().find(fd_server)->second;

    for (it = set_server.begin(); it != set_server.end(); ++it)
    {
        for (size_t i = 0;; ++i)
        {
            if ((*it)->get_name_server(i) == "")
                break;

            std::cout << "\n";
            std::cout << "Name: " << (*it)->get_name_server(i) << "\n";
            std::cout << "HOST: " << _headers.find("Host")->second << "\n";

            if ((*it)->get_name_server(i) == _headers.find("Host")->second)
            {
                std::cout << "match\n";
                _client->set_server_ptr(*it);
            }
            else
            {
                std::cout << "dump\n";
            }
        }
    }
    // if here host not find
}
