/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AMethod.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 12:54:14 by fmotte            #+#    #+#             */
/*   Updated: 2026/07/06 05:41:55 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AMethod.hpp"

#include "Body.hpp"
#include "Header.hpp"
#include "HttpRequest.hpp"
#include "Location.hpp"
#include "ARequest.hpp"
#include "Server.hpp"
#include "RequestContext.hpp"
#include "ResponseContext.hpp"

#include "execption.hpp"
#include "utilsDuplicate.hpp"
#include "utilsParsing.hpp"
#include "utilsRequest.hpp"

#include <dirent.h>
#include <sstream>
#include <sys/wait.h>
#include <unistd.h>
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


// LEs doubles Pipes et le in/out :
//  pipe_in[1]  ->  stdin   (body POST envoyé par le parent)
//  pipe_in[0]  <-  lecture par l'enfant
//  pipe_out[1] ->  stdout  (réponse HTML générée par l'enfant)
//  pipe_out[0] <-  lecture par le parent
// CGI pipe_in[1] à fermer sinon le serveur attendra indéfiniment (deadlock)

std::string AMethod::applyCGI(std::string path, const std::string &interpreter)
{
    int pipe_out[2];
    int pipe_in[2];

    if (pipe(pipe_out) == -1 || pipe(pipe_in) == -1)
    {
        std::cerr << "Error pipe\n";
        throw std::runtime_error("500");
    }

    pid_t pid = fork();

    if (pid == -1)
    {
        std::cerr << "Error fork\n";
        close(pipe_out[0]);
        close(pipe_out[1]);
        close(pipe_in[0]);
        close(pipe_in[1]);
        throw std::runtime_error("500");
    }

    if (pid == 0)
        manage_pipe(path, pipe_out, pipe_in, interpreter);

    close(pipe_out[1]);
    close(pipe_in[0]);

    if (_httpRequest->getBody() != NULL)
    {
        BodyContent body = _httpRequest->getBody()->getBodyContent();
        if (!body.empty())
            write(pipe_in[1], body.data(), body.size());
    }
    close(pipe_in[1]);

    char buffer[1024];
    int nb_read;
    std::string payload;

    while ((nb_read = read(pipe_out[0], buffer, sizeof(buffer))) > 0)
        payload.append(buffer, nb_read);

    close(pipe_out[0]);
    waitpid(pid, NULL, 0);

    std::string::size_type sep = payload.find("\r\n\r\n");
    std::string::size_type sepLen = 4;
    if (sep == std::string::npos)
    {
        sep = payload.find("\n\n");
        sepLen = 2;
    }
    if (sep != std::string::npos)
    {
        forwardCgiHeaders(payload.substr(0, sep));
        return payload.substr(sep + sepLen);
    }

    return payload;
}

void AMethod::forwardCgiHeaders(const std::string &headerBlock)
{
    std::stringstream stream(headerBlock);
    std::string line;

    while (std::getline(stream, line))
    {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        std::string::size_type colon = line.find(':');
        if (colon == std::string::npos)
            continue;

        std::string key = toLowerString(trimSpaces(line.substr(0, colon)));
        if (key == "set-cookie")
            getHttpRequest()->getRequestContext()->getARequest()->getResponseContext()->addCgiSetCookie(trimSpaces(line.substr(colon + 1)));
    }
}

void AMethod::manage_pipe(std::string path, int pipe_out[2], int pipe_in[2], const std::string &interpreter)
{
    close(pipe_out[0]);
    if (dup2(pipe_out[1], STDOUT_FILENO) == -1)
    {
        std::cerr << "dup2 error\n";
        exit(EXIT_FAILURE);
    }
    if (dup2(pipe_out[1], STDERR_FILENO) == -1)
    {
        std::cerr << "dup2 error\n";
        exit(EXIT_FAILURE);
    }
    close(pipe_out[1]);

    close(pipe_in[1]);
    if (dup2(pipe_in[0], STDIN_FILENO) == -1)
    {
        std::cerr << "dup2 error\n";
        exit(EXIT_FAILURE);
    }
    close(pipe_in[0]);

    // URI / query string / script name
    std::string uri = _httpRequest->getHeader()->getUri();
    std::string query = "";
    std::string scriptName = uri;
    std::string::size_type qpos = uri.find('?');
    if (qpos != std::string::npos)
    {
        query = uri.substr(qpos + 1);
        scriptName = uri.substr(0, qpos);
    }

    std::string method = (_httpRequest->getHeader()->getMethod() == POST) ? "POST" : "GET";
    std::string protocol = _httpRequest->getHeader()->getProtocol();
    HeaderContent hc = _httpRequest->getHeader()->getHeaderContent();

    std::string contentType = hc.count("content-type") ? hc.at("content-type") : "";
    std::string contentLength = hc.count("content-length") ? hc.at("content-length") : "";
    std::string cookie = hc.count("cookie") ? hc.at("cookie") : "";

    Listen *listen = _httpRequest->getRequestContext()->getServer()->getListen(0);
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

