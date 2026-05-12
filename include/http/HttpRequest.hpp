/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 14:01:38 by erpascua          #+#    #+#             */
/*   Updated: 2026/05/12 16:51:59 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"
#include "Location.hpp"
#include "Server.hpp"
#include "Webserv.hpp"
#include "GetMethod.hpp"
#include "DeleteMethod.hpp"

#include "execption.hpp"
#include "struct.hpp"
#include "utilsRequest.hpp"
#include "colors.hpp"

#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <cstdio>
#include <iostream>

class Client;
class Server;
class Location;

class HttpRequest
{
  private:
    // =====================
    // ==    Attributs    ==
    // =====================
    HttpRequest();
    Client *_client;
    Server *_server;
    Location *_location;
    HttpMethod _method;
    std::string _uri;
    std::string _protocol;
    std::map<std::string, std::string> _headers;
    std::vector<__uint8_t> _body;
    bool _keepAlive;
    size_t _contentLength;
    std::string _path_root;
    size_t _totalChunked;

  public:
    // =====================
    // ==       OCF       ==
    // =====================
    HttpRequest(Client *client);
    HttpRequest(const HttpRequest &cpy);
    HttpRequest &operator=(const HttpRequest &cpy);
    ~HttpRequest();

    // =====================
    // ==     Getters     ==
    // =====================
    HttpMethod getMethod() const;
    const std::string &getUri() const;
    const std::string &getProtocol() const;
    void setClient(Client *client);
    void setServer(Server *server);
    void setLocation(Location *location);
    Server *getServer(void) const;

    // =====================
    // == 	  Member	  ==
    // =====================
    void parseHttpRequest(const std::string &headerContent);
    void parseHeader(const std::string &headerContent);
    void parseHeaderMethod(const std::string &headerContent);
    void parseBody(const std::string &headerContent);
    void parseChunkedBody(const std::string &headerContent);
    void appendBodyBytes(const std::string &data);

    void interpretation(void);
    void bodyInterpretation(void);
    void linkToServer(void);
    void validateRequest(void);
    Location *findLocation(void);

    static std::string toLowerCopy(const std::string &value);
    static std::string trimSpaces(const std::string &value);
    static std::string getHeaderValue(const std::string &request, const std::string &headerName);
    static bool isCompleteChunkedBody(const std::string &request, std::string::size_type bodyStart);

    // =====================
    // ==     Validity    ==
    // =====================
    void isValidURI(void);
    void isValidProtocol(void);
    void isHostPresentAndValid(void);
    void checkAllowedMethods(Location *location);

    // Helper
    static const char *methodToString(HttpMethod method);
};

bool isCompleteHttpRequest(const std::string &request);
bool parseDecimalLength(const std::string &value, size_t &contentLength);
