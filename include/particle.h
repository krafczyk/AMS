// V. Choutko 6-june-96
//
// July 13, 1996.  ak.  add _ContPos and functions get/setNumbers;
// Sep  10, 1996.  ak.  V1.25, get/setgpart()
// Sep  26, 1996.  ak.  V1.27a, put CTC class into ctc.h
//                      functions get/setNumbers are modified
// Oct  10, 1996.  ak   implementation of friend class
//
// Oct  10, 1996.  ak _ContPos is moved to AMSLink
//
#ifndef __AMSPARTICLE__
#define __AMSPARTICLE__
#include <link.h>
#include <trrec.h>
#include <tofrec.h>
#include <beta.h>
#include <charge.h>
#include <ctc.h>

class AntiMatter: public AMSlink{
protected: 
 integer _pid;
 void _copyEl(){}
 void _printEl(ostream & stream){ stream << " Pid " << _pid<<endl;}
 void _writeEl(){}
public:
 AntiMatter(integer pid=0):_pid(pid){}
};

class AMSParticle: public AMSlink{
protected:

  AMSCTCCluster * _pctc[2];  // pointers to ctc
  AMSBeta * _pbeta;          // pointer to beta 
  AMSCharge * _pcharge;      // pointer to charge
  AMSTrTrack * _ptrack;      // pointer to track;

  integer _GPart;        // Geant particle ID

  number  _Mass;
  number  _ErrMass;
  number  _Momentum;
  number  _ErrMomentum;
  number  _Charge;
  number  _Theta;
  number  _Phi;
  number  _ThetaGl;
  number  _PhiGl;
  number  _CutoffMomentum;
  AMSPoint _Coo;
  CTC    _Value[2];
  AMSPoint _TOFCoo[4];  
  AMSPoint _AntiCoo[2];  
  AMSPoint _TrCoo[6];  
  number   _Local[6];

// new
   
   integer _gpart[2];
   number _prob[2];
   number _fittedmom[2];

// tmp
   
   number _mom;
   number _emom;
   number _mass;
   number _beta;
   number _ebeta;

  void _loc2gl();
  void _copyEl();
  void _printEl(ostream & stream){ stream << " Mass "<<_Mass<<
  " Error Mass "<<_ErrMass<<" Momentum "<<_Momentum<<" ErrMomentum "<<
   _ErrMomentum<<" Charge "<<_Charge<<" Theta "<<_Theta<<" Phi"<<_Phi<<
   " Coo "<<_Coo<<" ThetaGl "<<_ThetaGl<<" PhiGl "<<_PhiGl<<endl;}

static void monit(number & a, number & b,number sim[], int & n, int & s, int & ncall){};
static void alfun(integer & n, number xc[], number & fc, AMSParticle * ptr);
 void _writeEl();
 static integer _partP[38];
  static void _build(number rid, number err, number charge,AMSBeta *p, 
  number &mass, number &emass, number &mom, number &emom);
public:
  AMSParticle *  next(){return (AMSParticle*)_next;}
  AMSParticle():   _pbeta(0), _pcharge(0), _ptrack(0)
 {
    for(int i=0;i<2;i++){
     _Value[i]=CTC(0.,0.,1.,AMSPoint());
     _pctc[i]=0;
    }
    for(i=0;i<4;i++)_TOFCoo[i]=AMSPoint(0,0,0);
    for(i=0;i<2;i++)_AntiCoo[i]=AMSPoint(0,0,0);
    for(i=0;i<6;i++){
     _TrCoo[i]=AMSPoint(0,0,0);
     _Local[i]=0;
    }    
 }
  AMSParticle(AMSBeta * pbeta, AMSCharge * pcharge, AMSTrTrack * ptrack,
  number mass, number errmass, number momentum, number errmomentum,
  number charge, number theta, number phi, AMSPoint coo): 
  _pbeta(pbeta), _pcharge(pcharge), _ptrack(ptrack), 
  _Mass(mass),
  _ErrMass(errmass), _Momentum(momentum), _ErrMomentum(errmomentum),
  _Charge(charge), _Theta(theta), _Phi(phi), _Coo(coo)
 {
    for(int i=0;i<2;i++){
     _Value[i]=CTC(0.,0.,1.,AMSPoint());
     _pctc[i]=0;
    }
    for(i=0;i<4;i++)_TOFCoo[i]=AMSPoint(0,0,0);
    for(i=0;i<2;i++)_AntiCoo[i]=AMSPoint(0,0,0);
    for(i=0;i<6;i++){
     _TrCoo[i]=AMSPoint(0,0,0);
     _Local[i]=0;
    }
 }

  void ctcfit(); // CTC fit
  void toffit(); // TOF fit
  void antifit(); // Anti fit
  void pid();   // particle identification
  void refit(int i=0); // refit if necessary;
  static integer build(integer refit=0);
  static void print();
  number getcutoffmomentum() const {return _CutoffMomentum;}
//+

   AMSPoint getcoo() {return _Coo;}
   AMSPoint gettofcoo(integer i) {return _TOFCoo[i];}
#ifdef __DB__
   friend class AMSParticleD;

   number  geterrmass() {return _ErrMass;}
   number  geterrmomentum() {return _ErrMomentum;}


#endif
AMSBeta*       getpbeta()       { return _pbeta;}
AMSCharge*     getpcharge()     { return _pcharge;}
AMSTrTrack*    getptrack()      { return _ptrack;}
AMSCTCCluster* getpctc(int n)   {  return n>=0 && n<2 ? _pctc[n]:0;}

number  getmass() const {return _Mass;}
number  getmomentum() const {return _Momentum;}
number getcharge() const {return _Charge;}
AMSPoint getcoo() const { return _Coo;}
   number  gettheta() {return _Theta;}
   number  getphi() {return _Phi;}

void       setpbeta(AMSBeta* p)     {_pbeta   = p;}
void       setpcharge(AMSCharge* p) {_pcharge = p;}
void       setptrack(AMSTrTrack* p) {_ptrack  = p;}
void       setpctc(AMSCTCCluster* p, int n)   { if (n>=0 && n < 2) _pctc[n] = p;}

integer    getgpart()               {return _GPart;}
void       setgpart(integer gpart)  {_GPart = gpart;}
//-
};

#endif
