/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/04 14:50:27 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/28 12:21:11 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// =====================
// == Canonical Form  ==
// =====================

Server::Server()
    : _listens(0), _name_servers(0), _locations(0), _root(""), _index_files(0), _auto_index(false), _error_page(0),
      _client_max_body_size(0), _ret(HttpReturn())
{
}

Server::Server(const Webserv *)
    : _listens(0), _name_servers(0), _locations(0), _root(""), _index_files(0), _auto_index(false), _error_page(0),
      _client_max_body_size(0), _ret(HttpReturn())
{
}

Server::~Server()
{
}

Server::Server(const Server &other)
{
    *this = other;
}

Server &Server::operator=(const Server &other)
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

// NAME-SERVERS
void Server::addNameServer(std::string name)
{
    _name_servers.push_back(name);
}

std::string Server::getNameServer(size_t i)
{
    if (i >= _name_servers.size())
        return "";
    else
        return _name_servers[i];
}

// LISTEN
void Server::addListen(Listen listen)
{
    _listens.push_back(listen);
}

Listen *Server::getListen(size_t i)
{
    if (i >= _listens.size())
        return NULL;
    else
        return &_listens[i];
}

// LOCATION
void Server::addLocation(Location &location)
{
    _locations.push_back(location);
}
Location *Server::getLocation(size_t i)
{
    if (i < _locations.size())
        return &_locations[i];
    else
        return NULL;
}

// ROOT
void Server::setRoot(std::string root)
{
    _root = root;
}
std::string Server::getRoot(void)
{
    return _root;
}

// INDEX
void Server::addIndex(std::string index)
{
    _index_files.push_back(index);
}
std::string Server::getIndex(size_t i)
{
    if (i < _index_files.size())
        return _index_files[i];
    else
        return "";
}

// AUTO-INDEX
void Server::setAutoIndex(bool auto_index)
{
    _auto_index = auto_index;
}
bool Server::getAutoIndex(void)
{
    return _auto_index;
}

// ERROR-PAGE
void Server::addErrorPage(HttpReturn error_page)
{
    _error_page.push_back(error_page);
}

HttpReturn *Server::getErrorPage(size_t i)
{
    if (i < _error_page.size())
        return &_error_page[i];
    else
        return NULL;
}

// CLIENT-MAX-BODY-SIZE
void Server::setClientMaxBodySize(unsigned int client_max_body_size)
{
    _client_max_body_size = client_max_body_size;
}
unsigned int Server::getClientMaxBodySize(void)
{
    return _client_max_body_size;
}

// RETURN
void Server::setReturn(HttpReturn ret)
{
    _ret = ret;
}
HttpReturn *Server::getReturn(void)
{
    return &_ret;
}

// =====================
// ==     Method      ==
// =====================

// bool Server::initialisatio

void Server::initializeServer(std::vector<std::string> &tokens)
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

        initializeListens(tokens);
        initializeNameServers(tokens);
        initializeLocation(tokens);
        initializeRoot(tokens);
        initializeIndexFiles(tokens);
        initializeAutoIndex(tokens);
        initializeErrorPage(tokens);
        initializeClientMaxBodySize(tokens);
        initializeReturn(tokens);

        // Security to avoid inifite loop
        new_tokens_size = tokens.size();
        if (tokens_size == new_tokens_size)
            throw ExecptionWrongArgument(tokens[0]);
        tokens_size = new_tokens_size;
    }

    tokens.erase(tokens.begin());
}

void Server::initializeCheck()
{
    if (_listens.size() == 0)
        throw ExecptionMissElement("listen");

    if (_name_servers.size() == 0)
        throw ExecptionMissElement("server_name");

    if (_root == "")
        throw ExecptionMissElement("root");
}

void Server::initializeNameServers(std::vector<std::string> &tokens)
{
    if (tokens[0] == "server_name")
    {
        tokens.erase(tokens.begin());

        while (tokens[0] != ";")
        {
            addNameServer(tokens[0]);
            tokens.erase(tokens.begin());
        }
        tokens.erase(tokens.begin());
    }
}

void Server::initializeListens(std::vector<std::string> &tokens)
{
    char sep = ':';

    std::string sub_string;
    Listen listenAddr;

    listenAddr.ip = DEFAULT_IP;
    listenAddr.port = DEFAULT_PORT;

    if (tokens[0] == "listen")
    {
        tokens.erase(tokens.begin());

        if (tokens[0] == ";")
        {
            tokens.erase(tokens.begin());
            addListen(listenAddr);
            return;
        }
        std::stringstream iss(tokens[0]);
        tokens.erase(tokens.begin());

        while (getline(iss, sub_string, sep))
        {
            if (countOccurrences(sub_string, '.') == 3)
                listenAddr.ip = sub_string;
            else
            {
                std::istringstream convert(sub_string);
                convert >> listenAddr.port;

                if (convert.fail())
                    throw ExecptionFailConvertion(sub_string);
            }
        }

        if (tokens[0] != ";")
            throw ExecptionMissSemiColon();

        tokens.erase(tokens.begin());
        addListen(listenAddr);
    }
}

void Server::initializeLocation(std::vector<std::string> &tokens)
{
    if (tokens[0] == "location")
    {
        tokens.erase(tokens.begin());

        Location location;
        location.initializeLocation(tokens);
        addLocation(location);
    }
}

void Server::initializeIndexFiles(std::vector<std::string> &tokens)
{
    if (tokens[0] == "index")
    {
        tokens.erase(tokens.begin());
        while (tokens[0] != ";")
        {
            addIndex(tokens[0]);
            tokens.erase(tokens.begin());
        }

        if (tokens[0] != ";")
            throw ExecptionMissSemiColon();

        tokens.erase(tokens.begin());
    }
}

void Server::initializeRoot(std::vector<std::string> &tokens)
{
    std::string root = parseRootDirective(tokens);
    if (root != "")
        setRoot(root);
}

void Server::initializeAutoIndex(std::vector<std::string> &tokens)
{
    int auto_index = parseAutoIndexDirective(tokens);

    if (auto_index == 0)
        setAutoIndex(false);
    else if (auto_index == 1)
        setAutoIndex(true);
}

void Server::initializeErrorPage(std::vector<std::string> &tokens)
{
    bool is_init = false;
    HttpReturn error_page = parseErrorPageDirective(tokens, is_init);

    if (is_init)
        addErrorPage(error_page);
}

void Server::initializeClientMaxBodySize(std::vector<std::string> &tokens)
{
    unsigned int client_max_body_size = parseClientMaxBodySizeDirective(tokens);

    if (client_max_body_size != 0)
        setClientMaxBodySize(client_max_body_size);
}

void Server::initializeReturn(std::vector<std::string> &tokens)
{
    bool is_init = false;
    HttpReturn ret = parseReturnDirective(tokens, is_init);

    if (is_init)
        setReturn(ret);
}
