// Author V. Choutko 24-may-1996
#ifndef __AMSDBC__
#define __AMSDBC__
#include <typedefs.h> 
class AMSDBc {
//
// Geometrical and other constants etc 
//
private:
#define nl 6
#define nld 17
#define mxstr 642
// Tracker parameters
   static const integer _maxstrips;
   static const integer _nlay;
   static const integer _maxnlad;
   static const number  _layd[nl][5];
   static const number  _zposl[nl];
   static const integer _nlad[nl];
   static const integer _ntube[nl];
   static const integer _nsen[nl][nld];
   static const integer _nhalf[nl][nld];
   static const number  _zpos[nl];
   static const number  _ssize_active[nl][2];
   static const number  _ssize_inactive[nl][2];
   static const integer _nstripssen[nl][2];
   static const integer _nstripsdrp[nl][2];
   static const number  _c2c[nl];
   static const number  _tube_inner_dia[nl];
   static const number  _silicon_z[nl];
   static const number  _tube_w[nl];
   static const number  _zelec[nl][2];

public:

#ifdef __DB__
   friend class AMSDBcD;
#endif

// constants
   static const number raddeg;
   static const number pi;
// AMS parameters
     static  geant ams_size[3];
     static const char ams_name[129];
     static  geant ams_coo[3];
//bits
     static const integer BAD;
     static const integer USED;

// member functions
   inline static  integer Ntube(integer i){return _ntube[i];}
   inline static  number layd(integer i, integer j)
     {return _layd[i][j];}
   inline static  number  zposl(integer i){return _zposl[i];}
   inline static integer maxnlad(){return _maxnlad;}
   inline static integer maxstrips(){return _maxstrips;}
   inline static  number  zpos(integer i){return _zpos[i];}
   inline static number  ssize_active(integer ilay, integer side){
     #ifdef __AMSDEBUG__
     if(AMSDBc::debug){
       assert(ilay>=0 && ilay < AMSDBc::nlay()) ;
       assert(side>=0 && side<2);
     }
     #endif
    return _ssize_active[ilay][side];
   }
   inline static number  ssize_inactive(integer ilay, integer side){
     #ifdef __AMSDEBUG__
     if(AMSDBc::debug){
       assert(ilay>=0 && ilay < AMSDBc::nlay()) ;
       assert(side>=0 && side<2);
     }
     #endif
     return _ssize_inactive[ilay][side];
   }
   inline static number  c2c(integer i){return _c2c[i];}
   inline static number tube_inner_dia(integer i){return _tube_inner_dia[i];}
   inline static number  silicon_z(integer i){return _silicon_z[i];}
   inline static number tube_w(integer i){return _tube_w[i];}
   inline static number zelec(integer i, integer j){return _zelec[i][j];}

   inline static integer nlay(){return _nlay;}
   inline static integer nlad(integer ilay){
     #ifdef __AMSDEBUG__
     if(AMSDBc::debug){
       assert(ilay>0 && ilay <= AMSDBc::nlay()) ;
     }
     #endif
     return _nlad[ilay-1];
   }
   inline static integer nsen(integer ilay, integer ilayd){
     #ifdef __AMSDEBUG__
     if(AMSDBc::debug){
       assert(ilay>0 && ilay <= AMSDBc::nlay()) ;
       assert(ilayd>0 && ilayd<=AMSDBc::nlad(ilay));
     }
     #endif
     return _nsen[ilay-1][ilayd-1];
   }
   inline static integer nhalf(integer ilay, integer ilayd){
     #ifdef __AMSDEBUG__
     if(AMSDBc::debug){
       assert(ilay>0 && ilay <= AMSDBc::nlay()) ;
       assert(ilayd>0 && ilayd<=AMSDBc::nlad(ilay));
     }
     #endif
     return _nhalf[ilay-1][ilayd-1];
   }
   inline static integer NStripsSen(integer ilay, integer side){
     #ifdef __AMSDEBUG__
     if(AMSDBc::debug){
       assert(ilay>0 && ilay <= AMSDBc::nlay()) ;
       assert(side >=0 && side < 2);
     }
     #endif
     return _nstripssen[ilay-1][side];
   }
   inline static integer NStripsDrp(integer ilay, integer side){
     #ifdef __AMSDEBUG__
     if(AMSDBc::debug){
       assert(ilay>0 && ilay <= AMSDBc::nlay()) ;
       assert(side >=0 && side < 2);
     }
     #endif
     return _nstripsdrp[ilay-1][side];
   }
  static integer debug;
  static integer activeladdshuttle(integer i,integer j);
};




#endif
