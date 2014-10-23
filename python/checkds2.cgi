#!/usr/bin/env python2.4
#  $Id$
# this script suppose to check datasets
#
import sys, os
import cgi
import cgitb; cgitb.enable()
#sys.path.insert(0,'/afs/cern.ch/ams/Offline/AMSDataDir' + '/DataManagement/python/lib')
sys.path.insert(0,'./lib')
import RemoteClient
f=0
v=1
i=0
run2p=0
h=0
tabulated=0
runmin=1305853512
runmax=2000000000
source="" 
for x in sys.argv:
    if x == "-h": h=1
    elif x == "-v": v=1
    elif x == "-f": f=1
    elif x == "-ff": f=2
    elif x[0:2] == "-r" :
        run2p=int(x[2:len(x)])
    elif x[0:2] == "-d" :
        source=(x[2:len(x)])
    elif x[0:3] == "-mi" :
        runmin=(x[3:len(x)])
    elif x[0:3] == "-ma" :
        runmax=(x[3:len(x)])
html= RemoteClient.RemoteClient()
html.ConnectDB(1)
html.CheckDataSet(run2p,source,v,f,tabulated,runmin,runmax)


