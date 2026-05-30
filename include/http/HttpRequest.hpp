/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 14:01:38 by erpascua          #+#    #+#             */
/*   Updated: 2026/05/25 14:45:58 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

class Header;
class Body;
class Request;
class Location;

class HttpRequest
{
  private:
    // =====================
    // ==    Attributs    ==
    // =====================
    Header *_header;
    Body *_body;
    Request *_request;

    HttpRequest();

  public:
    // =====================
    // ==       OCF       ==
    // =====================
    HttpRequest(Request *request);
    ~HttpRequest();
    HttpRequest(const HttpRequest &cpy);
    HttpRequest &operator=(const HttpRequest &cpy);

    // =====================
    // ==     Getters     ==
    // =====================
    Header *getHeader() const;
    void setHeader(Header *header);
    Request *getRequest() const;
    void setRequest(Request *request);
    Body *getBody() const;
    void setBody(Body *body);

    // =====================
    // == 	  Member	  ==
    // =====================
    void initHeader(const std::string &headerContent);
    void initBody();
    std::string selectMethodHttp(Location *location);
};
