// V. Choutko 5-june-96
// A.Klimentov July 10, 1996. Objectivity version
//             July 25, 1996. add #pragma
//             Aug  22, 1996. change the position of memebers in the class
//             Sep  26, 1996. add getpattern function
//             Mar  06, 1997. up to date.
//
// Last Edit Oct 04, 1996. ak 

#include <cern.h>
#include <typedefs.h>
#include <beta.h>

class AMSTrTrackD;
#pragma ooclassref AMSTrTrackD <ttrack_ref.h>

class AMSEventD;
#pragma ooclassref AMSEventD <eventD_ref.h>

class AMSTOFClusterD;
#pragma ooclassref AMSTOFClusterD <tofrecD_ref.h>

class AMSChargeD;
#pragma ooclassref AMSChargeD <chargeD_ref.h>

class AMSParticleD;
#pragma ooclassref AMSParticleD <particleD_ref.h>

class AMSBetaD: public ooObj {

protected:

 number  _Beta ;
 number  _InvErrBeta;
 number  _Chi2;

 integer _Pattern;

 integer _Position;

public:

// Assosiations
 
 ooRef(AMSTOFClusterD)  pTOFBeta[] <-> pBetaTOF;
 ooRef(AMSEventD)       pEventB    <-> pBeta[];
 ooHandle(AMSTrTrackD)  pTrackBeta <-> pBetaT;
 ooHandle(AMSChargeD)   pChargeB   <-> pBetaCh;
 ooHandle(AMSParticleD) pParticleB <-> pBetaP;

// Constructor

 AMSBetaD();
 AMSBetaD(AMSBeta* p);

// Get Methods
 number  getchi2()    const {return _Chi2;}
 number  getbeta()    const {return _Beta;}
 number  getebeta()   const {return _InvErrBeta;}
 integer getpattern() const {return _Pattern;}

 void   copy(AMSBeta* p);

 integer getPosition() { return _Position;}

// Set Methods
 void setPosition(integer position) {_Position = position;}

};
