#!/usr/bin/env python2.4
#  $Id: chkquota.py,v 1.3 2009/07/07 14:01:47 choutko Exp $

#
# this script suppose to check disk space usage
#

import sys, os,commands
#sys.path.insert(0,os.environ['AMSDataDir'] + '/DataManagement/python/lib')
#sys.path.insert(0,'/var/www/cgi-bin/mon/lib')
sys.path.insert(0,'./lib')
import RemoteClient


v=1
i=0
dir="/s0fc00/scratch"
warml=1500000000
hardl=1800000000
for x in sys.argv:
    if x == "-i": i=1
    elif x == "-v": v=1
    elif x[0:2] == "-w":
        warml=int(x[2:len(x)])
    elif x[0:2] == "-h":
        hardl=int(x[2:len(x)])
    elif x[0:2] == "-d" :
        dir=x[2:len(x)]
html= RemoteClient.RemoteClient()
html.CheckQuota(dir,warml,hardl,i,v)


