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
#define TIME 2
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


// Checks if time conflict with two start times and two durations
bool time_conflict(string &start1, string &dur1, string &start2, string &dur2) {
	int time1 = atoi(start1.substr(7, 4).c_str());
	string duration = dur1.substr(10, dur1.size()-11);
	int hours = atoi(duration.c_str());
	int end_time1 = time1 + hours*100;

	if (duration.find(".") != string::npos) {
		float mins = atof(duration.substr(duration.find(".")).c_str());
		if (mins > 0) end_time1 += 60.0*mins;
		if ((end_time1-60) %100 == 0) end_time1 = end_time1 + 100 - 60;
	}
	
	int time2 = atoi(start2.substr(7, 4).c_str());
	duration = dur2.substr(10, dur2.size()-11);
	hours = atoi(duration.c_str());
	int end_time2 = time2 + hours*100;

	if (duration.find(".") != string::npos) {
		float mins = atof(duration.substr(duration.find(".")).c_str());
		if (mins > 0) end_time2 += 60.0*mins;
		if ((end_time2-60) %100 == 0) end_time2 = end_time2 + 100 - 60;
	}
	
	if (time1 <= time2 && time2 < end_time1) return true;
	if (time2 <= time1 && time1 < end_time2) return true;
	return false;
}

int send_response(const int r, const char response[]) {
	int rbyte;
	uint32_t response_len = strlen(response);
	uint32_t response_len2 = htonl(response_len);
		
	// Sending response message length
	if(rbyte = write(r, &response_len2, sizeof(uint32_t)) < 0){
		perror("SERVER: write response length error\n");
		return -1;
	}
	// Sending response
	if(rbyte = write(r, response, response_len) < 0){
		perror("SERVER: write response error\n");
		return -1;
	}
}


int add(const int r, char calname[], char buffer[]) {
	char response[BSIZE];
	int rbyte;
	bzero(response, BSIZE);
	strcpy(response, "<response>Event added</response>");

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
			new_File << line << endl;
			if (line.find(tokens[DATE]) != string::npos) {
				vector<string> current_tokens;
				split(line, ' ', current_tokens);
				
				// Check for time conflict and send warning response message
				if (time_conflict(tokens[TIME], tokens[DURATION], current_tokens[TIME], current_tokens[DURATION])) {
					bzero (response, BSIZE);
					strcpy (response, "<warning>Event ");
					strcat (response, tokens[EVENT].substr(6, tokens[EVENT].size()-7).c_str());
					strcat (response, " overlaps with event ");
					strcat (response, current_tokens[EVENT].substr(6, current_tokens[EVENT].size()-7).c_str());
					strcat (response, "</warning>");
				}
			}
		}
		new_File << buffer << endl;
		orig_File.close();
	
	// If calendar file doesn't exist, create, append xml header
	} else {
		cout << "NEW FILE" << endl;
		string xml_header = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
		new_File << xml_header;
		new_File << "\n";
		new_File << buffer;
		new_File << "\n";
	}
	
	new_File.close();
	string command = "mv " + replacefilename + " " + origfilename;
	system (command.c_str());
	
	send_response(r, response);
	return 1;
}

int remove_event(const int r, char calname[], char buffer[]) {
	char response[BSIZE];
	int rbyte;
	bzero(response, BSIZE);
	strcpy(response, "<response>Event removed</response>");
	
	// Initializing filenames
	string xml_ext = ".xml";
	string origfilename = calname + xml_ext;
	string replacefilename = "replace.xml";
	
	ifstream orig_File;
	ofstream new_File;

	// Check if calendar exists
	if (file_exists(origfilename.c_str())) {
		new_File.open(replacefilename.c_str());
		orig_File.open(origfilename.c_str());
		string line;
		
		// Check if event exists and removes if exists
		bool event_exists = false;
		while(getline(orig_File, line)) {
			if (line.find(buffer) != string::npos) {
				event_exists = true;
			} else new_File << line << endl;
		}
		
		// Error message to client if event doesn't exist
		if (!event_exists) {
			bzero (response, BSIZE);
			strcpy (response, "<error>Event does not exist</error>");
		}
		
		orig_File.close();
		new_File.close();
		string command = "mv " + replacefilename + " " + origfilename;
		system (command.c_str());
	
	// If calendar file doesn't exist
	} else {
		bzero (response, BSIZE);
		strcpy (response, "<error>Calendar does not exist</error>");
	}
	
	send_response(r, response);
	return 1;
}

int update(const int r, char calname[], char buffer[]) {
	char response[BSIZE];
	int rbyte;
	bzero(response, BSIZE);
	strcpy(response, "<response>Event updated</response>");
	
	// Initializing filenames
	string xml_ext = ".xml";
	string origfilename = calname + xml_ext;
	string replacefilename = "replace.xml";
	
	ifstream orig_File;
	ofstream new_File;

	// Check if calendar exists
	if (file_exists(origfilename.c_str())) {
		new_File.open(replacefilename.c_str());
		orig_File.open(origfilename.c_str());
		string line;
		
		string update_event(buffer);
		string find_event_str = update_event.substr(0, update_event.find("Info="));
		
		// Check if event exists and removes if exists
		bool event_exists = false;
		while(getline(orig_File, line)) {
			if (line.find(find_event_str) != string::npos) {
				new_File << update_event << endl;
				event_exists = true;
			} else new_File << line << endl;
		}
		
		// Error message to client if event doesn't exist
		if (!event_exists) {
			bzero (response, BSIZE);
			strcpy (response, "<error>Event does not exist</error>");
		}
		
		orig_File.close();
		new_File.close();
		string command = "mv " + replacefilename + " " + origfilename;
		system (command.c_str());
	
	// If calendar file doesn't exist
	} else {
		bzero (response, BSIZE);
		strcpy (response, "<error>Calendar does not exist</error>");
	}
	
	send_response(r, response);
	return 1;
}

int get(const int r, char calname[], char buffer[]) {
	char response[BSIZE];
	int n;
	bzero(response, BSIZE);
	strcpy(response, "<response>No more events</response>");
	
	// Initializing filenames
	string xml_ext = ".xml";
	string origfilename = calname + xml_ext;
	
	ifstream cal_File;

	// Check if calendar exists
	if (file_exists(origfilename.c_str())) {
		cal_File.open(origfilename.c_str());
		string line;
		
		// Check if event exists and removes if exists
		bool date_exists = false;
		while(getline(cal_File, line)) {
			if (line.find(buffer) != string::npos) {
				// Update status to so client knows to contiue reading
				if ((n = write(r, "M", sizeof(char))) < 0) {fprintf(stderr, "Get error when sending status to socket\n"); return 0;}
				send_response(r, line.c_str());
				date_exists = true;
			}
		}
		
		// Update status to notify client stop reading for calendar events
		if ((n = write(r, "N", sizeof(char))) < 0) {fprintf(stderr, "Get error when sending status to socket\n"); return 0;}
		
		
		// Error message to client if event doesn't exist
		if (!date_exists) {
			bzero (response, BSIZE);
			strcpy (response, "<response>There are no events for this date</response>");
		}
		
		cal_File.close();
	
	// If calendar file doesn't exist
	} else {
		bzero (response, BSIZE);
		strcpy (response, "<error>Calendar does not exist</error>");
	}
	
	send_response(r, response);
	return 1;
}



int get_slow(const int r, char calname[]) {
/*
	char response[BSIZE];
	int n;
	bzero(response, BSIZE);
	strcpy(response, "<response>No more events</response>");
	
	// Initializing filenames
	string xml_ext = ".xml";
	string origfilename = calname + xml_ext;
	
	ifstream cal_File;

	// Check if calendar exists
	if (file_exists(origfilename.c_str())) {
		cal_File.open(origfilename.c_str());
		string line;
		
		while(getline(cal_File, line)) {
			// Update status to so client knows to contiue reading
			if ((n = write(r, "M", sizeof(char))) < 0) {fprintf(stderr, "Get error when sending status to socket\n"); return 0;}
			send_response(r, line.c_str());
		}
		
		// Update status to notify client stop reading for calendar events
		if ((n = write(r, "N", sizeof(char))) < 0) {fprintf(stderr, "Get error when sending status to socket\n"); return 0;}
		
		cal_File.close();
	
	// If calendar file doesn't exist
	} else {
		bzero (response, BSIZE);
		strcpy (response, "<error>Calendar does not exist</error>");
		// Update status to notify client stop reading for calendar events
		if ((n = write(r, "N", sizeof(char))) < 0) {fprintf(stderr, "Get error when sending status to socket\n"); return 0;}
	}
	
	send_response(r, response);
	return 1;
*/
}


int main (int argc, char *argv[]){

	int r, s, rbyte, sbyte;
	uint32_t msgsize, umsgsize;
	char const *portno = PORTNO;
	struct addrinfo hint, *sinfo, *t;
	struct sockaddr_storage servaddr;
	char buffer[BSIZE], abuffer, cbuffer[BSIZE], mbuffer[BSIZE];

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
		
		//Receive XML size
		if(rbyte = read(r, &msgsize, sizeof(uint32_t)) < 0){
			perror("Server: Read size error\n");
			return 1;
		}
		msgsize = ntohl(msgsize);

		if (abuffer == 'S') {
			get_slow(r, calname);
		
		} else {
		
			//Receive XML
			bzero(mbuffer, BSIZE);
			if(rbyte = read(r, &mbuffer, msgsize) < 0){
				perror("SERVER: Read error\n");
				return -1;
			}
			mbuffer[msgsize] = 0;
			cout << "Message received: " << mbuffer << endl;


			// Fulfill request
			if(abuffer == 'A'){
				add(r, calname, mbuffer);
			
			}else if(abuffer == 'R'){
				remove_event(r, calname, mbuffer);							

			} else if(abuffer == 'U'){
				update(r, calname, mbuffer);
	
			} else if(abuffer == 'G'){
				get(r, calname, mbuffer);
		
			} else {
				perror("Action sent by client unknown!");
			}
		}
	}
}
