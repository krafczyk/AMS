import sys,os,time,string,re,socket
import ORBit
ORBit.load_typelib('Everything')
ORBit.load_file('/usr/include/server.idl')
import CORBA
from DBSQLServer import DBSQLServer
class DBServer:
    fields={'cid':None,'start':None,'ac':None,'orb':None,'root_poa':None,'mypoadbserver':None,'myref':None,'myior':None,'arsref':[],'arpref':[],'nhl':None,'ahls':None,'acl':None,'aml':None,'asl':None,'adbsl':None,'acl_maxc':0,'aml_maxc':0,'asl_maxc':0,'adbsl_maxc':0,'nsl':None,'ncl':None,'nkl':None,'rtb':None,'rtb_maxr':None,'dsti':None,'dsts':None,'db':None,'env':None,'rn':None,'dbfile':None,'ok':0,'rundummy':None}
    def __init__(self,ior,iord):
        if(ior == None):
            #start server
            self.start=time.time();
            self.cid=CID("DBServer")
            self.orb=CORBA.CORBA.ORB_init(sys.argv, 'orbit-local-orb')
            # add smth later
        else:
            #start client
            self.orb=CORBA.ORB_init()
            self.tm=self.orb.string_to_object(ior)
            self.arsref=[self.tm]
            self.cid=self.tm.getId()
            self.cid.Type=self.tm.Producer
            (length,ars)=self.tm.getARS(self.cid,self.tm.Any,0,1)
            self.iorp=self.orb.string_to_object(ars[0].IOR)
            self.cid.Type=self.tm.DBServer
            (length,ars)=self.tm.getARS(self.cid,self.tm.Any,0,1)
            if(length>0):
              self.iord=self.orb.string_to_object(ars[0].IOR)
              print self.iord.getId().Type
            else:
              self.iord=self.orb.string_to_object(iord)
            self.UpdateEverything()
    def CreateRun(self,run,fevent,levent,tfevent,tlevent,priority,datamc,path):
        self.dummyrun.Run=run
        self.dummyrun.FirstEvent=fevent
        self.dummyrun.LastEvent=levent
        self.dummyrun.TFEvent=tfevent
        self.dummyrun.TLEvent=tlevent
        self.dummyrun.Priority=priority
        self.dummyrun.DataMC=datamc
        self.dummyrun.filepath=path
        self.iorp.sendRunEvInfo(self.dummyrun,self.tm.Create)
        return 1
#        except: CORBA.SYSTEM_EXCEPTION, e:
#            print "Unable to create run ",e
#            return 0
        
    def UpdateEverything(self):
        self.dsts=[]
#	(length,self.dsts)=self.iorp.getDSTS(self.cid)
        maxr=1200
        (length,self.rtb,maxrun1)=self.iorp.getRunEvInfoS(self.cid,maxr)
        self.rundummy=self.rtb[0]
        len=0
        l1=0
        for run in self.rtb:
            if(run.Status==self.iorp.Finished):
                try:
                    (l,self.dst)=self.iorp.getDSTSR(self.cid,run.Run)
                    len=len+l
                    for ntuple in self.dst:
                        if(ntuple.Type!=self.iorp.EventTag):
                            self.dsts.append(ntuple)
                            l1=l1+1
                except:
                    print "Exception ",run.Run
        print l1
    def ct(self,status):
        if status==self.iorp.Ntuple:
            return "Ntuple"
        elif status==self.iorp.EventTag:
            return "EventTag"
        elif status==self.iorp.RootFile:
            return "RootFile"
        elif status==self.iorp.RawFile:
            return "RawFile"
        else:
            return None
        
    def cc(self,status):
        if status==self.tm.OneRunOnly:
            return "OneRunOnly"
        elif status==self.tm.Permanent:
            return "Permanent"
        elif status==self.tm.Killed:
            return "Killed"
        elif status==self.tm.TimeOut:
            return "TimeOut"
        elif status==self.tm.Active:
            return "Active"
        elif status==self.tm.Registered:
            return "Registered"
        elif status==self.tm.Submitted:
            return "Submitted"
        elif status==self.tm.Unknown:
            return "Unknown"
        elif status==self.tm.Lost:
            return "Lost"
        elif status==self.tm.ClearCreate:
            return "ClearCreate"
        elif status==self.tm.Create:
            return "Create"
        elif status==self.tm.Update:
            return "Update"
        elif status==self.tm.Delete:
            return "Delete"
        elif status==self.tm.GreaterThan:
            return "GreaterThan"
        elif status==self.tm.AnyButSelf:
            return "AnyButSelf"
        elif status==self.tm.Self:
            return "Self"
        elif status==self.tm.LessThan:
            return "LessThan"
        elif status==self.tm.Any:
            return "Any"
        
        else:
            return None
        


    def cr(self,status):
        if status==self.iorp.Foreign:
            return "Foreign"
        elif status==self.iorp.Canceled:
            return "Canceled"
        elif status==self.iorp.Allocated:
            return "Allocated"
        elif status==self.iorp.Unknown:
            return "Unknown"
        elif status==self.iorp.Finished:
            return "Finished"
        elif status==self.iorp.Processing:
            return "Processing"
        elif status==self.iorp.Failed:
            return "Failed"
        elif status==self.iorp.ToBeRerun:
            return "ToBeRerun"
        else:
            return None
        


    def cn(self,status):
        if status==self.iorp.Success:
            return "Success"
        elif status==self.iorp.InProgress:
            return "InProgress"
        elif status==self.iorp.Failure:
            return "Failure"
        elif status==self.iorp.Validated:
            return "Validated"
        else:
            return None
        



        
class CID:
    HostName=""
    uid=0
    pid=0
    ppid=0
    threads=1
    threads_change=0
    Mips=1000
    Type="Monitor"
    Status="NOP"
    StatusType="Permanent"
    Interface="default"
    
    def __init__(self,type="Monitor"):
        self.Type=type
        self.HostName=socket.gethostname()
        self.pid=os.getpid()
    
