// class AMSmceventgD
// method source file for the object AMSmceventgD
// Oct 02, 1996. First try with Objectivity 
//
// Nov 08, 1996. Last edit , ak.
//
#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <typedefs.h>
#include <mceventgD.h>

AMSmceventgD::AMSmceventgD() {}
AMSmceventgD::AMSmceventgD(AMSmceventg* p) {

 _coo       = p -> _coo;
 _dir       = p -> _dir;
 _mom       = p -> _mom;
 _mass      = p -> _mass;
 _charge    = p -> _charge;
 _ipart     = p -> _ipart;
 _seed[0]   = p -> _seed[0];
 _seed[1]   = p -> _seed[1];

}
AMSmceventgD::AMSmceventgD(AMSPoint coo, AMSDir dir, number mom, 
                           number mass, number charge, integer ipart, 
                           integer seed0, integer seed1)
{
 _coo      = coo;
 _dir      = dir;
 _mom      = mom;
 _mass     = mass;
 _charge   = charge;
 _ipart    = ipart;
 _seed[0]  = seed0;
 _seed[1]  = seed1;
}

void AMSmceventgD::copy(AMSmceventg* p) {

  p -> _coo     = _coo;
  p -> _dir     = _dir;       
  p -> _mom     = _mom;       
  p -> _mass    = _mass;      
  p -> _charge  = _charge;    
  p -> _ipart   = _ipart;     
  p -> _seed[0] = _seed[0];   
  p -> _seed[1] = _seed[1];   
}

void   AMSmceventgD::getAll(AMSPoint& coo, AMSDir& dir, number& mom, 
                           number& mass, number& charge, integer& ipart, 
                           integer& seed0, integer& seed1)
{
 coo    = _coo;
 dir    = _dir;
 mom    = _mom;
 mass   = _mass;
 charge = _charge;
 ipart  = _ipart;
 seed0  = _seed[0];
 seed1  = _seed[1];
}
