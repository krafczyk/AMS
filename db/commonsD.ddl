// AMS common blocks
// Objectivity version Oct 08, 1996 ak
// Nov 12, 1996. ak. remove amsffkey and iopa from class definition
// June add anti.
// Sep  add tkfield
//
// Last Edit : Sep 16, 1997. ak.
//

#include <typedefs.h>
#include <commons.h>

declare (ooVArray, integer);

class AMScommonsD : public ooObj {

protected :
 
 ooVArray(integer) antigeomffkey;
 ooVArray(integer) antimcffkey;
 ooVArray(integer) antirecffkey;

 ooVArray(integer) betafitffkeyD;

 ooVArray(integer) ccffkeyD;
 ooVArray(integer) chargefitffkeyD;

 ooVArray(integer) ctcgeomffkeyD;
 ooVArray(integer) ctcmcffkeyD;
 ooVArray(integer) ctcrecffkeyD;

 ooVArray(integer) tkfield;

 ooVArray(integer) tofcaffkey;
 ooVArray(integer) tofmcffkeyD;
 ooVArray(integer) tofrecffkeyD;

 ooVArray(integer) trcalibD;
 ooVArray(integer) trmcffkeyD;
 ooVArray(integer) trfitffkeyD;
 ooVArray(integer) trclffkeyD;

 ooVArray(integer) lvl1ffkey;
 ooVArray(integer) lvl3expffkey;
 ooVArray(integer) lvl3simffkey;
 ooVArray(integer) lvl3ffkey;

//
public:
//
// Constructor 

AMScommonsD();
//
ooStatus CmpConstants(integer RW);
void     CopyConstants();
};
