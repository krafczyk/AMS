// V. Choutko 6-june-96
//
// A.Klimentov Objectivity version July 13, 1996. 
//              add #pragma        July 25, 1996
// Aug 22, 1996. ak. change the position of members inside the class
// Sep 10, 1996. ak. V1.25, _GPart
//                   set one to many assoc., between particle and event
//                   V1.27, add CTC
// Mar 20, 1997. ak. pointers to CTC clusters
// Apr 08, 1997. ak. unidirectional link particle/track
//
// Last Edit : Apr 08, 1997. ak
//
#include <typedefs.h>
#include <point.h>
#include <ctc.h>
#include <particle.h>

class AMSEventD;
#pragma ooclassref AMSEventD <eventD_ref.h>

class AMSTrTrackD;
#pragma ooclassref AMSTrTrackD <ttrack_ref.h>

class AMSChargeD;
#pragma ooclassref AMSChargeD <chargeD_ref.h>

class AMSBetaD;
#pragma ooclassref AMSBetaD <tbeta_ref.h>

class AMSCTCClusterD;
#pragma ooclassref AMSCTCClusterD <ctcrecD_ref.h>

class AMSParticleD: public ooObj{

protected:

  AMSPoint _Coo;

  CTC      _Value[2];

  number   _Mass;
  number   _ErrMass;
  number   _Momentum;
  number   _ErrMomentum;
  number   _Charge;
  number   _Theta;
  number   _Phi;
  number   _SumAnti;

  integer  _GPart;
  integer  _Position;

public:

// Constructors
  AMSParticleD();
  AMSParticleD(AMSParticle* p);

// Associations
 ooHandle(AMSEventD)      pEventP  <-> pParticleE[] ;
 ooHandle(AMSChargeD)     pChargeP <-> pParticleC ;
 ooHandle(AMSBetaD)       pBetaP   : copy (delete);
 ooHandle(AMSTrTrackD)    pTrackP  : copy (delete);
 ooHandle(AMSCTCClusterD) pCTCClusterP[] : copy (delete);

 void   copy(AMSParticle* p);

 integer getPosition()            {return _Position;}
 void    setPosition(integer pos) {_Position = pos;}
 integer getGPart()               {return _GPart;}
 void    setGPart(integer gpart)  {_GPart = gpart;}
 number  getMass()                {return _Mass;}
 number  getErrMass()             {return _ErrMass;}
 number  getMomentum()            {return _Momentum;}
 number  getErrMomentum()         {return _ErrMomentum;}
 number  getCharge()              {return _Charge;}
 number  gettheta()               {return _Theta;}
 number  getPhi()                 {return _Phi;}
 AMSPoint getCoo()                {return _Coo;}
 number   getSumAnti()            {return _SumAnti;}
  
};
