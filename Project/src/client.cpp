#include <bits/stdc++.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "enums.h"

#define MAXLINE 4096   // max text line length
#define SERV_PORT 3000 // port

using namespace std;

int socketfd, loggedIn;

void connectToServer(char *ip);
void displayMenu();
void displayAdminMenu(string *username);
void displayUserMenu(string *username);
void displaySalerMenu(string *username);
int login (string *username);
void createUser();
void _register();
void displayReceiveMessage(int *socketfd);
void searchMovie();
void browseMovie();
void bookTicket();
void reserve(int showtimeId, int movieId);
bool isInteger(const string &s);
bool isValidTicketFormat(string tickets);
void logout();
void addMovie();
void addShowTimes();
vector<std::string> receiveMovieType(int *socketfd);
bool isTypeInList(vector<std::string> movieslist, string typeId);
void addNewMovieType();
void getListMovie();
void getListCinema();
void getListRoom();
void getListType();

int main(int argc, char **argv)
{
    loggedIn = 0;
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <server_address>\n", argv[0]);
        exit(1);
    }

    connectToServer(argv[1]);

    displayMenu();
    return 0;
}

void connectToServer(char *ip)
{
    struct sockaddr_in servaddr;
    // Create a socket for the client
    // If socketfd<0 there was an error in the creation of the socket
    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("[-]Problem in creating the socket");
        exit(2);
    }
    cout << "[+]Client Socket is created." << endl;
    // Creation of the socket
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port = htons(SERV_PORT); // convert to big-endian order
    // Connection of the client to the socket
    if (connect(socketfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("[-]Problem in connecting to the server");
        exit(3);
    }
    cout << "[+]Connected to Server." << endl;
}

void displayMenu()
{
    string username;
    while (1)
    {
        if (!loggedIn)
        {
            printf("\n====================================\n");
            printf("Welcome to Online Movie Ticket Reservation Application\n");
            printf("1. Register\n");
            printf("2. Login\n");
            printf("3. Exit\n");
            printf("Enter your choice: ");

            string choice;
            cin >> choice;

            switch (choice[0])
            {
            case '1':
                _register();
                break;

            case '2':
                loggedIn = login(&username);
                break;

            case '3':
                printf("Goodbye!\n");
                exit(0);
                break;
            default:
                printf("Invalid choice. Try again.\n");
                break;
            }
            printf("-----------------------------\n");
        }
        else
        {
            // display menu coresponsing to each user mode
            switch (loggedIn)
            {
            case BUYER:
                displayUserMenu(&username);
                break;
            case SALER:
                displaySalerMenu(&username);
                break;
            case ADMIN:
                displayAdminMenu(&username);
                break;
            default:
                printf("[-]Some error in get role of user!\n");
                exit(1);
            }
            printf("-----------------------------\n");
        }
    }
}

void displayAdminMenu(string *username)
{
    cout << "Welcome admin: " << *username << "\n What do you want to do?\n";
    printf("1. Create user account\n");
    printf("2. Logout\n");
    printf("Enter your choice: ");

    string choice;
    cin >> choice;

    switch (choice[0])
    {
    case '1':
        createUser();
        break;
    case '2':
        logout();
        break;
    default:
        printf("Invalid choice. Try again.\n");
        break;
    }
}

void displayUserMenu(string *username)
{
    cout << "Hello: " << *username << "\n What do you want to do?\n";
    printf("1. Search movies by title\n");
    printf("2. Browse movies by type, cinema location and showtime\n");
    printf("3. Book movie ticket(s)\n");
    printf("4. Logout\n");
    printf("Enter your choice: ");

    string choice;
    cin >> choice;

    switch (choice[0])
    {
    case '1':
        searchMovie();
        break;
    case '2':
        browseMovie();
        break;
    case '3':
        bookTicket();
        break;
    case '4':
        logout();
        break;
    default:
        printf("Invalid choice. Try again.\n");
        break;
    }
}

void displaySalerMenu(string *username)
{
    cout << "Hello: " << *username << "\n What do you want to do?\n";
    printf("1. Add new movie\n");
    printf("2. Announce movie showings\n");
    printf("3. Modify screening information\n");
    printf("4. Logout\n");
    printf("Enter your choice: ");

    string choice;
    cin >> choice;

    switch (choice[0])
    {
    case '1':
        addMovie();
        break;
    case '2':
        addShowTimes();
        break;
    case '3':
        //bookTicket();
        break;
    case '4':
        logout();
        break;
    default:
        printf("Invalid choice. Try again.\n");
        break;
    }
}

void createUser()
{
    char username[30], password[30], role[30];
    char sendline[MAXLINE], recvline[MAXLINE];
    string choice;

    cout << "\nCreate new user account...\n";
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    do {
        cout << "Choose role for account (1.buyer, 2.saler, 3.admin): ";
        cin >> choice;
        switch (choice[0]) {
            case '1':
                strcpy(role, "buyer");
                break;
            case '2':
                strcpy(role, "saler");
                break;
            case '3':
                strcpy(role, "admin");
                break;
            default:
                cout << "Invalid choice! Try again!!\n";    
        }
    } while (choice != "1" && choice != "2" && choice != "3");

    // store values in sendline
    sprintf(sendline, "%d\n%s %s %s\n", CREATE_USER, username, password, role);
    // send request to server with protocol: "CREATE_USER\n<username> <password> <role>\n"
    send(socketfd, sendline, strlen(sendline), 0);

    recv(socketfd, recvline, MAXLINE, 0);
    int auth = recvline[0] - '0';
    if (auth == SUCCESS)
    {
        printf("Create new account successfully!\n");
    }
    else if (auth == FAIL)
    {
        printf("Create account fail! This account already existed!!!\n");
    }
    else
    {
        perror(recvline);
        exit(4);
    }
}

void _register()
{
    char username[30], password[30];
    char sendline[MAXLINE], recvline[MAXLINE];

    cout << "Enter your username: ";
    cin >> username;
    cout << "Enter your password: ";
    cin >> password;

    // store values in sendline
    sprintf(sendline, "%d\n%s %s\n", REGISTER, username, password);
    // send request to server with protocol: "REGISTER\n<username> <password>\n"
    send(socketfd, sendline, strlen(sendline), 0);

    recv(socketfd, recvline, MAXLINE, 0);
    int auth = recvline[0] - '0';
    if (auth == SUCCESS)
    {
        printf("Register successfully!\nNow you can login with this new account.\n");
    }
    else if (auth == FAIL)
    {
        printf("This username already existed!!!\n");
    }
    else
    {
        perror(recvline);
        exit(4);
    }
}

int login(string *user)
{
    char username[30], password[30];
    // string username, password;
    char sendline[MAXLINE], recvline[MAXLINE];

    cout << "Enter your username: ";
    cin >> username;
    cout << "Enter your password: ";
    cin >> password;

    // store values in sendline
    sprintf(sendline, "%d\n%s %s\n", LOGIN, username, password);
    // send request to server with protocol: "LOGIN\n<username> <password>\n"
    send(socketfd, sendline, strlen(sendline), 0);

    recv(socketfd, recvline, MAXLINE, 0);
    int auth = recvline[0] - '0';
    if (auth == BUYER || auth == SALER || auth == ADMIN)
    {
        printf("You have logged in successfully with %s account!\n", (auth == BUYER ? "buyer" : (auth == SALER ? "saler" : "admin")));
        *user = username;
    }
    else if (auth == FAIL)
    {
        printf("Wrong username or password!!!\n");
    }
    else
    {
        perror(recvline);
        exit(4);
    }
    return auth;
}

void logout()
{
    loggedIn = 0;
    printf("Logged out successfully.\n\n");
}

void displayReceiveMessage(int *socketfd)
{
    char recvline[MAXLINE];
    int n;
    while ((n = recv(*socketfd, recvline, MAXLINE, 0)) > 0)
    {
        if (strcmp(recvline, "End"))
        {
            printf("%s", recvline);
            memset(recvline, 0, sizeof(recvline));
        }
        else
        {
            memset(recvline, 0, sizeof(recvline));
            break;
        }
    }
}

vector<std::string> receiveMovieType(int *socketfd)
{
    char recvline[MAXLINE];
    vector<std::string> movieType;
    int n;
    while ((n = recv(*socketfd, recvline, MAXLINE, 0)) > 0) {
        if (strcmp(recvline, "End")) {
            printf("%s", recvline);
            movieType.push_back(std::string(recvline));
            memset(recvline, 0, sizeof(recvline));
        } else {
            memset(recvline, 0, sizeof(recvline));
            break;
        }
    }
    return movieType;
}

void searchMovie()
{
    char title[255], sendline[MAXLINE], recvline[MAXLINE];
    int n;

    cout << "Enter the movie title you want to search:\n";
    cin >> title;

    sprintf(sendline, "%d\n%s\n", SEARCH, title);
    send(socketfd, sendline, strlen(sendline), 0);

    recv(socketfd, recvline, MAXLINE, 0);
    int response = recvline[0] - '0';
    if (response == SUCCESS)
    {
        displayReceiveMessage(&socketfd);
    }
    else if (response == FAIL)
    {
        printf("No movie matched!\n");
    }
    else
    {
        perror(recvline);
        exit(4);
    }
}

void browseMovie()
{
    int n;
    char typeId[25], cinemaId[25], weekday[25], sendline[MAXLINE], recvline[MAXLINE];

    // send request to get list of movie types
    sprintf(sendline, "%d\n", GET_LIST_TYPE);
    send(socketfd, sendline, strlen(sendline), 0);
    memset(sendline, 0, sizeof(sendline));

    recv(socketfd, recvline, MAXLINE, 0);
    int response = recvline[0] - '0';
    if (response == SUCCESS)
    {
        // display list of movie types
        displayReceiveMessage(&socketfd);
    }
    else if (response == FAIL)
    {
        printf("No type found!\n");
    }
    else
    {
        perror(recvline);
        exit(4);
    }

    cout << "Which movie type do you want to browse?\nEnter a specific typeId or enter ALL to browse all type:\n";
    cin >> typeId;
    for (int i = 0; i < strlen(typeId); ++i)
    {
        typeId[i] = toupper(typeId[i]);
    }
    // send request to get list of cinemas
    sprintf(sendline, "%d\n", GET_LIST_CINEMA);
    send(socketfd, sendline, strlen(sendline), 0);
    memset(sendline, 0, sizeof(sendline));

    recv(socketfd, recvline, MAXLINE, 0);
    response = recvline[0] - '0';
    if (response == SUCCESS)
    {
        // display list of cinemas
        displayReceiveMessage(&socketfd);
    }
    else if (response == FAIL)
    {
        printf("No cinema found!\n");
    }
    else
    {
        perror(recvline);
        exit(4);
    }

    cout << "Which cinema do you want to browse?\nEnter a specific cinemaId or enter ALL to browse all cinemas:\n";
    cin >> cinemaId;
    for (int i = 0; i < strlen(cinemaId); ++i)
    {
        cinemaId[i] = toupper(cinemaId[i]);
    }
    // display list of weekday
    cout << "What weekday do you want to browse movies?\nEnter the weekday (e.g., Monday, Tuesday) or ALL if you want to browse all days\n";
    while (true)
    {
        cin >> weekday;
        // Convert the input to lowercase for case-insensitive comparison
        transform(begin(weekday), end(weekday) - 1, begin(weekday), [](unsigned char c)
                  { return toupper(c); });

        if (string(weekday) == "MONDAY" || string(weekday) == "TUESDAY" || string(weekday) == "WEDNESDAY" ||
            string(weekday) == "THURSDAY" || string(weekday) == "FRIDAY" || string(weekday) == "SATURDAY" ||
            string(weekday) == "SUNDAY" || string(weekday) == "ALL")
        {
            break; // Input is valid, exit the loop
        }
        else
        {
            cout << "Invalid input. Please enter a valid weekday (e.g., Monday, Tuesday) or ALL\n";
        }
    }

    // send request to browse movie by type, cinema, weekday
    sprintf(sendline, "%d\n%s %s %s", BROWSE, typeId, cinemaId, weekday);
    send(socketfd, sendline, strlen(sendline), 0);

    recv(socketfd, recvline, MAXLINE, 0);
    response = recvline[0] - '0';
    if (response == SUCCESS)
    {
        // display list movies matched
        displayReceiveMessage(&socketfd);
    }
    else if (response == FAIL)
    {
        printf("No movie matched!\n");
    }
    else
    {
        perror(recvline);
        exit(4);
    }
}

bool isInteger(const string &s)
{
    if (s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+')))
        return false;

    char *p;
    strtol(s.c_str(), &p, 10);

    return (*p == 0);
}

bool isValidTicketFormat(string tickets)
{
    istringstream iss(tickets);
    string token;
    while (iss >> token)
    {
        if (token.size() < 2 || !isalpha(token[0]) || !isdigit(token[1]) || token[0] < 'A' || token[0] > 'Z' || token[1] < '1' || token[1] > '9')
        {
            return false;
        }
    }
    return true;
}

void reserve(int showtimeId, int movieId)
{
    char sendline[MAXLINE], recvline[MAXLINE];
    int noTickets = -1;
    // send request to detail info of showtime
    sprintf(sendline, "%d\n%d %d\n", GET_SHOWTIME_INFO, showtimeId, movieId);
    send(socketfd, sendline, strlen(sendline), 0);
    memset(sendline, 0, sizeof(sendline));

    recv(socketfd, recvline, MAXLINE, 0);
    int response = (int)(recvline[0] - '0');
    if (response == SUCCESS)
    {
        // display detail of showtime
        displayReceiveMessage(&socketfd);
    }
    else if (response == FAIL)
    {
        printf("No showtime matched!\n");
        return;
    }
    else
    {
        perror(recvline);
        exit(4);
    }

    // get list of tickets that buyer want to reserve
    while (true)
    {
        cout << "How many movie tickets do you want to reserve? (Enter 0 if you do not want to book ticket): ";
        string noTicketsStr;
        cin >> noTicketsStr;
        if (isInteger(noTicketsStr))
            noTickets = stoi(noTicketsStr);
        if (noTickets >= 0)
        {
            break;
            cin.ignore();
        }
        else
        {
            printf("Invalid number. The number of ticket must be non-negative! Please enter again\n");
        }
    }
    int trick = 1;
    if (noTickets > 0)
    {
        // Get list of tickets
        while (true)
        {
            string tickets;
            cout << "Enter the ticket IDs separated by space (Eg.: A1 B2 C3):\n";
            if (trick)
                cin.ignore(), trick--; // Clear input buffer
            getline(cin, tickets);
            if (!isValidTicketFormat(tickets))
            {
                printf("Invalid ticket format. Ticket IDs must be in the format [A-Z][1-9]. Example: A1 B2 C3\n");
            }
            else
            {
                // send request to reserve ticket by showtimeid, number of tickets, and list of ticket
                sprintf(sendline, "%d\n%d %d %s", RESERVE, showtimeId, noTickets, tickets.c_str());
                send(socketfd, sendline, strlen(sendline), 0);

                recv(socketfd, recvline, MAXLINE, 0);
                int response = recvline[0] - '0';
                if (response == SUCCESS)
                {
                    printf("You have successfully booked ticket(s): %s\n", tickets.c_str());
                }
                else if (response == FAIL)
                {
                    printf("Ticket booking was not successful, maybe the seat was booked by someone else or you type something wrong. Please try again!!!\n");
                    reserve(showtimeId, movieId); // Retry booking process
                }
                else
                {
                    perror(recvline);
                    exit(4);
                }
                break;
            }
        }
    }
}

void bookTicket()
{
    int n, movieId, showtimeId;
    char sendline[MAXLINE], recvline[MAXLINE];

    // send request to get list of movie
    sprintf(sendline, "%d\n", GET_LIST_MOVIES);
    send(socketfd, sendline, strlen(sendline), 0);
    memset(sendline, 0, sizeof(sendline));

    recv(socketfd, recvline, MAXLINE, 0);
    int response = recvline[0] - '0';
    if (response == SUCCESS)
    {
        // display list of movies
        displayReceiveMessage(&socketfd);
    }
    else if (response == FAIL)
    {
        printf("No movie found!\n");
        return;
    }
    else
    {
        perror(recvline);
        exit(4);
    }

    while (true)
    {
        cout << "Which movie do you want to reserve?\nEnter a specific movieId:\n";
        string movieIdStr;
        cin >> movieIdStr;
        if (isInteger(movieIdStr))
        {
            movieId = stoi(movieIdStr);

            // send request to get list of showtime by movieId
            sprintf(sendline, "%d\n%d\n", GET_LIST_SHOWTIME_BY_MOVIEID, movieId);
            send(socketfd, sendline, strlen(sendline), 0);
            memset(sendline, 0, sizeof(sendline));

            recv(socketfd, recvline, MAXLINE, 0);
            int response = recvline[0] - '0';
            if (response == SUCCESS)
            {
                // display list of showtimes
                displayReceiveMessage(&socketfd);
            }
            else if (response == FAIL)
            {
                printf("No showtime match with movieId = %d!\n", movieId);
                return;
            }
            else
            {
                perror(recvline);
                exit(4);
            }

            while (true)
            {
                cout << "Which showtime do you want to see?\nEnter a specific showtimeId:\n";
                string showtimeIdStr;
                cin >> showtimeIdStr;
                if (isInteger(showtimeIdStr))
                {
                    int showtimeId = stoi(showtimeIdStr);
                    reserve(showtimeId, movieId);
                    break;
                }
                else
                {
                    printf("Invalid input. Please enter a valid number for the showtimeId\n");
                }
            }
            break;
        }
        else
        {
            printf("Invalid input. Please enter a valid number for the movieId\n");
        }
    }
}

void addNewMovieType(){
    char typeName[50] ;
    char sendline[MAXLINE], recvline[MAXLINE];
    cout << "Enter Movie Type name: ";
    cin >> typeName;
    // store values in sendline
    sprintf(sendline, "%d\n%s\n", ADD_MOVIE_TYPE, typeName);
    send(socketfd, sendline, strlen(sendline), 0);

    recv(socketfd, recvline, MAXLINE, 0);
    int auth = recvline[0] - '0';
    if (auth == SUCCESS)
    {
        printf("Add new movie type successfully!\n");
    }
    else if (auth == FAIL)
    {
        printf("Add new movie type fail! This movie type already existed!!!\n");
    }
    else
    {
        perror(recvline);
        exit(4);
    }
}

void addMovie()
{
    char movieName[50];
    char sendline[MAXLINE], recvline[MAXLINE];
    string choice;
    char typeId[2], duration[30], description[30], role[30];
    cout << "\nAdd new movie...\n";
    cout << "Enter movie name: ";
    // cin >> movieName;
    cin.ignore();  // Ignore any previous newline character
    cin.getline(movieName, sizeof(movieName));

    sprintf(sendline, "%d\n", GET_LIST_TYPE);
    send (socketfd, sendline, strlen(sendline), 0);
    memset(sendline, 0, sizeof(sendline));
    vector<std::string> typeslist = receiveMovieType(&socketfd);
    while(true){
        cout << "Enter typeId: ";
        cin >> typeId;
        if(!isTypeInList(typeslist, typeId)){
            char c;
            cout << "TypeId is not in existing list. Do you want to add a new movie type?";
            cout << "Press y/n: ";
            cin >>c;
            if(c=='y'){
                addNewMovieType();
                break;
            }
        }
        else{
            break;
        }
    }
    
    cout << "Enter duration (HH:mm): ";
    cin >> duration;
    cout << "Enter description: ";
    cin >> description;
    // store values in sendline
    sprintf(sendline, "%d\n%s %s %s %s\n", ADD_MOVIE, movieName, typeId, duration, description);
    send(socketfd, sendline, strlen(sendline), 0);

    recv(socketfd, recvline, MAXLINE, 0);
    int auth = recvline[0] - '0';
    if (auth == SUCCESS)
    {
        printf("Add new movie successfully!\n");
    }
    else if (auth == FAIL)
    {
        printf("Add new movie fail! This movie already existed!!!\n");
    }
    else
    {
        perror(recvline);
        exit(4);
    }
}

bool isTypeInList(vector<std::string> movieslist, string typeId){
    for (string i: movieslist){
        if (i.find(typeId) != string::npos)
        {
            return true;
        }
    }
    return false;
}
void addShowTimes()
{
    char movieId[2], cinemaId[2], roomId[2], weekday[10], startTime[6], endTime[6] ;
    char sendline[MAXLINE], recvline[MAXLINE];
    string choice;
    cout << "\nAnnounce movie showings...\n";
    getListMovie();
    cin.clear();
    cout << "Enter movieId: ";
    cin >> movieId;
    getListCinema();
    cin.clear();
    cout << "Enter cinemaId: ";
    cin >> cinemaId;
    for (int i = 0; i < strlen(cinemaId); ++i)
    {
        cinemaId[i] = toupper(cinemaId[i]);
    }
    getListRoom();
    cout << "Enter roomId: ";
    cin >> roomId;
    cout << "Enter weekday (e.g., Mon, Tue, Wed, Thu, Fri, Sat, Sun): ";
    cin >> weekday;
    cout << "Enter start time (HH:mm): ";
    cin >> startTime;
    cout << "Enter end time (HH:mm): ";
    cin >> endTime;
    // store values in sendline
    sprintf(sendline, "%d\n%s %s %s %s %s %s\n", ADD_SHOW_TIMES, weekday, startTime, endTime, movieId, cinemaId, roomId);
    send(socketfd, sendline, strlen(sendline), 0);

    recv(socketfd, recvline, MAXLINE, 0);
    int auth = recvline[0] - '0';
    if (auth == SUCCESS)
    {
        printf("Announce movie showings successfully!\n");
    }
    else if (auth == FAIL)
    {
        printf("Announce movie showings fail! This movie showings already existed!!!\n");
    }
    else
    {
        perror(recvline);
        exit(4);
    }
}

void getListMovie()
{
    int n;
    char sendline[MAXLINE], recvline[MAXLINE];
    
    // send request to get list of movie 
    sprintf(sendline, "%d\n", GET_LIST_MOVIES);
    send (socketfd, sendline, strlen(sendline), 0);
    memset(sendline, 0, sizeof(sendline));

    // display list of movie 
    displayReceiveMessage(&socketfd);
}
void getListCinema()
{
    int n;
    char sendline[MAXLINE], recvline[MAXLINE];
    
    // send request to get list of cinema
    sprintf(sendline, "%d\n", GET_LIST_CINEMA);
    send (socketfd, sendline, strlen(sendline), 0);
    memset(sendline, 0, sizeof(sendline));

    recv(socketfd, recvline, MAXLINE, 0);
    int response = recvline[0] - '0';
    if (response == SUCCESS)
    {
        // display list of cinema
        displayReceiveMessage(&socketfd);
    }
    else if (response == FAIL)
    {
        printf("No cinema found!\n");
    }
    else
    {
        perror(recvline);
        exit(4);
    }
}

void getListRoom()
{
    int n;
    char sendline[MAXLINE], recvline[MAXLINE];
    
    // send request to get list of cinema
    sprintf(sendline, "%d\n", GET_LIST_ROOMS_OF_CINEMA);
    send (socketfd, sendline, strlen(sendline), 0);
    memset(sendline, 0, sizeof(sendline));

    recv(socketfd, recvline, MAXLINE, 0);
    int response = recvline[0] - '0';
    if (response == SUCCESS)
    {
        // display list of room
        displayReceiveMessage(&socketfd);
    }
    else if (response == FAIL)
    {
        printf("No room found!\n");
    }
    else
    {
        perror(recvline);
        exit(4);
    }
}

void getListType()
{
    int n;
    char sendline[MAXLINE], recvline[MAXLINE];
    
    // send request to get list of movie types
    sprintf(sendline, "%d\n", GET_LIST_TYPE);
    send(socketfd, sendline, strlen(sendline), 0);
    memset(sendline, 0, sizeof(sendline));

    recv(socketfd, recvline, MAXLINE, 0);
    int response = recvline[0] - '0';
    if (response == SUCCESS)
    {
        // display list of movie types
        displayReceiveMessage(&socketfd);
    }
    else if (response == FAIL)
    {
        printf("No type found!\n");
    }
    else
    {
        perror(recvline);
        exit(4);
    }
}