/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostMethod.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 19:46:04 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/13 15:35:53 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PostMethod.hpp"
#include "HttpRequest.hpp"

// =====================
// ==       OCF       ==
// =====================
PostMethod::PostMethod(HttpRequest *http_request, Location *location)
{
    setHttpRequest(http_request);
    setLocation(location);
    setMethod(POST);

    applyMethod();
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
void PostMethod::applyMethod(void)
{
    std::string path;
    std::string contentFile;

    path = createPath();
    std::cout << "Path to write: " << path << "\n";

    std::string filename = "PostContent";
    path = joinPath(path, filename);

    std::string str;
    std::vector<uint8_t> v = AMethod::getHttpRequest()->getBody();
    str.assign(v.begin(), v.end());

    // Writting
    std::ofstream w(path.c_str());
    if (w.is_open())
        w << str;
    w.close();
}