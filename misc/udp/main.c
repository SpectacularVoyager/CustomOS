#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024  // Buffer size for receiving data

void listen_udp(const char *host, int port) {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUF_SIZE];
    socklen_t client_len = sizeof(client_addr);

    // Create a UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(host);  // Host address (use INADDR_ANY for all interfaces)
    server_addr.sin_port = htons(port);  // Port number

    // Bind the socket to the address and port
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Listening on %s:%d...\n", host, port);

    while (1) {
        // Receive data from client
        ssize_t len = recvfrom(sockfd, (char *)buffer, BUF_SIZE, 0,
                               (struct sockaddr *)&client_addr, &client_len);
        if (len < 0) {
            perror("Receive failed");
            break;
        }

        buffer[len] = '\0';  // Null terminate the received data
        printf("Received message from %s:%d: %s\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);
    }

    close(sockfd);  // Close the socket
}

int main() {
    const char *host = "0.0.0.0";  // Listen on all available interfaces
    int port = 12345;  // UDP port to listen on

    listen_udp(host, port);
    return 0;
}

