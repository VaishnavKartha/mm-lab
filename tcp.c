#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sockfd, newsock;
    struct sockaddr_in server, client;
    socklen_t len = sizeof(client);
    char buffer[1024];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8080);

    bind(sockfd, (struct sockaddr *)&server, sizeof(server));
    listen(sockfd, 5);

    printf("Waiting for connection...\n");

    newsock = accept(sockfd, (struct sockaddr *)&client, &len);

    while (1) {

        memset(buffer, 0, sizeof(buffer));

        read(newsock, buffer, sizeof(buffer));
        printf("Client: %s\n", buffer);

        if (strncmp(buffer, "exit", 4) == 0)
            break;

        printf("Server: ");
        fgets(buffer, sizeof(buffer), stdin);

        send(newsock, buffer, strlen(buffer), 0);

        if (strncmp(buffer, "exit", 4) == 0)
            break;
    }

    close(newsock);
    close(sockfd);

    return 0;
}



//Client Side


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {

    int sockfd;
    struct sockaddr_in server;
    char buffer[1024];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sockfd, (struct sockaddr *)&server, sizeof(server));

    while (1) {

        printf("Client: ");
        fgets(buffer, sizeof(buffer), stdin);

        send(sockfd, buffer, strlen(buffer), 0);

        if (strncmp(buffer, "exit", 4) == 0)
            break;

        memset(buffer, 0, sizeof(buffer));

        read(sockfd, buffer, sizeof(buffer));
        printf("Server: %s\n", buffer);

        if (strncmp(buffer, "exit", 4) == 0)
            break;
    }

    close(sockfd);

    return 0;
}