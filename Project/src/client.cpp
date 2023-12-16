#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXLINE 4096   // max text line length
#define SERV_PORT 3000 // port

int socketfd;

void connectToServer(char *ip)
{
    struct sockaddr_in servaddr;
    // Create a socket for the client
    // If socketfd<0 there was an error in the creation of the socket
    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("[-]Problem in creating the socket");
        exit(2);
    }
    std::cout << "[+]Client Socket is created." << std::endl;
    // Creation of the socket
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port = htons(SERV_PORT); // convert to big-endian order
    // Connection of the client to the socket
    if (connect(socketfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("[-]Problem in connecting to the server");
        exit(3);
    }
    std::cout << "[+]Connected to Server." << std::endl;
}

int main(int argc, char **argv)
{
    int choice = 0, re;
    char uname[MAXLINE];
    char sendline[MAXLINE], recvline[MAXLINE];
    if (argc != 2)
    {
        std::cerr << "[-]Missing IP address of the server" << std::endl;
        exit(1);
    }
    connectToServer(argv[1]);
    // memset(&buffer, 0, sizeof(buffer));
    while (fgets(sendline, MAXLINE, stdin) != NULL)
    {
        send(socketfd, sendline, strlen(sendline), 0);
        if (recv(socketfd, recvline, MAXLINE, 0) == 0)
        {
            // error: server terminated prematurely
            perror("The server terminated prematurely");
            exit(4);
        }
        std::cout << "String received from the server: ";
        fputs(recvline, stdout);
        memset(sendline, 0, sizeof(sendline));
    }
    exit(0);
    return 0;
}