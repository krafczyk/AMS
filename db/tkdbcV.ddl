// Objectivity class Nov 06, 1997 ak
//
// Last edit : Nov 6, 1997 ak
//
#include <typedefs.h> 

class TKDBcD {
//
protected:
   number  _nrm[3][3];
   geant   _coo[3];
   integer _status;
   integer _gid;

public:
// constructor
   TKDBcD() {};

   friend class TKDBcV;

};

declare (ooVArray, TKDBcD);

class TKDBcV: public ooObj {
//
protected:

ooVArray(TKDBcD) _HeadSensor;   
ooVArray(TKDBcD) _HeadLayer;   
ooVArray(TKDBcD) _HeadLadder0;   
ooVArray(TKDBcD) _HeadLadder1;   
ooVArray(TKDBcD) _HeadMarker0;   
ooVArray(TKDBcD) _HeadMarker1;   

integer          _ReadOK;

public:
// constructor
TKDBcV();
//
ooStatus CmpConstants(); 
ooStatus ReadTKDBc();
//   
};




