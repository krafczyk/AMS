// class AMSTrTrack
// May  29, 1996. Objectivity DDL;
// July 10, 1996, add AMSBetaD assosiation;
//                add #pragma
// Aug  07, 1996, V124.
// Aug  22, 1996. change the place of members in the class
//
// Last Edit: Oct 10, 1996. ak. 
//
#include <typedefs.h>
#include <point.h>
#include <trrec.h>

class AMSEventD;
#pragma ooclassref AMSEventD <eventD_ref.h>

class AMSTrRecHitD;
#pragma ooclassref AMSTrRecHitD <trrechit_ref.h>

class AMSBetaD;
#pragma ooclassref AMSBetaD <tbeta_ref.h>

class AMSParticleD;
#pragma ooclassref AMSParticleD <particleD_ref.h>


class AMSTrTrackD: public ooObj {

protected:

AMSPoint _P0;
AMSPoint _GP0;
AMSPoint _HP0[2];

number _Chi2StrLine;
number _Chi2Circle;
number _CircleRidgidity;
number _Chi2FastFit;
number _Ridgidity;
number _ErrRidgidity;
number _Theta;
number _Phi;
number _GChi2;
number _GRidgidity;
number _GErrRidgidity;
number _GTheta;
number _GPhi;
number _HChi2[2];
number _HRidgidity[2];
number _HErrRidgidity[2];
number _HTheta[2];
number _HPhi[2];
number _Chi2MS;
number _GChi2MS;
number _RidgidityMS;
number _GRidgidityMS;

integer _Status;
integer _Pattern;
integer _NHits;
integer _GeaneFitDone;
integer _AdvancedFitDone;

integer _Position;

public:

// Assosiations
ooRef(AMSEventD)       pEventT      <-> pTrack[] ;
ooRef(AMSTrRecHitD)    pTrRecHitT[] <-> pTrackH[];
ooHandle(AMSBetaD)     pBetaT       <-> pTrackBeta;
ooHandle(AMSParticleD) pParticleT   <-> pTrackP;

//constructor 

//~AMSTrTrackD(){};
AMSTrTrackD();
AMSTrTrackD(AMSTrTrack* p);

// get methods
integer getPosition() {return _Position;}
integer getPattern()  {return _Pattern;}
void    copy(AMSTrTrack* p);

// set methods
void setPosition (integer pos) {_Position = pos;}
};
