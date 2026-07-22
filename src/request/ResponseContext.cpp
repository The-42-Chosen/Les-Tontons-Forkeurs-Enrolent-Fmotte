/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseContext.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/05 21:33:55 by fmotte            #+#    #+#             */
/*   Updated: 2026/07/22 13:46:01 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseContext.hpp"

#include "ARequest.hpp"

#include "execption.hpp"

// =====================
// == Canonical Form  ==
// =====================
ResponseContext::ResponseContext(ARequest *arequest)
    : _statusCode(200), _payload(""), _cgiSetCookies(0), _ARequest(NULL)
{
    setARequest(arequest);
}

ResponseContext::~ResponseContext()
{
}

ResponseContext::ResponseContext(const ResponseContext &other)
: _statusCode(other._statusCode), _payload(other._payload), _cgiSetCookies(other._cgiSetCookies), _ARequest(other._ARequest)
{
}

// =====================
// == Getter & Setter ==
// =====================

int ResponseContext::getStatusCode() const
{
    return _statusCode;
}

void ResponseContext::setStatusCode(int statusCode)
{
    _statusCode = statusCode;
}

std::string ResponseContext::getPayload() const
{
    return _payload;
}

void ResponseContext::setPayload(std::string payload)
{
    _payload = payload;
}

const std::vector<std::string> &ResponseContext::getCgiSetCookies() const
{
    return _cgiSetCookies;
}

void ResponseContext::addCgiSetCookie(const std::string &setCookieValue)
{
    _cgiSetCookies.push_back(setCookieValue);
}

ARequest *ResponseContext::getARequest(void) const
{
    return _ARequest;
}

void ResponseContext::setARequest(ARequest *arequest)
{
    if (arequest == NULL)
        throw ExecptionErrorUninitializedVariable("*arequest", "ResponseContext");

    _ARequest = arequest;
}
