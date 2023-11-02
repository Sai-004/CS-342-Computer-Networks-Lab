#include <stdio.h>
#include <bits/stdc++.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h> 
#include <fcntl.h>

using namespace std;

int main() {
    // Socket communication initiation for the server program
    int server_socket, client_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server, client;
    memset(&server, 0, sizeof(server));
    memset(&client, 0, sizeof(client));
    server.sin_family = AF_INET;
    server.sin_port = htons(9009);
    server.sin_addr.s_addr = INADDR_ANY;
    socklen_t add;

    bind(server_socket, (struct sockaddr*)&server, sizeof(server));

    // Print explanatory messages
    cout << "Server starting..." << endl;
    cout << "Using Stop-and-Wait flow control to send 10 messages." << endl;
    cout << "It ensures the sender waits for an ACK from the receiver after each message," << endl;
    cout << "guaranteeing single message transit, preventing congestion, and ensuring reliability." << endl;
    cout << "Crucial in scenarios with potential data loss or corruption." << endl;

    listen(server_socket, 10); 

    add = sizeof(client);
    client_socket = accept(server_socket, (struct sockaddr*)&client, &add);
    time_t t1, t2;
    char default_msg[50] = "Server message : ";
    char buff[50];
    int flag = 0;

    // Initialize variables for the select function
    fd_set set;
    struct timeval timeout;
    int rv;

    for (int i = 0; i < 10; i++) {
        cout << "Counter : " << i << "->";

        // Simulate sending a corrupt packet
        if (i == 6 && flag == 0) {
            i = 15;
            printf("\nSending a corrupt packet (simulating...)\n");
        }

        // Initialize and send the server message
        bzero(buff, sizeof(buff));
        char server_msg[60];
        resend_msg:
        bzero(server_msg, sizeof(server_msg));
        strcpy(server_msg, default_msg);
        server_msg[strlen(default_msg)] = i + '0';
        printf("Message sent to receiver: %s \n", server_msg);

        // Prepare for the select function
        FD_ZERO(&set);
        FD_SET(client_socket, &set);
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;

        // Send the message to the client and wait for acknowledgment
        write(client_socket, server_msg, sizeof(server_msg));
        rv = select(client_socket + 1, &set, NULL, NULL, &timeout);

        if (rv == -1)
            perror("select "); // an error occurred
        else if (rv == 0) {
            printf("\nResending message(timeout)...\n");
            if (i == 15)
                i = 6;
            goto resend_msg;
        } // a timeout occurred
        else
            read(client_socket, buff, sizeof(buff));

        // Check if the ACK matches the expected value
        if (buff[strlen(buff) - 1] != (i % 2) + '0') {
            printf("\nACK error... %s\n", buff);
            goto resend_msg;
        }

        printf("Message from receiver: %s\n", buff);
    }

    // Close the client and server sockets
    close(client_socket);
    close(server_socket);
    return 0;
}
