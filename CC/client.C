#include <client.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
char AMSClient::_streambuffer[1024];
ostrstream AMSClient::_ost(AMSClient::_streambuffer,sizeof(AMSClient::_streambuffer));
void AMSClient::_openLogFile(char * prefix){
AString fnam;
char * logdir=getenv("ProductionLogDir");
if(!logdir){
 cerr<<"AMSClient::_openLogFile-W-ProductionLogDirNotDefined:$AMSDataDir/prod.log will be used"<<endl;
 logdir=getenv("AMSDataDir");
 fnam=logdir;
 fnam+="/prod.log";
}
else fnam=logdir;
 fnam+="/";
 fnam+=prefix;
 fnam+="/";
 AString out="mkdir -p ";
 out+=fnam;
 system(out);
 time_t timecur;
 time(&timecur);
 char time[255];
 sprintf(time,"%d.%s.%d.%d",timecur,(const char *)_pid.HostName,_pid.pid,_pid.uid);
 fnam+=time;
 _fbin.open(fnam,ios::out);
 AString a("AMSClient::_openLogFile-F-UnableOpenLogFile ");
 a+=fnam; 
 if(!_fbin)FMessage((char *) a,DPS::Client::CInAbort);
}

extern "C" pid_t getpid();
extern "C" pid_t getppid();
extern "C" int gethostname(char*, int);
bool AMSClient::_getpidhost(uinteger uid, const char * iface){
 _pid.uid=uid;
 _pid.pid=getpid();
 _pid.ppid=getppid();
 char name[256];
 int len=255;
 if(gethostname(name,len))return false;
else{
   AString as=name;
   if(iface && strcmp(iface,"default")){
    as+=".";
    as+=iface;
   }
  _pid.HostName=(const char*)as;
  return true;
}
}

void AMSClient::FMessage(const char * message, DPS::Client::ClientExiting res){

cerr<<message<<endl;
   if(_fbin){
     time_t tcur;
     time(&tcur);
     _fbin<<message << " -F-  "<<ctime(&tcur)<<endl;
   }
   AMSClientError a(message,res);
   Error()=a;
   Exiting();
   throw a;
}

void AMSClient::EMessage(const char * message){

cerr<<message<<endl;
   if(_fbin){
     time_t tcur;
     time(&tcur);
     _fbin<<message << " -E- "<<ctime(&tcur)<<endl;
   }
}


void AMSClient::IMessage(const char * message){

cout<<message<<endl;
   if(_fbin){
     time_t tcur;
     time(&tcur);
     _fbin<<message << " -I- "<<ctime(&tcur)<<endl;
   }
}


char * AMSClient::print(const DPS::Client::ActiveClient & a,const char * mes){

_ost.seekp(0);
_ost<<mes <<" AC "<<print(a.id,_ost)<<" ARS Length "<<a.ars.length()<<" LastUp "<<ctime((const time_t *)&a.LastUpdate)<<" Start "<<ctime((const time_t *)&a.Start)<< "  Status " <<CS2string(a.Status)<<ends;
return _streambuffer;
}


char * AMSClient::print(const DPS::Client::CID & a,const char * mes){
_ost.seekp(0);
_ost<<mes<<" CID "<<print(a,_ost)<<ends;
return _streambuffer;
}


ostream & AMSClient::print(const DPS::Client::CID & a, ostream & o){
return o<<a.HostName<<" "<<a.Interface<<"  UID "<<a.uid<<" PID "<<a.pid<<" "<<a.ppid<<" Type "<<CT2string(a.Type)<<" Exit Status "<<CSE2string(a.Status);
}


char * AMSClient::print(const DPS::Client::ActiveHost & a,const char * mes){
_ost.seekp(0);
_ost<<mes <<" AH "<<a.HostName<<" "<<a.Interface<<" Status  "<<HS2string(a.Status)<< " CRun "<<a.ClientsRunning<<" CAll "<<a.ClientsAllowed<<" CProcessed "<<a.ClientsProcessed<<" CFailed "<<a.ClientsFailed<<" CKilled "<<a.ClientsKilled<<" LastUpdate "<<ctime((const time_t *)&a.LastUpdate)<<ends;
return _streambuffer;
}

char * AMSClient::print(const DPS::Producer::DST & a,const char * mes){
_ost.seekp(0);
_ost<<mes << " DST "<<a.Name <<" Insert "<<ctime((const time_t *)&a.Insert)<<" Begin "<<ctime((const time_t *)&a.Begin)<<" End "<<ctime((const time_t *)&a.End)<<" Run "<<a.Run << " 1st Event "<<a.FirstEvent<<" Last Event "<<a.LastEvent<<" Status "<<DSTS2string(a.Status)<<ends;
return _streambuffer;
}

char * AMSClient::print(const DPS::Server::CriticalOps & a,const char * mes){
_ost.seekp(0);
_ost<<mes<< "COp Action "<<OPS2string(a.Action) << " Client Type "<<CT2string(a.Type)<<" Client id "<<a.id<<" Time  "<<ctime((const time_t *)&a.TimeStamp)<<" TimeOut "<<a.TimeOut<<ends;
return _streambuffer;
}

char * AMSClient::print(const DPS::Producer::RunEvInfo & a,const char * mes){
_ost.seekp(0);
_ost<<mes<<" REI "<<a.uid<<" Run "<<a.Run<<" 1st Event "<<a.FirstEvent<<" Last Event "<<a.LastEvent<<" Prio "<<a.Priority<<" Path "<<a.FilePath<< "Status "<<RS2string(a.Status)<<" Client id "<<a.cuid<<" SubmitTime "<<ctime((const time_t *)&a.SubmitTime)<< print(a.cinfo,_ost)<<ends;
return _streambuffer;
}

char * AMSClient::print(const DPS::Producer::CurrentInfo & a,const char * mes){
_ost.seekp(0);
_ost<<mes<<" CInfo "<<print(a,_ost)<<ends;
return _streambuffer;
}


ostream & AMSClient::print(const DPS::Producer::CurrentInfo & a, ostream & o){
return o<<" Run " <<a.Run <<" EventsProcessed "<<a.EventsProcessed<<" LastEvent "<<a.LastEventProcessed<<" Errors "<<a.ErrorsFound<<" CPU "<<a.CPUTimeSpent<<" CPU/Event "<<a.CPUTimeSpent/(a.EventsProcessed+1)<<" Status "<<RS2string(a.Status);
}

char * AMSClient::CS2string(DPS::Client::ClientStatus a){
switch (a){
case DPS::Client::Lost:
return "Lost";
case DPS::Client::Unknown:
return "Unknown";
case DPS::Client::Submitted:
return "Submitted";
case DPS::Client::Registered:
return "Registered";
case DPS::Client::Active:
return "Active";
case DPS::Client::TimeOut:
return "TimeOut";
}
return " ";
}



char * AMSClient::CT2string(DPS::Client::ClientType a){
switch (a){
case DPS::Client::Generic:
return "Generic";
case DPS::Client::Server:
return "Server";
case DPS::Client::Consumer:
return "Consumer";
case DPS::Client::Producer:
return "Producer";
case DPS::Client::Monitor:
return "Monitor";
}
return " ";
}


char * AMSClient::HS2string(DPS::Client::HostStatus a){
switch (a){
case DPS::Client::NoResponse:
return "NoResponse";
case DPS::Client::LastClientFailed:
return "LastClientFailed";
case DPS::Client::OK:
return "OK";
}
return " ";
}


char * AMSClient::CSE2string(DPS::Client::ClientExiting a){
switch (a){
case DPS::Client::NOP:
return "NOP";
case DPS::Client::CInExit:
return "CInExit";
case DPS::Client::SInExit:
return "SInExit";
case DPS::Client::CInAbort:
return "CInAbort";
case DPS::Client::SInAbort:
return "SInAbort";
case DPS::Client::SInKill:
return "SInKill";
}
return " ";
}


char * AMSClient::RS2string(DPS::Producer::RunStatus a){
switch (a){
case DPS::Producer::ToBeRerun:
return "ToBeRerun";
case DPS::Producer::Failed:
return "Failed";
case DPS::Producer::Processing:
return "Processing";
case DPS::Producer::Finished:
return "Finished";
}
return " ";
}

char * AMSClient::DSTS2string(DPS::Producer::DSTStatus a){
switch (a){
case DPS::Producer::Success:
return "Success";
case DPS::Producer::Failure:
return "Failure";
}
return " ";
}

char * AMSClient::OPS2string(DPS::Server::OpType a){
switch (a){
case DPS::Server::StartClient:
return "StartClient";
case DPS::Server::KillClient:
return "KillClient";
case DPS::Server::ClearStartClient:
return "ClearStartClient";
case DPS::Server::ClearKillClient:
return "ClearKillClient";
}
return " ";
}

