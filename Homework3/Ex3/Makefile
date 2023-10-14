all: client2 server2

client2: client2.o
	gcc -o client2 client2.o

client2.o: client2.c
	gcc -c client2.c

server2: server2.o
	gcc -o server2 server2.o

server2.o: server2.c
	gcc -c server2.c

clean: 
	rm *.c client2 server2