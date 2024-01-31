#ifndef DBSTRUCT_H
#define DBSTRUCT_H

struct Type {
  int typeId;
  char typeName[255];
};
struct TypeList {
  struct Type *types;
  int size;
};

void initTypeList (struct TypeList* typeList);
void freeTypeList (struct TypeList *typeList) ;

struct Movie {
  int movieId;
  char movieName[255];
  char typeName[255];
  char duration[10];
  char description[255];
};

struct MovieList {
  struct Movie *movies;
  int size;
};

void initMovieList (struct MovieList* movieList);
void freeMovieList (struct MovieList *movieList);

struct ShowTime {
  int movieId;
  int showTimeId;
  int roomId;
  char seatMap[255];
  int noOfEmptySeats;
  char movieName[255];
  char typeName[255];
  char cinema[255];
  char weekday[255];
  char startTime[255];
  char endTime[255];
};

struct ShowTimeList {
  struct ShowTime *showTimes;
  int size;
};

void initShowTimeList (struct ShowTimeList* showTimeList);
void freeShowTimeList (struct ShowTimeList* ShowTimeList);

struct Room {
  int roomId;
  int cinemaId;
  int noOfSeats;
};

struct RoomList {
  struct Room *rooms;
  int size;
};

void initRoomList (struct RoomList* RoomList);
void freeRoomList (struct RoomList* RoomList);

struct Cinema {
  int cinemaId;
  char cinemaName[255];
  char location[255];
};
struct CinemaList {
  struct Cinema *cinemas;
  int size;
};

void initCinemaList (struct CinemaList* cinemaList);
void freeCinemaList (struct CinemaList *cinemaList) ;


#endif