/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AMethod.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 12:54:14 by fmotte            #+#    #+#             */
/*   Updated: 2026/06/14 17:54:29 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AMethod.hpp"

#include "Header.hpp"
#include "HttpRequest.hpp"
#include "Location.hpp"
#include "Request.hpp"
#include "Server.hpp"

#include "execption.hpp"
#include "utilsParsing.hpp"
#include "utilsRequest.hpp"

#include <sys/wait.h>
#include <dirent.h>

// =====================
// ==       OCF       ==
// =====================
AMethod::AMethod()
{
}

AMethod::AMethod(HttpRequest *httpRequest, HttpMethod method) : _httpRequest(NULL), _method(NONE)
{
    setHttpRequest(httpRequest);
    setMethod(method);
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
    _httpRequest = other._httpRequest;
    _method = other._method;
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

void AMethod::applyCGI(std::string path)
{
    int mypipe[2];

    if (pipe(mypipe) == -1)
    {
        std::cerr << "Error pipe\n";
        return;
    }

    pid_t pid = fork();

    if (pid == -1)
    {
        std::cerr << "Error fork\n";
        return;
    }

    if (pid == 0)
        manage_pipe(path, mypipe);

    close(mypipe[1]);

    char buffer[1024];
    int nb_read;

    while ((nb_read = read(mypipe[0], buffer, sizeof(buffer))) > 0)
        write(STDOUT_FILENO, buffer, nb_read);

    close(mypipe[0]);
    waitpid(pid, NULL, 0);
}

void manage_pipe(std::string path, int mypipe[2])
{
    close(mypipe[0]);

    if (dup2(mypipe[1], STDOUT_FILENO) == -1)
    {
        std::cerr << "dup2 error\n";
        exit(EXIT_FAILURE);
    }

    if (dup2(mypipe[1], STDERR_FILENO) == -1)
    {
        std::cerr << "dup2 error\n";
        exit(EXIT_FAILURE);
    }

    close(mypipe[1]);

    char *args[] = {const_cast<char *>("/usr/bin/python3"), const_cast<char *>(path.c_str()), NULL};
    char *envp[] = {NULL};

    if (execve(args[0], args, envp) == -1)
        exit(EXIT_FAILURE);
}

void AMethod::listContentFolder(const std::string& path, std::string& folderContent)
{
    DIR *dir;
    struct dirent *ent;
    
    std::cout << "Auto index\n";
    std::cout << "Path: " << path << "\n";
    
    if ((dir = opendir (path.c_str())) == NULL)
        throw std::runtime_error("500");
    
    while ((ent = readdir (dir)) != NULL)
    {
        folderContent.append(ent->d_name);
        if (ent->d_type == DT_DIR)
            folderContent.append("/");
        folderContent.append("\n");
    } 
    closedir (dir);
}

std::string AMethod::createContentAutoIndex(const std::string& path)
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