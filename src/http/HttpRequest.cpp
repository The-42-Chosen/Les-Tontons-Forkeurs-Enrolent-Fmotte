/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 13:15:18 by erpascua          #+#    #+#             */
/*   Updated: 2026/05/17 20:36:38 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

// =====================
// == Canonical Form  ==
// =====================

HttpRequest::HttpRequest(Client *client): _client(NULL), _header(NULL), _body(NULL), _location(NULL)
{
    initialisationHttpRequest(client);
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
        _client = other._client;
        _header = other._header;
        _body = other._body;
        _location = other._location;
    }
    return (*this);
}

// =====================
// == Getter & Setter ==
// =====================

Client *HttpRequest::getClient(void) const
{
    return _client;
}

void HttpRequest::setClient(Client *client)
{
    if (client == NULL)
        throw ExecptionErrorUninitializedVariable("*client", "HttpRequest");

    _client = client;
}

Server *HttpRequest::getServer(void) const
{
    return _server;
}

void HttpRequest::setServer(Server *server)
{
    if (server == NULL)
        throw ExecptionErrorUninitializedVariable("*server", "HttpRequest");

    _server = server;
}
    
Header *HttpRequest::getHeader(void) const
{
    return _header;
}

void HttpRequest::setHeader(Header *header)
{
    if (header == NULL)
        throw ExecptionErrorUninitializedVariable("*header", "HttpRequest");

    _header = header;
}

Body *HttpRequest::getBody(void) const
{
    return _body;
}

void HttpRequest::setBody(Body *body)
{
    if (body == NULL)
        throw ExecptionErrorUninitializedVariable("*body", "HttpRequest");

    _body = body;
}

Location *HttpRequest::getLocation(void) const
{
    return _location;
}

void HttpRequest::setLocation(Location *location)
{
    if (location == NULL)
        throw ExecptionErrorUninitializedVariable("*location", "HttpRequest");
    _location = location;
}

// =====================
// ==     Method      ==
// =====================

void HttpRequest::initialisationHttpRequest(Client *client)
{
    //Do function
    try
    {   
        setClient(client);
        parseHttpRequest(getClient()->getRequest());
        interpretation();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        std::string errorMsg = e.what();
        //handlingErrorCode(e);
        return;
    }
}

// void HttpRequest::handlingErrorCode(const std::exception &e)
// {
//     std::cerr << e.what() << '\n';
//     std::string errorMsg = e.what();
//     int errorCode = 500;
//     if (errorMsg.find("400") != std::string::npos)
//         errorCode = 400;
//     else if (errorMsg.find("404") != std::string::npos)
//         errorCode = 404;
//     else if (errorMsg.find("405") != std::string::npos)
//         errorCode = 405;
//     else if (errorMsg.find("413") != std::string::npos)
//         errorCode = 413;
//     else if (errorMsg.find("414") != std::string::npos)
//         errorCode = 414;
//     else if (errorMsg.find("501") != std::string::npos)
//         errorCode = 501;
//     else if (errorMsg.find("505") != std::string::npos)
//         errorCode = 505;

//     //HttpResponse errorResponse = HttpResponse::makeError(errorCode);
//     //errorResponse.send(getClient()->getClientFd());
// }

void HttpRequest::checkAllowedMethods(Location *location)
{
    std::set<HttpMethod> set_allowed_methods = location->getAllowedMethods();

    if (set_allowed_methods.size() == 0)
        return;
        
    std::set<HttpMethod>::iterator it = set_allowed_methods.begin();
    for (; it != set_allowed_methods.end(); ++it)
    {
        if (*it == getHeader()->getMethod())
            return;
    }
    throw std::runtime_error("405 Method Not Allowed");
}

void HttpRequest::parseHttpRequest(const std::string &headerContent)
{
    Header *header = new Header(headerContent);
    setHeader(header);

    linkToServer();
    
    Location *location = findLocation();
    if (location != NULL)
        setLocation(location);
    
    Body *body = new Body(*this);
    setBody(body);
}

std::string HttpRequest::getHeaderValue(const std::string &request, const std::string &headerName)
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

void HttpRequest::interpretation(void)
{   
    Header *header = getHeader();
    std::cout << "Method : |" << header->getMethod() << "| - Uri |" << header->getUri() << "| - Protocol |"
              << header->getProtocol() << "|" << std::endl;

    std::cout << "\nServer select: " << getClient()->getServerPtr()->getNameServer(0) << "\n";
    if (getClient()->getServerPtr()->getReturn()->code != 0)
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
    HttpMethod method = getHeader()->getMethod();
    if (method == GET)
        GetMethod method = GetMethod(this, location);

    else if (method == DELETE)
        DeleteMethod method = DeleteMethod(this, location);

    else if (method == POST)
        PostMethod method = PostMethod(this, location);

    else if (method == HEAD)
        HeadMethod method = HeadMethod(this, location);
}


void HttpRequest::linkToServer(void)
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
            std::cout << "HOST: " << getHeader()->getHeaderContent().find("host")->second << "\n";

            if ((*it)->getNameServer(i) == getHeader()->getHeaderContent().find("host")->second)
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

Location *HttpRequest::findLocation(void)
{
    Location *location;
    Location *best_location = NULL;
    int best_score = 100000;
    int score;

    for (int i = 0; (location = _client->getServerPtr()->getLocation(i)) != NULL; ++i)
    {
        score = longestPrefixMatch(getHeader()->getUri(), location->getName());

        if (score < best_score)
        {
            best_location = location;
            best_score = score;
        }
    }
    return best_location;
}


bool isCompleteHttpRequest(Client *client)
{   
    HttpRequest httpRequest(client);

    const std::string &request = client->getRequest();
    std::string::size_type headerEnd = request.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return (false);

    std::string::size_type bodyStart = headerEnd + 4;
    std::string transferEncoding = HttpRequest::getHeaderValue(request, "transfer-encoding");
    transferEncoding = toLowerString(trimSpaces(transferEncoding));
    if (transferEncoding == "chunked")
        return (isCompleteChunkedBody(request, bodyStart));

    std::string contentLengthValue = HttpRequest::getHeaderValue(request, "content-length");
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