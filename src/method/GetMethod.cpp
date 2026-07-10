/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetMethod.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 14:13:06 by fmotte            #+#    #+#             */
/*   Updated: 2026/07/08 22:05:34 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GetMethod.hpp"

#include "HandlePath.hpp"
#include "HttpRequest.hpp"

#include "utilsParsing.hpp"
#include "utilsRequest.hpp"

// =====================
// ==       OCF       ==
// =====================
GetMethod::GetMethod(HttpRequest *httpRequest) : AMethod(httpRequest, GET)
{
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
std::string GetMethod::applyMethod(Location *location)
{
    std::string contentFile;

    HandlePath handlePath(getHttpRequest());
    std::string path = handlePath.createPath(location);

    if (handlePath.getIsAutoIndex())
        return handlePath.createContentAutoIndex(path);

    std::string::size_type qpos = path.find('?');
    if (qpos != std::string::npos)
        path = path.substr(0, qpos);

    std::cout << "Path to read: " << path << "\n";

    checkPermisionReadFile(path);
    std::string extention = path.substr(path.find_last_of("."));

    parseConfigFile(path.c_str(), contentFile);
    // std::cout << "\ncontentFile: " << contentFile << "\n";

    return contentFile;
}
