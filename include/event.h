// Author V. Choutko 24-may-1996
// June 12, 1996. ak. add getEvent function
// Oct   1, 1997. ak. add TDV related functions
//
// Last Edit : Oct 2, 1997. ak.
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
  class ShuttlePar{
  public:
   geant StationR;
   geant StationTheta;
   geant StationPhi;
   geant GrMedPhi;
   geant StationYaw;
   geant StationPitch;
   geant StationRoll;
   geant StationSpeed;
   geant SunR;
   geant VelTheta;
   geant VelPhi;
   time_t Time;
  };
  class BeamPar{
   public:
    uinteger RunTag;   // runTag
    time_t Time;     // UNIX sec, of the manipulator PC
    float X;     //cm, the beam cross in AMS coo system
    float Y;     //cm
    float Z;     //cm
    float Theta; //rad [0-Pi], direction of the beam (Pi- from z-direction)
    float Phi;   //rad [0-2Pi]
    uinteger  Pid;   //Beam particle ID  == Geant One
    float Mom;   //Beam particle momentum (GeV/c) - 0-default
    float Intens;//Beam intensity (part/sec), 0-default
    uinteger  Cond;  //  Position no
    float BeamSize;  //beam size in cm;
 };

uinteger _status;
uinteger _run;
uinteger _runtype;
uinteger _Error;
geant _StationRad;    //cm 
geant _StationTheta; 
geant _StationPhi;   
geant _NorthPolePhi;
geant _Yaw;
geant _Pitch;
geant _Roll;
geant _StationSpeed;
geant _SunRad; 
geant _VelTheta; 
geant _VelPhi; 
time_t _time;
uinteger _usec;
static integer SRun;
static integer PosInRun;
static integer PosGlobal;
static AMSEvent * _Head;
static AMSNodeMap EventMap;
static ShuttlePar Array[60];
static BeamPar ArrayB[60];
void _copyEl();
void _writeEl();
void _init();
void _siamsinitrun();
void _signinitrun();
void _sitkinitrun();
void _sictcinitrun();
void _sitofinitrun();
void _siantiinitrun();

void _reamsinitrun();
void _retkinitrun();
void _rectcinitrun();
void _retofinitrun();
void _reantiinitrun();
void _reaxinitrun();
void _siamsinitevent();
void _signinitevent();
void _sitkinitevent();
void _sictcinitevent();
void _sitofinitevent();
void _siantiinitevent();
void _reamsinitevent();
void _retkinitevent();
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
void _sictcevent();
void _sidaqevent();
void _redaqevent();
void _retkevent(integer refit=0);
void _retofevent();
void _reantievent();
void _retrigevent();
void _rectcevent();
void _reaxevent();
void _caamsinitevent();
void _catkinitevent();
void _cactcinitevent();
void _catofinitevent();
void _cantinitevent();
void _caaxinitevent();
void _caamsevent();
void _catkevent();
void _catofevent();
void _cantievent();
void _cactcevent();
void _caaxevent();
void _validate(int i=0);
void _collectstatus();

void _startofrun();
static void _endofrun();

//+
void _validateDB();
//-
static const int16u _HDID;
AMSlink * _getheadC( AMSID id, integer sorted=0);
AMSlink * _getlastC( AMSID id);
integer _setheadC( AMSID id, AMSlink * p);
void _findC(AMSID & id);
AMSContainer * _getC(AMSID id);
public:
AMSEvent(AMSID id, integer run, integer runtype,time_t time,
uinteger usec,geant pole, geant stationT, geant stationP, geant StationR=666000000,geant yaw=0,geant pitch=0,geant roll=0,geant StationS=1.16e-3, geant SunR=0,geant SunT=0, geant SunP=0):AMSNode(id),_run(run),_status(0),
_time(time), _usec(usec),_runtype(runtype),_NorthPolePhi(pole),_StationPhi(stationP),_Roll(roll),_Yaw(yaw),_StationRad(StationR),_Pitch(pitch),_StationSpeed(StationS),_StationTheta(stationT),_SunRad(SunR),_VelTheta(SunT),_VelPhi(SunP),_Error(0){_Head=this;}
AMSEvent(AMSID id, integer run, integer runtype, time_t time, uinteger usec):AMSNode(id),_run(run),
   _runtype(runtype), _time(time), _usec(usec),_status(0),_Error(0){
   _Head=this;
}
~AMSEvent(){_Head=0;}
void _printEl(ostream & stream);
void loc2gl();
static void SetShuttlePar();
static AMSEvent * gethead()  {return _Head;}
static integer debug;
AMSlink * getheadC(char name[], integer id, integer sorted=0)
{return _getheadC(AMSID(name,id),sorted);}
integer buildC(char name[], integer par=0);
integer rebuildC(char name[], integer par=0);
static integer getSRun() {return SRun;}
 integer setbuilderC(char name[], pBuilder pb);
AMSlink * getlastC(char name[], integer id)
{return _getlastC(AMSID(name,id));}
AMSlink * getheadC( AMSID id, integer sorted=0){return _getheadC(id,sorted);}
integer setheadC(char name[], integer id, AMSlink *p)
{return _setheadC(AMSID(name,id),p);}
integer setheadC( AMSID id, AMSlink *p){return _setheadC(id,p);}
AMSContainer * getC(char name[], integer id){return _getC(AMSID(name,id));}
AMSContainer * getC( AMSID id){return _getC(id);}
integer getnC (char name[]);
void SetTimeCoo(integer rec=0);
void GetGeographicCoo(number & pole, number & theta, number &phi){
pole=_NorthPolePhi;theta=_StationTheta;phi=_StationPhi;}
static void  sethead(AMSEvent* head) 
{ _Head=head;if(_Head)AMSEvent::EventMap.map(*_Head);}
integer removeC();
void Recovery();
void write(int trig);
void copy();
void printA(integer debugl=0);
void event();
void seterror(){_Error=1;}
integer HasNoErrors(){return _Error==0;}
//+
uinteger getEvent() {return uinteger(_id);}
static integer _checkUpdate();
//-
integer addnext(AMSID id, AMSlink * p);
integer replace(AMSID id, AMSlink * p, AMSlink *prev);
uinteger getrun() const{return _run;}
uinteger  getstatus()const {return _status;}
geant getpitch() const{return _Pitch;}
geant getveltheta() const{return _VelTheta;}
geant getvelphi() const{return _VelPhi;}
geant getroll() const{return _Roll;}
geant getyaw() const{return _Yaw;}
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
static AMSID getTDVStatus();
// Interface with DAQ
static int16u  getdaqid(int btype){return 0x200 | btype <<13;}
static integer checkdaqid(int16u id);
static int16u  getdaqidSh(){return 1 | 14<<9;}
static integer checkdaqidSh(int16u id);
static void buildrawSh(integer length, int16u *p);
static void buildraw(integer length, int16u *p, uinteger &run, uinteger &event,
uinteger & runtype, time_t & time, uinteger & usec); 
static integer getmaxblocks(){return 1;}
static integer getmaxblocksSh(){return 1;}
static integer calcdaqlength(integer i){return 1+2+2+2+4;}
static integer calcdaqlengthSh(integer i){return 21;}
static integer calcTrackerHKl(integer i);
static void builddaq(integer i, integer length, int16u *p);
static void builddaqSh(integer i, integer length, int16u *p);
static void buildTrackerHKdaq(integer i, integer length, int16u *p);

friend class AMSJob;
};
#endif
