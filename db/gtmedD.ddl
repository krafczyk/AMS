// Author V. Choutko 24-may-1996
// 
// Sep 05, 1996. ak. First try with Objectivity
//                   Add _id and _name. The order of variables is changed
//
// Last Edit : Oct 10, 1996. ak.
// 

#include <iostream.h>
#include <string.h>
#include <cern.h>
#include <gmat.h>

#include <typedefs.h>

class AMSgtmedD : public ooObj 
{
 protected:
   geant _fieldm;
   geant _tmaxfd;
   geant _stemax;
   geant _deemax;
   geant _epsil;
   geant _stmin;
   geant _ubuf[1];
   geant _birks[3];

   integer _id;       //   ! get from AMSID

   integer _isvol;
   integer _itmed;
   integer _itmat;
   integer _ifield;

   char _yb;

 public:

   char    _name[80]; //   ! get from AMSID

 // Constructors
 AMSgtmedD();
 AMSgtmedD(integer id, AMSgtmed* p, char* name);
 //Set, Get functions
 inline integer getid() {return _id;}
        char*   getname() {return _name;}
        char    getyb()   {return _yb;}
 void   getNumbers(integer* ibuff, geant* gbuff);
};
