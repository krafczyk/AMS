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


class AMSServerI : public AMSNode{
protected:

uinteger _Submit;    // in fact active client ID
uinteger  _KillTimeOut;  // Kill Client Timeout
uinteger  _StartTimeOut;  // Start Client Timeout

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

DPS::Client::NominalClient_var _ncl;

typedef list<DPS::Client::NominalHost_var> NHL;
typedef list<DPS::Client::NominalHost_var>::iterator NHLI;
NHL  _nhl;

typedef map<AString,CORBA::String_var> MS;
 MS _refmap;

  void _UpdateACT(const DPS::Client::CID & cid, DPS::Client::ClientStatus status);
 public:
 class Eqs :public unary_function<DPS::Client::ActiveClient,bool>{
 DPS::Client::ActiveClient _a;
 public:
 explicit Eqs( const  DPS::Client::ActiveClient & a):_a(a){}
  bool operator () (const DPS::Client::ActiveClient_var & a){return _a.id.uid==a->id.uid;}
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

 virtual void StartClients(const DPS::Client::CID &cid)=0;
 virtual void CheckClients(const DPS::Client::CID &cid)=0; 
 virtual void KillClients(const DPS::Client::CID &cid)=0;
 virtual AMSServerI * getServer()=0;

  void PropagateAC( DPS::Client::ActiveClient & ac, DPS::Client::RecordChange rc);

  bool InactiveClientExists();

  AMSServerI * next(){return dynamic_cast<AMSServerI*>(AMSNode::next());}
  AMSServerI * prev(){return dynamic_cast<AMSServerI*>(AMSNode::prev());}
  AMSServerI * up(){return   dynamic_cast<AMSServerI*>(AMSNode::up());}
  AMSServerI * down(){return dynamic_cast<AMSServerI*>(AMSNode::down());}

  AMSServerI(AMSID id, AMSClient * parent=0,DPS::Client::ClientType type=DPS::Client::Generic):_Type(type),AMSNode(id),_Submit(0),_StartTimeOut(60),_KillTimeOut(120),_parent(parent){};
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
public:
  Server_impl(uinteger i=0):POA_DPS::Server(),AMSServerI(AMSID("Server",++i),0,DPS::Client::Server){};
  Server_impl(const map<AString, AMSServer::OrbitVars> & mo,  const DPS::Client::CID & cid,AMSClient * parent, char * NS=0, char* NH=0);
  Server_impl(const map<AString, AMSServer::OrbitVars> & mo, DPS::Server_ptr _cvar, const DPS::Client::CID & cid, AMSClient * parent);
  ~Server_impl(){if(_down)_down->remove();}

 void setInterface(const char * iface){_iface=iface;}
 AMSServerI * getServer(){return this;}
 virtual void StartClients(const DPS::Client::CID &cid);
 virtual void CheckClients(const DPS::Client::CID &cid); 
 virtual void KillClients(const DPS::Client::CID &cid);
  void _init();
  CORBA::Boolean sendId(DPS::Client::CID & cid, uinteger timeout) throw (CORBA::SystemException);
  CORBA::Boolean getNC(const DPS::Client::CID &cid, DPS::Client::NominalClient_out nc)throw (CORBA::SystemException);
   int getARS(const DPS::Client::CID & cid, DPS::Client::ARS_out ars)throw (CORBA::SystemException);
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



typedef list<DPS::Producer::RunEvInfo_var> RL;
typedef list<DPS::Producer::RunEvInfo_var>::iterator RLI;
RL _rl;

typedef list<DST_var> DSTL;
typedef list<DST_var>::iterator DSTLI;
DSTL _ntuple;

public:
 AMSServerI * getServer(){return up();}

  Producer_impl(const map<AString, AMSServer::OrbitVars> & mo,  const DPS::Client::CID & cid,AMSClient * parent,char * NC=0, char* RF=0, char* NS=0, char * TS=0);
  Producer_impl(uinteger i=0):POA_DPS::Producer(),AMSServerI(AMSID("Producer",++i),0,DPS::Client::Producer){};
  Producer_impl(const map<AString, AMSServer::OrbitVars> & mo, DPS::Server_ptr _cvar,DPS::Client::CID  cid, AMSClient * parent);
  void _init();
 virtual void StartClients(const DPS::Client::CID &cid);
 virtual void CheckClients(const DPS::Client::CID &cid); 
 virtual void KillClients(const DPS::Client::CID &cid);
  CORBA::Boolean sendId(DPS::Client::CID & cid, uinteger timeout) throw (CORBA::SystemException);
   int getARS(const DPS::Client::CID & cid, DPS::Client::ARS_out ars)throw (CORBA::SystemException);
  void Exiting(const DPS::Client::CID& cid,const char * Error, DPS::Client::ClientExiting  Status)throw (CORBA::SystemException);

  virtual TDV* getTDV(const char * name, DPS::time i, DPS::time b, DPS::time e)throw (CORBA::SystemException);
  void sendTDV( const TDV & tdv, const char * name, DPS::time Insert, DPS::time Begin, DPS::time End, int Propagate, int  Error )throw (CORBA::SystemException);
  TDVTable * getTDVTable(const char * Name, unsigned int rv,int  Propagate, int Error)throw (CORBA::SystemException);


  int getRunEvInfoS(const DPS::Client::CID &cid, RES_out res, unsigned int & maxrun)throw (CORBA::SystemException);
   void getRunEvInfo(const DPS::Client::CID &cid, RunEvInfo_out rv)throw (CORBA::SystemException);

  void sendRunEvInfo(const  RunEvInfo & ne,DPS::Client::RecordChange rc)throw (CORBA::SystemException);

  int getNtupleS(const DPS::Client::CID & ci, DSTS_out dsts)throw (CORBA::SystemException);

  void sendCurrentInfo(const DPS::Client::CID & ci, const  CurrentInfo &ci)throw (CORBA::SystemException);

  void sendNtupleEnd(const DPS::Client::CID & ci, const  DST & ne, DPS::Client::RecordChange rc)throw (CORBA::SystemException);

  void sendEventStatusEnd(const DPS::Client::CID & ci,const  EventStatusEnd & ne,  DPS::Client::RecordChange rc)throw (CORBA::SystemException);


void PropagateRun(const RunEvInfo & ri, DPS::Client::RecordChange rc);

class REInfo_find: public unary_function<RunEvInfo,bool>{
DPS::Producer::RunStatus _rs;
public:
 explicit REInfo_find(DPS::Producer::RunStatus rs):_rs(rs){}
 bool operator()(const RunEvInfo & a){return a.Status== _rs;}
};

class REInfo_Eqs: public unary_function<RunEvInfo,bool>{
RunEvInfo _re;
public:
 explicit REInfo_Eqs(const RunEvInfo & re):_re(re){}
 bool operator()(const RunEvInfo & a){return a.uid== _re.uid;}
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

