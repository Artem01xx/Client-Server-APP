#include <iostream>
#include <string>
#include <unordered_map>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

constexpr int MAX_BUFFER_SIZE = 1024;
constexpr int PORT = 12345;
// Connecting resources
std::unordered_map<std::string, std::string> resources = {
    {"resource1", "Hello"},
    {"resource2", "World"},
    {"resource3", "Thank you"}
};

void handleRequest(SOCKET sockfd) {
    char buffer[MAX_BUFFER_SIZE];
    sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    int bytesReceived = recvfrom(sockfd, buffer, sizeof(buffer), 0, (sockaddr*)&clientAddr, &clientAddrLen);
    if (bytesReceived < 0) {
        std::cerr << "Failed to receive data from client.\n";
        return;
    }

    std::string requestedResource(buffer, bytesReceived);
    std::string response;

    if (resources.find(requestedResource) != resources.end()) {
        response = "-BEGIN-\n" + resources[requestedResource] + "\n-END-";
    }
    else {
        response = "-ERROR-\nResource not found\n-END-";
    }

    int bytesSent = sendto(sockfd, response.c_str(), response.size(), 0, (sockaddr*)&clientAddr, clientAddrLen);
    if (bytesSent < 0) {
        std::cerr << "Failed to send response to client.\n";
        return;
    }
}

int main() {
  
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
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(sockfd, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind socket.\n";
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    std::cout << "Server started. Listening on port " << PORT << "\n";

    while (true) {
        std::thread(handleRequest, sockfd).detach();
    }

    closesocket(sockfd);
    WSACleanup();
    return 0;
}
