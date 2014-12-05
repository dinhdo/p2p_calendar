#!/usr/bin/python

import subprocess

subprocess.call(["mycal", "steve", "add", "021505", "0900", "2", "Test1"])subprocess.call(["./mycal", "steve", "add", "021505", "1300", "1", "Meeting1"])
subprocess.call(["./mycal", "steve", "add", "021505", "1600", "2.5", "Class"])subprocess.call(["./mycal", "tim", "add", "021505", "1500", "1.5", "Class"])subprocess.call(["./mycal", "tim", "add", "021605", "1500", "0.5", "Seminar"])
subprocess.call(["./mycal", "steve", "add", "021605", "0800", "1.5", "Dentist"])
subprocess.call(["./mycal", "tim", "add", "021505", "14.30", "15.30", "Gym"])
subprocess.call(["./mycal", "tim", "remove", "021505", "1500"])subprocess.call(["./mycal", "martin", "add", "021705", "0900", "2.5", "Shopping"])
subprocess.call(["./mycal", "steve", "update", "021605", "0800", "2", "Dentist"])
subprocess.call(["./mycal", "tim", "add", "021705", "0900", "3", "Squash"])subprocess.call(["./mycal", "martin", "update", "021705", "0800", "3.5", "Shopping"])
subprocess.call(["./mycal", "tim", "get", "021605"])subprocess.call(["./mycal", "martin", "get", "021705"])subprocess.call(["./mycal", "steve", "get", "021505"])subprocess.call(["./mycal", "steve", "getslow", "021505"])
