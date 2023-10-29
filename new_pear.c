#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>

#define PORT 12345 // Change this to your desired port number
#define MAX_CLIENTS 10

void handleClient(int clientSocket) {
    char buffer[1024];
    ssize_t bytesRead;

    while (1) {
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            break;
        }

        buffer[bytesRead] = '\0';
        printf("Received: %s", buffer);

        // You can process the received data here.

        send(clientSocket, buffer, bytesRead, 0); // Echo back to the sender
    }

    close(clientSocket);
}

int main() {
    int serverSocket, clientSocket, maxSocket;
    fd_set readfds;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    // Create a socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Set up the server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the server address
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Socket binding failed");
        close(serverSocket);
        exit(1);
    }

    // Start listening for incoming connections
    if (listen(serverSocket, MAX_CLIENTS) == -1) {
        perror("Socket listening failed");
        close(serverSocket);
        exit(1);
    }

    printf("Server is listening on port %d...\n", PORT);

    FD_ZERO(&readfds);
    FD_SET(serverSocket, &readfds);
    maxSocket = serverSocket;

    while (1) {
        fd_set tempSet = readfds;
        if (select(maxSocket + 1, &tempSet, NULL, NULL, NULL) == -1) {
            perror("Select failed");
            break;
        }

        for (int i = 0; i <= maxSocket; i++) {
            if (FD_ISSET(i, &tempSet)) {
                if (i == serverSocket) {
                    // New client connection
                    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrLen);
                    if (clientSocket == -1) {
                        perror("Client connection failed");
                        continue;
                    }

                    printf("Connection accepted from %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port);

                    FD_SET(clientSocket, &readfds);
                    if (clientSocket > maxSocket) {
                        maxSocket = clientSocket;
                    }
                } else {
                    // Data from a connected client
                    handleClient(i);
                    FD_CLR(i, &readfds);
                }
            }
        }
    }

    close(serverSocket);

    return 0;
}
