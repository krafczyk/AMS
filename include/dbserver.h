//  $Id: dbserver.h,v 1.6 2001/06/11 14:01:48 choutko Exp $
#ifndef __AMSDBSERVER__
#define __AMSDBSERVER__
#include <server.h>

class DBServer_impl : public virtual POA_DPS::DBServer, public AMSServerI{
protected:
  void _PurgeQueue(){};
  void _init();
public:
  DBServer_impl(const map<AString, AMSServer::OrbitVars> & mo,  const DPS::Client::CID & cid,AMSClient * parent);
  DBServer_impl(const map<AString, AMSServer::OrbitVars> & mo, DPS::Server_ptr _cvar,DPS::Client::CID  cid, AMSClient * parent);
 bool Master(bool advanced=true);
 AMSServerI * getServer(){return up();}
 virtual void UpdateDB(bool force=false);
 virtual void StartClients(const DPS::Client::CID &cid);
 virtual void CheckClients(const DPS::Client::CID &cid); 
 virtual void KillClients(const DPS::Client::CID &cid);
  CORBA::Boolean sendId(DPS::Client::CID & cid, uinteger timeout);
  void getId(DPS::Client::CID_out cid);
  //  int getNC(const DPS::Client::CID &cid, DPS::Client::NCS_out nc) throw (CORBA::SystemException,DPS::DBServer::DBProblem);
  int getNC(const DPS::Client::CID &cid, DPS::Client::NCS_out nc);
  int getNK(const DPS::Client::CID &cid, DPS::Client::NCS_out nc);
   int getARS(const DPS::Client::CID & cid, DPS::Client::ARS_out ars, DPS::Client::AccessType type=DPS::Client::Any,uinteger id=0, int selffirst=0);
   int getACS(const DPS::Client::CID &cid, ACS_out acs, unsigned int & maxc);
   void sendAC(const DPS::Client::CID &cid,  DPS::Client::ActiveClient & ac,DPS::Client::RecordChange rc);
   void sendACPerl(const DPS::Client::CID &cid,  const DPS::Client::ActiveClient & ac,DPS::Client::RecordChange rc);
   void sendAH(const DPS::Client::CID &cid,  const DPS::Client::ActiveHost & ah,DPS::Client::RecordChange rc);
   void sendNH(const DPS::Client::CID &cid,  const DPS::Client::NominalHost & ah,DPS::Client::RecordChange rc);
   void sendNC(const DPS::Client::CID &cid,  const DPS::Client::NominalClient & nc,DPS::Client::RecordChange rc);
   void sendNK(const DPS::Client::CID &cid,  const DPS::Client::NominalClient & nc,DPS::Client::RecordChange rc);
   CORBA::Boolean getDBSpace(const DPS::Client::CID &cid, const char * path, const char * addpath,DB_out db);
  void Exiting(const DPS::Client::CID& cid,const char * Error, DPS::Client::ClientExiting  Status);
   int getNHS(const DPS::Client::CID &cid,NHS_out nhl);
   int getAHS(const DPS::Client::CID &cid,AHS_out ahl);
   int getEnv(const DPS::Client::CID &cid, SS_out ss);
   void setEnv(const DPS::Client::CID &cid,const char * env, const char *path);
   void ping()throw (CORBA::SystemException);
   CORBA::Boolean AdvancedPing()throw (CORBA::SystemException){};
   void sendCriticalOps(const DPS::Client::CID &cid, const CriticalOps & op){};
  CORBA::Boolean TypeExists(DPS::Client::ClientType type);

  int getTDV(const DPS::Client::CID & cid,  TDVName & tdvname, TDVbody_out body);
  int getSplitTDV(const DPS::Client::CID & cid, unsigned int & pos, TDVName & tdvname, TDVbody_out body, TransferStatus &st);
  void sendTDV(const DPS::Client::CID & cid, const TDVbody & tdv, TDVName & tdvname );
  void sendTDVUpdate(const DPS::Client::CID & cid, const TDVName & tdvname );

  int getTDVTable(const DPS::Client::CID & cid,TDVName & tdvname, unsigned int run, TDVTable_out table);

  DPS::Client::ClientType    unsi2CT(unsigned int a);
  DPS::Client::ClientStatus  unsi2CS(unsigned int a);
  DPS::Client::ClientExiting unsi2CSE(unsigned int a);
  DPS::Client::HostStatus    unsi2HS(unsigned int a);
  DPS::Producer::RunStatus   unsi2RS(unsigned int a);
  DPS::Producer::RunMode     unsi2RM(unsigned int a);
  DPS::Producer::DSTType     unsi2DSTT(unsigned int a);
  DPS::Producer::DSTStatus   unsi2DSTS(unsigned int a);
  int getDSTInfoS(const DPS::Client::CID &cid, DSTIS_out res);
  int getRun(const DPS::Client::CID &cid, const FPath & fpath, RUN_out run,TransferStatus & st)throw (CORBA::SystemException,DPS::Producer::FailedOp);
  int sendFile(const DPS::Client::CID &cid, const FPath & fpath, const  RUN & file,TransferStatus & st)throw (CORBA::SystemException,DPS::Producer::FailedOp);
  int getRunEvInfoS(const DPS::Client::CID &cid, RES_out res, unsigned int & maxrun);
  int getRunEvInfoSPerl(const DPS::Client::CID &cid, RES_out res, unsigned int  maxrun, unsigned  int &maxrun1);
  char * getDBFilePath(const DPS::Client::CID &cid);
   void getRunEvInfo(const DPS::Client::CID &cid, RunEvInfo_out rv, DSTInfo_out dv);
  void sendRunEvInfo(const  RunEvInfo & ne,DPS::Client::RecordChange rc);
  void sendDSTInfo(const  DSTInfo & ne,DPS::Client::RecordChange rc);
  int getDSTS(const DPS::Client::CID & ci, DSTS_out dsts);
  void sendCurrentInfo(const DPS::Client::CID & ci, const  CurrentInfo &ci, int propagate);
  void sendDSTEnd(const DPS::Client::CID & ci, const  DST & ne, DPS::Client::RecordChange rc);
   int  getFreeHost(const DPS::Client::CID & ci, DPS::Client::ActiveHost_out host);
    void clearACS( DPS::Client::ClientType type);
    void clearNCS( DPS::Client::ClientType type);
    void clearAHS( DPS::Client::ClientType type);
    void clearNHS();
    void clearDST( DPS::Producer::DSTType type);
    void clearDSTI();
    void clearRunEvInfo( DPS::Producer::RunStatus status);
  }; 











#endif

