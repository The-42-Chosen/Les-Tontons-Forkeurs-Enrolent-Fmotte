/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/17 17:37:05 by fmotte            #+#    #+#             */
/*   Updated: 2026/05/17 21:01:33 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "HttpRequest.hpp"

class Header
{
    private:
        // =====================
        // ==    Attributs    ==
        // =====================
        HttpMethod _method;
        std::string _uri;
        std::string _protocol;
        HeaderContent _headerContent;
        std::string _host;
        
        Header();
        
    public:
        // =====================
        // ==       OCF       ==
        // =====================
        Header(const std::string &headerContent);
        ~Header();
        Header(const Header &other);
        Header &operator=(const Header &other);
        
        // =====================
        // ==     Getters     ==
        // =====================
        HttpMethod getMethod(void) const;
        void setMethod(const std::string &method);
        std::string getUri(void) const;
        void setUri(std::string uri);
        std::string getProtocol(void) const;
        void setProtocol(std::string protocol);
        HeaderContent getHeaderContent(void) const;
        void setHeaderContent(HeaderContent headerContent);
        void addHeaderContent(std::string key, std::string value);
        std::string getHost(void) const;
        void setHost(std::string host);
           
        // =====================
        // == 	  Member	  ==
        // =====================
        void initialisationHeader(const std::string &headerContent);
        void parseHeaderInfo(const std::string &headerContent);
        void parseHeaderContent(const std::string &headerContent);
        HttpMethod parseMethodToken(const std::string &method);
        std::string::size_type findEnd(const std::string &headerContent, const std::string &end);
};


