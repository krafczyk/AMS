// method source file for the object AMSTrMCCluster
// July 22, 1996. First try with Objectivity 
//
// Last Edit: Apr 1, 1997. ak.

#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <typedefs.h>
#include <mctofclusterD.h>


 AMSTOFMCClusterD::AMSTOFMCClusterD(AMSTOFMCCluster* p)
{
  _idsoft   = p -> idsoft;
  _xcoo     = p -> xcoo;
  _tof      = p -> tof;
  _edep     = p -> edep;
}
 
void  AMSTOFMCClusterD::add(AMSTOFMCCluster* p)
{
  _idsoft   = p -> idsoft;
  _xcoo     = p -> xcoo;
  _tof      = p -> tof;
  _edep     = p -> edep;
}

void AMSTOFMCClusterD::copy(AMSTOFMCCluster* p)
{
  p -> idsoft = _idsoft;   
  p -> xcoo   = _xcoo;     
  p -> tof    = _tof;      
  p -> edep   = _edep;     
}
 
