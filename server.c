/*
    Patrick Harris
    CISC450 Optional Programming Assignment
    File: server.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENTS 10 // Maximum number of clients that can connect to the server
#define BUFFER_SIZE 1024 // Size of the buffer used to send/receive messages

int client_sockets[MAX_CLIENTS] = {0}; // Array to store client socket file descriptors
// Function to handle client communication
void *handle_client(void *args) {
    int client_socket = *(int *)args; // Get client socket file descriptor from arguments
    char buffer[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int recv_size = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (recv_size == 0) {
            // Client disconnected
            printf("Client disconnected: %d\n", client_socket);
            break;
        } else if (recv_size == -1) {
            // error
            printf("Error receiving message from client: %d\n", client_socket);
            break;
        } else {
            // Broadcast message to all clients except sender
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (i != client_socket && client_sockets[i] != 0) { // Don't send message back to the sender
                    send(client_sockets[i], buffer, strlen(buffer), 0); // Send message to other clients
                }
            }
        }
    }
    // Remove client from list
    client_sockets[client_socket] = 0;
    // Close socket
    close(client_socket);
    pthread_exit(NULL); // Exit thread
}
int main() {
    int server_socket, client_socket, addr_size, i;
    struct sockaddr_in server_addr, client_addr;
    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        printf("Error creating server socket\n");
        exit(1);
    }
    // Prepare server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8000);
    // Bind socket to address
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        printf("Error binding server socket to address\n");
        exit(1);
    }
    // Listen for connections
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        printf("Error listening for connections\n");
        exit(1);
    }
    printf("Server started. Listening for connections...\n");
    while (1) {
        // Accept connection
        addr_size = sizeof(struct sockaddr_in);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, (socklen_t *)&addr_size);
        if (client_socket == -1) {
            printf("Error accepting connection\n");
            continue;
        }
        printf("Client connected: %d\n", client_socket);
        // Add client to list
        for (i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] == 0) {
                client_sockets[i] = client_socket;
                break;
            }
        }
        // Check if maximum number of clients reached
        if (i == MAX_CLIENTS) {
            printf("Maximum number of clients reached\n");
            continue;
        }
        // Create thread to handle client communication
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, &client_socket) < 0) {
            printf("Error creating thread\n");
            exit(1);
        }
    }
    return 0;
}