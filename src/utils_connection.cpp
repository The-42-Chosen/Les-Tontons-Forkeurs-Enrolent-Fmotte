/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_connection.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 14:55:37 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/11 18:44:35 by fmotte           ###   ########.fr       */
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
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

sockaddr_in create_socket_adrress(std::string ip_address, unsigned int port_number)
{
    sockaddr_in serverAddress;

    serverAddress.sin_family = AF_INET; // AF_INET : IPv4 protocol
    serverAddress.sin_port = htons(port_number);
    serverAddress.sin_addr.s_addr = inet_addr(ip_address.c_str());
    ;
    return serverAddress;
}

void add_socket_to_event(int epoll_fd, int socket_fd)
{
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = socket_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev);
    // if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev) == -1)
    //     throw ExecptionErrorFunction("epoll_ctl");
}

int get_new_client(int epoll_fd, int server_fd)
{
    int clientSocket;
    if ((clientSocket = accept(server_fd, NULL, NULL)) == -1)
        throw ExecptionErrorFunction("accept");

    set_nonblocking(clientSocket);
    add_socket_to_event(epoll_fd, clientSocket);
    std::cout << "Nouveau client connecté: fd=" << clientSocket << "\n";
    return clientSocket;
}

int get_message_from_client(int clientSocket, unsigned int size_buffer)
{
    int bytes;
    char buffer[size_buffer];

    if ((bytes = recv(clientSocket, buffer, sizeof(buffer), 0)) == -1)
        throw ExecptionErrorFunction("recv");

    buffer[bytes] = '\0';

    if (bytes == 0)
    {
        close(clientSocket); // Client déconnecté
        std::cout << "Client is disconnected\n";
    }
    else
        std::cout << "Message from client: " << buffer << "\n";

    return bytes;
}

void create_server_socket(std::string ip_address, unsigned int port_number, int serverSocket, unsigned int max_client)
{
    sockaddr_in serverAddress = create_socket_adrress(ip_address, port_number);

    bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    // if (bind(serverSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) == -1)
    //     throw ExecptionErrorFunction("bind");

    if (listen(serverSocket, max_client) == -1)
        throw ExecptionErrorFunction("listen");
}