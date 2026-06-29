/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/28 15:15:29 by erpascua          #+#    #+#             */
/*   Updated: 2026/06/29 01:48:37 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "struct.hpp"

typedef std::map<std::string, std::string> CookieMap;

class Cookie
{
  private:
    // =====================
    // ==    Attributs    ==
    // =====================
    std::string _name;
    std::string _value;
    std::string _path;
    long _maxAge;
    bool _httpOnly;

  public:
    // =====================
    // ==       OCF       ==
    // =====================
    Cookie();
    Cookie(const std::string &name, const std::string &value);
    Cookie(const Cookie &cpy);
    Cookie &operator=(const Cookie &cpy);
    ~Cookie();

    // =====================
    // ==     Getters     ==
    // =====================
    std::string getName(void) const;
    void setName(const std::string &name);
    std::string getValue(void) const;
    void setValue(const std::string &value);
    std::string getPath(void) const;
    void setPath(const std::string &path);
    long getMaxAge(void) const;
    void setMaxAge(long maxAge);
    bool getHttpOnly(void) const;
    void setHttpOnly(bool httpOnly);

    // =====================
    // == 	  Member	  ==
    // =====================
    std::string toSetCookieValue(void) const;
};

// =====================
// == 	  Fonction    ==
// =====================
CookieMap parseCookieHeader(const std::string &cookieHeader);
