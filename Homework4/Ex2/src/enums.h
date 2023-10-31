#ifndef ENUMS_H
#define ENUMS_H

enum UserActions {
    LOGIN = 1,
    VIEWWEEKDAYSCHEDULE = 2,
    VIEWWEEKSCHEDULE = 3
};

enum AuthCode {
    FAIL = 0,
    SUCCESS = 1
};

enum Data {
    USER = 0,
    COURSE = 1,
    REGISTRATION = 2
};

const char* FilePath[] = {
    "../data/user_account.txt",   // replace link format if you use Ubuntu 
    "../data/course_schedule.txt",  // replace link format if you use Ubuntu 
    "../data/student_registration.txt"  // replace link format if you use Ubuntu 
};
#endif // ENUMS_H
