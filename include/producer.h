//  $Id: producer.h,v 1.27 2012/05/09 16:24:45 choutko Exp $
#ifdef __CORBA__
#ifndef __AMSPRODUCER__
#define __AMSPRODUCER__
#include "daqevt.h"
#include "client.h"
#include "node.h"
#include "timeid.h"
#include <sys/time.h>
class AMSProducer: public AMSClient, public AMSNode{
protected:

typedef list<DPS::Producer_var> PList;
PList _plist;
static AMSProducer * _Head;
static AString * _dc;
bool _OnAir;
bool _FreshMan;
bool _Local;
bool _Solo;
bool _RemoteDST;
bool _Transfer;
float _CPUMipsTimeSpent;
DPS::Producer::CurrentInfo _cinfo;
DPS::Producer::DST   _ntend[3];  // 0 - ntuple 1 -root 2 - rawfile(mc)
DPS::Producer::DST   _evtag;
DPS::Producer::RunEvInfo_var   _reinfo;
DPS::Producer::DSTInfo_var   _dstinfo;
double _ST0;
float _T0;
int   _FreeSpace;
void _init(){};
public:
bool & Transfer(){return _Transfer;}
AMSProducer(int argc,  char * argv[], int debug) throw (AMSClientError);
~AMSProducer();
static AMSProducer* gethead(){return _Head;}
static  const char* GetDataCards()  {return (const char*)(*_dc);}
bool SetDataCards(); 
DPS::Producer::DST * getdst(DPS::Producer::DSTType type);
bool & OnAir(){return _OnAir;}
bool  IsLocal(){return _Local;}
bool  IsSolo(){return _Solo;}
int FreeSpace()const{return _FreeSpace;}
bool getior(const char * getiorvar);
void AddEvent();
void UpdateARS();
void getRunEventInfo(bool solo=false);
void sendCurrentRunInfo(bool force=false);
void getASL();
bool SendFile(const char * Remote, const char *Local,bool erase=true);
void sendid(int cpuf=0);
void SendTimeout(int cpuf);
void sendSelfId();
void sendDSTInfo();
bool Progressing();
void sendNtupleStart(DPS::Producer::DSTType type,const char * name,int run, int first,time_t begin);
void sendNtupleUpdate(DPS::Producer::DSTType type);
void sendNtupleEnd(DPS::Producer::DSTType type,int entries, int last, time_t end, bool suc);
void sendRunEnd(DAQEvent::InitResult res);
void sendRunEndMC();
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
