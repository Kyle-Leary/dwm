#include "config.h"
#include "broadcast.h"

#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

// listen to the socket file for changes, async.
#include <sys/inotify.h>

int main() {
    int sockfd, clientfd;
    struct sockaddr_un addr;

    // Create socket
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set socket address
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, BROADCAST_SOCKET, sizeof(addr.sun_path) - 1);

    // Bind socket to address
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Start listening for connections
    if (listen(sockfd, SOMAXCONN) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for incoming connections...\n");

    // Accept incoming connection
    clientfd = accept(sockfd, NULL, NULL);
    if (clientfd == -1) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    printf("Client connected.\n");

    // Send and receive data
    char buffer[256];
    ssize_t numBytes;

    // Receive data from client
    numBytes = recv(clientfd, buffer, sizeof(buffer) - 1, 0);
    if (numBytes == -1) {
        perror("recv");
        exit(EXIT_FAILURE);
    }

    // Null-terminate received data
    buffer[numBytes] = '\0';

    printf("Received data: %s\n", buffer);

    // Send a response to the client
    const char* response = "Hello from the server!";
    if (send(clientfd, response, strlen(response), 0) == -1) {
        perror("send");
        exit(EXIT_FAILURE);
    }

    // Close client socket
    close(clientfd);

    // Close server socket
    close(sockfd);

    return 0;
}


