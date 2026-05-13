/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetMethod.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 14:13:06 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/14 13:29:32 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GetMethod.hpp"

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
    std::string contentFile;

    path = createPath();
    std::cout << "Path to read: " << path << "\n";

    checkPermisionReadFile(path);
    parseConfigFile(path.c_str(), contentFile);

    std::cout << "\ncontentFile: " << contentFile << "\n";
}
