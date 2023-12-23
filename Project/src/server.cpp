#include <bits/stdc++.h>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include "dbConnector/mysql_connector.h"
#include "enums.h"

#define MAXLINE 4096   // max text line length
#define SERV_PORT 3000 // port
#define LISTENQ 8      // maximum number of client connections

using namespace std;

int socketfd;
int child_process_running = 1;

void sig_chld(int signo);
void initServer();
int _register(MySQLOperations mysqlOps, string username, string password);

int main(int argc, char **argv)
{
    int listenfd, connfd, n;
    pid_t childpid;
    socklen_t clilen;
    char buf[MAXLINE];
    struct sockaddr_in cliaddr, servaddr;
    initServer();
    for (;;)
    {
        clilen = sizeof(cliaddr);
        // accept a connection
        connfd = accept(socketfd, (struct sockaddr *)&cliaddr, &clilen);
        if (connfd < 0)
        {
            exit(1);
        }
        std::cout << "\n[+]" << inet_ntoa(cliaddr.sin_addr) << ":" << ntohs(cliaddr.sin_port) << " - Connection accepted. Received request..." << std::endl;

        if ((childpid = fork()) == 0) // if it’s 0, it’s child process
        {
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(cliaddr.sin_addr), client_ip, INET_ADDRSTRLEN); // convert ip number from binary to string
            int client_port = ntohs(cliaddr.sin_port);                           // network order to host short

            printf("\n[+]%s\n", "Child created for dealing with client requests");

            // close listening socket
            close(socketfd);

            // Initialize MySQL operations
            MySQLOperations mysqlOps;
            if (!mysqlOps.connect(ipAddress, username, password, database))
            {
                std::cout << "[-]Failed to connect to MySQL database\n"
                          << std::endl;
                exit(1);
            }
            else
                std::cout << "[+]Connected to Database!\n";

            while (child_process_running)
            {
                n = recv(connfd, buf, MAXLINE, 0);
                if (n < 0)
                {
                    perror("Read error");
                    exit(1);
                }
                if (n == 0)
                {
                    std::cout << "[+]" << client_ip << ":" << client_port << " - Disconnected" << std::endl;
                    exit(0);
                }

                // Log the received message
                std::cout << "[+]Received message from " << client_ip << ":" << client_port << " - " << buf << std::endl;

                int cmd = (int)(buf[0] - '0');
                switch (cmd)
                {
                case REGISTER:
                {
                    char username[20], password[20], response[5];
                    int noargs = sscanf(buf, "%d %s %s", &cmd, username, password);
                    if (noargs == 3)
                    {
                        int result = _register(mysqlOps, username, password);
                        response[0] = '0' + result;
                        send(connfd, response, MAXLINE, 0);
                    }
                    else
                        printf("[-]Invalid register protocol! %s\n", buf);
                    break;
                }
                default:
                    printf("[-]Invalid protocol.\n\n");
                }

                // n = send(connfd, buf, n, 0);
                // if (n < 0)
                // {
                //     perror("Send error");
                //     exit(1);
                // }
                // std::cout << "[+]Sent message to " << client_ip << ":" << client_port << " - " << buf << std::endl;
                int i;
                for (i = 0; i < MAXLINE; i++)
                    buf[i] = '\0';
            }
            mysqlOps.disconnect();
        }
    }
    signal(SIGCHLD, sig_chld); // signal sent when child process ends
    // close socket of the server
    close(connfd);
}

void sig_chld(int signo)
{
    pid_t pid;
    int stat;
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        std::cout << "[+]Child " << pid << " terminated with status " << stat << std::endl;
    return;
}

void initServer()
{
    struct sockaddr_in servaddr;
    // creation of the socket
    // If socketfd<0 there was an error in the creation of the socket
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0)
    {
        std::cout << "[-]Error in creating the socket" << std::endl;
        exit(1);
    }
    std::cout << "[+]Server Socket is created." << std::endl;

    // preparation of the socket address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    // bind the socket
    if (bind(socketfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        std::cout << "[-]Error in binding." << std::endl;
        exit(1);
    }
    // listen to the socket by creating a connection queue, then wait for clients
    if (listen(socketfd, LISTENQ) == 0)
    {
        std::cout << "[+]Listening...." << std::endl;
    }
    else
    {
        std::cout << "[-]Error in binding." << std::endl;
    }
}

int _register(MySQLOperations mysqlOps, string username, string password)
{
    // char sql[100];
    // sprintf(sql, "INSERT INTO %s VALUE (%s, %s);", userTbl, username, password);
    string sql = "INSERT INTO users(username, password) VALUES ('" + username + "','" + password + "');";
    bool res = mysqlOps.insertRecords(sql);
    cout << "SQL query: " << sql << '\n';
    if (res)
        return SUCCESS;
    else
        return FAIL;
}