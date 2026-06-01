/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 11:56:16 by erpascua          #+#    #+#             */
/*   Updated: 2026/05/30 18:40:40 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "struct.hpp"

class HandleRequest;
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
    HandleRequest *_request;

    HttpResponse();

  public:
    // =====================
    // == Canonical Form  ==
    // =====================
    HttpResponse(HandleRequest *Request);
    ~HttpResponse();

    // =====================
    // ==     Getters     ==
    // =====================
    std::string getResponseContent();
    void setResponseContent(std::string responseContent);
    void addResponseContent(std::string responseContent);
    HandleRequest *getRequest(void) const;
    void setRequest(HandleRequest *Request);

    // =====================
    // == 	  Methods	  ==
    // =====================
    void initialisationHttpResponse();
    AResponse *selectResponse(int statusCode, ErrorResponse &error, RedirResponse &redir,
                              CorrectResponse &correct);
    void sendToClient();
};
