#!/usr/bin/env python2.4
#  $Id: updtdv.py,v 1.1 2007/11/09 08:19:42 ams Exp $

import sys, os
sys.path.insert(0,os.environ['AMSDataDir'] + '/DataManagement/python/lib')
sys.path.insert(0,'./lib')
import RemoteClient


v=1
commit=0
name=""
for x in sys.argv:
    if x == "-h": h=1
    elif x == "-c": commit=1
    elif x == "-v": v=1
    elif x[0:2] == "-n" :
        name=(x[2:len(x)])

html= RemoteClient.RemoteClient()
html.ConnectDB(1)
if(html.ServerConnect()):
    html.buildTDV(name,commit,v)


