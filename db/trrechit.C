//  $Id: trrechit.C,v 1.5 2001/01/22 17:32:31 choutko Exp $
// class AMSTrRecHit 
// method source file for the object AMSTrRecHit
// May 09, 1996. First try with Objectivity 
//
// Oct 10, 1996. Last edit , ak.

#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <typedefs.h>
#include <recevent_ref.h>
#include <tcluster_ref.h>
#include <ttrack_ref.h>
#include <trrechit.h>

AMSTrRecHitD::AMSTrRecHitD() {}
AMSTrRecHitD::AMSTrRecHitD(AMSTrRecHit* p)
{
  _Status = p -> getstatus();
  _Layer  = p -> _Layer;
  _Hit    = p -> _Hit;
  _EHit   = p -> _EHit;
  _Sum    = p -> _Sum;
  _DifoSum= p -> _DifoSum;
//  _Xcl    = p -> getClusterP(0);
//  _Ycl    = p -> getClusterP(1);
}
void AMSTrRecHitD::copy(AMSTrRecHit* p)
{
  p -> setstatus(_Status); 
  p -> _Layer  = _Layer;  
  p -> _Hit    = _Hit;    
  p -> _EHit   = _EHit;   
  p -> _Sum    = _Sum;    
  p -> _DifoSum = _DifoSum;
}
