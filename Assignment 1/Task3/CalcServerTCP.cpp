#include <iostream>
#include <stack>
#include <string>
#include <sstream>
#include <winsock.h>

using namespace std;
#define PORT 9909
#define no_of_clients 5 //change the no of clients as needed

struct sockaddr_in serv;
fd_set f_read, f_write, f_excp;

int nMaxFd;
int serverSocket;
int nArrClient[no_of_clients];

bool checkExpressionValidity(const char *expression) {
    // Variables to track operands and operator
    bool foundOperator = false;
    bool foundOperand1 = false;
    bool foundOperand2 = false;

    // Skip leading spaces
    while (isspace(*expression)) {
        expression++;
    }

    // Check if the first character is a valid digit or a negative sign
    if (*expression == '-' || isdigit(*expression)) {
        foundOperand1 = true;
    } else {
        return false;
    }

    // Move past operand 1
    while (*expression && (isdigit(*expression) || *expression == '-')) {
        expression++;
    }

    // Skip spaces
    while (isspace(*expression)) {
        expression++;
    }

    // Check for a valid operator
    if (*expression == '+' || *expression == '-' || *expression == '*' ||
        *expression == '/' || *expression == '^') {
        foundOperator = true;
        expression++;
    } else {
        return false;
    }

    // Skip spaces
    while (isspace(*expression)) {
        expression++;
    }

    // Check if the next character is a valid digit or a negative sign
    if (*expression == '-' || isdigit(*expression)) {
        foundOperand2 = true;
    } else {
        return false;
    }

    // Move past operand 2
    while (*expression && (isdigit(*expression) || *expression == '-')) {
        expression++;
    }

    // Skip trailing spaces
    while (isspace(*expression)) {
        expression++;
    }

    // Expression is valid if all components are found and no additional characters are present
    return (*expression == '\0' && foundOperand1 && foundOperator && foundOperand2);
}

double evalExp(const char *expression) {
    char operatorChar;
    int operand1, operand2;
    sscanf(expression, "%d %c %d", &operand1, &operatorChar, &operand2);

    switch (operatorChar) {
        case '+':
            return operand1 + operand2;
        case '-':
            return operand1 - operand2;
        case '*':
            return operand1 * operand2;
        case '/':
            if (operand2 != 0) {
                return static_cast<double>(operand1) / operand2;
            } else {
                // Handle division by zero
                return NAN;
            }
        case '^':
            return pow(operand1, operand2);
        default:
            // Handle invalid operator
            return NAN;
    }
}


void ProcessNewMessage(int nClientsocket)
{
    cout << "Processing messages of client: " << nClientsocket << endl;
    char buff[256 + 1] = {
        0,
    };
    int temp = recv(nClientsocket, buff, 256, 0);
    if (temp < 0)
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
        cout << "Enter an expression:" << buff << endl;
        string exp_str;
        if (!checkExpressionValidity(buff))
            exp_str = "Invalid Expression";
        else
            exp_str = to_string(evalExp(buff));
        char *ch_arr = new char[exp_str.length() + 1];
        copy(exp_str.begin(), exp_str.end(), ch_arr);
        ch_arr[exp_str.length()] = '\0';
        send(nClientsocket, ch_arr, exp_str.size() + 1, 0);
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
            if (i == no_of_clients)
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

int main()
{
    int temp = 0;
    // Initialize the WSA variables -->enables those APIs which enable socket programming in windows
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) < 0)
        cout << "WSA failed to initialize" << endl;
    else
        cout << "WSA initialized" << endl;

    // Initialize the socket
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket < 0)
        cout << "not opened" << endl;
    else
        cout << "opened " << serverSocket << endl; // prints the socket id

    // Initialize the environment for sockaddr structure
    serv.sin_family = AF_INET;
    serv.sin_port = htons(PORT);
    serv.sin_addr.s_addr = INADDR_ANY;
    memset(&(serv.sin_zero), 0, 8);

    // Bind the socket to the local port
    temp = bind(serverSocket, (sockaddr *)&serv, sizeof(sockaddr));
    if (temp < 0)
    {
        cout << "Failed to bind to local port" << endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else
        cout << "binded to local port" << endl;

    // Listen the request from client
    temp = listen(serverSocket, no_of_clients);
    if (temp < 0)
    {
        cout << "Failed to listen" << endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else
        cout << "listening" << endl;

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

        for (int i = 0; i < 5; i++)
        {
            if (nArrClient[i] != 0)
            {
                FD_SET(nArrClient[i], &f_read);
                FD_SET(nArrClient[i], &f_excp);
            }
        }

        // cout << "before select call:" << f_read.fd_count << endl;
        temp = select(nMaxFd + 1, &f_read, &f_write, &f_excp, &tv);

        if (temp > 0)
        {
            // when someone connects with a message over a dedicated connection
            // cout << "data on port....processing now..." << endl;
            // Process the request
            ProcessNewRequest();
        }
        else if (temp == 0)
        {
            // none of the socket decriptors are ready
            // no connection is made
            // cout << "nothing on port:" << PORT << endl;
        }
        else
        {
            // it failed
            cout << "failed to communicate" << endl;
            WSACleanup();
            exit(EXIT_FAILURE);
        }
        // cout << "after select call:" << f_read.fd_count << endl;
    }
    return 0;
}