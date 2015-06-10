import cgi
import datetime
import traceback
import sys
import os
import time
import string
import re
import threading
import thread
import smtplib
import commands
import math
import socket
import pwd

import DBServer
import DBSQLServer
import JournalFile

from stat import *
from array import *

class RemoteClientException(Exception):
    def __init__(self, error):
        self.error = error
    def __str__(self):
        return self.error.encode("utf-8")

class InfoRemoteClientException(RemoteClientException):
    pass

class WarningRemoteClientException(RemoteClientException):
    pass

class ErrorRemoteClientException(RemoteClientException):
    pass

class ConnectionFailRemoteClientException(ErrorRemoteClientException):
    def __init__(self):
        self.error = "Could not connect to DB"

class CannotFindActiveProductionPeroidRemoteClientException(ErrorRemoteClientException):
    def __init__(self, path):
        self.error = "Cannot find Active Production Period for path %s" % path

class CannotAddAnyDiskRemoteClientException(ErrorRemoteClientException):
    def __init__(self):
        self.error = "doCopy-F-CannotFindAnyDisk"


def sorta(s,o):
    timenow=int(time.time())
    dts=timenow-s[2]
    dto=timenow-o[2]
    dss=s[1]
    dso=o[1]
    qs=float(dts)*float(dss)
    qo=float(dto)*float(dso)
    if qs<qo:
        return 1
    else:
        return -1

class RemoteClient:
    env={}
    dbclient=""
    sqlserver=""
    sqlconnected=-1
    productionPeriods = []
    logger = None
    cmd = None

    def getior(self,datamc=0):
        if self.sqlconnected==-1:
            self.ConnectDB()
        if(self.sqlconnected==1):
            sql="select IORS,IORP,IORD,dbfilename from Servers where status='Active' and datamc=%d order by lastupdate desc" %(datamc)
            ret=self.sqlserver.Query(sql)
            if(len(ret)>0):
                return ret[0][0],ret[0][2]
            else:
                return ""


    def ConnectDB(self, one=0):
        self.sqlserver = DBSQLServer.DBSQLServer(sys.argv, one=one, logger=self.logger)
        self.sqlconnected = self.sqlserver.Connect()
        self.setenv()
        return self.sqlconnected


    def CheckCRC(self,v,irm,upd,run2p,force,dir,nocastoronly):
        #
        #  check crc of files on disks
        #  copy from castor if bad crc found
        #  remove from disk if castor copy is eq damaged
        #  change ntuple status to 'Bad'
        #  Updates catalogs
        #
        # input par: 
        #                                     /dir are optional ones
        #  $verbose   verbose if 1
        #  $irm    
        #  $update    do sql/file rm  if 1
        #  $run2p   only process run $run2p if not 0
        #  $dir:   path to  files like /s0dat1
        #  $nocastoronly  only check crc for files without castor
        #  output par:
        #   1 if ok  0 otherwise
        #
        # 
        self.v=v 
        if irm==1: rm="rm -i "
        else: rm ="rm -f "
        # to be changed
        address="vitali.choutko@cern.ch"
        castorPrefix='/castor/cern.ch/ams'
        delimiter='MC'
        rfcp="/usr/local/bin/rfcp"
        whoami=os.getlogin()
        if not (whoami == None or whoami =='ams' or whoami=='casadmva' or whoami=='choutko'):
            print "castorPath -ERROR- script cannot be run from account : ",whoami 
            return 0

        sql="select ntuples.path,ntuples.crc,ntuples.castortime,ntuples.run,ntuples.fevent,ntuples.levent,ntuples.sizemb,ntuples.datamc from ntuples where  ntuples.path not like  '"+castorPrefix+"%' ";
        if force==0 :
            sql=sql+"  and ( ntuples.status='OK' or ntuples.status='Validated') "

        if dir != None:
            sql= sql+" and ntuples.path like '%"+dir+"%' "

        if run2p>0 :
            sql= sql+" and ntuples.run="+run2p+" ";

        if nocastoronly !=None and nocastoronly==1:
            sql=sql+" and ntuples.castortime=0 "

        sql=sql+" order by ntuples.run "
        ret_nt=self.sqlserver.Query(sql)
        totmb=0
        ntna=0
        badfs=[]
        cmb=0
        runs=0
        ntpf=0
        ntpb=0
        ntp=0
        for ntuple in ret_nt:
            totmb=totmb+ntuple[6]

        times=time.time()
        #print len(ret_nt)
        run=0
        self.checkFS(1, 600, "")
        for ntuple in ret_nt:
            if run != ntuple[3]:
                if(ntuple[7]==1):
                        delimiter='Data'
                else:   
                        delimiter='MC'
                junk=string.split(ntuple[0],delimiter)
                if len(junk)<2:
                    print "fatal problem with ",delimiter," for ",ntuple[0]," do nothing "
                    ntna=ntna+1
                    continue
                disk=(junk[0])[0:len(junk[0])-1];
                sql="select disk from filesystems where disk='"+disk+"' and isonline=1"
                ret_fs=self.sqlserver.Query(sql)
                if len(ret_fs) == 0:
                    found=0
                    for bd in badfs:
                        if bd == disk:
                            found=1
                            break
                    if not found:
                        badfs.append(disk)
                        if v:
                            print disk," is not online"
                    ntna=ntna+1
                    continue
                run=ntuple[3]
                timec=time.time()-times
                speed=cmb/(timec+1)
                timest=(totmb-cmb)/(speed+0.001)/3600
                if speed==0:
                    timest=0
                if v:
                    tn=len(ret_nt)
                    print "New run ",run,ntp," ntuples processed out of ",tn," for", timec," min ",speed," mbsec out of ",timest," hrs"
                runs=runs+1
            ntp=ntp+1
            cmb=cmb+ntuple[6]
            crccmd="/Offline/AMSDataDir/DataManagement/exe/linux/crc "+ntuple[0]+" "+str(ntuple[1])
            rstatus=self.__cmd(crccmd)
            rstatus=(rstatus>>8)
            if rstatus !=1:
                if v:
                    print ntuple[0]," crc error: ",rstatus
                ntpb=ntpb+1
                if ntuple[2]>0:
                    #copy from castor
                    a=0
                else:
                    #no castor file found and bad crc remove ntuple
                    ntpf=ntpf+1
                    if(ntuple[7]==1):
                        self.DeleteDataSet(run,dir,upd,v,force,0,ntuple[7])
        if v and ntp>0:
            print "Total of ",runs,"  runs, ",ntp," ntuples  processed. \n ",ntpb," bad ntuples found. \n ",ntpf,"  ntuples could not be repared\n ",ntna," ntuples could not be verified"


    def NotifyResp(self,message):
        # send mail message responsible for the server
        ret = self.sqlserver.sql("SELECT ADDRESS FROM MAILS WHERE RSERVER=1")
        # should be all emails from the table?
        if(len(ret[0][0])):
            address = ret[0][0]
            self.sendMailMessage(address,message," ")

         
    def CheckCRCT(self,v,irm,upd,run2p,force,dir,nocastoronly):
        #
        #  threaded version one thread per fs
        #  check crc of files on disks
        #  copy from castor if bad crc found
        #  remove from disk if castor copy is eq damaged
        #  change ntuple status to 'Bad'
        #  Updates catalogs
        #
        # input par: 
        #                                     /dir are optional ones
        #  $verbose   verbose if 1
        #  $irm    
        #  $update    do sql/file rm  if 1
        #  $run2p   only process run $run2p if not 0
        #  $dir:   path to  files like /s0dat1
        #  $nocastoronly  only check crc for files without castor
        #  output par:
        #   1 if ok  0 otherwise
        #
        # 
        if irm==1: rm="rm -i "
        else: rm ="rm -f "
        # to be changed
        global address,castorPrefix,delimiter,rfcp
        sql ="select address from mails where rserver=1"
        ret_a=self.sqlserver.Query(sql)
        address=ret_a[0][0]
        castorPrefix='/castor/cern.ch/ams'
        delimiter='MC'
        rfcp="/usr/bin/rfcp"
        whoami=os.getlogin()
        if not (whoami == None or whoami =='ams' or whoami=='casadmva' or whoami=='choutko'):
            print "castorPath -ERROR- script cannot be run from account : ",whoami 
            return 0
        sql="select ntuples.path,ntuples.crc,ntuples.castortime,ntuples.run,ntuples.fevent,ntuples.levent,ntuples.sizemb,ntuples.datamc from ntuples where  ntuples.path not like  '"+castorPrefix+"%' ";
        if force==0 :
            sql=sql+"  and ( ntuples.status='OK' or ntuples.status='Validated') "
        if dir != None:
            sql= sql+" and ntuples.path like '%"+dir+"%' "
        if run2p>0 :
            sql= sql+" and ntuples.run="+run2p+" ";
        if nocastoronly !=None and nocastoronly==1:
            sql=sql+" and ntuples.castortime=0 "
        sql=sql+" order by ntuples.run "
        ret_nt=self.sqlserver.Query(sql)
        global totmb
        totmb=0
        global badfs
        self.ntna=0
        badfs=[]
        self.update=upd
        self.cmb=0
        self.runs=0
        self.ntpf=0
        self.ntpb=0
        self.ntp=0
        self.v=v
        for ntuple in ret_nt:
            totmb=totmb+ntuple[6]
        self.times=time.time()
        self.CheckFS(1, 300, '/')
        sql="select disk from filesystems where isonline=1"
        ret_fs=self.sqlserver.Query(sql)
        global exitmutexes
        exitmutexes = {}
        global mutex
        mutex=thread.allocate_lock()
        for fs in ret_fs:
            exitmutexes[fs[0]]=thread.allocate_lock()
            thread.start_new(self.checkcrct,(fs[0],ret_nt))
            #self.checkcrct(fs[0],ret_nt)
        for key in exitmutexes.keys():
            while not exitmutexes[key].locked(): pass
        time.sleep(1)
        for key in exitmutexes.keys():
            while not exitmutexes[key].locked(): pass
        if v and self.ntp>0:
            print "Total of ",self.runs,"  runs, ",self.ntp," ntuples  processed. \n ",self.ntpb," bad ntuples found. \n ",self.ntpf,"  ntuples could not be repared\n ",self.ntna," ntuples could not be verified"

    def checkcrct(self,fs,ret_nt):
        mutex.acquire()
        print fs
        run=0
        mutex.release()
        for ntuple in ret_nt:
            mutex.acquire()
            if run != ntuple[3]:
                if(ntuple[7]==1):
                        delimiter='Data'
                else:
                        delimiter='MC'
                junk=string.split(ntuple[0],delimiter)
                if len(junk)<2:
                    print "fatal problem with ",delimiter," for ",ntuple[0]," do nothing "
                    self.ntna=self.ntna+1
                    mutex.release()
                    continue
                disk=(junk[0])[0:len(junk[0])-1];
                if disk != fs:
                    mutex.release()
                    continue
                sql="select disk from filesystems where disk='"+disk+"' and isonline=1"
                ret_fs=self.sqlserver.Query(sql)
                if len(ret_fs) == 0:
                    found=0
                    for bd in badfs:
                        if bd == disk:
                            found=1
                            break
                    if not found:
                        badfs.append(disk)
                        if self.v:
                            print disk," is not online"
                    self.ntna=self.ntna+1
                    mutex.release()
                    continue
                run=ntuple[3]
                timec=time.time()-self.times
                speed=self.cmb/(timec+1)
                timest=(totmb-self.cmb)/(speed+0.001)/3600
                if speed==0:
                    timest=0
                if self.v:
                    tn=len(ret_nt)
                    print "New run ",run,self.ntp," ntuples processed out of ",tn," for", timec," min ",speed," mbsec out of ",timest," hrs"
                self.runs=self.runs+1
            self.ntp=self.ntp+1
            self.cmb=self.cmb+ntuple[6]
            crccmd="/Offline/AMSDataDir/DataManagement/exe/linux/crc "+ntuple[0]+" "+str(ntuple[1])
            mutex.release()
            rstatus=self.__cmd(crccmd)
            mutex.acquire()
            rstatus=(rstatus>>8)
            if rstatus !=1:
                if self.v:
                    print ntuple[0]," crc error: ",rstatus
                    self.sendMailMessage(address,"crc failed"+ntuple[0]," ")
                    self.ntpb=self.ntpb+1
                if ntuple[2]>0:
                    #copy from castor
                    suc=1
                    junk=ntuple[0].split(delimiter)
                    if len(junk)>1:
                        castornt=castorPrefix+"/"+delimiter+junk[1]
                        sys=rfcp+" "+castornt+" "+ntuple[0]+".castor"
                        mutex.release()
                        i=self.__cmd(sys)
                        mutex.acquire()
                        if(i):
                            suc=0
                            if(self.v):
                                print sys," failed for ",castornt
                            self.ntpf=self.ntpf+1
                            self.__cmd("rm "+ntuple[0]+".castor")
                            continue
                        else:
                            crccmd="/Offline/AMSDataDir/DataManagement/exe/linux/crc "+ntuple[0]+".castor "+str(ntuple[1])
                            mutex.release()
                            rstatus=self.__cmd(crccmd)
                            mutex.acquire()
                            rstatus=(rstatus>>8)
                            if rstatus !=1:
                                if self.v:
                                    self.sendMailMessage(address,"crc castor failed"+castornt," ")
                                    print castornt," castor crc error: ",rstatus
                            if(suc):
                                if(self.update):
                                    self.__cmd("mv "+ntuple[0]+".castor "+ntuple[0])
                                else: self.__cmd("rm "+ntuple[0]+".castor")
                            else:
                                self.__cmd("rm "+ntuple[0]+".castor")
                                self.ntpf=self.ntpf+1
                                sql="update ntuples set ntuples.status='BAD' where ntuples.path='"+ntuple[0]+"' "
                                self.sqlserver.Update(sql)
                                sql="update ntuples set ntuples.crcflag=0 where ntuples.path='"+ntuple[0]+"' "
                                self.sqlserver.Update(sql)
                                sql="update ntuples set ntuples.path='"+castornt+"' where ntuples.path='"+ntuple[0]+"' ";
                                self.linkdataset(ntuple[0],"/Offline/DataSetsDir",0)
                                self.sqlserver.Update(sql)
                              
                                sql=" update jobs set realtriggers=realtriggers-"+str(ntuple[5])+"+"+str(ntuple[4])+"-1 where jid="+str(ntuple[3])
                                self.sqlserver.Update(sql)
                                if(self.update):
                                    fname=ntuple[0]
                                    res=self.sqlserver.Commit()
                                    if(res==0):
                                       if(self.v): print " commit failed for ",ntuple[0]
                                    else:
                                      sys=rm+" "+fname
                                      if(fname.find('/castor/cern.ch')>=0):
                                          sys="rfrm "+fname
                                      i=self.__cmd(sys)
                                      if(i): self.sendMailMessage(address,"unable to "+sys," ")
                                else: self.sqlserver.Commit(0)
                    else:
                      print "fatal error with delimiter for "+ntuple[0]+" do nothing"
                      self.ntna=self.ntna+1
                                    
                else:
                    #no castor file found and bad crc remove ntuple
                    self.ntpf=self.ntpf+1
                    sql="insert into ntuples_deleted select * from ntuples where ntuples.path='"+ntuple[0]+"'"
                    self.sqlserver.Update(sql)
                    timenow=int(time.time())
                    sql="update ntuples_deleted set timestamp="+str(timenow)+"  where path='"+ntuple[0]+"'"
                    self.sqlserver.Update(sql)
                    sql="delete from ntuples where ntuples.path='"+ntuple[0]+"'" 
                    self.sqlserver.Update(sql)
                    self.linkdataset(ntuple[0],"/Offline/DataSetsDir",0)
                    sql=" update jobs set realtriggers=realtriggers-"+str(ntuple[5])+"+"+str(ntuple[4])+"-1 where jid="+str(ntuple[3])
                    self.sqlserver.Update(sql)
                    sql="select path from ntuples where jid="+str(ntuple[3])
                    r2=self.sqlserver.Query(sql)
                    sql="select realtriggers from jobs where jid="+str(ntuple[3])
                    r3=self.sqlserver.Query(sql)
                    if(self.update):
                                    fname=ntuple[0]
                                    res=self.sqlserver.Commit()
                                    if(res==0):
                                       if(self.v): print " commit failed for ",ntuple[0]
                                    else:
                                      sys=rm+" "+fname
                                      if(fname.find('/castor/cern.ch')>=0):
                                          sys="rfrm "+fname
                                      i=self.__cmd(sys)
                                      if(i): self.sendMailMessage(address,"unable to "+sys," ")
                    else: self.sqlserver.Commit(0)

            mutex.release()
        exitmutexes[fs].acquire()


    def checkFS(self, updatedb=0, cachetime=60, path='/', vrb=0):
        #
        #  check  filesystems, update the tables accordingly if $updatedb is set
        #  status: Active  :  May be used 
        #          Reserved :  Set manually, may not be reseting
        #          Full     :  if < 3 GB left
        #         
        #  isonline: 1/0 
        #  return disk with highest available and online=1 and path like req
        #
        # add priority if the current hostname listed in the host table
        if(path== ""):
           sql="select disk from filesystems where isonline=1 and status='Active'  order by available desc" 
        else:
           sql="select disk from filesystems where isonline=1 and status='Active' and path like '%s%%' order by available desc" %(path)
        ret=self.sqlserver.Query(sql);

        if(updatedb<2 and time.time()-cachetime < self.dbfsUpdate() and len(ret)>0 ):
            return ret[0][0]

        if updatedb>0:
            sql="update mods set modtime=sysdate where tabname='fs'"
            self.sqlserver.Update(sql)
            self.sqlserver.Commit()

        if(path == ""):
           sql="select disk,host,status,allowed  from filesystems " 
        else:
           sql="select disk,host,status,allowed  from filesystems where path like '%s%%'" %(path)
        ret=self.sqlserver.Query(sql);

        for fs in ret:
               #
               #          check to see if it is can be readed
               #
               whoami=os.getuid()
               stf=fs[0]+"."+str(whoami)+".o"
               stf=re.sub(r'/','',stf)
               stf="/tmp/"+stf
               if(self.v):
                   self.__log_info(stf)

               self.__cmd("ls "+fs[0]+" 1>"+stf+" 2>&1 &")
               time.sleep(1)
               stat=[0,0,0,0,0,0,0,0,0,0,0]
               try:
                   stat =os.stat(stf)
               except:
                   self.__log_error("%s Failed to stat", stf)

               if (stat[ST_SIZE]==0 or (not os.path.isdir(fs[0]))) :
                   sql="update filesystems set isonline=0 where disk='"+str(fs[0])+"'"
                   if(self.v):
                       self.__log_info("%s Is Offline" % stf)
                   if updatedb!=0:
                       self.sqlserver.Update(sql)
                       self.sqlserver.Commit()
                   continue

               pair = self.__cmd_output("cat %s | grep Data | wc -l" % stf)
               if( not (pair[0]==0 and pair[1]=='1')):
                   sql="update filesystems set isonline=0 where disk='"+str(fs[0])+"'"
                   self.__log_info("%s Is Offline by 2" % stf)

                   if updatedb!=0:
                       self.sqlserver.Update(sql)
                       self.sqlserver.Commit()

                   continue           

               res=""
               os.unlink(stf)
               tmout="/afs/cern.ch/ams/local/bin/timeout --signal 9 10 "
               pair = self.__cmd_output("%s df -P %s | grep -v ^Filesystem | awk '{print $2, $3, $4}'" %(tmout, fs[0]))
               df_output = pair[1]
               self.__log_info(df_output)
#               try:
#                   res=os.statvfs(os.path.realpath(fs[0]))
#               except:
#                   print fs[0]," Is Offline"
#               if len(res) == 0:
               if (df_output == None or df_output == "" or df_output == 'Killed'):
                   isonline=0
               else:
                   blocks, occ, bavail = df_output.split(' ')
                   try:
                       blocks = int(blocks)
                       occ = int(occ)
                       bavail = int(bavail)
                   except:
                       self.__log_error("%s Is Offline (by df)" % fs[0])
                       continue

                   timestamp=int(time.time())
#                   bsize=res[1]
#                   blocks=res[2]
#                   bfree=res[3]
#                   bavail=res[4]
#                   files=res[5]
#                   ffree=res[6]
                   bsize = 1024
                   bfree = bavail
                   isonline=1
                   fac=float(bsize)/1024/1024
                   tot=blocks*fac
                   if(tot>1000000000):
                       status="Unknown"
                       isonline=0
                   if isonline:
                	status="Active"
                        occ=(blocks-bfree)*fac
                        if(path==""):
                            sql = "SELECT SUM(sizemb) FROM ntuples WHERE  PATH like '"+fs[0]+"%'"
                        else:
                            sql = "SELECT SUM(sizemb) FROM ntuples WHERE  PATH like '"+fs[0]+"%'"+" and path like '%"+path+"%'"
                 	sizemb=self.sqlserver.Query(sql)
                 	rused=0
                        if len(sizemb)>0 and sizemb[0][0] != None:
                            rused=sizemb[0][0]
                        if(path==""):
                            sql = "SELECT SUM(sizemb) FROM datafiles WHERE  PATH like '"+fs[0]+"%'"
                        else:
                            sql = "SELECT SUM(sizemb) FROM datafiles WHERE  PATH like '"+fs[0]+"%'"+" and path like '%"+path+"%'"
                        sizemb=self.sqlserver.Query(sql)
                        if len(sizemb)>0 and sizemb[0][0] != None:
                            rused=rused+sizemb[0][0]
                        
                        ava=bavail*fac-20000
                        ava1=tot*float(fs[3])/100-rused
                        if (fs[2].find('Reserved')>=0):
                            status='Reserved'
                        elif ava1<=0 or ava<30000:
                            status='Full'
                        if ava1< ava: ava=ava1;
                        if ava<0: ava=0
                        if(path == ""):
                            sql="update filesystems set isonline=1, totalsize="+str(tot)+", status='"+status+"',occupied="+str(occ)+",available="+str(ava)+",timestamp="+str(timestamp) +" where disk='"+fs[0]+"'"
                        else:
                            sql="update filesystems set isonline=1, totalsize="+str(tot)+", status='"+status+"',occupied="+str(occ)+",available="+str(ava)+",timestamp="+str(timestamp) +" where disk='"+fs[0]+"'"+ " and path like '"+path+"%'"
                        
                        if(vrb):
                            print sql," ava ",ava1,ava,rused,tot,fs[3]
                   else:
                       sql="update filesystems set isonline=0 where disk='%s'" % fs[0]

                   if updatedb>0:
                       self.sqlserver.Update(sql)
                       self.sqlserver.Commit()

        if(path == ""):
           sql="select disk from filesystems where isonline=1 and status='Active'  order by available desc" 
        else:
           sql="select disk from filesystems where isonline=1 and status='Active' and path='%s' order by available desc" %(path)

        ret=self.sqlserver.Query(sql)
        if(len(ret)<=0):
           self.NotifyResp('FileSystems are  Full or Offline Python: %s' % sql);
           sql="select path from ntuples where sizemb>20000"
           ret=self.sqlserver.Query(sql)
           for disk in ret:
               try:
                   fs=os.stat(disk[0])[ST_SIZE]
                   fs=int(fs/1024/1024+0.5)
                   if(fs<20000):
                       sql="update ntuples set sizemb=%d where path like '%s' " %(fs,disk[0])
                       self.sqlserver.Update(sql)
               except:
                   self.__log_error("Unable to stat %s" % disk[0])

           sql="select disk from filesystems where isonline=1 and status='Full' and path='%s' order by totalsize-occupied desc" %(path)
           ret=self.sqlserver.Query(sql)
           if(len(ret)<=0):
               self.sendMailMessage('vitali.choutko@cern.ch,baosong.shan@cern.ch,oleg.demakov@cern.ch', 'FileSystems are Offline: Exited!!!', sql)
               return None

        return ret[0][0]


    # deprecated?
    def dblupdate(self):
           part=0
           sql="select sessiontimezone from dual"
           got=self.sqlserver.Query(sql)
           if len(got)>0:
              sh1=string.split(got[0][0], ':')
              if( len(sh1)>1):
                  part=(float(sh1[0])+float(sh1[1])/60.)/24.
                        
           sql="select max(date_to_unixts(modtime-"+str(part)+")) as u_modtime from mods where tabname != 'fs'"
           rdt=self.sqlserver.Query(sql)
           lupdate=-1
           if len(rdt)>0: lupdate=rdt[0][0]
           return lupdate

    def dbfsUpdate(self):
           part=0
           got = self.sqlserver.sql("SELECT SESSIONTIMEZONE FROM DUAL")
           if len(got)>0:
              sh1=string.split(got[0][0], ':')
              if( len(sh1)>1):
                  part=(float(sh1[0])+float(sh1[1])/60.)/24.

           rdt = self.sqlserver.sql("SELECT MAX(DATE_TO_UNIXTS(MODTIME-:part)) AS U_MODTIME FROM MODS WHERE TABNAME=:fs", part=part, fs="fs")
           lupdate=-1
           if len(rdt)>0: lupdate=rdt[0][0]
           return lupdate

    # deprecated?
    def DiskIsOnline(self,disk):
           self.checkFS(1, 300, '/')
           sql="select disk from filesystems where isonline=1 and disk like '"+disk+"'"
           ret=self.sqlserver.Query(sql)
           if len(ret)>0: return 1
           return 0

    def sendMailMessage(self, add, sub, mes="", att=""):
        add = 'oleg.demakov@cern.ch'
        self.logger.info("MAIL: to='%s', subject='%s', message='%s', att='%s'", add, sub, mes, att)

        import mimetypes
        from email import Encoders
        from email.Message import Message
        from email.MIMEBase import MIMEBase
        from email.MIMEImage import MIMEImage
        from email.MIMEMultipart import MIMEMultipart
        from email.MIMEText import MIMEText

        hostname=socket.gethostname()
        mes += "\n\nHostname: %s\n\nTraceback Info:\n" %(hostname)
        for line in traceback.format_stack():
            mes += line.strip() + "\n"

        if(att == ""):
               message=MIMEText(mes)
               message['Subject']=sub
               message['To']=add
               message['From']='ams@mail.cern.ch'
               s=smtplib.SMTP()
               s.connect()
               s.sendmail(message['From'],message['To'],message.as_string())
               s.quit()
        else:
               message=MIMEMultipart()
               message['Subject']=sub
               message['To']=add
               message['From']='apache@pcamss0.cern.ch'
               ctype='text/plain'
               maintype, subtype = ctype.split('/', 1)
               msg=MIMEText(mes,_subtype=subtype)
               message.attach(msg)
               files=att.split(';')
               ctype='application/octet-stream'
               maintype, subtype = ctype.split('/', 1)
               for file in files:
                   path,name=file.split(',',1)
                   msg=MIMEBase(maintype,subtype)
                   fp = open(path)
                   msg.set_payload(fp.read())
                   fp.close()
                   encoders.encode_base64(msg)
                   msg.add_header('Content-Disposition', 'attachment', filename=name)
                   message.attach(msg)
               s=smtplib.SMTP()
               s.connect()
               s.sendmail(message['From'],message['To'],message.as_string())
               s.quit()
               
    def ValidateRuns(self,run2p,i,v,d,h,b,u,mt,datamc=0,force=0,nfs=0,castoronly=0,castorcopy=0,eos=0,skipcrc=0):
        self.s=""
        self.castorcopy=castorcopy
        self.castoronly=castoronly
        self.needfsmutex=nfs
        self.crczero=0
        self.failedcp=0
        self.thrusted=0
        self.copied=0
        self.failedcp=0
        self.bad=0
        self.unchecked=0
        self.gbDST=0
        self.run2p=run2p
        self.eos=eos
        self.skipcrc=skipcrc
        timenow=int(time.time())
        global rflag
        rglag=0
        self.validated=0
        self.checkEOS()
        HelpText="""
             validateRuns gets list of runs from production server
                  validates DSTs and copies them to final destination
                  update Runs and NTuples DB tables
     -h    - print help
     -i    - prompt before files removal
     -v    - verbose mode
     -rRun    -  perfom op only for run  Run;
     -d     -  delete completed runs/ntuples from server
     ./validateRuns.py  -v
     """
        if(i ==1):
            self.rm="rm -i "
        else: self.rm="rm -f "
        if(d==1):
            self.delete=1
        else: self.delete=0
        if(d==2):
            self.delete=2
        if(self.delete==2):
            for run in self.dbclient.rtb:
                if((run2p!=0 and run2p!=run.Run) and not(run2p<0 and run.Run>-run2p) and not(run2p/10000000000>0 and run.Run<=(run2p%10000000000))):
                    continue
                status=self.dbclient.cr(run.Status)
                if(status=='Finished' or status=='Foreign' or status == 'Canceled'):
                    uid=run.uid;
                    sql=" select ntuples.path from ntuples,dataruns where ntuples.run=dataruns.run and dataruns.status='Completed' and dataruns.jid=%d " %(uid)
                    ret=self.sqlserver.Query(sql)
                    ret2=self.sqlserver.Query("select path from ntuples where jid=-1")
                    if(datamc==0 and uid==run.Run):
                        uid=run.uid
                        sql=" select ntuples.path from ntuples,runs where ntuples.run=runs.run and runs.status='Completed' and ntuples.jid=%d " %(uid)
                        ret=self.sqlserver.Query(sql)
                        sql=" select datafiles.path from datafiles,runs where datafiles.run=runs.run and runs.status='Completed' and datafiles.type like 'MC%%' and runs.jid=%d " %(uid)
                        ret2=self.sqlserver.Query(sql)
                    for ntuple in self.dbclient.dsts:
                        if(self.dbclient.ct(ntuple.Type)=="EventTag"):
                            if(ntuple.Run==run.Run):
                                self.dbclient.iorp.sendDSTEnd(self.dbclient.cid,ntuple,self.dbclient.tm.Delete)
                                print " deleting eventtag ",ntuple.Name
                    if(len(ret)>0 or len(ret2)>0):
                        for ntuple in self.dbclient.dsts:
                            if(self.dbclient.ct(ntuple.Type)=="EventTag"):
                                if(ntuple.Run==run.Run):
#                                    self.dbclient.iorp.sendDSTEnd(self.dbclient.cid,ntuple,self.dbclient.tm.Delete)
#                                    print " deleting eventtag ",ntuple.Name
                                    continue
                            if(self.dbclient.cn(ntuple.Status)=="Validated"):
                                if(ntuple.Run==run.Run):
                                    self.dbclient.iorp.sendDSTEnd(self.dbclient.cid,ntuple,self.dbclient.tm.Delete)
                                    print " deleting ntuple ",ntuple.Name
                        print " deleting ",run.Run
                        self.dbclient.iorp.sendRunEvInfo(run,self.dbclient.tm.Delete)
                        
            return
        if(mt):
            self.mt=mt
        else: self.mt=0
        if(u>=1):
            self.deleteuncnt=1
        else: self.deleteuncnt=0
        if(b>=1):
            self.deletebad=1
        else: self.deletebad=0 
        if(b>=2):
            self.deleteunc=1
        else: self.deleteunc=0 
        if(h==1):
            print HelpText
            return 1
        if(v==1):
            self.v=1
        else: self.v=0 
        self.BadCRC=[0]
        self.CheckedDSTs=[0]
        self.BadDSTs=[0]
        self.GoodDSTs=[0]
        self.BadDSTCopy=[0]
        self.doCopyTime=0
        self.doCopyCalls=0
        self.fastntTime=0
        self.fastntCalls=0
        self.crcTime=0
        self.crcCalls=0
        self.copyTime=0
        self.copyCalls=0
        self.nCheckedCite=0
        self.nBadCopiesInRow=0
        self.CheckedRuns=[0]
        self.GoodRuns=[0]
        self.BadRuns=[0]
        # first check evr is ok
        timenow=int(time.time())
        self.valStTime=timenow
        sql = "SELECT flaglocal, timestamplocal from FilesProcessing"
        ret = self.sqlserver.Query(sql)
        if(ret[0][0]==1 and timenow-ret[0][1]<86400 and force==0):
            print "ValidateRuns-E-ProcessingFlagSet on ",ret[0][1]," exiting"
            return 0
        else: 
            print "ValidateRuns-I-ProcessingFlagSet  off ",ret[0][0],ret[0][1],force," continuing"
            self.setProcessingFlag(1, timenow, 1)

        global vdir,vlog,output
        vdir=self.getValidationDir()
        vlog=vdir+"validateRuns.log."+str(timenow)
        try:
            output=open(vlog,'w')
            print "ValidateRuns-I-Open ",vlog
        except IOError,e:
            print e
            self.setProcessingFlag(0, timenow, 1)
            return 0

        global vst
        vst=timenow
        ret=self.sqlserver.Query("SELECT min(begin) FROM productionset WHERE STATUS='Active' ORDER BY begin")
        if (len(ret)==0):
            print "ValidateRuns=E-CannotFindActiveProductionSet"
            self.setProcessingFlag(0, timenow, 1)
            return 0

        firstjobtime=ret[0][0]-24*60*60
        global exitmutexes
        exitmutexes = {}
        global mutex
        mutex=thread.allocate_lock()
        global fsmutexes
        fsmutexes = {}
        maxt=5
        if(mt>5):
            maxt=mt
        rtb_tuples=sorted(self.dbclient.rtb,key=lambda rtb: rtb.Run,reverse=True)
        for run in rtb_tuples:
            run2p=self.run2p
            if((run2p!=0 and run2p!=run.Run) and not(run2p<0 and run.Run>-run2p) and not(run2p/10000000000>0 and run.Run<=(run2p%10000000000))):
                continue
            self.CheckedRuns[0]=self.CheckedRuns[0]+1
            if( run.DataMC==datamc and mt ):
                exitmutexes[run.Run]=thread.allocate_lock()
                timenow=int(time.time())
                try:
                    if(datamc==0 and run.DataMC==datamc and run.Run!=run.uid):
                        thread.start_new(self.validatedatarun,(run,))
                    elif(datamc==0 and run.DataMC==datamc):
                        thread.start_new(self.validaterun,(run,))
                    elif(datamc==run.DataMC and datamc==1): 
                        thread.start_new(self.validatedatarun,(run,))
                        #print "thread started ",run.Run
                except:
                    i=0
                    for key in exitmutexes.keys():
                        if(not exitmutexes[key].locked()):
                            i=i+1
                    print "  exception ",i
                    cr=10000
                    while(cr>=i):
                        cr=0
                        for key in exitmutexes.keys():
                            if(not exitmutexes[key].locked()):
                                cr=cr+1
                        time.sleep(1)
                        try:
                            if(datamc==0 and run.DataMC==datamc and run.Run != run.uid):
                                thread.start_new(self.validatedatarun,(run,))
                            elif(datamc==0 and run.DataMC==datamc):
                                thread.start_new(self.validaterun,(run,))
                            elif(datamc==run.DataMC and datamc==1): 
                                thread.start_new(self.validatedatarun,(run,))
                        except:
                            print " Unable to start new thread "
                            time.sleep(60)
                it=1000
                while(it>=maxt):
                    it=0
                    for key in exitmutexes.keys():
                        if(not exitmutexes[key].locked()):
                            it=it+1
                    if(it>=maxt):
                        time.sleep(0.1)
            elif(datamc==run.DataMC and datamc==1):
                self.validatedatarun(run)
            elif(datamc==run.DataMC and datamc==0 and run.Run!=run.uid):
                print "run: %d" %(run.Run)
                self.validatedatarun(run)
            elif(datamc==run.DataMC and datamc==0):
                self.validaterun(run)
        for key in exitmutexes.keys():
            while not exitmutexes[key].locked():
                time.sleep(0.001)
                pass
        time.sleep(1)
        for key in exitmutexes:
            while not exitmutexes[key].locked():
                time.sleep(0.001)
                pass

        warn = """%s Ntuple(s) Successfully Validated.
                  %s Ntuple(s) Copied.
                  %s Ntuple(s) Not Copied.
                  %s Ntuple(s) Turned Bad.
                  %s Ntuples(s) Could Not Be Validated.
                  %s Ntuples Could Not be Validated But Assumed  OK.
              """ %(self.validated,self.copied,self.failedcp,self.bad,self.unchecked,self.thrusted)
        output.write(warn)
        output.close()

        self.printValidateStat()

        timenow=int(time.time())
        self.setProcessingFlag(0, timenow, 1)
        if(self.delete):
            for run in self.dbclient.rtb:
                if((run2p!=0 and run2p!=run.Run) and not(run2p<0 and run.Run>-run2p) and not(run2p/10000000000>0 and run.Run<=(run2p%10000000000))):
                    continue
        
                status=self.dbclient.cr(run.Status)
                if(status=='Finished' or status=='Foreign' or status == 'Canceled'):
                    uid=run.uid;
                    sql=" select ntuples.path from ntuples,dataruns where ntuples.run=dataruns.run and dataruns.status='Completed' and ntuples.jid=%d " %(uid)
                    ret=self.sqlserver.Query(sql)
                    ret2=self.sqlserver.Query("select path from ntuples where jid=-1")
                    if(datamc==0 and uid==run.Run):
                        uid=run.uid
                        sql=" select ntuples.path from ntuples,runs where ntuples.run=runs.run and runs.status='Completed' and ntuples.jid=%d " %(uid)
                        ret=self.sqlserver.Query(sql)
                        sql=" select datafiles.path from datafiles,runs where datafiles.run=runs.run and runs.status='Completed' and datafiles.type like 'MC%%' and runs.jid=%d " %(uid)
                        ret2=self.sqlserver.Query(sql)
                    if(len(ret)>0 or len(ret2)>0):
                        for ntuple in self.dbclient.dsts:
                            if(self.dbclient.ct(ntuple.Type)=="EventTag"):
                                if(ntuple.Run==run.Run):
                                    self.dbclient.iorp.sendDSTEnd(self.dbclient.cid,ntuple,self.dbclient.tm.Delete)
                                    print " deleting eventtag ",ntuple.Name
                                    continue
                            if(self.dbclient.cn(ntuple.Status)=="Validated"):
                                if(ntuple.Run==run.Run):
                                    self.dbclient.iorp.sendDSTEnd(self.dbclient.cid,ntuple,self.dbclient.tm.Delete)
                                    print " deleting ntuple ",ntuple.Name
                        print " deleting ",run.Run
                        self.dbclient.iorp.sendRunEvInfo(run,self.dbclient.tm.Delete)
                        
        if(self.deletebad):
              for run in self.dbclient.rtb:
                if((run2p!=0 and run2p!=run.Run) and not(run2p<0 and run.Run>-run2p) and not(run2p/10000000000>0 and run.Run<=(run2p%10000000000))):
                    continue
                status=self.dbclient.cr(run.Status)
                dataruns="dataruns"
                if(datamc==0 and run.uid==run.Run):
                        dataruns="runs"
                if(status=='Failed' and (datamc==1 or datamc==0)):
                    uid=run.uid;
                    sql=" update datafiles set status='BAD' where run=%d " %(run.Run)
                    if(datamc==1):
                        self.sqlserver.Update(sql)
                    sql="update %s set status='Failed' where jid=%d " %(dataruns,uid)
                    self.sqlserver.Update(sql)
                    print " deleting ",run.Run
                    self.dbclient.iorp.sendRunEvInfo(run,self.dbclient.tm.Delete)
                    self.sqlserver.Commit()

        if(self.deleteunc):
              for run in self.dbclient.rtb:
                if((run2p!=0 and run2p!=run.Run) and not(run2p<0 and run.Run>-run2p) and not(run2p/10000000000>0 and run.Run<=(run2p%10000000000))):
                    continue
                status=self.dbclient.cr(run.Status)
                if(status=='Finished' and datamc==1):
                    ro=self.sqlserver.Query("select run, status from dataruns where jid="+str(run.uid))
                    rstatus=ro[0][1]
                    uid=run.uid;
                    if(rstatus == 'Unchecked'): 
                        sql=" update datafiles set status='BAD' where run=%d " %(run.Run)
                        self.sqlserver.Update(sql)
                        sql="update dataruns set status='Failed' where jid=%d " %(uid)
                        self.sqlserver.Update(sql)
                        print " deleting ",run.Run
                        self.dbclient.iorp.sendRunEvInfo(run,self.dbclient.tm.Delete)

                    self.sqlserver.Commit()


        if(self.deleteuncnt):
              for run in self.dbclient.rtb:
                if((run2p!=0 and run2p!=run.Run) and not(run2p<0 and run.Run>-run2p) and not(run2p/10000000000>0 and run.Run<=(run2p%10000000000))):
                    continue
                status=self.dbclient.cr(run.Status)
                dataruns="dataruns"
                if(datamc==0 and run.uid==run.Run):
                    dataruns="runs"
                if(status=='Finished' and (datamc==0 or datamc==1)):
                    ro=self.sqlserver.Query("select run, status from "+dataruns+" where jid="+str(run.uid))
                    if(len(ro)<=0):
                        continue
                    rstatus=ro[0][1]
                    uid=run.uid;
                    if(rstatus == 'Unchecked'): 
                        print " updating ",run.Run
                        run.Status=self.dbclient.iorp.ToBeRerun
                        for ntuple in self.dbclient.dsts:
                            if(self.dbclient.ct(ntuple.Type)!="RootFile"):
                                continue
                            if(ntuple.Run==run.Run):
                                self.dbclient.iorp.sendDSTEnd(self.dbclient.cid,ntuple,self.dbclient.tm.Delete)
                                print " deleting ",ntuple.Name
                        run.cinfo.Status=self.dbclient.iorp.ToBeRerun
                        self.dbclient.iorp.sendRunEvInfo(run,self.dbclient.tm.Update)

        return 1


    def getValidationDir(self):
        ret = self.sqlserver.sql("SELECT MYVALUE FROM ENVIRONMENT WHERE MYKEY=:mykey", mykey='ValidationDirPath')
        if(len(ret)>0):
            return ret[0][0]
        else:
            return None

        
    def printValidateStat(self):
        timenow=int(time.time())
        vdir=self.getValidationDir()
        if(vdir == None):
            vdir="/tmp"
        vlog=vdir+"/validationRunsSummary.log."+str(timenow)
        try:
            output=open(vlog,'w')
        except IOError,e:
            print e
            return 0
        fl="------------- ValidateRuns ------------- "
        print fl
        output.write(fl)
        stime=time.asctime(time.localtime(self.valStTime))
        ltime=time.asctime(time.localtime(timenow))
        t0="Start Time : "+stime
        t1="End Time : "+ltime
        print t0,t1
        output.write(t0)
        output.write(t1)
        l0="   Runs (Listed, Completed, Bad, Failed) : %d,  %d, %d " %(self.CheckedRuns[0],self.GoodRuns[0],self.BadRuns[0])
        l1=" DSTs (Checked, Good, Bad, CRCi, CopyFail/CRC ) : %d, %d, %d, %d, %d) " %(self.CheckedDSTs[0], self.GoodDSTs[0], self.BadDSTs[0], self.BadCRC[0], self.BadDSTCopy[0])
        print l0
        print l1
        output.write(l0)
        output.write(l1)
        totalgb=self.gbDST/1024.
        chgb="Total GB %3.1f " %(totalgb)
        print chgb
        hours   = float(timenow - self.valStTime)/60./60.
        ch0="Total Time %3.1f hours " %(hours)
        ch1="doCopy (calls, time): %5d %3.1fh [cp file :%5d %3.1f] " %(self.doCopyCalls,float(self.doCopyTime)/60./60., self.copyCalls,float(self.copyTime)/3600.)
        ch2="CRC (calls,time) : %5d %3.1fh; Validate (calls,time) : : %5d %3.1fh " %(self.crcCalls,float(self.crcTime)/3600.,self.fastntCalls, float(self.fastntTime)/3600.)
        print ch0
        print ch1
        print ch2
        output.write(chgb)
        output.write(ch0)
        output.write(ch1)
        output.write(ch2)
        ll="-------------     Thats It          -------------"
        print ll
        output.write(ll)
        output.close()
        
    def validaterun(self,run):
        if(self.nBadCopiesInRow>30):
            output.write("too many docopy failures")
            print "too many docopy failurs"
            return 
        mutex.acquire()
        print "run started ",run.Run,run.uid
        odisk=None
        rmcmd=[]
        rmbad=[]
        rmdir=""
        cpntuples=[]
        mvntuples=[]
        timestamp=int(time.time())
        outputpath=None
	copyfailed=0
	runupdate="update runs set "
        interrupt=self.isinterrupt()
        if(interrupt):
            self.run2p=1
        ro=self.sqlserver.Query("select run, status from runs where run="+str(run.Run))
        if(len(ro)==0):
            self.InsertRun(run)
        ro=self.sqlserver.Query("select run, status from runs where run="+str(run.Run))
        r1=self.sqlserver.Query("select count(path)  from ntuples where run="+str(run.Run))
        r2=self.sqlserver.Query("select count(path)  from datafiles where type like 'MC%' and run="+str(run.Run))
        status=ro[0][1]
        status2=""
        if(status== 'Completed' and r1[0][0]==0 and r2[0][0]==0):
            status="Unchecked"
        if(status != 'Completed' and status != self.dbclient.cr(run.Status)):
            sql="update runs set status='%s' where run=%d" %(self.dbclient.cr(run.Status),run.Run)
            self.sqlserver.Update(sql)
        if(self.dbclient.cr(run.Status) == "Finished" and status != "TimeOut" and status !="Completed"):
            print "Check Run %d Status %s DBStatus %s" %(run.Run,self.dbclient.cr(run.Status),status)
            output.write("Check Run %d Status %s DBStatus %s" %(run.Run,self.dbclient.cr(run.Status),status))
            sql="select runs.status, jobs.content, cites.status from runs,jobs,cites where jobs.jid=%d and runs.jid=jobs.jid and cites.cid=jobs.cid" %(run.Run)
            r1=self.sqlserver.Query(sql)
            if(len(r1)==0):
                self.sqlserver.Update("update runs set status='Failed' where run="+str(run.Run))
                print "cannot find status, content in Jobs for JID=%d" %(run.Run)
                output.write( "cannot find status, content in Jobs for JID=%d" %(run.Run))
                sql="insert into jobs select jobs_deleted.* from jobs_deleted where jobs_deleted.jid=%d"  %(run.uid);
                self.sqlserver.Update(sql);
                self.sqlserver.Commit();
            else:
                jobstatus=r1[0][0]
                jobcontent=r1[0][1]
                citestatus=r1[0][2]
                # '-GR' runs
                if(jobcontent.find("-GR")>=0  or citestatus == "local"):
                    mips=run.cinfo.Mips
                    events=run.cinfo.EventsProcessed
                    errors=run.cinfo.CriticalErrorsFound
                    cputime=run.cinfo.CPUMipsTimeSpent
                    elapsed="%.2f" %(run.cinfo.TimeSpent)
                    host=run.cinfo.HostName
                    if(mips<=0):
                        print "Mips Problem for %d %s" %(mips, host)
                        mips=1000
                        if(events==0):
                            events=run.LastEvent-run.FirstEvent
                    cputime=cputime/mips*1000
                    cputime="%.2f" %(cputime)
                     
                    if(events==0 and errors==0 and self.dbclient.cr(run.Status) == "Finished"):
                        print "run events 0 "
                        self.sqlserver.Update("update runs set status='Unchecked' where run="+str(run.Run))
                    else:
 			if(cputime=="nan"):
				cputime=elapsed
                        sql="update jobs set events=%d, errors=%d, cputime=%s,elapsed=%s,host='%s',mips=%d,timestamp=%d where jid=%d" %(events,errors,cputime,elapsed,host,int(mips),timestamp,run.Run)
                        self.sqlserver.Update(sql)
                        output.write(sql)
                        self.sqlserver.Update("delete ntuples where jid="+str(run.uid))
                        ntuplelist=[]
                        for ntuple in self.dbclient.dsts:
                            if(self.dbclient.ct(ntuple.Type)!="RootFile" and self.dbclient.ct(ntuple.Type)!="Ntuple" and self.dbclient.ct(ntuple.Type)!="RawFile"):
                                continue
                            #print ntuple.Run,run.Run,self.dbclient.cn(ntuple.Status)
                            if( (self.dbclient.cn(ntuple.Status) == "Success" or  self.dbclient.cn(ntuple.Status) == "Validated") and ntuple.Run == run.Run):
                                ntuplelist.append(ntuple)
                        ntuplelist.sort(lambda x,y: cmp(y.Insert,x.Insert))
                        fevt=-1
                        dat0=0
                        for ntuple in ntuplelist:
                            if(fevt>=0 and ntuple.LastEvent>fevt):
                                ntuple.EventNumber=-1
                                print " problems with %d %s " %(run.Run,ntuple.Name)
                            else:
                                if(self.dbclient.ct(ntuple.Type)!="RawFile"):
                                    fevt=ntuple.FirstEvent
                                if(str(ntuple.Name).find(':/dat0')>=0):
                                    dat0=1
                        fevent=1
                        levent=0
                        for ntuple in ntuplelist:
                            delim=':'
                            if(str(ntuple.Name).find('///')>=0):
                                delim='//'
                            if(ntuple.EventNumber==-1):
                                fpatha=str(ntuple.Name).split(delim)
                                fpath=fpatha[len(fpatha)-1]
                                if(fpath.find('/castor/cern.ch')>=0):
                                    rmbad.append("rfrm "+fpath)
                                else:
                                    rmbad.append("rm "+fpath)
                            else:
                                if(str(ntuple.Name).find(':/dat0')>=0):
                                    ntuple.Status=self.dbclient.iorp.Success
                                self.CheckedDSTs[0]=self.CheckedDSTs[0]+1
                                levent=levent+ntuple.LastEvent-ntuple.FirstEvent+1
                                fpatha=str(ntuple.Name).split(delim)
                                fpath=fpatha[len(fpatha)-1]
                                badevents=ntuple.ErrorNumber
                                events=ntuple.EventNumber
                                status="OK"
                                rcp=1
                                if( not os.path.isfile(fpath) and fpath.find('/castor/cern.ch')<0 and fpath.find('/eosams/')<0):
                                        # try to copy to local dir
                                        parser=str(ntuple.Name).split(':')
                                        if(len(parser)>1):
                                           host=parser[0]
                                           hostok=0
                                           if(host.find('.cern.ch')>=0):
                                               hostok=1
                                           else:
                                               hparser=host.split('.')
                                               if(len(hparser)>1 and hparser[1]=='om'):
                                                   host=hparser[0]+".cern.ch"
                                                   hostok=1
                                               else:
                                                   print "host bizarre ",host
                                           if(hostok):
                                               dir=fpath.split(str(run.Run))
                                               cmd="mkdir -p "+dir[0]
                                               i=self.__cmd(cmd)
                                               cmd="scp -2 %s:%s %s " %(host,fpath,fpath)
                                               if(dir[0].find('/dat0/')>=0):
                                                   mutex.release()
                                                   i=self.__cmd(cmd)
                                                   mutex.acquire()
                                               else:
                                                   i=1
                                                   print "scp not executed for ",dir[0]
                                               if(i):
                                                   print cmd," failed nax"
                                                   rcp=0
                                               else:
                                                   cmd="ssh -x -2 "+host+" rm "+fpath
                                                   rmcmd.append(cmd)
                                        if(not os.path.isfile(fpath) and fpath.find('/castor/cern.ch')<0 and fpath.find('/eosams/')<0):
                                            print "unable to open file ",fpath

                                retcrc = self.compareCRC(fpath, ntuple.crc)
                                if(retcrc !=1):
                                            print "ValidateRuns-E_Error-CRC status ",retcrc
                                            output.write("ValidateRuns-E_Error-CRC status "+str(retcrc))
                                            self.BadCRC[0]=self.BadCRC[0]+1
                                            self.BadDSTs[0]=self.BadDSTs[0]+1
                                            self.bad=self.bad+1
                                            levent=levent-(ntuple.LastEvent-ntuple.FirstEvent+1)
                                            if(fpath.find('/castor/cern.ch')>=0):
                                                rmbad.append("rfrm "+fpath)
                                            else:
                                                rmbad.append("rm "+fpath)
                                            mvntuples.append(fpath)
                                else:
                                            (ret,i)=self.validateDST(fpath,ntuple.EventNumber,self.dbclient.ct(ntuple.Type),ntuple.LastEvent)
                                            if( i == 0xff00 or (i & 0xff)):
                                                if(ntuple.Status != self.dbclient.iorp.Validated):
                                                    status="Unchecked"
                                                    events=ntuple.EventNumber
                                                    self.unchecked=self.unchecked+1
                                                    copyfailed=1
                                                    break
                                                else:
                                                    self.thrusted=self.thrusted+1
                                            else:
                                                i= i>>8 
                                                if(i==134):
                                                    status2="STAGEIN"
                                                if(i/128):
                                                    events=0
                                                    status="Bad"+str(i-128)
                                                    self.bad=self.bad+1
                                                    levent=levent-(ntuple.LastEvent-ntuple.FirstEvent+1)
                                                    if(fpath.find('/castor/cern.ch')>=0):
                                                        rmbad.append("rfrm "+fpath)
                                                    else:
                                                        rmbad.append("rm "+fpath)
                                                else:
                                                    status="OK"
                                                    events=ntuple.EventNumber
                                                    badevents=(i*events/100)
                                                    self.validated=self.validated+1
                                                    (outputpatha,rstatus,odisk,castortime)=self.doCopy(run.Run,fpath,ntuple.crc,ntuple.Version,outputpath)
                                                    outputpath=None
                                                    if(outputpatha != None):
                                                        outputpath=outputpatha[:]
                                                        mvntuples.append(outputpath)
                                                    if(rstatus==1):
                                                        self.GoodDSTs[0]=self.GoodDSTs[0]+1
                                                        self.nBadCopiesInRow=0
                                                        if (self.skipcrc):
                                                            crctime = 0
                                                        else:
                                                            crctime = ntuple.Insert
                                                        self.InsertNtuple(run.Run,ntuple.Version,self.dbclient.ct(ntuple.Type),run.Run,ntuple.FirstEvent,ntuple.LastEvent,events,badevents,ntuple.Insert,ntuple.size,status,outputpath,ntuple.crc,crctime,1,castortime,run.DataMC)
                                                        output.write("insert %d %s %s %d" %(run.Run, outputpath, status,int(ntuple.size)))
                                                        self.copied=self.copied+1
                                                        self.gbDST=self.gbDST+ntuple.size
                                                        cpntuples.append(fpath)
                                                        runupdate="update runs set fevent="+str(fevent)+", levent="+str(levent)+", "
                                                    else:
                                                        self.BadDSTs[0]=self.BadDSTs[0]+1
                                                        self.BadDSTCopy[0]=self.BadDSTCopy[0]+1
                                                        output.write("failed to copy or wrong crc for %s" %(fpath))
                                                        copyfailed=1
                                                        self.nBadCopiesInRow=self.nBadCopiesInRow+1
                                                        levent=levent-(ntuple.LastEvent-ntuple.FirstEvent+1)
                                                        self.bad=self.bad+1
                                                        if(outputpath != None):
                                                            cmd="rm "+outputpath
                                                            if(outputpath.find('/castor/cern.ch')>=0):
                                                                cmd="rfrm "+outputpath
                                                            rstat=self.__cmd(cmd)
                                                            output.write("remove bad file "+cmd)

                        status="Failed"
                        if(odisk !=  None):
                            if(fsmutexes.has_key(odisk)):
                                fsmutexes[odisk].release()
                        if(copyfailed==0):
                            warn="Validation done Run %d " %(run.Run)
                            print warn
                            output.write(warn)
                        if(len(cpntuples)>0 and len(mvntuples)>0):
                               status='Completed'
                        elif(len(mvntuples)>0):
                             warn="Validation/copy failed run %d " %(run.Run)
                             print warn
                             output.write(warn)
                             status="Unchecked"
                             for ntuple in mvntuples:
                                 cmd="rm "+ntuple
                                 if(ntuple.find('/castor/cern.ch')>=0):
                                     cmd="rfrm "+ntuple
                                 self.__cmd(cmd)
                                 print "Validation failed "+cmd
                                 output.write("Validation failed "+cmd)
                                 self.failedcp=self.failedcp+1
                                 self.copied=self.copied-1
                             sql="delete ntuples where jid=%d" %(run.uid)
                             self.sqlserver.Update(sql)
                             runupdate="update runs set "
                             
                        else:
                            status="Unchecked"
#                        print "status ",status,len(status2),status2
                        if(len(status2)>0):
                            status=status2
                        if(status == "Completed"):
                            self.GoodRuns[0]=self.GoodRuns[0]+1
                        elif (status =="Failed"):
                            self.BadRuns[0]=self.BadRuns[0]+1
                        sql= "update runs set status='%s' where run=%d" %(status,run.Run)
                        self.sqlserver.Update(sql)
                        sql="select sum(ntuples.levent-ntuples.fevent+1),min(ntuples.fevent)  from ntuples,runs where ntuples.run=runs.run and runs.run="+str(run.Run)
                        r4=self.sqlserver.Query(sql)
                        if(len(r4)==0 or r4[0][0]==None):
                            sql="select sum(datafiles.levent-datafiles.fevent+1),min(datafiles.fevent)  from datafiles,runs where datafiles.run=runs.run and datafiles.type like 'MC%' and runs.run="+str(run.Run)
                            r4=self.sqlserver.Query(sql)
                            if(len(r4)==0):
                                ntevt=0
                                fevt=0
                            else:
                                ntevt=r4[0][0]
                                fevt=r4[0][1]
                        else:
                            ntevt=r4[0][0]
                            fevt=r4[0][1]
                        if(ntevt != run.LastEvent-run.FirstEvent+1 and ntevt!=None):
                            print "  ntuples/run mismatch %d %d %d " %(ntevt,run.LastEvent-run.FirstEvent+1,run.Run)
                        if(ntevt>0 and ntevt !=None):
                            sql="update runs set fevent=%d, levent=%d where jid=%d" %(fevt,ntevt-1+fevt,run.Run)
                            self.sqlserver.Update(sql)
                            sql="update jobs set realtriggers=%d,timekill=0 where jid=%d" %(ntevt,run.Run)
                            self.sqlserver.Update(sql)
                        for ntuple in cpntuples:
                            cmd="rm "+ntuple
                            rmdir="rm -r "
                            if(ntuple.find('/castor/cern.ch')>=0):
                                cmd="rfrm "+ntuple;
                                rmdir="nsrmdir  "
                            if(ntuple.find('/eosams/')>=0):
                                ntuple = self.eosLink2Real(ntuple)
                                cmd="%s rm %s" %(self.eosselect, ntuple)
                                rmdir="%s rmdir  " %(self.eosselect)
                            rmpath=ntuple.split('/')
                            for i in range (1,len(rmpath)-1):
                                rmdir=rmdir+"/"+rmpath[i]
                            for mn in mvntuples:
                                if(ntuple == mn):
                                    cmd=" "
                                    break
                            self.__cmd(cmd)
                            warn=" validation done "+cmd
                           
                            output.write(warn)
                            print warn
                        
                        if(status =="Completed"):
                            self.updateRunCatalog(run.Run)
                            warn=" Update RunCatalog table "+str(run.Run)
                            for cmd in rmcmd:
                                i=self.__cmd(cmd)
                                if(i):
                                    print "Remote command ",cmd," failed"
                            del rmcmd[:]
                            for cmd in rmbad:
                                i=self.__cmd(cmd)
                                if(i):
                                    print " remove bad ntuples command failed ",cmd
                            del rmbad [:]
        del cpntuples[:]
        del mvntuples[:]
        self.__cmd(rmdir)
        #print "run finished ",run.Run
	self.sqlserver.Commit()
        mutex.release()
        if(self.mt):
              exitmutexes[run.Run].acquire()

    def validatedatarun(self,run):
        if(self.nBadCopiesInRow>30):
            output.write("too many docopy failures")
            print "too many docopy failurs"
            return 
        datamc=run.DataMC
        mutex.acquire()
        # print "run started ",run.Run,run.uid
        odisk=None
        rmdir=""
        rmcmd=[]
        rmbad=[]
        cpntuples=[]
        mvntuples=[]
        timestamp=int(time.time())
        outputpath=None
	copyfailed=0
        interrupt=self.isinterrupt()
        #print "interrupt ",interrupt
        if(interrupt):
            self.run2p=1
        ro=self.sqlserver.Query("select run, status from dataruns where jid="+str(run.uid))
        if(len(ro)==0):
            self.InsertDataRun(run)
        rj=self.sqlserver.Query("select jid from jobs where jid="+str(run.uid))
        if(len(rj)==0):
            self.InsertJob(run.uid)
        ro=self.sqlserver.Query("select run, status from dataruns where jid="+str(run.uid))
        r1=self.sqlserver.Query("select count(path)  from ntuples where jid="+str(run.uid))
        status=ro[0][1]
        status2=""
        if(status== 'Completed' and r1[0][0]==0):
            status="Unchecked"
        if(status != 'Completed' and status != self.dbclient.cr(run.Status)):
            sql="update dataruns set status='%s' where jid=%d" %(self.dbclient.cr(run.Status),run.uid)
            self.sqlserver.Update(sql)
        if(self.dbclient.cr(run.Status) == "Finished" and status != "TimeOut" and status !="Completed"):
            print "Check Run %d Status %s DBStatus %s jid %d" %(run.Run,self.dbclient.cr(run.Status),status,run.uid)
            if(status=='Unchecked'):
                s0= "%d," %(run.uid)
                self.s=self.s+s0
                print self.s
                print "Check Run %d Status %s DBStatus %s jid %d" %(run.Run,self.dbclient.cr(run.Status),status,run.uid)
            output.write("Check Run %d Status %s DBStatus %s" %(run.Run,self.dbclient.cr(run.Status),status))
            sql="select dataruns.status, jobs.content, cites.status from dataruns,jobs,cites where jobs.jid=%d and dataruns.jid=jobs.jid and cites.cid=jobs.cid" %(run.uid)
            r1=self.sqlserver.Query(sql)
            if(len(r1)==0):
                self.sqlserver.Update("update dataruns set status='Failed' where jid="+str(run.uid))
                print "cannot find status, content in Jobs for JID=%d" %(run.uid)
                output.write( "cannot find status, content in Jobs for JID=%d" %(run.uid))
                sql="insert into jobs select jobs_deleted.* from jobs_deleted where jobs_deleted.jid=%d"  %(run.uid);
                self.sqlserver.Update(sql);
                self.sqlserver.Commit();
            else:
                jobstatus=r1[0][0]
                jobcontent=r1[0][1]
                citestatus=r1[0][2]
                # '-GR' runs
                if(jobcontent.find("-GR")>=0  or citestatus == "local"):
                    mips=run.cinfo.Mips
                    events=run.cinfo.EventsProcessed
                    errors=run.cinfo.CriticalErrorsFound
                    cputime=run.cinfo.CPUMipsTimeSpent
                    if(cputime!=cputime):
			cputime=run.cinfo.TimeSpent
                    elapsed="%.2f" %(run.cinfo.TimeSpent)
                    host=run.cinfo.HostName
                    if(mips<=0):
                        print "Mips Problem for %d %s" %(mips, host)
                        mips=1000
                        if(events==0):
                            events=run.LastEvent-run.FirstEvent
                    cputime=cputime/mips*1000
                    cputime="%.2f" %(cputime)
                    if(events==0 and errors==0 and self.dbclient.cr(run.Status) == "Finished"):
                        print "run events 0 "
                        self.sqlserver.Update("update dataruns set status='Unchecked' where jid="+str(run.uid))
                    else:
                        sql="update jobs set events=%d, errors=%d, cputime=%s,elapsed=%s,host='%s',mips=%d,timestamp=%d where jid=%d" %(events,errors,cputime,elapsed,host,int(mips),timestamp,run.uid)
                        self.sqlserver.Update(sql)
                        output.write(sql)
                        self.sqlserver.Update("delete ntuples where jid="+str(run.uid))
                        ntuplelist=[]
                        for ntuple in self.dbclient.dsts:
                            if(self.dbclient.ct(ntuple.Type)!="RootFile" and self.dbclient.ct(ntuple.Type)!="Ntuple"):
                                continue
                 #           print ntuple.Run,run.Run,self.dbclient.cn(ntuple.Status),ntuple.Name
                            if( (self.dbclient.cn(ntuple.Status) == "Success" or  self.dbclient.cn(ntuple.Status) == "Validated") and ntuple.Run == run.Run):
                                ntuplelist.append(ntuple)
                        ntuplelist.sort(lambda x,y: cmp(y.Insert,x.Insert))
                        fevt=-1
                        dat0=0
                        for ntuple in ntuplelist:
                            if(fevt>=0 and ntuple.LastEvent>fevt):
                                ntuple.EventNumber=-1
                                print " problems with %d %s " %(run.Run,ntuple.Name)
                            else:
                                fevt=ntuple.FirstEvent
                                if(str(ntuple.Name).find(':/dat0')>=0):
                                    dat0=1
                        fevent=1
                        levent=0
                        for ntuple in ntuplelist:
                            delim=':'
                            if(str(ntuple.Name).find('///')>0):
                                delim='//'
                            if(ntuple.EventNumber==-1):
                                fpatha=str(ntuple.Name).split(delim)
                                fpath=fpatha[len(fpatha)-1]
                                if(fpath.find('/castor/cern.ch')>=0):
                                    rmbad.append("rfrm "+fpath)
                                else:
                                    rmbad.append("rm "+fpath)
                            else:
                                if(str(ntuple.Name).find(':/dat0')>=0):
                                    ntuple.Status=self.dbclient.iorp.Success
                                self.CheckedDSTs[0]=self.CheckedDSTs[0]+1
                                levent=levent+ntuple.LastEvent-ntuple.FirstEvent+1
                                fpatha=str(ntuple.Name).split(delim)
                                fpath=fpatha[len(fpatha)-1]
                                badevents=ntuple.ErrorNumber
                                events=ntuple.EventNumber
                                status="OK"
                                rcp=1
                                if( not os.path.isfile(fpath) and fpath.find('/castor/cern.ch')<0 and fpath.find('/eosams/')<0):
                                        # try to copy to local dir
                                        parser=str(ntuple.Name).split(':')
                                        if(len(parser)>1):
                                           host=parser[0]
                                           hostok=0
                                           if(host.find('.cern.ch')>=0):
                                               hostok=1
                                           else:
                                               hparser=host.split('.')
                                               if((len(hparser)>1 and hparser[1]=='om') or len(hparser)==1):
                                                   host=hparser[0]+".cern.ch"
                                                   hostok=1
                                               else:
                                                   print "host bizarre ",host
                                           if(hostok):
                                               dir=fpath.split(str(run.Run))
                                               cmd="mkdir -p "+dir[0]
                                               i=self.__cmd(cmd)
                                               cmd="scp -2 %s:%s %s " %(host,fpath,fpath)
                                               if(dir[0].find('/dat0/')>=0):
                                                   mutex.release()
                                                   i=self.__cmd(cmd)
                                                   mutex.acquire()
                                               else:
                                                   i=1
                                                   print "scp not executed for ",dir[0]
                                               if(i):
                                                   print cmd," failed naxx"
                                                   rcp=0
                                               else:
                                                   cmd="ssh -x -2 "+host+" rm "+fpath
                                                   rmcmd.append(cmd)
                                        if(not os.path.isfile(fpath) and fpath.find('/castor/cern.ch')<0 and fpath.find('/eosams/')<0):
                                            print "unable to open file ",fpath

                                retcrc = self.compareCRC(fpath, ntuple.crc)
                                if(retcrc !=1):
                                            print "ValidateRuns-E_Error-CRC status ",retcrc
                                            output.write("ValidateRuns-E_Error-CRC status "+str(retcrc))
                                            self.BadCRC[0]=self.BadCRC[0]+1
                                            self.BadDSTs[0]=self.BadDSTs[0]+1
                                            self.bad=self.bad+1
                                            levent=levent-(ntuple.LastEvent-ntuple.FirstEvent+1)
                                            if(fpath.find('/castor/cern.ch')>=0):
                                                rmbad.append("rfrm "+fpath)
                                            else:
                                                rmbad.append("rm "+fpath)
                                            mvntuples.append(fpath)
                                else:
                                            (ret,i)=self.validateDST(fpath,ntuple.EventNumber,self.dbclient.ct(ntuple.Type),ntuple.LastEvent)
                                            if( i == 0xff00 or (i & 0xff)):
                                                if(ntuple.Status != self.dbclient.iorp.Validated):
                                                    status="Unchecked"
                                                    events=ntuple.EventNumber
                                                    self.unchecked=self.unchecked+1
                                                    copyfailed=1
                                                    break
                                                else:
                                                    self.thrusted=self.thrusted+1
                                            else:
                                                i= i>>8
                                                if(i==134):
                                                    status2="STAGEIN"
                                                if(i/128):
                                                    events=0
                                                    status="Bad"+str(i-128)
                                                    self.bad=self.bad+1
                                                    levent=levent-(ntuple.LastEvent-ntuple.FirstEvent+1)
                                                    if(fpath.find('/castor/cern.ch')>=0):
                                                        rmbad.append("rfrm "+fpath)
                                                    else:
                                                        rmbad.append("rm "+fpath)
                                                else:
                                                    status="OK"
                                                    events=ntuple.EventNumber
                                                    badevents=(i*events/100)
                                                    self.validated=self.validated+1
                                                    if(run.DataMC==0):
                                                        mcpath="/MC"
                                                    else:
                                                        mcpath="/Data"
                                                    (outputpatha,rstatus,odisk,castortime)=self.doCopy(run.uid,fpath,ntuple.crc,ntuple.Version,outputpath,mcpath)
                                                    outputpath=None
                                                    if(outputpatha != None):
                                                        outputpath=outputpatha[:]
                                                        mvntuples.append(outputpath)
                                                    if(rstatus==1):
                                                        self.GoodDSTs[0]=self.GoodDSTs[0]+1
                                                        self.nBadCopiesInRow=0
                                                        if (self.skipcrc):
                                                            crctime = 0
                                                        else:
                                                            crctime = ntuple.Insert
                                                        self.InsertNtuple(run.Run,ntuple.Version,self.dbclient.ct(ntuple.Type),run.uid,ntuple.FirstEvent,ntuple.LastEvent,events,badevents,ntuple.Insert,ntuple.size,status,outputpath,ntuple.crc,crctime,1,castortime,run.DataMC)
                                                        output.write("insert %d %s %s %d" %(run.Run, outputpath, status,int(ntuple.size)))
                                                        self.copied=self.copied+1
                                                        self.gbDST=self.gbDST+ntuple.size
                                                        cpntuples.append(fpath)
                                                    else:
                                                        self.BadDSTs[0]=self.BadDSTs[0]+1
                                                        self.BadDSTCopy[0]=self.BadDSTCopy[0]+1
                                                        output.write("failed to copy or wrong crc for %s" %(fpath))
                                                        copyfailed=1
                                                        self.nBadCopiesInRow=self.nBadCopiesInRow+1
                                                        levent=levent-(ntuple.LastEvent-ntuple.FirstEvent+1)
                                                        self.bad=self.bad+1
                                                        if(outputpath != None):
                                                            cmd="rm "+outputpath
                                                            if(outputpath.find('/castor/cern.ch')>=0):
                                                                cmd="rfrm "+outputpath
                                                            rstat=self.__cmd(cmd)
                                                            output.write("remove bad file "+cmd)

                        status="Failed"
                        if(odisk !=  None):
                            if(fsmutexes.has_key(odisk)):
                                fsmutexes[odisk].release()
                        if(copyfailed==0):
                            warn="Validation done Run %d " %(run.Run)
                            print warn
                            output.write(warn)
                        if(len(cpntuples)>0 and len(mvntuples)>0):
                               status='Completed'
                        elif(len(mvntuples)>0):
                             warn="Validation/copy failed run %d " %(run.Run)
                             print warn
                             output.write(warn)
                             status="Unchecked"
                             for ntuple in mvntuples:
                                 cmd="rm "+ntuple
                                 if(ntuple.find('/castor/cern.ch')>=0):
                                     cmd="rfrm "+ntuple
                                 if(self.deleteuncnt):
                                     self.__cmd(cmd)
                                 print "Validation failed "+cmd
                                 output.write("Validation failed "+cmd)
                                 self.failedcp=self.failedcp+1
                                 self.copied=self.copied-1
                             sql="delete ntuples where jid=%d" %(run.uid)
                             self.sqlserver.Update(sql)
                             
                        elif(len(cpntuples)>0):
                            status="Completed"
                        else:
                            status="Unchecked"
#                        print "status ",status,len(status2),status2
                        if(len(status2)>0):
                            status=status2
                        if(status == "Completed"):
                            self.GoodRuns[0]=self.GoodRuns[0]+1
                        elif (status =="Failed"):
                            self.BadRuns[0]=self.BadRuns[0]+1
                        sql= "update dataruns set status='%s' where jid=%d" %(status,run.uid)
                        self.sqlserver.Update(sql)
                        sql="select sum(ntuples.levent-ntuples.fevent+1),min(ntuples.fevent)  from ntuples,dataruns where ntuples.jid=dataruns.jid and dataruns.jid="+str(run.uid)
                        r4=self.sqlserver.Query(sql)
                        if(len(r4)==0):
                            ntevt=0
                            fevt=0
                        else:
                            ntevt=r4[0][0]
                            fevt=r4[0][1]
                        if(ntevt != run.LastEvent-run.FirstEvent+1 and ntevt!=None):
                            print "  ntuples/run mismatch %d %d %d " %(ntevt,run.LastEvent-run.FirstEvent+1,run.Run)
                        if(ntevt>0 and ntevt !=None):
                            sql="update dataruns set fevent=%d, levent=%d where jid=%d" %(fevt,ntevt-1+fevt,run.uid)
                            self.sqlserver.Update(sql)
                            sql="update jobs set realtriggers=%d,timekill=0 where jid=%d" %(ntevt,run.uid)
                            self.sqlserver.Update(sql)
                        for ntuple in cpntuples:
                            cmd="rm "+ntuple
                            rmdir="rm -r "
                            if(ntuple.find('/castor/cern.ch')>=0):
                                cmd="rfrm "+ntuple;
                                rmdir="nsrmdir  "
                            if(ntuple.find('/eosams/')>=0):
                                ntuple = self.eosLink2Real(ntuple)
                                cmd="%s rm %s" %(self.eosselect, ntuple)
                                rmdir="%s rmdir  " %(self.eosselect)
                            rmpath=ntuple.split('/')
                            for i in range (1,len(rmpath)-1):
                                rmdir=rmdir+"/"+rmpath[i]
                            for mn in mvntuples:
                                if(ntuple == mn):
                                    cmd=" "
                                    break
                            self.__cmd(cmd)
                            warn=" validation done "+cmd
                            output.write(warn)
                            print warn
                        
                        if(status =="Completed"):
                            for cmd in rmcmd:
                                i=self.__cmd(cmd)
                                if(i):
                                    print "Remote command ",cmd," failed"
                            del rmcmd[:]
                            for cmd in rmbad:
                                i=self.__cmd(cmd)
                                if(i):
                                    print " remove bad ntuples command failed ",cmd
                            del rmbad [:]
        del cpntuples[:]
        del mvntuples[:]
        self.__cmd(rmdir)
        #print "run finished ",run.Run,run.uid
	self.sqlserver.Commit()
        try:
            mutex.release()
        except:
            print "-E- Mutex not locked "
        if(self.mt):
              exitmutexes[run.Run].acquire()


    def isinterrupt(self):
        sql="select interrupt from filesprocessing "
        r4=self.sqlserver.QuerySafe(sql)
        interrupt=0
        if(r4!=None and len(r4)>0):
                interrupt=r4[0][0]
        if(interrupt>0):
                print " interrupted because interrupt is ",interrupt
                sql="update filesprocessing set interrupt=0"
                self.sqlserver.Update(sql)                
                self.sqlserver.Commit()  
        return interrupt


    def setProcessingFlag(self, flag, timenow, which):
        if(which==1):
            sql="UPDATE FILESPROCESSING SET FLAGLOCAL=:flag, TIMESTAMPLOCAL=:timenow"
        else:
            sql="UPDATE FILESPROCESSING SET FLAG=:flag, TIMESTAMP=:timenow"

        self.sqlserver.sql(sql, flag=flag, timenow=timenow)
        self.sqlserver.Commit()


    def ServerConnect(self,serverno=0):
        try:
            (ior,iord) = self.getior(serverno)
            self.dbclient = DBServer.DBServer(ior,iord)
            return 1
        except:
            print "Problem to ConnectServer "
            return 0


    def doCopy(self,run,inputfile,crc,version,outputpath,path='/MC'):
#       self.sqlserver.Commit()
       time0=time.time()
       time00=0
       (dbv,gbv,osv)=self.getDSTVersion(version)
       (period,prodstarttime,periodid)=self.getActiveProductionPeriodByVersion(dbv)
       if(period == None or prodstarttime==0):
           sys.exit("Cannot find Active Production Period for DB version "+str(dbv))
       self.doCopyCalls=self.doCopyCalls+1
       junk=inputfile.split('/')
       file=junk[len(junk)-1]
       filesize=1
       if(inputfile.find('/castor/cern.ch')<0 and inputfile.find('/eosams/')<0):
           try:
               filesize=os.stat(inputfile)[ST_SIZE]
           except:
               print "Problem to stat file ",inputfile;
               return None,0,None,0
       odisk=None
       if(filesize>0):
           junk=inputfile.split('/')
           idisk="/"+junk[1]
           #get output disk
           if(outputpath == None):
               stime=100
               odisk=None
               while(stime>60):
                   if(odisk!=None):
                       if(fsmutexes.has_key(odisk)):
                           fsmutexes[odisk].release()
                   try:
                       (outputpatha,gb,odisk,stime)=self.getOutputPath(period,idisk,path)
                   except IOError,e:
                       print e
                       print "Problem to getoutputpath file ",path
                       return None,0,None,0
                   except OSError,e:
                       print e
                       print "Problem to getoutputpath file ",path
                       return None,0,None,0
                   if(odisk==None):
                       print "odisk Problem to getoutputpath file ",path
                       return None,0,None,0
                   print "acquired:  ",outputpatha,gb,odisk,stime
               outputpath=outputpatha[:]
               if(outputpath.find('xyz')>=0 or gb==0):
                   self.setProcessingFlag(0, timenow, 1)
                   sys.exit("doCopy-F-CannotFindAnyDisk Exit")
           elif(outputpath.find('/castor/cern.ch')>=0):
               stime=100
               while(stime>60):
                   if(odisk!=None):
                       if(fsmutexes.has_key(odisk)):
                           fsmutexes[odisk].release()
                   try:
                       (outputpatha,gb,odisk,stime)=self.getOutputPath(period,idisk,path)
                   except IOError,e:
                       print e
                       print "Problem to getoutputpath file ",path
                       return None,0,None,0
                   except OSError,e:
                       print e
                       print "Problem to getoutputpath file ",path
                       return None,0,None,0
                   if(odisk==None):
                       print "odisk Problem to getoutputpath file ",path
                       return None,0,None,0
                   print "acquired:  ",outputpatha,gb,odisk,stime
               outputpath=outputpatha[:]
               if(outputpath.find('xyz')>=0 or gb==0):
                   self.setProcessingFlag(0, timenow, 1)
                   sys.exit("doCopy-F-CannotFindAnyDisk Exit")
           else:
               junk=outputpath.split('/')
               odisk="/"+junk[1]
               outputpath=""
               for i in range (0,len(junk)-3):
                   if(i>0):
                       outputpath=outputpath+"/"
                   outputpath=outputpath+junk[i]
           #find job
           sql="SELECT cites.name,jobname  FROM jobs,cites WHERE jid =%d AND cites.cid=jobs.cid" %(run)
           r1=self.sqlserver.Query(sql)
           self.sqlserver.Commit()
           if(len(r1)>0):
                   cite=r1[0][0]
                   jobname=r1[0][1]
                   dataset="unknown"
                   sql = "SELECT jobs.jid, datasets.name  FROM datasets, jobs WHERE jid =%d AND jobs.did=datasets.did" %(run)
                   r2=self.sqlserver.Query(sql)
                   if(len(r2)>0):
                       dataset=r2[0][1]
                       jobname=re.sub(cite+".",'',jobname,1)
                       jobname=re.sub(str(run)+".",'',jobname,1)
                       jobname=re.sub('.job','',jobname,1)
                       outputpath=outputpath+'/'+dataset+'/'+jobname
                       cmd="mkdir -p "+outputpath
                       if (outputpath.find('/eosams/') == 0):
                           cmd = self.eosselect + " mkdir -p " +  self.eosLink2Real(outputpath)
                       cmdstatus=self.__cmd(cmd)
                       if(cmdstatus==0):
                           outputpath=outputpath+"/"+file
                           cmdstatus=self.copyFile(inputfile,outputpath)
                           time00=time.time()
                           self.doCopyTime=self.doCopyTime+time00-time0
                           if(cmdstatus==0 or cmdstatus==1024):
                               if(cmdstatus==1024):
                                   rstatus=1
                               else:
                                   rstatus = self.compareCRC(outputpath, crc)

                               if(rstatus==1):
                                   (castortime,castorpath)=self.moveCastor(inputfile,outputpath)
                                   if(self.castoronly):
                                       if (castortime>0 and castorpath!=None):
                                           i2=inputfile
                                           inputfile=outputpath
                                           outputpath=castorpath
                                           if(self.castorcopy<0 and outputpath!=inputfile and inputfile.find('/castor')<0 and i2.find('/castor')<0):
                                               cmd="rm "+inputfile
                                               if (outputpath.find('/eosams/') == 0):
                                                   cmd = self.eosselect + " rm " +  self.eosLink2Real(outputpath)
                                               cmdstatus=self.__cmd(cmd)
                                               if(cmdstatus):
                                                   print "cmd failed ",cmdstatus,cmd
                                       # originaly on castor, castoronly specified, and castor copy failed
                                       elif (inputfile.find('/castor')>=0):
                                           print "doCopy-E-ErorrMoveCastor "
                                           return outputpath,0,odisk,0
                                   return outputpath,1,odisk,castortime
                               else:
                                   print "doCopy-E-ErorrCRC ",rstatus
                                   self.BadCRC[self.nCheckedCite]=self.BadCRC[self.nCheckedCite]+1
                                   return outputpath,0,odisk,0
                           else:
                               self.BadDSTCopy[self.nCheckedCite]=self.BadDSTCopy[self.nCheckedCite]+1
                               print "docopy-E-cannot copyfile",cmdstatus,inputfile,outputpath
                               return outputpath,0,odisk,0
                       else:
                           self.BadDSTCopy[self.nCheckedCite]=self.BadDSTCopy[self.nCheckedCite]+1
                           print "docopy-E-cannot ",cmd
                           return outputpath,0,odisk,0
       else:
           print "doCopy-E-cannot stat",inputfile
           self.BadDSTs[self.nCheckedCite]=self.BadDSTs[self.nCheckedCite]+1
           return None,0,odisk,0


    def __doCopyRaw(self, journalFile, inputfile, path='/MC'):
       time0=time.time()
       time00=0
#       (period,prodstarttime,periodid,vrs)=self.getActiveProductionPeriodByName(path)
       (period,prodstarttime,periodid,vrs)=self.getActiveProductionPeriodByName("2011A")
       if(period == None or prodstarttime==0):
           raise CannotFindActiveProductionPeroidRemoteClientException(path)

       self.doCopyCalls += 1
       junk=inputfile.split('/')
       file=junk[len(junk)-1]
       filesize=1
       if(inputfile.find('/castor/cern.ch')<0):
           filesize = os.stat(inputfile)[ST_SIZE]

       outputpath=None
       if(filesize>0):
           #get output disk
           if(outputpath == None):
               stime=100
               odisk=None
               while(stime>60):
                   if(odisk!=None):
                       if(fsmutexes.has_key(odisk)):
                           fsmutexes[odisk].release()

                   (outputpatha, gb, odisk, stime) = self.__getOutputPathRaw(period, path)
                   if(odisk==None):
                       self.logger.info("odisk Problem to __getOutputPathRaw file %s", path)
                       return None,0,None

                   self.logger.info("acquired: %s %s %s %s", outputpatha, gb, odisk, stime)

               outputpath=outputpatha[:]
               if(outputpath.find('xyz')>=0 or gb==0):
                   raise CannotAddAnyDiskRemoteClientException()

           else:
               junk=outputpath.split('/')
               odisk="/"+junk[1]
               outputpath=""
               for i in range (0,len(junk)-3):
                   if(i>0):
                       outputpath=outputpath+"/"

                   outputpath=outputpath+junk[i]

           outputpath = "%s/RawData" % outputpath
           cmd = "mkdir -p %s" % outputpath
           if (outputpath.find('/eosams/') == 0):
               cmd = "%s mkdir -p %s" % (self.eosselect, self.eosLink2Real(outputpath))

           cmdstatus = self.__cmd(cmd)
           if(cmdstatus==0):
               outputpath = "%s/%s" % (outputpath, file)
               cmdstatus = self.copyFile(inputfile, outputpath)
               time00=time.time()
               self.doCopyTime = self.doCopyTime+time00-time0
               if(cmdstatus==0):
                   rstatus = self.compareCRC(outputpath, journalFile.crc, journalFile.crc32)
                   if(rstatus==1):
                       castortime = self.__uploadtoCastor(outputpath)
                       return outputpath, 1, castortime
                   else:
                       self.logger.error("__doCopyRaw-E-ErorrCRC %d", rstatus)
                       self.BadCRC[self.nCheckedCite] += 1
                       return outputpath, 0, 0
               else:
                   self.logger.error("__doCopyRaw-E-cannot copyfile %d %s %s", cmdstatus, inputfile, outputpath)
                   self.BadDSTCopy[self.nCheckedCite] += 1
                   return outputpath,0,0
           else:
               self.logger.error("__doCopyRaw-E-cannot %s", cmd)
               self.BadDSTCopy[self.nCheckedCite] += 1
               return None,0,0
       else:
           self.logger.error("__doCopyRaw-E-cannot stat %s", inputfile)
           self.BadDSTs[self.nCheckedCite] += 1
           return None,0,0
    

    def __uploadtoCastor(self, input):
#
# only one file to uploads
#
        buf=input.split('/')
        output='/castor/cern.ch/ams'
        buf2=input.split(buf[1])
        output=output+buf2[1]
        tmout="/afs/cern.ch/ams/local/bin/timeout --signal 9 10 "
        cmd=tmout+" nsrm "+output
        cmdstatus=self.__cmd(cmd)
        ossize=10000000000
        if (input.find('/eosams/') == 0):
            ossize = self.eosGetSize(input)
            if (ossize < 0):
                print "unable to get size for eos file ",input
                ossize=10000000000
        else:
            try:
                ossize=os.stat(input)[ST_SIZE]
            except:
                print "unable to get size for ",input

        timeout=int(3600.*ossize/10000000000)
        if(timeout<30):
            timeout=30
        if(timeout>1800):
            timeout=1800
        tmout="/afs/cern.ch/ams/local/bin/timeout --signal 9 %d " %(timeout) 
        input_xrootd = input
        if (input.find('/eosams/') == 0):
            input_xrootd = self.eosLink2Xrootd(input)
        cmd=tmout+" /afs/cern.ch/exp/ams/Offline/root/Linux/527.icc64/bin/xrdcp -f -np -v "+input_xrootd+" 'root://castorpublic.cern.ch//"+output+"?svcClass=%s'" %(os.environ['STAGE_SVCCLASS']) 
        cmdstatus=self.__cmd(cmd)
        if(cmdstatus):
            print "Error uploadToCastor via xrdcp",input,output,cmdstatus
            tmout="/afs/cern.ch/ams/local/bin/timeout --signal 9 10 "
            cmd=tmout+" nsrm "+output
            cmdstatus=self.__cmd(cmd)
        else:
            return int(time.time())
        cmd=tmout+" /usr/bin/rfcp "+input+" "+output
        cmdstatus=self.__cmd(cmd)
        if(cmdstatus):
            print "Error uploadToCastor ",input,output,cmdstatus
            tmout="/afs/cern.ch/ams/local/bin/timeout --signal 9 10 "
            cmd=tmout+" nsrm "+output
            cmdstatus1=self.__cmd(cmd)
            if(cmdstatus):
                print "Error uploadToCastor ",input,output,cmdstatus
                return 0                        
            else:
                return int(time.time())
        else:
            return int(time.time())
        

    def getActiveProductionPeriodByName(self,name):
       ret = self.sqlserver.sql("SELECT NAME, BEGIN, DID,VDB FROM PRODUCTIONSET WHERE STATUS='Active' and name like '%' || :name || '%'", name=name)
       if(len(ret)>0):
           return self.trimblanks(ret[0][0]),ret[0][1],ret[0][2],self.trimblanks(ret[0][3])
       else:
          return None,0,0,"invalid"
      
    def getActiveProductionPeriodByVersion(self,dbv):
       ret=self.sqlserver.Query("SELECT NAME, BEGIN, DID  FROM ProductionSet WHERE STATUS='Active' and vdb='"+dbv+"'")
       if(len(ret)>0):
           return self.trimblanks(ret[0][0]),ret[0][1],ret[0][2]
       else:
          return None,0,0
      
    def getProductionSetIdByDatasetId(self, did):
       ret=self.sqlserver.Query("SELECT Productionset.DID FROM ProductionSet WHERE vdb = (SELECT datasets.version FROM datasets WHERE datasets.did = %d) AND productionset.STATUS='Active'" %(did))
       return ret[0][0]
      
    def trimblanks(self,expr):
        return expr.lstrip().rstrip()
       
    def checkDSTVersion(self,dstv):
        rstatus = 1
        junk = dstv.split('/')
        version = int(float(junk[0].replace('v', '')))
        build = int(junk[1].replace('build', ''))

        for p in self.productionPeriods:
            if (p['name'].find('Active') >= 0):
                vvv = int(p['vdb'].replace('v', ''))
                if (vvv == version):
                    bbb = int(p['vgbatch'].replace('build', ''))
                    if (build < bbb):
                        rstatus = 0
                        break
        return rstatus
                              
    def getDSTVersion(self,version):
        junk=version.split('/')
        if(len(junk)>2):
            return self.trimblanks(junk[0]),self.trimblanks(junk[1]),self.trimblanks(junk[2])
        else:
            return self.trimblanks(junk[0]),None,None
       
    def getOutputPath(self,period,idisk,path='/Data'):
        # try eos if self.eos is set
        if (self.eos):
            gb, eosmounted = self.checkEOS()
            if (gb > self.eosreservegb):
                outputpath = "%s%s/%s" %(self.eoslink, path, period)
                print outputpath, gb, self.eoslink, 0
                return outputpath, gb, self.eoslink, 0
            else:
                print "EOS has some problem in quota or not mounted (checkEOS() returned %d, %d), trying normal disk..." %(gb, eosmounted)
        #
        # select disk to be used to store ntuples
        #
        gb=0
        outputpath='xyz'
        ct=360
        outputdisk=None
        if(idisk=='/castor'):
                idisk=None
        if(idisk!=None):
            sql="SELECT disk, path, available, allowed  FROM filesystems WHERE status='Active' and isonline=1 and path='%s' and disk ='%s'ORDER BY priority DESC, available " %(path,idisk)
            ret=self.sqlserver.Query(sql)
            if(len(ret)<=0):
                ct=1
        rescue=self.checkFS(1, ct, path)
        if(idisk!=None):
            sql="SELECT disk, path, available, allowed  FROM filesystems WHERE status='Active' and isonline=1 and path='%s' and disk ='%s'ORDER BY priority DESC, available " %(path,idisk)
            ret=self.sqlserver.Query(sql)
            if(len(ret)<=0):
                idisk=None;

        tme=int(time.time())
        hostname=socket.gethostname()
        if(tme%2 ==0 and idisk==None):
            sql="SELECT disk, path, available, allowed  FROM filesystems WHERE status='Active' and isonline=1 and path='%s' and host like '%%%s%%' ORDER BY priority DESC, available " %(path,hostname)
            ret=self.sqlserver.Query(sql)
            if(len(ret)>0):
                idisk="";
        elif(idisk==None):
            sql = "SELECT disk, path, available, allowed  FROM filesystems WHERE status='Active' and isonline=1 and path='%s' and host like '%%%s%%' ORDER BY priority DESC, available DESC" %(path,hostname)
            ret=self.sqlserver.Query(sql)
            if(len(ret)>0):
                idisk="";

        if(tme%2 ==0 and idisk==None):
            sql="SELECT disk, path, available, allowed  FROM filesystems WHERE status='Active' and isonline=1 and path='%s' ORDER BY priority DESC, available " %(path)
        elif(idisk==None):
            sql = "SELECT disk, path, available, allowed  FROM filesystems WHERE status='Active' and isonline=1 and path='%s' ORDER BY priority DESC, available DESC" %(path)

        ret=self.sqlserver.Query(sql)

        if(len(ret)<=0):
            sql = "SELECT disk, path, totalsize-occupied, allowed  FROM filesystems WHERE status='Full' and isonline=1 and path='%s' ORDER BY totalsize-occupied DESC" %(path)
            ret=self.sqlserver.Query(sql)

        for disk in ret:
            outputdisk=self.trimblanks(disk[0])
            outputpath=self.trimblanks(disk[1])
            if(outputdisk.find('vice')>=0):
                outputpath=outputpath+"/"+period
            else:
                outputpath=outputdisk+outputpath+"/"+period
            self.__cmd("mkdir -p "+outputpath)
            mtime=os.stat(outputpath)[9]
            if(mtime != None):
                if(mtime!=0):
                    gb=disk[2]
                    break
        timew=time.time()
        if(self.needfsmutex):
            if(fsmutexes.has_key(outputdisk)):
                print "acquirng fs mutex for ",outputdisk,sql
                mutex.release()
                fsmutexes[outputdisk].acquire()
                mutex.acquire()
                print "got fs mutex for ",outputdisk
            else:
                fsmutexes[outputdisk]=thread.allocate_lock()
                mutex.release()
                print "acquirng first fs mutex for ",outputdisk,sql
                fsmutexes[outputdisk].acquire()
                mutex.acquire()
                print "got first fs mutex for ",outputdisk
        return outputpath,gb,outputdisk,time.time()-timew
           
    def checkEOS(self, path='/Data'):
        # return gbyte, mounted
        self.eosselect = '/afs/cern.ch/project/eos/installation/ams/bin/eos.select'
        eosmount = self.eosselect + ' -b fuse mount'
        eosumount = self.eosselect + ' -b fuse umount'
        self.eoshome = '/eos/ams'
        self.eoslink = '/eosams'
        self.eosreservegb = 100  #GBype reserved on EOS
        chkeoscmd = "%s quota -m %s%s | grep \"space=%s/ \" | grep \"gid=va\"; test ${PIPESTATUS[0]} -eq 0" %(self.eosselect, self.eoshome, path, self.eoshome)
        pair = self.__cmd_output(chkeoscmd)
        if (pair[0] != 0 or len(pair) < 2):
            print "%s\nreturned %d\n" %(chkeoscmd, pair[0])
            return -4, os.path.exists(self.eoslink)

        out=pair[1]
        try:
            quota = dict(s.split('=') for s in out.split())
        except:
            print "%s\nreturned %d, and output:\n%s" %(chkeoscmd, pair[0], out)
            return -5, os.path.exists(self.eoslink)
#        if (not os.path.exists(self.eoslink)):
#            self.__cmd(eosumount + ' $HOME/eos')
#            self.__cmd(eosmount + ' $HOME/eos')
        if (quota['statusbytes'] == 'ok' and quota['statusfiles'] == 'ok'):
#            print (int(quota['maxlogicalbytes'])-int(quota['usedlogicalbytes']))/1000000000, os.path.exists(self.eoslink)
#            print "Testing write-in"
#            ret = self.__cmd("touch /eosams/test && date >> /eosams/test")
#            if (ret):
#                self.__cmd("ls -l /eosams/")
#                return -2, os.path.exists(self.eoslink)
#            print "Testing readout"
#            pair = self.__cmd_output("cat /eosams/test")
#            if (pair[0]):
#                return -3, os.path.exists(self.eoslink)
#            print pair
#            print "Testing remove"
#            pair = self.__cmd_output("rm -v /eosams/test")
#            if (pair[0]):
#                return -4, os.path.exists(self.eoslink)
#            print pair
            return (int(quota['maxlogicalbytes'])-int(quota['usedlogicalbytes']))/1000000000, os.path.exists(self.eoslink)
        else:
            print "EOS quota problem: ", quota
            self.sendMailMessage('baosong.shan@cern.ch, oleg.demakov@cern.ch', 'EOS quota', quota)
            return -1, os.path.exists(self.eoslink)


    def __getOutputPathRaw(self, period, path='/MC'):
        # try eos if self.eos is set
        if (self.eos):
            gb, eosmounted = self.checkEOS()
            if (gb > self.eosreservegb):
                outputpath = "%s%s/%s" %(self.eoslink, path, period)
                print outputpath, gb, self.eoslink, 0
                return outputpath, gb, self.eoslink, 0
            else:
                print "EOS has some problem in quota or not mounted (checkEOS() returned %d, %d), trying normal disk..." %(gb, eosmounted)

        #
        # select disk to be used to store ntuples
        #
        gb=0
        outputpath='xyz'
        outputdisk=None 
        rescue=self.checkFS(1, 300, path)
        disks, sql = self.__getFSDisks(path)
        for disk in disks:
            outputdisk=self.trimblanks(disk[0])
            outputpath=self.trimblanks(disk[1])
            if(outputdisk.find('vice')>=0):
                outputpath=outputpath+"/"+period
            else:
                outputpath=outputdisk+outputpath+"/"+period

            self.__cmd("mkdir -p %s" % outputpath)
            try:
                mtime=os.stat(outputpath)[9]
            except:
                self.logger.info("Enable to open %s", outputpath)
                continue
           
            if(mtime != None):
                if(mtime!=0):
                    gb=disk[2]
                    break

        timew=time.time()
        self.logger.info("getoutputpath raw %s %s", outputpath, sql)
        return outputpath, gb, outputdisk, time.time()-timew

    def __getFSDisks(self, path):
        hostname = socket.gethostname()
        tme = int(time.time())
        if(tme%2 == 0):
            res = self.sqlserver.sql(
                "SELECT DISK, PATH, AVAILABLE, ALLOWED FROM FILESYSTEMS \
                WHERE STATUS='Active' AND ISONLINE=1 AND PATH=:path AND HOST LIKE '%' || :hostname || '%' ORDER BY PRIORITY DESC, AVAILABLE",
                path = path,
                hostname = hostname
            )
        else:
            res = self.sqlserver.sql(
                "SELECT DISK, PATH, AVAILABLE, ALLOWED FROM FILESYSTEMS \
                WHERE STATUS='Active' AND ISONLINE=1 AND PATH=:path AND HOST LIKE '%' || :hostname || '%' ORDER BY PRIORITY DESC, AVAILABLE DESC",
                path = path,
                hostname = hostname
            )

        if(len(res)<=0):
            if(tme%2 ==0):
                res = self.sqlserver.sql(
                    "SELECT DISK, PATH, AVAILABLE, ALLOWED FROM FILESYSTEMS \
                    WHERE STATUS='Active' AND ISONLINE=1 AND PATH=:path ORDER BY PRIORITY DESC, AVAILABLE",
                    path=path
                )
            else:
                res = self.sqlserver.sql(
                    "SELECT DISK, PATH, AVAILABLE, ALLOWED FROM FILESYSTEMS \
                    WHERE STATUS='Active' AND ISONLINE=1 AND PATH=:path ORDER BY PRIORITY DESC, AVAILABLE DESC",
                    path=path
                )

        if(len(res)<=0):
            res = self.sqlserver.sql(
                "SELECT DISK, PATH, TOTALSIZE-OCCUPIED, ALLOWED FROM FILESYSTEMS \
                WHERE STATUS='Full' AND ISONLINE=1 AND PATH=:path ORDER BY TOTALSIZE-OCCUPIED DESC",
                path=path
            )

        return (res, self.sqlserver.last_sql)


    def mkdirCastor(self, cdir):
        timeout = "/afs/cern.ch/ams/local/bin/timeout --signal 9 90 "
        i = self.__cmd("%s nsmkdir -p %s" %(timeout, cdir))
        if (i):
            i = self.__cmd("%s rfmkdir -p %s" %(timeout, cdir))
        return i


    def moveCastor(self,input,output):
        output_orig = output
        junk=output.split('/')
        castorprefix='/castor/cern.ch/ams'
        cdir=castorprefix
        mutex.release()
        for i in range (2,len(junk)-1):
            cdir += '/'+junk[i]
        cmove = cdir + '/' + junk[len(junk)-1]
        if(input.find(castorprefix)<0):
            if(self.castorcopy):
                if (self.mkdirCastor(cdir)):
                    mutex.acquire()
                    return 0, None
                # try first xrdcp and then rfcp
                if (output.find('/eosams/') == 0):
                    output = self.eosLink2Xrootd(output)
                cmd="/afs/cern.ch/ams/local/bin/timeout --signal 9 1800 /afs/cern.ch/exp/ams/Offline/root/Linux/527.icc64/bin/xrdcp -f -np -v -ODsvcClass=" + os.environ['STAGE_SVCCLASS'] + " "+output+" \"root://castorpublic.cern.ch//"+cmove+"\""
                i=self.__cmd(cmd)
                if(i and output.find('/eosams/') < 0):
                    print cmd
                    print "xrdcp failed, trying rfcp..."
                    cmd="/afs/cern.ch/ams/local/bin/timeout --signal 9 1800 /usr/bin/rfcp "+output_orig+" "+cmove
                    i=self.__cmd(cmd)
                if(i==0):
                    if(self.castorcopy>0):
                        mutex.acquire()
                        return int(time.time()),None
                    else:
                        mutex.acquire()
                        return int(time.time()),cmove
                else:
                    print "moveCastor-E-2 ",cmd
                    castordel="/usr/bin/rfrm "+cmove
                    self.__cmd(castordel)
                    mutex.acquire()
                    return 0,None   
            else:
                print "moveCastor-W-1 ",input,output
                mutex.acquire()
                return 0,None   
        if (self.mkdirCastor(cdir)):
            mutex.acquire()
            return 0, None
        cmd="rfrename "+input+" "+cmove
        i=self.__cmd(cmd)
        if (i!=0):
            print "rfrename failed, retrying with nsrename ..."
            cmd="nsrename -fv "+input+" "+cmove
            i=self.__cmd(cmd)
            
        if(i==0):
            mutex.acquire()
            return int(time.time()),cmove
        else:
            print "moveCastor-W-failed ",cmd
            cmdn="nsls "+cmove
            cmdstatus=self.__cmd(cmdn)
            if(cmdstatus):
                mutex.acquire()
                print "moveCastor-E-failed ",cmdn
                return 0,None
            else:
                mutex.acquire()
                return int(time.time()),cmove
                
                
    def getmoveCastor(self,input,output):
        junk=output.split('/')
        cmove='/castor/cern.ch/ams'
        if(input.find(cmove)<0):
            return None   
        for i in range (2,len(junk)):
            cmove=cmove+'/'+junk[i]
        cmdn="nsls "+cmove
        cmdstatus=self.__cmd(cmdn)
        if(cmdstatus):
            return None
        else:
            return cmove
    
    def eosLink2Xrootd(self, path):
        return path.replace('/eosams/', 'root://eosams.cern.ch//eos/ams/')

    def eosLink2Real(self, path):
        return path.replace('/eosams/', '/eos/ams/')

    def eosGetSize(self, path):
        cmd = "%s ls -l %s" % (self.eosselect, self.eosLink2Real(path))
        pair = self.__cmd_output(cmd)
        eossize = -1
        if (pair[0] != 0 or len(pair) < 2):
            print "%s\nreturned %d\n" %(cmd, pair[0])
            print "eosGetSize-E-of %s" %(path)
        else:
            out=pair[1]
            junk = out.split()
            if (len(junk) > 4):
                eossize = int(junk[4])
        return eossize

    def copyFile(self,input,output):
        output_orig = output
        input_orig = input
        mutex.release()
        if(input == output):
            self.__log_info("acquirung  mutex in copyfile ups")
            mutex.acquire()
            return 0
        time0=time.time()
        cmd="cp -p -d -v "+input+" "+output
        if(input.find('/castor/cern.ch')>=0):
            # check if already been put in place
            cmdn="nsls "+input;
            cmdstatus=self.__cmd(cmdn);
            if(cmdstatus):
                self.__log_error("copyFile-W-Failed %s" % cmdn)
                self.__log_error("copyFile-W-Failed %s" % output)
                cmove=self.getmoveCastor(input,output)
                if(cmove!=None):
                    input=cmove
                else:
                    if(self.castoronly):
                        mutex.acquire()
                        self.__log_info("copyFile-E-FailedCastorOnly %s %s" % (input, output))
                        return 1
            if(self.castoronly):
                mutex.acquire()
                return 1024
            if (output.find('/eosams/') == 0):
                output = self.eosLink2Xrootd(output)
            cmd="/afs/cern.ch/ams/local/bin/timeout --signal 9 1800 /afs/cern.ch/exp/ams/Offline/root/Linux/527.icc64/bin/xrdcp -f -np -v -OSsvcClass=" + os.environ['STAGE_SVCCLASS'] + " \"root://castorpublic.cern.ch//"+input+"\" \""+output+"\""
        else:
            if (cmd.find('/eosams/') >= 0):
                if (output.find('/eosams/') == 0):
                    output_xrd = self.eosLink2Xrootd(output)
                    output = self.eosLink2Real(output)
                else:
                    output_xrd = output
                if (input.find('/eosams/') == 0):
                    input_xrd = self.eosLink2Xrootd(input)
                    input = self.eosLink2Real(input)
                else:
                    input_xrd = input
#                cmd = self.eosselect + " cp -n -S " + input + " " + output
                cmd="/afs/cern.ch/ams/local/bin/timeout --signal 9 1800 /afs/cern.ch/exp/ams/Offline/root/Linux/527.icc64/bin/xrdcp -f -np -v -OSsvcClass=" + os.environ['STAGE_SVCCLASS'] + " \""+input_xrd+"\" \""+output_xrd+"\""
#
#      check if same disk
#
        ifa=input.split("/")
        ofa=output.split("/")
        if(ifa[1]==ofa[1]):
            # Already converted to real eos path
            if (ifa[1] == 'eos'):
                cmd = self.eosselect + " file rename " + input + " " + output
            else:
                cmd="mv "+input_orig+" "+output_orig

        #cmd="cp -pi -d -v "+input+" "+output
        cmdstatus=self.__cmd(cmd)
        if (cmdstatus and cmd.find("xrdcp") >= 0):
            # try first xrdcp and then rfcp
            self.__log_info("xrdcp failed, trying rfcp...")
            cmd="/afs/cern.ch/ams/local/bin/timeout --signal 9 1800 rfcp "+input_orig+" "+output_orig
            cmdstatus=self.__cmd(cmd)

        self.__log_info("acquirung mutex in copyfile %s" % cmd)
        mutex.acquire()
        self.__log_info("got mutex in copyfile %s" % cmd)
        if ( self.v ):
            self.__log_info("docopy-I- %s %d" % (cmd, cmdstatus))

        self.copyCalls=self.copyCalls+1
        self.copyTime=self.copyTime+time.time()-time0
        if(cmdstatus==0 and ifa[1]==ofa[1]):
            cmdstatus=1024

        return cmdstatus


    def updateRunCatalog(self,run):
        timestamp=int(time.time())
        sql = "SELECT runs.status, jobs.content, datasetsdesc.jobdesc FROM Runs, Jobs, datasetsdesc WHERE runs.jid=jobs.jid and jobs.jid="+str(run)+" and datasetsdesc.did=jobs.did and   split(jobs.jobname) like datasetsdesc.jobname"
        r0=self.sqlserver.Query(sql)
        if(len(r0)>0):
            runstatus=r0[0][0]
            jobcontent=r0[0][1]
            if(runstatus == "Completed"):
                sql="delete runcatalog where run=%d" %(run)
                if(self.v):
                    print sql
                self.sqlserver.Update(sql)
                sbuf=jobcontent.split("\n")
                sql0="insert into runcatalog(run"
                sql1="   values(%d" %(run)
                farray=("PART","PMIN","PMAX","TRIGGER","SPECTRUM","SETUP","FOCUS","COSMAX","PLANENO","GEOCUTOFF","COOCUB1","COOCUB2","COOCUB3","COOCUB4","COOCUB5","COOCUB6")
                narray=(1,1,1,0,0,0,0,1,0,0,1,1,1,1,1,1)
                i=0
                for ent in farray:
                    for line in sbuf:
                        if(line.find(ent+"=")>=0):
                            junk=line.split(ent+"=",1)
                            if(junk[0]!="" and junk[1]!=""):
                                if(ent=="TRIGGER"):
                                    sql0=sql0+", TRTYPE"
                                else:
                                    sql0=sql0+ent
                                if(narray[i]==0):
                                    sql1=sql1+", '"+junk[len(junk)-1]+"'"
                                else:
                                    sql1=sql1+", "+junk[len(junk)-1]
                            else:
                                if(ent=="SETUP"):
                                    sql0=sql0+", SETUP"
                                    sql1=sql1+", 'AMS02'"
                                if(ent=="TRIGGER"):
                                    sql0=sql0+", TRTYPE"
                                    sql1=sql1+", 'TriggerLVL1'"
                            break
                    i=i+1
#                for line in sbuf:
#                    if(line.find("generate")>=0):
#                        line=line.replace("C ","",1)
#                        line=line.replace("\\","",1)
#                        line=line.replace("\\","",1)
#                        line=self.trimblanks(line)
#                        sql0=sql0+", jobname"
#                        sql1=sql1+", '"+line+"'"
#                        break
                sql0=sql0+", jobname"
                sql1=sql1+", '"+r0[0][2]+"'"
                sql0=sql0+", TIMESTAMP) "
                sql1=sql1+", "+str(timestamp)+")"
                sql=sql0+sql1
                self.sqlserver.Update(sql)
            else:
                print "UpdateRunCatalog-E-CannitFindRunJobContent with jid=",run
    
              
    def InsertNtuple(self,run,version,type,jid,fevent,levent,events,errors,timestamp,size,status,path,crc,crctime,crcflag,castortime,datamc,fetime=None,letime=None,eostime=0):
        if (path.find('/eosams/') >= 0):
            eostime=time.time()
        if(type=="RawFile" and datamc==0):
#            paths="/Offline/RunsDir/MC/";
#            cmd="ln -sf %s %s" %(path,paths)
#            self.__cmd(cmd)
            paths=self.linkdataset(path,"/Offline/RunsDir",1)
            if (fetime is None or letime is None):
                sql="select fetime,letime from runs where jid=%d" %(jid)
                ret=self.sqlserver.Query(sql)
                fetime=0
                letime=0
                if(len(ret)>0):
                    fetime=ret[0][0]
                    letime=ret[0][1]
            else:
                self.sqlserver.Update("update runs set  fetime=%d, letime=%d where jid=%d" %(fetime,letime,jid))

            sql=" select content,jobname from jobs where jid=%d " %(jid)
            ret=self.sqlserver.Query(sql)
            part="-1"
            ds=""
            nick=""
            ver=""
            if(len(ret)>0):
                content=ret[0][0]
                junk=content.split("PART=")
                if(len(junk)>1):
                    junk1=junk[1].split("\n")
                    if(len(junk1)>0):
                        part=junk1[0]
                junk=content.split("DATASETNAME=")
                if(len(junk)>1):
                    junk1=junk[1].split("\n")
                    if(len(junk1)>0):
                        ds=junk1[0]+" "
                junk=content.split("NICKNAME=")
                if(len(junk)>0):
                    junk1=junk[1].split("\n")
                    if(len(junk1)>0):
                        nick=junk1[0]+" "
                if(content.find("v4.00")>=0):
                    ver="v4.00"
                elif(content.find("v5.00")>=0):
                    ver="v5.00"
                content=ret[0][1]
                junk=content.split(".")
                for i in range(2,len(junk)):
                    ds=ds+junk[i]
                    if(i<len(junk)-1):
                        ds=ds+"."
                if(len(junk)>0):
                    ds=ds+" "+nick+" "+junk[0]
            stype="MC "+ver+" "+ds
            ntsize=float(size)
            sizemb="%.f" %(ntsize)
            sql="delete from datafiles where run=%d and type like '%s'" %(run,stype)
            self.sqlserver.Update(sql)
            if (sys.argv[0].find('parsejf') >=0):
                self.sqlserver.Commit(1)
            sql=" insert into datafiles (RUN,VERSION,TYPE,FEVENT,LEVENT,NEVENTS,NEVENTSERR,TIMESTAMP,SIZEMB,STATUS,PATH,PATHB,CRC,CRCTIME,CASTORTIME,BACKUPTIME,TAG,FETIME,LETIME,PATHS,EOSTIME) values(%d,'%s','%s',%d,%d,%d,%d,%d,%s,'%s','%s',' ',%d,%d,%d,0,%s,%d,%d,'%s',%d)" %(run,version,stype,fevent,levent,events,errors,timestamp,sizemb,status,path,crc,crctime,castortime,part,fetime,letime,paths,eostime)
            print "  insert ntuple before ",sql
            self.sqlserver.Update(sql)
            if (sys.argv[0].find('parsejf') >=0):
                self.sqlserver.Commit(1)
            return
        
        junk=path.split("/")
        filename=self.trimblanks(junk[len(junk)-1])
        sp1=version.split('build')
        sp2=sp1[1].split('/')
        buildno=sp2[0]
        sql="SELECT jid, path FROM ntuples WHERE jid=%d AND path like '%%%s%%'" %(jid,filename)
        ret=self.sqlserver.Query(sql)
        if(len(ret)>0):
            sql="DELETE ntuples WHERE jid=%d AND path like  '%%%s%%'" %(jid,filename)
            print sql,ret[0][1]
        ntsize=float(size)
        if(ntsize>20000):
            print "Too Big size ",ntsize," for ",path
            ossize=os.stat(path)[ST_SIZE]
            ntsize=float(ossize)/1024/1024
            print "size Changed To ",ntsize," for ",path
        sizemb="%.f" %(ntsize)
        sql="delete from ntuples where path='%s'" %(path)
        self.sqlserver.Update(sql);
        sql = "INSERT INTO ntuples (RUN,VERSION,TYPE,JID,FEVENT,LEVENT,NEVENTS,NEVENTSERR,TIMESTAMP,SIZEMB,STATUS,PATH,CRC,CRCTIME,CRCFLAG,CASTORTIME,BUILDNO,DATAMC,EOSTIME) VALUES( %d, '%s','%s',%d,%d,%d,%d,%d,%d,%s,'%s','%s',%d,%d,%d,%d,%s,%d,%d)" %(run,version,type,jid,fevent,levent,events,errors,timestamp,sizemb,status,path,crc,crctime,crcflag,castortime,buildno,datamc,eostime)
	print "  insert ntuple before ",sql
        self.linkdataset(path,"/Offline/DataSetsDir",1)
        self.sqlserver.Update(sql)


    def setenv(self):
        if(os.environ.has_key('AMSDataDirRW')):
            dir=os.environ['AMSDataDirRW']
        else:
            dir="/afs/cern.ch/ams/Offline/AMSDataDir"
            os.environ['AMSDataDirRW']=dir

        if(not os.environ.has_key('STAGE_SVCCLASS')):
            os.environ['STAGE_SVCCLASS']='amscdr'

        self.env['AMSDataDir'] = dir

        key='AMSSoftwareDir'
        ret = self.sqlserver.sql("SELECT MYVALUE FROM ENVIRONMENT WHERE MYKEY=:mykey", mykey=key)
        if( len(ret)>0):
            self.env[key] = "%s/%s" % (self.env['AMSDataDir'], ret[0][0])
        else:
            # Exception?
            print "AMSSoftwareDir Not Defined "
            return 0

        return 1


    def validateDST(self,fname,nevents,ftype,levent):
        time0=time.time()
        ret=1
        vcode=0
        dtype=None
        if(ftype == "Ntuple"):
            dtype=0
        elif(ftype=="RootFile"):
            dtype=1
        if(dtype!=None):
            mutex.release()
            if(fname.find('/castor/cern.ch')>=0):
                prefix="rfio:"
            else:
                prefix=""   
            if(fname.find('/eosams/')==0):
                fname = self.eosLink2Xrootd(fname)
            validatecmd="/exe/linux/fastntrd64.exe %s%s %d %d %d " %(prefix,fname,nevents,dtype,levent)
            validatecmd=self.env['AMSSoftwareDir']+validatecmd
            validatecmd="/afs/cern.ch/ams/local/bin/timeout --signal 9 600 "+validatecmd
            vcode=self.__cmd(validatecmd)
#            mutex.release()
            if (fname.find('/castor/cern.ch')>=0 and vcode != 0 and vcode/256==134):
                self.__cmd("stager_get -M %s " %(fname))
#                time.sleep(5)
                vcode=self.__cmd(validatecmd)
                while (sys.argv[0].find('parsejf') >=0 and sys.argv[0].find('parsejf.data') < 0 and vcode/256==134):
                    time.sleep(30)
                    vcode=self.__cmd(validatecmd)
            print "acquirung  mutex in validate", validatecmd
            mutex.acquire()
            print "got  mutex in validate", validatecmd
            ret=1
        self.fastntCalls=self.fastntCalls+1
        self.fastntTime=self.fastntTime+time.time()-time0
        print "validateDST returning (ret=", ret, ", vcode=", vcode, ")."
        return ret,vcode
    
    def InsertRun(self,Run):
            run=Run.Run
            jid=Run.Run
            fevent=Run.FirstEvent
            levent=Run.LastEvent
            fetime=Run.TFEvent
            letime=Run.TLEvent
            submit=Run.SubmitTime
            status=self.dbclient.cr(Run.Status)
            sql="SELECT run, jid, fevent, levent, status FROM Runs WHERE run=%d" %(run)
            ret=self.sqlserver.Query(sql)
            doinsert=0
            if(len(ret)>0):
                dbrun=ret[0][0]
                dbjid=ret[0][1]
                dbfevent=ret[0][2]
                dblevent=ret[0][3]
                dbstatus=ret[0][4]
                if(dbjid==run and dbfevent==fevent and dblevent==levent and dbstatus==status):
                    print "InsertRun-E-",run,"AlreadyExists"
                else:
                    sql="DELETE runs WHERE run=%d" %(run)
                    self.sqlserver.Update(sql)
                    doinsert=1
            else:
                doinsert=1
            if doinsert==1:
                sql="INSERT INTO Runs VALUES(%d,%d,%d,%d,%d,%d,%d,'%s')" %(run,jid,fevent,levent,fetime,letime,submit,status)
                self.sqlserver.Update(sql)
                if(self.v):
                    print sql
                if(status=="Completed"):
                    if(self.v):
                        print "Update RunCatalog ",run

    def insertRun(self,run,jid,fevent,levent,fetime,letime,submit,status):
            sql="SELECT run, jid, fevent, levent, status FROM Runs WHERE run=%d" %(run)
            ret=self.sqlserver.Query(sql)
            doinsert=0
            if(len(ret)>0):
                dbrun=ret[0][0]
                dbjid=ret[0][1]
                dbfevent=ret[0][2]
                dblevent=ret[0][3]
                dbstatus=ret[0][4]
                if(dbjid==run and dbfevent==fevent and dblevent==levent and dbstatus==status):
                    print "InsertRun-E-",run,"AlreadyExists"
                else:
                    sql="DELETE runs WHERE run=%d" %(run)
                    self.sqlserver.Update(sql)
                    self.sqlserver.Commit(1)
                    doinsert=1
            else:
                doinsert=1
            if doinsert==1:
                sql="INSERT INTO Runs VALUES(%d,%d,%d,%d,%d,%d,%d,'%s')" %(run,jid,fevent,levent,fetime,letime,submit,status)
                self.sqlserver.Update(sql)
                self.sqlserver.Commit(1)
                if(self.v):
                    print sql
                if(status=="Completed"):
                    if(self.v):
                        print "Update RunCatalog ",run


    def InsertJob(self,jid):
        sql="insert into jobs select jobs_deleted.* from jobs_deleted where jid=%d  " %(jid)
        self.sqlserver.Update(sql)
        sql="delete from jobs_deleted where jid=%d "  %(jid)
        self.sqlserver.Update(sql)
        print "Job Restored ",jid
    def InsertDataRun(self,Run):
            run=Run.Run
            jid=Run.uid;
            fevent=Run.FirstEvent
            levent=Run.LastEvent
            fetime=Run.TFEvent
            letime=Run.TLEvent
            submit=Run.SubmitTime
            status=self.dbclient.cr(Run.Status)
            sql="SELECT run, jid, fevent, levent, status FROM dataRuns WHERE run=%d" %(run)
            ret=self.sqlserver.Query(sql)
            doinsert=0
            if(len(ret)>0):
                dbrun=ret[0][0]
                dbjid=ret[0][1]
                dbfevent=ret[0][2]
                dblevent=ret[0][3]
                dbstatus=ret[0][4]
                if(dbjid==jid and dbrun==run and dbfevent==fevent and dblevent==levent and dbstatus==status):
                    print "InsertRun-E-",run,"AlreadyExists"
                else:
                    sql="DELETE dataruns WHERE jid=%d" %(jid)
                    self.sqlserver.Update(sql)
                    doinsert=1
            else:
                doinsert=1
            if doinsert==1:
                sql="INSERT INTO dataRuns VALUES(%d,%d,%d,%d,%d,'%s',%d,%d)" %(run,fevent,levent,fetime,letime,status,jid,submit)
                self.sqlserver.Update(sql)
                if(self.v):
                    print sql

    def insertDataRun(self,run,jid,fevent,levent,fetime,letime,submit,status):
            sql="SELECT run, jid, fevent, levent, status FROM dataRuns WHERE jid = %d" %(jid)
            ret=self.sqlserver.Query(sql)
            doinsert=0
            if(len(ret)>0):
                dbrun=ret[0][0]
                dbjid=ret[0][1]
                dbfevent=ret[0][2]
                dblevent=ret[0][3]
                dbstatus=ret[0][4]
                if(dbjid==jid and dbrun==run and dbfevent==fevent and dblevent==levent and dbstatus==status):
                    print "InsertRun-E-",run,"AlreadyExists"
                else:
                    sql="DELETE dataruns WHERE jid=%d" %(jid)
                    self.sqlserver.Update(sql)
                    self.sqlserver.Commit(1)
                    doinsert=1
            else:
                doinsert=1
            if doinsert==1:
                sql="INSERT INTO dataRuns VALUES(%d,%d,%d,%d,%d,'%s',%d,%d)" %(run,fevent,levent,fetime,letime,status,jid,submit)
                self.sqlserver.Update(sql)
                self.sqlserver.Commit(1)
                if(self.v):
                    print sql

      
    def compareCRC(self, filename, crc, crc32=0):
        if (self.skipcrc):
            return 1

        self.crcCalls=self.crcCalls+1
        time0=time.time()
        if (filename.find('/eosams/') == 0):
            filename = self.eosLink2Xrootd(filename)

        if crc32 != 0:
            crc = crc32

        mutex.release()
        crccmd = "%s/exe/linux/crc %s %d" % (self.env['AMSSoftwareDir'], filename, crc)
        if(filename.find('/castor/cern.ch')>=0):
            rstatus=1
        else:
            rstatus=self.__cmd(crccmd)
            rstatus=rstatus>>8

        print "acquirung  mutex in calccrc", filename
        mutex.acquire()
        print "got  mutex in calccrc", filename
        if(self.v):
            print crccmd,rstatus

        self.crcTime=self.crcTime+time.time()-time0
        if(crc==0 and self.crczero==1):
            return 1
        else:
            return rstatus


    def parseJournalFiles(self,d,i,v,h,s,m,mt=1,co=0,eos=0,force=0,skipcrc=0):
        self.castoronly=co
        self.castorcopy=0
        self.eos=eos
        self.force=force
        self.crczero=0
        self.skipcrc=skipcrc
        firstjobname=0
        lastjobname=0
        HelpTxt = """
        parseJournalFiles check journal file directory for all cites
        -h      - print help
        -i      - prompt before files removal
        -v      - verbose mode
        -s      -  Only one journal file then quit
        -m      -  Mail owner if failed
        ./pj.py
        """
        if (d == 1):
            datamc = 1
        else:
            datamc = 0
        if(i==1):
            self.rm="rm -i "
        else: self.rm="rm -f "
        if(s==1):
            self.oneonly=1
        else: self.oneonly=0 
        if(h==1):
            print HelpText
            return 1
        if(v==1):
            self.v=1
        else: self.v=0 
        if(m==1):
            self.m=1
        else: self.m=0 
        if (mt):
            self.mt = mt
        else:
            self.mt = 0
        self.checkEOS()
        whoami=pwd.getpwuid(os.getuid())[0]
        if not (whoami == None or whoami =='ams' ):
            print "parseJournalFiles -ERROR- script cannot be run from account : ",whoami 
            return 0
        timenow=int(time.time())
        self.valStTime=timenow
        self.needfsmutex=0
        sql = "SELECT flag, timestamp from FilesProcessing"
        ret = self.sqlserver.Query(sql)
        if(ret[0][0]==1 and timenow-ret[0][1]<100000 and self.force == 0):
            print "ParseJournalFiles-E-ProcessingFlagSet on ",ret[0][1]," exiting"
            return 0
        else: 
            self.setProcessingFlag(1, timenow, 0)

        ret=self.sqlserver.Query("SELECT min(begin) FROM productionset WHERE STATUS='Active' ORDER BY begin")
        if (len(ret)==0):
            print "ValidateRuns=E-CannotFindActiveProductionSet"
            self.setProcessingFlag(0, timenow, 0)
            return 0
        else:
            periodStartTime = ret[0][0]
            print "periodStartTime is %d" %(periodStartTime)

        self.getProductionPeriods(0)
        firstjobtime=ret[0][0]-24*60*60
        lastjobtime=int(time.time())+24*60*60
        ret=self.sqlserver.Query("select max(cid) from cites")
        self.nCheckedCite=-1
        self.nActiveCites=0
        self.gbDST=0
        self.doCopyTime=0
        self.doCopyCalls=0
        self.fastntTime=0
        self.fastntCalls=0
        self.crcTime=0
        self.crcCalls=0
        self.copyTime=0
        self.copyCalls=0
        self.nBadCopiesInRow=0
        self.JF = []
        self.JouLastCheck = []
        self.JouDirPath = []
        self.CheckedRuns = []
        self.FailedRuns = []
        self.GoodRuns = []
        self.BadRuns = []
        self.CheckedDSTs = []
        self.GoodDSTs = []
        self.BadDSTs = []
        self.BadDSTCopy = []
        self.BadCRC = []
        self.BadRunID = []
        self.gbDST = []
        self.mail = None
        for i in range(0,ret[0][0]):
            self.JF.append(0)
            self.JouLastCheck.append(0)
            self.JouDirPath.append("xyz")
            self.CheckedRuns.append(0)
            self.FailedRuns.append(0)
            self.GoodRuns.append(0)
            self.BadRuns.append(0)
            self.CheckedDSTs.append(0)
            self.GoodDSTs.append(0)
            self.BadDSTs.append(0)
            self.BadDSTCopy.append(0)
            self.BadCRC.append(0)
            self.BadRunID.append(0)
            self.gbDST.append(0)
        minJobID=0
        ret=self.sqlserver.Query("SELECT dirpath,journals.timelast,name,journals.cid FROM journals,cites WHERE journals.cid=cites.cid")
        cid=-1
        global mutex
#        mutex=thread.allocate_lock()
        mutex = threading.Lock()
        if(len(ret)>0):
            for jou in ret:
                self.nCheckedCite=self.nCheckedCite+1
                timenow=int(time.time())
                dir=self.trimblanks(jou[0])
                if (datamc == 1):
                    dir = dir.replace("/MC/", "/Data/")
                cite=self.trimblanks(jou[2])
                timestamp=jou[1]
                lastcheck=self.EpochToDDMMYYHHMMSS(timestamp)
                if(jou[3]==1):
                    continue
                if(cid != jou[3]):
                    cid=jou[3]
                    sql="SELECT  min(jid) from jobs where cid=%d and timestamp>=%d"  %(cid, periodStartTime)
                    r0=self.sqlserver.Query(sql)
                    if(len(r0)==0):
                        sql="SELECT  min(jid) from jobs where cid=$cid and timestamp<%d"  %(periodStartTime)
                        r1=self.sqlserver.Query(sql)
                        if(len(r1)>0):
                            minJobID=r1[0][0]
                    else:
                        minJobID=r0[0][0]
                    self.JouDirPath[self.nCheckedCite]=cite
                    self.JouLastCheck[self.nCheckedCite]=lastcheck
                    title="Cite %s, Directory %s Last Check %s " %(cite,dir,lastcheck)
                    print title
                    newfile="./"
                    lastfile="./"
                    writelast=0
                    logdir=dir+"/log"
                    joudir=dir+"/jou"
                    ntdir=dir+"/nt"
                    try:
                        allfiles=os.listdir(joudir)
                    except:
                        print "Enable to open ",joudir
                        continue
                    threads = []
                    self.suc = {}
                    for file in allfiles:
                        if(file.find(".journal")<0 or re.match("^\.", file)):
                            continue
                        junk=file.split("journal.")
                        if(len(junk)>1):
                            if(junk[1]=="0" or junk[1]=="1" or junk[1]=="2"):
                                continue
                            if (not re.match("\.journal$", file)):
                                continue
#                            elif (junk[1] == "work"):
#                                ret = self.__cmd("mv %s/%s %s/%s" %(joudir,file,joudir,file.replace(".work","")))
#                                if (ret == 0):
#                                    file = file.replace(".work","")
#                                else:
#                                    continue
                        try:
                            fid=float(file.replace(".journal",""))
                        except:
                            print "%s is not a regular journal file name, skipping..." %(file)
                            continue
                        jid=int(fid)
                        if(int(fid)<minJobID):
                            print " invalid jid ",fid
                            self.BadRunID[self.nCheckedCite] += 1
                            continue
                        newfile=joudir+"/"+file
                        writetime=os.stat(newfile)[ST_MTIME]
                        if(writetime > writelast):
                            writelast=writetime
                            lastfile=newfile
                        self.suc[jid] = 0
                        if(writetime> timestamp-24*60*60*30):
                            if(self.v):
                                print "parsejournal file ",writetime,timestamp,newfile
                            if (self.mt):
                                while threading.activeCount() > 9:
#                                    print "waiting..."
                                    time.sleep(1)
                                try:
                                    parseThread = self.threadParseJournalFile(self,firstjobtime,lastjobtime,logdir,newfile,ntdir,cid,datamc,None)
                                    parseThread.start()
                                    threads.append(parseThread)
#                                    thread.startnew(self.parseJournalFile, (firstjobtime,lastjobtime,logdir,newfile,ntdir,cid,datamc,None,))
                                except:
                                    print "EXCEPTION in starting new thread";
                            else:
                                (self.suc[jid],logfile)=self.parseJournalFile(firstjobtime,lastjobtime,logdir,newfile,ntdir,cid,datamc,None)
                                print "parseJournalFile returned (%d, %s)" %(self.suc[jid], logfile)
                        else:
                            continue
                        if(self.suc[jid]>0 and self.mail):
                            sql="select jid,mid from jobs where jid=%d" %(self.suc[jid])
                            ret=self.sqlserver.Query(sql)
                            sql = "SELECT mails.name, mails.address, cites.name FROM Mails, Cites  WHERE mails.mid=%d and mails.cid=cites.cid" %(ret[0][1])
                            r4=self.sqlserver.Query(sql)
                            address='vitali.choutko@cern.ch'
                            if(len(r4)>0):
                                owner=r4[0][0]
                                address=r4[0][1]
                                cite=r4[0][2]
                                sub="Validation Failed for Job:  %d  %s" %(self.suc[jid],r4[0][1])
                                mes=" "
                                if(logfile != None):
                                    input=open(logfile,'r')
                                    mes=input.read()
                                    input.close()
                                self.sendMailMessage(address,sub,mes)
                        
                        self.JF[self.nCheckedCite]=self.JF[self.nCheckedCite]+1
                        if(self.oneonly):
                            break
                    for t in threads:
                        t.join()
                    print "All threads exited."
                    
                    
                    if(cid!=None):
                        if(writelast>0):
                            sql="UPDATE journals SET timestamp=%d,timelast=%d,lastfile = '%s' WHERE cid=%d" %(timenow,writelast,lastfile,cid)
                            self.nActiveCites=self.nActiveCites+1
                        else:
                            sql="update journals set timestamp=%d where cid=%d" %(timenow,cid)
                        self.sqlserver.Update(sql)
        else:
            print "parseHournalFile-W-JournalTableIsEmpty"
        self.printParserStat()
        (good,bad,message)=self.updateFilesProcessing()
        if(self.mail and good+bad>0):
            rmail=self.sqlserver.Query("select address from mails where rserver=1")
            if(len(rmail)>0):
                address=rnail[0][0]
                sub="parseHournalFile:  Good: %d  Bad: %d " %(good,bad)
                self.sendMailMessage(address,sub,message)

        self.setProcessingFlag(0, timenow, 0)
        return 1


    def updateFilesProcessing(self):
        timenow=int(time.time())
        nCites=self.nCheckedCite
        nJF=0
        nBD=0
        nGD=0
        nBR=0
        nFR=0
        nGR=0
        message=""
        for i in range(0,nCites):
            cid=i+2
            sql="select name from cites where cid=%d" %(cid)
            ret=self.sqlserver.Query(sql)
            if self.FailedRuns[i]+self.BadRuns[i]+self.GoodRuns[i]>0:
                message=message+" CiteNo: %s JournalFiles: %d GoodRuns:  %d BadRuns %d \n"  %(ret[0][0], self.JF[i],self.GoodRuns[i],self.BadRuns[i]+self.FailedRuns[i])
            nJF=nJF+self.JF[i]
            nGR=nGR+self.GoodRuns[i]
            nBR=nBR+self.BadRuns[i]
            nFR=nFR+self.FailedRuns[i]+self.BadRuns[i]
            nGD=nGD+self.GoodDSTs[i]
            nBD=nBD+self.BadDSTs[i]+self.BadCRC[i]
        sql="update filesprocessing set cites=%d, Active=%d,jou=%d,good=%d,failed=%d,gooddsts=%d,baddsts=%d,flag=0,timestamp=%d "  %(nCites,self.nActiveCites,nJF,nGR,nFR,nGD,nBD,timenow)
        self.sqlserver.Update(sql)
        return nGR,nFR,message
    def printParserStat(self):
        timenow=int(time.time())
        tR=0
        tD=0
        tBR=0
        tBD=0
        tGB=0
        vdir=self.getValidationDir()
        vdir=self.getValidationDir()
        vlog=vdir+"parseRunSummary.log."+str(timenow)
        try:
            output=open(vlog,'w')
            print "ParseJournalFileS-I-Open ",vlog
        except IOError,e:
            print e
            return 0
        firstline = "------------- parseJournalFiles ------------- "
        lastline  = "-------------     Thats It          ------------- "
        print firstline
        output.write(firstline + "\n")
        stime   = time.asctime(time.localtime(self.valStTime))
        ltime=time.asctime(time.localtime(timenow))
        hours   = (timenow - self.valStTime)/60/60;
        t0="Start Time : "+stime
        t1="End Time : "+ltime
        print t0,t1
        output.write(t0 + "\n")
        output.write(t1 + "\n")
        ctt = "Cites      : $nCheckedCite , active : $nActiveCites \n";
        ctt = "Cites      : %d , active : %d " %(self.nCheckedCite,self.nActiveCites)
        print ctt
        output.write(ctt + "\n")
        for i  in range(0,self.nCheckedCite+1):
            s1="Cite : "+self.JouDirPath[i]
            s2="Latest Journal : "+str(self.JouLastCheck[i])
            s3="New Files : "+str(self.JF[i])
            cj="%-20.15s %-20.40s %-50.30s" % (s1,s2,s3)
            print cj
            output.write(cj + "\n")
            if self.JF[i] > 0:
                l0 = "   Runs (Checked, Good, Bad, Failed) : %d %d %d %d " %( self.CheckedRuns[i], self.GoodRuns[i],  self.BadRuns[i], self.FailedRuns[i])
                l1 = "   DSTs (Checked, Good, Bad, CRC, CopyFail) :  %d %d %d %d %d %d" %(self.CheckedDSTs[i],  self.GoodDSTs[i], self.BadDSTs[i], self.BadCRC[i], self.BadDSTCopy[i], self.BadRunID[i])
                print l0
                print l1
                output.write(l0 + "\n")
                output.write(l1 + "\n")
            tR    = tR    + self.CheckedRuns[i]
            tBR += self.BadRuns[i] + self.FailedRuns[i]
            tD    = tD    + self.CheckedDSTs[i]
            tBD += self.BadDSTs[i] + self.BadCRC[i] + self.BadDSTCopy[i]
            tGB      = tGB      + self.gbDST[i]
        tGB = float(tGB)/1000.
        chGB = "%3.1f" %(tGB)
        summ =  "Total : Runs %d (%d), DSTs %d (%d), GB %s" %(tR,tBR,tD,tBD,chGB)
        print summ
        output.write(summ + "\n")
        ch0   = "Total Time %3.1f hours" %(hours)
        mbits = 0;
        if(self.doCopyTime > 0) :
            mbits = tGB*8/self.doCopyTime
        ch1 = " doCopy (calls, time, Mbit/s) : %5d %3.1fh %3.1f [cp file :%5d %3.1fh]; " %(self.doCopyCalls, float(self.doCopyTime)/60/60, mbits, self.copyCalls, float(self.copyTime)/60/60)
        ch2 = " CRC (calls,time) : %5d, %3.1fh ; Validate (calls,time) : %5d, %3.1fh]; "%(self.crcCalls, float(self.crcTime)/60/60,self.fastntCalls, self.fastntTime/60/60 )
        print ch0
        print ch1
        print ch2
        print lastline
        output.write(ch0 + "\n")
        output.write(ch1 + "\n")
        output.write(ch2 + "\n")
        output.write(lastline + "\n")
        output.close()

    def EpochToDDMMYYHHMMSS(self,tm):
        return time.strftime("%d %b %Y %H:%M:%S", time.localtime(tm))

    class threadParseJournalFile(threading.Thread):
        def __init__(self,caller, firstjobtime,lastjobtime,logdir,inputfile,dirpath,cid,datamc,outputpath):
            threading.Thread.__init__(self)
            self.caller = caller
            self.firstjobtime = firstjobtime
            self.lastjobtime = lastjobtime
            self.logdir = logdir
            self.inputfile = inputfile
            self.dirpath = dirpath
            self.cid = cid
            self.datamc = datamc
            self.outputpath = outputpath
        def run(self):
            RemoteClient.parseJournalFile(self.caller, self.firstjobtime, self.lastjobtime, self.logdir, self.inputfile, self.dirpath, self.cid, self.datamc, self.outputpath)

    def parseJournalFile(self,firstjobtime,lastjobtime,logdir,inputfile,dirpath,cid,datamc,outputpath):
#        outputpath=None
        host="Unknown"
        tevents=0
        terrors=0
        jobid=-1
        fevent=1
        levent=0
        patternsmatched=0
        startingjob=[]
        startingrun=[]
        opendst=[]
        closedst=[]
        runfinished=[]
        runincomplete=[]
        timestamp=int(time.time())
        lastjobid=0
        startingjobR=0
        startingrunR=0
        oprndstR=0
        runfinishedR=0
        status=None
        jobmips=-1
        self.failedcp=0
        self.thrusted=0
        self.copied=0
        self.bad=0
        self.unchecked=0
        self.validated=0
        leti=0
        feti=2000000000
        run=0
        mutex.acquire()
        inputwork = inputfile + ".work"
        inputorig = inputfile
        ret = self.__cmd("mv %s %s" %(inputfile, inputwork))
        if (ret == 0):
            inputfile = inputwork
        else:
            print " Unable to execute mv %s %s" %(inputfile, inputwork)
            mutex.release()
            return 0, ""
        try:
            input=open(inputfile,'r')
        except IOError,e:
            print e
            mutex.release()
            return 0, ""
        buf=input.read()
        input.close()
        sp1=buf.split(', UID ')
        if(len(sp1)>1):
            sp2=sp1[1].split(' , ')
            if(inputfile.find(sp2[0])<0):
                print "Fatal - Run %s does not match file  %s"  %(sp2[0],inputfile)
                cmd="mv %s %s.o"  %(inputfile,inputfile)
                self.__cmd(cmd)
                mutex.release()
                return 0, ""
            else:
                try:
                    files=os.listdir(dirpath)
                except  IOError,e:
                    print e
                    self.__cmd("mv %s %s" %(inputwork, inputfile))
                    mutex.release()
                    return 0, ""
                tnow=int(time.time())
                unstaged = 0
                for file in files:
                    if(file.find(sp2[0])>=0):
                        try:
                            mtim=os.stat(dirpath+"/"+file)[ST_MTIME]
                        except OSError,e:
                            mtim = 0
                        if(tnow-mtim<300):
                            print "run %s not yet completed.  Postponed " %(sp2[0])
                            self.__cmd("mv %s %s" %(inputwork, inputfile))
                            mutex.release()
                            return 0, ""
                        realpath=os.path.realpath(dirpath+"/"+file)
                        if (realpath.find('/castor/') >=0 ):
                            pair = self.__cmd_output("/afs/cern.ch/ams/local/bin/timeout --signal 9 600 stager_qry -M %s -S \*" % (realpath))
                            if (pair[0] != 0 or len(pair) < 2 or pair[1].find('STAGED') < 0 and pair[1].find('CANBEMIGR') < 0):
                                self.__cmd("stager_get -M %s" %(realpath))
                                unstaged += 1
                                print "%s has not been staged." %(realpath)
                            else:
                                print pair[1]
                if (unstaged > 0):
                    print "run %s not yet staged.  Postponed " %(sp2[0])
                    self.__cmd("mv %s %s" %(inputwork, inputfile))
                    mutex.release()
                    return 0, ""
        blocks=buf.split("-I-TimeStamp")
        cpntuples=[]
        mvntuples=[]
        runupdate=" update runs set "
        copyfailed=0
        timenow=int(time.time())
        jj=inputfile.split('/')
        joufile=jj[len(jj)-1]
        copylog=logdir+"/copyValidateCRC."+joufile+".log"
        try:
            output=open(copylog,'w')
        except IOError,e:
            print e
            mutex.release()
            return 0, ""
        run_incomplete = 0
        run_finished = 0
        for block in blocks:
            if (block.find('RunIncomplete') >=0 ):
                run_incomplete = 1
            if (block.find('RunFinished') >= 0):
                run_finished = 1
        for block in blocks:
            if(block.find('RunValidated')>=0):
               junk=block.split(',')
               for i in range (0,len(junk)):
                   junk[i]=self.trimblanks(junk[i])
                   if(junk[i].find('Path')>=0):
                       path=junk[i].split()
                       dirpath=self.trimblanks(path[1])
                       break
        validated_ntuples = 0
        for block in blocks:
            junk=block.split(",")
            for i in range (0,len(junk)):
                junk[i]=self.trimblanks(junk[i])
            jj=block.split()
            if (len(jj) == 0):
                continue
            try:
                utime=int(jj[0])
            except:
                continue
            if(utime < firstjobtime or utime > lastjobtime):
                self.BadRuns[self.nCheckedCite] += 1;
                output.write("*********** wrong timestamp : %d (%d,%d)\n" %(utime, firstjobtime, lastjobtime))
#                self.__cmd("mv %s %s.0" %(inputfile, inputorig))
#                output.write(lastline + "\n")
#                output.close
#                mutex.release()
#                return (jobid, copylog)
            if(block.find('RunIncomplete')>=0):
                if(startingrunR ==1):
                    output.write("RunIncomplete : ntuples validated: %d Continue \n" %(validated_ntuples))
                    output.write(block + "\n")
                    patternsmatched=0
                    RunIncompletePatterns=("RunIncomplete","Host","Run","EventsProcessed","LastEvent","Errors","CPU","Elapsed","CPU/Event","Status")
                    runincomplete = [None] * len(RunIncompletePatterns)
                    for i in range (0,len(junk)):
                        jj=junk[i].split()
                        if(len(jj)>1):
                            found=0
                            j=0
                            while j<len(RunIncompletePatterns) and found==0 :
                                if (jj[0] == RunIncompletePatterns[j]):
                                    runincomplete[j]=self.trimblanks(jj[1])
                                    patternsmatched=patternsmatched+1
                                    found=1
                                j=j+1
                    if(patternsmatched == len(RunIncompletePatterns)):
                       runincomplete[0]="RunIncomplete"
                       run=int(runincomplete[2])
                       runfinishedR=1
                       sql="SELECT run FROM runs WHERE run = %d AND levent=%s" %(run,runincomplete[4])
                       ret=self.sqlserver.Query(sql)
                       
                       if(len(ret)==0):
                           cputime="%.0f" %(float(runincomplete[6]))
                           elapsed="%.0f" %(float(runincomplete[7]))
                           host=runincomplete[1]
                           sql="UPDATE Jobs SET EVENTS=%s, ERRORS=%s,CPUTIME=%s, ELAPSED=%s,HOST='%s', TIMESTAMP = %d WHERE JID = (SELECT Runs.jid FROM Runs WHERE Runs.jid = %d)" %(runincomplete[3], runincomplete[5],cputime, elapsed,host,timestamp, run)
                           output.write(sql + "\n")
                           self.sqlserver.Update(sql)
                           sql="update runs set levent=%s where run=%d " %(runincomplete[4],run)
                           output.write(sql + "\n")
                           self.sqlserver.Update(sql)
                           self.sqlserver.Commit(1)
                    else:
                        output.write("parseJournalfile-W-RunIncomplete - cannot find all patterns \n")
                else:
                    print "parseJournalFile -W- RunIncomplete CHECK File ",joufile
                    runfinishedR=1
                    jj=joufule.split('.journal')
                    jobid=jj[0]
                    mailto="vitali.choutko@cern.ch"
                    subject = "RunIncomplet : "+inputfile
                    text    = " Program found RunIncomplete status in "+inputfile+" file. \n Please provide job log file to V.Choutko"
                    sql = "SELECT Mails.address FROM Jobs, Mails WHERE Jobs.jid=%d AND Mails.mid = Jobs.mid" %(jobid)
                    ret = selfsqlserverQuery(sql)
                    if(len(ret)>0):
                        mailto=mailto+", "+ret[0][0]
                    self.sendMailMessage(mailto,subject,text)
                    break
            elif(block.find('StartingJob')>=0):
                pattarnsmatched=0
                StartingJobPatterns = ("StartingJob", "HostName","UID","PID","Type","ExitStatus","StatusType")
                startingjob = [None] * len(StartingJobPatterns)
                for i in range (0,len(junk)):
                    jj=junk[i].split()
                    if(len(jj)>1):
                        found=0
                        j=0
                        while (j<len(StartingJobPatterns) and found==0):
                            if(jj[0] ==  StartingJobPatterns[j]):
                                startingjob[j]=self.trimblanks(jj[1])
                                patternsmatched=patternsmatched+1
                                found=1
                            j=j+1
                if(len(startingjob)>=3):
                    jobid=jobstartingjob[2]
                    if(self.findJob(jobid,buf,dirpath,cid) !=jobid):
                        output.write("fatail - cannot find JobInfo for %d\n" %(jobid))
                        self.BadRuns[self.nCheckedCite] += 1
                        system("mv %s %s.0" %(inputfile,inputorig))
                        mutex.release()
                        return 0, copylog
                else:
                        output.write("fatail - cannot find JobInfo for in %s\n" %(inputfile))
                        system("mv %s %s.0" %(inputfile,inputorig))
                        mutex.release()
                        return 0, copylog
                if(patternsmatched == len(StartingJobPatterns) or patternsmatched == len(StartingJobPatterns)-1):
                    startingjob[0]="StartingJob"
                    startingjobR=1
                    lastjobid=startingjob[2]
                else:
                    output.write("parseJournalFiles -W- StartingJob - cannot find all patterns\n")
            elif (block.find('JobStarted') >= 0):
# 19.09.03
#
#, JobStarted HostName pcamsf6 hrdl , UID 116 , PID 5751 5746 , Type
# Producer , ExitStatus NOP , StatusType OneRunOnly , Mips 2445
#                                                     ^^^^
#
                patternsmatched = 0
                JobStartedPatterns = ("JobStarted", "HostName","UID","PID","Type", "ExitStatus","StatusType","Mips")
                jobstarted = [None] * len(JobStartedPatterns)
                j = 0
                for pat in JobStartedPatterns:
                    for xyz in junk:
                        jj = xyz.split()
                        if (len(jj)>1):
                            if ( pat == jj[0] ):
                                patternsmatched += 1
                                jobstarted[j]=self.trimblanks(jj[1])
                                if (jj[0] == "Mips"):
                                    jobmips = int(self.trimblanks(jj[1]))
                                break
                    j += 1
                if (jobstarted[2] != None):
                    jobid = int(jobstarted[2])
                    if(self.findJob(jobid,buf,dirpath,cid) !=jobid):
                        output.write("fatal - cannot find JobInfo for %d\n" %(jobid))
                        self.BadRuns[self.nCheckedCite] += 1
                        self.__cmd("mv %s %s.0" %(inputfile,inputorig))
                        mutex.release()
                        return 0, copylog
                else:
                        output.write("fatal - cannot find JobInfo for in %s\n" %(inputfile))
                        print("fatal - cannot find JobInfo for in "+inputfile)
                        self.__cmd("mv %s %s.0" %(inputfile,inputorig))
                        mutex.release()
                        return 0, copylog 
                if(patternsmatched == len(JobStartedPatterns)-1 or patternsmatched == len(JobStartedPatterns)-2):
                    jobstarted[0]="JobStarted"
                    jobstartedR=1
                    lastjobid=int(jobstarted[2])
                    sql = "UPDATE Jobs SET mips = %d, host = '%s' where jid = %d" %(jobmips, jobstarted[1], lastjobid)
                    output.write(sql + "\n")
                    self.sqlserver.Update(sql)
                    self.sqlserver.Commit(1)
                else:
                    output.write("parseJournalFiles -W- JobStarted - cannot find all patterns\n")
            elif(block.find('StartingRun')>=0):
                patternsmatched = 0
                StartingRunPatterns = ("StartingRun","ID","Run","FirstEvent","LastEvent","Prio","Path","Status","History","CounterFail", "ClientID","SubmitTime","SubmitTimeU","Host","EventsProcessed","LastEvent","Errors","CPU","Elapsed","CPU/Event","Status","DataMC")
                startingrun = [None] * len(StartingRunPatterns)
                j=0
                for pat in StartingRunPatterns:
                    for xyz in junk:
                        jj=xyz.split()
                        if(len(jj)>1):
                            if(pat == jj[0]):
                                patternsmatched += 1
                                startingrun[j]=self.trimblanks(jj[1])
                                if(jj[0] == 'Run'):
                                    break
                    j=j+1
                runtype = int(startingrun[21])
                if (lastjobid != int(startingrun[2]) and datamc != 1):
                    # for MC recontruction
                    print "Changing data mc %d %s" %(lastjobid, startingrun[2])
                    runtype = 2
                run=startingrun[2]
                if (runtype != 0):
                    runtable = 'dataruns'
                else:
                    runtable = 'runs'
                startingrun[0]="StartingRun"
                sql=" select status from %s where jid=%d" %(runtable, lastjobid)
                rq=self.sqlserver.Query(sql)
                if(len(rq)>0 and rq[0][0].find('Completed')>=0):
                    print "Run ",run," already completed in database do nothing"
                    self.__cmd("mv -f %s %s.1" %(inputfile,inputorig))
                    mutex.release()
                    return 0, copylog
                if(patternsmatched == len(StartingRunPatterns)+3 or patternsmatched == len(StartingRunPatterns)+2):
                    startingrunR=1
                    self.CheckedRuns[self.nCheckedCite] += 1
                    if (runtype == 0):
                        self.insertRun(int(startingrun[2]),lastjobid,int(startingrun[3]),int(startingrun[4]),feti,leti,int(startingrun[12]),startingrun[7])
                    else:
                        self.insertDataRun(int(startingrun[2]),lastjobid,int(startingrun[3]),int(startingrun[4]),feti,leti,int(startingrun[12]),startingrun[7])
                    if(len(startingrun)>13):
                        host=startingrun[13]
                    if (runtype != 0 and (run_incomplete == 1 or run_finished == 0)):
                        # data do not allow incomplete run
                        print "Run %s incomplete or not finished while real data mode, do nothing." %(startingrun[2])
                        self.__cmd("mv %s %s.0" %(inputfile, inputorig))
                        mutex.release()
                        return 0, copylog

                    sql = "UPDATE Jobs set host='%s',events=%s, errors=%s,cputime=%s, elapsed=%s,timestamp=%d, mips = %s where jid=%d" %(host,startingrun[14],startingrun[16],startingrun[17],startingrun[18],timestamp,jobmips,lastjobid)
                    self.sqlserver.Update(sql)
                    self.sqlserver.Commit(1)
                    output.write(sql + "\n")
                else:
                    output.write("StartingRun - cannot find all patterns \n")
                    print "StartingRun -W- cannot find all patterns " ,patternsmatched,len(StartingRunPatterns)
            elif(block.find('OpenDST')>=0):
                patternsmatched = 0;
                OpenDSTPatterns = ("OpenDST","Status","Type","Name","Version","Size","CRC","Insert","Begin","End","Run","FirstEvent","LastEvent","EventNumber","ErrorNumber")
                opendst = [None] * len(OpenDSTPatterns)
                for i in range (0,len(junk)):
                    jj=junk[i].split()
                    if(len(jj)>1):
                        found=0
                        j=0
                        while(j<len(OpenDSTPatterns) and found==0):
                            if(jj[0] == OpenDSTPatterns[j] or jj[0] == OpenDSTPatterns[j].lower()):
                                opendst[j]=self.trimblanks(jj[1])
                                patternsmatched += 1
                                found=1
                            j += 1
                if(patternsmatched==len(OpenDSTPatterns)-1):
                    opendst[0]="OpenDST"
                    self.CheckedDSTs[self.nCheckedCite]=self.CheckedDSTs[self.nCheckedCite]+1
                else:
                    output.write("OPENDST - cannot find all patterns \n")
            
            elif(block.find("CloseDST")>=0):
                patternsmatched=0
                statusIndx=1
                fileIndx=3
                crcIndx=6
                CloseDSTPatterns=("CloseDST","Status","Type","Name","Version","Size","CRC","Insert","Begin","End","Run","FirstEvent","LastEvent","EventNumber","ErrorNumber")
                closedst = [None] * len(CloseDSTPatterns)
                for icl in range(0,len(CloseDSTPatterns)):
                    closedst[icl]=0
                for i in range (0,len(junk)):
                    jj = junk[i].split()
                    if (len(jj)>1):
                        found=0
                        j=0
                        while(j<len(CloseDSTPatterns) and found==0):
                            if(jj[0] == CloseDSTPatterns[j] or jj[0] == CloseDSTPatterns[j].lower()):
                                    closedst[j]=self.trimblanks(jj[1])
                                    patternsmatched=patternsmatched+1
                                    found=1
                            j=j+1
                if(feti>int(closedst[8]) and int(closedst[8])>0):
                    feti=int(closedst[8])
                if(leti<int(closedst[9])):
                    leti=int(closedst[9])
                if (patternsmatched == len(CloseDSTPatterns)-1):
                    if(closedst[crcIndx]==0):
                        output.write("Status : %d, CRC %d. Skip file : : %d\n" %(closedst[statusIndx],closedst[crcIndx],closedst[fileIndx]))
                    else:
                        pathwithouthost = (closedst[fileIndx].split(':'))[1]
                        if (re.match("^/castor/cern.ch/", pathwithouthost) or re.match("^/eosams/", pathwithouthost) or re.match("^/eos/ams/", pathwithouthost)):
                            dstfile = pathwithouthost
                            inputfilel = dstfile
                        else:
                            junk=closedst[fileIndx].split('/')
                            dstfile=self.trimblanks(junk[len(junk)-1])
                            filename=dstfile
                            dstfile=dirpath+"/"+dstfile
                            inputfilel = os.path.realpath(dstfile)
                            inputfilel = inputfilel.replace("/eos/ams/", "/eosams/")
                        dstlink = dstfile
                        if (re.match("^/castor", inputfilel)):
                            dstfile = inputfilel
                            if (outputpath is None):
                                ouputpath = '/castor/cern.ch/ams'
                        elif (re.match("^/eosams/", inputfilel)):
                            dstfile = inputfilel
#                            if (outputpath is None):
#                                outputpath = '/eosams'
                        elif (re.match("^/", inputfilel)):
                            junk = inputfilel.split('/')
                            if (runtype % 2 == 1):
                                path = '/Data'
                            else:
                                path = '/MC'
                            disk = junk[1]
                            sql = "SELECT disk FROM filesystems WHERE status='Active' and  isonline=1 and path = '%s' and disk = '%s' ORDER BY priority DESC, available" %(path, disk)
                            rq=self.sqlserver.Query(sql)
                            dstfile = inputfilel
                            if(len(rq)>0 and rq[0][0] == disk):
                                outputpath = disk
                            else:
                                sql = "SELECT disk FROM filesystems WHERE status='Full' and  Allowed=0 and path = '%s' and disk = '%s' ORDER BY priority DESC, available" %(path, disk)
                                rq=self.sqlserver.Query(sql)
                                dstfile = inputfilel
                                if(len(rq)>0 and rq[0][0] == disk):
                                    outputpath = disk
                        if (re.match("^/castor", dstfile)):
                            pid = os.getpid()
                            tmpf = "/tmp/castor.%d" %(pid)
                            if (os.path.isfile(tmpf)):
                                try:
                                    os.unlink(tmpf)
                                except IOError,e:
                                    print e
#                            self.__cmd("/afs/cern.ch/ams/local/bin/timeout --signal 9 600 stager_get -M %s" %(dstfile))
                            cmd = "/usr/bin/nsls -l %s >%s" %(dstfile, tmpf)
                            i = self.__cmd(cmd)
                            if (i == 0):
                                try:
                                    fo = open(tmpf, "r")
                                    line=fo.readline()
                                    fo.close
                                    junk = line.split()
                                    if (len(junk) > 4):
                                        dstsize = int(junk[4])
                                except IOError,e:
                                    print e
                                    print "parsejournalfile-E-Unableto open file %s" %(tmpf)
                            else:
                                print "parsejournalfile-E-Unableto %s" %(cmd)
                            os.unlink(tmpf)
                        elif (re.match("^/eosams/", inputfilel)):
                            dstsize = self.eosGetSize(inputfilel)
                            if (dstsize < 0):
                                print "parsejournalfile-E-Unableto open file %s" %(inputfilel)
                        else:
                            try:
                                dstsize=int(os.stat(dstfile)[ST_SIZE])
                            except OSError, e:
                                dstsize=-1
                        try:
                            dstsize
                        except NameError:
                            dstsize = -1
                        if (dstsize == -1):
                            output.write("parseJournalFile-W-CloseDST block : cannot stat %s\n" %(dstfile))
                            runfinishedR=1
                            copyfailed = -1
                            break
                        else:
                            if(closedst[1] != "Validated" and closedst[1] != "Success" and closedst[1] != "OK"):
                                output.write("parseJournalFile -W- CloseDST block : %s,  DST status  %s. Check anyway\n" %(dstfile,closedst[1]))
                            dstsize="%.1f" %(float(dstsize)/1024./1024.)
                            closedst[0]="CloseDST"
                            output.write(dstfile + "\n")
                            ntstatus=closedst[1]
                            nttype   =closedst[2]
                            version  =closedst[4]
                            ntcrc    =int(closedst[6])
                            run      =int(closedst[10])
                            #jobid    =int(closedst[10])
                            dstfevent=int(closedst[11])
                            dstlevent=int(closedst[12])
                            ntevents =int(closedst[13])
                            badevents=int(closedst[14])
                            if (self.checkDSTVersion(version)!=1):
                                output.write("------------ Check DST; Version : version / Min production version : %s \n" %(version,self.Version()))
                                self.unchecked += 1
                                copyfailed += 1
                                self.BadDSTs[self.nCheckedCite] += 1
                                break

                            levent += dstlevent-dstfevent+1
                            i = self.compareCRC(dstfile, ntcrc)
                            output.write("compareCRC(%s,%d):  Status %d \n" %(dstfile,ntcrc,i))
                            if(i!=1):
                                self.unchecked += 1
                                copyfailed += 1
                                self.BadCRC[self.nCheckedCite] += 1
                                break
                            (ret,i)=self.validateDST(dstfile,ntevents,nttype,dstlevent)
                            output.write("validateDST(%s, %d, %s, %d, %d) : Status : %d : Ret : %d\n" %(dstfile, ntevents, nttype, dstlevent, jobid, i, ret))
                            if (ret != 1):
                                self.unchecked += 1
                                copyfailed += 1
                                output.write(" validateDST return code != 1. Quit. \n")
                                break
                            if (i == 0xff00 or i & 0xff):
                                if (ntstatus != 'Validated'):
                                    ntstatus = "Unchecked"
                                    badevents="NULL"
                                    self.unchecked += 1
                                    copyfailed += 1
                                    break
                                else:
                                    self.thrusted += 1
                            else:
                                i >>= 8
                                if(i==134):
                                    ntstatus="STAGEIN"
                                    self.__cmd("mv -v %s %s" %(inputwork, inputorig))
                                    mutex.release()
                                    return 0, copylog
                                if (int(i/128)):
                                    ntevents = 0
                                    badevents = "NULL"
                                    ntstatus = "Bad%d" %(i-128)
                                    self.bad += 1
                                    levent -= dstlevent - dstfevent + 1
                                else:
                                    badevents = int(i*ntevents/100)
                                    tevents += ntevents
                                    terrors += badevents
                                    self.validated += 1
                                    ntstatus = 'Validated'
                                    if (runtype % 2 == 0):
                                        path = '/MC'
                                    else:
                                        path = '/Data'   
                                    castortime = 0
                                    (outputpatha, rstatus, odisk, castortime) = self.doCopy(jobid, dstfile, ntcrc, version, outputpath, path)
                                    outputpath = outputpatha
                                    if (outputpath != None):
                                        mvntuples.append(outputpath)
                                    else:
                                        print "Cannot find an active disk, skipping ", jobid
                                        self.__cmd("mv %s %s" %(inputwork, inputfile))
                                        mutex.release()
                                        return 0, copylog                                        
                                    output.write("doCopy return status : %d \n" %(rstatus))
                                    if (rstatus == 1):
                                        if (castortime == 0 and self.castorcopy):
                                            castorPrefix = '/castor/cern.ch/ams';
                                            junk = outputpath.split("/")
                                            castordir = castorPrefix
                                            for i in range(2, len(junk)-1):
                                                castordir += "/%s" %(junk[i])
#                                            sys = "/afs/cern.ch/ams/local/bin/timeout --signal 9 90 /usr/bin/nsmkdir -p %s" %(castordir)
#                                            i = self.__cmd(sys)
                                            i = self.mkdirCastor(castordir)
                                            rfcp = "/afs/cern.ch/ams/local/bin/timeout --signal 9 1800 /usr/bin/rfcp %s %s" %(outputpath, castordir)
                                            failure = self.__cmd(rfcp)
                                            if (failure):
                                                print " %s failed" %(rfcp)
                                            else:
                                                castortime = int(time.time())
                                        if (self.skipcrc or (self.castoronly and outputpath.find('/castor/cern.ch/') >= 0)):
                                            crctime = 0
                                        else:
                                            crctime = timestamp
                                        self.InsertNtuple(run, version, nttype, jobid, dstfevent, dstlevent, ntevents, badevents, timestamp, dstsize, ntstatus, outputpath, ntcrc, crctime, 1, castortime, runtype%2, feti, leti)
                                        self.sqlserver.Commit(1)
                                        output.write("insert ntuple : %d, %s, %s\n" %(run, outputpath, closedst[1]))
                                        self.gbDST[self.nCheckedCite] += float(dstsize)
                                        cpntuples.append(dstlink)
                                        validated_ntuples += 1
                                    else:
                                        output.write("***** Error in doCopy for : %s\n" %(outputpath))
                else:
                    output.write("parseJournalFiles -W- CloseDST - cannot find all patterns\n")
                # end CloseDST
                #
            elif (block.find("RunFinished")>=0):
#
# RunFinished CInfo  , Host pcamsvc , EventsProcessed 10796 , LastEvent 21000 ,
# Errors 3 , CPU 712.62 , Elapsed 725.923 , CPU/Event 0.0660017 , Status Finished
#
                patternsmatched = 0
                RunFinishedPatterns = ("RunFinished","Host","Run","EventsProcessed","LastEvent","Errors", "CPU","Elapsed","CPU/Event","Status")
                runfinished = [None] * len(RunFinishedPatterns)
                for i in range(0, len(junk)):
                    jj = junk[i].split()
                    if (len(jj)):
                        found = 0
                        j = 0
                        while (j < len(RunFinishedPatterns) and found == 0):
                            if (jj[0] == RunFinishedPatterns[j]):
                                runfinished[j] = self.trimblanks(jj[1])
                                patternsmatched += 1
                                found = 1
                            j += 1
                if (patternsmatched == len(RunFinishedPatterns) or patternsmatched == len(RunFinishedPatterns) - 1):
                    runfinished[0] = "RunFinished"
                    runfinishedR   = 1
                    if (runtype!=0):
                        runtable = 'dataruns'
                    else:
                        runtable = 'runs'
                    if (runtype==0):
                        sql = "UPDATE %s SET LEVENT=%s WHERE jid=%d" %(runtable, runfinished[4], lastjobid)
                        self.sqlserver.Update(sql)
                    else:
                        sql = "UPDATE jobs SET realtriggers=%s WHERE jid=%d" %(runfinished[3], lastjobid)
                        self.sqlserver.Update(sql)
                    self.sqlserver.Commit(1)
                    output.write(sql + "\n")
                    cputime = int(float(runfinished[6]))
                    elapsed = int(float(runfinished[7]))
                    host = runfinished[1]
                    sql = "update jobs set events=%s, errors=%s, cputime=%d, elapsed=%d, host='%s', mips=%d, timestamp=%d where jid = %d" %(runfinished[3], runfinished[5], cputime, elapsed, host, jobmips, timestamp, lastjobid)
                    output.write(sql + "\n")
                    self.sqlserver.Update(sql)
                    self.sqlserver.Commit(1)
                else:
                    output.write("parseJournalFile -W- RunFinished - cannot find all patterns %d/%d\n" %(patternsmatched, RunFinishedPatterns))
                    #
                    # end RunFinished
                    #
        if (startingrunR == 1 or runfinishedR == 1):
            status = "Failed"
            cmd = None
            inputfileLink = inputorig + '.0'
            inputfileAdd = inputorig + '.2'
#            if (copyfailed == 0):
            if (validated_ntuples > 0):
                if (len(cpntuples) > 0):
                    status = 'Completed'
                    inputfileLink = inputorig + '.1'
                    try:
                        jououtput=open(inputfileAdd,'w')
                    except IOError,e:
                        print e
                        mutex.release()
                        return 0, copylog
                    t = int(time.time())
                    tl = time.strftime("%c", time.localtime(t))
                    jououtput.write("\n")
                    jououtput.write("-I-TimeStamp %d %s\n" %(t, tl))
                    dst = len(cpntuples)
                    junk = outputpath.split('/')
                    outp = ""
                    for i in range(0, len(junk)-1):
                        if (i > 0):
                            outp += '/'
                        outp += junk[i]
                    run = int(startingrun[2])
                    jououtput.write(", RunValidated , Run %d , DST %d , Path %s  \n" %(run, dst, outp))
                    jououtput.close()
                    self.GoodRuns[self.nCheckedCite] += 1
                    if (runfinishedR != 1):
                        output.write("End of Run not found update Jobs \n")
                        sql = "UPDATE Jobs SET host = '%s', events = %d, errors = %d, cputime = -1, elapsed = -1, timestamp = %d where jid = %d" %(host, tevents, terrors, timestamp, lastjobid)
                        output.write(sql + " \n")
                        self.sqlserver.Update(sql)
                        self.sqlserver.Commit(1)
                    if (runtype == 0):
                        sql = "select sum(ntuples.levent-ntuples.fevent+1),min(ntuples.fevent)  from ntuples,runs where ntuples.run=runs.run and runs.run=%d and ntuples.datamc=0" %(run)
                        r4 = self.sqlserver.Query(sql)
                        ntevt = r4[0][0]
                        fevt = r4[0][1]
                        if (ntevt is None):
                            ntevt = 0
                        if (fevt is None):
                            fevt = 0
                        if (ntevt > 0):
                            sql="UPDATE Runs SET fevent=%d, Levent=%d, fetime=%d, letime=%d WHERE jid=%d" %(fevt, ntevt-1+fevt, feti, leti, run)
                            output.write(sql + "\n")
                            self.sqlserver.Update(sql)
                            self.sqlserver.Commit(1)
                        if (ntevt >= 0):
                            sql=" update jobs set realtriggers=%d, timekill=0 where jid=%d" %(ntevt, run)
                            output.write(sql + "\n")
                            self.sqlserver.Update(sql)
                            self.sqlserver.Commit(1)
                    for ntuple in cpntuples:
                        cmd = self.rm + ntuple
                        if (ntuple.find('/eosams/') == 0):
                            cmd = "%s rm %s" %(self.eosselect, self.eosLink2Real(ntuple))
                        output.write(cmd + "\n")
                        self.__cmd(cmd)
                        output.write("Validation done : system command %s \n" %(cmd))
                        self.GoodDSTs[self.nCheckedCite] += 1
                else:
                    self.BadRuns[self.nCheckedCite] += 1
            else:
                run = int(startingrun[2])
                output.write("Validation/copy failed = %d for  Run =%d \n" %(copyfailed, run))
                status = 'Unchecked'
                self.BadRuns[self.nCheckedCite] += 1
                for ntuple in mvntuples:
                    cmd = "%s %s" %(self.rm, ntuple)
                    if (ntuple.find('/eosams/') == 0):
                        cmd = "%s rm %s" %(self.eosselect, self.eosLink2Real(ntuple))
                    output.write("Validation failed : system command %s \n" %(cmd))
                    output.write(cmd + "\n")
                    self.__cmd(cmd)
                sql = "DELETE ntuples WHERE jid=%d" %(lastjobid)
                self.sqlserver.Update(sql)
                self.sqlserver.Commit(1)
                output.write(sql + "\n")
            if (runtype != 0):
                runtable = 'dataruns'
            else:
                runtable = 'runs'
            if (startingrun[2] != 0):
                sql = "UPDATE %s SET STATUS='%s' WHERE jid=%d" %(runtable, status, lastjobid)
                self.sqlserver.Update(sql)
                self.sqlserver.Commit(1)
                output.write("Update Runs : %s \n" %(sql))
                if (status == 'Failed' or status == 'Unchecked'):
                    sql = "SELECT dirpath FROM journals WHERE cid=-1"
                    ret = self.sqlserver.Query(sql)
                    if (ret[0][0] != None):
                        junkdir = ret[0][0]
                        if (self.v):
                            print "run status %s  moving mv %s/*%s* %s " %(status, dirpath, startingrun[2], junkdir)
                        sql = "update jobs set realtriggers=-1 where jid=%d" %(jobid)
                        output.write(sql + " \n")
                        self.sqlserver.Update(sql)
                        self.sqlserver.Commit(1)
                        output.write("Validation/copy failed : mv ntuples to %s \n" %(junkdir))
                        output.close
                        self.__cmd("mv %s %s" %(inputfile, inputfileLink))
                        mutex.release()
                        self.suc[jobid] = jobid
                        return (jobid, copylog)
            self.__cmd("mv %s %s" %(inputfile, inputfileLink))
            self.__cmd("cat %s >> %s" %(inputfileAdd, inputfileLink))
            os.unlink(inputfileAdd)
            output.write("mv %s %s\n" %(inputfile, inputfileLink))
            if (self.v == 1):
                print "mv %s %s\n" %(inputfile, inputfileLink)
            if (status == 'Completed'):
                if (runtype == 0):
                    self.updateRunCatalog(int(startingrun[2]))
                    if (self.v == 1):
                        print "Update RunCatalog table : %s" %(startingrun[2])
                self.sqlserver.Commit(1)
                mutex.release()
                return 0, copylog
        else:
            inputfileLink = inputorig + '.0'
            self.__cmd("mv %s %s" %(inputfile, inputfileLink))
            if (self.v == 1):
                print "mv %s %s\n" %(inputfile, inputfileLink)
            self.sqlserver.Commit(0)
            mutex.release()
            return -1, copylog
        self.sqlserver.Commit(0)
        output.close()
        mutex.release()
        return 0, copylog


       
    def buildTDV(self,name,commit,verbose):
        if(os.environ.has_key('AMSDataDirRW')):
            amsdatadir=os.environ['AMSDataDirRW']
        else:
            amsdatadir="/afs/cern.ch/ams/Offline/AMSDataDir"
        dbdir=os.path.join(amsdatadir,"DataBase")
        ltdv=dbdir+"/ltdv"
        for file in os.listdir(dbdir):
            id=1
            ret=self.sqlserver.Query("select count(id) from TDV");
            if(ret[0][0]>0):
                ret=self.sqlserver.Query("select max(id) from TDV");
                id=ret[0][0]+1
            path=os.path.join(dbdir,file)
            if(len(name)>1 and file.find(name)<0):
                continue
            if os.path.isdir(path):
                sql="select id from TDV where name='%s'" %file
                ret=self.sqlserver.Query(sql)
                if(len(ret)>0):
                    if(len(name)<=1):
                        id=ret[0][0]
#                        sql="delete  from TDV where name='%s'" %file
#                        self.sqlserver.Update(sql)
#                        continue
                    else:
                        id=ret[0][0]
#                        sql="delete  from TDV where name='%s'" %file
#                        self.sqlserver.Update(sql)
                for dir in os.listdir(path):
                    pdir=os.path.join(path,dir)
                    if(os.path.isdir(pdir)):
                        for tdv in os.listdir(pdir):
                            if(tdv.find(file)>=0):
                                ptdv=os.path.join(pdir,tdv)
                                sql= "select timestamp from TDV where path='%s'" %ptdv
                                ret=self.sqlserver.Query(sql)
                                if(len(ret)>0):
                                    if(ret[0][0]==os.stat(ptdv)[ST_MTIME]):
                                        sql="delete from TDV where path='%s'" %ptdv
                                        self.sqlserver.Update(sql)
                                        self.sqlserver.Commit(1)
                                        continue
                                    
                                if(verbose):
                                    print tdv;
                                if(tdv.find('.0.')>0):
                                    datamc=0
                                else:
                                    datamc=1
                                ltdvo="./ltdv.o"
                                try:
                                    os.unlink(ltdvo);
                                except:
                                    if(verbose):
                                        print " "
                                readtdv=ltdv+" "+ptdv+" > "+ltdvo
                                self.__cmd(readtdv)
                                fltdvo=open(ltdvo,'r')
                                good=0
                                crc=""
                                end=""
                                insert=""
                                begin=""
                                size=""
                                for line in fltdvo.readlines():
                                    if(line.find("OK")>=0):
                                        good=1
                                    if(good == 1):
                                        if(line.find("size")>=0):
                                            size=line.split()[1]
                                        elif(line.find("crc")>=0):
                                            crc=line.split()[1]
                                        elif(line.find("begin")>=0):
                                            begin=line.split()[1]
                                        elif(line.find("end")>=0):
                                            end=line.split()[1]
                                        elif(line.find("insert")>=0):
                                            insert=line.split()[1]
                                        
                                fltdvo.close()
                                if(len(size)>1 and len(end)>1 and len(insert)>1 and len(begin)>1 and len(crc)>1):
                                    sql="insert into tdv values( %d, '%s', '%s',%s,%s,%d,%s,%s,%s,%d)" %(id,file,ptdv,size[0:len(size)-1],crc[0:len(crc)-1],datamc,insert[0:len(insert)-1],begin[0:len(begin)-1],end[0:len(end)-1],os.stat(ptdv)[ST_MTIME])
                                    self.sqlserver.Update(sql)
                                    self.sqlserver.Commit(commit)

                                    
    def CheckDataSet(self,run2p,dataset,v,f,tab=0,runmin=0,runmax=2000000000):
        self.verbose=v
        self.run2p=run2p
        self.force=0         
        self.selectedstream = 1
        if (dataset.find('pass') >= 0 or dataset.find('std') >= 0):
            self.selectedstream = 0
        if(f>0):
           self.force=f
        rundd=""
        rund=""
        runn=""
        runst=""
        bad1=array('i')
        bad2=array('i')
        badeos=array('i')
        typess=["0SCI","0LAS","0CAL","0CMD","0CAB"]

        for type in typess:
           sql="select path,paths,run from datafiles where   type like '%s%%' " %(type)
           sql=sql+" and status not like '%BAD%'"
           files=self.sqlserver.Query(sql)
           for file in files:
                run=file[2]
                if(run2p!=0 and run!=run2p):
                        continue
                pathso=file[1]
                cmd="ls -lL "+pathso
                pair = self.__cmd_output(cmd)
                out=pair[1]
                if(out.find("No such")>=0 or pathso.find('ams.cern.ch/Offline')>=0):
                    paths=pathso.replace('ams.cern.ch/Offline','cern.ch/ams/Offline',1)
                    cmd="ln -sf "+file[0]+" "+paths
                    self.__cmd(cmd)
                    print cmd
        types=["0SCI","0LAS","0CAL","0CMD","0CAB"]
        datapath=dataset
        ds1=""
        ds2="invalid"
        did=-1
        if(dataset.find("/")>=0):
            junk=dataset.split('/')
            dataset=junk[len(junk)-1]
            ds1=junk[len(junk)-2]
            if(len(junk)>=3):
                ds2=junk[len(junk)-3]
            sql="select did from datasets where name like '%s' " %(ds1)
            ds=self.sqlserver.Query(sql)
            if(len(ds)==1):
                did=ds[0][0]
                if(f==2):
                        sql="select path from ntuples where path like '%%%s/%%' and datamc=1  and path not like '/castor%%' " %(datapath)
                        files=self.sqlserver.Query(sql)
                        for file in files:
                                self.linkdataset(file[0],"/afs/cern.ch/ams/Offline/DataSetsDir",1)
	    elif(len(ds)>1):
                (np,bp,pid,vdb)=self.getActiveProductionPeriodByName(ds2)
                sql="select did from datasets where name like '%s' and version='%s' " %(ds1,vdb)
                ds=self.sqlserver.Query(sql)
                if(len(ds)==1):
                    did=ds[0][0]

            else:
                sql="select did from datasets where name like '%s' " %(dataset)
	        ds=self.sqlserver.Query(sql) 
                if(len(ds)==1):
                       did=ds[0][0]
                       dataset=""
        else:
            sql="select did from datasets where name like '%s' " %(dataset)
            dataset=""
            ds=self.sqlserver.Query(sql)
            if(len(ds)==1):
                did=ds[0][0]
        if(did>0):
            runst=runst+" and jobs.did=%d " %(did)
        else:
            print " found / while did -1 , return "
            return
        if(tab):
            print "<HR>"
            print "<table border=1>"
        for type in types:
           sql="select path,paths,run from datafiles where type like '%s%%' " %(type)
           sql=sql+" and status not like '%BAD%'"
           files=self.sqlserver.Query(sql)
           for file in files:
                run=file[2]
                if(run2p!=0 and run!=run2p):
                        continue
                pathso=file[1]
                paths=pathso.replace('ams.cern.ch/Offline','cern.ch/ams/Offline',1)
                junk=paths.split('/')
                bpath=""
                for i in range (1,len(junk)):
                        bpath=bpath+"/"+junk[i]
                isdir=run/1000000
                isdir=isdir*1000000;
                sdir="/%d" %(isdir)
                if(paths.find(sdir)<0):
                        sysc="rm "+paths
                        self.__cmd(sysc)
                        bpath=bpath+sdir;
                        cmd="mkdir -p "+bpath
                        i=self.__cmd(cmd)
                        if(i):
                                print "Command failed ",cmd  
                        else:
                           sysc="ln -sf "+file[0]+" "+bpath
                           self.__cmd(sysc)
                           bpath=bpath+"/"+junk[len(junk)-1]
                           sql="update datafiles set paths='%s' where paths='%s'" %(bpath,file[1])     
                           self.sqlserver.Update(sql)
                           self.sqlserver.Commit(1)
                else:
                   continue     
                   sysc="ln -sf "+file[0]+" /afs/cern.ch/ams/Offline/RunsDir/"+type+"/"
                   self.__cmd(sysc)
        if(run2p!=0):
            rundd=" and datafiles.run=%d " %(run2p)
            rund=" and dataruns.run=%d " %(run2p)
            runn=" and ntuples.run=%d " %(run2p)
        runad=" and dataruns.run>=%s and dataruns.run<=%s " %(runmin,runmax)
        runadd=" and datafiles.run>=%s and datafiles.run<=%s " %(runmin,runmax)
        runan=" and ntuples.run>=%s and ntuples.run<=%s " %(runmin,runmax)
        rundd=rundd+runadd
        rund=rund+runad
        runn=runn+runan
        if(self.force>1):
            eos_prefix="/tmp/eosams"
            eosmount="/afs/cern.ch/project/eos/installation/0.3.1-22/bin/eos.select -b fuse mount "+eos_prefix 
            self.__cmd(eosmount)
            sql="select path,run,sizemb,jid,buildno from ntuples where path like '%%%s/%%' and eostime>0 and datamc=1  %s " %(datapath,runn)
            paths=self.sqlserver.Query(sql)
            for path in paths:
                if(path[0].find('/castor/cern.ch')>=0):
                    nsls="nsls -l "+path[0]
                    pair = self.__cmd_output(nsls)
                    if(pair[0]==0):
                        sp1=pair[1].split(' ')
                        if(len(sp1)>4):
                            pos=0
                            for chunk in sp1:
                                if(chunk!=""):
                                    if(pos==4):
                                        sp1[4]=chunk
                                    pos=pos+1
                                    
                            patheos=path[0].replace('/castor/cern.ch',eos_prefix,1)
                            eosls ="ls -l "+patheos
                            p2 = self.__cmd_output(eosls)
                            sizemb=path[2]
                            sp2=p2[1].split(' ')
                            if(p2[0]!=0 or len(sp2)<=4):
                                    badeos.append(path[1])
                                    print "Run ",path[1] ," have eos problems "
                            elif  (sp2[4]!=sp1[4] or int(sp2[4])==0):
                                    badeos.append(path[1])
                                    print "Run ",path[1] ," have eos problems ",sp2[4],sp1[4]
                            elif ( abs(int(sp2[4])/1024/1024-sizemb)>2):
                                if(int(sp2[4])/1000/1000-sizemb<2):
                                    print " correcting size ",int(int(sp2[4])/1000/1000),sizemb
                                    sizemb=int(int(sp2[4])/1024/1024+0.5)
                                    sql="update ntuples set sizemb=%d where path like '%s'" %(sizemb,path[0])
                                    self.sqlserver.Update(sql)
                                    self.sqlserver.Commit(1)
                                else:
#                                    badeos.append(path[1])
                                    print "Run ",path[1] ," have eos problems size ",sp2[4],sp1[4],sizemb
                                    
        if(self.force):
            sql="select run,sum(levent-fevent+1),sum(jid)/count(jid),sum(nevents) from ntuples where path like '%%%s/%%' and datamc=1  %s group by run" %(datapath,runn)
            files=self.sqlserver.Query(sql)
            sql="select run,levent-fevent+1 from datafiles where status='OK' and type like 'SCI%%' %s " %(rundd)
            runs=self.sqlserver.Query(sql)
            tot=0
            if(len(files)>0):
                for run in runs:
                    found=0
                    for file in files:
                        if(run[0]==file[0]):
                            found=1
                            if(run[1]>file[1]+1000):
                                bad2.append(run[0])
                                print "Run ",run," and ntuples disagree. run events=",run[1]," ntuple events=",file[1]
           		                     
                            break
                    if(found==0):
                        if(tab==0):
                            tot=tot+1
                            print "Run ",run,"  not found in dataset ",dataset,tot
			    sql="select run,dataruns.jid,realtriggers,dataruns.levent-dataruns.fevent+1 from dataruns,jobs where  jobs.jid=dataruns.jid and split(jobs.jobname) like '%%%s.job' %s %s  and run=%d" %(dataset,rund,runst,run[0])
  			    jid=self.sqlserver.Query(sql)
			    if(len(jid)==0):
				print " job for run ",run[0]  , " doesnot exists "
                            bad1.append(run[0])
                        else:
                            print "<tr>"
                            print "<td>Run %d  </td><td> not found in dataset %s</td>" %(run[0],dataset)
                            print "</tr>"
        else:
            sql="select run,sum(levent-fevent+1),sum(jid)/count(jid),sum(nevents) from ntuples where path like '%%%s/%%' and datamc=1  %s group by run" %(datapath,runn)
            files=self.sqlserver.Query(sql)
            sql="select run,dataruns.jid,dataruns.levent-dataruns.fevent+1 from dataruns,jobs where  jobs.jid=dataruns.jid and split(jobs.jobname) like '%%%s.job' %s %s" %(dataset,rund,runst)
            runs=self.sqlserver.Query(sql)
            tot=0
	    tota=0
	    totg=0	
            for file in files:
		found=0
		for run in runs:
                    if(file[2]==run[1]):
                        found=1
                        break
                if(found==0):
	            tota=tota+1		
                    sql="select run,dataruns.jid,dataruns.levent-dataruns.fevent+1 from dataruns,jobs where  jobs.jid=dataruns.jid and split(jobs.jobname) like '%%%s.job' %s %s and jobs.jid=%d" %(dataset,rund,runst,file[2])
                    print " jid ",file[2],file[0] ," had no dataruns or  job ",tota
                    sql="select timestamp,realtriggers from jobs where   jid=%d" %(file[2])
                    jts=self.sqlserver.Query(sql)
                    if(len(jts)>0):
                        jid=file[2]
                        submit=jts[0][0]
                        status="Completed"
                        rrun=file[0]
                        sql="select fevent,levent,fetime,letime from datafiles where status='OK' and type like 'SCI%%' and run=%d " %(rrun)
                        rt=self.sqlserver.Query(sql)
                        if(len(rt)>0):
                            fevent=rt[0][0]
                            levent=rt[0][1]
                            fetime=rt[0][2]
                            letime=rt[0][3]
                            sql="INSERT INTO dataRuns VALUES(%d,%d,%d,%d,%d,'%s',%d,%d)" %(rrun,fevent,levent,fetime,letime,status,jid,submit)
                            print sql
                            self.sqlserver.Update(sql)
     			    if(jts[0][1]<0):
				print "job ",file[2], " has realtriggers<0 "
				sql="update jobs set realtriggers=%d where jid=%d" %(file[3],file[2])
				self.sqlserver.Update(sql)  				
                            self.sqlserver.Commit(1)
	    print "pass1 ended"
            if(len(files)>0):
                for run in runs:
                    found=0
                    for file in files:
                        if(run[0]==file[0]):
                            found=1
                            if(run[2]!=file[1] and float(run[2])>float(file[1]) and (self.selectedstream == 0 or float(run[2])-float(file[1]) >= float(file[1])/float(file[3])*5.0 )):
                                if(tab==0):
                                    bad2.append(run[0])
                                    print "Run ",run," and ntuples disagree. run events=",run[2]," ntuple events=",file[1]
                                    print "%f v.s. %f" %(float(run[2])-float(file[1]), float(file[1])/float(file[3])*3.0);
                                else:
                                    print "<tr>"
                                    print "<td>Run %d,%d and ntuples disagree.</td><td> Run Events=%d </td><td>Ntuple Events=%d</td>" %(run[0],run[1],run[2],file[1])
                                    print "</tr>"
                            break
                    if(found==0):
  			tot=tot+1
                        if(tab==0):
                            print "Run ",run,"  not found in dataset ",dataset,tot
                            eosfind = "ls -l /tmp/eosams/ams/Data/AMS02/2014/ISS.B700/pass5/%d*.root" % run[0]
                            eosout = self.__cmd_output(eosfind)
                            eosfiles=eosout[1].split("\n")
      	                    for eos in eosfiles:
				e=eos.split(' ')
				if(len(e)>7):
					print e[8]
				continue
                                '''
				r64="/afs/cern.ch/ams/Offline/AMSDataDir/DataManagement/exe/linux/fastntrd64.exe %s 1 1 1 1 1 " %(e[8])
			 	cp=e[8].replace("/tmp/eosams","/castor/cern.ch",1)
				co = self.__cmd_output("nsls -l %s" % cp)
				if(co[0]):
					print " problem with castor ",co,cp
					continue
				f1 = self.__cmd_output(r64)
				f1a=f1[1]
				f1_1=f1a.split("entries")
				if(len(f1_1)<2):
					print " problem with ntuple 1 ",f1
					continue
				f1_2=f1_1[1].split(" ")
				r64="/afs/cern.ch/ams/Offline/AMSDataDir/DataManagement/exe/linux/fastntrd64.exe %s %s 1 1 1 1 " %(e[8],f1_2[1])
				f2 = self.__cmd_output(r64)
				f2a=f2[1]
				f2_1=f2a.split(",,,")
				if(len(f2_1)<3):
					print "problem with ntuple 2 ",f2
					continue
				le=int(f2_1[1])
				fe=int(f2_1[2])	
				ver=f2_1[3]
				crc="/afs/cern.ch/ams/Offline/AMSDataDir/DataManagement/exe/linux/crc %s 1 1" %(e[8])
				c2 = self.__cmd_output(crc)
				c3=c2[1]
				c3_1=c3.split(" ")
				timenow=int(time.time())
				version="build%s/xyz" %(ver)
				sizemb=int(int(e[4])/1024/1024+0.5)
				self.InsertNtuple(run[0],version,"RootFile",run[1],fe,le,int(f1_2[1]),0,timenow,sizemb,"Validated",cp,int(c3_1[1]),timenow,0,timenow,1,None,None,timenow)
				self.sqlserver.Commit(1)
                                '''
			    bad1.append(run[0])   		
                        else:
                            print "<tr>"
                            print "<td>Run %d Events %d  </td><td> Id %d </td><td>  not found in dataset %s</td>" %(run[0],run[2],run[1],dataset)
                            print "</tr>"
           	else:
			totg=totg+1
			if(totg%1000==1):
				print " good ",totg 
            else:
                if(tab==0):
                    print "dataset ",dataset, "not found "
                else:
                    print "<tr>"
                    print "<td>Dataset  %s not found in database</td>" %(cgi.escape(dataset))
                    print "</tr>"
                
        if(tab):
            print """
            </table>
            </HR>
            """
        else:   
           print "not found ",bad1
           print "disagree ",bad2       
           print "eos ",badeos       

    def DeleteDataSet(self,run2p,dataset,u,v,f,donly,datamc,buildno,castoronly):
        self.update=u
        self.verbose=v
        self.run2p=run2p
        self.force=f
        arun2p=""
        if(run2p.find(",")>=0):
            arun2p=" and ( run=-1 "
            junk=run2p.split(',')
            run2p=0
            for run in junk:
                srun2p=" or run=%s " %(run)
                arun2p=arun2p+srun2p
            arun2p=arun2p+" ) "
        else:
            try:
                run2p=int(run2p)	
            except:
                run2p=0
        rund=""
        runn=""
        runnd=""
        runst=" "
        runndf=" "
        runsname=""
        runbuild=""
        runbuildd=""
        if(buildno!=0):
            runbuild=" and ntuples.buildno=%d " %(buildno)
            runbuildd=" and ntuples_deleted.buildno=%d " %(buildno)
        if(castoronly!=0):
            buildno=1
            if(castoronly>0):
                runbuild=runbuild+" and ntuples.castortime>0 "
                runbuildd=runbuildd+" and ntuples_deleted.castortime>0 "
            if(castoronly<0):
                runbuild=runbuild+" and ntuples.castortime=0 "
                runbuildd=runbuildd+" and ntuples_deleted.castortime=0 "
        if(datamc==0):
            sql="select run,jid from ntuples where path like '%%%s/%%' and datamc=%d  %s " %(dataset,datamc,arun2p) 
            check=self.sqlserver.Query(sql)
            if(len(check)>0):
                if(check[0][0] != check[0][1]):
                    print "Changing DataMC to 10 "
                    datamc=10
        if(datamc!=0):
            runsname="dataruns"
            if(self.force==0):
                runst=" and dataruns.status='Completed' "
            if(run2p!=0):
                rund=" and dataruns.run=%d " %(run2p)
                runn=" and ntuples.run=%d " %(run2p)
                runnd=" and ntuples_deleted.run=%d " %(run2p)
            if(run2p<0):
                rund=" and dataruns.run>=%d " %(-run2p)
                runn=" and ntuples.run>=%d " %(-run2p)
                runnd=" and ntuples_deleted.run>=%d " %(-run2p)
            if(run2p/10000000000>0):
                run2p=run2p%10000000000
                rund=" and dataruns.run<%d " %(run2p)
                runn=" and ntuples.run<%d " %(run2p)
                runnd=" and ntuples_deleted.run<%d " %(run2p)
        else:
            runsname="runs"
            if(self.force==0):
                    runst=" and runs.status='Completed' "
            if(run2p!=0):
                rund=" and runs.run=%d " %(run2p)
                runn=" and ntuples.run=%d " %(run2p)
                runnd=" and ntuples_deleted.run=%d " %(run2p)
                runndf=" and run=%d " %(run2p)
            if(run2p<0):
                rund=" and runs.run>=%d " %(-run2p)
                runn=" and ntuples.run>=%d " %(-run2p)
                runnd=" and ntuples_deleted.run>=%d " %(-run2p)
                runndf=" and run>=%d " %(-run2p)
            if(run2p/10000000000>0):
                run2p=run2p%10000000000
                rund=" and runs.run<%d " %(run2p)
                runn=" and ntuples.run<%d " %(run2p)
                runnd=" and ntuples_deleted.run<%d " %(run2p)
        sql="select path,castortime,eostime from ntuples where path like '%%%s/%%' and datamc=%d %s %s %s" %(dataset,datamc%10,runn,runbuild,arun2p) 
        df=0
        files=self.sqlserver.Query(sql)
        datapath=dataset
        ds1=""
        ds2="invalid"
        did=-1
        if(dataset.find("/")>=0):
            junk=dataset.split('/')
            dataset=junk[len(junk)-1]
            ds1=junk[len(junk)-2]
            if(len(junk)>=3):
                ds2=junk[len(junk)-3]
            sql="select did from datasets where name like '%s' " %(ds1)
            ds=self.sqlserver.Query(sql)
            if(len(ds)==1):
                did=ds[0][0]
            elif(len(ds)>1):
                (np,bp,pid,vdb)=self.getActiveProductionPeriodByName(ds2)
                sql="select did from datasets where name like '%s' and version='%s' " %(ds1,vdb)
                ds=self.sqlserver.Query(sql)
                if(len(ds)==1):
                    did=ds[0][0]
            else:
                sql="select did from datasetsdesc where jobname like '%s' " %(dataset)
	        ds=self.sqlserver.Query(sql) 
                if(len(ds)==1):
                       did=ds[0][0]
                       dataset=""
        else:
            sql="select did from datasets where name like '%s' " %(dataset)
            dataset=""
            ds=self.sqlserver.Query(sql)
            if(len(ds)==1):
                did=ds[0][0]
        if(did>0):
            runst=runst+" and jobs.did=%d " %(did)
        else:
            print " found / while did -1 , return "
            return

        if(self.update):
            for file in files:
                if(df==0):
                    self.linkdataset(file[0],"/Offline/DataSetsDir",0)
                else:
                    self.linkdataset(file[0],"/Offline/RunsDir",0)
                cmd="rm "+file[0]
                i=0
                i=self.__cmd(cmd)
                if(i and self.force==-1 ):
                    print " Command Failed ",cmd
                    return
                else:
                    if(self.verbose):
                        print "deleted ",file[0]
                if(file[1]>0):
                    castorPrefix='/castor/cern.ch/ams'
                    delimiter='/Data/'
                    if(datamc==0 or datamc==10):
                        delimiter='/MC/'
                    junk=file[0].split(delimiter)
                    if len(junk)>=2:
                        castorfile=castorPrefix+delimiter+junk[1]
                        castordel="/usr/bin/rfrm "+castorfile
                        i=self.__cmd(castordel)
                        if(i):
                            print " CastorCommand Failed ",castordel
                if(file[2]>0 or self.force):
                    eosPrefix='/eos/ams'
                    delimiter='/Data/'
                    if(datamc==0 or datamc==10):
                        delimiter='/MC/'
                    junk=file[0].split(delimiter)
                    if len(junk)>=2:
                        eosfile=eosPrefix+delimiter+junk[1]
                        junk2 = eosfile.split(datapath)
                        if len(junk2) >= 2:
                            eosdir = junk2[0] + datapath;
                            print "chmod for " + eosdir + " ..."
                            eoschmod = "/afs/cern.ch/project/eos/installation/0.3.2/bin/eos.select chmod "
                            self.__cmd(eoschmod + " 750 " + eosdir)
                            eosdel="/afs/cern.ch/project/eos/installation/0.3.2/bin/eos.select rm  "+eosfile
                            i=self.__cmd(eosdel)
#                            self.__cmd(eoschmod + " 550 " + eosdir)
                            if(i):
                                print " EosCommand Failed ",eosdel
        if(len(files)==0):
            if(datamc==0):
                if(donly==0):
                    df=1
                    sql="select path,castortime from datafiles where path like '%%%s/%%' and type like 'MC%%'  %s %s " %(datapath,runndf,arun2p) 
                    files=self.sqlserver.Query(sql)
        if(len(files)>0 or self.force!=0):
            if(buildno>0):
                sql="insert into jobs_deleted select jobs.* from jobs,%s,ntuples where jobs.jobname like '%%%s.job'  and jobs.jid=%s.jid and ntuples.jid=jobs.jid %s %s  %s and ntuples.jid=%s.jid %s" %(runsname,dataset,runsname,runst,rund,runbuild,runsname,arun2p)
            else:
                sql="insert into jobs_deleted select jobs.* from jobs,%s where jobs.jobname like '%%%s.job'  and jobs.jid=%s.jid  %s %s %s " %(runsname,dataset,runsname,runst,rund,arun2p)
                
            if(donly==0):
                self.sqlserver.Update(sql)
            if(buildno>0):
                sql=" delete from jobs where exists (select * from %s,ntuples where %s.jid=jobs.jid and jobs.jid=ntuples.jid and jobs.jobname like '%%%s.job' %s %s %s  and ntuples.jid=%s.jid %s)" %(runsname,runsname,dataset,runst,rund,runbuild,runsname,arun2p)
            else:
                sql=" delete from jobs where exists (select * from %s where %s.jid=jobs.jid  and jobs.jobname like '%%%s.job' %s %s %s)" %(runsname,runsname,dataset,runst,rund,arun2p)
                
            if(donly==0):
                self.sqlserver.Update(sql)
            sql="insert into ntuples_deleted select * from ntuples where path like '%%%s/%%' and datamc=%d %s %s %s " %(datapath,datamc%10,runn,runbuild,arun2p)
            if(df==0):
                self.sqlserver.Update(sql)
                timenow=int(time.time())
                sql="update ntuples_deleted set timestamp="+str(timenow)+"  where path like '%%%s/%%' and datamc=%d %s %s %s" %(datapath,datamc%10,runnd,runbuildd,arun2p)
                self.sqlserver.Update(sql)
                sql="DELETE from ntuples where path like '%%%s/%%' and datamc=%d %s %s %s " %(datapath,datamc%10,runn,runbuild,arun2p)
                self.sqlserver.Update(sql)
            else:
                sql="DELETE from datafiles where path like '%%%s/%%' and type like 'MC%%' %s %s " %(datapath,runndf,arun2p)

                self.sqlserver.Update(sql)
                
            if(self.update):
                for file in files:
                    if(df==0):
                        self.linkdataset(file[0],"/Offline/DataSetsDir",0)
                    else:
                        self.linkdataset(file[0],"/Offline/RunsDir",0)
                    cmd="rm "+file[0]
                    i=0
		    i=self.__cmd(cmd)
                    if(i and self.force==-1 ):
                        print " Command Failed ",cmd
                        self.sqlserver.Commit(0)
                        return
                    else:
                        if(self.verbose):
                            print "deleted ",file[0]
                    if(file[1]>0):
                        castorPrefix='/castor/cern.ch/ams'
                        delimiter='/Data/'
                        if(datamc==0 or datamc==10):
                            delimiter='/MC/'
                        junk=file[0].split(delimiter)
                        if len(junk)>=2:
                            castorfile=castorPrefix+delimiter+junk[1]
                            castordel="/usr/bin/rfrm "+castorfile
                            i=self.__cmd(castordel)
                            if(i):
                                print " CastorCommand Failed ",castordel
                                
                
                self.sqlserver.Commit(1)

            else:
                self.sqlserver.Commit(0)
               
    def CheckQuota(self,dir,wl,hl,i,v):
        self.v=v
        delay=43200
        sleep=3600
        self.filesfound=0
        self.timenotify=0
        turn=0
        problem=0
        while 1:
            filelist=[]
            turn=turn+1
            notify=1
            junk=dir.split("/")
            cmd="df /%s" % junk[1]
            p = self.__cmd_output(cmd)
            sizet=0 
            if(p[0]==0):
                junk1=p[1].split("\n")
                sizet=int(junk1[2].split(" ")[22])
            alldir=os.listdir("/"+junk[1])
            for file in alldir:
                if(file.find(junk[2])<0):
                    cmd="du -ks /%s/%s" % (junk[1], file)
                    p = self.__cmd_output(cmd)
                    if(p[0]==0):
                        problem=0
                        sizet=sizet-int(p[1].split("/"+junk[1]+"/"+file)[0])
                    else:
                        problem=problem+1
                        if(problem%100 == 1):
                            message=" problem to "+cmd
            
            if(sizet>wl):
                try:
                    self.GetFiles(dir,filelist)
                except:
                    print " problem to read files from "+dir
                timenow=int(time.time())
                size=0
                for tuple in filelist:
                    size=size+tuple[1]
                nd1=0
                nd2=0
                sizet=size
                if(sizet>wl):
                    filelist.sort(sorta)
                now = datetime.datetime.now()
                if(sizet>hl):
                    i=-1;
                    for tuple in filelist:
                        i=i+1
                        if(sizet<hl*0.9):
                            break
                        else:
                            try:
                                os.unlink(tuple[0])
                                sizet=sizet-tuple[1]
                                del filelist[i]
                                i=i-1 
                                nd1=nd1+1
                            except:
                                continue
                if(sizet>wl):
                    i=-1;
                    for tuple in filelist:
                        i=i+1
                        if(sizet<wl*0.9):
                            break
                        else:
                            if(timenow-tuple[2]>delay):
                                sizet=sizet-tuple[1]
                                del filelist[i]
                                i=i-1 
                                nd2=nd2+1
                                try:
                                    os.unlink(tuple[0])
                                except:
                                    continue
                message ="Size reduced to %d %d+%d files deleted %s \n"  %(sizet,nd1,nd2,now.ctime())
                if(nd1+nd2>0):
                    print message
                else:
                    if(turn%100==1):
                        message="Size keep at %d \n" %(sizet)
                        print message
                time.sleep(sleep)
            time.sleep(sleep)
                                       
    def GetFiles(self,dir,filelist):
        for file in os.listdir(dir):
            pfile=os.path.join(dir,file)
            if(not os.path.islink(pfile) and os.path.isdir(pfile)):
                self.GetFiles(pfile,filelist)
            else:
                if(not os.path.islink(pfile) and not os.path.isdir(pfile)):
                    try:
                        o=(pfile,os.stat(pfile)[ST_SIZE]/1024,os.stat(pfile)[ST_MTIME])
                        filelist.append(o)
                    except:
                        print "problem get attributes ",pfile 


    def getDSTFromJournalFile(self,jidl,datamc,ntuple,update=0):

        ntuple.Status=self.dbclient.iorp.Failure
        ntuple.Type=self.dbclient.iorp.EventTag
        ntuple.Name=" "
        if(os.environ.has_key('ProductionLogDir')):
            runsdir=os.environ['ProductionLogDir']
        else:
            runsdir='/afs/cern.ch/ams/local/prod.log'
        if(datamc==0):
            runsdir=runsdir+'/MCProducer/'
        else:
            runsdir=runsdir+'/Producer/'
        fn=[]
        jids=jidl.split(',')
        for jidi in jids:
            jid=int(jidi)
            filej="%010d.journal" %(jid)
            pfilej=os.path.join(runsdir,filej)
            try:
                fltdvo=open(pfilej,'r')
                for linea in fltdvo.readlines():
                    line=linea.split('\n')[0]
                    if(line.find("CloseDST")>=0):
                        pat=line.split(',')
                        npat=0
                        for i in range (2,len(pat)-2):
                            attr=pat[i].split()
                            if(attr[1]=='Status' and attr[2]=='Validated'):
                                ntuple.Status=self.dbclient.iorp.Validated
                                npat=npat+1
                            if(attr[1]=='Type' and attr[2]=='RootFile'):
                                ntuple.Type=self.dbclient.iorp.RootFile
                                npat=npat+1
                            if(attr[1]=='Name'):
                                ntuple.Name=attr[2]
                                npat=npat+1
                            if(attr[1]=='Version'):
                                ntuple.Version=attr[2]
                                npat=npat+1
                            if(attr[1]=='Size'):
                                ntuple.Size=int(attr[2])
                                npat=npat+1
                            if(attr[1]=='crc'):
                                ntuple.crc=int(attr[2])
                                npat=npat+1
                            if(attr[1]=='Insert'):
                                ntuple.Insert=int(attr[2])
                                npat=npat+1
                            if(attr[1]=='Begin'):
                                ntuple.Begin=int(attr[2])
                                npat=npat+1
                            if(attr[1]=='End'):
                                ntuple.End=int(attr[2])
                                npat=npat+1
                            if(attr[1]=='FirstEvent'):
                                ntuple.FirstEvent=int(attr[2])
                                npat=npat+1
                            if(attr[1]=='LastEvent'):
                                ntuple.LastEvent=int(attr[2])
                                npat=npat+1
                            if(attr[1]=='EventNumber'):
                                ntuple.EventNumber=int(attr[2])
                                npat=npat+1
                            if(attr[1]=='ErrorNumber'):
                                ntuple.ErrorNumber=int(attr[2])
                                npat=npat+1
                            if(attr[1]=='Run'):
                                ntuple.Run=int(attr[2])
                                npat=npat+1
                        if(self.dbclient.ct(ntuple.Type)=="RootFile" and ntuple.Status == self.dbclient.iorp.Validated and npat==14):
                            fn.append(ntuple)
                            if(update):
                                self.dbclient.iorp.sendDSTEnd(self.dbclient.cid,ntuple,self.dbclient.tm.Create)

                fltdvo.close() 
            except IOError,e:
                print e
        return fn
        
        
    def transferDataFiles(self, run2p, i, v, u, h, source, c, replace, disk, eos):
        self.ConnectDB(1)
        if (not self.sqlconnected):
            raise ConnectionFailRemoteClientException()

        if(os.environ.has_key('RunsDir')):
            runsdir=os.environ['RunsDir']
        else:
            ret = self.sqlserver.sql("SELECT MYVALUE FROM ENVIRONMENT WHERE MYKEY=:mykey", mykey="RunsDir")
            if(len(ret)>0):
                runsdir=ret[0][0]
            else:
                runsdir="/afs/cern.ch/ams/Offline/RunsDir"

        global mutex
        mutex=thread.allocate_lock()
        mutex.acquire()

        self.checkEOS()

        self.v=v
        self.eos=eos
        self.BadCRC=[0]
        self.CheckedDSTs=[0]
        self.BadDSTs=[0]
        self.GoodDSTs=[0]
        self.BadDSTCopy=[0]
        self.doCopyTime=0
        self.doCopyCalls=0
        self.fastntTime=0
        self.fastntCalls=0
        self.crcTime=0
        self.crcCalls=0
        self.copyTime=0
        self.copyCalls=0
        self.nCheckedCite=0
        self.nBadCopiesInRow=0
        self.CheckedRuns=[0]
        self.GoodRuns=[0]
        self.BadRuns=[0]
        self.skipcrc=0
        delay=86400
        joudir = "%s/jou" % source
        filesfound=0
        timenotify=0
        while 1:
            for filej in os.listdir(joudir):
                if(filej.find(".jou.0")>=0):
                    notify=0
                if(filej.find(".jou.")>=0):
                    continue
                if(filej.find(".jou")<0):
                    continue

                pfilej = os.path.join(joudir,filej)
                timenow = time.time()
                file = filej.split(".jou")[0]
                pfile = os.path.join(source, file)
                if(not os.path.isfile(pfile)):
                    self.logger.error("File not found %s", pfile)
                    continue

                journalFile = JournalFile.JournalFile(pfilej)
                if journalFile.ok():
                    self.logger.info("Run %d", journalFile.run)

                    if(run2p!=0 and journalFile.run!=run2p):
                        continue

		    if(abs(journalFile.tfevent - journalFile.run) > 3600):
                        self.logger.error("TransferDataRuns-E-WrongRun %d %d", journalFile.run, journalFile.tfevent)
                        cmd = "mv %s %s.0" % (journalFile.file_path, journalFile.file_path)
                        self._cmd(cmd)
                        continue

                    ret = self.sqlserver.sql("SELECT PATH,RUN,NEVENTS,TYPE,FEVENT,LEVENT,PATHB FROM DATAFILES WHERE PATH LIKE '%' || :path", path=file);
                    calibnotfull=False
                    if(len(ret)>0):
                        orig=journalFile.f1.split('/')
                        origpath=""
                        if(len(orig)>2):
                             origpath=origpath+orig[len(orig)-3]+"/"+orig[len(orig)-2]+"/"+orig[len(orig)-1] 
                        orig=journalFile.f2.split('/')
                        if(len(orig)>2):
                             origpath=origpath+" "+orig[len(orig)-3]+"/"+orig[len(orig)-2]+"/"+orig[len(orig)-1] 

                        if(ret[0][3].find("CAL")>=0 and (ret[0][2]<journalFile.events or (ret[0][2]==journalFile.events and ret[0][6] != origpath))):
                           calibnotfull=True
                        if(ret[0][3].find("SCI")>=0 and (ret[0][2]<journalFile.events or (ret[0][2]==journalFile.events and ret[0][6] != origpath) ) and ret[0][4]>=journalFile.fevent and ret[0][5]<=journalFile.levent):
                           calibnotfull=True

                        if((calibnotfull or replace) and (run2p==0 or ret[0][1] == run2p) and (disk==None or ret[0][0].find(disk)>=0)):
                            fd = ret[0][0] 
                            cmd = "rm -rf %s" % fd
                            i = self.__cmd(cmd)
                            if(i):
                                self.logger.error("command failed '%s'", " ".join(cmd))
                                if(replace>1):
                                    self.sqlserver.sql("DELETE FROM DATAFILES WHERE PATH=:path", path=fd)
                                else:
                                    continue
                            else:
                                self.sqlserver.sql("DELETE FROM DATAFILES WHERE PATH=:path", path=fd)
                        else:
                            continue

                    (outputpath, ret, castortime) = self.__doCopyRaw(journalFile, pfile, '/Data')
                    if(ret==1):
                        sizemb = journalFile.size/2
                        type="UNK"
                        if(journalFile.t0=="5"):
                            type="SCI"
                            try:
                               itag = int(journalFile.tag)%256
                            except ValueError:
                               itag=0
                            if(itag==204):
                               type="LAS"

                        elif (journalFile.t0=="6"):
                            type="CAL"
                            journalFile.fevent = 1
                            journalFile.levent = journalFile.events

                        elif (journalFile.t0=="7"):
                            type="CMD"

                        elif (journalFile.t0=="8"):
                            type="HKD"

                        elif (journalFile.t0=="27" or journalFile.t0=="896"):
                            type="CAB"

                        bpath = "%s/%s" % (runsdir, type)
                        if(calibnotfull):
                            type = "%s MERGED " % type

                        if(type!="UNK"):
                            isdir = journalFile.run/1000000;
                            isdir = isdir*1000000;
                            sdir = "/%d" %(isdir)
                            bpath = bpath+sdir;
                            cmd = "mkdir %s" % bpath
                            i = self.__cmd(cmd)
                            if(i):
                                # we should not continue loop?
                                self.logger.error("command failed '%s'", " ".join(cmd))

                        cmd = "ln -sf %s %s" % (outputpath, bpath)
                        i = self.__cmd(cmd)
                        if(i):
                            # we should not continue loop?
                            self.logger.error("command failed '%s'", " ".join(cmd))

                        bpath = "%s/%s" % (bpath, file)
                        if(type=="SCI" or type=="LAS"):
                            type = "%s %s %s" % (type, journalFile.t0, journalFile.t1)
                            mnemonic = " TUSREL3";
                            for i in range (0,7):
                                it = (int(journalFile.t2)>>(i*4))&15;
                                if(it>0):
                                    type=type+mnemonic[i]
                                    type="%s%d" % (type, it)
                        else:
                            type = "%s %s %s %s %s" % (type, journalFile.t0, journalFile.t1, journalFile.t2, journalFile.t3)

                        status='OK'
                        if(type.find('SCI')>=0 and journalFile.tlevent - journalFile.tfevent < 60):
                            status='SHORT'

                        if (outputpath.find('/eosams/') >= 0):
                            eostime = time.time()
                        else:
                            eostime = 0

                        result = self.sqlserver.sql(
                            "INSERT INTO DATAFILES \
                            (RUN,VERSION,TYPE,FEVENT,LEVENT,NEVENTS,NEVENTSERR,TIMESTAMP,SIZEMB,STATUS,PATH,PATHB,CRC,CRC32,CRCTIME,CASTORTIME,BACKUPTIME,TAG,FETIME,LETIME,PATHS,EOSTIME) \
                            VALUES \
                            (:run,:version,:type,:fevent,:levent,:nevents,:neventserr,:timestamp,:sizemb,:status,:path,:pathb,:crc,:crc32,:crctime,:castortime,:backuptime,:tag,:fetime,:letime,:paths,:eostime)",
                            run = journalFile.run,
                            version = journalFile.version,
                            type = type,
                            fevent = journalFile.fevent,
                            levent = journalFile.levent,
                            nevents=journalFile.events,
                            neventserr = journalFile.errors,
                            timestamp = journalFile.rtime,
                            sizemb = sizemb,
                            status = status,
                            path = outputpath,
                            pathb = self.__getOrigpath(journalFile),
                            crc = journalFile.crc,
                            crc32 = journalFile.crc32,
                            crctime = int(timenow),
                            castortime = castortime,
                            backuptime = 0,
                            tag = journalFile.tag,
                            fetime = journalFile.tfevent,
                            letime = journalFile.tlevent,
                            paths = bpath,
                            eostime = eostime,
                        )
                        self.sqlserver.sql("COMMIT")

                        cmd = "rm -rf %s" % pfile
                        self.__cmd(cmd)

                        cmd = "mv %s %s.1" % (pfilej, pfilej)
                        self.__cmd(cmd)

                        notify=0
                        filesfound+=1
                    else:
                        if(outputpath!=None):
                            cmd = "rm %s" % outputpath
                            self.logger.error("TransferDataFiles-ECopyFail, run '%s'", cmd)
                            time.sleep(60) 
                            if(not os.path.islink(outputpath) and not os.path.isdir(outputpath)):
                                self.__cmd(cmd)

            if(c==0 or run2p!=0):
                break
            else:
                time.sleep(60)
                if(notify>0 and filesfound>0):
                    message="%d New DataRuns Transferred " % filesfound
                    self.NotifyResp(message)
                    filesfound = 0
#                    suc=self.dbserver.CreateRun(run,fevent,levent,tfevent,tlevent,0,1,outputpath)
#                    if(suc==1):
#                        self.sqlserver.Commit(1)
#                    else:
#                        self.sqlserver.Commit(0)



    def __getOrigpath(self, journalFile):
        orig = journalFile.f1.split('/')
        origpath = ""
        if(len(orig)>3):
            origpath = "%s/%s/%s/%s" % (orig[len(orig)-4], orig[len(orig)-3], orig[len(orig)-2], orig[len(orig)-1])

        orig = journalFile.f2.split('/')
        if(len(orig)>3):
            origpath = "%s %s/%s/%s/%s" % (origpath, orig[len(orig)-4], orig[len(orig)-3], orig[len(orig)-2], orig[len(orig)-1])

        return origpath


    def linkdataset(self,path,sdir,crdel):
        rmpath=path.split('/')
        newdir=sdir
        file=sdir      
        newfile=sdir      
        dir=sdir
        newdir=sdir
        for i in range (2,len(rmpath)):
                if(rmpath[i].find('.root')>=0):
                    rmrun=rmpath[i].split('.')
                    isdir=int(rmrun[0])/1000000;
                    isdir=isdir*1000000;
                    fdir="%d" %(isdir)
                    newfile=newfile+"/"+fdir
                    newdir=newdir+"/"+fdir
                file=file+"/"+rmpath[i]
                newfile=newfile+"/"+rmpath[i]
                if(i<len(rmpath)-1):
                    dir=dir+"/"+rmpath[i]
                    newdir=newdir+"/"+rmpath[i]
        self.__cmd("mkdir -p "+newdir)
        cmd=""
        if(crdel==1):
                if(path.find('/castor/cern.ch/ams')>=0):
                    path.replace('/castor/cern.ch/ams','/castor',1);
                cmd="ln -sf "+path+" "+newdir
                
        else:
                cmd="rm "+file
                i=self.__cmd(cmd)
                cmd="rm "+newfile
        i=self.__cmd(cmd)
        if(i):
                print "Problem with "+cmd
        return newfile


    def findJob(self, jid, buf, dir, cid):
        rstatus = 0
        sql = "SELECT jid,mid FROM Jobs WHERE jid=%d" %(jid)
        ret = self.sqlserver.Query(sql)
        if (len(ret)):
            rstatus = ret[0][0]
        else:
            sql = "SELECT jid,mid FROM Jobs_deleted WHERE jid=%d" %(jid)
            ret = self.sqlserver.Query(sql)
            if (len(ret)):
                rstatus = ret[0][0]
                print "Job %d restored from Jobs_deleted" %(jid)
                sql = "insert into jobs select * from jobs_deleted where jobs_deleted.jid=%d" %(jid)
                self.sqlserver.Update(sql)
                sql = "delete from jobs_deleted where jid=%d" %(jid)
                self.sqlserver.Update(sql)
                self.sqlserver.Commit()
            else:
#
#        got things from root file itself
#
                CloseDSTPatterns = {
                    'CloseDST' : None,
                    'Status' : None,
                    'Type' : None,
                    'Name' : None,
                    'Version' : 0,
                    'Size' : 0,
                    'crc' : 0,
                    'Insert' : 0,
                    'Begin' : 0,
                    'End' : 0,
                    'Run' : 0,
                    'FirstEvent' : 0,
                    'LastEvent' : 0,
                    'EventNumber' : 0,
                    'ErrorNumber' : 0
                }
                if (buf == ""):
                    joudir = ''
                    sql = "select dirpath from journals where cid=%d" %(cid)
                    ret = self.sqlserver.Query(sql)
                    if (ret[0][0] != None):
                        joudir = ret[0][0]
                    else:
                        return 0
                    try:
                        allfiles=os.listdir(joudir)
                    except:
                        print "Enable to open ",joudir
                        return 0
                    for file in allfiles:
                        if(file.find(".journal")>0 and re.match("\.journal", file)):
                            try:
                                input=open(joudir + '/' + file,'r')
                            except IOError,e:
                                print e
                                return 0
                            buf=input.read()
                            input.close()
                            break
                if (buf == ""):
                    return 0
                blocks = buf.split("-I-TimeStamp ")
                junk = blocks[1].split()
                ctime = junk[0]
                for line in blocks:
                    if (line.find("CloseDST") >= 0):
                        pat = line.split(' , ')
                        for entry in pat:
                            kv = entry.split()
                            if (len(kv)>1):
                                CloseDSTPatterns[kv[0]] = kv[1]
                        break
                if (CloseDSTPatterns['Type'] == 'RootFile'):
                    if (CloseDSTPatterns['Name'] != None):
                        jhost = CloseDSTPatterns['Name'].split(':')
                        host = jhost[0]
                        junk = CloseDSTPatterns['Name'].split('/')
                        fnam = jhost[1]                                   
                        if (dir != None):
                            fnam = dir + '/' + junk[len(junk)-1]
                        validatecmd = self.env['AMSSoftwareDir']+"/exe/linuxfastntrd64.exe %s 0 2 0 1" %(fnam)
                        self.__cmd(validatecmd)
                        fnam += '.jou'
                        self.__cmd("sed -i \"s/\\.job.*/.job/g\" %s" %(fnam))
                        bufj = ''
                        try:
                            jouf = open(fnam, 'r')
                        except IOError, e:
                            print e
                            return 0
                        bufj = jouf.read()
                        jouf.close()
                        os.unlink(fnam)
                        job = {}
                        cont = bufj.split("\n")
                        pats = ("DATASETNAME","ScriptName","TRIG","NICKNAME","SUBMITTIME")
                        job['TRIG'] = 100000000
                        for line in cont:
                            for pat in pats:
                                if (line.find("%s=" %(pat)) >= 0):
                                    junk = line.split('=')
                                    if (pat == 'ScriptName'):
                                        j2 = junk[1].split('/')
                                        job[pat] = trimblanks(j2[len(j2)-1])
                                    else:
                                        if (len(junk) > 1):
                                            job[pat] = trimblanks(junk[1])
                        ok = 1
                        for pat in pats:
                            if (job[pat] is None):
                                ok = 0
                                break
                        if (re.match("^\d+$", ctime)):
                            ctime = job['SUBMITTIME']
                        sql = "select did from datasets where name = %s" %(job['DATASETNAME'])
                        ret = self.sqlserver.Query(sql)
                        if (ok and ret[0][0] != None):
                            did = ret[0][0]
                            sql = "select mid from mails where cid = %d" %(cid)
                            ret = self.sqlserver.Query(sql)
                            pid = self.getProductionSetIdByDatasetId(did)
                            bufj.replace('$', '\$')
                            bufj.replace('"', '\\"')
                            bufj.replace('(', '\(')
                            bufj.replace(')', '\)')
                            bufj.replace('\\', '\\\\')
                            if (self.sqlserver.dbdrive.find('Oracle') >= 0):
                                bufj.replace("'", "''")
                            insertjobsql = "INSERT INTO Jobs VALUES (%d, %s, %d, %d, %d, %d, %d, 864000, %s, %d, %s, %s, 0, 0, 0, 0, 'STANDALONE', -1, %d, -1, 0)" %(jid, job['ScriptName'], ret[0][0], cid, did, job['SUBMITTIME'], job['TRIG'], bufj, ctime, job['NICKNAME'], host, pid)
                            self.sqlserver.Update(insertjobsql)
                            self.sqlserver.Commit()
                            sql = "SELECT jid,mid FROM Jobs WHERE jid = %d" %(jid)
                            ret = self.sqlserver.Query(sql)
                            if (ret[0][0] != None):
                                rstatus = ret[0][0]
                                print "  Job %d restored from ntuple" %(jid)
        return rstatus


    def getProductionPeriods(self, flag):
        i = 0
        sql = "SELECT DID, NAME, BEGIN, END, STATUS, VGBATCH, VDB, VOS  FROM ProductionSet ORDER by begin desc"
        ret = self.sqlserver.sql(sql)
        if (ret[0][0] != None):
            for p in ret:
                period = {}
                period['id']      = p[0]
                period['name']    = self.trimblanks(p[1])
                period['begin']   = p[2]
                period['end']     = p[3]
                period['status']  = self.trimblanks(p[4])
                period['vgbatch'] = self.trimblanks(p[5])
                period['vdb']     = self.trimblanks(p[6])
                period['vos']     = self.trimblanks(p[7])
                self.productionPeriods.append(period)
                if (flag == 1):
                    print " %5d %15s %10d %10d %12s %6s %6d %6s" %(period['id'], period['name'], period['begin'], period['end'], period['status'], period['vdb'], period['vgbatch'], period['vos'])
                i += 1

    def setLogger(self, logger):
        self.logger = logger

    def setCmd(self, cmd):
        self.cmd = cmd

    def __log_info(self, str):
        if self.logger == None:
            print stf
        else:
            self.logger.info(str)

    def __log_error(self, str):
        if self.logger == None:
            print stf
        else:
            self.logger.error(str)

    def __cmd(self, cmd):
        self.logger.debug("CMD='%s'", cmd)
        #return os.system(cmd)
        return 0

    def __cmd_output(self, cmd):
        self.logger.debug("CMD='%s'", cmd)
        return commands.getstatusoutput(cmd)
