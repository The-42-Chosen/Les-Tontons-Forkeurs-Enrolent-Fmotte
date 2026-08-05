/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeadMethod.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 19:46:04 by fmotte            #+#    #+#             */
/*   Updated: 2026/08/05 19:29:53 by erpascua         ###   ########.fr       */
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
std::string HeadMethod::applyMethod(Location *location)
{
    GetMethod get(getHttpRequest());

    return get.applyMethod(location);
}