/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 14:01:38 by erpascua          #+#    #+#             */
/*   Updated: 2026/04/20 16:09:30 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"
#include "execption.hpp"
#include "struct.hpp"
#include <map>
#include <string>
#include <vector>

class Client;

class HttpRequest
{
  private:
    // =====================
    // ==    Attributs    ==
    // =====================
    HttpRequest();
    Client *_client;
    method_http _method;
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
    // == 	  Member	  ==
    // =====================
    void parseHttpRequest(const std::string &headerContent);
    void parseHeader(const std::string &headerContent);
    void parseHeaderMethod(const std::string &headerContent);
    void parseBody(const std::string &headerContent);

    void interpretation(void);
    void link_to_server(void);

    // =====================
    // ==     Validity    ==
    // =====================
    bool isValidURI(void);
    bool isValidProtocol(void);
    bool isHostPresentAndValid(void);

    // =====================
    // ==     Getters     ==
    // =====================
    method_http getMethod() const;
    const std::string &getUri() const;
    const std::string &getProtocol() const;
    void set_client(Client *client);

    // Helper
    static const char *methodToString(method_http method);
};
