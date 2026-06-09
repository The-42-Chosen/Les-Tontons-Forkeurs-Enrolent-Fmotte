/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AMethod.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 12:50:28 by fmotte            #+#    #+#             */
/*   Updated: 2026/06/15 13:37:06 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "struct.hpp"

class HttpRequest;
class Location;

class AMethod
{
  protected:
    // =====================
    // ==    Attributs    ==
    // =====================
    HttpRequest *_httpRequest;
    HttpMethod _method;

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

    // =====================
    // == 	  Member	  ==
    // =====================
    std::string selectRoot(Location *location);
    std::string resolveRequestedFilePath(std::string initPath);
    
    std::string createPath(Location *location, bool &isAutoIndex);
    std::string createPathWithLocation(Location *location, bool &isAutoIndex);
    std::string createPathWithServer(bool &isAutoIndex);
    
    void listContentFolder(const std::string& path, std::string& folderContent);
    std::string createContentAutoIndex(const std::string& path);
        
    virtual std::string applyMethod(Location *location) = 0;

    // =====================
    // ==       CGI       ==
    // =====================
    std::string applyCGI(std::string path, const std::string &interpreter);
    void manage_pipe(std::string path, int pipe_out[2], int pipe_in[2], const std::string &interpreter);
};
