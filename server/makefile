all: it_server mt_server

it_server: it_server.o
	g++ it_server.o -o it_server

it_server.o: it_server.cpp
	g++ -c -Wall it_server.cpp

mt_server: mt_server.o
	g++ mt_server.o -o mt_server -lpthread

mt_server.o: mt_server.cpp
	g++ -c -Wall mt_server.cpp

clean:
	rm -f *.o it_server mt_server *~
