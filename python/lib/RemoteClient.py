import sys,os,time,string,re,thread
from DBSQLServer import DBSQLServer
class RemoteClient:
    sqlserver=""
    def ConnectDB(self):
        self.sqlserver=DBSQLServer(sys.argv)
        ok=self.sqlserver.Connect()
        return ok
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

        if irm==1: rm="rm -i "
        else: rm ="rm -f "
        # to be changed
        address="vitali.choutko@cern.ch"
        castorPrefix='/castor/cern.ch/ams'
        delimiter='MC'
        rfcp="/usr/bin/rfcp"
        whoami=os.getlogin()
        if not (whoami == None or whoami =='ams' or whoami=='casadmva' or whoami=='choutko'):
            print "castorPath -ERROR- script cannot be run from account : ",whoami 
            return 0
        sql="select ntuples.path,ntuples.crc,ntuples.castortime,ntuples.run,ntuples.fevent,ntuples.levent,ntuples.sizemb from ntuples where  ntuples.path not like  '"+castorPrefix+"%' ";
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
        self.CheckFS(1)
        for ntuple in ret_nt:
            if run != ntuple[3]:
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
        if v and ntp>0:
            print "Total of ",runs,"  runs, ",ntp," ntuples  processed. \n ",ntpb," bad ntuples found. \n ",ntpf,"  ntuples could not be repared\n ",ntna," ntuples could not be verified"

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
        address="vitali.choutko@cern.ch"
        castorPrefix='/castor/cern.ch/ams'
        delimiter='MC'
        rfcp="/usr/bin/rfcp"
        whoami=os.getlogin()
        if not (whoami == None or whoami =='ams' or whoami=='casadmva' or whoami=='choutko'):
            print "castorPath -ERROR- script cannot be run from account : ",whoami 
            return 0
        sql="select ntuples.path,ntuples.crc,ntuples.castortime,ntuples.run,ntuples.fevent,ntuples.levent,ntuples.sizemb from ntuples where  ntuples.path not like  '"+castorPrefix+"%' ";
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
        self.cmb=0
        self.runs=0
        self.ntpf=0
        self.ntpb=0
        self.ntp=0
        self.v=v
        for ntuple in ret_nt:
            totmb=totmb+ntuple[6]
        self.times=time.time()
        self.CheckFS(1,300)
        sql="select disk from filesystems where isonline=1"
        ret_fs=self.sqlserver.Query(sql)
        global exitmutexes
        exitmutexes = {}
        global mutex
        mutex=thread.allocate_lock()
        for fs in ret_fs:
            exitmutexes[fs[0]]=thread.allocate_lock()
            thread.start_new(self.checkcrct,(fs[0],ret_nt))
        for key in exitmutexes.keys():
            while not exitmutexes[key].locked(): pass
        if v and self.ntp>0:
            print "Total of ",self.runs,"  runs, ",self.ntp," ntuples  processed. \n ",self.ntpb," bad ntuples found. \n ",self.ntpf,"  ntuples could not be repared\n ",ntna," ntuples could not be verified"

    def checkcrct(self,fs,ret_nt):
        mutex.acquire()
        print fs
        run=0
        mutex.release()
        for ntuple in ret_nt:
            mutex.acquire()
            if run != ntuple[3]:
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
                self.ntpb=self.ntpb+1
                if ntuple[2]>0:
                    #copy from castor
                    a=0
                else:
                    #no castor file found and bad crc remove ntuple
                    self.ntpf=self.ntpf+1
            mutex.release()
        exitmutexes[fs].acquire()

    def CheckFS(self,updatedb=0,cachetime=60):
        #
        #  check  filesystems, update the tables accordingly if $updatedb is set
        #  status: Active  :  May be used 
        #          Reserved :  Set manually, may not be reseting
        #          Full     :  if < 3 GB left
        #         
        #  isonline: 1/0 
        #  return disk with highest available and online=1
        #
           sql="select disk from filesystems where isonline=1 and status='Active' order by available desc"
           ret=self.sqlserver.Query(sql);
           if(time.time()-cachetime < self.dbfsupdate() and len(ret)>0):
               return ret[0][0]
           sql="select disk,host,status,allowed  from filesystems"
           ret=self.sqlserver.Query(sql);
           for fs in ret:
               #
               #          check to see if it is can be readed
               #
               stf=fs[0]+".o"
               stf=re.sub(r'/','',stf)
               stf="/tmp/"+stf
               print stf
               os.system("ls "+fs[0]+" 1>"+stf+" 2>&1 &")
               time.sleep(1)
               stat =os.stat(stf)
               os.unlink(stf)
               if stat[7]==0:
                   sql="update filesystems set isonline=0 where disk='"+str(fs[0])+"'"
                   if updatedb!=0:
                       self.sqlserver.Update(sql)
                   continue
               res=os.statvfs(fs[0])
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
                 	sql = "SELECT SUM(sizemb) FROM ntuples WHERE  PATH like '"+fs[0]+"%'"
                 	sizemb=self.sqlserver.Query(sql)
                 	rused=0
                        if len(sizemb)>0 and sizemb[0][0] != None:
                            rused=sizemb[0][0]
                        ava=bavail*fac
                        ava1=tot*float(fs[3])/100-rused
                        if fs[2]=='Reserved':
                            status='Reserved'
                        elif ava1<0 or ava<3000:
                            status='Full'
                        if ava1< ava: ava=ava1;
                        if ava<0: ava=0
                        sql="update filesystems set isonline=1, totalsize="+str(tot)+", status='"+status+"',occupied="+str(occ)+",available="+str(ava)+",timestamp="+str(timestamp) +" where disk='"+fs[0]+"'"
                   else:
                    sql="update filesystems set isonline=0 where disk='"+fs[0]+"'"
                   if updatedb>0:
                    self.sqlserver.Update(sql)
                    self.sqlserver.Commit()
           sql="select disk from filesystems where isonline=1 and status='Active' order by available desc"
           ret=self.sqlserver.Query(sql)
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
           self.CheckFS(1,300)
           sql="select disk from filesystems where isonline=1 and disk like '"+disk+"'"
           ret=self.sqlserver.Query(sql)
           if len(ret)>0: return 1
           return 0

