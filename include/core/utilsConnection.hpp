/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilsConnection.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 14:55:35 by fmotte            #+#    #+#             */
/*   Updated: 2026/07/22 12:50:10 by fmotte           ###   ########.fr       */
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

sockaddr_in createSocketAddress(std::string ip_address, unsigned int port_number);
int createServerSocket(std::string ip_address, unsigned int port_number, unsigned int max_client);
void removeFdFromEvent(EventData *eventData, int epoll_webserv);

template <typename PTR> void addFdToEvent(int epoll_fd, int socket_fd, uint32_t event, FdType type, PTR *ptr)
{
    setNonblocking(socket_fd);

    struct epoll_event ev;
    ev.events = event;

    EventData *eventData = new EventData;
    eventData->ptr = ptr;
    eventData->fd = socket_fd;
    eventData->type = type;
    ev.data.ptr = eventData;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev) == -1)
        throw ExecptionErrorFunction("epoll_ctl");
}