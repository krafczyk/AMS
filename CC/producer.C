//  $Id: producer.C,v 1.44 2002/01/08 13:43:12 choutko Exp $
#include <unistd.h>
#include <stdlib.h>
#include <producer.h>
#include <cern.h>
#include <commons.h> 
#include <stdio.h>
#include <iostream.h>
#include <event.h>
#include <job.h>
#include<algorithm>
#include <sys/statfs.h>
#include <sys/timeb.h>
#include <sys/stat.h>
#include <sys/file.h>
AMSProducer * AMSProducer::_Head=0;
AMSProducer::AMSProducer(int argc, char* argv[], int debug) throw(AMSClientError):AMSClient(),AMSNode(AMSID("AMSProducer",0)),_OnAir(false),_FreshMan(true){
DPS::Producer_var pnill=DPS::Producer::_nil();
_plist.push_back(pnill);
if(_Head){
 FMessage("AMSProducer::::AMSProducer-E-AMSProducerALreadyExists",DPS::Client::CInAbort);
}
else{
 char * ior=0;
 uinteger uid=0;
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
     _MT=true;
     break;
    case 'U':   //uid
     uid=atoi(++pchar);
     break;
    case 'A': //amsdatadir
      setenv("AMSDataDir",++pchar,1);
      break;
  }
 }
  if(!ior){
   if(_debug)_openLogFile("Producer");
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
      if(_debug)_openLogFile("Producer");
      FMessage("AMSProducer::AMSProducer-F-UnableToGetHostName", DPS::Client::CInAbort);
     }
     if(_debug)_openLogFile("Producer");
     time_t cput=3*AMSFFKEY.CpuLimit;
     if(!(_pvar->sendId(_pid,cput))){
       sleep(10);
      if(!(_pvar->sendId(_pid,cput))){
       // dieHard
       FMessage("Server Requested Termination after sendID ",DPS::Client::SInAbort);
      }
     }
     _Head=this;
     IMessage(AMSClient::print(_pid,"sendID-I-Success"));
      return;       
     }
    }
   }
   catch (CORBA::MARSHAL a){
   FMessage("AMSProducer::AMSProducer-F-CORBA::MARSHAL",DPS::Client::CInAbort);
  }
  catch (CORBA::COMM_FAILURE a){
   FMessage("AMSProducer::AMSProducer-F-CORBA::COMM_FAILURE",DPS::Client::CInAbort);
  }
  catch (CORBA::SystemException & a){
   FMessage("AMSProducer::AMSProducer-F-CORBA::SystemError",DPS::Client::CInAbort);
  }
}
FMessage("AMSProducer::AMSProducer-F-UnableToInitCorba",DPS::Client::CInAbort);
}



void AMSProducer::getRunEventInfo(){
UpdateARS();
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  try{
    (*li)->getRunEvInfo(_pid,_reinfo,_dstinfo);
         
    if(_dstinfo->DieHard){
      
      if(!_FreshMan && _dstinfo->DieHard==1)FMessage("AMSProducer::getRunEventinfo-I-ServerRequestedExit",DPS::Client::SInExit);
      else FMessage("AMSProducer::getRunEventinfo-I-ServerRequestedAbort",DPS::Client::SInAbort);
    }
    _FreshMan=false;
    _cinfo.Run=_reinfo->Run;
    _cinfo.HostName=_pid.HostName; 
    SELECTFFKEY.Run=_reinfo->Run;
    SELECTFFKEY.Event=_reinfo->FirstEvent;
    SELECTFFKEY.RunE=_reinfo->Run;
    SELECTFFKEY.EventE=_reinfo->LastEvent;    
    _cinfo.EventsProcessed=0;
    _cinfo.ErrorsFound=0;
    _cinfo.Status=DPS::Producer::Processing;
    _cinfo.CPUTimeSpent=0;
    _cinfo.TimeSpent=0;



    if(_dstinfo->Mode==DPS::Producer::RILO || _dstinfo->Mode==DPS::Producer::RIRO){ 
     DAQEvent::setfile((const char *)(_reinfo->FilePath));
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
    AString ntpath=(const char *)_dstinfo->OutputDirPath;
    ntpath+="/";
    char tmp[80];
    sprintf(tmp,"%d",_reinfo->Run);
    ntpath+=tmp;
    ntpath+="/";
     IOPA.hlun=0;
     IOPA.WriteRoot=0;
    if(_dstinfo->type == DPS::Producer::RootFile){
      IOPA.WriteRoot=1;
      AMSJob::gethead()->SetRootPath((const char *)ntpath);
    }
    else{
       IOPA.hlun=1;
       AMSJob::gethead()->SetNtuplePath((const char *)ntpath);
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
 FMessage("AMSProducer::getRunEventinfo-F-UnableToGetRunEvInfo",DPS::Client::CInAbort);
}

void AMSProducer::sendCurrentRunInfo(bool force){
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
}
//cout <<" sendcurrentinfo end "<<endl;
}

void AMSProducer::getASL(){
}


void AMSProducer::sendNtupleEnd(DPS::Producer::DSTType type,int entries, int last, time_t end, bool success){
cout <<" sendntupleend start "<<endl;

DPS::Producer::DST *ntend=getdst(type);
if(ntend){
ntend->Status=success?DPS::Producer::Success:DPS::Producer::Failure;
ntend->EventNumber=entries;
ntend->LastEvent=last;
ntend->End=end;
ntend->Type=type;
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
    struct stat statbuf;
    stat((const char*)a(bstart), &statbuf);
    

ntend->Insert=statbuf.st_ctime;
ntend->size=statbuf.st_size/1024./1024.+0.5;

}
cout << " nt end " <<ntend->Insert<<" "<<ntend->Begin<<" "<<ntend->End<<endl;
UpdateARS();
sendDSTInfo();

if(_dstinfo->Mode==DPS::Producer::LIRO || _dstinfo->Mode==DPS::Producer::RIRO){
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
    struct stat statbuf;
    stat((const char*)a(bstart), &statbuf);
   ifstream fbin;
   fbin.open((const char*)a(bstart));
   if(fbin){
    DPS::Producer::TransferStatus st=DPS::Producer::Begin;
    const int maxs=16000000;
     DPS::Producer::RUN_var vrun=new DPS::Producer::RUN();
    while(st !=DPS::Producer::End){
     int last=statbuf.st_size-fpath.pos;
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
       }
     }
     fpath.pos+=last;
    }
     fbin.close();
     unlink( ((const char*)a(bstart)));
      a=(const char*)_pid.HostName;
     a+=":REMOTE:";
     a+=(const char*)fpath.fname;
     ntend->Name=(const char *)a;
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
FMessage("AMSProducer::sendRunEnd-F-UnableToSendNtupleEndInfo ",DPS::Client::CInAbort);
}
   else FMessage("AMSProducer::sendNtupleEnd-F-UnableToSendNtuplein mode RO ",DPS::Client::CInAbort);

  }

 




else{
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
FMessage("AMSProducer::sendNtupleEnd-F-UnableToSendNtupleEndInfo ",DPS::Client::CInAbort);
}
}
else{
FMessage("AMSProducer::sendNtupleEnd-F-UNknownDSTType ",DPS::Client::CInAbort);
}  
}



void AMSProducer::sendNtupleStart(DPS::Producer::DSTType type,const char * name, int run, int first,time_t begin){
cout <<" sendntuplestart start "<<endl;
DPS::Producer::DST *ntend=getdst(type);
if(ntend){
AString a=(const char*)_pid.HostName;
a+=":";
a+=name;
ntend->Name=(const char *)a;
ntend->Run=run;
ntend->FirstEvent=first;
ntend->Begin=begin;
time_t tt;
time(&tt);
ntend->Insert=tt;
ntend->End=0;
ntend->LastEvent=0;
ntend->EventNumber=0;
ntend->Status=DPS::Producer::InProgress;
ntend->Type=type;
ntend->size=0;
UpdateARS();


sendDSTInfo();

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
FMessage("AMSProducer::sendNtupleStart-F-UnableToSendNtupleStartInfo ",DPS::Client::CInAbort);
}
else{
FMessage("AMSProducer::sendNtupleEnd-F-UNknownDSTType ",DPS::Client::CInAbort);
}
}

void AMSProducer::sendNtupleUpdate(DPS::Producer::DSTType type){
cout <<" sendntupleupdate start "<<endl;
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
    struct stat statbuf;
    stat((const char*)a(bstart), &statbuf);
    

ntend->Insert=statbuf.st_ctime;
ntend->size=statbuf.st_size/1024./1024.+0.5;

UpdateARS();


sendDSTInfo();
cout <<" sendntupleupdate middle "<<endl;

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
FMessage("AMSProducer::sendNtupleUpdate-F-UnableToSendNtupleStartInfo ",DPS::Client::CInAbort);
}
else{
FMessage("AMSProducer::sendNtupleUpdate-F-UNknownDSTType ",DPS::Client::CInAbort);
}  

}

void AMSProducer::Exiting(const char * message){
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
}


void AMSProducer::UpdateARS(){
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
     if(_MT){
     for(int i=0;i<length;i++){
        CORBA::Object_var obj=_orb->string_to_object((ars[i]).IOR);
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
        CORBA::Object_var obj=_orb->string_to_object((ars[i]).IOR);
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
_cinfo.Status= (res==DAQEvent::OK)?DPS::Producer::Finished: DPS::Producer::Failed;

    struct timeb  ft;
    ftime(&ft);
    double st=ft.time+ft.millitm/1000.;
_cinfo.TimeSpent=st-_ST0;

TIMEX(_cinfo.CPUTimeSpent);
_cinfo.CPUTimeSpent=_cinfo.CPUTimeSpent-_T0;
UpdateARS();
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
FMessage("AMSProducer::sendRunEnd-F-UnableToSendRunEndInfo ",DPS::Client::CInAbort);


}

void AMSProducer::AddEvent(){
if(_cinfo.Run == AMSEvent::gethead()->getrun()){
 _cinfo.EventsProcessed++;
 _cinfo.LastEventProcessed=AMSEvent::gethead()->getid();
  if(!AMSEvent::gethead()->HasNoErrors())_cinfo.ErrorsFound++;
  if(!AMSEvent::gethead()->HasNoCriticalErrors())_cinfo.CriticalErrorsFound++;
}
if(!(AMSEvent::gethead()->HasNoCriticalErrors())){
  TIMEX(_cinfo.CPUTimeSpent);
  _cinfo.CPUTimeSpent=_cinfo.CPUTimeSpent-_T0;

    struct timeb  ft;
    ftime(&ft);
    double st=ft.time+ft.millitm/1000.;
    _cinfo.TimeSpent=st-_ST0;

  sendCurrentRunInfo();
}
else if(_cinfo.EventsProcessed%_dstinfo->UpdateFreq==1 ){
  TIMEX(_cinfo.CPUTimeSpent);
  _cinfo.CPUTimeSpent=_cinfo.CPUTimeSpent-_T0;

    struct timeb  ft;
    ftime(&ft);
    double st=ft.time+ft.millitm/1000.;
    _cinfo.TimeSpent=st-_ST0;

  sendCurrentRunInfo(_cinfo.EventsProcessed%(_dstinfo->UpdateFreq*10)==1);
}
}

AMSProducer::~AMSProducer(){
const char * a=0;
cout <<" ams producer destructor called "<<endl;
Exiting(_up?_up->getMessage():a);
}


void AMSProducer::InitTDV( uinteger run){
AMSTimeID * head=AMSJob::gethead()->gettimestructure();
for (AMSTimeID * pser=dynamic_cast<AMSTimeID*>(head->down());pser;pser=dynamic_cast<AMSTimeID*>(pser->next())){
DPS::Producer::TDVName a;
a.Name=pser->getname();
a.DataMC=pser->getid();
a.Size=pser->GetNbytes();
a.CRC=pser->getCRC();
time_t i,b,e;
pser->gettime(i,b,e);
a.Entry.id=0;
a.Entry.Insert=i;
a.Entry.Begin=b;
a.Entry.End=e;
DPS::Producer::TDVTable * ptdv;
//IMessage(AMSClient::print(a," INITDV "));
 int suc=0;
 int length;
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
if(!suc)FMessage("AMSProducer::getinitTDV-F-UnableTogetTDVTable",DPS::Client::CInAbort);
DPS::Producer::TDVTable_var tvar=ptdv;
uinteger *ibe[5];

if(a.Success){
for(int i=0;i<5;i++){
 ibe[i]=new uinteger[length];
}
 for(int j=0;j<length;j++){
    ibe[0][j]=tvar[j].id;
    ibe[1][j]=tvar[j].Insert;
    ibe[2][j]=tvar[j].Begin;
    ibe[3][j]=tvar[j].End;
  }
  pser->fillDB(length,ibe);
}
else{
FMessage("AMSProducer::getinitTDV-F-tdvgetFailed",DPS::Client::CInAbort);
}
}
}

bool AMSProducer::getTDV(AMSTimeID * tdv, int id){
//cout <<" trying to get tdv "<<tdv->getname()<<endl;
DPS::Producer::TDVbody * pbody;
DPS::Producer::TDVName name;
name.Name=tdv->getname();
name.DataMC=tdv->getid();
name.CRC=tdv->getCRC();
name.Size=tdv->GetNbytes();
name.Entry.id=id;
time_t i,b,e;
tdv->gettime(i,b,e);
name.Entry.Insert=i;
name.Entry.Begin=b;
name.Entry.End=e;
 int length=0;
 int suc=0;
 bool oncemore=false;
again:
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  
  try{
    _OnAir=true;
    length=(*li)->getTDV(_pid,name,pbody);
    _OnAir=false;
    suc++;
    break;
  }
  catch  (CORBA::SystemException & a){
    _OnAir=false;
  }
 }
if(!suc){
 if(oncemore){
  FMessage("AMSProducer::getTDV-F-UnableTogetTDV",DPS::Client::CInAbort);
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
 int nb=tdv->CopyIn(vbody->get_buffer());
 if(nb){
  tdv->SetTime(name.Entry.Insert,name.Entry.Begin,name.Entry.End);
  cout <<"  gettdv success "<<endl;
  return true;
 }
}
  cout <<"  gettdv failuire "<<endl;
return false;
}
bool AMSProducer::getSplitTDV(AMSTimeID * tdv, int id){
cout <<" trying to get tdv "<<tdv->getname()<<endl;
DPS::Producer::TDVbody * pbody;
DPS::Producer::TDVName name;
name.Name=tdv->getname();
name.DataMC=tdv->getid();
name.CRC=tdv->getCRC();
name.Size=tdv->GetNbytes();
name.Entry.id=id;
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
 FMessage("AMSProducer::getTDV-F-UnableTogetTDV",DPS::Client::CInAbort);
 return false;
}
if(!totallength){
  vb2=pbody;
}
else{
 DPS::Producer::TDVbody_var vbody=pbody;
 vb2->length(totallength+length);
 for(int i=0;i<length;i++){
  vb2[i+totallength]=vbody[i];
 }
}
totallength+=length;
}
if(name.Success){
 int nb=tdv->CopyIn(vb2->get_buffer());
 if(nb){
  tdv->SetTime(name.Entry.Insert,name.Entry.Begin,name.Entry.End);
  cout <<"  gettdv success "<<endl;
  return true;
 }
}
  cout <<"  gettdv failure "<<endl;
return false;
}

bool AMSProducer::sendTDV(const AMSTimeID * tdv){
cout <<" Send tdv get for "<<tdv->getname()<<endl;

DPS::Producer::TDVName name;
name.Name=tdv->getname();
name.DataMC=tdv->getid();
name.CRC=tdv->getCRC();
name.Size=tdv->GetNbytes();
time_t i,b,e;
tdv->gettime(i,b,e);
name.Entry.Insert=i;
name.Entry.Begin=b;
name.Entry.End=e;
 int suc=0;
DPS::Producer::TDVbody_var vbody=new DPS::Producer::TDVbody();
vbody->length(name.Size/sizeof(integer));
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
_evtag.Status=fail?DPS::Producer::Failure:DPS::Producer::Success;
_evtag.EventNumber=nelem;
_evtag.FirstEvent=first;
_evtag.LastEvent=last;
_evtag.Insert=insert;
_evtag.Begin=begin;
_evtag.End=end;
_evtag.Run=run;
_evtag.Type=DPS::Producer::EventTag;
_evtag.size=0;

UpdateARS();
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
    _OnAir=false;
  }
}
FMessage("AMSProducer::sendRunEnd-F-UnableToSendEventTagEndInfo ",DPS::Client::CInAbort);



}


void AMSProducer::sendEventTagBegin(const char * name,uinteger run,uinteger first){
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
_evtag.Type=DPS::Producer::EventTag;


UpdateARS();
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
FMessage("AMSProducer::sendRunEnd-F-UnableToSendEventTagBeginInfo ",DPS::Client::CInAbort);



}



void AMSProducer::sendDSTInfo(){

     struct statfs buffer;
     int fail=statfs((const char *)_dstinfo->OutputDirPath, &buffer);
    if(fail){
      _dstinfo->FreeSpace=-1;
      _dstinfo->TotalSpace=-1;
    }
    else{
     _dstinfo->FreeSpace= (buffer.f_bavail*(buffer.f_bsize/1024.));
     _dstinfo->TotalSpace= (buffer.f_blocks*(buffer.f_bsize/1024.));
    }
    for( list<DPS::Producer_var>::iterator ni = _plist.begin();ni!=_plist.end();++ni){
      try{
    _OnAir=true;
       (*ni)->sendDSTInfo(_dstinfo,DPS::Client::Update);
    _OnAir=false;
      }
      catch  (CORBA::SystemException & a){
    _OnAir=false;
      }
    }



}

bool AMSProducer::Progressing(){
static integer total=-1;
static float xt=0;
static double xte=0;
float lt;
    struct timeb  ft;
    ftime(&ft);
    double st=ft.time+ft.millitm/1000.;
TIMEX(lt);
cerr <<"AMSPRoducer::Progressing "<<total<<" "<<lt-xt<<" "<<AMSFFKEY.CpuLimit*2<<" "<<_cinfo.EventsProcessed<<" "<<st-xte<<endl;
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
default:
return 0;
}
}
