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
  _idsoft   = p -> idsoft;
  _xcoo     = p -> xcoo;
  _tof      = p -> tof;
  _edep     = p -> edep;
}
 
void AMSAntiMCClusterD::copy(AMSAntiMCCluster* p)
{
  p -> idsoft = _idsoft;
  p -> xcoo   = _xcoo;     
  p -> tof    = _tof;      
  p -> edep   = _edep;     
}
 
