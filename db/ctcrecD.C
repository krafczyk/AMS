//  $Id: ctcrecD.C,v 1.4 2001/01/22 17:32:27 choutko Exp $
// class AMSCTCClusterD
// method source file for the object AMSCTCClusterD
// Sep 27, 1996. First try with Objectivity 
//
// Mar 06, 1996. Last edit , ak.

#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <typedefs.h>
//#include <eventD_ref.h>
#include <ctcrecD.h>

AMSCTCClusterD::AMSCTCClusterD() {}
AMSCTCClusterD::AMSCTCClusterD(AMSCTCCluster* p)
{
  _Signal          = p -> _Signal;
  _ErrorSignal     = p -> _ErrorSignal;
  _CorrectedSignal = p -> _CorrectedSignal;
  _Coo             = p -> _Coo;
  _ErrorCoo        = p -> _ErrorCoo;
  _Layer           = p -> _Layer;
  _status          = p -> getstatus();
}

void AMSCTCClusterD::copy(AMSCTCCluster* p)
{
   p -> _Signal = _Signal;          
   p -> _ErrorSignal = _ErrorSignal;     
   p -> _CorrectedSignal = _CorrectedSignal; 
   p -> _Coo = _Coo;             
   p -> _ErrorCoo = _ErrorCoo;        
   p -> _Layer    = _Layer;
   p -> setstatus(_status);
}
