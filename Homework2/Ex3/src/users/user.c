#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "user.h"

void initUserList(struct UserList* userList) {
    userList->head = NULL;
    userList->size = 0;
}

void addUser(struct UserList* userList, char studentID[], char password[]) {
    struct User* newUser = (struct User*)malloc(sizeof(struct User));
    if (newUser == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }

    strcpy(newUser->studentID, studentID);
    strcpy(newUser->password, password);
    newUser->next = userList->head;
    userList->head = newUser;
    userList->size++;
}

// Function to read user account data from a file
void readUserList(struct UserList *userList, char filePath[]) {
    FILE *file = fopen(filePath, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening user account file.\n");
        exit(1);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        struct User newUser;
        if (sscanf(line, "%s %s", newUser.studentID, newUser.password) == 2) {
            addUser(userList, newUser.studentID, newUser.password);
        } else {
            fprintf(stderr, "Error parsing user account data.\n");
        }
    }

    fclose(file);
}

int authenticateUser(struct UserList* userList, char studentID[], char password[]) {
    struct User* current = userList->head;
    while (current != NULL) {
        if (strcmp(current->studentID, studentID) == 0 && strcmp(current->password, password) == 0) {
            return 1; // Authentication successful
        }
        current = current->next;
    }
    return 0; // Authentication failed
}

void freeUserList(struct UserList* userList) {
    struct User* current = userList->head;
    while (current != NULL) {
        struct User* temp = current;
        current = current->next;
        free(temp);
    }
    userList->head = NULL;
    userList->size = 0;
}

void displayUserList(const struct UserList *userList) {
    printf("User List: \n");
    printf("===========\n");

    struct User *current = userList->head;
    while (current != NULL) {
        printf("Student ID: %s\n", current->studentID);
        current = current->next;
    }
    printf("============\n");
}