#include "mysql_connector.h"

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
