#include <stdlib.h>
#include <server.h>
#include <fstream.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <algorithm>
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
    case 'D':    //debuf
     _debug=atoi(++pchar);
     break;
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
    case 'F':   //NominalInterface
     iface=++pchar;
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
if(_debug)_openLogFile("Server");
_getpidhost(uid);
if(iface){
 ifstream fbin;
 fbin.open(iface);
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
   cout <<"qq2 "<<tmpbuf<<endl;
   _orbmap[a]=e;
   cout <<"qq "<<tmpbuf<<endl;
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
  _pser= new Server_impl(_orbmap,this,nserver,nhost);
  if(rfile){  
   _pser->add(new Producer_impl(_orbmap,this,nproducer,rfile,ntuplestring,eventtagstring));
  }  
  
 }
 else{

 try{
   CORBA::Object_var obj=(_orbmap.find("default")->second)._orb->string_to_object(ior);
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
    }
 }
 }
}
 catch(CORBA::SystemException &ex){
   FMessage("AMSServer::AMSServer-F-UnableToInitIOR ",DPS::Client::CInAbort);
  } 
}
  _pser->_init();
  Server_impl * pser=dynamic_cast<Server_impl*>(_pser);
  pser->setInterface(iface);
  pser->StartSelf(_pid);
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
#ifdef __AMSDEBUG__
cout<< " Exiting ...."<<(message?message:" ")<<endl;
#endif
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
       PropagateAC((*j),DPS::Client::Update);
       return;
     }   
     }
}

void AMSServerI::PropagateAC(const DPS::Client::ActiveClient & ac,DPS::Client::RecordChange rc){
  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
  DPS::Client::CID cid;
  cid.Type=_pser->getType();
  cid.Interface= (const char *) " ";
    DPS::Client::ARS * pars;
    int length=_pser->getARS(cid,pars);
    DPS::Client::ARS_var arf=pars;
  for(int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Server_var _pvar=DPS::Server::_narrow(obj);
    _pvar->sendAC(ac.id,ac,rc);
   }
   catch (CORBA::SystemException &ex){
     // Have to Kill Servers Here
     _pser->KillClients(cid);
   }
  } 
  }   


bool AMSServerI::InactiveClientExists(){
for(ACLI li= _acl.begin();li!=_acl.end();++li){
 if((*li)->Status!=DPS::Client::Active)return false;
}
return true;
}


Server_impl::Server_impl(const map<AString, AMSServer::OrbitVars> & mo, AMSClient* parent, char* NS, char * NH): POA_DPS::Server(),AMSServerI(AMSID("Server",0),parent,DPS::Client::Server){

typedef map<AString, AMSServer::OrbitVars>::const_iterator MOI;

Server_impl * pcur =0;

for(MOI i=mo.begin();i!=mo.end();++i){
   if(!pcur)pcur=this;
//   else add(pcur = new Server_impl());
  PortableServer::ObjectId_var oid=(i->second)._poa->activate_object(pcur);
  DPS::Server_var _ref = reinterpret_cast<DPS::Server_ptr>((i->second)._poa->id_to_reference(oid));
   _refmap[i->first]=(i->second)._orb->object_to_string(_ref);
   if(!strcmp((const char *)(i->first),"default")){
    _defaultorb=(i->second)._orb;
   }
}
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
  _parent->FMessage("Server_impl::Server_impl-F-UnableToOpenNSFile ",DPS::Client::CInAbort);
}
}
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
   if(!strcmp((const char *)(i->first),"default")){
    _defaultorb=(i->second)._orb;
   }
}

//Get NominalServer

if(!_pvar->getNC(cid,_ncl))_parent->FMessage("Server-UnsbleToReadNServer",DPS::Client::CInAbort);


//Now Read NominalHost

DPS::Client::NHS * pnhs;
int length=_pvar->getNHS(cid,pnhs);
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

}



void Server_impl::StartClients(const DPS::Client::CID & pid){

if(InactiveClientExists())return;

// Check if no lock

      time_t tt;
      time(&tt);
   for ( list<DPS::Server::CriticalOps>::iterator li=_col.begin();li!=_col.end();++li){
     if (li->Action == StartClient){
       if(tt<(*li).TimeStamp+(*li).TimeOut){
          return;
       }
       else{
        _col.erase(li);        
       }
     }
   }
  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
  DPS::Client::CID cid;
  cid.Type=_pser->getType();
  cid.Interface= (const char *) " ";
  DPS::Server::CriticalOps op;
  op.TimeStamp=tt;
  op.TimeOut=_StartTimeOut;
  op.Type=getType();
  op.Action=StartClient;
    DPS::Client::ARS * pars;
    int length=_pser->getARS(cid,pars);
    DPS::Client::ARS_var arf=pars;
  for(int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Server_var _pvar=DPS::Server::_narrow(obj);
    _pvar->sendCriticalOps(pid,op);
  }
   catch (CORBA::SystemException &ex){
     // Have to Kill Servers Here
     _pser->KillClients(cid);
   }
  } 
 // Check if there are some hosts to run on
 _ahl.sort(Less());
 for(AHLI i=_ahl.begin();i!=_ahl.end();++i){
  if(_acl.size()<_ncl->MaxClients ){
  if((*i)->Status!=NoResponse){
    if((*i)->ClientsRunning>=(*i)->ClientsAllowed)continue;
    // HereStartClient
   CORBA::String_var _refstring=_refmap.find((const char *)((*i)->Interface))->second;
#ifdef __AMSDEBUG__
    cout <<((*i)->Interface)<<" "<<_refstring<<endl;
#endif
    AString submit;
    submit+=(const char*)(_ncl->SubmitCommand);  
    submit+=" ";
    submit+=(const char*)((*i)->HostName);
    submit+=" ";
    submit+=(const char*)(_ncl->LogPath);  
    _Submit++;
    submit+="Server.";
    char tmp[80];
    sprintf(tmp,"%d",_Submit);
    submit+=tmp;
    submit+=".log ";
    submit+=(const char*)(_ncl->WholeScriptPath);  
    submit+=" -";
    submit+=(const char*) _refstring;
    submit+=" -U";
    submit+=tmp;
    submit+=" -F";
    submit+= _iface;
#ifdef __AMSDEBUG__
    submit+=" -D1";
    cout <<submit<<endl;
#endif
    int out=system(submit);
    if(out==0){
     // Add New Active Client
     DPS::Client::ActiveClient ac;
     (ac.id).HostName=CORBA::string_dup((*i)->HostName);
     ac.id.Interface=(*i)->Interface;
     (ac.id).uid=_Submit;
     ac.id.Type=getType();
     ac.Status=DPS::Client::Submitted;
     time_t tt;
     time(&tt);
     ac.LastUpdate=tt;     
     ac.Start=tt;
     (ac.ars).length(1);
/*
     _acl.push_back(ac);
     ((*i)->ClientsRunning)++;
*/
    PropagateAC(ac,DPS::Client::Create);
    }
    else{
     AString a="StartClients-E-UnableToStartClient ";
     a+=submit;
     _parent->EMessage((const char *)a);
    }
   }
  } 
 }  
  op.Action=ClearStartClient;
  for(int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Server_var _pvar=DPS::Server::_narrow(obj);
    _pvar->sendCriticalOps(pid,op);
  }
   catch (CORBA::SystemException &ex){
   }
  }
}

void Server_impl::KillClients(const DPS::Client::CID & cid){
}

void Server_impl::CheckClients(const DPS::Client::CID & cid){
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
   ah.ClientsAllowed=min((*i)->CPUNumber/_ncl->CPUNeeded,(*i)->Memory/float(_ncl->MemoryNeeded));
 
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
       PropagateAC((*j),DPS::Client::Update);
#ifdef __AMSDEBUG__
       cout <<"Server_impl::sendId-I-RegClient "<<cid.uid<<" "<<cid.pid<<" "<<cid.ppid<<endl;
#endif 
       return true;
      }
     }
     return false;



}

CORBA::Boolean Server_impl::getNC(const DPS::Client::CID &cid, DPS::Client::NominalClient_out nc)throw (CORBA::SystemException){
for(AMSServerI * pser=this;pser;pser= pser->next()?pser->next():pser->down()){
if(pser->getType()==cid.Type){
nc= new DPS::Client::NominalClient(_ncl);
return true;
}
}
return false;
}




int Server_impl::getARS(const DPS::Client::CID & cid, DPS::Client::ARS_out  arf)throw (CORBA::SystemException){
DPS::Client::ARS_var arv=new ARS();
int length=0;
for (ACLI li=_acl.begin();li!=_acl.end();++li){
  for (int i=0;i<((*li)->ars).length();i++){
    if( (((*li)->ars)[i]).Type == cid.Type && !strcmp((const char *)  (((*li)->ars)[i]).Interface, (const char *)cid.Interface)){
      length++;
    }
    else if( !strcmp(" ", (const char *)cid.Interface) && (((*li)->ars)[i]).Type == cid.Type && !strcmp((const char *)  (((*li)->ars)[i]).Interface, (const char *)((*li)->id).Interface)){
      length++;
    }
  }
}

if(length==0){ 
  arv->length(1);
  cerr<<"***ERROR*** getars-length== 0"<<endl;
}
else arv->length(length);
length=0;
for (ACLI li=_acl.begin();li!=_acl.end();++li){
  for (int i=0;i<((*li)->ars).length();i++){
    if( (((*li)->ars)[i]).Type == cid.Type && !strcmp((const char *)  (((*li)->ars)[i]).Interface, (const char *)cid.Interface)){
      arv[length++]=((*li)->ars)[i];
    }
    else if( !strcmp(" ", (const char *)cid.Interface) && (((*li)->ars)[i]).Type == cid.Type && !strcmp((const char *)  (((*li)->ars)[i]).Interface, (const char *)((*li)->id).Interface)){
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



   void Server_impl::sendAC(const DPS::Client::CID &  cid, const DPS::Client::ActiveClient & ac, DPS::Client::RecordChange rc)throw (CORBA::SystemException){
  for (AMSServerI* pcur=this;pcur;pcur=pcur->next()?pcur->next():pcur->down()){
    if(pcur->getType()==cid.Type){
      ACLI li=find_if(pcur->getacl().begin(),pcur->getacl().end(),Eqs(ac));
      switch (rc){
       case DPS::Client::Update:
       if(li==pcur->getacl().end())_parent->EMessage("Client not found for editing");
       else{
        DPS::Client::ActiveClient_var vac= new DPS::Client::ActiveClient(ac);
        *li=new DPS::Client::ActiveClient(ac);
       }
       break;
       case DPS::Client::Delete:
       if(li==pcur->getacl().end())_parent->EMessage("Client not found for deleting");
       else{
        pcur->getacl().erase(li);
//      Here find the corr Ahost and update it
         for(AHLI i=pcur->getahl().begin();i!=pcur->getahl().end();++i){
            if(!strcmp((const char *)(*i)->HostName, (const char *)((*li)->id).HostName)){
        ((*i)->ClientsRunning)--;
       ((*i)->ClientsProcessed)++; 
       switch (((*li)->id).Status){
       case DPS::Client::CInExit:
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
       }
       break;
       case DPS::Client::Create:
       if(li !=pcur->getacl().end())_parent->EMessage("Client alreadyexists");
       else{
       pcur->getacl().push_back();
//     Here find the corr ahost and update it
         for(AHLI i=pcur->getahl().begin();i!=pcur->getahl().end();++i){
            if(!strcmp((const char *)(*i)->HostName, (const char *)((*li)->id).HostName)){
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
cout <<"Client "<<cid.uid<<" exits "<<cid.HostName<<endl;
// find and remove client
for( ACLI li=_acl.begin();li!=_acl.end();++li){
 if (cid.uid==((*li)->id).uid){
   (*li)->id.Status=status;
   PropagateAC(*li,DPS::Client::Delete);
   return;
 }

}
 _parent->EMessage("Server_impl::Exiting::No Such Client");
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
      case StartClient: KillClient:
        pcur->getcol().push_back(op);
       break;
      case ClearStartClient: ClearKillClient:
        for (COLI li=pcur->getcol().begin(); li!=pcur->getcol().end();++li){
         if(op.Action == (*li).Action){
          pcur->getcol().erase(li);
          break;
         }
        }
        break;
     }     
    }
    break;
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



void Server_impl::StartSelf(const DPS::Client::CID & cid){
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
       cout <<"  length "<<length <<endl;
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
     else (as.ars).length(1);
   PropagateAC(as,DPS::Client::Create);

}


//------------------------------Producer_impl--------------------------------


Producer_impl::Producer_impl(const map<AString, AMSServer::OrbitVars> & mo, AMSClient* parent,char * NC, char *RF, char *NS, char *TS): POA_DPS::Producer(),AMSServerI(AMSID("Producer",0),parent,DPS::Client::Producer),_RunID(0){

typedef map<AString, AMSServer::OrbitVars>::const_iterator MOI;
Producer_impl * pcur =0;
for(MOI i=mo.begin();i!=mo.end();++i){
   if(!pcur)pcur=this;
//   else add(pcur = new Producer_impl());
 PortableServer::ObjectId_var oid=(i->second)._poa->activate_object(pcur);
  DPS::Producer_var _ref = reinterpret_cast<DPS::Producer_ptr>((i->second)._poa->id_to_reference(oid));
   _refmap[i->first]=((i->second)._orb)->object_to_string(_ref);
   if(!strcmp((const char *)(i->first),"default")){
    _defaultorb=(i->second)._orb;
   }
}
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
 _parent->FMessage("Producer_impl::Producer_impl-F-UnableToOpenNCFile ",DPS::Client::CInAbort);
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
   DPS::Producer::RunEvInfo  re;
   re.uid=_RunID++;
    char tmpbuf[1024];
   fbin>>re.Run>>re.FirstEvent>>re.LastEvent>>re.Priority>>tmpbuf;
   re.FilePath=(const char*)tmpbuf;
   if(re.LastEvent<re.FirstEvent)re.LastEvent=2000000000;
   re.Status=DPS::Producer::ToBeRerun;
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
   if(!strcmp((const char *)(i->first),"default")){
    _defaultorb=(i->second)._orb;
   }
}

//Here Read NominalClient

cid.Type=getType();
if(!_svar->getNC(cid,_ncl))_parent->FMessage("Producer::UnabletogetNClient",DPS::Client::CInAbort);


//Now Read ActiveHost

DPS::Client::AHS * pahs;
int length=_svar->getAHS(cid,pahs);
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

//Get producer_var and read producer specifics

    Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
    cid.Interface= (const char *) " ";
    DPS::Client::ARS * pars;
    length=_pser->getARS(cid,pars);
    DPS::Client::ARS_var arf=pars;
    DPS::Producer_var _pvar=DPS::Producer::_nil();
  for(int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=(mo.find((const char *)arf[i].Interface)->second)._orb->string_to_object(arf[i].IOR);
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



//Now Read Ntuple

DSTS * pdsts;
length=_pvar->getNtupleS(cid,pdsts);
DSTS_var dsts=pdsts;
for(int i=0;i<length;i++){
DST_var vdst= new DST(dsts[i]);
 _ntuple.push_back(vdst);
}


}




void Producer_impl::_init(){
if(_ahl.size())return;
// here init active host list
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
   ah.ClientsAllowed=min((*i)->CPUNumber/_ncl->CPUNeeded,(*i)->Memory/float(_ncl->MemoryNeeded));

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
 else{
  _parent->FMessage("Producer_impl::_init-F-UnableToConnectToServer",DPS::Client::CInAbort);
 }
}





void Producer_impl::StartClients(const DPS::Client::CID & pid){
//if(InactiveClientExists())return;
// Check if no lock
      time_t tt;
      time(&tt);
   for ( list<DPS::Server::CriticalOps>::iterator li=_col.begin();li!=_col.end();++li){
     if ((*li).Action == DPS::Server::StartClient){
       if(tt<(*li).TimeStamp+(*li).TimeOut){
          return;
       }
       else{
        _col.erase(li);        
       }
     }
   }
  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
  DPS::Client::CID cid;
  cid.Type=_pser->getType();
  cid.Interface= (const char *) " ";
  DPS::Server::CriticalOps op;
  op.TimeStamp=tt;
  op.TimeOut=_StartTimeOut;
  op.Type=getType();
  op.Action=DPS::Server::StartClient;
    DPS::Client::ARS * pars;
    int length=_pser->getARS(cid,pars);
    DPS::Client::ARS_var arf=pars;
  for(int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Server_var _pvar=DPS::Server::_narrow(obj);
    _pvar->sendCriticalOps(pid,op);
   }
   catch (CORBA::SystemException &ex){
     // Have to Kill Servers Here
     _pser->KillClients(cid);
   }
  }
 // Check if there are some hosts to run on
 _ahl.sort(Less());
 for(AHLI i=_ahl.begin();i!=_ahl.end();++i){
  if(_acl.size()<_ncl->MaxClients && _acl.size()<_rl.size()){
  if((*i)->Status!=NoResponse){
    if((*i)->ClientsRunning>=(*i)->ClientsAllowed)continue;
    // HereStartClient
   CORBA::String_var _refstring=_refmap.find((const char *)((*i)->Interface))->second;
#ifdef __AMSDEBUG__
    cout <<((*i)->Interface)<<" "<<_refstring<<endl;
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
    cout <<submit<<endl;
#endif
    int out=system(submit);
    if(out==0){
     // Add New Active Client
     DPS::Client::ActiveClient ac;
     (ac.id).HostName=CORBA::string_dup((*i)->HostName);
     ac.id.Interface=(*i)->Interface;
     (ac.id).uid=_Submit;
     ac.id.Type=getType();
     ac.Status=DPS::Client::Submitted;
     time_t tt;
     time(&tt);
     ac.LastUpdate=tt;     
     ac.Start=tt;
     (ac.ars).length(1);
    PropagateAC(ac,DPS::Client::Create);
    }
    else{
     AString a="StartClients-E-UnableToStartClient ";
     a+=submit;
     _parent->EMessage((const char *)a);
    }
   }
  } 
 }

  op.Action=DPS::Server::ClearStartClient;
  for(int i=0;i<length;i++){
  try{
    CORBA::Object_var obj=_defaultorb->string_to_object(arf[i].IOR);
    DPS::Server_var _pvar=DPS::Server::_narrow(obj);
    _pvar->sendCriticalOps(pid,op);
  }
   catch (CORBA::SystemException &ex){
   }
  }
  
}




void Producer_impl::KillClients(const DPS::Client::CID & cid){
}

void Producer_impl::CheckClients(const DPS::Client::CID & cid){
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
       cout <<"Producer_impl::sendId-I-RegClient "<<cid.uid<<" "<<cid.pid<<" "<<cid.ppid<<endl;
       PropagateAC((*j)
,DPS::Client::Update);
       return false;
#endif 
       return true;
      }
     }
     return false;


}


int Producer_impl::getARS(const DPS::Client::CID & cid, DPS::Client::ARS_out arf)throw (CORBA::SystemException){


 Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 

return _pser->getARS(cid, arf);


}




void Producer_impl::Exiting(const CID & cid, const char * message, DPS::Client::ClientExiting status)throw (CORBA::SystemException){
cout <<"Client "<<cid.uid<<" exits "<<cid.HostName<<endl;
// find and remove client
for( ACLI li=_acl.begin();li!=_acl.end();++li){
 if (cid.uid==((*li)->id).uid){
   PropagateAC(*li,DPS::Client::Delete);
   return;
 }

}
 _parent->EMessage("Producer_impl::Exiting::No Such Client");
}






DPS::Producer::TDV* Producer_impl::getTDV(const char * name, DPS::time i, DPS::time b, DPS::time e)throw (CORBA::SystemException){
}

void Producer_impl::sendTDV( const TDV & tdv, const char * name, DPS::time Insert, DPS::time Begin, DPS::time End, int Propagate, int  Error )throw (CORBA::SystemException){
}

DPS::Producer::TDVTable * Producer_impl::getTDVTable(const char * Name, unsigned int Run ,int  Propagate, int Error)throw (CORBA::SystemException){
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
_rl.sort(Less());
RunEvInfo_var rv=new RunEvInfo(); 
RLI li=find_if(_rl.begin(),_rl.end(),REInfo_find(ToBeRerun));
if(li==_rl.end())li=find_if(_rl.begin(),_rl.end(),REInfo_find(Failed));
if(li==_rl.end())rv->DieHard=1;
else {
 rv=*li;
 (*li)->Status=Processing;
 (*li)->cuid=cid.uid;
 PropagateRun(*li, DPS::Client::Update);
}
ro=rv._retn();
_UpdateACT(cid,DPS::Client::Active);
}



void Producer_impl::sendRunEvInfo(const  RunEvInfo & ne, DPS::Client::RecordChange rc)throw (CORBA::SystemException){

 RLI li=find_if(_rl.begin(),_rl.end(),REInfo_Eqs(ne));
 switch (rc){
 case DPS::Client::Update:
  if(li==_rl.end())_parent->EMessage("RunEv not found for editing");
  else *li=new RunEvInfo(ne);
  break;
 case DPS::Client::Delete:
  if(li==_rl.end())_parent->EMessage("runEv not found for deleting");
  else _rl.erase(li);
  break;
 case DPS::Client::Create:
  if(li != _rl.end())_parent->EMessage("Run already exists");
  else {
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

for( RLI li=_rl.begin();li!=_rl.end();++li){
 if((*li)->cuid==cid.uid){
   (*li)->cinfo=ci;
   (*li)->Status=ci.Status;
   PropagateRun(*li,DPS::Client::Update);
   break;
 } 
}


_UpdateACT(cid,DPS::Client::Active);

}





void Producer_impl::sendNtupleEnd(const DPS::Client::CID & ci, const  DST & ne, DPS::Client::RecordChange rc)throw (CORBA::SystemException){
 switch (rc){
 case DPS::Client::Create:
 DST_var vne= new DST(ne);
 _ntuple.push_back(vne);
 break;
 }
}

void Producer_impl::sendEventStatusEnd(const DPS::Client::CID & ci,const  EventStatusEnd & ne,  DPS::Client::RecordChange rc)throw (CORBA::SystemException){
}


void Producer_impl::PropagateRun(const RunEvInfo & ri, DPS::Client::RecordChange rc){
  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 
  DPS::Client::CID cid;
  cid.Type=getType();
  cid.Interface= (const char *) " ";
    DPS::Client::ARS * pars;
    int length=_pser->getARS(cid,pars);
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
     _pser->KillClients(cid);
     //HaveTo reiterate here (later, as killclients will be ready)
   }
  } 

}



