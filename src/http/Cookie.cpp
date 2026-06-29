/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 01:49:53 by erpascua          #+#    #+#             */
/*   Updated: 2026/06/29 02:11:09 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cookie.hpp"
#include "utilsDuplicate.hpp"

// =====================
// ==       OCF       ==
// =====================

Cookie::Cookie() : _name("default"), _value(""), _path(""), _maxAge(-1), _httpOnly(false)
{
}
Cookie::Cookie(const std::string &name, const std::string &value)
    : _name(name), _value(value), _path(""), _maxAge(-1), _httpOnly(false)
{
}
Cookie::Cookie(const Cookie &cpy)
{
    *this = cpy;
}

Cookie &Cookie::operator=(const Cookie &cpy)
{
    if (this != &cpy)
    {
        _name = cpy._name;
        _value = cpy._value;
        _path = cpy._path;
        _maxAge = cpy._maxAge;
        _httpOnly = cpy._httpOnly;
    }
    return (*this);
}

Cookie::~Cookie()
{
}

// =====================
// ==     Getters     ==
// =====================

std::string Cookie::getName(void) const
{
    return (_name);
}
void Cookie::setName(const std::string &name)
{
    _name = name;
}
std::string Cookie::getValue(void) const
{
    return (_value);
}
void Cookie::setValue(const std::string &value)
{
    _value = value;
}
std::string Cookie::getPath(void) const
{
    return (_path);
}
void Cookie::setPath(const std::string &path)
{
    _path = path;
}
long Cookie::getMaxAge(void) const
{
    return (_maxAge);
}
void Cookie::setMaxAge(long maxAge)
{
    _maxAge = maxAge;
}
bool Cookie::getHttpOnly(void) const
{
    return (_httpOnly);
}
void Cookie::setHttpOnly(bool httpOnly)
{
    _httpOnly = httpOnly;
}

// =====================
// == 	  Member	  ==
// =====================

std::string Cookie::toSetCookieValue(void) const
{
    std::ostringstream cookieVal;

    cookieVal << _name << '=' << _value;
    if (!_path.empty())
        cookieVal << "; Path=" << _path;
    if (_maxAge > 0)
        cookieVal << "; Max-Age=" << _maxAge;
    if (_httpOnly)
        cookieVal << "; HttpOnly";

    return (cookieVal.str());
}

CookieMap parseCookieHeader(const std::string &cookieHeader)
{
    CookieMap cookies;
    std::stringstream headerStream(cookieHeader);
    std::string cookiePart;

    while (std::getline(headerStream, cookiePart, ';'))
    {
        std::string::size_type separatorPos = cookiePart.find('=');

        if (separatorPos == std::string::npos)
            continue;

        std::string cookieName = trimSpaces(cookiePart.substr(0, separatorPos));
        std::string cookieValue = trimSpaces(cookiePart.substr(separatorPos + 1));

        if (!cookieName.empty())
            cookies[cookieName] = cookieValue;
    }
    return (cookies);
}
