/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_connection.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 14:55:35 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/17 16:29:05 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"
#include "struct.hpp"

extern int stop_webserv;

class Client;

void handleSigint(int sig);
void initializeSignal(struct sigaction &sa);
int setNonblocking(int fd);

sockaddr_in createSocketAddress(std::string ip_address, unsigned int port_number);
void addSocketToEvent(int epoll_fd, int socket_fd, Client *client);
int createServerSocket(std::string ip_address, unsigned int port_number, unsigned int max_client);
