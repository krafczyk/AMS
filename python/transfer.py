#!/usr/bin/env python2.4
#  $Id$
# this script suppose to check & transfer data files from
# $AMSRawFilesI dir to /disk/Data/Raw/year/
#
import sys, os
#sys.path.insert(0,os.environ['AMSDataDir'] + '/DataManagement/python/lib')
sys.path.insert(0,os.environ['Offline'] + '/vdev/python/lib')
#sys.path.insert(0,'./python/lib/')
#sys.path.insert(0,'lib')
import TransferDataFilesScript

p=0
c=1
v=1
i=0
u=1
run2p=0
h=0
source=""
disk=""
eos=0
if(os.environ.has_key('ProductionRunsDir')):
    source=os.environ['ProductionRunsDir']

for x in sys.argv:
    if x == "-h": h=1
    elif x == "-i": i=1
    elif x == "-v": v=1
    elif x == "-u": u=1
    elif x== "-p" : p=1
    elif x== "-pp" : p=2
    elif x == "-c0": c=0
    elif x == "-eos": eos=1
    elif x[0:2] == "-r" :
        run2p=int(x[2:len(x)])
    elif x[0:2] == "-d" :
        disk=(x[2:len(x)])
    elif x[0:2] == "-s" :
        source=x[2:len(x)]

if(len(source)<2):
    print "source directory not defined, exiting "
    sys.exit()


transfer = TransferDataFilesScript.TransferDataFilesScript()
transfer.run(run2p, i, v, u, h, source, c, p, disk, eos)


