/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilsConnection.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 14:55:37 by fmotte            #+#    #+#             */
/*   Updated: 2026/07/08 21:22:50 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utilsConnection.hpp"
#include "execption.hpp"

void handleSigint(int sig)
{
    (void)sig;
    stop_webserv = 1;
}

void initializeSignal(void)
{
	signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, handleSigint);
}

int setNonblocking(int fd)
{
    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
        throw ExecptionErrorFunction("fcntl");
    return 0;
}

sockaddr_in createSocketAddress(std::string ip_address, unsigned int port_number)
{
    sockaddr_in serverAddress;

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port_number);
    serverAddress.sin_addr.s_addr = inet_addr(ip_address.c_str()); // ❌ A SUPPR
    return serverAddress;

    // === 	❌ WAAAAAIIIT FCTION INTERDITE❌===
    // sockaddr_in        serverAddress;
    // struct addrinfo    hints;
    // struct addrinfo   *res;
    //
    // std::memset(&hints, 0, sizeof(hints));
    // hints.ai_family   = AF_INET;
    // hints.ai_socktype = SOCK_STREAM;
    //
    // if (getaddrinfo(ip_address.c_str(), NULL, &hints, &res) != 0)
    //     throw ExecptionErrorFunction("getaddrinfo");
    //
    // serverAddress = *(struct sockaddr_in *)res->ai_addr;
    // serverAddress.sin_port = htons(port_number);
    // freeaddrinfo(res);
    // return serverAddress;
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
