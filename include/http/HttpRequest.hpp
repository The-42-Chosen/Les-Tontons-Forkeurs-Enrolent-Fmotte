/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 14:01:38 by erpascua          #+#    #+#             */
/*   Updated: 2026/07/22 14:05:38 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "struct.hpp"

#include "Cookie.hpp"

class Header;
class Body;
class RequestContext;
class Location;

class HttpRequest
{
  private:
    // =====================
    // ==    Attributs    ==
    // =====================
    Header *_header;
    Body *_body;
    RequestContext *_requestContext;

    HttpRequest();

  public:
    // =====================
    // ==       OCF       ==
    // =====================
    HttpRequest(RequestContext *requestContext);
    ~HttpRequest();
    HttpRequest(const HttpRequest &other);

    // =====================
    // ==     Getters     ==
    // =====================
    Header *getHeader() const;
    void setHeader(Header *header);
    RequestContext *getRequestContext() const;
    void setRequestContext(RequestContext *requestContext);
    Body *getBody() const;
    void setBody(Body *body);

    // =====================
    // == 	  Member	  ==
    // =====================
    void initHeader(const std::string &headerContent);
    void initBody();
    CookieMap getCookies() const;
};
