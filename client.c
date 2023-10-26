#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int client_socket;
    struct sockaddr_in server_addr;

    // Create a socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error in socket");
        exit(1);
    }

    // Configure the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080); // Port number (must match the server's port)
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server's IP address

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error in connect");
        exit(1);
    }

    printf("Connected to the server.\n");

    // Close the client socket
    close(client_socket);

    return 0;
}
