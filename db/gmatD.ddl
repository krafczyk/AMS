// Author V. Choutko 24-may-1996
// 
// Sep 05, 1996. ak. First try with Objectivity
//                   Add _id and _name. The order of variables is changed
//
// Last Edit : Nov 27, 1996. ak.
// 

#include <iostream.h>
#include <string.h>
#include <cern.h>
#include <gmat.h>

#include <typedefs.h>

declare (ooVArray, geant);

class AMSgmatD : public ooObj 
{
 protected:
   integer _id;       //   ! get from AMSID
   integer _imate;

   geant _rho; //density
   geant _radl; //X_0
   geant _absl; //Abs l
   geant _ubuf[1];
   
   ooVArray(geant) _a;
   ooVArray(geant) _z;
   ooVArray(geant) _w;

   integer _npar;

 public:

   char    _name[80]; //   ! get from AMSID

// Constructors

 AMSgmatD();
 AMSgmatD(integer id, AMSgmat* p, geant* a, geant* z, geant* w, char* name);

// Set/Get

 inline integer getid()   {return _id;}
        char*   getname() {return _name;}
 inline integer getnpar()  {return _npar;}
 void   getNumbers(integer& id, integer& imate,  
                   geant& rho, geant& radl, geant& absl,
                   geant* a, geant* z, geant* w);
 ooStatus CmpMaterials(integer id, AMSgmat* p);
};
