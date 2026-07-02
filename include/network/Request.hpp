/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 14:35:48 by fmotte            #+#    #+#             */
/*   Updated: 2026/07/02 04:38:45 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "execption.hpp"

class Server;
class HttpRequest;
class Client;
class Location;

class HandleRequest
{
  private:
    // =====================
    // ==    Attributs    ==
    // =====================
    Client *_client;
    Server *_server;
    HttpRequest *_httpRequest;
    int _statusCode;
    Location *_location;
    std::string _payload;
    std::vector<std::string> _cgiSetCookies;

  public:
    // =====================
    // ==       OCF       ==
    // =====================
    HandleRequest();
    HandleRequest(const HandleRequest &cpy);
    HandleRequest &operator=(const HandleRequest &cpy);
    ~HandleRequest();

    // =====================
    // ==     Getters     ==
    // =====================
    Client *getClient(void) const;
    void setClient(Client *client);
    Server *getServer(void) const;
    void setServer(Server *server);
    HttpRequest *getHttpRequest(void) const;
    void setHttpRequest(HttpRequest *httpRequest);
    int getStatusCode() const;
    void setStatusCode(int statusCode);
    Location *getLocation(void) const;
    void setLocation(Location *location);
    std::string getPayload() const;
    void setPayload(std::string payload);
    const std::vector<std::string> &getCgiSetCookies() const;
    void addCgiSetCookie(const std::string &setCookieValue);

    // =====================
    // == 	  Member	  ==
    // =====================
    bool initialisationRequest(Client *client);
    void processRequest();
    void linkToServer(void);
    void validateRequest(Location *location);
    Location *findLocation(void);
    void checkAllowedMethods(Location *location);
    void checkServerIsOpen();
    void checkLocationIsOpen(Location *location);
};

// =====================
// == 	  Fonction    ==
// =====================
bool isCompleteRequest(const std::string &request);
bool isCompleteChunkedBody(const std::string &request, std::string::size_type bodyStart);
bool isFinalChunkComplete(const std::string &request, std::string::size_type current);
std::string initSizeToken(const std::string &request, const std::string::size_type &current,
                          const std::string::size_type &lineEnd);
std::string getHeaderValue(const std::string &request, const std::string &headerName);
bool parseDecimalLength(const std::string &value, size_t &contentLength);
