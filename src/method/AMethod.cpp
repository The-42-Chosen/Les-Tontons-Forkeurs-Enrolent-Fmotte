/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AMethod.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 12:54:14 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/14 15:31:43 by erpascua         ###   ########.fr       */
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

AMethod::AMethod(HttpRequest *http_request, Location *location) : _request(NULL), _location(NULL)
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
    _request = other._request;
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
    return _request;
}

void AMethod::setHttpRequest(HttpRequest *request)
{
    if (request == NULL)
        throw ExecptionErrorUninitializedVariable("*request", "AMethod");
    _request = request;
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
    std::string pathFile;
    std::string pathLoc;
    std::string pathRoot;

    if (_location->getRoot() != "")
        pathRoot = _location->getRoot();
    else
        pathRoot = _request->getServer()->getRoot();

    pathLoc = joinPath(pathRoot, _location->getName());

    if (_method == POST)
        return pathLoc;

    pathFile = joinPath(pathLoc, returnLastElementPath(_request->getUri()));

    if (_method == POST)
        return pathLoc;

    pathFile = joinPath(pathLoc, returnLastElementPath(_request->getUri()));

    if (isFinishByFile(pathFile))
        return pathFile;

    if (_method == GET)
    {
        if (_location->getIndex() != "")
            return joinPath(pathLoc, _location->getIndex());

        if (_location->getAutoIndex())
            return "";
    }
    return createPathWithServer();
}

std::string AMethod::createPathWithServer()
{
    std::string pathFile;
    std::string checkPath;
    std::string pathRoot;
    std::string index;

    pathRoot = _request->getServer()->getRoot();

    if (_method == POST)
        return pathRoot;
    pathFile = joinPath(pathRoot, returnLastElementPath(_request->getUri()));

    if (isFinishByFile(pathFile))
        return pathFile;

    if (_method == GET)
    {
        for (size_t i = 0; (index = _request->getServer()->getIndex(i)) != ""; ++i)
        {
            checkPath = joinPath(pathRoot, index);
            if (access(checkPath.c_str(), F_OK) != -1 && access(checkPath.c_str(), R_OK) != -1)
                return (checkPath);
        }

        if (_request->getServer()->getAutoIndex())
            return "";
    }
    throw std::runtime_error("404 Not Found");
}

void AMethod::applyMethod(void)
{
}
