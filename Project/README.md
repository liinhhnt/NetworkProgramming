Install mysql connector/C++:
```shell
sudo apt-get install libmysqlcppconn-dev
```

Run sample code to verify whether mysql connector lib is installed:
```
g++ -o test_mysql test_mysql.cpp -lmysqlcppconn
./test_mysql
```

Run sample code to verify whether mysql connector can connect with database:
```
g++ -o test_ConnectDB testConnectDB.cpp -lmysqlcppconn
./test_ConnectDB
```