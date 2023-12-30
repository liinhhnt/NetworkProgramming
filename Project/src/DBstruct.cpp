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

void initMovieList (struct MovieList* movieList) {
  movieList->movies = NULL;
  movieList->size = 0;
}

void freeMovieList (struct MovieList *movieList) {
  if (movieList->movies != NULL) {
    free(movieList->movies);
  }
  movieList->size = 0;
}

void initBrowse (struct Browse* browse) {
  browse->showTimes = NULL;
  browse->noShowTime = 0;
}

void freeBrowse (struct Browse *browse) {
  if (browse->showTimes != NULL) {
    free(browse->showTimes);
  }
  browse->noShowTime = 0;
}

void initBrowseList (struct BrowseList* browseList) {
  browseList->browses = NULL;
  browseList->size = 0;
}

void freeBrowseList (struct BrowseList *browseList) {
  if (browseList->browses != NULL) {
    free(browseList->browses);
  }
  browseList->size = 0;
}