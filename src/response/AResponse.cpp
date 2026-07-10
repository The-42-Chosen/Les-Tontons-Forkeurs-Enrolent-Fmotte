/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AResponse.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 17:25:00 by fmotte            #+#    #+#             */
/*   Updated: 2026/07/06 02:23:09 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AResponse.hpp"

#include "ARequest.hpp"
#include "Client.hpp"
#include "Cookie.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "RequestContext.hpp"
#include "ResponseContext.hpp"
#include "Webserv.hpp"

#include "execption.hpp"
#include "utilsDuplicate.hpp"
#include "utilsResponse.hpp"

// =====================
// == Canonical Form  ==
// =====================
AResponse::AResponse(HttpResponse *httpResponse, int statusCode)
    : _httpResponse(NULL), _statusCode(-1), _statusMessage("")
{
    setHttpResponse(httpResponse);
    updateCodeError(statusCode);
}

AResponse::~AResponse()
{
}

// =====================
// ==     Getters     ==
// =====================
int AResponse::getStatusCode() const
{
    return _statusCode;
}

void AResponse::setStatusCode(int statusCode)
{
    _statusCode = statusCode;
}

std::string AResponse::getStatusMessage() const
{
    return _statusMessage;
}

void AResponse::setStatusMessage(const std::string &statusMessage)
{
    _statusMessage = statusMessage;
}

HeaderContent AResponse::getHeaderContent(void) const
{
    return _headerContent;
}

void AResponse::addHeaderContent(std::string key, std::string value)
{
    _headerContent[key] = value;
}

void AResponse::setHeaderContent(HeaderContent headerContent)
{
    _headerContent = headerContent;
}

void AResponse::addSetCookie(const std::string &setCookieValue)
{
    _setCookies.push_back(setCookieValue);
}

HttpResponse *AResponse::getHttpResponse() const
{
    return _httpResponse;
}

void AResponse::setHttpResponse(HttpResponse *httpResponse)
{
    if (httpResponse == NULL)
        throw ExecptionErrorUninitializedVariable("*httpResponse", "AResponse");

    _httpResponse = httpResponse;
}

// =====================
// == 	  Methods	  ==
// =====================

void AResponse::updateCodeError(int statusCode)
{
    setStatusCode(statusCode);
    setStatusMessage(httpStatusToString(statusCode));
}

std::string AResponse::makeStatusLine()
{
    std::string firstLine;
    firstLine += "HTTP/1.1";

    firstLine += " ";
    firstLine += intToString(getStatusCode());

    firstLine += " ";
    firstLine += getStatusMessage();
    firstLine += "\n";

    return firstLine;
}

void AResponse::makeHeader()
{
    addHeaderContent("date", makeHttpDate());
    handleSession();
}

void AResponse::handleSession()
{
    ARequest *arequest = getHttpResponse()->getARequest();
    Client *client = arequest->getRequestContext()->getClient();
    HttpRequest *httpRequest = arequest->getRequestContext()->getHttpRequest();

    if (client == NULL || httpRequest == NULL)
        return;

    bool isNewSession = false;
    if (!client->hasSession())
    {
        CookieMap cookies = httpRequest->getCookies();
        CookieMap::const_iterator it = cookies.find("session_id");

        if (it != cookies.end() && !it->second.empty())
            client->setSessionId(it->second);
        else
        {
            client->setSessionId(generateSessionId());
            isNewSession = true;
        }
    }

    int visits = client->getWebserv()->touchSession(client->getSessionId());

    if (isNewSession)
    {
        Cookie session("session_id", client->getSessionId());
        session.setPath("/");
        session.setHttpOnly(true);
        session.setMaxAge(3600);
        addSetCookie(session.toSetCookieValue());
    }
    addHeaderContent("X-Visit-Count", intToString(visits));

    const std::vector<std::string> &cgiCookies = arequest->getResponseContext()->getCgiSetCookies();
    for (size_t i = 0; i < cgiCookies.size(); ++i)
        addSetCookie(cgiCookies[i]);
}

std::string AResponse::makeHttpDate()
{
    time_t now = time(NULL);

    struct tm *gmt = gmtime(&now);
    char buffer[100];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt);

    return std::string(buffer);
}

std::string AResponse::headerToString()
{
    std::string header;

    HeaderContent headerContent = getHeaderContent();
    for (HeaderContent::iterator it = headerContent.begin(); it != headerContent.end(); ++it)
    {
        header += it->first;
        header += ":";
        header += it->second;
        header += "\n";
    }

    // Cookies <-> CGI
    for (size_t i = 0; i < _setCookies.size(); ++i)
    {
        header += "Set-Cookie:";
        header += _setCookies[i];
        header += "\n";
    }
    return header;
}

bool AResponse::containsHtmlTags(const std::string &body)
{
    return (body.find("<html>") != std::string::npos && body.find("</html>") != std::string::npos);
}