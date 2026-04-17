/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 14:01:38 by erpascua          #+#    #+#             */
/*   Updated: 2026/04/17 18:18:58 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "struct.hpp"
#include <map>
#include <string>
#include <vector>

class HttpRequest
{
  private:
  	// =====================
	// ==    Attributs    ==
	// =====================
    HttpRequest();
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
    HttpRequest(std::string requestRawContent);
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

    // Helper
    static const char *methodToString(method_http method);
};
