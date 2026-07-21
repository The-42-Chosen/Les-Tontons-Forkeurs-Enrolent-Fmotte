/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorResponse.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 16:30:52 by fmotte            #+#    #+#             */
/*   Updated: 2026/07/06 02:24:47 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ErrorResponse.hpp"

#include "ARequest.hpp"
#include "HttpResponse.hpp"
#include "RequestContext.hpp"
#include "ResponseContext.hpp"
#include "Server.hpp"

// =====================
// == Canonical Form  ==
// =====================
ErrorResponse::ErrorResponse(HttpResponse *httpResponse, int statusCode) : AResponse(httpResponse, statusCode)
{
}

ErrorResponse::~ErrorResponse()
{
}

// =====================
// == 	  Methods	  ==
// =====================

std::string ErrorResponse::builtErrorPage()
{
    std::string errorMessage;

    errorMessage += "<html><body><h1>";

    errorMessage += intToString(getStatusCode());
    errorMessage += " ";
    errorMessage += getStatusMessage();

    errorMessage += "</h1></body></html>";

    addHeaderContent("content-type", "text/html");
    return errorMessage;
}

std::string ErrorResponse::getRightPageError()
{
    // We had a SegFault if incomplete method without that. We need to check if server is NULL
    Server *server = getHttpResponse()->getARequest()->getRequestContext()->getServer();

    if (server == NULL)
        return "";

    for (size_t i = 0;; ++i)
    {
        HttpErrorPage *errorPage = server->getErrorPage(i);

        if (errorPage == NULL)
            return "";

        if (errorPage->code == getStatusCode())
        {
            addHeaderContent("content-type", "text/html");
            return errorPage->path_page;
        }
    }
}

std::string ErrorResponse::makeErrorPage()
{
    std::string pathPageError = getRightPageError();
    if (pathPageError != "")
    {
        std::string content_file;
        if (parseConfigFile(pathPageError.c_str(), content_file) == 0)
            return content_file;
        updateCodeError(500);
    }
    return builtErrorPage();
}

void ErrorResponse::applyResponse()
{
    HttpResponse *response = getHttpResponse();

    std::string statusLine = makeStatusLine();
    makeHeader();
    std::string body = makeErrorPage();
    addHeaderContent("Content-Length", intToString(static_cast<int>(body.size())));

    response->addResponseContent(statusLine);
    response->addResponseContent(headerToString());
    response->addResponseContent("\n");
    response->addResponseContent(body);
}