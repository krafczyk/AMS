#include <producer.h>
#include <cern.h>
#include <commons.h> 
#include <stdio.h>
#include <unistd.h>
#include <iostream.h>
#include <event.h>
#include <job.h>
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
     getRunEventInfo();
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

void AMSProducer::getTDVTable(uinteger size, uinteger * db[5]){
}

void AMSProducer::getTDV(const char * tdv,uinteger Tag, uinteger Insert, uinteger Begin, uinteger End){
}

void AMSProducer::getRunEventInfo(){
UpdateARS();
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  try{
    (*li)->getRunEvInfo(_pid,_reinfo);
    if(_reinfo->DieHard)FMessage("AMSProducer::getRunEventinfo-I-ServerRequestedExit",DPS::Client::SInExit);
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
    DAQEvent::setfile((const char *)(_reinfo->FilePath));
    AMSJob::gethead()->SetNtuplePath(_reinfo->OutputDirPath);
    TIMEX(_T0);
    IMessage(AMSClient::print(_reinfo," get reinfo "));
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


void AMSProducer::sendNtupleEnd(const char * name, int entries, int last, time_t end, bool success){
_ntend.Name=name;
_ntend.Status=success?DPS::Producer::Success:DPS::Producer::Failure;
_ntend.EventNumber=entries;
_ntend.LastEvent=last;
_ntend.End=end;
if(_ntend.End==0 || _ntend.LastEvent==0)_ntend.Status=DPS::Producer::Failure;
time_t tt;
time(&tt);
_ntend.Insert=tt;
uinteger suc=0;
cout << " nt end " <<_ntend.Insert<<" "<<_ntend.Begin<<" "<<_ntend.End<<endl;
UpdateARS();
 for( list<DPS::Producer_var>::iterator li = _plist.begin();li!=_plist.end();++li){
  try{
   if(!CORBA::is_nil(*li)){
    (*li)->sendNtupleEnd(_pid,_ntend,DPS::Client::Create);
     suc++;
   }
  }
  catch  (CORBA::SystemException & a){
  }
}
if(!suc)FMessage("AMSProducer::sendRunEnd-F-UnableToSendRunEndInfo ",DPS::Client::CInAbort);

if( _ntend.Status==DPS::Producer::Failure)FMessage("Ntuple Failure",DPS::Client::CInAbort);
  
}



void AMSProducer::sendNtupleStart(int run, int first,time_t begin){
_ntend.Run=run;
_ntend.FirstEvent=first;
_ntend.Begin=begin;
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
     int length=(*li)->getARS(_pid,pars,100000000);
     DPS::Client::ARS_var ars=pars;
     if(length==0){
      FMessage("getARS-S-UnableToGetARS ",DPS::Client::CInAbort);
     }   
     for(int i=0;i<length;i++){
        CORBA::Object_var obj=_orb->string_to_object((ars[i]).IOR);
        if(!CORBA::is_nil(obj)){
        DPS::Producer_var _pvar=DPS::Producer::_narrow(obj);
        if(!CORBA::is_nil(_pvar)){
         if(i==0)_plist.clear();
         _plist.push_front(_pvar);
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
else if(_cinfo.EventsProcessed%_reinfo->UpdateFreq==1 ){
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