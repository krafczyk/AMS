// Last Edit : Mar 06, 1997. add nlay.
//
// Nov 3, 1997.
//
#include <typedefs.h>

class CTCDBcD : public ooObj {

private:

integer _geomId;
integer _nlay;
geant   _tdcabw;
geant   _ftpulw;

public:

CTCDBcD();
ooStatus CmpConstants();

};
