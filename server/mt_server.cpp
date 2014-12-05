/*
	Dinh Do & Kim Ngo
	Networks: Project 3c
	p2p_calendar: it_server.cpp

	mt_server.cpp is a multithreaded server that handles multiple clients at a time. It awaits and fulfills the following requests from clients: add new event, remove event, update existing calendar event and get events for a date.

*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <pthread.h>


