// method source file for the object AMSAntiMCCluster
//
// Last Edit: Oct 10, 1996. ak.

#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <typedefs.h>
//#include <eventD_ref.h>
#include <mcanticlusterD.h>


 AMSAntiMCClusterD:: AMSAntiMCClusterD(AMSAntiMCCluster* p)
{
  _idsoft   = p -> _idsoft;
  _xcoo     = p -> _xcoo;
  _tof      = p -> _tof;
  _edep     = p -> _edep;
}
 
void AMSAntiMCClusterD::copy(AMSAntiMCCluster* p)
{
  p -> _idsoft = _idsoft;
  p -> _xcoo   = _xcoo;     
  p -> _tof    = _tof;      
  p -> _edep   = _edep;     
}
 
