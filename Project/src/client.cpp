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

int socketfd, loggedIn;

void connectToServer(char *ip);
void displayMenu();
void displayUserMenu(string *username);
int login (string *username);
void _register();
void searchMovie();
void browseMovie();
void bookTicket();
void logout();

int main(int argc, char **argv)
{
    loggedIn = 0;
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <server_address>\n", argv[0]);
        exit(1);
    }

    connectToServer(argv[1]);

    displayMenu();
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

            string choice;
            cin >> choice;

            switch (choice[0])
            {
            case '1':
                _register();
                break;

            case '2':
                loggedIn = login(&username);
                break;

            case '3':
                printf("Goodbye!\n");
                exit(0);
                break;
            default:
                printf("Invalid choice. Try again.\n");
                break;
            }
            printf("-----------------------------\n");
        }
        else
        {
            //display menu coresponsing to each user mode
            switch (loggedIn)
            {
                case BUYER:
                    displayUserMenu(&username);
                    break;
                case SALER:
                    // Implement code here
                    break;
                case ADMIN:
                    // Implement code here
                    break;
                default:
                    printf("[-]Some error in get role of user!\n");
                    exit(1);
            }
            printf("-----------------------------\n");
        }
    }
}

void displayUserMenu(string *username)
{
    cout << "Hello: " << *username << "\n What do you want to do?\n";
    printf("1. Search movies by title\n");
    printf("2. Browse movies by type, cinema location and showtime\n");
    printf("3. Book movie ticket(s)\n");
    printf("4. Logout\n");
    printf("Enter your choice: ");

    string choice;
    cin >> choice;

    switch (choice[0])
    {
    case '1':
        searchMovie();
        break;
    case '2':
        //browseMovie();
        break;
    case '3':
        //bookTicket();
        break;
    case '4':
        logout();
        break;
    default:
        printf("Invalid choice. Try again.\n");
        break;
    }
}

void _register()
{
    char username[30], password[30];
    char sendline[MAXLINE], recvline[MAXLINE];

    cout << "Enter your username: ";
    cin >> username;
    cout << "Enter your password: ";
    cin >> password;

    // store values in sendline
    sprintf(sendline, "%d\n%s %s\n", REGISTER, username, password);
    // send request to server with protocol: "REGISTER\n<username> <password>\n"
    send (socketfd, sendline, strlen(sendline), 0);

    recv(socketfd, recvline, MAXLINE, 0);
    int auth = recvline[0] - '0';
    if (auth == SUCCESS){
        printf("Register successfully!\nNow you can login with this new account.\n");
    }
    else if (auth == FAIL)
    {
        printf("This username already existed!!!\n");
    } else {
        perror(recvline);
        exit(4);
    }
}

int login (string *user)
{
    char username[30], password[30];
    // string username, password;
    char sendline[MAXLINE], recvline[MAXLINE];

    cout << "Enter your username: ";
    cin >> username;
    cout << "Enter your password: ";
    cin >> password;

    // store values in sendline
    sprintf(sendline, "%d\n%s %s\n", LOGIN, username, password);
    // send request to server with protocol: "LOGIN\n<username> <password>\n"
    send (socketfd, sendline, strlen(sendline), 0);

    recv(socketfd, recvline, MAXLINE, 0);
    int auth = recvline[0] - '0';
    if (auth == BUYER || auth == SALER || auth == ADMIN){
        printf("You have logged in successfully with %s account!\n", (auth==BUYER?"buyer":(auth==SALER?"saler":"admin")));
        *user = username;
    }
    else if (auth == FAIL)
    {
        printf("Wrong username or password!!!\n");
    } else {
        perror(recvline);
        exit(4);
    }
    return auth;
}

void logout()
{
    loggedIn = 0;
    printf("Logged out successfully.\n\n");
}

void searchMovie()
{
    char title[255], sendline[MAXLINE], recvline[MAXLINE];
    int n;

    cout << "Enter the movie title you want to search:\n";
    cin >> title;

    sprintf(sendline, "%d\n%s\n", SEARCH, title);;
    send (socketfd, sendline, strlen(sendline), 0);

    while ((n = recv(socketfd, recvline, MAXLINE, 0)) > 0) {
            if (strcmp(recvline, "End")) {
                printf("%s", recvline);
                memset(recvline, 0, sizeof(recvline));
            } else {
                memset(recvline, 0, sizeof(recvline));
                break;
            }
        }
}