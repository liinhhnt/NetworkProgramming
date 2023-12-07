#ifndef ENUMS_H
#define ENUMS_H

enum UserActions {
    REGISTER = 1,
    LOGIN = 2,
    SEARCH = 3,
    BROWSER = 4,
    RESERVE = 5,
    ADD_MOVIE = 6,
    ANNOUNCE_SHOWING = 7,
    GET_LIST_MOVIES = 8,
    GET_LIST_THEATER = 9, 
    GET_LIST_ROOMS_OR_THEATER = 10,
    GET_LIST_SHOWID = 11,
    MODIFY_SHOWING = 12,
    CREATE_USER = 13
};

enum AuthCode {
    FAIL = 0,
    SUCCESS = 1
};

// Replace these fields to match with your database
const char * database = "ticketBooking";
const char * username = "root";
const char * password = "123";
const char * ipAddress = "tcp://127.0.0.1:3309";

#endif // ENUMS_H
