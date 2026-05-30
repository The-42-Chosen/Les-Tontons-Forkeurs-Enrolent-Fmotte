/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 11:56:16 by erpascua          #+#    #+#             */
/*   Updated: 2026/05/29 15:49:36 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "struct.hpp"

class Request;

class HttpResponse
{
  private:
    // =====================
    // ==    Attributs    ==
    // =====================
    std::string _responseContent;
    Request *_request;

    HttpResponse();

  public:
    // =====================
    // == Canonical Form  ==
    // =====================
    HttpResponse(Request *Request);
    ~HttpResponse();

    // =====================
    // ==     Getters     ==
    // =====================
    std::string getResponseContent();
    void setResponseContent(std::string responseContent);
    void addResponseContent(std::string responseContent);
    Request *getRequest(void) const;
    void setRequest(Request *Request);

    // =====================
    // == 	  Methods	  ==
    // =====================
    void initialisationHttpResponse();
    void handleError(int statusCode);
    void handleRedirection(int statusCode);
    void handleCorrect(int statusCode);
    void sendToClient();
};
