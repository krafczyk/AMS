// Author V. Choutko 24-may-1996
//
// Objectivity class Oct 07, 1996 ak
//
// Last edit : Oct 07, 1996 ak
//
#include <typedefs.h> 

#define nl 6
#define nld 17
#define mxstr 642

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
   integer _ntubeD[nl];
   integer _nsenD[nl][nld];
   integer _nhalfD[nl][nld];
   number  _zposD[nl];
   number  _ssize_activeD[nl][2];
   number  _ssize_inactiveD[nl][2];
   integer _nstripssenD[nl][2];
   integer _nstripsdrpD[nl][2];
   number  _c2cD[nl];
   number  _tube_inner_diaD[nl];
   number  _silicon_zD[nl];
   number  _tube_wD[nl];
   number  _zelecD[nl][2];

   number raddegD;
   number piD;
// AMS parameters
   geant ams_sizeD[3];
   char ams_nameD[129];
   geant ams_cooD[3];
//bits
   integer BADD;
   integer USEDD;
   integer debugD;

public:
// constructor
   AMSDBcD();
   CmpConstants();

// member functions
   inline integer Ntube(integer i){return _ntubeD[i];}
   inline number layd(integer i, integer j)
     {return _laydD[i][j];}
   inline number  zposl(integer i){return _zposlD[i];}
   inline integer maxnlad(){return _maxnladD;}
   inline integer maxstrips(){return _maxstripsD;}
   inline number  zpos(integer i){return _zposD[i];}
   inline number  ssize_active(integer ilay, integer side){
    return _ssize_activeD[ilay][side];
   }
   inline number  ssize_inactive(integer ilay, integer side){
     return _ssize_inactiveD[ilay][side];
   }
   inline number  c2c(integer i){return _c2cD[i];}
   inline number tube_inner_dia(integer i){return _tube_inner_diaD[i];}
   inline number  silicon_z(integer i){return _silicon_zD[i];}
   inline number tube_w(integer i){return _tube_wD[i];}
   inline number zelec(integer i, integer j){return _zelecD[i][j];}

   inline integer nlay(){return _nlayD;}
   inline integer nlad(integer ilay){
     return _nladD[ilay-1];
   }
   inline integer nsen(integer ilay, integer ilayd){
     return _nsenD[ilay-1][ilayd-1];
   }
   inline integer nhalf(integer ilay, integer ilayd){
     return _nhalfD[ilay-1][ilayd-1];
   }
   inline integer NStripsSen(integer ilay, integer side){
     return _nstripssenD[ilay-1][side];
   }
   inline integer NStripsDrp(integer ilay, integer side){
     return _nstripsdrpD[ilay-1][side];
   }
   integer activeladdshuttle(integer i,integer j);
};




