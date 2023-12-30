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
#include "DBstruct.h"

#define MAXLINE 4096   // max text line length
#define SERV_PORT 3000 // port
#define LISTENQ 8      // maximum number of client connections

using namespace std;

int socketfd;
int child_process_running = 1;

void sig_chld(int signo);
void initServer();
void log_recv_msg(string client_ip, int client_port, string buf);
void log_send_msg(int connfd, string client_ip, int client_port, char response[]);
int _register(MySQLOperations *mysqlOps, string username, string password);
int login(MySQLOperations *mysqlOps, string username, string password);
void searchMovie(int connfd, string client_ip, int client_port, MySQLOperations *mysqlOps, string title);
void getListType(int connfd, string client_ip, int client_port, MySQLOperations *mysqlOps);
void getListCinema(int connfd, string client_ip, int client_port, MySQLOperations *mysqlOps);
void browseMovie(int connfd, string client_ip, int client_port, MySQLOperations *mysqlOps, string typeId, string movieId, string weekday);

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
        cout << "\n[+]" << inet_ntoa(cliaddr.sin_addr) << ":" << ntohs(cliaddr.sin_port) << " - Connection accepted. Received request..." << endl;

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
                cout << "[-]Failed to connect to MySQL database\n"
                     << endl;
                exit(1);
            }
            else
                cout << "[+]Connected to Database!\n";

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
                    cout << "[+]" << client_ip << ":" << client_port << " - Disconnected" << endl;
                    exit(0);
                }

                log_recv_msg(client_ip, client_port, buf);

                int cmd = 0;

                for (int i=0; i<strlen(buf); i++)
                {
                    char digit = buf[i];
                    if (digit == '\n') break;
                    cmd = cmd * 10 + (int)(digit - '0');
                }
                switch (cmd)
                {
                case REGISTER:
                {
                    char username[20], password[20], response[50];
                    int noargs = sscanf(buf, "%d\n%s %s\n", &cmd, username, password);
                    if (noargs == 3)
                    {
                        int result = _register(&mysqlOps, username, password);
                        response[0] = '0' + result;
                        response[1] = '\0';
                        log_send_msg(connfd, client_ip, client_port, response);
                    }
                    else
                    {
                        printf("[-]Invalid register protocol! %s\n", buf);
                        sprintf(response, "Invalid register protocol!\n");
                        log_send_msg(connfd, client_ip, client_port, response);
                    }
                    break;
                }
                case LOGIN:
                {
                    char username[20], password[20], response[50];
                    int noargs = sscanf(buf, "%d\n%s %s\n", &cmd, username, password);
                    if (noargs == 3)
                    {
                        int result = login(&mysqlOps, username, password);
                        response[0] = '0' + result;
                        response[1] = '\0';
                        log_send_msg(connfd, client_ip, client_port, response);
                    }
                    else
                    {
                        printf("[-]Invalid login protocol! %s\n", buf);
                        sprintf(response, "Invalid login protocol!\n");
                        log_send_msg(connfd, client_ip, client_port, response);
                    }
                    break;
                }
                case SEARCH:
                {
                    char title[255], response[50];
                    int noargs = sscanf(buf, "%d\n%s\n", &cmd, title);
                    if (noargs == 2)
                    {
                        searchMovie(connfd, client_ip,client_port, &mysqlOps, title);
                    }
                    else
                    {
                        printf("[-]Invalid search protocol! %s\n", buf);
                        sprintf(response, "Invalid search protocol!\n");
                        log_send_msg(connfd, client_ip, client_port, response);
                    }
                    break;
                }
                case GET_LIST_TYPE:
                    getListType(connfd, client_ip,client_port, &mysqlOps);
                    break;
                case GET_LIST_CINEMA:
                    getListCinema(connfd, client_ip,client_port, &mysqlOps);
                    break;
                case BROWSE:
                {
                    char typeId[25], cinemaId[25], weekday[25], response[50];
                    int noargs = sscanf(buf, "%d\n%s %s %s\n", &cmd, typeId, cinemaId, weekday);
                    if (noargs == 4)
                    {
                        browseMovie(connfd, client_ip,client_port, &mysqlOps, typeId, cinemaId, weekday);
                    }
                    else
                    {
                        printf("[-]Invalid browse protocol! %s\n", buf);
                        sprintf(response, "Invalid browse protocol!\n");
                        log_send_msg(connfd, client_ip, client_port, response);
                    }
                    break;
                }
                default:
                {
                    char response[50];
                    printf("[-]Invalid protocol: wrong command code\n\n");
                    sprintf(response, "Invalid  protocol: wrong command code\n");
                    log_send_msg(connfd, client_ip, client_port, response);
                }
                }

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

void log_recv_msg(string client_ip, int client_port, string buf)
{
    cout << "[+]Received message from " << client_ip << ":" << client_port << " - " << buf << endl;
}

void log_send_msg(int connfd, string client_ip, int client_port, char response[])
{
    int n = send(connfd, response, MAXLINE, 0);
    if (n < 0)
    {
        perror("Send error");
        exit(1);
    }
    cout << "[+]Sent message to " << client_ip << ":" << client_port << " - " << response << '\n';
}

void sig_chld(int signo)
{
    pid_t pid;
    int stat;
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        cout << "[+]Child " << pid << " terminated with status " << stat << endl;
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
        cout << "[-]Error in creating the socket" << endl;
        exit(1);
    }
    cout << "[+]Server Socket is created." << endl;

    // preparation of the socket address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    // bind the socket
    if (bind(socketfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        cout << "[-]Error in binding." << endl;
        exit(1);
    }
    // listen to the socket by creating a connection queue, then wait for clients
    if (listen(socketfd, LISTENQ) == 0)
    {
        cout << "[+]Listening...." << endl;
    }
    else
    {
        cout << "[-]Error in binding." << endl;
    }
}

int _register(MySQLOperations *mysqlOps, string username, string password)
{
    string sql = "INSERT INTO users(username, password) VALUES ('" + username + "','" + password + "');";
    bool res = (*mysqlOps).insertRecords(sql);
    cout << "SQL query: " << sql << '\n';
    if (res)
        return SUCCESS;
    else
        return FAIL;
}

// output: integer indicate usermode
int login(MySQLOperations *mysqlOps, string username, string password)
{
    string sql = "SELECT * FROM users WHERE username = '" + username + "' AND password = '" + password + "';";
    cout << "SQL query: " << sql << '\n';
    int res = (*mysqlOps).checkRoleUser(sql);
    if (res)
        return res;
    else
        return FAIL;
}

void searchMovie(int connfd, string client_ip, int client_port, MySQLOperations *mysqlOps, string title)
{
     // Chuyển chuỗi title về dạng chữ thường
    std::transform(title.begin(), title.end(), title.begin(), ::tolower);
    string sql = "SELECT * FROM movies m JOIN types t WHERE LOWER(movieName) like '%" + title + "%' and m.typeId=t.typeId;";
    cout << "SQL query: " << sql << '\n';

    struct MovieList movieList;
    initMovieList(&movieList);
    (*mysqlOps).getListMovies(&movieList, sql);

    char sendline[MAXLINE];
    char END[10] = "End";

    printf("[+]Begin send response...\n");

    sprintf(sendline, "Number of movies matched: %d\n", movieList.size);
    log_send_msg(connfd, client_ip, client_port, sendline);

    send(connfd, "===============================================================================================================================================\n", MAXLINE, 0);
    sprintf(sendline, "%-10s | %-25s | %-10s | %-10s | %-75s |\n", "MovieId", "Movie name", "Type", "Duration", "Description");
    log_send_msg(connfd, client_ip, client_port, sendline);
    memset(sendline, 0, strlen(sendline));
    send(connfd, "-----------------------------------------------------------------------------------------------------------------------------------------------|\n", MAXLINE, 0);
    for (int i = 0; i < movieList.size; i++) {
        struct Movie movie = movieList.movies[i];
            sprintf(sendline, "%-10d | %-25s | %-10s | %-10s | %-75s |\n",
                movie.movieId, movie.movieName, movie.typeName, movie.duration, movie.describtion);
            log_send_msg(connfd, client_ip, client_port, sendline);
            memset(sendline, 0, MAXLINE);
    }
    send(connfd, "===============================================================================================================================================\n", MAXLINE, 0);
    usleep(500000);
    sprintf(sendline, "%s", END);
    log_send_msg(connfd, client_ip, client_port, sendline);
    memset(sendline, 0, strlen(sendline));
    freeMovieList(&movieList);

    printf("[+]Send completely!\n");
}

void getListType(int connfd, string client_ip, int client_port, MySQLOperations *mysqlOps)
{
    string sql = "SELECT * FROM types;";
    cout << "SQL query: " << sql << '\n';

    struct TypeList typeList; 
    initTypeList(&typeList); 
    (*mysqlOps).getListTypes(&typeList, sql); 

    char sendline[MAXLINE];
    char END[10] = "End";

    printf("[+]Begin send response...\n");

    sprintf(sendline, "Number of available movie types: %d\n", typeList.size); // 
    log_send_msg(connfd, client_ip, client_port, sendline);

    send(connfd, "========================================\n", MAXLINE, 0);
    sprintf(sendline, "%-10s | %-25s |\n", "TypeId", "Type name"); 
    log_send_msg(connfd, client_ip, client_port, sendline);
    memset(sendline, 0, strlen(sendline));
    send(connfd, "---------------------------------------|\n", MAXLINE, 0);
    for (int i = 0; i < typeList.size; i++) {
        struct Type type = typeList.types[i]; 
        sprintf(sendline, "%-10d | %-25s |\n", 
            type.typeId, type.typeName);
        log_send_msg(connfd, client_ip, client_port, sendline);
        memset(sendline, 0, MAXLINE);
    }
    send(connfd, "========================================\n", MAXLINE, 0);
    usleep(500000);
    sprintf(sendline, "%s", END);
    log_send_msg(connfd, client_ip, client_port, sendline);
    memset(sendline, 0, strlen(sendline));
    freeTypeList(&typeList); 

    printf("[+]Send completely!\n");
}

void getListCinema(int connfd, string client_ip, int client_port, MySQLOperations *mysqlOps)
{
    string sql = "SELECT * FROM cinemas;";
    cout << "SQL query: " << sql << '\n';

    struct CinemaList cinemaList; 
    initCinemaList(&cinemaList); 
    (*mysqlOps).getListCinemas(&cinemaList, sql); 

    char sendline[MAXLINE];
    char END[10] = "End";

    printf("[+]Begin send response...\n");

    sprintf(sendline, "Number of available cinemas: %d\n", cinemaList.size); // 
    log_send_msg(connfd, client_ip, client_port, sendline);

    send(connfd, "====================================================================\n", MAXLINE, 0);
    sprintf(sendline, "%-10s | %-25s | %-25s |\n", "CinemaId", "Cinema name", "Location"); 
    log_send_msg(connfd, client_ip, client_port, sendline);
    memset(sendline, 0, strlen(sendline));
    send(connfd, "-------------------------------------------------------------------|\n", MAXLINE, 0);
    for (int i = 0; i < cinemaList.size; i++) {
        struct Cinema cinema = cinemaList.cinemas[i]; 
        sprintf(sendline, "%-10d | %-25s | %-25s |\n", 
            cinema.cinemaId, cinema.cinemaName, cinema.location);
        log_send_msg(connfd, client_ip, client_port, sendline);
        memset(sendline, 0, MAXLINE);
    }
    send(connfd, "====================================================================\n", MAXLINE, 0);
    usleep(500000);
    sprintf(sendline, "%s", END);
    log_send_msg(connfd, client_ip, client_port, sendline);
    memset(sendline, 0, strlen(sendline));
    freeCinemaList(&cinemaList); 

    printf("[+]Send completely!\n");
}

void browseMovie(int connfd, string client_ip, int client_port, MySQLOperations *mysqlOps, string typeId, string cinemaId, string weekday)
{
    string sql = "SELECT m.movieId, m.movieName, t.typeName, c.cinemaName, s.weekday, s.startTime, s.endTime, s.showTimeId, s.roomId, s.seatMap, s.noOfEmptySeats FROM movies m, cinemas c, types t, showtimes s WHERE m.movieId = s.movieId AND m.typeId = t.typeId AND s.cinemaId = c.cinemaId";
    if (typeId != "ALL") sql += " AND t.typeId = " + typeId;
    if (cinemaId != "ALL") sql += " AND c.cinemaId = " + cinemaId;
    if (weekday != "All") sql += " AND s.weekday = " + weekday;
    sql += ";";
    cout << "SQL query: " << sql << '\n';

    struct ShowTimeList browseList; 
    initShowTimeList(&browseList); 
    (*mysqlOps).getListShowTimes(&browseList, sql); 

    char sendline[MAXLINE];
    char END[10] = "End";

    printf("[+]Begin send response...\n");

    sprintf(sendline, "Number of movies matched with typeId = %s, cinemaId = %s and showtime = %s: %d\n", typeId.c_str(), cinemaId.c_str(), weekday.c_str(), browseList.size); 
    log_send_msg(connfd, client_ip, client_port, sendline);

    send(connfd, "==================================================================================================================\n", MAXLINE, 0);
    sprintf(sendline, "| %-7s | %-25s | %-10s | %-20s | %-10s | %-10s | %-10s |\n", "MovieId", "Movie name", "Type name", "Cinema name", "Showtime", "Start time", "End time"); 
    log_send_msg(connfd, client_ip, client_port, sendline);
    memset(sendline, 0, strlen(sendline));
    send(connfd, "-----------------------------------------------------------------------------------------------------------------|\n", MAXLINE, 0);
    for (int i = 0; i < browseList.size; i++) {
        struct ShowTime browse = browseList.showTimes[i]; 
        sprintf(sendline, "| %-7d | %-25s | %-10s | %-20s | %-10s | %-10s | %-10s |\n", 
            browse.movieId, browse.movieName, browse.typeName, browse.cinema, browse.weekday, browse.startTime, browse.endTime);
        log_send_msg(connfd, client_ip, client_port, sendline);
        memset(sendline, 0, MAXLINE);
    }
    send(connfd, "==================================================================================================================\n", MAXLINE, 0);
    usleep(500000);
    sprintf(sendline, "%s", END);
    log_send_msg(connfd, client_ip, client_port, sendline);
    memset(sendline, 0, strlen(sendline));
    freeShowTimeList(&browseList); 

    printf("[+]Send completely!\n");
}   
