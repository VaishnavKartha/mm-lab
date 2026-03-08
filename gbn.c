#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define LOSS_PROBABILITY 30  // 30% chance of ACK loss

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    int ack;

    srand(time(0));  // Random seed for ACK simulation

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for client
    if (listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server: Waiting for connection...\n");

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
        memset(buffer, 0, BUFFER_SIZE);

        int valread = read(new_socket, buffer, BUFFER_SIZE);

        if (valread == 0)
            break;

        ack = atoi(buffer);

        printf("Server: Received packet %d\n", ack);

        // Simulate ACK loss
        if (rand() % 100 < LOSS_PROBABILITY)
        {
            printf("Server: ACK for packet %d lost!\n\n", ack);
        }
        else
        {
            sleep(1);  // Simulate processing delay

            printf("Server: ACK sent for packet %d\n\n", ack);

            memset(buffer, 0, BUFFER_SIZE);

            sprintf(buffer, "%d", ack);  // Send ACK

            send(new_socket, buffer, strlen(buffer) + 1, 0);
        }
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
#define TIMEOUT 3          // Timeout in seconds
#define WINDOW_SIZE 4      // Sliding window size
#define TOTAL_PACKETS 10   // Number of packets to send

int main()
{
    int sock = 0;
    struct sockaddr_in serv_addr;

    char buffer[BUFFER_SIZE] = {0};

    struct timeval tv;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(sock,
                (struct sockaddr *)&serv_addr,
                sizeof(serv_addr)) < 0)
    {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Client: Connected to server.\n");

    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;

    setsockopt(sock,
               SOL_SOCKET,
               SO_RCVTIMEO,
               (const char *)&tv,
               sizeof(tv));

    int base = 1;
    int next_to_send = 1;
    int ack, packets_acked = 0;

    while (packets_acked < TOTAL_PACKETS)
    {
        // Send all packets in the window
        while (next_to_send < base + WINDOW_SIZE &&
               next_to_send <= TOTAL_PACKETS)
        {
            memset(buffer, 0, BUFFER_SIZE);

            printf("Client: Sending packet %d\n", next_to_send);

            sprintf(buffer, "%d", next_to_send);

            send(sock, buffer, strlen(buffer) + 1, 0);

            next_to_send++;
        }

        // Wait for ACK
        memset(buffer, 0, BUFFER_SIZE);

        int valread = read(sock, buffer, BUFFER_SIZE);

        if (valread > 0)
        {
            ack = atoi(buffer);

            printf("Client: ACK received for packet %d\n", ack);

            // Slide the window if ACK corresponds to the base
            if (ack == base)
            {
                base = ack + 1;
                packets_acked = ack;
            }
        }
        else
        {
            printf("Client: Timeout! Retransmitting from packet %d...\n", base);

            next_to_send = base;  // Reset next_to_send to base for retransmission
        }
    }

    printf("Client: All packets sent successfully.\n");

    close(sock);

    return 0;
}