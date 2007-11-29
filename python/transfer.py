#!/usr/bin/env python2.4
#  $Id: transfer.py,v 1.1 2007/11/29 11:02:28 choutko Exp $
# this script suppose to check & transfer data files from
# $AMSRawFilesI dir to /disk/Data/Raw/year/
#
import sys, os
sys.path.insert(0,os.environ['AMSDataDir'] + '/DataManagement/python/lib')
sys.path.insert(0,'./lib')
import RemoteClient


v=1
i=0
u=0
run2p=0
h=0
source=""
if(os.environ.has_key('ProductionRunsDir')):
    source=os.environ['ProductionRunsDir']
for x in sys.argv:
    if x == "-h": h=1
    elif x == "-i": i=1
    elif x == "-v": v=1
    elif x == "-u": u=1
    elif x[0:2] == "-r" :
        run2p=int(x[2:len(x)])
    elif x[0:2] == "-s" :
        source=x[2:lex(x)]
if(len(source)<2):
    print "source directory not defined, exiting "
    sys.exit()
html= RemoteClient.RemoteClient()
html.ConnectDB(1)
#if(html.ServerConnect(1)):
html.TransferDataFiles(run2p,i,v,u,h,source)


