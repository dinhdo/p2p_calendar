/*
	Created by Kim Ngo and Dinh Do
	Dec. 4, 2014
	
*/

#include <stdio.h>#include <stdlib.h>#include <unistd.h>#include <errno.h>
#include <string.h>#include <sys/types.h>#include <sys/socket.h>#include <netinet/in.h>#include <arpa/inet.h>#include <netdb.h>
#include <stdbool.h>
#include <ctype.h>


#define SERVERNAME "student02.cse.nd.edu"
#define PORT 1203

#define BSIZE 256 // Buffer size
#define CAL 1
#define ACTION 2
#define DATE 3
#define TIME 4
#define DURATION 5
#define EVENT 6

// date format MMDDYY
bool date_format(const char date[]) {
	if (strlen(date) != 6) return false;
	while (*date)
		if (isdigit(*date++) == 0) return false;
	return true;
}

// time format HHMM
bool time_format(const char time[]) {
	if (strlen(time) != 4) return false;
	while (*time)
		if (isdigit(*time++) == 0) return false;
	return true;
}

// duration format float > 0
bool duration_format(const char duration[]) {
	if (atof(duration) == 0 || atof(duration) < 0) return false;
	return true;
}


int add(char *argv[]) {
	char buff[BSIZE];
	bzero(buff, BSIZE);
	
	if (!date_format(argv[DATE])) {printf("Date format issue\n"); return 0;}
	else if (!time_format(argv[TIME])) {printf("Time format issue\n"); return 0;}
	else if (!duration_format(argv[DURATION])) {printf("Duration format issue\n"); return 0;}
	
	strcpy(buff, "<add Date=\"");
	strcat(buff, argv[DATE]);
	strcat(buff, "\" Start=\"");
	strcat(buff, argv[TIME]);
	strcat(buff, "\" Duration=\"");
	strcat(buff, argv[DURATION]);
	strcat(buff, "\" Info=\"");
	strcat(buff, argv[EVENT]);
	strcat(buff, "\" />");
	
	printf("Calendar %s: ", argv[CAL]);
	printf("%s\n", buff);
	return 1;
}

int remove_event(char *argv[]) {
	char buff[BSIZE];
	bzero(buff, BSIZE);
	
	if (!date_format(argv[DATE])) {printf("Date format issue\n"); return 0;}
	else if (!time_format(argv[TIME])) {printf("Time format issue\n"); return 0;}
	
	strcpy(buff, "<remove Date=\"");
	strcat(buff, argv[DATE]);
	strcat(buff, "\" Start=\"");
	strcat(buff, argv[TIME]);
	strcat(buff, "\" />");
	
	printf("Calendar %s: ", argv[CAL]);
	printf("%s\n", buff);
	return 1;
}

int update(char *argv[]) {

}

int get(char *argv[]) {

}

int getslow(char *argv[]) {

}

int main(int argc, char *argv[]) {

	// Requires hostname argument in command line
	if (argc < 3) {
		printf("usage: %s [calendar_name] [action] (data for action)\n", argv[0]);
		exit(0);
	}
	
	char *action = argv[2];
	if (strcmp(action, "add") == 0) {
		if (argc == 7) add(argv);
		else printf("usage: %s [calendar_name] [add] [date] [start_time] [duration] [event_name]\n", argv[0]);
	
	} else if (action == "remove") {
		if (argc == 5) remove_event(argv);
		else printf("usage: %s [calendar_name] [remove] [date] [start_time]\n", argv[0]);
	
	} else if (action == "update") {
		if (argc == 5) update(argv);
		else printf("usage: %s [calendar_name] [update] [date] [start_time]\n", argv[0]);
	
	} else if (action == "get") {
		if (argc == 4) get(argv);
		else printf("usage: %s [calendar_name] [get] [date]\n", argv[0]);
	
	} else if (action == "getslow") {
		if (argc == 3) getslow(argv);
		else printf("usage: %s [calendar_name] [getslow]\n", argv[0]);

	} else printf("Invalid action\n");


	
	// TCP Client
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[BSIZE];
	
	portno = PORT;	// port number
	server = gethostbyname(SERVERNAME);	// host name
	
	// Openning socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "Error openning socket\n");
	}
	
	// Server information
	bzero((char*) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
	
	// Connect to port
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		fprintf(stderr, "Error connecting\n");
		exit(0);
	}
	
	
	
	close(sockfd);

return 0;
}
