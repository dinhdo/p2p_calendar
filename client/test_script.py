#!/usr/bin/python

import subprocess

subprocess.call(["mycal", "steve", "add", "021505", "0900", "2", "Test1"])
subprocess.call(["./mycal", "steve", "add", "021505", "1600", "2.5", "Class"])
subprocess.call(["./mycal", "steve", "add", "021605", "0800", "1.5", "Dentist"])
subprocess.call(["./mycal", "tim", "add", "021505", "14.30", "15.30", "Gym"])
subprocess.call(["./mycal", "tim", "remove", "021505", "1500"])
subprocess.call(["./mycal", "steve", "update", "021605", "0800", "2", "Dentist"])
subprocess.call(["./mycal", "tim", "add", "021705", "0900", "3", "Squash"])
subprocess.call(["./mycal", "tim", "get", "021605"])