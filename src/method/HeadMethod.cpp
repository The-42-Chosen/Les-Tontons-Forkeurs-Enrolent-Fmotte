/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeadMethod.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 19:46:04 by fmotte            #+#    #+#             */
/*   Updated: 2026/07/06 06:01:47 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HeadMethod.hpp"

#include "GetMethod.hpp"
#include "HttpRequest.hpp"

// =====================
// ==       OCF       ==
// =====================
HeadMethod::HeadMethod(HttpRequest *httpRequest) : AMethod(httpRequest, HEAD)
{
}

HeadMethod::~HeadMethod()
{
}

HeadMethod::HeadMethod(const HeadMethod &other) : AMethod(other)
{
    *this = other;
}

HeadMethod &HeadMethod::operator=(const HeadMethod &other)
{
    AMethod::operator=(other);
    return (*this);
}

// =====================
// == 	  Member	  ==
// =====================
// HEAD resolves the resource exactly like GET; the body is stripped later
// by HttpResponse::removeBodyForHeadMethod so Content-Length stays correct
std::string HeadMethod::applyMethod(Location *location)
{
    GetMethod get(getHttpRequest());

    return get.applyMethod(location);
}