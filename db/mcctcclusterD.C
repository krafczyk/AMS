// method source file for the object AMSCTCMCCluster
// Sep 30, 1996. First try with Objectivity 
//
// Last Edit: Oct 10, 1996. ak.

#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <typedefs.h>
#include <mcctcclusterD.h>


 AMSCTCMCClusterD:: AMSCTCMCClusterD (AMSCTCMCCluster* p)
{
  _idsoft   = p -> _idsoft;
  _xcoo     = p -> _xcoo;
  //_xdir     = p -> xdir;
  _step     = p -> _step;
  _edep     = p -> _charge;
  _beta     = p -> _beta;
}

 void  AMSCTCMCClusterD:: copy(AMSCTCMCCluster* p)
{
  p -> _idsoft = _idsoft;   
  p -> _xcoo   = _xcoo;     
  //p -> xdir = _xdir;     
  p -> _step  = _step;     
  p -> _charge  = _edep;     
  p -> _beta  = _beta;     
}
 
