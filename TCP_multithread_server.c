#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include<signal.h>
#include <unistd.h>
#include<pthread.h>
#include<stdlib.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#define MAX_SEND_BUF 1600

void con_handler(void *);

int newsockfd, i;
int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in serv_addr;
    int address_len = sizeof(serv_addr);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("Error calling Socket");
        exit(1);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8000);

    if(bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        printf("error binding");
        exit(1);
    }

    if(listen(sockfd,5) < 0)
    {
        printf("error listening");
        exit(1);
    }
    
    while(1)
    {
        newsockfd = accept(sockfd,(struct sockaddr *) &serv_addr, (socklen_t*)&address_len);

        if (newsockfd < 0)
        {
            printf("error accepting");
            exit(1);
        }
        pthread_t child;
        if(pthread_create( &child, NULL ,  con_handler , (void*) newsockfd) < 0)
        {
            printf("error creating thread");
            exit(1);
        }
    }
    close(newsockfd);
}


void con_handler(void *sockfd)
{
    int sock = (int)sockfd;
    int n, buffer;
   
        char msg[1000];
        long data_len;
        data_len = recv(newsockfd,msg,sizeof(msg),0);                              //recieve the file name from client        
        if(data_len)
        {
            printf("\n\nClient connected to Multithread connection oriented server\n");
            printf("File name recieved: %s\n", msg);
        }
        int file;                                                                   //read the local file (server file)
        if((file = open(msg,O_RDWR)) == -1)
        {
            char* invalid = "INVALID";
            printf("File not found\n");
            printf("Client disconnected\n");
            printf("%s\n",strerror(errno));
            send(newsockfd,invalid,MAX_SEND_BUF,0);
        }
        else
        {
            printf("File opened successfully\n");
            ssize_t read_bytes;
            ssize_t sent_bytes;
            
            char send_buf[MAX_SEND_BUF];
            while( (read_bytes = read(file, send_buf, MAX_SEND_BUF)) > 0 )          //read the contents of file on server
            {
                if( (sent_bytes = send(newsockfd, send_buf, read_bytes, 0)) < read_bytes )  //send the contents of file to client
                {
                    printf("send error");
                    //return -1;
                }
            }
            printf("\n");
            close(file);
            printf("\nClient disconnected\n");
        }
        close(newsockfd);
    //pthread_exit(NULL);
}
