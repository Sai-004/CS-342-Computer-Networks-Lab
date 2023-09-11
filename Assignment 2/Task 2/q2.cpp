#include <bits/stdc++.h>
#include <winsock2.h>

using namespace std;

class WebCache
{
private:
    list<string> dq;
    map<string, pair<string, list<string>::iterator>> mpp;
    int sizee;

    void recent(const string &key)
    {
        dq.erase(mpp[key].second);
        dq.push_front(key);
        mpp[key].second = dq.begin();
    }

public:
    WebCache(int capacity) : sizee(capacity) {}

    string get(const string &url)
    {
        // Check if the URL ends with "/"
        string modifiedUrl = url;
        if (url.back() != '/')
        {
            modifiedUrl += "/";
        }

        if (mpp.find(modifiedUrl) != mpp.end())
        {
            recent(modifiedUrl);
            cout << "Cache Hit: " << modifiedUrl << endl;
            return mpp[modifiedUrl].first;
        }
        else
        {
            string content = getcontent(modifiedUrl);
            put(modifiedUrl, content);
            cout << "Cache Miss: " << modifiedUrl << endl;
            return content;
        }
    }

    string getcontent(const string &url)
    {
        WSADATA ws;
        if (WSAStartup(MAKEWORD(2, 2), &ws) < 0)
            cout << "WSA failed to initialize" << endl;
        else
            cout << "WSA initialized" << endl;

        SOCKET socket_desc;
        struct sockaddr_in serv_addr;
        struct hostent *server;
        char buffer[4096];

        // exit case
        if (url == "exit")
        {
            WSACleanup();
            return "exit";
        }

        // Parse the URL
        string protocol, fullHost, resource;
        size_t protocolEnd = url.find("://");
        if (protocolEnd != string::npos)
        {
            protocol = url.substr(0, protocolEnd);
            size_t hostStart = protocolEnd + 3;
            size_t resourceStart = url.find("/", hostStart);
            fullHost = url.substr(hostStart, resourceStart - hostStart);
            resource = url.substr(resourceStart);
        }
        else
        {
            cerr << "Invalid URL format." << endl;
            WSACleanup();
            return "";
        }

        string host = fullHost;
        string port = "80"; // Port is always 80
        string query = "";

        // Initialize the socket
        socket_desc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (socket_desc < 0)
            cout << "Socket not initialized" << endl;
        else
            cout << "Socket initialized" << endl;

        server = gethostbyname(host.c_str());
        if (server == nullptr)
        {
            cerr << "Couldn't resolve the hostname." << endl;
            closesocket(socket_desc);
            WSACleanup();
            return "";
        }

        ZeroMemory(&serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(stoi(port));
        memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

        if (connect(socket_desc, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
        {
            cerr << "Connection failed." << endl;
            closesocket(socket_desc);
            WSACleanup();
            return "";
        }

        string request = "GET " + resource + query + " HTTP/1.1\r\nHost: " + fullHost + "\r\nConnection: close\r\n\r\n";

        if (send(socket_desc, request.c_str(), static_cast<int>(request.size()), 0) == SOCKET_ERROR)
        {
            cerr << "Failed to send request." << endl;
            closesocket(socket_desc);
            WSACleanup();
            return "";
        }

        int n;
        string raw_site;
        while ((n = recv(socket_desc, buffer, sizeof(buffer), 0)) > 0)
        {
            raw_site.append(buffer, n);
        }

        closesocket(socket_desc);
        WSACleanup();
        return raw_site;
    }

    void put(const string &key, const string &value)
    {
        if (mpp.find(key) == mpp.end())
        {
            dq.push_front(key);
            mpp[key] = {value, dq.begin()};
            sizee--;
        }
        else
        {
            recent(key);
            mpp[key].first = value;
        }
        if (sizee < 0)
        {
            mpp.erase(dq.back());
            dq.pop_back();
            sizee++;
        }
    }
    void display()
    {
        int i = 1;

        for (auto &val : dq)
        {
            cout << "Cache_id " << i << " : " << val << "\n";
            i++;
        }
    }
};

int main()
{
    WebCache cache(5);

    string x;
    while (true)
    {
        cout << "Enter website URL (or 'exit'): ";
        cin >> x;
        if (x == "exit")
        {
            break;
        }
        cout << cache.get(x) << endl;
        cache.display();
    }

    return 0;
}
