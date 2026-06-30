/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AMethod.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 12:54:14 by fmotte            #+#    #+#             */
/*   Updated: 2026/06/30 17:24:10 by fmotte           ###   ########.fr       */
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
#include "utilsConnection.hpp"

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

AMethod::AMethod(HttpRequest *httpRequest, HttpMethod method) : _httpRequest(NULL), _method(NONE)//, _isCGI(false)
{
    setHttpRequest(httpRequest);
    setMethod(method);
}

AMethod::~AMethod()
{
    //delete getCGI();
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

// CGI *AMethod::getCGI() const
// {
//     return _cgi;
// }

// void AMethod::setCGI(CGI *cgi)
// {
//     if (cgi == NULL)
//         throw ExecptionErrorUninitializedVariable("*cgi", "AMethod");

//     _cgi = cgi;
// }

// bool AMethod::getIsCGI() const
// {
//     return _isCGI;
// }

// void AMethod::setIsCGI(bool _isCGI)
// {
//     _isCGI = _isCGI;
// }

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



















void AMethod::createPipe(int pipeIn[2], int pipeOut[2])
{
    if (pipe(pipeOut) == -1 || pipe(pipeIn) == -1)
    {
        std::cerr << "Error pipe\n";
        throw std::runtime_error("500");
    }

    setNonblocking(pipeOut[1]);
    setNonblocking(pipeIn[0]);
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


std::string AMethod::applyCGI(std::string path, const std::string &interpreter)
{
    
    int pipeIn[2];
    int pipeOut[2];
    
    createPipe(pipeIn, pipeOut);
    
    pid_t pid = fork();
    checkForkCreate(pid, pipeIn, pipeOut);
    
    if (pid == 0)
        manage_pipe(path, pipeOut, pipeIn, interpreter);
    close(pipeOut[1]);
    
    sendDataToChild(pipeIn);

    std::string payload = receivedDataFromChild(pipeOut);
    waitpid(pid, NULL, 0);
    
    payload = processDataFromChild(payload);
    return payload;
}

// std::string AMethod::initCGI(const std::string &path, const std::string &interpreter)
// {
//     CGI *cgi = new CGI(this);
//     setCGI(cgi);
//     setIsCGI(true);
//     getCGI()->initializationCGI(path, interpreter);
//     getCGI()->connectToEpoll();
//     return "";
// }

void AMethod::manage_pipe(const std::string &path, int pipeIn[2], int pipeOut[2], const std::string &interpreter)
{
    close(pipeOut[0]);
    if (dup2(pipeOut[1], STDOUT_FILENO) == -1)
    {
        std::cerr << "dup2 error\n";
        exit(EXIT_FAILURE);
    }
    if (dup2(pipeOut[1], STDERR_FILENO) == -1)
    {
        std::cerr << "dup2 error\n";
        exit(EXIT_FAILURE);
    }

    close(pipeOut[1]);
    close(pipeIn[1]);
    
    if (dup2(pipeIn[0], STDIN_FILENO) == -1)
    {
        std::cerr << "dup2 error\n";
        exit(EXIT_FAILURE);
    }
    close(pipeIn[0]);

    // URI / query string / script name
    std::string uri = getHttpRequest()->getHeader()->getUri();
    std::string query = "";
    std::string scriptName = uri;
    std::string::size_type qpos = uri.find('?');
    if (qpos != std::string::npos)
    {
        query = uri.substr(qpos + 1);
        scriptName = uri.substr(0, qpos);
    }

    std::string method = (getHttpRequest()->getHeader()->getMethod() == POST) ? "POST" : "GET";
    std::string protocol = getHttpRequest()->getHeader()->getProtocol();
    
    HeaderContent hc = getHttpRequest()->getHeader()->getHeaderContent();
    std::string contentType = hc.count("content-type") ? hc.at("content-type") : "";
    std::string contentLength = hc.count("content-length") ? hc.at("content-length") : "";
    std::string cookie = hc.count("cookie") ? hc.at("cookie") : "";

    Listen *listen = getHttpRequest()->getRequest()->getServer()->getListen(0);
    std::string serverName = (listen && !listen->ip.empty()) ? listen->ip : "localhost";
    std::string serverPort = "80";
    if (listen)
    {
        std::ostringstream oss;
        oss << listen->port;
        serverPort = oss.str();
    }

    std::string::size_type pslash = path.rfind('/');
    std::string scriptFile = (pslash != std::string::npos) ? path.substr(pslash + 1) : path;

    std::vector<std::string> envStrings;
    envStrings.push_back("REQUEST_METHOD=" + method);
    envStrings.push_back("QUERY_STRING=" + query);
    envStrings.push_back("SERVER_PROTOCOL=" + protocol);
    envStrings.push_back("CONTENT_TYPE=" + contentType);
    envStrings.push_back("CONTENT_LENGTH=" + contentLength);
    envStrings.push_back("SCRIPT_NAME=" + scriptName);
    envStrings.push_back("SCRIPT_FILENAME=" + scriptFile);
    envStrings.push_back("REDIRECT_STATUS=200");
    envStrings.push_back("PATH_INFO=");
    envStrings.push_back("SERVER_NAME=" + serverName);
    envStrings.push_back("SERVER_PORT=" + serverPort);
    envStrings.push_back("HTTP_COOKIE=" + cookie);

    std::vector<char *> envp;
    for (std::vector<std::string>::iterator it = envStrings.begin(); it != envStrings.end(); ++it)
        envp.push_back(const_cast<char *>(it->c_str()));
    envp.push_back(NULL);

    // Relative paths treatment
    std::string::size_type slash = path.rfind('/');
    if (slash != std::string::npos)
        chdir(path.substr(0, slash).c_str());

    std::string localScript = "./" + scriptFile;

    char *args[3];
    if (interpreter.empty())
    {
        args[0] = const_cast<char *>(localScript.c_str());
        args[1] = NULL;
        args[2] = NULL;
    }
    else
    {
        args[0] = const_cast<char *>(interpreter.c_str());
        args[1] = const_cast<char *>(localScript.c_str());
        args[2] = NULL;
    }

    if (execve(args[0], args, envp.data()) == -1)
        exit(EXIT_FAILURE);
}