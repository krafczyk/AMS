//
// July 13, 1996.  ak  Objectivity version
// July 25, 1996.  ak. Add #pragma
// Aug  22, 1996.  ak. Rearrange the position of members in class
// Sep  24, 1996.  ak. set one to many assoc, between charge and event
// Mar   6, 1997.  ak. up to date
//
// Last Edit : Oct 10, 1996

#include <typedefs.h>
#include <charge.h> 

class AMSCharge;
const integer nchargeD=7; // e,p,He...C

class AMSEventD;
#pragma ooclassref AMSEventD <eventD_ref.h>

class AMSBetaD;
#pragma ooclassref AMSBetaD <tbeta_ref.h>

class AMSParticleD;
#pragma ooclassref AMSParticleD <particleD_ref.h>

class AMSChargeD: public ooObj {

protected:

 number _ProbTOF[nchargeD];        //(prob) to be e, p, He, C
 number _ProbTracker[nchargeD];    // (prob) to be e, p, He, C

 integer _ChargeTOF;
 integer _ChargeTracker;

 integer _Position;
 
public:

// Constructors
  AMSChargeD();
  AMSChargeD(AMSCharge* p);

// Assosiations

  ooHandle(AMSEventD)    pEventCh    <-> pChargeE[];
  ooHandle(AMSBetaD)     pBetaCh     <-> pChargeB;
  ooHandle(AMSParticleD) pParticleC  <-> pChargeP;

  void   copy(AMSCharge* p);

  integer getPosition()            {return _Position;}
  void    setPosition(integer pos) {_Position = pos;}

};
