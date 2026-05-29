/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorResponse.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 16:30:52 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/29 15:47:22 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "ErrorResponse.hpp"

#include "HttpResponse.hpp"
#include "Request.hpp"
#include "Server.hpp"

// =====================
// == Canonical Form  ==
// =====================
ErrorResponse::ErrorResponse(HttpResponse *httpResponse, int statusCode): AResponse(httpResponse, statusCode)
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
    return  errorMessage;
}

std::string ErrorResponse::getRightPageError()
{
    for (size_t i = 0 ;; ++i)
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