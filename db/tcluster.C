//  method source file for the object AMSTrCluster
// May 10, 1996. First try with Objectivity 
// Aug 06, 1996. Add _Side, to build compuond predicate during iteration
// Sep 10, 1996. V1.25, _pValues
// Oct 16, 1996. remove AMSTrIdSoft.
//
// Last Edit: Oct 16, 1996. ak.
//
#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <typedefs.h>
#include <eventD_ref.h>
#include <trrechit_ref.h>
#include <tcluster.h>

AMSTrClusterD::AMSTrClusterD(){}
AMSTrClusterD::AMSTrClusterD ( AMSTrCluster* p)
{
  //  _Id     = p -> _Id;
  _Status = p -> _Status;
  _Nelem  = p -> _Nelem;
  _Sum    = p -> _Sum;
  _Sigma  = p -> _Sigma;
  _Mean   = p -> _Mean;
  _Rms    = p -> _Rms;
  _ErrorMean = p -> _ErrorMean;
  _Side   = p -> _Id.getside();
}

void AMSTrClusterD::setValues(number* values)
{
  if (_Nelem > 0) {
   _pValues.resize(_Nelem);
   for (integer i =0; i< _Nelem; i++) { _pValues.set(i,values[i]);}
  }
}    

void AMSTrClusterD::getValues(number* values)
{
  if (_Nelem > 0) {
   for (integer i =0; i< _Nelem; i++) { values[i] = _pValues[i];}
  }
}    

 void AMSTrClusterD::copy(AMSTrCluster* p)
{
  //   p -> _Id = _Id;     
   p -> _Status = _Status; 
   p -> _Nelem  = _Nelem;  
   p -> _Sum    = _Sum;    
   p -> _Sigma =  _Sigma;  
   p -> _Mean  = _Mean;   
   p -> _Rms   = _Rms;    
   p -> _ErrorMean = _ErrorMean; 
}

  





