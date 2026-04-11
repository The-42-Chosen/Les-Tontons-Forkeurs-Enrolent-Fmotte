/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/04 14:50:27 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/11 16:01:32 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "server.hpp"
# include "utils.hpp"
# include "execption.hpp"
# include "utils_connection.hpp"

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
    _client_max_body_size(DEFAULT_CLIENT_MAX_BODY_SIZE),
    _ret(s_return())
    {}
    
Server::~Server() {}

Server::Server(const Server &other) {*this = other;}

Server& Server::operator= (const Server &other)
{
    this->_listens = other._listens;
    this->_name_servers = other._name_servers;
    this->_locations = other._locations;
    this->_root = other._root;
    this->_index_files = other._index_files;
    this->_servers = other._servers;
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

//LOCATION
void Server::add_location(Location &location) {_locations.push_back(location);}
Location* Server::get_location(size_t i)
{
    if (i <_locations.size())
       return &_locations[i];
    else
        return NULL;
}
        
//ROOT
void Server::set_root(std::string root) {_root = root;}
std::string Server::get_root(void) {return _root;}

//INDEX
void Server::add_index(std::string index) {_index_files.push_back(index);}
std::string Server::get_index(size_t i)
{
    if (i <_index_files.size())
       return _index_files[i];
    else
        return "";
}

//AUTO-INDEX
void Server::set_auto_index(bool auto_index) {_auto_index = auto_index;}
bool Server::get_auto_index(void) {return _auto_index;}

//ERROR-PAGE
void Server::add_error_page(s_return error_page) {_error_page.push_back(error_page);}

s_return* Server::get_error_page(size_t i)
{
    if (i < _error_page.size())
        return &_error_page[i];
    else
        return NULL;
}

//CLIENT-MAX-BODY-SIZE
void Server::set_client_max_body_size(unsigned int client_max_body_size) {_client_max_body_size = client_max_body_size;}
unsigned int Server::get_client_max_body_size(void) {return _client_max_body_size;}

//RETURN
void Server::set_return(s_return ret) {_ret = ret;}
s_return* Server::get_return(void) {return &_ret;}

//SERVERS
Server* Server::get_server(size_t i)
{
    if (i < _servers.size())
        return &_servers[i];
    else
        return NULL;
}

size_t Server::get_servers_count(void)
{
    return _servers.size();
}

std::vector<Server>* Server::get_all_servers(void)
{
    return &_servers;
}

// =====================
// ==     Method      ==
// =====================

// bool Server::initialisatio

bool Server::initialisation_webserv(std::vector <std::string> &tokens)
{
    return splitServers(tokens);
}

bool Server::splitServers(std::vector <std::string> &tokens)
{
    _servers.clear();

    try
    {
        while (!tokens.empty())
        {
            Server server;
            server.initialisation_server(tokens);
            server.initialisation_check();
            _servers.push_back(server);
        }
        return (true);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return (false);
    }
}

void Server::initialisation_server(std::vector <std::string> &tokens)
{
    if (tokens.empty())
        throw ExecptionMissBrace();

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

        initialisation_listens(tokens);
        initialisation_name_servers(tokens);
        initialisation_location(tokens);
        initialisation_root(tokens);
        initialisation_index_files(tokens);
        initialisation_auto_index(tokens);
        initialisation_error_page(tokens);
        initialisation_client_max_body_size(tokens);
        initialisation_return(tokens);
        
        //Security to avoid inifite loop
        new_tokens_size = tokens.size();
        if (tokens_size == new_tokens_size)
            throw ExecptionWrongArgument(tokens[0]);
        tokens_size = new_tokens_size;
    }
    
    tokens.erase(tokens.begin());
    
    // size_t i = 0;
    // std::string string;
    // s_listen *listen;
    // s_return *error_page;
    // Location *loc;
    
    // std::cout << "\nNEW SERVER:\n";
    // while ((string = get_name_server(i)) != "")
    // {
    //     std::cout << "Server name: " << string << "\n";
    //     i++;
    // }
    // std::cout << "\n";
    
    // i = 0;
    // while ((string = get_index(i)) != "")
    // {
    //     std::cout << "Index: " << string << "\n";
    //     i++;
    // }
    // std::cout << "\n";
    
    // i = 0;
    // while ((listen = get_listen(i)) != NULL)
    // {
    //     std::cout << "\n";
    //     std::cout << "IP: " << listen->ip << "\n";
    //     std::cout << "Port: " << listen->port << "\n";
    //     i++;
    // }

    // i = 0;
    // while ((loc = get_location(i)) != NULL)
    // {
    //     std::cout << "\nLocation\n";
    //     std::cout << "Name " <<loc->get_name() << "\n";
        
    //     if (! loc->get_methode_http().empty())
    //         std::cout << "Method " << *loc->get_methode_http().begin() << "\n";
    //     else
    //         std::cout << "Method  NULL\n";
            
    //     std::cout << "ROOT " << loc->get_root() << "\n";
    //     std::cout << "Index " << loc->get_index() << "\n";
    //     std::cout << "Auto " << loc->get_auto_index() << "\n";
        
    //     if (loc->get_error_page() != NULL)
    //     {
    //         std::cout << "ERROR code " << loc->get_error_page()->code << "\n";
    //         std::cout << "ERROR value " << loc->get_error_page()->value << "\n";
    //     }
        
    //     std::cout << "CLIENT " << loc->get_client_max_body_size() << "\n";
            
    //     if (loc->get_return() != NULL)
    //     {
    //         std::cout << "RETURN code " << loc->get_return()->code << "\n";
    //         std::cout << "RETURN value " << loc->get_return()->value << "\n";
    //     }
    //     i++;
    // }
    
    // i = 0;
    // while ((error_page = get_error_page(i)) != NULL)
    // {
    //     std::cout << "\nError Page\n";
    //     std::cout << "Code: " << error_page->code << "\n";
    //     std::cout << "Value: " << error_page->value << "\n";
    //     i++;
    // }
    
    // std::cout << "\n";
    // std::cout << "Root: " << get_root() << "\n";
    // std::cout << "Autoindex: " << get_auto_index() << "\n";
    // std::cout << "Client_max_body_size: " << get_client_max_body_size() << "\n";
    
    
    // if (get_return() != NULL)
    // {
    //     std::cout << "\nReturn\n";
    //     std::cout << "Code: " << get_return()->code << "\n";
    //     std::cout << "Value: " << get_return()->value << "\n";  
    // }
}

void Server::initialisation_check()
{
    if (_listens.size() == 0)
        throw ExecptionMissElement("listen");
    
    if (_name_servers.size() == 0)
        throw ExecptionMissElement("server_name");
        
    if (_root == "")
        throw ExecptionMissElement("root");
}

void Server::initialisation_name_servers(std::vector <std::string> &tokens)
{
    if (tokens[0] == "server_name")
    {
        tokens.erase(tokens.begin());

        while (tokens[0] != ";")
        {
            add_name_server(tokens[0]);
            tokens.erase(tokens.begin());
        }
        tokens.erase(tokens.begin());
    }
}

void Server::initialisation_listens(std::vector <std::string> &tokens)
{
    char sep = ':';
    
    std::string sub_string;
    s_listen listen;
    
    listen.ip = DEFAULT_IP;
    listen.port = DEFAULT_PORT;
    
    if (tokens[0] == "listen")
    {
        tokens.erase(tokens.begin());
        
		if (tokens[0] == ";")
		{
			tokens.erase(tokens.begin());    
        	add_listen(listen);
			return ;
		}
        std::stringstream iss(tokens[0]);
        tokens.erase(tokens.begin());
                
        while (getline(iss, sub_string, sep))
        {
            if (get_nb_occurence(sub_string, '.') == 3)
                listen.ip = sub_string;
            else
            {
                std::istringstream convert(sub_string);
                convert >> listen.port;
                
                if (convert.fail())
                    throw ExecptionFailConvertion(sub_string);
            }
        }
        
        if (tokens[0] != ";")
            throw ExecptionMissSemiColon();
            
        tokens.erase(tokens.begin());    
        add_listen(listen);
    }
}

void Server::initialisation_location(std::vector <std::string> &tokens)
{   
    if (tokens[0] == "location")
    {
        tokens.erase(tokens.begin());
        
        Location location;
        location.init_location(tokens);
        add_location(location);
    }
}

void Server::initialisation_index_files(std::vector <std::string> &tokens)
{
    if (tokens[0] == "index")
    {
        tokens.erase(tokens.begin());
        while (tokens[0] != ";")
        {
            add_index(tokens[0]);
            tokens.erase(tokens.begin());
        }
        
        if (tokens[0] != ";")
            throw ExecptionMissSemiColon();
            
        tokens.erase(tokens.begin());
    }
}

void Server::initialisation_root(std::vector <std::string> &tokens)
{
    std::string root = return_root(tokens);
    if (root != "")
        set_root(root);
}

void Server::initialisation_auto_index(std::vector <std::string> &tokens)
{
    int auto_index = return_auto_index(tokens);
    
    if (auto_index == 0)
        set_auto_index(false);
    else if (auto_index == 1)
        set_auto_index(true);
}

void Server::initialisation_error_page(std::vector <std::string> &tokens)
{   
    bool is_init = false;
    s_return error_page = return_error_page(tokens, is_init);
    
    if (is_init)
        add_error_page(error_page);
}   

void Server::initialisation_client_max_body_size(std::vector <std::string> &tokens)
{   
    unsigned int client_max_body_size = return_client_max_body_size(tokens);

    if (client_max_body_size != 0)
        set_client_max_body_size(client_max_body_size);
}

void Server::initialisation_return(std::vector <std::string> &tokens)
{
    bool is_init = false;
    s_return ret = return_return(tokens, is_init);
    
    if (is_init)
        set_return(ret);
}

void initialisation_connection(std::vector <std::string> &tokens)
{
    
}