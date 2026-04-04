/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/04 14:50:27 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/04 20:23:51 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "server.hpp"


// =====================
// == Canonical Form  ==
// =====================
        
Server::Server(): 
    _listens(0), 
    _name_servers(0),
    _locations(0),
    _root(""),
    _index_files(0),
    _auto_index(false),
    _error_page(0),
    _client_max_body_size(0),
    _ret(NULL)
    {}
    
Server::~Server(){}

Server::Server(const Server &other){*this = other;}

Server& Server::operator= (const Server &other)
{
    this->_listens = other._listens;
    this->_name_servers = other._name_servers;
    this->_locations = other._locations;
    this->_root = other._root;
    this->_index_files = other._index_files;
    this->_auto_index = other._auto_index;
    this->_error_page = other._error_page;
    this->_client_max_body_size = other._client_max_body_size;
    this->_ret = other._ret;

    return (*this);
}

// =====================
// == Getter & Setter ==
// =====================

//NAME-SERVERS
void Server::add_name_server(std::string name) {_name_servers.push_back(name);}

std::string Server::get_name_server(size_t i)
{   
    if (i >= _name_servers.size())
        return "";
    else
        return _name_servers[i];
}

//LISTEN
void Server::add_listen(s_listen listen) {_listens.push_back(listen);}

s_listen* Server::get_listen(size_t i)
{
    if (i >= _listens.size())
        return NULL;
    else
        return &_listens[i];
}

//INDEX
void Server::add_index(std::string index) {_index_files.push_back(index);}
std::string Server::get_index(size_t i)
{
    if (i >= _index_files.size())
        return "";
    else
        return _index_files[i];
}
        
//ROOT
void Server::set_root(std::string root) {_root = root;}
std::string Server::get_root(void) {return _root;}

//AUTO-INDEX
void Server::set_auto_index(bool auto_index) {_auto_index = auto_index;}
bool Server::get_auto_index(void) {return _auto_index;}

//ERROR-PAGE
void Server::add_error_page(s_return error_page) {_error_page.push_back(error_page);}

s_return* Server::get_error_page(size_t i)
{
    if (i >= _error_page.size())
        return NULL;
    else
        return &_error_page[i];
}

//CLIENT-MAX-BODY-SIZE
void Server::set_client_max_body_size(unsigned int client_max_body_size) {_client_max_body_size = client_max_body_size;}
unsigned int Server::get_client_max_body_size(void) {return _client_max_body_size;}
        
// =====================
// ==     Method      ==
// =====================

void Server::initialisation_server(std::vector <std::string> tokens)
{
    size_t i = 0;
    
    //Big try encompasses the loop
    while (i < tokens.size())
    {
        std::cout << tokens[i] << '\n';
        initialisation_listens(tokens, i);
        initialisation_name_servers(tokens, i);
        initialisation_index_files(tokens, i);
        initialisation_root(tokens, i);
        
        initialisation_auto_index(tokens, i);
        initialisation_error_page(tokens, i);
        initialisation_client_max_body_size(tokens, i);
        ++i;
    }
    
    //Execpt
        //Clean Exit
    
    i = 0;
    std::string string;
    s_listen *listen;
    s_return *error_page;
    while ((string = get_name_server(i)) != "")
    {
        std::cout << "Server name: " << string << "\n";
        i++;
    }
    std::cout << "\n";
    
    i = 0;
    while ((string = get_index(i)) != "")
    {
        std::cout << "Index: " << string << "\n";
        i++;
    }
    std::cout << "\n";
    
    i = 0;
    while ((listen = get_listen(i)) != NULL)
    {
        std::cout << "\n";
        std::cout << "IP: " << listen->ip << "\n";
        std::cout << "Port: " << listen->port << "\n";
        i++;
    }

    i = 0;
    while ((error_page = get_error_page(i)) != NULL)
    {
        std::cout << "\n";
        std::cout << "Code: " << error_page->code << "\n";
        std::cout << "Value: " << error_page->value << "\n";
        i++;
    }
    std::cout << "\n";
    
    std::cout << "Root: " << get_root() << "\n";
    std::cout << "Autoindex: " << get_auto_index() << "\n";
    std::cout << "Client_max_body_size: " << get_client_max_body_size() << "\n";
}

void Server::initialisation_name_servers(std::vector <std::string> &tokens, size_t &i)
{
    if (tokens[i] == "server_name")
    {
        tokens.erase(tokens.begin() + i);
        while (*(tokens[i].end() - 1) != ';')
        {
            add_name_server(tokens[i]);
            tokens.erase(tokens.begin() + i);
        }
        add_name_server(tokens[i]);
        tokens.erase(tokens.begin() + i);
    }
}

unsigned int get_nb_occurence(const std::string &string, const char occ)
{
    unsigned int nb_occ = 0;
    unsigned int i = 0;

    while (string[i] != '\0')
    {
        if (string[i] == occ)
            ++nb_occ;
        ++i;
    }
    return nb_occ;
}

bool Server::initialisation_listens(std::vector <std::string> &tokens, size_t &i)
{
    char sep = ':';
    
    std::string sub_string;
    s_listen listen;
    
    listen.ip = "0.0.0.0";  //Put a macro for the default port
    listen.port = 0;        //Same for the port
    
    if (tokens[i] == "listen")
    {
        tokens.erase(tokens.begin() + i);
        std::stringstream iss(tokens[i]);
        
        //remove semi colon at the end if
        while (getline(iss, sub_string, sep))
        {
            if (get_nb_occurence(sub_string, '.') == 3)
                listen.ip = sub_string;
            else
            {
                std::istringstream convert(sub_string);
                convert >> listen.port;
                
                if (convert.fail())
                {
                    std::cerr << "Error: Can't convert (" << sub_string << ") to unsigned int\n";
                    return (true);
                }
            }
        }
        add_listen(listen);
    }
    return (false);
}

void Server::initialisation_index_files(std::vector <std::string> &tokens, size_t &i)
{
    if (tokens[i] == "index")
    {
        tokens.erase(tokens.begin() + i);
        while (*(tokens[i].end() - 1) != ';')
        {
            add_index(tokens[i]);
            tokens.erase(tokens.begin() + i);
        }
        add_index(tokens[i]);
        tokens.erase(tokens.begin() + i);
    }
}

void Server::initialisation_root(std::vector <std::string> &tokens, size_t &i)
{
    if (tokens[i] == "root")
    {
        tokens.erase(tokens.begin() + i);
        set_root(tokens[i]);
        tokens.erase(tokens.begin() + i);
    }
}

bool Server::initialisation_auto_index(std::vector <std::string> &tokens, size_t &i)
{
    if (tokens[i] == "autoindex")
    {
        tokens.erase(tokens.begin() + i);
        if (tokens[i] == "on;" || tokens[i] == "true;")
        {
            tokens.erase(tokens.begin() + i);
            set_auto_index(true);
            return false;
        }
            
        else if (tokens[i] == "off;" || tokens[i] == "false;")
        {
            tokens.erase(tokens.begin() + i);
            set_auto_index(false);
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

bool Server::initialisation_error_page(std::vector <std::string> &tokens, size_t &i)
{   
    s_return error_page;
    
    if (tokens[i] == "error_page")
    {
        tokens.erase(tokens.begin() + i);
        
        std::istringstream convert(tokens[i]);
        convert >> error_page.code;
        tokens.erase(tokens.begin() + i);
         
        if (convert.fail())
        {
            std::cerr << "Error: Can't convert (" << tokens[i] << ") to unsigned int\n";
            return (true);
        }

        error_page.value = tokens[i];
        tokens.erase(tokens.begin() + i);
        
        add_error_page(error_page);
        return false;
    }
    return false;
}

bool Server::initialisation_client_max_body_size(std::vector <std::string> &tokens, size_t &i)
{   
    unsigned int client_max_body_size;
    
    if (tokens[i] == "client_max_body_size")
    {
        tokens.erase(tokens.begin() + i);
        
        std::istringstream convert(tokens[i]);
        convert >> client_max_body_size;
        
        if (convert.fail())
        {
            std::cerr << "Error: Can't convert (" << tokens[i] << ") to unsigned int\n";
            return (true);
        }
        
        set_client_max_body_size(client_max_body_size);
        tokens.erase(tokens.begin() + i);
    }
    return false;
}