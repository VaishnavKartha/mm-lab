#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    int ack_prob = 70;   // 70% chance of sending ACK

    srand(time(0));      // Random seed for ACK simulation

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Define server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket to port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server: Waiting for connection...\n");

    // Accept connection from client
    if ((new_socket = accept(server_fd,
                             (struct sockaddr *)&address,
                             (socklen_t *)&addrlen)) < 0)
    {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    printf("Server: Connection established.\n");

    while (1)
    {
        // Receive packet from client
        int valread = read(new_socket, buffer, BUFFER_SIZE);

        if (valread == 0)
            break;

        printf("Server: Received packet - %s\n", buffer);

        // Simulate ACK or loss
        if (rand() % 100 < ack_prob)
        {
            printf("Server: ACK sent for packet %s\n\n", buffer);
            send(new_socket, "ACK", strlen("ACK"), 0);
        }
        else
        {
            printf("Server: ACK lost for packet %s\n\n", buffer);
        }

        memset(buffer, 0, BUFFER_SIZE);   // Clear buffer
    }

    close(new_socket);
    close(server_fd);

    return 0;
}




//client

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define TIMEOUT 3      // Timeout in seconds

int main()
{
    int sock = 0;
    struct sockaddr_in serv_addr;

    char buffer[BUFFER_SIZE] = {0};
    char packet[50];

    struct timeval tv;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IP address
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address or Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(sock,
                (struct sockaddr *)&serv_addr,
                sizeof(serv_addr)) < 0)
    {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Client: Connected to server.\n");

    // Set socket timeout
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;

    setsockopt(sock,
               SOL_SOCKET,
               SO_RCVTIMEO,
               (const char *)&tv,
               sizeof(tv));

    int packet_number = 1;

    while (packet_number <= 5)
    {
        sprintf(packet, "%d", packet_number);

        printf("Client: Sending packet %d...\n", packet_number);

        send(sock, packet, strlen(packet), 0);

        // Wait for ACK
        int valread = read(sock, buffer, BUFFER_SIZE);

        if (valread > 0 && strcmp(buffer, "ACK") == 0)
        {
            printf("Client: ACK received for packet %d\n\n", packet_number);
            packet_number++;
        }
        else
        {
            printf("Client: Timeout! Retransmitting packet %d...\n\n",
                   packet_number);
        }

        memset(buffer, 0, BUFFER_SIZE);   // Clear buffer
    }

    printf("Client: All packets sent successfully.\n");

    close(sock);

    return 0;
}