#include <iostream>
#include <string>
#include <Winsock2.h>
using namespace std;

#include <thread>

#pragma comment(lib, "ws2_32.lib")

void ReceiveMessages(SOCKET clientSocket) {
    while (true) {
        char buffer[1024];
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            std::cerr << "Server disconnected." << std::endl;
            closesocket(clientSocket);
            return;
        }

        buffer[bytesRead] = '\0';
        std::cout << "Received from: " << buffer << std::endl;
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        buffer[bytesRead] = '\0';
        std::cout << "Received message: " << buffer << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        std::cerr << "Prototype: " << argv[0] << " <Server_IP_Address>" << std::endl;
        return 1;
    }

    const char* server_ip = argv[1];

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Error creating client socket." << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(server_ip); // Replace with the server's IP address
    serverAddr.sin_port = htons(12345); // Replace with the server's port number

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connect failed." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::string message;
    cout << "Username: " ;
    std::getline(std::cin, message);
    send(clientSocket, message.c_str(), message.size(), 0);

    std::thread(ReceiveMessages, clientSocket).detach();

    while (true) {
        std::getline(std::cin, message);
        send(clientSocket, message.c_str(), message.size(), 0);
        std::getline(std::cin, message);
        send(clientSocket, message.c_str(), message.size(), 0);

        if (message == "/exit") {
            std::cout << "Exiting chat..." << std::endl;
            closesocket(clientSocket);
            break;
        }
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}