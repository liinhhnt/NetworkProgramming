#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "users/user.h"
#include "courses/courses.h"
#include "registrations/registration.h"

// #define userFilePath "..\\data\\user_account.txt"   // replace link format if you use Ubuntu
// #define courseFilePath "..\\data\\course_schedule.txt"  // replace link format if you use Ubuntu
// #define registrationFilePath "..\\data\\student_registration.txt"  // replace link format if you use Ubuntu
#define userFilePath "../data/user_account.txt"
#define courseFilePath "../data/course_schedule.txt"
#define registrationFilePath "../data/student_registration.txt"
#define MAXLINE 4096   /*max text line length*/
#define SERV_PORT 3000 /*port*/
#define LISTENQ 8      /*maximum number of client connections */

struct UserList userList;
struct CourseList courseList;
struct RegistrationList registrationList;

int login(char (*studentID)[])
{
    char username[20];
    char pwd[20];
    printf("Enter you username (studentID): \n");
    scanf("%s", username);
    printf("Enter you password: \n");
    scanf("%s", pwd);
    int auth = authenticateUser(&userList, username, pwd);
    if (auth)
    {
        printf("Login successfully!\n");
        strcpy(*studentID, username);
        return 1; // success
    }
    else
    {
        printf("Invalid username or password!!!\n");
        return 0; // fail
    }
}

void viewSchedule(char studentID[])
{
    char weekday[20];
    printf("Enter the weekday (e.g., Monday, Tuesday) or type ALL to view week schedule: ");
    scanf("%s", weekday);
    getchar();
    if (!strcmp(weekday, "Monday") || !strcmp(weekday, "Tuesday") || !strcmp(weekday, "Wednesday") || !strcmp(weekday, "Thursday") || !strcmp(weekday, "Friday"))
    {
        displayCourseListByStudentIDAndWeekDay(&registrationList, &courseList, studentID, weekday);
    }
    else if (strcmp(weekday, "ALL") == 0)
    {
        displayALLCoursesByStudentID(&registrationList, &courseList, studentID);
    }
    else
        printf("Invalid weekday!!!\n\n");
}

int main(int argc, char **argv)
{
    int listenfd, connfd, n;
    pid_t childpid;
    socklen_t clilen;
    char buf[MAXLINE];
    struct sockaddr_in cliaddr, servaddr;

    // creation of the socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    // preparation of the socket address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);

    printf("%s\n", "Server running...waiting for connections.");

    // Initialize user list and read user data
    initUserList(&userList);
    readUserList(&userList, userFilePath);

    // Initialize course list and read course schedule data
    initCourseList(&courseList);
    readCourseSchedule(&courseList, courseFilePath);

    // Initialize registration list and read student registrations
    initRegistrationList(&registrationList);
    readStudentRegistrations(&registrationList, registrationFilePath);

    for (;;)
    {
        int loggedIn = 0;
        char studentID[20];
        while (1)
        {
            if (!loggedIn)
            {
                printf("====================================\n");
                printf("Welcome to Study Schedule Management\n");
                printf("1. Login\n");
                printf("2. Exit\n");
                printf("Enter your choice: ");

                int choice;
                scanf("%d", &choice);
                getchar();
                if (choice == 1)
                {
                    loggedIn = login(&studentID);
                }
                else if (choice == 2)
                {
                    printf("Goodbye!\n");
                    break;
                }
                else
                {
                    printf("Invalid choice. Try again.\n\n");
                }
            }
            else
            {
                printf("----------------------------------\n");
                printf("Logged in as Student ID: %s\n", studentID);
                printf("1. View Schedule\n");
                printf("2. Logout\n");
                printf("Enter your choice: ");

                int choice;
                scanf("%d", &choice);
                getchar();
                if (choice == 1)
                {
                    viewSchedule(studentID);
                }
                else if (choice == 2)
                {
                    loggedIn = 0;
                    printf("Logged out successfully.\n\n");
                }
                else
                {
                    printf("Invalid choice. Try again.\n");
                }
            }
        }

        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        printf("%s\n", "Received request...");

        while ((n = recv(connfd, buf, MAXLINE, 0)) > 0)
        {
            printf("%s", "String received from and resent to the client:");
            puts(buf);
            send(connfd, buf, n, 0);
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
    // Free allocated memory
    freeUserList(&userList);
    freeCourseList(&courseList);
    freeRegistrationList(&registrationList);
    return 0;
}