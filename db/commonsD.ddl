// AMS common blocks
// Objectivity version Oct 08, 1996 ak
// Nov 12, 1996. ak. remove amsffkey and iopa from class definition
//
// Last Edit : Nov 26, 1996. ak.
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
 ooVArray(integer) trfitffkeyD;
 ooVArray(integer) ccffkeyD;
 ooVArray(integer) trclffkeyD;

//
public:
//
// Constructor 

AMScommonsD();
//
ooStatus CmpConstants();
void     CopyConstants();
};
