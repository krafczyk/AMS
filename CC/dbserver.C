//  $Id: dbserver.C,v 1.8 2001/02/19 13:48:53 choutko Exp $
#include <dbserver.h>





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

