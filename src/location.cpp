/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/06 15:45:46 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/06 20:28:28 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "location.hpp"

// =====================
// == Canonical Form  ==
// =====================

Location::Location():
    _name(""),
    _allowed_methods(),
    _root(""),
    _index(""),
    _auto_index(false),
    _error_page(s_return()),
    _client_max_body_size(0),
    _ret(s_return())
    {}

Location::~Location() {}

Location::Location(const Location &other) {*this = other;}

Location& Location::operator= (const Location &other)
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

//NAME
void Location::set_name(std::string name) {_name = name;}
std::string Location::get_name(void) {return _name;}

//METHOD-HTTP
void Location::add_methode_http(method_http i) {_allowed_methods.insert(i);}
std::set<method_http> Location::get_methode_http(void) {return _allowed_methods;}

//INDEX
void Location::set_index(std::string index) {_index = index;}
std::string Location::get_index(void) {return _index;}
        
//ROOT
void Location::set_root(std::string root) {_root = root;}
std::string Location::get_root(void) {return _root;}

//AUTO-INDEX
void Location::set_auto_index(bool auto_index) {_auto_index = auto_index;}
bool Location::get_auto_index(void) {return _auto_index;}

//CLIENT-MAX-BODY-SIZE
void Location::set_client_max_body_size(unsigned int client_max_body_size) {_client_max_body_size = client_max_body_size;}
unsigned int Location::get_client_max_body_size(void) {return _client_max_body_size;}

//ERROR-PAGE
void Location::set_error_page(s_return error_page) {_error_page = error_page;}
s_return* Location::get_error_page(void) {return &_error_page;}

//RETURN
void Location::set_return(s_return ret) {_ret = ret;}
s_return* Location::get_return(void) {return &_ret;}

// =====================
// ==     Method      ==
// =====================

void Location::init_location(std::vector <std::string> &tokens)
{   
    set_name(tokens[0]);
    
    //If miss bracket raise execption
    tokens.erase(tokens.begin());
    tokens.erase(tokens.begin());
    while (tokens[0] != "}")
    {
        init_location_allowed_methods(tokens);
        init_location_root(tokens);
        init_location_index(tokens);
        init_location_auto_index(tokens);
        init_location_client_max_body_size(tokens);
        init_locatoin_error_page(tokens);
        init_location_return(tokens);

        //Add security to avoid inifite loop
    }
    tokens.erase(tokens.begin());
}

void Location::init_location_allowed_methods(std::vector <std::string> &tokens)
{
    if (tokens[0] == "allowed_methods")
    {

        tokens.erase(tokens.begin());
        
        while (tokens[0] != ";")
        {
            if (tokens[0] == "GET") add_methode_http(GET);
            else if (tokens[0] == "POST") add_methode_http(POST);
            else if (tokens[0] == "DELETE") add_methode_http(DELETE);
            else if (tokens[0] == "HEAD") add_methode_http(HEAD);
            else
            {
                // raise erro
            }
    
            tokens.erase(tokens.begin());
        }

        tokens.erase(tokens.begin());
    }
}

void Location::init_location_root(std::vector <std::string> &tokens)
{
    if (tokens[0] == "root")
    {

        tokens.erase(tokens.begin());
        set_root(tokens[0]);

        tokens.erase(tokens.begin());

        tokens.erase(tokens.begin());
    }
}

void Location::init_location_index(std::vector <std::string> &tokens)
{
    if (tokens[0] == "index")
    {

        tokens.erase(tokens.begin());
        set_index(tokens[0]);

        tokens.erase(tokens.begin());
        
        //If different than semi colon raise execption

        tokens.erase(tokens.begin());
    }
}

bool Location::init_location_auto_index(std::vector <std::string> &tokens)
{   
    if (tokens[0] == "autoindex")
    {

        tokens.erase(tokens.begin());
        if (tokens[0] == "on" || tokens[0] == "true")
        {

    
            tokens.erase(tokens.begin());
            set_auto_index(true);
            
            //If different than semi colon raise execption
    
            tokens.erase(tokens.begin());
            return false;
        }  
        else if (tokens[0] == "off" || tokens[0] == "false")
        {
            
    
            tokens.erase(tokens.begin());
            set_auto_index(false);
            
            //If different than semi colon raise execption
    
            tokens.erase(tokens.begin());
            return false;
        }
        else
        {
            std::cerr << "Error: Wrong argument for auto_index\n";
            return true;
        }
    }
    return false;
}

bool Location::init_location_client_max_body_size(std::vector <std::string> &tokens)
{
    unsigned int client_max_body_size;
    
    if (tokens[0] == "client_max_body_size")
    {
        tokens.erase(tokens.begin());
        
        std::istringstream convert(tokens[0]);
        convert >> client_max_body_size;
        
        if (convert.fail())
        {
            std::cerr << "Error: Can't convert (" << tokens[0] << ") to unsigned int\n";
            return (true);
        }
        
        set_client_max_body_size(client_max_body_size);
        tokens.erase(tokens.begin());

        //If different than semi colon raise execption
        tokens.erase(tokens.begin());
    }
    return false;
}

bool Location::init_locatoin_error_page(std::vector <std::string> &tokens)
{
    if (tokens[0] == "error_page")
    {
        s_return error_page;

        tokens.erase(tokens.begin());
        
        std::istringstream convert(tokens[0]);
        convert >> error_page.code;

        tokens.erase(tokens.begin());
         
        if (convert.fail())
        {
            std::cerr << "Error: Can't convert (" << tokens[0] << ") to unsigned int\n";
            return (true);
        }

        error_page.value = tokens[0];
        set_error_page(error_page);
        tokens.erase(tokens.begin());

        //If different than semi colon raise execptiom
        tokens.erase(tokens.begin());
        return false;
    }
    return false;
}

bool Location::init_location_return(std::vector <std::string> &tokens)
{
    if (tokens[0] == "return")
    {
        s_return ret;

        tokens.erase(tokens.begin());
        
        std::istringstream convert(tokens[0]);
        convert >> ret.code;

        tokens.erase(tokens.begin());
         
        if (convert.fail())
        {
            std::cerr << "Error: Can't convert (" << tokens[0] << ") to unsigned int\n";
            return (true);
        }

        ret.value = tokens[0];
        set_return(ret);

        tokens.erase(tokens.begin());
        //If different than semi colon raise execption

        tokens.erase(tokens.begin());
        return false;
    }
    return false;
}