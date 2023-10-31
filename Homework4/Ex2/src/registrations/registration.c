#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "registration.h"

void initRegistrationList(struct RegistrationList *registrationList) {
    registrationList->registration = NULL;
    registrationList->numRegistrations = 0;
}

// Function to read student registrations from a file
void readStudentRegistrations(struct RegistrationList *registrationList, char filePath[]) {
    FILE *file = fopen(filePath, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening student registration file.\n");
        exit(1);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (registrationList->registration == NULL) {
            registrationList->registration = (struct Registration*) malloc(sizeof(struct Registration));
        } else {
            registrationList->registration = (struct Registration*) realloc(registrationList->registration, (registrationList->numRegistrations+1)*sizeof(struct Registration));
        }
        
        if (registrationList->registration == NULL) {
            fprintf(stderr, "Memory allocation failed.\n");
            exit(1);
        }
        struct Registration *registration = &registrationList->registration[registrationList->numRegistrations];

        sscanf(line, "%s %s", registration->studentID, registration->courseCode);
        (registrationList->numRegistrations)++;
    }

    fclose(file);
}

void displayRegistrationList(const struct RegistrationList *registrationList) {
    printf("Registration List: \n");
    printf("===========\n");
    for (int i=0; i<registrationList->numRegistrations; i++) {
        printf("%s  %s\n", registrationList->registration[i].studentID, registrationList->registration[i].courseCode);
    }
    printf("============\n");
}

// Function to free the linked list of registrations
void freeRegistrationList(struct RegistrationList* registrationList) {
    if (registrationList->registration != NULL) {
        free(registrationList->registration);
    }
    registrationList->numRegistrations = 0;
}

