//  $Id: anticlusterD.C,v 1.5 2001/01/22 17:32:27 choutko Exp $
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
  _status   = p -> getstatus();
//  _signal   = pR-> getsignal();
}
 
void AMSAntiClusterD::copy(AMSAntiCluster* p)
{
  p -> _coo    = _coo;     
  p -> _ecoo   = _ecoo;     
  p -> _edep   = _edep;     
  p -> _sector = _sector;
  p -> setstatus(_status);
}
 
void AMSAntiClusterD::add(AMSAntiCluster* p)
{
  _coo      = p -> _coo;
  _ecoo     = p -> _ecoo;
  _edep     = p -> _edep;
  _sector   = p -> _sector;
  _status   = p -> getstatus();
//  _signal   = pR -> getsignal();
}
