#ifndef __AMSPRODSERVER__
#define __AMSPRODSERVER__
#include <typedefs.h>
#include <time.h>
#include <iostream.h>
#include <unistd.h>
#include <list>
#include <queue>
#include <server-cpp-skels.hh>
#include <ORBitservices/CosNaming.hh>


class Producer_impl : public virtual POA_DPS::Producer{
public:
  virtual void sendId(const DPS::Client::CID& cid, int p, int e) throw (CORBA::SystemException);
  virtual void getACL(ACL_out acl);
  virtual void getNCL(NCL_out ncl);
  virtual TDV* getTDV(const char * name, DPS::time i, DPS::time b, DPS::time e);
  void sendTDV( const TDV & tdv, const char * name, DPS::time Insert, DPS::time Begin, DPS::time End, int Propagate, int  Error );
  void sendCurrentInfo(const  CurrentInfo &ci, int  Propagate, int Error);
  TDVTable * getTDVTable(const char * Name, const  RunEv& rv,int  Propagate, int Error);
  void sendNtupleEnd(const  NtupleEnd & ne, int Propagate, int Error);
  void sendEventStatusEnd(const  EventStatusEnd & ne, int Propagate, int Error);
  void sendEndRunInfo(const  EndRunInfo & ne, int Propagate, int Error);

  virtual DPS::Producer::RunEvInfo *  getRunEvInfo(int p,int e) throw (CORBA::SystemException); 
};
class Server_impl : public virtual POA_DPS::Server{
public:
  virtual void sendId(const DPS::Client::CID& cid, int p, int e) throw (CORBA::SystemException);
  virtual void getACL(ACL_out acl);
  virtual void getAHL(AHL_out ahl);
  virtual void getNCL(NCL_out ncl);
  void sendCriticalOps(const CriticalOps &, int, int);
  void sendACL(const ACL & acl, int Propagate,int Error);
  void sendAHL(const AHL & ahl, int Propagate,int Error);
  void sendNCL(const NCL & ncl, int Propagate,int Error);
  void ping();
};





class AMSServer{
protected:
static AMSServer * _Head;
CORBA::ORB_var  _orb; 
PortableServer::POA_var _poa;
PortableServer::POAManager_var _mgr;
Producer_impl* _ppro;
Server_impl*   _pser;
class Prio{
 public:
 bool operator()(const DPS::Producer::RunEv &a,const DPS::Producer::RunEv &b){
 return a.Priority<b.Priority;}
};
typedef  priority_queue<DPS::Producer::RunEv,vector<DPS::Producer::RunEv>,Prio> RunQueue;
RunQueue _rq;
typedef list<DPS::Producer::RunEvInfo_var> RunEvInfo;
RunEvInfo _rei;
typedef list<DPS::Client::ActiveClient_var> ACL;
ACL  _acl;
ACL  _asl;
public:
AMSServer(int argc, char *argv[]);
static AMSServer* & Singleton(){return _Head;};
const CORBA::ORB_var & getOrb()const {return _orb;}
void init();
void run();
};



#endif

