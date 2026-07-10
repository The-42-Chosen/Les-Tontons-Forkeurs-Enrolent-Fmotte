/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 00:33:34 by fmotte            #+#    #+#             */
/*   Updated: 2026/07/06 04:29:00 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "StaticRequest.hpp"

#include "AMethod.hpp"
#include "DeleteMethod.hpp"
#include "GetMethod.hpp"
#include "HeadMethod.hpp"
#include "Header.hpp"
#include "HttpRequest.hpp"
#include "PostMethod.hpp"
#include "RequestContext.hpp"
#include "ResponseContext.hpp"

#include "execption.hpp"
// =====================
// ==       OCF       ==
// =====================
StaticRequest::StaticRequest(ARequest arequest) : ARequest(arequest)
{
}

StaticRequest::~StaticRequest()
{
}

// =====================
// ==     Method      ==
// =====================
void StaticRequest::selectMethodHttp()
{
    HttpRequest *httpRequest = getRequestContext()->getHttpRequest();
    HttpMethod httpMethod = httpRequest->getHeader()->getMethod();
    ;
    AMethod *method = NULL;

    GetMethod get(httpRequest);
    DeleteMethod del(httpRequest);
    PostMethod post(httpRequest);
    HeadMethod head(httpRequest);

    if (httpMethod == GET)
        method = &get;
    else if (httpMethod == DELETE)
        method = &del;
    else if (httpMethod == POST)
        method = &post;
    else if (httpMethod == HEAD)
        method = &head;

    std::string payload = method->applyMethod(getRequestContext()->getLocation());
    getResponseContext()->setPayload(payload);
}