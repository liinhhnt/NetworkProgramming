#include <stdio.h>
#include <stdlib.h>
#include "DBstruct.h"

void initTypeList (struct TypeList* typeList) {
  typeList->types = NULL;
  typeList->size = 0;
}

void freeTypeList (struct TypeList *typeList) {
  if (typeList->types != NULL) {
    free(typeList->types);
  }
  typeList->size = 0;
}


void initShowTimeList (struct ShowTimeList* showtimeList) {
  showtimeList->showTimes = NULL;
  showtimeList->size = 0;
}

void freeShowTimeList (struct ShowTimeList *showtimeList) {
  if (showtimeList->showTimes != NULL) {
    free(showtimeList->showTimes);
  }
  showtimeList->size = 0;
}

void initCinemaList (struct CinemaList* cinemaList) {
  cinemaList->cinemas = NULL;
  cinemaList->size = 0;
}

void freeCinemaList (struct CinemaList *cinemaList) {
  if (cinemaList->cinemas != NULL) {
    free(cinemaList->cinemas);
  }
  cinemaList->size = 0;
}

void initMovieList(struct MovieList* movieList) {
    movieList->movies = NULL;
    movieList->size = 0;
}

void freeMovieList(struct MovieList* movieList) {
    if (movieList->movies != NULL) {
        free(movieList->movies);
    }
    movieList->size = 0;
}
