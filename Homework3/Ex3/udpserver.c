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
    struct sockaddr_in servaddr, cliaddr1, cliaddr2, tmpaddr;
    int numOfClient = 0;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) >= 0) {
        printf("Server is running at port %d\n", SERV_PORT);
    }
    else {
        perror("bind failed");
        return 0;
    }

    for (;;)
    {
        char client1_ip[INET_ADDRSTRLEN], client2_ip[INET_ADDRSTRLEN];
        int client1_port, client2_port;

        len = sizeof(tmpaddr);
        n = recvfrom(sockfd, mesg, MAXLINE, 0, (struct sockaddr *)&tmpaddr, &len);

        // printf ("Number of clients: %d\n", numOfClient);
        switch (numOfClient)
        {
        case 0:
            inet_ntop(AF_INET, &(tmpaddr.sin_addr), client1_ip, INET_ADDRSTRLEN); // convert ip number from binary to string
            client1_port = ntohs(tmpaddr.sin_port);                               // network order to host short
            // Assign tmpaddr to cliaddr1
            memcpy(&cliaddr1, &tmpaddr, sizeof(tmpaddr));
            printf("The first client: %s: %d ", client1_ip, client1_port);
            numOfClient++;
            sendto(sockfd, mesg, n, 0, (struct sockaddr *)&cliaddr1, len);
            break;

        case 1:
            inet_ntop(AF_INET, &(tmpaddr.sin_addr), client2_ip, INET_ADDRSTRLEN); // convert ip number from binary to string
            client2_port = ntohs(tmpaddr.sin_port);                               // network order to host short
            // Assign tmpaddr to cliaddr1
            memcpy(&cliaddr2, &tmpaddr, sizeof(tmpaddr));
            printf("The second client: %s: %d ", client2_ip, client2_port);
            numOfClient++;
            break;
        default:
            // if message sent from client 1
            if (memcmp(&tmpaddr, &cliaddr1, sizeof(tmpaddr)) == 0) {
                sendto(sockfd, mesg, n, 0, (struct sockaddr *)&cliaddr2, len);
            } 
            else {
                sendto(sockfd, mesg, n, 0, (struct sockaddr *)&cliaddr1, len);
            }
            break;
        }
    }
    close(sockfd);
}
