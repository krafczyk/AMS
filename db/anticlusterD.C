// method source file for the object AMSAntiClusterD
//
// Last Edit: Jun 02, 1997. ak.

#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <typedefs.h>
#include <anticlusterD.h>


 AMSAntiClusterD:: AMSAntiClusterD(AMSAntiCluster* p)
{
  _coo      = p -> _coo;
  _ecoo     = p -> _ecoo;
  _edep     = p -> _edep;
  _sector   = p -> _sector;
  _status   = p -> _status;
}
 
void AMSAntiClusterD::copy(AMSAntiCluster* p)
{
  p -> _coo    = _coo;     
  p -> _ecoo   = _ecoo;     
  p -> _edep   = _edep;     
  p -> _sector = _sector;
  p -> _status = _status;

}
 
void AMSAntiClusterD::add(AMSAntiCluster* p)
{
  _coo      = p -> _coo;
  _ecoo     = p -> _ecoo;
  _edep     = p -> _edep;
  _sector   = p -> _sector;
  _status   = p -> _status;

}
