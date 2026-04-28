/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 13:15:18 by erpascua          #+#    #+#             */
/*   Updated: 2026/04/28 12:43:39 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include "colors.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

// =====================
// == Canonical Form  ==
// =====================

HttpRequest::HttpRequest() : _keepAlive(false), _contentLength(0)
{
}

HttpRequest::HttpRequest(Client *client) : _client(NULL), _server(NULL), _keepAlive(false), _contentLength(0)
{
    try
    {
        setClient(client);
        parseHttpRequest(client->getRequest());
        interpretation();
        bodyInterpretation();
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

HttpMethod HttpRequest::getMethod() const
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

const char *HttpRequest::methodToString(HttpMethod method)
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
        if (it->first == "host")
        {
            if (!it->second.empty())
                return;
            else
                throw std::runtime_error("400 Bad Request");
        }
    }
    throw std::runtime_error("400 Bad Request");
}

static HttpMethod parseMethodToken(const std::string &method)
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

void HttpRequest::checkAllowedMethods(Location *location)
{
    std::set<HttpMethod> set_allowed_methods = location->getAllowedMethods();

    if (set_allowed_methods.size() == 0)
        return;

    std::set<HttpMethod>::iterator it = set_allowed_methods.begin();
    for (; it != set_allowed_methods.end(); ++it)
    {
        if (*it == _method)
            return;
    }
    throw std::runtime_error("405 Method Not Allowed");
}

void HttpRequest::checkPermisionReadFile(std::string path)
{
    if (path.find("../") != std::string::npos)
        throw std::runtime_error("403 Forbidden");

    if (access(path.c_str(), F_OK) == -1)
        throw std::runtime_error("404 Not Found");

    if (access(path.c_str(), R_OK) == -1)
        throw std::runtime_error("403 Forbidden");
}

bool HttpRequest::isFinishByFile(std::string path)
{
    struct stat buff;

    std::cout << "Check " << path << "\n";

    if (access(path.c_str(), F_OK) == -1)
        return false;

    if (stat(path.c_str(), &buff) != 0)
        return false;

    if (S_ISREG(buff.st_mode))
        return true;

    return false;
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

            _headers[toLowerString(key)] = toLowerString(value);
        }
        current = next + 2;
        isHostPresentAndValid();
    }
}

// _body
void HttpRequest::parseBody(const std::string &headerContent)
{
    _body.clear();

    std::map<std::string, std::string>::const_iterator contentLengthIt = _headers.find("content-length");
    std::map<std::string, std::string>::const_iterator transferEncodingIt = _headers.find("transfer-encoding");
    std::map<std::string, std::string>::const_iterator connectionIt = _headers.find("connection");

    if (connectionIt != _headers.end() && connectionIt->second == "keep-alive")
        _keepAlive = true;

    if (transferEncodingIt != _headers.end())
    {
        if (transferEncodingIt->second != "chunked")
            throw std::runtime_error("501 Not Implemented");

        std::cout << GREEN << "Body treatment method : " << transferEncodingIt->first << " | "
                  << transferEncodingIt->second << RESET << std::endl;
        parseChunkedBody(headerContent);
        _contentLength = _body.size();
        return;
    }

    if (contentLengthIt != _headers.end())
    {
        std::stringstream ssLength(contentLengthIt->second);
        if (!(ssLength >> _contentLength))
            throw std::runtime_error("400 Bad Request");

        std::string::size_type bodyStart = headerContent.find("\r\n\r\n");
        if (bodyStart == std::string::npos)
            throw std::runtime_error("400 Bad Request");

        bodyStart += 4;
        if (bodyStart + _contentLength > headerContent.size())
            throw std::runtime_error("400 Bad Request");

        std::cout << GREEN << "Body treatment method : " << contentLengthIt->first << " | " << contentLengthIt->second
                  << RESET << std::endl;
        appendBodyBytes(headerContent.substr(bodyStart, _contentLength));
        return;
    }

    if (headerContent.find("\r\n\r\n") != std::string::npos)
        return;
}

void HttpRequest::parseHttpRequest(const std::string &headerContent)
{
    parseHeaderMethod(headerContent);
    parseHeader(headerContent);
    parseBody(headerContent);
}

void HttpRequest::interpretation(void)
{
    std::cout << "Method : |" << this->getMethod() << "| - Uri |" << this->getUri() << "| - Protocol |"
              << this->getProtocol() << "|" << std::endl;

    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); it++)
        std::cout << it->first << " | " << it->second << std::endl;

    linkToServer();
    std::cout << "\nServer select: " << _client->getServerPtr()->getNameServer(0) << "\n";
    if (_client->getServerPtr()->getReturn()->code != 0)
    {
        std::cout << "Server close\n";
        return; // Server Close
    }
    validateRequest();
}

void HttpRequest::validateRequest(void)
{
    Location *location;
    std::string path_root;

    location = findLocation();
    if (location == NULL)
        std::cout << "No location found\n";

    else
    {
        std::cout << "Location: " << location->getName() << "\n";

        if (location->getReturn()->code != 0)
        {
            std::cout << "Server close\n";
            return; // Server Close
        }
        checkAllowedMethods(location);
    }
    // Which method -> different behavior
    if (_method == GET)
        applyGetMethod(location);
}

void HttpRequest::bodyInterpretation(void)
{
    std::cout << YELLOW << "_body content |";
    for (std::vector<__uint8_t>::const_iterator it = _body.begin(); it != _body.end(); it++)
        std::cout << *it;
    std::cout << "|" << RESET << std::endl;
}

void HttpRequest::appendBodyBytes(const std::string &data)
{
    for (std::string::size_type i = 0; i < data.size(); ++i)
        _body.push_back(static_cast<__uint8_t>(data[i]));
}

void HttpRequest::parseChunkedBody(const std::string &headerContent)
{
    std::string::size_type current = headerContent.find("\r\n\r\n");
    if (current == std::string::npos)
        return;
    current += 4;

    while (current < headerContent.size())
    {
        std::string::size_type lineEnd = headerContent.find("\r\n", current);
        if (lineEnd == std::string::npos)
            throw std::runtime_error("400 Bad Request");

        size_t chunkSize = parseChunkSize(headerContent.substr(current, lineEnd - current));
        current = lineEnd + 2;

        if (chunkSize == 0)
            return;

        if (current + chunkSize > headerContent.size())
            throw std::runtime_error("400 Bad Request");

        appendBodyBytes(headerContent.substr(current, chunkSize));
        current += chunkSize;

        if (headerContent.substr(current, 2) != "\r\n")
            throw std::runtime_error("400 Bad Request");
        current += 2;
    }
}

void HttpRequest::linkToServer(void)
{
    int fd_server = _client->getServerFd();
    std::set<Server *>::iterator it;
    std::set<Server *> set_server = _client->getWebserv()->getFdToServersMap().find(fd_server)->second;

    for (it = set_server.begin(); it != set_server.end(); ++it)
    {
        for (size_t i = 0;; ++i)
        {
            if ((*it)->getNameServer(i) == "")
                break;

            std::cout << "\n";
            std::cout << "Name: " << (*it)->getNameServer(i) << "\n";
            std::cout << "HOST: " << _headers.find("host")->second << "\n";

            if ((*it)->getNameServer(i) == _headers.find("host")->second)
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

    for (int i = 0; (location = _client->getServerPtr()->getLocation(i)) != NULL; ++i)
    {
        score = longestPrefixMatch(_uri, location->getName());

        if (score < best_score)
        {
            best_location = location;
            best_score = score;
        }
    }
    return best_location;
}

std::string HttpRequest::createPathWithLocation(Location *location)
{
    std::string path_file;
    std::string path_loc;
    std::string path_root;

    if (location->getRoot() != "")
        path_root = location->getRoot();
    else
        path_root = getServer()->getRoot();

    path_loc = joinPath(path_root, location->getName());
    path_file = joinPath(path_loc, returnLastElementPath(_uri));

    if (isFinishByFile(path_file))
        return path_file;

    if (location->getIndex() != "")
        return joinPath(path_loc, location->getIndex());

    if (location->getAutoIndex())
        return "";

    return createPathWithServer();
}

std::string HttpRequest::createPathWithServer()
{
    std::string path_file;
    std::string check_path;
    std::string path_root;
    std::string index;

    path_root = getServer()->getRoot();
    path_file = joinPath(path_root, returnLastElementPath(_uri));

    if (isFinishByFile(path_file))
        return path_file;

    for (size_t i = 0; (index = getServer()->getIndex(i)) != ""; ++i)
    {
        check_path = joinPath(path_root, index);
        if (access(check_path.c_str(), F_OK) != -1 && access(check_path.c_str(), R_OK) != -1)
            return (check_path);
    }

    if (getServer()->getAutoIndex())
        return "";

    throw std::runtime_error("404 Not Found");
}

std::string HttpRequest::createPath(Location *location)
{
    if (location != NULL)
        return createPathWithLocation(location);

    return createPathWithServer();
}

void HttpRequest::applyGetMethod(Location *location)
{
    std::string path;
    std::string contentFile;

    path = createPath(location);
    std::cout << "Path to read: " << path << "\n";

    checkPermisionReadFile(path);
    parseConfigFile(path.c_str(), contentFile);

    std::cout << "\ncontentFile: " << contentFile << "\n";
}
