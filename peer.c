#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

int main() {
    // Create a socket
    int peer_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (peer_socket == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Set SO_REUSEADDR option
    int opt = 1;
    if (setsockopt(peer_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1) {
        perror("Setsockopt failed");
        exit(1);
    }

    // Bind the socket to an IP address and port
    struct sockaddr_in peer_address;
    peer_address.sin_family = AF_INET;
    peer_address.sin_port = htons(8080); // Port for peer1
    peer_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(peer_socket, (struct sockaddr*)&peer_address, sizeof(peer_address)) == -1) {
        perror("Binding failed");
        exit(1);
    }

    printf("Peer 1 is listening...\n");

    // Listen for incoming connections
    if (listen(peer_socket, 5) == -1) {
        perror("Listening failed");
        exit(1);
    }

    fd_set read_fds;
    struct timeval timeout;
    int max_fd = peer_socket + 1;

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(peer_socket, &read_fds);
        FD_SET(STDIN_FILENO, &read_fds);

        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int ready_fds = select(max_fd, &read_fds, NULL, NULL, &timeout);

        if (ready_fds < 0) {
            perror("Select failed");
            exit(1);
        } else if (ready_fds > 0) {
            if (FD_ISSET(peer_socket, &read_fds)) {
                // Accept incoming connections from other peers
                int incoming_socket = accept(peer_socket, NULL, NULL);
                if (incoming_socket == -1) {
                    perror("Acceptance failed");
                    exit(1);
                }

                char message[256];
                memset(message, 0, sizeof(message));

                // Receive a message from Peer 2
                if (recv(incoming_socket, message, sizeof(message), 0) == -1) {
                    perror("Receive failed");
                    exit(1);
                }

                printf("Peer 1 received from Peer 2: %s", message);

                // Send a response message to Peer 2
                printf("Enter a message to send to Peer 2: ");
                fgets(message, sizeof(message), stdin);
                send(incoming_socket, message, strlen(message), 0);

                // Close the incoming socket
                close(incoming_socket);
            }

            if (FD_ISSET(STDIN_FILENO, &read_fds)) {
                char message[256];
                memset(message, 0, sizeof(message));

                // Read a message from the user
                fgets(message, sizeof(message), stdin);

                // Send the message to Peer 2
                send(peer_socket, message, strlen(message), 0);
            }
        }
    }

    // Close the peer socket
    close(peer_socket);

    return 0;
}
