// AMS common blocks
// Objectivity version Oct 08, 1996 ak
// Nov 12, 1996. ak. remove amsffkey and iopa from class definition
//
// Last Edit : Mar 06, 1997. ak.
//

#include <typedefs.h>
#include <commons.h>

declare (ooVArray, integer);

class AMScommonsD : public ooObj {

protected :
 
 ooVArray(integer) tofmcffkeyD;
 ooVArray(integer) tofrecffkeyD;
 ooVArray(integer) ctcgeomffkeyD;
 ooVArray(integer) ctcmcffkeyD;
 ooVArray(integer) ctcrecffkeyD;
 ooVArray(integer) trmcffkeyD;
 ooVArray(integer) betafitffkeyD;
 ooVArray(integer) chargefitffkeyD;
 ooVArray(integer) trfitffkeyD;
 ooVArray(integer) ccffkeyD;
 ooVArray(integer) trclffkeyD;
 ooVArray(integer) trcalibD;
 ooVArray(integer) trigffkeyD;

//
public:
//
// Constructor 

AMScommonsD();
//
ooStatus CmpConstants(integer RW);
void     CopyConstants();
};
