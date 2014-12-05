all: mycal

mycal: client.o
	gcc client.o -o mycal

client.o: client.c
	gcc -c client.c

clean:
	rm -f *.o mycal *~
