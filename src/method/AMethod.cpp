/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AMethod.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 12:54:14 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/13 15:48:06 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AMethod.hpp"

#include "HttpRequest.hpp"
#include "Location.hpp"

// =====================
// ==       OCF       ==
// =====================
AMethod::AMethod()
{
}

AMethod::AMethod(HttpRequest *http_request, Location *location) : _http_request(NULL), _location(NULL)
{
    setHttpRequest(http_request);
    setLocation(location);
}

AMethod::~AMethod()
{
}
AMethod::AMethod(const AMethod &other)
{
    *this = other;
}

AMethod &AMethod::operator=(const AMethod &other)
{
    _http_request = other._http_request;
    _location = other._location;
    return (*this);
}

HttpMethod AMethod::getMethod(void)
{
    return _method;
}

void AMethod::setMethod(HttpMethod method)
{
    _method = method;
}


// =====================
// ==     Getters     ==
// =====================
Location *AMethod::getLocation(void) const
{
    return _location;
}

void AMethod::setLocation(Location *location)
{
    if (location == NULL)
        throw ExecptionErrorUninitializedVariable("*location", "AMethod");
    _location = location;
}

HttpRequest *AMethod::getHttpRequest(void) const
{
    return _http_request;
}

void AMethod::setHttpRequest(HttpRequest *http_request)
{
    if (http_request == NULL)
        throw ExecptionErrorUninitializedVariable("*http_request", "AMethod");
    _http_request = http_request;
}

// =====================
// == 	  Member	  ==
// =====================
std::string AMethod::createPath()
{
    if (_location != NULL)
        return createPathWithLocation();

    return createPathWithServer();
}

std::string AMethod::createPathWithLocation()
{
    std::string path_file;
    std::string path_loc;
    std::string path_root;

    if (_location->getRoot() != "")
        path_root = _location->getRoot();
    else
        path_root = _http_request->getServer()->getRoot();

    path_loc = joinPath(path_root, _location->getName());

    if (_method == POST)
        return path_loc;

    path_file = joinPath(path_loc, returnLastElementPath(_http_request->getUri()));
     
    if (isFinishByFile(path_file))
        return path_file;

    if (_location->getIndex() != "")
        return joinPath(path_loc, _location->getIndex());

    if (_location->getAutoIndex())
        return "";

    return createPathWithServer();
}

std::string AMethod::createPathWithServer()
{
    std::string path_file;
    std::string check_path;
    std::string path_root;
    std::string index;

    path_root = _http_request->getServer()->getRoot();

    if (_method == POST)
        return path_root;
        
    path_file = joinPath(path_root, returnLastElementPath(_http_request->getUri()));

    if (isFinishByFile(path_file))
        return path_file;

    for (size_t i = 0; (index = _http_request->getServer()->getIndex(i)) != ""; ++i)
    {
        check_path = joinPath(path_root, index);
        if (access(check_path.c_str(), F_OK) != -1 && access(check_path.c_str(), R_OK) != -1)
            return (check_path);
    }

    if (_http_request->getServer()->getAutoIndex())
        return "";

    throw std::runtime_error("404 Not Found");
}

void AMethod::applyMethod(void)
{
}