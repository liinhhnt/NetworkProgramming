#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define MAXLINE 4096   /*max text line length*/
#define SERV_PORT 3002 /*port*/

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;
    char sendline[MAXLINE], recvline[MAXLINE];

    // basic check of the arguments
    // additional checks can be inserted
    if (argc != 3)
    {
        perror("Usage: TCPClient <IP address of the server> <sending file>");
        exit(1);
    }

    // Create a socket for the client
    // If sockfd<0 there was an error in the creation of the socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Problem in creating the socket");
        exit(2);
    }

    // Creation of the socket
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(SERV_PORT); // convert to big-endian order
    printf("Creating connection!");

    // Connection of the client to the socket
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("Problem in connecting to the server");
        exit(3);
    }
    printf("Done!\n");

    FILE *ptr = fopen(argv[2], "r");
    if (ptr == NULL) {
        perror("Error opening file on client");
        exit(1);
    }
    
    size_t n;
    char buffer[MAXLINE];
    
    while ((n = fread(buffer, sizeof(char), MAXLINE, ptr)) > 0) {
        send(sockfd, buffer, n, 0);
    }

    fclose(ptr);
    close(sockfd);
    exit(0);
}
