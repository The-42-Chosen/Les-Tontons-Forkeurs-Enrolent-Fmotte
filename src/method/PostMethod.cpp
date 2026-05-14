/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostMethod.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 19:46:04 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/14 15:27:49 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PostMethod.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

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

    path = createPath();
    std::cout << "Path to write: " << path << "\n";

    std::string filename = "PostContent";
    path = joinPath(path, filename);
    std::string str;
    std::vector<uint8_t> v = AMethod::getHttpRequest()->getBody();
    str.assign(v.begin(), v.end());

    std::ofstream w(path.c_str());
    if (!w.is_open())
        throw std::runtime_error("500 Internal Server Error");
    w << str;
    w.close();

    Body responseBody;
    std::string msg = "<html><body><h1>201 Created</h1><p>Resource stored.</p></body></html>";
    responseBody.assign(msg.begin(), msg.end());
    HttpResponse response(201, responseBody, "text/html");
    response.send(_request->getClient()->getClientFd());
}
