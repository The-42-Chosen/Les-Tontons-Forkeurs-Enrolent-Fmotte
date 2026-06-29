/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AMethod.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 12:54:14 by fmotte            #+#    #+#             */
/*   Updated: 2026/06/22 11:30:06 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AMethod.hpp"

#include "Body.hpp"
#include "Header.hpp"
#include "HttpRequest.hpp"
#include "Location.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "CGI.hpp"

#include "execption.hpp"
#include "utilsParsing.hpp"
#include "utilsRequest.hpp"


#include <dirent.h>
#include <sstream>
#include <sys/wait.h>
#include <vector>

// =====================
// ==       OCF       ==
// =====================
AMethod::AMethod()
{
}

AMethod::AMethod(HttpRequest *httpRequest, HttpMethod method) : _httpRequest(NULL), _method(NONE), _isCGI(false)
{
    setHttpRequest(httpRequest);
    setMethod(method);
}

AMethod::~AMethod()
{
    delete getCGI();
}

AMethod::AMethod(const AMethod &other)
{
    *this = other;
}

AMethod &AMethod::operator=(const AMethod &other)
{
    _httpRequest = other._httpRequest;
    _method = other._method;
    return (*this);
}

// =====================
// ==     Getters     ==
// =====================

HttpRequest *AMethod::getHttpRequest(void) const
{
    return _httpRequest;
}

void AMethod::setHttpRequest(HttpRequest *httpRequest)
{
    if (httpRequest == NULL)
        throw ExecptionErrorUninitializedVariable("*httpRequest", "AMethod");

    _httpRequest = httpRequest;
}

HttpMethod AMethod::getMethod(void)
{
    return _method;
}

void AMethod::setMethod(HttpMethod method)
{
    _method = method;
}

CGI *AMethod::getCGI() const
{
    return _cgi;
}

void AMethod::setCGI(CGI *cgi)
{
    if (cgi == NULL)
        throw ExecptionErrorUninitializedVariable("*cgi", "AMethod");

    _cgi = cgi;
}

bool AMethod::getIsCGI() const
{
    return _isCGI;
}

void AMethod::setIsCGI(bool _isCGI)
{
    _isCGI = _isCGI;
}

// =====================
// == 	  Member	  ==
// =====================
std::string AMethod::createPath(Location *location, bool &isAutoIndex)
{
    if (location != NULL)
        return createPathWithLocation(location, isAutoIndex);

    return createPathWithServer(isAutoIndex);
}

std::string AMethod::selectRoot(Location *location)
{
    std::string pathRoot;

    if (location != NULL && location->getRoot() != "")
        pathRoot = location->getRoot();
    else
        pathRoot = getHttpRequest()->getRequest()->getServer()->getRoot();

    return pathRoot;
}

std::string AMethod::resolveRequestedFilePath(std::string initPath)
{
    std::string pathFile;

    pathFile = joinPath(initPath, returnLastElementPath(getHttpRequest()->getHeader()->getUri()));

    if (isFinishByFile(pathFile))
        return pathFile;
    return "";
}

std::string AMethod::createPathWithLocation(Location *location, bool &isAutoIndex)
{
    std::string pathFile;
    std::string pathLoc;
    std::string pathRoot;

    pathRoot = selectRoot(location);

    pathLoc = joinPath(pathRoot, location->getName());

    if (_method == POST)
        return pathLoc;

    if ((pathFile = resolveRequestedFilePath(pathLoc)) != "")
        return pathFile;

    if (_method == GET)
    {
        if (location->getIndex() != "")
            return joinPath(pathLoc, location->getIndex());

        if (location->getAutoIndex())
        {
            isAutoIndex = true;
            return pathLoc;
        }
    }
    return createPathWithServer(isAutoIndex);
}

std::string AMethod::createPathWithServer(bool &isAutoIndex)
{
    std::string pathFile;
    std::string checkPath;
    std::string pathRoot;
    std::string index;

    pathRoot = selectRoot(NULL);

    if (_method == POST)
        return pathRoot;

    if ((pathFile = resolveRequestedFilePath(pathRoot)) != "")
        return pathFile;

    if (_method == GET)
    {
        for (size_t i = 0; (index = getHttpRequest()->getRequest()->getServer()->getIndex(i)) != ""; ++i)
        {
            checkPath = joinPath(pathRoot, index);
            if (access(checkPath.c_str(), F_OK) != -1 && access(checkPath.c_str(), R_OK) != -1)
                return (checkPath);
        }

        if (getHttpRequest()->getRequest()->getServer()->getAutoIndex())
        {
            isAutoIndex = true;
            return pathRoot;
        }
    }
    throw std::runtime_error("404");
}


void AMethod::listContentFolder(const std::string &path, std::string &folderContent)
{
    DIR *dir;
    struct dirent *ent;

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

std::string AMethod::createContentAutoIndex(const std::string &path)
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





















void AMethod::checkForkCreate(pid_t pid, int pipe_in[2], int pipe_out[2])
{
    if (pid == -1)
    {
        std::cerr << "Error fork\n";
        close(pipe_out[0]);
        close(pipe_out[1]);
        close(pipe_in[0]);
        close(pipe_in[1]);
        throw std::runtime_error("500");
    }
}

void AMethod::sendDataToChild(int pipe_in[2])
{
    close(pipe_in[0]);
    
    if (getHttpRequest()->getBody() != NULL)
    {
        BodyContent body = getHttpRequest()->getBody()->getBodyContent();
        if (!body.empty())
            write(pipe_in[1], body.data(), body.size());
    }
    close(pipe_in[1]);
}

std::string AMethod::receivedDataFromChild(int pipe_out[2])
{
    char buffer[1024];
    int nb_read;
    std::string payload;

    while ((nb_read = read(pipe_out[0], buffer, sizeof(buffer))) > 0)
        payload.append(buffer, nb_read);

    close(pipe_out[0]);
    
    return payload;
}

std::string AMethod::processDataFromChild(const std::string &payload)
{
    std::string::size_type sep = payload.find("\r\n\r\n");
    std::string::size_type sepLen = 4;
    if (sep == std::string::npos)
    {
        sep = payload.find("\n\n");
        sepLen = 2;
    }
    if (sep != std::string::npos)
        return payload.substr(sep + sepLen);

    return payload;
}


// std::string AMethod::applyCGI(std::string path, const std::string &interpreter)
// {
    
//     pid_t pid = fork();
//     checkForkCreate(pid, pipe_in, pipe_out);
    
//     if (pid == 0)
//         manage_pipe(path, pipe_out, pipe_in, interpreter);
//     close(pipe_out[1]);
    
//     sendDataToChild(pipe_in);

//     std::string payload = receivedDataFromChild(pipe_out);
//     waitpid(pid, NULL, 0);
    
//     payload = processDataFromChild(payload);
//     return payload;
// }

std::string AMethod::initCGI(const std::string &path, const std::string &interpreter)
{
    CGI *cgi = new CGI(this);
    setCGI(cgi);
    setIsCGI(true);
    getCGI()->initializationCGI(path, interpreter);
    getCGI()->connectToEpoll();
    return "";
}