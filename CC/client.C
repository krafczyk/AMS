#include <client.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
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
bool AMSClient::_getpidhost(uinteger uid){
 _pid.uid=uid;
 _pid.pid=getpid();
 _pid.ppid=getppid();
 char name[256];
 int len=255;
 if(gethostname(name,len))return false;
else{
  _pid.HostName=(const char*)name;
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

