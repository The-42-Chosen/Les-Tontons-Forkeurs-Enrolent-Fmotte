/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIRequest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 16:35:36 by fmotte            #+#    #+#             */
/*   Updated: 2026/08/06 19:35:17 by erpascua         ###   ########.fr       */
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

#include <cctype>

// =====================
// ==       OCF       ==
// =====================

CGIRequest::CGIRequest(ARequest arequest)
    : ARequest(arequest), _pid(-1), _cgiBuffer(""), _bodyBytesSent(0), _eventDataWriteChild(NULL),
      _eventDataReadChild(NULL)
{
    _pipeIn[0] = -1;
    _pipeIn[1] = -1;
    _pipeOut[0] = -1;
    _pipeOut[1] = -1;
}

CGIRequest::~CGIRequest()
{
    // closing the fds also removes them from epoll; never use a fd already
    // closed elsewhere (it could have been reused by another client)
    closeStdinPipe();
    closeStdoutPipe();

    delete _eventDataWriteChild;
    delete _eventDataReadChild;
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

EventData *CGIRequest::geteventDataWrite() const
{
    return _eventDataWriteChild;
}

void CGIRequest::seteventDataWrite(EventData *eventDataWriteChild)
{
    if (eventDataWriteChild == NULL)
        throw ExecptionErrorUninitializedVariable("*eventDataWriteChild", "CGIRequest");

    _eventDataWriteChild = eventDataWriteChild;
}

EventData *CGIRequest::geteventDataRead() const
{
    return _eventDataReadChild;
}

void CGIRequest::seteventDataRead(EventData *eventDataReadChild)
{
    if (eventDataReadChild == NULL)
        throw ExecptionErrorUninitializedVariable("*eventDataReadChild", "CGIRequest");

    _eventDataReadChild = eventDataReadChild;
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
    // The child ends (pipeIn[0]/pipeOut[1]) stay blocking: they become the
    // CGI stdin/stdout. The parent ends are made non-blocking by addFdToEvent.
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
        _pipeOut[0] = -1;
        _pipeOut[1] = -1;
        _pipeIn[0] = -1;
        _pipeIn[1] = -1;
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

    // The parent does not use the child ends of the pipes
    close(getPipeOut()[1]);
    _pipeOut[1] = -1;
    close(getPipeIn()[0]);
    _pipeIn[0] = -1;

    connectToEpoll();
}

void CGIRequest::connectToEpoll()
{
    int epoll_fd = getRequestContext()->getClient()->getWebserv()->getEpollFd();

    EventData *eventData1 = addFdToEvent(epoll_fd, getPipeIn()[1], EPOLLOUT, WRITECHILD, this);
    EventData *eventData2 = addFdToEvent(epoll_fd, getPipeOut()[0], EPOLLIN, READCHILD, this);

    std::cout << "Add to Epoll\n";

    seteventDataWrite(eventData1);
    seteventDataRead(eventData2);
}

// One write per EPOLLOUT event on the pipe; the return value says if the
// whole body has been fed to the child (stdin closed -> EOF for the CGI)
bool CGIRequest::sendDataToChild()
{
    Body *body = getRequestContext()->getHttpRequest()->getBody();

    if (body != NULL && _bodyBytesSent < body->getBodyContent().size())
    {
        const BodyContent &content = body->getBodyContent();
        size_t remaining = content.size() - _bodyBytesSent;
        ssize_t nb_written = write(getPipeIn()[1], &content[_bodyBytesSent], remaining);

        if (nb_written > 0)
        {
            _bodyBytesSent += nb_written;
            if (_bodyBytesSent < content.size())
                return false; // pipe full: wait for the next EPOLLOUT event
        }
        // nb_written <= 0: the child is gone, stop feeding it
    }

    closeStdinPipe();
    return true;
}

bool CGIRequest::receivedDataFromChild()
{
    char buffer[65536];
    ssize_t nb_read = read(getPipeOut()[0], buffer, sizeof(buffer));

    if (nb_read > 0)
    {
        // One read per event: epoll (level-triggered) will notify again
        // if more data is already waiting in the pipe
        _cgiBuffer.append(buffer, nb_read);
        return false;
    }

    if (nb_read == 0) // EOF : l'enfant a fermé stdout
    {
        getResponseContext()->setPayload(_cgiBuffer);
        return true; // -> on finalise
    }
    // nb_read == -1 : rien de plus pour l'instant, on attend le prochain event
    return false;
}

// Explicit DEL before close: a CGI child forked in the same loop turn still
// holds a copy of the fd, so close() alone would leave a stale epoll entry
void CGIRequest::closeStdinPipe()
{
    if (_pipeIn[1] >= 0)
    {
        epoll_ctl(getRequestContext()->getClient()->getWebserv()->getEpollFd(), EPOLL_CTL_DEL, _pipeIn[1], NULL);
        close(_pipeIn[1]);
        _pipeIn[1] = -1;
    }
}

void CGIRequest::closeStdoutPipe()
{
    if (_pipeOut[0] >= 0)
    {
        epoll_ctl(getRequestContext()->getClient()->getWebserv()->getEpollFd(), EPOLL_CTL_DEL, _pipeOut[0], NULL);
        close(_pipeOut[0]);
        _pipeOut[0] = -1;
    }
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
    bool statusSet = false;

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
        else if (key == "status")
        {
            std::stringstream ss(trimSpaces(line.substr(colon + 1)));
            int code = 0;
            if (ss >> code && code >= 100 && code <= 599)
            {
                getResponseContext()->setStatusCode(code);
                statusSet = true;
            }
        }
    }

    if (!statusSet)
        applyDefaultCgiStatus();
}

void CGIRequest::applyDefaultCgiStatus()
{
    HttpMethod method = getRequestContext()->getHttpRequest()->getHeader()->getMethod();

    if (method == POST)
        getResponseContext()->setStatusCode(201);
    else if (method == DELETE)
        getResponseContext()->setStatusCode(204);
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

    for (HeaderContent::const_iterator it = hc.begin(); it != hc.end(); ++it)
    {
        if (it->first == "content-type" || it->first == "content-length")
            continue;

        std::string name = it->first;
        for (std::string::size_type i = 0; i < name.size(); ++i)
            name[i] = (name[i] == '-') ? '_' : std::toupper(name[i]);

        envStrings.push_back("HTTP_" + name + "=" + it->second);
    }

    std::vector<char *> envp;
    for (std::vector<std::string>::iterator it = envStrings.begin(); it != envStrings.end(); ++it)
        envp.push_back(const_cast<char *>(it->c_str()));
    envp.push_back(NULL);

    // Relative paths treatment
    std::string::size_type slash = path.rfind('/');
    if (slash != std::string::npos)
    {
        int ret = chdir(path.substr(0, slash).c_str());
        (void)ret;
    }

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