// V. Choutko 6-june-96
//
// A.Klimentov Objectivity version July 13, 1996. 
//              add #pragma        July 25, 1996
// Aug 22, 1996. ak. change the position of members inside the class
// Sep 10, 1996. ak. V1.25, _GPart
// Sep 24, 1996. ak. set one to many assoc., between particle and event
// Sep 27, 1996. ak. V1.27, add CTC
//
// Last Edit : Oct 10, 1996. ak
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

class AMSParticleD: public ooObj{

protected:

  AMSPoint _Coo;

  CTC      _Value;

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
 ooHandle(AMSEventD)   pEventP  <-> pParticleE[] ;
 ooHandle(AMSBetaD)    pBetaP   <-> pParticleB ;
 ooHandle(AMSTrTrackD) pTrackP  <-> pParticleT ;
 ooHandle(AMSChargeD)  pChargeP <-> pParticleC ;

 void   copy(AMSParticle* p);

 integer getPosition()            {return _Position;}
 void    setPosition(integer pos) {_Position = pos;}
 integer getGPart()               {return _GPart;}
 void    setGPart(integer gpart)  {_GPart = gpart;}

};
