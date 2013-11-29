#!/usr/bin/env python2.4

import sys, os
sys.path.insert(0,os.environ['AMSDataDir'] + '/DataManagement/python/lib')
sys.path.insert(0,os.environ['Offline'] + '/vdev/python/lib')
import RemoteClient

v=1
i=0
d=0
h=0
mt=0
castoronly=0
force=0
s=0
m=0

for x in sys.argv:
    if x == "-h": h=1
    elif x == "-i": i=1
    elif x == "-v": v=1
    elif x == "-d": d=1
    elif x == "-castoronly": castoronly=1
    elif x == "-mt": mt=1
    elif x == "-s": s=1
    elif x == "-m": m=1
html= RemoteClient.RemoteClient()
html.ConnectDB(1)
if(html.ServerConnect()):
    html.parseJournalFiles(d,i,v,h,s,m,mt,castoronly)
