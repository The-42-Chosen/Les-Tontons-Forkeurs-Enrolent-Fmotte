/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeadMethod.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 19:46:04 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/13 19:37:06 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HeadMethod.hpp"
#include "HttpRequest.hpp"

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
    std::string contentFile;

    path = createPath();
    std::cout << "Path to contexte read: " << path << "\n";

    checkPermisionReadFile(path);

    if (stat(path.c_str(), &st) == 0)
    {
        std::cout << "Taille : " <<  st.st_size << "\n";
        
        char buffer[100];

        std::tm* timeinfo = std::localtime(&st.st_mtime);

        std::strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", timeinfo);
        std::cout << "Derniere modification : " << buffer << "\n";
    }
}