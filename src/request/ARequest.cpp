/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ARequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 14:35:51 by fmotte            #+#    #+#             */
/*   Updated: 2026/07/06 00:25:58 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ARequest.hpp"

#include "RequestContext.hpp"
#include "ResponseContext.hpp"

#include "execption.hpp"

// =====================
// == Canonical Form  ==
// =====================

ARequest::ARequest(): _requestContext(NULL), _responseContext(NULL)
{

}

ARequest::ARequest(const ARequest &other): 
_requestContext(other._requestContext), 
_responseContext(other._responseContext)
{
}

ARequest::~ARequest()
{
    // delete _requestContext;
    // delete _responseContext;
}

// =====================
// == Getter & Setter ==
// =====================

RequestContext *ARequest::getRequestContext() const
{
    return _requestContext;
}

void ARequest::setRequestContext(RequestContext* requestContext)
{
    if (requestContext == NULL)
        throw ExecptionErrorUninitializedVariable("*requestContext", "ARequest");

    _requestContext = requestContext;
}

ResponseContext *ARequest::getResponseContext() const
{
    return _responseContext;
}

void ARequest::setResponseContext(ResponseContext* responseContext)
{
    if (responseContext == NULL)
        throw ExecptionErrorUninitializedVariable("*responseContext", "ARequest");

    _responseContext = responseContext;
}

// =====================
// ==     Method      ==
// =====================
int ARequest::initialisationARequest()
{
    try
    {
        RequestContext *requestContext = new RequestContext(this);
        ResponseContext *responseContext = new ResponseContext(this);

        setRequestContext(requestContext);
        setResponseContext(responseContext);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}






