#include <stdlib.h>
#include <producer.h>
#include <cern.h>
#include <commons.h> 
#include <stdio.h>
#include <unistd.h>
#include <iostream.h>
#include <event.h>
#include <job.h>
#include<algorithm>
AMSProducer * AMSProducer::_Head=0;
AMSProducer::AMSProducer(int argc, char* argv[], int debug) throw(AMSClientError):AMSClient(),AMSNode(AMSID("AMSProducer",0)){
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
      // dieHard
      FMessage("Server Requested Termination after sendID ",DPS::Client::SInExit);
     }
     IMessage(AMSClient::print(_pid,"sendID-I-Success"));
     _Head=this;
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
    if(_dstinfo->DieHard)FMessage("AMSProducer::getRunEventinfo-I-ServerRequestedExit",DPS::Client::SInExit);
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
     DPS::Producer::RunTransferStatus st=DPS::Producer::Begin;
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
    AMSJob::gethead()->SetNtuplePath((const char *)ntpath);
    TIMEX(_T0);
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

void AMSProducer::sendCurrentRunInfo(){
int failure=0;
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  try{
   if(!CORBA::is_nil(*li)){
    (*li)->sendCurrentInfo(_pid,_cinfo);
     break;
   }
  }
  catch  (CORBA::SystemException & a){
   failure++;
  }
}
if(failure)UpdateARS();


}

void AMSProducer::getASL(){
}


void AMSProducer::sendNtupleEnd(int entries, int last, time_t end, bool success){
_ntend.Status=success?DPS::Producer::Success:DPS::Producer::Failure;
_ntend.EventNumber=entries;
_ntend.LastEvent=last;
_ntend.End=end;
_ntend.Type=DPS::Producer::Ntuple;
if(_ntend.End==0 || _ntend.LastEvent==0)_ntend.Status=DPS::Producer::Failure;
time_t tt;
time(&tt);
_ntend.Insert=tt;
cout << " nt end " <<_ntend.Insert<<" "<<_ntend.Begin<<" "<<_ntend.End<<endl;
UpdateARS();
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  try{
   if(!CORBA::is_nil(*li)){
    (*li)->sendDSTEnd(_pid,_ntend,DPS::Client::Update);
     if( _ntend.Status==DPS::Producer::Failure)FMessage("Ntuple Failure",DPS::Client::CInAbort);
     return;
   }
  }
  catch  (CORBA::SystemException & a){
  }
}
FMessage("AMSProducer::sendRunEnd-F-UnableToSendNtupleEndInfo ",DPS::Client::CInAbort);

  
}



void AMSProducer::sendNtupleStart(const char * name, int run, int first,time_t begin){
AString a=(const char*)_pid.HostName;
a+=":";
a+=name;
_ntend.Name=(const char *)a;
_ntend.Run=run;
_ntend.FirstEvent=first;
_ntend.Begin=begin;
_ntend.Insert=0;
_ntend.End=0;
_ntend.LastEvent=0;
_ntend.EventNumber=0;
_ntend.Status=DPS::Producer::InProgress;
_ntend.Type=DPS::Producer::Ntuple;
UpdateARS();
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  try{
   if(!CORBA::is_nil(*li)){
    (*li)->sendDSTEnd(_pid,_ntend,DPS::Client::Create);
    return;
   }
  }
  catch  (CORBA::SystemException & a){
  }
}
FMessage("AMSProducer::sendRunEnd-F-UnableToSendNtupleStartInfo ",DPS::Client::CInAbort);
}


void AMSProducer::Exiting(const char * message){
if(_ExitInProgress)return;
cout<< " Exiting ...."<<(message?message:" ")<<endl;
_ExitInProgress=true;
_pid.Status=AMSClient::_error.ExitReason();
for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
try{
if(!CORBA::is_nil(*li))
(*li)->Exiting(_pid,(message?message:AMSClient::_error.getMessage()),AMSClient::_error.ExitReason());
break;
}
catch  (CORBA::SystemException & a){}
}
}


void AMSProducer::UpdateARS(){
for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
try{
if(!CORBA::is_nil(*li)){
     DPS::Client::ARS * pars;
     int length=(*li)->getARS(_pid,pars,DPS::Client::Any,0);
     DPS::Client::ARS_var ars=pars;
     if(length==0){
      FMessage("getARS-S-UnableToGetARS ",DPS::Client::CInAbort);
     }   
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
             _pvar->getId(pcid);
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
            r=0;
          }
         if(r>0.5)_plist.push_front(_pvar);
         else _plist.push_back(_pvar);
        }
        }
       }
      break;
}
}
catch  (CORBA::SystemException & a){}
}

}


void AMSProducer::sendRunEnd(DAQEvent::InitResult res){
if(_dstinfo->Mode ==DPS::Producer::LILO || _dstinfo->Mode==DPS::Producer::LIRO){
AString command="rm -f ";
command+=DAQEvent::getfile();
system(command);
}
_cinfo.Status= (res==DAQEvent::OK)?DPS::Producer::Finished: DPS::Producer::Failed;
TIMEX(_cinfo.CPUTimeSpent);
_cinfo.CPUTimeSpent+=-_T0;
UpdateARS();
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  try{
   if(!CORBA::is_nil(*li)){
    (*li)->sendCurrentInfo(_pid,_cinfo);
     return;
   }
  }
  catch  (CORBA::SystemException & a){
  }
}
FMessage("AMSProducer::sendRunEnd-F-UnableToSendRunEndInfo ",DPS::Client::CInAbort);


}

void AMSProducer::AddEvent(){
if(_cinfo.Run == AMSEvent::gethead()->getrun()){
 _cinfo.EventsProcessed++;
 _cinfo.LastEventProcessed=AMSEvent::gethead()->getid();
  if(!AMSEvent::gethead()->HasNoErrors())_cinfo.ErrorsFound++;
}
if(!(AMSEvent::gethead()->HasNoCriticalErrors())){
  TIMEX(_cinfo.CPUTimeSpent);
  _cinfo.CPUTimeSpent+=-_T0;
  sendCurrentRunInfo();
}
else if(_cinfo.EventsProcessed%_dstinfo->UpdateFreq==1 ){
  TIMEX(_cinfo.CPUTimeSpent);
  _cinfo.CPUTimeSpent+=-_T0;
  sendCurrentRunInfo();
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
    length=(*li)->getTDVTable(_pid,a,run,ptdv);
    suc++;
    break;
  }
  catch  (CORBA::SystemException & a){
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
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  
  try{
    length=(*li)->getTDV(_pid,name,pbody);
    suc++;
    break;
  }
  catch  (CORBA::SystemException & a){
  }
 }
if(!suc){
 FMessage("AMSProducer::getTDV-F-UnableTogetTDV",DPS::Client::CInAbort);
 return false;
}
DPS::Producer::TDVbody_var vbody=pbody;
if(name.Success){
 int nb=tdv->CopyIn(vbody->get_buffer());
 if(nb){
  tdv->SetTime(name.Entry.Insert,name.Entry.Begin,name.Entry.End);
  return true;
 }
}
return false;
}

bool AMSProducer::sendTDV(const AMSTimeID * tdv){
cout <<" Sendt dv get for "<<tdv->getname()<<endl;

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
tdv->CopyOut(vbody->get_buffer(0));
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  
  try{
    (*li)->sendTDV(_pid,vbody,name);
    suc++;
    break;
  }
  catch  (CORBA::SystemException & a){
  }
 }
if(!suc){
 FMessage("AMSProducer::getTDV-F-UnableTosendTDV",DPS::Client::CInAbort);
 return false;
}
return true;
}

void AMSProducer::sendEventTagEnd(const char * name,uinteger run,time_t insert, time_t begin,time_t end,uinteger first,uinteger last,integer nelem, bool fail){
AString a=name;
a+=".";
char tmp[80];
sprintf(tmp,"%d",run);
a+=tmp;
a+=".";
sprintf(tmp,"%d",first);
a+=tmp;
_evtag.Name=(const char *)a;
_evtag.Status=fail?DPS::Producer::Failure:DPS::Producer::Success;
_evtag.EventNumber=nelem;
_evtag.FirstEvent=first;
_evtag.LastEvent=last;
_evtag.Insert=insert;
_evtag.Begin=begin;
_evtag.End=end;
_evtag.Run=run;
_evtag.Type=DPS::Producer::EventTag;


UpdateARS();
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  try{
   if(!CORBA::is_nil(*li)){
    (*li)->sendDSTEnd(_pid,_evtag,DPS::Client::Update);
    if( _evtag.Status==DPS::Producer::Failure)FMessage("EventTag Failure",DPS::Client::CInAbort);
    return;
   }
  }
  catch  (CORBA::SystemException & a){
  }
}
FMessage("AMSProducer::sendRunEnd-F-UnableToSendEventTagEndInfo ",DPS::Client::CInAbort);



}


void AMSProducer::sendEventTagBegin(const char * name,uinteger run,uinteger first){
AString a=name;
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
    (*li)->sendDSTEnd(_pid,_evtag,DPS::Client::Create);
    if( _evtag.Status==DPS::Producer::Failure)FMessage("EventTagBegin Failure",DPS::Client::CInAbort);
    return;
   }
  }
  catch  (CORBA::SystemException & a){
  }
}
FMessage("AMSProducer::sendRunEnd-F-UnableToSendEventTagBeginInfo ",DPS::Client::CInAbort);



}
