// A.Klimentov. Method file of AMSParticleD.
//
// July 13, 1996.  ak.  Objectivity version
// Sep  10, 1996.  ak.  V1.25
// Dec  13, 1996.  ak.  _Value[0], temporary solution
//
// Last Edit Dec 13, 1996. a.
//
#include <iostream.h>
#include <string.h>
#include <cern.h>
#include <point.h>
#include <ctc.h>

#include <typedefs.h>
#include <tbeta_ref.h>
#include <ttrack_ref.h>
#include <chargeD_ref.h>
#include <eventD_ref.h>
#include <particleD.h>


AMSParticleD:: AMSParticleD() {}
AMSParticleD:: AMSParticleD(AMSParticle* p) {
  
  _Coo            = p -> _Coo;
  _GPart          = p -> _GPart;
  _Value[0]       = p -> _Value[0];  
  _Value[1]       = p -> _Value[1];  
  _SumAnti        = p -> _SumAnti;
  _Mass           = p -> _Mass;
  _ErrMass        = p -> _ErrMass;
  _Momentum       = p -> _Momentum;
  _ErrMomentum    = p -> _ErrMomentum;
  _Charge         = p -> _Charge;
  _Theta          = p -> _Theta;
  _Phi            = p -> _Phi;

}

void   AMSParticleD::copy(AMSParticle* p)
{
   p -> _Coo     = _Coo;
   p -> _GPart   = _GPart;
   p -> _Value[0]= _Value[0]; 
   p -> _Value[1]= _Value[1]; 
   p -> _SumAnti = _SumAnti;
   p -> _Mass    = _Mass;
   p -> _ErrMass = _ErrMass;        
   p -> _Momentum = _Momentum;
   p -> _ErrMomentum = _ErrMomentum;    
   p -> _Charge      = _Charge;         
   p -> _Theta       = _Theta;          
   p -> _Phi         = _Phi;            
}


