/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AResponse.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 17:25:00 by fmotte            #+#    #+#             */
/*   Updated: 2026/06/14 15:51:49 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AResponse.hpp"

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
    return header;
}

bool AResponse::containsHtmlTags(const std::string &body)
{
    return (body.find("<html>") != std::string::npos && body.find("</html>") != std::string::npos);
}