#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#define SERV_PORT 1255
#define MAXLINE 255
main()
{
    int sockfd, n;
    socklen_t len;
    char mesg[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) >= 0)
    {
        printf("Server is running at port %d", SERV_PORT);
    }
    else
    {
        perror("bind failed");
        return 0;
    }

    for (;;)
    {
        len = sizeof(cliaddr);
        n = recvfrom(sockfd, mesg, MAXLINE, 0, (struct sockaddr *)&cliaddr, &len);

        // Get client IP address and port
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(cliaddr.sin_addr), client_ip, INET_ADDRSTRLEN); // convert ip number from binary to string
        int client_port = ntohs(cliaddr.sin_port); // network order to host short
        printf("Receive from: %s: %d ", client_ip, client_port);

        printf(mesg);
        sendto(sockfd, mesg, n, 0, (struct sockaddr *)&cliaddr, len);
    }
    close(sockfd);
}
