// method source file for the object AMSTrMCCluster
// June 04, 1996. First try with Objectivity 
//
// Last Edit: Jun 05, 1996. ak.

#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <typedefs.h>
#include <eventD_ref.h>
#include <tmccluster.h>
//#include <trid.h>

AMSTrMCClusterD::AMSTrMCClusterD (AMSTrMCCluster* p)
{
  integer i, j;

  _idsoft = p -> _idsoft;
  _itra   = p -> _itra;
  for (i=0; i<2; i++)
    {
      _left[i]   = p -> _left[i];
      _center[i] = p -> _center[i];
      _right[i]  = p -> _right[i];
    }
  _xca    = p -> _xca;
  _xcb    = p -> _xcb;
  _xgl    = p -> _xgl;
  _sum    = p -> _sum;
  for( i=0; i<2; i++) {
    for (j = 0; j<5; j++) {
     _ss[i][j]= p -> _ss[i][j];
    }
  }
}
 void   AMSTrMCClusterD::copy(AMSTrMCCluster* p)
{
  integer i, j;

  p -> _idsoft = _idsoft;
  p -> _itra   = _itra;   
  for (i=0; i<2; i++)
    {
      p -> _left[i] = _left[i];
      p -> _center[i] = _center[i]; 
      p -> _right[i]  = _right[i];  
    }
  p -> _xca = _xca;    
  p -> _xcb = _xcb;    
  p -> _xgl = _xgl;    
  p -> _sum = _sum;    
  for( i=0; i<2; i++) {
    for (j = 0; j<5; j++) {
     p -> _ss[i][j] = _ss[i][j];
    }
  }
}
