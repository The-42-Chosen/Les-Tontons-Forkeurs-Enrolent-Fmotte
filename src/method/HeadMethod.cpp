/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeadMethod.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 19:46:04 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/14 15:27:49 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HeadMethod.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

// =====================
// ==       OCF       ==
// =====================
HeadMethod::HeadMethod(HttpRequest *http_request, Location *location)
{
    setHttpRequest(http_request);
    setLocation(location);
    setMethod(HEAD);

    applyMethod();
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
void HeadMethod::applyMethod(void)
{
    struct stat st;
    std::string path;

    path = createPath();
    std::cout << "Path to contexte read: " << path << "\n";

    checkPermisionReadFile(path);

    if (stat(path.c_str(), &st) != 0)
        throw std::runtime_error("500 Internal Server Error");

    BodyContent emptyBody;
    //HttpResponse response(200, emptyBody, "text/html");
    std::stringstream ss;
    ss << st.st_size;
    //response.addHeader("Content-Length", ss.str());
    //response.send(_request->getClient()->getClientFd());
}