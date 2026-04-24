/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 14:01:38 by erpascua          #+#    #+#             */
/*   Updated: 2026/04/24 17:40:53 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"
#include "execption.hpp"
#include "struct.hpp"
#include "utilsRequest.hpp"
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

class Client;
class Server;

class HttpRequest
{
  private:
    // =====================
    // ==    Attributs    ==
    // =====================
    HttpRequest();
    Client *_client;
    Server *_server;
    HttpMethod _method;
    std::string _uri;
    std::string _protocol;
    std::map<std::string, std::string> _headers;
    std::vector<__uint8_t> _body;
    bool _keepAlive;
    size_t _contentLength;

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

    void readFile(Location *location);
    std::string resolveRoot(Location *location);
    std::string createPath(Location *location);

    // =====================
    // ==     Validity    ==
    // =====================
    void isValidURI(void);
    void isValidProtocol(void);
    void isHostPresentAndValid(void);
    void checkAllowedMethods(Location *location);
    void checkPermisionReadFile(std::string path);
    void isFinishByFile(std::string path);

    // Helper
    static const char *methodToString(HttpMethod method);
};
