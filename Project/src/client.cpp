#include <bits/stdc++.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "enums.h"

#define MAXLINE 4096   // max text line length
#define SERV_PORT 3000 // port

using namespace std;

int socketfd;

void connectToServer(char *ip);
void displayMenu();
int login (string *username);
void _register();

int main(int argc, char **argv)
{
    int choice = 0, re;
    char uname[MAXLINE];
    char sendline[MAXLINE], recvline[MAXLINE];
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <server_address>\n", argv[0]);
        exit(1);
    }

    connectToServer(argv[1]);

    displayMenu();

    // while (fgets(sendline, MAXLINE, stdin) != NULL)
    // {
    //     send(socketfd, sendline, strlen(sendline), 0);
    //     if (recv(socketfd, recvline, MAXLINE, 0) == 0)
    //     {
    //         // error: server terminated prematurely
    //         perror("The server terminated prematurely");
    //         exit(4);
    //     }
    //     std::cout << "[+]String received from the server: ";
    //     fputs(recvline, stdout);
    //     memset(sendline, 0, sizeof(sendline));
    //     memset(recvline, 0, sizeof(recvline));
    // }
    // exit(0);
    return 0;
}

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

void displayMenu()
{
    for (;;)
    {
        int loggedIn = 0;
        string username;
        while (1)
        {
            if (!loggedIn)
            {
                printf("\n====================================\n");
                printf("Welcome to Online Movie Ticket Reservation Application\n");
                printf("1. Register\n");
                printf("2. Login\n");
                printf("3. Exit\n");
                printf("Enter your choice: ");

                int choice;
                scanf("%d", &choice);
                getchar();

                switch (choice)
                {
                case 1:
                    _register();
                    break;

                case 2:
                    cout << "LOGIN\n";
                    // loggedIn = login(&username);
                    break;

                case 3:
                    printf("Goodbye!\n");
                    exit(0);
                    break;
                default:
                    printf("Invalid choice. Try again.\n\n");
                }
            }
            else
            {
                // printf("----------------------------------\n");
                // printf("Logged in as Student ID: %s\n", studentID);
                // printf("1. View Schedule\n");
                // printf("2. Logout\n");
                // printf("Enter your choice: ");

                // int choice;
                // scanf("%d", &choice);
                // getchar();
                // if (choice == 1)
                // {
                //     viewSchedule(studentID);
                // }
                // else if (choice == 2)
                // {
                //     loggedIn = 0;
                //     close(sockfd);
                //     printf("Logged out successfully.\n\n");
                //     exit(0);
                // }
                // else
                // {
                //     printf("Invalid choice. Try again.\n");
                // }
            }
        }
    }
}

void _register()
{
    char username[30], password[30];
    // string username, password;
    char sendline[MAXLINE], recvline[MAXLINE];

    cout << "Enter your username: ";
    cin >> username;
    cout << "Enter your password: ";
    cin >> password;

    // store values in sendline
    sprintf(sendline, "%d\n%s %s\n", REGISTER, username, password);
    // send request to server with protocol: "REGISTER\n<username> <password>\n"
    send (socketfd, sendline, strlen(sendline), 0);
    // eg.: 1 linhnt 123

    recv(socketfd, recvline, MAXLINE, 0);
    int auth = recvline[0] - '0';
    if (auth == SUCCESS){
        printf("Register successfully!\nNow you can login with this new account.\n");
    }
    else if (auth == FAIL)
    {
        printf("This username already existed!!!\n");
    } else {
        perror("The server terminated prematurely");
        exit(4);
    }
}