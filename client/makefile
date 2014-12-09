all: myCal

myCal: client.o
	gcc client.o -o myCal

client.o: client.c
	gcc -c -Wall client.c

clean:
	rm -f *.o myCal *~
