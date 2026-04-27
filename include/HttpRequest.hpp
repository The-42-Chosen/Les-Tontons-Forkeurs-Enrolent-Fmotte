/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 14:01:38 by erpascua          #+#    #+#             */
/*   Updated: 2026/04/27 18:39:10 by erpascua         ###   ########.fr       */
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
    Server *getServer(void) const;
    void setLocation(Location *location);
    Location *getLocation(void) const;
    void setRoot(std::string root);
    std::string getRoot(void) const;

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
    Location *findLocation(void);
    void resolveRoot(void);

    static std::string toLowerCopy(const std::string &value);
    static std::string trimSpaces(const std::string &value);
    static std::string getHeaderValue(const std::string &request, const std::string &headerName);
    static bool hasChunkedEncoding(const std::string &transferEncoding);
    static bool isCompleteChunkedBody(const std::string &request, std::string::size_type bodyStart);

    // =====================
    // ==     Validity    ==
    // =====================
    void isValidURI(void);
    void isValidProtocol(void);
    void isHostPresentAndValid(void);
    void checkAllowedMethods(void);

    // Helper
    static const char *methodToString(HttpMethod method);
};

bool isCompleteHttpRequest(const std::string &request);
bool parseDecimalLength(const std::string &value, size_t &contentLength);
