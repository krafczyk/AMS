#!/usr/bin/python

#you need twill libraries to run this script


import time
import twill
import StringIO
import os.path

from twill import get_browser
from twill.commands import *
from twill import commands

timeToSleepIfNoJobs = 3600	#seconds between submit atempt if no jobs available
timeToSleepBeetwenJobs = 7200		#seconds between job requests if there are jobs available

twill.set_output(StringIO.StringIO())

ppath = "pwd"
email = "dmitri.filippov@cern.ch"

while(True):
	if not os.path.exists(ppath):
		print "Config file does not exist. Exiting..."
		time.sleep(timeToSleepBeetwenJobs)
		continue
	file = open(ppath)
	passwd = file.readline().strip()
	file.close()
	go("http://pcamss0.cern.ch/rc.html")
	formvalue(1,2,email)
	submit(3)

	while (show().find("CTT") == -1):
		print "No jobs yet. Waiting..."
		time.sleep(timeToSleepIfNoJobs)

	submit(4)
	submit(16)
	formvalue(1,4,passwd)
	submit(5)
	if(show().find("User Authorization Failed") > -1):
		print "Authorization failed. Waiting for the next turn..."
	else:
		print "Jobs requested. Waiting for the next turn..."
	time.sleep(timeToSleepBeetwenJobs)



