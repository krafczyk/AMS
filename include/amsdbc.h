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
// Tracker parameters
   static const integer _nlay;
   static const integer _maxnlad;
   static const number  _layd[nl][5];
   static const number  _xposl[nl];
   static const number  _yposl[nl];
   static const number  _zposl[nl];
   static const number  _nrml[nl][3][3];
   static const integer _nlad[nl];
   static const integer _nsen[nl][nld];
   static const integer _nhalf[nl][nld];
   static const number  _zpos[nl];
   static const number  _ssize_active[nl][2];
   static const number  _ssize_inactive[nl][2];
   static const integer _nstripssen[nl][2];
   static const integer _nstripssenR[nl][2];
   static const integer _nstripsdrp[nl][2];
   static const number  _silicon_z[nl];
   static const number  _zelec[nl][3];
   static const number  _c2c[nl];
   static const number  _halfldist[nl];
   static const number  _support_foam_w[nl];
   static const number  _support_foam_tol[nl];
   static const number  _support_hc_w[nl];
   static const number  _support_hc_r[nl];
   static const number  _support_hc_z[nl];
   static const integer _nladshuttle[nl];
   static const integer _boundladshuttle[nl];
   static const number _PlMarkerPos[2][nl][4][3];  // 1st wjb
                                                   // 2nd hasan
   static uinteger * _Cumulus;
public:

#ifdef __DB__
   friend class AMSDBcD;
#endif

// constants
   static const number raddeg;
   static const number pi;
   static const number twopi;
// AMS parameters
     static  geant ams_size[3];
     static number ams_nrm[3][3];
     static const integer ams_rotmno;
     static const char ams_name[129];
     static  geant ams_coo[3];
//bits
     static const integer AMBIG;
     static const integer BAD;
     static const integer USED;
     static const integer DELETED;
     static const integer RELEASED;
     static const integer REFITTED;
     static const integer WIDE;
     static const integer BADHIS;
     static const integer ONESIDE;
     static const integer BADTIME;
     static const integer NEAR;
     static const integer WEAK;
     static const integer AwayTOF;
     static const integer FalseX;
     static const integer FalseTOFX;
     static const integer RECOVERED;
     static const integer LocalDB;

// machine dependent values

    static integer BigEndian;
// member functions
   inline static  number layd(integer i, integer j)
     {return _layd[i][j];}
inline static  number  PlMarkerPos(int i,int j,int k,int l)
{return _PlMarkerPos[i][j][k][l];}

   inline static  number  xposl(integer i){return _xposl[i];}
   inline static  number  yposl(integer i){return _yposl[i];}
   inline static  number  zposl(integer i){return _zposl[i];}
   inline static  number nrml(int i, int j, int k){return _nrml[k][i][j];}
   inline static integer maxnlad(){return _maxnlad;}
   inline static  number  zpos(integer i){return _zpos[i];}
   inline static  number  halfldist(integer i){return _halfldist[i];}
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
   inline static number support_foam_w(integer i){return _support_foam_w[i];}
   inline static number support_foam_tol(integer i){return _support_foam_tol[i];}
   inline static number support_hc_w(integer i){return _support_hc_w[i];}
   inline static number support_hc_r(integer i){return _support_hc_r[i];}
   inline static number support_hc_z(integer i){return _support_hc_z[i];}

   inline static number  silicon_z(integer i){return _silicon_z[i];}
   inline static number zelec(integer i, integer j){return _zelec[i][j];}

   inline static integer nlay(){return _nlay;}
   static uinteger Cumulus(integer ilay);

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
   static integer maxstripsdrp();
   inline static integer NStripsSen(integer ilay, integer side){
     #ifdef __AMSDEBUG__
     if(AMSDBc::debug){
       assert(ilay>0 && ilay <= AMSDBc::nlay()) ;
       assert(side >=0 && side < 2);
     }
     #endif
     return _nstripssen[ilay-1][side];
   }
   inline static integer NStripsSenR(integer ilay, integer side){
     #ifdef __AMSDEBUG__
     if(AMSDBc::debug){
       assert(ilay>0 && ilay <= AMSDBc::nlay()) ;
       assert(side >=0 && side < 2);
     }
     #endif
     return _nstripssenR[ilay-1][side];
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
  static integer activeladdshuttle(integer i,integer j, integer s);
};




#endif
