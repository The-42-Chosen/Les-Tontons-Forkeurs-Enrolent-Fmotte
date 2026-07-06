/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostMethod.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 19:46:04 by fmotte            #+#    #+#             */
/*   Updated: 2026/07/06 06:01:53 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PostMethod.hpp"

#include "Body.hpp"
#include "HttpRequest.hpp"
#include "HandlePath.hpp"

#include "utilsParsing.hpp"

// =====================
// ==       OCF       ==
// =====================
PostMethod::PostMethod(HttpRequest *httpRequest) : AMethod(httpRequest, POST)
{
}

PostMethod::~PostMethod()
{
}

PostMethod::PostMethod(const PostMethod &other) : AMethod(other)
{
    *this = other;
}

PostMethod &PostMethod::operator=(const PostMethod &other)
{
    AMethod::operator=(other);
    return (*this);
}

// =====================
// == 	  Member	  ==
// =====================
std::string PostMethod::applyMethod(Location *location)
{
    HandlePath handlePath(getHttpRequest());
    std::string path = handlePath.createPath(location);
    std::cout << "Path to write: " << path << "\n";

    std::string filename = "PostContent";
    path = joinPath(path, filename);
    std::string str;
    std::vector<uint8_t> v = getHttpRequest()->getBody()->getBodyContent();
    str.assign(v.begin(), v.end());

    std::ofstream w(path.c_str());
    if (!w.is_open())
        throw std::runtime_error("500");

    w << str;
    w.close();

    return "";
}
