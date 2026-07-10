/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandlePath.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 05:37:38 by fmotte            #+#    #+#             */
/*   Updated: 2026/07/08 21:55:14 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HandlePath.hpp"

#include "Header.hpp"
#include "HttpRequest.hpp"
#include "Location.hpp"
#include "RequestContext.hpp"
#include "Server.hpp"

#include "execption.hpp"
#include "utilsParsing.hpp"
#include "utilsRequest.hpp"

#include <dirent.h>

// =====================
// ==       OCF       ==
// =====================
HandlePath::HandlePath(HttpRequest *httpRequest) : _path(""), _isAutoIndex(false), _httpRequest(NULL)
{
    setHttpRequest(httpRequest);
}

HandlePath::~HandlePath()
{
}

// =====================
// ==     Getters     ==
// =====================
std::string HandlePath::getPath() const
{
    return _path;
}

void HandlePath::setPath(std::string path)
{
    _path = path;
}

bool HandlePath::getIsAutoIndex() const
{
    return _isAutoIndex;
}

void HandlePath::setIsAutoIndex(bool isAutoIndex)
{
    _isAutoIndex = isAutoIndex;
}

HttpRequest *HandlePath::getHttpRequest() const
{
    return _httpRequest;
}

void HandlePath::setHttpRequest(HttpRequest *httpRequest)
{
    if (httpRequest == NULL)
        throw ExecptionErrorUninitializedVariable("*httpRequest", "HandlePath");

    _httpRequest = httpRequest;
}

// =====================
// == 	  Member	  ==
// =====================
std::string HandlePath::createPath(Location *location)
{
    if (location != NULL)
        return createPathWithLocation(location);

    return createPathWithServer();
}

std::string HandlePath::selectRoot(Location *location)
{
    std::string pathRoot;

    if (location != NULL && location->getRoot() != "")
        pathRoot = location->getRoot();
    else
        pathRoot = getHttpRequest()->getRequestContext()->getServer()->getRoot();

    return pathRoot;
}

std::string HandlePath::resolveRequestedFilePath(std::string initPath)
{
    std::string pathFile;

    pathFile = joinPath(initPath, returnLastElementPath(getHttpRequest()->getHeader()->getScriptName()));

    if (isFinishByFile(pathFile))
        return pathFile;
    return "";
}

std::string HandlePath::createPathWithLocation(Location *location)
{
    std::string pathFile;
    std::string pathLoc;
    std::string pathRoot;

    pathRoot = selectRoot(location);
    pathLoc = joinPath(pathRoot, location->getName());

    HttpMethod method = getHttpRequest()->getHeader()->getMethod();
    if (method == POST)
        return pathLoc;

    if ((pathFile = resolveRequestedFilePath(pathLoc)) != "")
        return pathFile;

    if (method == GET)
    {
        if (location->getIndex() != "")
            return joinPath(pathLoc, location->getIndex());

        if (location->getAutoIndex())
        {
            setIsAutoIndex(true);
            return pathLoc;
        }
    }
    return createPathWithServer();
}

std::string HandlePath::createPathWithServer()
{
    std::string pathFile;
    std::string checkPath;
    std::string pathRoot;
    std::string index;

    pathRoot = selectRoot(NULL);
    HttpMethod method = getHttpRequest()->getHeader()->getMethod();

    if (method == POST)
        return pathRoot;

    if ((pathFile = resolveRequestedFilePath(pathRoot)) != "")
        return pathFile;

    if (method == GET)
    {
        for (size_t i = 0; (index = getHttpRequest()->getRequestContext()->getServer()->getIndex(i)) != ""; ++i)
        {
            checkPath = joinPath(pathRoot, index);
            if (access(checkPath.c_str(), F_OK) != -1 && access(checkPath.c_str(), R_OK) != -1)
                return (checkPath);
        }

        if (getHttpRequest()->getRequestContext()->getServer()->getAutoIndex())
        {
            setIsAutoIndex(true);
            return pathRoot;
        }
    }
    throw std::runtime_error("404");
}

void HandlePath::listContentFolder(const std::string &path, std::string &folderContent)
{
    DIR *dir;
    struct dirent *ent;

    std::cout << "Auto index\n";
    std::cout << "Path: " << path << "\n";

    if ((dir = opendir(path.c_str())) == NULL)
        throw std::runtime_error("500");

    while ((ent = readdir(dir)) != NULL)
    {
        folderContent.append(ent->d_name);
        if (ent->d_type == DT_DIR)
            folderContent.append("/");
        folderContent.append("\n");
    }
    closedir(dir);
}

std::string HandlePath::createContentAutoIndex(const std::string &path)
{
    size_t pos = 0;
    std::string token;
    std::string payload;
    std::string delimiter = "\n";

    std::string folderContent;
    listContentFolder(path, folderContent);

    payload += "<html><head><title>Index of /files/</title></head><body><h1><ul>";

    while ((pos = folderContent.find(delimiter)) != std::string::npos)
    {
        token = folderContent.substr(0, pos);
        payload += "<li><a href=\"";
        payload += token;
        payload += "\">";
        payload += token;
        payload += "</a></li>";
        folderContent.erase(0, pos + delimiter.length());
    }

    payload += "</ul></body></html>";

    return payload;
}