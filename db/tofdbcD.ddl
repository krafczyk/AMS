// Objectivity class Oct 08, 1996 ak.
// ( origianal TOFDBcD class by E.Choumilov 13.06.96.)
//
// Last Edit : Jan 21, 1997. 
//
// TOFDBcD should be modified according to the last E.C. version
//
#include <string.h>
#include <typedefs.h>
//
// TOF global constants definition
//geometry :
#define SCMXBR 14 // max nmb of bars/layer
#define SCLRS   4 // real nmb of layers in TOF-systems
#define SCROTN 1000 // start nmb of abs. numbering of TOF rot. matrixes
#define SCBTPN 5 //nmb of sc. bar types (different by length now)
//MC
#define AMSDISL 1000 //max length of distributions in AMSDistr class
#define SCANPNT 3 //scan points in MC t-calibration (AMSTOFScan class)
#define SCTBMX 10000 //length of flash-ADC array
//DAQ
#define SCTHMX1 16 //max trigger hits 
#define SCTHMX2 16 //max fast TDC (history) hits  
#define SCTHMX3 5 //max slow TDC (stretcher) hits
#define SCTHMX4 5 //max ADC hits  
//
class TOFDBcD {
//
//  Geometrical constants for TOF-system
//
private:
  integer _brtypeD[SCLRS][SCMXBR];//bar types(pos. in the list below)
  integer _plrotmD[SCLRS];        // TOF planes rotation mask
  char    _brfnamD[SCBTPN][6];    //list of t-calib. file names
  number  _brlenD[SCBTPN];        // bar lengthes for SCBTPN types
  number  _supstrD[10];           // supp. structure parameters
  number  _plnstrD[15];           // sc. plane structure param.
  number  _mcfparD[10];           // MC parameters (float)
  integer SCBRS[SCLRS];          // real nmb of bars per layer (top->bot)
  integer _debug;
//
//  Member functions
//
public:  
// constructor;
  TOFDBcD();
  inline number brlen(integer ilay, integer ibar){
    int btyp=_brtypeD[ilay][ibar];
    return _brlenD[btyp-1];
  }
//---
  inline int brtype(integer ilay, integer ibar){
    return _brtypeD[ilay][ibar];
  }
//
  inline void brfnam(integer ibt, char a[]){
    strcpy(a,&_brfnamD[ibt-1][0]);
  }
//
  inline integer plrotm(integer ilay){
    return _plrotmD[ilay];
  }
//
  inline number supstr(integer i){
    return _supstrD[i-1];
  }
//
  inline number plnstr(integer i){
    return _plnstrD[i-1];
  }
//   function to get Z-position of scint. bar=ib in layer=il
  inline number getzsc(integer il, integer ib){
  number dz;
  number zc;
  dz=_plnstrD[5]+2.*_plnstrD[6];// counter thickness
  if(il==0)
    zc=_supstrD[0]+_supstrD[6]+_plnstrD[0]+dz/2.;
  if(il==1)
    zc=_supstrD[0]-_plnstrD[1]-dz/2.;
  if(il==2)
    zc=_supstrD[1]+_plnstrD[1]+dz/2.;
  if(il==3)
    zc=_supstrD[1]-_supstrD[6]-_plnstrD[0]-dz/2.;
  if(il==0||il==2)zc=zc+(1-ib%2)*_plnstrD[2];
  if(il==1||il==3)zc=zc-(1-ib%2)*_plnstrD[2];
  return(zc);
  }  
// function to get transv. position of scint. bar=ib in layer=il
  inline number gettsc(integer il, integer ib){
  number dx;
  number x,co[2],dxt;
  dx=_plnstrD[4]+2.*_plnstrD[6];//width of sc.counter(bar+cover)
  dxt=(SCBRS[il]-1)*(dx-_plnstrD[3]);//first-last sc.count. bars distance
  if(il<2){
    co[0]=_supstrD[2];// <--top TOF-subsystem X-shift
    co[1]=_supstrD[3];// <--top TOF-subsystem Y-shift
  }
  if(il>1){
    co[0]=_supstrD[4];// <--bot TOF-subsystem X-shift
    co[1]=_supstrD[5];// <--bot TOF-subsystem Y-shift
  }
  if(_plrotmD[il]==0){  // <-- unrotated planes
    x=co[0]-dxt/2.+ib*(dx-_plnstrD[3]);
  }
  if(_plrotmD[il]==1){  // <-- rotated planes
    x=co[1]-dxt/2.+ib*(dx-_plnstrD[3]);
  }
  return(x);
  }
// functiom to get MC parameters (float) :
  inline number mcfpar(integer i){
    return _mcfparD[i-1];
  }
//  
};

