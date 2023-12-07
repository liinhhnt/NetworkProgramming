#include <bits/stdc++.h>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include "enums.h"

using namespace std;
using namespace sql;

int main() {
    try {
        // Create a MySQL Connector/C++ driver
        sql::Driver *driver;
        driver = get_driver_instance();

        // Connect to the MySQL database
        sql::Connection *conn;
        conn = driver->connect(ipAddress, username, password);
        conn->setSchema(database);

        // Sample SQL queries
        string createTableQuery = "CREATE TABLE IF NOT EXISTS users (id INT AUTO_INCREMENT, name VARCHAR(50), PRIMARY KEY (id))";
        string insertQuery = "INSERT INTO users (name) VALUES ('Alice'), ('Bob'), ('Charlie')";
        string selectQuery = "SELECT * FROM users";

        // Execute SQL queries
        sql::Statement *stmt = conn->createStatement();
        
        stmt->execute(createTableQuery); // Create 'users' table if it doesn't exist
        stmt->execute(insertQuery); // Insert records into 'users'

        sql::ResultSet *res = stmt->executeQuery(selectQuery); // Select all records from 'users'

        // Print the results
        while (res->next()) {
            cout << "ID: " << res->getInt("id") << ", Name: " << res->getString("name") << endl;
        }

        delete res;
        delete stmt;
        delete conn;

    } catch (sql::SQLException &e) {
        cout << "SQLException: " << e.what() << endl;
    }

    return 0;
}
