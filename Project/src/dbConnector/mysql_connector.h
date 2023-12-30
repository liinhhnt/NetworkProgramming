#ifndef MYSQL_OPERATIONS_H
#define MYSQL_OPERATIONS_H

#include <iostream>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

using namespace std;
using namespace sql;

class MySQLOperations {
private:
    sql::Driver *driver;
    sql::Connection *conn;
    sql::Statement *stmt;
    
public:
    MySQLOperations();
    ~MySQLOperations();
    bool connect(const string& ipAddress, const string& username, const string& password, const string& database);
    bool createTableIfNotExists(const string& createTableQuery);
    bool insertRecords(const string& insertQuery);
    int checkRoleUser(const string& countQuery);
    void getListMovies(struct MovieList *movieList, const string& query);
    void getListTypes(struct TypeList *typeList, const string& query);
    void getListCinemas(struct CinemaList *cinemaList, const string& query);
    void getListShowTimes(struct ShowTimeList *showTimeList, const std::string& query);
    void disconnect();
};

#endif // MYSQL_OPERATIONS_H
