#ifdef __CORBA__
#ifndef __AMSPRODUCER__
#define __AMSPRODUCER__
#include <daqevt.h>
#include <client.h>
#include <node.h>
class AMSProducer: public AMSClient, public AMSNode{
protected:

typedef list<DPS::Producer_var> PList;
PList _plist;
static AMSProducer * _Head;
DPS::Producer::CurrentInfo _cinfo;
DPS::Producer::DST   _ntend;
DPS::Producer::RunEvInfo_var   _reinfo;
float _T0;
void _init(){};
public:

AMSProducer(int argc,  char * argv[], int debug) throw (AMSClientError);
~AMSProducer();
static AMSProducer* gethead(){return _Head;}
void getTDVTable(uinteger size, uinteger * db[5]);
void getTDV(const char * tdv,uinteger Tag, uinteger Insert, uinteger Begin, uinteger End);  
void AddEvent();
void UpdateARS();
void getRunEventInfo();
void sendCurrentRunInfo();
void getASL();
void sendSelfId();
void sendNtupleStart(int run, int first,time_t begin);
void sendNtupleEnd(const char * name, int entries, int last, time_t end, bool suc);
void sendRunEnd(DAQEvent::InitResult res);
virtual void Exiting(const char * message=0);
};

#endif
#endif
