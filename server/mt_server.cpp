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
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <pthread.h>

#define PORTNO "1203"
#define BSIZE 256
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

using namespace std;


bool file_exists(const char *filename) {
	ifstream infile(filename);
	return infile.good();
}

int get_slow (const int r, char calname[]){
	
	cout << "\nFulfilling get_slow request from client" << endl;
	char buffer[BSIZE];
	uint32_t msgsize;
	int rbyte;
	
	ifstream orig_File;
	string xml_ext = ".xml";
	string filename = calname + xml_ext;

	if (file_exists(filename.c_str())){
		orig_File.open(filename.c_str());
		string line;
		while(getline(orig_File, line)) {					
			uint32_t response_len = line.length();
			uint32_t response_len2 = htonl(response_len);

			sleep(1);

			if(rbyte = write(r, &response_len2, sizeof(uint32_t)) < 0){
				perror("SERVER: write response length error\n");
				return -1;
			}

			if(rbyte = write(r, line.c_str(), response_len) < 0){
				perror("SERVER: write response error\n");
				return -1;
			}
		}

		orig_File.close();		

	} else{
		char error[BSIZE];
		strcpy(error, "Calendar does not exist. Cannot get_slow.");
		uint32_t msgsize = strlen(error);				
		uint32_t umsgsize = htonl(msgsize);
		int sbyte;
		
		//Send message size
		if(sbyte = write(r, &umsgsize, sizeof(uint32_t)) < 0){
			perror("Server: Error in sending error message");
			return 1;
		}

		//Send error message
		cout << "Sending error message: " << error << endl;
		if(sbyte = write(r, &error, msgsize) < 0){
			perror("Server: Error in sending error message");
			return 1;				
		}
	}

	char endmsg = ';';
	uint32_t size = sizeof(endmsg);
	uint32_t usize = htonl(size);
	if(rbyte = write(r, &usize, sizeof(uint32_t)) < 0){
		perror("SERVER: write response length error\n");
		return -1;
	}
	
	if(rbyte = write(r, &endmsg, size) < 0){
		perror("SERVER: write response error\n");
		return -1;	
	}
		

}		

void * cal_req (void * param){
	int rbyte;
	uint32_t msgsize;
	char buffer[BSIZE], abuffer, cbuffer[BSIZE];
	int r = *((int *) param);

	//Receive Action
	if(rbyte = read(r, &abuffer, sizeof(char)) < 0){
		perror("SERVER: Read error\n");
		exit;
	}

	cout << "Received action: '" << abuffer << "'"<< endl;


	//get length of filename
	if((rbyte=read(r, &msgsize, sizeof(uint32_t))) < 0){
		perror("read error: lengthname\n");
		exit(1);
	}

	msgsize = ntohl(msgsize);
	cout << "Received length of filename: " << msgsize << endl;

	bzero(cbuffer, BSIZE);
	//Receive Calendar Name
	if(rbyte = read(r, &cbuffer, msgsize) < 0){
		perror("SERVER: Read error\n");
		exit;
	}

	cout << "Received calendar name: " << cbuffer << endl;
	char calname[BSIZE];
	strcpy(calname, cbuffer);
	
	if(abuffer == 'S'){

		int S = get_slow(r, calname);

	} else if(abuffer == 'A'){

	} else if(abuffer == 'R'){
		
	} else if(abuffer == 'U'){

	} else if(abuffer == 'G'){

	}

}

int main (int argc, char *argv[]){

	int r, s, rbyte, sbyte;
	uint32_t msgsize, umsgsize;
	char const *portno = PORTNO;
	struct addrinfo hint, *sinfo, *t;
	struct sockaddr_storage servaddr;
	char buffer[BSIZE], abuffer, cbuffer[BSIZE];

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

	//Listen for clients	
	while (listen(s, 5) > -1) {

		socklen_t sinlen = sizeof(struct sockaddr_storage);

		//Create new socket
		if((r = accept(s, (struct sockaddr *)&servaddr, &sinlen))< 0){		
			fprintf(stderr, "Error accepting\n");
			return 1;
		}

		
		//Create new thread & run calendar request function
		pthread_t id;
		if(pthread_create(&id, NULL, cal_req, &r) < 0){
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
		
	}

}
		
