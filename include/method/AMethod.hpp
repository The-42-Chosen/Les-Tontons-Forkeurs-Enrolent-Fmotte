/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AMethod.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 12:50:28 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/13 15:33:34 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "execption.hpp"
#include "utilsParsing.hpp"
#include "utilsRequest.hpp"

class HttpRequest;
class Location;

class AMethod
{
  protected:
    // =====================
    // ==    Attributs    ==
    // =====================
    HttpRequest *_http_request;
    Location *_location;
    HttpMethod _method;

    AMethod();

  public:
    // =====================
    // ==       OCF       ==
    // =====================
    AMethod(HttpRequest *http_request, Location *location);
    virtual ~AMethod() = 0;
    AMethod(const AMethod &other);
    AMethod &operator=(const AMethod &other);

    // =====================
    // ==     Getters     ==
    // =====================
    Location *getLocation(void) const;
    void setLocation(Location *location);
    HttpRequest *getHttpRequest(void) const;
    void setHttpRequest(HttpRequest *http_request);
    HttpMethod getMethod(void);
    void setMethod(HttpMethod method);

    // =====================
    // == 	  Member	  ==
    // =====================
    std::string createPath();
    std::string createPathWithLocation();
    std::string createPathWithServer();

    virtual void applyMethod(void) = 0;
};
