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
class AMSEvent: public AMSNode{
private:
integer _run;
integer _runtype;
static integer SRun;
static AMSEvent * _Head;
static AMSNodeMap EventMap;
time_t _time;
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
void _reamsinitrun();
void _retkinitrun();
void _retrdinitrun();
void _rectcinitrun();
void _retofinitrun();
void _reaxinitrun();
void _siamsinitevent();
void _signinitevent();
void _sitkinitevent();
void _sitrdinitevent();
void _sictcinitevent();
void _sitofinitevent();
void _reamsinitevent();
void _retkinitevent();
void _retrdinitevent();
void _rectcinitevent();
void _retofinitevent();
void _reaxinitevent();
void _siamsevent();
void _reamsevent();
void _sitkevent();
void _sitofevent();
void _sitrdevent();
void _sictcevent();
void _retkevent();
void _retofevent();
void _retrdevent();
void _rectcevent();
void _reaxevent();
AMSlink * _getheadC( AMSID id, integer sorted=0);
integer _setheadC( AMSID id, AMSlink * p);
AMSContainer * _getC(AMSID id);
public:
AMSEvent(AMSID id, integer run, integer runtype,time_t time ):AMSNode(id),_run(run),
_time(time), _runtype(runtype){_Head=this;}
~AMSEvent(){_Head=0;}
static AMSEvent * gethead()  {return _Head;}
static integer debug;
AMSlink * getheadC(char name[], integer id, integer sorted=0)
{return _getheadC(AMSID(name,id),sorted);}
AMSlink * getheadC( AMSID id, integer sorted=0){return _getheadC(id,sorted);}
integer setheadC(char name[], integer id, AMSlink *p)
{return _setheadC(AMSID(name,id),p);}
integer setheadC( AMSID id, AMSlink *p){return _setheadC(id,p);}
AMSContainer * getC(char name[], integer id){return _getC(AMSID(name,id));}
AMSContainer * getC( AMSID id){return _getC(id);}
static void  sethead(AMSEvent* head) 
{ _Head=head;if(_Head)AMSEvent::EventMap.map(*_Head);}
void init();
void write();
void copy();
void printA(integer debugl=0);
void event();
//+
integer getEvent() {return _id;}
//-
integer addnext(AMSID id, AMSlink * p);
integer getrun() const{return _run;}
integer& setrun() {return _run;}
integer getruntype() const{return _runtype;}
integer& setruntype() {return _runtype;}
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
};
#endif
