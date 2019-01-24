#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#define MAX_RECV_BUF 1600

int main(int argc, char *argv[])
{
    /* 
    *  sockfd is the socket descriptor which is an integer value
    *  filename is used to store the name of the file 
    */
    int sockfd;
    struct sockaddr_in serv_addr;
    char *file_name;
    
    /* Filename is passed as a command line argument */
    file_name=argv[1];

    /* 
    *  Socket() call takes three arguments:
    *  The family of protocol used/address family 
    *  Type of socket
    *  Protocol number or 0 for a given family and type 
    */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    /* 
    *  Socket call will return a socket descriptor on success which is an integer 
    *  And it will return '-1' for error
    */
    if (sockfd == -1) 
    {
        printf("\nerror calling socket\n");
        exit(1);
    }

    /* Populating the sockaddr_in struct with the following values */
    /* Assigning the AF_INET (Internet Protocol v4) address family */
    serv_addr.sin_family = AF_INET; 

    /* Populating the Server IP address with the value of the localhost IP address */
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    /* Converting the port number received from host byte order to network byte order */
    serv_addr.sin_port = htons(8000); 

    /* 
    *  Connect () takes three arguments:
    *  The sockect descriptor
    *  Server Address Structure
    *  And the size of Address 
    */
    if (connect(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
    /* Connect() returns '-1' on failure and 0 on success */
    {
        printf("\nError connecting to server\n");
        exit(1);
    }
    
    ssize_t recv_bytes;
    char recv_buff[MAX_RECV_BUF];

    printf("\nConnecting to server..\n\n");
    /* 
    *Sends the filename to the server 
    */
    send(sockfd,file_name,strlen(file_name),0);
    
    int ptr;
    char *newfile = "newfile.txt";
    mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;

    /* 
    * recv() receives the file contents from the server file
    */
    while((recv_bytes=recv(sockfd,recv_buff,MAX_RECV_BUF,0))>0)
    {
        if((ptr= open(newfile,O_WRONLY|O_CREAT,0644))<0)
        {
            printf("Error in writing to file\n");
        }
        if(strncmp(recv_buff,"INVALID",7)==0)
        {
            printf("File not found\n\n");
        }
        else
        {
            open(newfile,O_RDWR|O_CREAT|O_EXCL|O_APPEND,mode);
            write(ptr,recv_buff,recv_bytes);
            printf("%s\n",recv_buff);
            printf("\nNew file '%s' has been created and the above contents from server file have been copied to it.\n\n", newfile);
        }
    }
    close(sockfd);
    return 0;
}
