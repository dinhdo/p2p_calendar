/*
	Created by Kim Ngo and Dinh Do
	Dec. 4, 2014
	
*/

#include <stdio.h>#include <stdlib.h>#include <unistd.h>#include <errno.h>
#include <string.h>#include <sys/types.h>#include <sys/socket.h>#include <netinet/in.h>#include <arpa/inet.h>#include <netdb.h>
#include <stdbool.h>
#include <ctype.h>


#define SERVERNAME "student01.cse.nd.edu"
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
	if (atoi(date) < 10000 && atoi(date) < 130000) return false;
	// Month must begin with MMDDYY: 01XX

	return true;
}

// time format HHMM
bool time_format(const char time[]) {
	if (strlen(time) != 4) return false;
	if (atoi(time) < 0) return false;	// Can't be negative
	while (*time)
		if (isdigit(*time++) == 0) return false;
	return true;
}

// duration format float > 0
bool duration_format(const char duration[]) {
	if (atof(duration) == 0 || atof(duration) < 0) return false;
	return true;
}


int send_msg(const int sockfd, const char msg_type, const char msg[], const char cal[]) {
	uint32_t cal_len, msg_len, response_len;
	char response[BSIZE];
	int n;
	
	//printf("%s\n", cal);
	//printf("%s\n", msg);
	
	// Converting string lengths to network shorts
	uint32_t clen = strlen(cal);
	uint32_t mlen = strlen(msg);

	cal_len = htonl(clen);
	msg_len = htonl(mlen);
	
	if ((n = write(sockfd, &msg_type, sizeof(char))) < 0) {
		fprintf(stderr, "Add error when writing calendar length to socket\n");
		return 0;
	}
	
	// Sending calendar name length to server
	if ((n = write(sockfd, &cal_len, sizeof(uint32_t))) < 0) {
		fprintf(stderr, "Add error when writing calendar length to socket\n");
		return 0;
	}
	
	// Sending calendar name to server
	if ((n = write(sockfd, cal, clen)) < 0) {
		fprintf(stderr, "Add error when writing calenar name to socket\n");
		return 0;
	}
	
	// Sending message length to server
	if ((n = write(sockfd, &msg_len, sizeof(uint32_t))) < 0) {
		fprintf(stderr, "Add error when writing message length to socket\n");
		return 0;
	}

	
	// Sending message to server
	if ((n = write(sockfd, msg, mlen)) < 0) {
		fprintf(stderr, "Add error when writing message to socket\n");
		return 0;
	}
	
	
	/* THIS IS FOR GET FUNCTION */
	if (msg_type == 'G') {
		char status = 'M', buffer[BSIZE];
		uint32_t event_len;
		
		// Read events while status is M, N for no events
		char buff[BSIZE];
		while (status == 'M') {

			// Read status
			if ((n = read(sockfd, &status, sizeof(char))) < 0) {
				fprintf(stderr, "Get error when reading date status from socket\n");
				return 0;
			}
			if (status != 'M') break;
			
			// Read event length
			if ((n = read(sockfd, &event_len, sizeof(uint32_t))) < 0) {
				fprintf(stderr, "Get error when reading events for date length from socket\n");
				return 0;
			}
			// Read event msg
			event_len = ntohl(event_len);
			bzero(buff, BSIZE);
			if ((n = read(sockfd, buff, event_len)) < 0) {
				fprintf(stderr, "Get error when reading events for date length from socket\n");
				return 0;
			}
			printf("%s\n", buff);
		}
	}
	/* FINISHED GET FUNCTION PORTION */
	
	
	// Reading response length from server
	if ((n = read(sockfd, &response_len, sizeof(uint32_t))) < 0) {
		fprintf(stderr, "Add error when reading response length from socket\n");
		return 0;
	}
	
	// Reading response from server
	bzero(response, BSIZE);
	if ((n = read(sockfd, response, ntohl(response_len))) < 0) {
		fprintf(stderr, "Add error when reading response from socket\n");
		return 0;
	}
	printf("%s\n", response);
	return 1;
}

// adds event to calendar through the server using command line arguments
int add(const int sockfd, char *argv[]) {
	char buff[BSIZE];
	bzero(buff, BSIZE);
	int n;
	
	if (!date_format(argv[DATE])) {printf("Date format issue\n"); return 0;}
	else if (!time_format(argv[TIME])) {printf("Time format issue\n"); return 0;}
	else if (!duration_format(argv[DURATION])) {printf("Duration format issue\n"); return 0;}
	
	strcpy(buff, "<event Date=\"");
	strcat(buff, argv[DATE]);
	strcat(buff, "\" Start=\"");
	strcat(buff, argv[TIME]);
	strcat(buff, "\" Duration=\"");
	strcat(buff, argv[DURATION]);
	strcat(buff, "\" Info=\"");
	strcat(buff, argv[EVENT]);
	strcat(buff, "\" />");
	
	send_msg(sockfd, 'A', buff, argv[CAL]);
	return 1;
}


// Removes an event from a calendar through the server with command line arguments
int remove_event(const int sockfd, char *argv[]) {
	char buff[BSIZE];
	bzero(buff, BSIZE);
	uint32_t cal_len, msg_len, response_len;
	int n;
	
	if (!date_format(argv[DATE])) {printf("Date format issue\n"); return 0;}
	else if (!time_format(argv[TIME])) {printf("Time format issue\n"); return 0;}
	
	strcpy(buff, "<event Date=\"");
	strcat(buff, argv[DATE]);
	strcat(buff, "\" Start=\"");
	strcat(buff, argv[TIME]);
	strcat(buff, "\"");
	
	send_msg(sockfd, 'R', buff, argv[CAL]);
	//printf("updating %s: ", argv[CAL]);
	//printf("%s\n", buff);
	return 1;
}

int update(const int sockfd, char *argv[]) {
	char buff[BSIZE];
	bzero(buff, BSIZE);
	
	if (!date_format(argv[DATE])) {printf("Date format issue\n"); return 0;}
	else if (!time_format(argv[TIME])) {printf("Time format issue\n"); return 0;}
	else if (!duration_format(argv[DURATION])) {printf("Duration format issue\n"); return 0;}
	
	strcpy(buff, "<event Date=\"");
	strcat(buff, argv[DATE]);
	strcat(buff, "\" Start=\"");
	strcat(buff, argv[TIME]);
	strcat(buff, "\" Duration=\"");
	strcat(buff, argv[DURATION]);
	strcat(buff, "\" Info=\"");
	strcat(buff, argv[EVENT]);
	strcat(buff, "\" />");
	
	send_msg(sockfd, 'U', buff, argv[CAL]);
	return 1;
}

int get(const int sockfd, char *argv[]) {
	char buff[BSIZE], status;
	bzero(buff, BSIZE);
	uint32_t event_len;
	
	if (!date_format(argv[DATE])) {printf("Date format issue\n"); return 0;}
	
	strcpy(buff, "<event Date=\"");
	strcat(buff, argv[DATE]);
	strcat(buff, "\"");
	
	send_msg(sockfd, 'G', buff, argv[CAL]);
	


	
	//printf("getting %s: ", argv[CAL]);
	//printf("%s\n", buff);
	
	return 1;
}

int getslow(const int sockfd, char *argv[]) {
	char buff[BSIZE];
	bzero(buff, BSIZE);
	uint32_t cal_len, cal_len2, response_len;
	char response[BSIZE];
	int n;
	
	// Converting string lengths to network shorts
	cal_len = strlen(argv[CAL]);
	cal_len2 = htonl(cal_len);
	
	if ((n = write(sockfd, "S", sizeof(char))) < 0) {
		fprintf(stderr, "Add error when writing calendar length to socket\n");
		return 0;
	}
	
	// Sending calendar name length to server
	if ((n = write(sockfd, &cal_len2, sizeof(uint32_t))) < 0) {
		fprintf(stderr, "Add error when writing calendar length to socket\n");
		return 0;
	}
	
	// Sending calendar name to server
	if ((n = write(sockfd, argv[CAL], cal_len)) < 0) {
		fprintf(stderr, "Add error when writing calenar name to socket\n");
		return 0;
	}
	

	while(1){
		// Reading response length from server
		if ((n = read(sockfd, &response_len, sizeof(uint32_t))) < 0) {
			fprintf(stderr, "Add error when reading response length from socket\n");
			return 0;
		}
	
		// Reading responses from server
		bzero(response, BSIZE);
		if ((n = read(sockfd, response, ntohl(response_len))) < 0) {
			fprintf(stderr, "Add error when reading response from socket\n");
			return 0;
		}
		
		if(response[0] == ';') break;	
		printf("%s\n", response);
	}

	return 1;
}

int main(int argc, char *argv[]) {

	// Requires hostname argument in command line
	if (argc < 3) {
		printf("usage: %s [calendar_name] [action] (data for action)\n", argv[0]);
		exit(0);
	}
	
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
	
	
	
	char *action = argv[2];
	if (strcmp(action, "add") == 0) {
		if (argc == 7) add(sockfd, argv);
		else printf("usage: %s [calendar_name] [add] [date] [start_time] [duration] [event_name]\n", argv[0]);
	
	} else if (strcmp(action, "remove") == 0) {
		if (argc == 5) remove_event(sockfd, argv);
		else printf("usage: %s [calendar_name] [remove] [date] [start_time]\n", argv[0]);
	
	} else if (strcmp(action, "update") == 0) {
		if (argc == 7) update(sockfd, argv);
		else printf("usage: %s [calendar_name] [update] [date] [start_time]\n", argv[0]);
	
	} else if (strcmp(action, "get") == 0) {
		if (argc == 4) get(sockfd, argv);
		else printf("usage: %s [calendar_name] [get] [date]\n", argv[0]);
	
	} else if (strcmp(action, "getslow") == 0) {
		if (argc == 3) getslow(sockfd, argv);
		else printf("usage: %s [calendar_name] [getslow]\n", argv[0]);

	} else printf("Invalid action\n");

		
	close(sockfd);

return 0;
}
