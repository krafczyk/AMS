// Author V. Choutko 24-may-1996
 
#ifndef __AMSJOB__
#define __AMSJOB__
#include <typedefs.h>
#include <node.h>
#include <snode.h>
#include <gvolume.h>
#include <gmat.h>
#include <iomanip.h>
#include <amsgobj.h>
const integer maxtrig=20;
class AMSJob : public AMSNode{
private:
integer _jobtype;  // 0 == simulation
char _Setup[256];

char _TriggerC[maxtrig][256];
integer _TriggerI;
integer _TriggerN;
static AMSJob* _Head;
void _init(){}
void _siamsdata();
void _sitkdata();
void _signdata();
void _sitrddata();
void _sictcdata();
void _sitofdata();
void _reamsdata();
void _retkdata();
void _retrddata();
void _rectcdata();
void _retofdata();
void _reaxdata();
void _siamsinitjob();
void _sitkinitjob();
void _signinitjob();
void _sitrdinitjob();
void _sictcinitjob();
void _sitofinitjob();
void _reamsinitjob();
void _retkinitjob();
void _regninitjob();
void _retrdinitjob();
void _rectcinitjob();
void _retofinitjob();
void _reaxinitjob();
static AMSNodeMap JobMap;
public:
AMSJob(AMSID id=0,integer jobtype=0):AMSNode(id),_jobtype(jobtype)
{_Setup[0]='\0';_TriggerC[0][0]='\0';_TriggerI=1;_TriggerN=0;cout <<
"AMS Software version "<<setw(4)<<AMSID::getversion()<<endl;}
static AMSJob* gethead(){return _Head;}
static AMSJob * & sethead(){return _Head;}
void setsetup(char * setup);
void settrigger(char * triggerC, integer triggerN, integer triggerI);
inline char * getsetup(){return _Setup;}
inline integer gettriggerI(){return _TriggerI;}
inline integer gettriggerN(){return _TriggerN;}
inline char * gettriggerC(integer n){return n>=0 && n<_TriggerN ? _TriggerC[n]:0;}
integer getjobtype() const{return _jobtype;}
integer&  setjobtype() {return _jobtype;}
AMSNode * getnodep(AMSID  id) const{return JobMap.getp(id);}
AMSgvolume * getgeom(AMSID id=0);
AMSgvolume * getgeomvolume(AMSID id){return   (AMSgvolume*)AMSgObj::GVolMap.getp(id);}
AMSgmat * getmat(AMSID id=0);
AMSgtmed * getmed(AMSID id=0);
void data();
void udata();
void init();
void printJ(ostream & stream);
static integer debug;
static void map(){JobMap.map(*_Head);}
};
#endif
