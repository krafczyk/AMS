#include <stdlib.h>
#include <server.h>
#include <fstream.h>
#include <astring.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
AMSServer* AMSServer::_Head=0;

int main(int argc, char * argv[]){

 try{
    AMSServer::Singleton()=new AMSServer(argc,argv);
 }
  catch (CORBA::SystemException &a){
   cerr <<"CorbaSystemExceptionDuringInitialization "<<endl;
   return 1;
 }
Back:
    for(;;){
     try{
      AMSServer::Singleton()->UpdateDB();
      AMSServer::Singleton()->SystemCheck();     
     }
     catch(CORBA::SystemException &a){
      cerr <<"CorbaSystemExceptionDuringUpdateDB/SystemCheck "<<endl;
       goto Back;
     }
     try{
      sleep(1);
      AMSServer::Singleton()->getOrb()->perform_work();
     }
     catch(CORBA::SystemException &a){
      cerr <<"CorbaSystemExceptionDuringPerform_Work "<<endl;
       goto Back;
     }
    }
 return 0;
}


AMSServer::AMSServer(int argc, char* argv[]){

 char * ior=0;
 uinteger uid=0;
 char *rfile=0;
 char *nserver=0;
 char *nhost=0;
 char *nproducer=0;
 char *ntuplestring=0;
 char *eventtagstring=0;
 for (char *pchar=0; argc>1 &&(pchar=argv[1],*pchar=='-'); (argv++,argc--)){
    pchar++;
    switch (*pchar){
    case 'I':   //ior
     ior=pchar;
     break;
    case 'R':   //RunFileName
     rfile=++pchar;
     break;
    case 'H':   //NominalHostFile
     nhost=++pchar;
     break;
    case 'S':   //NominalServer
     nserver=++pchar;
     break;
    case 'P':   //NominalProducer
     nproducer=++pchar;
     break;
    case 'N':   //NtupleOutputDir
     ntuplestring=++pchar;
     break;
    case 'T':   //EventTagOutputDir
     eventtagstring=++pchar;
     break;
    case 'U':   //uid
     uid=atoi(++pchar);
     break;
  }
 }
  _orb=CORBA::ORB_init(argc,argv);
   CORBA::Object_var obj=_orb->resolve_initial_references("RootPOA");
  _poa=PortableServer::POA::_narrow(obj);
  _mgr=_poa->the_POAManager();
  _mgr->activate();
  _pser= new Server_impl(_poa,_orb,nserver,nhost);
 if(ior==0){      //  Primary
  if(rfile){  
   _pser->add(new Producer_impl(_poa,_orb,nproducer,rfile,ntuplestring,eventtagstring));
  }  
  
 }
 else{
 }


}

Producer_impl::Producer_impl(PortableServer::POA_ptr poa, CORBA::ORB_ptr orb, char * NC, char *RF, char *NS, char *TS): POA_DPS::Producer(),AMSServerI(AMSID("Producer",0)){

 PortableServer::ObjectId_var oid=poa->activate_object(this);
 _ref = reinterpret_cast<DPS::Producer_ptr>(poa->id_to_reference(oid));
 _refstring=orb->object_to_string(_ref);
// Here read nominalclients
   
if(NC){
 ifstream fbin;
 fbin.open(NC);
 if(fbin){
   _ncl= new DPS::Client::NominalClient();
   if(fbin.get()=='#')fbin.ignore(1024,'\n');
   else fbin.seekg(fbin.tellg()-sizeof(char));
   fbin>>_ncl->MaxClients>>_ncl->CPUNeeded>>_ncl->MemoryNeeded;
   char tmpbuf[1024];
   fbin>>tmpbuf;
   _ncl->WholeScriptPath=(const char*)tmpbuf;
   fbin.getline(tmpbuf,1024);
   _ncl->LogPath= (const char*)tmpbuf;
   fbin.getline(tmpbuf,1024);
    _ncl->SubmitCommand=(const char*)tmpbuf;
 }
 else{
  cerr<<"Producer_impl::Producer_impl-F-UnableToOpenNCFile "<<NC<<endl;
  abort();
}
}
//Here read runfiletable
if(RF){
 ifstream fbin;
 fbin.open(RF);
 if(fbin){
   int cur=0;
   if(fbin.get()=='#')fbin.ignore(1024,'\n');
   else fbin.seekg(fbin.tellg()-sizeof(char));
  while(!fbin.eof() && fbin.good()){ 
   DPS::Producer::RunEvInfo_var  re = new DPS::Producer::RunEvInfo();
    char tmpbuf[1024];
   fbin>>re->Run>>re->FirstEvent>>re->LastEvent>>re->Priority>>tmpbuf;
   re->FilePath=(const char*)tmpbuf;
   if(re->LastEvent<re->FirstEvent)re->LastEvent=2000000000;
   re->Status=DPS::Producer::ToBeRerun;
   re->UpdateFreq=100;
   re->DieHard=0;
   re->OutputDirPath=(const char*)NS;
   time_t tt;
   time(&tt);
   re->SubmitTime=tt; 
   if(fbin.good())_rq.push(re); 
//    cout <<++cur<<" "<<re->Run<<endl;
  }
 }
else{
  cerr<<"Producer_impl::Producer_impl-F-UnableToOpenRunFile "<<RF<<endl;
  abort();
}
 cout <<"RQsize "<<_rq.size()<<endl;
}
}

Server_impl::Server_impl(PortableServer::POA_ptr poa, CORBA::ORB_ptr orb, char* NS, char * NH): POA_DPS::Server(),AMSServerI(AMSID("Server",0)){
//Here init asl

 PortableServer::ObjectId_var oid=poa->activate_object(this);
 _ref = reinterpret_cast<DPS::Server_ptr>(poa->id_to_reference(oid));
 _refstring=orb->object_to_string(_ref);
if(NS){
 ifstream fbin;
 fbin.open(NS);
 if(fbin){
   _ncl= new DPS::Client::NominalClient();
   if(fbin.get()=='#')fbin.ignore(1024,'\n');
   else fbin.seekg(fbin.tellg()-sizeof(char));
   fbin>>_ncl->MaxClients>>_ncl->CPUNeeded>>_ncl->MemoryNeeded;
   char tmpbuf[1024];
   fbin>>tmpbuf;
   _ncl->WholeScriptPath=(const char*)tmpbuf;
   fbin.getline(tmpbuf,1024);
   _ncl->LogPath= (const char*)tmpbuf;
   fbin.getline(tmpbuf,1024);
    _ncl->SubmitCommand=(const char*)tmpbuf;
 }
 else{
  cerr<<"Server_impl::Server_impl-F-UnableToOpenNSFile "<<NS<<endl;
  abort();
}
}
if(NH){
 ifstream fbin;
 fbin.open(NH);
 if(fbin){
   if(fbin.get()=='#')fbin.ignore(1024,'\n');
   else fbin.seekg(fbin.tellg()-sizeof(char));
  while(!fbin.eof() && fbin.good()){ 
   DPS::Server::NominalHost_var nh= new DPS::Server::NominalHost();
   char tmpbuf[1024];
   fbin>>tmpbuf;
   nh->HostName=(const char*)tmpbuf;
   fbin>>tmpbuf;
   nh->Interface=(const char*)tmpbuf;
   AString hn((const char*)(nh->HostName));
   hn+=".";
   hn+=(const char*)nh->Interface;
   nh->HostName=(const char *)hn;
   fbin>>tmpbuf;
   nh->OS= (const char*)tmpbuf;
   fbin>>nh->CPUNumber>>nh->Memory>>nh->Clock;
   if(fbin.good())_nhl.push_back(nh);  
 }
 }
 else{
  cerr<<"Server_impl::Server_impl-F-UnableToOpenNHFile "<<NH<<endl;
  abort();
}
}
     _init();

}

void Producer_impl::_init(){
// here connect to servers
 Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
 if(_pser){
  typedef list<DPS::Server::NominalHost_var>::const_iterator NHLI;
  typedef list<DPS::Server::NominalHost_var> NHL;
  for(NHLI i=(_pser->getNHL()).begin();i!=(_pser->getNHL()).end();++i){
   DPS::Client::ActiveHost_var ah= new DPS::Client::ActiveHost();
   ah->HostName=CORBA::string_dup((*i)->HostName);
   if(_pser->pingHost((const char*)(ah->HostName)))ah->Status=DPS::Client::OK; 
   else ah->Status=DPS::Client::NoResponse; 
   ah->ClientsProcessed=0;
   ah->ClientsFailed=0;
   ah->ClientsRunning=0;
   ah->ClientsAllowed=min((*i)->CPUNumber/_ncl->CPUNeeded,(*i)->Memory/float(_ncl->MemoryNeeded));
 
   time_t tt;
   time(&tt);
   ah->LastUpdate=tt;
   _ahl.push_back(ah);
  }
 }
 else{
  cerr<<"Producer_impl::_init-F-UnableToConnectToServer"<<endl;
  abort();
 }
}

void Server_impl::_init(){
// here connect to servers
 Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
 if(_pser){
  typedef list<DPS::Server::NominalHost_var>::const_iterator NHLI;
  typedef list<DPS::Server::NominalHost_var> NHL;
  for(NHLI i=(_pser->getNHL()).begin();i!=(_pser->getNHL()).end();++i){
   DPS::Client::ActiveHost_var ah= new DPS::Client::ActiveHost();
   ah->HostName=CORBA::string_dup((*i)->HostName);
   if(_pser->pingHost((const char*)(ah->HostName)))ah->Status=DPS::Client::OK; 
   else ah->Status=DPS::Client::NoResponse; 
   ah->ClientsProcessed=0;
   ah->ClientsFailed=0;
   ah->ClientsRunning=0;
   ah->ClientsAllowed=min((*i)->CPUNumber/_ncl->CPUNeeded,(*i)->Memory/float(_ncl->MemoryNeeded));
 
   time_t tt;
   time(&tt);
   ah->LastUpdate=tt;
   _ahl.push_back(ah);
  }
 }
 else{
  cerr<<"Server_impl::_init-F-UnableToConnectToServer"<<endl;
  abort();
 }
 // Registered itself in _acl
DPS::Client::ActiveClient_var as= new DPS::Client::ActiveClient();
     
     (as->id).uid=_Submit;
     (as->id).pid=getpid();
     (as->id).ppid=getppid();
     char name[256];
     int len=255;
     if(gethostname(name,len)){
       cerr<<"Server_impl-ctor-S-UnableToGetHostName"<<endl;
       (as->id).HostName=(const char *) " ";
     }
     else (as->id).HostName=(const char *) name;
     as->IOR= CORBA::string_dup(_refstring);
     as->Type=DPS::Client::Server;
     as->Status=DPS::Client::Registered;
     time_t tt;
     time(&tt);
     as->LastUpdate=tt;     
     as->Start=tt;
     _asl.push_back(as);
//   Find corr server
 for(AHLI i=_ahl.begin();i!=_ahl.end();++i){
  if(strstr((const char *)((*i)->HostName),(const char *)((as->id).HostName))){
   ((*i)->ClientsRunning)++;
   break;
  }
 }

}


CORBA::Boolean Producer_impl::sendId(const DPS::Client::CID& cid, int p, int e) throw (CORBA::SystemException){
     for(ACLI j=_acl.begin();j!=_acl.end();++j){
      if(((*j)->id).uid==cid.uid){
       ((*j)->id).pid=cid.pid;
       ((*j)->id).ppid=cid.ppid;
       (*j)->Status=DPS::Client::Registered;
       time_t tt;
       time(&tt);
       (*j)->LastUpdate=tt;
#ifdef __AMSDEBUG__
       cout <<"Producer_impl::sendId-I-RegClient "<<cid.pid<<" "<<cid.ppid<<endl;
       return false;
#endif 
       return true;
      }
     }
     return false;


}


void Producer_impl::getACL(ACS_out acl) throw (CORBA::SystemException){
}

void Producer_impl::getNCL(NCS_out ncl)throw (CORBA::SystemException){
}

DPS::Producer::TDV* Producer_impl::getTDV(const char * name, DPS::time i, DPS::time b, DPS::time e)throw (CORBA::SystemException){
}

void Producer_impl::sendTDV( const TDV & tdv, const char * name, DPS::time Insert, DPS::time Begin, DPS::time End, int Propagate, int  Error )throw (CORBA::SystemException){
}

void Producer_impl::sendCurrentInfo(const  CurrentInfo &ci, int  Propagate, int Error)throw (CORBA::SystemException){
}

  
DPS::Producer::TDVTable * Producer_impl::getTDVTable(const char * Name, unsigned int Run ,int  Propagate, int Error)throw (CORBA::SystemException){
}


  void Producer_impl::sendNtupleEnd(const  NtupleEnd & ne, int Propagate, int Error)throw (CORBA::SystemException){
}


void Producer_impl::sendEventStatusEnd(const  EventStatusEnd & ne, int Propagate, int Error)throw (CORBA::SystemException){
}


void Producer_impl::sendEndRunInfo(const  EndRunInfo & ne, int Propagate, int Error) throw (CORBA::SystemException){
}



DPS::Producer::RunEvInfo *  Producer_impl::getRunEvInfo(int p,int e) throw (CORBA::SystemException){

}



  CORBA::Boolean Server_impl::sendId(const DPS::Client::CID& cid, int p, int e) throw (CORBA::SystemException){
}

   void Server_impl::getACL(ACS_out acl)throw (CORBA::SystemException){
}

   void Server_impl::getAHL(AHS_out ahl)throw (CORBA::SystemException){
}

   void Server_impl::getNCL(NCS_out ncl)throw (CORBA::SystemException){
}

   void Server_impl::sendCriticalOps(const CriticalOps &, int, int)throw (CORBA::SystemException){
}

  void Server_impl::sendACL(const ACS & acl, int Propagate,int Error)throw (CORBA::SystemException){
}

  void Server_impl::sendAHL(const AHS & ahl, int Propagate,int Error)throw (CORBA::SystemException){
}

  void Server_impl::sendNCL(const NCS & ncl, int Propagate,int Error)throw (CORBA::SystemException){
}

  void Server_impl::ping()throw (CORBA::SystemException){
}

  bool Server_impl::pingHost(const char * host){
   char tmpbuf[1024];
   strcpy(tmpbuf,"ping -c 1 ");
   strcat(tmpbuf,host);
   return system(tmpbuf)==0;


}


void AMSServer::SystemCheck(){
// Here run Start,Stop,Kill,Check Clients

for(AMSServerI * pcur=_pser; pcur; pcur=pcur->down()?pcur->down():pcur->next()){
 pcur->StartClients();
 pcur->CheckClients();
 pcur->KillClients();
}
     

}

void AMSServer::UpdateDB(){
// Her don't know yet
}


void Producer_impl::StartClients(){

 // Check if there are some hosts to run on
 _ahl.sort(Prio());
 for(AHLI i=_ahl.begin();i!=_ahl.end();++i){
  if(_acl.size()<_ncl->MaxClients && _acl.size()<_rq.size()){
  if((*i)->Status!=NoResponse){
    int curc=0;
    for(ACLI j=_acl.begin();j!=_acl.end();++j){
     if( ((*j)->id).HostName == (*i)->HostName){
      if(++curc>=(*i)->ClientsAllowed)break;
     } 
    }   
    if(curc>=(*i)->ClientsAllowed)break;
    // HereStartClient
#ifdef __AMSDEBUG__
    cout <<_refstring<<endl;
#endif
    AString submit;
    submit+=(const char*)(_ncl->SubmitCommand);  
    submit+=" ";
    submit+=(const char*)((*i)->HostName);
    submit+=" ";
    submit+=(const char*)(_ncl->LogPath);  
    _Submit++;
    submit+="Producer.";
    char tmp[80];
    sprintf(tmp,"%d",_Submit);
    submit+=tmp;
    submit+=".log ";
    submit+=(const char*)(_ncl->WholeScriptPath);  
    submit+=" -";
    submit+=(const char*) _refstring;
    submit+=" -U";
    submit+=tmp;
#ifdef __AMSDEBUG__
    submit+=" -D1";
#endif
    int out=system(submit);
    if(out==0){
     // Add New Active Client
     DPS::Client::ActiveClient_var ac=new DPS::Client::ActiveClient();
     (ac->id).HostName=CORBA::string_dup((*i)->HostName);
     (ac->id).uid=_Submit;
     ac->IOR= (const char *) " ";
     ac->Type=DPS::Client::Producer;
     ac->Status=DPS::Client::Submitted;
     time_t tt;
     time(&tt);
     ac->LastUpdate=tt;     
     ac->Start=tt;
     _acl.push_back(ac);
     ((*i)->ClientsRunning)++;
    }
    else{
     cerr<<"Producer_impl::StartClients-E-UnableToStartClient "<<submit<<endl;
    }
   }
  } 
 }  
}


void Producer_impl::KillClients(){
}

void Producer_impl::CheckClients(){
}

void Server_impl::StartClients(){
if(_asl.size()<_ncl->MaxClients ){
  //Starting Servers Here
}
}

void Server_impl::KillClients(){
}

void Server_impl::CheckClients(){
}





