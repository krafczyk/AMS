// Author V. Choutko 24-may-1996
//
// Mar 24, 1997. ak. add _eventRtype 
//                       set it when read event from the database
//                       functions to check it isMCBanks(), isRecoBanks(),
//                       isRawBanks()
// Oct  1, 1997. ak. tdv dbase related modifications.
// Nov   , 1997. ak. 
//
// Last Edit: Nov 24, 1997.
//
// 21 Feb 1998: D. Casadei added AMSJob::_retofonlineinitjob();
#ifndef __AMSJOB__
#define __AMSJOB__
#include <typedefs.h>
#include <node.h>
#include <snode.h>
#include <gvolume.h>
#include <gmat.h>
#include <iomanip.h>
#include <amsgobj.h>
#include <timeid.h>
#include <commons.h>
#include <status.h>
#ifdef __DB__
#include <db_comm.h>
#endif

class AMSNtuple;
const integer maxtrig=20;
const integer maxtdv=255;
const integer maxtdvsize=256;

#ifdef __DB__
//+ ak. Oct 1, 1997.
// integer        ntdvNames;                       // number of TDV's types
// char           *tdvNameTab[maxtdv];             // TDV's nomenclature
class tdv_time {

  public :
   char*   _name;
   integer _id;
   time_t  _insert;
   time_t  _begin;
   time_t  _end;
   integer _size;
 
// methods
     integer getid() {return _id;}
     tdv_time() {_insert = _begin = _end = 0;}
gettime(time_t &insert, time_t &begin, time_t &end) {
    insert = _insert;
    begin  = _begin;
    end    = _end;
}
settime(time_t insert, time_t begin, time_t end) {
    _insert = insert;
    _begin  = begin;
    _end    = end;
}
setname(char* name) {if (name) {
                        _name = new char[strlen(name)+1];
                        strcpy (_name,name);
 }
}
char* getname() {return _name;}
//
};
//-
#endif

class AMSJob : public AMSNode{
private:
uinteger _jobtype;    // 0 == simulation
uinteger _eventRtype; // see eventR 
char _Setup[256];

char _TriggerC[maxtrig][256];
integer _TriggerI;
integer _TriggerN;
integer _TriggerOr;
char _TDVC[maxtdv][maxtdvsize];
integer _TDVN;
static AMSJob* _Head;
static AMSNtuple* _pntuple;
void _init(){};
void _sitrigdata();
void _siamsdata();
void _sitkdata();
void _signdata();
void _sitrddata();
void _sictcdata();
void _sitofdata();
void _siecaldata();
void _siantidata();
void _reamsdata();
void _retkdata();
void _remfdata();
void _retrddata();
void _rectcdata();
void _retofdata();
void _reantidata();
void _reecaldata();
void _reaxdata();
void _redaqdata();

void _siamsinitjob();
void _sitkinitjob();
void _sitriginitjob();
void _signinitjob();
void _sitrdinitjob();
void _sictcinitjob();
void _sitofinitjob();
void _siecalinitjob();
void _siantiinitjob();
void _redaqinitjob();
void _reamsinitjob();
void _remfinitjob();
void _retkinitjob();
void _regninitjob();
void _retrdinitjob();
void _rectcinitjob();
void _retofinitjob();
void _reecalinitjob();
void _retofonlineinitjob();
void _reantiinitjob();
void _reaxinitjob();
void _caamsinitjob();
void _catkinitjob();
void _catrdinitjob();
void _cactcinitjob();
void _catofinitjob();
void _cantinitjob();
void _caaxinitjob();
void _dbinitjob();
void _dbendjob();
void _tkendjob();
void _trdendjob();
void _ctcendjob();
void _tofendjob();
void _ecalendjob();
void _antiendjob();
void _axendjob();
void _timeinitjob();
public:
static AMSNodeMap JobMap;

#ifdef __DB__
//+ ak, Oct 1, 1997
static tdv_time*              _tdv;
static integer*               _ptr_start;
static integer*               _ptr_end;

static integer FindTheBestTDV(char* name, integer id, time_t timeV, integer &S, 
                              time_t &I, time_t &B, time_t &E);
static integer FillJobTDV(integer nobj, tdv_time* tdv);
static integer SetTDVPtrs(integer start[], integer end[]);
       integer FillTDVTable();
//-
#endif

AMSJob(AMSID id=0,uinteger jobtype=0);
~AMSJob();
static AMSJob* gethead(){return _Head;}
void setsetup(char * setup);
void settrigger(char * triggerC, integer triggerN, 
integer triggerOr);
void settdv(char * TDVC, integer TDVN);
inline char * getsetup(){return _Setup;}
inline integer gettriggerOr(){return _TriggerOr;}
inline integer gettriggerI(){return _TriggerI;}
inline integer gettriggerN(){return _TriggerN;}
inline char * gettriggerC(integer n){return n>=0 && n<_TriggerN ? _TriggerC[n]:0;}
char * gettdvc(integer n){return n>=0 && n<_TDVN ? _TDVC[n]:0;}
integer gettdvn() const {return _TDVN;}
// bit fields
// cannot use directly bitfields due to big/little endian
//
const static uinteger Reconstruction;
const static uinteger RealData;
const static uinteger CTracker;
const static uinteger CTOF;
const static uinteger CAnti;
const static uinteger CCerenkov;
const static uinteger CMagnet;
const static uinteger CRICH;
const static uinteger CTRD;
const static uinteger CAMS;
const static uinteger Calibration;
const static uinteger MTracker;
const static uinteger MTOF;
const static uinteger MAnti;
const static uinteger MCerenkov;
const static uinteger MLVL1;
const static uinteger MLVL3;
const static uinteger MAxAMS;
const static uinteger MAll;
const static uinteger Monitoring;
const static uinteger Production;
uinteger isReconstruction(){return _jobtype & Reconstruction;}
uinteger isSimulation(){return !isReconstruction();}
uinteger isCalibration(){return _jobtype & Calibration;}
uinteger isMonitoring(){return _jobtype & Monitoring;}
uinteger isRealData(){return _jobtype & RealData;}
uinteger isMCData(){ return !isRealData();}
uinteger isProduction(){ return _jobtype & Production;}
uinteger jobtype() {return _jobtype;}
uinteger setjobtype(uinteger checker){return _jobtype | checker;}   

uinteger eventRtype()     { return _eventRtype;}
void     seteventRtype(integer eventR) {_eventRtype = eventR;}

#ifdef __DB__
uinteger isReadSetup() { return (_eventRtype/DBReadSetup)%2;}
uinteger isMCBanks()   { return (_eventRtype/DBWriteMC)%2;}
uinteger isRecoBanks() { return (_eventRtype/DBWriteRecE)%2;}
uinteger isRawBanks()  { return (_eventRtype/DBWriteRawE)%2;}
#endif

AMSNode * getnodep(AMSID  id) const{return JobMap.getp(id);}
AMSgvolume * getgeom(AMSID id=0);
AMSgvolume * getgeomvolume(AMSID id){return   (AMSgvolume*)JobMap.getp(id);}
AMSStatus * getstatustable();
AMSgmat * getmat(AMSID id=0);
AMSgtmed * getmed(AMSID id=0);
AMSNtuple * getntuple(AMSID id=0);
void data();
void udata();
void init();
void uhinit(integer run=0,integer pass=0);
void urinit(integer pass=0);
void uhend();
AMSTimeID * gettimestructure();
AMSNode * getaligstructure();
AMSTimeID * gettimestructure(const AMSID & id);
void printJ(ostream & stream);
static integer debug;
static void map(integer remap=0){remap==0?JobMap.map(*_Head):JobMap.remap();}
};
#endif
