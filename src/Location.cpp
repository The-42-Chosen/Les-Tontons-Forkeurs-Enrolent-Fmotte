/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/06 15:45:46 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/23 13:35:39 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"
#include "Server.hpp"
#include "execption.hpp"
#include "utilsDuplicate.hpp"

// =====================
// == Canonical Form  ==
// =====================

Location::Location()
    : _name(""), _allowed_methods(), _root(""), _index(""), _auto_index(false), _error_page(HttpReturn()),
      _client_max_body_size(DEFAULT_CLIENT_MAX_BODY_SIZE), _ret(HttpReturn())
{
}

Location::~Location()
{
}

Location::Location(const Location &other)
{
    *this = other;
}

Location &Location::operator=(const Location &other)
{
    _name = other._name;
    _allowed_methods = other._allowed_methods;
    _root = other._root;
    _index = other._index;
    _auto_index = other._auto_index;
    _error_page = other._error_page;
    _client_max_body_size = other._client_max_body_size;
    _ret = other._ret;

    return (*this);
}

const std::string Location::list_allowed_methods[4] = {"GET", "POST", "DELETE", "HEAD"};

// =====================
// == Getter & Setter ==
// =====================

// NAME
void Location::setName(std::string name)
{
    _name = name;
}
std::string Location::getName(void)
{
    return _name;
}

// METHOD-HTTP
void Location::addAllowedMethod(HttpMethod i)
{
    _allowed_methods.insert(i);
}
std::set<HttpMethod> Location::getAllowedMethods(void)
{
    return _allowed_methods;
}

// INDEX
void Location::setIndex(std::string index)
{
    _index = index;
}
std::string Location::getIndex(void)
{
    return _index;
}

// ROOT
void Location::setRoot(std::string root)
{
    _root = root;
}
std::string Location::getRoot(void)
{
    return _root;
}

// AUTO-INDEX
void Location::setAutoIndex(bool auto_index)
{
    _auto_index = auto_index;
}
bool Location::getAutoIndex(void)
{
    return _auto_index;
}

// CLIENT-MAX-BODY-SIZE
void Location::setClientMaxBodySize(unsigned int client_max_body_size)
{
    _client_max_body_size = client_max_body_size;
}
unsigned int Location::getClientMaxBodySize(void)
{
    return _client_max_body_size;
}

// ERROR-PAGE
void Location::setErrorPage(HttpReturn error_page)
{
    _error_page = error_page;
}
HttpReturn *Location::getErrorPage(void)
{
    return &_error_page;
}

// RETURN
void Location::setReturn(HttpReturn ret)
{
    _ret = ret;
}
HttpReturn *Location::getReturn(void)
{
    return &_ret;
}

// =====================
// ==     Method      ==
// =====================

void Location::initializeLocation(std::vector<std::string> &tokens)
{
    if (tokens.empty())
        throw ExecptionMissBrace();

    setName(tokens[0]);
    tokens.erase(tokens.begin());

    if (tokens.empty())
        throw ExecptionMissBrace();

    if (tokens[0] != "{")
        throw ExecptionMissBrace();

    tokens.erase(tokens.begin());
    size_t tokens_size = tokens.size();
    size_t new_tokens_size;
    while (true)
    {
        if (tokens.empty())
            throw ExecptionMissBrace();
        if (tokens[0] == "}")
            break;

        initializeLocationAllowedMethods(tokens);
        initializeLocationRoot(tokens);
        initializeLocationIndex(tokens);
        initializeLocationAutoIndex(tokens);
        initializeLocationClientMaxBodySize(tokens);
        initializeLocationErrorPage(tokens);
        initializeLocationReturn(tokens);

        // Security to avoid inifite loop
        new_tokens_size = tokens.size();
        if (tokens_size == new_tokens_size)
            throw ExecptionWrongArgument(tokens[0]);
        tokens_size = new_tokens_size;
    }
    tokens.erase(tokens.begin());
}

void Location::initializeLocationAllowedMethods(std::vector<std::string> &tokens)
{
    if (tokens[0] == "allowed_methods")
    {

        tokens.erase(tokens.begin());

        while (tokens[0] != ";")
        {
            if (tokens[0] == "GET")
                addAllowedMethod(GET);
            else if (tokens[0] == "POST")
                addAllowedMethod(POST);
            else if (tokens[0] == "DELETE")
                addAllowedMethod(DELETE);
            else if (tokens[0] == "HEAD")
                addAllowedMethod(HEAD);
            else
                throw ExecptionIllegalMethod(tokens[0]);

            tokens.erase(tokens.begin());
        }
        tokens.erase(tokens.begin());
    }
}

void Location::initializeLocationRoot(std::vector<std::string> &tokens)
{
    std::string root = parseRootDirective(tokens);
    if (root != "")
        setRoot(root);
}

void Location::initializeLocationIndex(std::vector<std::string> &tokens)
{
    if (tokens[0] == "index")
    {
        tokens.erase(tokens.begin());
        setIndex(tokens[0]);

        tokens.erase(tokens.begin());

        if (tokens[0] != ";")
            throw ExecptionMissSemiColon();

        tokens.erase(tokens.begin());
    }
}

void Location::initializeLocationAutoIndex(std::vector<std::string> &tokens)
{
    int auto_index = parseAutoIndexDirective(tokens);

    if (auto_index == 0)
        setAutoIndex(false);
    else if (auto_index == 1)
        setAutoIndex(true);
}

void Location::initializeLocationClientMaxBodySize(std::vector<std::string> &tokens)
{
    unsigned int client_max_body_size = parseClientMaxBodySizeDirective(tokens);

    if (client_max_body_size != 0)
        setClientMaxBodySize(client_max_body_size);
}

void Location::initializeLocationErrorPage(std::vector<std::string> &tokens)
{
    bool is_init = false;
    HttpReturn error_page = parseErrorPageDirective(tokens, is_init);

    if (is_init)
        setErrorPage(error_page);
}

void Location::initializeLocationReturn(std::vector<std::string> &tokens)
{
    bool is_init = false;
    HttpReturn ret = parseReturnDirective(tokens, is_init);

    if (is_init)
        setReturn(ret);
}
