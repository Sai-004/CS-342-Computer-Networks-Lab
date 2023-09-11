#include <iostream>
#include <winsock.h>
using namespace std;
#define PORT 9909
struct sockaddr_in serv;

int main()
{
    int nRet = 0;
    // Initialize the WSA variables -->enables those APIs which enable socket programming in windows
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) < 0)
        cout << "WSA failed to initialize" << endl;
    else
        cout << "WSA initialized" << endl;

    // Initialize the socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket < 0)
        cout << "Socket not initialized" << endl;
    else
        cout << "Initialized Socket: " << clientSocket << endl; // prints the socket id

    // Initialize the environment for sockaddr structure
    serv.sin_family = AF_INET;
    serv.sin_port = htons(PORT);
    // serv.sin_addr.s_addr = INADDR_ANY; // collects the system IP directly but cannot be used for the client codes.
    serv.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(&(serv.sin_zero), 0, 8);

    nRet = connect(clientSocket, (struct sockaddr *)&serv, sizeof(serv));
    if (nRet < 0)
    {
        cout << "Failed to connect" << endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else
    {
        cout << "Connecting to server...." << endl;
        // talk to the server
        char message[256] = {
            0,
        };
        recv(clientSocket, message, 255, 0);
        // cout << "press any key to read the message from server" << endl;
        // getchar();
        cout << message << endl
             << endl;

        // sending messages to server
        cout << "Start chat with Server: ";
        while (1)
        {
            fgets(message, 256, stdin);
            message[strcspn(message, "\n")] = '\0'; // Remove newline character
            send(clientSocket, message, 256, 0);
            if (strcmp(message, "\\exit") == 0)
            {
                cout << "Exiting the client." << endl;
                break;
            }
            recv(clientSocket, message, 256, 0);
            cout << "Message from server: " << message << endl
                 << "Your message: ";
        }
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}