#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Création du socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Configuration de l'adresse
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("10.16.4.14");
    address.sin_port = htons(9999);

    // Bind
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));

    std::cout << "Serveur en attente..." << std::endl;

    // Listen
    listen(server_fd, 5);

    while (true) {
        // Accept
        client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        std::cout << "Client connecté !" << std::endl;

        // Receive
        read(client_socket, buffer, 1024);
        std::cout << "Message reçu: " << buffer << std::endl;

        // Send
        const char* reply = "Message received";
        send(client_socket, reply, strlen(reply), 0);

        // Close
        close(client_socket);
    }

    return 0;
}
