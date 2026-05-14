/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetMethod.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 14:13:06 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/14 19:31:50 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GetMethod.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

// =====================
// ==       OCF       ==
// =====================
GetMethod::GetMethod(HttpRequest *http_request, Location *location)
{
    setHttpRequest(http_request);
    setLocation(location);
    setMethod(GET);
    applyMethod();
}

GetMethod::~GetMethod()
{
}

GetMethod::GetMethod(const GetMethod &other) : AMethod(other)
{
    *this = other;
}

GetMethod &GetMethod::operator=(const GetMethod &other)
{
    AMethod::operator=(other);
    return (*this);
}

// =====================
// == 	  Member	  ==
// =====================
void GetMethod::applyMethod(void)
{
    std::string path;

    path = createPath();
    std::cout << "Path to read: " << path << "\n";

    checkPermisionReadFile(path);


    std::string extention = path.substr(path.find_last_of("."));

    if (extention == ".py" || extention == ".php" )
        applyCGI(path);
    else
    {
        HttpResponse response = HttpResponse::makeFile(path);
        response.send(_request->getClient()->getClientFd());     
    }
}
