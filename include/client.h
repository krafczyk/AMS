#ifndef __AMSCLIENT__
#define __AMSCLIENT__
#include <typedefs.h>
#include <astring.h>
#include <list>
#include <fstream.h>
#include <server-cpp-stubs.hh>
#include <orb/orbitcpp_exception.hh>
#include <strstream.h>
class AMSClientError{
protected:
 DPS::Client::ClientExiting _exit;
AString _message;
public:
const char* getMessage() const {return (const char*)_message;}
DPS::Client::ClientExiting ExitReason() const {return _exit;}
AMSClientError(const char * message,DPS::Client::ClientExiting exit=DPS::Client::CInExit):
_message(message),_exit(exit){};
};
class AMSClient{
protected:
 bool _ExitInProgress;
 AMSClientError  _error;
 int _debug;
 CORBA::ORB_var _orb;
 fstream _fbin;
 DPS::Client::CID _pid;
 DPS::Client::ClientExiting _exit;
 void _openLogFile(char * prefix);
 bool _getpidhost(uinteger pid, const char* iface=0);
 static char _streambuffer[1024]; 
 static ostrstream _ost;
public:
AMSClient(int debug=0):_debug(debug),_error(" "),_ExitInProgress(false){}
virtual ~AMSClient(){};
AMSClientError & Error(){return _error;}
int  Debug() const{return _debug;}
const DPS::Client::CID & getcid()const {return _pid;}
DPS::Client::ClientExiting & ExitReason(){return _exit;}
void FMessage(const char * ch, DPS::Client::ClientExiting res);
void EMessage(const char * ch);
void IMessage(const char * ch);
virtual void Exiting(const char * message=0)=0;
static char * print(const DPS::Client::ActiveClient & a, const char *m=" ");
static char * print(const DPS::Client::CID & a, const char *m=" ");
static ostream &  print(const DPS::Client::CID & a, ostream & o);
static char * print(const DPS::Client::ActiveHost & a, const char *m=" ");
static char * print(const DPS::Producer::RunEvInfo & a, const char *m=" ");
static char * print(const DPS::Server::CriticalOps & a,const  char *m=" ");
static char * print(const DPS::Producer::CurrentInfo & a, const char *m=" ");
static ostream & print(const DPS::Producer::CurrentInfo & a, ostream & o);
static char * print(const DPS::Producer::DST & a, const char *m=" ");
static char * CS2string(DPS::Client::ClientStatus a);
static char * CT2string(DPS::Client::ClientType a);
static char * CSE2string(DPS::Client::ClientExiting);
static char * HS2string(DPS::Client::HostStatus a);
static char * RS2string(DPS::Producer::RunStatus a);
static char * DSTS2string(DPS::Producer::DSTStatus a);
static char * OPS2string(DPS::Server::OpType a);
};

#endif
