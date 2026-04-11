/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 17:09:20 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/11 18:14:55 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "struct.hpp"
# include "server.hpp"

# define MAX_CLIENT 10
# define MAX_EVENTS 10
# define SIZE_BUFFER 1024

class Webserv
{
    private:
        std::vector<Server> _vector_server;
        std::vector<int> _vector_client_fd;
        
    public:

        // =====================
        // == Canonical Form  ==
        // =====================
        
        Webserv();
        ~Webserv();
        Webserv(const Webserv &other);
        Webserv& operator= (const Webserv &other);


        // =====================
        // == Getter & Setter ==
        // =====================
        
        //SERVERS
        Server* get_server(size_t i);
        size_t get_servers_count(void);
        std::vector<Server>* get_all_servers(void);
        
        
        // =====================
        // ==     Method      ==
        // =====================
        
        bool initialisation_webserv(std::vector <std::string> &tokens);
        bool splitServers(std::vector <std::string> &tokens);

        bool initialisation_connection();
        void initialisation_socket(int epoll_fd, int serverSocket);
        void webserv_listen(int epoll_fd, int serverSocket);
        void manage_connection(int epoll_fd, int serverSocket, int event_fd);
        void close_connection(int serverSocket, int epoll_fd);
};