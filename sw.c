#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<time.h>

#define PORT 8080

typedef struct packet{
    char data[1024];
}Packet;

typedef struct frame{
    int frame_kind;
    int sq_no;
    int ack;
    Packet packet;
}Frame;

int main(){
    int sockfd;
    struct sockaddr_in server,client;
    socklen_t len=sizeof(client);

    Frame frame_recv,frame_snd;

    srand(time(0));

    sockfd=socket(AF_INET,SOCK_DGRAM,0);

    server.sin_family=AF_INET;
    server.sin_port=htons(PORT);
    server.sin_addr.s_addr=INADDR_ANY;

    bind(sockfd,(struct sockaddr*)&server,sizeof(server));

    int expected_frame=0;

    while(1){

        int n = recvfrom(sockfd,&frame_recv,sizeof(Frame),0,(struct sockaddr *)&client,&len);

        if( n > 0){

            if(frame_recv.frame_kind == 1 && frame_recv.sq_no == expected_frame ){

                printf("[Server] Frame %d received : %s\n",frame_recv.sq_no,frame_recv.packet.data);

                int random=rand() % 100 ;

                if(random < 80 ){
                    frame_snd.frame_kind = 0;
                    frame_snd.ack=expected_frame + 1;

                    sendto(sockfd,&frame_snd,sizeof(Frame),0,(struct sockaddr *)&client,len);

                    printf("[Server] Ack sent for frame %d (Random=%d)\n ",frame_recv.sq_no,random);

                    expected_frame ++ ;
                }

                else{
                    printf("[Server] Ack Lost\n");
                }


            }else{
                printf("[Server] Duplicate Frame %d Received\n",frame_recv.sq_no);

                frame_snd.frame_kind = 0;
                frame_snd.ack=expected_frame;


                sendto(sockfd,&frame_snd,sizeof(Frame),0,(struct sockaddr*)&client,len);

                printf("Ack Resent\n");
            }
        }
    }

    close(sockfd);

    return 0 ;
}













//client
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<string.h>
#include<sys/time.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define PORT 8080

typedef struct packet{
    char data[1024];
}Packet;

typedef struct frame{
    int frame_kind;
    int sq_no;
    int ack;
    Packet packet;
}Frame;

int main(){
    int sockfd;
    struct sockaddr_in server;
    socklen_t len=sizeof(server);

    int frame_id=0;

    srand(time(0));

    sockfd=socket(AF_INET,SOCK_DGRAM,0);

    struct timeval tv;

    tv.tv_sec=2;
    tv.tv_usec=0;

    setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv));

    server.sin_family=AF_INET;
    server.sin_port=htons(PORT);
    server.sin_addr.s_addr=inet_addr("127.0.0.1");

    Frame frame_recv,frame_snd;

    while(1){

        frame_snd.frame_kind=1;
        frame_snd.sq_no=frame_id;

        printf("\nEnter the data: ");
        fgets(frame_snd.packet.data,sizeof(frame_snd.packet.data),stdin);

       while(1){

        int random=rand()%100;

        if(random < 80){

            printf("[Client] Frame %d sent (random=%d)\n",frame_id,random);

            sendto(sockfd,&frame_snd,sizeof(Frame),0,(struct sockaddr *)&server,len);


        }else{
            printf("[Client] Frame Lost\n");
        }


        int n = recvfrom(sockfd,&frame_recv,sizeof(Frame),0,(struct sockaddr *)&server,&len);

        if(n>0){

            if(frame_recv.ack == frame_id + 1){
                printf("[Client] ACK %d received\n",frame_recv.ack);
                frame_id ++;
                break;


            }else{
                printf("[Client] ACK lost retransmitting\n");
            }
        }
       }



        
    }

    close(sockfd);

    return 0;
}