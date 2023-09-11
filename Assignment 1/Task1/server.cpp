#include <iostream>
#include <winsock.h>
#include "base64_decoder.cpp"
using namespace std;

const int MSG_LEN = 256;
#define no_of_clients 5

struct sockaddr_in serv;
fd_set f_read, f_write, f_excp;

int nMaxFd;
int serverSocket;
int nArrClient[no_of_clients];

void ProcessNewMessage(int nClientsocket)
{
    cout << "Processing messages of client: " << nClientsocket << endl;
    char buff[256 + 1] = {
        0,
    };
    int nRet = recv(nClientsocket, buff, MSG_LEN, 0);
    if (nRet < 0)
    {
        cout << "Error occured....Closing the connection: " << nClientsocket << endl;
        closesocket(nClientsocket);
        for (int i = 0; i < no_of_clients; i++)
        {
            if (nArrClient[i] == nClientsocket)
            {
                nArrClient[i] = 0;
                break;
            }
        }
    }
    else
    {
        buff[nRet] = '\0';

        int message_type = 0;
        char message[MSG_LEN];
        sscanf(buff, "%d|%s", &message_type, message);
        cout << "Received message_type: " << message_type << "\nReceived raw message: " << message << endl;

        if (message_type == 1)
        {
            string decoded_message = base64_decode(string(message));
            cout << "Decoded message: " << decoded_message << endl;

            const char *ack_message = "2 | Acknowledged";
            send(nClientsocket, ack_message, strlen(ack_message), 0);
        }
        else if (message_type == 3)
        {
            cout << "Received close communication message." << endl;
            closesocket(nClientsocket);
            for (int i = 0; i < 5; i++)
            {
                if (nArrClient[i] == nClientsocket)
                {
                    nArrClient[i] = 0;
                    break;
                }
            }
            cout << "***************************************" << endl;
            return;
        }
        else
        {
            cerr << "Received unknown message type: " << message_type << endl;
            closesocket(nClientsocket);
            for (int i = 0; i < 5; i++)
            {
                if (nArrClient[i] == nClientsocket)
                {
                    nArrClient[i] = 0;
                    break;
                }
            }
            return;
        }
        cout << "***************************************" << endl;
    }
}

void ProcessNewRequest()
{
    // New connection
    if (FD_ISSET(serverSocket, &f_read))
    {
        int nlen = sizeof(struct sockaddr);
        int nClientsocket = accept(serverSocket, NULL, &nlen);
        if (nClientsocket > 0)
        {
            int i;
            // Put it into the Client fd_set (same as fd set of server)
            for (i = 0; i < no_of_clients; i++)
            {
                if (nArrClient[i] == 0)
                {
                    nArrClient[i] = nClientsocket;
                    send(nClientsocket, "!!! Connection established !!!", 31, 0);
                    break;
                }
            }
            if (i == 5)
            {
                cout << "No space for a new connection" << endl;
            }
        }
    }
    else
    {
        for (int i = 0; i < no_of_clients; i++)
        {
            if (FD_ISSET(nArrClient[i], &f_read))
            {
                ProcessNewMessage(nArrClient[i]);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Usage: " << argv[0] << " <Server_Port_Number>" << endl;
        return 1;
    }

    int nRet = 0;
    // Initialize the WSA variables -->enables those APIs which enable socket programming in windows
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) < 0)
        cout << "WSA failed to initialize" << endl;
    else
        cout << "WSA initialized" << endl;

    // Initialize the socket
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket < 0)
        cout << "Socket not initialized" << endl;
    else
        cout << "Initialized Socket: " << serverSocket << endl; // prints the socket id

    // Initialize the environment for sockaddr structure
    serv.sin_family = AF_INET;
    serv.sin_port = htons(stoi(argv[1]));
    serv.sin_addr.s_addr = INADDR_ANY; // collects the system IP directly
    memset(&(serv.sin_zero), 0, 8);

    // Bind the socket to the local port
    nRet = bind(serverSocket, (sockaddr *)&serv, sizeof(sockaddr));
    if (nRet < 0)
    {
        cout << "Failed to bind to local port" << endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else
        cout << "Binded to local port" << endl;

    // Listen the request from client
    nRet = listen(serverSocket, no_of_clients);
    if (nRet < 0)
    {
        cout << "Failed to listen" << endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else
        cout << "Server is listening on port " << argv[1] << "..." << endl;
        cout << "***************************************" << endl;

    nMaxFd = serverSocket;
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    // keep waiting for the incoming requests and proceed as per request
    while (1)
    {
        // everytime these socket discriptors are written to avoid failure for empty requests
        FD_ZERO(&f_read);
        FD_ZERO(&f_write);
        FD_ZERO(&f_excp);

        FD_SET(serverSocket, &f_read);
        FD_SET(serverSocket, &f_excp);

        for (int i = 0; i < no_of_clients; i++)
        {
            if (nArrClient[i] != 0)
            {
                FD_SET(nArrClient[i], &f_read);
                FD_SET(nArrClient[i], &f_excp);
            }
        }

        nRet = select(nMaxFd + 1, &f_read, &f_write, &f_excp, &tv);
        if (nRet > 0)
        {
            // Process the request
            ProcessNewRequest();
        }
        else if (nRet == 0)
        {
            // no connection is made
            // cout << "nothing on port:" << PORT << endl;
        }
        else
        {
            // it failed
            cout << "Failed to communicate" << endl;
            WSACleanup();
            exit(EXIT_FAILURE);
        }
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}