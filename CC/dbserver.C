//  $Id: dbserver.C,v 1.12 2001/02/28 14:44:54 choutko Exp $
//
//  Feb 14, 2001. a.k. ORACLE subroutines from server.C
//  Feb 21, 2001. a.k. unique process identification -> ID+TYPE
//                      
//  Last Edit : Feb 26, 2001. ak
//
#include <amsdbc.h>
#include <dbserver.h>
#ifdef __AMSORACLE__
#include <oracle.h>
#endif



  DBServer_impl::DBServer_impl(const map<AString, AMSServer::OrbitVars> & mo,  const DPS::Client::CID & cid,AMSClient * parent):POA_DPS::DBServer(),AMSServerI(AMSID("DBServer",0),parent,DPS::Client::DBServer){
typedef map<AString, AMSServer::OrbitVars>::const_iterator MOI;
DBServer_impl * pcur =0;
for(MOI i=mo.begin();i!=mo.end();++i){
   if(!pcur)pcur=this;
//   else add(pcur = new DBServer_impl());
 PortableServer::ObjectId_var oid=(i->second)._poa->activate_object(pcur);
  DPS::DBServer_var _ref = reinterpret_cast<DPS::DBServer_ptr>((i->second)._poa->id_to_reference(oid));
   _refmap[i->first]=((i->second)._orb)->object_to_string(_ref);
   if(!strcmp((const char *)(i->first),(const char*)cid.Interface)){
    _defaultorb=(i->second)._orb;
   }
}

// local client

     DPS::Client::ActiveClient_var as=new DPS::Client::ActiveClient();
     as->id= cid;
     as->id.Type=getType();
     as->Status=DPS::Client::Active;
     time_t tt;
     time(&tt);
     as->LastUpdate=tt;     
     as->Start=tt;
    int length=_refmap.size();
    if(length){
     (as->ars).length(length);
    length=0;
      for(  map<AString,CORBA::String_var>::iterator mi=getrefmap().begin();mi!=getrefmap().end();++mi){
        ((as->ars)[length]).Interface=(const char *)(mi->first);
        ((as->ars)[length]).IOR=(const char *)(mi->second);
        ((as->ars)[length]).Type=getType();
        ((as->ars)[length]).uid=as->id.uid;
        length++;
       }
    }
     else {
      (as->ars).length(1);
      ((as->ars)[0]).Interface=(const char *)("Dummy");
      ((as->ars)[0]).IOR=(const char *)(" ");
      ((as->ars)[0]).Type=DPS::Client::Generic;
      ((as->ars)[0]).uid=0;
     }
     _acl.push_back(as);

}

   DBServer_impl::DBServer_impl(const map<AString, AMSServer::OrbitVars> & mo, DPS::Server_ptr _cvar,DPS::Client::CID  cid, AMSClient * parent):POA_DPS::DBServer(),AMSServerI(AMSID("DBServer",0),parent,DPS::Client::DBServer){

typedef map<AString, AMSServer::OrbitVars>::const_iterator MOI;
DBServer_impl * pcur =0;
for(MOI i=mo.begin();i!=mo.end();++i){
   if(!pcur)pcur=this;
//   else add(pcur = new DBServer_impl());
 PortableServer::ObjectId_var oid=(i->second)._poa->activate_object(pcur);
  DPS::DBServer_var _ref = reinterpret_cast<DPS::DBServer_ptr>((i->second)._poa->id_to_reference(oid));
   _refmap[i->first]=((i->second)._orb)->object_to_string(_ref);
   if(!strcmp((const char *)(i->first),(const char*)cid.Interface)){
    _defaultorb=(i->second)._orb;
   }
}


// local client

     DPS::Client::ActiveClient_var as=new DPS::Client::ActiveClient();
     as->id= cid;
     as->id.Type=getType();
     as->Status=DPS::Client::Active;
     time_t tt;
     time(&tt);
     as->LastUpdate=tt;     
     as->Start=tt;
    int length=_refmap.size();
    if(length){
     (as->ars).length(length);
    length=0;
      for(  map<AString,CORBA::String_var>::iterator mi=getrefmap().begin();mi!=getrefmap().end();++mi){
        ((as->ars)[length]).Interface=(const char *)(mi->first);
        ((as->ars)[length]).IOR=(const char *)(mi->second);
        ((as->ars)[length]).Type=getType();
        ((as->ars)[length]).uid=as->id.uid;
        length++;
       }
    }
     else {
      (as->ars).length(1);
      ((as->ars)[0]).Interface=(const char *)("Dummy");
      ((as->ars)[0]).IOR=(const char *)(" ");
      ((as->ars)[0]).Type=DPS::Client::Generic;
      ((as->ars)[0]).uid=0;
     }
     _acl.push_back(as);



//Now Read ActiveClient




ReReadTables(_cvar);


}

//
//  functions to fill with oracle stuff
//  see  examples in ~/AMS/perl/lib/POADBServer.pm
//  cid.Type should be used in general to discriminate between requests 
//  error should manifest via throwing an exception:
//  throw DBProblem((const char*)"Some Error");


#ifdef __AMSORACLE__
 
void  DBServer_impl::_init(){
// connect to ORACLE amsdb 
  AMSoracle::oracle_connect();
  if (_parent -> InitOracle()) {
   AMSoracle::initActiveTables();
   AMSoracle::initProdInfoTable();
   AMSoracle::commit();  
  }
}




  void  DBServer_impl::UpdateDB(bool force=false){
    // here do smth then there is no more runs to work
 }

  int  DBServer_impl::getNK(const DPS::Client::CID &cid, DPS::Client::NCS_out nc)
{
  DPS::Client::CID tcid(cid);
  tcid.Type=DPS::Client::Killer;
  int nclients =  getNC(tcid,nc);
  return nclients;
}

  int  DBServer_impl::getNC(const DPS::Client::CID &cid, DPS::Client::NCS_out nc) 
    //throw (CORBA::SystemException,DPS::DBServer::DBProblem)
{

// get list of Nominal Clients (producers and servers)
   int nclients = 0;
   int rstat    = -1;

   char spath[1024];
   char lpath[1024];
   char scmd[1024];
   char host[40];

   AMSoracle::ProcDesc *procdesc = 0;

   NCS_var acv = new NCS();
   unsigned int type = cid.Type;

   nclients = AMSoracle::getNominalClientN(cid.Type);
   if (nclients > 0) {
     procdesc = new AMSoracle::ProcDesc[nclients];
     nclients = AMSoracle::getNominalClientDesc(cid.Type, procdesc);
     if (nclients > 0) {
      acv -> length(nclients);
       for (int i=0; i<nclients; i++) {
         procdesc[i].get(acv[i].uid,
                         type,
                         acv[i].MaxClients, 
                         acv[i].CPUNeeded,
                         acv[i].MemoryNeeded, 
                         spath, lpath, scmd, host);

         acv[i].WholeScriptPath = (const char *)spath;
         acv[i].LogPath         = (const char *)lpath;
         acv[i].SubmitCommand   = (const char *)scmd;
         acv[i].HostName        = (const char *)host;

         acv[i].Type = cid.Type;
       }
     }
     if (procdesc) delete [] procdesc;
   }
   if (nclients < 1) { 
     acv -> length(1);
     nclients = 0;
     cout<<"DBServer_impl::getNC -W- cannot get NominalProcess information"
         <<"client type = "<<_parent -> CT2string(cid.Type)<<endl;
   }
   nc = acv._retn();
   return nclients;
  }


   int  DBServer_impl::getACS(const DPS::Client::CID &cid, ACS_out acs, unsigned int & maxc){

     unsigned int status = 0;
     unsigned int type   = 0;
     unsigned int reftype   = 0;

     unsigned int clientid = 0;

     char   interface[40];
     char   hostname[40];
     char   ior[1024];
     unsigned int uid;

     AMSoracle::ActiveClientDesc *acdesc = 0;
     AMSoracle::ActiveClientRef  *acref  = 0;
     AMSoracle::ActiveClientId   *acid   = 0;



   ACS_var acv = new ACS();
   maxc = 0;

    int nclients = AMSoracle::getActiveClientN(cid.Type);
    if (nclients > 0) {
     maxc = AMSoracle::getActiveClientNextId(cid.Type);
     acdesc = new AMSoracle::ActiveClientDesc[nclients];
     acid   = new AMSoracle::ActiveClientId[nclients];
     acv -> length(nclients);
     nclients = AMSoracle::getActiveClientDesc(cid.Type, acdesc);
     if (nclients > 0) {
       AMSoracle::getActiveClientId(cid.Type, acid);
       for (int i=0; i<nclients; i++) {
        acdesc[i].get(clientid, 
                        acv[i].LastUpdate, 
                        acv[i].Start,
                        status);
        acv[i].Status = unsi2CS(status);
        acid[i].get(acv[i].id.uid,
                    acv[i].id.pid,
                    acv[i].id.ppid,
                    type,
                    status,
                    interface,
                    hostname);
         acv[i].id.Interface = (const char *)interface;
         acv[i].id.HostName  = (const char *)hostname;
         acv[i].id.Status = unsi2CSE(status);
         acv[i].id.Type = unsi2CT(type);
         int nref = AMSoracle::getActiveClientRefN(clientid);
         acref  = new AMSoracle::ActiveClientRef[nref];
          (acv[i].ars).length(nref);
          AMSoracle::getActiveClientRef(clientid, acref);
          for (int j=0; j<nref; j++) {
            acref[j].get(ior,
                         interface,
                         type,
                         reftype,
                         uid);
             acv[i].ars[j].IOR       = (const char *)ior;
             acv[i].ars[j].Interface = (const char *)interface;
             acv[i].ars[j].Type = unsi2CT(reftype);
             acv[i].ars[j].uid  = uid;
          }
      }
     }
    }
   if (nclients < 1) {
     cout<<"getACS -I- cannot get ActiveProcess information for cid.Type - "<<_parent->CT2string(cid.Type)<<endl;     
     acv -> length(1);
     nclients = 0;
     maxc = 0;
   }
   if (acdesc && acid && acref) {
     delete [] acdesc;
     delete [] acid;
     delete [] acref;
    }

   acs = acv._retn();
   return nclients;
   }

   void  DBServer_impl::sendAC(const DPS::Client::CID &cid,  
              DPS::Client::ActiveClient & ac, DPS::Client::RecordChange rc)
{
     int rstat = 0; 
     bool succ = AMSoracle::findActiveClient(cid.uid, cid.Type);
       switch (rc) 
       {
        case DPS::Client::Create:
          if (!succ ) {
           rstat = AMSoracle::insertActiveClient(cid.uid,
                                                 cid.Type,
                                                 ac.LastUpdate,
                                                 ac.Start,
                                                 ac.Status);
           if (rstat == 1) {
            int reflng = ac.ars.length();
             for (int i=0; i<reflng; i++) {
              rstat = AMSoracle::insertActiveClientRef(
                                             cid.uid, 
                                             cid.Type,
                                             (const char *)ac.ars[i].IOR,
                                             (const char *)ac.ars[i].Interface,
                                             ac.ars[i].Type,
                                             ac.ars[i].uid);
             }
           }
           if (rstat == 1) {
            rstat= AMSoracle::insertActiveClientId(
                                             cid.uid,
                                             ac.id.pid,
                                             ac.id.ppid,
                                             (const char *)ac.id.HostName,
                                             ac.id.Type,
                                             ac.id.Status,
                                             (const char *)ac.id.Interface);
           }
           if (rstat == 1) AMSoracle::commit();
          } else {
            _parent -> EMessage(AMSClient::print(ac,"DBServer_impl::sendAC -client exists. not recreated"));
          }       
         break;
        case DPS::Client::Update:
         if (succ) {
           rstat = AMSoracle::updateActiveClient(cid.uid,
                                            cid.Type,
                                            ac.LastUpdate,
                                            ac.Start,
                                            ac.Status);
           if (rstat == 1) {
             rstat= AMSoracle::deleteActiveClientId(cid.uid, ac.id.Type);
            rstat= AMSoracle::insertActiveClientId(
                                             cid.uid,
                                             ac.id.pid,
                                             ac.id.ppid,
                                             (const char *)ac.id.HostName,
                                             ac.id.Type,
                                             ac.id.Status,
                                             (const char *)ac.id.Interface);
           }
           if (rstat == 1) AMSoracle::commit();
         } else {
            _parent -> EMessage(AMSClient::print(ac,"DBServer_impl::sendAC -client not exists. no update"));
         }
         break;
        case DPS::Client::Delete:
         if (succ) {
           if (AMSoracle::deleteActiveClient(cid.uid, ac.id.Type) == 1) {
            AMSoracle::commit();
          } else {
            _parent -> EMessage(AMSClient::print(ac,"DBServer_impl::sendAC -delete failed"));
           }
         } else {
            _parent -> EMessage(AMSClient::print(ac,"DBServer_impl::sendAC - unknown cid"));
         }
        break;
       }
}

   void  DBServer_impl::sendAH(const DPS::Client::CID &cid,  const DPS::Client::ActiveHost & ah,DPS::Client::RecordChange rc)
{

  int hexist = AMSoracle::findActiveHost((const char*)ah.HostName, cid.Type); 
  switch (rc) {
  case DPS::Client::Delete:
    if (hexist) {
     AMSoracle::deleteActiveHost(cid.Type, (const char*)ah.HostName); 
     AMSoracle::deleteHostActiveClients(cid.Type, (const char*)ah.HostName);
     AMSoracle::commit();
    } else {
     _parent -> EMessage(AMSClient::print(ah,"DBServer_impl::sendAH - unknown host. not delete"));
    }
    break;
  case DPS::Client::Create:
    if (hexist) {
     _parent -> EMessage(AMSClient::print(ah,"DBServer_impl::sendAH - host exists. not create"));
    } else {
     if( AMSoracle::insertActiveHost(
                          cid.Type,
                         (const char*)ah.HostName,
                          ah.Status,
                          ah.ClientsRunning,
                          ah.ClientsAllowed,
                          ah.ClientsProcessed,
                          ah.ClientsFailed,
                          ah.ClientsKilled,
                          ah.LastUpdate) == 1) AMSoracle::commit();
    } 
     break;
  case DPS::Client::Update:
     if( AMSoracle::updateActiveHost(
                          cid.Type,
                         (const char*)ah.HostName,
                          ah.Status,
                          ah.ClientsRunning,
                          ah.ClientsAllowed,
                          ah.ClientsProcessed,
                          ah.ClientsFailed,
                          ah.ClientsKilled,
                          ah.LastUpdate) == 1) AMSoracle::commit();
    break;
  }
}  

   void  DBServer_impl::sendNH(const DPS::Client::CID &cid,  const DPS::Client::NominalHost & ah,DPS::Client::RecordChange rc)
{
     int hexist = AMSoracle::findHost((const char*)ah.HostName);
     switch (rc) {
     case DPS::Client::Delete:
       if (hexist) {
         if (AMSoracle::deleteNominalHost((const char*)ah.HostName) == 1) AMSoracle::commit();
       } else {
         _parent -> EMessage(AMSClient::print(ah,"nominal host not exists, not delete"));
       }
       break;
     case DPS::Client::Create:
       if (hexist) {
         _parent -> EMessage(AMSClient::print(ah,"nominal host exists, not create"));
       } else {
        if (AMSoracle::insertNominalHost((const char*)ah.HostName,
                                         (const char*)ah.Interface,
                                         (const char*)ah.OS,
                                         ah.CPUNumber,
                                         ah.Memory,
                                         ah.Clock) == 1) AMSoracle::commit();
       }
       break;
     case DPS::Client::Update:
       if (hexist) {
        AMSoracle::deleteNominalHost((const char*)ah.HostName);
        if (
            AMSoracle::insertNominalHost((const char*)ah.HostName,
                                         (const char*)ah.Interface,
                                         (const char*)ah.OS,
                                         ah.CPUNumber,
                                         ah.Memory,
                                         ah.Clock) == 1) AMSoracle::commit();
       } else {
         _parent -> EMessage(AMSClient::print(ah,"nominal host not exists, not update"));
       }
       break;
     }
}

   void  DBServer_impl::sendNC(const DPS::Client::CID &cid,  const DPS::Client::NominalClient & nc,DPS::Client::RecordChange rc)
{

     unsigned int clientId = AMSoracle::findNominalClient(nc.uid, nc.Type);
     switch (rc) {
     case DPS::Client::Delete:
       if (clientId) {
         if (AMSoracle::deleteNominalClient(nc.uid, nc.Type) == 1) AMSoracle::commit();
       } else {
         _parent -> EMessage(AMSClient::print(nc,"client not exists, not delete"));
       }
       break;
     case DPS::Client::Create:
       if (clientId) {
         _parent -> EMessage(AMSClient::print(nc,"client exists, not recreated"));
       } else {
        if (AMSoracle::insertNominalClient(
                                         nc.uid,
                                         nc.Type,
                                         nc.MaxClients,
                                         nc.CPUNeeded,
                                         nc.MemoryNeeded,
                                         (const char*)nc.WholeScriptPath,
                                         (const char*)nc.LogPath,
                                         (const char*)nc.SubmitCommand,
                                         (const char*)nc.HostName,
                                         nc.LogInTheEnd) == 1) AMSoracle::commit();
       }
       break;
     case DPS::Client::Update:
       if (!clientId) {
         _parent -> EMessage(AMSClient::print(nc,"client not exists, no update"));
       } else {
        AMSoracle::deleteNominalClient(nc.uid, nc.Type);
        if (AMSoracle::insertNominalClient(
                                         nc.uid,
                                         nc.Type,
                                         nc.MaxClients,
                                         nc.CPUNeeded,
                                         nc.MemoryNeeded,
                                         (const char*)nc.WholeScriptPath,
                                         (const char*)nc.LogPath,
                                         (const char*)nc.SubmitCommand,
                                         (const char*)nc.HostName,
                                         nc.LogInTheEnd) == 1)   AMSoracle::commit();
       }
       break;
     }

}

   void  DBServer_impl::sendNK(const DPS::Client::CID &cid,  const DPS::Client::NominalClient & nc,DPS::Client::RecordChange rc)
{
  DPS::Client::CID tcid(cid);
  tcid.Type=DPS::Client::Killer;
  sendNC(tcid, nc, rc);
}

   CORBA::Boolean  DBServer_impl::getDBSpace(const DPS::Client::CID &cid, const char * path, const char * addpath,DB_out db){
}



   int  DBServer_impl::getNHS(const DPS::Client::CID &cid,NHS_out nhl)
{
  //
  // get list of nominal hosts
  //
  AMSoracle::NominalHost *hostl = 0;
  NHS_var acv = new NHS();

  char hostname[40];
  char interface[40];
  char os[80];

  int nhosts = AMSoracle::getNominalHostN();
  if (nhosts > 0) {
    hostl = new AMSoracle::NominalHost[nhosts];
    nhosts = AMSoracle::getNominalHostList(hostl);
    if (nhosts) {
      acv -> length(nhosts);
      for (int i=0; i<nhosts; i++) {
        hostl[i].get(hostname, interface, os,
                     acv[i].CPUNumber,
                     acv[i].Memory,
                     acv[i].Clock);
        acv[i].HostName = (const char *)hostname;
        acv[i].Interface= (const char *)interface;
        acv[i].OS       = (const char *)os;

      }
      if (hostl) delete [] hostl;
    } else {
      cout<<"DBServer_impl::getNHS -F- cannot get nominal host list"<<endl;      
    }
  } else {
      cout<<"DBServer_impl::getNHS -F- cannot get nominal host list"<<endl;      
  }
  if (nhosts < 1) {
      acv -> length(1);
      nhosts = 0;
  }
  nhl = acv._retn();
  return nhosts;
}

   int  DBServer_impl::getAHS(const DPS::Client::CID &cid,AHS_out ahl)
{
  AMSoracle::ActiveHost *hostl = 0;
  unsigned int status;

  char   hostname[40];
  char   interface[40];

  AHS_var acv= new AHS();


  int nhosts = AMSoracle::getActiveHostN(cid.Type);
  if (nhosts > 0) {
    hostl = new AMSoracle::ActiveHost[nhosts];
    nhosts = AMSoracle::getActiveHostList(cid.Type, hostl);
    if (nhosts > 0) {
      acv -> length(nhosts);
      for (int i=0; i<nhosts; i++) {
        hostl[i].get(
                     hostname, interface,
                    status,
                    acv[i].ClientsRunning,
                    acv[i].ClientsAllowed,
                    acv[i].ClientsProcessed,
                    acv[i].ClientsFailed,
                    acv[i].ClientsKilled,
                    acv[i].LastUpdate,
                    acv[i].Clock);
        acv[i].HostName = (const char*)hostname;
        acv[i].Interface= (const char*)interface;
        acv[i].Status = unsi2HS(status);
      }
    }
  }

  if (nhosts< 1) {
   nhosts = 0;
   acv -> length(1);
  }

  if (hostl) delete [] hostl;

  ahl=acv._retn();
  return nhosts;
}



  int  DBServer_impl::getTDV(const DPS::Client::CID & cid,  TDVName & tdvname, TDVbody_out body)
{

int length=0;
TDVbody_var vbody=new TDVbody();

  int  rstat = -1;
  unsigned int *pdata = 0;
  AMSoracle::TDVrec   *tdv = 0;
  tdvname.Success=false;
  tdv = new AMSoracle::TDVrec;
  tdv -> setname(tdvname.Name, tdvname.DataMC);
  tdv -> utime(tdvname.Entry.Insert, tdvname.Entry.Begin, tdvname.Entry.End);
  rstat = AMSoracle::gettdv(tdv, 1);
  if (rstat != 1) rstat = AMSoracle::gettdv(tdv, 0);
  if (rstat == 1) {
    int nbytes = tdv -> getsize();
    pdata = new unsigned int[nbytes/sizeof(unsigned int)-1];
    if (pdata) {
      if (AMSoracle::gettdvbody(tdv, pdata) == 1) {
        length = nbytes/sizeof(uinteger);
        //        AMSTimeID::_convert(pdata,length);
        cout<<"Insert "<<pdata[length-3]<<endl;
        cout<<"Begin  "<<pdata[length-2]<<endl;
        cout<<"End    "<<pdata[length-1]<<endl;
        length = length - 3; // i/b/e
        tdvname.Success=true;
      }
    }
  } else {
    cout<<"getTDV -E- cannot find TDV for "<<tdvname.Name<<"."<<tdvname.DataMC<<endl;
    cout<<"           i/b/e               "<<tdvname.Entry.Insert<<"/"
        <<tdvname.Entry.Begin<<"/"<<tdvname.Entry.End<<endl;
  }

 if(tdvname.Success){
  vbody->length(length);
  memcpy(vbody->get_buffer(),pdata,length*sizeof(*pdata)-3);
 }
 else{
  vbody->length(1);
 }

  if (pdata) delete [] pdata;
  if (tdv)   delete tdv;

 body=vbody._retn();
 return length;

}

  int  DBServer_impl::getSplitTDV(const DPS::Client::CID & cid, unsigned int & pos, TDVName & tdvname, TDVbody_out body, TransferStatus &st)
{
 st=Continue;

 int length=0;
 TDVbody_var vbody= new TDVbody();

  int rstat = -1;
  unsigned int *pdata = 0;
  AMSoracle::TDVrec   *tdv = 0;
  tdvname.Success=false;
  tdv = new AMSoracle::TDVrec;
  tdv -> setname(tdvname.Name, tdvname.DataMC);
  if (tdvname.Entry.id != 0)     
     tdv -> utime(tdvname.Entry.Insert, tdvname.Entry.Begin, tdvname.Entry.End);
  rstat = AMSoracle::gettdv(tdv, tdvname.Entry.id);
  if (rstat == 1) {
   if (tdvname.Entry.id == 0)     
    tdv -> getutime(tdvname.Entry.Insert, tdvname.Entry.Begin, tdvname.Entry.End);
    int nbytes = tdv -> getsize();
    length = nbytes/sizeof(uinteger);
    cout <<" length "<<length<<endl;;
    vbody->length(length);
      if ( AMSoracle::gettdvbody(tdv, vbody->get_buffer()) == 1) {
        //        AMSTimeID::_convert(pdata,length);
        length = length - 3; // i/b/e
        tdvname.Success=true;
      }
    }
  const int maxs=500000;
 if(tdvname.Success){

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
  length = 0;
  vbody->length(1);
  st=End;
 }

 if (tdv)   delete  tdv;

 body=vbody._retn();
 return length;

}


  void  DBServer_impl::sendTDV(const DPS::Client::CID & cid, const TDVbody & tdv, TDVName & tdvname )
{


  
tdvname.Success=false;
time_t i,b,e;
time_t tminsert, tmbegin, tmend;

i=tdvname.Entry.Insert;
b=tdvname.Entry.Begin;
e=tdvname.Entry.End;

tminsert = i;
tmbegin  = b;
tmend    = e;



  //
  // tdvbody for EventTag aka EVentStatusTable
  // [0] - run number
  // [1] - events
  // [2...2+events-1]              - event numbers
  // [2+inc...2+inc+events-1]      - tag 1
  // [2+2*inc...2+2*inc+events-1]  - event position in file
  // [2+3*inc...2+3*inc+events-1]  - tag 2 !! only for AMS02 Tags
  //
  const int ntagtypes = 2;
  const char *amstdvtag[] = {"EventStatusTable", "EventStatus2Table"};
  const int ntags[ntagtypes] = {1,2};
  const int elem = 2;
  int   nelem;

  AMSoracle::EventTag   *tag = 0;
  AMSoracle::ProdRun    *prun = 0; 
    cout<<"****************sendTDV******************"<<endl;
    cout<<tdvname.Name<<endl;
    cout<<"TDV size "<<tdv.length()<<endl;
    nelem = 0;
    for (int n=0; n<ntagtypes; n++) {
      if (strcmp(tdvname.Name,amstdvtag[n]) == 0) nelem = elem + ntags[n];
    }
    if (nelem) {
     int N = (tdv.length() - 3);
     int inc = N/nelem;
     uinteger trun    = tdv[0]; // run number
     integer  tidx    = 0;
     integer  events  = tdv[1];  // number of events
     uinteger fevent  = -1;
     uinteger levent  = -1;
     integer  tevents = -1;
     unsigned int tstamp = 0;
     unsigned int tfirst;
     unsigned int tlast;
     int doinsert = 0;
     if (events) {
      fevent = tdv[2];
      levent = tdv[2+events-1];
      int Nr = 0;
      prun = new AMSoracle::ProdRun[100];
      if(AMSoracle::getprodruns(trun, Nr, prun) == 1) {
        if (Nr) {
        for (int r=0; r<Nr; r++) {
          cout<<"sendTDV -I- found run "<<endl;
          cout<<"run, events, first/last event "
              <<trun<<", "<<prun[r].events<<" "<<prun[r].fevent<<"/"<<prun[r].levent<<endl;
          cout<<"inserted "<<prun[r].timestamp<<endl;
          if (prun[r].fevent >= fevent && prun[r].levent <= levent) {
            AMSoracle::deleteProdRun(prun[r].idx);    // delete run from m_prodruns
            AMSoracle::deletetags(prun[r].idx);   // and all assoc. tags from m_tags
            AMSoracle::commit();
            cout<<"sendTDV -I- RUN "<<trun<<" AND ALL TAGS ARE DELETED !!!"<<endl;
          }
        }
       }
       doinsert = 1;
      }
      if (prun) delete [] prun;
      if (doinsert) {
         prun = new AMSoracle::ProdRun;
         if (prun) {
          tstamp = 0;
          prun -> set(trun,events,fevent,levent,tstamp,0);
          prun -> settime(tminsert,tmbegin,tmend);
          tidx = AMSoracle::insertProdRun(prun);
          if (tidx) AMSoracle::commit();
          delete prun;
         } else {
           cout<<"sendTDV -I- failed to create AMSoracle::ProdRun *prun"<<endl;
         }
        tag = new AMSoracle::EventTag[events];
        if (nelem == 3) {
         for (int i=0; i<events; i++) {
          int ii = 2+i;
          tag[i].set(trun, tidx, tdv[ii], tdv[ii+2*inc], tdv[ii+inc],0);
         }
        }
        else
        {
         for (int i=0; i<events; i++) {
          int ii = 2+i;
          tag[i].set(trun, tidx, tdv[ii], tdv[ii+2*inc], tdv[ii+inc],tdv[ii+inc*3]);
         }
        } 
        AMSoracle::inserttags(tag, events);
        if (tag) delete [] tag;
        // done in inserttags - AMSoracle::commit();
       }
     }
    }
}



  int  DBServer_impl::getTDVTable(const DPS::Client::CID & cid,TDVName & tdvname, unsigned int runid, TDVTable_out table){
    // run = runid

 const int defTDVfound = 777;
 const int maxTDVnamelength = 256;
 
 char tname[maxTDVnamelength];

 // static  long utimef;
 // static  long utimel;

 long utimef;
 long utimel;

 int  ntdvs  = 0;
 int  length = 0;

 AMSoracle::TDVutime *tdvutime=0;
 TDVTable_var vtable;

 unsigned int run = 0;

 // static unsigned int oldrun = 0;

 tdvname.Success=false;  
 int rstat = 0;

   run = AMSoracle::findrun(runid);
   if (!run) {
     cout<<"DBServer_impl::getTDVTable -F- Run with given ID not found"<<endl; 
// get time of first and last event of 'run'
   } else {
    rstat = AMSoracle::findrun(run, utimef, utimel);
    if (rstat > 0) {
     tdvname.Success=true;  

// check ORACLE table for TDV with tdvname
    if ((const char *)tdvname.Name && strlen((const char *)tdvname.Name) < maxTDVnamelength) { 
     strcpy(tname,(const char *)tdvname.Name);
     ntdvs = AMSoracle::counttdv(tname, tdvname.DataMC, utimef, utimel);
     if (ntdvs < 1) {
      ntdvs  = 0;
     } else {
      tdvutime = new AMSoracle::TDVutime[ntdvs];
      if (AMSoracle::findtdv(tname, tdvname.DataMC, utimef, utimel, ntdvs, tdvutime) == 1) {
       if (ntdvs > 1) {
        if (AMSoracle::trimtdv(ntdvs,tdvutime) != 1) cout<<"getTDVTable -W- trimtdv failed"<<endl;
       }
      } else {
       ntdvs = 0;
      }
     }
    }
  } else {
   cout<<"getTDVTable -E- run "<<run<<" not found"<<endl;
  }
 }
 vtable=new TDVTable();
 if ( tdvname.Success && ntdvs) {
   vtable->length(ntdvs);
    for( int i=0; i<ntdvs; i++){
      vtable[i].id=tdvutime[i].insert;
      vtable[i].Insert=tdvutime[i].insert;
      vtable[i].Begin=tdvutime[i].begin;
      vtable[i].End=tdvutime[i].end;
    }
 }
 else{
   ntdvs=0;
   vtable->length(1);
 }

 if (tdvutime) delete [] tdvutime;

 table= vtable._retn();
 return ntdvs;

  }


  int  DBServer_impl::getDSTInfoS(const DPS::Client::CID &cid, DSTIS_out res)
{
  DSTIS_var acv= new DSTIS();
  AMSoracle::DSTInfo *dstinfo = 0;
  
  unsigned int mode;
  char     hostname[40];
  char     dirpath[1024];
   
  int ndsts = AMSoracle::getDSTInfoN();
  if (ndsts) {
    dstinfo = new AMSoracle::DSTInfo[ndsts];
    int n = AMSoracle::getDSTInfo(dstinfo);
    if (n > 0) {
      if (n < ndsts) ndsts = n;
      acv->length(ndsts);
      for (int i=0; i<ndsts; i++) {
        dstinfo[i].get(
                       acv[i].uid,
                       hostname,
                       dirpath,
                       mode,
                       acv[i].UpdateFreq,
                       acv[i].DieHard,
                       acv[i].FreeSpace,
                       acv[i].TotalSpace);
        acv[i].Mode = unsi2RM(mode);
        acv[i].HostName = (const char*)hostname;
        acv[i].OutputDirPath = (const char*)dirpath;
      }
    }
    if (dstinfo) delete [] dstinfo;
  } else {
    acv->length(1);
  }
  res=acv._retn();
  return ndsts;

}

  int  DBServer_impl::getRun(const DPS::Client::CID &cid, const FPath & fpath, RUN_out run,TransferStatus & st)throw (CORBA::SystemException,DPS::DBServer::FailedOp){
    //dummy
}

  int  DBServer_impl::getRunEvInfoS(const DPS::Client::CID &cid, RES_out res, unsigned int & maxrun)
{
  int nruns;
  unsigned int status;
  unsigned int history;
  unsigned int uid;
  char   filepath[1024];
  char   hostname[40];

  AMSoracle::ProdRun *ptable = 0;
  AMSoracle::RunTable *rtable = 0;

  RES_var acv = new RES();

  nruns =  AMSoracle::getRunsN();
  if (nruns > 0) {
   rtable = new AMSoracle::RunTable[nruns];
   if (AMSoracle::getRunTable(nruns, maxrun, rtable) == 1) {
    acv -> length(nruns);
     ptable = new AMSoracle::ProdRun;
     for (int i=0; i<nruns; i++) {
       rtable[i].get(acv[i].uid,
                    acv[i].Run,
                    acv[i].FirstEvent,
                    acv[i].LastEvent,
                    acv[i].TFEvent,
                    acv[i].TLEvent,
                    acv[i].Priority,
                    filepath,
                    status,
                    history,
                    acv[i].SubmitTime,
                    acv[i].cuid);
       //
       acv[i].LastEvent = acv[i].LastEvent/200;
       if (acv[i].LastEvent < 100) acv[i].LastEvent = 100;
       //
      acv[i].FilePath = (const char*)filepath;
      acv[i].Status  = unsi2RS(status);
      acv[i].History = unsi2RS(history);
      if(AMSoracle::getProdRunStat(acv[i].Run, ptable) == 1) {
       ptable -> getInfo(acv[i].cinfo.Run,
                         uid,
                         acv[i].cinfo.EventsProcessed,
                         acv[i].cinfo.LastEventProcessed,
                         acv[i].cinfo.CriticalErrorsFound,
                         acv[i].cinfo.ErrorsFound,
                         acv[i].cinfo.CPUTimeSpent,
                         acv[i].cinfo.TimeSpent,
                         status,
                         hostname);
       acv[i].cinfo.Status = unsi2RS(status);
       acv[i].cinfo.HostName = (const char*)hostname;
      }
     }
   } else {
    acv -> length(1);
    nruns = 1;
   }
  } else {
    acv -> length(1);
    nruns = 1;
  }


 if(rtable) delete [] rtable;
 if(ptable) delete ptable;

  res=acv._retn();
  return nruns;
}

   void  DBServer_impl::getRunEvInfo(const DPS::Client::CID &cid, RunEvInfo_out rv, DSTInfo_out dv)
{
// get next run 
  unsigned int status;
  unsigned int history;
  unsigned int mode;
  char     hostname[40];
  char     dirpath[1024];
   
  AMSoracle::RunTable *rtable = new AMSoracle::RunTable;
  AMSoracle::DSTInfo *dstinfo = 0;

  RunEvInfo_var av=new RunEvInfo();
  DSTInfo_var dav =new DSTInfo();

  int ndsts = AMSoracle::getDSTInfoN((const char*)cid.HostName);
  if (ndsts == 1) {
    dstinfo = new AMSoracle::DSTInfo;
    int n = AMSoracle::getDSTInfo((const char*)cid.HostName, dstinfo);
    if (n > 0) {
        dstinfo -> get(
                       dav -> uid,
                       hostname,
                       dirpath,
                       mode,
                       dav -> UpdateFreq,
                       dav -> DieHard,
                       dav -> FreeSpace, 
                       dav -> TotalSpace);

        dav -> Mode = unsi2RM(mode);
        dav -> HostName = (const char*)hostname;
        dav -> OutputDirPath = (const char*)dirpath;
    }
    if (dstinfo) delete dstinfo;
  } else if (ndsts < 1){
      _parent -> EMessage(AMSClient::print(cid,"DBServer_impl::getRunEvInfo - DSTInfo not found. "));
  } else if (ndsts > 1) {
      _parent -> EMessage(AMSClient::print(cid,"DBServer_impl::getRunEvInfo - more than 1 DSTInfo. "));
  } 
  if(AMSoracle::getRun((const char*)cid.HostName,rtable) == 1) {
      rtable -> get(av -> uid,
                    av -> Run,
                    av -> FirstEvent,
                    av -> LastEvent,
                    av -> TFEvent,
                    av -> TLEvent,
                    av -> Priority,
                    dirpath, 
                    status,
                    history,
                    av -> SubmitTime,
                    av -> cuid);
      av -> FilePath = (const char*)dirpath;
      av -> Status  = unsi2RS(status);
      av -> History = unsi2RS(history);

      unsigned int insert = (time_t)0;
      AMSoracle::setRunStatus(av->uid, DPS::Producer::Processing);
      AMSoracle::setRunSubmitTime(av -> uid, insert);
      AMSoracle::commit();
      dav->DieHard=0;
  } else {
    dav->DieHard=1;
  }

  if (rtable) delete rtable;

  rv = av._retn();
  dv = dav._retn();
}


  int  DBServer_impl::getDSTS(const DPS::Client::CID & ci, DSTS_out dsts)
{
  AMSoracle::DST *dst = 0;
  unsigned int status;
  unsigned int type;
  char name[1024];

  DSTS_var acv= new DSTS();
  int ndsts = AMSoracle::getDSTN();
  if (ndsts) {
    dst = new AMSoracle::DST[ndsts];
    int n = AMSoracle::getDST(dst);
    if (n > 0) {
      if (n<ndsts) ndsts = n;
      acv->length(ndsts);
      for (int i=0; i<ndsts; i++) {
        dst[i].get(
                   dsts[i].Insert,
                   dsts[i].Begin,
                   dsts[i].End,
                   dsts[i].Run,
                   dsts[i].FirstEvent,
                   dsts[i].LastEvent,
                   dsts[i].EventNumber,
                   status,
                   type,
                   dsts[i].size,
                   name);
        dsts[i].Status = unsi2DSTS(status);
        dsts[i].Type   = unsi2DSTT(type);
        dsts[i].Name = (const char*)name;
      }
    }
    if (dst) delete [] dst; 
 } else {
    acv->length(1);
  }
  dsts=acv._retn();
  return ndsts;
}

   int  DBServer_impl::getFreeHost(const DPS::Client::CID & ci, DPS::Client::ActiveHost_out host)
{
  int rstatus = 0;
  AMSoracle::ActiveHost *hostl = 0;
  unsigned int status;

  char hostname[40];
  char interface[40];

    DPS::Client::ActiveHost_var ahl = new DPS::Client::ActiveHost();

    hostl = new AMSoracle::ActiveHost;
    if (AMSoracle::getHost(ci.Type, hostl) == 1) {
        
        hostl -> get(
                    hostname,
                    interface,
                    status,
                    ahl -> ClientsRunning,
                    ahl -> ClientsAllowed,
                    ahl -> ClientsProcessed,
                    ahl   -> ClientsFailed,
                    ahl   -> ClientsKilled,
                    ahl   -> LastUpdate,
                    ahl   -> Clock);
        ahl -> HostName = (const char*)hostname;
        ahl -> Interface= (const char*)interface;
        ahl -> Status = unsi2HS(status);
        rstatus = 1;
        if (ci.Type == DPS::Client::Producer) {
          int nruns = AMSoracle::RunsToBeRerun();
          int nactiveclients = AMSoracle::getActiveClientN(ci.Type);
          if (nactiveclients >= nruns) rstatus = 0;
        }
    } else {
      // cout<<"DBServer_impl::getFreeHost -I- do not start client with id, type = "
      //   <<ci.uid<<", "<<ci.Type<<endl;
    }
    if (hostl) delete hostl;

    host=ahl._retn();
    return rstatus;
}

  void  DBServer_impl::sendDSTEnd(const DPS::Client::CID & ci, const  DST & ne, DPS::Client::RecordChange rc)
{
  int dste = AMSoracle::findDST((const char*)ne.Name, ne.Type);

  switch (rc){
   case DPS::Client::Create:
     if (dste) {
      _parent -> EMessage(AMSClient::print(ne,"DBServer_impl::sendDSTEnd - DST exists. not recreated"));
     }  else {
     if (AMSoracle::insertDST(
                              ne.Insert,
                              ne.Begin,
                              ne.End,
                              ne.Run,
                              ne.FirstEvent,
                              ne.LastEvent,
                              ne.EventNumber,
                              ne.Status,
                              ne.Type,
                              ne.size,
                              (const char*)ne.Name) == 1) AMSoracle::commit();
     }
     break;
   case DPS::Client::Update:
     if (!dste) {
      _parent -> EMessage(AMSClient::print(ne,"DBServer_impl::sendDSTEnd - DST not exists. no update"));
     }  else {
      if (AMSoracle::updateDST(
                              ne.Insert,
                              ne.Begin,
                              ne.End,
                              ne.Run,
                              ne.FirstEvent,
                              ne.LastEvent,
                              ne.EventNumber,
                              ne.Status,
                              ne.Type,
                              ne.size,
                              (const char*)ne.Name) == 1) AMSoracle::commit();
     }
     break;
   case DPS::Client::Delete:
     if (dste) {
       if (AMSoracle::deleteDST((const char*)ne.Name, ne.Type) == 1) AMSoracle::commit();
     } else {
      _parent -> EMessage(AMSClient::print(ne,"DBServer_impl::sendDSTEnd - DST not exists. not deleted"));
     }
     break;
  }
}

  void  DBServer_impl::sendDSTInfo(const  DSTInfo & ne,DPS::Client::RecordChange rc)
{

  int dstinfo = AMSoracle::findDSTInfo(ne.uid);
  
  switch (rc){
   
   case DPS::Client::Delete:
     if (dstinfo) {
       if (AMSoracle::deleteDSTInfo(ne.uid) == 1) AMSoracle::commit();
     } else {
      _parent -> EMessage(AMSClient::print(ne,"DBServer_impl::sendDSTInfo - DSTinfo not exists. not deleted"));
     }
     break;
   case DPS::Client::Create:
     if (dstinfo) {
      _parent -> EMessage(AMSClient::print(ne,"DBServer_impl::sendDSTInfo - DSTinfo exists. not recreated"));
     }  else {
     if (AMSoracle::insertDSTInfo(ne.uid,
                                 (const char *)ne.HostName,
                                 (const char *)ne.OutputDirPath,
                                 ne.Mode,
                                 ne.UpdateFreq,
                                 ne.DieHard,
                                 ne.FreeSpace,
                                 ne.TotalSpace) == 1) AMSoracle::commit();
     }
     break;
   case DPS::Client::Update:
     if (!dstinfo) {
      _parent -> EMessage(AMSClient::print(ne,"DBServer_impl::sendDSTInfo - DSTinfo not exists. no update"));
     }  else {
      if (AMSoracle::updateDSTInfo(ne.uid,
                                   (const char *)ne.HostName,
                                   (const char *)ne.OutputDirPath,
                                   ne.Mode,
                                   ne.UpdateFreq,
                                   ne.DieHard,
                                   ne.FreeSpace,
                                   ne.TotalSpace) == 1) AMSoracle::commit();
     }
     break;
  }
}

  void  DBServer_impl::sendRunEvInfo(const  RunEvInfo & ne,DPS::Client::RecordChange rc)
{
  unsigned int run = ne.Run;
  unsigned int uid = ne.uid;
  char     filepath[1024];

  int runfound = AMSoracle::findRun(run, uid);
  AMSoracle::RunTable *rtable = 0;
  AMSoracle::ProdRun  *prun   = 0;
  switch(rc) {
  case DPS::Client::Create:
    if (!runfound) {
      strcpy(filepath, ne.FilePath);
      rtable = new AMSoracle::RunTable;
      rtable -> set(ne.Run, ne.uid, ne.FirstEvent, ne.LastEvent, ne.TFEvent,
                    ne.TLEvent, ne.Priority, 
                    filepath, 
                    ne.Status, ne.History, ne.SubmitTime, ne.cuid);
      AMSoracle::insertRunTable(rtable,1);
      prun = new AMSoracle::ProdRun;
      prun -> updateInfo(ne.Run, 
                         ne.uid,
                         ne.cinfo.EventsProcessed,
                         ne.cinfo.LastEventProcessed,
                         ne.cinfo.CriticalErrorsFound,
                         ne.cinfo.ErrorsFound,
                         ne.cinfo.CPUTimeSpent,
                         ne.cinfo.TimeSpent,
                         ne.cinfo.Status,
                         (const char *)ne.cinfo.HostName);
      if (AMSoracle::updateProdTable(prun) == 1)  AMSoracle::commit();
    } else {
      _parent -> EMessage(AMSClient::print(ne,"Run exists. not recreated"));
    }
    break;
  case DPS::Client::Update:
    if (runfound) {
      strcpy(filepath, ne.FilePath);
      rtable = new AMSoracle::RunTable;
      rtable -> set(ne.Run, ne.uid, ne.FirstEvent, ne.LastEvent, ne.TFEvent,
                    ne.TLEvent, ne.Priority, 
                    filepath, 
                    ne.Status, ne.History, ne.SubmitTime, ne.cuid);
      AMSoracle::updateRunTable(rtable);
      prun = new AMSoracle::ProdRun;
      prun -> updateInfo(ne.Run, 
                         ne.uid,
                         ne.cinfo.EventsProcessed,
                         ne.cinfo.LastEventProcessed,
                         ne.cinfo.CriticalErrorsFound,
                         ne.cinfo.ErrorsFound,
                         ne.cinfo.CPUTimeSpent,
                         ne.cinfo.TimeSpent,
                         ne.cinfo.Status,
                         (const char *)ne.cinfo.HostName);

      if (AMSoracle::updateProdTable(prun) == 1)  AMSoracle::commit();
    } else {
      _parent -> EMessage(AMSClient::print(ne,"Run not exists. no update"));
    }
    break;                               
  case DPS::Client::Delete:
    if (runfound) {
     AMSoracle::deleteRun(run, uid);
     AMSoracle::deleteProdRun(run, uid);
     AMSoracle::deleteProdInfo(run, uid);
     AMSoracle::commit();
    } else {
      _parent -> EMessage(AMSClient::print(ne,"Run not exists. no delete"));
    }

    break;
  }
      if(rtable) delete rtable;
      if(prun)   delete prun;
}

    void DBServer_impl::clearACS( DPS::Client::ClientType type)
{
        AMSoracle::deleteActiveClient(type);
        AMSoracle::commit();
    }

    void DBServer_impl::clearNCS( DPS::Client::ClientType type)
{
       AMSoracle::deleteNominalClient(type);
       AMSoracle::commit();
     }

    void DBServer_impl::clearAHS( DPS::Client::ClientType type)
{
     AMSoracle::deleteActiveHost(type); 
     AMSoracle::commit();
}

    void DBServer_impl::clearNHS()
{
   AMSoracle::deleteNominalHosts();
   AMSoracle::commit();
}
    void DBServer_impl::clearDST( DSTType type)
{
     AMSoracle::deleteDST(type);
     AMSoracle::commit();
}
    void DBServer_impl::clearDSTI()
{
     AMSoracle::deleteDSTInfo();
     AMSoracle::commit();
}

    void DBServer_impl::clearRunEvInfo( RunStatus status)
{
  AMSoracle::deleteRunEvInfo(status);
  AMSoracle::commit();
}







// Dummy functions provided dbservershares same proc with server


 bool  DBServer_impl::Master(){

    Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 

    return _pser->Master();


 }




 int  DBServer_impl::getARS(const DPS::Client::CID & cid, DPS::Client::ARS_out arf, DPS::Client::AccessType type=DPS::Client::Any,uinteger id=0, int selffirst){

     Server_impl* _pser=dynamic_cast<Server_impl*>(getServer()); 

     return _pser->getARS(cid, arf,type,id,selffirst);


  }


 void  DBServer_impl::StartClients(const DPS::Client::CID &cid){
}
 void  DBServer_impl::CheckClients(const DPS::Client::CID &cid){
 }
 void  DBServer_impl::KillClients(const DPS::Client::CID &cid){
 }
  CORBA::Boolean  DBServer_impl::sendId(DPS::Client::CID & cid, uinteger timeout){
}

  void  DBServer_impl::getId(DPS::Client::CID_out cid){
  }

 
  void  DBServer_impl::Exiting(const DPS::Client::CID& cid,const char * Error, DPS::Client::ClientExiting  Status){
}




// Dummy Functions








  char * DBServer_impl::getDBFilePath(const DPS::Client::CID &cid){
   return CORBA::string_dup("Oracle");
}


  int  DBServer_impl::getRunEvInfoSPerl(const DPS::Client::CID &cid, RES_out res, unsigned int  maxrun, unsigned int & m2){
}


   void  DBServer_impl::sendACPerl(const DPS::Client::CID &cid, const  DPS::Client::ActiveClient & ac,DPS::Client::RecordChange rc){
}




   int  DBServer_impl::getEnv(const DPS::Client::CID &cid, SS_out ss){
}

   void  DBServer_impl::setEnv(const DPS::Client::CID &cid,const char * env, const char *path){
}



  int  DBServer_impl::sendFile(const DPS::Client::CID &cid, const FPath & fpath, const  RUN & file,TransferStatus & st)throw (CORBA::SystemException,DPS::DBServer::FailedOp){
}


void DBServer_impl::sendTDVUpdate(const DPS::Client::CID & cid,  const TDVName & tdvname ){
}





  void  DBServer_impl::sendCurrentInfo(const DPS::Client::CID & cid, const  CurrentInfo &ci, int propagate){
}





   void  DBServer_impl::ping()throw (CORBA::SystemException){
}

  CORBA::Boolean  DBServer_impl::TypeExists(DPS::Client::ClientType type){
}

DPS::Producer::RunStatus DBServer_impl::unsi2RS(unsigned int a){
  switch (a){
  case 0 : 
    return DPS::Producer::ToBeRerun;
  case 1 :
    return DPS::Producer::Failed;
  case 2 :
    return DPS::Producer::Processing;
  case 3 :
    return DPS::Producer::Finished;
  }
  return DPS::Producer::Unknown;
}


DPS::Client::ClientType  DBServer_impl::unsi2CT(unsigned int a){
  switch (a){
  case 0 : 
    return DPS::Client::Generic;
  case 1 :
    return DPS::Client::Server;
  case 2 :
    return DPS::Client::Consumer;
  case 3 :
    return DPS::Client::Producer;
  case 4 :
    return DPS::Client::Monitor;
  case 5 :
    return DPS::Client::DBServer;
  case 6 :
    return DPS::Client::ODBServer;
  case 7 :
    return DPS::Client::Killer;
  }
  return DPS::Client::Generic;
}

DPS::Client::ClientStatus DBServer_impl::unsi2CS(unsigned int a){
  switch (a){
  case 0 :
    return DPS::Client::Lost;
  case 1:
    return DPS::Client::Unknown;
  case 2 :
    return DPS::Client::Submitted;
  case 3 :
    return DPS::Client::Registered;
  case 4 :
    return DPS::Client::Active;
  case 5 :
    return DPS::Client::TimeOut;
  case 6 :
    return DPS::Client::Killed;
  }
  return DPS::Client::Unknown;
}

DPS::Client::ClientExiting DBServer_impl::unsi2CSE(unsigned int  a)
{
  switch (a){
  case 0 :
    return DPS::Client::NOP;
  case 1 :
    return DPS::Client::CInExit;
  case 2 :
    return DPS::Client::SInExit;
  case 3 :
    return DPS::Client::CInAbort;
  case 4 :
    return DPS::Client::SInAbort;
  case 5  :
    return DPS::Client::SInKill;
  }
  return DPS::Client::NOP;
}

DPS::Client::HostStatus DBServer_impl::unsi2HS(unsigned int a){
  switch (a){
  case 0 :
    return DPS::Client::NoResponse;
  case 1:
    return DPS::Client::InActive;
  case 2 :
    return DPS::Client::LastClientFailed;
  case 3 :
    return DPS::Client::OK;
  }
}

DPS::Producer::RunMode DBServer_impl::unsi2RM(unsigned int a){
  switch (a){
  case 0 :
    return DPS::Producer::RILO;
  case 1:
    return DPS::Producer::LILO;
  case 2 :
    return DPS::Producer::RIRO;
  case 3 :
    return DPS::Producer::LIRO;
  }
}

DPS::Producer::DSTStatus DBServer_impl::unsi2DSTS(unsigned int a){
  switch (a){
  case 0 :
    return DPS::Producer::Success;
  case 1:
    return DPS::Producer::InProgress;
  case 2 :
    return DPS::Producer::Failure;
  }
}

DPS::Producer::DSTType DBServer_impl::unsi2DSTT(unsigned int a){
  switch (a){
  case 0 :
    return DPS::Producer::Ntuple;
  case 1:
    return DPS::Producer::EventTag;
  case 2 :
    return DPS::Producer::RootFile;
  }
}
#else
void  DBServer_impl::_init(){
  // here oracle connect
}




void  DBServer_impl::UpdateDB(bool force=false){
  // here do smth then there is no more runs to work
}

int  DBServer_impl::getNC(const DPS::Client::CID &cid, DPS::Client::NCS_out nc){
}

int  DBServer_impl::getNK(const DPS::Client::CID &cid, DPS::Client::NCS_out nc){
}


int  DBServer_impl::getACS(const DPS::Client::CID &cid, ACS_out acs, unsigned int & maxc){
}

void  DBServer_impl::sendAC(const DPS::Client::CID &cid,  DPS::Client::ActiveClient & ac,DPS::Client::RecordChange rc){
}

void  DBServer_impl::sendAH(const DPS::Client::CID &cid,  const DPS::Client::ActiveHost & ah,DPS::Client::RecordChange rc){
}

void  DBServer_impl::sendNH(const DPS::Client::CID &cid,  const DPS::Client::NominalHost & ah,DPS::Client::RecordChange rc){
}

void  DBServer_impl::sendNC(const DPS::Client::CID &cid,  const DPS::Client::NominalClient & nc,DPS::Client::RecordChange rc){
}

void  DBServer_impl::sendNK(const DPS::Client::CID &cid,  const DPS::Client::NominalClient & nc,DPS::Client::RecordChange rc){
}

CORBA::Boolean  DBServer_impl::getDBSpace(const DPS::Client::CID &cid, const char * path, const char * addpath,DB_out db){
}

int  DBServer_impl::getNHS(const DPS::Client::CID &cid,NHS_out nhl){
}

int  DBServer_impl::getAHS(const DPS::Client::CID &cid,AHS_out ahl){
  //throw DBProblem((const char*)"Some Error");

}



int  DBServer_impl::getTDV(const DPS::Client::CID & cid,  TDVName & tdvname, TDVbody_out body){
}

int  DBServer_impl::getSplitTDV(const DPS::Client::CID & cid, unsigned int & pos, TDVName & tdvname, TDVbody_out body, TransferStatus &st){
}

void  DBServer_impl::sendTDV(const DPS::Client::CID & cid, const TDVbody & tdv, TDVName & tdvname ){
}



int  DBServer_impl::getTDVTable(const DPS::Client::CID & cid,TDVName & tdvname, unsigned int run, TDVTable_out table){
}


int  DBServer_impl::getDSTInfoS(const DPS::Client::CID &cid, DSTIS_out res){
}

int  DBServer_impl::getRun(const DPS::Client::CID &cid, const FPath & fpath, RUN_out run,TransferStatus & st)throw (CORBA::SystemException,DPS::DBServer::FailedOp){
}
int  DBServer_impl::getRunEvInfoS(const DPS::Client::CID &cid, RES_out res, unsigned int & maxrun){




}

void  DBServer_impl::getRunEvInfo(const DPS::Client::CID &cid, RunEvInfo_out rv, DSTInfo_out dv){



}


int  DBServer_impl::getDSTS(const DPS::Client::CID & ci, DSTS_out dsts){
}
int  DBServer_impl::getFreeHost(const DPS::Client::CID & ci, DPS::Client::ActiveHost_out host){
}



void  DBServer_impl::sendDSTEnd(const DPS::Client::CID & ci, const  DST & ne, DPS::Client::RecordChange rc){
}

void  DBServer_impl::sendDSTInfo(const  DSTInfo & ne,DPS::Client::RecordChange rc){
}

void  DBServer_impl::sendRunEvInfo(const  RunEvInfo & ne,DPS::Client::RecordChange rc){
}


void DBServer_impl::clearACS( DPS::Client::ClientType type){
}

void DBServer_impl::clearNCS( DPS::Client::ClientType type){
}

void DBServer_impl::clearAHS( DPS::Client::ClientType type){
}
void DBServer_impl::clearNHS(){
}
void DBServer_impl::clearDST( DSTType type){
}
void DBServer_impl::clearDSTI(){
}

void DBServer_impl::clearRunEvInfo( RunStatus status){
}







// Dummy functions provided dbservershares same proc with server


bool  DBServer_impl::Master(){

  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer());

  return _pser->Master();


}


int  DBServer_impl::getARS(const DPS::Client::CID & cid, DPS::Client::ARS_out arf, DPS::Client::AccessType type=DPS::Client::Any,uinteger id=0, int selffirst){

  Server_impl* _pser=dynamic_cast<Server_impl*>(getServer());

  return _pser->getARS(cid, arf,type,id,selffirst);


}


void  DBServer_impl::StartClients(const DPS::Client::CID &cid){
}
void  DBServer_impl::CheckClients(const DPS::Client::CID &cid){
}
void  DBServer_impl::KillClients(const DPS::Client::CID &cid){
}
CORBA::Boolean  DBServer_impl::sendId(DPS::Client::CID & cid, uinteger timeout){
}

void  DBServer_impl::getId(DPS::Client::CID_out cid){
}


void  DBServer_impl::Exiting(const DPS::Client::CID& cid,const char * Error, DPS::Client::ClientExiting  Status){
}




// Dummy Functions

char * DBServer_impl::getDBFilePath(const DPS::Client::CID &cid){
  return CORBA::string_dup("Oracle");
}


int  DBServer_impl::getRunEvInfoSPerl(const DPS::Client::CID &cid, RES_out res, unsigned int  maxrun, unsigned int & m2){
}


void  DBServer_impl::sendACPerl(const DPS::Client::CID &cid, const  DPS::Client::ActiveClient & ac,DPS::Client::RecordChange rc){
}




int  DBServer_impl::getEnv(const DPS::Client::CID &cid, SS_out ss){
}

void  DBServer_impl::setEnv(const DPS::Client::CID &cid,const char * env, const char *path){
}



int  DBServer_impl::sendFile(const DPS::Client::CID &cid, const FPath & fpath, const  RUN & file,TransferStatus &st)throw (CORBA::SystemException,DPS::DBServer::FailedOp){
}


void DBServer_impl::sendTDVUpdate(const DPS::Client::CID & cid,  const TDVName & tdvname ){
}





void  DBServer_impl::sendCurrentInfo(const DPS::Client::CID & cid, const  CurrentInfo &ci, int propagate){
}





void  DBServer_impl::ping()throw (CORBA::SystemException){
}

CORBA::Boolean  DBServer_impl::TypeExists(DPS::Client::ClientType type){
}



#endif
