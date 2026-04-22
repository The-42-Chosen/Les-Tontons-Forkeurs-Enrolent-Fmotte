/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 14:01:38 by erpascua          #+#    #+#             */
/*   Updated: 2026/04/22 20:58:53 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"
#include "execption.hpp"
#include "struct.hpp"
#include "utils_request.hpp"
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
    method_http _method;
    std::string _uri;
    std::string _protocol;
    std::map<std::string, std::string> _headers;
    std::vector<__uint8_t> _body;
    bool _keepAlive;
    size_t _contentLength;
    std::string _path_root;

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
    method_http getMethod() const;
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
    void link_to_server(void);
    Location *findLocation(void);
    void init_root(void);

    // =====================
    // ==     Validity    ==
    // =====================
    void isValidURI(void);
    void isValidProtocol(void);
    void isHostPresentAndValid(void);
    void checkAllowedMethods(void);

    // Helper
    static const char *methodToString(method_http method);
};
