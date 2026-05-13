/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 11:56:16 by erpascua          #+#    #+#             */
/*   Updated: 2026/05/13 20:08:45 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "HttpRequest.hpp"
# include "utilsResponse.hpp"
# include <map>
# include <string>
# include <dirent.h>
# include <sys/stat.h>
# include <sstream>
# include <fstream>

class HttpResponse
{
  private:
    // =====================
    // ==    Attributs    ==
    // =====================
    int _statusCode;
    Body _body;
    Header _headers;
    std::string _statusMessage;

    HttpResponse();

  public:
    // =====================
    // == Canonical Form  ==
    // =====================
    HttpResponse(int code, Body &body, const std::string &contentType);
    ~HttpResponse();

    // =====================
    // == 	  Methods	  ==
    // =====================
    void addHeader(const std::string &key, const std::string &val);
    std::string build() const;
    void send(int fd) const;

    // =====================
    // == 	  Makers 	  ==
    // =====================
    static HttpResponse makeFile(const std::string &path);
    static HttpResponse makeError(int code, const std::string &customPage = "");
    static HttpResponse makeRedirect(int code, const std::string &location);
    static HttpResponse makeAutoIndex(const std::string &dirPath, const std::string &uri);
};

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 11:56:16 by erpascua          #+#    #+#             */
/*   Updated: 2026/05/12 19:23:34 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "HttpRequest.hpp"
#include <map>
#include <string>

class HttpResponse
{
  private:
	// =====================
	// ==    Attributs    ==
	// =====================
	int _statusCode;
	std::string _statusMessage;
	Header _headers;
	Body _body;

	HttpResponse();

  public:
	// =====================
	// == Canonical Form  ==
	// =====================
	HttpResponse(int code, Body &body, const std::string &contentType);
	~HttpResponse();

	// =====================
	// == 		Methods	  ==
	// =====================
	void addHeader(const std::string &key, const std::string &val);
	std::string build() const;
	void send(int fd) const;

	// =====================
	// == 		Makers 	  ==
	// =====================
	static HttpResponse makeFile(const std::string &path);
	static HttpResponse makeError(int code, const std::string &customPage = "");
	static HttpResponse makeRedirect(int code, const std::string &location);
	static HttpResponse makeAutoIndex(const std::string &dirPath, const std::string &uri);
};
