//  $Id: producer.h,v 1.12 2001/03/02 10:40:56 choutko Exp $
#ifdef __CORBA__
#ifndef __AMSPRODUCER__
#define __AMSPRODUCER__
#include <daqevt.h>
#include <client.h>
#include <node.h>
#include <timeid.h>
#include <sys/time.h>
class AMSProducer: public AMSClient, public AMSNode{
protected:

typedef list<DPS::Producer_var> PList;
PList _plist;
static AMSProducer * _Head;
DPS::Producer::CurrentInfo _cinfo;
DPS::Producer::DST   _ntend[2];  // 0 - ntuple 1 -root
DPS::Producer::DST   _evtag;
DPS::Producer::RunEvInfo_var   _reinfo;
DPS::Producer::DSTInfo_var   _dstinfo;
double _ST0;
float _T0;
void _init(){};
public:

AMSProducer(int argc,  char * argv[], int debug) throw (AMSClientError);
~AMSProducer();
static AMSProducer* gethead(){return _Head;}
DPS::Producer::DST * getdst(DPS::Producer::DSTType type);
void AddEvent();
void UpdateARS();
void getRunEventInfo();
void sendCurrentRunInfo(bool force=false);
void getASL();
void sendSelfId();
void sendDSTInfo();
bool Progressing();
void sendNtupleStart(DPS::Producer::DSTType type,const char * name,int run, int first,time_t begin);
void sendNtupleUpdate(DPS::Producer::DSTType type);
void sendNtupleEnd(DPS::Producer::DSTType type,int entries, int last, time_t end, bool suc);
void sendRunEnd(DAQEvent::InitResult res);
void sendEventTagEnd(const char *name, uinteger run,time_t insert, time_t begin,time_t end,uinteger first,uinteger last,integer nelem, bool fail);  
void sendEventTagBegin(const char * name,uinteger run,uinteger first);  
bool getTDV(AMSTimeID * tdv, int id);
bool getSplitTDV(AMSTimeID * tdv, int id);
bool sendTDV(const AMSTimeID * tdv);
virtual void Exiting(const char * message=0);
void InitTDV(uinteger run);
};

#endif
#endif
