/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AMethod.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 12:50:28 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/25 11:34:40 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "struct.hpp"

class HttpRequest;
class Location;

class AMethod
{
  protected:
    // =====================
    // ==    Attributs    ==
    // =====================
    HttpRequest *_httpRequest;
    HttpMethod _method;

    AMethod();

  public:
    // =====================
    // ==       OCF       ==
    // =====================

    AMethod(HttpRequest *httpRequest, HttpMethod method);
    virtual ~AMethod() = 0;
    AMethod(const AMethod &other);
    AMethod &operator=(const AMethod &other);

    // =====================
    // ==     Getters     ==
    // =====================
    HttpRequest *getHttpRequest(void) const;
    void setHttpRequest(HttpRequest *httpRequest);
    HttpMethod getMethod(void);
    void setMethod(HttpMethod method);

    // =====================
    // == 	  Member	  ==
    // =====================
    std::string createPath(Location *location);
    std::string createPathWithLocation(Location *location);
    std::string createPathWithServer();

    virtual std::string applyMethod(Location *location) = 0;
    void applyCGI(std::string path);
};

void manage_pipe(std::string path, int mypipe[2]);