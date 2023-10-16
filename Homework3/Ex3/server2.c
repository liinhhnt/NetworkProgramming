#include <arpa/inet.h> 
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int SERV_PORT = 1255;
int MAXLINE = 255;
main(int argc, char *argv[])
{
    if (argc != 1 && argc != 2)
    {
        fprintf(stderr, "Usage: server\n");
        fprintf(stderr, "Usage: server PORT\n");
        exit(EXIT_FAILURE);
    }
    else if (argc == 2)
    {
        SERV_PORT = atoi(argv[1]);
    }

    int sockfd, n;
    char mesg[MAXLINE];
    socklen_t len;
    struct sockaddr_in servaddr, cliaddr[2], tmpaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) >= 0)
    {
        printf("Server is running at port %d\n", SERV_PORT);
    }
    else
    {
        perror("bind failed");
        return 0;
    }

    // Waiting until 2 clients connect
    int numOfClient = 0;
    len = sizeof(struct sockaddr);
    while (numOfClient < 2)
    {
        memset(mesg, '\0', (strlen(mesg)));
        n = recvfrom(sockfd, mesg, MAXLINE, 0, (struct sockaddr *)&tmpaddr, &len);
        if (n < 0)
        {
            perror("\nEEError: ");
            continue;
        }

        // Add client to chat
        // if same client, skip
        if (numOfClient == 1 &&
            cliaddr[0].sin_addr.s_addr == tmpaddr.sin_addr.s_addr &&
            cliaddr[0].sin_port == tmpaddr.sin_port)
        {
            continue;
        }
        cliaddr[numOfClient] = tmpaddr;
        printf("Client %d joined: %s %d\n", numOfClient+1, inet_ntoa(tmpaddr.sin_addr), ntohs(tmpaddr.sin_port));
        numOfClient++;

        // Acknowledge client
        sprintf(mesg, "Status: Waiting for others...");
        n = sendto(sockfd, mesg, strlen(mesg), 0, (struct sockaddr*)&tmpaddr, sizeof(struct sockaddr));
         if (n < 0) {
            perror("\nEError: ");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        // Enough clients to start chatting
        if (numOfClient == 2) {
            sprintf(mesg, "Status: Enough clients. Let's chat!");
            for (int i=0; i<2; i++) {
                n = sendto(sockfd, mesg, strlen(mesg), 0, (struct sockaddr*)&cliaddr[i], len);
                if (n < 0) {
                    perror("\nEerror: ");
                    close(sockfd);
                    exit(EXIT_FAILURE);
                }
            }
        }
    }

    while (1) {
        memset(mesg, '\0', (strlen(mesg) + 1));

        n= recvfrom(sockfd, mesg, MAXLINE, 0, (struct sockaddr *)&tmpaddr, &len);
        if (n < 0)
            perror("\nError: ");
        else {
            mesg[n] = '\0';
            printf("[%s:%d]: %s\n", inet_ntoa(tmpaddr.sin_addr),
                   ntohs(tmpaddr.sin_port), mesg);
        }

        // Send mesg to client
        for (int i=0; i<2; i++) {
            if (cliaddr[i].sin_addr.s_addr == tmpaddr.sin_addr.s_addr &&
            cliaddr[i].sin_port == tmpaddr.sin_port) {
                continue;
            }
            n = sendto(sockfd, mesg, strlen(mesg), 0, (struct sockaddr*)&cliaddr[i], len);
            if (n < 0) {
                perror("\nError:");
                close(sockfd);
                exit(EXIT_FAILURE);        
            }
        } 
    }
    close(sockfd);
}
