/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 14:35:51 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/25 16:56:07 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

#include "HttpRequest.hpp"
#include "Location.hpp"
#include "Client.hpp"
#include "Header.hpp"
#include "utilsRequest.hpp"

// =====================
// == Canonical Form  ==
// =====================

Request::Request(): _client(NULL), _httpRequest(NULL), _statusCode(200), _location(NULL)
{
}

Request::Request(const Request &other)
{
    *this = other;
}

Request &Request::operator=(const Request &other)
{
    if (this != &other)
    {
        _client = other._client;
        _httpRequest = other._httpRequest;
        _server = other._server;
    }
    return (*this);
}

Request::~Request()
{
    delete _httpRequest;
}

// =====================
// == Getter & Setter ==
// =====================

Client *Request::getClient(void) const
{
    return _client;
}

void Request::setClient(Client *client)
{
    if (client == NULL)
        throw ExecptionErrorUninitializedVariable("*client", "Request");

    _client = client;
}

Server *Request::getServer(void) const
{
    return _server;
}

void Request::setServer(Server *server)
{
    if (server == NULL)
        throw ExecptionErrorUninitializedVariable("*server", "Request");

    _server = server;
}

HttpRequest *Request::getHttpRequest(void) const
{
    return _httpRequest;
}

void Request::setHttpRequest(HttpRequest *httpRequest)
{
    if (httpRequest == NULL)
        throw ExecptionErrorUninitializedVariable("*httpRequest", "Request");

    _httpRequest = httpRequest;
}  

int Request::getStatusCode()
{
    return _statusCode;
}

void Request::setStatusCode(int statusCode)
{
    _statusCode = statusCode;
}

Location *Request::getLocation(void) const
{
    return _location;
}

void Request::setLocation(Location *location)
{
    if (location == NULL)
        throw ExecptionErrorUninitializedVariable("*location", "HttpRequest");
        
    _location = location;
}

// =====================
// ==     Method      ==
// =====================
bool Request::initialisationRequest(Client *client)
{
    try
    {   
        setClient(client);
        HttpRequest *httpRequest = new HttpRequest(this);
        setHttpRequest(httpRequest);
        
        getHttpRequest()->initHeader(getClient()->getContentRequest());
        linkToServer();
        getHttpRequest()->initBody();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';

        int num;
        std::istringstream(e.what()) >> num;
        setStatusCode(num);
        
        return false;
    }
    return true;
}

std::string Request::processRequest()
{
    std::string payload;
    
    try
    {
        Location *location = findLocation();
        validateRequest(location);
        payload = getHttpRequest()->selectMethodHttp(location);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';

        int num;
        std::istringstream(e.what()) >> num;
        setStatusCode(num);
    }
    return payload;
}

void Request::checkAllowedMethods(Location *location)
{   
    if (location == NULL)
        return;
        
    std::set<HttpMethod> set_allowed_methods = location->getAllowedMethods();

    if (set_allowed_methods.size() == 0)
        return;
        
    std::set<HttpMethod>::iterator it = set_allowed_methods.begin();
    for (; it != set_allowed_methods.end(); ++it)
    {
        if (*it == getHttpRequest()->getHeader()->getMethod())
            return;
    }
    throw std::runtime_error("405");
}

void Request::checkServerIsOpen()
{
    if (getClient()->getServerPtr()->getReturn()->code != 0)
    {
        std::cout << "Server close\n";
        throw std::runtime_error("503");
    }
}

void Request::checkLocationIsOpen(Location *location)
{
    if (location == NULL)
        return;
        
    std::cout << "Location: " << location->getName() << "\n";
    if (location->getReturn()->code != 0)
    {
        std::cout << "Server close\n";
        throw std::runtime_error("503");
    }
}

void Request::validateRequest(Location *location)
{
    checkServerIsOpen();
    checkLocationIsOpen(location);
    checkAllowedMethods(location);
}

void Request::linkToServer(void)
{
    int fd_server = getClient()->getServerFd();
    std::set<Server *>::iterator it;
    std::set<Server *> set_server = getClient()->getWebserv()->getFdToServersMap().find(fd_server)->second;

    for (it = set_server.begin(); it != set_server.end(); ++it)
    {
        for (size_t i = 0;; ++i)
        {
            if ((*it)->getNameServer(i) == "")
                break;

            std::cout << "\n";
            std::cout << "Name: " << (*it)->getNameServer(i) << "\n";
            std::cout << "HOST: " << getHttpRequest()->getHeader()->getHost() << "\n";

            if ((*it)->getNameServer(i) == getHttpRequest()->getHeader()->getHost())
            {
                setServer(*it);
                getClient()->setServerPtr(*it);
                return;
            }
        }
    }
    setServer(*(set_server.begin()));
    getClient()->setServerPtr(*(set_server.begin()));
}

Location *Request::findLocation(void)
{
    Location *location;
    Location *best_location = NULL;
    int best_score = 100000;
    int score;
  
    for (int i = 0; (location = getClient()->getServerPtr()->getLocation(i)) != NULL; ++i)
    {
        score = longestPrefixMatch(getHttpRequest()->getHeader()->getUri(), location->getName());

        if (score < best_score)
        {
            best_location = location;
            best_score = score;
        }
    }
    return best_location;
}

//Code d'Eric
//C'est pas le mien
bool isCompleteRequest(const std::string &request)
{   
    std::string::size_type headerEnd = request.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return (false);

    std::string::size_type bodyStart = headerEnd + 4;
    std::string transferEncoding = getHeaderValue(request, "transfer-encoding");
    transferEncoding = toLowerString(trimSpaces(transferEncoding));
    if (transferEncoding == "chunked")
        return (isCompleteChunkedBody(request, bodyStart));

    std::string contentLengthValue = getHeaderValue(request, "content-length");
    if (!contentLengthValue.empty())
    {
        size_t contentLength = 0;
        if (!parseDecimalLength(contentLengthValue, contentLength))
            return (true);
        return ((request.size() - bodyStart) >= contentLength);
    }
    return (true);
}

bool isCompleteChunkedBody(const std::string &request, std::string::size_type bodyStart)
{
    std::string::size_type current = bodyStart;

    while (current < request.size())
    {
        std::string::size_type lineEnd = request.find("\r\n", current);
        if (lineEnd == std::string::npos)
            return (false);

        std::string sizeToken = initSizeToken(request, current, lineEnd);

        std::stringstream ss(sizeToken);
        size_t chunkSize = 0;
        ss >> std::hex >> chunkSize;
        if (sizeToken.empty() || ss.fail() || !ss.eof())
            return (true);

        current = lineEnd + 2;
        if (chunkSize == 0)
            return(isFinalChunkComplete(request, current));

        if (current + chunkSize + 2 > request.size())
            return (false);

        if (request.substr(current + chunkSize, 2) != "\r\n")
            return (true);

        current += chunkSize + 2;
    }
    return (false);
}

bool isFinalChunkComplete(const std::string &request, std::string::size_type current)
{
    if (request.size() < current + 2)
        return (false);

    if (request.substr(current, 2) == "\r\n")
        return (true);

    std::string::size_type trailersEnd = request.find("\r\n\r\n", current);
    return (trailersEnd != std::string::npos);
}

std::string initSizeToken(const std::string &request, const std::string::size_type &current, const std::string::size_type &lineEnd)
{
    std::string sizeToken = request.substr(current, lineEnd - current);
    std::string::size_type semicolon = sizeToken.find(';');

    if (semicolon != std::string::npos)
        sizeToken = sizeToken.substr(0, semicolon);

    sizeToken = trimSpaces(sizeToken);
    return sizeToken;
}

bool parseDecimalLength(const std::string &value, size_t &contentLength)
{
    std::stringstream ss(value);
    size_t parsed = 0;

    if (!(ss >> parsed))
        return (false);

    if (!ss.eof())
        return (false);
        
    contentLength = parsed;
    return (true);
}

std::string getHeaderValue(const std::string &request, const std::string &headerName)
{
    std::string::size_type headerEnd = request.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return ("");

    std::string loweredHeaderName = toLowerString(headerName);
    std::string::size_type current = request.find("\r\n");
    if (current == std::string::npos)
        return ("");
    current += 2;

    while (current < headerEnd)
    {
        std::string::size_type lineEnd = request.find("\r\n", current);
        if (lineEnd == std::string::npos || lineEnd > headerEnd)
            break;
            
        std::string line = request.substr(current, lineEnd - current);
        std::string::size_type colon = line.find(':');
        if (colon != std::string::npos)
        {
            std::string key = toLowerString(trimSpaces(line.substr(0, colon)));
            if (key == loweredHeaderName)
                return (trimSpaces(line.substr(colon + 1)));
        }
        current = lineEnd + 2;
    }
    return ("");
}