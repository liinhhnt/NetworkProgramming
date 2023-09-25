#ifndef REGISTRATION_H
#define REGISTRATION_H

struct Registration {
    char studentID[20];
    char courseCode[20];
};

struct RegistrationList {
    struct Registration* registration;
    int numRegistrations;
};

void initRegistrationList(struct RegistrationList *registrationList);
void readStudentRegistrations(struct RegistrationList *head, char filePath[]);
void freeRegistrationList(struct RegistrationList *head);
void displayRegistrationList(const struct RegistrationList *registrationList);

#endif
