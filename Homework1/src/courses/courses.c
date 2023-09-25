#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "courses.h"
#include "../registrations/registration.h"

void initCourseList(struct CourseList *courseList)
{
    courseList->courses = NULL;
    courseList->size = 0;
}

void convertWeekdayToString(char (*weekday)[]) {
    if (strcmp(*weekday, "2") == 0) strcpy(*weekday, "Monday");
    else if (strcmp(*weekday, "3") == 0) strcpy(*weekday, "Tuesday");
    else if (strcmp(*weekday, "4") == 0) strcpy(*weekday, "Wednesday");
    else if (strcmp(*weekday, "5") == 0) strcpy(*weekday, "Thursday");
    else if (strcmp(*weekday, "6") == 0) strcpy(*weekday, "Friday");
}

void convertTimeOfDayToString(char (*timeOfDay)[]) {
    if (strcmp(*timeOfDay, "1") == 0) strcpy(*timeOfDay, "Morning");
    else if (strcmp(*timeOfDay, "2") == 0) strcpy(*timeOfDay, "Afternoon");
}

int convertWeekdayToInt (char weekday[]) {
     if (strcmp(weekday, "Monday") == 0) return 2;
    if (strcmp(weekday, "Tuesday") == 0)  return 3;
    if (strcmp(weekday, "Wednesday") == 0) return 4;
    if (strcmp(weekday, "Thursday") == 0) return 5;
    if (strcmp(weekday, "Friday") == 0) return 6;
}

void parseCourseInfo(char line[], struct Course *course)
{
    sscanf(line, "%s %s %[^0-9] %3s,%3s,%[^A-Z]%s;",
           course->courseID, course->code, course->name, course->weekday, course->timeOfDay, course->weeks, course->room);

    course->period[0] = course->weekday[2];
    course->period[1] = '-';
    course->period[2] = course->timeOfDay[2];
    course->period[3] = '\0';

    course->weekday[1] = '\0';

    course->timeOfDay[0] = course->timeOfDay[1];
    course->timeOfDay[1] = '\0';

    convertWeekdayToString(&(course->weekday));
    convertTimeOfDayToString(&(course->timeOfDay));

    size_t weeks_len = strlen(course->weeks);
    if (weeks_len > 0) {
        course->weeks[weeks_len - 1] = '\0'; // remove comma at the end of string
    }

    size_t room_len = strlen(course->room);
    if (room_len > 0) {
        course->room[room_len - 1] = '\0';  // remove semicolon at the end of string
    }
}

// Function to read course schedule from a file
void readCourseSchedule(struct CourseList *courseList, char filePath[])
{
    FILE *file = fopen(filePath, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening course schedule file.\n");
        exit(1);
    }

    // Read course schedule from the file and populate the dynamic array
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (courseList->courses == NULL) {
            courseList->courses = (struct Course *)malloc(sizeof(struct Course));
        } else {
            courseList->courses = (struct Course *)realloc(courseList->courses, (courseList->size + 1) * sizeof(struct Course));
        }

        if (courseList->courses == NULL) {
            fprintf(stderr, "Memory allocation failed.\n");
            exit(1);
        }

        // Parse and store course information
        struct Course *course = &courseList->courses[courseList->size];
        parseCourseInfo(line, course);
        courseList->size++;
    }
    fclose(file);
}

void displayAllCourses(struct CourseList *courseList)
{
    printf("==================\n");
    printf("Course List:\n");
    for (int i = 0; i < courseList->size; i++)
    {
        struct Course *course = &courseList->courses[i];
        printf("%s   %s    %s    %s    %s    %s    %s    %s\n", course->courseID, course->code, course->name,
               course->weekday, course->timeOfDay, course->period, course->weeks, course->room);
    }
}

void getStudentRegistrationList(struct RegistrationList *registrationList, char studentID[], struct CourseList* studentRegisLst)
{
    for (int i = 0; i < registrationList->numRegistrations; i++)
    {
        if (strcmp(registrationList->registration[i].studentID, studentID) == 0) {
            if (studentRegisLst->courses == NULL) {
                studentRegisLst->courses = (struct Course *)malloc(sizeof(struct Course));
            } else {
                studentRegisLst->courses = (struct Course *)realloc(studentRegisLst->courses, (studentRegisLst->size + 1) * sizeof(struct Course));
            }
            if (studentRegisLst->courses == NULL) {
                fprintf(stderr, "Memory allocation failed.\n");
                exit(1);
            }
            
            strcpy(studentRegisLst->courses[studentRegisLst->size].courseID, registrationList->registration[i].courseCode);
            studentRegisLst->size++;
        }
    }
}

void getCourseListByCourseIDAndWeekDay (struct CourseList *courseList, struct CourseList *courseListByStudentID, char weekday[] ) {
    for (int i=0; i<courseList->size; i++) {
        for (int j=0; j<courseListByStudentID->size; j++) {
            if (strcmp(courseList->courses[i].courseID, courseListByStudentID->courses[j].courseID) == 0
                && (strcmp(weekday, "ALL") == 0 || strcmp(courseList->courses[i].weekday, weekday) == 0)) {
                    strcpy(courseListByStudentID->courses[j].code, courseList->courses[i].code);
                    strcpy(courseListByStudentID->courses[j].name, courseList->courses[i].name);
                    strcpy(courseListByStudentID->courses[j].weekday, courseList->courses[i].weekday);
                    strcpy(courseListByStudentID->courses[j].timeOfDay, courseList->courses[i].timeOfDay);
                    strcpy(courseListByStudentID->courses[j].period, courseList->courses[i].period);
                    strcpy(courseListByStudentID->courses[j].weeks, courseList->courses[i].weeks);
                    strcpy(courseListByStudentID->courses[j].room, courseList->courses[i].room);
                    break;
            }
        }
    }
}

void displayCourseListByStudentIDAndWeekDay(struct RegistrationList *registrationList, struct CourseList *courseList, char studentID[], char weekday[]) {
    struct CourseList courseListByStudentID;
    initCourseList (&courseListByStudentID);
    getStudentRegistrationList(registrationList, studentID, &courseListByStudentID);
    getCourseListByCourseIDAndWeekDay (courseList, &courseListByStudentID, weekday);
    // display
    printf("================================================================================================================\n");
    printf("%-10s | %-25s | %-10s | %-12s | %-15s | %-30s | %-10s\n", "Code", "Course", "Week Day", "AM/PM", "Period", "Week", "Room");
    printf("----------------------------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < courseListByStudentID.size; i++) {
        struct Course course = courseListByStudentID.courses[i];
        if (strlen(course.code) && (strcmp(course.weekday, weekday) == 0)) {
            printf("%-10s | %-25s | %-10s | %-12s | %-15s | %-30s | %-10s\n",
                course.code, course.name, course.weekday, course.timeOfDay, course.period, course.weeks, course.room);
        }
    }
    printf("================================================================================================================\n");
    freeCourseList(&courseListByStudentID);
}

void displayALLCoursesByStudentID(struct RegistrationList *registrationList, struct CourseList *courseList, char studentID[]) {
    struct CourseList courseListByStudentID;
    initCourseList (&courseListByStudentID);
    getStudentRegistrationList(registrationList, studentID, &courseListByStudentID);
    getCourseListByCourseIDAndWeekDay (courseList, &courseListByStudentID, "ALL");
       //process to display
    char schedule[10][15][10] = {0}; // schedule[day][period];
    for (int i = 0; i < courseListByStudentID.size; i++) {
        struct Course course = courseListByStudentID.courses[i];
         if (strlen(course.code)) {
            int begin = (int) (course.period[0]-'0');
            int end = (int) (course.period[2]-'0');
            if (strcmp(course.timeOfDay, "Afternoon") == 0) {
                begin += 6;
                end += 6;
            }
            int day = convertWeekdayToInt(course.weekday);
            for (int j=begin; j<=end; j++) {
                strcpy(schedule[day][j], course.room);
            }
            printf("%d\n", day);
         }
    }
    //Display the schedule 
    printf("+============================================================+\n");
    printf("|    | Monday   | Tuesday  | Wednesday | Thursday | Friday   |\n");
    printf("|------------------------------------------------------------|\n");
    for (int t = 1; t <= 12; t++) {
        printf("| %-3d|", t);
        if (schedule[2][t]) printf(" %-8s |", schedule[2][t]); else printf("           |");
        if (schedule[3][t]) printf(" %-8s |", schedule[3][t]); else printf("           |");
        if (schedule[4][t]) printf(" %-9s |", schedule[4][t]); else printf("           |");
        if (schedule[5][t]) printf(" %-8s |", schedule[5][t]); else printf("           |");
        if (schedule[6][t]) printf(" %-8s |\n", schedule[6][t]); else printf("           |\n");
    }
    printf("+============================================================+\n");
    freeCourseList(&courseListByStudentID);
}

void displayCourseList(struct CourseList *courseList) {
    printf("==================\n");
    printf("Course List:\n");
    printf("%-10s%-8s%-25s%-15s%-15s%-10s%-20s%-10s\n",
           "Course ID", "Code", "Name", "Weekday", "Time of Day", "Period", "Weeks", "Room");
    printf("------------------------------------------------------------------------------\n");

    for (int i = 0; i < courseList->size; i++) {
        struct Course *course = &courseList->courses[i];
        printf("%-10s%-8s%-25s%-15s%-15s%-10s%-20s%-10s\n",
               course->courseID, course->code, course->name, course->weekday,
               course->timeOfDay, course->period, course->weeks, course->room);
    }

    printf("==================\n");
}
void freeCourseList(struct CourseList *courseList)
{
    if (courseList->courses != NULL)
    {
        free(courseList->courses);
    }
    courseList->size = 0;
}
