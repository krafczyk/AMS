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
  AMSPoint _Coo;
  CTC    _Value[2];

  void _copyEl();
  void _printEl(ostream & stream){ stream << " Mass "<<_Mass<<
  " Error Mass "<<_ErrMass<<" Momentum "<<_Momentum<<" ErrMomentum "<<
   _ErrMomentum<<" Charge "<<_Charge<<" Theta "<<_Theta<<" Phi"<<_Phi<<
   " Coo "<<_Coo<<endl;}
 void _writeEl();
 static geant _massP[19];
 static integer _chargeP[10];
 static integer _partP[20];
public:
  AMSParticle *  next(){return (AMSParticle*)_next;}
  AMSParticle():   _pbeta(0), _pcharge(0), _ptrack(0)
 {
    for(int i=0;i<2;i++){
     _Value[i]=CTC(0.,0.,1.,AMSPoint());
     _pctc[i]=0;
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
 }

  void ctcfit(); // CTC fit
  void pid();   // particle identification
  void refit(); // refit if necessary;
  static void build();
  static void print();
//+

#ifdef __DB__
   friend class AMSParticleD;

   number  getmass() {return _Mass;}
   number  geterrmass() {return _ErrMass;}
   number  getmomentum() {return _Momentum;}
   number  geterrmomentum() {return _ErrMomentum;}
   number  getcharge() {return _Charge;}
   number  gettheta() {return _Theta;}
   number  getphi() {return _Phi;}
   AMSPoint getcoo() {return _Coo;}


#endif
AMSBeta*       getpbeta()       { return _pbeta;}
AMSCharge*     getpcharge()     { return _pcharge;}
AMSTrTrack*    getptrack()      { return _ptrack;}
AMSCTCCluster* getpctc(int n)   {  return n>=0 && n<2 ? _pctc[n]:0;}

number getcharge() const {return _Charge;}
void       setpbeta(AMSBeta* p)     {_pbeta   = p;}
void       setpcharge(AMSCharge* p) {_pcharge = p;}
void       setptrack(AMSTrTrack* p) {_ptrack  = p;}
void       setpctc(AMSCTCCluster* p, int n)   { if (n>=0 && n < 2) _pctc[n] = p;}

integer    getgpart()               {return _GPart;}
void       setgpart(integer gpart)  {_GPart = gpart;}
//-
};

#endif
