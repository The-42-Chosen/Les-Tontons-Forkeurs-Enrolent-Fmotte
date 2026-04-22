/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 13:15:18 by erpascua          #+#    #+#             */
/*   Updated: 2026/04/22 11:20:23 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

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
        parseHttpRequest(client->getRequest());
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
        throw ExecptionErrorUninitializedVariable("*client", "HttpRequest");
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

void HttpRequest::setServer(Server *server)
{
    if (server == NULL)
        throw ExecptionErrorUninitializedVariable("*server", "HttpRequest");
    _server = server;
}
Server *HttpRequest::getServer(void) const
{
    return _server;
}

void HttpRequest::setLocation(Location *location)
{
    if (location == NULL)
        throw ExecptionErrorUninitializedVariable("*location", "HttpRequest");
    _location = location;
}

Location *HttpRequest::getLocation(void) const
{
    return _location;
}

void HttpRequest::setRoot(std::string root)
{
    if (root == "")
        throw ExecptionErrorUninitializedVariable("root", "HttpRequest");
    _path_root = root;
}

std::string HttpRequest::getRoot(void) const
{
    return _path_root;
}

// =====================
// == 	Validations   ==
// =====================

void HttpRequest::isValidURI(void)
{
    if (_uri[0] != '/')
        throw std::runtime_error("400 Bad Request");

    if (_uri.size() > 8192)
        throw std::runtime_error("414 URI Too Long");
}

void HttpRequest::isValidProtocol(void)
{
    if (_protocol != "HTTP/1.1")
        throw std::runtime_error("505 HTTP Version Not Supported");
}

void HttpRequest::isHostPresentAndValid(void)
{
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); it++)
    {
        if (it->first == "Host")
        {
            if (!it->second.empty())
                return;
            else
                throw std::runtime_error("400 Bad Request");
        }
    }
    throw std::runtime_error("400 Bad Request");
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

void HttpRequest::checkAllowedMethods(void)
{
    std::set<method_http> set_allowed_methods = getLocation()->get_methode_http();

    if (set_allowed_methods.size() == 0)
        return;

    std::set<method_http>::iterator it = set_allowed_methods.begin();
    for (; it != set_allowed_methods.end(); ++it)
    {
        if (*it == _method)
            return;
    }
    throw std::runtime_error("405 Method Not Allowed");
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

    isValidURI();
    isValidProtocol();
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
        isHostPresentAndValid();
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
    Location *location;
    std::string path_root;

    std::cout << "Method : |" << this->getMethod() << "| - Uri |" << this->getUri() << "| - Protocol |"
              << this->getProtocol() << "|" << std::endl;

    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); it++)
        std::cout << it->first << " | " << it->second << std::endl;

    link_to_server();
    std::cout << "\nServer select: " << _client->getServerPtr()->get_name_server(0) << "\n";

    if (_client->getServerPtr()->get_return()->code != 0)
    {
        std::cout << "Server close\n";
        return; // Server Close
    }

    // Init varible for execution

    // Implemente function
    location = findLocation();
    if (location == NULL)
        std::cout << "No location look to index\n";

    else
    {
        setLocation(location);
        std::cout << "Location: " << location->get_name() << "\n";

        if (location->get_return()->code != 0)
        {
            std::cout << "Server close\n";
            return; // Server Close
        }
        checkAllowedMethods();
    }

    init_root();
    std::cout << "root: " << getRoot() << "\n";
}

void HttpRequest::link_to_server(void)
{
    int fd_server = _client->getServerFd();
    std::set<Server *>::iterator it;
    std::set<Server *> set_server = _client->getWebserv()->get_map().find(fd_server)->second;

    for (it = set_server.begin(); it != set_server.end(); ++it)
    {
        for (size_t i = 0;; ++i)
        {
            if ((*it)->get_name_server(i) == "")
                break;

            if ((*it)->get_name_server(i) == _headers.find("Host")->second)
            {
                setServer(*it);
                _client->setServerPtr(*it);
                return;
            }
        }
    }
    setServer(*(set_server.begin()));
    _client->setServerPtr(*(set_server.begin()));
}

Location *HttpRequest::findLocation(void)
{
    Location *location;
    Location *best_location = NULL;
    int best_score = 100000;
    int score;
    int i = 0;

    while ((location = _client->getServerPtr()->get_location(i)) != NULL)
    {
        score = longestPrefixMatch(_uri, location->get_name());

        if (score < best_score)
        {
            best_location = location;
            best_score = score;
        }
        ++i;
    }
    return best_location;
}

void HttpRequest::init_root(void)
{
    if (getLocation()->getRoot() != "")
        setRoot(getLocation()->getRoot());
    else
        setRoot(getServer()->getRoot());
}