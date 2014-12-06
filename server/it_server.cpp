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
#include <string>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#define PORTNO "1203"
#define BSIZE 256
#define DATE 1
#define START 2
#define DURATION 3
#define EVENT 4

using namespace std;

bool file_exists(const char *filename) {
	ifstream infile(filename);
	return infile.good();
}

vector<string> &split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

int add(const int r, char calname[]) {
	char buffer[BSIZE];
	uint32_t msgsize;
	int rbyte;
	
			cout << "In ADD action!" << endl;

			//Receive XML size
			if(rbyte = read(r, &msgsize, sizeof(uint32_t)) < 0){
				perror("Server: Read size error\n");
				return 1;
			}


			msgsize = ntohl(msgsize);
			cout << "Read msg size: " << msgsize << endl;
			

			//Receive XML
			bzero(buffer, BSIZE);
			cout << buffer << endl;
			if(rbyte = read(r, &buffer, msgsize) < 0){
				perror("SERVER: Read error\n");
				return -1;
			}

			buffer[msgsize] = 0;
			cout << "Received message: " << buffer << endl;
			
			// Initializing filenames
			string xml_ext = ".xml";
			string origfilename = calname + xml_ext;
			string replacefilename = "replace.xml";
			
			ifstream orig_File;
			ofstream new_File;
			new_File.open(replacefilename.c_str());


			// If calendar file exists, check for conflicts
			if (file_exists(origfilename.c_str())) {
				vector<string> tokens;
				split(buffer, ' ', tokens);
				orig_File.open(origfilename.c_str());
				string line;
				while(getline(orig_File, line)) {
					if (line.find(tokens[DATE]) != string::npos) {
						cout << "SAME DATE" << endl;
					}
				
				}
				orig_File.close();
			
			// If calendar file doesn't exist, create, append xml header
			} else {
				string xml_header = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
				new_File << xml_header;
				new_File << buffer << endl;
			}
			

			cout << "sending response length" << endl;
			char response[] = "test response message";
			uint32_t response_len = strlen(response);
			uint32_t response_len2 = htonl(response_len);
			
			if(rbyte = write(r, &response_len2, sizeof(uint32_t)) < 0){
				perror("SERVER: write response length error\n");
				return -1;
			}
			cout << "sending response" << endl;
			if(rbyte = write(r, response, response_len) < 0){
				perror("SERVER: write response error\n");
				return -1;
			}
			
			cout << "Appended event: " << buffer << endl;
			new_File.close();
			string command = "mv " + replacefilename + " " + origfilename;
			system (command.c_str());
			//fclose (orig_File);
			//fclose (new_File);
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


	while (1) {

		//Listen for Clients
		if(listen (s, 5) == -1){
			perror("Listen error\n");
			exit(1);
		}

		socklen_t sinlen = sizeof(struct sockaddr_storage);

		if((r = accept(s, (struct sockaddr *)&servaddr, &sinlen))< 0){		
				perror("accept error\n");
				return 1;
		}

		
		//Receive Action
		if(rbyte = read(r, &abuffer, sizeof(char)) < 0){
				perror("SERVER: Read error\n");
				return -1;
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
				return -1;
		}

		cout << "Received calendar name: " << cbuffer << endl;
		char calname[BSIZE];
		strcpy(calname, cbuffer);

		cout << "Cal name after strcpy: " << calname << endl;

		cout << "abuffer = " << abuffer << endl;

		// Fulfill request
		if(abuffer == 'A'){
			
			add(r, calname);
			
		}else if(abuffer == 'R'){
	
			//Receive XML
			if(rbyte = read(r, &buffer, msgsize) < 0){
				perror("SERVER: Read error\n");
				return -1;
			}

			buffer[msgsize] = 0;


			char filename [BSIZE];
			strcpy(filename, calname);
			strcat(filename, ".xml");	

			//Try to open file			
			ifstream ifile(filename);

			//If the file exists
			if(ifile){
				
				//Parse through and find block event that matches date and start above
				//Remove event *****
				
			} else {
			//If file does not exist; send error message	
					
				char error[BSIZE];
				strcpy(error, "Calendar does not exist. Cannot remove event.");
				msgsize = strlen(error);
				
				//Send message size
				umsgsize = htonl(msgsize);
				if(sbyte = write(r, &umsgsize, sizeof(uint32_t)) < 0){
					perror("Server: Error in sending error message");
					return 1;
				}

				//Send error message
				if(sbyte = write(r, &error, msgsize) < 0){
					perror("Server: Error in sending error message");
					return 1;				
				}
			} 								

		} else if(abuffer == 'U'){

			//Receive XML
			if(rbyte = read(r, &buffer, msgsize) < 0){
				perror("SERVER: Read error\n");
				return -1;
			}

			buffer[msgsize] = 0;

			char filename [BSIZE];
			strcpy(filename, calname);
			strcat(filename, ".xml");	
			
			//Try to open file			
			ifstream ifile(filename);

			//If the file exists
			if(ifile){

				//Parse through file and find block event
				//Update event with new information given
				
			} else {
			//If file does not exist; send error message	
					
				char error[BSIZE];
				strcpy(error, "Calendar does not exist. Cannot remove event.");
				msgsize = strlen(error);
				
				//Send message size
				umsgsize = htonl(msgsize);
				if(sbyte = write(r, &umsgsize, sizeof(uint32_t)) < 0){
					perror("Server: Error in sending error message");
					return 1;
				}

				//Send error message
				if(sbyte = write(r, &error, msgsize) < 0){
					perror("Server: Error in sending error message");
					return 1;				
				}
			} 
	
		} else if(abuffer == 'G'){

			//Receive date
			if(rbyte = read(r, &buffer, msgsize) < 0){
				perror("SERVER: Read error\n");
				return -1;
			}

			buffer[msgsize] = 0;

			char filename [BSIZE];
			strcpy(filename, calname);
			strcat(filename, ".xml");

			//Try to open file			
			ifstream ifile(filename);

			//If the file exists
			if(ifile){
		
				//Parse through file, locate events with matching date
				//Send info to client
				
			} else {
			//If file does not exist; send error message	
					
				char error[BSIZE];
				strcpy(error, "Calendar does not exist. Cannot remove event.");
				msgsize = strlen(error);
				
				//Send message size
				umsgsize = htonl(msgsize);
				if(sbyte = write(r, &umsgsize, sizeof(uint32_t)) < 0){
					perror("Server: Error in sending error message");
					return 1;
				}

				//Send error message
				if(sbyte = write(r, &error, msgsize) < 0){
					perror("Server: Error in sending error message");
					return 1;				
				}
			} 	
			
		
		} else if (abuffer == 'S'){

			char filename [BSIZE];
			strcpy(filename, calname);
			strcat(filename, ".xml");	
			
			//Try to open file			
			ifstream ifile(filename);

			//If the file exists
			if(ifile){
				//Send all events to client separated by block
				//1 second intervals
				
			} else {
			//If file does not exist; send error message	
					
				char error[BSIZE];
				strcpy(error, "Calendar does not exist. Cannot remove event.");
				msgsize = strlen(error);
				
				//Send message size
				umsgsize = htonl(msgsize);
				if(sbyte = write(r, &umsgsize, sizeof(uint32_t)) < 0){
					perror("Server: Error in sending error message");
					return 1;
				}

				//Send error message
				if(sbyte = write(r, &error, msgsize) < 0){
					perror("Server: Error in sending error message");
					return 1;				
				}
			} 
		
		} else {
			perror("Action sent by client unknown!");
		}		

	}
}
