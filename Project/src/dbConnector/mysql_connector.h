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
    void selectAllRecords(const string& selectQuery);
    int checkRoleUser(const string& countQuery);
    void disconnect();
};

#endif // MYSQL_OPERATIONS_H
