#!/usr/bin/python

import subprocess

subprocess.call(["./myCal", "steve", "add", "021505", "0900", "2", "Test1"])
subprocess.call(["./myCal", "steve", "add", "021505", "1300", "1", "Meeting1"])
subprocess.call(["./myCal", "steve", "add", "021505", "1600", "2.5", "Class"])
subprocess.call(["./myCal", "tim", "add", "021505", "1500", "1.5", "Class"])
subprocess.call(["./myCal", "tim", "add", "021605", "1500", "0.5", "Seminar"])
subprocess.call(["./myCal", "steve", "add", "021605", "0800", "1.5", "Dentist"])
subprocess.call(["./myCal", "tim", "add", "021505", "14.30", "15.30", "Gym"])
subprocess.call(["./myCal", "tim", "remove", "021505", "1500"])
subprocess.call(["./myCal", "martin", "add", "021705", "0900", "2.5", "Shopping"])
subprocess.call(["./myCal", "steve", "update", "021605", "0800", "2", "Dentist"])
subprocess.call(["./myCal", "tim", "add", "021705", "0900", "3", "Squash"])
subprocess.call(["./myCal", "martin", "update", "021705", "0800", "3.5", "Shopping"])
subprocess.call(["./myCal", "tim", "get", "021605"])
subprocess.call(["./myCal", "martin", "get", "021705"])
subprocess.call(["./myCal", "steve", "get", "021505"])
subprocess.call(["./myCal", "steve", "getslow", "021505"])
