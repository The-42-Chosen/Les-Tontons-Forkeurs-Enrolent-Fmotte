/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseContext.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/05 21:33:52 by fmotte            #+#    #+#             */
/*   Updated: 2026/07/22 13:45:46 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "struct.hpp"

class ARequest;

class ResponseContext
{
  private:
    // =====================
    // ==    Attributs    ==
    // =====================
    int _statusCode;
    std::string _payload;
    std::vector<std::string> _cgiSetCookies;
    ARequest *_ARequest;

    ResponseContext();

  public:
    // =====================
    // ==       OCF       ==
    // =====================
    ResponseContext(ARequest *arequest);
    ~ResponseContext();
    ResponseContext(const ResponseContext &other);
    
    // =====================
    // ==     Getters     ==
    // =====================
    int getStatusCode() const;
    void setStatusCode(int statusCode);
    std::string getPayload() const;
    void setPayload(std::string payload);
    const std::vector<std::string> &getCgiSetCookies() const;
    void addCgiSetCookie(const std::string &setCookieValue);
    ARequest *getARequest(void) const;
    void setARequest(ARequest *arequest);
};
