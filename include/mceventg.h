// Author V. Choutko 24-may-1996
// 
// Oct 02, 1996. ak. add set/getNumbers, rearrange class member functions
//                   add next()
// Oct  04, 1996.  ak _ContPos is moved to AMSLink
//                    implement friend class
// Feb  10, 1997. ak. AMSIOD - friend class
//
// Last edit : Feb 10, 1997. ak. 
//
#ifndef __AMSMCEVENTG__
#define __AMSMCEVENTG__
#include <point.h>
#include <cern.h>
#include <link.h>
#include <io.h>
#include <time.h>
class orbit{
public:
integer Nskip;
number AlphaSinThetaMax;
number AlphaSpeed;
number ThetaI;
number PhiI;
number PhiZero;
number PolePhi;
number PoleTheta;
number EarthSpeed;
number FlightTime;
tm Begin;
tm End;
};
class AMSmceventg: public AMSlink {
private:

AMSPoint _coo;
AMSDir   _dir;

number _mom;
number _mass;
number _charge;
integer _ipart;
integer _seed[2];
integer _nskip;
static AMSPoint _coorange[2];
static AMSPoint _dirrange[2];
static number _momrange[2];
static integer _fixedpoint;
static integer _fixedplane;
static integer _fixeddir;
static integer _fixedmom;
static number _albedorate;
static number _albedocz;
static number _planesw[6];
void _printEl(ostream & stream);
void _copyEl();
void _writeEl();
public:
static orbit Orbit;
integer getseed(integer i)const{return (i>=0 && i<2) ? _seed[i]: 0;}
void setseed(integer seed[2]){_seed[0]=seed[0];_seed[1]=seed[1];}
static integer debug;
AMSmceventg(integer seed[2]){_next=0;_nskip=0;setseed(seed);}
AMSmceventg( const AMSIO & io);
~AMSmceventg(){}
void init(integer);
void run(integer ipart);
void run();
void InitSeed();
void gener();
integer accept();
integer EarthModulation();
static void setcuts( geant [],geant[],geant[],integer,geant,geant);
static void setspectra(integer begind, integer begint, integer endd, 
integer endt, integer ipart, integer low);
static integer _hid;
AMSmceventg *  next(){return (AMSmceventg*)_next;}
//+
AMSmceventg(){_next=0;}
#ifdef __DB__
   friend class AMSmceventgD;
   friend class AMSmceventD;
#endif
};
#endif
