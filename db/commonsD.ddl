// AMS common blocks
// Objectivity version Oct 08, 1996 ak
// Nov 12, 1996. ak. remove amsffkey and iopa from class definition
//
// Last Edit : Nov 12, 1996. ak.
//

#include <typedefs.h>
#include <commons.h>

class AMScommonsD : public ooObj {

protected :
 TOFMCFFKEY_DEF   tofmcffkeyD;
 TOFRECFFKEY_DEF  tofrecffkeyD;
 CTCGEOMFFKEY_DEF ctcgeomffkeyD;
 CTCMCFFKEY_DEF   ctcmcffkeyD;
 CTCRECFFKEY_DEF  ctcrecffkeyD;
 TRMCFFKEY_DEF    trmcffkeyD;
 BETAFITFFKEY_DEF betafitffkeyD;
 TRFITFFKEY_DEF   trfitffkeyD;
 CCFFKEY_DEF      ccffkeyD;
 TRCLFFKEY_DEF    trclffkeyD;

//
public:
//
// Constructor 

AMScommonsD();
//
ooStatus CmpConstants();
};

