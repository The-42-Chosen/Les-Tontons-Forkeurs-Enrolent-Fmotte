/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Body.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 19:53:54 by fmotte            #+#    #+#             */
/*   Updated: 2026/06/15 14:53:39 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "struct.hpp"
#include <stdint.h>

class HttpRequest;

class Body
{
  private:
    // =====================
    // ==    Attributs    ==
    // =====================
    BodyContent _bodyContent;
    bool _keepAlive;
    size_t _contentLength;
    size_t _totalChunked;
    HttpRequest *_httpRequest;

    Body();

  public:
    // =====================
    // ==       OCF       ==
    // =====================
    Body(const HttpRequest &httpRequest);
    ~Body();
    Body(const Body &other);
    Body &operator=(const Body &other);

    // =====================
    // ==     Getters     ==
    // =====================
    BodyContent getBodyContent(void) const;
    void setBodyContent(BodyContent body);
    void addBodyContent(uint8_t bytes);
    bool getKeepAlive(void) const;
    void setKeepAlive(bool keepAlive);
    size_t getContentLenght(void) const;
    void setContentLenght(size_t contentLength);
    size_t getTotalChunked(void) const;
    void setTotalChunked(size_t totalChunked);
    void addTotalChunked(size_t totalChunked);
    HttpRequest *getHttpRequest(void) const;
    void setHttpRequest(HttpRequest *httpRequest);

    // =====================
    // == 	  Member	  ==
    // =====================
    void initialisationBody();
    void bodyprint(void);
    void parseBody(const std::string &headerContent);
    void parseChunkedBody(const std::string &headerContent);
    void appendBodyBytes(const std::string &data);
    size_t initMaxBodySize();
    void configureKeepAlive(const HeaderContent &headers);
    bool handleTransferEncoding(const HeaderContent &header, const std::string &headerContent);
    bool parseContentLengthBody(const HeaderContent &header, const std::string &headerContent);
    bool handleLastChunk(const std::string &headerContent, size_t chunkSize, std::string::size_type current);
    std::string::size_type initCurrent(const std::string &headerContent);
    void updateTotalChunked(size_t chunkSize, size_t maxBodySize);
};
