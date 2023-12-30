#include "mysql_connector.h"
#include "../DBstruct.h"
#include <string.h>

MySQLOperations::MySQLOperations() {
    driver = get_driver_instance();
    conn = nullptr;
    stmt = nullptr;
}

MySQLOperations::~MySQLOperations() {
    delete stmt;
    delete conn;
}

bool MySQLOperations::connect(const string& ipAddress, const string& username, const string& password, const string& database) {
    try {
        conn = driver->connect(ipAddress, username, password);
        if (conn) {
            conn->setSchema(database);
            stmt = conn->createStatement();
            return true;
        }
    } catch (SQLException &e) {
        cerr << "Error connecting: " << e.what() << endl;
    }
    return false;
}

bool MySQLOperations::createTableIfNotExists(const string& createTableQuery) {
    try {
        stmt->execute(createTableQuery);
        return true;
    } catch (SQLException &e) {
        cerr << "Error creating table: " << e.what() << endl;
    }
    return false;
}

bool MySQLOperations::insertRecords(const string& insertQuery) {
    try {
        stmt->execute(insertQuery);
        return true;
    } catch (SQLException &e) {
        cerr << "Error inserting records: " << e.what() << endl;
    }
    return false;
}

int MySQLOperations::checkRoleUser(const string& countQuery) {
    try {
        sql::ResultSet *res = stmt->executeQuery(countQuery);
        string role; 
        if (res->next()) {
            role = res->getString(3);
            if (role == "buyer") return 1;
            else if (role == "saler") return 2;
            else if (role == "admin") return 3;
            else return 0;
        }
        else return 0;
    } catch (SQLException &e) {
        cerr << "Error: " << e.what() << endl;
        return 0;
    }
}

void MySQLOperations::getListMovies(struct MovieList *movieList, const string& query)
{
    try {
        sql::ResultSet *res = stmt->executeQuery(query);
        int movieListSize = movieList->size;
        while (res->next()) {
            if (movieList->movies == NULL) {
                movieList->movies = (struct Movie*)malloc(sizeof(struct Movie));
            } else {
                movieList->movies = (struct Movie*) realloc(movieList->movies, (movieListSize + 1)*sizeof(struct Movie));
            }
            if (movieList->movies == NULL) {
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
    } catch (SQLException &e) {
        cerr << "Error selecting records: " << e.what() << endl;
    }
}

void MySQLOperations::selectAllRecords(const string& selectQuery) {
    try {
        sql::ResultSet *res = stmt->executeQuery(selectQuery);
        sql::ResultSetMetaData *metaData = res->getMetaData();
        
        int columnCount = metaData->getColumnCount();
        while (res->next()) {
            for (int i = 1; i <= columnCount; ++i) {
                cout << metaData->getColumnLabel(i) << ": ";
                switch (metaData->getColumnType(i)) {
                    case sql::DataType::INTEGER:
                        cout << res->getInt(i) << ", ";
                        break;
                    case sql::DataType::VARCHAR:
                    case sql::DataType::CHAR:
                        cout << res->getString(i) << ", ";
                        break;
                    // Handle other data types as needed
                    default:
                        cout << res->getString(i) << ", ";
                        break;
                }
            }
            cout << endl;
        }
        delete res;
    } catch (SQLException &e) {
        cerr << "Error selecting records: " << e.what() << endl;
    }
}


void MySQLOperations::disconnect() {
    delete stmt;
    delete conn;
}
