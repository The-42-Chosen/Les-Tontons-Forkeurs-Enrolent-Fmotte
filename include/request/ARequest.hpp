/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ARequest.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 14:35:48 by fmotte            #+#    #+#             */
/*   Updated: 2026/07/06 04:08:16 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "struct.hpp"

class RequestContext;
class ResponseContext;

class ARequest
{
  private:
    // =====================
    // ==    Attributs    ==
    // =====================
    RequestContext *_requestContext;
    ResponseContext *_responseContext;

  public:
    // =====================
    // ==       OCF       ==
    // =====================
    ARequest();
    ARequest(const ARequest &arequest);
    virtual ~ARequest();

    // =====================
    // ==     Getters     ==
    // =====================
    RequestContext *getRequestContext() const;
    void setRequestContext(RequestContext *requestContext);
    ResponseContext *getResponseContext() const;
    void setResponseContext(ResponseContext *responseContext);

    // =====================
    // == 	  Member	  ==
    // =====================
    int initialisationARequest();
};
