#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#include "enums.h"
#include "users/user.h"
#include "courses/courses.h"
#include "registrations/registration.h"

struct UserList userList;
struct CourseList courseList;
struct RegistrationList registrationList;

#define MAXLINE 4096   /*max text line length*/
#define SERV_PORT 3001 /*port*/
#define LISTENQ 8      /*maximum number of client connections */

int login(char * studentID, char * username, char * password) {
    int auth = authenticateUser(&userList, username, password);
    if (auth) {
        printf("Login successfully!\n");
        strcpy(studentID, username);
        return 1;
    } else {
        printf("Login failed!\n");
        return 0;
    }
}

void viewSchedule(char studentID[], char *weekday) {
    if (!strcmp(weekday, "Monday") || !strcmp(weekday, "Tuesday") || !strcmp(weekday, "Wednesday") || !strcmp(weekday, "Thursday") || !strcmp(weekday, "Friday")) {
        displayCourseListByStudentIDAndWeekDay(&registrationList, &courseList, studentID, weekday);
    }
    else if (strcmp(weekday, "ALL")==0) {
        displayALLCoursesByStudentID (&registrationList, &courseList, studentID);
    }
    else printf ("Invalid weekday!!!\n\n");
}

void init() {
    // Initialize user list and read user data
    initUserList(&userList);
    readUserList(&userList, FilePath[USER]);

    // Initialize course list and read course schedule data
    initCourseList(&courseList);
    readCourseSchedule(&courseList, FilePath[COURSE]);

    //Initialize registration list and read student registrations
    initRegistrationList(&registrationList);
    readStudentRegistrations(&registrationList, FilePath[REGISTRATION]);
}

int main(int argc, char **argv) {
    int listenfd, connfd, n;
    pid_t childpid;
    socklen_t clilen;
    char buf[MAXLINE], sendline[MAXLINE];
    struct sockaddr_in cliaddr, servaddr;
    char studentID[20];
    char END[10] = "End"; 

    // creation of the socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    // preparation of the socket address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);

    printf("%s\n", "Server running...waiting for connections.");

    // initialize data 
    init();
    
    for (;;){

        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        printf("%s\n", "Received request...");

        int result, auth;
        while ((n = recv(connfd, buf, MAXLINE, 0)) > 0) {
            char cmd = buf[0];
            char weekday[20];
            switch (cmd)
            {
            case '1':
                char username[20], password[20];
                result = sscanf(buf, "%c %s %s", &cmd, username, password);
                if (result == 3) {
                    auth = login(studentID, username, password);
                    sendline[0] = '0' + auth;
                    send(connfd, sendline, MAXLINE, 0);
                } else printf("Invalid username or password! %s\n", buf);
                break;
            case '2':
                result = sscanf(buf, "%c %s", &cmd, weekday);
                if (result == 2) {
                    viewSchedule(studentID, weekday);
                    FILE *file = fopen("timetable.txt", "r"); // file receives the timetable.      
                    while((n = fgets(sendline, MAXLINE, file)) != NULL) {
                        n = send(connfd, sendline, strlen(sendline), 0);
                    }
                    fclose(file);
                    usleep(500000);
                    n = send(connfd, END, strlen(END), 0);
                } else printf("Invalid weekday! %s\n", buf);
                break;
            case '3':
                result = sscanf(buf, "%c %s", &cmd, weekday);
                if (result == 2) {
                    viewSchedule(studentID, weekday);
                    FILE *file = fopen("timetable.txt", "r"); // file receives the timetable.      
                    while((n = fgets(sendline, MAXLINE, file)) != NULL) {
                        n = send(connfd, sendline, strlen(sendline), 0);
                    }   
                    fclose(file);
                    usleep(500000);
                    n = send(connfd, END, strlen(END), 0);
                } else printf("Invalid weekday! %s\n", buf);
                break;
            default:
                printf("Invalid command! %s\n", buf);
                break;
            }
            printf("%s", "String received from and resent to the client:");
            puts(buf);
            int i;
            for(i=0;i<MAXLINE;i++) buf[i] = '\0';
        }

        if (n < 0)
        {
            perror("Read error");
            exit(1);
        }
        close(connfd);
    }
    // close listening socket
    close(listenfd);
}