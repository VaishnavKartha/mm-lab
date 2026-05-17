#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<time.h>
#include<sys/time.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define PORT 8080
#define TOTAL_PACKTES 10
#define WINDOW_SIZE 4
#define BUFFER_SIZE 1024

int received[100];


int main(){
    int sockfd;
    struct sockaddr_in server,client;
    char buffer[BUFFER_SIZE]={0};
    socklen_t len=sizeof(client);

    sockfd=socket(AF_INET,SOCK_DGRAM,0);

    server.sin_family=AF_INET;
    server.sin_port=htons(PORT);
    server.sin_addr.s_addr=INADDR_ANY;

    bind(sockfd,(struct sockaddr  *)&server,sizeof(server));

    srand(time(0));

    while(1){

        memset(buffer,0,BUFFER_SIZE);

        int n = recvfrom(sockfd,buffer,BUFFER_SIZE,0,(struct sockaddr *)&client,&len);

        if(n > 0){

            int seq = atoi(buffer);

            if(received[seq] == 0){

                received[seq] = 1;

                int random=rand() % 100 ;

                if(random < 80){

                    printf("Server : Acknowledgement sent for packet %d\n\n",seq);

                    memset(buffer,0,BUFFER_SIZE);
                    sprintf(buffer,"%d",seq);
                    sendto(sockfd,buffer,strlen(buffer),0,(struct sockaddr *)&client,len);
                }else{

                    printf("Server : Ack for packet %d lost\n",seq);
                }
            }else{

                printf("Server : Duplicate Packet received\n");

                memset(buffer,0,BUFFER_SIZE);

                sprintf(buffer,"%d",seq);

                sendto(sockfd,buffer,strlen(buffer),0,(struct sockaddr*)&client,len);

                printf("Server : Ack resent for packet %d\n",seq);


            }

        }
    }

    close(sockfd);
    return 0;


}




#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<time.h>
#include<sys/time.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define PORT 8080
#define TOTAL_PACKTES 10
#define WINDOW_SIZE 4
#define BUFFER_SIZE 1024


int main(){

    int sockfd;
    struct sockaddr_in server;

    char buffer[BUFFER_SIZE]={0};
    socklen_t len=sizeof(server);

    sockfd=socket(AF_INET,SOCK_DGRAM,0);

    server.sin_family=AF_INET;
    server.sin_port=htons(PORT);
    server.sin_addr.s_addr=inet_addr("127.0.0.1");

    srand(time(0));

    struct timeval tv;
    tv.tv_sec=2;
    tv.tv_usec=0;

    int packets_ack=0;
    int base=1;
    int next_to_send=1;

    int acked[100] = {0};



    setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv));

    while(base <= TOTAL_PACKTES){

        while(next_to_send < base + WINDOW_SIZE && next_to_send <= TOTAL_PACKTES){

            memset(buffer,0,BUFFER_SIZE);

            printf("Client: Packet %d sent\n",next_to_send);

            sprintf(buffer,"%d",next_to_send);
            sendto(sockfd,buffer,strlen(buffer),0,(struct sockaddr*)&server,len);
            next_to_send++;


        }

        memset(buffer,0,BUFFER_SIZE);
        int n = recvfrom(sockfd,buffer,BUFFER_SIZE,0,(struct sockaddr *)&server,&len);
        
        if(n > 0){

            int ack = atoi(buffer);

            printf("Client : Recieved ACK for %d\n",ack);

            acked[ack] = 1;

            while(acked[base] == 1){
                base++;
            }

            for(int i=base;i<next_to_send;i++){

                if(acked[i] == 0){

                    sprintf(buffer,"%d",i);
                    sendto(sockfd,buffer,strlen(buffer),0,(struct sockaddr*)&server,len);

                    printf("Client: Retransmitting packet %d \n",i);
                }
            }


        }


    }

    printf("All packets transmitted\n");

    close(sockfd);

    return 0;



}