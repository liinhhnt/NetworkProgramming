#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#define MAXLINE 4096   /*max text line length*/
#define SERV_PORT 3002 /*port*/
#define LISTENQ 8      /*maximum number of client connections */

int main(int argc, char **argv)
{
    int listenfd, connfd;
    ssize_t n;
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

    printf("%s\n", "Server running...waiting for connections.");

    for (;;)
    {

        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        printf("%s\n", "Received request...");

        char filename[MAXLINE] = "o.txt";

        FILE *fo = fopen(filename, "w");
        if (fo == NULL)
        {
            perror("Error opening the output file");
            exit(1);
        }

        // printf ("File description of accept socket: %d\n", connfd);
        while ((n = recv(connfd, buf, MAXLINE, 0)) > 0)
        {
            fwrite(buf, 1, n, fo);
        }
        if (n < 0)
        {
            perror("Read error");
            exit(1);
        }
        close(connfd);
        fclose(fo);
    }
    // close listening socket
    close(listenfd);
}