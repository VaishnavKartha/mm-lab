#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main() {

    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    SOCKET sockfd, newsock;
    struct sockaddr_in server, client;
    int len = sizeof(client);
    char buffer[1024];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8080);

    bind(sockfd, (struct sockaddr *)&server, sizeof(server));
    listen(sockfd, 5);

    printf("Waiting for connection...\n");

    newsock = accept(sockfd, (struct sockaddr *)&client, &len);

    

    memset(buffer, 0, sizeof(buffer));

    recv(newsock, buffer, sizeof(buffer), 0);
    printf("Client: %s\n", buffer);

    
    
    int num=atoi(buffer);

    if(num%2==0){
        strcpy(buffer,"Number is Even");
    }else{
            strcpy(buffer,"Number is Odd");
    }

    

    send(newsock, buffer, strlen(buffer), 0);

        
    

    closesocket(newsock);
    closesocket(sockfd);
    WSACleanup();

    return 0;
}