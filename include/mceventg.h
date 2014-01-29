//  $Id: mceventg.h,v 1.54 2014/01/29 17:48:16 choutko Exp $
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
#include "point.h"
#include "cern.h"
#include "link.h"
#include "io.h"
#include <time.h>
#include "astro.h"
class G4Track;
class orbit{
public:
integer Nskip;
integer Nskip2;
integer Ntot;
number AlphaTanThetaMax;
number AlphaSpeed;
number AlphaAltitude;
number ThetaI;
number PhiI;
number PhiZero;
number PolePhi;
number PolePhiStatic;
number PoleTheta;
number EarthSpeed;
number NodeSpeed; //ISN  
number InitTime;
number FlightTime;
number EarthR;
number DipoleR;
number DipoleTheta;
number DipolePhi;
AMSDir Axis;
tm Init;
tm Begin;
tm End;
integer UpdateOrbit(number curtime, geant & ThetaS, geant & PhiS, geant & PolePhi, number & RaS, number & DecS, number & GLatS, number & GLongS, time_t & time); 
void UpdateOrbit(number theta,number phi, integer dir);
void UpdateAxis(number veltheta, number velphi, number theta, number phi);
orbit(geant ThetaI=0,geant PhiI=0, geant PolePhi=0, integer Dir=1);
~orbit(){cout <<"Nskip "<<Nskip<<" Nskip2 "<<Nskip2<<endl;}
};

class AMSmctrack: public AMSlink{
protected:
number _radl;
number _absl;
number _pos[3];
char _vname[4];
void _copyEl(){};
void _writeEl();
void _printEl(ostream & stream){};
public:
AMSmctrack(number radl,number absl, float pos[3], char name[]):AMSlink(),_radl(radl),_absl(absl){
 for (int i=0;i<3;i++)_pos[i]=pos[i];
 for (int i=0;i<4;i++)_vname[i]=name[i];
}
#ifdef __WRITEROOT__
 friend class MCTrackR;
#endif
AMSPoint getcoo() {return AMSPoint(_pos);}
};

class AMSmceventg: public AMSlink {
private:
typedef struct tbpos{
  float X;
  float Y;
  float Z;
  float DX;
  float DY;
  float DZ;
} tbpos;

AMSPoint _coo;
AMSDir   _dir;

number _mom;
number _mass;
number _charge;
number _delay;
integer _ipart;
integer _trkid;
integer _parentid;
integer _seed[2];
integer _nskip;
short int _tbline;
short int _parinfo;
static AMSPoint *_r_c;
static AMSDir *_dir_c;
static AMSPoint _coorange[2];
static AMSPoint _dirrange[2];
static number _momrange[2];
static number _Livetime;
static number _DT;
static integer _fixedpoint;
static integer _fixedplane;
static integer _fixeddir;
static integer _fixedmom;
static number _albedorate;
static number _albedocz;
static number _planesw[6];
static number _flux[30];
static geant* _spectra[30];
static int _particle[30];
static int _nucleons[30];
integer operator < (AMSlink & o) const {
 return dynamic_cast<AMSmceventg*>(&o) ? _mom> ((AMSmceventg*)(&o))->_mom:0;
}
void _copyEl();
void _writeEl();
void _printEl(ostream & stream);
public:
static number LiveTime(){return _Livetime;}
static number DT(){return _DT;}  // time difference in to cons events usec
static bool CalcLiveTime(int icase);
static void SaveSeeds();
static void RestoreSeeds();
static void PrintSeeds(ostream & o);
static void SetSeed(int seed);
static orbit Orbit;
AMSPoint getcoo()const {return _coo;}
integer getseed(integer i)const{return (i>=0 && i<2) ? _seed[i]: 0;}
void setseed(integer seed[2]){_seed[0]=seed[0];_seed[1]=seed[1];}
static integer fixedmom(){return _fixedmom;}
static integer fixeddir(){return _fixeddir;}
static void FillMCInfo();
static int    readposfromfile(char* filename,vector<tbpos> &poslist);
static double extract(float m, float s);
#ifdef __G4AMS__
static void FillMCInfoG4( G4Track const * );
#endif


#ifdef __AMSVMC__
static void FillMCInfoVMC(int vmc_ipart,int vmc_inwvol,int CurrentLevel,const char* CurrentVolName,float* vmc_vect);
#endif

static integer debug;

static integer Out(integer status=0);
AMSmceventg(integer ip, geant mom, const AMSPoint & coo, const AMSDir & dir, integer nskip=0);
AMSmceventg(integer ip, integer trkid, integer parentid, geant mom, const AMSPoint & coo, const AMSDir & dir, integer parinfo,float mass, float charge, integer nskip=0);
 AMSmceventg(integer seed[2]){_next=0;_nskip=0;setseed(seed);_tbline=0;_trkid=1;_parentid=0;}
AMSmceventg( const AMSIO & io);
~AMSmceventg(){}
void init(integer id);
void run(integer ipart);
void run(integer ipart, integer ipat);
void run();


#ifdef __AMSVMC__

 void runvmc(integer ipart,TVirtualMCStack* fStack,int vmc_version);
 void runvmc(integer ipart, integer ipat,int vmc_version);
 void runvmc(TVirtualMCStack* fStack,int vmc_version);

#endif


#ifdef __G4AMS__
void runG4(integer ipart=0);
#endif
void InitSeed();
void gener();
integer fastcheck(geant x, geant y, geant z, geant the, geant phi);
number getcharge()const {return _charge;}
number getdelay()const {return _delay;}
void setdelay(number delay){_delay=delay;}
void gendelay();
AMSDir getdir()const {return _dir;}
number getmom()const {return _mom;}
void setmom(number mom) { _mom=mom;}
integer & pid() {return _ipart;}
bool Primary();
number getmass() const {return _mass;}
integer accept();
bool SpecialCuts(integer cut);
integer acceptio();
integer EarthModulation();
static void setcuts( geant [],geant[],geant[],integer,geant,geant);
static void setspectra(integer begind, integer begint, integer endd, 
integer endt, integer ipart, integer low);
//  Nuclear natural fluxes
static void NaturalFlux_spline(double x[], double y[], int n, double yp1, double ypn, double y2[]);
static void NaturalFlux_splint(double xa[], double ya[], double y2a[], int n, double x, double *y);
static double NaturalFlux_TrueFluxFun_CubSpline(double * x, double * par);
static double NaturalFlux(int gpid, double trueRigidity, const char *  acceptance, int &error);

static void lookupsourcesp(integer sourceid,number & constant,number & index); //ISN 
static void lookupsourcecoo(integer sourceid, geant & rasource, geant & decsource); //ISN
static integer _hid;
static void endjob();
AMSmceventg *  next(){return (AMSmceventg*)_next;}
 // Interface with DAQ

 static int16u getdaqid(){return 24;}
 static integer checkdaqid(int16u id);
 static integer calcdaqlength(integer i);
 static integer getmaxblocks(){return 1;}
 static void builddaq(integer i, integer n, int16u *p);
 static void buildraw(integer n, int16u *p);
AMSmceventg(){_next=0;}
#ifdef __WRITEROOT__
   friend class MCEventgR;
#endif
};
#endif
