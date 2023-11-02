#include <iostream>
#include <winsock.h>
using namespace std;

#define PORT 9909
struct sockaddr_in srv;

int main()
{
    int nRet = 0;
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) < 0)
        cout << "WSA failed to initialize" << endl;
    else cout << "WSA initialized" << endl;

    int clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket < 0) cout << "not opened" << endl;
    else cout << "opened " << clientSocket << endl; //prints the socket id

    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);
    srv.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(&(srv.sin_zero), 0, 8);

    char buff[256] = { 0, };

    cout << "Send your expression to server:" << endl;
    while (1)
    {
        fgets(buff, 256, stdin);
        sendto(clientSocket, buff, strlen(buff), 0, (struct sockaddr*)&srv, sizeof(srv));

        int addr_len = sizeof(struct sockaddr);
        nRet = recvfrom(clientSocket, buff, 255, 0, (struct sockaddr*)&srv, &addr_len);
        if (nRet < 0)
        {
            cout << "Error receiving data" << endl;
            continue;
        }
        buff[nRet] = '\0';
        cout << "Result : " << buff << endl << "Now send next expression:" << endl;
    }

    WSACleanup();
    return 0;
}