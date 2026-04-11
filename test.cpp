#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int server_fd, client_socket;
    struct sockaddr_in address, client_addr;
    socklen_t addrlen = sizeof(client_addr);
    char buffer[1024];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(9999);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind");
        return 1;
    }

    listen(server_fd, 5);
    std::cout << "Serveur en attente..." << std::endl;

    while (true) {
        client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &addrlen);
        if (client_socket < 0) {
            perror("accept");
            continue;
        }

        std::cout << "Client connecté !" << std::endl;

        memset(buffer, 0, sizeof(buffer));
        int valread = read(client_socket, buffer, 1023);

        if (valread > 0) {
            buffer[valread] = '\0';
            std::cout << "Message reçu: " << buffer << std::endl;
        }

        const char* reply = "Message received";
        send(client_socket, reply, strlen(reply), 0);

        close(client_socket);
    }

    return 0;
}