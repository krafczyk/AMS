//  $Id: dbserver.C,v 1.2 2001/02/06 15:46:20 choutko Exp $
#include <dbserver.h>

void  DBServer_impl::_init(){
}




  DBServer_impl::DBServer_impl(const map<AString, AMSServer::OrbitVars> & mo,  const DPS::Client::CID & cid,AMSClient * parent,char * NC=0, char* RF=0, char* NS=0, char * TS=0):POA_DPS::DBServer(),AMSServerI(AMSID("DBServer",0),parent,DPS::Client::DBServer){
}
   DBServer_impl::DBServer_impl(const map<AString, AMSServer::OrbitVars> & mo, DPS::Server_ptr _cvar,DPS::Client::CID  cid, AMSClient * parent):POA_DPS::DBServer(),AMSServerI(AMSID("DBServer",0),parent,DPS::Client::DBServer){
}
 bool  DBServer_impl::Master(){
 }
 bool  DBServer_impl::PingDBServer(const DPS::Client::ActiveClient & ac){
}

  void  DBServer_impl::UpdateDB(bool force=false){
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
  int  DBServer_impl::getNC(const DPS::Client::CID &cid, DPS::Client::NCS_out nc){
}
  int  DBServer_impl::getNK(const DPS::Client::CID &cid, DPS::Client::NCS_out nc){
}
   int  DBServer_impl::getARS(const DPS::Client::CID & cid, DPS::Client::ARS_out ars, DPS::Client::AccessType type=DPS::Client::Any,uinteger id=0){
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

  void  DBServer_impl::Exiting(const DPS::Client::CID& cid,const char * Error, DPS::Client::ClientExiting  Status){
}

   int  DBServer_impl::getNHS(const DPS::Client::CID &cid,NHS_out nhl){
}

   int  DBServer_impl::getAHS(const DPS::Client::CID &cid,AHS_out ahl){
}

   int  DBServer_impl::getEnv(const DPS::Client::CID &cid, SS_out ss){
}

   void  DBServer_impl::setEnv(const DPS::Client::CID &cid,const char * env, const char *path){
}

   void  DBServer_impl::ping()throw (CORBA::SystemException){
}

  CORBA::Boolean  DBServer_impl::TypeExists(DPS::Client::ClientType type){
}


  void  DBServer_impl::StartSelf(const DPS::Client::CID & cid, DPS::Client::RecordChange rc){
}

  int  DBServer_impl::getTDV(const DPS::Client::CID & cid,  TDVName & tdvname, TDVbody_out body){
}

  int  DBServer_impl::getSplitTDV(const DPS::Client::CID & cid, unsigned int & pos, TDVName & tdvname, TDVbody_out body, TransferStatus &st){
}

  void  DBServer_impl::sendTDV(const DPS::Client::CID & cid, const TDVbody & tdv, TDVName & tdvname ){
}

  void  DBServer_impl::sendTDVUpdate(const DPS::Client::CID & cid, const TDVName & tdvname ){
}


  int  DBServer_impl::getTDVTable(const DPS::Client::CID & cid,TDVName & tdvname, unsigned int run, TDVTable_out table){
}


  int  DBServer_impl::getDSTInfoS(const DPS::Client::CID &cid, DSTIS_out res){
}

  int  DBServer_impl::getRun(const DPS::Client::CID &cid, const FPath & fpath, RUN_out run,TransferStatus & st)throw (CORBA::SystemException,DPS::Producer::FailedOp){
}

  int  DBServer_impl::sendFile(const DPS::Client::CID &cid, const FPath & fpath, const  RUN & file,TransferStatus & st)throw (CORBA::SystemException,DPS::Producer::FailedOp){
}

  int  DBServer_impl::getRunEvInfoS(const DPS::Client::CID &cid, RES_out res, unsigned int & maxrun){
}

   void  DBServer_impl::getRunEvInfo(const DPS::Client::CID &cid, RunEvInfo_out rv, DSTInfo_out dv){
}

  void  DBServer_impl::sendRunEvInfo(const  RunEvInfo & ne,DPS::Client::RecordChange rc){
}

  void  DBServer_impl::sendDSTInfo(const  DSTInfo & ne,DPS::Client::RecordChange rc){
}

  int  DBServer_impl::getDSTS(const DPS::Client::CID & ci, DSTS_out dsts){
}

  void  DBServer_impl::sendCurrentInfo(const DPS::Client::CID & cid, const  CurrentInfo &ci, int propagate){
}

  void  DBServer_impl::sendDSTEnd(const DPS::Client::CID & ci, const  DST & ne, DPS::Client::RecordChange rc){
}

   int  DBServer_impl::getFreeHost(const DPS::Client::CID & ci, DPS::Client::ActiveHost_out host){
}













