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
DPS::Producer::RunEvInfo_var   _reinfo;
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
void sendNtupleStart(const char * name,int run, int first,time_t begin);
void sendNtupleEnd(int entries, int last, time_t end, bool suc);
void sendRunEnd(DAQEvent::InitResult res);
bool getTDV(AMSTimeID * tdv, int id);
bool sendTDV(const AMSTimeID * tdv);
virtual void Exiting(const char * message=0);
void InitTDV(uinteger run);
};

#endif
#endif
