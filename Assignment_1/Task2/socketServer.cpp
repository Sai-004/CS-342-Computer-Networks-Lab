#include <iostream>
#include <winsock.h>
using namespace std;

#define PORT 9909
// details about the port where we are listening from and the IP address
#define no_of_clients 5

struct sockaddr_in serv;
fd_set f_read, f_write, f_excp;
// socket discripters --> index to a table of sockets
// f_read - socket discripters which are ready to read
// f_write - ready to write to the network
// f_excp - ready to through exception
// at max 64 socket discripters

int nMaxFd;
int serverSocket;
int nArrClient[no_of_clients];

void ProcessNewMessage(int nClientsocket)
{
    cout << "Processing messages of client: " << nClientsocket << endl;
    char buff[256 + 1] = {
        0,
    };
    int nRet = recv(nClientsocket, buff, 256, 0);
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
        if (strcmp(buff, "\\exit") == 0) {
            cout << "Client requested exit...Closing the connection" << endl;
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
        cout << "Message from Client: " << buff << endl;
        // send response to client
        char buff[256] = {0,};
        cout << "Your message: ";
        fgets(buff, 256, stdin);
        send(nClientsocket, buff, 256, 0);
        // send(nClientsocket, "Processed the request", 22, 0);
        cout << "**********************************" << endl;
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
                // Got a new message from Client
                // recv new message
                // queue for new works for your server to fulfill the request.
                ProcessNewMessage(nArrClient[i]);
            }
        }
    }
}

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
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket < 0)
        cout << "Socket not initialized" << endl;
    else
        cout << "Initialized Socket: " << serverSocket << endl; // prints the socket id

    // Initialize the environment for sockaddr structure
    serv.sin_family = AF_INET;
    serv.sin_port = htons(PORT);
    serv.sin_addr.s_addr = INADDR_ANY; // collects the system IP directly
    // serv.sin_addr.s_addr=inet_addr("127.0.0.0"); -- for using any required IP
    memset(&(serv.sin_zero), 0, 8);

    // setsockopt call
    int nOptval = 0;
    int nOptlen = sizeof(nOptval);
    // SO_REUSEADDR --> same port can be used in other sockets calls in the code
    // SO_EXCLUSIVEADDR --> make the port exclusive for the particular socket
    nRet = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&nOptval, nOptlen);
    if (!nRet)
        cout << "setsockopt call successful" << endl;
    else
    {
        cout << "failed setsockopt call";
        WSACleanup();
        exit(EXIT_FAILURE);
    }

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
        cout << "listening...." << endl
             << "**********************************" << endl;

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
            // when someone connects with a message over a dedicated connection
            // cout << "Processing data from port: " << PORT << endl;
            // Process the request
            ProcessNewRequest();
        }
        else if (nRet == 0)
        {
            // none of the socket decriptors are ready
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
    return 0;
}