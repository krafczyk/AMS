// Author A.Klimentov Apr 1, 1997
// method source file for the object AMSTOFMCClusterV
//
// Last Edit: Apr  1, 1997. ak.

#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <typedefs.h>
#include <mctofclusterD.h>
#include <mctofclusterV.h>


 AMSTOFMCClusterV:: AMSTOFMCClusterV(integer size)
{
 _tofmccluster.resize(size);
}

ooStatus AMSTOFMCClusterV::add(integer n, AMSTOFMCClusterD tofmccluster)
{
  integer rstatus = oocError;
  if (_tofmccluster.size() > n) { 
   _tofmccluster.set(n,tofmccluster);
   rstatus = oocSuccess;
  } 
  return rstatus;
}

AMSTOFMCClusterD AMSTOFMCClusterV::get(integer n)
{
  if (_tofmccluster.size() > n) { return _tofmccluster[n]; }
  return NULL;
}
 
 
