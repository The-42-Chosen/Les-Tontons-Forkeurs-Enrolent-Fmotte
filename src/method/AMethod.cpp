/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AMethod.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 12:54:14 by fmotte            #+#    #+#             */
/*   Updated: 2026/06/09 14:42:55 by erpascua         ###   ########.fr       */
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
#include <vector>

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
std::string AMethod::createPath(Location *location)
{
    if (location != NULL)
        return createPathWithLocation(location);

    return createPathWithServer();
}

std::string AMethod::createPathWithLocation(Location *location)
{
    std::string pathFile;
    std::string pathLoc;
    std::string pathRoot;

    if (location->getRoot() != "")
        pathRoot = location->getRoot();
    else
        pathRoot = getHttpRequest()->getRequest()->getServer()->getRoot();

    pathLoc = joinPath(pathRoot, location->getName());

    if (_method == POST)
        return pathLoc;

    pathFile = joinPath(pathLoc, returnLastElementPath(getHttpRequest()->getHeader()->getUri()));

    if (isFinishByFile(pathFile))
        return pathFile;

    if (_method == GET)
    {
        if (location->getIndex() != "")
            return joinPath(pathLoc, location->getIndex());

        if (location->getAutoIndex())
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

    pathRoot = getHttpRequest()->getRequest()->getServer()->getRoot();

    if (_method == POST)
        return pathRoot;

    pathFile = joinPath(pathRoot, returnLastElementPath(getHttpRequest()->getHeader()->getUri()));

    if (isFinishByFile(pathFile))
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
            return "";
    }
    throw std::runtime_error("404");
}

std::string AMethod::applyCGI(std::string path, const std::string &interpreter)
{
    int mypipe[2];

    if (pipe(mypipe) == -1)
    {
        std::cerr << "Error pipe\n";
        throw std::runtime_error("500");
    }

    pid_t pid = fork();

    if (pid == -1)
    {
        std::cerr << "Error fork\n";
        close(mypipe[0]);
        close(mypipe[1]);
        throw std::runtime_error("500");
    }

    if (pid == 0)
        manage_pipe(path, mypipe, interpreter);

    close(mypipe[1]);

    char buffer[1024];
    int nb_read;
    std::string payload;

    while ((nb_read = read(mypipe[0], buffer, sizeof(buffer))) > 0)
        payload.append(buffer, nb_read);

    close(mypipe[0]);
    waitpid(pid, NULL, 0);

    return payload;
}

void AMethod::manage_pipe(std::string path, int mypipe[2], const std::string &interpreter)
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

    std::string uri = _httpRequest->getHeader()->getUri();
    std::string query = "";
    std::string::size_type qpos = uri.find('?');
    if (qpos != std::string::npos)
        query = uri.substr(qpos + 1);

    std::string method = (_httpRequest->getHeader()->getMethod() == POST) ? "POST" : "GET";
    std::string protocol = _httpRequest->getHeader()->getProtocol();
    HeaderContent hc = _httpRequest->getHeader()->getHeaderContent();

    std::string contentType = hc.count("content-type") ? hc.at("content-type") : "";
    std::string contentLength = hc.count("content-length") ? hc.at("content-length") : "";

    std::vector<std::string> envStrings = {
        "REQUEST_METHOD=" + method,    "QUERY_STRING=" + query,           "SERVER_PROTOCOL=" + protocol,
        "CONTENT_TYPE=" + contentType, "CONTENT_LENGTH=" + contentLength, "SCRIPT_NAME=" + path,
    };

    std::vector<char *> envp;
    for (auto &s : envStrings)
        envp.push_back(const_cast<char *>(s.c_str()));
    envp.push_back(nullptr);

    char *args[3];
    if (interpreter.empty())
    {
        args[0] = const_cast<char *>(path.c_str());
        args[1] = NULL;
        args[2] = NULL;
    }
    else
    {
        args[0] = const_cast<char *>(interpreter.c_str());
        args[1] = const_cast<char *>(path.c_str());
        args[2] = NULL;
    }

    if (execve(args[0], args, envp.data()) == -1)
        exit(EXIT_FAILURE);
}