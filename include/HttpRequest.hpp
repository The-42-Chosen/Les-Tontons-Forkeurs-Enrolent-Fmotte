/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 14:01:38 by erpascua          #+#    #+#             */
/*   Updated: 2026/04/17 16:27:41 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "struct.hpp"
# include <map>
# include <string>
# include <vector>
# include "Client.hpp"
# include "execption.hpp"

class Client;

class HttpRequest
{
  private:
    // Attributs
    HttpRequest();
    Client* _client;
    method_http _method;
    std::string _uri;
    std::string _protocol;
    std::map<std::string, std::string> _headers;
    std::vector<__uint8_t> _body;
    bool _keepAlive;
    size_t _contentLength;

  public:
    // OCF
    HttpRequest(Client* client);
    HttpRequest(const HttpRequest &cpy);
    HttpRequest &operator=(const HttpRequest &cpy);
    ~HttpRequest();

    // Member Functions
    HttpRequest &parseHttpRequest(const std::string &headerContent);
    HttpRequest &parseHeader(const std::string &headerContent);
    HttpRequest &parseHeaderMethod(const std::string &headerContent);
    HttpRequest &parseBody(const std::string &headerContent);
  
    void interpretation(void);
    void link_to_server(void);
    
    // Getter
    method_http getMethod() const;
    const std::string &getUri() const;
    const std::string &getProtocol() const;
    void set_client(Client *client);
    
    // Helper
    static const char *methodToString(method_http method);
};
