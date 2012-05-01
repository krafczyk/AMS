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
#include "TrExtAlignDB.h"
#include "TrdKCalib.h"
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

#include "cfortran.h"
#include "d_blobel.h"

extern "C" {
   void dvallin_(int*, double*, int*);
   void dvalley_(double*, double*, int*);
}



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

  const integer nParLR           = 20;         // iP range[3-100 GeV/c] || 20 layers
  
  const integer	TrdMinAdc	 = 12; //10; //12; //15;
  const integer	TrdMaxAdc	 = 4048; //3500;//4048;
  const integer nTrdMaxHits      = 100;

  const integer mTrdHits	 = 30;
  const integer	mTofHits  	 = 10;
  const integer	mAccHits 	 = 10;

  const integer FirstDayAMSonISS = 138;	         // day in the year 2011

  const number  TrdStrawRadius 	 = 0.30;	 // in cm 
  const number  TrdOffsetDx 	 = 0.047;	 // cm
  const number  TrdOffsetDy 	 = 0.039;	 // cm

  const number  ToFLayer1Z       = 63.65;// 65.2;          // cm

  /// change from ver.5
  const number  CalMomMin        = 2.0;   //  3.0;
  const number  CalMomMax        = 600.0; //100.0;
  const integer CalMomNbin       = 30;    // 20;

  const number	fPXeMin 	 =  700.0;
  const number	fPXeMax	         = 1000.0;
  const integer nBinfPXe 	 = 6;

  const number  CutTrdTrkD       = 1.0;
  
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
  int Mod, Straw, GC, GG, hitD;   //== hitD: segments
  float hitXY, hitZ, hitR;              
  float hitXYraw, hitZraw;        //== hit positions before align
  float	EadcR, EadcCM, EadcC1, EadcCS;
  double TrdD, TrkD, Len2D, Len3D;
  double TrdDraw, TrkDraw;
  float  Dxy, Dz;

 AC_TrdHits(): Lad(-1), Lay(-1), Tub(-1), Mod(-1), Straw(-1), GC(-1),GG(-1), 
    hitD(-1), hitXY(0), hitZ(0), hitR(0), hitXYraw(0), hitZraw(0),
    EadcR(0), EadcCM(0), EadcC1(0), EadcCS(0),
    TrdD(0), TrkD(0), TrdDraw(0), TrkDraw(0), Dxy(0), Dz(0),
    Len2D(0), Len3D(0) {
   
    _ilayer=_iladder=_itube=_ihaddr=_istraw=_igasgrp=_igascir=_imodule=_ihitD=0;
    _ihitXY =_ihitZ=_ihitR= 0;
    _iamp=0.;
    _iTrdD=_iTrkD=_iLen2D=_iLen3D=0.;
  };
  
  ~AC_TrdHits() {};

  void CheckTrdRawHit (void)
  {std::cout << "lay=%d "<< _ilayer <<" lad= "<< _iladder <<" tub= "<< _itube <<" amp= "<< _iamp <<std::endl;};
   
  
  int   GetLayer()  {return _ilayer;};
  int   GetLadder() {return _iladder;};
  int   GetTube()   {return _itube;};
  int   GetStraw()  {return _istraw;};
  float GetAmp()    {return _iamp;};
  float GetHaddr()  {return _ihaddr;};

  void GetHitInfo(TrdRawHitR *rhit) {
    _iladder=rhit->Ladder; _ilayer=rhit->Layer; _itube=rhit->Tube; _iamp=rhit->Amp; _ihaddr=rhit->Haddr;
    _imodule= GetTrdModule(rhit->Layer, rhit->Ladder);
    _istraw = _imodule*nTrdTubes + _itube;
    _ihitD  = GetD();  
    _ihitXY = GetXY(0);  
    _ihitZ  = GetZ(0);
    return;
  };

  
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
  };

  
  int GetD(){
    if( (_ilayer>=16) || (_ilayer<=3) ) return 1;  //== YZ
    else                                return 0;  //== XZ
  };

  float GetXY(int corr){
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
  };

  float GetZ(int corr){
    int   lad = _iladder;
    
    if(_ilayer<12) lad++;
    if(_ilayer< 4) lad++;
    
    float z = 85.275 + 2.9*(float)_ilayer;
    if(lad%2==0) z += 1.45;
    
    return z;
  };

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
  };

  int GetTrdStrawNumber();
  int GetTrdStrawNumber(TrdRawHitR *rhit);

  int GetTrdModule(void);
  int GetTrdModule(int layer, int ladder); 
  
  int TrdStraw2Module(int Straw);

  int TrdStraw2LayLad(int Straw, int &Lad, int &Lay);

  int TrdStraw2Layer(int Straw);

  int TrdStraw2Ladder(int Straw); 

  int GetTrdGasGroup();
  int GetTrdGasGroup(int layer, int ladder);

  int GetTrdGasCircuit();
  int GetTrdGasCircuit(int layer, int ladder);

  void PrintTrdHit(Option_t *option = "") const;

  double GetTrdModuleAlignmentCorrection(int Layer, int Module);
  
  double DistanceFromLine(double cx, double cy, double ax, double ay, double bx, double by);

  double GetTrdHitTrkDistance(int d, float xy, float z, AMSPoint cPtrk, AMSDir dPtrk);
  double GetTrdHitTrkDistance(AC_TrdHits *AC, AMSPoint cPtrk, AMSDir dPtrk); 

  double GetTrdPathLen3D(int lay, float xy, float z, AMSPoint cP, AMSDir dP);
  double GetTrdPathLen3D(AC_TrdHits *AC, AMSPoint cP, AMSDir dP);
  

  friend class TrdSCalib;

  ClassDef(AC_TrdHits,2)
};

//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------



class TrdSCalibR { 

 private:
  int algo,patt,refit, pmass, pcharge;
  static TrdSCalibR* head[64];
  vector<AC_TrdHits*> TrdNHits;
 
 private:
  /// filename for GetTrdHits
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

  static const char *TrdGeomUpdateDir[];
  static const char *TrdGeomDB[];
 
  bool p_Graph[nParLR];
  TGraph *p_gr_L[nParLR];
  
  bool he_Graph[nParLR];
  TGraph *he_gr_L[nParLR];

  bool e_Graph[nParLR];
  TGraph *e_gr_L[nParLR];


  /// extended TrdRawHit class
 public:
  TF1 *fBetheBlochProton,*fBetheBlochHelium;
  unsigned int SCalibLevel;
  unsigned int TrdTrackLevel;
  unsigned int TrdGainMethod, TrdAlignMethod;

  int Htime, iFlag;
  double Xtime, Atime; 
  float iPabs, iQabs, iRsigned, iRerrinv, iRabs, iChisq;
  double TrdTkD, TrdTkDa;

  static vector<int> nTrdModulesPerLayer;
  static vector< vector<int> > mStraw; 	
  static vector<int> aP, aA;
  static vector<double> xTrks, yTrks, zTrks;
  
  vector<TH1F*> h_TrdGasCirMPV;
  vector<TH1F*> h_TrdModuleMPV;
  
  /// use for ver.02
  vector<TH1D*> h_TrdLR_Prot, h_TrdLR_Elec, h_TrdLR_Heli;

  /// use for Toy MC
  vector<TF1*> fTrdLR_fElectron, fTrdLR_fProton, fTrdLR_fHelium;
 
  /// use for ver.03
  vector<TGraphErrors*> g_TrdCalibMPV;

  /// use for ver.04
  vector<AC_TrdHits*> TrdSHits;

  /// + use for ver.05
  TSpline3* grTrkXZ;  
  TSpline3* grTrkYZ;

  vector<TF1*> fTrdLR_fElectronXe0, fTrdLR_fElectronXe1, fTrdLR_fElectronXe2;
  vector<TF1*> fTrdLR_fElectronXe3, fTrdLR_fElectronXe4, fTrdLR_fElectronXe5;
  

  vector< vector<double> > TrdScalibXdaysMpv; //== vector array for gain run period
  vector< vector<double> > TrdScalibMpv;      //== vector array for gain calib mop

  vector< vector<double> > TrdScalibXdaysPos; //== vector array for align run period
  vector< vector<double> > TrdScalibPos;      //== vector array for align position

  vector<int> nTrdHitLayer;    //== number of trd hit layer [1-20]
  vector<float> TrdSum8Amp;    //== the largest 8 sum of signals after sorting
  vector<float> TruncatedMean; //== truncated mean of signals after sorting
  vector<float> TrdMedian;     //== median of signals after sorting
  vector<double> TrdLRs;       //== log likelihoods (e/p, He/p, e/He)
  vector<double> TrdLRs_MC;    //== log likelihoods from toyMC
  
  unsigned int FirstCalRunTime, LastCalRunTime;     //== gain calibration run time period
  unsigned int FirstAlignRunTime, LastAlignRunTime; //== trd alignment run time period
  unsigned int FirstPdfRunTime, LastPdfRunTime;     //== used PDF run time period

  int iBinGasCir, iBinModule;

  AMSPoint cTrd, cTrk, c0Trd;
  AMSDir   dTrd, dTrk;
  
 public:  
  int  dummy;
  bool FirstCall, TrdCalib_01, TrdCalib_02, TrdCalib_03, TrdCalib_04;
  bool TrdGain_01, TrdGain_02, TrdGain_03;
  bool SetTrdGeom, SetTrdAlign, TrdAlign_01, TrdAlign_02,  TrdAlign_03; 
  bool FirstLLCall, FirstMCCall;
  float  FirstCalXday, LastCalXday, FirstAlignXday, LastAlignXday;    //== ver.4
  float  FirstPdfXday, LastPdfXday;    //== ver.4
  
  vector<double>	   TrdLR_xProt, TrdLR_xHeli, TrdLR_xElec;
  vector<double>	   TrdLR_nProt, TrdLR_nHeli, TrdLR_nElec;
  vector< vector<double> > TrdLR_pProt, TrdLR_pHeli, TrdLR_pElec;
  
  vector<TGraph *> TrdLR_Gr_Prot, TrdLR_Gr_Elec, TrdLR_Gr_Heli;

  /// add on 2012.02.01 to consider partial Xe pressure
  vector<double>  TrdPDF_xProt, TrdPDF_nProt;
  vector<double>  TrdPDF_xHeli, TrdPDF_nHeli;
  vector<double>  TrdPDF_nElec_Xe0, TrdPDF_nElec_Xe1, TrdPDF_nElec_Xe2; 
  vector<double>  TrdPDF_nElec_Xe3, TrdPDF_nElec_Xe4, TrdPDF_nElec_Xe5;
  vector<TGraph *> grTrdS_PDF_Prot, grTrdS_PDF_Heli; 
  vector<TGraph *> grTrdS_PDF_Elec_Xe0, grTrdS_PDF_Elec_Xe1, grTrdS_PDF_Elec_Xe2;
  vector<TGraph *> grTrdS_PDF_Elec_Xe3, grTrdS_PDF_Elec_Xe4, grTrdS_PDF_Elec_Xe5;
  TGraph *grTrdS_Xe;

  TF1 *fTrdSigmaDx, *fTrdSigmaDy, *fTrd95Da;
  TF1 *fTrdLR;
  
  /// Likelihoods normalized
  double Lprod_Proton, Lprod_Helium, Lprod_Electron;
  double L4prod_Proton, L4prod_Helium, L4prod_Electron;
  /// Likelihoods normalized from ToyMC
  double Lprod_ProtonMC, Lprod_HeliumMC, Lprod_ElectronMC;
  /// external input to set trd low amplitude threshold
  float ExtTrdMinAdc;


  TrdSCalibR();

 ~TrdSCalibR(); 
  void Clear(){
    nTrdHitLayer.clear();  
    TrdSum8Amp.clear(); 
    TruncatedMean.clear();
    TrdMedian.clear();
    TrdLRs.clear();    
    TrdLRs_MC.clear();   
    TrdSHits.clear();
  }
  
  int Isitsame (TrdSCalibR& ObjRawHit) {
    if (&ObjRawHit == this) return true;
    else return false;
  };


 public:

  void GenmStrawMatrix(int Debug);
  void ReadLFname();

  void SetExtTrdMinAdc(float trdamp){ ExtTrdMinAdc = trdamp;};

  ///(Z= upper TOF)
  bool GetcTrd(TrdHTrackR *trdht); 
  bool GetdTrd(TrdHTrackR *trdht);
  bool GetcTrd(TrdTrackR  *trdt); 
  bool GetdTrd(TrdTrackR  *trdt);
  bool GetcTrkdTrk(TrTrackR *trt);

  bool MatchingTrdTKtrack(float Rabs, int Debug); 

 public:
  int GetEvthTime(AMSEventR *ev, int Debug);
  int GetEvthTime(time_t EvtTime, int Debug);

  double GetEvtxTime(AMSEventR *ev, int Debug);
  double GetEvtxTime(time_t EvtTime, int Debug);

  int GetModCalibTimePeriod(TString fname, TString hname, int Debug);
  
  bool Get01TrdCalibration(TString fname, int Debug);
  bool Get02TrdCalibration(TString fname, int Debug);
  bool Get03TrdCalibration(TString fname, int Debug);
  bool Get04TrdCalibration(TString fname, int Debug);
 
  bool GetTrdCalibHistos(int CalibLevel, int Debug);
  bool GetTrdV3CalibHistos(int CalibLevel, int Debug);
  bool GetTrdV4CalibHistos(int CalibLevel, int Debug);
  bool GetTrdV5CalibHistos(int Debug);

  bool Get01TrdAlignment(TString fname, int Debug);

  
  void GetBinGasCirModule(int hTime, int CalibLevel, int iCir, int iMod, int Debug);

  int GetTrdSum8(int Debug);

  int GetTruncatedMean(int Debug);
  int GetTrdMedian(int Debug);
  


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
	
    //double dEdX  = XX1*pow(Z,2)/pow(Beta,2)*(0.5*log(XX2*pow(Beta,2))-pow(Beta,2)-XX3*log(Beta*Gamma));
    double dEdX    = XX1 * TMath::Power(Z,2) / TMath::Power(Beta,2)
      * (TMath::Log(XX2*Beta) - TMath::Power(Beta,2) - XX3 * TMath::Log(Beta*Gamma) );
    
    return dEdX;
  }
  
  //--------------------------------------------------
  ///	x[0] = Eadc, par[0] = iP = Momentum Bin
  double TrdLR_fProton_v02(double *x, double *par) {
    
    int         iP    = (int) par[0];	
    double 	L     = 0.0;	
    if (x[0]<TrdMinAdc) {
      L = TrdLR_Gr_Prot.at(iP)->Eval(TrdMinAdc) * 1.0/(1.0+TMath::Exp(-10.0*(x[0]-TrdMinAdc)));	
    } else if (x[0]>TrdMaxAdc) {
      L = TrdLR_Gr_Prot.at(iP)->Eval(TrdMaxAdc) * 1.0/(1.0+TMath::Exp(-1.0*(TrdMaxAdc-x[0])));
    } else {
      L = TrdLR_Gr_Prot.at(iP)->Eval(x[0]);
    }
    return L/TrdLR_nProt.at(iP);
  }
  
  //--------------------------------------------------
  ///	x[0] = Eadc, par[0] = iP = Momentum Bin
  double TrdLR_fHelium_v02(double *x, double *par) {
    
    int 	iP    = (int) par[0];	
    double 	L     = 0.0;	
    if (x[0]<TrdMinAdc) {
      L = TrdLR_Gr_Heli.at(iP)->Eval(TrdMinAdc) * 1.0/(1.0+TMath::Exp(-10.0*(x[0]-TrdMinAdc)));	
    } else if (x[0]>TrdMaxAdc) {
      L = TrdLR_Gr_Heli.at(iP)->Eval(TrdMaxAdc) * 1.0/(1.0+TMath::Exp(-1.0*(TrdMaxAdc-x[0])));
    } else {
      L = TrdLR_Gr_Heli.at(iP)->Eval(x[0]);
    }
    return L/TrdLR_nHeli.at(iP);
  }
  
  //--------------------------------------------------
  ///	x[0] = Eadc, par[0] = Layer
  double TrdLR_fElectron_v02(double *x, double *par) {
    
    int 	Layer = (int) par[0];
    double 	L     = 0.0;	
    if (x[0]<TrdMinAdc) {
      L = TrdLR_Gr_Elec.at(Layer)->Eval(TrdMinAdc) * 1.0/(1.0+TMath::Exp(-10.0*(x[0]-TrdMinAdc)));	
    } else if (x[0]>TrdMaxAdc) {
      L = TrdLR_Gr_Elec.at(Layer)->Eval(TrdMaxAdc) * 1.0/(1.0+TMath::Exp(-1.0*(TrdMaxAdc-x[0])));
    } else {
      L = TrdLR_Gr_Elec.at(Layer)->Eval(x[0]);
    }
    return L/TrdLR_nElec.at(Layer);
  }
  //--------------------------------------------------
  ///	x[0] = Eadc, par[0] = iP = Momentum Bin
  double TrdS_PDF_fProton(double *x, double *par) { 
    
    int         iP    = (int) par[0];	
    double 	L     = 0.0;	
    if (x[0]<TrdMinAdc) {
      L = TrdLR_Gr_Prot.at(iP)->Eval(TrdMinAdc) * 1.0/(1.0+TMath::Exp(-10.0*(x[0]-TrdMinAdc)));	
    } else if (x[0]>TrdMaxAdc) {
      L = TrdLR_Gr_Prot.at(iP)->Eval(TrdMaxAdc) * 1.0/(1.0+TMath::Exp(-1.0*(TrdMaxAdc-x[0])));
    } else {
      L = TrdLR_Gr_Prot.at(iP)->Eval(x[0]);
    }
    return max(1E-10,L/TrdPDF_nProt.at(iP));
  }

  //--------------------------------------------------
  ///	x[0] = Eadc, par[0] = iP = Momentum Bin
  double TrdS_PDF_fHelium(double *x, double *par) {
    
    int 	iP    = (int) par[0];	
    double 	L     = 0.0;	
    if (x[0]<TrdMinAdc) {
      L = TrdLR_Gr_Heli.at(iP)->Eval(TrdMinAdc) * 1.0/(1.0+TMath::Exp(-10.0*(x[0]-TrdMinAdc)));	
    } else if (x[0]>TrdMaxAdc) {
      L = TrdLR_Gr_Heli.at(iP)->Eval(TrdMaxAdc) * 1.0/(1.0+TMath::Exp(-1.0*(TrdMaxAdc-x[0])));
    } else {
      L = TrdLR_Gr_Heli.at(iP)->Eval(x[0]);
    }
    return max(1E-10, L/TrdPDF_nHeli.at(iP));
  }
  
  //--------------------------------------------------
  //	x[0] = Eadc, par[0] = iP = Momentum Bin
  //
  double TrdS_PDF_fElectron(double *x, double *par) {
    
    int 		iL 	= (int) par[0];		//	which TRD Layer
    int			iXe	= (int) par[1];		// 	which Xe pressure Bin
    double 	L 		= 0.0;	
    if (x[0]<TrdMinAdc) {
      switch (iXe) {
      case 0: L = grTrdS_PDF_Elec_Xe0.at(iL)->Eval(TrdMinAdc) * 1.0/(1.0+exp(-10.0*(x[0]-TrdMinAdc)));	break;
      case 1: L = grTrdS_PDF_Elec_Xe1.at(iL)->Eval(TrdMinAdc) * 1.0/(1.0+exp(-10.0*(x[0]-TrdMinAdc)));	break;
      case 2: L = grTrdS_PDF_Elec_Xe2.at(iL)->Eval(TrdMinAdc) * 1.0/(1.0+exp(-10.0*(x[0]-TrdMinAdc)));	break;
      case 3: L = grTrdS_PDF_Elec_Xe3.at(iL)->Eval(TrdMinAdc) * 1.0/(1.0+exp(-10.0*(x[0]-TrdMinAdc)));	break;
      case 4: L = grTrdS_PDF_Elec_Xe4.at(iL)->Eval(TrdMinAdc) * 1.0/(1.0+exp(-10.0*(x[0]-TrdMinAdc)));	break;
      case 5: L = grTrdS_PDF_Elec_Xe5.at(iL)->Eval(TrdMinAdc) * 1.0/(1.0+exp(-10.0*(x[0]-TrdMinAdc)));	break;
      }
    } else if (x[0]>TrdMaxAdc) {
      switch (iXe) {
      case 0: L = grTrdS_PDF_Elec_Xe0.at(iL)->Eval(TrdMaxAdc) * 1.0/(1.0+exp(-1.0*(TrdMaxAdc-x[0]))); break;
      case 1: L = grTrdS_PDF_Elec_Xe1.at(iL)->Eval(TrdMaxAdc) * 1.0/(1.0+exp(-1.0*(TrdMaxAdc-x[0]))); break;
      case 2: L = grTrdS_PDF_Elec_Xe2.at(iL)->Eval(TrdMaxAdc) * 1.0/(1.0+exp(-1.0*(TrdMaxAdc-x[0]))); break;
      case 3: L = grTrdS_PDF_Elec_Xe3.at(iL)->Eval(TrdMaxAdc) * 1.0/(1.0+exp(-1.0*(TrdMaxAdc-x[0]))); break;
      case 4: L = grTrdS_PDF_Elec_Xe4.at(iL)->Eval(TrdMaxAdc) * 1.0/(1.0+exp(-1.0*(TrdMaxAdc-x[0]))); break;
      case 5: L = grTrdS_PDF_Elec_Xe5.at(iL)->Eval(TrdMaxAdc) * 1.0/(1.0+exp(-1.0*(TrdMaxAdc-x[0]))); break;
      }
    } else {
      switch (iXe) {
      case 0: L = grTrdS_PDF_Elec_Xe0.at(iL)->Eval(x[0]); break;
      case 1: L = grTrdS_PDF_Elec_Xe1.at(iL)->Eval(x[0]); break;
      case 2: L = grTrdS_PDF_Elec_Xe2.at(iL)->Eval(x[0]); break;
      case 3: L = grTrdS_PDF_Elec_Xe3.at(iL)->Eval(x[0]); break;
      case 4: L = grTrdS_PDF_Elec_Xe4.at(iL)->Eval(x[0]); break;
      case 5: L = grTrdS_PDF_Elec_Xe5.at(iL)->Eval(x[0]); break;
      }
    }
  
    switch (iXe) {
    case 0: return 	max(1E-10,L/TrdPDF_nElec_Xe0.at(iL));
    case 1: return 	max(1E-10,L/TrdPDF_nElec_Xe1.at(iL));
    case 2: return 	max(1E-10,L/TrdPDF_nElec_Xe2.at(iL));
    case 3: return 	max(1E-10,L/TrdPDF_nElec_Xe3.at(iL));
    case 4: return 	max(1E-10,L/TrdPDF_nElec_Xe4.at(iL));
    case 5: return 	max(1E-10,L/TrdPDF_nElec_Xe5.at(iL));
    }
    std::cout << "Error in TrdS_PDF_fElectron: Xe=" << iXe << std::endl;
    return 1E-10;
  }

  double FunRms(double *x, double *par) {
    return sqrt(pow(par[0]/x[0],2) + pow(par[1],2));
  }

  //--------------------------------------------------


 public:
  
  bool Init(int CalibLevel, int Debug);
  
  bool TrdLR_GetParameters(string fName, string hName, int nPar, vector<double> &xMin, vector< vector<double> > &Par);
  
  void TrdLR_CalcIni(int Debug);
  
  void TrdLR_CalcIni_v01(int Debug);
  vector<double> TrdLR_Calc_v01(float Pabs, vector<AC_TrdHits*> ACTrdHits, int iFlag, int Debug);

  vector<double> GenLogBinning(int nBinLog, double Tmin, double Tmax);

  double TrdLR_fProton  (double *x, double *par);
  double TrdLR_fHelium  (double *x, double *par);
  double TrdLR_fElectron(double *x, double *par);

  /// ver.2 released on 2011.10.28
  vector<double> RootGetYbinsTH2(TH2 *hist, int Debug);
  bool TrdLR_CalcIni_v02(int Debug);
  vector<double> TrdLR_Calc(float Pabs, int iFlag, int Debug);

  /// toyMC add on 2011.11.02 for ver.2 & ver.3
  bool TrdLR_MC_CalcIni(int Debug);
  vector<double> TrdLR_MC_Calc(float Pabs, vector<bool> PartId, int iFlag, int Debug);
 
  /// add BetheBloch Correction on 2011.11.27
  double GetProtonBetheBlochCorrFactor(float Pabs);
  double GetHeliumBetheBlochCorrFactor(float Pabs);

  /// ver.4 partial Xe PDFs on 2012.02.01
  int GetXeInterval(double xDay); 
  bool TrdLR_CalcIniPDF(int Debug);
  int TrdLR_CalcXe(double xDay, float Pabs, int iFlag, int Debug);
  bool TrdLR_MC_CalcIniXe(int Debug);
  int TrdLR_MC_CalcXe(double xDay, float Pabs, vector<bool> PartId, int Debug);
 
  /// add new align correction based on module level on 2012.02.07
  int GetUnknownHitPos(TrTrackR *Trtrk, float &alx, float &aly, float gz);
  int GetUnknownHitPos(TrdTrackR *Trdtrk, float &alx, float &aly, float gz);
  int GetUnknownHitPos(TrdHTrackR *TrdHtrk, float &alx, float &aly, float gz);
  int ProcessAlignCorrection(TrdHTrackR *TrdHtrk, AC_TrdHits *ACHit, int Debug);
  int ProcessAlignCorrection(TrdTrackR *Trdtrk, AC_TrdHits *ACHit, int Debug);
  /// add new align correction based on layer level on 2012.02.22
  int ProcessAlignCorrection(unsigned int iTrdAlignMethod, TrdTrackR *Trdtrk, AC_TrdHits *ACHit, int Debug);
  
  int RootTGraph2VectorX(TGraph *gr, vector<double> &vx);
  int RootTGraph2VectorY(TGraph *gr, vector<double> &vy);
  vector<double> RootGetXbinsTH1(TH1 *hist);

  bool TrdScalibMinDist(double xDayRef, int xP, int iMod);
  bool TrdScalibMinDist(double xDayRef, int xP, vector<double> &TrdScalibXdays);

  int TrdScalibBinarySearch(double key, int iMod, int Debug); 
  int TrdScalibBinarySearch(double key, vector<double> &TrdScalibXdays, int Debug);

  double TrdScalibInterpolate(int iMod, double xDayRef, int &xP, int Debug);
  double TrdScalibInterpolate(double xDayRef, int &xP, vector<double> &TrdScalibXdays, vector<double> &TrdScalibVal, int Debug);

  
  int InitTrdSCalib(int CalVer, int TrdTrackType, int Debug);
  int InitNewTrdSCalib(int CalVer, int TrdTrackType, int GainMethod, int AlignMethod, int Debug);
  int ProcessTrdEvt(AMSEventR *pev, int Debug=0);
  int ProcessTrdHit(TrdHTrackR *TrdHtrk, TrTrackR *Trtrk, int Debug);
  int ProcessTrdHit(TrdTrackR  *Trdtrk,  TrTrackR *Trtrk, int Debug);
  int ProcessTrdHit(TrTrackR *Trtrk, int Debug);

  int BuildTrdSCalib(time_t evut, double fMom, TrdHTrackR *TrdHtrk, TrTrackR *Trtrk, double &s1,double &s2, double &s3 , int Debug);

  int GetnTrdHitLayer( vector<AC_TrdHits*> &TrdHits, int Debug);
  
  vector<int> TrdFillHits( vector<AC_TrdHits*> &TrdHits, int Debug);
  vector<int> TrdFillHits2( vector<AC_TrdHits*> &TrdHits, TSpline3 *grTrkXZ, TSpline3 *grTrkYZ, int Debug);
  vector<int> CalPathLen3D(vector<AC_TrdHits*> &TrdHits, TrTrackR *Trtrk, int TrdStrkLevel, int Debug);

  int GetTrdHitsInAcceptance(vector<AC_TrdHits*> &TrdHits, TSpline3 *grTrkXZ, TSpline3 *grTrkYZ, vector<int> &Straws, int &nTrdLay, int Debug);
  int IterateTrk4MS(float aRig, vector<AC_TrdHits*> &TrdHits, TSpline3 *grTrkXZ, TSpline3 *grTrkYZ, vector<float> &Results, int Debug);
  int TrdTrkChi2(vector<AC_TrdHits*> TrdHits, TSpline3 *grTrkXZ, TSpline3 *grTrkYZ, double DeltaX, double DeltaY, double &Chi2, int &nTrdHits, int Debug);

  bool NeedTrkSpline(TrTrackR *Trtrk, int Debug);
  int GetTrkSpline(TSpline3* &grTrkXZ, TSpline3* &grTrkYZ, int msFlag, int Debug);
  int GetLocalTrkVec(float zTrdCor, TSpline3 *grTrkXZ, TSpline3 *grTrkYZ, AMSPoint &cTrk, AMSDir &bTrk, double DeltaX, double DeltaY, int Debug);
  
  int GetTrdNewHits(TSpline3 *grTrkXZ, TSpline3 *grTrkYZ, vector<AC_TrdHits*> TrdHits, int Debug);
  int GetTrdNewHits(vector<AC_TrdHits*> TrdHits, int Debug);

  int GetThisTrdHit(AC_TrdHits* &TrdHit, int Debug);
  int GetTrkCoordinates(TrTrackR *Trtrk, int Debug);

  bool TrdHitSortFunction_TrkD(AC_TrdHits* Hit1, AC_TrdHits* Hit2) {return (fabs(Hit1->TrkD)<fabs(Hit2->TrkD));}
  bool TrdHitSortFunction_Lay(AC_TrdHits* Hit1, AC_TrdHits* Hit2) {return (fabs(Hit1->Lay)<fabs(Hit2->Lay));}

  int InitiateTrdRawHit(AMSEventR *pev, vector<AC_TrdHits> &TrdHits, int Debug);
  int InitiateTrdRawHit(AMSEventR *pev, vector<AC_TrdHits*> &TrdHits, int Debug);
  int InitiateTrdRawHit(AMSEventR *pev, int Debug);

  
  /// Trd Align Methods
  TrdKCalib _DB_instance;  //== TrdAlignMethod = 2 from Z.Weng  
  //TRDZCalib thetrdz;   //== TrdAlignMethod = 3 from V.Zhukov

  vector<double> TrkXcors, TrkYcors; 

  /// Trd New Geometry
  float  Tht_L1, Phi_L1, X_L1, Y_L1;          //== Layer-1N  +159.067 cm
  float  X_TU, Y_TU, X_TC, Y_TC, X_TL, Y_TL;  //== TRD-Upper +136.75 cm, TRD-Center +113.55 cm, TRD-Lower +90.35 cm 
  float  Tht_UT, Phi_UT, X_UT, Y_UT;          //== Upper-TOF  +63.65 cm (between Plane 1/2)
  float  Tht_RI, Phi_RI, X_RI, Y_RI;          //== RICH  -73.6 cm (Center of Radiator)
  float  Tht_L9, Phi_L9, X_L9, Y_L9;          //== Layer-9   -136.041 cm

  //TSpline3 *grTrkXZ, *grTrkYZ;
  TGraph      *TG[2];  //!  for z-interpolation
  TSpline3    *TS[2];  //!  for z-interpolation

  float TRD_SHIFT[3]; //== TrdGeom global shift
  float TRD_ROT[3];   //== TrdGeom global rotation: 
  //Int_t GetGeo[2][6][7][4][3]; // crate,nudr,nufe,nute -> layer,ladder,direction(0,1)
  
  /// module displacement from shimming:     
  float Mod_Dz[328];         // Z-Offset/mu-m in Octagon from Shimming     
  float Mod_Arz[328];        // Tilt/mu-rad   in Octagom from Shimming

  bool  AC_InitTrdGeom(int Debug);
  int   AC_InitTrdMove( char* fname, int Debug );
  int   AC_InitTrdShim( char* fname, int Debug ); 
  void  AC_InitInterpolate2Z(float dZ_UT, float dZ_L1, float dY_UT, float dY_L1);
  void  AC_ClearInterpolate2Z();
  float AC_Interpolate2Z(float Z, int D);
  int   AC_ModAlign(AC_TrdHits* &ACHit, int Debug);



  static TrdSCalibR* gethead(int i=0);
  
  static void  KillPointer(){
    int i=0;
#ifdef _OPENMP
    i=omp_get_thread_num();
#endif
    delete head[i];
    head[i]=0; 
  }
 

  ClassDef(TrdSCalibR,5)
    };

#endif

