#include<iostream>
#include<stack>
#include <string>
#include <sstream>
#include<winsock.h>
using namespace std;
#define PORT 9909
struct sockaddr_in srv;
struct sockaddr_in clt;
fd_set fr, fw, fe;

int nMaxFd;
int serverSocket;
int nArrClient[5];

bool isValidExpression(const string& expression) {
    stack<char> parenthesesStack;

    for (size_t i = 0; i < expression.length(); ++i) {
        char c = expression[i];

        if (isspace(c)) {
            continue; // Ignore spaces
        }

        if (isdigit(c) || c == '.' || c == '-' || c == '+') {
            if (c == '-' || c == '+') {
                // Check if the minus or plus sign is at the beginning or follows an operator
                if (i == 0 || (!isdigit(expression[i - 1]) && expression[i - 1] != '.')) {
                    continue;
                }
            }
            // Handle numbers, including decimals and negative numbers
            while (i < expression.length() && (std::isdigit(expression[i]) || expression[i] == '.')) {
                ++i;
            }
            --i;
            continue;
        }

        if (c == '(' || c == '{' || c == '[') {
            parenthesesStack.push(c);
        } else if (c == ')' || c == '}' || c == ']') {
            if (parenthesesStack.empty()) {
                return false; // Mismatched closing parenthesis
            }

            char top = parenthesesStack.top();
            parenthesesStack.pop();

            if ((c == ')' && top != '(') || (c == '}' && top != '{') || (c == ']' && top != '[')) {
                return false; // Mismatched parenthesis
            }
        } else if (c == '+' || c == '-' || c == '*' || c == '/') {
            continue; // Operators are valid
        } else {
            return false; // Invalid character
        }
    }

    return parenthesesStack.empty(); // Check if all opening parentheses are closed
}

bool isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

int precedence(char op) {
    if (op == '+' || op == '-')
        return 1;
    if (op == '*' || op == '/')
        return 2;
    return 0;
}

double applyOperator(double a, double b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
        default: return 0;
    }
}

double evaluateExpression(const string& expression) {
    stack<double> values;
    stack<char> operators;

    stringstream ss(expression);
    string token;

    while (ss >> token) {
        if (isdigit(token[0]) || (token[0] == '-' && token.size() > 1)) {
            values.push(stod(token));
        } else if (isOperator(token[0])) {
            while (!operators.empty() && precedence(operators.top()) >= precedence(token[0])) {
                char op = operators.top(); operators.pop();
                double b = values.top(); values.pop();
                double a = values.top(); values.pop();
                values.push(applyOperator(a, b, op));
            }
            operators.push(token[0]);
        }
    }

    while (!operators.empty()) {
        char op = operators.top(); operators.pop();
        double b = values.top(); values.pop();
        double a = values.top(); values.pop();
        values.push(applyOperator(a, b, op));
    }

    return values.top();
}


int main()
{
    int nRet = 0;
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) < 0)
        cout << "WSA failed to initialize" << endl;
    else cout << "WSA initialized" << endl;

    int serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket < 0) cout << "not opened" << endl;
    else cout << "opened " << serverSocket << endl;

    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);
    srv.sin_addr.s_addr = INADDR_ANY;
    memset(&(srv.sin_zero), 0, 8);

    int nOptval = 0;
    int nOptlen = sizeof(nOptval);
    nRet = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&nOptval, nOptlen);
    if (!nRet) cout << "setsockopt call successful" << endl;
    else {
        cout << "failed setsockopt call";
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    nRet = bind(serverSocket, (sockaddr*)&srv, sizeof(sockaddr));
    if (nRet < 0) {
        cout << "Failed to bind to local port" << endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else cout << "binded to local port" << endl;

    char buff[256 + 1] = { 0, };
    int addr_len = sizeof(struct sockaddr);

    while (1) {
        nRet = recvfrom(serverSocket, buff, 256, 0, (struct sockaddr*)&srv, &addr_len);
        if (nRet < 0) {
            cout << "Error in receiving data" << endl;
        }
        else {
            cout << "Received message: " << buff << endl;
            string s;
            if (!isValidExpression(buff))
                s = "Expression not valid";
            else
                s = to_string(evaluateExpression(buff));
            char* charArray = new char[s.length() + 1];
            copy(s.begin(), s.end(), charArray);
            charArray[s.length()] = '\0';
            sendto(serverSocket, charArray, s.size() + 1, 0, (struct sockaddr*)&srv, sizeof(struct sockaddr));
            cout << "Sent response: " << charArray << endl;
            delete[] charArray;
        }
    }

    WSACleanup();
    return 0;


}