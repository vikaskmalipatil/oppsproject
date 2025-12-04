#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")

class SimpleServer {
public:
    void start(int port, std::string (*handler)(std::string, std::string)) {
        WSADATA wsa;
        WSAStartup(MAKEWORD(2, 2), &wsa);

        SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocket == INVALID_SOCKET) {
            std::cerr << "Socket creation failed\n";
            return;
        }

        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);

        if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Bind failed\n";
            closesocket(serverSocket);
            WSACleanup();
            return;
        }

        listen(serverSocket, 10);
        std::cout << "Server running at http://localhost:" << port << "\n";

        while (true) {
            SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
            if (clientSocket == INVALID_SOCKET) continue;

            char buffer[8192];
            int received = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (received <= 0) {
                closesocket(clientSocket);
                continue;
            }

            std::string request(buffer, received);
            std::string response = handler(request, std::to_string(port));

            send(clientSocket, response.c_str(), (int)response.size(), 0);
            closesocket(clientSocket);
        }

        closesocket(serverSocket);
        WSACleanup();
    }
};
