/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 14:01:38 by erpascua          #+#    #+#             */
/*   Updated: 2026/05/17 20:36:29 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "DeleteMethod.hpp"
#include "GetMethod.hpp"
#include "HeadMethod.hpp"
#include "Location.hpp"
#include "PostMethod.hpp"
#include "Server.hpp"
#include "Webserv.hpp"
#include "Header.hpp"
#include "Body.hpp"

class Client;
class Server;
class Header;
class Body;
class Location;
class HttpRequest
{
  private:
    // =====================
    // ==    Attributs    ==
    // =====================
    Client *_client;
    Server *_server;
    Header *_header;
    Body *_body;
    Location *_location;
    
    HttpRequest();
    
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
    Client *getClient(void) const;
    void setClient(Client *client);
    Server *getServer(void) const;
    void setServer(Server *server);
    Header *getHeader(void) const;
    void setHeader(Header *header);
    Body *getBody(void) const;
    void setBody(Body *body);
    Location *getLocation(void) const;
    void setLocation(Location *location);

    // =====================
    // == 	  Member	    ==
    // =====================
    void parseHttpRequest(const std::string &headerContent);
    void initialisationHttpRequest(Client *client);

    void interpretation(void);
    void linkToServer(void);
    void validateRequest(void);
    Location *findLocation(void);
    //void handlingErrorCode(const std::exception &e);

    static std::string getHeaderValue(const std::string &request, const std::string &headerName);

    // =====================
    // ==     Validity    ==
    // =====================
    void checkAllowedMethods(Location *location);
  };
  
// =====================
// == 	  Fonction    ==
// =====================
bool isCompleteHttpRequest(const std::string &request);
bool isCompleteChunkedBody(const std::string &request, std::string::size_type bodyStart);
bool isFinalChunkComplete(const std::string &request, std::string::size_type current);
std::string initSizeToken(const std::string &request, const std::string::size_type &current, const std::string::size_type &lineEnd);

bool parseDecimalLength(const std::string &value, size_t &contentLength);
