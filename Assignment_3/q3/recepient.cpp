#include <stdio.h>
#include <bits/stdc++.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h> 

using namespace std;

int main() {
    // Socket communication initiation for the client program
    int client_socket;
    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Define the client socket address structure
    struct sockaddr_in client;
    memset(&client, 0, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_port = htons(9009);
    client.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Attempt to establish a connection with the server
    if (connect(client_socket, (struct sockaddr*)&client, sizeof(client)) == -1) {
        printf("Connection failed");
        return 0;
    }

    // Define two acknowledgement (ACK) messages for sequence 0 and 1
    char state_0[50] = "ACK-0";
    char state_1[50] = "ACK-1";
    char buff[100];

    // Flags for simulating corruption or problems
    int flag1 = 1, flag2 = 1;

    for (int i = 0; i < 10; i++) {
        cout << "Counter : " << i << "->";
        bzero(buff, sizeof(buff));

        // Simulate message corruption or problems
        if ((i == 3 && flag1 == 1) || (i == 7 && flag2 == 1)) {
            

            // Read the message received by the receiver (though it's simulated)
            read(client_socket, buff, sizeof(buff));
            // Update the flags for next iteration
            if (i == 3 && flag1 == 1)
			{
				printf("Simulating no acknowledgment ...\n");
                flag1 = 0;//no acknowledgement
			}
            else if (flag1 == 0)
            {
				printf("Simulating NAK \n");
                write(client_socket, state_0, sizeof(state_0));//wrong Acknowlegnment
                flag2 = 0;
            }

            printf("Message received by receiver: %s \n", buff);

            // Decrement the loop variable to nullify the increment
            i--;
            continue;
        }

        // Read the message from the server
        int n = read(client_socket, buff, sizeof(buff));

        // Check if the received message is corrupted or not
        if (buff[strlen(buff) - 1] != i + '0') {
            //if the msg got corrupted
			printf("Message received by receiver: %s", buff);
            printf("\tDiscarding packet(corrupted)...\n");

            i--;
            continue;
        }

        // Display the message received by the receiver
        printf("Message received by receiver: %s \n", buff);

        // Send an acknowledgment based on the sequence number (state_0 for even, state_1 for odd)
        if (i % 2 == 0)
            write(client_socket, state_0, sizeof(state_0));
        else 
            write(client_socket, state_1, sizeof(state_1));    

        printf("Acknowledgement sent\n");
    }	

    // Close the client socket
    close(client_socket);
    return 0;
}
