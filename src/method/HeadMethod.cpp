/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeadMethod.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 19:46:04 by fmotte            #+#    #+#             */
/*   Updated: 2026/06/14 15:23:02 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HeadMethod.hpp"

#include "HttpRequest.hpp"

#include "utilsRequest.hpp"

#include <ctime>

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
    struct stat st;
    std::string path;
    bool isAutoIndex = false;
    
    path = createPath(location, isAutoIndex);
    std::cout << "Path to contexte read: " << path << "\n";

    checkPermisionReadFile(path);

    if (stat(path.c_str(), &st) != 0)
        throw std::runtime_error("500");

    std::cout << "Taille : " << st.st_size << "\n";
    char buffer[100];
    std::tm *timeinfo = std::localtime(&st.st_mtime);
    std::strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", timeinfo);
    std::cout << "Derniere modification : " << buffer << "\n";

    return buffer;
}