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

class AMSServerI : public AMSNode{
 protected:
 unsigned long _Submit;
 typedef map<AString,CORBA::String_var> MS;
 MS _refmap;
 public:
 virtual void StartClients()=0;
 virtual void CheckClients()=0; 
 virtual void KillClients()=0;
 virtual AMSServerI * getServer()=0;
  AMSServerI * next(){return dynamic_cast<AMSServerI*>(AMSNode::next());}
  AMSServerI * prev(){return dynamic_cast<AMSServerI*>(AMSNode::prev());}
  AMSServerI * up(){return   dynamic_cast<AMSServerI*>(AMSNode::up());}
  AMSServerI * down(){return dynamic_cast<AMSServerI*>(AMSNode::down());}
  AMSServerI(AMSID id):AMSNode(id),_Submit(0){};
};



class AMSServer{
public:
class OrbitVars{
public:
CORBA::ORB_ptr  _orb;    
PortableServer::POA_ptr _poa;
PortableServer::POAManager_ptr _mgr;
/*
~OrbitVars(){CORBA::release(_orb);
             CORBA::release(_poa);
             CORBA::release(_mgr);}
*/
};
protected:
static AMSServer * _Head;
typedef map<AString, AMSServer::OrbitVars> MO;  
MO _orbmap; 
AMSServerI *   _pser;
public:
AMSServer(int argc, char *argv[]);
static AMSServer* & Singleton(){return _Head;};
void Listening();
void UpdateDB();
void SystemCheck();
~AMSServer(){if(_pser)delete _pser;}

};





class Producer_impl : public virtual POA_DPS::Producer, public AMSServerI{
protected:
class Prio{
 public:
 bool operator()(const DPS::Producer::RunEvInfo_var &a,const DPS::Producer::RunEvInfo_var &b){
 return a->Priority<b->Priority;}
 bool operator()(const DPS::Client::ActiveHost_var &a,const DPS::Client::ActiveHost_var &b){
if(a->Status == b->Status || b->Status==OK)return false;
else if(a->Status != NoResponse)return true;
else return false;
}
};
typedef  priority_queue<DPS::Producer::RunEvInfo_var,vector<DPS::Producer::RunEvInfo_var>,Prio> RunQueue;
/*
DPS::Producer_mgr _ref;
CORBA::String_var _refstring;
*/
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
   CORBA::Boolean sendId(const DPS::Client::CID& cid, int p, int e) throw (CORBA::SystemException);
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
  Producer_impl(const map<AString, AMSServer::OrbitVars> & mo, char * NC=0, char* RF=0, char* NS=0, char * TS=0);
};
class Server_impl : public virtual POA_DPS::Server, public AMSServerI{
protected:
typedef list<DPS::Client::ActiveHost_var> AHL;
typedef list<DPS::Client::ActiveHost_var>::iterator AHLI;
AHL  _ahl;
typedef list<DPS::Client::ActiveClient_var> ACL;
typedef list<DPS::Client::ActiveClient_var>::iterator ACLI;
ACL  _asl;     // Active Server List
DPS::Client::NominalClient_var _ncl;
typedef list<DPS::Server::NominalHost_var> NHL;
NHL  _nhl;
public:
 NHL & getNHL(){return _nhl;}
 AMSServerI * getServer(){return this;}
 virtual void StartClients();
 virtual void CheckClients(); 
 virtual void KillClients();
  void _init();
  CORBA::Boolean sendId(const DPS::Client::CID& cid, int p, int e) throw (CORBA::SystemException);
   void getACL(ACS_out acl)throw (CORBA::SystemException);
   void getAHL(AHS_out ahl)throw (CORBA::SystemException);
   void getNCL(NCS_out ncl)throw (CORBA::SystemException);
   void sendCriticalOps(const CriticalOps &, int, int)throw (CORBA::SystemException);
  void sendACL(const ACS & acl, int Propagate,int Error)throw (CORBA::SystemException);
  void sendAHL(const AHS & ahl, int Propagate,int Error)throw (CORBA::SystemException);
  void sendNCL(const NCS & ncl, int Propagate,int Error)throw (CORBA::SystemException);
  void ping()throw (CORBA::SystemException);
  Server_impl(const map<AString, AMSServer::OrbitVars> & mo, char * NS=0, char* NH=0);
  ~Server_impl(){if(_down)_down->remove();}
  bool pingHost(const char * host);
};







#endif

