#!/usr/bin/env python2.4
#  $Id$

import sys, os
#sys.path.insert(0,os.environ['AMSDataDir'] + '/DataManagement/python/lib')
sys.path.insert(0,os.environ['Offline'] + '/vdev/python/lib')
sys.path.insert(0,'./lib')
import RemoteClient


v=1
i=0
d=0
b=0
run2p=0
h=0
u=0
mt=0
force=0
eos=0
#u =1 for reverse unchecked runs to tobererun & deleting ntuples

for x in sys.argv:
    if x == "-h": h=1
    elif x == "-i": i=1
    elif x == "-v": v=1
    elif x == "-d": d=1
    elif x == "-dd": d=2
    elif x == "-u": u=1
    elif x == "-force": force=1
    elif x == "-mt": mt=1
    elif x == "-eos": eos=1
    elif x[0:2] == "-b":
        b=1
        if(len(x)>2):
            b=int(x[2:len(x)])
    elif x[0:2] == "-r" :
        run2p=int(x[2:len(x)])
if(d==0):
    b=0   
html= RemoteClient.RemoteClient()
html.ConnectDB(1)
if(html.ServerConnect(4)):
    html.ValidateRuns(run2p,i,v,d,h,b,u,mt,0,force,0,1,0,eos)


