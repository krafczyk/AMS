// class AMSTrTrack
// May  29, 1996. Objectivity DDL;
// July 10, 1996, add AMSBetaD assosiation;
//                add #pragma
// Aug    , 1996, V124.
//                change the place of members in the class
// Mar  06, 1997  up to date
//                use uni-directional association between track and hits
// Apr  08, 1997. unidirectional link between event and track
//                unidirectional link between beta/track, particle/track
//
// Last Edit: Apr 8, 1997. ak. 
//
#include <typedefs.h>
#include <point.h>
#include <trrec.h>


class AMSTrRecHitD;
#pragma ooclassref AMSTrRecHitD <trrechit_ref.h>


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

integer _Pattern;
integer _NHits;
integer _FastFitDone;
integer _GeaneFitDone;
integer _AdvancedFitDone;

integer _Position;  //pos from link.h

public:

// Assosiations
ooRef(AMSTrRecHitD)    pTrRecHitT[] : copy(delete);

//constructor 

AMSTrTrackD();
AMSTrTrackD(AMSTrTrack* p);

// get methods
integer getPosition() {return _Position;}
integer getPattern()  {return _Pattern;}
integer getnhits()    {return _NHits;}

void    copy(AMSTrTrack* p);

// set methods
void setPosition (integer pos) {_Position = pos;}
};
