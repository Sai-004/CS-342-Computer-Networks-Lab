#include <iostream>
#include <winsock.h>
#include "base64_encoder.cpp"
using namespace std;

struct sockaddr_in serv;

void send_message(int clientSocket, int message_type, const string &message)
{
    string encoded_message = base64_encode(message);
    string full_message = to_string(message_type) + "|" + encoded_message;

    send(clientSocket, full_message.c_str(), full_message.length(), 0);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <Server_IP_Address> <Server_Port_Number>" << endl;
        return 1;
    }

    const char *server_ip = argv[1];
    int server_port = stoi(argv[2]);

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
    serv.sin_port = htons(server_port);
    serv.sin_addr.s_addr = inet_addr(server_ip);
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
        // talk to the server
        char message[256] = {
            0,
        };
        recv(clientSocket, message, 255, 0);
        cout << "Trying to connect to server at " << server_ip << ":" << server_port <<"...."<< endl;
        cout << message << endl
             << endl;

        while (1)
        {
            int message_type = 0;
            cout << "Enter message type (1: Regular, 3: Close): ";
            cin >> message_type;

            if (message_type == 3)
            {
                send_message(clientSocket, 3, "");
                break;
            }

            cin.ignore(); // Clear newline from previous input
            string message;
            cout << "Enter message: ";
            getline(cin, message);

            send_message(clientSocket, 1, message);

            char ack_buffer[1024];
            int buff = recv(clientSocket, ack_buffer, sizeof(ack_buffer), 0);
            if (buff > 0)
            {
                ack_buffer[buff] = '\0'; // Null-terminate the received data
                cout << "Server acknowledgment: " << ack_buffer << endl;
            }
        }
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}