#ifndef __TKDBC__
#define __TKDBC__
#include <typedefs.h>

#include <iostream.h>
#include <fstream.h>


#ifdef __NAMESPACE__
namespace trconst{
const integer maxlay=8;
const integer maxlad=17;
}
using namespace trconst;
#else
const integer maxlay=8; 
const integer maxlad=17; 
#endif

class AMSPoint;
class TKDBc{
private:
 number _nrm[3][3];
 geant _coo[3];
 integer _status;
 integer _gid;
 static TKDBc * _HeadSensor;
 static TKDBc * _HeadLayer;
 static TKDBc * _HeadLadder[2];
 static TKDBc * _HeadMarker[2];
 static integer * _patconf[maxlay];
 static integer * _patpoints;
 static integer * _patmiss[maxlay];
 static integer  _patd[maxlay]; 
 static integer * _patallow;
 static integer * _patallow2;
 static uinteger _Npat;
 static integer _NumberSen;
 static integer _NumberLayer;
 static integer _NumberLadder;
 static integer _NumberMarkers;
 static integer _ReadOK; 
// Tracker parameters
   static  integer _nlaysi;    // number of si layers
   static  number   _layd[maxlay][5]; // pointer to supp plane
   static  number   _xposl[maxlay];   // pointers to si layers pos
   static  number   _yposl[maxlay];
   static  number   _zposl[maxlay];
   static  number   _nrml[maxlay][3][3];
   static  integer  _nlad[maxlay];
   static  integer  _nsen[maxlay][maxlad];
   static  integer  _nhalf[maxlay][maxlad];
   static  number   _zpos[maxlay];
   static  number   _ssize_active[maxlay][2];
   static  number   _ssize_inactive[maxlay][2];
   static  integer  _nstripssen[maxlay][2];
   static  integer  _nstripssenR[maxlay][2];
   static  integer  _nstripsdrp[maxlay][2];
   static  number   _silicon_z[maxlay];
   static  number   _zelec[maxlay][3];
   static  number   _c2c[maxlay];
   static  number   _halfldist[maxlay];
   static  number   _support_foam_w[maxlay];
   static  number   _support_foam_tol[maxlay];
   static  number   _support_hc_w[maxlay];
   static  number   _support_hc_r[maxlay];
   static  number   _support_hc_z[maxlay];
   static  integer  _nladshuttle[maxlay][2];
   static  integer  _boundladshuttle[maxlay][2];
   static  number   _PlMarkerPos[maxlay][2][4][3];  // 1st wjb
                                                    // 2nd hasan
   static uinteger factorial(uinteger n);
   static void InitPattern();
   static uinteger * _Cumulus;


 class TRLDGM_def{
 public:
   int Layer;
   int Ladder;
   int Half;
   geant Coo[3];
   geant Nrm[3][3];
   geant CooO[3];
   geant NrmO[3][3];
   geant Rx[10];
   geant Ry[10];
   geant Rz[10];
   int Status[10];
 };
 class TRLYGM_def{
 public:
   int Layer;
   geant Coo[3];
   geant Nrm[3][3];
   geant CooO[3];
   geant NrmO[3][3];
   geant Ra[4];
   geant Rx[4];
   geant Ry[4];
   geant Rz[4];
 };

public:
  TKDBc(): _status(0){}
 static integer getnum(integer layer, integer ladder, integer sensor);
 static integer getnumd(integer layer, integer ladder);
 static void init( );
 static void read();
 static void write(integer update = 0);
 static void updatef();
 static void compileg();
 static void fitting(int il, int nls, int nmeas, integer StaLadders[17][2],
          AMSPoint CooLadder[17][2][10], int CooStatus[17][2][10],
         number LadderNrm[17][2][3][3], TRLDGM_def & TRLDGM,
         TRLYGM_def & TRLYGM, integer xPosLadder[17][2][10],
         AMSPoint LadderCoo[17][2]);
 static integer GetStatus(integer layer, integer ladder, integer sensor){
                return _HeadSensor[getnum(layer,ladder,sensor)]._status;
 }
 static integer update(){return _ReadOK==0;}
 static void SetSensor(integer layer, integer ladder, integer sensor,
             integer  status, geant coo[],number nrm[3][3], integer gid);
 static void GetSensor(integer layer, integer ladder, integer sensor, 
                          integer & status, geant coo[],number nrm[3][3]);
 
 static void SetLayer(integer layer,
              integer  status, geant coo[],number nrm[3][3], integer gid);
 static void GetLayer(integer layer,  
                          integer & status, geant coo[],number nrm[3][3],
                          integer &gid);
 static void SetLadder(integer layer,integer ladder,integer half,
             integer  status, geant coo[],number nrm[3][3],integer gid);
 static void GetLadder(integer layer, integer ladder, integer half,
                          integer & status, geant coo[],number nrm[3][3],
                          integer &gid);
 static void GetMarker(integer layer, integer markerno, integer markerpos,
                       integer & status, geant coo[], number nrm[3][3]);    
 static void SetMarker(integer layer, integer markerno, integer markerpos,
                       integer & status, geant coo[], number nrm[3][3], integer gid);    
 
  static integer patmiss(uinteger pat, uinteger lay){return _patmiss[lay][pat];}
  static integer patconf(uinteger pat, uinteger lay){return _patconf[lay][pat];}
  static integer patconf3(uinteger pat, uinteger lay){return _patconf[lay][pat]-1;}
  static integer patd(uinteger lay){return _patd[lay];}
  static integer patallow(uinteger pat){return pat<_Npat?_patallow[pat]:-1;}
  static integer patallowFalseX(uinteger pat){return pat<_Npat?_patallow2[pat]:-1;}
  static integer ambig(uinteger pat);
  static integer patpoints(uinteger pat){return pat<_Npat?_patpoints[pat]:-1;}
  static uinteger npat(){return _Npat;}
// member functions
   inline static  number layd(integer i, integer j)
     {return _layd[i][j];}
inline static  number  PlMarkerPos(int i,int j,int k,int l)
{return _PlMarkerPos[j][i][k][l];}

   inline static  number  xposl(integer i){return _xposl[i];}
   inline static  number  yposl(integer i){return _yposl[i];}
   inline static  number  zposl(integer i){return _zposl[i];}
   inline static  number nrml(int i, int j, int k){return _nrml[k][i][j];}
   inline static integer maxnlad(){return maxlad;}
   inline static  number  zpos(integer i){return _zpos[i];}
   inline static  number  halfldist(integer i){return _halfldist[i];}
   inline static number  ssize_active(integer ilay, integer side){
     #ifdef __AMSDEBUG__
       assert(ilay>=0 && ilay < nlay()) ;
       assert(side>=0 && side<2);
     #endif
    return _ssize_active[ilay][side];
   }
   inline static number  ssize_inactive(integer ilay, integer side){
     #ifdef __AMSDEBUG__
       assert(ilay>=0 && ilay < nlay()) ;
       assert(side>=0 && side<2);
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

   inline static integer nlay(){return _nlaysi;}
   static uinteger Cumulus(integer ilay);

   inline static integer nlad(integer ilay){
     #ifdef __AMSDEBUG__
       assert(ilay>0 && ilay <= nlay()) ;
     #endif
     return _nlad[ilay-1];
   }
   inline static integer nsen(integer ilay, integer ilayd){
     #ifdef __AMSDEBUG__
       assert(ilay>0 && ilay <= nlay()) ;
       assert(ilayd>0 && ilayd<=nlad(ilay));
     #endif
     return _nsen[ilay-1][ilayd-1];
   }
   inline static integer nhalf(integer ilay, integer ilayd){
     #ifdef __AMSDEBUG__
       assert(ilay>0 && ilay <= nlay()) ;
       assert(ilayd>0 && ilayd<=nlad(ilay));
     #endif
     return _nhalf[ilay-1][ilayd-1];
   }
   static integer maxstripsdrp();
   inline static integer NStripsSen(integer ilay, integer side){
     #ifdef __AMSDEBUG__
       assert(ilay>0 && ilay <= nlay()) ;
       assert(side >=0 && side < 2);
     #endif
     return _nstripssen[ilay-1][side];
   }
   inline static integer NStripsSenR(integer ilay, integer side){
     #ifdef __AMSDEBUG__
       assert(ilay>0 && ilay <= nlay()) ;
       assert(side >=0 && side < 2);
     #endif
     return _nstripssenR[ilay-1][side];
   }
   inline static integer NStripsDrp(integer ilay, integer side){
     #ifdef __AMSDEBUG__
       assert(ilay>0 && ilay <= nlay()) ;
       assert(side >=0 && side < 2);
     #endif
     return _nstripsdrp[ilay-1][side];
   }
  static integer activeladdshuttle(integer i,integer j, integer s);



friend class AMSTrTrack;
#ifdef __DB__
   friend class TKDBcV;
#endif

};


class TKDBcI{
public:
TKDBcI();
~TKDBcI();
private:
 static integer _Count;
};
static TKDBcI tkI;



#endif

