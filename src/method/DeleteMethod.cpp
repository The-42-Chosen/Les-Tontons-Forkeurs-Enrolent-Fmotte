/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteMethod.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 14:13:06 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/13 15:35:33 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DeleteMethod.hpp"

// =====================
// ==       OCF       ==
// =====================
DeleteMethod::DeleteMethod(HttpRequest *http_request, Location *location)
{
    setHttpRequest(http_request);
    setLocation(location);
    setMethod(DELETE);
    
    applyMethod();
}

DeleteMethod::~DeleteMethod()
{
}

DeleteMethod::DeleteMethod(const DeleteMethod &other) : AMethod(other)
{
    *this = other;
}

DeleteMethod &DeleteMethod::operator=(const DeleteMethod &other)
{
    AMethod::operator=(other);
    return (*this);
}

// =====================
// == 	  Member	  ==
// =====================
void DeleteMethod::applyMethod(void)
{
    std::string path;
    struct stat buff;

    path = createPath();
    std::cout << "Path to file to delete " << path << "\n";

    if (access(path.c_str(), F_OK) == -1)
        throw std::runtime_error("404 Not Found");

    if (stat(path.c_str(), &buff) != 0)
        throw std::runtime_error("500 Internal Server Error");

    if (S_ISREG(buff.st_mode))
        std::remove(path.c_str());
    else
        rmdir(path.c_str());
}