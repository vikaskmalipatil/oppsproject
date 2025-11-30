#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <functional>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

class SimpleServer {
public:
    void start(int port, std::function<std::string(std::string, std::string)> handler) {
        WSADATA data;
        WSAStartup(MAKEWORD(2,2), &data);

        SOCKET server = socket(AF_INET, SOCK_STREAM, 0);

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        bind(server, (sockaddr*)&addr, sizeof(addr));
        listen(server, 5);

        std::cout << "Server running at http://localhost:" << port << "\n";

        while (true) {
            sockaddr_in client{};
            int size = sizeof(client);

            SOCKET sock = accept(server, (sockaddr*)&client, &size);
            if (sock == INVALID_SOCKET) continue;

            char buffer[8192];
            int len = recv(sock, buffer, sizeof(buffer)-1, 0);
            if (len <= 0) { closesocket(sock); continue; }
            buffer[len] = 0;
            std::string req = buffer;

            std::string resp = handler(req, std::to_string(port));

            send(sock, resp.c_str(), (int)resp.size(), 0);
            closesocket(sock);
        }

        closesocket(server);
        WSACleanup();
    }
};
