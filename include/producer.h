#ifdef __CORBA__
#ifndef __AMSPRODUCER__
#define __AMSPRODUCER__
#include <daqevt.h>
#include <client.h>
#include <node.h>
#include <timeid.h>
class AMSProducer: public AMSClient, public AMSNode{
protected:

typedef list<DPS::Producer_var> PList;
PList _plist;
static AMSProducer * _Head;
DPS::Producer::CurrentInfo _cinfo;
DPS::Producer::DST   _ntend;
DPS::Producer::DST   _evtag;
DPS::Producer::RunEvInfo_var   _reinfo;
DPS::Producer::DSTInfo_var   _dstinfo;
float _T0;
void _init(){};
public:

AMSProducer(int argc,  char * argv[], int debug) throw (AMSClientError);
~AMSProducer();
static AMSProducer* gethead(){return _Head;}
void AddEvent();
void UpdateARS();
void getRunEventInfo();
void sendCurrentRunInfo();
void getASL();
void sendSelfId();
void sendDSTInfo();
void sendNtupleStart(const char * name,int run, int first,time_t begin);
void sendNtupleEnd(int entries, int last, time_t end, bool suc);
void sendRunEnd(DAQEvent::InitResult res);
void sendEventTagEnd(const char *name, uinteger run,time_t insert, time_t begin,time_t end,uinteger first,uinteger last,integer nelem, bool fail);  
void sendEventTagBegin(const char * name,uinteger run,uinteger first);  
bool getTDV(AMSTimeID * tdv, int id);
bool sendTDV(const AMSTimeID * tdv);
virtual void Exiting(const char * message=0);
void InitTDV(uinteger run);
};

#endif
#endif
