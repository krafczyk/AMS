#ifndef __TRDDBC__
#define __TRDDBC__
#include <typedefs.h>

#include <iostream.h>
#include <fstream.h>

namespace trdconst{
// counting always from 0 and from top to bottom
const uinteger maxo=7;   
const uinteger mtrdo=1;   
const uinteger maxlay=20;
const uinteger maxlad=21;
const uinteger maxtube=16;
const uinteger TRDROTMATRIXNO=9001;
}
using trdconst::mtrdo;
using trdconst::maxo;
using trdconst::TRDROTMATRIXNO;
class TRDDBc{

private:
 number _nrm[3][3];
 geant _coo[3];
 uinteger _status;
 uinteger _gid;

   // Media    
   static char* _OctagonMedia[maxo];
   static char* _TubesMedia;
   static char* _ITubesMedia;
   static char* _RadiatorMedia;
   static char* _TubesBoxMedia;

   // Quantities
    static uinteger   _TRDOctagonNo;
    static uinteger   _OctagonNo;
    static uinteger   _PrimaryOctagonNo;
    static uinteger   _PrimaryOctagon[maxo];
    static uinteger   _NoTRDOctagons[mtrdo];
    static  uinteger  _LayersNo[mtrdo];
    static  uinteger  _LaddersNo[mtrdo][trdconst::maxlay];
    static uinteger   _TubesNo[mtrdo][trdconst::maxlay][trdconst::maxlad];
    static uinteger   _NumberTubes;
    static uinteger   _NumberLadders;
    static const number  _TubeInnerDiameter;
    static const number  _TubeWallThickness;
    static const number  _TubeBoxThickness;
    static const number  _LadderThickness;
    static const integer _LadderOrientation[mtrdo][trdconst::maxlay];    

 //Sizes    
   static number    _OctagonDimensions[maxo][10]; 
   static number    _LaddersDimensions[mtrdo][trdconst::maxlay][trdconst::maxlad][3];
   static number    _TubesDimensions[mtrdo][trdconst::maxlay][trdconst::maxlad][3];    
   static number    _TubesBoxDimensions[mtrdo][trdconst::maxlay][trdconst::maxlad][10];    
   static number    _RadiatorDimensions[mtrdo][trdconst::maxlay][trdconst::maxlad][3];

  // Positions & Orientations
  
   static TRDDBc * _HeadOctagonPos;
   static TRDDBc * _HeadLadderPos;
   static TRDDBc * _HeadRadiatorPos;
   static TRDDBc * _HeadTubeBoxPos;
   static TRDDBc * _HeadTubePos;

  // Debug
  static uinteger _check(uinteger oct, uinteger layer=0,
  uinteger ladder=0,uinteger tube=0);

public:

  TRDDBc(): _status(0){}
   static void init( );
   static void read();
   static void write();
   static char* CodeLad(uinteger gid); 
   //  getnumbers for elements   
   static uinteger getnumTube(uinteger tube,uinteger ladder, uinteger layer, uinteger oct=1);
   static uinteger getnumLadder(uinteger ladder, uinteger layer, uinteger oct=1);


   static uinteger getnumOctagon(uinteger oct);


   // GetSet Structures

 static void SetOctagon(uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3], uinteger &gid);
 static void GetOctagon(uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid);

 static void SetLadder(uinteger ladder, uinteger layer, uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3],uinteger gid);
 static void GetLadder(uinteger ladder, uinteger layer, uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid);

 static void SetRadiator(uinteger ladder, uinteger layer, uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3],uinteger gid);
 static void GetRadiator(uinteger ladder, uinteger layer, uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid);

 static void SetTubeBox(uinteger ladder, uinteger layer, uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3],uinteger gid);
 static void GetTubeBox(uinteger ladder, uinteger layer, uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid);


 static void SetTube(uinteger tube, uinteger ladder, uinteger layer, uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3],uinteger gid);
 static void GetTube(uinteger tube, uinteger ladder, uinteger layer, uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid);
   static uinteger GetStatusTube(uinteger tube, uinteger ladder, uinteger layer, uinteger oct){
      return _HeadTubePos[getnumTube(tube,layer,ladder,oct)]._status; }


   // Public function to get Quantities
   static uinteger   TRDOctagonNo(){return _TRDOctagonNo;}
   static uinteger   PrimaryOctagonNo(){return _PrimaryOctagonNo;}
   static integer   GetPrimaryOctagon(uinteger oct){return oct<OctagonNo()?integer(_PrimaryOctagon[oct]):-1;}
   static uinteger   NoTRDOctagons(uinteger oct){return oct<TRDOctagonNo()?_NoTRDOctagons[oct]:0;}
   static uinteger   OctagonNo(){return _OctagonNo;}
   static  uinteger  LayersNo(uinteger toct){return toct<TRDOctagonNo()?_LayersNo[toct]:0;}
   static  uinteger  LaddersNo(uinteger toct, uinteger lay){return lay<LayersNo(toct)?_LaddersNo[toct][lay]:0;}
   static uinteger   TubesNo(uinteger toct, uinteger lay, uinteger lad){return lad<LaddersNo(toct,lay)?_TubesNo[toct][lay][lad]:0;}
   

    static  number  TubeInnerDiameter(){return _TubeInnerDiameter;}
    static  number  TubeWallThickness(){return _TubeWallThickness;}
    static  number  TubeBoxThickness(){return _TubeBoxThickness;}
    static  number  LadderThickness(){return _LadderThickness;}
    static  integer LadderOrientation(uinteger oct, uinteger lay){return lay<LayersNo(oct)?_LadderOrientation[oct][lay]:0;}     
    static  integer LadderShift(uinteger oct, uinteger lay,uinteger lad){return lad%2?-1:1;}



 //Sizes    
   static number&    OctagonDimensions(uinteger oct, uinteger index); 
   static number&    LaddersDimensions(uinteger toct, uinteger lay, uinteger lad, uinteger index);
   static number&    TubesDimensions(uinteger toct, uinteger lay, uinteger lad,uinteger index);    
   static number    ITubesDimensions(uinteger toct, uinteger lay, uinteger lad,uinteger index);    
   static number&    TubesBoxDimensions(uinteger toct, uinteger lay, uinteger lad,uinteger index); 
   static number&    RadiatorDimensions(uinteger toct, uinteger lay, uinteger lad,uinteger index);

 //Media

   static char* OctagonMedia(uinteger oct){return oct<maxo?_OctagonMedia[oct]:0;}
   static char* TubesMedia(){return _TubesMedia;}
   static char* ITubesMedia(){return _ITubesMedia;}
   static char* RadiatorMedia(){return _RadiatorMedia;}
   static char* TubesBoxMedia(){return _TubesBoxMedia;}
   static char* LaddersMedia(){return _OctagonMedia[_NoTRDOctagons[0]];}



  
};


class TRDDBcI{
public:
TRDDBcI();
~TRDDBcI();
private:
 static integer _Count;
};
static TRDDBcI trdI;


#endif
