#include <iostream>
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

int socketfd;
int child_process_running = 1;

void sig_chld(int signo)
{
    pid_t pid;
    int stat;
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        std::cout << "child " << pid << " terminated with status " << stat << std::endl;
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
        std::cout << "[-]Error in connection." << std::endl;
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

int main(int argc, char **argv)
{
    int listenfd, connfd, n, state;
    pid_t childpid;
    socklen_t clilen;
    char buf[MAXLINE];
    struct sockaddr_in cliaddr, servaddr;
    initServer();
    for (;;)
    {
        clilen = sizeof(cliaddr);
        connfd = accept(socketfd, (struct sockaddr *)&cliaddr, &clilen);
        if (connfd < 0)
        {
            exit(1);
        }
        std::cout << "\n[+]" << inet_ntoa(cliaddr.sin_addr) << ":" << ntohs(cliaddr.sin_port) << " - Connection accepted. Received request..." << std::endl;
        if ((childpid = fork()) == 0)
        {
            // close listening socket
            close(socketfd);
             // Initialize MySQL operations
            MySQLOperations mysqlOps;
            if (!mysqlOps.connect(ipAddress, username, password, database)) {
                std::cout << "Failed to connect to MySQL database." << std::endl;
                exit(1);
            }
            else std::cout << "Connected to Database!\n";

            while (child_process_running)
            {
                n = recv(connfd, &state, sizeof(state), 0);
                if (n < 0)
                {
                    perror("Read error");
                    exit(1);
                }
                if (n == 0)
                {
                    std::cout << "[+]" << inet_ntoa(cliaddr.sin_addr) << ":" << ntohs(cliaddr.sin_port) << " - Disconnected" << std::endl;
                    exit(0);
                }
                state = ntohl(state);
                //process the data here
                      //test
                mysqlOps.selectAllRecords("SELECT * FROM cinemas;");
                mysqlOps.selectAllRecords("SELECT * FROM users;");
                
            }
            mysqlOps.disconnect();
        }
    }
    close(connfd);
}


