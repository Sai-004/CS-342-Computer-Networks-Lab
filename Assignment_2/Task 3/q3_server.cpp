#include <bits/stdc++.h>
#include <Winsock2.h>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

std::vector<SOCKET> clients;
vector<string> clientnames;

void HandleClient(SOCKET clientSocket)
{
    while (true)
    {
        char buffer[1024];
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0)
        {
            // Handle client disconnection
            closesocket(clientSocket);
            auto it = std::find(clients.begin(), clients.end(), clientSocket);
            if (it != clients.end())
            {
                clients.erase(it);
                clientnames.erase(clientnames.begin() + (it - clients.begin())); // Remove corresponding client name
            }
            return;
        }
        int c = -1;
        for (int i = 0; i < clients.size(); i++)
        {
            if (clients[i] == clientSocket)
            {
                c = i;
                break;
            }
        }

        // Broadcast the received message to all clients
        if (strcmp(buffer, "ALL") == 0)
        {
            int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
            for (int i = 0; i < clients.size(); i++)
            {
                if (clients[i] != clientSocket)
                {
                    send(clients[i], clientnames[c].c_str(), bytesRead, 0);
                    send(clients[i], buffer, bytesRead, 0);
                }
            }
        }
        else
        {
            int d = -1;
            for (int i = 0; i < clients.size(); i++)
            {
                if (strcmp(buffer, clientnames[i].c_str()) == 0)
                {
                    d = i;
                    
                }
            }
            if(d == -1)
            {
                send(clients[c], "Server", 6, 0);
                int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                send(clients[c], "No such client", 14, 0);
            }
            else{
            send(clients[d], clientnames[c].c_str(), bytesRead, 0);
            int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
            send(clients[d], buffer, bytesRead, 0);
            }
        }
    }
}

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Error creating server socket." << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(12345); // Change to your desired port number

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Bind failed." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, 10) == SOCKET_ERROR)
    {
        std::cerr << "Listen failed." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server is listening..." << std::endl;

    while (true)
    {
        
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr << "Accept failed." << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }
        //if(clients.size() < 3)
        //{
            clients.push_back(clientSocket);
            char buffer[1024];
            int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
            buffer[bytesRead] = '\0';
            string str(buffer);
            clientnames.push_back(str);

            thread(HandleClient, clientSocket).detach();
        //}
        //else{
        //    cout << "Limit reached";
        //}
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}