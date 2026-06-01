/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CorrectResponse.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/29 15:44:05 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/29 15:47:44 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CorrectResponse.hpp"

#include "HttpResponse.hpp"
#include "Request.hpp"
#include "Server.hpp"

// =====================
// == Canonical Form  ==
// =====================
CorrectResponse::CorrectResponse(HttpResponse *httpResponse, int statusCode) : AResponse(httpResponse, statusCode)
{
}

CorrectResponse::~CorrectResponse()
{
}

// =====================
// ==     Member      ==
// =====================
std::string CorrectResponse::getCorrectPage()
{
    for (size_t i = 0;; ++i)
    {
        HttpErrorPage *errorPage = getHttpResponse()->getRequest()->getServer()->getErrorPage(i);

        if (errorPage == NULL)
            return "";

        if (errorPage->code == getStatusCode())
        {
            addHeaderContent("content-type", "text/html");
            return errorPage->path_page;
        }
    }
}

void CorrectResponse::applyResponse()
{
    HttpResponse *response = getHttpResponse();

    std::string statusLine = makeStatusLine();
    makeHeader();
    std::string body = response->getRequest()->getPayload();
    addHeaderContent("Content-Length", intToString(static_cast<int>(body.size())));

    response->addResponseContent(statusLine);
    response->addResponseContent(headerToString());
    response->addResponseContent("\n\n");
    response->addResponseContent(body);
}