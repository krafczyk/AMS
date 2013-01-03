#!/usr/bin/env python2.4
#  $Id: rdlastraw24.py,v 1.1 2013/01/03 18:29:47 choutko Exp $

import sys, os,time
#sys.path.insert(0,os.environ['AMSDataDir'] + '/DataManagement/python/lib')
#sys.path.insert(0,'/var/www/cgi-bin/mon/lib')
sys.path.insert(0,'./lib')
import datetime
import DBSQLServer


run2p=""

for x in sys.argv:
    if x[0:2] == "-r" :
        run2p=(x[2:len(x)])
        run2p=" and run="+run2p



# DESCRIBE DATAFILES
# RUN                                       NOT NULL NUMBER(38)
# VERSION                                            VARCHAR2(64)
# TYPE                                               VARCHAR2(64)
# FEVENT                                             NUMBER(38)
# LEVENT                                             NUMBER(38)
# NEVENTS                                            NUMBER(38)
# NEVENTSERR                                         NUMBER(38)
# TIMESTAMP                                          NUMBER(38)
# SIZEMB                                             NUMBER(38)
# STATUS                                             VARCHAR2(64)
# PATH                                               VARCHAR2(255)
# PATHB                                              VARCHAR2(255)
# CRC                                                NUMBER(38)
# CRCTIME                                            NUMBER(38)
# CASTORTIME                                         NUMBER(38)
# BACKUPTIME                                         NUMBER(38)
# TAG                                                NUMBER(38)
# FETIME                                             NUMBER(38)
# LETIME                                             NUMBER(38)
# PATHS                                              VARCHAR2(255)

o=DBSQLServer.DBSQLServer(sys.argv,0)
ok=o.ConnectRO()
if(ok):
        xtime=int(time.time())
        sql = "SELECT run,path,pathb fROM amsdes.datafiles where type like 'SCI%%' and status like 'OK'  %s  ORDER BY run desc" %(run2p)
        ret=o.Query(sql)
        for line in ret:
                if(xtime-line[0]<86400):
                        print line[0],line[1],line[2]