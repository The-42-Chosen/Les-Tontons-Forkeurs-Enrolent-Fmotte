/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/04 14:43:21 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/23 13:32:58 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/epoll.h>

#include "Location.hpp"
#include "Webserv.hpp"
#include "execption.hpp"
#include "struct.hpp"
#include "utilsDuplicate.hpp"
#include "utilsParsing.hpp"

#define DEFAULT_IP "0.0.0.0"
#define DEFAULT_PORT 8080
#define DEFAULT_CLIENT_MAX_BODY_SIZE 5000

class Webserv;

class Server
{
  private:
    const Webserv *_webser;

    std::vector<Listen> _listens;
    std::vector<std::string> _name_servers;
    std::vector<Location> _locations;
    std::string _root;
    std::vector<std::string> _index_files;
    std::vector<Server> _servers;

    bool _auto_index;
    std::vector<HttpReturn> _error_page;
    unsigned int _client_max_body_size;
    HttpReturn _ret;

  public:
    // =====================
    // == Canonical Form  ==
    // =====================

    Server();
    Server(const Webserv *webser);
    ~Server();
    Server(const Server &other);
    Server &operator=(const Server &other);

    // =====================
    // == Getter & Setter ==
    // =====================

    // NAME-SERVERS
    void addNameServer(std::string name);
    std::string getNameServer(size_t i);

    // LISTEN
    void addListen(Listen listen);
    Listen *getListen(size_t i);

    // LOCATION
    void addLocation(Location &location);
    Location *getLocation(size_t i);

    // ROOT
    void setRoot(std::string root);
    std::string getRoot(void);

    // INDEX
    void addIndex(std::string index);
    std::string getIndex(size_t i);

    // AUTO-INDEX
    void setAutoIndex(bool auto_index);
    bool getAutoIndex(void);

    // ERROR-PAGE
    void addErrorPage(HttpReturn error_page);
    HttpReturn *getErrorPage(size_t i);

    // CLIENT-MAX-BODY-SIZE
    void setClientMaxBodySize(unsigned int client_max_body_size);
    unsigned int getClientMaxBodySize(void);

    // RETURN
    void setReturn(HttpReturn ret);
    HttpReturn *getReturn(void);

    // =====================
    // ==     Method      ==
    // =====================

    void initializeServer(std::vector<std::string> &tokens);
    void initializeCheck();

    void initializeListens(std::vector<std::string> &tokens);
    void initializeNameServers(std::vector<std::string> &tokens);
    void initializeLocation(std::vector<std::string> &tokens);

    void initializeRoot(std::vector<std::string> &tokens);

    void initializeIndexFiles(std::vector<std::string> &tokens);
    void initializeAutoIndex(std::vector<std::string> &tokens);

    void initializeErrorPage(std::vector<std::string> &tokens);
    void initializeClientMaxBodySize(std::vector<std::string> &tokens);
    void initializeReturn(std::vector<std::string> &tokens);
};
