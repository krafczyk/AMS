//  $Id: tofrecD.C,v 1.3 2001/01/22 17:32:31 choutko Exp $
// class AMSTOFClusterD
// method source file for the object AMSTOFClusterD
// June 23, 1996. First try with Objectivity 
//
// Oct 10, 1996. Last edit , ak.

#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <typedefs.h>
//#include <eventD_ref.h>
#include <tbeta_ref.h>
#include <tofrecD.h>

AMSTOFClusterD::AMSTOFClusterD() {}
AMSTOFClusterD::AMSTOFClusterD(AMSTOFCluster* p)
{
  _ntof   = p -> _ntof;
  _plane  = p -> _plane;
  _edep   = p -> _edep;
  _Coo    = p -> _Coo;
  _ErrorCoo = p -> _ErrorCoo;
  _time     =  p -> _time;
  _etime    =  p -> _etime;
  _status   =  p -> getstatus();
}

void AMSTOFClusterD::copy(AMSTOFCluster* p)
{
   p -> _ntof   = _ntof;   
   p -> _plane =  _plane;  
   p -> _edep  =  _edep;   
   p -> _Coo   =  _Coo;    
   p -> _ErrorCoo = _ErrorCoo; 
   p -> _time     = _time;     
   p -> _etime    = _etime;    
   p -> setstatus(_status);
}

