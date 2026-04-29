/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilsConnection.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 14:55:37 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/23 13:35:39 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utilsConnection.hpp"
#include "execption.hpp"

void handleSigint(int sig)
{
    (void)sig;
    stop_webserv = 1;
}

void initializeSignal(struct sigaction &sa)
{
    sa.sa_handler = handleSigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
}

int setNonblocking(int fd)
{
    int flags;
    if ((flags = fcntl(fd, F_GETFL, 0)) == -1)
        throw ExecptionErrorFunction("fcntl");

    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

sockaddr_in createSocketAddress(std::string ip_address, unsigned int port_number)
{
    sockaddr_in serverAddress;

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port_number);
    serverAddress.sin_addr.s_addr = inet_addr(ip_address.c_str());
    return serverAddress;
}

void addSocketToEvent(int epoll_fd, int socket_fd, Client *client)
{
    setNonblocking(socket_fd);

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = socket_fd;
    if (client != NULL)
        ev.data.ptr = client;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev) == -1)
        throw ExecptionErrorFunction("epoll_ctl");
}

int createServerSocket(std::string ip_address, unsigned int port_number, unsigned int max_client)
{
    int serverSocket;

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        throw ExecptionErrorFunction("socket");

    sockaddr_in serverAddress = createSocketAddress(ip_address, port_number);

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
        throw ExecptionErrorFunction("bind");

    if (listen(serverSocket, max_client) == -1)
        throw ExecptionErrorFunction("listen");

    return serverSocket;
}
