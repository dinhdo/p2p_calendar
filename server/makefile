all: it_server

it_server: it_server.o
        g++ it_server.o -o it_server

it_server.o: it_server.cpp
        g++ -c it_server.cpp

clean:
        rm -f *.o it_server *~
