#!/usr/bin/env python2.4
#  $Id: del.py,v 1.5 2011/02/14 16:32:30 ams Exp $
# this script suppose to delete datasets
# $AMSRawFilesI dir to /disk/Data/Raw/year/
#
import sys, os
sys.path.insert(0,os.environ['AMSDataDir'] + '/DataManagement/python/lib')
sys.path.insert(0,'./lib')
import RemoteClient
f=0
v=1
i=0
u=1
run2p=0
h=0
source=""
donly=0
datamc=1
for x in sys.argv:
    if x == "-h": h=1
    elif x == "-v": v=1
    elif x == "-M": datamc=0
    elif x == "-f": f=1
    elif x == "-u": u=1
    elif x == "-o": donly=1
    elif x[0:2] == "-r" :
        run2p=int(x[2:len(x)])
    elif x[0:2] == "-d" :
        source=x[2:len(x)]
if(len(source)<2):
    print "dataset  not defined, exiting "
    sys.exit()
html= RemoteClient.RemoteClient()
html.ConnectDB(1)
#donly :  only delete data, not dataruns
html.DeleteDataSet(run2p,source,u,v,f,donly,datamc)


