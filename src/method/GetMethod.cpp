/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetMethod.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 14:13:06 by fmotte            #+#    #+#             */
/*   Updated: 2026/06/14 15:50:44 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GetMethod.hpp"

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
    std::string path;
    std::string contentFile;
    bool isAutoIndex = false;
    
    path = createPath(location, isAutoIndex);

    if (isAutoIndex)
        return createContentAutoIndex(path);
        
    std::cout << "Path to read: " << path << "\n";

    checkPermisionReadFile(path);
    std::string extention = path.substr(path.find_last_of("."));

    if (extention == ".py" || extention == ".php")
        applyCGI(path);
    else
    {
        parseConfigFile(path.c_str(), contentFile);
        std::cout << "\ncontentFile: " << contentFile << "\n";
    }

    return contentFile;
}
