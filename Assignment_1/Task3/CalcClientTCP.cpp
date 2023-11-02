#include <iostream>
#include <winsock.h>

using namespace std;
#define PORT 9909

struct sockaddr_in serv;

int main()
{
    int temp = 0;
    // Initialize the WSA variables
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) < 0)
        cout << "WSA failed to initialize" << endl;
    else
        cout << "WSA initialized" << endl;

    // Initialize the socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket < 0)
        cout << "socket not initialized" << endl;
    else
        cout << "Socket Initialized " << clientSocket << endl;

    // Initialize the environment for sockaddr structure
    serv.sin_family = AF_INET;
    serv.sin_port = htons(PORT);
    serv.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(&(serv.sin_zero), 0, 8);

    temp = connect(clientSocket, (struct sockaddr *)&serv, sizeof(serv));
    if (temp < 0)
    {
        cout << "Connection Failed" << endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else
    {
        cout << "connecting server...." << endl;
        // talk to the client
        char message[256] = {
            0,
        };
        recv(clientSocket, message, 255, 0);
        cout << message << endl
             << endl;

        // sending new message
        cout << "Enter the Expression (space seperated): ";
        while (1)
        {
            fgets(message, 256, stdin);
            send(clientSocket, message, 256, 0);
            recv(clientSocket, message, 256, 0);
            cout << "Result : " << message << endl
                 << "Enter the Expression: " << endl;
        }
    }
}