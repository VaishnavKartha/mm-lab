#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {

    int sockfd;
    struct sockaddr_in server, client;
    char buffer[1024];
    socklen_t len = sizeof(client);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8080);

    bind(sockfd, (struct sockaddr *)&server, sizeof(server));

    printf("UDP Chat Server Started...\n");

    while (1) {

        memset(buffer, 0, sizeof(buffer));

        recvfrom(sockfd, buffer, sizeof(buffer), 0,
                 (struct sockaddr *)&client, &len);

        printf("Client: %s\n", buffer);

        if (strncmp(buffer, "exit", 4) == 0)
            break;

        printf("Server: ");
        fgets(buffer, sizeof(buffer), stdin);

        sendto(sockfd, buffer, strlen(buffer), 0,
               (struct sockaddr *)&client, len);

        if (strncmp(buffer, "exit", 4) == 0)
            break;
    }

    close(sockfd);

    return 0;
}






//client



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {

    int sockfd;
    struct sockaddr_in server;
    char buffer[1024];
    socklen_t len = sizeof(server);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("UDP Chat Client Started...\n");

    while (1) {

        printf("Client: ");
        fgets(buffer, sizeof(buffer), stdin);

        sendto(sockfd, buffer, strlen(buffer), 0,
               (struct sockaddr *)&server, len);

        if (strncmp(buffer, "exit", 4) == 0)    
            break;

        memset(buffer, 0, sizeof(buffer));

        recvfrom(sockfd, buffer, sizeof(buffer), 0,
                 (struct sockaddr *)&server, &len);

        printf("Server: %s\n", buffer);

        if (strncmp(buffer, "exit", 4) == 0)
            break;
    }

    close(sockfd);

    return 0;
}