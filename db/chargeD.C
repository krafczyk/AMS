//  $Id: chargeD.C,v 1.3 2001/01/22 17:32:27 choutko Exp $
// A.Klimentov. Method file of AMSChargeD. July, 24, 1996
//
// July 24, 1996.  ak  Objectivity version
// Oct  10, 1996.  ak  implement friend class
//

#include <iostream.h>
#include <string.h>
#include <cern.h>
#include <typedefs.h>
#include <tbeta_ref.h>
#include <particleD_ref.h>
#include <recevent_ref.h>
#include <chargeD.h>



AMSChargeD::AMSChargeD() {}
AMSChargeD::AMSChargeD(AMSCharge* p) {
 _ChargeTOF     = p -> _ChargeTOF;
 _ChargeTracker = p -> _ChargeTracker;
 for (int i=0; i<ncharge; i++) {
  _ProbTOF[i] = p -> _ProbTOF[i];
  _ProbTracker[i] = p -> _ProbTracker[i];
 }
}

void AMSChargeD::copy(AMSCharge* p) {
  p -> _ChargeTOF = _ChargeTOF;
  p -> _ChargeTracker = _ChargeTracker;
 for (int i=0; i<ncharge; i++) {
  p -> _ProbTOF[i] = _ProbTOF[i];
  p -> _ProbTracker[i] = _ProbTracker[i];
 }
}

