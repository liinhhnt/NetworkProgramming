#include "../registrations/registration.h"
#ifndef COURSES_H
#define COURSES_H

struct Course {
    char courseID[20];
    char code[20];
    char name[50];
    char weekday[15];
    char timeOfDay[15];
    char period[10];
    char weeks[100];
    char room[20];
};

struct CourseList {
    struct Course *courses;
    int size;
};

void initCourseList(struct CourseList *courseList);
void readCourseSchedule(struct CourseList *courseList, char filePath[]);
void convertWeekdayToString(char (*weekday)[]);
void convertTimeOfDayToString(char (*timeOfDay)[]);
int convertWeekdayToInt (char weekday[]);
void displayCourses(struct CourseList *courseList);
void getStudentRegistrationList(struct RegistrationList *registrationList, char studentID[], struct CourseList* courseListByStudentID);
void getCourseListByCourseIDAndWeekDay (struct CourseList *courseList, struct CourseList *courseListByStudentID, char weekday[]);
void displayCourseList(struct CourseList *courseList);
void displayCourseListByStudentIDAndWeekDay(struct RegistrationList *registrationList, struct CourseList *courseList, char studentID[], char weekday[], int connfd);
void displayALLCoursesByStudentID(struct RegistrationList *registrationList, struct CourseList *courseList, char studentID[], int connfd);
void freeCourseList(struct CourseList *courseList);

#endif
