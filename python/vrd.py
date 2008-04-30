#!/usr/bin/env python2.4
#  $Id: vrd.py,v 1.3 2008/04/30 10:34:06 ams Exp $

import sys, os
#sys.path.insert(0,os.environ['AMSDataDir'] + '/DataManagement/python/lib')
sys.path.insert(0,'/var/www/cgi-bin/mon/lib')
sys.path.insert(0,'./lib')
import RemoteClient


v=1
i=0
d=0
b=0
run2p=0
h=0
for x in sys.argv:
    if x == "-h": h=1
    elif x == "-i": i=1
    elif x == "-v": v=1
    elif x == "-d": d=1
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
if(html.ServerConnect()):
    html.ValidateRuns(run2p,i,v,d,h,b,1)


