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
  char describtion[100];
};

struct MovieList {
  struct Movie *movies;
  int size;
};

void initMovieList (struct MovieList* movieList);
void freeMovieList (struct MovieList *movieList);

struct ShowTime {
  char weekday[255];
  char startTime[255];
  char endTime[255];
};

struct Browse {
  char movieName[255];
  char typeName[255];
  char cinema[255];
  struct ShowTime *showTimes;
  int noShowTime = 0;
};

struct BrowseList {
  struct Browse *browses;
  int size;
};

void initBrowse (struct Browse* browse);
void initBrowseList (struct BrowseList* browseList);
void freeBrowse (struct Browse* browse);
void freeBrowseList (struct BrowseList* browseList);

#endif