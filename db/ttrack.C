//  $Id: ttrack.C,v 1.5 2001/01/22 17:32:31 choutko Exp $
// class AMSTrTrack
// May 29, 1996. Objectivity DDL
// Aug 07, 1996. V1.24
//
// Mar 14, 1997. Last edit , ak.

#include <iostream.h>
#include <string.h>
#include <cern.h>
#include <typedefs.h>
#include <recevent_ref.h>
#include <trrechit_ref.h>
#include <tbeta_ref.h>
#include <particleD_ref.h>
#include <ttrack.h>

AMSTrTrackD::AMSTrTrackD() {}
AMSTrTrackD::AMSTrTrackD(AMSTrTrack* p)
{
 _Pattern         = p -> _Pattern;
 _NHits           = p -> _NHits;
 _FastFitDone     = p -> _FastFitDone;
 _GeaneFitDone    = p -> _GeaneFitDone;
 _AdvancedFitDone = p -> _AdvancedFitDone;
 _Chi2StrLine     = p -> _Chi2StrLine;
 _Chi2Circle      = p -> _Chi2Circle;
 _CircleRidgidity = p -> _CircleRidgidity;
 _Chi2FastFit     = p -> _Chi2FastFit;
 _Ridgidity       = p -> _Ridgidity;
 _ErrRidgidity    = p -> _ErrRidgidity;
 _Theta           = p -> _Theta;
 _Phi             = p -> _Phi;
 _P0              = p -> _P0;
 _GChi2            = p -> _GChi2;
 _GRidgidity       = p -> _GRidgidity;
 _GErrRidgidity    = p -> _GErrRidgidity;
 _GTheta           = p -> _GTheta;
 _GPhi             = p -> _GPhi;
 _GP0              = p -> _GP0;
 for (integer i=0; i<2; i++) {
  _HChi2[i]         = p -> _HChi2[i];
  _HRidgidity[i]    = p -> _HRidgidity[i];
  _HErrRidgidity[i] = p -> _HErrRidgidity[i]; 
  _HTheta[i]        = p -> _HTheta[i];        
  _HPhi[i]          = p -> _HPhi[i];          
  _HP0[i]           = p -> _HP0[i];           
 }
 _Chi2MS            = p -> _Chi2MS;
 _GChi2MS           = p -> _GChi2MS;
 _RidgidityMS       = p -> _RidgidityMS;
 _GRidgidityMS      = p -> _GRidgidityMS;
}

void AMSTrTrackD::copy(AMSTrTrack* p)
{
  p -> _Pattern         = _Pattern;         
  p -> _NHits           = _NHits;           
  p -> _FastFitDone     = _FastFitDone;    
  p -> _GeaneFitDone    = _GeaneFitDone;    
  p -> _AdvancedFitDone = _AdvancedFitDone; 
  p -> _Chi2StrLine     = _Chi2StrLine;     
  p -> _Chi2Circle      = _Chi2Circle;      
  p -> _CircleRidgidity = _CircleRidgidity; 
  p -> _Chi2FastFit     = _Chi2FastFit;     
  p -> _Ridgidity       = _Ridgidity;       
  p -> _ErrRidgidity    = _ErrRidgidity;    
  p -> _Theta           = _Theta;           
  p -> _Phi             = _Phi;             
  p -> _P0              = _P0;                          
  p -> _GChi2           = _GChi2;            
  p -> _GRidgidity      = _GRidgidity;       
  p -> _GErrRidgidity   = _GErrRidgidity;    
  p -> _GTheta          = _GTheta;           
  p -> _GPhi            = _GPhi;             
  p -> _GP0             = _GP0;              
 for (integer i=0; i<2; i++) {
   p -> _HChi2[i]         = _HChi2[i];         
   p -> _HRidgidity[i]    = _HRidgidity[i];    
   p -> _HErrRidgidity[i] = _HErrRidgidity[i]; 
   p -> _HTheta[i]        = _HTheta[i];                
   p -> _HPhi[i]          = _HPhi[i];                    
   p -> _HP0[i]           = _HP0[i];                      
 }
  p -> _Chi2MS       = _Chi2MS;            
  p -> _GChi2MS      = _GChi2MS;           
  p -> _RidgidityMS  = _RidgidityMS;       
  p -> _GRidgidityMS = _GRidgidityMS;      
}
