// Author V. Choutko 24-may-1996
// Last Edit June 12, 1996. ak. add getEvent function
// 
#ifndef __AMSEVENT__
#define __AMSEVENT__
#include <job.h>
#include <typedefs.h>
#include <upool.h>
#include <node.h>
#include <snode.h>
#include <link.h>
#include <cont.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <mceventg.h>
#include <daqevt.h>
class AMSEvent: public AMSNode{
private:
uinteger _run;
uinteger _runtype;
number _NorthPolePhi;
number _StationTheta;
number _StationPhi;
time_t _time;
static integer SRun;
static AMSEvent * _Head;
static AMSNodeMap EventMap;
void _printEl(ostream & stream);
void _copyEl();
void _writeEl();
void _init();
void _siamsinitrun();
void _signinitrun();
void _sitkinitrun();
void _sitrdinitrun();
void _sictcinitrun();
void _sitofinitrun();
void _siantiinitrun();

void _reamsinitrun();
void _retkinitrun();
void _retrdinitrun();
void _rectcinitrun();
void _retofinitrun();
void _reantiinitrun();
void _reaxinitrun();
void _siamsinitevent();
void _signinitevent();
void _sitkinitevent();
void _sitrdinitevent();
void _sictcinitevent();
void _sitofinitevent();
void _siantiinitevent();
void _reamsinitevent();
void _retkinitevent();
void _retrdinitevent();
void _rectcinitevent();
void _retofinitevent();
void _reantiinitevent();
void _reaxinitevent();
void _retriginitevent();
void _siamsevent();
void _redaqinitevent();
void _reamsevent();
void _sitrigevent();
void _sitkevent();
void _sitofevent();
void _siantievent();
void _sitrdevent();
void _sictcevent();
void _sidaqevent();
void _redaqevent();
void _retkevent(integer refit=0);
void _retofevent();
void _reantievent();
void _retrdevent();
void _rectcevent();
void _reaxevent();
void _caamsinitevent();
void _catkinitevent();
void _catrdinitevent();
void _cactcinitevent();
void _catofinitevent();
void _caaxinitevent();
void _caamsevent();
void _catkevent();
void _catofevent();
void _catrdevent();
void _cactcevent();
void _caaxevent();
void _validate();
static const int16u _HDID;
AMSlink * _getheadC( AMSID id, integer sorted=0);
AMSlink * _getlastC( AMSID id);
integer _setheadC( AMSID id, AMSlink * p);
AMSContainer * _getC(AMSID id);
public:
AMSEvent(AMSID id, integer run, integer runtype,time_t time,
number pole, number stationT, number stationP ):AMSNode(id),_run(run),
_time(time), _runtype(runtype),_NorthPolePhi(pole),_StationPhi(stationP),
_StationTheta(stationT){_Head=this;}
AMSEvent(AMSID id, integer run, integer runtype, time_t time=0):AMSNode(id),_run(run),
   _runtype(runtype), _time(time){
   SetTimeCoo();
   _Head=this;
}
~AMSEvent(){_Head=0;}
static AMSEvent * gethead()  {return _Head;}
static integer debug;
AMSlink * getheadC(char name[], integer id, integer sorted=0)
{return _getheadC(AMSID(name,id),sorted);}
AMSlink * getlastC(char name[], integer id)
{return _getlastC(AMSID(name,id));}
AMSlink * getheadC( AMSID id, integer sorted=0){return _getheadC(id,sorted);}
integer setheadC(char name[], integer id, AMSlink *p)
{return _setheadC(AMSID(name,id),p);}
integer setheadC( AMSID id, AMSlink *p){return _setheadC(id,p);}
AMSContainer * getC(char name[], integer id){return _getC(AMSID(name,id));}
AMSContainer * getC( AMSID id){return _getC(id);}
integer getnC (char name[]);
void SetTimeCoo();
void GetGeographicCoo(number & pole, number & theta, number &phi){
pole=_NorthPolePhi;theta=_StationTheta;phi=_StationPhi;}
static void  sethead(AMSEvent* head) 
{ _Head=head;if(_Head)AMSEvent::EventMap.map(*_Head);}
integer removeC();
void Recovery();
void write();
void copy();
void printA(integer debugl=0);
void event();
//+
uinteger getEvent() {return uinteger(_id);}
//-
integer addnext(AMSID id, AMSlink * p);
integer replace(AMSID id, AMSlink * p, AMSlink *prev);
uinteger getrun() const{return _run;}
uinteger& setrun() {return _run;}
uinteger getruntype() const{return _runtype;}
uinteger& setruntype() {return _runtype;}
time_t gettime(){return _time;}
time_t& settime(){return _time;}
void * operator new(size_t t, void *p) {return p;}
void * operator new(size_t t) {return UPool.insert(t);}
void operator delete(void *p)
  {if(p){((AMSEvent*)p)->~AMSEvent();p=0;UPool.udelete(p);}}

inline AMSNode * getp(const AMSID & id, char hint=0) const{
  return AMSEvent::EventMap.getp(id,hint);
}
inline void printH(){
  AMSEvent::EventMap.print();
}

// Interface with DAQ
static int16u  getdaqid(){return 0x200;}
static integer checkdaqid(int16u id){return id==getdaqid();}
static void buildraw(integer length, int16u *p, uinteger &run, uinteger &event,
uinteger & runtype, time_t & time); 
static integer getmaxblocks(){return 1;}
static integer calcdaqlength(integer i){return 1+2+1+2+3;}
static void builddaq(integer i, integer length, int16u *p);


};
#endif
