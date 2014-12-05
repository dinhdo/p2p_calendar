/*
	Dinh Do & Kim Ngo
	Networks: Project 3c
	p2p_calendar: it_server.cpp

	it_server.cpp is an iterative server that handles one client at a time. It awaits and fulfills the following requests from the client: add new event, remove event, update existing calendar event and get events for a date.

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

#define PORTNO "9766"

int main (int argc, char *argv[]){

	int r, s;
	char *portno = PORTNO;
	struct addrinfo hint, *sinfo, *t;
	struct sockaddr_storage servaddr;

	memset (&hint, 0, sizeof(struct addrinfo));
	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_flags = AI_PASSIVE;

	if((r = getaddrinfo(NULL, portno, &hint, &sinfo)) != 0){
		perror("Error in getaddrinfo\n");
		return 1;
	}
		
	for (t = sinfo; t != NULL; t = t->ai_next){
		if((s = socket(t->ai_family, t->ai_socktype, t->ai_protocol)) == -1){
			perror("server:socket error\n");
			continue;
		}

		int opt = 1;
		if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(int)) == -1){
			perror("Setsockopt error\n");
			exit(1);
		}
	
		if(bind(s, t->ai_addr, t->ai_addrlen) == -1){
			close (s);
			perror ("server: bind error\n");
			continue;
		}

		break;
	}

	
	//Check if bind was successful
	if (t == NULL){
		fprintf(stderr, "Error in bind\n");
		return 1;
	}

	freeaddrinfo(sinfo);


	while (1) {

		//listen for clients
		if(listen (s, 5) == -1){
			perror("Listen error\n");
			exit(1);
		}

		socklen_t sinlen = sizeof(struct sockaddr_storage);

		if((r = accept(s, (struct sockaddr *)&servaddr, &sinlen))< 0){		
				perror("accept error\n");
				return 1;
		}

	}
}
