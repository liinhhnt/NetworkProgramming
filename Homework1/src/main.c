#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "users/user.h"
#include "courses/courses.h"
#include "registrations/registration.h"

#define userFilePath "..\\data\\user_account.txt"   // replace link format if you use Ubuntu 
#define courseFilePath "..\\data\\course_schedule.txt"  // replace link format if you use Ubuntu 
#define registrationFilePath "..\\data\\student_registration.txt"  // replace link format if you use Ubuntu 


struct UserList userList;
struct CourseList courseList;
struct RegistrationList registrationList;

int login (char (*studentID)[]) {
    char username[20]; char pwd[20];
    printf("Enter you username (studentID): \n");
    scanf ("%s", username);
    printf("Enter you password: \n");
    scanf ("%s", pwd);
    int auth = authenticateUser(&userList, username, pwd);
    if (auth) {
        printf("Login successfully!\n");
        strcpy(*studentID, username);
        return 1; // success
    }
    else {
        printf ("Invalid username or password!!!\n");
        return 0; //fail
    }
}

void viewSchedule (char studentID[]) {
    char weekday[20];
    printf("Enter the weekday (e.g., Monday, Tuesday) or type ALL to view week schedule: ");
    scanf("%s", weekday);
    getchar();
    if (!strcmp(weekday, "Monday") || !strcmp(weekday, "Tuesday") || !strcmp(weekday, "Wednesday") || !strcmp(weekday, "Thursday") || !strcmp(weekday, "Friday")) {
        displayCourseListByStudentIDAndWeekDay(&registrationList, &courseList, studentID, weekday);
    }
    else if (strcmp(weekday, "ALL")==0) {
        displayALLCoursesByStudentID (&registrationList, &courseList, studentID);
    }
    else printf ("Invalid weekday!!!\n\n");
}

int main() {
    // Initialize user list and read user data
    initUserList(&userList);
    readUserList(&userList, userFilePath);

    // Initialize course list and read course schedule data
    initCourseList(&courseList);
    readCourseSchedule(&courseList, courseFilePath);

    //Initialize registration list and read student registrations
    initRegistrationList(&registrationList);
    readStudentRegistrations(&registrationList, registrationFilePath);

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
            } else if (choice == 2) {
                printf("Goodbye!\n");
                break;
            } else {
                printf("Invalid choice. Try again.\n\n");
            }
        } else {
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
                printf("Logged out successfully.\n\n");
            } else {
                printf("Invalid choice. Try again.\n");
            }
        }
    }

    //Free allocated memory
    freeUserList(&userList);
    freeCourseList(&courseList);
    freeRegistrationList(&registrationList);
    return 0;
}