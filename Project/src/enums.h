#ifndef ENUMS_H
#define ENUMS_H

// Replace these fields to match with your database
const char * database = "ticketBooking";
const char * username = "root";
const char * password = "ntmn12345";
const char * ipAddress = "tcp://127.0.0.1:3306";

enum UserActions {
    REGISTER = 1,
    LOGIN = 2,
    SEARCH = 3,
    BROWSE = 4,
    RESERVE = 5,
    ADD_MOVIE = 6,
    ANNOUNCE_SHOWING = 7,
    GET_LIST_MOVIES = 8,
    GET_LIST_CINEMA = 9, 
    GET_LIST_ROOMS_OF_CINEMA = 10,
    GET_LIST_SHOWTIME_BY_MOVIEID = 11,
    MODIFY_SHOWING = 12,
    CREATE_USER = 13,
    GET_LIST_TYPE = 14,
    GET_SHOWTIME_INFO = 15,
    ADD_SHOW_TIMES = 16,
    ADD_MOVIE_TYPE = 17,
    // GET_LIST_MOVIES = 18,
};

enum AuthCode {
    FAIL = 0,
    SUCCESS = 1
};

enum UserMode {
    BUYER = 1,
    SALER = 2,
    ADMIN = 3
};

#endif // ENUMS_H
