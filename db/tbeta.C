// class AMSBetaD
//
// July 10, 1996. 
// Last edit , Oct 10, ak.

#include <iostream.h>
#include <string.h>
#include <cern.h>
#include <typedefs.h>
#include <recevent_ref.h>
#include <particleD_ref.h>
#include <tofrecD_ref.h>
#include <chargeD_ref.h>
#include <ttrack_ref.h>
#include <tbeta.h>

AMSBetaD:: AMSBetaD() {}
AMSBetaD:: AMSBetaD(AMSBeta* p) {

 _Beta       = p -> _Beta;
 _InvErrBeta = p -> _InvErrBeta;
 _Chi2       = p -> _Chi2;
 _Pattern    = p -> _Pattern;

}

void AMSBetaD::copy(AMSBeta* p) {

 p -> _Beta       = _Beta;
 p -> _InvErrBeta = _InvErrBeta;
 p -> _Chi2       = _Chi2;
 p -> _Pattern    = _Pattern;

}
