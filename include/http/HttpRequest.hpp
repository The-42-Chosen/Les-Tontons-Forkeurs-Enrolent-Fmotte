/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 14:01:38 by erpascua          #+#    #+#             */
/*   Updated: 2026/05/30 18:40:40 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

class Header;
class Body;
class HandleRequest;
class Location;

class HttpRequest
{
  private:
    // =====================
    // ==    Attributs    ==
    // =====================
    Header *_header;
    Body *_body;
    HandleRequest *_request;

    HttpRequest();

  public:
    // =====================
    // ==       OCF       ==
    // =====================
    HttpRequest(HandleRequest *request);
    ~HttpRequest();
    HttpRequest(const HttpRequest &cpy);
    HttpRequest &operator=(const HttpRequest &cpy);

    // =====================
    // ==     Getters     ==
    // =====================
    Header *getHeader() const;
    void setHeader(Header *header);
    HandleRequest *getRequest() const;
    void setRequest(HandleRequest *request);
    Body *getBody() const;
    void setBody(Body *body);

    // =====================
    // == 	  Member	  ==
    // =====================
    void initHeader(const std::string &headerContent);
    void initBody();
    std::string selectMethodHttp(Location *location);
};
