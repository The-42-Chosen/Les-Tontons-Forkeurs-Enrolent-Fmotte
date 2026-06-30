/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AMethod.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 12:50:28 by fmotte            #+#    #+#             */
/*   Updated: 2026/06/30 17:23:38 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "struct.hpp"

class HttpRequest;
class Location;
// class CGI;

class AMethod
{
  protected:
    // =====================
    // ==    Attributs    ==
    // =====================
    HttpRequest *_httpRequest;
    HttpMethod _method;
    // bool _isCGI;
    // CGI *_cgi;
    
    AMethod();

  public:
    // =====================
    // ==       OCF       ==
    // =====================

    AMethod(HttpRequest *httpRequest, HttpMethod method);
    virtual ~AMethod() = 0;
    AMethod(const AMethod &other);
    AMethod &operator=(const AMethod &other);

    // =====================
    // ==     Getters     ==
    // =====================
    HttpRequest *getHttpRequest(void) const;
    void setHttpRequest(HttpRequest *httpRequest);
    HttpMethod getMethod(void);
    void setMethod(HttpMethod method);
    // CGI *getCGI() const;
    // void setCGI(CGI *cgi);
    // bool getIsCGI() const;
    // void setIsCGI(bool _isCGI);
    
    // =====================
    // == 	  Member	  ==
    // =====================
    std::string selectRoot(Location *location);
    std::string resolveRequestedFilePath(std::string initPath);

    std::string createPath(Location *location, bool &isAutoIndex);
    std::string createPathWithLocation(Location *location, bool &isAutoIndex);
    std::string createPathWithServer(bool &isAutoIndex);

    void listContentFolder(const std::string &path, std::string &folderContent);
    std::string createContentAutoIndex(const std::string &path);

    virtual std::string applyMethod(Location *location) = 0;
    
    // =====================
    // ==       CGI       ==
    // =====================
    
    void createPipe(int pipeIn[2], int pipeOut[2]);
    void checkForkCreate(pid_t pid, int pipe_in[2], int pipe_out[2]);
    void sendDataToChild(int pipe_in[2]);
    std::string receivedDataFromChild(int pipe_out[2]);
    std::string processDataFromChild(const std::string &payload);
    
    // std::string initCGI(const std::string &path, const std::string &interpreter);

    void manage_pipe(const std::string &path, int pipeIn[2], int pipeOut[2], const std::string &interpreter);
    std::string applyCGI(std::string path, const std::string &interpreter);

};
