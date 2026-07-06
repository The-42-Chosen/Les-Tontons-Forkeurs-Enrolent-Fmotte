/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 11:56:16 by erpascua          #+#    #+#             */
/*   Updated: 2026/07/06 02:08:11 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "struct.hpp"

class ARequest;
class Location;
class AResponse;
class ErrorResponse;
class RedirResponse;
class CorrectResponse;

class HttpResponse
{
  private:
    // =====================
    // ==    Attributs    ==
    // =====================
    std::string _responseContent;
    ARequest *_arequest;

    HttpResponse();

  public:
    // =====================
    // == Canonical Form  ==
    // =====================
    HttpResponse(ARequest *arequest);
    ~HttpResponse();

    // =====================
    // ==     Getters     ==
    // =====================
    std::string getResponseContent();
    void setResponseContent(std::string responseContent);
    void addResponseContent(std::string responseContent);
    ARequest *getARequest(void) const;
    void setARequest(ARequest *arequest);

    // =====================
    // == 	  Methods	  ==
    // =====================
    void initialisationHttpResponse();
    AResponse *selectResponse(int statusCode, ErrorResponse &error, RedirResponse &redir, CorrectResponse &correct);
    void sendToClient();
};
