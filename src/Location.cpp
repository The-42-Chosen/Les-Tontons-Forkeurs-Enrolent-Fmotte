/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/06 15:45:46 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/12 18:04:42 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"
#include "execption.hpp"
#include "Server.hpp"
#include "utils_duplicate.hpp"

// =====================
// == Canonical Form  ==
// =====================

Location::Location()
    : _name(""), _allowed_methods(), _root(""), _index(""), _auto_index(false), _error_page(s_return()),
      _client_max_body_size(DEFAULT_CLIENT_MAX_BODY_SIZE), _ret(s_return())
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
void Location::set_name(std::string name)
{
    _name = name;
}
std::string Location::get_name(void)
{
    return _name;
}

// METHOD-HTTP
void Location::add_methode_http(method_http i)
{
    _allowed_methods.insert(i);
}
std::set<method_http> Location::get_methode_http(void)
{
    return _allowed_methods;
}

// INDEX
void Location::set_index(std::string index)
{
    _index = index;
}
std::string Location::get_index(void)
{
    return _index;
}

// ROOT
void Location::set_root(std::string root)
{
    _root = root;
}
std::string Location::get_root(void)
{
    return _root;
}

// AUTO-INDEX
void Location::set_auto_index(bool auto_index)
{
    _auto_index = auto_index;
}
bool Location::get_auto_index(void)
{
    return _auto_index;
}

// CLIENT-MAX-BODY-SIZE
void Location::set_client_max_body_size(unsigned int client_max_body_size)
{
    _client_max_body_size = client_max_body_size;
}
unsigned int Location::get_client_max_body_size(void)
{
    return _client_max_body_size;
}

// ERROR-PAGE
void Location::set_error_page(s_return error_page)
{
    _error_page = error_page;
}
s_return *Location::get_error_page(void)
{
    return &_error_page;
}

// RETURN
void Location::set_return(s_return ret)
{
    _ret = ret;
}
s_return *Location::get_return(void)
{
    return &_ret;
}

// =====================
// ==     Method      ==
// =====================

void Location::init_location(std::vector<std::string> &tokens)
{
    if (tokens.empty())
        throw ExecptionMissBrace();

    set_name(tokens[0]);
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

        init_location_allowed_methods(tokens);
        init_location_root(tokens);
        init_location_index(tokens);
        init_location_auto_index(tokens);
        init_location_client_max_body_size(tokens);
        init_locatoin_error_page(tokens);
        init_location_return(tokens);

        // Security to avoid inifite loop
        new_tokens_size = tokens.size();
        if (tokens_size == new_tokens_size)
            throw ExecptionWrongArgument(tokens[0]);
        tokens_size = new_tokens_size;
    }
    tokens.erase(tokens.begin());
}

void Location::init_location_allowed_methods(std::vector<std::string> &tokens)
{
    if (tokens[0] == "allowed_methods")
    {

        tokens.erase(tokens.begin());

        while (tokens[0] != ";")
        {
            if (tokens[0] == "GET")
                add_methode_http(GET);
            else if (tokens[0] == "POST")
                add_methode_http(POST);
            else if (tokens[0] == "DELETE")
                add_methode_http(DELETE);
            else if (tokens[0] == "HEAD")
                add_methode_http(HEAD);
            else
                throw ExecptionIllegalMethod(tokens[0]);

            tokens.erase(tokens.begin());
        }
        tokens.erase(tokens.begin());
    }
}

void Location::init_location_root(std::vector<std::string> &tokens)
{
    std::string root = return_root(tokens);
    if (root != "")
        set_root(root);
}

void Location::init_location_index(std::vector<std::string> &tokens)
{
    if (tokens[0] == "index")
    {
        tokens.erase(tokens.begin());
        set_index(tokens[0]);

        tokens.erase(tokens.begin());

        if (tokens[0] != ";")
            throw ExecptionMissSemiColon();

        tokens.erase(tokens.begin());
    }
}

void Location::init_location_auto_index(std::vector<std::string> &tokens)
{
    int auto_index = return_auto_index(tokens);

    if (auto_index == 0)
        set_auto_index(false);
    else if (auto_index == 1)
        set_auto_index(true);
}

void Location::init_location_client_max_body_size(std::vector<std::string> &tokens)
{
    unsigned int client_max_body_size = return_client_max_body_size(tokens);

    if (client_max_body_size != 0)
        set_client_max_body_size(client_max_body_size);
}

void Location::init_locatoin_error_page(std::vector<std::string> &tokens)
{
    bool is_init = false;
    s_return error_page = return_error_page(tokens, is_init);

    if (is_init)
        set_error_page(error_page);
}

void Location::init_location_return(std::vector<std::string> &tokens)
{
    bool is_init = false;
    s_return ret = return_return(tokens, is_init);

    if (is_init)
        set_return(ret);
}