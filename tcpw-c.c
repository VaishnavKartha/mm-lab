#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main() {

    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    SOCKET sockfd;
    struct sockaddr_in server;
    char buffer[1024];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sockfd, (struct sockaddr *)&server, sizeof(server));


    printf("Client: ");
    fgets(buffer, sizeof(buffer), stdin);

    send(sockfd, buffer, strlen(buffer), 0);

    

    memset(buffer, 0, sizeof(buffer));

    recv(sockfd, buffer, sizeof(buffer), 0);
    printf("Server: %s\n", buffer);

        
    

    closesocket(sockfd);
    WSACleanup();

    return 0;
}