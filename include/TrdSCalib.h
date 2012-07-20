#ifndef _TrdSCalib_
#define _TrdSCalib_

#include <iostream> 
#include <map>
#include <vector>
#include <algorithm>
#include <cmath>
#include <bitset>

#include "point.h"
#include "TrdRawHit.h"
#include "TrdHRecon.h"
#include "TrdHTrack.h"
#ifdef _PGTRACK_
#include "TrExtAlignDB.h"
#include "TrdKCalib.h"
#include "TrdKCluster.h"
#endif
//#include "TrdZCalib.h"
#include "TrdHCalib.h"
#include "TrdHCharge.h"

#ifdef  _PGTRACK_
#include "TrTrack.h"
#include "TrTrackSelection.h"
#else
#include "trrec.h"
#endif

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TString.h"
#include "TROOT.h"
#include "TMath.h"
#include "TVector3.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraphSmooth.h"
#include "TSpline.h"

#ifdef _OPENMP
#include <omp.h>
#endif

namespace trdconst{

  const integer nTrdGasCir       = 10;
  const integer nTrdGasGroups    = 41;
  const integer	nTrdLadders      = 18;
  const integer nTrdLayers       = 20;
  const integer nTrdModules      = 328;
  const integer nTrdStraws       = 5248;
  const integer nTrdTubes        = 16;
 
  const number  TrdMeanMPV       = 60.0;
  const number  TrdMeanMPVSC     = 56.0;
  const number  TrdEadcMin       =  7.0;
  const number  TrdEadcMax       = 4048.0;
  const number  TrdMinPathLen3D  = 0.1;

  const integer	TrdLR_Prot_nPar	 = 12;
  const integer	TrdLR_Heli_nPar	 = 10;
  const integer	TrdLR_Elec_nPar	 = 12;
 
  /// version less than 4 only
  const integer nParLR           = 20;         // iP range[3-100 GeV/c] || 20 layers 
  
  const integer	TrdMinAdc	 = 12; 
  const integer	TrdMaxAdc	 = 4096; //3500;//4096;
  const integer nTrdMaxHits      = 200;
  const integer nTrdMinHits      = 8;

  const integer mTrdHits	 = 30;
  const integer	mTofHits  	 = 10;
  const integer	mAccHits 	 = 10;

  const integer FirstDayAMSonISS = 138;	         // day in the year 2011

  const number  TrdStrawRadius 	 = 0.30;	 // in cm 
  const number  TrdOffsetDx 	 = 0.047;	 // cm
  const number  TrdOffsetDy 	 = 0.039;	 // cm

  const number  TrdMaxAdcLen     = 2*TrdStrawRadius*TrdMaxAdc;

  const number  ToFLayer1Z       = 63.65;// 65.2;          // cm

  /// change from ver.5
  const number  CalMomMin        = 2.0;   //  3.0;
  const number  CalMomMax        = 600.0; //100.0;
  const integer CalMomNbin       = 30;    // 20;

  const number	fPXeMin 	 =  700.0;
  const number	fPXeMax	         = 1000.0;
  const integer nBinfPXe 	 = 6;

  const number  CutTrdTrkD       = 1.2;
  
  const number  mElec  		 = 0.51099892E-3;
  const number  mProt  		 = 0.93827203;
  const number  mHeli  		 = 3.725976;
  const number  nNucElec 	 = 1.0;
  const number  nNucProt         = 1.0;
  const number  nNucHeli         = 4.0;

  /// in mm
  const float  TrdCorr[328] = {	
    0.8746, 0.2304, 0.5654, 0.3403, 0.4326, 0.4100, 0.3636, 0.1811, 0.1644, 0.3510, 0.5517, 0.4199, 
    0.3125, 0.2281, 0.2380, 0.1841, 0.0157,-0.0167, 0.0039, 0.0165,-0.0824,-0.1022,-0.1162,-0.0736,
    -0.0065,-0.0088, 0.0018,-0.0363, 0.0286, 0.0221, 0.0387, 0.0716, 0.0285,-0.0602,-0.0988,-0.0065,
    -0.0383,-0.1351,-0.2650,-0.2073,-0.2003,-0.3144,-0.3044,-0.1498,-0.0910,-0.3005,-0.2580,-0.3027,
    -0.3471,-0.4325,-0.3364,-0.2066,-0.3219,-0.2970,-0.0990,-0.1980, 0.4290, 0.2926, 0.4003, 0.4494, 
    0.3170, 0.3119, 0.3272, 0.2926, 0.3230, 0.2716, 0.2845, 0.3847, 0.3722, 0.2646, 0.2906, 0.3630, 
    0.1803,-0.0452,-0.1077,-0.0323, 0.0460, 0.0802, 0.1054, 0.1787, 0.0778,-0.0615,-0.0848,-0.0087, 
    0.0019,-0.0403,-0.0118, 0.0608, 0.0763, 0.0927, 0.0870, 0.0452, 0.0670, 0.0793, 0.0747, 0.0510,
    -0.0297,-0.1049,-0.0790,-0.1132,-0.1223,-0.0290,-0.0166,-0.1343,-0.2099,-0.2001,-0.2502,-0.3379,
    -0.3651,-0.3939,-0.3672,-0.2501,-0.3138,-0.3923,-0.3908,-0.4493,-0.5014,-0.5566,-0.4922,-0.5591,
    -0.0086, 0.3984, 0.4039, 0.4192, 0.3564, 0.3267, 0.3999, 0.3788, 0.4389, 0.3782, 0.3325, 0.3561, 
    0.2193, 0.1956, 0.2688, 0.2834, 0.1813, 0.1255, 0.2624, 0.2651, 0.0976,-0.0405, 0.0132, 0.0749, 
    0.0196,-0.0346, 0.0317,-0.0147,-0.0671,-0.1075,-0.0116, 0.0011,-0.0045, 0.0461, 0.0894, 0.0549, 
    0.0200, 0.0386,-0.0227,-0.0341, 0.0006,-0.0553,-0.1098,-0.1273,-0.1306,-0.1768,-0.3015,-0.3093,
    -0.2388,-0.2531,-0.2922,-0.3293,-0.3548,-0.2998,-0.3348,-0.3601,-0.3566,-0.3770,-0.4086,-0.4631,
    -0.4865,-0.3797,-0.5830,-0.5853,-0.6225, 0.5590, 0.6421, 0.4326, 0.4453, 0.3518, 0.3443, 0.4240, 
    0.4480, 0.4217, 0.4100, 0.3475, 0.3932, 0.3729, 0.3498, 0.3236, 0.3765, 0.2978, 0.2634, 0.3439, 
    0.2216, 0.1457, 0.1852, 0.1816, 0.0912, 0.1212, 0.2029, 0.1387, 0.0671, 0.0904, 0.0416, 0.0629, 
    0.1056, 0.0203,-0.0236, 0.0001, 0.0346,-0.0743,-0.0688, 0.0211,-0.0650, 0.0131, 0.0742, 0.0820, 
    0.0016,-0.0254,-0.0487,-0.0804,-0.1945,-0.2288,-0.2721,-0.3347,-0.4014,-0.4102,-0.3018,-0.3047,
    -0.3938,-0.3920,-0.3344,-0.2524,-0.3128,-0.5134,-0.4901,-0.4887,-0.5401,-0.5181,-0.4497,-0.3425,
    -0.3610,-0.6246,-0.6916,-0.5596,-1.0506, 0.5968, 0.6007, 0.4913, 0.3989, 0.4202, 0.3813, 0.3778, 
    0.2581, 0.1809, 0.1466, 0.1537, 0.1673, 0.1903, 0.1604, 0.1454, 0.0910, 0.0874,-0.0289,-0.0319, 
    0.1785, 0.1051, 0.0914, 0.0781, 0.0067, 0.1135, 0.1107, 0.1031, 0.0941,-0.0224,-0.0276,-0.0025, 
    0.0345,-0.0331,-0.0204, 0.0522, 0.0658, 0.0976, 0.0414,-0.0327,-0.0580,-0.0655, 0.1002, 0.1429,
    -0.0048, 0.0959, 0.0153,-0.1155,-0.2205,-0.3488,-0.2229,-0.0741,-0.1208,-0.3507,-0.3141,-0.3381,
    -0.4723,-0.5368,-0.3743,-0.1955,-0.2763,-0.5589,-0.4703,-0.5320,-0.6978,-0.6035,-0.6572,-0.5556,
    -0.8837, 0.0000, 0.0000, 0.0000}; 

  //== imported from ACroot3.h on 2011.01.10
  const number Zutof   = +63.65;  	// for extrapolation to Z-UTOF
  const number Zecup   = -142.8;        // for extrapolation to ECAL-TOP
  const number Zecal   = -150.0;  	// for extrapolation to ECAL-COG
  const number Zrich   =  -73.6;   	// for extrapolation to RICH-Radiator
  const number Z_l1    = +159.10;//+159.067;   	// for extrapolation to Tracker JLayer-1
  const number Z_l9    = -136.041 ;  	// for extrapolation to Tracker JLayer-9
  const number Ztrd    = 113.55;  	// for extrapolation to TRD Center
  const number ZtrdU   = 136.75;  	// for extrapolation to TRD Upper
  const number ZtrdC  =  113.55;        // for extrapolation to TRD Center
  const number ZtrdL   =  90.35;  	// for extrapolation to TRD Lower

  /*  
      #define Geom_AMS_Utof_Z	63.65  	// for extrapolation to Z-UTOF
      #define Geom_AMS_L1N_Z   159.10   // for extrapolation to Tracker Layer-1n
      #define Geom_AMS_TRD_ZC  113.55  	// for extrapolation to TRD Center
      #define Geom_AMS_TRD_ZU  136.75  	// for extrapolation to TRD Upper
      #define Geom_AMS_TRD_ZL   90.35  	// for extrapolation to TRD Lower
  */

  const number ZL1N    = 159.10;

  const number TRD_Dx  = -220.e-4;    // global TRD X-Shift/cm        -220 mu
  const number TRD_Dy  = -240.e-4;      // global TRD Y-Shift/cm        -240 mu
  const number TRD_Dz  = 1100.e-4;      // global TRD Z-Shift/cm        1100 mu 
    
  const number TRD_Ax0 = -200.e-6;      // global TRD X-Rot (YZ-Tilt)   -340 mu-rad const.
  const number TRD_Ax1 = -120.e-6;      // global TRD X-Rot (YZ-Tilt)   -280 mu-rad linear
  const number TRD_Ax2 = 210.e-6 ;      // global TRD X-Rot (YZ-Tilt)    580 mu-rad quadr.
  
  const number TRD_Ay0 = -340.e-6;      // global TRD Y-Rot (XZ-Tilt)   -340 mu-rad const.
  const number TRD_Ay1 = 160.e-6;       // global TRD Y-Rot (XZ-Tilt)    300 mu-rad linear
  const number TRD_Ay2 = 280.e-6;       // global TRD Y-Rot (XZ-Tilt)    240 mu-rad quadr.
  
  const number TRD_Az  = 330.e-6;       // global TRD Z-Rot              375 mu-rad       
  const number TRD_As  = 0.e-6;         // global TRD XvsY module shear  100 mu-rad 

  const integer nDataRuns = 13336;      // updated on 2011.12.23
  const integer nMaxError = 99;
 
}
using namespace trdconst;

class AMSEventR;
class TrdRawHitR;
class TrdHSegmentR;
class TrdHTrackR;
class TrdTrackR;
class TrTrackR;
class TRDDB;
class TRDZCalib;
class AC_TrdHits;
class TrdSCalibR;

//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------
 
template <class T>
class myPair {
  T a, b;
 public:
  myPair (T first, T second)
    {a=first; b=second;}
  T GetMax();
};

template <class T>
T myPair<T>::GetMax() {
  T result;
  result = a>b? a : b;
  return result;
}

//--------------------------------------------------
//--- Extension of TrdRawHitR  
//--------------------------------------------------
class AC_TrdHits {
  
 protected:
  int _ilayer, _iladder, _itube, _ihaddr, _istraw, _igasgrp, _igascir, _imodule, _ihitD;
  float  _iamp, _ihitXY, _ihitR, _ihitZ;
  double _iTrdD, _iTrkD, _iLen2D, _iLen3D;
 
 public:
  int Lad, Lay, Tub, Had;

  /// id for module, straw tube, gas circuit, gas group and segment
  int Mod, Straw, GC, GG, hitD;   //== hitD: segments

  /// hit positions before alignment correction
  float hitXYraw, hitZraw;       

  /// hit positions after alignment correction
  float hitXY, hitZ, hitR;   

  /// deposited energy: raw, corrected at gas-circuit level, corrected at straw module level
  float	EadcR, EadcC1, EadcCS;

  /// hit residuals: based on trd track, track track before and after calibration
  double TrdDraw, TrkDraw, TrdD, TrkD;

  /// pathlength in 2D and 3D 
  double Len2D, Len3D;
 
  /// delta|XY-XYraw| and delta|Z-Zraw| after alignment correction
  float  Dxy, Dz;

 AC_TrdHits(): Lad(-1), Lay(-1), Tub(-1), Mod(-1), Straw(-1), GC(-1),GG(-1), 
    hitD(-1), hitXY(0), hitZ(0), hitR(0), hitXYraw(0), hitZraw(0),
    EadcR(0), EadcC1(0), EadcCS(0),
    TrdD(0), TrkD(0), TrdDraw(0), TrkDraw(0), Dxy(0), Dz(0),
    Len2D(0), Len3D(0) {
   
    _ilayer=_iladder=_itube=_ihaddr=_istraw=_igasgrp=_igascir=_imodule=_ihitD=0;
    _ihitXY =_ihitZ=_ihitR= 0;
    _iamp=0.;
    _iTrdD=_iTrkD=_iLen2D=_iLen3D=0.;
  };
  
  ~AC_TrdHits() {};

  /// check TrdRawHit information
  void CheckTrdRawHit (void)
  {std::cout << "lay=%d "<< _ilayer <<" lad= "<< _iladder <<" tub= "<< _itube <<" amp= "<< _iamp <<std::endl;};
   
  /// get trd layer: 0 - 19
  int   GetLayer()  {return _ilayer;}
  
  /// get trd ladder: 0 - 17
  int   GetLadder() {return _iladder;}

  /// get tube id: 0 - 16
  int   GetTube()   {return _itube;}

  /// get straw id: 0 - 5247 
  int   GetStraw()  {return _istraw;}

  /// get straw amplitude
  float GetAmp()    {return _iamp;}

  /// get hit hardware address: cufhh  c crate[0-1],u udr[0-6] f ufe [0-7] hh channel[0-63]
  float GetHaddr()  {return _ihaddr;}
  
  /// call AC hits from TrdRawHit
  void GetHitInfo(TrdRawHitR *rhit) {
    _iladder=rhit->Ladder; _ilayer=rhit->Layer; _itube=rhit->Tube; _iamp=rhit->Amp; _ihaddr=rhit->Haddr;
    _imodule= GetTrdModule(rhit->Layer, rhit->Ladder);
    _istraw = _imodule*nTrdTubes + _itube;
    _ihitD  = GetD();  
    _ihitXY = GetXY();  
    _ihitZ  = GetZ();
    return;
  }

  /// fill AC hits
  void SetHitPos() {
    Lay = _ilayer; Lad = _iladder; Tub = _itube; Straw = _istraw; EadcR = _iamp; Had = _ihaddr;
    hitD= _ihitD; hitZ = _ihitZ; hitXY = _ihitXY, hitR = _ihitR;
    hitXYraw = _ihitXY;  hitZraw= _ihitZ;
    hitXY    = hitXYraw; hitZ   = hitZraw;

    GC    = GetTrdGasCircuit();    //== 0 -  9
    GG    = GetTrdGasGroup();      //== 0 - 40 
    Straw = GetTrdStrawNumber();
    Mod   = GetTrdModule();
   
    return;
  }

  /// return 0: XZ, 1:YZ
  int GetD(){
    if( (_ilayer>=16) || (_ilayer<=3) ) return 1;  //== YZ
    else                                return 0;  //== XZ
  }

  /// get hit X(Y) std-coord.
  float GetXY(){
    int   lad = _iladder;
    
    if(_ilayer<12) lad++;
    if(_ilayer< 4) lad++;
    
    float r = 10.1*(float)(lad-9);
   
    if( hitD==1 ){           /// y-measurement
      if( lad>=12 ) r+=0.78; /// bulkheads
      if( lad<= 5 ) r-=0.78; 
    }      
    
    r += 0.31 + 0.62*(float)_itube;
    if(_itube >=  1) r+=0.03;
    if(_itube >=  4) r+=0.03;
    if(_itube >=  7) r+=0.03;
    if(_itube >=  9) r+=0.03;
    if(_itube >= 12) r+=0.03;
    if(_itube >= 15) r+=0.03;

    return r;
  }

  /// get hit Z std-coord.
  float GetZ(){
    int   lad = _iladder;
    
    if(_ilayer<12) lad++;
    if(_ilayer< 4) lad++;
    
    float z = 85.275 + 2.9*(float)_ilayer;
    if(lad%2==0) z += 1.45;
    
    return z;
  }

  /// convert straw id to std-coord.(XY, Z)
  void TrdStraw2Coordinates(int iStraw) {
    TrdStraw2LayLad(iStraw, Lad, Lay);
    Mod = (int) iStraw/nTrdTubes; 
    Tub = iStraw - Mod*trdconst::nTrdTubes;
    if( (Lay>=16) || (Lay<=3) ) hitD = 1;  
    else                        hitD = 0;  

    int   lad = Lad;
    if(Lad<12) lad++;
    if(Lad< 4) lad++;
    
    hitZraw = 85.275 + 2.9*(float)Lay;
    if(lad%2==0) hitZraw += 1.45;

    float r= 10.1*(float)(lad-9);
   
    if( hitD==1 ){          
      if( lad>=12 ) r +=0.78; /// bulkheads
      if( lad<= 5 ) r -=0.78; 
    }      
    
    r += 0.31 + 0.62*(float)Tub;
    if(Tub >=  1) r +=0.03;
    if(Tub >=  4) r +=0.03;
    if(Tub >=  7) r +=0.03;
    if(Tub >=  9) r +=0.03;
    if(Tub >= 12) r +=0.03;
    if(Tub >= 15) r +=0.03;

    hitXYraw = r;

    hitXY = hitXYraw;
    hitZ  = hitZraw;

    return;
  }
  
  /// get straw id
  int GetTrdStrawNumber();
  int GetTrdStrawNumber(TrdRawHitR *rhit);

  /// get module id
  int GetTrdModule(void);
  int GetTrdModule(int layer, int ladder);
  
  /// get moudle id from straw id
  int TrdStraw2Module(int Straw);

  /// get layer and ladder from straw id
  int TrdStraw2LayLad(int Straw, int &Lad, int &Lay);
  
  /// get layer from straw id
  int TrdStraw2Layer(int Straw);
  
  /// get ladder from straw id
  int TrdStraw2Ladder(int Straw);
  
  /// geet trd gas group: 1 - 41
  int GetTrdGasGroup();
  int GetTrdGasGroup(int layer, int ladder);

  /// get trd gas circuit: 1 - 10
  int GetTrdGasCircuit();
  int GetTrdGasCircuit(int layer, int ladder);

  void PrintTrdHit(Option_t *option = "") const;
  
  /// get distance from the line
  double DistanceFromLine(double cx, double cy, double ax, double ay, double bx, double by);

  /// get hit residual from tracker track or trd track 
  double GetTrdHitTrkDistance(int d, float xy, float z, AMSPoint cPtrk, AMSDir dPtrk);
  double GetTrdHitTrkDistance(AC_TrdHits *AC, AMSPoint cPtrk, AMSDir dPtrk); 

  /// get pathlength 3D after alignment correction
  double GetTrdPathLen3D(int lay, float xy, float z, AMSPoint cP, AMSDir dP);
  double GetTrdPathLen3D(AC_TrdHits *AC, AMSPoint cP, AMSDir dP);

  friend class TrdSCalib;

  ClassDef(AC_TrdHits,3)
};

//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------


class TrdSChi2Fit{
 public:
  vector<AC_TrdHits*> TrdHits;
  double RmsX;
  double RmsY;
  int nTrdHits;
  int Debug;
  
  ///  chisq minimization for multiple scattering correction
  TrdSChi2Fit():RmsX(0.),RmsY(0.),nTrdHits(0),Debug(0){};
  ClassDef(TrdSChi2Fit,1)
};


class TrdSCalibR { 

 private:
  int algo,patt,refit, pmass, pcharge, _ierror;
  int _nOntrackhit, _nOfftrackhit;
  static TrdSCalibR* head;
  #pragma omp threadprivate (head)
  vector<AC_TrdHits*> TrdNHits;
 
 private:
  /// filename for Trd Calibration
  static const char* TrdDBUpdateDir[];
  static const char* TrdCalDBUpdate[];

  /// filename for TrdAlignDB
  static const char *TrdAlignDBUpdate[];

  /// dirname  for TrdAlignExternalDB 
  static const char *TrdAlignDBExtDir[]; 

  /// filename for TrdLR_CalcIni
  static const char* TrdElectronProtonHeliumLFname[];

  /// ordering name of TrdLL Calculator
  static const char* TrdLLname[];

  /// ordering name of TrdSum8 
  static const char* TrdSum8name[];

  /// ordering name of EvtPartID
  static const char* EvtPartID[];

  /// ordering name of Trd Track
  static const char* TrdTrackTypeName[];

  /// ordering name of Trd Geometry DB
  static const char *TrdGeomUpdateDir[];
  static const char *TrdGeomDB[];

  /// char for AMSDataDir
  const char *pPath;



 public:
  TrdSChi2Fit fit;
  TF1 *fBetheBlochProton,*fBetheBlochHelium;

  /// version of TrdSCalib: curret version = 5
  unsigned int SCalibLevel;

  /// trd track methods (0: TrdHTrack, 1:TrdTrack)
  unsigned int TrdTrackLevel;

  /// trd gain correction methods (0: AC, 1:MIT, 2:AC optional)
  unsigned int TrdGainMethod, TrdAlignMethod;

  /// general particle stuffs
  float iPabs, iQabs, iRsigned, iRerrinv, iRabs, iChisq;

  /// vector differecne between trd and track directional vectors
  double TrdTkD, TrdTkDa;

  /// nr of trd modules / layer
  static vector<int> nTrdModulesPerLayer;

  /// nr of trd module id in [ladder][layer]
  static vector< vector<int> > mStraw; 	

  /// 
  static vector<int> aP, aA;
  static vector<double> xTrks, yTrks, zTrks;
 
  int Htime, iFlag;

  /// time parameters to access MIT gain correction factor
  double Xtime, Atime; 


  /// gain calibration db time period
  unsigned int FirstCalRunTime, LastCalRunTime;     

  /// alignment db time period
  unsigned int FirstAlignRunTime, LastAlignRunTime;

  /// PDFs db time period
  unsigned int FirstPdfRunTime, LastPdfRunTime; 

  /// gas circuit id and module id used for CalibLevel < 4
  int iBinGasCir, iBinModule;

  /// AMSPoint for trd and tracker track extrapolated at ToF Layer1Z position
  AMSPoint cTrd, cTrk, c0Trd;

  /// AMSDir for trd and tracker track extrapolated at ToF Layer1Z position
  AMSDir   dTrd, dTrk;
    
  /// use for CalibLevel= 1 for gain correction
  vector<TH1F*> h_TrdGasCirMPV;

  /// use for CalibLevel= 2 for gain correction
  vector<TH1F*> h_TrdModuleMPV;
  
  /// use for ver.02
  vector<TH1D*> h_TrdLR_Prot, h_TrdLR_Elec, h_TrdLR_Heli;

  /// use for Toy MC (v5)
  //vector<TF1*> fTrdLR_fElectron, fTrdLR_fProton, fTrdLR_fHelium;

  /// use for Toy MC (v6)
  //vector<TF1*> fTrdLR_fProton[nBinfPXe], fTrdLR_fHelium[nBinfPXe], fTrdLR_fElectron[nBinfPXe];
  vector< vector<TF1*> > fTrdLR_fProton;
  vector< vector<TF1*> > fTrdLR_fHelium;
  vector< vector<TF1*> > fTrdLR_fElectron;
 
  /// use for CalibLevel= 3 for gain correction
  vector<TGraphErrors*> g_TrdCalibMPV;

  /// use for CalibLevel= 4,5 for ontrack trd hits
  vector<AC_TrdHits*> TrdSHits;

  /// use for CalibLevel= 5
  TSpline3 grTrkXZ, grTrkYZ;


  /// vector array for gain period and mop values
  vector< vector<double> > TrdScalibXdaysMpv, TrdScalibMpv;     

  /// vector array for align period and position values
  vector< vector<double> > TrdScalibXdaysPos, TrdScalibPos; 

  /// number of trd hit layer [0-19]
  vector<int> nTrdHitLayer;  

  /// sum of the largest eight hit amplitudes: TrdSum8Amp[0]: before TrdSum8Amp[1]: after gain correction
  vector<float> TrdSum8Amp;   

  /// vector of truncated mean: [0]: before [1]: after gain correction
  vector<float> TruncatedMean;

  /// vector of median: [0]: before [1]: after gain correction
  vector<float> TrdMedian; 

  /// probability for (0:e, 1:p, 2:He)
  vector<double> TrdLPs;  

  /// log likelihoods (0:e/(p+e), 1:He/(p+He), 2:e/(He+e))
  vector<double> TrdLRs;      

  /// toyMC log likelihoods (0:e/(p+e), 1:He/(p+He), 2:e/(He+e))
  vector<double> TrdLRs_MC;   
 
 public:  
  int  dummy;

  /// booliean for given CalibLevel
  bool FirstCall, TrdCalib_01, TrdCalib_02, TrdCalib_03, TrdCalib_04;

  /// booliean for selected gain / align method
  bool TrdGain_01, TrdGain_02, TrdGain_03, TrdAlign_01, TrdAlign_02,  TrdAlign_03;
  
  /// booliean for AC alignment correction
  bool SetTrdGeom, SetTrdAlign; 

  /// booliean for likelihood set
  bool FirstLLCall, FirstMCCall;

  /// db time intervals in day unit
  float  FirstCalXday, LastCalXday, FirstAlignXday, LastAlignXday, FirstPdfXday, LastPdfXday; 

  /// vector container for PDFs (p, e, He)
  vector<TGraph *> TrdLR_Gr_Prot, TrdLR_Gr_Elec, TrdLR_Gr_Heli;

  /// vector for momentum bins array for protons and heliums 
  vector<double>  TrdPDF_xProt, TrdPDF_xHeli;

  /// vector for PDF values in given momentum bin 
  vector<double>  TrdPDF_nProt, TrdPDF_nHeli;

  /// vector for Xe partial pressure in electron PDFs
  vector<double>  TrdPDF_nElec_Xe0, TrdPDF_nElec_Xe1, TrdPDF_nElec_Xe2, TrdPDF_nElec_Xe3, TrdPDF_nElec_Xe4, TrdPDF_nElec_Xe5;

  /// vector for Xe partial pressure in proton and helium PDFs 
  //vector<TGraph *> grTrdS_PDF_Prot, grTrdS_PDF_Heli; 

  /// vector for Xe partial pressure in electron PDFs
  vector<TGraph *> grTrdS_PDF_Elec_Xe0, grTrdS_PDF_Elec_Xe1, grTrdS_PDF_Elec_Xe2, grTrdS_PDF_Elec_Xe3, grTrdS_PDF_Elec_Xe4, grTrdS_PDF_Elec_Xe5;

 
  /// vector of xbins for proton, helium and electron PDFs (v6)
  vector<double> TrdS_PDF_xProt, TrdS_PDF_xHeli, TrdS_PDF_xElec;

  /// vector for Xe partial pressure in proton and helium PDFs  (v6)
  vector<TGraph*> grTrdS_PDF_Prot[nBinfPXe], grTrdS_PDF_Heli[nBinfPXe]; 
  
  /// vector for proton, helium PDF values in given Rigidity and Xe partial pressure bins (v6)
  vector<double> TrdS_PDF_nProt[nBinfPXe], TrdS_PDF_nHeli[nBinfPXe], TrdS_PDF_nElec[nBinfPXe];

  /// vector for Xe partial pressure in electron PDFs (v6)
  vector<TGraph*> grTrdS_PDF_Elec[nBinfPXe];

  /// graph of Xe partial pressure as a function of xTime
  TGraph *grTrdS_Xe;

  /// trd and tracker track matching stuffs
  TF1 *fTrdSigmaDx, *fTrdSigmaDy, *fTrd95Da;
  
  /// Likelihoods Products 
  double Lprod_Proton, Lprod_Helium, Lprod_Electron;

  /// Likelihoods Products from ToyMC
  double Lprod_ProtonMC, Lprod_HeliumMC, Lprod_ElectronMC;

  /// external input to set trd low amplitude threshold
  float ExtTrdMinAdc;


  TrdSCalibR();

 ~TrdSCalibR(); 

 /// clear vectors
  void Clear(){
    TrkXcors.clear();
    TrkYcors.clear();
    
    nTrdHitLayer.clear();  
    TrdSum8Amp.clear(); 
    TruncatedMean.clear();
    TrdMedian.clear();
    TrdLPs.clear();    
    TrdLRs.clear();    
    TrdLRs_MC.clear(); 

    TrdSHits.clear();

  }
  
  int Isitsame (TrdSCalibR& ObjRawHit) {
    if (&ObjRawHit == this) return true;
    else return false;
  };


 public:

  int GetnOnTrackHit(){return _nOntrackhit;};
  int GetnOffTrackHit() {return _nOfftrackhit;};

   /// assign mStraw[ladder][layer] and nTrdMoudlesPerLayer[layer]
  void GenmStrawMatrix(int Debug);
  
  /// external assignment of the minimum hit amplitude 
  void SetExtTrdMinAdc(float trdamp){ ExtTrdMinAdc = trdamp;};

  /// get AMSPoint and AMSDir of TrdHTrack at Z= upper TOF
  bool GetcTrd(TrdHTrackR *trdht); 
  bool GetdTrd(TrdHTrackR *trdht);

  /// get AMSPoint and AMSDir of TrdTrack at Z= upper TOF
  bool GetcTrd(TrdTrackR  *trdt); 
  bool GetdTrd(TrdTrackR  *trdt);

  /// get AMSPoint and AMSDir of TrTrack at Z= upper TOF
  bool GetcTrkdTrk(TrTrackR *trt);

  /// check matching trd and track track 
  bool MatchingTrdTKtrack(float Rabs, int Debug=0); 
  
  /// convert event time to hTime
  int GetEvthTime(AMSEventR *ev, int Debug=0);

  /// convert time_t to hTime
  int GetEvthTime(time_t EvtTime, int Debug=0);

  /// convert event time to xTime
  double GetEvtxTime(AMSEventR *ev, int Debug=0);

  /// convert event time_t to xTime
  double GetEvtxTime(time_t EvtTime, int Debug=0);

  
  int GetModCalibTimePeriod(TString fname, TString hname, int Debug=0);
  
  bool Get01TrdCalibration(TString fname, int Debug=0);
  bool Get02TrdCalibration(TString fname, int Debug=0);
  bool Get03TrdCalibration(TString fname, int Debug=0);
  bool Get04TrdCalibration(TString fname, int Debug=0);
 
  
  /// get trd gain circuit id and module id
  void GetBinGasCirModule(int hTime, int CalibLevel, int iCir, int iMod, int Debug=0);
  
  /// get trd gain calibration histos (ver 1,2)
  bool GetTrdCalibHistos(int CalibLevel, int Debug=0);

  /// get trd gain calibration histos (ver 3)
  bool GetTrdV3CalibHistos(int CalibLevel, int Debug=0);

  /// get trd gain calibration histos (ver 4)
  bool GetTrdV4CalibHistos(int CalibLevel, int Debug=0);

  /// get trd gain calibration histos (ver 5)
  bool GetTrdV5CalibHistos(int Debug=0);

  /// get trd alignment db from AC method 1
  bool GetTrdAlignment(TString fname, int Debug=0);

  /// get sum of the largest trd ontrack hits
  int GetTrdSum8(int Debug=0);

  /// get truncated mean from trd ontrack hits
  int GetTruncatedMean(int Debug=0);

  /// get trd median from trd ontrack hits
  int GetTrdMedian(int Debug=0);
  


 private:
  ///--------------------------------------------------
  /// PDF function definition for electron,proton  & helium
  ///--------------------------------------------------

  double FunTrdSigmaDy(double *x, double *par){
    return 0.1*(par[0] + par[1]*exp(-par[2]*x[0]));
  };
  
  double Fun2LandauExpo(double *x, double *par){
    double cLan1  = par[0];     double MPV1   = par[1];
    double Sigma1 = par[2];     double cLan2  = par[3];
    double MPV2	  = par[4];     double Sigma2 = par[5];
    double Temp   = par[6];     double Offset = par[7];
    double fun 	  = cLan1*TMath::Landau(x[0],MPV1,Sigma1); 
    fun          += cLan2*TMath::Landau(x[0],MPV2,Sigma2);
    fun          *= 1.0/(1.0+TMath::Exp(Temp*(x[0]-Offset)));
    return fun;
  };
  
  double FunAsymGaus(double *x, double *par){
    double N 	  = par[0];    double x0      = par[1];
    double sigma  = par[2];    double delta   = par[3];
    
    double arg0 = 0, arg1 = 0, arg2 = 0, gaus = 0;
    arg0 = N/TMath::Sqrt(2*TMath::Pi()*sigma*sigma);
    if (sigma-delta != 0) arg1 = (x[0]-x0)/(sigma-delta);
    if (sigma+delta != 0) arg2 = (x[0]-x0)/(sigma+delta);
    
    if (x[0]<=x0) 
      gaus = arg0*TMath::Exp(-0.5*arg1*arg1);
    else 
      gaus = arg0*TMath::Exp(-0.5*arg2*arg2);		
    
    return gaus;
  };  

  double Fun2LandauGausExpo(double *x, double *par){
    double fun1 = Fun2LandauExpo(x,par);
    double fun2 = FunAsymGaus(x,&par[8]);
    return fun1+fun2;
  }
  
  double FunLandauGaus(Double_t *x, Double_t *par){
    /// Numeric constants
    double invsq2pi = 1./TMath::Sqrt(2*TMath::Pi()); //0.3989422804014;// (2 pi)^(-1/2)
    double mpshift  = -0.22278298;                   // Landau maximum location
    
    /// Control constants
    double np = 100.0;      // number of convolution steps
    double sc =   5.0;      // convolution extends to +-sc Gaussian sigmas
    
    /// Variables
    double xx;
    double mpc;
    double fland;
    double sum = 0.0;
    double xlow,xupp;
    double step;
    double i;
    
    /// MP shift correction
    mpc = par[1] - mpshift * par[0]; 
    
    /// Range of convolution integral
    xlow = x[0] - sc * par[3];
    xupp = x[0] + sc * par[3];
    
    step = (xupp-xlow) / np;
    
    /// Convolution integral of Landau and Gaussian by sum
    for(i=1.0; i<=np/2; i++) {
      xx = xlow + (i-.5) * step;
      fland = TMath::Landau(xx,mpc,par[0]) / par[0];
      sum += fland * TMath::Gaus(x[0],xx,par[3]);
      
      xx = xupp - (i-.5) * step;
      fland = TMath::Landau(xx,mpc,par[0]) / par[0];
      sum += fland * TMath::Gaus(x[0],xx,par[3]);
    }
    
    return (par[2] * step * sum * invsq2pi / par[3]);
  };


  double FunLandauGaus3Exp(double *x, double *par){
    double Fun1    = FunLandauGaus(x,par);

    double Temp1   = par[4];     double Offset1 = par[5];
    double Temp2   = par[6];     double Offset2 = par[7];
    double Temp3   = par[8];     double Offset3 = par[9];

    double arg1    = 1.0+TMath::Exp(Temp1*(x[0]-Offset1));
    double arg2    = 1.0+TMath::Exp(Temp2*(x[0]-Offset2));
    double arg3    = 1.0+TMath::Exp(Temp3*(Offset3-x[0]));
    double Fun2    = arg1/arg2/arg3;
    
    return Fun1*Fun2;
  };
  
  double Fun2Landau3Expo(double *x, double *par){  
    double cLan1   = par[ 0];    double MPV1    = par[ 1];
    double Sigma1  = par[ 2];    double cLan2   = par[ 3];
    double MPV2	   = par[ 4];    double Sigma2  = par[ 5];
    double Temp1   = par[ 6];    double Offset1 = par[ 7];
    double Temp2   = par[ 8];    double Offset2 = par[ 9];
    double Temp3   = par[10];    double Offset3 = par[11];
    
    double arg1    = cLan1*TMath::Landau(x[0],MPV1,Sigma1);
    arg1          += cLan2*TMath::Landau(x[0],MPV2,Sigma2);
    double arg2    = 1.0+TMath::Exp(Temp1*(x[0]-Offset1));
    double arg3    = 1.0+TMath::Exp(Temp2*(x[0]-Offset2));
    double arg4    = 1.0+TMath::Exp(Temp3*(Offset3-x[0]));
    
    return arg1/arg2/arg3/arg4;
  };
  
  double FunBetheBloch(double *x, double *par) {
    double 	P      = x[0];
    double 	XX1    = par[0];
    double 	XX2    = par[1];
    double 	XX3    = par[2];
    double	M      = par[3];
    double	Z      = par[4];
    double 	E      = TMath::Sqrt(P*P + M*M);
    double 	Beta   = P/E;
    double 	Gamma  = E/M;
  
    double dEdX    = XX1 * TMath::Power(Z,2) / TMath::Power(Beta,2)
      * (TMath::Log(XX2*Beta) - TMath::Power(Beta,2) - XX3 * TMath::Log(Beta*Gamma) );
    
    return dEdX;
  }

  //--------------------------------------------------
  ///	x[0] = Eadc, par[0] = Xe bin, par[1] = Rigidity bin
  //
  double TrdS_PDF_fProton(double *x, double *par) {
    
    int     iXe 	= (int) par[0];
    int     iR 	        = (int) par[1];
    double   L 		= 0.0;	
    if (x[0]<TrdMinAdc) {
      L = grTrdS_PDF_Prot[iXe].at(iR)->Eval(TrdMinAdc) * 1.0/(1.0+TMath::Exp(-10.0*(x[0]-TrdMinAdc)));	
    } else if (x[0]>TrdMaxAdcLen) {
      L = grTrdS_PDF_Prot[iXe].at(iR)->Eval(TrdMaxAdcLen) * 1.0/(1.0+TMath::Exp(-1.0*(TrdMaxAdcLen-x[0])));
    } else {
      L = grTrdS_PDF_Prot[iXe].at(iR)->Eval(x[0]);
    }
    return max(1E-10,L/TrdS_PDF_nProt[iXe].at(iR));
  }

  //--------------------------------------------------
  ///	x[0] = Eadc, par[0] = Xe bin, par[1] = Rigidity bin
  //
  double TrdS_PDF_fHelium(double *x, double *par) {
    int     iXe 	= (int) par[0];
    int     iR 	        = (int) par[1];
    double   L 		= 0.0;	
    if (x[0]<TrdMinAdc) {
      L = grTrdS_PDF_Heli[iXe].at(iR)->Eval(TrdMinAdc) * 1.0/(1.0+exp(-10.0*(x[0]-TrdMinAdc)));	
    } else if (x[0]>TrdMaxAdcLen) {
      L = grTrdS_PDF_Heli[iXe].at(iR)->Eval(TrdMaxAdcLen) * 1.0/(1.0+exp(-1.0*(TrdMaxAdcLen-x[0])));
    } else {
      L = grTrdS_PDF_Heli[iXe].at(iR)->Eval(x[0]);
    }
    return max(1E-10,L/TrdS_PDF_nHeli[iXe].at(iR));
  }

  //--------------------------------------------------
  ///	x[0] = Eadc, par[0] = Trd layer, par[1] = Rigidity bin
  //
  double TrdS_PDF_fElectron(double *x, double *par) {
    int     iXe 	= (int) par[0];
    int     iL 	        = (int) par[1];
    double   L 		= 0.0;	
    
    if (x[0]<TrdMinAdc) {
      L = grTrdS_PDF_Elec[iXe].at(iL)->Eval(TrdMinAdc) * 1.0/(1.0+exp(-10.0*(x[0]-TrdMinAdc)));	
    } else if (x[0]>TrdMaxAdcLen) {
      L = grTrdS_PDF_Elec[iXe].at(iL)->Eval(TrdMaxAdcLen) * 1.0/(1.0+exp(-1.0*(TrdMaxAdcLen-x[0])));
    } else {
      L = grTrdS_PDF_Elec[iXe].at(iL)->Eval(x[0]);
    }
    return max(1E-10,L/TrdS_PDF_nElec[iXe].at(iL));
  }
 
  //--------------------------------------------------


 public:
  
  /// initiate trd gain/alignment calibration
  bool Init(int CalibLevel, int Debug=0);
 
  /// get log binning 
  vector<double> GenLogBinning(int nBinLog, double Tmin, double Tmax);

  /// Proton BetheBloch Correction
  double GetProtonBetheBlochCorrFactor(float Pabs);

  /// Helium BetheBloch Correction
  double GetHeliumBetheBlochCorrFactor(float Pabs);

  /// get Xe partial pressure in day unit
  int GetXeInterval(double xDay); 

  /// initiate likelihood calculation 
  bool TrdLR_CalcIniPDF(int Debug=0);

  /// calculate likelihoods
  int TrdLR_CalcXe(double xDay, float Rabs, int iFlag, int Debug=0);

  /// initiate toyMC likelihood calculation
  bool TrdLR_MC_CalcIniXe(int Debug=0);

  /// calcualte toyMC likelihoods
  int TrdLR_MC_CalcXe(double xDay, float Rabs, vector<bool> PartId, int Debug=0);
 
  /// add new align correction based on module level on 2012.02.07
  int GetUnknownHitPos(TrTrackR *Trtrk, float &alx, float &aly, float gz);
  int GetUnknownHitPos(TrdTrackR *Trdtrk, float &alx, float &aly, float gz);
  int GetUnknownHitPos(TrdHTrackR *TrdHtrk, float &alx, float &aly, float gz);
  int ProcessAlignCorrection(TrdHTrackR *TrdHtrk, AC_TrdHits *ACHit, int Debug=0);
  int ProcessAlignCorrection(TrdTrackR *Trdtrk, AC_TrdHits *ACHit, int Debug=0);
  /// add new align correction based on layer level on 2012.02.22
  int ProcessAlignCorrection(unsigned int iTrdAlignMethod, TrdTrackR *Trdtrk, AC_TrdHits *ACHit, int Debug=0);
  
  int RootTGraph2VectorX(TGraph *gr, vector<double> &vx);
  int RootTGraph2VectorY(TGraph *gr, vector<double> &vy);
  vector<double> RootGetXbinsTH1(TH1 *hist);

  
  /// search the clostest time bin  
  bool TrdScalibMinDist(double xDayRef, int xP, int iMod);
  bool TrdScalibMinDist(double xDayRef, int xP, vector<double> &TrdScalibXdays);

  /// search the closest time bin 
  int TrdScalibBinarySearch(double key, int iMod, int Debug=0); 
  int TrdScalibBinarySearch(double key, vector<double> &TrdScalibXdays, int Debug=0);

  /// linear interpolation of mop value at the closest time bin 
  double TrdScalibInterpolate(int iMod, double xDayRef, int &xP, int Debug=0);
  double TrdScalibInterpolate(double xDayRef, int &xP, vector<double> &TrdScalibXdays, vector<double> &TrdScalibVal, int Debug=0);

  /// initiate TrdSCalib ver.4
  int InitTrdSCalib(int CalVer, int TrdTrackType, int Debug=0);

  /// initiate TrdSCalib ver.5
  int InitNewTrdSCalib(int CalVer, int TrdTrackType, int GainMethod, int AlignMethod, int Debug=0);

  /// process trd event at given particle event
  int ProcessTrdEvt(AMSEventR *pev, int Debug=0);
 
  /// process trd event with tracker track instead of trd standalone
  int ProcessTrdEvtWithTrTrack(AMSEventR *pev, TrTrackR *Trtrk, int Debug=0);

  /// process trdZ event with tracker track instead of trd standalone
  int ProcessTrdZ(AMSEventR *pev, TrTrackR *Trtrk, int Debug=0);

  /// process trdHtrack hits 
  int ProcessTrdHit(TrdHTrackR *TrdHtrk, int Debug=0);

  /// process trdtrack hits
  int ProcessTrdHit(TrdTrackR  *Trdtrk, int Debug=0);

  /// process trd hits with tracker track
  int ProcessTrdHit(TrTrackR *Trtrk, int Debug=0);

  /// process tracker track
  int ProcessTrTrack(TrTrackR* Trtrk);

  /// calculate particle likelihoods in gbatch
  int BuildTrdSCalib(time_t evut, double fMom, TrdHTrackR *TrdHtrk, TrTrackR *Trtrk, double &s1,double &s2, double &s3 , int Debug=0);

  /// get nr. of hits per layer ontrack 
  int GetnTrdHitLayer( vector<AC_TrdHits*> &TrdHits, int Debug=0);
  
  /// fill TrdSHits which passing starw tube gas volume without MS correction
  vector<int> TrdFillHits( vector<AC_TrdHits*> &TrdHits, int Debug=0);

  /// fill TrdSHits which passing starw tube gas volume with MS correction
  vector<int> TrdFillHits2( vector<AC_TrdHits*> &TrdHits, int Debug=0);

  /// get 3D path length
  vector<int> CalPathLen3D(vector<AC_TrdHits*> &TrdHits, TrTrackR *Trtrk, int TrdStrkLevel, int Debug=0);

  /// multiple-scattering(MS) correction
  int IterateTrk4MS_ROOT(float aRig, vector<AC_TrdHits*> &TrdHits, vector<float> &Results, int Debug=0);

  /// chisq routine for MS correction
  int TrdTrkChi2(vector<AC_TrdHits*> TrdHits, double DeltaX, double DeltaY, double &Chi2, int &nTrdHits, int Debug=0);

  /// check tracker layer 1 hit
  bool NeedTrkSpline(TrTrackR *Trtrk, int Debug=0);

  /// define track spline from track track
  int GetTrkSpline(int msFlag, int Debug=0);

  /// get local track vector within MS correction
  int GetLocalTrkVec(float zTrdCor, AMSPoint &cTrk, AMSDir &bTrk, double DeltaX, double DeltaY, int Debug=0);
  
  /// get TrdHits with MS correction
  int GetTrdNewHits_ms(vector<AC_TrdHits*> TrdHits, int Debug=0);

  /// get TrdHits without MS correction
  int GetTrdNewHits(vector<AC_TrdHits*> TrdHits, int Debug=0);

  /// apply gain & alignment correction factor to hit 
  int GetThisTrdHit(AC_TrdHits* &TrdHit, int Debug=0);

  /// get track extrapolation at various position 
  int GetTrkCoordinates(TrTrackR *Trtrk, int Debug=0);

  /// sort hits according to resituals
  bool TrdHitSortFunction_TrkD(AC_TrdHits* Hit1, AC_TrdHits* Hit2) {return (fabs(Hit1->TrkD)<fabs(Hit2->TrkD));}

  /// sort hits according to layer
  bool TrdHitSortFunction_Lay(AC_TrdHits* Hit1, AC_TrdHits* Hit2) {return (fabs(Hit1->Lay)<fabs(Hit2->Lay));}

  /// fill TrdNHits from TrdRawHit at given event
  int InitiateTrdRawHit(AMSEventR *pev, vector<AC_TrdHits> &TrdHits, int Debug=0);
  int InitiateTrdRawHit(AMSEventR *pev, vector<AC_TrdHits*> &TrdHits, int Debug=0);
  int InitiateTrdRawHit(AMSEventR *pev, int Debug=0);
 
  /// clear TrdNHits memory
  void ClearTrdNHits(){for(vector<AC_TrdHits*>::iterator i= TrdNHits.begin(); i != TrdNHits.end(); ++i) if(*i) delete *i;}

  /// clear TrdSHits memory
  void ClearTrdSHits(){for(vector<AC_TrdHits*>::iterator i= TrdSHits.begin(); i != TrdSHits.end(); ++i) if(*i) delete *i;}
  
  /// Trd Alignment: TrdAlignMethod = 2 from Z.Weng  
 #ifdef _PGTRACK_
 TrdKCalib _DB_instance;  //== TrdAlignMethod = 2 from Z.Weng 
#endif
  /// Trd Alignment: TrdAlignMethod = 3 from V.Zhukov
  //TRDZCalib thetrdz;


 public:
  
  /// vector for multiple scattering correction at several detector z coordinates
  /// TrkXcors[0] = X_UT; TrkXcors[1] = X_TL; TrkXcors[2] = X_TC; TrkXcors[3] = X_TU;		
  /// TrkXcors[4] = X_L1; TrkXcors[6] = X_L9;		
  /// TrkXcors[5] = trCooL1[0];      //== L1 hit coord.
  /// TrkXcors[7] = trCooL9[0];      //== L9 hit coord. 
  vector<double> TrkXcors, TrkYcors; 

  /// Extra Trd Align: Layer-1N  +159.067 cm
  float  Tht_L1, Phi_L1, X_L1, Y_L1;  

  /// TRD-Upper +136.75 cm, TRD-Center +113.55 cm, TRD-Lower +90.35 cm 
  float  X_TU, Y_TU, X_TC, Y_TC, X_TL, Y_TL; 

  /// Upper-TOF  +63.65 cm (between Plane 1/2)
  float  Tht_UT, Phi_UT, X_UT, Y_UT;

  /// RICH  -73.6 cm (Center of Radiator)
  float  Tht_RI, Phi_RI, X_RI, Y_RI; 

  /// Layer-9   -136.041 cm
  float  Tht_L9, Phi_L9, X_L9, Y_L9;    

  /// graph for z-interpolation
  TGraph      *TG[2]; 

  /// spline for z-interpolation
  TSpline3    *TS[2]; 

  /// TrdGeom global shift and rotation
  float TRD_SHIFT[3], TRD_ROT[3]; //== TrdGeom global shift
  
  /// module displacement from shimming: Z-Offset/mu-m 
  float Mod_Dz[328];        
  
  /// module displacement from shimming:  Tilt/mu-rad
  float Mod_Arz[328];      

  /// initiate TrdMove and TrdSHim
  bool  AC_InitTrdGeom(int Debug=0);

  /// read trd shift parameters
  int   AC_InitTrdMove( char* fname, int Debug=0 );

  /// read trd shimming parameters
  int   AC_InitTrdShim( char* fname, int Debug=0 );

  /// make splines from trd geometry parameters given (1., 1., 0, 0) 
  void  AC_InitInterpolate2Z(float dZ_UT, float dZ_L1, float dY_UT, float dY_L1);

  /// interpolate a position along the wire at Z
  float AC_Interpolate2Z(float Z, int D);

  /// clear graph and spline vectors
  void  AC_ClearInterpolate2Z();

  /// apply module movement
  int   AC_ModAlign(AC_TrdHits* &ACHit, int Debug=0);



  static TrdSCalibR* gethead();
  
  static void  KillPointer(){
    delete head;
    head=0; 
  }
 

  ClassDef(TrdSCalibR,7)
    };

#endif

