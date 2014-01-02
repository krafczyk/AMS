//  $Id: producer.C,v 1.187.2.9 2014/01/02 15:31:54 choutko Exp $
#include <unistd.h>
#include <stdlib.h>
#include "producer.h"
#include "cern.h"
#include "commons.h" 
#include <stdio.h>
#include <iostream>
#include "event.h"
#include "job.h"
#include<algorithm>
#include <sys/statfs.h>
#include <sys/timeb.h>
#include <sys/stat.h>
#include <sys/file.h> 
#include <malloc.h>
#include <netdb.h>

AMSProducer * AMSProducer::_Head=0;
AString * AMSProducer::_dc=0; 
AMSProducer::AMSProducer(int argc, char* argv[], int debug) throw(AMSClientError):AMSClient(),AMSNode(AMSID("AMSProducer",0)),_RemoteDST(false),_OnAir(false),_FreshMan(true),_Local(true),_Solo(false),_Transfer(false),_FreeSpace(-1){
DPS::Producer_var pnill=DPS::Producer::_nil();
mallopt(M_CHECK_ACTION,1);

_plist.push_back(pnill);
if(_Head){
 FMessage("AMSProducer::AMSProducer-E-AMSProducerALreadyExists",DPS::Client::CInAbort);
}
else{
 char * ior=0;
 uinteger uid=0;
 float ClockCor=1;
 for (char *pchar=0; argc>1 &&(pchar=argv[1],*pchar=='-'); (argv++,argc--)){
    pchar++;
    switch (*pchar){
    case 'I':   //ior
     ior=pchar;
     break;
    case 'D':   //debug
     _debug=atoi(++pchar);
     break;
    case 'M':   //debug
     __MT=true;
     break;
    case 'U':   //uid
     uid=atol(++pchar);
     break;
    case 'C':   //correction factor to computer clock 
     ClockCor=atol(++pchar)/100.;
     if(ClockCor>0 && ClockCor<5){
      AMSFFKEY.CpuLimit*=ClockCor;
      if(ClockCor>1)GCTIME.TIMEND*=ClockCor;
      cout <<" AMSProducer::AMSProducer-I-CPULimitChangedTo "<<AMSFFKEY.CpuLimit<<endl;
     }
     break;
    case 'G':   //local
     _Local=false;
     if(pchar+1 && *(pchar+1)=='R'){
        _RemoteDST=true;
        cout<< "AMSProducer::AMSProducer-I-RemoteDSTSelected "<<endl;
     }
     break;
    case 'A': //amsdatadir
      setenv("AMSDataDir",++pchar,1);
      break;
    case 'S':   // Solo, no IOR 
      if(pchar+1 && *(pchar+1)=='Y'){
       _Solo=true;
      }
      break;
  }
 }
  if(!ior ){
    
   if(_debug){
     if(!AMSJob::gethead()->isRealData())_openLogFile("MCProducer",_Solo);
     else _openLogFile("Producer",_Solo);
   }
//   _Head=this;
// return;
   FMessage("AMSProducer::AMSProducer-F-NoIOR",DPS::Client::CInAbort);
  }
  _orb= CORBA::ORB_init(argc,argv);
  try{
   CORBA::Object_var obj=_orb->string_to_object(ior);
   if(!CORBA::is_nil(obj)){
    DPS::Producer_var _pvar=DPS::Producer::_narrow(obj);
    if(!CORBA::is_nil(_pvar)){
     _plist.clear();
     _plist.push_front(_pvar);
  if(!_getpidhost(uid)){
   if(_debug){
     if(!AMSJob::gethead()->isRealData())_openLogFile("MCProducer",_Solo);
     else _openLogFile("Producer",_Solo);
   }
      FMessage("AMSProducer::AMSProducer-F-UnableToGetHostName", DPS::Client::CInAbort);
 }
   if(_debug){
     if(!AMSJob::gethead()->isRealData())_openLogFile("MCProducer",_Solo);
     else _openLogFile("Producer",_Solo);
   }
     _Head=this;
     return;
}
}

}
  catch (CORBA::SystemException & a){
  FMessage("AMSProducer::AMSProducer-F-CORBA::SystemError",DPS::Client::CInAbort);
  }

}
FMessage("AMSProducer::AMSProducer-F-UnableToInitCorba",DPS::Client::CInAbort);

}


void AMSProducer::sendid(int cpuf){
if(!cpuf){
    _pid.Mips=AMSCommonsI::getmips();
    _pid.threads=AMSEvent::get_num_threads_pot();
    _pid.threads_change=0;
    cout <<"  Mips:  "<<_pid.Mips<<" Threads "<<_pid.threads<<endl;
   #ifdef _OPENMP
       int maxt=omp_get_num_procs();
    if(1){
        if(_pid.threads!=maxt && _pid.threads>maxt/2){
           AMSEvent::set_num_threads(  maxt/2);
           _pid.threads=AMSEvent::get_num_threads_pot();
           cout <<"  ThreadsChanged  "<<_pid.Mips<<" Threads "<<_pid.threads<<endl;
        }
     }
#endif    
   
    bool ok=SetDataCards();
     _pid.StatusType=DPS::Producer::OneRunOnly;
     _pid.Type=DPS::Producer::Generic;
if (_Solo){
      _pid.Type=DPS::Producer::Standalone;
//      _pid.StatusType=DPS::Producer::OneRunOnly;
      LMessage(AMSClient::print(_pid,"JobStarted"));
      if(!ok ){
       cerr<<" AMSProducer::sendid-F-UnableToSetDataCards "<<endl;
       abort();
      } 
      else{
             LMessage(AMSClient::print(_pid,"MetaDataAdded")); 
      }
      return;
}
else if(!ok){
        AString pc="AMSProducer::sendid-F-UnableToSetDataCards ";
        pc+=(const char*)_pid.Interface;
        FMessage((const char*)pc,DPS::Client::CInAbort);
}
}

again:

     list<DPS::Producer_var>::iterator li = _plist.begin();
     time_t cput=6*AMSFFKEY.CpuLimit;
     time_t cpug=12*3600;
     if(!IsLocal() && cput<cpug)cput=cpug; 
    if(cpuf)cput=cpuf; 
    cout <<"   TimeOut sending "<<cput<<endl;
    struct hostent* h;
    h=gethostbyname("pcamss0.cern.ch");
      IMessage(AMSClient::print(_pid,"sendID-I-Before"));
  try{
     if(!((*li)->sendId(_pid,_pid.Mips,cput))){
       if(_pid.uid)sleep(10);
       else{
        // dieHard
        AString pc="Server Requested Termination after sendID Because Of ";
        pc+=(const char*)_pid.Interface;
        FMessage((const char*)pc,DPS::Client::SInAbort);
       }
      if(!((*li)->sendId(_pid,_pid.Mips,cput))){
       // dieHard
       AString pc="Server Requested Termination after sendID Because Of ";
       pc+=(const char*)_pid.Interface;
       FMessage((const char*)pc,DPS::Client::SInAbort);
      }
     }
         cout <<"  Mips:  "<<_pid.Mips<<endl;
     IMessage(AMSClient::print(_pid,"sendID-I-Success"));
     LMessage(AMSClient::print(_pid,"JobStarted"));
            cout <<"  Mips:  "<<_pid.Mips<<endl;

      return;       
     }
   catch (CORBA::MARSHAL a){
   if(getior("GetIorExec"))goto again;
   else FMessage("AMSProducer::AMSProducer-F-CORBA::MARSHAL",DPS::Client::CInAbort);
  }
  catch (CORBA::COMM_FAILURE a){
   if(getior("GetIorExec"))goto again;
   else FMessage("AMSProducer::AMSProducer-F-CORBA::COMM_FAILURE",DPS::Client::CInAbort);
  }
  catch (CORBA::SystemException & a){
   if(getior("GetIorExec"))goto again;
   else FMessage("AMSProducer::AMSProducer-F-CORBA::SystemError",DPS::Client::CInAbort);
  }

}




void AMSProducer::getRunEventInfo(bool solo){
if(solo && _Solo)return;
if (_Solo){
    struct timeb  ft;
    ftime(&ft);
    _ST0=ft.time+ft.millitm/1000.;
    if(_debug)cout <<"ST0 "<<_ST0<<endl;
    TIMEX(_T0);
    if(_debug)cout <<"T0 "<<_T0<<endl;
     _reinfo =new DPS::Producer::RunEvInfo(); 
     _dstinfo =new DPS::Producer::DSTInfo(); 
     _dstinfo->UpdateFreq=1000;
     if(IOPA.WriteRoot)_dstinfo->type = DPS::Producer::RootFile;
     else if(IOPA.hlun) _dstinfo->type = DPS::Producer::Ntuple;
     else if(DAQCFFKEY.mode/10>0)_dstinfo->type= DPS::Producer::RawFile;
      else {
         cerr<<" AMSProducer::getRuneventInfo-F-NoOutputMediaDefined "<<endl;
         exit(1);
       }
     _reinfo->uid=0;
     _reinfo->Priority=0;
     time_t ct;
     time(&ct);
     _reinfo->SubmitTime=ct;
     _reinfo->cuid=_pid.uid;
     _reinfo->cinfo.LastEventProcessed=0;
     _reinfo->cinfo.HostName=_pid.HostName; 
     _reinfo->cinfo.Status=DPS::Producer::Processing;
     _reinfo->Priority=0;
   if(AMSJob::gethead()->isSimulation()){
    _reinfo->DataMC=0;
    _reinfo->FirstEvent=GCFLAG.IEVENT+1;
    _reinfo->LastEvent=GCFLAG.NEVENT;
    _reinfo->Run=CCFFKEY.run;
    _reinfo->Status=DPS::Producer::Allocated;
    _reinfo->History=DPS::Producer::Foreign;
    _reinfo->CounterFail=0;
   }
   else{
   _reinfo->DataMC=1;
   if(!AMSJob::gethead()->isRealData()){
   _reinfo->DataMC=0;
   }
   _reinfo->uid=_pid.uid;
    _reinfo->CounterFail=0;
    _reinfo->Status=DPS::Producer::Allocated;
    _reinfo->History=DPS::Producer::Foreign;
    _reinfo->Run=SELECTFFKEY.Run;
    _reinfo->FirstEvent=SELECTFFKEY.Event;
    _reinfo->LastEvent=SELECTFFKEY.EventE;
    SELECTFFKEY.Run=0;
    SELECTFFKEY.Event=0;
    SELECTFFKEY.EventE=0;
   }
    //SELECTFFKEY.Run=_reinfo->Run;
    //SELECTFFKEY.Event=_reinfo->FirstEvent;
    //SELECTFFKEY.RunE=_reinfo->Run;
    //SELECTFFKEY.EventE=_reinfo->LastEvent;    
    _cinfo.Mips=AMSCommonsI::getmips();
    _cinfo.EventsProcessed=0;
    _cinfo.LastEventProcessed=0;
    _cinfo.ErrorsFound=0;
    _cinfo.Status=DPS::Producer::Processing;
    _cinfo.CPUTimeSpent=0;
    _cinfo.CPUMipsTimeSpent=0;
    _CPUMipsTimeSpent=0;
    _cinfo.TimeSpent=0;
    _cinfo.HostName=_pid.HostName; 
    _cinfo.Run=_reinfo->Run;
    _reinfo->cinfo=_cinfo;
     if(_reinfo->Run!=0)LMessage(AMSClient::print(_reinfo,"StartingRun"));
  return;
}


UpdateARS();
bool mtry=false; 
_pid.threads=AMSEvent::get_num_threads_pot();
_pid.threads_change=0;
again:
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  try{
    (*li)->getRunEvInfo(_pid,_reinfo,_dstinfo);
         
    if(_dstinfo->DieHard){
     if(!mtry){
      cerr <<" problem with runevinfo sleep 2 sec "<<_dstinfo->DieHard<<endl;
       sleep(2);
       mtry=true;
       goto again;
    } 
      cerr <<" DieHard "<<_dstinfo->DieHard<<endl; 
      if(!_FreshMan && _dstinfo->DieHard==1)FMessage("AMSProducer::getRunEventinfo-I-ServerRequestedExit",DPS::Client::SInExit);
      else FMessage("AMSProducer::getRunEventinfo-I-ServerRequestedAbort",DPS::Client::SInAbort);
    }
    _FreshMan=false;
    _cinfo.Run=_reinfo->Run;
    _cinfo.HostName=_pid.HostName; 
    AMSEvent::set_num_threads((_reinfo->cinfo).CriticalErrorsFound/2097152);
    cout <<"  ***-*** setting thtreads to "<< (_reinfo->cinfo).CriticalErrorsFound/2097152 << " "<<MISCFFKEY.NumThreads<<endl;
    SELECTFFKEY.Run=_reinfo->Run;
    SELECTFFKEY.Event=_reinfo->FirstEvent;
    SELECTFFKEY.RunE=_reinfo->Run;
    //SELECTFFKEY.EventE=_reinfo->LastEvent;    
    _pid.threads=AMSEvent::get_num_threads_pot();
    _reinfo->cinfo.LastEventProcessed=0;
   _cinfo.Mips=AMSCommonsI::getmips();
    _cinfo.EventsProcessed=(_reinfo->cinfo).EventsProcessed;
    _cinfo.ErrorsFound=(_reinfo->cinfo).ErrorsFound;
    _cinfo.CriticalErrorsFound=((_reinfo->cinfo).CriticalErrorsFound%2097152);
    _cinfo.Status=DPS::Producer::Processing;
    _cinfo.CPUTimeSpent=0;
    _cinfo.CPUMipsTimeSpent=(_reinfo->cinfo).CPUMipsTimeSpent;
    _CPUMipsTimeSpent=_cinfo.CPUMipsTimeSpent;
    cout << "  _cinfo CPUMIPS "<<_cinfo.CPUMipsTimeSpent<<" "<<_cinfo.EventsProcessed<<endl;
    _cinfo.TimeSpent=0;

   if(AMSJob::gethead()->isSimulation()){
    GCFLAG.IEVENT=_reinfo->FirstEvent;
/*
    if(GCFLAG.IEVENT>1 ){
     // should call the rndm odnako
     cerr<<"AMSProducer::getRunEventInfo-W-ChangingRNDMBecauseFirstEventNumberNotOne"<<endl;
     for(int i=1;i<GCFLAG.IEVENT;i++){
      geant dum;
      RNDM(dum);
     }
    }
*/
    if(GCFLAG.IEVENT>1 ){
    if(!_reinfo->rndm1 ||  !_reinfo->rndm2){

     cerr<<"AMSProducer::getRunEventInfo-W-FirstEventNumberNotOneButRNDMIsZero"<<endl;
     for(int i=1;i<GCFLAG.IEVENT;i++){
      geant dum;
      RNDM(dum);
     }

    }
    else{
      cout<<"AMSProducer::getRunEventInfo-W-ChangingRNDMBecauseFirstEventNumberNotOne "<<_reinfo->rndm1<<" "<<_reinfo->rndm2<<endl;
       GCFLAG.NRNDM[0]=_reinfo->rndm1;      
       GCFLAG.NRNDM[1]=_reinfo->rndm2;
       AMSmceventg::RestoreSeeds();      
    }
    }
    GCFLAG.NEVENT=_reinfo->LastEvent;
    CCFFKEY.run=_reinfo->Run;
   }
   else{

    if(_dstinfo->Mode==DPS::Producer::RILO || _dstinfo->Mode==DPS::Producer::RIRO){ 
     if(_pid.StatusType!=DPS::Producer::OneRunOnly){
      DAQEvent::setfile((const char *)(_reinfo->FilePath));
     }
}
else{
     AString fpath=(const char *)_dstinfo->OutputDirPath;
     AString fmake="mkdir -p ";
     fmake+=fpath;
     system((const char*)fmake);
     fpath+="/run.";
     char tmp[80];
     sprintf(tmp,"%d",_reinfo->Run);
     fpath+=tmp;
     fpath+=".";
     sprintf(tmp,"%d",_pid.uid);
     fpath+=tmp;
     DAQEvent::setfile((const char *)(fpath));
     ofstream fbin;
     fbin.open((const char*)fpath);
     if(!fbin){
       cerr <<fpath<<endl;
       FMessage("Unable to open tmp run file ", DPS::Client::CInAbort); 
     }
     DPS::Producer::TransferStatus st=DPS::Producer::Begin;
     DPS::Producer::RUN * prun;
     DPS::Producer::FPath fp;
     fp.fname=(const char*)_reinfo->FilePath;
     fp.pos=0;   
     while(st!=DPS::Producer::End){
     try{
      int length=(*li)->getRun(_pid,fp,prun,st);
      DPS::Producer::RUN_var  vrun=prun;
      if(length)fbin.write((char*)vrun->get_buffer(),length);      
      fp.pos+=length;
     }
     catch (DPS::Producer::FailedOp & a){
      FMessage((const char *)a.message,DPS::Client::SInAbort);
     }
    }
    if(!fbin.good()){
       cerr <<fpath<<endl;
       FMessage("Unable to write tmp run file ", DPS::Client::CInAbort); 
    }
    fbin.close();
}
}
//check ROOTSYS; don;t really need it so put dummy value
  if(char  *crnr=getenv("ROOTSYS")){
   }
   else{
      cerr<<"AMSPRoducer-W-ROOTSYSNotDefined"<<endl;
      setenv("ROOTSYS","/afs/cern.ch/exp/ams/Offline/root/Linux/pro",1);
   }
//  Check here CERN_ROOT;  put /cern/2001 if no
// check if proposed dst file is writeable
   bool writeable=false;
   if(char  *crnr=getenv("CERN_ROOT")){
   }
   else{
      cerr<<"AMSPRoducer-W-CERN_ROOTNotDefined"<<endl;
      setenv("CERN_ROOT","/cern/2001",1);
   }
ndir:  
   if(char *ntd=getenv("NtupleDir")){
     AString cmd=" mkdir -p -v  ";
     cmd+=ntd;
     system(cmd);
     cout <<"AMSProducer::getRunEvInfo-I-CreatingOutputDir "<<cmd<<endl;
     cmd=" touch ";
     cmd+=ntd;
     cmd+="/qq";
     int i=system(cmd);
     if(i==0){
      writeable=true;
      cmd=ntd;
      cmd+="/qq";
      unlink((const char*)cmd);
     }
     else{
      cerr<<"AMSProducer::getRunEventInfo-E-UnwritableDir "<<ntd<<endl;
      if(getenv("NtupleDir2")){
         char *nt2=getenv("NtupleDir2");
        if(strlen(nt2)){
          string nt2_new=nt2;
          char whoami[255]="";
          if(getlogin() && !strstr(getlogin(),"root")){
              sprintf(whoami,"%s/%u",getlogin(),_pid.pid);
              cout <<" AMSProducer-I-getlogin() "<<whoami<<endl;
          }      
          else if(getenv("LOGNAME"))sprintf(whoami,"%s/%u",getenv("LOGNAME"),_pid.pid);
          int pos=nt2_new.find("whoami");  
          if(pos>=0 && whoami && strlen(whoami))nt2_new.replace(pos,6,whoami);
          setenv("NtupleDir",nt2_new.c_str(),1);
          cout <<"producer-I-RedefinedNtupleDir "<<getenv("NtupleDir")<<endl;
        } 
        else setenv("NtupleDir",getenv("NtupleDir2"),1);
        unsetenv("NtupleDir2");
        goto ndir;
      }    
      else if(getenv("NtupleDir3")){
        setenv("NtupleDir",getenv("NtupleDir3"),1);
        unsetenv("NtupleDir3");
        goto ndir;
      }    
   }
   }
   LMessage(AMSClient::print(_reinfo,"StartingRun"));

     if(IOPA.WriteRoot)_dstinfo->type = DPS::Producer::RootFile;
     else if(IOPA.hlun) _dstinfo->type = DPS::Producer::Ntuple;
     else if(DAQCFFKEY.mode/10>0)_dstinfo->type= DPS::Producer::RawFile;
      else {
         cerr<<" AMSProducer::getRuneventInfo-F-NoOutputMediaDefined "<<endl;
         exit(1);
       }
   if(IsLocal() && !writeable){
    cout <<"AMSProducer-getRunEvInfo-S-NtupleDir "<<getenv("NtupleDir")<<" IsNotWriteable"<<endl; 
    AString ntpath=(const char *)_dstinfo->OutputDirPath;
    setenv("NtupleDir",(const char*)ntpath,1);
//  check if it is writeable
     AString cmd=" touch ";
     cmd+=ntpath;
     cmd+="/qq";
     int i=system(cmd);
     if(i==0){
      writeable=true;
      cmd=ntpath;
      cmd+="/qq";
      unlink((const char*)cmd);
     }
     else{
      cerr<<"AMSProducer::getRunEventInfo-F-UnwritableDir "<<ntpath<<endl;
      FMessage("Unable to write dstfile ", DPS::Client::CInAbort); 
     }     

    ntpath+="/";
    char tmp[80];
    sprintf(tmp,"%d",_reinfo->Run);
    ntpath+=tmp;
    ntpath+="/";
    if(_dstinfo->type == DPS::Producer::RootFile){
      AMSJob::gethead()->SetRootPath((const char *)ntpath);
    }
    else{
       AMSJob::gethead()->SetNtuplePath((const char *)ntpath);
    }
    }
    else{
     if(_dstinfo->type == DPS::Producer::RootFile)
     _dstinfo->OutputDirPath=AMSJob::gethead()->GetRootPath();
     else
     _dstinfo->OutputDirPath=AMSJob::gethead()->GetNtuplePath();
     if(_RemoteDST){
       if(_dstinfo->Mode==DPS::Producer::LILO)_dstinfo->Mode=DPS::Producer::LIRO;
       if(_dstinfo->Mode==DPS::Producer::RILO)_dstinfo->Mode=DPS::Producer::RIRO;
    }
    else{
       if(_dstinfo->Mode==DPS::Producer::LIRO)_dstinfo->Mode=DPS::Producer::LILO;
       if(_dstinfo->Mode==DPS::Producer::RIRO)_dstinfo->Mode=DPS::Producer::RILO;
    }
    }
    struct timeb  ft;
    ftime(&ft);
    _ST0=ft.time+ft.millitm/1000.;
    if(_debug)cout <<"ST0 "<<_ST0<<endl;
    TIMEX(_T0);
    if(_debug)cout <<"T0 "<<_T0<<endl;
    IMessage(AMSClient::print(_reinfo," get reinfo "));
    IMessage(AMSClient::print(_dstinfo," get dstinfo "));
    InitTDV(_reinfo->uid);
    return;
  }
  catch  (CORBA::SystemException & a){
  }
 }
 if(getior("GetIorExec"))goto again;
 else FMessage("AMSProducer::getRunEventinfo-F-UnableToGetRunEvInfo",DPS::Client::CInAbort);
}

void AMSProducer::sendCurrentRunInfo(bool force){
if (_Solo)return;
_pid.threads_change=AMSEvent::get_num_threads()-_pid.threads;
_pid.threads=AMSEvent::get_num_threads();
if(_OnAir){
  EMessage("AMSProducer::sendCurrentrunInfo-W-AlreadyOnAir ");
 return;
}
//cout <<" sendcurrentinfo start "<<endl;
int failure=0;
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  try{
   if(!CORBA::is_nil(*li)){
    _OnAir=true;
    (*li)->sendCurrentInfo(_pid,_cinfo,0);
    _OnAir=false;
    break;
   }
  }
  catch  (CORBA::SystemException & a){
    _OnAir=false;
   failure++;
  }
}
if(failure)UpdateARS();

if(force){
  if(IOPA.hlun)sendNtupleUpdate(DPS::Producer::Ntuple);
  else if(IOPA.WriteRoot)sendNtupleUpdate(DPS::Producer::RootFile);
  if(DAQCFFKEY.mode/10>0)sendNtupleUpdate(DPS::Producer::RawFile);
}
//cout <<" sendcurrentinfo end "<<endl;
}

void AMSProducer::getASL(){
}


void AMSProducer::sendNtupleEnd(DPS::Producer::DSTType type,int entries, int last, time_t end, bool success){
cout <<" sendntupleend start "<<endl;
 _Transfer=true;

DPS::Producer::DST *ntend=getdst(type);
if(ntend){
ntend->crc=0;
if(success && end>0){
 ntend->Status=DPS::Producer::Success;
}
else {
  if(!end)cerr<<"AMSProducer::sendNtupleEnd-E-LastEventTimeZero "<<endl;
  ntend->Status=DPS::Producer::Failure;
}
//ntend->Status=success?(DPS::Producer::Success):(DPS::Producer::Failure);
ntend->EventNumber=entries;
if(last){
  ntend->LastEvent=last;
_cinfo.LastEventProcessed=last;
}
else ntend->LastEvent =_cinfo.LastEventProcessed;
ntend->End=end;
ntend->Type=type;
AMSmceventg::SaveSeeds();
ntend->rndm1=GCFLAG.NRNDM[0];
ntend->rndm2=GCFLAG.NRNDM[1];
//cout <<"  Sending Last Event " <<ntend->LastEvent<<" "<<ntend->rndm1<<" "<<ntend->rndm2<<endl;
cout<< " sending last event "<<ntend->LastEvent<<" "<<ntend->End<<endl;
if(ntend->End==0 || ntend->LastEvent==0)ntend->Status=DPS::Producer::Failure;
{
   AString a=(const char*)ntend->Name;
   int bstart=0;
   for (int i=0;i<a.length();i++){
    if(a[i]==':'){
     bstart=i+1;
     break;
    }
   }
   int bend=0;
   for (int i=a.length()-1;i>=0;i--){
    if(a[i]=='/'){
     bend=i+1;
     break;
    }
   }
   int bnt=bstart+(getenv("NtupleDir")?strlen(getenv("NtupleDir")):0);
   if(a.length()>bnt && a[bnt]=='/')bnt++;
   
 if(!_Solo)sendCurrentRunInfo();


// add crc to a local file (to save bandwitdh)

{


   struct stat64 statbuf;
    stat64((const char*)a(bstart), &statbuf);
      ntend->Insert=statbuf.st_ctime;
      ntend->size=statbuf.st_size/1024./1024.+0.5;
 

   if(!AMSTimeID::_Table){
     AMSTimeID::_InitTable;
   }
   ifstream fbin;
   fbin.open((const char*)a(bstart));
   uinteger crc=0;
   if(fbin){
         cout <<"SendNtupleEnd-I-AddingCRC "<<(const char*)a(bstart)<<endl;
          unsigned int chunk[65536]; 
         int i=0;
          long long fsize=statbuf.st_size;
         for(;;){
           if(!fsize) break;
           unsigned int myread=fsize>sizeof(chunk)?sizeof(chunk):fsize;
           fbin.read((char*)chunk,myread);
           fsize-=myread;
           if(fbin.good() && !fbin.eof()){
           int beg;
           if(i==0){
            crc=~chunk[0];
            beg=1;
           }
           else{
            beg=0;
           }
           if(i%8192==0){
               cout <<"SendNtupleEnd-I-AddingCRC "<<fsize/1024/1024<< " MB left"<<endl;
               if(!_Solo)sendCurrentRunInfo();
           }
           for(int m=beg;m<myread/sizeof(chunk[0]);m++){
            for(int j=0;j<3;j++)crc=AMSTimeID::_Table[crc>>24]^(crc<<8);
            crc=crc^chunk[m];  
           }
           i++;
          }
          else break;
         }
         fbin.close();
         ntend->crc=~crc;
   }
   else{
         cout <<"SendNtupleEnd-W-UnableToAddCRC "<<(const char*)a(bstart)<<endl;
   }
  if(!_Solo)sendCurrentRunInfo();
}



// add validation
if(type!=DPS::Producer::RawFile){
const char *exedir=getenv("ExeDir");
const char *nve=getenv("NtupleValidatorExec");
if(exedir && nve && AMSCommonsI::getosname()){
 AString systemc(exedir);
  systemc+="/";
  systemc+=AMSCommonsI::getosname();
  systemc+="/";
  systemc+=nve;
  systemc+=" ";
  systemc+=a(bstart);
  systemc+=" ";
  char tmp[80];
  sprintf(tmp,"%d",ntend->EventNumber);
  systemc+=tmp;
  if(IOPA.WriteRoot)systemc+=" 1 ";
  else systemc+=" 0 ";
  sprintf(tmp,"%d",ntend->LastEvent);
   systemc+=tmp; 
  cout<<"AMSProducer::sendNtupleEnd-I-ValidatingBy "<<systemc<<endl;
  int i=system(systemc);
  cout<<"AMSProducer::sendNtupleEnd-I-ValidatedBy "<<systemc<<" "<<i<<endl;
  if( (i == 0xff00) || (i & 0xff)){
// Unable To Check
   cerr<<"AMSProducer::sendNtupleEnd-E-UnableToValideNtupleBy "<<systemc<<endl;
  }
  else{
    i=(i>>8);
    if(i/128 && ( !AMSEvent::_checkUpdate())){
     cerr<<"  AMSProducer::sendNtupleEnd-E-Ntuple failure "<<i<<" "<< AMSEvent::_checkUpdate()<<endl;
     ntend->Status=DPS::Producer::Failure;
    }
    else{
     ntend->Status=DPS::Producer::Validated;
     ntend->ErrorNumber=int(i*ntend->EventNumber/100);
    }
  }
 }
 else if(!nve){
   cerr<<"AMSProducer::sendNtupleEnd-E-UnableToValideNtupleBecauseNtupleValidatorExecIsNull"<<endl;
 }
 else if(!exedir){
   cerr<<"AMSProducer::sendNtupleEnd-E-UnableToValideNtupleBecauseExeDirIsNull"<<endl;
 }
 else {
   cerr<<"AMSProducer::sendNtupleEnd-E-UnableToValideNtupleBecauseOsNameIsNull"<<endl;
 }
}
else{
     ntend->Status=DPS::Producer::Validated;
}


// Move ntuple to the dest directory
string destdirc="";
char *destdir=getenv("NtupleDestDir");
if(getenv("NtupleDir") && destdir && strcmp(destdir,getenv("NtupleDir"))){
 char *means=getenv("TransferBy");
 AString fmake;
 AString fcopy;
againcpmeans:
 if(means && ((means[0]=='r' && means[1]=='f') || strstr(means,"xrdcp")|| strstr(means,"rfcp"))){
                if(getenv("TransferSharedLib")){
                 setenv("LD_LIBRARY_PATH",getenv("TransferSharedLib"),1);
                }

  char local[]="/afs/cern.ch/ams/Offline/AMSDataDir";
   char *localbin=0;
   if(getenv("AMSDataDir"))localbin=getenv("AMSDataDir");
   else localbin=local;
   fmake=localbin;
   fmake+="/DataManagement/exe/linux/timeout --signal 9 9 ";
   fmake+="nsmkdir -p ";
  if(getenv("TransferMakeDir"))fmake=getenv("TransferMakeDir");
  fcopy=means;
  fcopy+=" ";
 }
 else if(means){
  fmake+="mkdir -p ";
  fcopy=means;
  fcopy+=" ";
 } 
 else{
  fmake="mkdir -p ";
  fcopy="cp ";
 }
  string rfio=destdir;
  if(rfio.find("/castor/cern.ch")!=-1){
    fmake+=rfio.c_str()+rfio.find("/castor/cern.ch");
  }
  else fmake+=rfio.c_str();
// fmake+='/';
// for (int k=bnt;k<bend;k++)fmake+=a[k];
 int imake=system((const char*)fmake);
 fcopy+=(const char*)a(bstart);
 fcopy+="  ";
 fcopy+=destdir; 
// fcopy+='/';
// for (int k=bnt;k<bend;k++)fcopy+=a[k];
int tmc=3600;
if(strstr((const char *)fcopy,"rfcp"))tmc=10800;
 if(!_Solo)sendid(tmc);
int ntry=3;
 bool suc=false;
 bool retryonce=false;
againcp:
 for(int j=0;j<ntry;j++){
  sleep(1<<(j+1));
  if(!_Solo)sendCurrentRunInfo();
  cout <<"SendNtupleEnd-I-StartCopyingDST "<<j<<" Try "<<(const char*)fcopy<<endl;
  if(!system((const char*)fcopy)){
   suc=true;
   cout <<"SendNtupleEnd-I-CopiedDSTSuccesfully "<<j<<" Try "<<(const char*)fcopy<<endl;
 // add more validation here
 //
    string file2v="";    
    string ff=(const char*)fcopy;
  if(ff.find("/castor/cern.ch")!=-1){
     file2v+="rfio:";
  }
  file2v+=destdir;
  file2v+='/';
  for (int k=bend;k<a.length();k++)file2v+=a[k];
if(type!=DPS::Producer::RawFile){
const char *exedir=getenv("ExeDir");
const char *nve=getenv("NtupleValidatorExec");
if(exedir && nve && AMSCommonsI::getosname()){
 AString systemc(exedir);
  systemc+="/";
  systemc+=AMSCommonsI::getosname();
  systemc+="/";
  systemc+=nve;
  systemc+=" ";
  systemc+=file2v.c_str();
  systemc+=" ";
  char tmp[80];
  sprintf(tmp,"%d",ntend->EventNumber);
  systemc+=tmp;
  if(IOPA.WriteRoot)systemc+=" 1 ";
  else systemc+=" 0 ";
  sprintf(tmp,"%d",ntend->LastEvent);
   systemc+=tmp; 
  cout<<"AMSProducer::sendNtupleEnd-I-ValidatingBy "<<systemc<<endl;
  int i=system(systemc);
  cout<<"AMSProducer::sendNtupleEnd-I-ValidatedBy "<<systemc<<" "<<i<<endl;
  if( (i == 0xff00) || (i & 0xff)){
// Unable To Check
   cerr<<"AMSProducer::sendNtupleEnd-E-UnableToValideNtupleBy "<<systemc<<endl;
  }
  else{
    i=(i>>8);
    if(i/128 && ( !AMSEvent::_checkUpdate())){
     cerr<<"  AMSProducer::sendNtupleEnd-E-Ntuple failure "<<i<<" "<< AMSEvent::_checkUpdate()<<endl;
     ntend->Status=DPS::Producer::Failure;
    }
    else{
     ntend->Status=DPS::Producer::Validated;
     ntend->ErrorNumber=int(i*ntend->EventNumber/100);
    }
  }
 }
 else if(!nve){
   cerr<<"AMSProducer::sendNtupleEnd-E-UnableToValideNtupleBecauseNtupleValidatorExecIsNull"<<endl;
 }
 else if(!exedir){
   cerr<<"AMSProducer::sendNtupleEnd-E-UnableToValideNtupleBecauseExeDirIsNull"<<endl;
 }
 else {
   cerr<<"AMSProducer::sendNtupleEnd-E-UnableToValideNtupleBecauseOsNameIsNull"<<endl;
 }
if(ntend->Status==DPS::Producer::Failure){
if(!retryonce){
retryonce=true;
cerr<<"AMSProducer::sendNtupleEnd-E-2ndValidationFailedTrytoRecopy"<<endl;
goto againcp;
}
}
}



        
  if(!_Solo)sendCurrentRunInfo();
   if((_Solo || !(_dstinfo->Mode==DPS::Producer::LIRO || _dstinfo->Mode==DPS::Producer::RIRO)) &&
    1){
    AString rm="rm -rf ";
    rm+=a(bstart);
    system((const char*)rm);
    cout <<"SendNtupleEnd-I-DeletingDSTBy "<<(const char*)rm<<endl;
     struct statfs64 buffer;
     int fail=statfs64((const char*)destdir, &buffer);
    if(fail){
      ntend->FreeSpace=-1;
      ntend->TotalSpace=-1;
    }
    else{
     ntend->FreeSpace= (buffer.f_bavail*(buffer.f_bsize/1024.))/1024;
     ntend->TotalSpace= (buffer.f_blocks*(buffer.f_bsize/1024.))/1024;
    }

    _FreeSpace=ntend->FreeSpace;
    AString b="";
    for(int k=0;k<bstart;k++)b+=a[k];
    b+=destdir;
    b+="/";
    //b+=a(bnt);
    b+=a(bend);
    a=b;
    ntend->Name=(const char*)a;
   }
   break;
  }
 }
 if(!suc && means){
   if(!_Solo)sendid(3600);
   cerr <<"SendNtupleEnd-E-UnabletoCopyDSTSuccesfully "<<" Tried "<<(const char*)fcopy<<endl;
  if(getenv("TransferRawByB") && strlen(getenv("TransferRawByB"))){
    setenv("TransferBy",getenv("TransferRawByB"),1);
    unsetenv("TransferRawByB");
    means=getenv("TransferBy");
    goto againcpmeans;
  }
  
  char *nd2=getenv("NtupleDestDirBackup");
  char *nd20=getenv("NtupleDestDir00");
  char *td2=getenv("TransferRawBy2");
  means=NULL;
  if(nd2 &&strlen(nd2)){
   
   char tmp[1024];
   sprintf(tmp,"%s/%d.%d",nd2,_pid.uid,_pid.pid);
    destdirc=tmp; 
   unsetenv("NtupleDestDirBackup");
   fmake="mkdir -p ";
   fmake+=tmp;  
   int i=system((const char*)fmake);
   if(!i){
   fcopy="cp ";
   fcopy+=(const char*)a(bstart);
   fcopy+="  ";
   fcopy+=tmp; 
//   destdir=nd2;
   destdir=destdirc.c_str(); 
   goto againcp;
   }
   else{
   cerr <<"SendNtupleEnd-E-UnabletoMakeDirBy "<<(const char*)fmake<<endl;
   }
  }
  else if(nd20 &&strlen(nd20)){
    char hostdefault[]="ams.cern.ch";
    char *host=getenv("AMSRescueHost")?getenv("AMSRescueHost"):hostdefault;
    fmake="ssh ";
    fmake+=host;
    fmake+=" mkdir -p ";
    fmake+=nd20;
   int i=system((const char*)fmake);
   if(!i){
   fcopy="scp ";
   fcopy+=(const char*)a(bstart);
   fcopy+=" ";
   fcopy+=host;    
   fcopy+=":";
   fcopy+=nd20; 
   suc=true;
   destdir=nd20;
   goto againcp;
   }
   else{
   cerr <<"SendNtupleEnd-E-UnabletoMakeDirBy "<<(const char*)fmake<<endl;
   }
  

  }
  
 }
}

  if(!_Solo)sendCurrentRunInfo();





char *means=getenv("TransferBy");

 if(!means){
 
    struct stat64 statbuf;
    stat64((const char*)a(bstart), &statbuf);

ntend->Insert=statbuf.st_ctime;
ntend->size=statbuf.st_size/1024./1024.+0.5;
}
ntend->ErrorNumber=0;

  if(!_Solo)sendCurrentRunInfo();

/*
//add crc
   if(!AMSTimeID::_Table){
     AMSTimeID::_InitTable;
   }
   ifstream fbin;
   sleep(1);
   fbin.open((const char*)a(bstart));
   uinteger crc=0;
   if(fbin){
         cout <<"SendNtupleEnd-I-AddingCRC "<<(const char*)a(bstart)<<endl;
          unsigned int chunk[65536]; 
         int i=0;
          long long fsize=statbuf.st_size;
         for(;;){
           if(!fsize) break;
           unsigned int myread=fsize>sizeof(chunk)?sizeof(chunk):fsize;
           fbin.read((char*)chunk,myread);
           fsize-=myread;
           if(fbin.good() && !fbin.eof()){
           int beg;
           if(i==0){
            crc=~chunk[0];
            beg=1;
           }
           else{
            beg=0;
           }
           if(i%4096==0){
               cout <<"SendNtupleEnd-I-AddingCRC "<<fsize/1024/1024<< " MB left"<<endl;
               if(!_Solo)sendCurrentRunInfo();
           }
           for(int m=beg;m<myread/sizeof(chunk[0]);m++){
            for(int j=0;j<3;j++)crc=AMSTimeID::_Table[crc>>24]^(crc<<8);
            crc=crc^chunk[m];  
           }
           i++;
          }
          else break;
         }
         fbin.close();
         ntend->crc=~crc;
   }
  if(!_Solo)sendCurrentRunInfo();

*/


}


LMessage(AMSClient::print(*ntend,"CloseDST"));

cout << " nt end " <<ntend->Insert<<" "<<ntend->Begin<<" "<<ntend->End<<endl;
 if(!_Solo)sendid(int(6*AMSFFKEY.CpuLimit));
 _Transfer=false;
if(_Solo){
  return;
}
UpdateARS();
sendDSTInfo();

if(_dstinfo->Mode==DPS::Producer::LIRO || _dstinfo->Mode==DPS::Producer::RIRO){
aga1:
for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  try{
   if(!CORBA::is_nil(*li)){
    _OnAir=true;
    (*li)->sendDSTEnd(_pid,*ntend,DPS::Client::Delete);
    _OnAir=false;
     break;
   }
  }
  catch  (CORBA::SystemException & a){
    _OnAir=false;
 if(getior("GetIorExec"))goto aga1;
 else FMessage("AMSProducer::sendNtupleEnd-F-UnableToDeleteNtuple ",DPS::Client::CInAbort);
  }
}
   AString a=(const char*)ntend->Name;
   int bstart=0;
   for (int i=0;i<a.length();i++){
    if(a[i]==':'){
     bstart=i+1;
     break;
    }
   }
   int start=0;
   for (int i=a.length()-1;i>=0;i--){
    if(a[i]=='/'){
     start=i+1;
     break;
    }
   }
    DPS::Producer::FPath fpath;
    fpath.fname=(const char*)a(start);    
    fpath.pos=0;
    cout <<" file name "<<a<<" "<<start<<endl;    
    struct stat64 statbuf;
    int suc=stat64((const char*)a(bstart), &statbuf);
    if(suc){
      FMessage("AMSProducer::sendNtupleEnd-F-UnableToStatNtuplein mode RO ",DPS::Client::CInAbort);
     }

   ifstream fbin;
   fbin.open((const char*)a(bstart));
   if(fbin){
    DPS::Producer::TransferStatus st=DPS::Producer::Begin;
    const int maxs=2000000;
     DPS::Producer::RUN_var vrun=new DPS::Producer::RUN();
    while(st !=DPS::Producer::End){
     long long last=statbuf.st_size-fpath.pos;
     if(last>maxs)last=maxs;
     else st=DPS::Producer::End;
     vrun->length(last);
     fbin.read(( char*)vrun->get_buffer(),last);
     if(!fbin.good()){
      FMessage("AMSProducer::sendNtupleEnd-F-UnableToReadNtuplein mode RO ",DPS::Client::CInAbort);
     }
      for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
      try{
       _OnAir=true;
       (*li)->sendFile(_pid,fpath,vrun,st);
       _OnAir=false;
        break;
       }
       catch (DPS::Producer::FailedOp & a){
        _OnAir=false;
        FMessage((const char *)a.message,DPS::Client::SInAbort);
       }
       catch  (CORBA::SystemException & a){
       _OnAir=false;
       FMessage("AMSProducer::sendNtupleEnd-F-UnableToSendNtupleBody ",DPS::Client::CInAbort);
       }
     }
     fpath.pos+=last;
      if(st==DPS::Producer::Begin)st=DPS::Producer::Continue;
    }
     fbin.close();
     unlink( ((const char*)a(bstart)));
      a=(const char*)_pid.HostName;
//     a+=":REMOTE:";
       a+=":";
     a+=(const char*)fpath.fname;
     ntend->Name=(const char *)a;
     if(ntend->Status !=DPS::Producer::Failure)ntend->Status=DPS::Producer::Success;
     again:
     for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
      try{
       if(!CORBA::is_nil(*li)){
        _OnAir=true;
        (*li)->sendDSTEnd(_pid,*ntend,DPS::Client::Create);
         _OnAir=false;
        return;
       }
      }
      catch  (CORBA::SystemException & a){
      _OnAir=false;
     }
    }
 if(getior("GetIorExec"))goto again;
 else FMessage("AMSProducer::sendRunEnd-F-UnableToSendNtupleEndInfo ",DPS::Client::CInAbort);
}
   else FMessage("AMSProducer::sendNtupleEnd-F-UnableToSendNtuplein mode RO ",DPS::Client::CInAbort);

  }

 




else{
 again1:
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  try{
   if(!CORBA::is_nil(*li)){
    _OnAir=true;
    (*li)->sendDSTEnd(_pid,*ntend,DPS::Client::Update);
    _OnAir=false;
     if( ntend->Status==DPS::Producer::Failure)FMessage("Ntuple Failure",DPS::Client::CInAbort);
     return;
   }
  }
  catch  (CORBA::SystemException & a){
    _OnAir=false;
  }
}
 if(getior("GetIorExec"))goto again1;
 else FMessage("AMSProducer::sendNtupleEnd-F-UnableToSendNtupleEndInfo ",DPS::Client::CInAbort);
}
}
else{
FMessage("AMSProducer::sendNtupleEnd-F-UNknownDSTType ",DPS::Client::CInAbort);
}  
}



void AMSProducer::sendNtupleStart(DPS::Producer::DSTType type,const char * name, int run, int first,time_t begin){
DPS::Producer::DST *ntend=getdst(type);
if(ntend){
AString a=(const char*)_pid.HostName;
a+=":";
a+=name;
ntend->Name=(const char *)a;
a=AMSCommonsI::getversion();
char tmp[80];
sprintf(tmp,"%d",AMSCommonsI::getbuildno());
a+="/build";
a+=tmp;
sprintf(tmp,"%d",AMSCommonsI::getosno());
a+="/os";
a+=tmp;
ntend->Version=(const char*)a;
ntend->Run=run;
ntend->crc=0;
ntend->FirstEvent=first;
ntend->Begin=begin;
time_t tt;
time(&tt);
ntend->Insert=tt;
ntend->End=0;
ntend->LastEvent=0;
ntend->EventNumber=0;
ntend->ErrorNumber=0;
ntend->Status=DPS::Producer::InProgress;
ntend->Type=type;
ntend->size=0;
     struct statfs64 buffer;
     int fail=statfs64((const char *)name, &buffer);
    if(fail){
      ntend->FreeSpace=-1;
      ntend->TotalSpace=-1;
    }
    else{
     ntend->FreeSpace= (buffer.f_bavail*(buffer.f_bsize/1024.))/1024;
     ntend->TotalSpace= (buffer.f_blocks*(buffer.f_bsize/1024.))/1024;
    }
    _FreeSpace=ntend->FreeSpace;
cout <<" sendntuplestart start "<<name<<" "<<ntend->FreeSpace<<" "<<ntend->TotalSpace<<endl;



LMessage(AMSClient::print(*ntend,"OpenDST"));
if(_Solo)return;
UpdateARS();


sendDSTInfo();
again:
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  try{
   if(!CORBA::is_nil(*li)){
    _OnAir=true;
    (*li)->sendDSTEnd(_pid,*ntend,DPS::Client::Create);
    _OnAir=false;
     goto checkdd;
   }
  }
  catch  (CORBA::SystemException & a){
    _OnAir=false;
  }
}
 if(getior("GetIorExec"))goto again;
else FMessage("AMSProducer::sendNtupleStart-F-UnableToSendNtupleStartInfo ",DPS::Client::CInAbort);
}
else{
FMessage("AMSProducer::sendNtupleEnd-F-UNknownDSTType ",DPS::Client::CInAbort);
}


checkdd:
{
   AString a=(const char*)ntend->Name;
   int bstart=0;
   for (int i=0;i<a.length();i++){
    if(a[i]==':'){
     bstart=i+1;
     break;
    }
   }
   int bend=0;
   for (int i=a.length()-1;i>=0;i--){
    if(a[i]=='/'){
     bend=i+1;
     break;
    }
   }
   int bnt=bstart+(getenv("NtupleDir")?strlen(getenv("NtupleDir")):0);
   if(a.length()>bnt && a[bnt]=='/')bnt++;
   

castortry:

// check if dd writeable 

char *destdir=getenv("NtupleDestDir");
if(getenv("NtupleDir") && destdir && strcmp(destdir,getenv("NtupleDir"))){

   AString a=(const char*)ntend->Name;
   int bstart=0;
   for (int i=0;i<a.length();i++){
    if(a[i]==':'){
     bstart=i+1;
     break;
    }
   }
   int bend=0;
   for (int i=a.length()-1;i>=0;i--){
    if(a[i]=='/'){
     bend=i+1;
     break;
    }
   }
   int bnt=bstart+getenv("NtupleDir")?strlen(getenv("NtupleDir")):0;
   if(a.length()>bnt && a[bnt]=='/')bnt++;
   
 char *means=getenv("TransferBy");
 AString fmake;
 if(means && ((means[0]=='r' && means[1]=='f') || strstr(means,"xrdcp")|| strstr(means,"rfcp"))){
                if(getenv("TransferSharedLib")){
                 setenv("LD_LIBRARY_PATH",getenv("TransferSharedLib"),1);
                }
  char local[]="/afs/cern.ch/ams/Offline/AMSDataDir";
   char *localbin=0;
   if(getenv("AMSDataDir"))localbin=getenv("AMSDataDir");
   else localbin=local;
   fmake=localbin;
   fmake+="/DataManagement/exe/linux/timeout --signal 9 9 ";
   fmake+="nsmkdir -p ";
  if(getenv("TransferMakeDir"))fmake=getenv("TransferMakeDir");
  string rfio=destdir;
  if(rfio.find("/castor/cern.ch")!=-1){
    fmake+=rfio.c_str()+rfio.find("/castor/cern.ch");
  }
  else fmake+=rfio.c_str();
 }
 else{
  fmake="mkdir -p ";
  fmake+=destdir;
 }
   cout <<"SendNtupleStart-I-MakingDestDir "<<(const char*)fmake<<endl;

 if(system((const char*)fmake)){
       cerr<<"AMSProducer::sendNtupleStart-E-Unable create dst dest  dir "<<fmake<<endl;
        if(!strstr((const char*)destdir,"/castor/cern.ch/ams")){
           int bstart=0;
           for(int i=1;i<strlen(destdir);i++){
            if(destdir[i]=='/'){
                bstart=i;
                break;
            }
           }
           if(bstart){
             setenv("TransferBy",getenv("TransferRawBy")?getenv("TransferRawBy"):"rfcp ",1);
             AString newdd="";
             if(strstr(getenv("TransferBy"),"root:")){
             string cc=getenv("TransferBy");
              newdd=cc.c_str()+cc.rfind("root:"); 

              cc[cc.rfind("root:")]='\0'; 
              setenv("TransferBy",cc.c_str(),1);
              cout <<"AMSProducer::sendNtupleStart-I-TransferBy SetTo "<<getenv("TransferBy")<<endl;
             }
             newdd+="/castor/cern.ch/ams";
             newdd+=destdir+bstart;
             setenv("NtupleDestDir00",getenv("NtupleDestDir"),1);
             setenv("NtupleDestDir",(const char*)newdd,1);
             setenv("STAGE_HOST","castorpublic",1);
             setenv("RFIO_USE_CASTOR_V2","YES",1);
             setenv("STAGE_SVCCLASS","amscdr",1);
             setenv("CASTOR_INSTANCE","castorpublic",1);
             goto castortry;
            }
          }
       FMessage("Unable create dst dest  dir ", DPS::Client::CInAbort); 
 }
}
}
}

void AMSProducer::sendNtupleUpdate(DPS::Producer::DSTType type){
if(_Solo)return;
DPS::Producer::DST *ntend=getdst(type);
if(ntend){
ntend->Status=DPS::Producer::InProgress;
   AString a=(const char*)ntend->Name;
   int bstart=0;
   for (int i=0;i<a.length();i++){
    if(a[i]==':'){
     bstart=i+1;
     break;
    }
   }
    struct stat64 statbuf;
    stat64((const char*)a(bstart), &statbuf);
    

ntend->Insert=statbuf.st_ctime;
ntend->size=statbuf.st_size/1024./1024.+0.5;


     struct statfs64 buffer;
     int fail=statfs64((const char*)a(bstart), &buffer);
    if(fail){
      ntend->FreeSpace=-1;
      ntend->TotalSpace=-1;
    }
    else{
     ntend->FreeSpace= (buffer.f_bavail*(buffer.f_bsize/1024.))/1024;
     ntend->TotalSpace= (buffer.f_blocks*(buffer.f_bsize/1024.))/1024;
    }
    _FreeSpace=ntend->FreeSpace;

//cout <<" sendntupleupdate start "<<a(bstart)<<" "<<ntend->FreeSpace<<" "<<ntend->TotalSpace<<endl;

UpdateARS();


sendDSTInfo();
cout <<" sendntupleupdate middle "<<endl;
again:
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  try{
   if(!CORBA::is_nil(*li)){
    _OnAir=true;
    (*li)->sendDSTEnd(_pid,*ntend,DPS::Client::Update);
    _OnAir=false;
    cout <<" sendntupleupdate end "<<endl;
    return;
   }
  }
  catch  (CORBA::SystemException & a){
    _OnAir=false;
  }
}
 if(getior("GetIorExec"))goto again;
else FMessage("AMSProducer::sendNtupleUpdate-F-UnableToSendNtupleStartInfo ",DPS::Client::CInAbort);
}
else{
FMessage("AMSProducer::sendNtupleUpdate-F-UNknownDSTType ",DPS::Client::CInAbort);
}  

}

void AMSProducer::Exiting(const char * message){
if(_Solo)return;
if(_ExitInProgress)return;
cout<< " Exiting ...."<<(message?message:" ")<<endl;
_ExitInProgress=true;
_pid.Status=AMSClient::_error.ExitReason();
for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
try{
if(!CORBA::is_nil(*li)){
    _OnAir=true;
(*li)->Exiting(_pid,(message?message:AMSClient::_error.getMessage()),AMSClient::_error.ExitReason());
    _OnAir=false;
cout <<" exiting ok"<<endl;
break;
}
}
catch  (CORBA::SystemException & a){}
    _OnAir=false;
}
if(DAQCFFKEY.BTypeInDAQ[0]<=5){
char tmp[256];
sprintf(tmp," (sleep 60 ; bkill %u )&",_pid.pid);
system(tmp);
sprintf(tmp," (sleep 60 ; kill -9 %u )&",_pid.pid);
system(tmp);
cout <<" Exiting-I-Bkill "<<tmp<<endl;
}
else{
char tmp[256];
sprintf(tmp," (sleep 7200 ; bkill %u )&",_pid.pid);
system(tmp);
sprintf(tmp," (sleep 7200 ; kill -9 %u )&",_pid.pid);
system(tmp);
cout <<" Exiting-I-Bkill "<<tmp<<endl;
}


}


void AMSProducer::UpdateARS(){
if(AMSJob::gethead()->isSimulation()){
 return;
}
for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
try{
if(!CORBA::is_nil(*li)){
     DPS::Client::ARS * pars;
    _OnAir=true;
     int length=(*li)->getARS(_pid,pars,DPS::Client::Any,0,true);
    _OnAir=false;
     DPS::Client::ARS_var ars=pars;
     if(length==0){
      FMessage("getARS-S-UnableToGetARS ",DPS::Client::CInAbort);
     }   
     if(__MT){
     for(int i=0;i<length;i++){
        CORBA::Object_var obj=_orb->string_to_object((ars.operator[](i)).IOR);
        if(!CORBA::is_nil(obj)){
        DPS::Producer_var _pvar=DPS::Producer::_narrow(obj);
        if(!CORBA::is_nil(_pvar)){
         if(i==0)_plist.clear();
         _plist.push_back(_pvar);
        }
        }
       }
     }
     else{
      bool LocalHostFound=false;
     for(int i=0;i<length;i++){
        CORBA::Object_var obj=_orb->string_to_object((ars.operator[](i)).IOR);
        if(!CORBA::is_nil(obj)){
        DPS::Producer_var _pvar=DPS::Producer::_narrow(obj);
        if(!CORBA::is_nil(_pvar)){
         if(i==0)_plist.clear();
         double r=double(rand())/RAND_MAX;
          try{
           DPS::Client::CID * pcid;       
             _OnAir=true;
             _pvar->getId(pcid);
             _OnAir=false;
           DPS::Client::CID_var cid=pcid;
            if(strstr((const char *)cid->HostName, (const char *)_pid.HostName)){
              if(!LocalHostFound){
#ifdef __AMSDEBUG__
              cout <<" LocalHostFound "<<cid->HostName<<endl;
#endif
                r=1;
                LocalHostFound=true;
              }
            }
            else if(LocalHostFound)r=0;
          }
          catch  (CORBA::SystemException & a){
            _OnAir=false;
            r=0;
          }
         if(r>0.5)_plist.push_front(_pvar);
         else _plist.push_back(_pvar);
        }
        }
       }
     }
      break;
}
}
catch  (CORBA::SystemException & a){}
    _OnAir=false;
}

}


void AMSProducer::sendRunEnd(DAQEvent::InitResult res){
if(_dstinfo->Mode ==DPS::Producer::LILO || _dstinfo->Mode==DPS::Producer::LIRO){
unlink( DAQEvent::getfile());
}
     if(res!=DAQEvent::OK && (res!=DAQEvent::NoInputFile || _cinfo.LastEventProcessed==0) ){
 _cinfo.Status=DPS::Producer::Failed;
if(_reinfo->Run!=0){
    FMessage("AMSProducer::sendRunEnd-F-RunFailed ",DPS::Client::CInAbort);
}
else{
    FMessage("AMSProducer::sendRunEnd-I-RunFinished ",DPS::Client::CInAbort);
}
}
else _cinfo.Status=DPS::Producer::Finished;

// check event numbers
   
    int fail=1;
    if(1){
      fail=(_reinfo->LastEvent-_reinfo->FirstEvent+1)/(_cinfo.EventsProcessed+1)*10;      
     if(fail<1)fail=1;
    }
    if(-_cinfo.LastEventProcessed+_reinfo->LastEvent>fail){
       _cinfo.Status=DPS::Producer::Failed;
        cerr<<"AMSProducer::sendRunEnd-S-NotAllEvetnsProcessed "<<_cinfo.LastEventProcessed<<" "<<_reinfo->LastEvent<<" "<<endl;
      FMessage("AMSProducer::sendRunEnd-F-RunFailed ",DPS::Client::CInAbort);

    }
    else if(_cinfo.LastEventProcessed-_reinfo->LastEvent>1){
        cerr<<"AMSProducer::sendRunEnd-W-TooManyEventsProcessed "<<_cinfo.LastEventProcessed<<" "<<_reinfo->LastEvent<<" "<<endl;
    }
    struct timeb  ft;
    ftime(&ft);
    double st=ft.time+ft.millitm/1000.;
_cinfo.TimeSpent=st-_ST0;

TIMEX(_cinfo.CPUTimeSpent);
_cinfo.CPUTimeSpent=_cinfo.CPUTimeSpent-_T0;
_cinfo.CPUMipsTimeSpent=_CPUMipsTimeSpent+(_cinfo.CPUTimeSpent)*_cinfo.Mips/1000;

        LMessage(AMSClient::print(_cinfo," RunFinished"));
        if(_Solo)return;
UpdateARS();
again:
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  try{
   if(!CORBA::is_nil(*li)){
    _OnAir=true;
    (*li)->sendCurrentInfo(_pid,_cinfo,0);
    _OnAir=false;

     return;
   }
  }
  catch  (CORBA::SystemException & a){
    _OnAir=false;
  }
}
 if(getior("GetIorExec"))goto again;
else FMessage("AMSProducer::sendRunEnd-F-UnableToSendRunEndInfo ",DPS::Client::CInAbort);


}
void AMSProducer::sendRunEndMC(){
if (!_Head)return;
double error=5./sqrt(double(GCFLAG.IDEVT+1));
if (error<0.01)error=0.01;
if(error>0.5)error=0.5;
if(DAQCFFKEY.mode/10>0)error=1;
if(GCFLAG.NEVENT*(1-error) > GCFLAG.IEVENT+1 || GCFLAG.NEVENT==0 || GCFLAG.IEVENT<2){
_cinfo.Status= DPS::Producer::Failed;
}
else _cinfo.Status= DPS::Producer::Finished;
    struct timeb  ft;
    ftime(&ft);
    double st=ft.time+ft.millitm/1000.;
_cinfo.TimeSpent=st-_ST0;

TIMEX(_cinfo.CPUTimeSpent);
_cinfo.CPUTimeSpent=_cinfo.CPUTimeSpent-_T0;
_cinfo.CPUMipsTimeSpent=_CPUMipsTimeSpent+(_cinfo.CPUTimeSpent)*_cinfo.Mips/1000;

        if(_cinfo.Status!= DPS::Producer::Finished){
         LMessage(AMSClient::print(_cinfo," RunIncomplete"));
         FMessage("RunIncomplete ", DPS::Client::CInAbort); 
        }
        else LMessage(AMSClient::print(_cinfo," RunFinished"));
        if(_Solo)return;
UpdateARS();
again:
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  try{
   if(!CORBA::is_nil(*li)){
    _OnAir=true;
    (*li)->sendCurrentInfo(_pid,_cinfo,0);
    _OnAir=false;

     return;
   }
  }
  catch  (CORBA::SystemException & a){
    _OnAir=false;
  }
}
 if(getior("GetIorExec"))goto again;
else FMessage("AMSProducer::sendRunEndMC-F-UnableToSendRunEndInfo ",DPS::Client::CInAbort);


}

void AMSProducer::AddEvent(){
if(_cinfo.Run == AMSEvent::gethead()->getrun()){
 _cinfo.EventsProcessed++;
 _cinfo.LastEventProcessed=AMSEvent::gethead()->getmid();
  if(!AMSEvent::gethead()->HasNoErrors())_cinfo.ErrorsFound++;
  if(!AMSEvent::gethead()->HasNoCriticalErrors())_cinfo.CriticalErrorsFound++;
}
    struct timeb  ft;
    ftime(&ft);
    double st=ft.time+ft.millitm/1000.;
double cll=AMSFFKEY.CpuLimit;
if(cll<20)cll=20;
if(AMSEvent::gethead()->HasFatalErrors()){
 FMessage("AMSProducer::AddEvent-F-EventHasFatalError ",DPS::Client::CInAbort);
}
else if(!(AMSEvent::gethead()->HasNoCriticalErrors())){
  TIMEX(_cinfo.CPUTimeSpent);
  _cinfo.CPUTimeSpent=_cinfo.CPUTimeSpent-_T0;
_cinfo.CPUMipsTimeSpent=_CPUMipsTimeSpent+(_cinfo.CPUTimeSpent)*_cinfo.Mips/1000;

    _cinfo.TimeSpent=st-_ST0;

  sendCurrentRunInfo();
}
else if(_cinfo.EventsProcessed%_dstinfo->UpdateFreq==1 || st-_ST0-_cinfo.TimeSpent>cll){
  TIMEX(_cinfo.CPUTimeSpent);
  _cinfo.CPUTimeSpent=_cinfo.CPUTimeSpent-_T0;
_cinfo.CPUMipsTimeSpent=_CPUMipsTimeSpent+(_cinfo.CPUTimeSpent)*_cinfo.Mips/1000;

    _cinfo.TimeSpent=st-_ST0;
  cout <<"  sending info "<<endl;
  sendCurrentRunInfo(true);
}
//cout <<" _cinfo.EventsProcessed "<<_cinfo.EventsProcessed<<" "<<_dstinfo->UpdateFreq<<" "<<st<<" "<<_ST0<<" "<<_cinfo.TimeSpent<<" "<<cll<<endl;
}

AMSProducer::~AMSProducer(){
const char * a=0;
cout <<" ams producer destructor called "<<endl;

Exiting(_up?_up->getMessage():a);
cout <<"  mallopt set "<<endl;
mallopt(M_CHECK_ACTION,0);
}


void AMSProducer::InitTDV( uinteger run){
AMSTimeID * head=AMSJob::gethead()->gettimestructure();
for (AMSTimeID * pser=dynamic_cast<AMSTimeID*>(head->down());pser;pser=dynamic_cast<AMSTimeID*>(pser->next())){
DPS::Producer::TDVName a;
if (!pser->Verify())continue;
a.Name=pser->getname();
a.DataMC=pser->getid();
if(pser->Variable())a.Size=-pser->GetNbytesM()+2*sizeof(uinteger);
else a.Size=pser->GetNbytes();
a.CRC=pser->getCRC();
a.File="";
time_t i,b,e;
pser->gettime(i,b,e);
a.Entry.id=0;
a.Entry.Insert=i;
a.Entry.Begin=b;
a.Entry.End=e;
DPS::Producer::TDVTable * ptdv;
IMessage(AMSClient::print(a," INITDV "));
 int suc=0;
 int length;
again:
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  
  try{
    _OnAir=true;
    length=(*li)->getTDVTable(_pid,a,run,ptdv);
    _OnAir=false;
    suc++;
    break;
  }
  catch  (CORBA::SystemException & a){
    _OnAir=false;
  }
 }
if(!suc){
 if(getior("GetIorExec"))goto again;
 else FMessage("AMSProducer::getinitTDV-F-UnableTogetTDVTable",DPS::Client::CInAbort);
}
DPS::Producer::TDVTable_var tvar=ptdv;
uinteger *ibe[5];

if(a.Success){
for(int i=0;i<5;i++){
 ibe[i]=new uinteger[length];
}
 for(int j=0;j<length;j++){
    ibe[0][j]=tvar.operator[](j).id;
    ibe[1][j]=tvar.operator[](j).Insert;
    ibe[2][j]=tvar.operator[](j).Begin;
    ibe[3][j]=tvar.operator[](j).End;
  }
  pser->fillDB(length,ibe);
}
else{
FMessage("AMSProducer::getinitTDV-F-tdvgetFailed",DPS::Client::CInAbort);
}
}
}

bool AMSProducer::getTDV(AMSTimeID * tdv, int id){
cout <<" trying to get tdv "<<tdv->getname()<<" "<<tdv->getid()<<endl;
DPS::Producer::TDVbody * pbody;
DPS::Producer::TDVName name;
name.Name=tdv->getname();
name.DataMC=tdv->getid();
name.CRC=tdv->getCRC();
name.Size=tdv->GetNbytes();
name.Entry.id=id;
name.File="";
time_t i,b,e;
tdv->gettime(i,b,e);
name.Entry.Insert=i;
name.Entry.Begin=b;
name.Entry.End=e;
cout <<" time ibe "<<i<<" "<<b<<" "<<e<<endl;
 int length=0;
 int suc=0;
 bool oncemore=false;
 bool exhausted=false;
again:
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  
  try{
    _OnAir=true;
    length=(*li)->getTDV(_pid,name,pbody);
    _OnAir=false;
    suc++;
    break;
  }
  catch  (const CORBA::SystemException &  a){
     cerr<< "Problems with TDV "<<endl;
//     cerr<< "Problems with TDV "<< a._orbitcpp_get_repoid()<<endl;
    _OnAir=false;
  }
 }
if(!suc){
 if(oncemore){
 if(getior("GetIorExec") && !exhausted){
    exhausted=true;
    goto again;
 }
  else FMessage("AMSProducer::getTDV-F-UnableTogetTDV",DPS::Client::CInAbort);
  return false;
 }
 else{
  oncemore=true;
  sleep(5);
   UpdateARS();
  goto again;
 }
}
DPS::Producer::TDVbody_var vbody=pbody;
if(name.Success){
if(tdv->Variable()){
tdv->SetNbytes(length*sizeof(integer));
}

 int nb=tdv->CopyIn(vbody->get_buffer());
 if(nb){
  tdv->SetTime(name.Entry.Insert,name.Entry.Begin,name.Entry.End);
  return true;
 }
}
return false;
}
bool AMSProducer::getSplitTDV(AMSTimeID * tdv, int id){
cout <<" trying to get split tdv "<<tdv->getname()<<" "<<tdv->getid()<<" "<<id<<endl;
DPS::Producer::TDVbody * pbody;
DPS::Producer::TDVName name;
name.Name=tdv->getname();
name.DataMC=tdv->getid();
name.CRC=tdv->getCRC();
if(tdv->Variable()){
 cout<<" AMSProducer::getSplitTDV-IVarTDV found "<<tdv->getname()<<" "<<tdv->GetNbytes()<<" "<<tdv->getfile()<<endl;
}
 name.Size=tdv->GetNbytes();
name.Entry.id=id;
name.File="";
time_t i,b,e;
tdv->gettime(i,b,e);
name.Entry.Insert=i;
name.Entry.Begin=b;
name.Entry.End=e;
 int length=0;
 uinteger pos=0;
 DPS::Producer::TransferStatus st=DPS::Producer::Begin;
 DPS::Producer::TDVbody_var vb2=new DPS::Producer::TDVbody();
 uinteger totallength=0;
 while (st!=DPS::Producer::End){
 int suc=0;
 bool exausted=false;
 again:
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  
  try{
    _OnAir=true;
    length=(*li)->getSplitTDV(_pid,pos,name,pbody,st);
    _OnAir=false;
    suc++;
    break;
  }
  catch  (CORBA::SystemException & a){
    _OnAir=false;
  }
}
if(!suc){
 if(getior("GetIorExec")&& !exausted){
   exausted=true;
   goto again;
}
 else FMessage("AMSProducer::getSplitTDV-F-UnableTogetTDV",DPS::Client::CInAbort);
 return false;
}
if(!totallength){
  vb2=pbody;
}
else{
 DPS::Producer::TDVbody_var vbody=pbody;
 vb2->length(totallength+length);
 for(int i=0;i<length;i++){
  vb2[i+totallength]=vbody.operator[](i);
 }
}
totallength+=length;
}
if(name.Success){
cout <<"AMSProducer::getSplitTDV-I-get time ibe "<<i<<" "<<b<<" "<<e<<" "<<totallength*sizeof(integer)<<" "<<tdv->GetNbytes()<<endl;
if(tdv->Variable()){
tdv->SetNbytes(totallength*sizeof(integer));
}
 int nb=tdv->CopyIn(vb2->get_buffer());
tdv->setfile((const char*)name.File);
cout <<"TDVFileNameRead "<<tdv->getfile()<<endl; 
 if(nb){
  tdv->SetTime(name.Entry.Insert,name.Entry.Begin,name.Entry.End);
  cout <<"  gettdv success "<<" "<<name.Entry.Begin<<" "<<name.Entry.End<<endl;
  return true;
 }
}
  cout <<"  gettdv failure "<<endl;
FMessage("AMSProducer::sendTDV-F-UnableTogetTDV",DPS::Client::CInAbort);
return false;
}

bool AMSProducer::sendTDV(const AMSTimeID * tdv){
cout <<" Send tdv get for "<<tdv->getname()<<endl;
        if(_Solo){
cerr <<" SendTDV-F-SoloModeDetectedWhileSendTDV "<<tdv->getname()<<endl;
abort();
}

DPS::Producer::TDVName name;
name.Name=tdv->getname();
name.DataMC=tdv->getid();
name.CRC=tdv->getCRC();
if(tdv->Variable())name.Size=-tdv->GetNbytesM()+2*sizeof(uinteger);
else name.Size=tdv->GetNbytes();
time_t i,b,e;
tdv->gettime(i,b,e);
name.Entry.Insert=i;
name.Entry.Begin=b;
name.Entry.End=e;
name.File="";
 int suc=0;
DPS::Producer::TDVbody_var vbody=new DPS::Producer::TDVbody();
vbody->length(tdv->GetNbytes()/sizeof(integer));
tdv->CopyOut(vbody->get_buffer());
 bool transienterror=false;
 bool again=false;
AGAIN:
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  
  try{
    _OnAir=true;
    (*li)->sendTDV(_pid,vbody,name);
    _OnAir=false;
    suc++;
    break;
  }
   catch (CORBA::TRANSIENT &tr){
    _OnAir=false;
    if(!again){
      transienterror=true;
      again=true;
      EMessage("AMSProducer::sendTDV-E-TransientErrorOccurs");
    }
    sleep(1);    
  }
  catch  (CORBA::SystemException & a){
    _OnAir=false;
  }
 }
if(!suc && !transienterror){
 FMessage("AMSProducer::sendTDV-F-UnableTosendTDV",DPS::Client::CInAbort);
 return false;
}
else if(!suc){
transienterror=false;
goto AGAIN;
}
return true;
}

void AMSProducer::sendEventTagEnd(const char * name,uinteger run,time_t insert, time_t begin,time_t end,uinteger first,uinteger last,integer nelem, bool fail){
//_evtag.Status=fail?DPS::Producer::Failure:DPS::Producer::Success;
if(_Solo)return;
if(fail){
 _evtag.Status=DPS::Producer::Failure;
}
else  _evtag.Status=DPS::Producer::Success;
AString a=(const char*)_pid.HostName;
a+=":";
a+=name;
a+=".";
char tmp[80];
sprintf(tmp,"%d",run);
a+=tmp;
a+=".";
sprintf(tmp,"%d",first);
a+=tmp;
_evtag.Name=(const char *)a;
_evtag.EventNumber=nelem;
_evtag.FirstEvent=first;
_evtag.LastEvent=last;
_evtag.Insert=insert;
_evtag.Begin=begin;
_evtag.End=end;
_evtag.Run=run;
_evtag.Version="";
_evtag.Type=DPS::Producer::EventTag;
    struct stat64 statbuf;
    stat64((const char*)name,&statbuf);
_evtag.size=statbuf.st_size;
LMessage(AMSClient::print(_evtag,"CloseDST"));
if(_Solo)return;

UpdateARS();
bool notagain =true;
again:
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  try{
   if(!CORBA::is_nil(*li)){
    _OnAir=true;
    (*li)->sendDSTEnd(_pid,_evtag,DPS::Client::Update);
    _OnAir=false;
    if( _evtag.Status==DPS::Producer::Failure)FMessage("EventTag Failure",DPS::Client::CInAbort);
    return;
   }
  }
  catch  (CORBA::SystemException & a){
     cerr<< "Problems with SendEventTsagEndInfo "<< a._orbitcpp_get_repoid()<<endl;
    _OnAir=false;
  }
}
 if(getior("GetIorExec") && notagain){
   notagain=false;
   goto again;
 }
else FMessage("AMSProducer::sendRunEnd-F-UnableToSendEventTagEndInfo ",DPS::Client::CInAbort);



}


void AMSProducer::sendEventTagBegin(const char * name,uinteger run,uinteger first){
if(_Solo)return;
AString a=(const char*)_pid.HostName;
a+=":";
a+=name;
a+=".";
char tmp[80];
sprintf(tmp,"%d",run);
a+=tmp;
a+=".";
sprintf(tmp,"%d",first);
a+=tmp;
_evtag.Name=(const char *)a;
_evtag.Status=DPS::Producer::InProgress;
_evtag.EventNumber=1;
_evtag.FirstEvent=first;
_evtag.LastEvent=first;
_evtag.Insert=0;
_evtag.Begin=0;
_evtag.End=0;
_evtag.Run=run;
_evtag.Version="";
_evtag.size=0;
_evtag.Type=DPS::Producer::EventTag;


LMessage(AMSClient::print(_evtag,"OpenDST"));
if(_Solo)return;

UpdateARS();
again:
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  try{
   if(!CORBA::is_nil(*li)){
    _OnAir=true;
    (*li)->sendDSTEnd(_pid,_evtag,DPS::Client::Create);
    _OnAir=false;
    if( _evtag.Status==DPS::Producer::Failure)FMessage("EventTagBegin Failure",DPS::Client::CInAbort);
    return;
   }
  }
  catch  (CORBA::SystemException & a){
    _OnAir=false;
  }
}
 if(getior("GetIorExec"))goto again;
else FMessage("AMSProducer::sendRunEnd-F-UnableToSendEventTagBeginInfo ",DPS::Client::CInAbort);



}



void AMSProducer::sendDSTInfo(){

     struct statfs64 buffer;
     int fail=statfs64((const char *)_dstinfo->OutputDirPath, &buffer);
    if(fail){
      _dstinfo->FreeSpace=-1;
      _dstinfo->TotalSpace=-1;
    }
    else{
     _dstinfo->FreeSpace= (buffer.f_bavail*(buffer.f_bsize/1024.))/1024;
     _dstinfo->TotalSpace= (buffer.f_blocks*(buffer.f_bsize/1024.))/1024;
    }
    for( list<DPS::Producer_var>::iterator ni = _plist.begin();ni!=_plist.end();++ni){
      try{
    _OnAir=true;
       (*ni)->sendDSTInfo(_dstinfo,DPS::Client::Update);
    _OnAir=false;
      }
      catch  (CORBA::SystemException & a){
      cerr<<" sendDSTInfo-E-UnableToSend "<<endl; 
    _OnAir=false;
      }
    }



}

bool AMSProducer::Progressing(){
static integer total=-1;
static float xt=0;
static double xte=0;
#pragma omp threadprivate (total,xt,xte)
float lt;
    struct timeb  ft;
    ftime(&ft);
    double st=ft.time+ft.millitm/1000.;
TIMEX(lt);
cerr <<"AMSPRoducer::Progressing "<<total<<" "<<lt-xt<<" "<<AMSFFKEY.CpuLimit*2<<" "<<_cinfo.EventsProcessed<<" "<<st-xte<<endl;
 if(_Transfer)return true;
 if(total>=0 && total == _cinfo.EventsProcessed && (lt-xt>AMSFFKEY.CpuLimit*2.5 || st-xte>AMSFFKEY.CpuLimit*100)){
   total=-1;   
   return false;
 }
 else if(total<0){
  total=_cinfo.EventsProcessed;
  TIMEX(xt);
    struct timeb  ft;
    ftime(&ft);
    xte=ft.time+ft.millitm/1000.;
  return true;
 }
 else if(total != _cinfo.EventsProcessed){
  total=-1;
  return true;
 }
 else{
  return true;
 }
}

DPS::Producer::DST* AMSProducer::getdst(DPS::Producer::DSTType type){
switch (type){
case DPS::Producer::Ntuple:
return &(_ntend[0]);
break;
case DPS::Producer::RootFile:
return &(_ntend[1]);
break;
case DPS::Producer::RawFile:
return &(_ntend[2]);
break;
default:
return 0;
}
}

bool AMSProducer::getior(const char * getiorvar){
_OnAir=false;
char iort[1024];
const char *exedir=getenv("ExeDir");
const char *nve=getenv(getiorvar);
const char *version=AMSCommonsI::getversion(); 
const char *nvr=AMSCommonsI::getosversion(); 
int maxtries=1200;
int delay=1;
if(exedir && nve && AMSCommonsI::getosname()){
 char t1[1024];
 strcpy(t1,exedir);
 strcat(t1,"/../prod");
 setenv("TNS_ADMIN",t1,0);
 if(getenv("AMSOracle")){
 setenv("LD_LIBRARY_PATH",getenv("AMSOracle"),1);
}
else{
 setenv("LD_LIBRARY_PATH","/afs/cern.ch/ams/oracle/10205/lib",1);
}

 for (int tries=0;tries<maxtries;tries++){
  sleep(delay);
  delay*=8;
  if(delay>60)delay=60;
  AString systemc(exedir);
  systemc+="/";
  systemc+=AMSCommonsI::getosname();
  systemc+="/";
  systemc+=nve;
  if(strstr(nvr,".el6")){
   systemc+=".so";
  }
  else if(strstr(nvr,"2.6")){
   systemc+=".so";
  }
  if(strstr(version,"v4.00")){
    systemc+=" -m ";
  }
  else if(getenv("AMSServerNo")){
    systemc+=" -s";
    systemc+=getenv("AMSServerNo");
  }
  systemc+=" > /tmp/getior.";
  char tmp[80];
  sprintf(tmp,"%d",getpid());
  systemc+=tmp;
  int i=system(systemc);
  if(i){
   cerr <<" AMSProducer::getenv-E-UnableTo "<<systemc<<endl;
   systemc="rm /tmp/getior."; 
   systemc+=tmp;
//   system(systemc);
   continue;
  }
  else{
   systemc="/tmp/getior."; 
   systemc+=tmp;
   ifstream fbin;
   fbin.open(systemc);
   iort[0]='\0';
   fbin>>iort;
   fbin.close();
   systemc="rm /tmp/getior."; 
   systemc+=tmp;
   system(systemc);
   if(iort[0]=='-'){
    cerr <<" AMSProducer::getenv-E-UnableToGetIOR "<<iort<<endl;
    continue;
   }
   else{
   try{
    cout <<" AMSProducer::getenv-I-GetIOR "<<iort<<endl;
    CORBA::Object_var obj=_orb->string_to_object(iort);
    if(!CORBA::is_nil(obj)){
     DPS::Producer_var _pvar=DPS::Producer::_narrow(obj);
     _pvar->pingp();
     _plist.clear();
     _plist.push_front(_pvar);
     return true;
    }
   }
  catch (CORBA::SystemException & a){
    EMessage("AMSProducer::getior-E-UnableToUpdateIOR");
    continue;
  }
   }
  }
}
}
else{
    EMessage("AMSProducer::getior-E-UnableToTryToGetIORBecauseSomeVarAreNull");
}
return false;
}

bool AMSProducer::SetDataCards(){
  _dc=new AString("");
  char tmp[80];
  char tmpp[80];
  char tmpu[80];
  sprintf(tmpp,"%d",_pid.ppid);
  sprintf(tmp,"%d",_pid.pid);
  if(PRODFFKEY.Job>0 && _pid.uid!=PRODFFKEY.Job){
   cerr<<" AMSProducer::SetDataCards-W-JobPidDiscrepance "<<PRODFFKEY.Job<<" "<<_pid.uid<<endl;
  sprintf(tmpu,"%d",PRODFFKEY.Job);
}
else sprintf(tmpu,"%d",_pid.uid);
  AString fout="/tmp/";
  fout+=tmp;
  fout+=".dc"; 
  AString cmd="ps -elfw --cols 400 | grep ";
  cmd+=tmpp;
  cmd+=" 1>";
  cmd+=(const char*)fout;
  cmd+=" 2>&1 ";
  AString dc="";
  int i=system((const char*)cmd);
  if(i==0 ){
   ifstream fbin;
   fbin.open((const char*)fout);
   AString fscript="";
    bool found=false;
   cout <<"looking for "<<tmpu<<endl;
    while(fbin.good() && !fbin.eof()){
     fbin.ignore(1024,' ');
     char tmpbuf[1024];
     fbin>>tmpbuf;
     if(strstr(tmpbuf,tmpu) && strstr(tmpbuf,".job")){
      cout <<"   Found job "<<tmpbuf<<endl;
      int beg=-1;
      for(int k=0;k<strlen(tmpbuf);k++){
         if(tmpbuf[k]=='('){  
           beg=k;
           break;
         }
      }
        
      fscript+=tmpbuf+beg+1;
      found=true;
      break;
     } 
    }
    fbin.close();
    if(found)unlink((const char*)fout);
    else {
      cout << " AMSProducer-W-UsingAMSFSCRIPT "<<endl;
      fscript=getenv("AMSFSCRIPT");
    }

{
//
// Check if submitted via lsf 
// set up hostname  and pid as lxplus/ams job_entry
//
              vector <int>id;
               id.clear();
              char atmpu[80];
              sprintf(atmpu,"%d",_pid.pid);
              AString afout="/tmp/";
              afout+=tmpu;
              afout+=".bjobs";
              AString afscript="bjobs -J '";
              afscript+=(const char *)fscript;
              afscript+="*'     1>";
              afscript+=(const char*)afout;
              afscript+=" 2>&1";
             cout << "AMSClient-I-Trying "<<(const char*)afscript<<endl;
              // cern lxplus6 id
              bool lxplus6=strstr(_pid.HostName,"cern.ch")!=0;
              lxplus6=true;
              bool lxplus6_1=((const char*)_pid.HostName)[0]=='b' && ((const char*)_pid.HostName)[1]=='6';
              bool lxplus6_2=((const char*)_pid.HostName)[0]=='p';
              if(lxplus6_2){
                for(int k=1;k<strlen(_pid.HostName);k++){
                  if(((const char*)_pid.HostName)[k]=='.')break;
                  if(!isdigit( ((const char*)_pid.HostName)[k]))lxplus6_2=false;
                }
              }
              lxplus6=lxplus6 && (lxplus6_1 || lxplus6_2); 
              if(lxplus6){
                cout <<" AMSProducer-I-lxplus6 Detected"<<endl;
              }
               //  not if pcamsf4
              int i=!(lxplus6 || strstr(_pid.HostName,"lsb") || strstr(_pid.HostName,"lxb") || strstr(_pid.HostName,"vmb"))?1:system((const char*)afscript);
              char line[1024];
              bool amsprod=false;
              if(i==0){
                ifstream afbin;
                afbin.open((const char*)afout);
                while(afbin.good() && !afbin.eof()){
                afbin.getline(line,1023);
                if(strstr(line,"amsprod")){
                 amsprod=true;
                 break;
                }
                }
                afbin.close();
                afbin.clear();
                afbin.open((const char*)afout);
                while(afbin.good() && !afbin.eof()){
                afbin.getline(line,1023);
                if(strstr(line,"JOBID")){
                  int idd;
                  afbin>>idd;
                   
                  id.push_back(idd);
                  cout << "AMSProducer-I-bsubDetected "<<idd<<endl;
                }
               }
                afbin.close();
               }
               unlink ( (const char*)afout); 
                for(int i=0;i<int(id.size())-1;i++){
                      cerr<<"AMSProducer-E-bsubduplicatediddetected "<<id[id.size()-1]<<" "<<id[i]<<endl;
             }
              if(id.size()>0){
               char *lxplus5=getenv("AMSPublicBatch");
               if(!lxplus5 || !strlen(lxplus5))setenv("AMSPublicBatch","lxplus5.cern.ch",1);
              lxplus5=getenv("AMSPublicBatch");
                  if(strstr(_pid.HostName,"lsb") || strstr(_pid.HostName,"lxb")|| strstr(_pid.HostName,"vmb") || lxplus6){

                    _pid.HostName=amsprod?"lxplus.cern.ch":lxplus5;
                    if(lxplus6)_pid.HostName="lxplus6.cern.ch";
                    _pid.pid=id[id.size()-1];
                    _pid.ppid=0;
                  }
                  else if(strstr(_pid.HostName,"pcamss0")){
                    _pid.HostName="pcamss0.cern.ch";
                    _pid.pid=id[id.size()-1];
                    _pid.ppid=0;
}
                  else if(strstr(_pid.HostName,"ams")){
                    _pid.HostName="pcamsf2.cern.ch";
                    _pid.pid=id[id.size()-1];
                    _pid.ppid=0;
                  }
                 } 
           }





    ifstream f1;
    f1.open((const char *)fscript);
    if(f1){
    char tmpbuf[1024];
     (*_dc)+="ScriptName=";
     (*_dc)+=(const char *)fscript;
     (*_dc)+='\n';
    while(f1.good() && !f1.eof()){ 
     f1.getline(tmpbuf,1023);
     (*_dc)+=tmpbuf;
     (*_dc)+='\n';
     //cout <<"  *************************8  ******************8"<<endl;
     //cout<<(const char *)(*_dc)<<endl;
    }
    }
    else{
     cerr<<" AMSProducer::SetDatacards-S-UnableToOpenFile "<<(const char *)fscript<<"|"<<endl;
     return false; 
    }
  }
  else{
   cerr <<" AMSProducer::SetDataCards-S-UnableToSys "<<(const char *)cmd<<endl;  
    return false;
  }
  return true;
} 
void AMSProducer::SendTimeout(int tmout){
if(_Solo)return;
    sendid(tmout);
}

bool AMSProducer::SendFile(const char *Remote, const char *Local, bool erase){
    if(_Solo)return;
    sendCurrentRunInfo(true);
    DPS::Producer::FPath fpath;
    fpath.fname=Remote;
    fpath.pos=0;
   ifstream fbin;
    struct stat64 statbuf;
    int suc=stat64(Local, &statbuf);
   if(suc){
      EMessage("AMSProducer::SendFile-S-UnableToStatFile");
      return false;
    }
   fbin.open(Local);
   if(fbin){
    DPS::Producer::TransferStatus st=DPS::Producer::Begin;
    const int maxs=2000000;
     DPS::Producer::RUN_var vrun=new DPS::Producer::RUN();
    while(st !=DPS::Producer::End){
     long long last=statbuf.st_size-fpath.pos;
     if(last>maxs)last=maxs;
     else st=DPS::Producer::End;
     vrun->length(last);
     fbin.read(( char*)vrun->get_buffer(),last);
     if(!fbin.good()){
      EMessage("AMSProducer::sendFile-F-UnableToReadNtuplein mode RO ");
      return false;
     }
      for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
      try{
       _OnAir=true;
       (*li)->sendFile(_pid,fpath,vrun,st);
       _OnAir=false;
        break;
       }
       catch (DPS::Producer::FailedOp & a){
        _OnAir=false;
        FMessage((const char *)a.message,DPS::Client::SInAbort);
       }
       catch  (CORBA::SystemException & a){
       _OnAir=false;
       FMessage("AMSProducer::sendFile-UnableToSendNtupleBody ",DPS::Client::CInAbort);
       }
     }
     fpath.pos+=last;
      if(st==DPS::Producer::Begin)st=DPS::Producer::Continue;
    }
     fbin.close();
     if(erase)unlink(Local);
     return true;
}
else {
cerr<<"AMSProducer::SendFile-E-UnableToOpen "<<Local<<endl;
return false;
}
}
