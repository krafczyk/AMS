#ifndef __AMSPRODSERVER__
#define __AMSPRODSERVER__
#include <typedefs.h>
#include <time.h>
#include <iostream.h>
#include <unistd.h>
#include <list>
#include <queue>
#include <server-cpp-common.hh> 
#include <server-cpp-skels.hh>
#include <ORBitservices/CosNaming.hh>
#include <node.h>

class AMSServerI : public AMSNode{
 public:
 virtual void StartClients()=0;
 virtual void CheckClients()=0; 
 virtual void KillClients()=0;
 virtual AMSServerI * getServer()=0;
  AMSServerI * next(){return dynamic_cast<AMSServerI*>(AMSNode::next());}
  AMSServerI * prev(){return dynamic_cast<AMSServerI*>(AMSNode::prev());}
  AMSServerI * up(){return   dynamic_cast<AMSServerI*>(AMSNode::up());}
  AMSServerI * down(){return dynamic_cast<AMSServerI*>(AMSNode::down());}
  AMSServerI(AMSID id):AMSNode(id){};
};

class Producer_impl : public virtual POA_DPS::Producer, public AMSServerI{
protected:
class Prio{
 public:
 bool operator()(const DPS::Producer::RunEvInfo_var &a,const DPS::Producer::RunEvInfo_var &b){
 return a->Priority<b->Priority;}
};
typedef  priority_queue<DPS::Producer::RunEvInfo_var,vector<DPS::Producer::RunEvInfo_var>,Prio> RunQueue;
//typedef list<DPS::Producer::RunEvInfo> RunQueue;
DPS::Producer_mgr _ref;
RunQueue _rq;
typedef list<DPS::Client::ActiveClient_var> ACL;
typedef list<DPS::Client::ActiveClient_var>::iterator ACLI;
ACL  _acl;
DPS::Client::NominalClient_var  _ncl;
typedef list<DPS::Client::ActiveHost_var> AHL;
typedef list<DPS::Client::ActiveHost_var>::iterator AHLI;
AHL  _ahl;
public:
 AMSServerI * getServer(){return up();}
 virtual void StartClients();
 virtual void CheckClients(); 
 virtual void KillClients();
   void sendId(const DPS::Client::CID& cid, int p, int e) throw (CORBA::SystemException);
  void _init();
  virtual void getACL(ACS_out acl)throw (CORBA::SystemException);
  virtual void getNCL(NCS_out ncl)throw (CORBA::SystemException);
  virtual TDV* getTDV(const char * name, DPS::time i, DPS::time b, DPS::time e)throw (CORBA::SystemException);
  void sendTDV( const TDV & tdv, const char * name, DPS::time Insert, DPS::time Begin, DPS::time End, int Propagate, int  Error )throw (CORBA::SystemException);
  void sendCurrentInfo(const  CurrentInfo &ci, int  Propagate, int Error)throw (CORBA::SystemException);
  TDVTable * getTDVTable(const char * Name, unsigned int rv,int  Propagate, int Error)throw (CORBA::SystemException);
  void sendNtupleEnd(const  NtupleEnd & ne, int Propagate, int Error)throw (CORBA::SystemException);
  void sendEventStatusEnd(const  EventStatusEnd & ne, int Propagate, int Error)throw (CORBA::SystemException);
  void sendEndRunInfo(const  EndRunInfo & ne, int Propagate, int Error)throw (CORBA::SystemException);

  virtual DPS::Producer::RunEvInfo *  getRunEvInfo(int p,int e)throw (CORBA::SystemException);
  Producer_impl(PortableServer::POA_ptr poa, char * NC=0, char* RF=0, char* NS=0, char * TS=0);
};
class Server_impl : public virtual POA_DPS::Server, public AMSServerI{
protected:
typedef list<DPS::Client::ActiveClient_var> ACL;
typedef list<DPS::Client::ActiveClient_var>::iterator ACLI;
ACL  _acl;
DPS::Client::NominalClient_var _ncl;
typedef list<DPS::Server::NominalHost_var> NHL;
NHL  _nhl;
DPS::Server_mgr _ref;
public:
 NHL & getNHL(){return _nhl;}
 AMSServerI * getServer(){return this;}
 virtual void StartClients();
 virtual void CheckClients(); 
 virtual void KillClients();
  void _init(){};
  virtual void sendId(const DPS::Client::CID& cid, int p, int e) throw (CORBA::SystemException);
   void getACL(ACS_out acl)throw (CORBA::SystemException);
   void getAHL(AHS_out ahl)throw (CORBA::SystemException);
   void getNCL(NCS_out ncl)throw (CORBA::SystemException);
   void sendCriticalOps(const CriticalOps &, int, int)throw (CORBA::SystemException);
  void sendACL(const ACS & acl, int Propagate,int Error)throw (CORBA::SystemException);
  void sendAHL(const AHS & ahl, int Propagate,int Error)throw (CORBA::SystemException);
  void sendNCL(const NCS & ncl, int Propagate,int Error)throw (CORBA::SystemException);
  void ping()throw (CORBA::SystemException);
  Server_impl(PortableServer::POA_ptr poa, char * NS=0, char* NH=0);
  ~Server_impl(){if(_down)_down->remove();}
  bool pingHost(const char * host);
};





class AMSServer{
protected:
static AMSServer * _Head;
CORBA::ORB_var  _orb; 
PortableServer::POA_var _poa;
PortableServer::POAManager_var _mgr;
AMSServerI *   _pser;
public:
AMSServer(int argc, char *argv[]);
static AMSServer* & Singleton(){return _Head;};
const CORBA::ORB_var & getOrb()const {return _orb;}
void UpdateDB();
void SystemCheck();
~AMSServer(){if(_pser)delete _pser;}

};



#endif

