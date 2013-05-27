import sys,os,time,string,re,thread,smtplib,commands,math,socket
from stat import *
from DBSQLServer import DBSQLServer
from DBServer import DBServer
import cgi
import datetime
from array import *
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
    def getior(self,datamc=0):
        if self.sqlconnected==-1:
            self.ConnectDB()
        if(self.sqlconnected==1):
            sql="select IORS,IORP,IORD,dbfilename from Servers where status='Active' and datamc=%d order by lastupdate desc" %(datamc)
            ret=self.sqlserver.Query(sql)
            if(len(ret)>0):
                return ret[0][0],ret[0][2]
            else: return ""
    def ConnectDB(self,one=0):
        self.sqlserver=DBSQLServer(sys.argv,one)
        self.sqlconnected=self.sqlserver.Connect()
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
        self.CheckFS(1,600,"")
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
            rstatus=os.system(crccmd)
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
        sql ="select address from mails where rserver=1"
        ret_a=self.sqlserver.Query(sql)
        if(len(ret_a[0][0])):
            address=ret_a[0][0]
            self.sendmailmessage(address,message," ")

         
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
        self.CheckFS(1,300,'/')
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
            rstatus=os.system(crccmd)
            mutex.acquire()
            rstatus=(rstatus>>8)
            if rstatus !=1:
                if self.v:
                    print ntuple[0]," crc error: ",rstatus
                    self.sendmailmessage(address,"crc failed"+ntuple[0]," ")
                    self.ntpb=self.ntpb+1
                if ntuple[2]>0:
                    #copy from castor
                    suc=1
                    junk=ntuple[0].split(delimiter)
                    if len(junk)>1:
                        castornt=castorPrefix+"/"+delimiter+junk[1]
                        sys=rfcp+" "+castornt+" "+ntuple[0]+".castor"
                        mutex.release()
                        i=os.system(sys)
                        mutex.acquire()
                        if(i):
                            suc=0
                            if(self.v):
                                print sys," failed for ",castornt
                            self.ntpf=self.ntpf+1
                            os.system("rm "+ntuple[0]+".castor")
                            continue
                        else:
                            crccmd="/Offline/AMSDataDir/DataManagement/exe/linux/crc "+ntuple[0]+".castor "+str(ntuple[1])
                            mutex.release()
                            rstatus=os.system(crccmd)
                            mutex.acquire()
                            rstatus=(rstatus>>8)
                            if rstatus !=1:
                                if self.v:
                                    self.sendmailmessage(address,"crc castor failed"+castornt," ")
                                    print castornt," castor crc error: ",rstatus
                            if(suc):
                                if(self.update):
                                    os.system("mv "+ntuple[0]+".castor "+ntuple[0])
                                else: os.system("rm "+ntuple[0]+".castor")
                            else:
                                os.system("rm "+ntuple[0]+".castor")
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
                                      i=os.system(sys)
                                      if(i): self.sendmailmessage(address,"unable to "+sys," ")
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
                                      i=os.system(sys)
                                      if(i): self.sendmailmessage(address,"unable to "+sys," ")
                    else: self.sqlserver.Commit(0)

            mutex.release()
        exitmutexes[fs].acquire()

    def CheckFS(self,updatedb=0,cachetime=60,path='/',vrb=0):
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
        hostname=socket.gethostname()
        if(path== ""):
           sql="select disk from filesystems where isonline=1 and status='Active'  order by available desc" 
        else:
           sql="select disk from filesystems where isonline=1 and status='Active' and path like '%s%%' order by available desc" %(path)
        ret=self.sqlserver.Query(sql);
        if(updatedb<2 and time.time()-cachetime < self.dbfsupdate() and len(ret)>0 ):
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
                   print stf
               os.system("ls "+fs[0]+" 1>"+stf+" 2>&1 &")
               time.sleep(1)
               stat=[0,0,0,0,0,0,0,0,0,0,0]
               try:
                   stat =os.stat(stf)
                   os.unlink(stf)
               except:
                   print stf, " Failed to stat " 
               if stat[ST_SIZE]==0:
                   sql="update filesystems set isonline=0 where disk='"+str(fs[0])+"'"
                   if(self.v):
                       print stf," Is Offline"
                   if updatedb!=0:
                       self.sqlserver.Update(sql)
                       self.sqlserver.Commit()
                   continue
               res=""
               try:
                   res=os.statvfs(os.path.realpath(fs[0]))
               except:
                   print fs[0]," Is Offline"
               if len(res) == 0:
                   isonline=0
               else:
                   timestamp=int(time.time())
                   bsize=res[1]
                   blocks=res[2]
                   bfree=res[3]
                   bavail=res[4]
                   files=res[5]
                   ffree=res[6]
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
                    sql="update filesystems set isonline=0 where disk='"+fs[0]+"'"
                   if updatedb>0:
                    self.sqlserver.Update(sql)
                    self.sqlserver.Commit()
        if(path == ""):
           sql="select disk from filesystems where isonline=1 and status='Active'  order by available desc" 
        else:
           sql="select disk from filesystems where isonline=1 and status='Active' and path='%s' order by available desc" %(path)
        ret=self.sqlserver.Query(sql)
        if(len(ret)<=0):
           self.sendmailmessage('vitali.choutko@cern.ch','FileSystems are  Full or Offline Python',sql)
           sql="select path from ntuples where sizemb>20000"
           ret=self.sqlserver.Query(sql)
           for disk in ret:
               try:
                   fs=os.stat(disk[0])[ST_SIZE]
                   fs=int(fs/1000/1000+0.5)
                   if(fs<20000):
                       sql="update ntuples set sizemb=%d where path like '%s' " %(fs,disk[0])
                       self.sqlserver.Update(sql)
               except:
                   print "unable to stat",disk[0]
           sql="select disk from filesystems where isonline=1 and status='Full' and path='%s' order by totalsize-occupied desc" %(path)
           ret=self.sqlserver.Query(sql)
           if(len(ret)<=0):
               self.sendmailmessage('vitali.choutko@cern.ch,baosong.shan@cern.ch','FileSystems are  Offline: Exited!!!',sql)
               return None
        return ret[0][0]

    def  dblupdate(self):
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

    def dbfsupdate(self):
           part=0
           sql="select sessiontimezone from dual"
           got=self.sqlserver.Query(sql)
           if len(got)>0:
              sh1=string.split(got[0][0], ':')
              if( len(sh1)>1):
                  part=(float(sh1[0])+float(sh1[1])/60.)/24.
           sql="select max(date_to_unixts(modtime-"+str(part)+")) as u_modtime from mods where tabname = 'fs'"
           rdt=self.sqlserver.Query(sql)
           lupdate=-1
           if len(rdt)>0: lupdate=rdt[0][0]
           return lupdate

    def DiskIsOnline(self,disk):
           self.CheckFS(1,300,'/')
           sql="select disk from filesystems where isonline=1 and disk like '"+disk+"'"
           ret=self.sqlserver.Query(sql)
           if len(ret)>0: return 1
           return 0
    def sendmailmessage(self,add,sub,mes="",att=""):
        import mimetypes
        from email import encoders
        from email.message import Message
        from email.mime.base import MIMEBase
        from email.mime.image import MIMEImage
        from email.mime.multipart import MIMEMultipart
        from email.mime.text import MIMEText
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
               
    def ValidateRuns(self,run2p,i,v,d,h,b,u,mt,datamc=0,force=0,nfs=0,castoronly=0):
        self.s=""
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
        timenow=int(time.time())
        global rflag
        rglag=0
        self.validated=0
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
        if(mt==1):
            self.mt=1
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
            self.setprocessingflag(1,timenow,1)
        global vdir,vlog,output
        vdir=self.getValidationDir()
        vlog=vdir+"validateRuns.log."+str(timenow)
        try:
            output=open(vlog,'w')
            print "ValidateRuns-I-Open ",vlog
        except IOError,e:
            print e
            self.setprocessingflag(0,timenow,1)
            return 0
        global vst
        vst=timenow
        ret=self.sqlserver.Query("SELECT min(begin) FROM productionset WHERE STATUS='Active' ORDER BY begin")
        if (len(ret)==0):
            print "ValidateRuns=E-CannotFindActiveProductionSet"
            self.setprocessingflag(0,timenow,1)
            return 0
        firstjobtime=ret[0][0]-24*60*60
        global exitmutexes
        exitmutexes = {}
        global mutex
        mutex=thread.allocate_lock()
        global fsmutexes
        fsmutexes = {}
        maxt=5
        rtb_tuples=sorted(self.dbclient.rtb,key=lambda rtb: rtb.Run,reverse=True)
        for run in rtb_tuples:
            run2p=self.run2p
            if((run2p!=0 and run2p!=run.Run) and not(run2p<0 and run.Run>-run2p) and not(run2p/10000000000>0 and run.Run<=(run2p%10000000000))):
                continue
            self.CheckedRuns[0]=self.CheckedRuns[0]+1
            if( run.DataMC==datamc and mt==1 ):
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
        self.setprocessingflag(0,timenow,1)
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
        sql="SELECT myvalue FROM Environment WHERE mykey='ValidationDirPath'"
        ret=self.sqlserver.Query(sql)
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
                                if( not os.path.isfile(fpath) and fpath.find('/castor/cern.ch')<0):
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
                                               i=os.system(cmd)
                                               cmd="scp -2 %s:%s %s " %(host,fpath,fpath)
                                               if(dir[0].find('/dat0/')>=0):
                                                   mutex.release()
                                                   i=os.system(cmd)
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
                                        if(not os.path.isfile(fpath) and fpath.find('/castor/cern.ch')<0):
                                            print "unable to open file ",fpath
                                retcrc=self.calculateCRC(fpath,ntuple.crc)
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
                                                        self.InsertNtuple(run.Run,ntuple.Version,self.dbclient.ct(ntuple.Type),run.Run,ntuple.FirstEvent,ntuple.LastEvent,events,badevents,ntuple.Insert,ntuple.size,status,outputpath,ntuple.crc,ntuple.Insert,1,0,run.DataMC)
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
                                                            rstat=os.system(cmd)
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
                                 os.system(cmd)
                                 print "Validation failed "+cmd
                                 output.write("Validation failed "+cmd)
                                 self.failedcp=self.failedcp+1
                                 self.copied=self.copied-1
                             sql="delete ntuples where jid=%d" %(run.uid)
                             self.sqlserver.Update(sql)
                             runupdate="update runs set "
                             
                        else:
                            status="Unchecked"
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
                            rmpath=ntuple.split('/')
                            rmdir="rm -r "
                            if(ntuple.find('/castor/cern.ch')>=0):
                                cmd="rfrm "+ntuple;
                                rmdir="nsrmdir  "
                            for i in range (1,len(rmpath)-1):
                                rmdir=rmdir+"/"+rmpath[i]
                            for mn in mvntuples:
                                if(ntuple == mn):
                                    cmd=" "
                                    break
                            os.system(cmd)
                            warn=" validation done "+cmd
                           
                            output.write(warn)
                            print warn
                        
                        if(status =="Completed"):
                            self.updateRunCatalog(run.Run)
                            warn=" Update RunCatalog table "+str(run.Run)
                            for cmd in rmcmd:
                                i=os.system(cmd)
                                if(i):
                                    print "Remote command ",cmd," failed"
                            del rmcmd[:]
                            for cmd in rmbad:
                                i=os.system(cmd)
                                if(i):
                                    print " remove bad ntuples command failed ",cmd
                            del rmbad [:]
        del cpntuples[:]
        del mvntuples[:]
        os.system(rmdir)
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
                                if( not os.path.isfile(fpath) and fpath.find('/castor/cern.ch')<0):
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
                                               i=os.system(cmd)
                                               cmd="scp -2 %s:%s %s " %(host,fpath,fpath)
                                               if(dir[0].find('/dat0/')>=0):
                                                   mutex.release()
                                                   i=os.system(cmd)
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
                                        if(not os.path.isfile(fpath) and fpath.find('/castor/cern.ch')<0):
                                            print "unable to open file ",fpath
                                retcrc=self.calculateCRC(fpath,ntuple.crc)
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
                                                        self.InsertNtuple(run.Run,ntuple.Version,self.dbclient.ct(ntuple.Type),run.uid,ntuple.FirstEvent,ntuple.LastEvent,events,badevents,ntuple.Insert,ntuple.size,status,outputpath,ntuple.crc,ntuple.Insert,1,castortime,run.DataMC)
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
                                                            rstat=os.system(cmd)
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
                                     os.system(cmd)
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
                            rmpath=ntuple.split('/')
                            rmdir="rm -r "
                            if(ntuple.find('/castor/cern.ch')>=0):
                                cmd="rfrm "+ntuple;
                                rmdir="nsrmdir  "
                            for i in range (1,len(rmpath)-1):
                                rmdir=rmdir+"/"+rmpath[i]
                            for mn in mvntuples:
                                if(ntuple == mn):
                                    cmd=" "
                                    break
                            os.system(cmd)
                            warn=" validation done "+cmd
                            output.write(warn)
                            print warn
                        
                        if(status =="Completed"):
                            for cmd in rmcmd:
                                i=os.system(cmd)
                                if(i):
                                    print "Remote command ",cmd," failed"
                            del rmcmd[:]
                            for cmd in rmbad:
                                i=os.system(cmd)
                                if(i):
                                    print " remove bad ntuples command failed ",cmd
                            del rmbad [:]
        del cpntuples[:]
        del mvntuples[:]
        os.system(rmdir)
        #print "run finished ",run.Run,run.uid
	self.sqlserver.Commit()
        mutex.release()
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
    def setprocessingflag(self,flag,timenow,which):
        sql="Update FilesProcessing set flag="+str(flag)+",timestamp="+str(timenow)
        if(which==1):
            sql="Update FilesProcessing set flaglocal="+str(flag)+",timestamplocal="+str(timenow)

        self.sqlserver.Update(sql)
        self.sqlserver.Commit()


    def getValidationDir(self):
        sql="SELECT myvalue FROM Environment WHERE mykey='ValidationDirPath'"
        return self.sqlserver.Query(sql)[0][0]

    def ServerConnect(self,datamc=0):
        try:
            (ior,iord)=self.getior(datamc)
            self.dbclient=DBServer(ior,iord)
            return 1
        except:
            print "Problem to ConnectServer "
            return 0
    
    def doCopy(self,run,inputfile,crc,version,outputpath,path='/MC'):
       self.sqlserver.Commit()
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
       if(inputfile.find('/castor/cern.ch')<0):
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
                   self.setprocessingflag(0,timenow,1)
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
                   self.setprocessingflag(0,timenow,1)
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
                       cmdstatus=os.system(cmd)
                       if(cmdstatus==0):
                           outputpath=outputpath+"/"+file
                           cmdstatus=self.copyFile(inputfile,outputpath)
                           time00=time.time()
                           self.doCopyTime=self.doCopyTime+time00-time0
                           if(cmdstatus==0 or cmdstatus==1024):
                               if(cmdstatus==1024):
                                   rstatus=1
                               else:
                                   rstatus=self.calculateCRC(outputpath,crc)
                               if(rstatus==1):
                                   (castortime,castorpath)=self.moveCastor(inputfile,outputpath)
                                   if(self.castoronly and castortime>0 and castorpath!=None):
                                       outputpath=castorpath
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



    def doCopyRaw(self,run,inputfile,crc,path='/MC'):
       time0=time.time()
       time00=0
#       (period,prodstarttime,periodid)=self.getActiveProductionPeriodByName(path)
       (period,prodstarttime,periodid)=self.getActiveProductionPeriodByName("2011A")
       if(period == None or prodstarttime==0):
           sys.exit("Cannot find Active Production Period for path "+str(path))
       self.doCopyCalls=self.doCopyCalls+1
       junk=inputfile.split('/')
       file=junk[len(junk)-1]
       filesize=1
       if(inputfile.find('/castor/cern.ch')<0):
           filesize=os.stat(inputfile)[ST_SIZE]
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
                   (outputpatha,gb,odisk,stime)=self.getOutputPathRaw(period,path)
                   if(odisk==None):
                       print "odisk Problem to getoutputpathraw file ",path
                       return None,0,None,0
                   print "acquired:  ",outputpatha,gb,odisk,stime
               outputpath=outputpatha[:]
               if(outputpath.find('xyz')>=0 or gb==0):
                   sys.exit("doCopy-F-CannotFindAnyDisk Exit")
           else:
               junk=outputpath.split('/')
               odisk="/"+junk[1]
               outputpath=""
               for i in range (0,len(junk)-3):
                   if(i>0):
                       outputpath=outputpath+"/"
                   outputpath=outputpath+junk[i]
           dataset="RawData"
           outputpath=outputpath+'/'+dataset
           cmd="mkdir -p "+outputpath
           cmdstatus=os.system(cmd)
           if(cmdstatus==0):
               outputpath=outputpath+"/"+file
               cmdstatus=self.copyFile(inputfile,outputpath)
               time00=time.time()
               self.doCopyTime=self.doCopyTime+time00-time0
               if(cmdstatus==0):
                   rstatus=self.calculateCRC(outputpath,crc)
                   if(rstatus==1):
                       castortime=self.UploadtoCastor(outputpath)
                       return outputpath,1,castortime
                   else:
                       print "doCopyraw-E-ErorrCRC ",rstatus
                       self.BadCRC[self.nCheckedCite]=self.BadCRC[self.nCheckedCite]+1
                       return outputpath,0,0
               else:
                   self.BadDSTCopy[self.nCheckedCite]=self.BadDSTCopy[self.nCheckedCite]+1
                   print "docopyraw-E-cannot copyfile",cmdstatus,inputfile,outputpath
                   return outputpath,0,0
           else:
               self.BadDSTCopy[self.nCheckedCite]=self.BadDSTCopy[self.nCheckedCite]+1
               print "docopyraw-E-cannot ",cmd
               return None,0,0
               
       else:
           print "doCopyraw-E-cannot stat",inputfile
           self.BadDSTs[self.nCheckedCite]=self.BadDSTs[self.nCheckedCite]+1
           return None,0,0
    
    def UploadtoCastor(self,input):
#
# only one file to uploads
#
        buf=input.split('/')
        output='/castor/cern.ch/ams'
        buf2=input.split(buf[1])
        output=output+buf2[1]
        cmd="nsrm "+output
        cmdstatus=os.system(cmd)
        ossize=10000000000
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
        cmd=tmout+" /afs/cern.ch/exp/ams/Offline/root/Linux/527.icc64/bin/xrdcp "+input+" 'root://castorpublic.cern.ch//"+output+"?svcClass=amscdr'" 
        cmdstatus=os.system(cmd)
        if(cmdstatus):
            print "Error uploadToCastor via xrdcp",input,output,cmdstatus
            cmd="nsrm "+output
            cmdstatus=os.system(cmd)
        else:
            return int(time.time())
        cmd=tmout+" /usr/bin/rfcp "+input+" "+output
        cmdstatus=os.system(cmd)
        if(cmdstatus):
            print "Error uploadToCastor ",input,output,cmdstatus
            cmd="nsrm "+output
            cmdstatus1=os.system(cmd)
            if(cmdstatus):
                print "Error uploadToCastor ",input,output,cmdstatus
                return 0                        
            else:
                return int(time.time())
        else:
            return int(time.time())
        

    def getActiveProductionPeriodByName(self,name):
       ret=self.sqlserver.Query("SELECT NAME, BEGIN, DID  FROM ProductionSet WHERE STATUS='Active' and name like '%"+name+"%'")
       if(len(ret)>0):
           return self.trimblanks(ret[0][0]),ret[0][1],ret[0][2]
       else:
          return None,0,0
      
    def getActiveProductionPeriodByVersion(self,dbv):
       ret=self.sqlserver.Query("SELECT NAME, BEGIN, DID  FROM ProductionSet WHERE STATUS='Active' and vdb='"+dbv+"'")
       if(len(ret)>0):
           return self.trimblanks(ret[0][0]),ret[0][1],ret[0][2]
       else:
          return None,0,0
      
    def trimblanks(self,expr):
        return expr.lstrip().rstrip()
       
                              
    def getDSTVersion(self,version):
        junk=version.split('/')
        if(len(junk)>2):
            return self.trimblanks(junk[0]),self.trimblanks(junk[1]),self.trimblanks(junk[2])
        else:
            return self.trimblanks(junk[0]),None,None
       
    def getOutputPath(self,period,idisk,path='/Data'):
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
        rescue=self.CheckFS(1,ct,path)
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
            os.system("mkdir -p "+outputpath)
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
           
           
    def getOutputPathRaw(self,period,path='/MC'):
        #
        # select disk to be used to store ntuples
        #
        gb=0
        outputpath='xyz'
        outputdisk=None 
        rescue=self.CheckFS(1,300,path)
        hostname=socket.gethostname()
        tme=int(time.time())
        if(tme%2 ==0):
            sql="SELECT disk, path, available, allowed  FROM filesystems WHERE status='Active' and isonline=1 and path='%s' ORDER BY priority DESC, available " %(path)
        else:
            sql = "SELECT disk, path, available, allowed  FROM filesystems WHERE status='Active' and isonline=1 and path='%s' ORDER BY priority DESC, available DESC" %(path)
        if(tme%2 ==0):
            sql="SELECT disk, path, available, allowed  FROM filesystems WHERE status='Active' and isonline=1 and path='%s' and host like '%%%s%%' ORDER BY priority DESC, available " %(path,hostname)
        else:
            sql = "SELECT disk, path, available, allowed  FROM filesystems WHERE status='Active' and isonline=1 and path='%s' and host like '%%%s%%' ORDER BY priority DESC, available DESC" %(path,hostname)
        ret=self.sqlserver.Query(sql)
        if(len(ret)<=0):
            if(tme%2 ==0):
                sql="SELECT disk, path, available, allowed  FROM filesystems WHERE status='Active' and isonline=1 and path='%s' ORDER BY priority DESC, available " %(path)
            else:
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
            os.system("mkdir -p "+outputpath)
            
            mtime=os.stat(outputpath)[9]
            if(mtime != None):
                if(mtime!=0):
                    gb=disk[2]
                    break
        timew=time.time()
        print "getoutputpath raw ",outputpath,sql
        return outputpath,gb,outputdisk,time.time()-timew
           
    def moveCastor(self,input,output):
        junk=output.split('/')
        cmove='/castor/cern.ch/ams'
        if(input.find(cmove)<0):
            return 0,None   
        for i in range (2,len(junk)):
            cmove=cmove+'/'+junk[i]
        cmd="rfrename "+input+" "+cmove
        i=os.system(cmd)
        if(i==0):
            return int(time.time()),cmove
        else:
            cmdn="nsls "+cmove
            cmdstatus=os.system(cmdn)
            if(cmdstatus):
                return 0,None
            else:
                return int(time.time()),cmove
                
    def getmoveCastor(self,input,output):
        junk=output.split('/')
        cmove='/castor/cern.ch/ams'
        if(input.find(cmove)<0):
            return None   
        for i in range (2,len(junk)):
            cmove=cmove+'/'+junk[i]
        cmdn="nsls "+cmove
        cmdstatus=os.system(cmdn)
        if(cmdstatus):
            return None
        else:
            return cmove
    
    def copyFile(self,input,output):
        mutex.release()
        if(input == output):
            print "acquirung  mutex in copyfile ups"
            mutex.acquire()
            return 0
        time0=time.time()
        cmd="cp -p -d -v "+input+" "+output
        if(input.find('/castor/cern.ch')>=0):
            # check if already been put in place
            cmdn="nsls "+input;
            cmdstatus=os.system(cmdn);
            if(cmdstatus):
                print "copyFile-W-Failed ",cmdn
                cmove=self.getmoveCastor(input,output)
                if(cmove!=None):
                    input=cmove

            cmd="rfcp "+input+" "+output
            if(self.castoronly):
                mutex.acquire()
                return 1024
#
#      check if same disk
#
        ifa=input.split("/")
        ofa=output.split("/")
        if(ifa[1]==ofa[1]):
                cmd="mv "+input+" "+output
        #cmd="cp -pi -d -v "+input+" "+output
        cmdstatus=os.system(cmd)
        print "acquirung  mutex in copyfile ", cmd
        mutex.acquire()
        print "got  mutex in copyfile ", cmd
        if ( self.v ):
            print "docopy-I-",cmd,cmdstatus
        self.copyCalls=self.copyCalls+1
        self.copyTime=self.copyTime+time.time()-time0
        if(cmdstatus==0 and ifa[1]==ofa[1]):
            cmdstatus=1024
        return cmdstatus


    def updateRunCatalog(self,run):
        timestamp=int(time.time())
        sql = "SELECT runs.status, jobs.content FROM Runs, Jobs WHERE runs.jid=jobs.jid and jobs.jid="+str(run)
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
                for line in sbuf:
                    if(line.find("generate")>=0):
                        line=line.replace("C ","",1)
                        line=line.replace("\\","",1)
                        line=line.replace("\\","",1)
                        line=self.trimblanks(line)
                        sql0=sql0+", jobname"
                        sql1=sql1+", '"+line+"'"
                        break
                sql0=sql0+", TIMESTAMP) "
                sql1=sql1+", "+str(timestamp)+")"
                sql=sql0+sql1
                self.sqlserver.Update(sql)
            else:
                print "UpdateRunCatalog-E-CannitFindRunJobContent with jid=",run
    
              
    def InsertNtuple(self,run,version,type,jid,fevent,levent,events,errors,timestamp,size,status,path,crc,crctime,crcflag,castortime,datamc):
        if(type=="RawFile" and datamc==0):
#            paths="/Offline/RunsDir/MC/";
#            cmd="ln -sf %s %s" %(path,paths)
#            os.system(cmd)
            paths=self.linkdataset(path,"/Offline/RunsDir",1)
            sql="select fetime,letime from runs where jid=%d" %(jid)
            ret=self.sqlserver.Query(sql)
            fetime=0
            letime=0
            if(len(ret)>0):
                fetime=ret[0][0]
                letime=ret[0][1]
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
            sql=" insert into datafiles values(%d,'%s','%s',%d,%d,%d,%d,%d,%s,'%s','%s',' ',%d,%d,%d,0,%s,%d,%d,'%s')" %(run,version,stype,fevent,levent,events,errors,timestamp,sizemb,status,path,crc,crctime,castortime,part,fetime,letime,paths)
            self.sqlserver.Update(sql)
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
            ntsize=float(ossize)/1000/1000
            print "size Changed To ",ntsize," for ",path
        sizemb="%.f" %(ntsize)
        sql="delete from ntuples where path='%s'" %(path)
        self.sqlserver.Update(sql);
        sql = "INSERT INTO ntuples VALUES( %d, '%s','%s',%d,%d,%d,%d,%d,%d,%s,'%s','%s',%d,%d,%d,%d,%s,%d)" %(run,version,type,jid,fevent,levent,events,errors,timestamp,sizemb,status,path,crc,crctime,crcflag,castortime,buildno,datamc)
        self.linkdataset(path,"/Offline/DataSetsDir",1)
        self.sqlserver.Update(sql)
   
    def setenv(self):
        if(os.environ.has_key('AMSDataDirRW')):
            dir=os.environ['AMSDataDirRW']
        else:
            dir="/afs/cern.ch/ams/Offline/AMSDataDir"
            os.environ['AMSDataDirRW']=dir
        self.env['AMSDataDir']=dir
        key='AMSSoftwareDir'
        sql="select myvalue from Environment where mykey='"+key+"'";
        ret=self.sqlserver.Query(sql)
        if( len(ret)>0):
            self.env[key]=self.env['AMSDataDir']+"/"+ret[0][0]
        else:
            print "AMSSoftwareSir Not Defined "
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
            validatecmd="/exe/linux/fastntrd64.exe %s%s %d %d %d " %(prefix,fname,nevents,dtype,levent)
            validatecmd=self.env['AMSSoftwareDir']+validatecmd
            validatecmd="/afs/cern.ch/ams/local/bin/timeout --signal 9 600 "+validatecmd
            vcode=os.system(validatecmd)
            if(fname.find('/castor/cern.ch')>=0 and vcode/256==134):
                time.sleep(5)
                vcode=os.system(validatecmd)
            print "acquirung  mutex in validate", validatecmd
            mutex.acquire()
            print "got  mutex in validate", validatecmd
            ret=1
        self.fastntCalls=self.fastntCalls+1
        self.fastntTime=self.fastntTime+time.time()-time0
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
                    sql="DELETE dataruns WHERE run=%d" %(run)
                    self.sqlserver.Update(sql)
                    doinsert=1
            else:
                doinsert=1
            if doinsert==1:
                sql="INSERT INTO dataRuns VALUES(%d,%d,%d,%d,%d,'%s',%d,%d)" %(run,fevent,levent,fetime,letime,status,jid,submit)
                self.sqlserver.Update(sql)
                if(self.v):
                    print sql

       
    def calculateCRC(self,filename,crc):
        self.crcCalls=self.crcCalls+1
        time0=time.time()
        mutex.release()
        crccmd=self.env['AMSSoftwareDir']+"/exe/linux/crc "+filename+" "+str(crc)
        rstatus=os.system(crccmd)
        rstatus=rstatus>>8
        if(filename.find('/castor/cern.ch')>=0):
            rstatus=1
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
    def parseJoutnalFiles(self,i,v,h,s,m):
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
        else: self.v=0 
        whoami=os.getlogin()
        if not (whoami == None or whoami =='ams' ):
            print "parseJournalFiles -ERROR- script cannot be run from account : ",whoami 
            return 0
        timenow=int(time.time())
        self.valStTime=timenow
        sql = "SELECT flag, timestamp from FilesProcessing"
        ret = self.sqlserver.Query(sql)
        if(ret[0][0]==1 and timenow-ret[0][1]<100000):
            print "ParseJournalFiles-E-ProcessingFlagSet on ",ret[0][1]," exiting"
            return 0
        else: self.setprocessingflag(1,timenow,0)
        ret=self.sqlserver.Query("SELECT min(begin) FROM productionset WHERE STATUS='Active' ORDER BY begin")
        if (len(ret)==0):
            print "ValidateRuns=E-CannotFindActiveProductionSet"
            self.setprocessingflag(0,timenow,0)
            return 0
        firstjobtime=ret[0][0]-24*60*60
        lastjobtime=int(time.time())+24*60*60
        ret=self.sqlserver.Query("select max(cid) from cities")
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
        self.nCheckedCite=0
        self.nBadCopiesInRow=0
        for i in range(0,ret[0][0]-1):
            self.JF[i]=0
            self.JFLastCheck[i]=0
            self.JouDirPath=[i]="xyz"
            self.CheckedRuns[i]=0
            self.FailedRuns[i]=0
            self.GoodRuns[i]=0
            self.BadRuns[i]=0
            self.GoodDSTs[i]=0
            self.BadDSTs[i]=0
            self.BadCRC[i]=0
            self.BadRunID[i]=0
            self.gbDST[i]=0
        minJobID=0
        ret=self.sqlserver.Query("SELECT dirpath,journals.timelast,name,journals.cid FROM journals,cites WHERE journals.cid=cites.cid")
        cid=-1
        if(len(ret)>0):
            for jou in ret:
                self.nCheckedCite=self.nCheckedCite+1
                timenow=int(time.time())
                dir=self.trimblanks(jou[0])
                cite=self.trimblanks(jou[2])
                timestamp=jou[1]
                lastcheck=self.EpochToDDMMYYHHMMSS(timestamp)
                if(jou[3]==1):
                    continue
                if(cid != jou[3]):
                    cid=jou[3]
                    sql="SELECT  min(jid) from jobs where cid=$cid and timestamp>=%d"  %(periodStartTime)
                    r0=self.sqlserver.Query(sql)
                    if(len(r0)==0):
                        sql="SELECT  min(jid) from jobs where cid=$cid and timestamp<%d"  %(periodStartTime)
                        r1=self.sqlserver.Query(sql)
                        if(len(r1)>0):
                            minJobID=r1[0][0]
                    else:
                        minJobID=r0[0][0]
                    self.JouDirPath[nCheckedCite]=cite
                    self.JouLastCheck[nCheckedCite]=lastcheck
                    title="Cite %s, Directory %s Last Check %s " %(cite,dir,lastcheck)
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
                    for file in allfiles:
                        if(file.find(".journal")<0):
                            continue
                        junk=file.split("journal.")
                        if(len(junk)>1):
                            if(junk[1]==0 or junk[1]==1 or junk[1]==2):
                                continue
                        fid=file.replace(".journal","")
                        if(int(fid)<minJobID):
                            print " invalid jid ",fid
                            self.BadRunID[cite]=self.BadRunID[cite]+1
                            continue
                        newfile=joudir+"/"+file
                        writetime=os.stat(newfile)[ST_MTIME]
                        if(writetime > writelast):
                            writelast=writetime
                            lastfile=newfile
                        if(self.v):
                            print "parsejournal file ",writetime,timestamp,newfile
                        if(writetime> timestamp-24*60*60*30):
                            (suc,logfile)=self.parseJournalFile(firstjobtime,lastjobtime,logdir,newfile,ntdir,cid)
                        if(suc>0 and mail):
                            sql="select jid,mid from jobs where jid=%d" %(suc)
                            ret=self.sqlserver.Query(sql)
                            sql = "SELECT mails.name, mails.address, cites.name FROM Mails, Cites  WHERE mails.mid=%d and mails.cid=cites.cid" %(ret[0][1])
                            r4=self.sqlserver.Query(sql)
                            address='vitali.choutko@cern.ch'
                            if(len(r4)>0):
                                owner=r4[0][0]
                                address=r4[0][1]
                                cite=r4[0][2]
                                sub="Validation Failed for Job:  %d  %s" %(suc,r4[0][1])
                                mes=" "
                                if(logfile != None):
                                    input=open(logfile,'r')
                                    mes=input.read()
                                    input.close()
                                self.sendmailmessage(address,sub,mes)
                        
                        self.JF[nCheckedCite]=self.JF[nCheckedCite]+1
                        if(self.oneonly):
                            break
                    
                    
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
                self.sendmailmessage(address,sub,message)
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
        output.write(firstline)
        stime   = time.asctime(time.localtime(self.ValStTime))
        ltime=time.asctime(time.localtime(timenow))
        hours   = (timenow - self.ValStTime)/60/60;
        t0="Start Time : "+stime
        t1="End Time : "+ltime
        print t0,t1
        output.write(t0)
        output.write(t1)
        ctt = "Cites      : $nCheckedCite , active : $nActiveCites \n";
        ctt = "Cites      : %d , active : %d " %(self.nCheckedCite,self.nActiveCites)
        print ctt
        output.write(ctt)
        for i  in range(0,self.nCheckedCite):
            s1="Cite : "+self.JouDirPath[i]
            s2="Latest Journal : "+str(self.JouLastCheck[i])
            s3="New Files : "+str(self.JF[i])
            cj="%-20.15s %-20.40s %-50.30s" % (s1,s2,s3)
            print cj
            output.write(cj)
            if self.JF[i] > 0:
                l0 = "   Runs (Checked, Good, Bad, Failed) : %d %d %d %d " %( self.CheckedRuns[i], self.GoodRuns[i],  self.BadRuns[i], self.FailedRuns[i])
                l1 = "   DSTs (Checked, Good, Bad, CRC, CopyFail) :  %d %d %d %d %d %d" %(self.CheckedDSTs[i],  self.GoodDSTs[i], self.BadDSTs[i], self.BadCRC[i], self.BadDSTCopy[i], self.BadRunID[i])
                print l0
                print l1
                output.write(l0)
                output.write(l1)
            tR    = tR    + self.CheckedRuns[i]
            tRB = trb + self.BadRuns[i] + self.FailedRuns[i]
            tD    = tD    + self.CheckedDSTs[i]
            tDB = tDB + self.BadDSTs[i] + self.BadCRC[i] + self.BadDSTCopy[i]
            tGB      = tGB      + self.gbDST[i]
        totalGB = float(totalGB)/1000.
        chGB = "%3.1f" %(totalGB)
        summ =  "Total : Runs %d (%d), DSTs %d (%d), GB %s" %(tR,tRB,tD,tDB,chGB)
        print summ
        output.write(summ)
        ch0   = "Total Time %3.1f hours" %(hours)
        mbits = 0;
        if(self.doCopyTime > 0) :
            mbits = totalGB*8/self.doCopyTime
        ch1 = " doCopy (calls, time, Mbit/s) : %5d %3.1fh %3.1f [cp file :%5d %3.1fh]; " %(self.doCopyCalls, float(self.doCopyTime)/60/60, mbits, self.copyCalls, float(self.copyTime)/60/60)
        ch2 = " CRC (calls,time) : %5d, %3.1fh ; Validate (calls,time) : %5d, %3.1fh]; "%(self.crcCalls, float(self.crcTime)/60/60,self.fastntCalls, self.fastntTime/60/60 )
        print ch0
        print ch1
        print ch2
        print lastline
        output.write(ch0)
        output.write(ch1)
        output.write(ch2)
        output.write(lastline)
        output.close()
    def EpochToDDMMYYHHMMSS(self,tm):
        return time.strftime("%d %b %Y %H:%M:%S", time.localtime(tm))
    def parseJournalFile(self,firstjobtime,lastjobtime,logdir,inputfile,dirpath,cid):
        outputpath=None
        host="Unknow"
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
        self.failedcp=0
        self.bad=0
        self.unchecked=0
        self.validated=0
        leti=0
        feti=2000000000
        run=0
        try:
            input=open(inputfile,'r')
        except IOError,e:
            print e
            return 0
        buf=input.read()
        input.close()
        sp1=buf.split(', UID ')
        if(len(sp1)>1):
            sp2=sp1[1].split(' , ')
            if(inputfile.find(sp2[0])<0):
                print "Fatal - Run %s does not match file  %s"  %(sp2[0],inputfile)
                cmd="mv %s %s.o"  %(inputfile,inputfile)
                os.system(cmd)
                return 0
            else:
                try:
                    files=os.listdir(dirpath)
                except  IOError,e:
                    print e
                tnow=int(time.time())
                for file in files:
                   if(file.find(sp2[0])>=0):
                       mtim=os.stat(dirpath+"/"+file)[ST_MTIME]
                       if(tnow-mtim<300):
                           print "run %s not yet completed.  Postponed " %(sp2[0])
                           return 0
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
            return 0
        for block in blocks:
            if(block.find('RunValidated')>=0):
               junk=block.split(',')
               for i in range (0,len(junk)):
                   junk[i]=self.trimblanks(junk[i])
                   if(junk[i].find('Path')>=0):
                       path=junk[i].split(' ')
                       dirpath=self.trimblanks(path[1])
                       break
        for block in blocks:
            junk=block.slpit(",")
            for i in range (0,len(junk)):
                junk[i]=self.trimblanks(junk[i])
            jj=block.split(" ")
            utime=jj[0]
            if(utime> firstjobtime and utime <lastjobtime):
                if(block.find('RunIncomplete')>=0):
                    if(startingrunR ==1):
                        output.write("RunIncomplete : ntuples validated: %d Continue " %(validated))
                        output.write(block)
                        patternsmatched=0
                        RunIncompletePatterns=("RunIncomplete","Host","Run","EventsProcessed","LastEvent","Errors","CPU","Elapsed","CPU/Event","Status")
                        for i in range (0,len(junk)):
                            jj=junk[i].split(" ")
                            if(len(jj)>1):
                                found=0
                                j=0
                                while j<len(RunIncompletePatterns) and found==0 :
                                    if (jj[0] == RunIncompletePatterns[j]):
                                        runincomplete[j]=trimblanks(jj[1])
                                        patternsmatched=patternsmatched+1
                                        found=1
                                    j=j+1
                        if(patternsmatched == len(RunIncompletePatterns)):
                           runincomplete[0]="RunIncomplete"
                           run=runincomplete[2]
                           runfinishedR=1
                           sql="SELECT run FROM runs WHERE run = %d AND levent=%d" %(run,runincomplete[4])
                           ret=self.sqlserver.Query(sql)
                           
                           if(len(ret)==0):
                               cputime="%.0f" %(runincomplete[6])
                               elapsed="%.0f" %(runincomplete[7])
                               host=runincomplete[1]
                               sql="UPDATE Jobs SET EVENTS=%d, ERRORS=%d,CPUTIME=%s, ELAPSED=%s,HOST='%s', TIMESTAMP = %d WHERE JID = (SELECT Runs.jid FROM Runs WHERE Runs.jid = %d)" %(runincomplete[3], runincomplete[5],cputime, elapsed,host,timestamp, run)
                               output.write(sql)
                               self.sqlserver.Update(sql)
                               sql="update runs set levent=%d where run=%d " %(runincomplete[4],run)
                               output.write(sql)
                               self.sqlserver.Update(sql)
                        else:
                            output.write("parseJournalfile-W-RunIncomplete - cannot find all patterns ")
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
                        self.sendmailmessage(mailto,subject,text)
                        break
                elif(block.find('StartingJob')>=0):
                    pattarnsmatched=0
                    StartingJobPatterns = ("StartingJob", "HostName","UID","PID","Type","ExitStatus","StatusType")
                    for i in range (0,len(junk)):
                        jj=junk[i].split(' ')
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
                        jobid=startingjob[2]
                        if(self.findJob(jobif,buf,dirpath,cid) !=jobid):
                            output.write("fatail - cannot find JobInfo for "+str(jobid))
                            self.BadRuns[self.nCheckedCite]=self.BadRuns[self.nCheckedCite]+1
                            system("mv %s %s.0" %(inputfile,inputfile))
                            return 0
                    else:
                            output.write("fatail - cannot find JobInfo for in "+inputfile)
                            system("mv %s %s.0" %(inputfile,inputfile))
                            return 0
                    if(patternsmatched == len(StartingJobPatterns) or patternsmatched == len(StartingJobPatterns)-1):
                        startingjob[0]="StartingJob"
                        startingjobR=1
                        lastjobid=startingjob[2]
                    else:
                        output.write("parseJournalFiles -W- StartingJob - cannot find all patterns")
                elif(block.find('StartingRun')>=0):
                    patternsmatched = 0;
                    StartingRunPatterns = ("StartingRun","ID","Run","FirstEvent","LastEvent","Prio","Path","Status","History","CounterFail", "ClientID","SubmitTime","SubmitTimeU","Host","EventsProcessed","LastEvent","Errors","CPU","Elapsed","CPU/Event","Status")
                    j=0
                    for pat in StartingRunPatterns:
                        for xyz in junk:
                            jj=xyz.split(" ")
                            if(len(jj)>2):
                                if(pat == jj[0]):
                                    patternsmatched=patternsmatched+1
                                    startingrunrun[j]=self.trimblanks(jj[1])
                                    if(jj[0] == 'Run'):
                                        break
                        j=j+1
                    if(patternsmatched == len(StartingRunsPAtterns)+3 or patternsmatched == len(StartingRunsPAtterns)+2):
                        run=startingrun[2]
                        startingrun[0]="StartingRun"
                        sql=" select status from runs where jid=%d" %(run)
                        rq=self.sqlserver.Query(sql)
                        if(len(rq)>0 and rq[0][0].find('Completed'>=0)):
                            print "Run ",run," already comleted in database do nothing"
                            os.system("mv %s %s.1" %(inputfile,inputfile))
                            return 0
                        startingRunR=1
                        self.CheckedRuns[self.nCheckedCite]=self.CheckedRuns[self.nCheckedCite]+1
                        self.insertRun(startingrun[2],lastjobid,startingrun[3],startingrun[4],feti,leti,startingrun[12],startingrun[7])
                        if(len(startingrun)>13):
                            host=startingrun[13]
                        sql = "UPDATE Jobs set host='%s',events=%s, errors=%s,cputime=%s, elapsed=%s,timestamp=%d, mips = %s where jid=%d" %(host,startingrun[14],startingrun[16],startingrun[17],startingrun[18],timestamp,jobmips,lastjobid)
                        self.sqlserver.Update(sql)
                        output.write(sql)
                    else:
                        output.write("StartingRun - cannot find all patterns ")
                        print "StartingRun -W- cannot find all patterns " ,patternsmatched,len(StartingRunPatterns)
                elif(block.find('OpenDST')>=0):
                    patternsmatched = 0;
                    OpenDSTPatterns = ("OpenDST","Status","Type","Name","Version","Size","CRC","Insert","Begin","End","Run","FirstEvent","LastEvent","EventNumber","ErrorNumber")
                    for i in range (0,len(junk)):
                        jj=junk[i].split(' ')
                        if(len(jj)>2):
                            found=0
                            j=0
                            while(j<len(OpenDSTPAtterns) and found==0):
                                if(jj[0] == OpenDSTPatterns[j] or jj[0] == OpenDSTPatterns[j].lc()):
                                    opendst[j]=self.trimblanks(jj[1])
                                    patternsmatched=patternsmatched+1
                                    found=1
                                j=j+1
                    if(patternsmatched==OpenDSTPattern.len()-1):
                        opendst[0]="OpenDST"
                        self.CheckedDSTs[self.nCheckedCite]=self.CheckedDSTs[self.nCheckedCite]+1
                    else:
                        output.write("OPENDST - cannot find all patterns ")
                
                elif(block.find("CloseDST")>=0):
                    patternsmatched=0
                    statusIndx=1
                    fileIndx=3
                    crcIndx=6
                    CloseDSTPAtterns=("CloseDST","Status","Type","Name","Version","Size","CRC","Insert","Begin","End","Run","FirstEvent","LastEvent","EventNumber","ErrorNumber")
                    for icl in range(0,CloseDSTPAtterns.len()-1):
                        closedst[icl]=0
                    for i in range (0,junk.len()):
                        found=0
                        j=0
                        while(j<len(CloseDSTPatterns) and found==0):
                            if(jj[0] == CloseDSTPatterns[j] or jj[0] == CloseDSTPatterns[j].lc()):
                                    closedst[j]=self.trimblanks(jj[1])
                                    patternsmatched=patternsmatched+1
                                    found=1
                            j=j+1
                    if(feti>closedst[8] and closedst[8]>0):
                        feti=closedst[8]
                    if(leti<closedst[9]):
                        leti=closedst[9]
                    if (patternsmatched == CloseDSTPatterns.len()-1):
                        if(closedst[crcIndx]==0):
                            output.write("Status : %d, CRC %d. Skip file : : %d" %(closedst[statusIndx],closedst[crcIndx],closedst[fileIndx]))
                        else:
                            junk=closedst[fileIndx].split()
                            dstfile=self.trimblanks(junk[junk.len()-1])
                            filename=dstfile[:]
                            dstfile=dirpath+"/"+dstfile
                            dstsize=os.stat(dstfile)[ST_SIZE]
                            if(dstsize==None):
                                output.write("parseJournalFile-W-CloseDST block : cannot stat "+dstfile)
                                runfinishedR=1
                                dstsize=-1
                                copyfailed=-1
                                break
                            else:
                                if(closedst[1] != "Validated" and closedst[1] != "Success" and closedst[1] != "OK"):
                                    output.write("parseJournalFile -W- CloseDST block : $s,  DST status  %s. Check anyway" %(dstfile,closedst[1]))
                                dstsizs="%.1f" %(float(dstsize)/1000./1000.)
                                closedst[0]="CloseDST"
                                output.write(dstfile)
                                ntstatus=closedst[1]
                                nttype   =closedst[2]
                                version  =closedst[4]
                                ntcrc    =closedst[6]
                                run      =closedst[10]
                                jobid    =closedst[10]
                                dstfevent=closedst[11]
                                dstlevent=closedst[12]
                                ntevents =closedst[13]
                                badevents=closedst[14]
                                if (self.checkDSTVersion(version)!=1):
                                           output.write("------------ Check DST; Version : version / Min production version : %s" %(version,self.Version()))
                                           self.unchecked=self.unchecked+1
                                           self.copyfailed=self.copyfailed+1
                                           self.BadDSTs[self.nCheckedCite]=self.BadDSTs[self.nCheckedCite]+1
                                           break
                                levent=levent+dstlevent-dstfevent+1
                                i=self.caculateCRC(dstfile,ntcrc)
                                output.write("calculateCRC(%s,%d):  Status %d " %(dstfile,ntcrc,i))
                                if(i!=1):
                                           self.unchecked=self.unchecked+1
                                           self.copyfailed=self.copyfailed+1
                                           self.BadCRCi[self.nCheckedCite]=self.BadCRCi[self.nCheckedCite]+1
                                           break
                                (ret,i)=self.validateDST(dstfile,ntevents,nttype,dstlevent)
                                           
       
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
                                os.system(readtdv)
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
                                            size=line.split(" ")[1]
                                        elif(line.find("crc")>=0):
                                            crc=line.split(" ")[1]
                                        elif(line.find("begin")>=0):
                                            begin=line.split(" ")[1]
                                        elif(line.find("end")>=0):
                                            end=line.split(" ")[1]
                                        elif(line.find("insert")>=0):
                                            insert=line.split(" ")[1]
                                        
                                fltdvo.close()
                                if(len(size)>1 and len(end)>1 and len(insert)>1 and len(begin)>1 and len(crc)>1):
                                    sql="insert into tdv values( %d, '%s', '%s',%s,%s,%d,%s,%s,%s,%d)" %(id,file,ptdv,size[0:len(size)-1],crc[0:len(crc)-1],datamc,insert[0:len(insert)-1],begin[0:len(begin)-1],end[0:len(end)-1],os.stat(ptdv)[ST_MTIME])
                                    self.sqlserver.Update(sql)
                                    self.sqlserver.Commit(commit)

                                    
    def CheckDataSet(self,run2p,dataset,v,f,tab=0):
        self.verbose=v
        self.run2p=run2p
        self.force=0         
        if(f==1):
           self.force=1
        rundd=""
        rund=""
        runn=""
        runst=""
        bad1=array('i')
        bad2=array('i')
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
                pair=commands.getstatusoutput(cmd)
                out=pair[1]
                if(out.find("No such")>=0 or pathso.find('ams.cern.ch/Offline')>=0):
                    paths=pathso.replace('ams.cern.ch/Offline','cern.ch/ams/Offline',1)
                    cmd="ln -sf "+file[0]+" "+paths
                    os.system(cmd)
                    print cmd
        types=["0SCI","0LAS","0CAL","0CMD","0CAB"]
        datapath=dataset
        ds1=""
        ds2=""
        did=-1
        if(dataset.find("/")>=0):
            junk=dataset.split('/')
            dataset=junk[len(junk)-1]
            ds1=junk[len(junk)-2]
            sql="select did from datasets where name like '%s' " %(ds1)
            ds=self.sqlserver.Query(sql)
            if(len(ds)==1):
                did=ds[0][0]
                if(f==2):
                        sql="select path from ntuples where path like '%%%s/%%' and datamc=1  and path not like '/castor%%' " %(datapath)
                        files=self.sqlserver.Query(sql)
                        for file in files:
                                self.linkdataset(file[0],"/afs/cern.ch/ams/Offline/DataSetsDir",1)
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
                for i in range (1,len(junk)-1):
                        bpath=bpath+"/"+junk[i]
                isdir=run/1000000
                isdir=isdir*1000000;
                sdir="/%d" %(isdir)
                if(paths.find(sdir)<0):
                        sysc="rm "+paths
                        os.system(sysc)
                        bpath=bpath+sdir;
                        cmd="mkdir -p "+bpath
                        i=os.system(cmd)
                        if(i):
                                print "Command failed ",cmd  
                        else:
                           sysc="ln -sf "+file[0]+" "+bpath
                           os.system(sysc)
                           bpath=bpath+"/"+junk[len(junk)-1]
                           sql="update datafiles set paths='%s' where paths='%s'" %(bpath,file[1])     
                           self.sqlserver.Update(sql)
                           self.sqlserver.Commit(1)
                else:
                   continue     
                   sysc="ln -sf "+file[0]+" /afs/cern.ch/ams/Offline/RunsDir/"+type+"/"
                   os.system(sysc)
        if(run2p!=0):
            rundd=" and datafiles.run=%d " %(run2p)
            rund=" and dataruns.run=%d " %(run2p)
            runn=" and ntuples.run=%d " %(run2p)
        if(self.force):
            sql="select run from ntuples where path like '%%%s/%%' and datamc=1  %s group by run" %(dataset,runn)
            files=self.sqlserver.Query(sql)
            sql="select run from datafiles where status='OK' and type like 'SCI%%' %s " %(rundd)
            runs=self.sqlserver.Query(sql)
            if(len(files)>0):
                for run in runs:
                    found=0
                    for file in files:
                        if(run==file):
                            found=1
                            break
                    if(found==0):
                        if(tab==0):
                            print "Run ",run,"  not found in dataset ",dataset
                            bad1.append(run[0])
                        else:
                            print "<tr>"
                            print "<td>Run %d  </td><td> not found in dataset %s</td>" %(run[0],dataset)
                            print "</tr>"
        else:
            sql="select run,sum(levent-fevent+1) from ntuples where path like '%%%s/%%' and datamc=1  %s group by run" %(datapath,runn)
            files=self.sqlserver.Query(sql)
            sql="select run,dataruns.jid,dataruns.levent-dataruns.fevent+1 from dataruns,jobs where  jobs.jid=dataruns.jid and split(jobs.jobname) like '%%%s.job' %s %s" %(dataset,rund,runst)
            runs=self.sqlserver.Query(sql)
            if(len(files)>0):
                for run in runs:
                    found=0
                    for file in files:
                        if(run[0]==file[0]):
                            found=1
                            if(run[2]!=file[1] and float(run[2])>float(file[1])):
                                if(tab==0):
                                    bad2.append(run[0])
                                    print "Run ",run," and ntuples disagree. run events=",run[2]," ntuple events=",file[1]
                                else:
                                    print "<tr>"
                                    print "<td>Run %d,%d and ntuples disagree.</td><td> Run Events=%d </td><td>Ntuple Events=%d</td>" %(run[0],run[1],run[2],file[1])
                                    print "</tr>"
                            break
                    if(found==0):
                        if(tab==0):
                            print "Run ",run,"  not found in dataset ",dataset
			    bad1.append(run[0])   		
                        else:
                            print "<tr>"
                            print "<td>Run %d Events %d  </td><td> Id %d </td><td>  not found in dataset %s</td>" %(run[0],run[2],run[1],dataset)
                            print "</tr>"
            
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
            if(casuoronly>0):
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
        sql="select path,castortime from ntuples where path like '%%%s/%%' and datamc=%d %s %s %s" %(dataset,datamc%10,runn,runbuild,arun2p) 
        df=0
        files=self.sqlserver.Query(sql)
        datapath=dataset
        ds1=""
        ds2=""
        did=-1
        if(dataset.find("/")>=0):
            junk=dataset.split('/')
            dataset=junk[len(junk)-1]
            ds1=junk[len(junk)-2]
            sql="select did from datasets where name like '%s' " %(ds1)
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

        if(self.update):
            for file in files:
                if(df==0):
                    self.linkdataset(file[0],"/Offline/DataSetsDir",0)
                else:
                    self.linkdataset(file[0],"/Offline/RunsDir",0)
                cmd="rm "+file[0]
                i=0
                i=os.system(cmd)
                if(i and self.force==-1 ):
                    print " Command Failed ",cmd
                    return
                else:
                    if(self.verbose):
                        print "deleted ",file[0]
                if(file[1]>0):
                    castorPrefix='/castor/cern.ch/ams/'
                    delimiter='Data'
                    if(datamc==0):
                        delimiter='MC'
                    junk=file[0].split(delimiter)
                    if len(junk)>=2:
                        castorfile=castorPrefix+delimiter+junk[1]
                        castordel="/usr/bin/rfrm "+castorfile
                        i=os.system(castordel)
                        if(i):
                            print " CastorCommand Failed ",castordel
        if(len(files)==0):
            if(datamc==0):
                if(donly==0):
                    df=1
                    sql="select path,castortime from datafiles where path like '%%%s/%%' and type like 'MC%%'  %s " %(datapath,runndf) 
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
		    i=os.system(cmd)
                    if(i and self.force==-1 ):
                        print " Command Failed ",cmd
                        self.sqlserver.Commit(0)
                        return
                    else:
                        if(self.verbose):
                            print "deleted ",file[0]
                    if(file[1]>0):
                        castorPrefix='/castor/cern.ch/ams/'
                        delimiter='Data'
                        if(datamc==0):
                            delimiter='MC'
                        junk=file[0].split(delimiter)
                        if len(junk)>=2:
                            castorfile=castorPrefix+delimiter+junk[1]
                            castordel="/usr/bin/rfrm "+castorfile
                            i=os.system(castordel)
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
            cmd="df /"+junk[1]
            p=commands.getstatusoutput(cmd)
            sizet=0 
            if(p[0]==0):
                junk1=p[1].split("\n")
                sizet=int(junk1[2].split(" ")[22])
            alldir=os.listdir("/"+junk[1])
            for file in alldir:
                if(file.find(junk[2])<0):
                    cmd="du -ks /"+junk[1]+"/"+file
                    p=commands.getstatusoutput(cmd)
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
                            attr=pat[i].split(' ')
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
        
        
        
    def TransferDataFiles(self,run2p,i,v,u,h,source,c,replace,disk):
        if(os.environ.has_key('RunsDir')):
            runsdir=os.environ['RunsDir']
        else:
            sql = "SELECT myvalue from Environment where mykey='RunsDir'"
            ret=self.sqlserver.Query(sql)
            if(len(ret)>0):
                runsdir=ret[0][0]
            else:
                runsdir="/afs/cern.ch/ams/Offline/RunsDir"
        global mutex
        mutex=thread.allocate_lock()
        mutex.acquire()
        self.v=v
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
        delay=86400
        joudir=source+"/jou";
        filesfound=0
        timenotify=0
        while 1:
            notify=1
            for filej in os.listdir(joudir):
                pfilej=os.path.join(joudir,filej)
                if(filej.find(".jou.0")>=0):
                    notify=0
                if(filej.find(".jou.")>=0):
                    continue
                if(filej.find(".jou")<0):
                    continue
                writetime=os.stat(pfilej)[ST_MTIME]
                timenow=time.time()
                file=filej.split(".jou")[0]
                pfile=os.path.join(source,file)
                if(not os.path.isfile(pfile)):
                    print "file not found ",pfile
                    continue
                fltdvo=open(pfilej,'r')
                good=0
                crc=""
                fevent=""
                levent=""
                events=""
                size=""
                tfevent=""
                tlevent=""
                path=""
                errors=""
                timestamp=""
                tag=""
                t0=""
                t1=""
                t2=""
                t3=""
                run=""
                f1=""
                f2=""
                version=""
                for linea in fltdvo.readlines():
                    line=linea.split('\n')[0]
                    if(line.find("Run")>=0):
                        run=line.split("=")[1]
                    elif(line.find("FEvent")>=0):
                        fevent=line.split("=")[1]
                    elif(line.find("LEvent")>=0):
                        levent=line.split("=")[1]
                    elif(line.find("TFevent")>=0):
                        tfevent=line.split("=")[1]
                    elif(line.find("TLevent")>=0):
                        tlevent=line.split("=")[1]
                    elif(line.find("Version")>=0):
                        version=line.split("=")[1]
                    elif(line.find("NEvent")>=0):
                        events=line.split("=")[1]
                    elif(line.find("NError")>=0):
                        errors=line.split("=")[1]
                    elif(line.find("CRC")>=0):
                        crc=line.split("=")[1]
                    elif(line.find("Size")>=0):
                        size=line.split("=")[1]
                    elif(line.find("Timestamp")>=0):
                        rtime=line.split("=")[1]
                    elif(line.find("Path")>=0):
                        path=line.split("=")[1]
                    elif(line.find("Tag")>=0):
                        tag=line.split("=")[1]
                    elif(line.find("Type0")>=0):
                        t0=line.split("=")[1]
                    elif(line.find("Type1")>=0):
                        t1=line.split("=")[1]
                    elif(line.find("Type2")>=0):
                        t2=line.split("=")[1]
                    elif(line.find("Type3")>=0):
                        t3=line.split("=")[1]
                    elif(line.find("FileF")>=0):
                        f1=line.split("=")[1]
                    elif(line.find("FileL")>=0):
                        f2=line.split("=")[1]
                fltdvo.close()

                if(len(size)>0 and len(crc)>0 and len(events)>0 and len(tlevent)>0 and len(tfevent)>0 and len(levent)>0 and len(fevent)>0 and len(run)>0 and len(rtime)>0):

                    print "Run ",run;
                    if(run2p!=0 and int(run)!=run2p):
                        continue
		    if(abs(int(tfevent)-int(run)) >3600):
		        print "TransferDataRuns-E-WrongRun ",run," ",tfevent
			cmd="mv "+pfilej+" "+pfilej+".0"
                        os.system(cmd)
		        continue 
                    sql="select path,run,nevents,type,fevent,levent,pathb from datafiles where path like '%"+file+"'"
                    ret=self.sqlserver.Query(sql);
                    eventsi=int(events)
                    feventi=int(fevent)
                    leventi=int(levent)
                    calibnotfull=False
                    if(len(ret)>0):
                        orig=f1.split('/')
                        origpath=""
                        if(len(orig)>2):
                             origpath=origpath+orig[len(orig)-3]+"/"+orig[len(orig)-2]+"/"+orig[len(orig)-1] 
                        orig=f2.split('/')
                        if(len(orig)>2):
                             origpath=origpath+" "+orig[len(orig)-3]+"/"+orig[len(orig)-2]+"/"+orig[len(orig)-1] 

                        if(ret[0][3].find("CAL")>=0 and (ret[0][2]<eventsi or (ret[0][2]==eventsi and ret[0][6] != origpath))):
                           calibnotfull=True
                        if(ret[0][3].find("SCI")>=0 and (ret[0][2]<eventsi or (ret[0][2]==eventsi and ret[0][6] != origpath) ) and ret[0][4]>=feventi and ret[0][5]<=leventi):
                           calibnotfull=True
                        if((calibnotfull or replace) and (run2p==0 or ret[0][1] == run2p) and (disk==None or ret[0][0].find(disk)>=0)):
                            fd=ret[0][0] 
                            cmd="rm -rf "+fd
                            i=os.system(cmd)
                            if(i):
                                print "command failed ",cmd
                                if(replace>1):
                                    sql="delete from datafiles where path='"+fd+"'"  
                                    self.sqlserver.Update(sql)
                                else:
                                    continue
                            else:
                                sql="delete from datafiles where path='"+fd+"'"  
                                self.sqlserver.Update(sql)
                        else:
                            continue
                    (outputpath,ret,castortime)=self.doCopyRaw(run,pfile,int(crc),'/Data')
                    if(ret==1):
                        sizemb=int(size)/2
                        type="UNK"
                        if(t0=="5"):
                            type="SCI"
                            try:
                               itag = int(tag)%256
                            except ValueError:
                               itag=0
                            if(itag==204):
                               type="LAS"
                        elif (t0=="6"):
                            type="CAL"
                            fevent=1
                            levent=events
                        elif (t0=="7"):
                            type="CMD"
                        elif (t0=="8"):
                            type="HKD"
                        elif (t0=="27" or t0=="896"):
                            type="CAB"
                        bpath=runsdir+"/"+type
                        if(calibnotfull):
                            type=type+" MERGED "
                        if(type!="UNK"):
                                isdir=int(run)/1000000;
                                isdir=isdir*1000000;
                                sdir="/%d" %(isdir)
                                bpath=bpath+sdir;
                                cmd="mkdir -p "+bpath
                                i=os.system(cmd)
                                if(i):
                                        print "Command failed ",cmd  
                        cmd="ln -sf "+outputpath+" "+bpath
                        i=os.system(cmd)
                        if(i):
                            print "Command Failed ",cmd
                        bpath=bpath+"/"+file
                        it2=int(t2)
                        if(type=="SCI" or type=="LAS"):
                            type=type+" "+t0+" "+t1+" "
                            mnemonic=" TUSREL3";
                            for i in range (0,7):
                                it=(it2>>(i*4))&15;
                                if(it>0):
                                    type=type+mnemonic[i]
                                    type="%s%d" %(type,it)
                        else:
                            type=type+" "+t0+" "+t1+" "+t2+" "+t3
                        orig=f1.split('/')
                        origpath=""
                        if(len(orig)>3):
                             origpath+=orig[len(orig)-4]+"/"+orig[len(orig)-3]+"/"+orig[len(orig)-2]+"/"+orig[len(orig)-1] 
                        orig=f2.split('/')
                        if(len(orig)>3):
                             origpath+=" "+orig[len(orig)-4]+"/"+orig[len(orig)-3]+"/"+orig[len(orig)-2]+"/"+orig[len(orig)-1] 
                        status='OK'
                        if(type.find('SCI')>=0 and int(tlevent)-int(tfevent)<60):
                            status='SHORT'
                        sql ="insert into datafiles values(%s,'%s','%s',%s,%s,%s,%s,%s,%d,'%s','%s','%s',%s,%s,%d,0,%s,%s,%s,'%s')" %(run,version,type,fevent,levent,events,errors,rtime,sizemb,status,outputpath,origpath,crc,int(timenow),castortime,tag,tfevent,tlevent,bpath)
                        self.sqlserver.Update(sql)
                        self.sqlserver.Commit(1)
                        cmd="rm -rf "+pfile
                        os.system(cmd)
                        cmd="mv "+pfilej+" "+pfilej+".1"
                        os.system(cmd)
                        notify=0
                        filesfound=filesfound+1
                    else:
                        if(outputpath!=None):
                            cmd="rm  "+outputpath
                            print "TransferDataFiles-ECopyFail ",cmd 
                            time.sleep(60) 
                            if(not os.path.islink(outputpath) and not os.path.isdir(outputpath)):
                                os.system(cmd)
            if(c==0 or run2p!=0):
                break;
            else:
                time.sleep(60)
                if(notify>0 and filesfound>0):
                    message="%d New DataRuns Transferred " %(filesfound)
                    self.NotifyResp(message)
                    filesfound=0
#                    suc=self.dbserver.CreateRun(run,fevent,levent,tfevent,tlevent,0,1,outputpath)
#                    if(suc==1):
#                        self.sqlserver.Commit(1)
#                    else:
#                        self.sqlserver.Commit(0)

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
        os.system("mkdir -p "+newdir)
        cmd=""
        if(crdel==1):
                if(path.find('/castor/cern.ch/ams')>=0):
                    path.replace('/castor/cern.ch/ams','/castor',1);
                cmd="ln -sf "+path+" "+newdir
                
        else:
                cmd="rm "+file
                i=os.system(cmd)
                cmd="rm "+newfile
        i=os.system(cmd)
        if(i):
                print "Problem with "+cmd
        return newfile
