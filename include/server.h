#ifndef __AMSPRODSERVER__
#define __AMSPRODSERVER__
#include <typedefs.h>
#include <time.h>
#include <iostream.h>
#include <unistd.h>
#include <map>
#include <list>
#include <queue>
#include <server-cpp-common.hh> 
#include <server-cpp-skels.hh>
#include <ORBitservices/CosNaming.hh>
#include <node.h>
#include <astring.h>
#include <client.h>
#include <timeid.h>

class AMSServerI : public AMSNode{
protected:

uinteger _Submit;    // in fact active client ID
uinteger  _KillTimeOut;  // Kill Client Timeout
uinteger  _StartTimeOut;  // Start Client Timeout
bool _ActivateQueue;
uinteger _RecID;
CORBA::ORB_ptr  _defaultorb;    
AMSClient * _parent;

typedef list<DPS::Server::CriticalOps> COL;
typedef list<DPS::Server::CriticalOps>::iterator COLI;
COL _col;

 DPS::Client::ClientType _Type;
typedef list<DPS::Client::ActiveHost_var> AHL;
typedef list<DPS::Client::ActiveHost_var>::iterator AHLI;
AHL  _ahl;

typedef list<DPS::Client::ActiveClient_var> ACL;
typedef list<DPS::Client::ActiveClient_var>::iterator ACLI;
ACL  _acl;     // Active Server List
class ACA{
private:
DPS::Client::ActiveClient _a;
DPS::Client::RecordChange _rc;
public:
DPS::Client::ActiveClient & getacv(){return _a;}
DPS::Client::RecordChange getaction(){return _rc;}
ACA(const DPS::Client::ActiveClient & a,DPS::Client::RecordChange rc):_a(a),_rc(rc){}

};
typedef list<ACA> ACAL;
typedef list<ACA>::iterator ACALI;
ACAL _acqueue;

DPS::Client::NominalClient_var _ncl;


typedef list<DPS::Client::NominalHost_var> NHL;
typedef list<DPS::Client::NominalHost_var>::iterator NHLI;
NHL  _nhl;

typedef map<AString,CORBA::String_var> MS;
 MS _refmap;
virtual void _PurgeQueue()=0;

  void _UpdateACT(const DPS::Client::CID & cid, DPS::Client::ClientStatus status);
 public:
 class Eqs :public unary_function<DPS::Client::ActiveClient,bool>{
 DPS::Client::ActiveClient _a;
 public:
 explicit Eqs( const  DPS::Client::ActiveClient & a):_a(a){}
  bool operator () (const DPS::Client::ActiveClient_var & a){return _a.id.uid==a->id.uid;}
};
 class find :public unary_function<DPS::Client::ActiveClient,bool>{
 DPS::Client::ClientStatus _st;
 public:
 explicit find( const  DPS::Client::ClientStatus st):_st(st){}
  bool operator () (const DPS::Client::ActiveClient_var & a){return a->Status==_st;}
};
  uinteger getmaxcl() const {return _Submit;}
  DPS::Client::NominalClient_var getncl(){return _ncl;}
  void addone(){++_Submit;}
  COL & getcol(){return _col;}
  ACL & getacl(){return _acl;}
  AHL & getahl(){return _ahl;}
  NHL & getnhl(){return _nhl;}
  MS & getrefmap(){return _refmap;}
  DPS::Client::ClientType getType()const {return _Type;}

 virtual void UpdateDB()=0;
 virtual void StartClients(const DPS::Client::CID &cid)=0;
 virtual void CheckClients(const DPS::Client::CID &cid)=0; 
 virtual void KillClients(const DPS::Client::CID &cid)=0;
 virtual AMSServerI * getServer()=0;

  void PropagateAC( DPS::Client::ActiveClient & ac, DPS::Client::RecordChange rc,int uid=100000000);

  bool InactiveClientExists();
  void RegisteredClientExists();
  AMSServerI * next(){return dynamic_cast<AMSServerI*>(AMSNode::next());}
  AMSServerI * prev(){return dynamic_cast<AMSServerI*>(AMSNode::prev());}
  AMSServerI * up(){return   dynamic_cast<AMSServerI*>(AMSNode::up());}
  AMSServerI * down(){return dynamic_cast<AMSServerI*>(AMSNode::down());}

  AMSServerI(AMSID id, AMSClient * parent=0,DPS::Client::ClientType type=DPS::Client::Generic):_Type(type),AMSNode(id),_Submit(0),_StartTimeOut(60),_KillTimeOut(120),_parent(parent),_ActivateQueue(false),_RecID(0){};
};


class AMSServer : public AMSClient{
public:
class OrbitVars{
public:
CORBA::ORB_ptr  _orb;    
PortableServer::POA_ptr _poa;
PortableServer::POAManager_ptr _mgr;
};
protected:
bool _GlobalError;
static AMSServer * _Head;
typedef map<AString, AMSServer::OrbitVars> MO;  
MO _orbmap; 
AMSServerI *   _pser;
public:
AMSServer(int argc, char *argv[]);
static AMSServer* & Singleton(){return _Head;};
void Listening(int sleeptime=0);
void UpdateDB();
void SystemCheck();
void Exiting(const char * message=0);
~AMSServer(){Exiting();if(_pser)delete _pser;}
};







class Server_impl : public virtual POA_DPS::Server, public AMSServerI{
protected:
AString _iface;
DPS::Client::NominalClient_var _nki;
OpType _clear(OpType type){ if(type==StartClient)return ClearStartClient;else if (type==KillClient)return ClearKillClient;else return ClearCheckClient;}
public:
  Server_impl(uinteger i=0):POA_DPS::Server(),AMSServerI(AMSID("Server",++i),0,DPS::Client::Server){};
  Server_impl(const map<AString, AMSServer::OrbitVars> & mo,  const DPS::Client::CID & cid,AMSClient * parent, char * NS=0, char* NH=0, char *NK=0);
  Server_impl(const map<AString, AMSServer::OrbitVars> & mo, DPS::Server_ptr _cvar, const DPS::Client::CID & cid, AMSClient * parent);
  ~Server_impl(){if(_down)_down->remove();}
 bool Master();
 integer Kill(const DPS::Client::ActiveClient & ac, int signal, bool self);
 bool PingServer(const DPS::Client::ActiveClient & ac);
 bool Lock(const DPS::Client::CID & cid, OpType op, DPS::Client::ClientType type, int Time);
 void setInterface(const char * iface){_iface=iface;}
 AMSServerI * getServer(){return this;}
 virtual void UpdateDB();
 virtual void StartClients(const DPS::Client::CID &cid);
 virtual void CheckClients(const DPS::Client::CID &cid); 
 virtual void KillClients(const DPS::Client::CID &cid);
void _PurgeQueue();
  void _init();
  CORBA::Boolean sendId(DPS::Client::CID & cid, uinteger timeout) throw (CORBA::SystemException);
  CORBA::Boolean getNC(const DPS::Client::CID &cid, DPS::Client::NominalClient_out nc)throw (CORBA::SystemException);
  CORBA::Boolean getNK(const DPS::Client::CID &cid, DPS::Client::NominalClient_out nc)throw (CORBA::SystemException);
   int getARS(const DPS::Client::CID & cid, DPS::Client::ARS_out ars, int maxcid=100000000)throw (CORBA::SystemException);
   int getACS(const DPS::Client::CID &cid, ACS_out acs, unsigned int & maxc)throw (CORBA::SystemException);
   void sendAC(const DPS::Client::CID &cid,  DPS::Client::ActiveClient & ac,DPS::Client::RecordChange rc)throw (CORBA::SystemException);
  void Exiting(const DPS::Client::CID& cid,const char * Error, DPS::Client::ClientExiting  Status)throw (CORBA::SystemException);
   int getNHS(const DPS::Client::CID &cid,NHS_out nhl)throw (CORBA::SystemException);
   int getAHS(const DPS::Client::CID &cid,AHS_out ahl)throw (CORBA::SystemException);

   void ping()throw (CORBA::SystemException);
   void sendCriticalOps(const DPS::Client::CID &cid, const CriticalOps & op)throw (CORBA::SystemException);

  bool pingHost(const char * host);
  CORBA::Boolean TypeExists(DPS::Client::ClientType type)throw (CORBA::SystemException);

  void StartSelf(const DPS::Client::CID & cid, DPS::Client::RecordChange rc);

};




class Producer_impl : public virtual POA_DPS::Producer, public AMSServerI{
protected:

uinteger _RunID;

bool _DBInitialized;

typedef map<AMSID,AMSTimeID*> TID;
typedef map<AMSID,AMSTimeID*>::iterator TIDI;
TID _tid; 


typedef list<DPS::Producer::RunEvInfo_var> RL;
typedef list<DPS::Producer::RunEvInfo_var>::iterator RLI;
RL _rl;

typedef list<DST_var> DSTL;
typedef list<DST_var>::iterator DSTLI;
DSTL _ntuple;


class RA{
private:
RunEvInfo _a;
DPS::Client::RecordChange _rc;
public:
RunEvInfo & getacv(){return _a;}
DPS::Client::RecordChange getaction(){return _rc;}
RA(const RunEvInfo & a,DPS::Client::RecordChange rc):_a(a),_rc(rc){}

};
typedef list<RA> RAL;
typedef list<RA>::iterator RALI;
RAL _runqueue;

class DSTA{
private:
DST _a;
DPS::Client::RecordChange _rc;
public:
DST & getacv(){return _a;}
DPS::Client::RecordChange getaction(){return _rc;}
DSTA(const DST & a,DPS::Client::RecordChange rc):_a(a),_rc(rc){}

};
typedef list<DSTA> DSTAL;
typedef list<DSTA>::iterator DSTALI;
DSTAL _ntqueue;


public:
 bool Master();
 AMSServerI * getServer(){return up();}

  Producer_impl(const map<AString, AMSServer::OrbitVars> & mo,  const DPS::Client::CID & cid,AMSClient * parent,char * NC=0, char* RF=0, char* NS=0, char * TS=0);
  Producer_impl(uinteger i=0):POA_DPS::Producer(),AMSServerI(AMSID("Producer",++i),0,DPS::Client::Producer){};
  Producer_impl(const map<AString, AMSServer::OrbitVars> & mo, DPS::Server_ptr _cvar,DPS::Client::CID  cid, AMSClient * parent);
  void _PurgeQueue();
  void _init();
 virtual void UpdateDB();
 virtual void StartClients(const DPS::Client::CID &cid);
 virtual void CheckClients(const DPS::Client::CID &cid); 
 virtual void KillClients(const DPS::Client::CID &cid);
  void RunFailed(const DPS::Client::ActiveClient & ac);
  CORBA::Boolean sendId(DPS::Client::CID & cid, uinteger timeout) throw (CORBA::SystemException);
   int getARS(const DPS::Client::CID & cid, DPS::Client::ARS_out ars, int maxcid=100000000)throw (CORBA::SystemException);
  void Exiting(const DPS::Client::CID& cid,const char * Error, DPS::Client::ClientExiting  Status)throw (CORBA::SystemException);
  int getTDV(const DPS::Client::CID & cid,  TDVName & tdvname, TDVbody_out body)throw (CORBA::SystemException);
  void sendTDV(const DPS::Client::CID & cid, const TDVbody & tdv, TDVName & tdvname )throw (CORBA::SystemException);

  TIDI & _findTDV(const TDVName & tdv);  
  int getTDVTable(const DPS::Client::CID & cid,TDVName & tdvname, unsigned int run, TDVTable_out table)throw (CORBA::SystemException);


  int getRunEvInfoS(const DPS::Client::CID &cid, RES_out res, unsigned int & maxrun)throw (CORBA::SystemException);
   void getRunEvInfo(const DPS::Client::CID &cid, RunEvInfo_out rv)throw (CORBA::SystemException);

  void sendRunEvInfo(const  RunEvInfo & ne,DPS::Client::RecordChange rc)throw (CORBA::SystemException);

  int getNtupleS(const DPS::Client::CID & ci, DSTS_out dsts)throw (CORBA::SystemException);

  void sendCurrentInfo(const DPS::Client::CID & ci, const  CurrentInfo &ci)throw (CORBA::SystemException);

  void sendNtupleEnd(const DPS::Client::CID & ci, const  DST & ne, DPS::Client::RecordChange rc)throw (CORBA::SystemException);

  void sendEventStatusEnd(const DPS::Client::CID & ci,const  EventStatusEnd & ne,  DPS::Client::RecordChange rc)throw (CORBA::SystemException);


void PropagateRun(const RunEvInfo & ri, DPS::Client::RecordChange rc, int uid=100000000);
void PropagateNtuple(const DST & ri, DPS::Client::RecordChange rc, int uid=100000000);

class REInfo_find: public unary_function<RunEvInfo,bool>{
DPS::Client::CID _cid;
RunStatus _rs;
public:
 explicit REInfo_find(const DPS::Client::CID & cid, RunStatus rs):_cid(cid),_rs(rs){}
 bool operator()(const RunEvInfo & a){
  return a.Status==ToBeRerun && a.History==_rs && strcmp((const char *) _cid.HostName, (const char *) a.cinfo.HostName);
}
};
class REInfo_process: public unary_function<RunEvInfo,bool>{
public:
 explicit REInfo_process(){}
 bool operator()(const RunEvInfo & a){
  return a.Status!=Finished && a.Status!=Failed;
}
};

class REInfo_Eqs: public unary_function<RunEvInfo,bool>{
RunEvInfo _re;
public:
 explicit REInfo_Eqs(const RunEvInfo & re):_re(re){}
 explicit REInfo_Eqs(unsigned int uid){_re.uid=uid;}
 bool operator()(const RunEvInfo & a){return a.uid== _re.uid;}
};

class REInfo_Count: public unary_function<RunEvInfo,bool>{
public:
 explicit REInfo_Count(){}
 bool operator()(const RunEvInfo & a){return a.Status== ToBeRerun || a.Status==Processing;}
};

class REInfo_EqsClient: public unary_function<RunEvInfo,bool>{
DPS::Client::CID _cid;
public:
 explicit REInfo_EqsClient(const DPS::Client::CID & cid):_cid(cid){}
 bool operator()(const RunEvInfo & a){return a.cuid== _cid.uid;}
};




};




class Less{
public:
bool operator () (const DPS::Client::ActiveClient & a,const DPS::Client::ActiveClient & b){return a.id.uid<b.id.uid;}
//bool operator () (const DPS::Client::ActiveClient_var & a,const DPS::Client::ActiveClient_var & b){return a->id.uid<b->id.uid;}

bool operator()(const DPS::Producer::RunEvInfo &a,const DPS::Producer::RunEvInfo &b){
if(a.Priority!=b.Priority)return a.Priority>b.Priority;
else return a.Status<b.Status;
}

bool operator ()(const DPS::Client::ActiveHost &a,const DPS::Client::ActiveHost &b){
if(a.Status == b.Status || b.Status==DPS::Client::OK)return false;
else if(a.Status != DPS::Client::NoResponse)return true;
else return false;

}


};








#endif

