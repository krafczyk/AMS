//  $Id: mcctcclusterD.C,v 1.5 2001/01/22 17:32:30 choutko Exp $
// method source file for the object AMSCTCMCCluster
// Sep 30, 1996. First try with Objectivity 
//
// Last Edit: Mar 06, 1997.  ak.

#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <typedefs.h>
#include <mcctcclusterD.h>


 AMSCTCMCClusterD:: AMSCTCMCClusterD (AMSCTCMCCluster* p)
{
  _idsoft   = p -> _idsoft;
  _xcoo     = p -> _xcoo;
  _xdir     = p -> _xdir;
  _step     = p -> _step;
  _edep     = p -> _edep;
  _charge   = p -> _charge;
  _beta     = p -> _beta;
  _time     = p -> _time;
}

 void  AMSCTCMCClusterD:: copy(AMSCTCMCCluster* p)
{
  p -> _idsoft = _idsoft;   
  p -> _xcoo   = _xcoo;     
  p -> _xdir   = _xdir;     
  p -> _step   = _step;     
  p -> _charge = _charge;     
  p -> _edep   = _edep;     
  p -> _beta   = _beta;  
  p -> _time   = _time;   
}
 
