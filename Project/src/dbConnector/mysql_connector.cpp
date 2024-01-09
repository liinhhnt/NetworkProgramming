#include "mysql_connector.h"
#include "../DBstruct.h"
#include <string.h>

MySQLOperations::MySQLOperations()
{
    driver = get_driver_instance();
    conn = nullptr;
    stmt = nullptr;
}

MySQLOperations::~MySQLOperations()
{
    delete stmt;
    delete conn;
}

bool MySQLOperations::connect(const string &ipAddress, const string &username, const string &password, const string &database)
{
    try
    {
        conn = driver->connect(ipAddress, username, password);
        if (conn)
        {
            conn->setSchema(database);
            stmt = conn->createStatement();
            return true;
        }
    }
    catch (SQLException &e)
    {
        cerr << "Error connecting: " << e.what() << endl;
    }
    return false;
}

bool MySQLOperations::createTableIfNotExists(const string &createTableQuery)
{
    try
    {
        stmt->execute(createTableQuery);
        return true;
    }
    catch (SQLException &e)
    {
        cerr << "Error creating table: " << e.what() << endl;
    }
    return false;
}

bool MySQLOperations::insertRecords(const string &insertQuery)
{
    try
    {
        stmt->execute(insertQuery);
        return true;
    }
    catch (SQLException &e)
    {
        cerr << "Error inserting records: " << e.what() << endl;
    }
    return false;
}

int MySQLOperations::checkRoleUser(const string &countQuery)
{
    try
    {
        sql::ResultSet *res = stmt->executeQuery(countQuery);
        string role;
        if (res->next())
        {
            role = res->getString(3);
            if (role == "buyer")
                return 1;
            else if (role == "saler")
                return 2;
            else if (role == "admin")
                return 3;
            else
                return 0;
        }
        else
            return 0;
    }
    catch (SQLException &e)
    {
        cerr << "Error: " << e.what() << endl;
        return 0;
    }
}

void MySQLOperations::getListMovies(struct MovieList *movieList, const string &query)
{
    try
    {
        sql::ResultSet *res = stmt->executeQuery(query);
        int movieListSize = movieList->size;
        while (res->next())
        {
            if (movieList->movies == NULL)
            {
                movieList->movies = (struct Movie *)malloc(sizeof(struct Movie));
            }
            else
            {
                movieList->movies = (struct Movie *)realloc(movieList->movies, (movieListSize + 1) * sizeof(struct Movie));
            }
            if (movieList->movies == NULL)
            {
                fprintf(stderr, "Memory allocation failed.\n");
                exit(1);
            }
            movieList->movies[movieListSize].movieId = res->getInt("movieId");
            strncpy(movieList->movies[movieListSize].movieName, res->getString("movieName").c_str(), sizeof(movieList->movies[movieListSize].movieName) - 1);
            movieList->movies[movieListSize].movieName[sizeof(movieList->movies[movieListSize].movieName) - 1] = '\0';
            strncpy(movieList->movies[movieListSize].typeName, res->getString("typeName").c_str(), sizeof(movieList->movies[movieListSize].typeName) - 1);
            movieList->movies[movieListSize].typeName[sizeof(movieList->movies[movieListSize].typeName) - 1] = '\0';
            strncpy(movieList->movies[movieListSize].duration, res->getString("duration").c_str(), sizeof(movieList->movies[movieListSize].duration) - 1);
            movieList->movies[movieListSize].duration[sizeof(movieList->movies[movieListSize].duration) - 1] = '\0';
            strncpy(movieList->movies[movieListSize].describtion, res->getString("description").c_str(), sizeof(movieList->movies[movieListSize].describtion) - 1);
            movieList->movies[movieListSize].describtion[sizeof(movieList->movies[movieListSize].describtion) - 1] = '\0';
            movieList->size = ++movieListSize;
        }
        delete res;
    }
    catch (SQLException &e)
    {
        cerr << "Error selecting records: " << e.what() << endl;
    }
}

void MySQLOperations::getListTypes(struct TypeList *typeList, const string &query)
{
    try
    {
        sql::ResultSet *res = stmt->executeQuery(query);
        int typeListSize = typeList->size;
        while (res->next())
        {
            if (typeList->types == NULL)
            {
                typeList->types = (struct Type *)malloc(sizeof(struct Type));
            }
            else
            {
                typeList->types = (struct Type *)realloc(typeList->types, (typeListSize + 1) * sizeof(struct Type));
            }
            if (typeList->types == NULL)
            {
                fprintf(stderr, "Memory allocation failed.\n");
                exit(1);
            }
            typeList->types[typeListSize].typeId = res->getInt("typeId");
            strncpy(typeList->types[typeListSize].typeName, res->getString("typeName").c_str(), sizeof(typeList->types[typeListSize].typeName) - 1);
            typeList->types[typeListSize].typeName[sizeof(typeList->types[typeListSize].typeName) - 1] = '\0';
            typeList->size = ++typeListSize;
        }
        delete res;
    }
    catch (SQLException &e)
    {
        cerr << "Error selecting records: " << e.what() << endl;
    }
}

void MySQLOperations::getListCinemas(struct CinemaList *cinemaList, const string &query)
{
    try
    {
        sql::ResultSet *res = stmt->executeQuery(query);
        int cinemaListSize = cinemaList->size;
        while (res->next())
        {
            if (cinemaList->cinemas == NULL)
            {
                cinemaList->cinemas = (struct Cinema *)malloc(sizeof(struct Cinema));
            }
            else
            {
                cinemaList->cinemas = (struct Cinema *)realloc(cinemaList->cinemas, (cinemaListSize + 1) * sizeof(struct Cinema));
            }
            if (cinemaList->cinemas == NULL)
            {
                fprintf(stderr, "Memory allocation failed.\n");
                exit(1);
            }
            cinemaList->cinemas[cinemaListSize].cinemaId = res->getInt("cinemaId");
            strncpy(cinemaList->cinemas[cinemaListSize].cinemaName, res->getString("cinemaName").c_str(), sizeof(cinemaList->cinemas[cinemaListSize].cinemaName) - 1);
            cinemaList->cinemas[cinemaListSize].cinemaName[sizeof(cinemaList->cinemas[cinemaListSize].cinemaName) - 1] = '\0';
            strncpy(cinemaList->cinemas[cinemaListSize].location, res->getString("location").c_str(), sizeof(cinemaList->cinemas[cinemaListSize].location) - 1);
            cinemaList->cinemas[cinemaListSize].location[sizeof(cinemaList->cinemas[cinemaListSize].location) - 1] = '\0';
            cinemaList->size = ++cinemaListSize;
        }
        delete res;
    }
    catch (SQLException &e)
    {
        cerr << "Error selecting records: " << e.what() << endl;
    }
}

int getNullableInt(sql::ResultSet *res, const std::string &columnName, int defaultValue = 0)
{
    if (!res->isNull(columnName))
    {
        return res->getInt(columnName);
    }
    else
    {
        return defaultValue;
    }
}

void MySQLOperations::getListShowTimes(struct ShowTimeList *showTimeList, const std::string &query)
{
    try
    {
        sql::ResultSet *res = stmt->executeQuery(query);

        while (res->next())
        {
            struct ShowTime showTime;
            showTime.showTimeId = getNullableInt(res, "showTimeId", 0);
            showTime.movieId = getNullableInt(res, "movieId", 0);
            showTime.roomId = getNullableInt(res, "roomId", 0);
            showTime.noOfEmptySeats = getNullableInt(res, "noOfEmptySeats", 0);

            strncpy(showTime.seatMap, res->getString("seatMap").c_str(), sizeof(showTime.seatMap) - 1);
            showTime.seatMap[sizeof(showTime.seatMap) - 1] = '\0';

            try {
                strncpy(showTime.movieName, res->getString("movieName").c_str(), sizeof(showTime.movieName) - 1);
                showTime.movieName[sizeof(showTime.movieName) - 1] = '\0';
            }
            catch (const sql::SQLException &e)
            {
                showTime.movieName[0] = '\0';
            }

            try {
                strncpy(showTime.typeName, res->getString("typeName").c_str(), sizeof(showTime.typeName) - 1);
                showTime.typeName[sizeof(showTime.typeName) - 1] = '\0';
            }
            catch (const sql::SQLException &e)
            {
                showTime.typeName[0] = '\0';
            }

            try {
                strncpy(showTime.cinema, res->getString("cinemaName").c_str(), sizeof(showTime.cinema) - 1);
                showTime.cinema[sizeof(showTime.cinema) - 1] = '\0';
            }
            catch (const sql::SQLException &e)
            {
                showTime.cinema[0] = '\0';
            }

            strncpy(showTime.weekday, res->getString("weekday").c_str(), sizeof(showTime.weekday) - 1);
            showTime.weekday[sizeof(showTime.weekday) - 1] = '\0';

            strncpy(showTime.startTime, res->getString("startTime").c_str(), sizeof(showTime.startTime) - 1);
            showTime.startTime[sizeof(showTime.startTime) - 1] = '\0';

            strncpy(showTime.endTime, res->getString("endTime").c_str(), sizeof(showTime.endTime) - 1);
            showTime.endTime[sizeof(showTime.endTime) - 1] = '\0';

            showTimeList->showTimes = (struct ShowTime *)realloc(showTimeList->showTimes, (showTimeList->size + 1) * sizeof(struct ShowTime));
            if (showTimeList->showTimes == NULL)
            {
                fprintf(stderr, "Memory allocation failed.\n");
                exit(1);
            }
            showTimeList->showTimes[showTimeList->size++] = showTime;
        }

        delete res;
    }
    catch (SQLException &e)
    {
        cerr << "Error selecting records: " << e.what() << endl;
    }
}

int MySQLOperations::updateShowTimeSeatMap(int showTimeId, const std::string &query)
{
    try
    {
        stmt->executeUpdate(query);
        return 1;
    }
    catch (SQLException &e)
    {
        cerr << "Error updating seatMap for ShowTimeId " << showTimeId << ": " << e.what() << endl;
        return 0;
    }
}

void MySQLOperations::disconnect()
{
    delete stmt;
    delete conn;
}
