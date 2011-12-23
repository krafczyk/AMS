#ifndef _TrdSCalib_
#define _TrdSCalib_

#include <map>
#include <vector>
#include <algorithm>

#include "TObject.h"
#include "point.h"
#include "TrdRawHit.h"
#include "TrdHRecon.h"
#include "TrdHTrack.h"
#include "TrTrack.h"
#include "TrTrackSelection.h"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TString.h"
#include "TROOT.h"
#include "TMath.h"
#include "TVector3.h"
#include "TGraph.h"
#include "TGraphSmooth.h"
#include "TObjArray.h"


namespace trdconst{

  const integer nTrdGasCir       = 10;
  const integer nTrdGasGroups    = 40;
  const integer	nTrdLadders      = 18;
  const integer nTrdLayers       = 20;
  const integer nTrdModules      = 328;
  const integer nTrdStraws       = 5248;
  const integer nTrdTubes        = 16;

  const number  TrdMeanMPV       = 70.0;
  const number  TrdMeanMPVSC     = 56.0;
  const number  TrdEadcMin       =  7.0;
  const number  TrdEadcMax       = 4048.0;
  

  const integer	TrdLR_Prot_nPar	 = 12;
  const integer	TrdLR_Heli_nPar	 = 10;
  const integer	TrdLR_Elec_nPar	 = 12;

  const integer nParLR           = 20;         // iP range[3-100 GeV/c] || 20 layers
  
  const integer	TrdMinAdc	 = 15;
  const integer	TrdMaxAdc	 = 3500;

  const integer mTrdHits	 = 30;
  const integer	mTofHits  	 = 10;
  const integer	mAccHits 	 = 10;

  const integer FirstDayAMSonISS = 138;	         // day in the year 2011

  const number  TrdStrawRadius 	 = 0.3;		 // in cm
  const number  TrdOffsetDx 	 = 0.047;	 // cm
  const number  TrdOffsetDy 	 = 0.039;	 // cm

  const number  ToFLayer1Z       = 65.2;          // cm

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

 
}
using namespace trdconst;

class AMSEventR;
class TrdRawHitR;
class TrdHSegmentR;
class TrdHTrackR;
class TrdTrackR;
class TrTrackR;
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
class AC_TrdHits: public TObject{
  
 protected:
  int _ilayer, _iladder, _itube, _ihaddr, _igasgrp, _igascir, _imodule;
  float _iamp;

 public:
  int Lad, Lay, Tub, Had; 
  int Mod, Straw, Man;
  float	EadcR, EadcCM, EadcC1, EadcCS;
  float TrdD, TrkD, Len2D, Len3D;

 AC_TrdHits(): Lad(-1), Lay(-1), Tub(-1), Mod(-1), Straw(-1), Man(-1), 
    EadcR(0), EadcCM(0), EadcC1(0), EadcCS(0),
    TrdD(0), TrkD(0), Len2D(0), Len3D(0){
   
    _ilayer=_iladder=_itube=_ihaddr=_igasgrp=_igascir=_imodule = 0;
    _iamp=0;
    
  };
  
  ~AC_TrdHits() {};

  void CheckTrdRawHit (void)
  {std::cout << "lay=%d "<< _ilayer <<" lad= "<< _iladder <<" tub= "<< _itube <<" amp= "<< _iamp <<std::endl;};
   
  
  int   GetLayer()  {return _ilayer;};
  int   GetLadder() {return _iladder;};
  int   GetTube()   {return _itube;};
  float GetAmp()    {return _iamp;};
  float GetHaddr()  {return _ihaddr;};

  void GetHitInfo(TrdRawHitR *rhit) {
    _iladder=rhit->Ladder; _ilayer=rhit->Layer; _itube=rhit->Tube; _iamp=rhit->Amp; _ihaddr=rhit->Haddr;
  };
  
  void SetHitPos() {
    Lay= _ilayer; Lad= _iladder; Tub= _itube; EadcR= _iamp; Had= _ihaddr;
  };
  
  int GetD(){
    if( (_ilayer>=16) || (_ilayer<=3) ) return 1; 
    else                                return 0; 
  };

  float GetXY(){
    static float r = 0;
    int   lad = _iladder;
    
    if(_ilayer<12) lad++;
    if(_ilayer< 4) lad++;
    
    r = 10.1*(float)(lad-9);
    
    if( GetD()==1 ){        /// y-measurement
      if( lad>=12 ) r+=0.78; /// bulkhead
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

  float GetZ(){
    static float z = 0.;
    int   lad = _iladder;
    
    if(_ilayer<12) lad++;
    if(_ilayer< 4) lad++;
    
    z = 85.275 + 2.9*(float)_ilayer;
    if(lad%2==0) z += 1.45;

    return z;
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

  double GetTrdPathLen3D(int lay, float xy, float z, AMSPoint cP, AMSDir dP);
  

  friend class TrdSCalib;

  ClassDef(AC_TrdHits,1)
};



 
//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------

class TrdSCalibR: public TObject{
  
 private:
  int algo,patt,refit, pmass, pcharge;
  static TrdSCalibR* head;

  
 private:
  /// filename for GetTrdHits
  static const char* TrdDBUpdateDir[];
  static const char* TrdCalDBUpdate[];
  /// filename for TrdLR_CalcIni
  static const char* TrdElectronProtonHeliumLFname[];
  /// ordering name of TrdLL Calculator
  static const char* TrdLLname[];
  /// ordering name of TrdSum8 
  static const char* TrdSum8name[];
  /// ordering name of EvtPartID
  static const char* EvtPartID[];
 

  /// extended TrdRawHit class
 public:

  static vector<int> nTrdModulesPerLayer;
  static vector< vector<int> > mStraw; 	
  
  vector<TH1F *> h_TrdGasCirMPV;
  vector<TH1F *> h_TrdModuleMPV;
  
  /// use for ver.02
  vector<TH1D*> h_TrdLR_Prot, h_TrdLR_Elec, h_TrdLR_Heli;

  /// use for Toy MC
  vector<TF1*> fTrdLR_fElectron, fTrdLR_fProton, fTrdLR_fHelium;

  unsigned int FirstCalRunTime, LastCalRunTime;

  static TObjArray *ObjTrdGasCirMPV_list;
  static TObjArray *ObjTrdModuleMPV_list;

  int iBinGasCir, iBinModule;

  AMSPoint cTrd, cTrk, c0Trd;
  AMSDir   dTrd, dTrk;
  
 public:  
  int  dummy;
  bool FirstCall, TrdCalib_01, TrdCalib_02;
  bool FirstLLCall, FirstMCCall;
  
  vector<double>	   TrdLR_xProt, TrdLR_xHeli, TrdLR_xElec;
  vector<double>	   TrdLR_nProt, TrdLR_nHeli, TrdLR_nElec;
  vector< vector<double> > TrdLR_pProt, TrdLR_pHeli, TrdLR_pElec;

  vector<TGraph *> TrdLR_Gr_Prot, TrdLR_Gr_Elec, TrdLR_Gr_Heli;


  TF1 *fTrdSigmaDx, *fTrdSigmaDy, *fTrd95Da;
  TF1 *fTrdLR;
  
 TrdSCalibR(): algo(1), patt(3), refit(2),
  dummy(0) {
    FirstCall   = true;
    FirstLLCall = true;
    FirstMCCall = true;
    h_TrdGasCirMPV.clear();
    h_TrdModuleMPV.clear();

    FirstCalRunTime = 2000000000;
    LastCalRunTime  = 0;

    TrdLR_xProt.clear();
    TrdLR_xHeli.clear();
    TrdLR_xElec.clear();

    TrdLR_nProt.clear();
    TrdLR_nHeli.clear();
    TrdLR_nElec.clear();

    TrdLR_pProt.clear();
    TrdLR_pHeli.clear();
    TrdLR_pElec.clear();

    TrdLR_Gr_Prot.clear();
    TrdLR_Gr_Elec.clear();
    TrdLR_Gr_Heli.clear();

    for(int i=0;i<3;i++) cTrd[i]=dTrd[i]=cTrk[i]=dTrk[i]=0.0;

    fTrdSigmaDy = new TF1("fTrdSigmaDy",this, &TrdSCalibR::FunTrdSigmaDy,0.0,1000.0,3);
    fTrdSigmaDy->SetParameters(1.686,0.1505,0.2347); 
    fTrdSigmaDx = new TF1("fTrdSigmaDx",this, &TrdSCalibR::FunTrdSigmaDy,0.0,1000.0,3);
    fTrdSigmaDx->SetParameters(2.484,0.1183,0.3487);
    fTrd95Da = new TF1("fTrd95Da",this, &TrdSCalibR::FunTrdSigmaDy,0.0,1000.0,3);  
    fTrd95Da->SetParameters(0.7729,0.7324,0.2005);

    fTrdLR      = 0;
    //fTrdLR = new TF1("fTrdLR",this,&TrdSCalibR::Fun2Landau3Expo, 0.0,4000.0, 0);
    ///avoids being deleted next time a new one with the same name is constructed 
    //gROOT->GetListOfFunctions()->Remove(fTrdLR);

  }
  

  virtual ~TrdSCalibR(){
    if ( h_TrdGasCirMPV.size() != 0) 
      for(int i=0; i< trdconst::nTrdGasCir;i++)  delete h_TrdGasCirMPV[i];
    if ( h_TrdModuleMPV.size() != 0) 
      for(int i=0; i< trdconst::nTrdModules;i++) delete h_TrdModuleMPV[i];

    if( h_TrdLR_Prot.size() !=0) 
      for(int i=0; i< trdconst::nParLR;i++) delete h_TrdLR_Prot[i];

    if( h_TrdLR_Elec.size() !=0) 
      for(int i=0; i< trdconst::nParLR;i++) delete h_TrdLR_Elec[i];

    if( h_TrdLR_Heli.size() !=0) 
      for(int i=0; i< trdconst::nParLR;i++) delete h_TrdLR_Heli[i];

    if( fTrdLR_fProton.size() !=0) 
      for(int i=0; i< trdconst::nParLR;i++) delete fTrdLR_fProton[i];

    if( fTrdLR_fElectron.size() !=0) 
      for(int i=0; i< trdconst::nParLR;i++) delete fTrdLR_fElectron[i];

    if( fTrdLR_fHelium.size() !=0) 
      for(int i=0; i< trdconst::nParLR;i++) delete fTrdLR_fHelium[i];

   
  }
 
  
  int Isitsame (TrdSCalibR& ObjRawHit) {
    if (&ObjRawHit == this) return true;
    else return false;
  };


 public:

  void GenmStrawMatrix(int Debug);
  void ReadLFname();

  ///(Z= upper TOF)
  bool GetcTrd(TrdHTrackR *trdht); 
  bool GetdTrd(TrdHTrackR *trdht);
  bool GetcTrd(TrdTrackR  *trdt); 
  bool GetdTrd(TrdTrackR  *trdt);
  bool GetcTrkdTrk(TrTrackR *trt);

  bool MatchingTrdTKtrack(float Pabs, int Debug); 

 public:
  int GetEvthTime(AMSEventR *ev, int Debug);
  int GetEvthTime(time_t EvtTime, int Debug);
  int GetModCalibTimePeriod(TString fname, int Debug);

  bool Get01TrdCalibration(TString fname, int Debug);
  
  bool Get02TrdCalibration(TString fname, int Debug);

  bool GetTrdCalibHistos(int CalibLevel, int Debug);

  void GetBinGasCirModule(int hTime, int CalibLevel, int iCir, int iMod, int Debug);

  vector<float> GetTrdSum8(vector<AC_TrdHits*> TrdHits, int Debug);


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
//--------------------------------------------------
///	x[0] = Eadc, par[0] = iP = Momentum Bin
double TrdLR_fProton_v02(double *x, double *par) {
	
  int 		iP    = par[0];	
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
	
  int 		iP    = par[0];	
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
	
  int 		Layer = par[0];
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
  

 public:

  bool Init(int CalibLevel, int Debug);

  bool TrdLR_GetParameters(string fName, string hName, int nPar, vector<double> &xMin, vector< vector<double> > &Par);
  
  void TrdLR_CalcIni(int Debug) {TrdLR_CalcIni_v02(Debug);}


  void TrdLR_CalcIni_v01(int Debug);
  vector<double> TrdLR_Calc_v01(float Pabs, vector<AC_TrdHits*> ACTrdHits, int iFlag, int Debug);

  vector<double> GenLogBinning(int nBinLog, double Tmin, double Tmax);

  double TrdLR_fProton  (double *x, double *par);
  double TrdLR_fHelium  (double *x, double *par);
  double TrdLR_fElectron(double *x, double *par);

  /// ver.2 released on 2011.10.28
  vector<double> RootGetYbinsTH2(TH2 *hist, int Debug);
  bool TrdLR_CalcIni_v02(int Debug);
  vector<double> TrdLR_Calc(float Pabs, vector<AC_TrdHits*> TrdHits, int iFlag, int Debug);

  /// toyMC add on 2011.11.02
  bool TrdLR_MC_CalcIni(int Debug);
  vector<double> TrdLR_MC_Calc(float Pabs,  vector<AC_TrdHits*> TrdHits, vector<bool> PartId, int iFlag, int Debug);
    
  static TrdSCalibR* gethead(){
    if(!head) head=new TrdSCalibR();
    return head;
  }
  
  static void  KillPointer(){
    delete head;
    head=0; 
  }
 

  ClassDef(TrdSCalibR,2)
};

#endif

