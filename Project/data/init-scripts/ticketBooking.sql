DROP DATABASE IF EXISTS ticketBooking; 
CREATE DATABASE ticketBooking;
USE ticketBooking;

CREATE TABLE types (
    typeId INT NOT NULL AUTO_INCREMENT,
    typeName VARCHAR(255) NOT NULL,
    PRIMARY KEY (typeId)
);

CREATE TABLE movies (
    movieId INT NOT NULL AUTO_INCREMENT,
    movieName VARCHAR(255) NOT NULL,
    typeId INT,
    duration time DEFAULT '2:00',
    description TEXT,
    PRIMARY KEY (movieId),
    FOREIGN KEY (typeId) REFERENCES types (typeId)
);

CREATE TABLE cinemas (
    cinemaId INT NOT NULL AUTO_INCREMENT,
    cinemaName VARCHAR(255) NOT NULL,
    location VARCHAR(255),
    PRIMARY KEY (cinemaId)
);

CREATE TABLE rooms (
    roomid INT NOT NULL AUTO_INCREMENT,
    cinemaId INT NOT NULL,
    noOfSeats INT DEFAULT 20,
    defaultSeatMap VARCHAR(255) DEFAULT "EEEEE/EEEEE/EEEEE/EEEEE",
    PRIMARY KEY (roomid),
    FOREIGN KEY (cinemaId) REFERENCES cinemas (cinemaId)
);

CREATE TABLE showtimes (
    showTimeId INT NOT NULL AUTO_INCREMENT,
    weekday VARCHAR(50) NOT NULL,
    startTime time,
    endTime time,
    movieId INT NOT NULL, 
    cinemaId INT NOT NULL,
    roomId INT NOT NULL,
    seatMap VARCHAR(255),
    noOfEmptySeats INT,
    PRIMARY KEY (showTimeId),
    FOREIGN KEY (movieId) REFERENCES movies (movieId),
    FOREIGN KEY (cinemaId) REFERENCES cinemas (cinemaId),
    FOREIGN KEY (roomId) REFERENCES rooms (roomId)
);

DELIMITER //

CREATE TRIGGER set_showtime_seatmap_emptyseats BEFORE INSERT ON showtimes
FOR EACH ROW
BEGIN
    DECLARE default_seatmap VARCHAR(255);
    DECLARE total_seats INT;

    -- Get the defaultSeatMap and noOfSeats from the rooms table
    SELECT defaultSeatMap, noOfSeats INTO default_seatmap, total_seats
    FROM rooms
    WHERE roomid = NEW.roomId;

    -- Set seatMap and noOfEmptySeats for the newly inserted showtime
    SET NEW.seatMap = default_seatmap;
    SET NEW.noOfEmptySeats = total_seats;
END;
//
DELIMITER ;


DELIMITER //

CREATE PROCEDURE CheckRoomAvailability(IN new_startTime DATETIME, IN new_endTime DATETIME, IN new_roomId INT, IN new_weekday varchar(50)	)
BEGIN
    DECLARE room_occupied INT;

    -- Kiểm tra xem phòng có trống không trong khung giờ tương ứng
    SELECT COUNT(*)
    INTO room_occupied
    FROM showtimes
    WHERE roomId = new_roomId
        AND weekday = new_weekday
        AND NOT (
            new_startTime >= endTime 
            OR new_endTime <= startTime
        );

    IF room_occupied > 1 THEN
        SIGNAL SQLSTATE '45000'
        SET MESSAGE_TEXT = 'This room is not available for this showtime';
    END IF;
END;
//

DELIMITER ;

DELIMITER //

CREATE TRIGGER check_room_availability
BEFORE INSERT ON showtimes
FOR EACH ROW
BEGIN
    CALL CheckRoomAvailability(NEW.startTime, NEW.endTime, NEW.roomId, NEW.weekday);
END;
//

DELIMITER ;

DELIMITER //

CREATE TRIGGER check_room_availability_on_update
BEFORE UPDATE ON showtimes
FOR EACH ROW
BEGIN
    IF (NEW.startTime IS NOT NULL AND NEW.endTime IS NOT NULL AND NEW.roomId IS NOT NULL AND NEW.weekday IS NOT NULL) THEN
        CALL CheckRoomAvailability(NEW.startTime, NEW.endTime, NEW.roomId, NEW.weekday);
    END IF;
END;
//

DELIMITER ;

CREATE TABLE users (
    username VARCHAR(255) NOT NULL,
    password VARCHAR(255) NOT NULL,
    role VARCHAR(255) DEFAULT 'buyer',
    PRIMARY KEY (username)
);

INSERT INTO types (typeName) VALUES 
    ('Action'),
    ('Drama'),
    ('Comedy'),
    ('Thriller'),
    ('Adventure'),
    ('Sci-Fi'),
    ('Horror');

INSERT INTO movies (movieName, typeId, duration, description) VALUES 
    ('Inception', 5, '2:30', 'A mind-bending action thriller about dreams.'),
    ('The Shawshank Redemption', 2, '2:15', 'A gripping drama about hope and friendship.'),
    ('Pulp Fiction', 4, '2:15', 'An iconic crime film with intertwined storylines.'),
    ('The Dark Knight', 5, '2:30', 'A superhero film featuring Batman and the Joker.'),
    ('Forrest Gump', 2, '2:15', 'A heartwarming drama following a man through decades of American history.'),
    ('The Matrix', 6, '2:00', 'A groundbreaking sci-fi film exploring the nature of reality.'),
    ('The Silence of the Lambs', 4, '2:00', 'A chilling thriller about an FBI agent and a cannibalistic serial killer.'),
    ('Jaws', 1, '2:15', 'A classic horror film about a giant man-eating shark.'),
    ('Jurassic Park', 5, '2:15', 'An adventure film where dinosaurs are brought back to life.'),
    ('The Godfather', 2, '2:45', 'A crime epic about the Corleone family.');

INSERT INTO cinemas (cinemaName, location) VALUES 
    ('Cineplex Mega', '123 ABC Street, City A'),
    ('Star Cinemas', '456 XYZ Street, City B'),
    ('Central Movies', '789 DEF Street, City C'),
    ('Metropolis Movies', '101 GHI Street, City D'),
    ('Golden Theaters', '321 JKL Street, City E');
    
INSERT INTO rooms (cinemaId, noOfSeats, defaultSeatMap)
VALUES (1, 20, 'EEEEE/EEEEE/EEEEE/EEEEE');
INSERT INTO rooms (cinemaId, noOfSeats, defaultSeatMap)
VALUES (1, 25, 'EEEEEEEE/EEEEEEEEE/EEEEEEEE');
INSERT INTO rooms (cinemaId, noOfSeats, defaultSeatMap)
VALUES (1, 15, 'EEEEE/EEEEE/EEEEE');
INSERT INTO rooms (cinemaId, noOfSeats, defaultSeatMap)
VALUES (1, 24, 'EEEEE/EEEEE/EEEEE/EEEEE/EEEE');
INSERT INTO rooms (cinemaId, noOfSeats, defaultSeatMap)
VALUES (1, 22, 'EEEEEEE/EEEEE/EEEEE/EEEEE');

INSERT INTO rooms (cinemaId, noOfSeats, defaultSeatMap)
VALUES (2, 20, 'EEEEE/EEEEE/EEEEE/EEEEE');
INSERT INTO rooms (cinemaId, noOfSeats, defaultSeatMap)
VALUES (2, 25, 'EEEEEEEE/EEEEEEEEEEEE/EEEEE');
INSERT INTO rooms (cinemaId, noOfSeats, defaultSeatMap)
VALUES (2, 25, 'EEEEE/EEEEE/EEEEE/EEEEE/EEEEE');
INSERT INTO rooms (cinemaId, noOfSeats, defaultSeatMap)
VALUES (2, 24, 'EEEEE/EEEEE/EEEEE/EEEEE/EEEE');
INSERT INTO rooms (cinemaId, noOfSeats, defaultSeatMap)
VALUES (2, 23, 'EEEEEEEE/EEEEE/EEEEE/EEEEE');

INSERT INTO rooms (cinemaId, noOfSeats, defaultSeatMap)
VALUES (3, 20, 'EEEEE/EEEEE/EEEEE/EEEEE');
INSERT INTO rooms (cinemaId, noOfSeats, defaultSeatMap)
VALUES (3, 25, 'EEEEEEEE/EEEEEEEEE/EEEEEEEE');
INSERT INTO rooms (cinemaId, noOfSeats, defaultSeatMap)
VALUES (3, 15, 'EEEEE/EEEEE/EEEEE');
INSERT INTO rooms (cinemaId, noOfSeats, defaultSeatMap)
VALUES (3, 24, 'EEEEE/EEEEE/EEEEE/EEEEE/EEEE');
INSERT INTO rooms (cinemaId, noOfSeats, defaultSeatMap)
VALUES (3, 22, 'EEEEEEE/EEEEE/EEEEE/EEEEE');

INSERT INTO rooms (cinemaId, noOfSeats, defaultSeatMap)
VALUES (4, 20, 'EEEEE/EEEEE/EEEEE/EEEEE');
INSERT INTO rooms (cinemaId, noOfSeats, defaultSeatMap)
VALUES (4, 25, 'EEEEEEEE/EEEEEEEEEEEE/EEEEE');
INSERT INTO rooms (cinemaId, noOfSeats, defaultSeatMap)
VALUES (4, 25, 'EEEEE/EEEEE/EEEEE/EEEEE/EEEEE');
INSERT INTO rooms (cinemaId, noOfSeats, defaultSeatMap)
VALUES (4, 24, 'EEEEE/EEEEE/EEEEE/EEEEE/EEEE');
INSERT INTO rooms (cinemaId, noOfSeats, defaultSeatMap)
VALUES (4, 23, 'EEEEEEEE/EEEEE/EEEEE/EEEEE');


INSERT INTO rooms (cinemaId, noOfSeats, defaultSeatMap)
VALUES (5, 20, 'EEEEE/EEEEE/EEEEE/EEEEE');
INSERT INTO rooms (cinemaId, noOfSeats, defaultSeatMap)
VALUES (5, 25, 'EEEEEEEE/EEEEEEEEEEEE/EEEEE');
INSERT INTO rooms (cinemaId, noOfSeats, defaultSeatMap)
VALUES (5, 25, 'EEEEE/EEEEE/EEEEE/EEEEE/EEEEE');

INSERT INTO showtimes (weekday, startTime, endTime, movieId, cinemaId, roomId)
VALUES 
    ('Monday', '18:00', '20:30', 1, 1, 1),
    ('Monday', '20:30', '23:00', 2, 1, 2),
    ('Tuesday', '15:00', '17:30', 3, 1, 3),
    ('Tuesday', '18:30', '21:00', 4, 1, 4),
    ('Wednesday', '19:00', '21:30', 5, 1, 5),
    ('Wednesday', '21:30', '00:00', 6, 2, 6),
    ('Thursday', '17:00', '19:30', 7, 2, 7),
    ('Friday', '20:00', '22:30', 8, 2, 8),
    ('Sunday', '14:30', '17:00', 9, 2, 9),
    ('Saturday', '17:00', '20:00', 10, 3, 11);

INSERT INTO showtimes (weekday, startTime, endTime, movieId, cinemaId, roomId)
VALUES 
    ('Monday', '20:30', '23:00', 1, 1, 1);
    
INSERT INTO users (username, password, role) VALUES
    ('Liinhh NT', '123', 'buyer'),
    ('Ngoc Tran', '123', 'buyer'),
    ('Linh', '123', 'saler'),
    ('admin', 'adminpass', 'admin');

use ticketBooking;
SELECT * FROM users WHERE username = 'user7' AND password = '123';
SELECT * FROM users WHERE username = 'user12' AND password = '123';

use ticketBooking;
SELECT * from movies m join types t where movieName like '%ark%' and m.typeId=t.typeId;

