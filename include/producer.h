#ifndef __AMSPRODUCER__
#define __AMSPRODUCER__
#include <typedefs.h>
#include <astring.h>
#include <list>
#include <fstream.h>
#include <server-cpp-stubs.hh>
#include <orb/orbitcpp_exception.hh>
class AMSProducer{
protected:
int _debug;
fstream _fbin;
DPS::Client::CID_var _pid;
typedef list<DPS::Producer_var> PList;
PList _plist;
static AMSProducer * _Head;
void _openLogFile();
void _getpidhost(uinteger pid);
class Error{
protected:
int _severity;
AString _message;
public:
Error(const char * message, int severity):_severity(severity),_message(message){};
const char* getMessage() const {return (const char*)_message;}
int getSeverity() const {return _severity;}
};
public:
AMSProducer(int argc,  char * argv[], int debug) throw (AMSProducer::Error);
void Message(const char * ch, int stream=0);
static AMSProducer* gethead(){return _Head;}
int  Debug() const{return _debug;}
void getTDVTable(uinteger size, uinteger * db[5]);
void getTDV(const char * tdv,uinteger Tag, uinteger Insert, uinteger Begin, uinteger End);  
void getRunEventinfo();
void sendCurrentRunInfo();
void getASL();
void sendSelfId();
void sendNtupleEnd();
void sendRunEnd();
};

#endif
