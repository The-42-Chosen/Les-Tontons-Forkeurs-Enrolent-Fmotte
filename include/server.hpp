/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/04 14:43:21 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/04 20:17:36 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "struct.hpp"

class Server
{
    private:
        
        std::vector<s_listen>       _listens;
        std::vector<std::string>    _name_servers;
        std::vector<s_location>     _locations;
        std::string                 _root;
        std::vector<std::string>    _index_files;

        bool _auto_index;
        std::vector<s_return>     _error_page;
        unsigned int               _client_max_body_size;
        s_return*                  _ret;
    
    public:

        // =====================
        // == Canonical Form  ==
        // =====================
        Server();
        ~Server();
        Server(const Server &other);
        Server& operator= (const Server &other);

        
        // =====================
        // == Getter & Setter ==
        // =====================
        
        //NAME-SERVERS
        void add_name_server(std::string name);
        std::string get_name_server(size_t i);
        
        //LISTEN
        void add_listen(s_listen listen);
        s_listen* get_listen(size_t i);
        
        //INDEX
        void add_index(std::string index);
        std::string get_index(size_t i);
        
        //ROOT
        void set_root(std::string root);
        std::string get_root(void);
        
        //AUTO-INDEX
        void set_auto_index(bool auto_index);
        bool get_auto_index(void);
        
        //ERROR-PAGE
        void add_error_page(s_return error_page);
        s_return* get_error_page(size_t i);
        
        //CLIENT-MAX-BODY-SIZE
        void set_client_max_body_size(unsigned int client_max_body_size);
        unsigned int get_client_max_body_size(void);
        
        // =====================
        // ==     Method      ==
        // =====================
        
        void initialisation_server(std::vector <std::string> tokens);
        
        bool initialisation_listens(std::vector <std::string> &tokens, size_t &i);
        void initialisation_name_servers(std::vector <std::string> &tokens, size_t &i);
        
        void initialisation_index_files(std::vector <std::string> &tokens, size_t &i);
        void initialisation_root(std::vector <std::string> &tokens, size_t &i);
        
        bool initialisation_auto_index(std::vector <std::string> &tokens, size_t &i);
        bool initialisation_error_page(std::vector <std::string> &tokens, size_t &i);
        bool initialisation_client_max_body_size(std::vector <std::string> &tokens, size_t &i);
};  


unsigned int get_nb_occurence(const std::string &string, const char occ);