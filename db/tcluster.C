//  method source file for the object AMSTrCluster
// May 10, 1996. First try with Objectivity 
// Aug 06, 1996. Add _Side, to build compound predicate during iteration
// Sep 10, 1996. V1.25, _pValues
// Oct 16, 1996. remove AMSTrIdSoft.
// Nov 15, 1996. NelemL and NelemR
// Mar 20, 1997. getnelem() is corrected
//
// Last Edit: Mar 20, 1997. ak.
//
#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <typedefs.h>
#include <db_comm.h>
#include <recevent_ref.h>
#include <trrechit_ref.h>
#include <tcluster.h>

AMSTrClusterD::AMSTrClusterD(){}
AMSTrClusterD::AMSTrClusterD ( AMSTrCluster* p)
{
  _Id     = p -> _Id;
  _NelemL = p -> _NelemL;
  _NelemR = p -> _NelemR;
  _Sum    = p -> _Sum;
  _Sigma  = p -> _Sigma;
  _Mean   = p -> _Mean;
  _Rms    = p -> _Rms;
  _ErrorMean = p -> _ErrorMean;
  _status = p -> getstatus();
}

void AMSTrClusterD::setValues(number* values)
{
  if (getnelem() > 0) {
   _pValues.resize(getnelem());
   for (integer i =0; i< getnelem(); i++) { _pValues.set(i,values[i]);}
  }
}    

void AMSTrClusterD::getValues(number* values)
{
  if (getnelem() > 0) {
   for (integer i =0; i< getnelem(); i++) { values[i] = _pValues[i];}
  }
}    

 void AMSTrClusterD::copy(AMSTrCluster* p)
{
   p -> _Id      = _Id;
   p -> _NelemL  = _NelemL;  
   p -> _NelemR  = _NelemR;  
   p -> _Sum     = _Sum;    
   p -> _Sigma   =  _Sigma;  
   p -> _Mean    = _Mean;   
   p -> _Rms     = _Rms;    
   p -> _ErrorMean = _ErrorMean; 
   p -> setstatus(_status);
}

  





