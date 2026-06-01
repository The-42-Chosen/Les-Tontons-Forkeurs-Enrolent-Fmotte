/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AResponse.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 17:25:03 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/27 18:28:59 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "struct.hpp"

class HttpResponse;

class AResponse
{
  private:
    // =====================
    // ==    Attributs    ==
    // =====================
    HttpResponse *_httpResponse;
    int _statusCode;
    std::string _statusMessage;
    HeaderContent _headerContent;

  public:
    // =====================
    // == Canonical Form  ==
    // =====================
    AResponse(HttpResponse *httpResponse, int statusCode);
    virtual ~AResponse();

    // =====================
    // ==     Getters     ==
    // =====================
    HttpResponse *getHttpResponse() const;
    void setHttpResponse(HttpResponse *httpResponse);
    int getStatusCode() const;
    void setStatusCode(int statusCode);
    std::string getStatusMessage() const;
    void setStatusMessage(const std::string &statusMessagex);
    HeaderContent getHeaderContent(void) const;
    void setHeaderContent(HeaderContent headerContent);
    void addHeaderContent(std::string key, std::string value);

    // =====================
    // == 	  Methods	  ==
    // =====================
    void updateCodeError(int statusCode);
    std::string makeStatusLine();
    void makeHeader();
    std::string makeHttpDate();
    std::string headerToString();

    virtual void applyResponse() = 0;
};
