// Author V. Choutko 24-may-1996
//
// Objectivity class Oct 07, 1996 ak
//
// Mar 06, 1997. up to date 
// Sep 15, 1997. up to date. remove non-db member functions
//
// Last edit : Sep 15, 1997 ak
//
#include <typedefs.h> 

#define nl 6
#define nld 17
#define mxstr 640

class AMSDBcD: public ooObj {
//
// Geometrical and other constants etc 
//
protected:
// Tracker parameters
   integer _maxstripsD;
   integer _nlayD;
   integer _maxnladD;
   number  _laydD[nl][5];
   number  _zposlD[nl];
   integer _nladD[nl];
   integer _nsenD[nl][nld];
   integer _nhalfD[nl][nld];
   number  _zposD[nl];
   number  _ssize_activeD[nl][2];
   number  _ssize_inactiveD[nl][2];
   integer _nstripssenD[nl][2];
   integer _nstripsdrpD[nl][2];
   number  _silicon_zD[nl];
   number  _zelecD[nl][2];
   number  _c2cD[nl];
   number  _halfldist[nl];
   number  _support_wD[nl];
   integer _nladshuttle[nl];
   integer _boundladshuttle[nl];
    

   number raddegD;
   number piD;
   number twopi;

// AMS parameters
   geant ams_sizeD[3];
   char ams_nameD[129];
   geant ams_cooD[3];
//bits
   integer AMBIG;
   integer BAD;
   integer USED;
   integer DELETED;
//machine dependent values
   integer BigEndian;

   integer debug;
   
public:
// constructor
   AMSDBcD();
//
   CmpConstants();

};




