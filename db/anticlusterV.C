//  $Id: anticlusterV.C,v 1.2 2001/01/22 17:32:27 choutko Exp $
// method source file for the object AMSAntiClusterV
//
// Last Edit: June 02, 1997. ak.

#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <typedefs.h>
#include <anticlusterD.h>
#include <anticlusterV.h>




AMSAntiClusterV:: AMSAntiClusterV(integer size)
{
 _anticluster.resize(size);
}

ooStatus AMSAntiClusterV::add(integer n, AMSAntiClusterD anticluster)
{
  integer rstatus = oocError;
  if (_anticluster.size() > n) { 
   _anticluster.set(n,anticluster);
   rstatus = oocSuccess;
  } 
  return rstatus;
}

AMSAntiClusterD AMSAntiClusterV::get(integer n)
{
  if (_anticluster.size() > n) { return _anticluster[n]; }
  return NULL;
}
