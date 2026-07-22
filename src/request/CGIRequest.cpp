/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIRequest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:35:36 by fmotte            #+#    #+#             */
/*   Updated: 2026/07/22 12:43:15 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIRequest.hpp"

#include "ARequest.hpp"
#include "Body.hpp"
#include "HandlePath.hpp"
#include "Header.hpp"
#include "HttpRequest.hpp"
#include "Location.hpp"
#include "RequestContext.hpp"
#include "ResponseContext.hpp"
#include "Server.hpp"

#include "utilsConnection.hpp"
#include "utilsRequest.hpp"

// =====================
// ==       OCF       ==
// =====================

CGIRequest::CGIRequest(ARequest arequest) : ARequest(arequest)
{
}

CGIRequest::~CGIRequest()
{
}

// =====================
// ==     Getters     ==
// =====================

int *CGIRequest::getPipeIn()
{
    return _pipeIn;
}

void CGIRequest::setPipeIn(int pipeIn[2])
{
    _pipeIn[0] = pipeIn[0];
    _pipeIn[1] = pipeIn[1];
}

int *CGIRequest::getPipeOut()
{
    return _pipeOut;
}

void CGIRequest::setPipeOut(int pipeOut[2])
{
    _pipeOut[0] = pipeOut[0];
    _pipeOut[1] = pipeOut[1];
}

pid_t CGIRequest::getPid() const
{
    return _pid;
}

void CGIRequest::setPid(pid_t pid)
{
    _pid = pid;
}

// =====================
// == 	  Member	  ==
// =====================

// LEs doubles Pipes et le in/out :
//  pipeIn[1]  ->  stdin   (body POST envoyé par le parent)
//  pipeIn[0]  <-  lecture par l'enfant
//  pipeOut[1] ->  stdout  (réponse HTML générée par l'enfant)
//  pipeOut[0] <-  lecture par le parent
// CGIRequest pipeIn[1] à fermer sinon le serveur attendra indéfiniment (deadlock)

void CGIRequest::createPipe(int pipeIn[2], int pipeOut[2])
{
    if (pipe(pipeOut) == -1 || pipe(pipeIn) == -1)
    {
        std::cerr << "Error pipe\n";
        throw std::runtime_error("500");
    }

    setNonblocking(pipeOut[1]);
    setNonblocking(pipeIn[0]);
}

void CGIRequest::checkForkCreate(pid_t pid)
{
    if (pid == -1)
    {
        std::cerr << "Error fork\n";
        close(getPipeOut()[0]);
        close(getPipeOut()[1]);
        close(getPipeIn()[0]);
        close(getPipeIn()[1]);
        throw std::runtime_error("500");
    }
}

void CGIRequest::initializationCGIRequest(const std::string &interpreter)
{
    int pipeIn[2];
    int pipeOut[2];

    HandlePath handlePath(getRequestContext()->getHttpRequest());
    checkPermisionReadFile(handlePath.createPathCgi(getRequestContext()->getLocation()));

    createPipe(pipeIn, pipeOut);
    setPipeIn(pipeIn);
    setPipeOut(pipeOut);

    pid_t pid = fork();
    setPid(pid);
    checkForkCreate(pid);

    if (pid == 0)
    {
        // child must never return to the parent's event loop: any exception here ends the child process
        try
        {
            manage_pipe(interpreter);
        }
        catch (const std::exception &e)
        {
            std::cerr << "CGI child error: " << e.what() << "\n";
        }
        exit(EXIT_FAILURE);
    }

    close(getPipeOut()[1]);

    connectToEpoll();
}

void CGIRequest::connectToEpoll()
{
    int epoll_fd = getRequestContext()->getClient()->getWebserv()->getEpollFd();

    addFdToEvent(epoll_fd, getPipeIn()[1], EPOLLOUT, WRITECHILD, this);
    addFdToEvent(epoll_fd, getPipeOut()[0], EPOLLIN, READCHILD, this);
}

void CGIRequest::sendDataToChild()
{
    close(getPipeIn()[0]);

    if (getRequestContext()->getHttpRequest()->getBody() != NULL)
    {
        BodyContent body = getRequestContext()->getHttpRequest()->getBody()->getBodyContent();
        if (!body.empty())
            write(getPipeIn()[1], body.data(), body.size());
    }
    close(getPipeIn()[1]);
}

bool CGIRequest::receivedDataFromChild()
{
    char buffer[4096];
    ssize_t nb_read;

    while ((nb_read = read(getPipeOut()[0], buffer, sizeof(buffer))) > 0)
        _cgiBuffer.append(buffer, nb_read);

    if (nb_read == 0)               // EOF : l'enfant a fermé stdout
    {
        getResponseContext()->setPayload(_cgiBuffer);
        return true;                // -> on finalise
    }
    // nb_read == -1 : sur un fd non-bloquant on suppose EAGAIN
    // (au sujet 42 tu n'as pas le droit de tester errno après read ;
    //  ici -1 => "rien de plus pour l'instant", on attend le prochain event)
    return false;
}

void CGIRequest::processDataFromChild()
{
    const std::string &payload = getResponseContext()->getPayload();

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
        return getResponseContext()->setPayload(payload.substr(sep + sepLen));
    }

    getResponseContext()->setPayload(payload);
}

void CGIRequest::forwardCgiHeaders(const std::string &headerBlock)
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
            getResponseContext()->addCgiSetCookie(trimSpaces(line.substr(colon + 1)));
    }
}

void CGIRequest::manage_pipe(const std::string &interpreter)
{
    int *pipeIn = getPipeIn();
    int *pipeOut = getPipeOut();

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
    std::string query = getRequestContext()->getHttpRequest()->getHeader()->getQuery();
    std::string scriptName = getRequestContext()->getHttpRequest()->getHeader()->getScriptName();

    std::string method = (getRequestContext()->getHttpRequest()->getHeader()->getMethod() == POST) ? "POST" : "GET";
    std::string protocol = getRequestContext()->getHttpRequest()->getHeader()->getProtocol();

    HeaderContent hc = getRequestContext()->getHttpRequest()->getHeader()->getHeaderContent();
    std::string contentType = hc.count("content-type") ? hc.at("content-type") : "";
    std::string contentLength = hc.count("content-length") ? hc.at("content-length") : "";
    std::string cookie = hc.count("cookie") ? hc.at("cookie") : "";

    Listen *listen = getRequestContext()->getServer()->getListen(0);
    std::string serverName = (listen && !listen->ip.empty()) ? listen->ip : "localhost";
    std::string serverPort = "80";
    if (listen)
    {
        std::ostringstream oss;
        oss << listen->port;
        serverPort = oss.str();
    }

    HandlePath handlePath(getRequestContext()->getHttpRequest());
    std::string path = handlePath.createPathCgi(getRequestContext()->getLocation());

    checkPermisionReadFile(path);

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