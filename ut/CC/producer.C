#include <producer.h>
#include <cern.h>
#include <commons.h> 
#include <stdio.h>
#include <unistd.h>
#include <iostream.h>
AMSProducer * AMSProducer::_Head=0;
AMSProducer::AMSProducer(int argc, char* argv[], int debug) throw(AMSProducer::Error):_debug(debug),_pid(0){
if(_Head)throw AMSProducer::Error("AMSProducer::AMSProducer-E-AMSProducerALreadyExists",0); 
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
   throw AMSProducer::Error("AMSProducer::AMSProducer-F-NoIOR",2);
  }
  _pid= new DPS::Client::CID(); 
  _getpidhost(uid);
  if(_debug)_openLogFile();
  CORBA::ORB_var orb= CORBA::ORB_init(argc,argv);
  try{
   CORBA::Object_var obj=orb->string_to_object(ior);
   if(CORBA::is_nil(obj))throw AMSProducer::Error("AMSProducer::AMSProducer-F-CORBA::is_nil-1",2);
   DPS::Producer_var var=DPS::Producer::_narrow(obj);
   if(CORBA::is_nil(var))throw AMSProducer::Error("AMSProducer::AMSProducer-F-CORBA::is_nil-2",2);
   _plist.push_back(var);
   if(!(var->sendId(_pid,1,0))){
     // dieHard
     Message("Server Requested Termination after sendID ",2);
     abort();
   }
   Message("sendID-I-");
  }
   catch (CORBA::MARSHAL a){
   throw  AMSProducer::Error("AMSProducer::AMSProducer-F-CORBA::MARSHAL",2);
  }
  catch (CORBA::COMM_FAILURE a){
   throw  AMSProducer::Error("AMSProducer::AMSProducer-F-CORBA::COMM_FAILURE",2);
  }
  catch (CORBA::SystemException & a){
   throw  AMSProducer::Error("AMSProducer::AMSProducer-F-CORBA::SystemError",2);
  }
}
_Head=this;
}

void AMSProducer::getTDVTable(uinteger size, uinteger * db[5]){
}

void AMSProducer::getTDV(const char * tdv,uinteger Tag, uinteger Insert, uinteger Begin, uinteger End){
}

void AMSProducer::getRunEventinfo(){
}

void AMSProducer::sendCurrentRunInfo(){
}

void AMSProducer::getASL(){
}


void AMSProducer::sendNtupleEnd(){
}

void AMSProducer::sendRunEnd(){
}


#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
void AMSProducer::_openLogFile(){
AString fnam;
char * logdir=getenv("ProductionLogDir");
if(!logdir){
 cerr<<"AMSProducer::_openLogFile-W-ProductionLogDirNotDefined:$AMSDataDir/prod.log will be used"<<endl;
 logdir=getenv("AMSDataDir");
 fnam=logdir;
 fnam+="/prod.log";
}
else fnam=logdir;
 fnam+="/";
 time_t timecur;
 time(&timecur);
 char time[255];
 sprintf(time,"%d.%s.%d.%d",timecur,(const char *)_pid->HostName,_pid->pid,_pid->uid);
 fnam+=time;
 _fbin.open(fnam,ios::out);
 if(!_fbin)throw AMSProducer::Error("AMSProducer::_openLogFile-F-UnableOpenLogFile",2);
}

extern "C" pid_t getpid();
extern "C" pid_t getppid();
extern "C" int gethostname(char*, int);
void AMSProducer::_getpidhost(uinteger uid){
 _pid->uid=uid;
 _pid->pid=getpid();
 _pid->ppid=getppid();
 char name[256];
 int len=255;
 if(gethostname(name,len))throw AMSProducer::Error("AMSProducer::_getpidhost-F-UnableGetHostName",2);
 _pid->HostName=(const char*)name;
}

void AMSProducer::Message(const char * message, int stream){

#ifdef __AMSDEBUG__
cout<<message<<endl;
#else
if(stream==1)cout<<message<<endl;
else if(stream>0)cerr<<message<<endl;
#endif
   if(_fbin){
     time_t tcur;
     time(&tcur);
     _fbin<<message << " "<<ctime(&tcur)<<endl;
   }
}
