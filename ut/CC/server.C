#include <stdlib.h>
#include <server.h>
#include <fstream.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <algorithm>
#include <amsdbc.h>
AMSServer* AMSServer::_Head=0;

int main(int argc, char * argv[]){

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
 delete AMSServer::Singleton();
 return 0;
}




AMSServer::AMSServer(int argc, char* argv[]):_GlobalError(false){
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
 
 for (char *pchar=0; argc>1 &&(pchar=argv[1],*pchar=='-'); (argv++,argc--)){
    pchar++;
    switch (*pchar){
    case 'D':    //debuf
     _debug=atoi(++pchar);
     break;
    case 'I':   //Interface
     iface=++pchar;
     break;
    case 'C':   //corba ior
     ior=++pchar;
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
if(niface){
 ifstream fbin;
 fbin.open(niface);
 if(fbin){
   if(fbin.get()=='#')fbin.ignore(1024,'\n');
   else fbin.seekg(fbin.tellg()-sizeof(char));
  while(!fbin.eof() && fbin.good()){ 
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
  if(rfile){  
   _pser->add(new Producer_impl(_orbmap,_pid,this,nproducer,rfile,ntuplestring,eventtagstring));
   _pser->_init();
  Server_impl * pser=dynamic_cast<Server_impl*>(_pser);
  pser->setInterface(niface);
  pser->StartSelf(_pid,DPS::Client::Create);
  }  
  
 }
 else{

 try{
   CORBA::Object_var obj=(_orbmap.find((const char *)_pid.Interface)->second)._orb->string_to_object(ior);
    if(!CORBA::is_nil(obj)){
     DPS::Server_var _svar=DPS::Server::_narrow(obj);
     if(!CORBA::is_nil(_svar)){
         
        if(!(_svar->sendId(_pid,60))){
         FMessage("Server Requested Termination after sendID ",DPS::Client::SInAbort);
        }
        else{
         _pser= new Server_impl(_orbmap,_svar,_pid,this);
          Producer_impl a;
          if(_svar->TypeExists(a.getType())){
              _pser->add(new Producer_impl(_orbmap,_svar,_pid,this));
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
       (i->second)._orb->perform_work();
      }
}


void AMSServer::UpdateDB(){
// Her don't know yet
for(AMSServerI * pcur=_pser; pcur; pcur=(pcur->down())?pcur->down():pcur->next()){
pcur->UpdateDB();
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

void AMSServerI::PropagateAC(DPS::Client::ActiveClient & ac,DPS::Client::RecordChange rc,int uid){
  if(_ActivateQueue)_acqueue.push_back(ACA(ac,rc));
  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
  DPS::Client::CID cid;
  cid.Type=_pser->getType();
  cid.Interface= (const char *) " ";
    DPS::Client::ARS * pars;
    int length=_pser->getARS(cid,pars,uid);
    DPS::Client::ARS_var arf=pars;
  if(length==0){
      _parent->EMessage(AMSClient::print(ac,"PropagateAC-getARS retrurns 0"));
  }
//  cout <<"propagateAC - getars returns "<<length<<endl;
  for(int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Server_var _pvar=DPS::Server::_narrow(obj);
    _pvar->sendAC(ac.id,ac,rc);
   }
   catch (CORBA::SystemException &ex){
     cerr<<"oops corba exc found for "<<i<<endl;
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
   _RecID=(*li)->id.uid;
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

bool AMSServerI::InactiveClientExists(){
for(ACLI li= _acl.begin();li!=_acl.end();++li){
 if((*li)->Status!=DPS::Client::Active){
   return true;
  }
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
   DPS::Client::NominalClient_var ncl= new DPS::Client::NominalClient();
   if(fbin.get()=='#')fbin.ignore(1024,'\n');
   else fbin.seekg(fbin.tellg()-sizeof(char));
   fbin>>ncl->MaxClients>>ncl->CPUNeeded>>ncl->MemoryNeeded;
    char tmpbuf[1024];
    fbin>>tmpbuf;
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
   DPS::Client::NominalClient_var ncl= new DPS::Client::NominalClient();
   if(fbin.get()=='#')fbin.ignore(1024,'\n');
   else fbin.seekg(fbin.tellg()-sizeof(char));
   fbin>>ncl->MaxClients>>ncl->CPUNeeded>>ncl->MemoryNeeded;
    char tmpbuf[1024];
    fbin>>tmpbuf;
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
 cout <<"ACL size"<<_acl.size()<<" "<<(*_ncl.begin())->MaxClients<<endl;

}



void Server_impl::StartClients(const DPS::Client::CID & pid){
if(!Master())return;
RegisteredClientExists();
if(InactiveClientExists())return;
  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 



if(!_pser->Lock(pid,StartClient,getType(),_StartTimeOut))return;

 // Check if there are some hosts to run on
 _ahl.sort(Less());
 for(AHLI i=_ahl.begin();i!=_ahl.end();++i){
  if(_acl.size()<(*_ncl.begin())->MaxClients ){
  if((*i)->Status!=NoResponse){
    if((*i)->ClientsRunning>=(*i)->ClientsAllowed)continue;
    // HereStartClient
   CORBA::String_var _refstring=_refmap.find((const char *)((*i)->Interface))->second;
#ifdef __AMSDEBUG__
//    cout <<((*i)->Interface)<<" "<<_refstring<<endl;
#endif
    NCLI cli=find_if(_ncl.begin(),_ncl.end(),NCL_find((const char *)(*i)->HostName)); 
    if(cli==_ncl.end())cli=_ncl.begin();
    AString submit;
    submit+=(const char*)((*cli)->SubmitCommand);  
    submit+=" ";
    submit+=(const char*)((*i)->HostName);
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
    submit+=(const char*)(*i)->Interface;
    if((*cli)->LogInTheEnd){
     submit+=" ";
     submit+=(const char*)((*cli)->LogPath);  
     submit+="Server.";
     submit+=tmp;
     submit+=".log ";
    }
#ifdef __AMSDEBUG__
    submit+=" -D1";
    cout <<submit<<endl;
#endif
    submit+=" &";
    int out=system(submit);
    if(out==0){
     // Add New Active Client
     DPS::Client::ActiveClient ac;
     (ac.id).HostName=CORBA::string_dup((*i)->HostName);
     ac.id.Interface=(*i)->Interface;
     (ac.id).uid=_Submit+1;
     ac.id.Type=getType();
     ac.id.pid=0;
     ac.id.ppid=0;
     ac.Status=DPS::Client::Submitted;
     time_t tt;
     time(&tt);
     ac.LastUpdate=tt;     
     ac.Start=tt;
     (ac.ars).length(1);
     ((ac.ars)[0]).Interface=(const char *)("Dummy");
/*
     _acl.push_back(ac);
     ((*i)->ClientsRunning)++;
*/
    DPS::Client::ActiveClient_var acv=new DPS::Client::ActiveClient(ac);
    PropagateAC(acv,DPS::Client::Create);
    }
    else{
     (*i)->Status=DPS::Client::LastClientFailed; 
     ((*i)->ClientsFailed)++; 
     AString a="StartClients-E-UnableToStartClient ";
     a+=submit;
     _parent->EMessage((const char *) a);
     
    }
    break;
   }
  }
 } 
 _pser->Lock(pid,ClearStartClient,getType(),_StartTimeOut);
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
   (*li)->id.Status=DPS::Client::SInKill;
   DPS::Client::ActiveClient_var acv=*li;
   PropagateAC(acv,DPS::Client::Delete);
 }
 else{
   (*li)->id.Status=DPS::Client::SInKill;
   (*li)->Status=DPS::Client::Killed;
   DPS::Client::ActiveClient_var acv=*li;
   PropagateAC(acv,DPS::Client::Update);
    //_pser->Kill((*li),SIGTERM,true);
 }
}

_pser->Lock(pid,DPS::Server::ClearKillClient,getType(),_KillTimeOut);


}

void Server_impl::CheckClients(const DPS::Client::CID & cid){
if(!Master())return;
Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
if(!_pser->Lock(cid,DPS::Server::CheckClient,getType(),_KillTimeOut))return;
for(AHLI li=_ahl.begin();li!=_ahl.end();++li){
 if((*li)->Status==NoResponse)if(_pser->pingHost((const char*)((*li)->HostName)))(*li)->Status=DPS::Client::OK;
}
time_t tt;
time(&tt);
for(ACLI li=_acl.begin();li!=_acl.end();++li){
 // find clients with timeout
 if((*li)->Status!=DPS::Client::Killed && (*li)->LastUpdate+_KillTimeOut<tt){
   ;
   if(PingServer(*li)){
    (*li)->LastUpdate=tt;
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
   ah.ClientsRunning=0;
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

int Server_impl::getNC(const DPS::Client::CID &cid, NCS_out acs)throw (CORBA::SystemException){

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






int Server_impl::getARS(const DPS::Client::CID & cid, DPS::Client::ARS_out  arf,int maxcid)throw (CORBA::SystemException){
DPS::Client::ARS_var arv=new ARS();
int length=0;
for (ACLI li=_acl.begin();li!=_acl.end();++li){
    int uid=(*li)->id.uid;
  for (int i=0;i<((*li)->ars).length();i++){
    if( (((*li)->ars)[i]).Type == cid.Type && !strcmp((const char *)  (((*li)->ars)[i]).Interface, (const char *)cid.Interface) && (uid<maxcid || uid==-maxcid)){
      length++;
    }
    else if( !strcmp(" ", (const char *)cid.Interface) && (((*li)->ars)[i]).Type == cid.Type && !strcmp((const char *)  (((*li)->ars)[i]).Interface, (const char *)((*li)->id).Interface) && (uid<maxcid || uid==-maxcid)){
      length++;
    }
  }
}

if(length==0){ 
  arv->length(1);
//  cerr<<"***ERROR*** getars-length== 0"<<endl;
}
else arv->length(length);
length=0;
for (ACLI li=_acl.begin();li!=_acl.end();++li){
    int uid=(*li)->id.uid;
  for (int i=0;i<((*li)->ars).length();i++){
    if( (((*li)->ars)[i]).Type == cid.Type && !strcmp((const char *)  (((*li)->ars)[i]).Interface, (const char *)cid.Interface) && (uid<maxcid || uid==-maxcid)){
      arv[length++]=((*li)->ars)[i];
    }
    else if( !strcmp(" ", (const char *)cid.Interface) && (((*li)->ars)[i]).Type == cid.Type && !strcmp((const char *)  (((*li)->ars)[i]).Interface, (const char *)((*li)->id).Interface) && (uid<maxcid || uid==-maxcid)){
      arv[length++]=((*li)->ars)[i];
    }
  }
}


arf=arv._retn();
return length;
}




int Server_impl::getACS(const DPS::Client::CID &cid, ACS_out acs, unsigned int & maxc)throw (CORBA::SystemException){

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
}
}
acs=acv._retn();
return length;
}
}


}



   void Server_impl::sendAC(const DPS::Client::CID &  cid,  DPS::Client::ActiveClient & ac, DPS::Client::RecordChange rc)throw (CORBA::SystemException){
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
            if(!strcmp((const char *)(*i)->HostName, (const char *)((*li)->id).HostName)){
//          cout << " host found for deleteing "<<endl;
        ((*i)->ClientsRunning)--;
       ((*i)->ClientsProcessed)++; 
       switch (((*li)->id).Status){
       case DPS::Client::CInExit: 
        break;
        case DPS::Client::SInExit:
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



void Server_impl::Exiting(const CID & cid, const char * message, DPS::Client::ClientExiting status)throw (CORBA::SystemException){
_parent->IMessage(AMSClient::print(cid,message?message:"Server exiting"));
// find and remove client
for( ACLI li=_acl.begin();li!=_acl.end();++li){
 if (cid.uid==((*li)->id).uid){
   (*li)->id.Status=status;
   DPS::Client::ActiveClient_var acv=*li;
   PropagateAC(acv,DPS::Client::Delete);
   return;
 }

}
 _parent->EMessage(AMSClient::print(cid,"Server_impl::Exiting::No Such Client"));
}



int Server_impl::getNHS(const DPS::Client::CID &cid, NHS_out acs)throw (CORBA::SystemException){

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
        length++;
       }
     }
    }
     else {
      (as.ars).length(1);
      ((as.ars)[0]).Interface=(const char *)("Dummy");
      ((as.ars)[0]).IOR=(const char *)(" ");
      ((as.ars)[0]).Type=DPS::Client::Generic;
     }
   DPS::Client::ActiveClient_var acv=new DPS::Client::ActiveClient(as);
   PropagateAC(acv,rc);
    sendAC(as.id,as,rc);
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
 if(fbin){
   DPS::Client::NominalClient_var ncl= new DPS::Client::NominalClient();
   if(fbin.get()=='#')fbin.ignore(1024,'\n');
   else fbin.seekg(fbin.tellg()-sizeof(char));
   fbin>>ncl->MaxClients>>ncl->CPUNeeded>>ncl->MemoryNeeded;
    char tmpbuf[1024];
    fbin>>tmpbuf;
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
//Here read runfiletable
if(RF){
 ifstream fbin;
 fbin.open(RF);
 if(fbin){
   int cur=0;
   if(fbin.get()=='#')fbin.ignore(1024,'\n');
   else fbin.seekg(fbin.tellg()-sizeof(char));
  while(!fbin.eof() && fbin.good()){ 
   DPS::Producer::RunEvInfo  re;
   re.uid=_RunID++;
    char tmpbuf[1024];
   fbin>>re.Run>>re.FirstEvent>>re.LastEvent>>re.TFEvent>>re.TLEvent>>re.Priority>>tmpbuf;
   re.FilePath=(const char*)tmpbuf;
   if(re.LastEvent<re.FirstEvent)re.LastEvent=2000000000;
   re.Status=DPS::Producer::ToBeRerun;
   re.History=DPS::Producer::ToBeRerun;
   re.UpdateFreq=100;
   re.DieHard=0;
   re.OutputDirPath=(const char*)NS;
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
length=_pvar->getNtupleS(cid,pdsts);
DSTS_var dsts=pdsts;
for(int i=0;i<length;i++){
DST_var vdst= new DST(dsts[i]);
 _ntuple.push_back(vdst);
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
if(InactiveClientExists())return;
if(getServer()->InactiveClientExists())return;
  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
  if(!_pser->Lock(pid,DPS::Server::StartClient,getType(),_StartTimeOut))return;
 // Check if there are some hosts to run on
 _ahl.sort(Less());
 for(AHLI i=_ahl.begin();i!=_ahl.end();++i){
  if(_acl.size()<(*_ncl.begin())->MaxClients && _acl.size()<count_if(_rl.begin(),_rl.end(),REInfo_Count())){
  if((*i)->Status!=NoResponse){
    if((*i)->ClientsRunning>=(*i)->ClientsAllowed)continue;
    // HereStartClient
   CORBA::String_var _refstring=_refmap.find((const char *)((*i)->Interface))->second;
#ifdef __AMSDEBUG__
//    cout <<((*i)->Interface)<<" "<<_refstring<<endl;
#endif
    NCLI cli=find_if(_ncl.begin(),_ncl.end(),NCL_find((const char *)(*i)->HostName)); 
    if(cli==_ncl.end())cli=_ncl.begin();
    AString submit;
    char tmp[80];
     sprintf(tmp,"%d",_Submit+1);
    submit+=(const char *)(*cli)->SubmitCommand;  
    submit+=" ";
    submit+=(const char*)((*i)->HostName);
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
    if((*cli)->LogInTheEnd){
     submit+=" ";
     submit+=(const char*)((*cli)->LogPath);  
     submit+="Producer.";
     submit+=tmp;
     submit+=".log ";
    }
#ifdef __AMSDEBUG__
    submit+=" -D1";
    cout <<submit<<endl;
#endif
    submit+=" &";
    int out=system(submit);
    if(out==0){
     // Add New Active Client
     DPS::Client::ActiveClient ac;
     (ac.id).HostName=CORBA::string_dup((*i)->HostName);
     ac.id.Interface=(*i)->Interface;
     (ac.id).uid=_Submit+1;
     ac.id.pid=0;
     ac.id.ppid=0;
     ac.id.Type=getType();
     ac.Status=DPS::Client::Submitted;
     time_t tt;
     time(&tt);
     ac.LastUpdate=tt;     
     ac.Start=tt;
     (ac.ars).length(1);
     ((ac.ars)[0]).Interface=(const char *)("Dummy");
    DPS::Client::ActiveClient_var acv=new DPS::Client::ActiveClient(ac);
    PropagateAC(acv,DPS::Client::Create);
    }
    else{
     (*i)->Status=DPS::Client::LastClientFailed; 
     ((*i)->ClientsFailed)++; 
     AString a="StartClients-E-UnableToStartClient ";
     a+=submit;
     _parent->EMessage((const char *)a);
    }
    break;
   }
  }
 }

 _pser->Lock(pid,DPS::Server::ClearStartClient,getType(),_StartTimeOut);  
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
   (*li)->id.Status=DPS::Client::SInKill;
   DPS::Client::ActiveClient_var acv=*li;
   PropagateAC(acv,DPS::Client::Delete);
 }
 else{
   (*li)->id.Status=DPS::Client::SInKill;
   (*li)->Status=DPS::Client::Killed;
   DPS::Client::ActiveClient_var acv=*li;
   PropagateAC(acv,DPS::Client::Update);
    //_pser->Kill((*li),SIGTERM,true);
 }
}

_pser->Lock(pid,DPS::Server::ClearKillClient,getType(),_KillTimeOut);
}





void Producer_impl::CheckClients(const DPS::Client::CID & cid){
if(!Master())return;
Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
if(!_pser->Lock(cid,DPS::Server::CheckClient,getType(),_KillTimeOut))return;
for(AHLI li=_ahl.begin();li!=_ahl.end();++li){
 if((*li)->Status==NoResponse)if(_pser->pingHost((const char*)((*li)->HostName)))(*li)->Status=DPS::Client::OK;
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
       DPS::Client::ActiveClient_var acv=*j;
       PropagateAC(acv,DPS::Client::Update);
//       return false;
#endif 
       return true;
      }
     }
     return false;


}


int Producer_impl::getARS(const DPS::Client::CID & cid, DPS::Client::ARS_out arf, int maxcid)throw (CORBA::SystemException){



 Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 

return _pser->getARS(cid, arf,maxcid);

}





void Producer_impl::Exiting(const CID & cid, const char * message, DPS::Client::ClientExiting status)throw (CORBA::SystemException){
_parent->IMessage(AMSClient::print(cid,message?message:" Producer Exiting"));
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
       PropagateRun(rv,DPS::Client::Update);
     }
    }

   return;
 }

}
 _parent->EMessage(AMSClient::print(cid,"Producer_impl::Exiting::No Such Client"));
}





int Producer_impl::getTDV(const DPS::Client::CID & cid,  TDVName & tdvname, TDVbody_out body)throw (CORBA::SystemException){
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


void Producer_impl::sendTDV(const DPS::Client::CID & cid, const TDVbody & tdv, TDVName & tdvname )throw (CORBA::SystemException){
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
// Here ohe has to update map as well
}
}
  
int Producer_impl::getTDVTable(const DPS::Client::CID & cid, TDVName & tdvname, unsigned int id, TDVTable_out table)throw (CORBA::SystemException){

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




void Producer_impl::getRunEvInfo(const DPS::Client::CID &cid, RunEvInfo_out ro)throw (CORBA::SystemException){
DPS::Client::CID pid;
pid.Interface=(const char *)" ";
pid.Type=getType();
DPS::Client::ARS * pars;      
int length=getARS(pid,pars,_parent->getcid().uid);
DPS::Client::ARS_var arf=pars; 
 for(int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Producer_var _pvar=DPS::Producer::_narrow(obj);
     cout <<" transferred to master "<<_parent->getcid().uid<<endl;
    _pvar->getRunEvInfo(cid,ro);
    return;
  }
    catch (CORBA::SystemException &ex){
     // Have to Kill Servers Here
   }
 }
 cout <<" Master getrunevinfo "<<_parent->getcid().uid<<endl;
_rl.sort(Less());
RunEvInfo_var rv=new RunEvInfo(); 
RLI li=find_if(_rl.begin(),_rl.end(),REInfo_find(cid,ToBeRerun));
if(li==_rl.end()){
 li=find_if(_rl.begin(),_rl.end(),REInfo_find(cid,Failed));
 if(li==_rl.end())rv->DieHard=1;
 else if(_parent->Debug()){
   cout <<  "****FAILED RUN RERUN "<<(*li)->Run<<" was "<<(*li)->cinfo.HostName <<" by "<<cid.HostName<<endl;
 }
}
if(li==_rl.end())rv->DieHard=1;
else if ( find_if(_rl.begin(),_rl.end(),REInfo_EqsClient(cid))!=_rl.end()){
rv->DieHard=1;
if(_parent->Debug())_parent->IMessage(AMSClient::print(cid,"Die HARD  !!!!!!!"));
}
else {
 rv=*li;
 rv->Status=Processing;
 rv->cuid=cid.uid;
  if(_parent->Debug()){
   _parent->IMessage(AMSClient::print (cid,"New Run Asked by"));  
   _parent->IMessage(AMSClient::print(rv));
  }
 PropagateRun(rv, DPS::Client::Update);
}
ro=rv._retn();
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


int Producer_impl::getNtupleS(const DPS::Client::CID & ci, DSTS_out dsts)throw (CORBA::SystemException){

DSTS_var acv= new DSTS();
int length=0;
length=_ntuple.size();
if(length==0){
acv->length(1);
}
else{
acv->length(length);
length=0;
for(DSTLI li=_ntuple.begin();li!=_ntuple.end();++li){
 acv[length++]=*li;
}
}
dsts=acv._retn();
return length;
}


void Producer_impl::sendCurrentInfo(const DPS::Client::CID & cid, const  CurrentInfo &ci)throw (CORBA::SystemException){

RLI li=find_if(_rl.begin(),_rl.end(),REInfo_EqsClient(cid));
if(li !=_rl.end()){
   RunEvInfo_var rv=*li; 
   rv->cinfo=ci;
   rv->Status=ci.Status;
    if(ci.Status ==Finished || ci.Status==Failed){
      if(_parent->Debug()){
       _parent->IMessage(AMSClient::print(cid,"sendCurrentInfo from "));
       _parent->IMessage(AMSClient::print(ci));
      }
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





void Producer_impl::sendNtupleEnd(const DPS::Client::CID & ci, const  DST & ne, DPS::Client::RecordChange rc)throw (CORBA::SystemException){
if(_parent->Debug()){
  _parent->IMessage(AMSClient::print(ci,"sendntupleinfo get from "));
  _parent->IMessage(AMSClient::print(ne));
}
 DST_var vne= new DST(ne);
 switch (rc){
 case DPS::Client::Create:
 for(DSTLI li=_ntuple.begin();li!=_ntuple.end();++li){
  if(!strcmp((const char *)(*li)->Name,(const char *)ne.Name)){
   _parent->EMessage(AMSClient::print(*li,"Create:Ntuple Already exists"));
   return;
  }
 }
 _ntuple.push_back(vne);
 break;
 case DPS::Client::Update:
 for(DSTLI li=_ntuple.begin();li!=_ntuple.end();++li){
  if(!strcmp((const char *)(*li)->Name,(const char *)ne.Name)){
   switch ((*li)->Status){
    case InProgress:
     *li=vne;
      return;
    default:
    _parent->EMessage(AMSClient::print(vne,"Update:Ntuple Already Exists "));
    return;
   }
   break;
  }
 }
    _parent->EMessage(AMSClient::print(vne,"Update:Ntuple Not Found "));
 
}
}

void Producer_impl::sendEventStatusEnd(const DPS::Client::CID & ci,const  EventStatusEnd & ne,  DPS::Client::RecordChange rc)throw (CORBA::SystemException){
}


void Producer_impl::PropagateRun(const RunEvInfo & ri, DPS::Client::RecordChange rc, int uid){
  if(_ActivateQueue)_runqueue.push_back(RA(ri,rc));
  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
  DPS::Client::CID cid;
  cid.Type=getType();
  cid.Interface= (const char *) " ";
    DPS::Client::ARS * pars;
    int length=_pser->getARS(cid,pars,uid);
    DPS::Client::ARS_var arf=pars;
  for(int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Producer_var _pvar=DPS::Producer::_narrow(obj);
    _pvar->sendRunEvInfo(ri,rc);
   }
   catch (CORBA::SystemException &ex){
     // Have to Kill Servers Here
      cid.Type=_pser->getType();
   }
  } 

}

void Producer_impl::PropagateNtuple(const DST & ri, DPS::Client::RecordChange rc, int uid){
  if(_ActivateQueue)_ntqueue.push_back(DSTA(ri,rc));
  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
  DPS::Client::CID cid;
  cid.Type=getType();
  cid.Interface= (const char *) " ";
    DPS::Client::ARS * pars;
    int length=_pser->getARS(cid,pars,uid);
    DPS::Client::ARS_var arf=pars;
  for(int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Producer_var _pvar=DPS::Producer::_narrow(obj);
    _pvar->sendNtupleEnd(_parent->getcid(),ri,rc);
   }
   catch (CORBA::SystemException &ex){
   }
  } 

}


void Server_impl::UpdateDB(){
}

void Producer_impl::UpdateDB(){
//just to do something

static bool resultdone=false;
if(!resultdone){
 if(!count_if(_rl.begin(),_rl.end(),REInfo_process())){
  resultdone=true;
  for(RLI li= _rl.begin();li!=_rl.end();++li){
    if((*li)->Status==Finished)_parent->IMessage(AMSClient::print((*li),"Finished :"));
  }
  for(RLI li= _rl.begin();li!=_rl.end();++li){
   if((*li)->Status==Failed)_parent->IMessage(AMSClient::print((*li),"Failed :"));
  }
cout << "Host Status **********"<<endl;
for (AHLI li=_ahl.begin();li!=_ahl.end();++li){
_parent->IMessage(AMSClient::print((*li),"Host "));
}

cout <<"DST status ******" <<endl;
for (DSTLI li=_ntuple.begin();li!=_ntuple.end();++li){
_parent->IMessage(AMSClient::print((*li),"Ntuple "));
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
int length=getARS(cid,pars,_parent->getcid().uid);
DPS::Client::ARS_var arf=pars;
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

    cout <<"kill "<<submit;
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
          return false;
      }
      else{
       _col.erase(li);        
       break;
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
       _parent->EMessage(AMSClient::print(rv, " run Failed "));
       PropagateRun(rv,DPS::Client::Update);
     }
    }

}


bool Server_impl::PingServer(const DPS::Client::ActiveClient & ac){
    DPS::Client::CID cid=_parent->getcid();
    cid.Type=getType();
    cid.Interface= (const char *)" ";
    DPS::Client::ARS * pars;
    int i=-ac.id.uid;
    int length=getARS(cid,pars,i);
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
   catch (CORBA::SystemException &ex){
   }
  } 
  return false;
}


void Server_impl::_PurgeQueue(){
for(ACALI li=_acqueue.begin();li!=_acqueue.end();++li){
if(_parent->Debug())_parent->IMessage(AMSClient::print(li->getacv(),"PurgingACQueue"));
 PropagateAC(li->getacv(),li->getaction(),_RecID);
}
_acqueue.clear();
}

void Producer_impl::_PurgeQueue(){
for(ACALI li=_acqueue.begin();li!=_acqueue.end();++li){
if(_parent->Debug())_parent->IMessage(AMSClient::print(li->getacv(),"PurgingACQueue"));
 PropagateAC(li->getacv(),li->getaction(),_RecID);
}
_acqueue.clear();
for(RALI li=_runqueue.begin();li!=_runqueue.end();++li){
if(_parent->Debug())_parent->IMessage(AMSClient::print(li->getacv(),"PurgingRunQueue"));
 PropagateRun(li->getacv(),li->getaction(),_RecID);
}
_runqueue.clear();
for(DSTALI li=_ntqueue.begin();li!=_ntqueue.end();++li){
if(_parent->Debug())_parent->IMessage(AMSClient::print(li->getacv(),"PurgingNTQueue"));
 PropagateNtuple(li->getacv(),li->getaction(),_RecID);
}
_ntqueue.clear();

}
