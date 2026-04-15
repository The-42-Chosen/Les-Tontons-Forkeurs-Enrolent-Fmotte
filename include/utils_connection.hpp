/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_connection.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 14:55:35 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/15 15:17:32 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "struct.hpp"
#include "Client.hpp"

extern int stop_webserv;

void handle_sigint(int sig);
void init_signal(struct sigaction &sa);
int set_nonblocking(int fd);

sockaddr_in create_socket_adrress(std::string ip_address, unsigned int port_number);
void add_socket_to_event(int epoll_fd, int socket_fd, Client* client);
int create_server_socket(std::string ip_address, unsigned int port_number, unsigned int max_client);