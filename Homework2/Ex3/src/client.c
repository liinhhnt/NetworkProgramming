#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAXLINE 4096   /*max text line length*/
#define SERVER_PORT 3000

// Function to create a socket and connect to the server
int connectToServer(const char *server_address) {
    int sockfd;
    struct sockaddr_in servaddr;

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Configure the server address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    servaddr.sin_addr.s_addr = inet_addr(server_address);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Connection to the server failed");
        exit(1);
    }

    return sockfd;
}

// Function to send a request to the server
void sendRequest(int sockfd, const char *request) {
    // Send the request to the server
    if (send(sockfd, request, strlen(request), 0) < 0) {
        perror("Send failed");
        exit(1);
    }
}

// Function to receive a response from the server
void receiveResponse(int sockfd, char *response, int max_response_size) {
    // Receive the response from the server
    int bytes_received = recv(sockfd, response, max_response_size - 1, 0);
    if (bytes_received < 0) {
        perror("Receive failed");
        exit(1);
    }
    printf("%s", "String received from the server: ");
    fputs(response, stdout);
    response[bytes_received] = '\0'; // Null-terminate the received data
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <server_address>\n", argv[0]);
        exit(1);
    }

    const char *server_address = argv[1];
    int sockfd;
    char sendline[MAXLINE], recvline[MAXLINE];

    // Connect to the server using the provided server address
    sockfd = connectToServer(server_address);

    // Rest of the code remains the same as in the previous example
    sendRequest(sockfd, "Nguyen Thi Linh 20200349");
    while (fgets(sendline, MAXLINE, stdin) != NULL)
    {

        send(sockfd, sendline, strlen(sendline), 0);

        if (recv(sockfd, recvline, MAXLINE, 0) == 0)
        {
            // error: server terminated prematurely
            perror("The server terminated prematurely");
            exit(4);
        }
        printf("%s", "String received from the server: ");
        fputs(recvline, stdout);
    }

    return 0;
}
