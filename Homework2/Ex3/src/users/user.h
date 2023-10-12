#ifndef USER_H
#define USER_H

struct User {
    char studentID[20];
    char password[20];
    struct User* next;
};

struct UserList {
    struct User* head;
    int size;
};

void initUserList(struct UserList* userList);
void addUser(struct UserList* userList, char studentID[], char password[]);
void readUserList(struct UserList *userList, char filePath[]);
int authenticateUser(struct UserList* userList, char studentID[], char password[]);
void freeUserList(struct UserList* userList);
void displayUserList (const struct UserList* userList);

#endif
