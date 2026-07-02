/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 14:35:51 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/30 18:40:40 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

#include "Client.hpp"
#include "Header.hpp"
#include "HttpRequest.hpp"
#include "Location.hpp"
#include "utilsRequest.hpp"

// =====================
// == Canonical Form  ==
// =====================

HandleRequest::HandleRequest()
    : _client(NULL), _server(NULL), _httpRequest(NULL), _statusCode(200), _location(NULL), _payload("")
{
}

HandleRequest::HandleRequest(const HandleRequest &other)
{
    *this = other;
}

HandleRequest &HandleRequest::operator=(const HandleRequest &other)
{
    if (this != &other)
    {
        _client = other._client;
        _httpRequest = other._httpRequest;
        _server = other._server;
    }
    return (*this);
}

HandleRequest::~HandleRequest()
{
    delete _httpRequest;
}

// =====================
// == Getter & Setter ==
// =====================

Client *HandleRequest::getClient(void) const
{
    return _client;
}

void HandleRequest::setClient(Client *client)
{
    if (client == NULL)
        throw ExecptionErrorUninitializedVariable("*client", "Request");

    _client = client;
}

Server *HandleRequest::getServer(void) const
{
    return _server;
}

void HandleRequest::setServer(Server *server)
{
    if (server == NULL)
        throw ExecptionErrorUninitializedVariable("*server", "Request");

    _server = server;
}

HttpRequest *HandleRequest::getHttpRequest(void) const
{
    return _httpRequest;
}

void HandleRequest::setHttpRequest(HttpRequest *httpRequest)
{
    if (httpRequest == NULL)
        throw ExecptionErrorUninitializedVariable("*httpRequest", "Request");

    _httpRequest = httpRequest;
}

int HandleRequest::getStatusCode() const
{
    return _statusCode;
}

void HandleRequest::setStatusCode(int statusCode)
{
    _statusCode = statusCode;
}

Location *HandleRequest::getLocation(void) const
{
    return _location;
}

void HandleRequest::setLocation(Location *location)
{
    if (location == NULL)
        throw ExecptionErrorUninitializedVariable("*location", "HttpRequest");

    _location = location;
}

std::string HandleRequest::getPayload() const
{
    return _payload;
}

void HandleRequest::setPayload(std::string payload)
{
    _payload = payload;
}

const std::vector<std::string> &HandleRequest::getCgiSetCookies() const
{
    return _cgiSetCookies;
}

void HandleRequest::addCgiSetCookie(const std::string &setCookieValue)
{
    _cgiSetCookies.push_back(setCookieValue);
}

// =====================
// ==     Method      ==
// =====================
bool HandleRequest::initialisationRequest(Client *client)
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

void HandleRequest::processRequest()
{
    try
    {
        Location *location = findLocation();
        validateRequest(location);
        std::string payload = getHttpRequest()->selectMethodHttp(location);
        setPayload(payload);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';

        int num;
        std::istringstream(e.what()) >> num;
        setStatusCode(num);
    }
}

void HandleRequest::checkAllowedMethods(Location *location)
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

void HandleRequest::checkServerIsOpen()
{
    int code = getClient()->getServerPtr()->getReturn()->code;

    if (code != 0)
    {
        setPayload(getClient()->getServerPtr()->getReturn()->value);
        throw std::runtime_error(intToString(code));
    }
}

void HandleRequest::checkLocationIsOpen(Location *location)
{
    if (location == NULL)
        return;

    std::cout << "Location: " << location->getName() << "\n";

    int code = location->getReturn()->code;
    if (code != 0)
    {
        setPayload(location->getReturn()->value);
        throw std::runtime_error(intToString(code));
    }
}

void HandleRequest::validateRequest(Location *location)
{
    checkServerIsOpen();
    checkLocationIsOpen(location);
    checkAllowedMethods(location);
}

void HandleRequest::linkToServer(void)
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

Location *HandleRequest::findLocation(void)
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

// Code d'Eric
// C'est pas le mien
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
            return (isFinalChunkComplete(request, current));

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

std::string initSizeToken(const std::string &request, const std::string::size_type &current,
                          const std::string::size_type &lineEnd)
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