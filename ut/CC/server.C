//  $Id: server.C,v 1.41 2001/02/09 13:08:49 choutko Exp $
#include <stdlib.h>
#include <server.h>
#include <fstream.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <algorithm>
#include <amsdbc.h>
#include <signal.h>         
#include <dbserver.h>
AMSServer* AMSServer::_Head=0;
void (handler)(int);   
void (handler)(int sig){    
switch (sig){
case SIGTERM: 
AMSServer::Singleton()->FMessage("Interrupt Received",DPS::Client::CInExit);
break;
case SIGXCPU:
AMSServer::Singleton()->FMessage("Interrupt Received",DPS::Client::CInExit);
break;
case SIGINT:
AMSServer::Singleton()->FMessage("Interrupt Received",DPS::Client::CInExit);
break;

case SIGUSR1:
// stop everything;
AMSServer::Singleton()->StopEverything();
break;
case SIGUSR2:
//dump IOR in logfile
AMSServer::Singleton()->DumpIOR();
break;
case SIGHUP:
AMSServer::Singleton()->UpdateDB(true);
break;
}
}
int main(int argc, char * argv[]){
     *signal(SIGTERM, handler);
     *signal(SIGXCPU,handler);
     *signal(SIGINT, handler);
     *signal(SIGUSR1, handler);
     *signal(SIGUSR2, handler);
     *signal(SIGHUP, handler);

 try{
    AMSServer::Singleton()=new AMSServer(argc,argv);
 }
  catch (CORBA::SystemException &a){
   cerr<<"CorbaSystemExceptionDuringInitialization "<<endl;
   return 1;
 }
 catch (AMSClientError a){
  cerr<<a.getMessage()<<endl;
   return 1;
  } 
   AMSServer::Singleton()->IMessage("Initialization Completed");
   AMSServer::Singleton()->DumpIOR();
    for(;;){
     try{
      AMSServer::Singleton()->UpdateDB();
      AMSServer::Singleton()->SystemCheck();     
     }
     catch(CORBA::SystemException &a){
      cerr <<"CorbaSystemExceptionDuringUpdateDB/SystemCheck "<<endl;
       continue;
     }
     try{
      AMSServer::Singleton()->Listening(1);
     }
     catch(CORBA::SystemException &a){
      cerr <<"CorbaSystemExceptionDuringPerform_Work "<<endl;
       continue;
     }
      catch (AMSClientError a){
      cerr<<a.getMessage()<<endl;
      break;
          }
    }
 return 0;
}




AMSServer::AMSServer(int argc, char* argv[]):_GlobalError(false){
 char *dbfilename=0;
 char *iface=0;
 char *niface=0;
 char * ior=0;
 uinteger uid=1;
 char *rfile=0;
 char *nserver=0;
 char *nkiller=0;
 char *nhost=0;
 char *nproducer=0;
 char *ntuplestring=0;
 char *eventtagstring=0;
 char *amsd=0;
 char *amsp=0;
 char *amsperl=0;
 
 for (char *pchar=0; argc>1 &&(pchar=argv[1],*pchar=='-'); (argv++,argc--)){
    pchar++;
    switch (*pchar){
    case 'O':    // Oracle
      _Oracle=true;
    case 'D':    //debug
     _debug=atoi(++pchar);
     break;
    case 'A':   //AMSDataDir
     amsd=++pchar;
      setenv("AMSDataDir",amsd,1);
     break;
    case 'a':   //AMSProdOutputDir
     amsp=++pchar;
      setenv("AMSProdOutputDir",amsp,1);
     break;
    case 'p':   //AMSProdPerl
     amsperl=++pchar;
      setenv("AMSProdPerl",amsperl,1);
     break;
    case 'I':   //Interface
     iface=++pchar;
     break;
    case 'C':   //corba ior
     ior=++pchar;
     break;
    case 'B':   //DBfile name
     dbfilename=++pchar;
     break;
    case 'b':   //DBfile name
     _MaxDBProcesses=atoi(++pchar);
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
    case 'K':   //NominalKiller
     nkiller=++pchar;
     break;
    case 'F':   //NominalInterface
     niface=++pchar;
     break;
    case 'P':   //NominalProducer
     nproducer=++pchar;
     break;
    case 'N':   //NominalNtuple
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
setdbfile(dbfilename);
_getpidhost(uid,iface);
if(_debug)_openLogFile("Server");
_pid.Type=DPS::Client::Server;
if(iface)_pid.Interface = (const char *)iface;
else _pid.Interface= (const char *)"default";
   AString amsdatadir; 
   char* gtv=getenv("AMSDataDir");
   if(gtv && strlen(gtv)>0){
     amsdatadir=gtv;
   }
    else FMessage("AMSDataDirNotDefined",DPS::Client::CInAbort);
    amsdatadir+="/DataBase/";
     AMSDBc::amsdatabase=new char[strlen((const char*)amsdatadir)+1];
     strcpy(AMSDBc::amsdatabase,(const char *)amsdatadir);
     cout <<"amsdatadir  "<<AMSDBc::amsdatabase<<endl;
if(niface){
 ifstream fbin;
 fbin.open(niface);
 if(fbin){
  while(!fbin.eof() && fbin.good()){ 
   if(fbin.get()=='#'){
      fbin.ignore(1024,'\n');
      continue;
   }
   else fbin.seekg(fbin.tellg()-sizeof(char));
   char tmpbuf[1024];
   fbin>>tmpbuf;
   if(fbin.good()){
     
  try{
   OrbitVars e;
   if(strstr(tmpbuf,"default"))e._orb=CORBA::ORB_init(argc,argv);
   else {
    AString a=(const char*)_pid.HostName;
    a+=".";
    a+=tmpbuf;
       e._orb=CORBA::ORB_init(argc,argv,(const char *)a);
   }
   CORBA::Object_var obj=e._orb->resolve_initial_references("RootPOA");
   e._poa=PortableServer::POA::_narrow(obj);
/*
   // orbit doesnot support policies in cpp yet -> mod the sources, so root poa now has multiple_id policy
   PortableServer::IdUniquenessPolicy_var lf=e._poa->create_iduniqueness_policy(PortableServer::MULTIPLE_ID);
   CORBA::Policy pl;
   pl.length(1);
    pl[0]=lf;
   e._poa=e._poa->create_POA("AMSPOA",PortableServer::POAManager::_nil(),pl);
*/
   e._mgr=e._poa->the_POAManager();
   e._mgr->activate();
   AString a(tmpbuf);
   _orbmap[a]=e;
   }   
  catch (CORBA::SystemException &ex){
   cerr <<"AMSServer::AMSServer-E-CorbaSysExeceptionOnInit "<<tmpbuf<<endl;
  }
  }  
  }
 }
 else{
 
  FMessage("AMSServer::AMSServer-F-UnableToOpenNIFile ",DPS::Client::CInAbort);
 }
 if(_orbmap.size()<1){
   FMessage("AMSServer::AMSServer-F-NoInterfacesOpen ",DPS::Client::CInAbort);
 }
}
else{
  FMessage("AMSServer::AMSServer-F-UnableToFindNIFile ",DPS::Client::CInAbort);
}

 if(ior==0){      //  Primary
  _pser= new Server_impl(_orbmap,_pid,this,nserver,nhost,nkiller);
   if(_Oracle){
    _pser->add(new DBServer_impl(_orbmap,_pid,this));
   } 
   else{
    _pser->add(new Client_impl(DPS::Client::DBServer,"DBServerPerl",this));
   }
  if(rfile){  
   _pser->add(new Producer_impl(_orbmap,_pid,this,nproducer,rfile,ntuplestring,eventtagstring));
  }  
   _pser->add(new Client_impl(DPS::Client::Monitor,"Monitor",this));
  _pser->_init();
  Server_impl * pser=dynamic_cast<Server_impl*>(_pser);
  pser->setInterface(niface);
  pser->StartSelf(_pid,DPS::Client::Create);
  
 }
 else{
 try{
   CORBA::Object_var obj=(_orbmap.find((const char *)_pid.Interface)->second)._orb->string_to_object(ior);
    if(!CORBA::is_nil(obj)){
     DPS::Server_var _svar=DPS::Server::_narrow(obj);
     if(!CORBA::is_nil(_svar)){
         
        if(!(_svar->sendId(_pid,90))){
         FMessage("Server Requested Termination after sendID ",DPS::Client::SInAbort);
        }
        else{
         _pser= new Server_impl(_orbmap,_svar,_pid,this);
          Producer_impl a;
          if(_svar->TypeExists(a.getType())){
              _pser->add(new Producer_impl(_orbmap,_svar,_pid,this));
          }
           _pser->add(new Client_impl(DPS::Client::Monitor,"Monitor",_svar,_pid,this));
   if(_Oracle){
           _pser->add(new DBServer_impl(_orbmap,_svar,_pid,this));
   }
   else{
           _pser->add(new Client_impl(DPS::Client::DBServer,"DBServerPerl",_svar,_pid,this));
   }
  Server_impl * pser=dynamic_cast<Server_impl*>(_pser);
  pser->setInterface(niface);
  pser->StartSelf(_pid,DPS::Client::Update);

    }
 }
 }
}
 catch(CORBA::SystemException &ex){
   FMessage("AMSServer::AMSServer-F-UnableToInitIOR ",DPS::Client::CInAbort);
  } 
}
}




void AMSServer::Listening(int sleeptime){
typedef map<AString, AMSServer::OrbitVars>::iterator MOI; 
      for(MOI i=_orbmap.begin();i!=_orbmap.end();++i){
       if(sleeptime>0)sleep(sleeptime);
       ((*i).second)._orb->perform_work();
      }
}


void AMSServer::UpdateDB(bool force){
// Her don't know yet
for(AMSServerI * pcur=_pser; pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
pcur->UpdateDB(force);
}
}

void AMSServer::SystemCheck(){
// Here run Start,Stop,Kill,Check Clients

for(AMSServerI * pcur=_pser; pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
 Listening();
 if(!_GlobalError)pcur->StartClients(_pid);
 Listening();
 pcur->CheckClients(_pid);
 Listening();
 pcur->KillClients(_pid);
}
     

}

void AMSServer::Exiting(const char * message){
if(_ExitInProgress)return;
cout<< " Exiting ...."<<(message?message:" ")<<endl;
_ExitInProgress=true;
  Server_impl* pser=dynamic_cast<Server_impl*>(_pser);
    DPS::Client::ARS * pars;
    int length=pser?pser->getARS(_pid,pars):0;
    DPS::Client::ARS_var arf=pars;
  for(int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=(_orbmap.find((const char*)(arf[i].Interface))->second)._orb->string_to_object(arf[i].IOR);

    DPS::Server_var _pvar=DPS::Server::_narrow(obj);
    _pvar->Exiting(_pid,(message?message:AMSClient::_error.getMessage()),AMSClient::_error.ExitReason());
     break;
   }
   catch (CORBA::SystemException &ex){
     // Have to Kill Servers Here
     
   }
}
}

void AMSServerI::_UpdateACT(const DPS::Client::CID & cid, DPS::Client::ClientStatus st){
     for(ACLI j=_acl.begin();j!=_acl.end();++j){
     if((*j)->id.uid==cid.uid){
       time_t tt;
       time(&tt);
       (*j)->LastUpdate=tt;
       (*j)->Status=st;
       DPS::Client::ActiveClient_var acv=*j;
       PropagateAC(acv,DPS::Client::Update);
       return;
     }   
     }
}

void AMSServerI::PropagateAC(DPS::Client::ActiveClient & ac,DPS::Client::RecordChange rc,DPS::Client::AccessType type, uinteger uid){

  PropagateACDB(ac,rc);

  if(_ActivateQueue)_acqueue.push_back(ACA(ac,rc));
  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
  DPS::Client::CID cid;
  cid.Type=_pser->getType();
  cid.Interface= (const char *) " ";
    DPS::Client::ARS * pars;
    int length=_pser->getARS(cid,pars,type,uid);
    DPS::Client::ARS_var arf=pars;
  if(length==0){
      _parent->EMessage(AMSClient::print(ac,"PropagateAC-getARS retrurns 0"));
  }
//  cout <<"propagateAC - getars returns "<<length<<endl;
  time_t tt;
  time(&tt);
  for(int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Server_var _pvar=DPS::Server::_narrow(obj);
     DPS::Client::CID acid=ac.id;
    _pvar->sendAC(acid,ac,rc);
   }
   catch (CORBA::SystemException &ex){
     cerr<<"oops corba exc found for "<<i<<" "<<ctime(&tt)<< endl;
/*
     DPS::Client::ActiveClient ac;
     ac.id.uid=arf[i].uid;
     ACLI li=find_if(_pser->getacl().begin(),_pser->getacl().end(),Eqs(ac));
     if(li!=_pser->getacl().end()){
      (*li)->Status==DPS::Client::TimeOut;
      PropagateAC(*li,DPS::Client::Update,DPS::Client::AnyButSelf,ac.id.uid);
     }
     else{
      _parent->EMessage(AMSClient::print(ac," PropagateRunExceptionClientNotFou\
nd"));
     }
*/
   }
  } 
  }   

void AMSServerI::PropagateAH(const DPS::Client::CID & pid,DPS::Client::ActiveHost & ah,DPS::Client::RecordChange rc,DPS::Client::AccessType type, uinteger uid){
  PropagateAHDB(pid,ah,rc);
  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
  DPS::Client::CID cid;
  cid.Type=_pser->getType();
  cid.Interface= (const char *) " ";
    DPS::Client::ARS * pars;
    int length=_pser->getARS(cid,pars,type,uid);
    DPS::Client::ARS_var arf=pars;
  if(length==0){
      _parent->EMessage(AMSClient::print(ah,"PropagateAC-getARS retrurns 0"));
  }
//  cout <<"propagateAC - getars returns "<<length<<endl;
  time_t tt;
  time(&tt);
  for(int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Server_var _pvar=DPS::Server::_narrow(obj);
    _pvar->sendAH(pid,ah,rc);
   }
   catch (CORBA::SystemException &ex){
     cerr<<"oops corba exc found for "<<i<<" "<<ctime(&tt)<<endl;
   }
  } 
  }   

void AMSServerI::RegisteredClientExists(){
  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
for(ACLI li= _pser->getacl().begin();li!=_pser->getacl().end();++li){
 if((*li)->Status==DPS::Client::Registered){
 if(!_ActivateQueue){
   if(_parent->Debug())_parent->IMessage("Queue Activated");
   _ActivateQueue=true;
   _RecID=((*li)->id.uid);
 }   
   return;
 }
}
if(_ActivateQueue){
   _ActivateQueue=false;
   _PurgeQueue();
   if(_parent->Debug())_parent->IMessage("Queue Purged");

}
}

bool AMSServerI::InactiveClientExists(DPS::Client::ClientType ctype){
for(ACLI li= _acl.begin();li!=_acl.end();++li){
 if((*li)->Status!=DPS::Client::Active){
//   if(_parent->Debug())_parent->IMessage(AMSClient::print(*li," Inactive Client Found"));
   return true;
  }
}
if(getType() == DPS::Client::DBServer && getType()!=ctype){
  if(_parent->DBServerExists() && !_acl.size())return true;
}
return false;
}


Server_impl::Server_impl(const map<AString, AMSServer::OrbitVars> & mo,  const DPS::Client::CID & cid,AMSClient* parent, char* NS, char * NH, char *NK): POA_DPS::Server(),AMSServerI(AMSID("Server",0),parent,DPS::Client::Server){

typedef map<AString, AMSServer::OrbitVars>::const_iterator MOI;

Server_impl * pcur =0;

for(MOI i=mo.begin();i!=mo.end();++i){
   if(!pcur)pcur=this;
//   else add(pcur = new Server_impl());
  PortableServer::ObjectId_var oid=(i->second)._poa->activate_object(pcur);
  DPS::Server_var _ref = reinterpret_cast<DPS::Server_ptr>((i->second)._poa->id_to_reference(oid));
   _refmap[i->first]=(i->second)._orb->object_to_string(_ref);
   if(!strcmp((const char *)(i->first),(const char*)cid.Interface)){
    _defaultorb=(i->second)._orb;
   }
}
if(NS){
 ifstream fbin;
 fbin.open(NS);
 if(fbin){
   unsigned int uid=0;
   DPS::Client::NominalClient_var ncl= new DPS::Client::NominalClient();
   if(fbin.get()=='#')fbin.ignore(1024,'\n');
   else fbin.seekg(fbin.tellg()-sizeof(char));
   fbin>>ncl->MaxClients>>ncl->CPUNeeded>>ncl->MemoryNeeded;
    char tmpbuf[1024];
    fbin>>tmpbuf;
    ncl->WholeScriptPath=(const char*)tmpbuf;
    ncl->Type=DPS::Client::Server;
   fbin.ignore(1024,'\n');
  while(!fbin.eof() && fbin.good()){ 
   fbin>>tmpbuf;
   ncl->HostName=(const char*)tmpbuf;
   fbin>>ncl->LogInTheEnd;
   fbin.ignore(1024,'\n');
   fbin.getline(tmpbuf,1024);
   ncl->LogPath= (const char*)tmpbuf;
   fbin.getline(tmpbuf,1024);
    ncl->SubmitCommand=(const char*)tmpbuf;
    ncl->uid=++uid;
    if(fbin.good())_ncl.push_back(ncl);
 }
 }
 else{
  cerr<<"Server_impl::Server_impl-F-UnableToOpenNSFile "<<NS<<endl;
  _parent->FMessage("Server_impl::Server_impl-F-UnableToOpenNSFile ",DPS::Client::CInAbort);
}
}
else _parent->FMessage("Server_impl::Server_impl-F-NoNSFile",DPS::Client::CInAbort);
if(NK){
 ifstream fbin;
 fbin.open(NK);
 if(fbin){
   unsigned int uid=0;
   DPS::Client::NominalClient_var ncl= new DPS::Client::NominalClient();
   if(fbin.get()=='#')fbin.ignore(1024,'\n');
   else fbin.seekg(fbin.tellg()-sizeof(char));
   fbin>>ncl->MaxClients>>ncl->CPUNeeded>>ncl->MemoryNeeded;
    char tmpbuf[1024];
    fbin>>tmpbuf;
    ncl->Type=DPS::Client::Killer;
    ncl->WholeScriptPath=(const char*)tmpbuf;
   fbin.ignore(1024,'\n');
  while(!fbin.eof() && fbin.good()){ 
   fbin>>tmpbuf;
   ncl->HostName=(const char*)tmpbuf;
   fbin>>ncl->LogInTheEnd;
   fbin.ignore(1024,'\n');
   fbin.getline(tmpbuf,1024);
   ncl->LogPath= (const char*)tmpbuf;
   fbin.getline(tmpbuf,1024);
    ncl->SubmitCommand=(const char*)tmpbuf;
    ncl->uid=++uid;
    if(fbin.good())_nki.push_back(ncl);
  }
 }
 else{
  cerr<<"Server_impl::Server_impl-F-UnableToOpenNKFile "<<NK<<endl;
  _parent->FMessage("Server_impl::Server_impl-F-UnableToOpenNKFile ",DPS::Client::CInAbort);
}
}
else _parent->FMessage("Server_impl::Server_impl-F-NoNKFile",DPS::Client::CInAbort);
if(NH){
 ifstream fbin;
 fbin.open(NH);
 if(fbin){
   if(fbin.get()=='#')fbin.ignore(1024,'\n');
   else fbin.seekg(fbin.tellg()-sizeof(char));
  while(!fbin.eof() && fbin.good()){ 
   DPS::Server::NominalHost nh;
   char tmpbuf[1024];
   fbin>>tmpbuf;
   nh.HostName=(const char*)tmpbuf;
   fbin>>tmpbuf;
   nh.Interface=(const char*)tmpbuf;
   AString hn((const char*)(nh.HostName));
   hn+=".";
   hn+=(const char*)nh.Interface;
   nh.HostName=(const char *)hn;
   fbin>>tmpbuf;
   nh.OS= (const char*)tmpbuf;
   fbin>>nh.CPUNumber>>nh.Memory>>nh.Clock;
   DPS::Client::NominalHost_var vnh= new  DPS::Client::NominalHost(nh);
   if(fbin.good())_nhl.push_back(vnh);  
 }
 }
 else{
  cerr<<"Server_impl::Server_impl-F-UnableToOpenNHFile "<<NH<<endl;
  _parent->FMessage("Server_impl::Server_impl-F-UnableToOpenNHFile ",DPS::Client::CInAbort);
}
}
else _parent->FMessage("Server_impl::Server_impl-F-NoNHFile",DPS::Client::CInAbort);

}


Server_impl::Server_impl(const map<AString, AMSServer::OrbitVars> & mo, DPS::Server_ptr _pvar, const DPS::Client::CID & cid,AMSClient* parent): POA_DPS::Server(),AMSServerI(AMSID("Server",0),parent,DPS::Client::Server){
typedef map<AString, AMSServer::OrbitVars>::const_iterator MOI;

Server_impl * pcur =0;

for(MOI i=mo.begin();i!=mo.end();++i){
   if(!pcur)pcur=this;
//   else add(pcur = new Server_impl());
  PortableServer::ObjectId_var oid=(i->second)._poa->activate_object(pcur);
  DPS::Server_var _ref = reinterpret_cast<DPS::Server_ptr>((i->second)._poa->id_to_reference(oid));
   _refmap[i->first]=(i->second)._orb->object_to_string(_ref);
   if(!strcmp((const char *)(i->first),(const char*)cid.Interface)){
    _defaultorb=(i->second)._orb;
   }
}
//Get NominalServer
{
DPS::Client::NCS * pncs;
int length=_pvar->getNC(cid,pncs);
NCS_var ncs=pncs;
for(int i=0;i<length;i++){
 DPS::Client::NominalClient_var vnh= new DPS::Client::NominalClient(ncs[i]);
 _ncl.push_back(vnh);
}
}
//Get NominalKiller

DPS::Client::NCS * pncs;
int length=_pvar->getNK(cid,pncs);
NCS_var ncs=pncs;
for(int i=0;i<length;i++){
 DPS::Client::NominalClient_var vnh= new DPS::Client::NominalClient(ncs[i]);
 _nki.push_back(vnh);
}

//Now Read NominalHost

DPS::Client::NHS * pnhs;
length=_pvar->getNHS(cid,pnhs);
NHS_var nhs=pnhs;
for(int i=0;i<length;i++){
 DPS::Client::NominalHost_var vnh= new DPS::Client::NominalHost(nhs[i]);
 _nhl.push_back(vnh);
}

//Now Read ActiveHost

DPS::Client::AHS * pahs;
length=_pvar->getAHS(cid,pahs);
AHS_var ahs=pahs;
for(int i=0;i<length;i++){
 DPS::Client::ActiveHost_var vah= new DPS::Client::ActiveHost(ahs[i]);
 _parent->IMessage(AMSClient::print(vah, "getAHS: "));
 _ahl.push_back(vah);
}

//Now Read ActiveClient

DPS::Client::ACS * pacs;
length=_pvar->getACS(cid,pacs,_Submit);
ACS_var acs=pacs;
for(int i=0;i<length;i++){
 DPS::Client::ActiveClient_var vac= new DPS::Client::ActiveClient(acs[i]);
 _acl.push_back(vac);
}
 cout <<"ACL size"<<_acl.size()<<" "<<(*_ncl.begin())->MaxClients<<" "<<_Submit<<" "<<endl;

}



void Server_impl::StartClients(const DPS::Client::CID & pid){
if(!Master())return;
RegisteredClientExists();
for(AMSServerI * pcur=getServer(); pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
if(pcur->InactiveClientExists(getType()))return;
 
}
  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 



 // Check if there are some hosts to run on
 DPS::Client::ActiveHost_var ahlv=new DPS::Client::ActiveHost();
 bool suc=false;
 if(_parent->DBServerExists() && getHostInfoSDB(_parent->getcid(),ahlv)){
  suc=true;
 }
 else{
 _ahl.sort(Less());
if(_acl.size()<(*_ncl.begin())->MaxClients ){
 for(AHLI i=_ahl.begin();i!=_ahl.end();++i){
  if((*i)->Status!=NoResponse && (*i)->Status!=InActive){
    if((*i)->ClientsRunning>=(*i)->ClientsAllowed)continue;
    // HereStartClient
    ahlv=*i;
    suc=true;
    break;
   }
  }
 }
 }
  if(suc){
  if(!_pser->Lock(pid,StartClient,getType(),_StartTimeOut))return;
   CORBA::String_var _refstring=_refmap.find((const char *)((ahlv)->Interface))->second;
#ifdef __AMSDEBUG__
    cout <<((ahlv)->Interface)<<" "<<_refstring<<endl;
#endif
    NCLI cli=find_if(_ncl.begin(),_ncl.end(),NCL_find((const char *)(ahlv)->HostName)); 
    if(cli==_ncl.end())cli=_ncl.begin();
    AString submit;
    submit+=(const char*)((*cli)->SubmitCommand);  
    submit+=" ";
    submit+=(const char*)((ahlv)->HostName);
     char tmp[80];
     sprintf(tmp,"%d",_Submit+1);
     submit+=" ";
    if(!(*cli)->LogInTheEnd){
     submit+=(const char*)((*cli)->LogPath);  
     submit+="Server.";
     submit+=tmp;
     submit+=".log ";
    }
    submit+=(const char*)((*cli)->WholeScriptPath);  
    submit+=" -C";
    submit+=(const char*) _refstring;
    submit+=" -U";
    submit+=tmp;
    submit+=" -F";
    submit+= _iface;
    submit+=" -I";
    submit+=(const char*)(ahlv)->Interface;
    if(_parent->IsOracle())submit+=" -O";
    if(_parent->Debug())submit+=" -D1";
    UpdateDBFileName(); 
    if(_parent->getdbfile()){
     submit+=" -B";
     submit+=_parent->getdbfile();
    }
    submit+=" -b";
    sprintf(tmp,"%d",_parent->getmaxdb());
    submit+=tmp;
    submit+=" -A";
     submit+=getenv("AMSDataDir");
     if(getenv("AMSProdOutputDir")){
      submit+=" -a";
      submit+=getenv("AMSProdOutputDir");
     }
    if((*cli)->LogInTheEnd){
     submit+=" ";
     submit+=(const char*)((*cli)->LogPath);  
     submit+="Server.";
     submit+=tmp;
     submit+=".log ";
    }
    if(_parent->Debug())_parent->IMessage(submit);
    submit+=" &";
    int out=system(submit);
     time_t tt;
     time(&tt);
     (ahlv)->LastUpdate=tt;     
    if(out==0){
     // Add New Active Client
     DPS::Client::ActiveClient ac;
     (ac.id).HostName=CORBA::string_dup((ahlv)->HostName);
     ac.id.Interface=(ahlv)->Interface;
     (ac.id).uid=_Submit+1;
     ac.id.Type=getType();
     ac.id.ppid=0;
     ac.id.Status=DPS::Client::NOP;
     ac.Status=DPS::Client::Submitted;
     time_t tt;
     time(&tt);
     ac.LastUpdate=tt;     
     ac.Start=tt;
     (ac.ars).length(1);
     ((ac.ars)[0]).Interface=(const char *)("Dummy");
     ((ac.ars)[0]).IOR=(const char *)(" ");
     ((ac.ars)[0]).Type=DPS::Client::Generic;
     ((ac.ars)[0]).uid=0;
/*
     _acl.push_back(ac);
*/
//     ((ahlv)->ClientsRunning)++;
    (ahlv)->Status=DPS::Client::OK; 
     DPS::Client::CID cid=_parent->getcid();      
     cid.Type=getType();
     cid.Interface= (const char *) " "; 
    PropagateAH(cid,(ahlv),DPS::Client::Update);
    DPS::Client::ActiveClient_var acv=new DPS::Client::ActiveClient(ac);
    PropagateAC(acv,DPS::Client::Create);
//      Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
//      _pser->MonInfo("Started client ",DPS::Client::Info);
    }
    else{
     (ahlv)->Status=DPS::Client::LastClientFailed; 
     ((ahlv)->ClientsFailed)++; 
     DPS::Client::CID cid=_parent->getcid();      
     cid.Type=getType();
     cid.Interface= (const char *) " "; 
     PropagateAH(cid,(ahlv),DPS::Client::Update);

     AString a="StartClients-E-UnableToStartClient ";
     a+=submit;
     _parent->EMessage((const char *) a);
      Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
      _pser->MonInfo((const char *) a,DPS::Client::Error);

     
    }
    _pser->Lock(pid,ClearStartClient,getType(),_StartTimeOut);
  }
}

#include <signal.h>
void Server_impl::KillClients(const DPS::Client::CID & pid){
if(!Master())return;
Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
if(!_pser->Lock(pid,DPS::Server::KillClient,getType(),_KillTimeOut))return;

ACLI li=find_if(_acl.begin(),_acl.end(),find(DPS::Client::Killed));
if(li!=_acl.end()){
 //kill by -9 here
   if(_parent->Debug())_parent->EMessage(AMSClient::print(*li, " KILL -9"));
    int iret=_pser->Kill((*li),SIGKILL,true);
    if(iret){
     _parent->EMessage(AMSClient::print(*li,"Server::Unable To Kill Client"));
    }
    DPS::Client::ActiveClient_var acv=*li;
    PropagateAC(acv,DPS::Client::Delete);
}
 li=find_if(_acl.begin(),_acl.end(),find(DPS::Client::TimeOut));
if(li!=_acl.end()){
 
 if(!_pser->pingHost((const char*)((*li)->id.HostName))){
    for(AHLI i=_ahl.begin();i!=_ahl.end();++i){
      if(!strcmp((const char *)(*i)->HostName, (const char *)((*li)->id).HostName)){
       (*i)->Status=NoResponse;
       _pser->MonInfo(AMSClient::print(*i,"No Response From: "),DPS::Client::Warning);
    }
}  
   (*li)->id.Status=DPS::Client::SInKill;
   DPS::Client::ActiveClient_var acv=*li;
   PropagateAC(acv,DPS::Client::Delete);
 }
  else{
   if(_pser->MonDialog(AMSClient::print(*li,"Asking To Kill Client: "),DPS::Client::Error)){
   (*li)->id.Status=DPS::Client::SInKill;
   (*li)->Status=DPS::Client::Killed;
   DPS::Client::ActiveClient_var acv=*li;
   PropagateAC(acv,DPS::Client::Update);
    //_pser->Kill((*li),SIGTERM,true);
  }
  else{
    _UpdateACT((*li)->id,DPS::Client::Active);
  }
 }
}

_pser->Lock(pid,DPS::Server::ClearKillClient,getType(),_KillTimeOut);


}

void Server_impl::CheckClients(const DPS::Client::CID & cid){
if(!Master())return;
Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
if(!_pser->Lock(cid,DPS::Server::CheckClient,getType(),_KillTimeOut))return;
for(AHLI li=_ahl.begin();li!=_ahl.end();++li){
 if((*li)->Status==NoResponse && double(rand())/RAND_MAX>0.98)if(_pser->pingHost((const char*)((*li)->HostName)))(*li)->Status=DPS::Client::OK;
}
time_t tt;
time(&tt);
for(ACLI li=_acl.begin();li!=_acl.end();++li){
 // find clients with timeout
 if((*li)->Status!=DPS::Client::Killed && (*li)->LastUpdate+_KillTimeOut<tt){
   ;
   if(PingServer(*li)){
     
    _UpdateACT((*li)->id,DPS::Client::Active);
//    (*li)->LastUpdate=tt;
   }
   else{
    DPS::Client::ActiveClient_var acv=*li;
    acv->Status=DPS::Client::TimeOut;
    if(_parent->Debug())_parent->EMessage(AMSClient::print(acv,"Client TIMEOUT"));
    PropagateAC(acv,DPS::Client::Update,DPS::Client::AnyButSelf,acv->id.uid);
 }
 }
}

_pser->Lock(cid,DPS::Server::ClearCheckClient,getType(),_KillTimeOut);
}


void Server_impl::_init(){
// here active host list
if(_ahl.size())return;
 Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
 if(_pser){
  for(NHLI i=(_pser->getnhl()).begin();i!=(_pser->getnhl()).end();++i){
   DPS::Client::ActiveHost ah;
   ah.HostName=CORBA::string_dup((*i)->HostName);
   ah.Interface=CORBA::string_dup((*i)->Interface);
   if(_pser->pingHost((const char*)(ah.HostName)))ah.Status=DPS::Client::OK; 
   else ah.Status=DPS::Client::NoResponse; 
   ah.ClientsProcessed=0;
   ah.ClientsFailed=0;
   ah.ClientsKilled=0;
   ah.ClientsRunning=0;
   ah.Clock=(*i)->Clock;  
   ah.ClientsAllowed=min((*i)->CPUNumber/(*_ncl.begin())->CPUNeeded,(*i)->Memory/float((*_ncl.begin())->MemoryNeeded));
 
   time_t tt;
   time(&tt);
   ah.LastUpdate=tt;
//Check Interface
   if(_refmap.find((const char *)(ah.Interface))==_refmap.end()){
   ah.Interface=(const char *)((_refmap.begin())->first);
#ifdef __AMSDEBUG__
       if((_refmap.begin())->first != "default"){
        cerr<<"Server_impl::_init-S-FirstRefmapEntry!=default "<<(_refmap.begin())->first<<endl;
   }
#endif
   }
   DPS::Client::ActiveHost_var vah= new DPS::Client::ActiveHost(ah);
   _ahl.push_back(vah);
  }
 }
 else{
  _parent->FMessage("Server_impl::_init-F-UnableToConnectToServer",DPS::Client::CInAbort);
 }

}



  CORBA::Boolean Server_impl::sendId(DPS::Client::CID& cid, uinteger timeout) throw (CORBA::SystemException){
if(cid.Type==DPS::Client::Server){
 _KillTimeOut=timeout;
     for(ACLI j=_acl.begin();j!=_acl.end();++j){
      if(((*j)->id).uid==cid.uid){
       ((*j)->id).pid=cid.pid;
       ((*j)->id).ppid=cid.ppid;
       cid.Interface=CORBA::string_dup(((*j)->id).Interface);
       cid.Type=((*j)->id).Type;
       (*j)->Status=DPS::Client::Registered;
       time_t tt;
       time(&tt);
       (*j)->LastUpdate=tt;
       DPS::Client::ActiveClient_var acv=*j;
       PropagateAC(acv,DPS::Client::Update);
#ifdef __AMSDEBUG__
        _parent->IMessage(AMSClient::print(cid,"Server_impl::sendId-I-RegClient") );
#endif 
       return true;
      }
     }
     return false;
}
else if(cid.Type==DPS::Client::Monitor){
  for (AMSServerI* pcur=this;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
    if(pcur->getType()==cid.Type){
      (dynamic_cast<Client_impl*>(pcur))->sendId(cid,timeout);
      break;
    }
}
}
else if(cid.Type==DPS::Client::DBServer){
  for (AMSServerI* pcur=this;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
    if(pcur->getType()==cid.Type){
      (dynamic_cast<Client_impl*>(pcur))->sendId(cid,timeout);
      break;
    }
}
}
}

int Server_impl::getNC(const DPS::Client::CID &cid, NCS_out acs)throw (CORBA::SystemException){


 if(_parent->DBServerExists()){
        Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
     DPS::Client::CID pid=_parent->getcid();
     pid.Type=DPS::Client::DBServer;
     pid.Interface= (const char *) " ";
     DPS::Client::ARS * pars;
     int length=_pser->getARS(pid,pars,DPS::Client::Any,0,1);
     DPS::Client::ARS_var arf=pars;
     for(int i=0;i<length;i++){
      try{
       CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
       DPS::DBServer_var _pvar=DPS::DBServer::_narrow(obj);
          return _pvar->getNC(cid,acs);
       }
       catch(DPS::DBServer::DBProblem &dbl){
       _parent->EMessage((const char*)dbl.message); 
       }
       catch (CORBA::SystemException &ex){
        // Have to Kill Servers Here
        _parent->EMessage("dbserver corba error during getnc" );   
       }
     }
 }


NCS_var acv= new NCS();
int length=0;
for(AMSServerI * pser=this;pser;pser= pser->next()?pser->next():pser->down()){
if(pser->getType()==cid.Type){
length=pser->getncl().size();
if(length==0){
acv->length(1);
}
else{
acv->length(length);
length=0;
for(NCLI li=pser->getncl().begin();li!=pser->getncl().end();++li){
 acv[length++]=*li;
}
}
acs=acv._retn();
return length;
}
}


}

int Server_impl::getNK(const DPS::Client::CID &cid, NCS_out acs)throw (CORBA::SystemException){
 if(_parent->DBServerExists()){
        Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
     DPS::Client::CID pid=_parent->getcid();
     pid.Type=DPS::Client::DBServer;
     pid.Interface= (const char *) " ";
     DPS::Client::ARS * pars;
     int length=_pser->getARS(pid,pars,DPS::Client::Any,0,1);
     DPS::Client::ARS_var arf=pars;
     for(int i=0;i<length;i++){
      try{
       CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
       DPS::DBServer_var _pvar=DPS::DBServer::_narrow(obj);
          return _pvar->getNK(cid,acs);
       }
       catch(DPS::DBServer::DBProblem &dbl){
       _parent->EMessage((const char*)dbl.message); 
       }
       catch (CORBA::SystemException &ex){
        // Have to Kill Servers Here
        _parent->EMessage("dbserver corba error during getnk" );   
       }
     }
 }

NCS_var acv= new NCS();
int length=0;
length=_nki.size();
if(length==0){
acv->length(1);
}
else{
acv->length(length);
length=0;
for(NCLI li=_nki.begin();li!=_nki.end();++li){
 acv[length++]=*li;
}
}
acs=acv._retn();
return length;
}



bool Server_impl::ARSaux(DPS::Client::AccessType type,uint id, uint compare){
 switch (type){
  case DPS::Client::Any:
  return true;
  case DPS::Client::LessThan:
   return compare<id;
  case DPS::Client::Self:
   return compare==id;
  case DPS::Client::AnyButSelf:
   return compare!=id;
  case DPS::Client::GreaterThan:
   return compare>id;
  default:
   return false;
 }
}


int Server_impl::getARS(const DPS::Client::CID & cid, DPS::Client::ARS_out  arf,DPS::Client::AccessType type,uinteger maxcid, int selffirst)throw (CORBA::SystemException){
DPS::Client::ARS_var arv=new ARS();
int length=0;
AMSServerI * _pser=getServer();
AMSServerI *  pser=0;
if(cid.Type == DPS::Client::Server || cid.Type == DPS::Client::Producer){
 pser=_pser;
}
else{
 for(AMSServerI * pcur=_pser; pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
  if(pcur->getType() == cid.Type){
    pser=pcur;
  }
}
}
if(!pser)pser=_pser;
   pser->getacl().sort(Less(cid,selffirst));
for (ACLI li=pser->getacl().begin();li!=pser->getacl().end();++li){
    bool pred=ARSaux(type,maxcid,(*li)->id.uid);
  if(pred){
   for (int i=0;i<((*li)->ars).length();i++){
    if( (((*li)->ars)[i]).Type == cid.Type && !strcmp((const char *)  (((*li)->ars)[i]).Interface, (const char *)cid.Interface)){
      length++;
    }
    else if( !strcmp(" ", (const char *)cid.Interface) && (((*li)->ars)[i]).Type == cid.Type && !strcmp((const char *)  (((*li)->ars)[i]).Interface, (const char *)((*li)->id).Interface)){
      length++;
    }
  }
 }
}

if(length==0){ 
  arv->length(1);
//  cerr<<"***ERROR*** getars-length== 0"<<endl;
}
else arv->length(length);
length=0;
for (ACLI li=pser->getacl().begin();li!=pser->getacl().end();++li){
    bool pred=ARSaux(type,maxcid,(*li)->id.uid);
  if(pred){
  for (int i=0;i<((*li)->ars).length();i++){
    if( (((*li)->ars)[i]).Type == cid.Type && !strcmp((const char *)  (((*li)->ars)[i]).Interface, (const char *)cid.Interface)){
      arv[length++]=((*li)->ars)[i];
    }
    else if( !strcmp(" ", (const char *)cid.Interface) && (((*li)->ars)[i]).Type == cid.Type && !strcmp((const char *)  (((*li)->ars)[i]).Interface, (const char *)((*li)->id).Interface)){
      arv[length++]=((*li)->ars)[i];
    }
  }
 }
}


arf=arv._retn();
return length;
}




int Server_impl::getACS(const DPS::Client::CID &cid, ACS_out acs, unsigned int & maxc)throw (CORBA::SystemException){

 if(_parent->DBServerExists()){
        Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
     DPS::Client::CID pid=_parent->getcid();
     pid.Type=DPS::Client::DBServer;
     pid.Interface= (const char *) " ";
     DPS::Client::ARS * pars;
     int length=_pser->getARS(pid,pars,DPS::Client::Any,0,1);
     DPS::Client::ARS_var arf=pars;
     for(int i=0;i<length;i++){
      try{
       CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
       DPS::DBServer_var _pvar=DPS::DBServer::_narrow(obj);
          return _pvar->getACS(cid,acs,maxc);
       }
       catch(DPS::DBServer::DBProblem &dbl){
       _parent->EMessage((const char*)dbl.message); 
       }
       catch (CORBA::SystemException &ex){
        // Have to Kill Servers Here
        _parent->EMessage("dbserver corba error during getacs" );   
       }
     }
 }


//_parent->IMessage(AMSClient::print(cid," type "));
ACS_var acv= new ACS();
int length=0;
for(AMSServerI * pser=this;pser;pser= pser->next()?pser->next():pser->down()){
if(pser->getType()==cid.Type){
maxc=pser->getmaxcl();
length=pser->getacl().size();
if(length==0){
acv->length(1);
}
else{
acv->length(length);
length=0;
for(ACLI li=pser->getacl().begin();li!=pser->getacl().end();++li){
 acv[length++]=*li;
// _parent->IMessage(AMSClient::print(acv[length-1],"getacs"));
}
}
acs=acv._retn();
return length;
}
}
return 0;

}



   void Server_impl::sendAC(const DPS::Client::CID &  cid,   DPS::Client::ActiveClient & ac, DPS::Client::RecordChange rc)throw (CORBA::SystemException){
  for (AMSServerI* pcur=this;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
    if(pcur->getType()==cid.Type){
      ACLI li=find_if(pcur->getacl().begin(),pcur->getacl().end(),Eqs(ac));
      switch (rc){
       case DPS::Client::Update:
       if(li==pcur->getacl().end())_parent->EMessage(AMSClient::print(cid,"Client not found for editing"));
       else{
        DPS::Client::ActiveClient_var vac= new DPS::Client::ActiveClient(ac);
        replace_if(li,pcur->getacl().end(),Eqs(ac),vac);
       }
       break;
       case DPS::Client::Delete:
       if(li==pcur->getacl().end())_parent->EMessage(AMSClient::print(cid,"Client not found for deleting"));
       else{
        if(_parent->Debug())_parent->IMessage(AMSClient::print(ac,"Deleting Client "));
        Producer_impl * pprod=dynamic_cast<Producer_impl*>(pcur);
        if(pprod)pprod->RunFailed(ac);
//      Here find the corr Ahost and update it
         for(AHLI i=pcur->getahl().begin();i!=pcur->getahl().end();++i){
            if(!strcmp((const char *)(*i)->HostName, (const char *)(ac.id).HostName)){
//          cout << " host found for deleteing "<<endl;
        ((*i)->ClientsRunning)--;
       ((*i)->ClientsProcessed)++; 
     time_t tt;
     time(&tt);
     (*i)->LastUpdate=tt;     

       switch ((ac.id).Status){
       case DPS::Client::CInExit: 
            (*i)->Status=DPS::Client::OK; 
        break;
        case DPS::Client::SInExit:
           (*i)->Status=DPS::Client::OK; 
            break;
       case DPS::Client::CInAbort:
        (*i)->Status=DPS::Client::LastClientFailed; 
        ((*i)->ClientsFailed)++; 
        break;
       case DPS::Client::SInKill:
        (*i)->Status=DPS::Client::LastClientFailed; 
        ((*i)->ClientsKilled)++; 
        break;
       }  
        break;
       }
       }
        pcur->getacl().erase(li);
       }
       break;
       case DPS::Client::Create:
       if(li !=pcur->getacl().end()){
         _parent->EMessage(AMSClient::print(*li,"Client alreadyexists"));
       }
       else{
        pcur->addone();
        if(_parent->Debug())_parent->IMessage(AMSClient::print(ac,"added client "));
        DPS::Client::ActiveClient_var vac= new DPS::Client::ActiveClient(ac);
       pcur->getacl().push_back(vac);
       cout <<pcur->getacl().size()<<endl;
//     Here find the corr ahost and update it
         for(AHLI i=pcur->getahl().begin();i!=pcur->getahl().end();++i){
            if(!strcmp((const char *)(*i)->HostName, (const char *)(vac->id).HostName)){
//          cout << " host found for creating "<<endl;
        ((*i)->ClientsRunning)++;
        break;
}
         }
       }       
       break;
      }
    break;
    }
}
}
   void Server_impl::sendAH(const DPS::Client::CID &  cid,  const DPS::Client::ActiveHost & ah, DPS::Client::RecordChange rc)throw (CORBA::SystemException){
  for (AMSServerI* pcur=this;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
    if(pcur->getType()==cid.Type){
      AHLI li=find_if(pcur->getahl().begin(),pcur->getahl().end(),Eqs_h(ah));
      switch (rc){
       case DPS::Client::Update:
       if(li==pcur->getahl().end())_parent->EMessage(AMSClient::print(ah,"Host not found for editing"));
       else{
        DPS::Client::ActiveHost_var vac= new DPS::Client::ActiveHost(ah);
        cout <<" SendAH-I-"<<" "<<cid.Type<<" "<<ah.HostName<<" "<<(*li)->HostName<<endl;
        replace_if(li,pcur->getahl().end(),Eqs_h(ah),vac);
       }
       break;
       case DPS::Client::Delete:
       if(li==pcur->getahl().end())_parent->EMessage(AMSClient::print(cid,"Host not found for deleting"));
       else{
        if(_parent->Debug())_parent->IMessage(AMSClient::print(ah,"Deleting Host "));
        pcur->getahl().erase(li);
       }
       break;
       case DPS::Client::Create:
       if(li!=pcur->getahl().end())_parent->EMessage(AMSClient::print(ah,"Host already exists"));
       else{
        DPS::Client::ActiveHost_var vac= new DPS::Client::ActiveHost(ah);
         pcur->getahl().push_back(vac);
       }
       break;
       default:
        _parent->EMessage(AMSClient::print(ah,"sendAH-Unknown rc value"));
      }
    break;
    }
}
}


   void Server_impl::sendNH(const DPS::Client::CID &  cid, const DPS::Client::NominalHost & ah, DPS::Client::RecordChange rc)throw (CORBA::SystemException){
  for (AMSServerI* pcur=this;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
    if(pcur->getType()==cid.Type){
      NHLI li=find_if(pcur->getnhl().begin(),pcur->getnhl().end(),Eqs_nh(ah));
      switch (rc){
       case DPS::Client::Update:
       if(li==pcur->getnhl().end())_parent->EMessage(AMSClient::print(cid,"Host not found for editing"));
       else{
        DPS::Client::NominalHost_var vac= new DPS::Client::NominalHost(ah);
        replace_if(li,pcur->getnhl().end(),Eqs_nh(ah),vac);
       }
       break;
       case DPS::Client::Delete:
       if(li==pcur->getnhl().end())_parent->EMessage(AMSClient::print(cid,"Host not found for deleting"));
       else{
        if(_parent->Debug())_parent->IMessage(AMSClient::print(ah,"Deleting Host "));
        pcur->getnhl().erase(li);
       }
       break;
       case DPS::Client::Create:
       if(li!=pcur->getnhl().end())_parent->EMessage(AMSClient::print(ah,"Host already exists"));
       else{
        DPS::Client::NominalHost_var vac= new DPS::Client::NominalHost(ah);
         pcur->getnhl().push_back(vac);
       }
       break;
       default:
        _parent->EMessage(AMSClient::print(ah,"sendNH-Unknown rc value"));
      }
    break;
    }
}
}



void Server_impl::Exiting(const CID & cid, const char * message, DPS::Client::ClientExiting status)throw (CORBA::SystemException){
// find and remove client
if(cid.Type==DPS::Client::Server){
_parent->IMessage(AMSClient::print(cid,message?message:"Server exiting"));
Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
_pser->MonInfo(AMSClient::print(cid,message?message:" Server Exiting "),DPS::Client::Info);
for( ACLI li=_acl.begin();li!=_acl.end();++li){
 if (cid.uid==((*li)->id).uid){
   (*li)->id.Status=status;
   DPS::Client::ActiveClient_var acv=*li;
   PropagateAC(acv,DPS::Client::Delete,DPS::Client::AnyButSelf,cid.uid);
   return;
 }

}
 _parent->EMessage(AMSClient::print(cid,"Server_impl::Exiting::No Such Client"));
}
else{
  for (AMSServerI* pcur=this;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
    if(pcur->getType()==cid.Type){
      (dynamic_cast<Client_impl*>(pcur))->Exiting(cid,message,status);
      break;
    }
}
}


}

int Server_impl::getNHS(const DPS::Client::CID &cid, NHS_out acs)throw (CORBA::SystemException){

 if(_parent->DBServerExists()){
        Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
     DPS::Client::CID pid=_parent->getcid();
     pid.Type=DPS::Client::DBServer;
     pid.Interface= (const char *) " ";
     DPS::Client::ARS * pars;
     int length=_pser->getARS(pid,pars,DPS::Client::Any,0,1);
     DPS::Client::ARS_var arf=pars;
     for(int i=0;i<length;i++){
      try{
       CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
       DPS::DBServer_var _pvar=DPS::DBServer::_narrow(obj);
          return _pvar->getNHS(cid,acs);
       }
       catch(DPS::DBServer::DBProblem &dbl){
       _parent->EMessage((const char*)dbl.message); 
       }
       catch (CORBA::SystemException &ex){
        // Have to Kill Servers Here
        _parent->EMessage("dbserver corba error during getnhs" );   
       }
     }
 }



NHS_var acv= new NHS();
int length=0;
for(AMSServerI * pser=this;pser;pser= pser->next()?pser->next():pser->down()){
if(pser->getType()==cid.Type){
length=pser->getnhl().size();
if(length==0){
acv->length(1);
}
else{
acv->length(length);
length=0;
for(NHLI li=pser->getnhl().begin();li!=pser->getnhl().end();++li){
 acv[length++]=*li;
}
}
acs=acv._retn();
return length;
}
}


}

int Server_impl::getAHS(const DPS::Client::CID &cid, AHS_out acs)throw (CORBA::SystemException){



 if(_parent->DBServerExists()){
        Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
     DPS::Client::CID pid=_parent->getcid();
     pid.Type=DPS::Client::DBServer;
     pid.Interface= (const char *) " ";
     DPS::Client::ARS * pars;
     int length=_pser->getARS(pid,pars,DPS::Client::Any,0,1);
     DPS::Client::ARS_var arf=pars;
     for(int i=0;i<length;i++){
      try{
       CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
       DPS::DBServer_var _pvar=DPS::DBServer::_narrow(obj);
          return _pvar->getAHS(cid,acs);
       }
       catch(DPS::DBServer::DBProblem &dbl){
        _parent->EMessage((const char*)dbl.message); 
       }
       catch (CORBA::SystemException &ex){
        // Have to Kill Servers Here
        _parent->EMessage("dbserver corba error during getahs" );   
       }

     }
 }


AHS_var acv= new AHS();
int length=0;

for(AMSServerI * pser=this;pser;pser= pser->next()?pser->next():pser->down()){
if(pser->getType()==cid.Type){
length=pser->getahl().size();
if(length==0){
acv->length(1);
}
else{
acv->length(length);
length=0;
for(AHLI li=pser->getahl().begin();li!=pser->getahl().end();++li){
 acv[length++]=*li;
}
}
acs=acv._retn();
return length;
}
}


}


  void Server_impl::ping()throw (CORBA::SystemException){
}


   void Server_impl::sendCriticalOps(const DPS::Client::CID & cid, const CriticalOps & op)throw (CORBA::SystemException){
  for (AMSServerI* pcur=this;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
    if(pcur->getType()==op.Type){
     switch (op.Action){
      case StartClient: KillClient: CheckClient:
        CriticalOps op1(op);
        if(op.Action==StartClient)op1.Action=ClearStartClient;        
        if(op.Action==KillClient)op1.Action=ClearKillClient;        
        if(op.Action==CheckClient)op1.Action=ClearCheckClient;        
        pcur->getcol().push_back(op1);
        
//        cout << "set lock "<<cid.uid<<" "<<op.Type<<endl;
       break;
      case ClearStartClient: ClearKillClient: ClearCheckClient:
        for (COLI li=pcur->getcol().begin(); li!=pcur->getcol().end();++li){
         if(op.Action == (*li).Action){
          pcur->getcol().erase(li);
//         cout << "remove lock "<<cid.uid<<" "<<op.Type<<endl;
          break;
         }
        }
        break;
     }     
     break;
    }
  }
}

  bool Server_impl::pingHost(const char * host){
   char tmpbuf[1024];
   strcpy(tmpbuf,"ping -c 1 ");
   strcat(tmpbuf,host);
   return system(tmpbuf)==0;


}



CORBA::Boolean Server_impl::TypeExists(DPS::Client::ClientType type)throw (CORBA::SystemException){
for(AMSServerI * pcur=this;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
 if(pcur->getType()==type)return true;
}
return false;
}



void Server_impl::StartSelf(const DPS::Client::CID & cid, DPS::Client::RecordChange rc){
 // Registered itself in _acl
     DPS::Client::ActiveClient as;
     as.id= cid;
     as.Status=DPS::Client::Active;
     time_t tt;
     time(&tt);
     as.LastUpdate=tt;     
     as.Start=tt;
    int length=0;
    for (AMSServerI * pser=this;pser; pser=pser->down()?pser->down():pser->next())length+=pser->getrefmap().size();
//       cout <<"  length "<<length <<endl;
    if(length){
     (as.ars).length(length);
    length=0;
     for (AMSServerI * pser=this;pser; pser=pser->down()?pser->down():pser->next()){
      for(  map<AString,CORBA::String_var>::iterator mi=pser->getrefmap().begin();mi!=pser->getrefmap().end();++mi){
        ((as.ars)[length]).Interface=(const char *)(mi->first);
        ((as.ars)[length]).IOR=(const char *)(mi->second);
        ((as.ars)[length]).Type=pser->getType();
        ((as.ars)[length]).uid=as.id.uid;
        length++;
       }
     }
    }
     else {
      (as.ars).length(1);
      ((as.ars)[0]).Interface=(const char *)("Dummy");
      ((as.ars)[0]).IOR=(const char *)(" ");
      ((as.ars)[0]).Type=DPS::Client::Generic;
      ((as.ars)[0]).uid=0;
     }
   DPS::Client::ActiveClient_var acv=new DPS::Client::ActiveClient(as);
   PropagateAC(acv,rc);
   DPS::Client::CID asid=as.id;
    sendAC(asid,as,rc);
}


//------------------------------Producer_impl--------------------------------


Producer_impl::Producer_impl(const map<AString, AMSServer::OrbitVars> & mo, const DPS::Client::CID & cid,AMSClient* parent,char * NC, char *RF, char *NS, char *TS): POA_DPS::Producer(),AMSServerI(AMSID("Producer",0),parent,DPS::Client::Producer),_RunID(0){
typedef map<AString, AMSServer::OrbitVars>::const_iterator MOI;
Producer_impl * pcur =0;
for(MOI i=mo.begin();i!=mo.end();++i){
   if(!pcur)pcur=this;
//   else add(pcur = new Producer_impl());
 PortableServer::ObjectId_var oid=(i->second)._poa->activate_object(pcur);
  DPS::Producer_var _ref = reinterpret_cast<DPS::Producer_ptr>((i->second)._poa->id_to_reference(oid));
   _refmap[i->first]=((i->second)._orb)->object_to_string(_ref);
   if(!strcmp((const char *)(i->first),(const char*)cid.Interface)){
    _defaultorb=(i->second)._orb;
   }
}
// Here read nominalclients
   
if(NC){
 ifstream fbin;
 fbin.open(NC);
 unsigned int uid=0;
 if(fbin){
   DPS::Client::NominalClient_var ncl= new DPS::Client::NominalClient();
   if(fbin.get()=='#')fbin.ignore(1024,'\n');
   else fbin.seekg(fbin.tellg()-sizeof(char));
   fbin>>ncl->MaxClients>>ncl->CPUNeeded>>ncl->MemoryNeeded;
    char tmpbuf[1024];
    fbin>>tmpbuf;
    ncl->WholeScriptPath=(const char*)tmpbuf;
    ncl->Type=DPS::Client::Producer;
   fbin.ignore(1024,'\n');
  while(!fbin.eof() && fbin.good()){ 
   fbin>>tmpbuf;
   ncl->HostName=(const char*)tmpbuf;
   fbin>>ncl->LogInTheEnd;
   fbin.ignore(1024,'\n');
   fbin.getline(tmpbuf,1024);
   ncl->LogPath= (const char*)tmpbuf;
   fbin.getline(tmpbuf,1024);
    ncl->SubmitCommand=(const char*)tmpbuf;
    ncl->uid=++uid;
    if(fbin.good())_ncl.push_back(ncl);
  }
 }
 else{
 _parent->FMessage("Producer_impl::Producer_impl-F-UnableToOpenNCFile ",DPS::Client::CInAbort);
}
}
else{
 _parent->FMessage("Producer_impl::Producer_impl-F-UnableToFindNCFile ",DPS::Client::CInAbort);
}

if(NS){
 ifstream fbin;
 fbin.open(NS);
 if(fbin){
   unsigned int uid=0;
   DSTInfo_var ncl= new DSTInfo();
    char tmpbuf[1024];
  while(!fbin.eof() && fbin.good()){ 
   if(fbin.get()=='#'){
      fbin.ignore(1024,'\n');
      continue;
   }
   else fbin.seekg(fbin.tellg()-sizeof(char));
   fbin>>tmpbuf;
   ncl->HostName=(const char*)tmpbuf;
   ncl->uid=++uid;
   ncl->FreeSpace=-1;
   ncl->TotalSpace=-1;
   int imode;
   fbin>>imode;
   switch (imode){
     case 0:
     ncl->Mode=RILO;
     break;
     case 1:
     ncl->Mode=LILO;
     break;
     case 2:
     ncl->Mode=RIRO;
     break;
     case 3:
     ncl->Mode=LIRO;
     break;
     default:
     ncl->Mode=RILO;
     break;
   }
   fbin>>ncl->UpdateFreq;
   fbin>>tmpbuf;
    ncl->OutputDirPath=(const char*)tmpbuf;
    ncl->DieHard=0;
    
    if(fbin.good())_dstinfo.push_back(ncl);
  }
 }
 else{
 _parent->FMessage("Producer_impl::Producer_impl-F-UnableToOpenNtupeFile ",DPS::Client::CInAbort);
}
}
else{
 _parent->FMessage("Producer_impl::Producer_impl-F-UnableToFindNtupeFile ",DPS::Client::CInAbort);
}

//Here read runfiletable
if(RF){
 ifstream fbin;
 fbin.open(RF);
 if(fbin){
   int cur=0;
  while(!fbin.eof() && fbin.good()){ 
   if(fbin.get()=='#'){
      fbin.ignore(1024,'\n');
      continue;
   }
   else fbin.seekg(fbin.tellg()-sizeof(char));
   DPS::Producer::RunEvInfo  re;
   re.uid=_RunID++;
    char tmpbuf[1024];
   fbin>>re.Run>>re.FirstEvent>>re.LastEvent>>re.TFEvent>>re.TLEvent>>re.Priority>>tmpbuf;
   re.FilePath=(const char*)tmpbuf;
   if(re.LastEvent<re.FirstEvent)re.LastEvent=2000000000;
   re.Status=DPS::Producer::ToBeRerun;
   re.History=DPS::Producer::ToBeRerun;
   time_t tt;
   time(&tt);
   re.SubmitTime=tt; 
   re.cinfo.Run=re.Run;
   re.cinfo.EventsProcessed=0;
   re.cinfo.LastEventProcessed=0;
   re.cinfo.ErrorsFound=0;
   re.cinfo.CPUTimeSpent=0;
   re.cinfo.Status=re.Status;
   re.cinfo.HostName=" ";
   RunEvInfo_var vre= new RunEvInfo(re);
   if(fbin.good())_rl.push_back(vre); 
//    cout <<++cur<<" "<<re.Run<<endl;
  }
 }
else{
  cerr<<"Producer_impl::Producer_impl-F-UnableToOpenRunFile "<<RF<<endl;
  _parent->FMessage("Producer_impl::Producer_impl-F-UnableToOpenRunFile ",DPS::Client::CInAbort);
}
 cout <<"RQsize "<<_rl.size()<<endl;
 _rl.sort(Less());
}
}



Producer_impl::Producer_impl(const map<AString, AMSServer::OrbitVars> & mo, DPS::Server_ptr _svar, DPS::Client::CID  cid,AMSClient* parent): POA_DPS::Producer(),AMSServerI(AMSID("Producer",0),parent,DPS::Client::Producer),_RunID(0){

typedef map<AString, AMSServer::OrbitVars>::const_iterator MOI;
Producer_impl * pcur =0;
for(MOI i=mo.begin();i!=mo.end();++i){
   if(!pcur)pcur=this;
//   else add(pcur = new Producer_impl());
 PortableServer::ObjectId_var oid=(i->second)._poa->activate_object(pcur);
  DPS::Producer_var _ref = reinterpret_cast<DPS::Producer_ptr>((i->second)._poa->id_to_reference(oid));
   _refmap[i->first]=((i->second)._orb)->object_to_string(_ref);
   if(!strcmp((const char *)(i->first),(const char*)cid.Interface)){
    _defaultorb=(i->second)._orb;
   }
}

//Here Read NominalClient

cid.Type=getType();

DPS::Client::NCS * pncs;
int length=_svar->getNC(cid,pncs);
NCS_var ncs=pncs;
for(int i=0;i<length;i++){
 DPS::Client::NominalClient_var vnh= new DPS::Client::NominalClient(ncs[i]);
 _ncl.push_back(vnh);
}



//Now Read ActiveHost

DPS::Client::AHS * pahs;
 length=_svar->getAHS(cid,pahs);
AHS_var ahs=pahs;
for(int i=0;i<length;i++){
 DPS::Client::ActiveHost_var vah= new DPS::Client::ActiveHost(ahs[i]);
 _parent->IMessage(AMSClient::print(vah, "getAHSP: ")); 
 _ahl.push_back(vah);
}

//Now Read ActiveClient

DPS::Client::ACS * pacs;
length=_svar->getACS(cid,pacs,_Submit);
ACS_var acs=pacs;
for(int i=0;i<length;i++){
 DPS::Client::ActiveClient_var vac= new DPS::Client::ActiveClient(acs[i]);
 _acl.push_back(vac);
}

 cout <<"ACL size pr"<<_acl.size()<<" "<<(*_ncl.begin())->MaxClients<<" "<<_Submit<<" "<<endl;

}




void Producer_impl::_init(){
 Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
 if(!_pser){
  _parent->FMessage("Producer_impl::_init-F-UnableToConnectToServer",DPS::Client::CInAbort);
 }
if(_ahl.size()){
//Get producer_var and read producer specifics
    DPS::Client::CID cid=_parent->getcid();
    cid.Type=getType();
    cid.Interface= (const char *) " ";
    DPS::Client::ARS * pars;
    int length=_pser->getARS(cid,pars);
    DPS::Client::ARS_var arf=pars;
    DPS::Producer_var _pvar=DPS::Producer::_nil();
  for(int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    _pvar=DPS::Producer::_narrow(obj);
     if(!CORBA::is_nil(_pvar))break;
   }
   catch (CORBA::SystemException &ex){
   }
  } 
   if(CORBA::is_nil(_pvar))_parent->FMessage("Producer_impl::ctor-UnableToGetpvar",DPS::Client::CInAbort);  

//Read dstinfo

//Here read runfiletable
DSTIS * pdstis;
length=_pvar->getDSTInfoS(cid, pdstis);
DSTIS_var dstis=pdstis; 

for(int i=0;i<length;i++){
DSTInfo_var vre= new DSTInfo(dstis[i]);
 _dstinfo.push_back(vre);
}
 cout <<"DSTinfosize "<<_dstinfo.size()<<endl;



//Here read runfiletable
RES * pres;
length=_pvar->getRunEvInfoS(cid, pres,_RunID);
RES_var res=pres; 

for(int i=0;i<length;i++){
RunEvInfo_var vre= new RunEvInfo(res[i]);
 _rl.push_back(vre);
}
 cout <<"RQsize "<<_rl.size()<<endl;



//Now Read Ntuple

DSTS * pdsts;
length=_pvar->getDSTS(cid,pdsts);
DSTS_var dsts=pdsts;
for(int i=0;i<length;i++){
DST_var vdst= new DST(dsts[i]);
 _dst.insert(make_pair(vdst->Type,vdst));
}

}

else{
// here init active host list
  for(NHLI i=(_pser->getnhl()).begin();i!=(_pser->getnhl()).end();++i){
   DPS::Client::ActiveHost ah;
   ah.HostName=CORBA::string_dup((*i)->HostName);
   ah.Interface=CORBA::string_dup((*i)->Interface);
   if(_pser->pingHost((const char*)(ah.HostName)))ah.Status=DPS::Client::OK;
   else ah.Status=DPS::Client::NoResponse;
   ah.ClientsProcessed=0;
   ah.ClientsFailed=0;
   ah.ClientsKilled=0;
   ah.ClientsRunning=0;
   ah.Clock=(*i)->Clock;  
   ah.ClientsAllowed=min((*i)->CPUNumber/(*_ncl.begin())->CPUNeeded,(*i)->Memory/float((*_ncl.begin())->MemoryNeeded));

   time_t tt;
   time(&tt);
   ah.LastUpdate=tt;
//Check Interface
   if(_refmap.find((const char *)(ah.Interface))==_refmap.end()){
   ah.Interface=(const char *)((_refmap.begin())->first);
#ifdef __AMSDEBUG__
       if((_refmap.begin())->first != "default"){
        cerr<<"Producer_impl::_init-S-FirstRefmapEntry!=default "<<(_refmap.begin())->first<<endl;
   }
#endif
   }
   DPS::Client::ActiveHost_var vah= new DPS::Client::ActiveHost(ah);
   _ahl.push_back(vah);
  }

}
}





void Producer_impl::StartClients(const DPS::Client::CID & pid){
if(!Master())return;
RegisteredClientExists();
for(AMSServerI * pcur=getServer(); pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
if(pcur->InactiveClientExists(getType()))return;
 
}

  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
 // Check if there are some hosts to run on
 DPS::Client::ActiveHost_var ahlv=new DPS::Client::ActiveHost();
 bool suc=false;
 if(_parent->DBServerExists() && getHostInfoSDB(_parent->getcid(),ahlv)){
  suc=true;
 }
 else{
 _ahl.sort(Less());
  if(_acl.size()<(*_ncl.begin())->MaxClients && _acl.size()<count_if(_rl.begin(),_rl.end(),REInfo_Count())){
 for(AHLI i=_ahl.begin();i!=_ahl.end();++i){
  if((*i)->Status!=NoResponse && (*i)->Status!=InActive){
    if((*i)->ClientsRunning>=(*i)->ClientsAllowed)continue;
     ahlv=*i;
     suc=true;
     break;
    }
   }
  }
 }
  if(suc){ 
  if(!_pser->Lock(pid,DPS::Server::StartClient,getType(),_StartTimeOut))return;
   // HereStartClient
   CORBA::String_var _refstring=_refmap.find((const char *)((ahlv)->Interface))->second;
#ifdef __AMSDEBUG__
//    cout <<((ahlv)->Interface)<<" "<<_refstring<<endl;
#endif
    NCLI cli=find_if(_ncl.begin(),_ncl.end(),NCL_find((const char *)(ahlv)->HostName)); 
    if(cli==_ncl.end())cli=_ncl.begin();
    AString submit;
    char tmp[80];
     sprintf(tmp,"%d",_Submit+1);
    submit+=(const char *)(*cli)->SubmitCommand;  
    submit+=" ";
    submit+=(const char*)((ahlv)->HostName);
    submit+=" ";
    if(!(*cli)->LogInTheEnd){
     submit+=(const char*)((*cli)->LogPath);  
     submit+="Producer.";
     submit+=tmp;
     submit+=".log ";
    }
    submit+=(const char*)((*cli)->WholeScriptPath);  
    submit+=" -";
    submit+=(const char*) _refstring;
    submit+=" -U";
    submit+=tmp;
    if(_parent->Debug())submit+=" -D1";
    submit+=" -A";
     submit+=getenv("AMSDataDir");
    if((*cli)->LogInTheEnd){
     submit+=" ";
     submit+=(const char*)((*cli)->LogPath);  
     submit+="Producer.";
     submit+=tmp;
     submit+=".log ";
    }
#ifdef __AMSDEBUG__
    cout <<submit<<endl;
#endif
    submit+=" &";
    int out=system(submit);
     time_t tt;
     time(&tt);
     (ahlv)->LastUpdate=tt;     
    if(out==0){
     // Add New Active Client
     DPS::Client::ActiveClient ac;
     (ac.id).HostName=CORBA::string_dup((ahlv)->HostName);
     ac.id.Interface=(ahlv)->Interface;
     (ac.id).uid=_Submit+1;
     ac.id.pid=0;
     ac.id.ppid=0;
     ac.id.Type=getType();
     ac.id.Status=DPS::Client::NOP;
     ac.Status=DPS::Client::Submitted;
     time_t tt;
     time(&tt);
     ac.LastUpdate=tt;     
     ac.Start=tt;
     (ac.ars).length(1);
     ((ac.ars)[0]).Interface=(const char *)("Dummy");
     ((ac.ars)[0]).IOR=(const char *)(" ");
     ((ac.ars)[0]).Type=DPS::Client::Generic;
     ((ac.ars)[0]).uid=0;
    (ahlv)->Status=DPS::Client::OK; 
     DPS::Client::CID cid=_parent->getcid();      
      cid.Type=getType();
      cid.Interface= (const char *) " "; 
    PropagateAH(cid,(ahlv),DPS::Client::Update);

    DPS::Client::ActiveClient_var acv=new DPS::Client::ActiveClient(ac);
    PropagateAC(acv,DPS::Client::Create);
//     ((ahlv)->ClientsRunning)++;
    }
    else{
     (ahlv)->Status=DPS::Client::LastClientFailed; 
     ((ahlv)->ClientsFailed)++; 
     DPS::Client::CID cid=_parent->getcid();      
      cid.Type=getType();
      cid.Interface= (const char *) " "; 
    PropagateAH(cid,(ahlv),DPS::Client::Update);
     AString a="StartClients-E-UnableToStartClient ";
     a+=submit;
     _parent->EMessage((const char *)a);
      Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
      _pser->MonInfo((const char *) a,DPS::Client::Error);
    }
    _pser->Lock(pid,DPS::Server::ClearStartClient,getType(),_StartTimeOut);  
   }

}



#include <signal.h>
void Producer_impl::KillClients(const DPS::Client::CID & pid){
if(!Master())return;


Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
if(!_pser->Lock(pid,DPS::Server::KillClient,getType(),_KillTimeOut))return;



ACLI li=find_if(_acl.begin(),_acl.end(),find(DPS::Client::Killed));
if(li!=_acl.end()){
 //kill by -9 here
 
   if(_parent->Debug())_parent->EMessage(AMSClient::print(*li, " KILL -9"));
    int iret=_pser->Kill((*li),SIGKILL,true);
    if(iret){
     _parent->EMessage(AMSClient::print(*li,"Producer::Unable To Kill Client"));
    }

    DPS::Client::ActiveClient_var acv=*li;
    PropagateAC(acv,DPS::Client::Delete);
}
 li=find_if(_acl.begin(),_acl.end(),find(DPS::Client::TimeOut));
if(li!=_acl.end()){
 
 if(!_pser->pingHost((const char*)((*li)->id.HostName))){
    for(AHLI i=_ahl.begin();i!=_ahl.end();++i){
      if(!strcmp((const char *)(*i)->HostName, (const char *)((*li)->id).HostName)){
       (*i)->Status=NoResponse;
       _pser->MonInfo(AMSClient::print(*i,"No Response From: "),DPS::Client::Warning);
    }
}  

   (*li)->id.Status=DPS::Client::SInKill;
   DPS::Client::ActiveClient_var acv=*li;
   PropagateAC(acv,DPS::Client::Delete);
 }
 else {
   if(_pser->MonDialog(AMSClient::print(*li,"Asking To Kill Client: "),DPS::Client::Error)){
   (*li)->id.Status=DPS::Client::SInKill;
   (*li)->Status=DPS::Client::Killed;
   DPS::Client::ActiveClient_var acv=*li;
   PropagateAC(acv,DPS::Client::Update);
    _pser->Kill((*li),SIGHUP,true);
  }
  else{
    _UpdateACT((*li)->id,DPS::Client::Active);
  }
 }
}

_pser->Lock(pid,DPS::Server::ClearKillClient,getType(),_KillTimeOut);
}





void Producer_impl::CheckClients(const DPS::Client::CID & cid){
if(!Master())return;
Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
if(!_pser->Lock(cid,DPS::Server::CheckClient,getType(),_KillTimeOut))return;
for(AHLI li=_ahl.begin();li!=_ahl.end();++li){
 if((*li)->Status==NoResponse && double(rand())/RAND_MAX>0.98)if(_pser->pingHost((const char*)((*li)->HostName)))(*li)->Status=DPS::Client::OK;
}
time_t tt;
time(&tt);
for(ACLI li=_acl.begin();li!=_acl.end();++li){
 // find clients with timeout
 if((*li)->Status!=DPS::Client::Killed && (*li)->LastUpdate+_KillTimeOut<tt){
   DPS::Client::ActiveClient_var acv=*li;
   acv->Status=DPS::Client::TimeOut;
   if(_parent->Debug())_parent->EMessage(AMSClient::print(acv,"Client TIMEOUT"));
   PropagateAC(acv,DPS::Client::Update);
 }
}

_pser->Lock(cid,DPS::Server::ClearCheckClient,getType(),_KillTimeOut);
}









CORBA::Boolean Producer_impl::sendId(DPS::Client::CID & cid, uinteger timeout) throw (CORBA::SystemException){
 if(timeout>_KillTimeOut)_KillTimeOut=timeout;
     for(ACLI j=_acl.begin();j!=_acl.end();++j){
      if(((*j)->id).uid==cid.uid){
       ((*j)->id).pid=cid.pid;
       ((*j)->id).ppid=cid.ppid;
       cid.Interface=CORBA::string_dup(((*j)->id).Interface);
       cid.Type=((*j)->id).Type;
       (*j)->Status=DPS::Client::Registered;
       time_t tt;
       time(&tt);
       (*j)->LastUpdate=tt;
#ifdef __AMSDEBUG__
       _parent->IMessage(AMSClient::print(cid,"Producer_impl::sendId-I-RegClient "));
#endif 
       DPS::Client::ActiveClient_var acv=*j;
       PropagateAC(acv,DPS::Client::Update);
//       return false;
       return true;
      }
     }
       _parent->EMessage(AMSClient::print(cid,"Producer_impl::sendId-E-RegClientNotFound "));
     return false;


}


int Producer_impl::getARS(const DPS::Client::CID & cid, DPS::Client::ARS_out arf, DPS::Client::AccessType type, uinteger id, int selffirst)throw (CORBA::SystemException){

_UpdateACT(cid,DPS::Client::Active);


 Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 

return _pser->getARS(cid, arf,type,id,selffirst);

}





void Producer_impl::Exiting(const CID & cid, const char * message, DPS::Client::ClientExiting status)throw (CORBA::SystemException){
_parent->IMessage(AMSClient::print(cid,message?message:" Producer Exiting"));
Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
_pser->MonInfo(AMSClient::print(cid,message?message:" Producer Exiting "),DPS::Client::Info);
// find and remove client
for( ACLI li=_acl.begin();li!=_acl.end();++li){
 if (cid.uid==((*li)->id).uid){
   (*li)->id.Status=status;
   DPS::Client::ActiveClient_var acv=*li;
   PropagateAC(acv,DPS::Client::Delete);
   RLI li=find_if(_rl.begin(),_rl.end(),REInfo_EqsClient(cid));
    if(li!=_rl.end()){
     if((*li)->Status == Processing){
       RunEvInfo_var rv=*li;
       rv->cinfo.HostName=cid.HostName;
      if(rv->History !=Failed){
        rv->Status=ToBeRerun;
        rv->History=Failed;
      }
      else      rv->Status=Failed;
       rv->cuid=0;
       _parent->EMessage(AMSClient::print(rv, " run Failed "));
       Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
       _pser->MonInfo(AMSClient::print(rv, " Run Failed: "),DPS::Client::Error);
       PropagateRun(rv,DPS::Client::Update);
     }
    }

   return;
 }

}
 _parent->EMessage(AMSClient::print(cid,"Producer_impl::Exiting::No Such Client"));
}





int Producer_impl::getTDV(const DPS::Client::CID & cid,  TDVName & tdvname, TDVbody_out body)throw (CORBA::SystemException){
_UpdateACT(cid,DPS::Client::Active);

 if(_parent->IsOracle()){
     Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
     DPS::Client::CID pid=_parent->getcid();
     pid.Type=DPS::Client::DBServer;
     pid.Interface= (const char *) " ";
     DPS::Client::ARS * pars;
     int length=_pser->getARS(pid,pars,DPS::Client::Any,0,1);
     DPS::Client::ARS_var arf=pars;
     for(int i=0;i<length;i++){
      try{
       CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
       DPS::DBServer_var _pvar=DPS::DBServer::_narrow(obj);
          return _pvar->getTDV(cid,tdvname,body);
       }
       catch (CORBA::SystemException &ex){
        // Have to Kill Servers Here
       }
     }
     TDVbody_var vbody=new TDVbody();
     tdvname.Success=false;
     vbody->length(1);
     body=vbody._retn();
     return 0;


 }
 else{
 int length=0;
 TIDI li=_findTDV(tdvname);
 tdvname.Success=false;
 if(li!=_tid.end()){
  time_t b=tdvname.Entry.Begin;
  tdvname.Success=li->second->read((const char*)AMSDBc::amsdatabase,tdvname.Entry.id,b);
 }
 time_t i,b,e;
 li->second->gettime(i,b,e);
  tdvname.Entry.Insert=i;
  tdvname.Entry.Begin=b;
  tdvname.Entry.End=e;
  TDVbody_var vbody=new TDVbody();
  if(tdvname.Success){
   length=li->second->GetNbytes()/sizeof(uinteger);
   vbody->length(length);
   li->second->CopyOut(vbody->get_buffer());
  }
  else{
   vbody->length(1);
  }
  body=vbody._retn();
  return length;
 }
}

int Producer_impl::getSplitTDV(const DPS::Client::CID & cid,  unsigned int & pos,TDVName & tdvname, TDVbody_out body, TransferStatus & st)throw (CORBA::SystemException){
_UpdateACT(cid,DPS::Client::Active);


 if(_parent->IsOracle()){
     Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
     DPS::Client::CID pid=_parent->getcid();
     pid.Type=DPS::Client::DBServer;
     pid.Interface= (const char *) " ";
     DPS::Client::ARS * pars;
     int length=_pser->getARS(pid,pars,DPS::Client::Any,0,1);
     DPS::Client::ARS_var arf=pars;
     for(int i=0;i<length;i++){
      try{
       CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
       DPS::DBServer_var _pvar=DPS::DBServer::_narrow(obj);
          return _pvar->getSplitTDV(cid,pos,tdvname,body,st);
       }
       catch (CORBA::SystemException &ex){
        // Have to Kill Servers Here
       }
     }
     TDVbody_var vbody=new TDVbody();
     tdvname.Success=false;
     vbody->length(1);
     body=vbody._retn();
     return 0;


 }
 else{




st=Continue;
int length=0;
TIDI li=_findTDV(tdvname);
tdvname.Success=false;
if(li!=_tid.end()){
 time_t b=tdvname.Entry.Begin;
 tdvname.Success=li->second->read((const char*)AMSDBc::amsdatabase,tdvname.Entry.id,b);
}
time_t i,b,e;
li->second->gettime(i,b,e);
 tdvname.Entry.Insert=i;
 tdvname.Entry.Begin=b;
 tdvname.Entry.End=e;
 TDVbody_var vbody=new TDVbody();
 if(tdvname.Success){
  length=li->second->GetNbytes()/sizeof(uinteger);
  const int maxs=400000;
  vbody->length(length);
  li->second->CopyOut(vbody->get_buffer());
  length-=pos;
  if(length>maxs)length=maxs;
  else st=End;
   if(pos){
    for (uinteger i=0;i<length;i++){
      vbody[i]=vbody[i+pos];
    }        
   }
   vbody->length(length);
   pos+=length;
 }
 else{
  vbody->length(1);
 }
 body=vbody._retn();
 return length;
}
}


void Producer_impl::sendTDV(const DPS::Client::CID & cid, const TDVbody & tdv, TDVName & tdvname )throw (CORBA::SystemException){
_UpdateACT(cid,DPS::Client::Active);



 if(_parent->IsOracle()){
     Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
     DPS::Client::CID pid=_parent->getcid();
     pid.Type=DPS::Client::DBServer;
     pid.Interface= (const char *) " ";
     DPS::Client::ARS * pars;
     int length=_pser->getARS(pid,pars,DPS::Client::Any,0,1);
     DPS::Client::ARS_var arf=pars;
     for(int i=0;i<length;i++){
      try{
       CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
       DPS::DBServer_var _pvar=DPS::DBServer::_narrow(obj);
         _pvar->sendTDV(cid,tdv,tdvname);
         return;
       }
       catch (CORBA::SystemException &ex){
        // Have to Kill Servers Here
       }
     }


 }
 else{



TIDI li=_findTDV(tdvname);
tdvname.Success=false;
if(li!=_tid.end()){
time_t i,b,e;
i=tdvname.Entry.Insert;
b=tdvname.Entry.Begin;
e=tdvname.Entry.End;
li->second->SetTime(i,b,e);
li->second->CopyIn(tdv.get_buffer()); 
 tdvname.Success=li->second->write((const char*)AMSDBc::amsdatabase,1);

  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
  DPS::Client::CID pid;
  pid.Type=getType();
  pid.Interface= (const char *) " ";
    DPS::Client::ARS * pars;
    int length=_pser->getARS(pid,pars,DPS::Client::Any,0,1);
    DPS::Client::ARS_var arf=pars;
  for(int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Producer_var _pvar=DPS::Producer::_narrow(obj);
    _pvar->sendTDVUpdate(cid,tdvname);
   }
   catch (CORBA::SystemException &ex){
     // Have to Kill Servers Here
   }
  }



}
}
}

void Producer_impl::sendTDVUpdate(const DPS::Client::CID & cid,  const TDVName & tdvname )throw (CORBA::SystemException){
TIDI li=_findTDV(tdvname);
if(li!=_tid.end()){
time_t i,b,e;
i=tdvname.Entry.Insert;
b=tdvname.Entry.Begin;
e=tdvname.Entry.End;
li->second->SetTime(i,b,e);
try{
li->second->updatedb();
}
catch (bad_alloc aba){
 _parent->EMessage("sendTDVUpdate::NoMemoryAvailable");
}
}
}

  
int Producer_impl::getTDVTable(const DPS::Client::CID & cid, TDVName & tdvname, unsigned int id, TDVTable_out table)throw (CORBA::SystemException){
_UpdateACT(cid,DPS::Client::Active);

TIDI li=_findTDV(tdvname);
tdvname.Success=false;
int length=0;
TDVTable_var vtable=new TDVTable();
if(li!=_tid.end()){
  RLI ri=find_if(_rl.begin(),_rl.end(),REInfo_Eqs(id));
  if(ri!=_rl.end()){
    tdvname.Success=true;
    AMSTimeID::IBE ibe=li->second->findsubtable((*ri)->TFEvent,(*ri)->TLEvent);
    vtable->length(ibe.size());
    for( AMSTimeID::IBEI ti=ibe.begin();ti!=ibe.end();++ti){
      vtable[length].id=ti->id;
      vtable[length].Insert=ti->insert;
      vtable[length].Begin=ti->begin;
      vtable[length].End=ti->end;
      length++; 
    }
  }
}
if(length==0){
 vtable->length(1);
}
table= vtable._retn();
return length;
}

#include <new.h>
Producer_impl::TIDI & Producer_impl::_findTDV(const TDVName & tdv){
AMSID id((const char*)tdv.Name,tdv.DataMC);
TIDI li=_tid.find(id);
if(li==_tid.end()){
try{
 uinteger *pdata = new uinteger[tdv.Size/sizeof(uinteger)-1];
 if(pdata){
  time_t b=tdv.Entry.Begin;
  time_t e=tdv.Entry.End;
  _tid[id]=new AMSTimeID(id,(*(localtime(&b))),(*(localtime(&e))),tdv.Size-sizeof(uinteger),pdata,AMSTimeID::Server);
 li=_tid.find(id);
 }
}
catch (bad_alloc aba){
 _parent->EMessage("_findTDV::NoMemoryAvailable");
}

}
return li;
}



void Producer_impl::getId(DPS::Client::CID_out  cid)throw (CORBA::SystemException){

  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
   _pser->getId(cid);
}
void Server_impl::getId(DPS::Client::CID_out  cid)throw (CORBA::SystemException){
DPS::Client::CID_var cvar=new DPS::Client::CID(_parent->getcid());
cid=cvar._retn();
}

 int Producer_impl::getRunEvInfoS(const DPS::Client::CID &cid, RES_out res, unsigned int & maxrun)throw (CORBA::SystemException){
 
RES_var acv= new RES();
int length=0;
maxrun=_RunID;
length=_rl.size();
if(length==0){
acv->length(1);
}
else{
acv->length(length);
length=0;
for(RLI li=_rl.begin();li!=_rl.end();++li){
 acv[length++]=*li;
}
}
res=acv._retn();
return length;
}

 int Producer_impl::getDSTInfoS(const DPS::Client::CID &cid, DSTIS_out res)throw (CORBA::SystemException){
 
DSTIS_var acv= new DSTIS();
int length=0;
length=_dstinfo.size();
if(length==0){
acv->length(1);
}
else{
acv->length(length);
length=0;
for(DSTILI li=_dstinfo.begin();li!=_dstinfo.end();++li){
 acv[length++]=*li;
}
}
res=acv._retn();
return length;
}




void Producer_impl::getRunEvInfo(const DPS::Client::CID &cid, RunEvInfo_out ro,DSTInfo_out dso)throw (CORBA::SystemException){



 DPS::Client::CID pid;
 pid.Interface=(const char *)" ";
 pid.Type=getType();
 DPS::Client::ARS * pars;      
 int length=getARS(pid,pars,DPS::Client::LessThan,_parent->getcid().uid);
 DPS::Client::ARS_var arf=pars; 
 for(int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Producer_var _pvar=DPS::Producer::_narrow(obj);
     cout <<" transferred to master "<<_parent->getcid().uid<<endl;
    _pvar->getRunEvInfo(cid,ro,dso);
    return;
  }
   catch (CORBA::TRANSIENT &tr){
    _parent->EMessage(AMSClient::print(cid,"Transient Error Occurs"));
    sleep(1);    
    try{
     CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
     DPS::Producer_var _pvar=DPS::Producer::_narrow(obj);
     _pvar->getRunEvInfo(cid,ro,dso);
    }
    catch (CORBA::SystemException &ex){
     _parent->EMessage(AMSClient::print(cid,"Transient Error Persists"));
     // Have to Kill Servers Here
    }
   }
    catch (CORBA::SystemException &ex){
     // Have to Kill Servers Here
   }
 }

 cout <<" Master getrunevinfo "<<_parent->getcid().uid<<endl;
 _UpdateACT(cid,DPS::Client::Active);


  DSTInfo_var dv =new DSTInfo();
  RunEvInfo_var rv=new RunEvInfo(); 

 if(_parent->DBServerExists()){

  bool succ=getRunEvInfoSDB(cid,rv,dv);
  if(! succ){
     _parent->EMessage(AMSClient::print(_parent->getcid(),"getRunEvInfoSDBFailed"));
   dv->DieHard=1;
   ro=rv._retn();
   dso=dv._retn();
   return;
 }
 
}
else{


{
 DSTILI li=find_if(_dstinfo.begin(),_dstinfo.end(),DSTInfo_find(cid));
 if(li==_dstinfo.end())li=_dstinfo.begin();
 dv= *li;
 }
_rl.sort(Less());
RLI li=find_if(_rl.begin(),_rl.end(),REInfo_find(cid,ToBeRerun));
if(li==_rl.end()){
 li=find_if(_rl.begin(),_rl.end(),REInfo_find(cid,Failed));
 if(li==_rl.end())dv->DieHard=1;
 else if(_parent->Debug()){
   cout <<  "****FAILED RUN RERUN "<<(*li)->Run<<" was "<<(*li)->cinfo.HostName <<" by "<<cid.HostName<<endl;
 }
}
if(li==_rl.end())dv->DieHard=1;
else if( find_if(_rl.begin(),_rl.end(),REInfo_EqsClient(cid))!=_rl.end()){
 dv->DieHard=1;
 if(_parent->Debug()){
  _parent->EMessage(AMSClient::print(cid,"Die HARD  !!!!!!!"));
  RLI rvi=find_if(_rl.begin(),_rl.end(),REInfo_EqsClient(cid));
  _parent->EMessage(AMSClient::print(*rvi,"Run/Client Logic Problem:"));
 }
}
else {
 rv=*li;
}
}
if(dv->DieHard !=1){
 rv->Status=Processing;
 rv->cuid=cid.uid;
 time_t tt;
 time(&tt);
 rv->SubmitTime=tt; 
  if(_parent->Debug()){
   _parent->IMessage(AMSClient::print (cid,"New Run Asked by"));  
   _parent->IMessage(AMSClient::print(rv));
  }
 PropagateRun(rv, DPS::Client::Update);
  uinteger smartfirst=getSmartFirst(rv->Run);
 if( smartfirst>rv->FirstEvent){
   rv->FirstEvent=smartfirst;
   if(smartfirst>rv->LastEvent){
   _parent->EMessage(AMSClient::print(rv,"***SMART PROBLEM***"));
    
   }
  }
 }
ro=rv._retn();
dso=dv._retn();
_UpdateACT(cid,DPS::Client::Active);

}



void Producer_impl::sendRunEvInfo(const  RunEvInfo & ne, DPS::Client::RecordChange rc)throw (CORBA::SystemException){

 RLI li=find_if(_rl.begin(),_rl.end(),REInfo_Eqs(ne));
 switch (rc){
 case DPS::Client::Update:
  if(li==_rl.end())_parent->EMessage(AMSClient::print(ne,"RunEv not found for editing"));
  else *li=new RunEvInfo(ne);
  break;
 case DPS::Client::Delete:
  if(li==_rl.end())_parent->EMessage(AMSClient::print(ne,"runEv not found for deleting"));
  else _rl.erase(li);
  break;
 case DPS::Client::Create:
  if(li != _rl.end()){
     _parent->EMessage(AMSClient::print(ne,"Run already exists"));
  }
  else {
   _RunID++;
   RunEvInfo_var rv=new RunEvInfo(ne);
  _rl.push_back(rv); 
  }
  break;
}
}

void Producer_impl::sendDSTInfo(const  DSTInfo & ne, DPS::Client::RecordChange rc)throw (CORBA::SystemException){
 PropagateDSTInfoDB(ne,rc);
 DSTILI li=find_if(_dstinfo.begin(),_dstinfo.end(),DSTInfo_Eqs(ne));
 switch (rc){
 case DPS::Client::Update:
  if(li==_dstinfo.end())_parent->EMessage(AMSClient::print(ne,"DSTInfo not found for editing"));
  else {
        DSTInfo_var vac= new DSTInfo(ne);
        replace_if(li,_dstinfo.end(),DSTInfo_Eqs(ne),vac);
  }
  break;
 case DPS::Client::Create:
       if(li!=_dstinfo.end())_parent->EMessage(AMSClient::print(ne,"DSTInfo already exists"));
       else{
        DSTInfo_var vac= new DSTInfo(ne);
         _dstinfo.push_back(vac);
       }
  break;
 case DPS::Client::Delete:
       if(li==_dstinfo.end())_parent->EMessage(AMSClient::print(ne,"DSTInfo not found for deleting"));
       else{
         _dstinfo.erase(li);
       }
  break;
 default:
        _parent->EMessage(AMSClient::print(ne,"sendDSTInfo-Unknown rc value"));

}
}


int Producer_impl::getDSTS(const DPS::Client::CID & ci, DSTS_out dsts)throw (CORBA::SystemException){

DSTS_var acv= new DSTS();
int length=0;
length=_dst.size();
if(length==0){
acv->length(1);
}
else{
acv->length(length);
length=0;
for(DSTLI li=_dst.begin();li!=_dst.end();++li){
 acv[length++]=(*li).second;
}
}
dsts=acv._retn();
return length;
}


void Producer_impl::sendCurrentInfo(const DPS::Client::CID & cid, const  CurrentInfo &ci, int propagate)throw (CORBA::SystemException){


RLI li=find_if(_rl.begin(),_rl.end(),REInfo_EqsClient(cid));
if(li !=_rl.end()){
   RunEvInfo_var rv=*li; 
   rv->cinfo=ci;
   rv->Status=ci.Status;
    if(ci.Status ==Finished || ci.Status==Failed){
/*
      if(_parent->Debug()){
       _parent->IMessage(AMSClient::print(cid,"sendCurrentInfo from "));
       _parent->IMessage(AMSClient::print(ci));
      }
*/
      rv->cuid=0;
      rv->cinfo.HostName=cid.HostName;
      if(rv->Status==Failed && rv->History !=Failed){
        rv->Status=ToBeRerun;
        rv->History=Failed;
      }
    }
   PropagateRun(rv,DPS::Client::Update);
   _UpdateACT(cid,DPS::Client::Active);
}
else{
 _parent->EMessage(AMSClient::print(cid,"sendCurrentInfo-EUnable to find Run Record for Client"));
 _parent->EMessage(AMSClient::print(ci));
}
}





void Producer_impl::sendDSTEnd(const DPS::Client::CID & ci, const  DST & ne, DPS::Client::RecordChange rc)throw (CORBA::SystemException){
_UpdateACT(ci,DPS::Client::Active);
if(_parent->Debug()){
  _parent->IMessage(AMSClient::print(ci,"senddstinfo get from "));
  _parent->IMessage(AMSClient::print(ne));
}
 DST_var vne= new DST(ne);
 pair<DSTLI,DSTLI>b=_dst.equal_range(ne.Type);
 switch (rc){
 case DPS::Client::Create:
 for(DSTLI li=b.first;li!=b.second;++li){
  if(!strcmp((const char *)(li->second)->Name,(const char *)ne.Name)){
   _parent->EMessage(AMSClient::print(li->second,"Create:DST Already exists"));
   return;
  }
 }
 _dst.insert(make_pair(ne.Type,vne));
  if(ci.Type!=DPS::Client::Server)PropagateDST(ne,DPS::Client::Create,DPS::Client::AnyButSelf,_parent->getcid().uid);
 break;
 case DPS::Client::Update:
 for(DSTLI li=b.first;li!=b.second;++li){
  if(!strcmp((const char *)(li->second)->Name,(const char *)ne.Name)){
   switch ((li->second)->Status){
    case InProgress:
     (li->second)=vne;
      if(ci.Type!=DPS::Client::Server)PropagateDST(ne,DPS::Client::Update,DPS::Client::AnyButSelf,_parent->getcid().uid);
      return;
    default:
    _parent->EMessage(AMSClient::print(vne,"Update:DST Already Exists "));
     (li->second)=vne;
      if(ci.Type!=DPS::Client::Server)PropagateDST(ne,DPS::Client::Update,DPS::Client::AnyButSelf,_parent->getcid().uid);
    return;
   }
   break;
  }
 }
    _parent->EMessage(AMSClient::print(vne,"Update:DST Not Found "));
 break;
 case DPS::Client::Delete:
 for(DSTLI li=b.first;li!=b.second;++li){
  if(!strcmp((const char *)(li->second)->Name,(const char *)ne.Name)){
      if(ci.Type!=DPS::Client::Server)PropagateDST((*li).second,rc,DPS::Client::AnyButSelf,_parent->getcid().uid);
     _dst.erase(li);
     return;       
      
  }
 } 
    _parent->EMessage(AMSClient::print(vne,"Delete:DST Not Found "));
 
}
}



void Producer_impl::PropagateRun(const RunEvInfo & ri, DPS::Client::RecordChange rc, DPS::Client::AccessType type, uinteger uid){

  PropagateRunDB(ri,rc);

  if(_ActivateQueue)_runqueue.push_back(RA(ri,rc));
  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 


  DPS::Client::CID cid;
  cid.Type=getType();
  cid.Interface= (const char *) " ";
    DPS::Client::ARS * pars;
    int length=_pser->getARS(cid,pars,type,uid);
    DPS::Client::ARS_var arf=pars;
  for(int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Producer_var _pvar=DPS::Producer::_narrow(obj);
    _pvar->sendRunEvInfo(ri,rc);
   }
   catch (CORBA::SystemException &ex){
     // Have to Kill Servers Here
   }
  } 

}

void Producer_impl::PropagateDST(const DST & ri, DPS::Client::RecordChange rc, DPS::Client::AccessType type,uinteger uid){

  PropagateDSTDB(ri,rc);

  if(_ActivateQueue)_dstqueue.push_back(DSTA(ri,rc));
  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
  DPS::Client::CID cid;
  cid.Type=getType();
  cid.Interface= (const char *) " ";
    DPS::Client::ARS * pars;
    int length=_pser->getARS(cid,pars,type,uid);
    DPS::Client::ARS_var arf=pars;
  for(int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Producer_var _pvar=DPS::Producer::_narrow(obj);
    _pvar->sendDSTEnd(_parent->getcid(),ri,rc);
   }
   catch (CORBA::SystemException &ex){
   }
  } 

}


void Server_impl::UpdateDB(bool force){
}

void Producer_impl::UpdateDB(bool force){
//just to do something




static bool resultdone=false;
if(!resultdone){
 if(force || !count_if(_rl.begin(),_rl.end(),REInfo_process())){
  if(!force)resultdone=true;

  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
  float dbfree,dbtotal;
  DPS::Server::DB* db;
  _pser->getDBSpace(_parent->getcid(),"AMSDataDir","/DataBase/",db);
  DPS::Server::DB_var dbv=db;
  cout <<" DBSpace Free "<<dbv->dbfree <<" Mb out of Total "<<dbv->dbtotal<<endl;
{
  for(DSTILI li= _dstinfo.begin();li!=_dstinfo.end();++li){
    _parent->IMessage(AMSClient::print((*li),"DiskSpace Info"));
  }
}

  for(RLI li= _rl.begin();li!=_rl.end();++li){
    if((*li)->Status==Finished)_parent->IMessage(AMSClient::print((*li),"Finished :"));
  }
  for(RLI li= _rl.begin();li!=_rl.end();++li){
   if((*li)->Status==Failed)_parent->IMessage(AMSClient::print((*li),"Failed :"));
  }
  for(RLI li= _rl.begin();li!=_rl.end();++li){
   if((*li)->Status==Processing)_parent->IMessage(AMSClient::print((*li),"Processing :"));
  }
cout << "Host Status **********"<<endl;
for (AHLI li=_ahl.begin();li!=_ahl.end();++li){
_parent->IMessage(AMSClient::print((*li),"Host "));
}

cout <<"DST status ******" <<endl;
{
 pair<DSTLI,DSTLI>b=_dst.equal_range(Ntuple);

for (DSTLI li=b.first;li!=b.second;++li){
_parent->IMessage(AMSClient::print((li->second),"DST "));
}
}
{
 pair<DSTLI,DSTLI>b=_dst.equal_range(RootFile);

for (DSTLI li=b.first;li!=b.second;++li){
_parent->IMessage(AMSClient::print((li->second),"DST "));
}
}
{
 pair<DSTLI,DSTLI>b=_dst.equal_range(EventTag);

for (DSTLI li=b.first;li!=b.second;++li){
_parent->IMessage(AMSClient::print((li->second),"DST "));
}
}
 }
//
}


}

bool Server_impl::Master(){

DPS::Client::CID cid;
cid.Interface=(const char *)" ";
cid.Type=getType();
DPS::Client::ARS * pars;
int length=getARS(cid,pars,DPS::Client::LessThan,_parent->getcid().uid);
DPS::Client::ARS_var arf=pars;
  time_t tt;
  time(&tt);
 for(int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Server_var _pvar=DPS::Server::_narrow(obj);
    if(!CORBA::is_nil(_pvar)){
    _pvar->ping();
     return false;
   }
  }
    catch (CORBA::SystemException &ex){
     cerr<<" Master oops corba exc for "<<i<<" "<<length<<" "<<ctime(&tt)<<endl;
   }
 }
return true;
}


bool Producer_impl::Master(){
 Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 

return _pser->Master();
}


integer Server_impl::Kill(const DPS::Client::ActiveClient & ac, int signal, bool self){
  //start  killer here
   if(ac.id.pid==0)return 0;
   AString submit;
    NCLI cli=find_if(_nki.begin(),_nki.end(),NCL_find((const char *)(ac.id.HostName))); 
    if(cli==_nki.end())cli=_nki.begin();
    char tmp[80];
     sprintf(tmp,"%d",ac.id.uid);
    submit+=(const char*)((*cli)->SubmitCommand);
    submit+=" ";
    submit+=(const char*)(ac.id.HostName);
    submit+=" ";
    if(!(*cli)->LogInTheEnd){
     submit+=(const char*)((*cli)->LogPath);
     submit+="Killer.";
     submit+=tmp;
     submit+=".log ";
    }
    submit+=(const char*)((*cli)->WholeScriptPath);
    submit+=" -";
    sprintf(tmp,"%d",signal);
    submit+=tmp;
    submit+=" ";
    sprintf(tmp,"%d",(self?ac.id.pid:ac.id.ppid));
    submit+=tmp;
    if((*cli)->LogInTheEnd){
     submit+=" ";
     submit+=(const char*)((*cli)->LogPath);
     submit+="Killer.";
     sprintf(tmp,"%d",ac.id.uid);
     submit+=tmp;
     submit+=".log ";
    }

    cout <<"kill "<<submit<<endl;
    return system(submit);
}


bool Server_impl::Lock(const DPS::Client::CID & pid, OpType optype, ClientType type,int TimeOut){ 
time_t tt;
time(&tt);
switch (optype){

case StartClient: KillClient: CheckClient:
// Check if no lock
  for (AMSServerI* pcur=this;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
    if(pcur->getType()==type){
    for ( list<DPS::Server::CriticalOps>::iterator li=pcur->getcol().begin();li!=pcur->getcol().end();++li){
      if (li->Action == _clear(optype)){
        if(tt<(*li).TimeStamp+(*li).TimeOut){
          if(_parent->Debug())_parent->IMessage(AMSClient::print(*li,"Lock found"));
          return false;
      }
      else{
       _col.erase(li);        
      }
     }
   }
  }
 }
break;
}

  DPS::Client::CID cid;
  cid.Type=getType();
  cid.Interface= (const char *) " ";
  DPS::Server::CriticalOps op;
  op.TimeStamp=tt;
  op.TimeOut=TimeOut;
  op.Type=type;
  op.Action=optype;
  op.id=pid.uid;
  DPS::Client::ARS * pars;
  int length=getARS(cid,pars);
  DPS::Client::ARS_var arf=pars;
  for(int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Server_var _pvar=DPS::Server::_narrow(obj);
    _pvar->sendCriticalOps(pid,op);
  }
   catch (CORBA::SystemException &ex){
   }
  }
 return true;
} 


void Producer_impl::RunFailed(const DPS::Client::ActiveClient & acv){
   uinteger Run=0;
   RLI li=find_if(_rl.begin(),_rl.end(),REInfo_EqsClient((acv.id)));
    if(li!=_rl.end()){
     if((*li)->Status == Processing){
       RunEvInfo_var rv=*li;
       rv->cinfo.HostName=(acv.id).HostName;
      if(rv->History !=Failed){
        rv->Status=ToBeRerun;
        rv->History=Failed;
      }
      else      rv->Status=Failed;
       rv->cuid=0;
       Run=rv->Run;
       _parent->EMessage(AMSClient::print(rv, " run Failed "));
       Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
       _pser->MonInfo(AMSClient::print(rv, " Run Failed: "),DPS::Client::Error);
       PropagateRun(rv,DPS::Client::Update);
     }
    }

for(DSTLI ni=_dst.begin();ni!=_dst.end();++ni){
 if( ((*ni).second)->Run==Run){
  ((*ni).second)->Status=Failure;
   
  PropagateDST((*ni).second,DPS::Client::Update,DPS::Client::AnyButSelf,_parent->getcid().uid);

 }
}



}


bool Server_impl::PingServer(const DPS::Client::ActiveClient & ac){
    DPS::Client::CID cid=_parent->getcid();
    cid.Type=getType();
    cid.Interface= (const char *)" ";
    DPS::Client::ARS * pars;
    int length=getARS(cid,pars,DPS::Client::Self,ac.id.uid);
    DPS::Client::ARS_var arf=pars;
  for(int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Server_var _pvar=DPS::Server::_narrow(obj);
     if(!CORBA::is_nil(_pvar)){
     _pvar->ping();
     return true;
     }
   }
   catch (CORBA::TRANSIENT &tr){
    _parent->EMessage(AMSClient::print(ac,"Transient Error Occurs"));
    return true;    
   }
   catch (CORBA::SystemException &ex){
   }
  } 
  return false;
}

bool Producer_impl::NotAllServersAlive(){
    DPS::Client::CID cid=_parent->getcid();
    cid.Type=getServer()->getType();
    cid.Interface= (const char *)" ";
    DPS::Client::ARS * pars;
    int length=getARS(cid,pars);
    DPS::Client::ARS_var arf=pars;
  for(int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Server_var _pvar=DPS::Server::_narrow(obj);
     if(!CORBA::is_nil(_pvar)){
     _pvar->ping();
     }
   }
   catch (CORBA::SystemException &ex){
     return false;
   }
  } 
  return true;
}


void Server_impl::_PurgeQueue(){
for(ACALI li=_acqueue.begin();li!=_acqueue.end();++li){
if(_parent->Debug())_parent->IMessage(AMSClient::print(li->getacv(),"PurgingACQueue"));
 PropagateAC(li->getacv(),li->getaction(),DPS::Client::Self,_RecID);
}
_acqueue.clear();
}

void Producer_impl::_PurgeQueue(){
for(ACALI li=_acqueue.begin();li!=_acqueue.end();++li){
if(_parent->Debug())_parent->IMessage(AMSClient::print(li->getacv(),"PurgingACQueue"));
 PropagateAC(li->getacv(),li->getaction(),DPS::Client::Self,_RecID);
}
_acqueue.clear();
for(RALI li=_runqueue.begin();li!=_runqueue.end();++li){
if(_parent->Debug())_parent->IMessage(AMSClient::print(li->getacv(),"PurgingRunQueue"));
 PropagateRun(li->getacv(),li->getaction(),DPS::Client::Self,_RecID);
}
_runqueue.clear();
for(DSTALI li=_dstqueue.begin();li!=_dstqueue.end();++li){
if(_parent->Debug())_parent->IMessage(AMSClient::print(li->getacv(),"PurgingDSTQueue"));
 PropagateDST(li->getacv(),li->getaction(),DPS::Client::Self,_RecID);
}
_dstqueue.clear();

}

void AMSServer::StopEverything(){
for (AMSServerI * pser=_pser;pser;pser=pser->down()?pser->down():pser->next()){
   Server_impl *ptrue=dynamic_cast<Server_impl*>(pser->getServer());
  if(ptrue){
    for (AMSServerI::AHLI li=pser->getahl().begin();li!=pser->getahl().end();++li){
     (*li)->Status=DPS::Client::InActive;
   }
   for (AMSServerI::ACLI li=pser->getacl().begin();li!=pser->getacl().end();++li){
    if((*li)->id.uid!=_pid.uid || (*li)->id.pid!=_pid.pid)ptrue->Kill(*li,SIGTERM,true);
  }
 }
}
IMessage("Raising SIGTERM ");
raise(SIGTERM);
}


void AMSServer::DumpIOR(){
IMessage("DumpIOR");
 for(map<AString,CORBA::String_var>::iterator li=_pser->getrefmap().begin();li!=_pser->getrefmap().end();++li){
IMessage((const char*)li->first);
IMessage((const char*)li->second);
}

 ofstream fbin;
 fbin.open("/tmp/DumpIOR");
 if(fbin){
 for(map<AString,CORBA::String_var>::iterator li=_pser->getrefmap().begin();li!=_pser->getrefmap().end();++li){
 fbin <<((const char*)li->first)<<endl;
 fbin<<((const char*)li->second)<<endl;
 }
 fbin.close();
 }
 else EMessage(" Unable To Open /tmp/DumpIOR File");

}


uinteger Producer_impl::getSmartFirst(uinteger run){
 pair<DSTLI,DSTLI>dst[3];
 bool present[3];
 dst[0]=_dst.equal_range(Ntuple);
 dst[1]=_dst.equal_range(RootFile);
 dst[2]=_dst.equal_range(EventTag);
  bool check=false;
 for (int i=0;i<3;i++){
  if(dst[i].first==dst[i].second)present[i]=false;
  else present[i]=true;
   check=check || present[i];
 }
  if(!check)return 0;
  uinteger first[3]={INT_MAX,INT_MAX,INT_MAX};
 for(int i=0;i<3;i++){
  if(present[i]){
   first[i]=0;
   for(DSTLI li=dst[i].first;li!=dst[i].second;++li){
    if((li->second)->Status ==Success && (li->second)->Run==run){
     if(first[i]<((li->second)->LastEvent)+1)first[i]=((li->second)->LastEvent)+1;
    }
   }
  }
 }
 uinteger veryfirst=INT_MAX;
 for(int i=0;i<3;i++){
  if(veryfirst>first[i])veryfirst=first[i];
 }
 return veryfirst;
}

#include <sys/stat.h>
#include <sys/file.h>

int Producer_impl::getRun(const DPS::Client::CID &cid, const FPath & fpath ,RUN_out run,TransferStatus & st)throw (CORBA::SystemException,FailedOp){
const int maxs=16000000;
_UpdateACT(cid,DPS::Client::Active);
 ifstream fbin;
 struct stat statbuf;
 st=Continue;
 stat((const char*)fpath.fname, &statbuf);
 fbin.open((const char*)fpath.fname);
 if(!fbin){
  throw FailedOp("Server-F-Unable to open file");
 }
 int last=statbuf.st_size-fpath.pos;
 if(last>maxs)last=maxs;
 else st=End;
RUN_var vrun=new RUN();
vrun->length(last);
 fbin.seekg(fpath.pos);
 fbin.read(( char*)vrun->get_buffer(),last);
 if(!fbin.good()){
   throw FailedOp((const char*)"Server-F-Unable to read file");
 }
 fbin.close();
 run=vrun._retn();
 _UpdateACT(cid,DPS::Client::Active);
 return last;
}


int Producer_impl::sendFile(const DPS::Client::CID &cid, const FPath & fpath ,const  RUN & run,TransferStatus & st)throw (CORBA::SystemException,FailedOp){
const int maxs=16000000;
_UpdateACT(cid,DPS::Client::Active);
   AString fname;
   char* gtv=getenv("AMSProdOutputDir");
   if(gtv && strlen(gtv)>0){
     fname=gtv;
     fname+="/";
   }
   else throw FailedOp((const char*)"Server-F-AMSProdOutputDir NotDefined");
   fname+=(const char*)fpath.fname;
 ofstream fbin;
 fbin.open((const char*)fname,ios::out|ios::app);
 if(!fbin){
  throw FailedOp((const char*)"Server-F-Unable to open file");
 }
 fbin.write(( char*)run.get_buffer(),run.length());
 if(!fbin.good()){
   throw FailedOp((const char*)"Server-F-Unable to write file");
 }
 fbin.close();
  _UpdateACT(cid,DPS::Client::Active);
 return 1;
}



void Server_impl::sendNC(const DPS::Client::CID &  cid, const  DPS::Client::NominalClient & nc, DPS::Client::RecordChange rc)throw (CORBA::SystemException){


  for (AMSServerI* pcur=this;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
    if(pcur->getType()==cid.Type){
      NCLI li=find_if(pcur->getncl().begin(),pcur->getncl().end(),Eqs_n(nc));
      switch (rc){
       case DPS::Client::Update:
       if(li==pcur->getncl().end())_parent->EMessage(AMSClient::print(nc,"Client not found for editing"));
       else{
        DPS::Client::NominalClient_var vac= new DPS::Client::NominalClient(nc);
        replace_if(li,pcur->getncl().end(),Eqs_n(nc),vac);
       }
       break;
       case DPS::Client::Create:
       if(li!=pcur->getncl().end())_parent->EMessage(AMSClient::print(nc,"Client already exists"));
       else{
        DPS::Client::NominalClient_var vac= new DPS::Client::NominalClient(nc);
         pcur->getncl().push_back(vac);
       }
       break;
       case DPS::Client::Delete:
       if(li==pcur->getncl().end())_parent->EMessage(AMSClient::print(nc,"Client not found for deleting"));
       else{
         pcur->getncl().erase(li);
       }
       break;
       default:
        _parent->EMessage(AMSClient::print(nc,"sendNC-Unknown rc value"));
     }

   }
 }
}

void Server_impl::sendNK(const DPS::Client::CID &  cid, const  DPS::Client::NominalClient & nc, DPS::Client::RecordChange rc)throw (CORBA::SystemException){


      NCLI li=find_if(_nki.begin(),_nki.end(),Eqs_n(nc));
      switch (rc){
       case DPS::Client::Update:
       if(li==_nki.end())_parent->EMessage(AMSClient::print(nc,"sendNK-Client not found for editing"));
       else{
        DPS::Client::NominalClient_var vac= new DPS::Client::NominalClient(nc);
        replace_if(li,_nki.end(),Eqs_n(nc),vac);
       }
       break;
       case DPS::Client::Create:
       if(li!=_nki.end())_parent->EMessage(AMSClient::print(nc,"sendNK-Client already exists"));
       else{
        DPS::Client::NominalClient_var vac= new DPS::Client::NominalClient(nc);
         _nki.push_back(vac);
       }
       break;
       case DPS::Client::Delete:
       if(li==_nki.end())_parent->EMessage(AMSClient::print(nc,"sendNK-Client not found for deleting"));
       else{
         _nki.erase(li);
       }
       break;
       default:
        _parent->EMessage(AMSClient::print(nc,"sendNK-Unknown rc value"));
     }

   }

#include <sys/statfs.h>
CORBA::Boolean Server_impl::getDBSpace(const DPS::Client::CID &cid, const char * path, const char * addpath, DB_out dbo)throw (CORBA::SystemException){

   DB_var dbv= new DB();
   AString amsdatadir; 
   char* gtv=getenv(path);
   if(gtv && strlen(gtv)>0){
     amsdatadir=gtv;
   }
   else{
      AString a("UnknownENVingetDBSpace ");
      a+=path;
      a+=" ";
      a+=addpath;
    _parent->EMessage(a);
   dbv->fs=(const char *)" Unknown";
   dbv->dbfree=-1;
   dbv->dbtotal=-1;
   dbv->bs=-1;
   dbo=dbv._retn();
   return false;
  }
    
    amsdatadir+=addpath;
  struct statfs buffer;
  int fail=statfs((const char*)amsdatadir,&buffer); 
  if(fail){
   dbv->fs=(const char *)amsdatadir;
   dbv->dbfree=-1;
   dbv->dbtotal=-1;
   dbv->bs=-1;
   dbo=dbv._retn();
   return false;
  }
  else{
   dbv->fs=(const char *)amsdatadir;
   dbv->dbfree= (buffer.f_bavail*(buffer.f_bsize/1024./1024.));
   dbv->dbtotal= (buffer.f_blocks*(buffer.f_bsize/1024/1024.));
   dbv->bs=1024*1024;
   dbo=dbv._retn();
  }
  return true;
}



int Client_impl::getARS(const DPS::Client::CID & cid, DPS::Client::ARS_out arf, DPS::Client::AccessType type, uinteger id, int selffirst)throw (CORBA::SystemException){



 Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 

return _pser->getARS(cid, arf,type,id,selffirst);

}
  CORBA::Boolean Client_impl::sendId(DPS::Client::CID& cid, uinteger timeout) throw (CORBA::SystemException){
    _KillTimeOut=timeout;
    if(cid.Type==DPS::Client::Monitor){
     DPS::Client::ActiveClient_var vac=new DPS::Client::ActiveClient();
      if(_acl.size()){
       _Submit=((*(_acl.begin()))->id).uid;
      }
      else _Submit=0;
     if(cid.uid==0){
      cid.uid=++_Submit;
     } 
      (vac->id).uid=cid.uid;
      (vac->id).pid=cid.pid;
      (vac->id).ppid=cid.ppid;
      (vac->id).Type=cid.Type;
      (vac->id).Interface=cid.Interface;
       vac->Status=DPS::Client::Registered;
       time_t tt;
       time(&tt);
       vac->LastUpdate=tt;
       vac->Start=tt;
     (vac->ars).length(1);
     ((vac->ars)[0]).Interface=(const char *)("Dummy");
     ((vac->ars)[0]).IOR=(const char *)(" ");
     ((vac->ars)[0]).Type=DPS::Client::Generic;
#ifdef __AMSDEBUG__
        _parent->IMessage(AMSClient::print(cid,"Client_impl::sendId-I-RegClient") );
#endif 
         _acl.push_front(vac);
       return true;
     }
    else if(cid.Type==DPS::Client::DBServer){
     for(ACLI j=_acl.begin();j!=_acl.end();++j){
      if(((*j)->id).uid==cid.uid){
        cid.ppid=((*j)->id).pid;
       ((*j)->id).pid=cid.pid;
       ((*j)->id).ppid=cid.ppid;
       cid.Interface=CORBA::string_dup(((*j)->id).Interface);
       cid.Type=((*j)->id).Type;
       (*j)->Status=DPS::Client::Registered;
       time_t tt;
       time(&tt);
       (*j)->LastUpdate=tt;
#ifdef __AMSDEBUG__
       _parent->IMessage(AMSClient::print(cid,"Client_impl::sendId-I-RegClient "));
#endif 
//       DPS::Client::ActiveClient_var acv=*j;
//       PropagateAC(acv,DPS::Client::Update);
       return true;
      }
      }
       _parent->EMessage(AMSClient::print(cid,"Client_impl::sendId-E-RegClientNotFound "));
     return false;

    }
  }

void Client_impl::getId(DPS::Client::CID_out  cid)throw (CORBA::SystemException){

  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
   _pser->getId(cid);
}

  Client_impl::Client_impl(DPS::Client::ClientType type, const char* ctype,AMSClient * parent):POA_DPS::Client(),AMSServerI(AMSID(ctype,0),parent,type){
 if(type==DBServer){
 }
}

Client_impl::Client_impl(DPS::Client::ClientType type, const char* ctype,DPS::Server_ptr _svar,DPS::Client::CID  cid,AMSClient * parent):POA_DPS::Client(),AMSServerI(AMSID(ctype,0),parent,type){

DPS::Client::ACS * pacs;
cid.Type=getType();
int length=_svar->getACS(cid,pacs,_Submit);
ACS_var acs=pacs;
for(int i=0;i<length;i++){
 DPS::Client::ActiveClient_var vac= new DPS::Client::ActiveClient(acs[i]);
 _acl.push_back(vac);
}
 cout <<"AML size "<<_acl.size()<<" "<<_Submit<<endl;

}

void Client_impl::_init(){
  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
  _defaultorb=_pser->getdefaultorb();
}


void Client_impl::StartClients(const DPS::Client::CID & pid){
 if(getType() == DBServer){
  RegisteredClientExists();
    for(AMSServerI * pcur=getServer(); pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
    if(pcur->InactiveClientExists(getType()))return;
 
}
  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
  bool freeslot=true;
    for(ACLI li=_acl.begin();li!=_acl.end();++li){
     if(strstr((const char*)_parent->getcid().HostName,(const char*)(*li)->id.HostName)){
       freeslot=false;
       break;
     }
    } 
  if(_acl.size()<_parent->getmaxdb() && freeslot){
   if(_pser->Lock(pid,DPS::Server::StartClient,getType(),_StartTimeOut)){
   char * perldir=getenv("AMSProdPerl");
     if(!perldir){
      _parent->EMessage("AMSProdPerl not defined");
     }
     else{
               AString submit(perldir);
               submit+="/dbserver.start -U";
               char tmp[80];
               sprintf(tmp,"%d",_Submit+1);
               submit+=tmp;
               _pser->UpdateDBFileName();
               if(_parent->getdbfile()){
                 submit+=" -F";
                 submit+=_parent->getdbfile();
               }
               else{
                submit+=" -A";
                submit+=getenv("AMSDataDir");
                submit+="/prod.log/";
               }
               submit+=" -I";
               submit+=perldir;
               submit+=" -D1";
               submit+=" -i";
   CORBA::String_var _refstring=_pser->getrefmap().find((const char*)_parent->getcid().Interface)->second;
               submit+=(const char*) _refstring;
           cout <<" submit "<<submit<<endl;
      int pid=fork();
//      int pid=1000;
      if(pid==-1){
       _parent->EMessage("Unable to fork DBServer");
      }
      else if(pid==0){
        //Child
               char *argv[4];
               argv[0] = "";
               argv[1] = "-c";
               char *arg = new char[submit.length()+1];
               strcpy(arg,(const char*)submit);
               argv[2] = arg;
               argv[3] = 0;
               execve("/bin/sh", argv, __environ);
               exit(127);
      }
      else{
     // Add New Active Client
       DPS::Client::ActiveClient ac;
       ac.id=_parent->getcid();
       ac.id.uid=_Submit+1;
     ac.id.Type=getType();
     ac.id.ppid=ac.id.pid;
     ac.id.pid=pid;;
     ac.id.Status=DPS::Client::NOP;
     ac.Status=DPS::Client::Submitted;
     time_t tt;
     time(&tt);
     ac.LastUpdate=tt;     
     ac.Start=tt;
     (ac.ars).length(1);
     ((ac.ars)[0]).Interface=(const char *)("Dummy");
     ((ac.ars)[0]).IOR=(const char *)(" ");
     ((ac.ars)[0]).Type=DPS::Client::Generic;
     ((ac.ars)[0]).uid=0;
     DPS::Client::ActiveClient_var acv=new DPS::Client::ActiveClient(ac);
     PropagateAC(acv,DPS::Client::Create);

      }
     }
     _pser->Lock(pid,DPS::Server::ClearStartClient,getType(),_StartTimeOut);
    }
   }
  }

 }

void Client_impl::CheckClients(const DPS::Client::CID & cid){
if(!Master())return;
Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
if(!_pser->Lock(cid,DPS::Server::CheckClient,getType(),_KillTimeOut))return;
time_t tt;
time(&tt);
for(ACLI li=_acl.begin();li!=_acl.end();++li){
 // find clients with timeout
 if((*li)->Status!=DPS::Client::Killed && (*li)->LastUpdate+_KillTimeOut<tt){
   if(PingClient(*li)){
    _UpdateACT((*li)->id,DPS::Client::Active);
   }
   else{
    DPS::Client::ActiveClient_var acv=*li;
    acv->Status=DPS::Client::TimeOut;
    if(_parent->Debug())_parent->EMessage(AMSClient::print(acv,"Client TIMEOUT"));
    PropagateAC(acv,DPS::Client::Update);
 }
 }
}

_pser->Lock(cid,DPS::Server::ClearCheckClient,getType(),_KillTimeOut);

}

void Client_impl::KillClients(const DPS::Client::CID & pid){

if(!Master())return;
Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
if(!_pser->Lock(pid,DPS::Server::KillClient,getType(),_KillTimeOut))return;

 ACLI li=find_if(_acl.begin(),_acl.end(),find(DPS::Client::TimeOut)); 
 if(li!=_acl.end()){
    DPS::Client::ActiveClient_var acv=*li;
    PropagateAC(acv,DPS::Client::Delete);
 }

_pser->Lock(pid,DPS::Server::ClearKillClient,getType(),_KillTimeOut);


}



void Client_impl::Exiting(const CID & cid, const char * message, DPS::Client::ClientExiting status)throw (CORBA::SystemException){
_parent->IMessage(AMSClient::print(cid,message?message:"Client exiting"));
// find and remove client
for( ACLI li=_acl.begin();li!=_acl.end();++li){
 if (cid.uid==((*li)->id).uid){
        if(_parent->Debug())_parent->IMessage(AMSClient::print((*li),"Deleting Client "));
   DPS::Client::ActiveClient_var acv=*li;
   PropagateAC(acv,DPS::Client::Delete);
   return;

}
 _parent->EMessage(AMSClient::print(cid,"Server_impl::Exiting::No Such Client"));
}

}
bool Server_impl::MonDialog(const char * message, DPS::Client::ErrorType error){
  unsigned long timeout=20;
  for (AMSServerI* pcur=this;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
    if(pcur->getType()==DPS::Client::Monitor){
     for (ACLI li=pcur->getacl().begin();li!=pcur->getacl().end();++li){
      try{
       CORBA::Object_var obj=_defaultorb->string_to_object(((*li)->ars)[0].IOR);
       if(!CORBA::is_nil(obj)){
        DPS::Monitor_var _mvar=DPS::Monitor::_narrow(obj);
       if(!CORBA::is_nil(_mvar)){
        try{
         CORBA::Boolean kill=_mvar->MonDialog(_parent->getcid(),message,error,timeout);
         return kill;
        }
        catch (CORBA::SystemException &ex){
         cerr<<" oops corba exc talking monitor"<<endl;
        }
       }
       }
       }
         catch (CORBA::SystemException &ex){
         cerr<<" oops corba exc init mondialog"<<endl;
         }
     }

     break;
    }
  }
  return true;
}

void Server_impl::MonInfo(const char * message, DPS::Client::ErrorType error){
  unsigned long timeout=20;
  for (AMSServerI* pcur=this;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
    if(pcur->getType()==DPS::Client::Monitor){
     for (ACLI li=pcur->getacl().begin();li!=pcur->getacl().end();++li){
      try{
       CORBA::Object_var obj=_defaultorb->string_to_object(((*li)->ars)[0].IOR);
       if(!CORBA::is_nil(obj)){
        DPS::Monitor_var _mvar=DPS::Monitor::_narrow(obj);
       if(!CORBA::is_nil(_mvar)){
        try{
         _mvar->MonInfo(_parent->getcid(),message,error,timeout);
         return;
        }
        catch (CORBA::SystemException &ex){
         cerr<<" oops corba exc talking monitor"<<endl;
        }
       }
       }
       }
         catch (CORBA::SystemException &ex){
         cerr<<" oops corba exc init moninfo"<<endl;
         }
     }
     break;
    }
  }
  
}

bool Client_impl::PingClient(const DPS::Client::ActiveClient & ac){
       if(ac.id.Type==DPS::Client::Monitor ){
        try{
        CORBA::Object_var obj=_defaultorb->string_to_object((ac.ars)[0].IOR);
        if(!CORBA::is_nil(obj)){
        DPS::Monitor_var _mvar=DPS::Monitor::_narrow(obj);
        if(!CORBA::is_nil(_mvar)){
         try{
          _mvar->ping();
          return true;
         }
         catch (CORBA::SystemException &ex){
         cerr<<" oops corba exc talking monitor"<<endl;
         }
        }
       }
      }
      catch (CORBA::SystemException &ex){
         cerr<<" oops corba exc init monitor"<<endl;
      }
      return false;
   }
       if(ac.id.Type==DPS::Client::DBServer ){
        try{
        CORBA::Object_var obj=_defaultorb->string_to_object((ac.ars)[0].IOR);
        if(!CORBA::is_nil(obj)){
        DPS::DBServer_var _mvar=DPS::DBServer::_narrow(obj);
        if(!CORBA::is_nil(_mvar)){
         try{
          _mvar->ping();
          return true;
         }
         catch (CORBA::SystemException &ex){
         cerr<<" oops corba exc talking monitor"<<endl;
         }
        }
       }
      }
      catch (CORBA::SystemException &ex){
         cerr<<" oops corba exc init monitor"<<endl;
      }
      return false;
   }
   return true;
}



bool Client_impl::Master(){
 Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 

return _pser->Master();
}

void Server_impl::setEnv(const CID & cid, const char * env, const char *path)throw (CORBA::SystemException){
  setenv(env,path,1);

if(!strcmp(env,"AMSDataDir")){
   AString amsdatadir(path); 
   amsdatadir+="/DataBase/";
     if(AMSDBc::amsdatabase)delete[] AMSDBc::amsdatabase;
     AMSDBc::amsdatabase=new char[strlen((const char*)amsdatadir)+1];
     strcpy(AMSDBc::amsdatabase,(const char *)amsdatadir);
     _parent->IMessage("Server_impl::setEnv-I-DataBaseRedefined");
}
     AString message("Server_impl::setEnv-I-Redefined ");
     message+=env;
     message+=" to ";
     message+=path;
     _parent->IMessage((const char*)message);
}

int Server_impl::getEnv(const CID & cid, SS_out ss)throw (CORBA::SystemException){
char ** e1;
int length=0;
for (e1=__environ;*e1;e1++){
 if((*e1)[0]=='A' && (*e1)[1]=='M' &&(*e1)[2]=='S'){
  length++;
 }
}
SS_var vss= new SS();
if(length==0)vss->length(1);
else vss->length(length);
length=0;
for (e1=__environ;*e1;e1++){
 if((*e1)[0]=='A' && (*e1)[1]=='M'  &&(*e1)[2]=='S'){
  vss[length]=(const char*)(*e1);
  length++;
 }
}
 ss=vss._retn();
 return length;
}

void Server_impl::UpdateDBFileName(){
for(AMSServerI * pcur=getServer(); pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
 if(pcur->getType()==DPS::Client::DBServer){
   pcur->getacl().sort(Less(_parent->getcid()));
   for (ACLI li=pcur->getacl().begin();li!=pcur->getacl().end();++li){
   for (int i=0;i<((*li)->ars).length();i++){
    try{
      CORBA::Object_var obj=_defaultorb->string_to_object(((*li)->ars)[i].IOR);
      DPS::DBServer_var dvar=DPS::DBServer::_narrow(obj);
      CORBA::String_var filepath=dvar->getDBFilePath(_parent->getcid());
      _parent->setdbfile(filepath);
      return;
     }
     catch (CORBA::SystemException &ex){
      cerr<<" oops corba error during updatadbfile" <<endl;
     }
    }
   }
   _parent->resetdbfile();
   return;
 }
}
}

bool AMSServerI::getHostInfoSDB(const DPS::Client::CID & cid, DPS::Client::ActiveHost_var & prv){
//return false;
for(AMSServerI * pcur=getServer(); pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
 if(pcur->getType()==DPS::Client::DBServer){
   bool done=false;
   bool retry=false;
   pcur->getacl().sort(Less(_parent->getcid()));
   for (ACLI li=pcur->getacl().begin();li!=pcur->getacl().end();++li){
   while( ! done){
   for (int i=0;i<((*li)->ars).length();i++){
    try{
      CORBA::Object_var obj=_defaultorb->string_to_object(((*li)->ars)[i].IOR);
      DPS::DBServer_var dvar=DPS::DBServer::_narrow(obj);
      CORBA::String_var filepath=dvar->getDBFilePath(_parent->getcid());
      _parent->setdbfile(filepath);
      DPS::Client::AHS * pres;
      DPS::Client::CID tcid=cid;
      tcid.Type=getType();
      int length=dvar->getAHS(tcid, pres);
      DPS::Client::AHS_var res=pres; 
      if(length){
       _ahl.clear();
       for(int i=0;i<length;i++){
        DPS::Client::ActiveHost_var vre= new DPS::Client::ActiveHost(res[i]);
        _ahl.push_back(vre);
       }
      }
      else{
       _parent->EMessage(" UpdateRunTable-UnableToUpdate"); 
        return 0;
      }
      DPS::Client::ActiveHost *pre;
      int ret=dvar->getFreeHost(tcid,pre);     
      prv=pre;
      return ret;
     }
     catch(DPS::DBServer::DBProblem &dbl){
       _parent->EMessage((const char*)dbl.message); 
       sleep (2);
       retry=! retry;    
     }
     catch (CORBA::SystemException &ex){
      cerr<<" oops corba error during getfreehost" <<endl;
     }
    }
    if(!retry)return 0;
   }
  }
break;
}
}
return 0;

}
bool Producer_impl::getRunEvInfoSDB(const DPS::Client::CID & cid, RunEvInfo_var & prv, DSTInfo_var & pdv){
for(AMSServerI * pcur=getServer(); pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
 if(pcur->getType()==DPS::Client::DBServer){
   bool done=false;
   bool retry=false;
   pcur->getacl().sort(Less(_parent->getcid()));
   for (ACLI li=pcur->getacl().begin();li!=pcur->getacl().end();++li){
   while( ! done){
   for (int i=0;i<((*li)->ars).length();i++){
    try{
      CORBA::Object_var obj=_defaultorb->string_to_object(((*li)->ars)[i].IOR);
      DPS::DBServer_var dvar=DPS::DBServer::_narrow(obj);
      CORBA::String_var filepath=dvar->getDBFilePath(_parent->getcid());
      _parent->setdbfile(filepath);
      RES * pres;
      int length;
      if(strstr(pcur->getname(),"Perl")){
       length=dvar->getRunEvInfoSPerl(_parent->getcid(), pres,_RunID,_RunID);
      }
      else{
       length=dvar->getRunEvInfoS(_parent->getcid(), pres,_RunID);
      }
      RES_var res=pres; 
      if(length){
       _rl.clear();
       for(int i=0;i<length;i++){
        RunEvInfo_var vre= new RunEvInfo(res[i]);
        _rl.push_back(vre);
       }
      }
      else{
       _parent->EMessage(" UpdateRunTable-UnableToUpdate"); 
        return 0;
      }
      RunEvInfo *pre;
      DSTInfo *pde;
      dvar->getRunEvInfo(cid,pre,pde);     
      prv=pre;
      pdv=pde;
      return 1;
     }
     catch(DPS::DBServer::DBProblem &dbl){
       _parent->EMessage((const char*)dbl.message); 
       sleep (2);
       retry=! retry;    
     }
     catch (CORBA::SystemException &ex){
      cerr<<" oops corba error during getrunevinfo" <<endl;
     }
    }
    if(!retry)return 0;
   }
  }
break;
}
}
return 0;

}

bool Producer_impl::PropagateRunDB(const RunEvInfo & ri, DPS::Client::RecordChange rc){
for(AMSServerI * pcur=getServer(); pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
 if(pcur->getType()==DPS::Client::DBServer){
   bool done=false;
   bool retry=false;
   pcur->getacl().sort(Less(_parent->getcid()));
   for (ACLI li=pcur->getacl().begin();li!=pcur->getacl().end();++li){
   while( ! done){
   for (int i=0;i<((*li)->ars).length();i++){
    try{
      CORBA::Object_var obj=_defaultorb->string_to_object(((*li)->ars)[i].IOR);
      DPS::DBServer_var dvar=DPS::DBServer::_narrow(obj);
      dvar->sendRunEvInfo(ri,rc);
      return 1;
     }
     catch(DPS::DBServer::DBProblem &dbl){
       _parent->EMessage((const char*)dbl.message); 
       sleep (2);
       retry=! retry;    
     }
     catch (CORBA::SystemException &ex){
      cerr<<" oops corba error during sendrunevinfo" <<endl;
     }
    }
    if(!retry)return 0;
   }
  }
break;
}
}
return 0;

}

bool Producer_impl::PropagateDSTDB(const DST & ri, DPS::Client::RecordChange rc){
for(AMSServerI * pcur=getServer(); pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
 if(pcur->getType()==DPS::Client::DBServer){
   bool done=false;
   bool retry=false;
   pcur->getacl().sort(Less(_parent->getcid()));
   for (ACLI li=pcur->getacl().begin();li!=pcur->getacl().end();++li){
   while( ! done){
   for (int i=0;i<((*li)->ars).length();i++){
    try{
      CORBA::Object_var obj=_defaultorb->string_to_object(((*li)->ars)[i].IOR);
      DPS::DBServer_var dvar=DPS::DBServer::_narrow(obj);
      dvar->sendDSTEnd(_parent->getcid(),ri,rc);
      return 1;
     }
     catch(DPS::DBServer::DBProblem &dbl){
       _parent->EMessage((const char*)dbl.message); 
       sleep (2);
       retry=! retry;    
     }
     catch (CORBA::SystemException &ex){
      cerr<<" oops corba error during senddstend" <<endl;
     }
    }
    if(!retry)return 0;
   }
  }
break;
}
}
return 0;

}
bool Producer_impl::PropagateDSTInfoDB(const DSTInfo & ri, DPS::Client::RecordChange rc){
for(AMSServerI * pcur=getServer(); pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
 if(pcur->getType()==DPS::Client::DBServer){
   bool done=false;
   bool retry=false;
   pcur->getacl().sort(Less(_parent->getcid()));
   for (ACLI li=pcur->getacl().begin();li!=pcur->getacl().end();++li){
   while( ! done){
   for (int i=0;i<((*li)->ars).length();i++){
    try{
      CORBA::Object_var obj=_defaultorb->string_to_object(((*li)->ars)[i].IOR);
      DPS::DBServer_var dvar=DPS::DBServer::_narrow(obj);
      dvar->sendDSTInfo(ri,rc);
      return 1;
     }
     catch(DPS::DBServer::DBProblem &dbl){
       _parent->EMessage((const char*)dbl.message); 
       sleep (2);
       retry=! retry;    
     }
     catch (CORBA::SystemException &ex){
      cerr<<" oops corba error during senddstinfo" <<endl;
     }
    }
    if(!retry)return 0;
   }
  }
break;
}
}
return 0;

}

bool AMSServerI::PropagateACDB(DPS::Client::ActiveClient & ac, DPS::Client::RecordChange rc){
for(AMSServerI * pcur=getServer(); pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
 if(pcur->getType()==DPS::Client::DBServer){
   bool done=false;
   bool retry=false;
   pcur->getacl().sort(Less(_parent->getcid()));
   for (ACLI li=pcur->getacl().begin();li!=pcur->getacl().end();++li){
   while( ! done){
   for (int i=0;i<((*li)->ars).length();i++){
    try{
      CORBA::Object_var obj=_defaultorb->string_to_object(((*li)->ars)[i].IOR);
      DPS::DBServer_var dvar=DPS::DBServer::_narrow(obj);
      DPS::Client::CID acid=ac.id;
      if(strstr(pcur->getname(),"Perl")){
       dvar->sendACPerl(acid,ac,rc);
      }
      else{
       dvar->sendAC(acid,ac,rc);
      }
      if(rc == DPS::Client::Delete){
//      Here find the corr Ahost and update it
        for(AMSServerI * curp=getServer(); curp; curp=(curp->down())?curp->down():curp->next()){
          if(curp->getType()==ac.id.Type){
             for(AHLI i=curp->getahl().begin();i!=curp->getahl().end();++i){
               if(!strcmp((const char *)(*i)->HostName, (const char *)((ac.id).HostName))){
               DPS::Client::ActiveHost_var ahlv= *i;
               (ahlv->ClientsRunning)--;
               (ahlv->ClientsProcessed)++; 
               time_t tt;
               time(&tt);
               (ahlv)->LastUpdate=tt;     
               switch ((ac.id).Status){
               case DPS::Client::CInExit: 
                (ahlv)->Status=DPS::Client::OK; 
                break;
               case DPS::Client::SInExit:
                (ahlv)->Status=DPS::Client::OK; 
                break;
               case DPS::Client::CInAbort:
                (ahlv)->Status=DPS::Client::LastClientFailed; 
                ((ahlv)->ClientsFailed)++; 
                break;
               case DPS::Client::SInKill:
                (ahlv)->Status=DPS::Client::LastClientFailed; 
                ((ahlv)->ClientsKilled)++; 
                break;
               }
               dvar->sendAH(acid,ahlv,DPS::Client::Update);
               break;
              }  
             }
            }
           }
       }
       else if(rc == DPS::Client::Create){
        for(AMSServerI * curp=getServer(); curp; curp=(curp->down())?curp->down():curp->next()){
          if(curp->getType()==ac.id.Type){
             for(AHLI i=curp->getahl().begin();i!=curp->getahl().end();++i){
            if(!strcmp((const char *)(*i)->HostName, (const char *)(ac.id).HostName)){
            DPS::Client::ActiveHost_var ahlv= *i;
            ((ahlv)->ClientsRunning)++;
            time_t tt;
            time(&tt);
            (ahlv)->LastUpdate=tt;     
            (ahlv)->Status=DPS::Client::OK;
             dvar->sendAH(acid,ahlv,DPS::Client::Update);
            break;
        }
         }
       }
      }
      }
      return 1;

     }
     catch(DPS::DBServer::DBProblem &dbl){
       _parent->EMessage((const char*)dbl.message); 
       sleep (2);
       retry=! retry;    
     }
     catch (CORBA::SystemException &ex){
      cerr<<" oops corba error during sendac" <<endl;
     }
    }
    if(!retry)return 0;
   }
  }
break;
}
}
return 0;

}



bool AMSServerI::PropagateAHDB(const DPS::Client::CID & pid,DPS::Client::ActiveHost & ah,DPS::Client::RecordChange rc){
for(AMSServerI * pcur=getServer(); pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
 if(pcur->getType()==DPS::Client::DBServer){
   bool done=false;
   bool retry=false;
   pcur->getacl().sort(Less(_parent->getcid()));
   for (ACLI li=pcur->getacl().begin();li!=pcur->getacl().end();++li){
   while( ! done){
   for (int i=0;i<((*li)->ars).length();i++){
    try{
      CORBA::Object_var obj=_defaultorb->string_to_object(((*li)->ars)[i].IOR);
      DPS::DBServer_var dvar=DPS::DBServer::_narrow(obj);
      dvar->sendAH(pid,ah,rc);
      return 1;
     }
     catch(DPS::DBServer::DBProblem &dbl){
       _parent->EMessage((const char*)dbl.message); 
       sleep (2);
       retry=! retry;    
     }
     catch (CORBA::SystemException &ex){
      cerr<<" oops corba error during sendah" <<endl;
     }
    }
    if(!retry)return 0;
   }
  }
break;
}
}
return 0;

}



