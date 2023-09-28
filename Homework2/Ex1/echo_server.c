#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define MAXLINE 4096   /*max text line length*/
#define SERV_PORT 3001 /*port*/
#define LISTENQ 8      /*maximum number of client connections */

int main(int argc, char **argv)
{
    int listenfd, connfd, n;
    pid_t childpid;
    socklen_t clilen;
    char buf[MAXLINE];
    struct sockaddr_in cliaddr, servaddr;

    // creation of the socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    // preparation of the socket address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);

    printf("%s\n", "Server running...waiting for connections...");

    for (;;)
    {

        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        printf("%s\n", "Received request...");

        // Get client IP address and port
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(cliaddr.sin_addr), client_ip, INET_ADDRSTRLEN); // convert ip number from binary to string
        int client_port = ntohs(cliaddr.sin_port); // network order to host short
        printf("Client connected from IP: %s, Port: %d\n", client_ip, client_port);

        // Create a string containing the client IP and port
        char response[MAXLINE];
        snprintf(response, MAXLINE, "Your client IP: %s, Port: %d\n", client_ip, client_port);
        
        // Send the response to the client:
        send(connfd, response, strlen(response), 0);

        while ((n = recv(connfd, buf, MAXLINE, 0)) > 0)
        {
            printf("%s", "String received from and resent to the client:");
            puts(buf);
            send(connfd, buf, n, 0);
        }

        if (n < 0)
        {
            perror("Read error");
            exit(1);
        }
        close(connfd);
    }
    // close listening socket
    close(listenfd);
}