/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilsConnection.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 14:55:35 by fmotte            #+#    #+#             */
/*   Updated: 2026/06/18 14:45:57 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"
#include "struct.hpp"

extern int stop_webserv;

class Client;

void handleSigint(int sig);
void initializeSignal(void);
int setNonblocking(int fd);

void addFdToEvent(int epoll_fd, int socket_fd, uint32_t event);
sockaddr_in createSocketAddress(std::string ip_address, unsigned int port_number);
int createServerSocket(std::string ip_address, unsigned int port_number, unsigned int max_client);

// =====================
// ==    TEMPLATE     ==
// =====================

template <typename PTR> 
void addFdToEvent(int epoll_fd, int socket_fd, uint32_t event, PTR *ptr)
{
    setNonblocking(socket_fd);

    struct epoll_event ev;
    ev.events = event;

    EventData *eventData = new EventData;
    eventData->ptr = ptr;
    eventData->fd = socket_fd;
    ev.data.ptr = eventData;
    
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev) == -1)
        throw ExecptionErrorFunction("epoll_ctl");
}