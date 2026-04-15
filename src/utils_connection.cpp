/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_connection.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: erpascua <erpascua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 14:55:37 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/15 20:18:41 by erpascua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils_connection.hpp"
#include "execption.hpp"

void handle_sigint(int sig)
{
    (void)sig;
    stop_webserv = 1;
}

void init_signal(struct sigaction &sa)
{
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
}

int set_nonblocking(int fd)
{
    int flags;
    if ((flags = fcntl(fd, F_GETFL, 0)) == -1)
        throw ExecptionErrorFunction("fcntl");

    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

sockaddr_in create_socket_adrress(std::string ip_address, unsigned int port_number)
{
    sockaddr_in serverAddress;

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port_number);
    serverAddress.sin_addr.s_addr = inet_addr(ip_address.c_str());
    return serverAddress;
}

void add_socket_to_event(int epoll_fd, int socket_fd, Client *client)
{
    set_nonblocking(socket_fd);

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = socket_fd;
    if (client != NULL)
        ev.data.ptr = client;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev) == -1)
        throw ExecptionErrorFunction("epoll_ctl");
}

int create_server_socket(std::string ip_address, unsigned int port_number, unsigned int max_client)
{
    int serverSocket;

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        throw ExecptionErrorFunction("socket");
    
    sockaddr_in serverAddress = create_socket_adrress(ip_address, port_number);

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
        throw ExecptionErrorFunction("bind");

    if (listen(serverSocket, max_client) == -1)
        throw ExecptionErrorFunction("listen");

    return serverSocket;
}
