#include <stdlib.h>
#include <server.h>
#include <fstream.h>
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
  _pser= new Server_impl(_poa,nserver,nhost);
 if(ior==0){      //  Primary
  if(rfile){  
   _pser->add(new Producer_impl(_poa,nproducer,rfile,ntuplestring,eventtagstring));
  }  
  
 }
 else{
 }


}

Producer_impl::Producer_impl(PortableServer::POA_ptr poa, char * NC, char *RF, char *NS, char *TS): POA_DPS::Producer(),AMSServerI(AMSID("Producer",0)){

 PortableServer::ObjectId_var oid=poa->activate_object(this);
 _ref = reinterpret_cast<DPS::Producer_ptr>(poa->id_to_reference(oid));

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
   fbin>>tmpbuf;
   _ncl->LogPath= (const char*)tmpbuf;
   fbin>>tmpbuf;
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
   re->FilePath=(const char*)NS;
   _rq.push(re); 
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

Server_impl::Server_impl(PortableServer::POA_ptr poa, char* NS, char * NH): POA_DPS::Server(),AMSServerI(AMSID("Server",0)){
//Here init asl

 PortableServer::ObjectId_var oid=poa->activate_object(this);
 _ref = reinterpret_cast<DPS::Server_ptr>(poa->id_to_reference(oid));
if(NS){
 ifstream fbin;
 fbin.open(NS);
 if(fbin){
   DPS::Client::NominalClient_var _ncl= new DPS::Client::NominalClient();
   if(fbin.get()=='#')fbin.ignore(1024,'\n');
   else fbin.seekg(fbin.tellg()-sizeof(char));
   fbin>>_ncl->MaxClients>>_ncl->CPUNeeded>>_ncl->MemoryNeeded;
   char tmpbuf[1024];
   fbin>>tmpbuf;
   _ncl->WholeScriptPath=(const char*)tmpbuf;
   fbin>>tmpbuf;
   _ncl->LogPath= (const char*)tmpbuf;
   fbin>>tmpbuf;
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
   nh->OS= (const char*)tmpbuf;
   fbin>>nh->CPUNumber>>nh->Memory>>nh->Clock;
   _nhl.push_back(nh);  
 }
 }
 else{
  cerr<<"Server_impl::Server_impl-F-UnableToOpenNHFile "<<NH<<endl;
  abort();
}
}

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


void Producer_impl::sendId(const DPS::Client::CID& cid, int p, int e) throw (CORBA::SystemException){
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



  void Server_impl::sendId(const DPS::Client::CID& cid, int p, int e) throw (CORBA::SystemException){
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

for(AMSServerI * pcur=_pser; pcur; pcur->down()?pcur->down():pcur->next()){
 pcur->StartClients();
 pcur->CheckClients();
 pcur->KillClients();
}
     

}

void AMSServer::UpdateDB(){
// Her don't know yet
}


void Producer_impl::StartClients(){

if(_acl.size()<_ncl->MaxClients && _acl.size()<_rq.size()){
 // Check if there are some hosts to run on
 
}

}

void Producer_impl::KillClients(){
}

void Producer_impl::CheckClients(){
}

void Server_impl::StartClients(){
}

void Server_impl::KillClients(){
}

void Server_impl::CheckClients(){
}





