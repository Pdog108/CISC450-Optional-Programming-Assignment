/*
    Patrick Harris
    CISC450 Optional Programming Assignment
    File: client.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 8000
#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    // Create a client socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        printf("Error creating client socket\n");
        exit(1);
    }
    // Prepare the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    server_addr.sin_port = htons(SERVER_PORT);
    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        printf("Error connecting to server\n");
        exit(1);
    }
    printf("Connected to server\n");
    // Send and receive messages
    while (1) {
        // Clear the buffer
        memset(buffer, 0, BUFFER_SIZE);

        // Get a message from the user
        printf("Enter message: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        // Send the message to the server
        if (send(client_socket, buffer, strlen(buffer), 0) == -1) {
            printf("Error sending message to server\n");
            exit(1);
        }
        // Receive a message from the server
        int recv_size = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (recv_size == 0) {
            // The server disconnected
            printf("Server disconnected\n");
            break;
        } 
        else if (recv_size == -1) {
            // There was an error receiving the message
            printf("Error receiving message from server\n");
            exit(1);
        } 
        else {
            // Display the message received from the server
            printf("Received message from server: %s", buffer);
        }
    }
    // Close the socket
    close(client_socket);
    return 0;
}
