#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "enums.h"

#define MAXLINE 4096   /*max text line length*/
#define SERVER_PORT 3001

int sockfd;

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
    servaddr.sin_addr.s_addr = inet_addr(server_address);
    servaddr.sin_port = htons(SERVER_PORT);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Connection to the server failed");
        exit(1);
    }

    return sockfd;
}

int login(char (*studentID)[])
{
    char sendline[MAXLINE], recvline[MAXLINE];
    char username[20], pwd[20];

    printf("Enter you username (studentID): \n");
    scanf("%s", username);
    printf("Enter you password: \n");
    scanf("%s", pwd);
    
    // store values in sendline
    sprintf(sendline, "%d %s %s", LOGIN, username, pwd);
    send (sockfd, sendline, strlen(sendline), 0);
    // eg.: 1 linhnt 123

    recv(sockfd, recvline, MAXLINE, 0);
    int auth = recvline[0] - '0';
    if (auth == 1){
        printf("Login successfully!\n");
        strcpy(*studentID, username);
        return 1; // success
    }
    else if (auth == 0)
    {
        printf("Invalid username or password!!!\n");
        return 0; // fail
    } else {
        perror("The server terminated prematurely");
        exit(4);
    }
}

void viewSchedule (char studentID[]) {
    char weekday[20];
    char sendline[MAXLINE], recvline[MAXLINE];

    int n;
    printf("Enter the weekday (e.g., Monday, Tuesday) or type ALL to view week schedule: ");
    scanf("%s", weekday);
    getchar();
    if (!strcmp(weekday, "Monday") || !strcmp(weekday, "Tuesday") || !strcmp(weekday, "Wednesday") || !strcmp(weekday, "Thursday") || !strcmp(weekday, "Friday")) {
        sprintf(sendline, "%d %s", VIEWWEEKDAYSCHEDULE, weekday);
        // eg.: 2 Tuesday
        send (sockfd, sendline, strlen(sendline), 0);
        while ((n = recv(sockfd, recvline, MAXLINE, 0)) > 0) {
            if (strcmp(recvline, "End") != 0) {
                printf("%s", recvline);
                memset(recvline, 0, sizeof(recvline));
            } else {
                memset(recvline, 0, sizeof(recvline));
                break;
            }
        }
    }
    else if (strcmp(weekday, "ALL")==0) {
        sprintf(sendline, "%d %s", VIEWWEEKSCHEDULE, weekday);
        /// eg.: 3 ALL
        send (sockfd, sendline, strlen(sendline), 0);
        memset(recvline, 0, sizeof(recvline));
        while ((n = recv(sockfd, recvline, MAXLINE, 0)) > 0) {
            if (strcmp(recvline, "End") != 0) {
                printf("%s", recvline);
                memset(recvline, 0, sizeof(recvline));
            } else {
                memset(recvline, 0, sizeof(recvline));
                break;
            }
        }
    }
    else {
        printf ("Invalid weekday!!!\n\n");
        return;
    }
}

void displayMenu() {
    for (;;) {
        int loggedIn = 0;
        char studentID[20];
        while (1) {
            if (!loggedIn) {
                printf("====================================\n");
                printf("Welcome to Study Schedule Management\n");
                printf("1. Login\n");
                printf("2. Exit\n");
                printf("Enter your choice: ");

                int choice;
                scanf("%d", &choice);
                getchar();

                if (choice == 1) {
                    loggedIn = login(&studentID);
                }
                else if (choice == 2) {
                    printf("Goodbye!\n");
                    break;
                } 
                else {
                    printf("Invalid choice. Try again.\n\n");
                }
            }
            else {
                printf("----------------------------------\n");
                printf("Logged in as Student ID: %s\n", studentID);
                printf("1. View Schedule\n");
                printf("2. Logout\n");
                printf("Enter your choice: ");

                int choice;
                scanf("%d", &choice);
                getchar();
                if (choice == 1) {
                    viewSchedule(studentID);
                } else if (choice == 2) {
                    loggedIn = 0;
                    close(sockfd);
                    printf("Logged out successfully.\n\n");
                    exit(0);
                } else {
                    printf("Invalid choice. Try again.\n");
                }
            }
        }
    }
}



int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <server_address>\n", argv[0]);
        exit(1);
    }

    const char *server_address = argv[1];
    sockfd = connectToServer(server_address);

    displayMenu();

    return 0;
}
