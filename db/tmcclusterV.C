//  $Id: tmcclusterV.C,v 1.2 2001/01/22 17:32:31 choutko Exp $
// method source file for the object AMSTrMCCluster
// June 04, 1996. First try with Objectivity 
//
// Last Edit: Jun 05, 1996. ak.

#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <typedefs.h>
#include <tmccluster.h>
#include <tmcclusterV.h>

 AMSTrMCClusterV:: AMSTrMCClusterV(integer size)
{
 _trmccluster.resize(size);
}

ooStatus AMSTrMCClusterV::add(integer n, AMSTrMCClusterD trmccluster)
{
  integer rstatus = oocError;
  if (_trmccluster.size() > n) { 
   _trmccluster.set( n, trmccluster);
   rstatus = oocSuccess;
  } 
  return rstatus;
}

AMSTrMCClusterD AMSTrMCClusterV::get(integer n)
{
  if (_trmccluster.size() > n) { return _trmccluster[n]; }
  return NULL;
}
 
 
