// Author V. Choutko 24-may-1996
//
// Objectivity class Oct 07, 1996 ak
//
// Mar 06, 1997. up to date 
// Sep 15, 1997. up to date. remove non-db member functions
// Nov  3, 1997. modifications, V212
//
// Last edit : Nov 3, 1997 ak
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
   integer _nlay;
   integer _maxnlad;
   number  _layd[nl][5];
   number  _xposl[nl];
   number  _yposl[nl];
   number  _zposl[nl];
   number  _nrml[nl][3][3];
   integer _nlad[nl];
   integer _nsen[nl][nld];
   integer _nhalf[nl][nld];
   number  _zpos[nl];
   number  _ssize_active[nl][2];
   number  _ssize_inactive[nl][2];
   integer _nstripssen[nl][2];
   integer _nstripssenR[nl][2];
   integer _nstripsdrp[nl][2];
   number  _silicon_z[nl];
   number  _zelec[nl][3];
   number  _c2c[nl];
   number  _halfldist[nl];
   number  _support_foam_w[nl];
   number  _support_hc_w[nl];
   number  _support_hc_r[nl];
   number  _support_hc_z[nl];
   integer _nladshuttle[nl];
   integer _boundladshuttle[nl];
    

   number raddeg;
   number pi;
   number twopi;

// AMS parameters
   char ams_name[129];
   geant ams_size[3];
   geant ams_coo[3];
//bits
   integer AMBIG;
   integer BAD;
   integer USED;
   integer DELETED;
//machine dependent values
   integer BigEndian;

public:
// constructor
   AMSDBcD();
//
   CmpConstants();

};




