/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test2.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmotte <fmotte@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/09 16:13:50 by fmotte            #+#    #+#             */
/*   Updated: 2026/04/10 16:45:14 by fmotte           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <sys/socket.h>
# include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>

# include <string>
# include <iostream>

#include <unistd.h>
#include <signal.h>

extern int stop_webserv;
int stop_webserv = 0;
void handle_sigint(int sig){stop_webserv = 1;}

void init_signal(struct sigaction &sa)
{
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
}

int set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

sockaddr_in create_socket_adrress(std::string ip_address, unsigned int port_number)
{
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;        // AF_INET : IPv4 protocol
    serverAddress.sin_port = htons(port_number);
    serverAddress.sin_addr.s_addr = inet_addr(ip_address.c_str());;
    return serverAddress;
}

void add_socket_to_event(struct epoll_event &ev, int epoll_fd, int socket_fd)
{
    ev.events = EPOLLIN;
    ev.data.fd = socket_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev); //return  -1 if error
}

void get_new_client(struct epoll_event &ev, int epoll_fd, int server_fd)
{
    int clientSocket = accept(server_fd, nullptr, nullptr); //return  -1 if error
    set_nonblocking(clientSocket);
    add_socket_to_event(ev, epoll_fd, clientSocket);
    std::cout << "Nouveau client connecté: fd="<< clientSocket << "\n";
}

int get_message_from_client(int clientSocket, unsigned int size_buffer)
{
    char buffer[size_buffer];
    int bytes = recv(clientSocket, buffer, sizeof(buffer), 0); //return  -1 if error
    buffer[bytes] = '\0';
    
    if (bytes <= 0)
        close(clientSocket); // Client déconnecté
    else
        std::cout << "Message from client: " << buffer << "\n";
        
    return bytes;
}

void create_server_socket(std::string ip_address, unsigned int port_number, int serverSocket, unsigned int max_client)
{
    sockaddr_in serverAddress = create_socket_adrress(ip_address, port_number);
    bind(serverSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress)); //return  -1 if error
    listen(serverSocket, max_client); //return  -1 if error
    std::cout << "Serveur en attente..." << std::endl;
}

int main(void)
{   
    unsigned int port_number = 8080;
    std::string ip_address = "0.0.0.0";
    unsigned int max_client = 10;
    unsigned int max_event = 10;
    unsigned int size_buffer = 1024;
    
    std::string reply = "Message received\n";
        
    struct sigaction sa;
    
    //LOOP for each listen
        int serverSocket = socket(AF_INET, SOCK_STREAM, 0); //return  -1 if error
        // AF_INET : IPv4 protocol
        // SOCK_STREAM: TCP socket
        

        create_server_socket(ip_address, port_number, serverSocket, max_client);
        set_nonblocking(serverSocket);
        
        //Create Epoll
        int epoll_fd = epoll_create(1); //return  -1 if error
        struct epoll_event ev, events[max_event];
        add_socket_to_event(ev, epoll_fd, serverSocket);
    
    init_signal(sa);
    sigaction(SIGINT, &sa, NULL);
    
    while (1)
    {
        int nfds = epoll_wait(epoll_fd, events, max_event, -1); //return  -1 if error
        
        if (stop_webserv)
            return 0;
            
        for (unsigned int i = 0; i < nfds; ++i)
        {
            //New Client
            if (events[i].data.fd == serverSocket)
                get_new_client(ev, epoll_fd, serverSocket);
                
            else
            {
                int clientSocket = events[i].data.fd;
                int bytes = get_message_from_client(clientSocket, size_buffer);
                
                if (bytes)
                    send(clientSocket, reply.c_str(), reply.size(), 0); 
                
            } 
        }
    }
    
    close(serverSocket);

    return 0;
}