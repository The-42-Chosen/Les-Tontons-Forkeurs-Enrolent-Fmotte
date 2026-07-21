/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandlePath.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/06 05:37:27 by fmotte            #+#    #+#             */
/*   Updated: 2026/07/06 06:04:06 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "struct.hpp"

class Location;
class HttpRequest;

class HandlePath
{
  private:
    // =====================
    // ==    Attributs    ==
    // =====================
    std::string _path;
    bool _isAutoIndex;
    HttpRequest *_httpRequest;

    HandlePath();

  public:
    // =====================
    // ==       OCF       ==
    // =====================
    HandlePath(HttpRequest *httpRequest);
    ~HandlePath();

    // =====================
    // ==     Getters     ==
    // =====================
    std::string getPath() const;
    void setPath(std::string path);
    bool getIsAutoIndex() const;
    void setIsAutoIndex(bool isAutoIndex);
    HttpRequest *getHttpRequest() const;
    void setHttpRequest(HttpRequest *httpRequest);

    // =====================
    // == 	  Member	  ==
    // =====================
    std::string selectRoot(Location *location);
    std::string resolveRequestedFilePath(std::string initPath);

    std::string createPath(Location *location);
    std::string createPathWithLocation(Location *location);
    std::string createPathWithServer();
    std::string createPathCgi(Location *location);

    void listContentFolder(const std::string &path, std::string &folderContent);
    std::string createContentAutoIndex(const std::string &path);
};
