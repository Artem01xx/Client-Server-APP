#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

constexpr int MAX_BUFFER_SIZE = 1024;
constexpr int PORT = 12345;
constexpr const char* SERVER_ADDRESS = "127.0.0.1";

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << " <resource_name>\n";
        return 1;
    }
    
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed with error: " << result << "\n";
        return 1;
    }

   
    SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == INVALID_SOCKET) {
        std::cerr << "Failed to create socket.\n";
        WSACleanup();
        return 1;
    }

    
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_ADDRESS, &serverAddr.sin_addr);

    // Sending Request On Server
    std::string request = argv[1];
    sendto(sockfd, request.c_str(), request.size(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));

    // Answer From Server
    char buffer[MAX_BUFFER_SIZE];
    int bytesReceived = recvfrom(sockfd, buffer, sizeof(buffer), 0, nullptr, nullptr);
    if (bytesReceived < 0) {
        std::cerr << "Failed to receive response from server.\n";
    }
    else {
        std::cout << "Received response from server: " << "\n" << std::string(buffer, bytesReceived) << "\n";
    }

    closesocket(sockfd);
    WSACleanup();

    return 0;
}
