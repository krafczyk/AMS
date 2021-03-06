//  $Id$
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
#include "link.h"
#include "trrec.h"
#include "tofrec02.h"
#include "ecaldbc.h"
#include "beta.h"
#include "charge.h"
#include "trdrec.h"
#include "trdhrec.h"
#include "richrec.h"
#include "tofhit.h"
#include "Tofrec02_ihep.h"
class AMSVtx;

class AntiMatter: public AMSlink{
protected: 
  integer _pid;
  void _copyEl(){}
  void _printEl(ostream & stream){ stream << " Pid " << _pid<<endl;}
  void _writeEl(){}
public:
  AntiMatter(integer pid=0):_pid(pid){}
};

class AMSEcalShower;
class AMSParticle: public AMSlink{
protected:

  AMSBeta * _pbeta;          // pointer to beta 
  AMSBetaH * _pbetah;         //pointer to betah
  AMSCharge * _pcharge;      // pointer to charge
  AMSTrTrack * _ptrack;      // pointer to track;
  AMSTRDTrack * _ptrd;       // pointer to trd track 
  AMSTRDHTrack * _phtrd;       // pointer to trd htrack 
  AMSVtx * _pvert;           // pointer to vertex
  AMSRichRing * _prich;      // pointer to rich ring
  AMSRichRingNew * _prichB;  // pointer to another ring reconstruction (likelihood)
  AMSEcalShower  *_pShower;     // pointer to shower;
  integer _GPart;        // Geant particle ID

  number  _Mass;
  number  _ErrMass;
  number  _Momentum;
  number  _ErrMomentum;
  number  _Beta;
  number  _ErrBeta;
//--
  number  _Charge;
  number  _Theta;
  number  _Phi;
  number  _ThetaGl;
  number  _PhiGl;
  number  _CutoffMomentum;
  AMSPoint _Coo;
  AMSPoint _TOFCoo[4];  
  number   _TOFTLength[4];  
  AMSPoint _AntiCoo[2];
  number   _AntiCrAngle[2][2];//[dir][theta/phi]  
  AMSPoint _EcalCoo[2*ecalconst::ECSLMX];  
  AMSPoint _EcalSCoo[3];
  AMSPoint _TrCoo[trconst::maxlay];  
  AMSPoint _TRDCoo[2];  
  AMSPoint _RichCoo[2];
  number   _RichPath[2];
  number   _RichPathBeta[2];
  number   _RichLength;
  integer  _RichParticles;
  number   _Local[trconst::maxlay];
  number _TRDLikelihood;
  number _TRDHLikelihood;
  number _TRDHElik;
  number _TRDHPlik;
  integer _TRDCCnhit;

  number _TrdSH_E2P_lik;
  number _TrdSH_He2P_lik;
  number _TrdSH_E2He_lik;

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
  static void _calcmass(number momentum,number emom, number beta, number ebeta, number &mass, number &emass);
  static void _build(number rid, number err, number charge,number beta, number ebeta, 
		     number &mass, number &emass, number &mom, number &emom);
public:
#ifdef __WRITEROOT__
  friend class ParticleR;
#endif
  friend class AMSEvent;
  static number trdespect[30];
  static number trdpspect[30];
  AMSParticle *  next(){return (AMSParticle*)_next;}
  AMSParticle(AMSVtx *pvert);
  AMSParticle():   AMSlink(),_pbeta(0),_pbetah(0), _pcharge(0), _ptrack(0),_ptrd(0),_phtrd(0),_pvert(0),_prich(0),_prichB(0),_pShower(0)
  {
    int i;
    for(i=0;i<4;i++)_TOFCoo[i]=AMSPoint(0,0,0);
    for(i=0;i<4;i++)_TOFTLength[i]=0;
    for(i=0;i<2;i++){
      _AntiCoo[i]=AMSPoint(0,0,0);
      _AntiCrAngle[i][0]=AMSDBc::pi/2;//the
      _AntiCrAngle[i][1]=0;//phi
    }
    for(i=0;i<2*ecalconst::ECSLMX;i++)_EcalCoo[i]=AMSPoint(0,0,0);
    for(i=0;i<3;i++)_EcalSCoo[i]=AMSPoint(0,0,0);
    for(i=0;i<2;i++)_RichCoo[i]=AMSPoint(0,0,0);
    for(i=0;i<2;i++)_RichPath[i]=0.;
    for(i=0;i<2;i++)_RichPathBeta[i]=0.;
    _RichLength=0.;
#ifdef _PGTRACK_
    for(i=0;i<TkDBc::Head->nlay();i++)
#else
      for(i=0;i<TKDBc::nlay();i++)
#endif
	{
	  _TrCoo[i]=AMSPoint(0,0,0);
	  _Local[i]=0;
	}    
    _TRDCoo[0]=AMSPoint(0,0,0);
    _TRDCoo[1]=AMSPoint(0,0,0);
  }
  AMSParticle(AMSBeta * pbeta,AMSBetaH *pbetah, AMSCharge * pcharge, AMSTrTrack * ptrack,
	      number beta, number ebeta,number mass, number errmass, number momentum, number errmomentum,
	      number charge, number theta, number phi, AMSPoint coo): 
    _pbeta(pbeta),_pbetah(pbetah), _pcharge(pcharge), _ptrack(ptrack),_ptrd(0),_phtrd(0),_pvert(0),_prich(0),_prichB(0),_pShower(0), 
    _Mass(mass),_ErrMass(errmass), _Momentum(momentum), _ErrMomentum(errmomentum),_Beta(beta),_ErrBeta(ebeta),
    _Charge(charge), _Theta(theta), _Phi(phi), _Coo(coo)
  {
    int i;
    for(i=0;i<4;i++)_TOFCoo[i]=AMSPoint(0,0,0);
    for(i=0;i<4;i++)_TOFTLength[i]=0;
    for(i=0;i<2;i++){
      _AntiCoo[i]=AMSPoint(0,0,0);
      _AntiCrAngle[i][0]=AMSDBc::pi/2;//the
      _AntiCrAngle[i][1]=0;//phi
    }
    for(i=0;i<2*ecalconst::ECSLMX;i++)_EcalCoo[i]=AMSPoint(0,0,0);
    for(i=0;i<3;i++)_EcalSCoo[i]=AMSPoint(0,0,0);
    for(i=0;i<2;i++)_RichCoo[i]=AMSPoint(0,0,0);
    for(i=0;i<2;i++)_RichPath[i]=0.;
    for(i=0;i<2;i++)_RichPathBeta[i]=0.;
    _RichLength=0.;
#ifdef _PGTRACK_
    for(i=0;i<TkDBc::Head->nlay();i++)
#else
      for(i=0;i<TKDBc::nlay();i++)
#endif
	{
	  _TrCoo[i]=AMSPoint(0,0,0);
	  _Local[i]=0;
	}
  }

  void toffit(); // TOF fit
  void antifit(); // Anti fit
  void ecalfit(); // Ecal fit
  void trdfit(); //  trd fit
  void trd_likelihood(); //  trd likelihood
  void trd_Hlikelihood(); //  trd likelihood
  void richfit(); //  rich fit
  void pid();   // particle identification
  void refit(int i=0); // refit if necessary;
  static integer build(integer refit=0);
  static void print();
  number getcutoffmomentum() const {return _CutoffMomentum;}
  //+

  AMSPoint getcoo() {return _Coo;}
  AMSPoint gettofcoo(integer i) {return _TOFCoo[i];}
  number gettoftlen(integer i) {return _TOFTLength[i];}
  AMSPoint getecalcoo(integer i) {return _EcalCoo[i];}
  AMSBeta*       getpbeta()    const   { return _pbeta;}
  AMSCharge*     getpcharge()  const   { return _pcharge;}
  AMSTrTrack*    getptrack()   const   { return _ptrack;}
  AMSTRDTrack*   getptrd()     const   { return _ptrd;}
  AMSTRDHTrack*  getphtrd()   const   { return _phtrd;}
  AMSVtx*        getvert()     const   { return _pvert;}
  AMSRichRing *  getprich()    const   { return  _prich;}
  AMSRichRingNew *  getprichB()    const   { return  _prichB;}
  AMSEcalShower * getpshower()  const   {return _pShower;}

  number  getmass() const {return (number) _Mass;}
  number  getmomentum() const {return _Momentum;}
  number getcharge() const {return _Charge;}
  AMSPoint getcoo() const { return _Coo;}
  number  gettheta() {return _Theta;}
  number  getphi() {return _Phi;}

  void       setpbeta(AMSBeta* p)     {_pbeta   = p;}
  void       setpcharge(AMSCharge* p) {_pcharge = p;}
  void       setptrack(AMSTrTrack* p) {_ptrack  = p;}

  integer    getgpart()               {return _GPart;}
  void       setgpart(integer gpart)  {_GPart = gpart;}
  uinteger contnumber() const {
#ifdef _PGTRACK_
return _ptrack->IsFake() && !_pvert?1:0;}
#else
    return _ptrack->getpattern()<0 && !_pvert?1:0;}
#endif

  //-
  ~AMSParticle(){
#ifdef _PGTRACK_
    if(_ptrack && _ptrack->IsFake())
#else
    if(_ptrack && _ptrack->getpattern()==-1)
#endif
    {
      delete _ptrack;
      _ptrack=0;
    }
  }
};
#endif
