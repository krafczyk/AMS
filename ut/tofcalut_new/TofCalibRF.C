#define TofCalibPG_cxx
#define _PGTRACK_
#include "/Offline/vdev/include/root_RVSP.h"
//#include "/f2users/lquadran/AMS/include/root_RVSP.h"
#include "TF1.h"
#include "TH2.h"
#include "TRandom.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TMath.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TText.h"
#include "TEventList.h"
#include "TSelector.h"
#include "TChain.h"
#include "amschain.h"
#include <TROOT.h>
#include <TTree.h>
#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <TMinuit.h>
//#include <freetype.h>
//#include <root_setup.h>
//#include <SlowControlDB.h>
//---------------------------------------------------------------------------------------------------------------------
//                                                                          
//                                                                    
//                                 
//                           TofTdcCor      
const UInt_t kAnalyse[10]={      0,        0,        0,        0,       0,       0,        0,        0,       1,       0};
//[6]:TofAnalyz=1/2/3->TofResol/TofLev1Trig
//[3]:TrkAnaliz=1//->TrackAddrStudy//
//[9]:TofTimingProbl=1/2->Beta>0/Beta<0
//
//----> TofCalib-type selection:
//
const UInt_t kTofCalibType[3]={1,1,1};//Tdlv/Tzslw/Ampl
const UInt_t kTofCalibPrint[3]={2,2,2};//print level for Tdlv/Tzslw/Ampl-calib(0/1/2->no/norm/debug)
const Int_t kSpaceCalib=1;//0/1-> Earth/Space calib
const Bool_t kUseHe4Calib(kFALSE);
const Float_t kEvsPortionToUse=0.6;//(0.6,1) use this % of 3.5mln inp.evs to cover complete orbit
const Int_t kRDCalib=1;//1/0->rd/mc-calib
const UInt_t kRefFileN=1305815610;//tempor for RD
const UInt_t kMCRefN=2;//tempor for MC
// 
/*
const Float_t kTdlvMomLims[2][2]={0.2,50., //earth
                                   1.,50.};//space
const Float_t kTzslMomLims[2][2]={1.,50., //earth
                                  10.,100.}; //space
*/
const Float_t kTdlvMomLims[2][2]={0.2,50., //earth
                                   10.,100.};//space
const Float_t kTzslMomLims[2][2]={1.,50., //earth
                                   10.,100.}; //space

const Float_t kAmplMomLims[2][2]={0.1,50., //earth
                                  2.,50.}; //space(Beta>0.9)
//
//---------------------------------------------------------------------------------------------------------------------
//
const Int_t kNlogb=40;//max log10(rigid) bins
const Float_t kRiglmn=-0.3;//Log(Rig)min->0.5gv
const Float_t kRiglmx=3.7;//Log(Rig)max->5012gv
const Float_t kRiglbn=(kRiglmx-kRiglmn)/kNlogb;//bin width(0.1)
//--->EC:
const Int_t kNecsl=9;//EC Super-Layers
const Int_t kNecslx=4;
const Int_t kNecsly=5;
const Int_t kNecpm=36;//EC PMs per SL
const Float_t kECTrSize=65.8;//EC transv. size
const Float_t kEShowNorm=1.;//def.value
//const Float_t kEShowNorm=0.9664;//e_cal,offson,-0.3/1.7 cut on Ximp
//const Float_t kEShowNorm=0.951;//...,no cut on Ximp
//const Float_t kEShowNorm=0.9788;//e_cal,offsoff,no cut on Ximp
//const Float_t kEShowNorm=0.9563;//e_cal,offson,cal_from_offsoff(eccept h2l),no cut on Ximp
//const Float_t kEShowNorm=0.9857;//muc,h2lr from elcal +subtr.offset, no cut
//const Float_t kEShowNorm=0.9846;//muc1,h2lr from elcal +Nosubtr.offset, no cut
const Int_t kBEnergyIndx=4;//Beam energy index to select(0->6) 
const Float_t kBeamEnergies[7]={20,60,80,100,120,180,250};//nominal
const Float_t kBeamEnergiesT[7]={20,60,80,100,120,178.8,245.5};//true
const Float_t kCvel(29.979);// light velocity
//
//-----> for ams02p:
//
const Float_t kMCEnergies[5]={10,50,100,250,500};
const Float_t kSearchWind[5]={0.261,0.134,0.117,0.126,0.131};
const Int_t kMCEnIndx=4;
//-----> ACC:
const Int_t kNants=8;//ANTI sectors
const Float_t kAccPaddl=82.65;
const Float_t kAccThickn=0.8;
//-----> TRK:
const Int_t kNBadAdds=0;//<===== if>0, force TRK-tracks removal if address in BadAddrsList(first kNBadAdds items) 
const Int_t kNtrkl=9;//TRK layers
const Int_t kNladl[kNtrkl]={15,15,14,12,10,10,12,14,10};//TRK ladders/layer
const Int_t kNtradd=10000;//max TRK-track addresses list length
const Int_t kNtremx=200;//max stored TRK-track params/address(for histogramming)
const Int_t kTrkFitMInUse=9;//max.existing methodes
const Int_t kTrkFitMet=0;//currently used methode 0/1/2/...->VC/Geane/GeaneK/JA/Chik/ChikF/Linear/Circle/Simple)
//-----> TRD:
const Float_t kTrdBotZ=83.5;
//const Float_t kTrdBotZ=85.3;
//-----> TOF(general): 
const Int_t kNtofl=4;//TOF layers
const Int_t kNtofb[4]={8,8,10,8};//TOF bars per layer
const Int_t kTofBTypes=11;//tof bar types
const Int_t kTofNBars=34;//tot.Tof-bars
const Int_t kTofLBars=10;//max bars per layer
const int kTofFirstSidePos[4]={0,16,32,52};//position of first side of each layer 
const Int_t kTofPmtMX=3;//mas PMTs/SIDE
const Int_t kTofAdcMX=3700;//max. linearity range for TOF ADC
const Int_t kTofChanMx=kNtofl*kTofLBars*2;//max Tof channels 
const Float_t kTofExtBWid[4]={22.5,25.5,18.5,26.};//external bars width vs layer
const Float_t kTofIntBWid=12;//internal bars width
const Int_t kTofRefL=4;//tof ref.layer for monitoring
//--TofTAU-calib:
const Int_t kTdLBins=17; //max. number of Long-coord. bins for TDIF-calibration
const Int_t kTofAdc2Q=1;//[pC/Adch] Adc->Q conv.factor
const Int_t kTofBref=104;//LBB for  ref. layer/counter(LBB, set L=2 if layer 1 is dead) 
const Float_t kTofTzref=0;//T0 for ref.counter (0)
const Int_t kTofLref=1;//ref1ayer(1)/2, =2 means 1st layer in not functional, 2nd used as ref
Float_t TofTzSlope[kTofNBars*2];
const Bool_t kTofStrictLPatt=kFALSE;// (0)/1-> Use Weak(>=3of4)/Strict(4of4) LayerPattern
const Int_t kTofLcRelax=0;//relaxe long.cut for tof-trk matching
const Float_t kTofDefTzslop=6.5;//def.Tzsl-slope param.value(Pr)
const Float_t kTofDefTzslopHe=9.5;//9.5 def.Tzsl-slope param.value
const Int_t kTofTzSlopRel=1;//0/1 to fix/release slope param.
const Int_t kTofTzFitm=0;//fit-mode: 0/1/2->FitAll/IgnorTrapezCount/FitTrapezCount&FixOthers
const Int_t kAmProfBins=17;//max Longit-profile bins
const Int_t kAmEvStore=2000;//max events to store per chann(or bin)
const Int_t kAmBrtBins=kTofBTypes*kAmProfBins;//max bartypes*maxbins
const Int_t kAmProfPars=6;//max. parameters/side in A-profile(Apm<->Yloc) fit
const Int_t kLandfPars=4;//Landau function fit-parameters
//
const Int_t kNTofID=10;//number of TofIDs for Edep long.scan
const Int_t kTofID[kNTofID]={101,108,201,208,301,310,401,408,104,105};
const Int_t kNTofIDr=10;//number of TofIDs for raw-adc long.scan
const Int_t kTofIDr[kNTofIDr]={1011,1012,2011,2012,3011,3012,4011,4012,1041,1042};
const Int_t kPDFBnmx=100;//Max.bins in ElosPDF distribution
const Int_t kZindmx=14;//max Z-values for ElosPDF's
const Int_t kZpdfs[kZindmx]={1,1,2,3,4,5,6,7,8,9,10,11,12,13};
//                  Z-index:     1(e)  2(p)  3(He) 4(Li) 5(Be) 6(B)  7(C)   8(N) 9(O) 10   11   12    13   14
const Float_t kBpowTof[kZindmx]={1.60, 1.59, 1.30, 1.06, 0.93, 0.77, 0.667, 0.66,0.51,1.33,1.30,1.26,1.26,1.24};
const Float_t kBpowTrk[kZindmx]={1.92, 1.90, 1.94, 1.90, 1.80, 1.70, 1.65,  1.70,1.50,1.70,1.70,1.70,1.70,1.70};
const Float_t kBeta0=0.95;//TOF beta0 in beta-correction
const Float_t kBeta0tk=0.92;//TRK beta0 in beta-correction
const Int_t kZrefInd=2;//<===== index(1,2,...) of Z to select for TOF PDF's
const Float_t kTofTbw=0.0244141;//tof-tdc bin-width(ns)
//
const Double_t raddeg=57.295779;
const Double_t pi=3.14159265358979;
const Int_t kEvSelCDim=180;//ev.sel counters array dimension
const Float_t kProtMass=0.93827;
const Float_t kHeliumMass=3.726;
const Float_t kMuMass=0.1057;
const Bool_t kUseNewSlew(kTRUE); // use independent slewing corrections for calib.
//const UInt_t kTofSlwRunID=1305806831; // Slewing parameters RUN ID
//
//--------------------
//
class TofCalib{//<--- TAU-Calibrations
//
private:
//
  static Int_t _brtype[kTofNBars];
  static Float_t _brlen[kTofBTypes];
//
//Tdlv-part:
  static Double_t _tdiff[kTofNBars][kTdLBins];// side-times differences (ns)
  static Double_t _tdif2[kTofNBars][kTdLBins];// square of ...
  static Double_t _clong[kTofNBars][kTdLBins];// impact longit.coordinates(cm)
  static Int_t _nevnt[kTofNBars][kTdLBins];// event counters
  static Int_t _nbins[kTofBTypes];//numb.of coord-bins vs bar-type(<=TOF2GC::SCTDBM)
  static Int_t _brtyp[kTofNBars];
//Tzslw-part:
  static Float_t slope;
  static Float_t tzero[4][kTofLBars];
  static Double_t s0;
  static Double_t s1;
  static Double_t s3[4][kTofLBars];
  static Double_t s30[4][kTofLBars];
  static Double_t s4;
  static Double_t s6[3][kTofLBars];
  static Double_t s7[kTofLBars];
  static Double_t s8;
  static Double_t s12[kTofLBars][kTofLBars];
  static Double_t s13[kTofLBars][kTofLBars];
  static Double_t s14[kTofLBars][kTofLBars];
  static Double_t s15[3][kTofLBars];
  static Double_t s16[kTofLBars];
  static Double_t events;
  static Double_t resol;
  static Double_t resol1;
//Ampl-part:
  static Double_t ambin1[kAmBrtBins][kAmEvStore];// s1-signals for each ref_bar/bin/event
  static Int_t nevenb1[kAmBrtBins];// s1 events accum. per ref_bar/bin for ambin
  static Double_t ambin2[kAmBrtBins][kAmEvStore];// s2-signals for each ref_bar/bin/event
  static Int_t nevenb2[kAmBrtBins];// s2 events accum. per ref_bar/bin for ambin
  static Double_t amchan[kTofChanMx][kAmEvStore];//centr.inc side-signals for each channel/event
  static Int_t nevenc[kTofChanMx];// numb.of events accum. per channel for amchan
  static Float_t gains[kTofChanMx];//trunc. mean ch.signals ("0" impact) relat. to ref.ones
  static Float_t melfunchi2;
  static Float_t mfunamchi2;
  static Float_t btamp[2][kAmBrtBins];// MostProb bar-signals for each side/bin(ref.bars) 
  static Float_t ebtamp[2][kAmBrtBins];// Error in MostProb  for each side/bin(ref.bars) 
  static Int_t rbls[kTofBTypes];// list of ref. bars (diff. by bar type)(LBB)
  static Float_t profb[kTofBTypes][kAmProfBins];// profile bins width(counting from "-" side)
  static Float_t profp[kTofBTypes][kAmProfBins];// bin middle-point values
  static Int_t nprbn[kTofBTypes];// real number of prof.bins according to bar-type
  
  static Double_t a2dr[kTofChanMx];//sum of An/Sum(Dyn(pmt))-ratios for each channel
  static Double_t a2dr2[kTofChanMx];//sum of ratios**2 for each channel
  static integer neva2d[kTofChanMx];//number of events/channel for above sum
  static Double_t d2sdr[kTofChanMx][kTofPmtMX];//sum of Dyn(pm)/Sum(Dyn(pm))-gains for LBBS
  static Double_t d2sdr2[kTofChanMx][kTofPmtMX];//sum of ratios**2 for each LBBS
  static Int_t nevdgn[kTofChanMx];//number of events/channel for above sum
  
  static Double_t ammrfc[kTofBTypes];// mean charge (central incidence) for ref.counters
  static Int_t nevrfc[kTofBTypes];// number of accum. events for above sum
  static Double_t arefb[kTofBTypes][kAmEvStore];// tot-signals for each ref_bar/event
  static Int_t nrefb[kTofBTypes];// numb.of events accum. per ref_bar
  static Float_t fpar[10];// parameters to fit
  static Int_t nbinr;// working var. for A-profile fit
  static Int_t iside;// working var. for A-profile fit
  static Float_t fun2nb;// working var. for  fit
  static Float_t mcharge[kAmProfBins]; 
  static Float_t mcoord[kAmProfBins];
  static Float_t emchar[kAmProfBins];
  static Double_t aprofp[kTofBTypes][2*kAmProfPars];// A-profile parameters(ref.bars)
  static Float_t clent;//bar-length (to exchange with FCN)
  static Int_t cbtyp;//bar-type  (....................)
  static Int_t fitflg;//flag     (....................)
//
  static Float_t binsta[50];// bin-statistics arrays for dE/dX fit
  static Float_t bincoo[50];// bin-coordinate ...
  static Int_t elbt;//     tot.bins
  static Float_t elfitp[kLandfPars];// fit-parameters
  static Char_t eltit[60];  // title for fit

public:
  static Int_t calstat[4][kTofLBars][2];
  static Float_t brlen(Int_t ilay, Int_t ibar);
  static Int_t brtype(Int_t ilay, Int_t ibar);
  static Int_t barseqn(Int_t ilay, Int_t ibar);
  static Int_t npmtps(Int_t ilay, Int_t ibar);
  static Int_t cinit();
  static Int_t cwrite();
//Tdlv-functions:
  static void inittd();
  static void filltd(Int_t il, Int_t ib, Float_t td, Float_t coo);
  static Int_t fittd();//Tdelv-fits, results->efflightvel,to->file
//Tzslw-functions:
  static void inittz();
  static void filltz(Int_t ib[4],Double_t dtr[3],Double_t du[3]);
  static void mfuntz(Int_t &np, Double_t grad[],Double_t &f,Double_t x[],Int_t flg);
  static Int_t fittz();//Tzslw-fits, results->tzeros,slope->file
  static void TzslwHist();
  static geant getslop(){return slope;};
  static void gettzer(geant arr[]){
    int cnum;
    for(int il=0;il<4;il++){
    for(int ib=0;ib<kTofLBars;ib++){
      cnum=il*kTofLBars+ib;
      arr[cnum]=tzero[il][ib];
    }
    }
  };
//Ampl-functions:
  static void initam();
  static void filla2dg(Int_t il, Int_t ib, Float_t cin, Float_t ain[2], Float_t din[2][kTofPmtMX]);
  static void fillam(Int_t il, Int_t ib, Float_t am[2], Float_t coo);
  static void fillabs(Int_t il, Int_t ib, Float_t am[2], Float_t coo);
  static void melfun(Int_t &np, Double_t grad[], Double_t &f, Double_t x[], Int_t flg);
  static void mfunam(Int_t &np, Double_t grad[], Double_t &f, Double_t x[], Int_t flg);
  static Int_t fitam();  
  static void AmplHist();
//service:
  static UInt_t CFlistC[11];
  static Int_t FirstLastEvent[2]; 
  static Int_t FirstLastRun[2];
  static Char_t FirstEvDate[30]; 
  static Char_t LastEvDate[30];
// South Atlantic Anomaly
  static Bool_t saa(Float_t phi,Float_t theta);
};//--->endof TofCalib-class def
//-----------
//---> Tof-arrays inits, functions:
  Int_t TofCalib::_brtype[kTofNBars]={//bar-types map
              1, 2, 2, 2, 2, 2, 2, 1, 
              5, 6, 7, 7, 7, 7, 6, 5,
              8, 9,10,11,11,11,11,10, 9, 8,
              3, 4, 4, 4, 4, 4, 4, 3 
                                    };
//
  Float_t TofCalib::_brlen[kTofBTypes]={126.5, 130.5, 130., 134., 117.2, 127., 132.2, 110., 117.2, 127., 132.2};//bar-length vs bar-type
//- Tdelv-arrays
  Int_t TofCalib::_nbins[kTofBTypes]={15,16,16,16,15,16,17,14,15,16,17};//#coord-bins vs bar-type to have bin width = 8-9cm
  Double_t TofCalib::_tdiff[kTofNBars][kTdLBins];// side-times differences (ns)
  Double_t TofCalib::_tdif2[kTofNBars][kTdLBins];// square of ...
  Double_t TofCalib::_clong[kTofNBars][kTdLBins];// impact longit.coordinates(cm)
  Int_t TofCalib::_nevnt[kTofNBars][kTdLBins];// event counters
//Tzslw-arrays:
  Float_t TofCalib::slope;
  Float_t TofCalib::tzero[4][kTofLBars];
  Double_t TofCalib::s0;
  Double_t TofCalib::s1;
  Double_t TofCalib::s3[4][kTofLBars];
  Double_t TofCalib::s30[4][kTofLBars];
  Double_t TofCalib::s4;
  Double_t TofCalib::s6[3][kTofLBars];
  Double_t TofCalib::s7[kTofLBars];
  Double_t TofCalib::s8;
  Double_t TofCalib::s12[kTofLBars][kTofLBars];
  Double_t TofCalib::s13[kTofLBars][kTofLBars];
  Double_t TofCalib::s14[kTofLBars][kTofLBars];
  Double_t TofCalib::s15[3][kTofLBars];
  Double_t TofCalib::s16[kTofLBars];
  Double_t TofCalib::events;
  Double_t TofCalib::resol;
  Double_t TofCalib::resol1;
//--- Ampl-arrays;
  Double_t TofCalib::ambin1[kAmBrtBins][kAmEvStore];// s1-signals for each ref_bar/bin/event
  Int_t TofCalib::nevenb1[kAmBrtBins];// s1 events accum. per ref_bar/bin for ambin
  Double_t TofCalib::ambin2[kAmBrtBins][kAmEvStore];// s2-signals for each ref_bar/bin/event
  Int_t TofCalib::nevenb2[kAmBrtBins];// s2 events accum. per ref_bar/bin for ambin
  Double_t TofCalib::amchan[kTofChanMx][kAmEvStore];//centr.inc side-signals for each channel/event
  Int_t TofCalib::nevenc[kTofChanMx];// numb.of events accum. per channel for amchan
  Float_t TofCalib::gains[kTofChanMx];//trunc. mean ch.signals ("0" impact) relat. to ref.ones
  Float_t TofCalib::btamp[2][kAmBrtBins];// MostProb bar-signals for each side/bin(ref.bars) 
  Float_t TofCalib::ebtamp[2][kAmBrtBins];// Error in MostProb  for each side/bin(ref.bars) 
  Int_t TofCalib::rbls[kTofBTypes]={101,105,401,404,201,
                                    202,204,301,302,303,305};//ref.bar id's (for bar types 1-11)
  Float_t TofCalib::profb[kTofBTypes][kAmProfBins];// profile bins width(counting from "-" side)
  Float_t TofCalib::profp[kTofBTypes][kAmProfBins];// bin middle-point values
  Int_t TofCalib::nprbn[kTofBTypes]={16,16,16,16,15,16,16,14,15,16,16};//profile-bins vs bar-type
  
  Double_t TofCalib::a2dr[kTofChanMx];//sum of An/Sum(Dyn(pmt))-ratios for each channel
  Double_t TofCalib::a2dr2[kTofChanMx];//sum of ratios**2 for each channel
  integer TofCalib::neva2d[kTofChanMx];//number of events/channel for above sum
  Double_t TofCalib::d2sdr[kTofChanMx][kTofPmtMX];//sum of Dyn(pm)/Sum(Dyn(pm))-gains for LBBS
  Double_t TofCalib::d2sdr2[kTofChanMx][kTofPmtMX];//sum of ratios**2 for each LBBS
  Int_t TofCalib::nevdgn[kTofChanMx];//number of events/channel for above sum
  
  Double_t TofCalib::ammrfc[kTofBTypes];// mean charge (central incidence) for ref.counters
  Int_t TofCalib::nevrfc[kTofBTypes];// number of accum. events for above sum
  Double_t TofCalib::arefb[kTofBTypes][kAmEvStore];// tot-signals for each ref_bar/event
  Int_t TofCalib::nrefb[kTofBTypes];// numb.of events accum. per ref_bar
  Float_t TofCalib::fpar[10]={0,0,0,0,0,0,0,0,0,0};// parameters to fit
  Int_t TofCalib::nbinr;// working var. for A-profile fit
  Int_t TofCalib::iside;// working var. for A-profile fit
  Float_t TofCalib::fun2nb;// working var. for  fit
  Float_t TofCalib::mcharge[kAmProfBins]; 
  Float_t TofCalib::mcoord[kAmProfBins];
  Float_t TofCalib::emchar[kAmProfBins];
  Double_t TofCalib::aprofp[kTofBTypes][2*kAmProfPars];// A-profile parameters(ref.bars)
  Float_t TofCalib::clent;//bar-length (to exchange with FCN)
  Int_t TofCalib::cbtyp;//bar-type  (....................)
  Int_t TofCalib::fitflg;//flag     (....................)
//
  Float_t TofCalib::binsta[50];// bin-statistics arrays for dE/dX fit
  Float_t TofCalib::bincoo[50];// bin-coordinate ...
  Int_t TofCalib::elbt;//     tot.bins
  Float_t TofCalib::melfunchi2;
  Float_t TofCalib::mfunamchi2;
  Float_t TofCalib::elfitp[kLandfPars];// fit-parameters
  Char_t TofCalib::eltit[60];  // title for fit
//
  Int_t TofCalib::calstat[4][kTofLBars][2];
  UInt_t TofCalib::CFlistC[11]; 
  Int_t TofCalib::FirstLastEvent[2]; 
  Int_t TofCalib::FirstLastRun[2]; 
  Char_t TofCalib::FirstEvDate[30]; 
  Char_t TofCalib::LastEvDate[30]; 
//
//-------------------------
//--->general functions:
  Float_t TofCalib::brlen(Int_t ilay, Int_t ibar){
    int cnum=0;
    int btyp;
    for(int i=0;i<ilay;i++)cnum+=kNtofb[i];
    cnum+=ibar;
    btyp=_brtype[cnum];
    return _brlen[btyp-1];
  }
//---
  Int_t TofCalib::brtype(Int_t ilay, Int_t ibar){
    int cnum=0;
    for(int i=0;i<ilay;i++)cnum+=kNtofb[i];
    cnum+=ibar;
    return _brtype[cnum];
  }
//---
  Int_t TofCalib::barseqn(Int_t ilay, Int_t ibar){
    int cnum=0;
    for(int i=0;i<ilay;i++)cnum+=kNtofb[i];
    cnum+=ibar;
    return cnum;
  }
//---
  Int_t TofCalib::npmtps(Int_t ilay, Int_t ibar){
    int cnum=0;
    for(int i=0;i<ilay;i++)cnum+=kNtofb[i];
    cnum+=ibar;
    if(_brtype[cnum]==1 || _brtype[cnum]==3)return 3;
    else return 2;
  }
//----------------------------------------------------------------
//
class RunPar{
// 
private:
//
  static Float_t tofc[10];// TOF-cuts
  static Float_t genc[40];//general cuts
//
  static Int_t evsel[kEvSelCDim];//events, passed particular cuts
//
  static Bool_t mcdata;
//
  static Float_t rmcinp[kNlogb+2];//Rmc   inp distr.
  static Float_t rmcnos[kNlogb+2];//Rmc   NoSel(NoErrEvent) distr.
  static Float_t rmenos[kNlogb+2];//Rmeas NoSel(NoErrEvent) distr.
  static Float_t rmcpre[kNlogb+2];//Rmc   PreSel(OurSpace+Charge) distr.
  static Float_t rmepre[kNlogb+2];//Rmeas PreSel(OurSpace+Charge) distr.
  static Float_t rmcsel[kNlogb+2];//Rmc   Selected(All Cuts) distr.
  static Float_t rmesel[kNlogb+2];//Rmeas Selected(All Cuts) distr.
//
  static UInt_t _cumula[kNtrkl];
//
public:
//
  static Int_t firstev;
  static UInt_t StartRun;
  static Int_t Zval[kZindmx];//Z vs Zindex
  static Int_t NZevs[kZindmx];//collected events for each Zindex
  
  static unsigned long long trkadd[kNtradd];//current TRK list of uniqe addr.id
  static Int_t trkadde[kNtradd+1];//... stored events per list item(+1 for max.list ovfl)
  static Int_t trkaddp[kNtradd];//... ordered pointer's list
  static Int_t trkaddn;//total list elements
  static Float_t trxc0[kNtradd][kNtremx];//TRK X-croses(Z=0 plane) for given address  
  static Float_t tryc0[kNtradd][kNtremx]; 
  static Float_t trtgx[kNtradd][kNtremx];//... TangX ..... 
  static Float_t trtgy[kNtradd][kNtremx];
  
  static Float_t tdcorr[4][4][6][1024];//crat|slot|chan|bin
  static Int_t tdchmap[4][4][6]; 
//     
  static unsigned long long trkaddd[kNtradd];//TRK-tracks addresses list(over all events)
  static unsigned long long trkadds[kNtradd];//TRK-tracks sorted addresses list(over all events)
//---> functions:  
   static Bool_t WriteFile(unsigned long long arr[],Int_t nelem,Char_t fname[],UInt_t strun);
   static Bool_t ReadFile(unsigned long long arr[],Int_t nelem,Char_t fname[],UInt_t strun);
//   
  inline static UInt_t Cumul(Int_t layer){return _cumula[layer-1];};
//  (layer=1-9 !!!)
  
  inline static void SetMCF1(){mcdata=1;}
  inline static void SetMCF0(){mcdata=0;}
  inline static Bool_t GetMCF(){return mcdata;}
  static void init();
//--------
  static void addsev(Int_t i){
    assert(i>=0 && i< kEvSelCDim);
    evsel[i]+=1;
  }
  static Int_t getsev(Int_t i){
    assert(i>=0 && i< kEvSelCDim);
    return evsel[i];
  }
  static void prtselc();//to print selection counters
//
  static void imcinp(Int_t i){//to increment counter content
    assert(i>=0 && i< kNlogb+2);
    rmcinp[i]+=1;
  }
  static void imcnos(Int_t i){
    assert(i>=0 && i< kNlogb+2);
    rmcnos[i]+=1;
  }
  static void imenos(Int_t i){
    assert(i>=0 && i< kNlogb+2);
    rmenos[i]+=1;
  }
  static void imcpre(Int_t i){
    assert(i>=0 && i< kNlogb+2);
    rmcpre[i]+=1;
  }
  static void imepre(Int_t i){
    assert(i>=0 && i< kNlogb+2);
    rmepre[i]+=1;
  }
  static void imcsel(Int_t i){
    assert(i>=0 && i< kNlogb+2);
    rmcsel[i]+=1;
  }
  static void imesel(Int_t i){
    assert(i>=0 && i< kNlogb+2);
    rmesel[i]+=1;
  }
  static Float_t GetGenc(Int_t i){
    assert(i>=1 && i<=40);
    return genc[i-1];
  }
};//--->endof RunPar-class def
//--------------------------------
  void RunPar::init(){
    if(kRDCalib==1)SetMCF0();//RD
    else SetMCF1();
    trkaddn=0;
    for(Int_t i=0;i<kNtradd;i++){
      trkadd[i]=0;
      trkaddd[i]=0;
      trkadds[i]=0;
      trkadde[i]=0;
      trkaddp[i]=0;
      for(Int_t j=0;j<kNtremx;j++){
        trxc0[i][j]=0;
        tryc0[i][j]=0;
        trtgx[i][j]=0;
        trtgy[i][j]=0;
      }
    }

    _cumula[0]=1;
    for(int i=1;i<kNtrkl;i++){
      _cumula[i]=_cumula[i-1]*(1+kNladl[i-1]);
    }    
    
    for(Int_t i=0;i<kNlogb+2;i++){
      rmcinp[i]=0;
      rmcnos[i]=0;
      rmenos[i]=0;
      rmcpre[i]=0;
      rmepre[i]=0;
      rmcsel[i]=0;
      rmesel[i]=0;
    }
    for(Int_t i=0;i<kEvSelCDim;i++)evsel[i]=0;
    
//--->for PDF's business:    
    Zval[0]=1;//electr
    Zval[1]=1;//prot
    Zval[2]=2;//he
    Zval[3]=3;
    Zval[4]=4;
    Zval[5]=5;
    Zval[6]=6;
    Zval[7]=7;
    Zval[8]=8;
    Zval[9]=9;
    for(Int_t i=0;i<kZindmx;i++)NZevs[i]=0;
    
//--->general cuts:
    genc[0]=1.3;  // (1)max abs(beta)
    genc[1]=25.;  // (2)beta-fit max chi2_time
    genc[2]=10.;  // (3)beta-fit max chi2_space
    genc[3]=4.;  // (4)beta-fit max axceptable pattern
    
    genc[4]=10.;  // (5)max tof-clusters
     
    genc[5]=50000.;  // (6)trk max chi2
    genc[6]=20.;  // (7)trk max chi2sz
    genc[7]=10000.;  // (8)trk max chi2(noMS)
    
    genc[8]=12.;// (9)max long tof-trk matching(10)
    genc[9]=0.;  // (10)spare
    
    genc[10]=2.5;  // (11)SectEdep-thresh(if> -> Active)
    genc[11]=1.;  // (12)NAntiSectors>thresh(if> -> Active)
    
    genc[12]=3.;  // (13)min TrdSectors
    genc[13]=5.;  // (14)max TrdTrackChi2
    
    genc[14]=2.;  // (15)max Trk-Trd |Dx| at ref.plane
    genc[15]=7.;  // (16)max Trk-Trd |Dy| at ref.plane
    
    genc[16]=100.; //(17)ECAL EhitsTot MIP-thr(mev)
    genc[17]=5.;   //(18)ECfront/TRK cr.point min.accept.dist. from EC-edge(3)
    genc[18]=5.;   //(19)ECback/TRK cr.point min.accept.dist. from EC-edge(3)
    genc[19]=0.;   //(20)
    genc[20]=0.;   //(21)
    genc[21]=0.;   //(22)
    genc[22]=0.07;   //(23)max Trk-Trd |DAngleX| at ref.plane
    genc[23]=0.14;   //(24)max Trk-Trd |DAngleY| at ref.plane
    
    genc[24]=50.;//(25)max RichRing chi2 (do not exist now)
    genc[25]=3.; //(26)min hits in RichRing
    genc[26]=1.; //(27)max fraction of mirrored hits
    genc[27]=0.; //(28)min prob to be good ring
    genc[28]=2.; //(29)min photoelectrons
    genc[29]=1.; //(30)max beta-betaRefit assimetry
    genc[30]=0.;
    genc[31]=0.;
    genc[32]=0.;
    genc[33]=0.;
    genc[34]=0.;
    genc[35]=0.;
    genc[36]=0.;

    genc[37]=150000;// needed events for Tdelv-calib
    genc[38]=25000;// needed events for Tzslw-calib
    genc[39]=200000;// needed events for Amplf-calib

//
//---> Open TrkTrAddrList file if requested:
//
  if(kNBadAdds>0){
    Char_t fn[80];
    UInt_t refrun=1211903814;
    strcpy(fn,"TrkAddrList");
    Bool_t OK=ReadFile(trkadds,trkaddn,fn,refrun);
    if(OK)cout<<"=====> TrkTrAddrList is red from file "<<fn<<"."<<refrun<<endl;
    else cout<<"=====> Problem to read TrkTrAddrList from file "<<fn<<"."<<refrun<<" !!!"<<endl;
  }
//
//---> open file with TOF TdcCorr if requested:
//
  Char_t fname[80];
  Int_t crat,sslt,chan,bin,rdcr,rdsl,bmap,endflab;
  strcpy(fname,"TofTdcorRD.1209640559");
  
  if(kAnalyse[0]>0){//<--- init TofTdccor analysis

  ifstream crfile(fname,ios::in); // open file for reading
  if(!crfile){
    cout <<"<---- Error: Missing TofTdcCor-file !!? "<<fname<<endl;
    exit(1);
  }
//----
  for(crat=0;crat<4;crat++){//crates loop
    crfile >> rdcr;
    if(rdcr!=(crat+1)){
      cout<<"<---- Error: Broken structure, crate="<<crat+1<<" red as "<<rdcr<<endl;
      exit(1);
    }
    for(sslt=0;sslt<4;sslt++){// SFETs loop (no corr. for ACC (SFEA-card))
      crfile >> rdsl;
      if(rdsl!=(sslt+1)){
        cout<<"<---- Error: Broken structure, slot="<<sslt+1<<" red as "<<rdsl<<endl;
        exit(1);
      }
      crfile >> bmap;
      for(chan=0;chan<6;chan++){//Card channels loop(ch1-5,ft)
        tdchmap[crat][sslt][chan]=1;//non empty chan
        if((bmap & (1<<chan))==0){
          tdchmap[crat][sslt][chan]=0;// mark empty chan
	  continue;//empty or non-correctable channel
	}
        for(bin=0;bin<1024;bin++)crfile >> tdcorr[crat][sslt][chan][bin];//read one channel
      }
    }
  }
//
  crfile >> endflab;//read endfile-label
  crfile.close();
//
  if(endflab==12345){
    cout<<"      TofTdcCor-file is successfully read !"<<endl;
  }
  else{
    cout<<"<---- Error: broken structure in TofTdcCor-file !!!"<<endl;
    exit(1);
  }
  }//---> endof init TofTdccor analysis
//---    
  }//--->endof init
//-----------
  void RunPar::prtselc(){
    printf("\n");
    printf("    =================== RootRun event-statistics ===================\n");
    printf("\n");
    printf(" Entries to EventStatus control: % 8d\n",evsel[140]);
    printf(" Event has no error            : % 8d\n",evsel[141]);
    printf(" EventStatus OK(accepted)      : % 8d\n",evsel[142]);
    printf(" Entries to MyAnalysis         : % 8d\n",evsel[143]);
    printf(" Accepted for MyAnalysis       : % 8d\n",evsel[158]);
    printf(" MC entries                    : % 8d\n",evsel[144]);
    printf(" Accepted entries              : % 8d\n",evsel[145]);
    printf(" Event has LVL1                : % 8d\n",evsel[146]);
    printf(" (FTC|FTE) presence cut passed : % 8d\n",evsel[147]);
    printf(" LVL1 presence cut passed      : % 8d\n",evsel[148]);
    printf(" Particle presence cut passed  : % 8d\n",evsel[149]);
    printf(" Particle with TrkTR           : % 8d\n",evsel[155]);
    printf(" Particle with TrkTr+EcShow    : % 8d\n",evsel[154]);
    printf("     with NonZero charge       : % 8d\n",evsel[151]);
    printf("     with Zero charge          : % 8d\n",evsel[152]);
    printf(" Particle from outer space     : % 8d\n",evsel[153]);
    printf(" Particle out of SouthAtlAnom  : % 8d\n",evsel[156]);
    printf(" Particle out of PolesRegions  : % 8d\n",evsel[157]);
    printf(" Particle Charge>0 selected    : % 8d\n",evsel[160]);
    printf("      nCharge>0                : % 8d\n",evsel[161]);
    printf("      JorgePointerQ>0          : % 8d\n",evsel[162]);
    printf("      TofCahrge>0              : % 8d\n",evsel[163]);
    printf("      TrkCahrge>0              : % 8d\n",evsel[164]);
    printf("      TrdCharge>0              : % 8d\n",evsel[165]);
    printf("      RicCharge>0              : % 8d\n",evsel[166]);
    printf("---------- TofBeta quality --------------------\n");
    printf(" Particle contains TOF-beta    : % 8d\n",evsel[1]);
    printf(" TOF fed. volume OK            : % 8d\n",evsel[2]);
    printf(" TOF-Beta sign,quality OK      : % 8d\n",evsel[3]);
    printf("---------- TrkTrack quality -------------------\n");
    printf(" Particle contains TRKtrack    : % 8d\n",evsel[90]);
    printf(" ... track with TrueX          : % 8d\n",evsel[91]);
    printf(" ... track with AddrOK         : % 8d\n",evsel[92]);
    printf(" ... track with AdvancedFit    : % 8d\n",evsel[93]);
    printf(" ... track 'IsGood'(AdvF+TrueX): % 8d\n",evsel[94]);
    printf(" Track is passed 'TrueX'-cut   : % 8d\n",evsel[95]);
    printf(" ... + AdvFitOK(2HalfsOK)-cut  : % 8d\n",evsel[96]);
    printf(" ... + 'Chi2=Final'-cut        : % 8d\n",evsel[97]);
    printf("-----------------------------------------------\n");
    printf(" TOF&TRK-measurements OK       : % 8d\n",evsel[10]);
    printf(" TOF/TRK-track match OK        : % 8d\n",evsel[11]);
    printf(" TOFedep norm by TRK OK        : % 8d\n",evsel[20]);
    printf("-----------------------------------------------\n");
    printf(" ANTI-activity checks          : % 8d\n",evsel[12]);
    printf(" ANTI-activity found           : % 8d\n",evsel[13]);
    printf("-----------------------------------------------\n");
    printf(" Particle contains TRDtrack    : % 8d\n",evsel[14]);
    printf(" TRD-track Nsegments OK        : % 8d\n",evsel[15]);
    printf(" TRD-track Chi2 OK             : % 8d\n",evsel[16]);
    printf(" TRD-track quality OK          : % 8d\n",evsel[17]);
    printf("-----------------------------------------------\n");
    printf(" TRK- and TRD-tracks OK        : % 8d\n",evsel[18]);
    printf(" TRK/TRD-track matching OK     : % 8d\n",evsel[19]);
    printf("-----------------------------------------------\n");
    
    if(kTofCalibType[0]>0 || kTofCalibType[0]>0 || kTofCalibType[0]>0){
    printf("---------- General Calib.Comp.Rates -----------\n");
    printf(" Entries                       : % 8d\n",evsel[30]);
    printf(" TRK-track OK (rejected if not): % 8d\n",evsel[31]);
    printf(" + TOFBeta OK (rejected if not): % 8d\n",evsel[32]);
    printf(" + RawClust OK(rejected if not): % 8d\n",evsel[33]);
    printf(" ACC OK for A2Dratious(Nacc<7) : % 8d\n",evsel[34]);
    printf(" ...for Td/Tz/Again/Aabsn(Na<2): % 8d\n",evsel[35]);
    printf(" + ProtCharge                  : % 8d\n",evsel[36]);
    printf(" + HeliumCharge                : % 8d\n",evsel[37]);
    } 
    if(kTofCalibType[0]>0){
    printf("-------------- Tdlv-CALIB ---------------\n");
    printf(" Entries                       : % 8d\n",evsel[40]);
    printf(" RawClustQuality OK            : % 8d\n",evsel[41]);
    printf(" Charge OK (prot)              : % 8d\n",evsel[42]);
    printf(" PrevCalBeta OK (range,downw)  : % 8d\n",evsel[43]);
    printf(" Good TofClust/Trk matching    : % 8d\n",evsel[44]);
    }
    
    if(kTofCalibType[1]>0){
    printf("-------------- Tzslw-CALIB ---------------\n");
    printf(" Premonitor:                              \n");
    printf(" TOF&TRK-measurements OK       : % 8d\n",evsel[50]);
    printf("  + RawClust OK                : % 8d\n",evsel[51]);
    printf("  + Match4TzslwOK              : % 8d\n",evsel[52]);
    printf("  + TrapCounters OK            : % 8d\n",evsel[53]);
    printf("  + MomRange OK                : % 8d\n",evsel[65]);
    printf("  + BetaRange OK               : % 8d\n",evsel[66]);
    printf("      -----------------------------------      \n");
    printf(" Entries to calib              : % 8d\n",evsel[55]);
    printf(" RawClustQuality OK            : % 8d\n",evsel[56]);
    printf(" Charge OK (prot/mu)           : % 8d\n",evsel[57]);
    printf(" Trk MomRange OK               : % 8d\n",evsel[58]);
    printf(" PrevCalBeta OK (range,downw)  : % 8d\n",evsel[59]);
    printf(" MatchOK+TrapCOK+PathBetNormOK : % 8d\n",evsel[60]);
    printf("     +TofTruncEd OK (selected) : % 8d\n",evsel[61]);
    }
    
    if(kTofCalibType[1]>0){
    printf("-------------- Ampl-CALIB ---------------\n");
    printf(" Entries to calib              : % 8d\n",evsel[70]);
    printf(" An2Dyn: selected              : % 8d\n",evsel[71]);
    printf(" AnRelGains/AttLen:  partQ=1   : % 8d\n",evsel[75]);
    printf("          +MomRange(pos) OK    : % 8d\n",evsel[76]);
    printf("          +BetRange(tophem) OK : % 8d\n",evsel[77]);
    printf(" AbsNorm:  partQ=1             : % 8d\n",evsel[80]);
    printf("          +MomRange(pos) OK    : % 8d\n",evsel[81]);
    printf("          +BetRange(tophem) OK : % 8d\n",evsel[82]);
    }
    
    if(kAnalyse[6]==3){
    printf("-------------- TofBetaStudy(TB) ---------------\n");
    printf(" Selected                      : % 8d\n",evsel[100]);
    printf("    central counters events    : % 8d\n",evsel[101]);
    printf("    No Outer counters events   : % 8d\n",evsel[102]);
    printf("    No Top/Bot Corners events  : % 8d\n",evsel[103]);
    }
    if(kAnalyse[9]>0){
    printf("------------ TofTimingProblStudy --------------\n");
    printf(" Selected(1PperL,beta)         : % 8d\n",evsel[110]);
    printf(" Selected(BarPatt)             : % 8d\n",evsel[111]);
    printf(" Final(rigidity cut)           : % 8d\n",evsel[112]);
    }
    if(kAnalyse[6]==4){
    printf("-------------- TofStudy(ESTEC) ---------------\n");
    printf(" Selected                      : % 8d\n",evsel[120]);
    printf("           TransvMatchOK in L1 : % 8d\n",evsel[121]);
    printf("           TransvMatchOK in L2 : % 8d\n",evsel[122]);
    printf("           TransvMatchOK in L3 : % 8d\n",evsel[123]);
    printf("           TransvMatchOK in L4 : % 8d\n",evsel[124]);
    }
    printf("-----------------------------------------------\n");
  }
//--------> mem.reservation for RunPar static vals:
  Float_t RunPar::genc[40];
  Float_t RunPar::tofc[10];
  Int_t RunPar::evsel[kEvSelCDim];//events, selected by particular cuts
//
  UInt_t RunPar::_cumula[kNtrkl]={0,0,0,0,0,0,0,0};
//
  Bool_t RunPar::mcdata;
//
  Float_t RunPar::rmcinp[kNlogb+2];
  Float_t RunPar::rmcnos[kNlogb+2];
  Float_t RunPar::rmenos[kNlogb+2];
  Float_t RunPar::rmcpre[kNlogb+2];
  Float_t RunPar::rmepre[kNlogb+2];
  Float_t RunPar::rmcsel[kNlogb+2];
  Float_t RunPar::rmesel[kNlogb+2];
  Int_t RunPar::Zval[kZindmx];
  Int_t RunPar::NZevs[kZindmx];
  Int_t  RunPar::trkadde[kNtradd+1];
  Int_t  RunPar::trkaddp[kNtradd];
  Int_t  RunPar::trkaddn; 
  Float_t RunPar::trxc0[kNtradd][kNtremx];//TRK X-croses(Z=0 plane) for given address  
  Float_t RunPar::tryc0[kNtradd][kNtremx]; 
  Float_t RunPar::trtgx[kNtradd][kNtremx];//... TangX ..... 
  Float_t RunPar::trtgy[kNtradd][kNtremx];
  Float_t RunPar::tdcorr[4][4][6][1024]; 
  Int_t RunPar::tdchmap[4][4][6];
  UInt_t RunPar::StartRun;
  Int_t RunPar::firstev=0; 
//---
  unsigned long long RunPar::trkadd[kNtradd];
  unsigned long long RunPar::trkaddd[kNtradd];//mem.reservation
  unsigned long long RunPar::trkadds[kNtradd];//sorted list mem.reservation
//-------------------------------------------------------------------------
  Int_t TofCalib::cinit(){
//
  Char_t fname[80];
  Char_t name[80];
  Int_t ctyp,ntypes;
  Char_t datt[3];
  Char_t ext[80];
  Int_t date[2],year,mon,day,hour,min,sec;
  UInt_t iutct;
  tm begin;
  time_t utct;
  UInt_t verids[11],verid;
//
  if (kUseNewSlew) strcpy(name,"TofCflist2");// basic name for vers.list-file  
  else strcpy(name,"TofCflist");// basic name for vers.list-file  
  if(kRDCalib==0){
    strcpy(datt,"MC");
    sprintf(ext,"%d",kMCRefN);//MC-versn
  }
  else{
    strcpy(datt,"RD");
    sprintf(ext,"%d",kRefFileN);//RD-utc
  }
  strcat(name,datt);
  strcat(name,".");
  strcat(name,ext);
//
  strcpy(fname,"");
  strcat(fname,name);
  cout<<"====> TofCalib::cinit: Opening Calib_vers_list-file "<<fname<<'\n';
  ifstream vlfile(fname,ios::in);
  if(!vlfile){
    cout <<"<---- Error: missing vers.list-file !!? "<<fname<<endl;
    exit(1);
  }
  vlfile >> ntypes;// total number of calibr. file types in the list
  for(int i=0;i<ntypes;i++){
    vlfile >> verids[i];//
    CFlistC[i+1]=verids[i]; 
  }
  /*
  if (kUseNewSlew && ntypes==6) {
    ntypes++;
    CFlistC[ntypes]=kTofSlwRunID;
  }
  */
  CFlistC[0]=ntypes;
  if(kRDCalib==0){
    vlfile >> date[0];//YYYYMMDD beg.validity of TofCflistMC.ext file
    vlfile >> date[1];//HHMMSS ......................................
    year=date[0]/10000;//2004->
    mon=(date[0]%10000)/100;//1-12
    day=(date[0]%100);//1-31
    hour=date[1]/10000;//0-23
    min=(date[1]%10000)/100;//0-59
    sec=(date[1]%100);//0-59
    begin.tm_isdst=0;
    begin.tm_sec=sec;
    begin.tm_min=min;
    begin.tm_hour=hour;
    begin.tm_mday=day;
    begin.tm_mon=mon-1;
    begin.tm_year=year-1900;
    utct=mktime(& begin);
    iutct=uinteger(utct);
    cout<<"      TofCflistMC-file begin_date: year:month:day = "<<year<<":"<<mon<<":"<<day<<endl;
    cout<<"                                     hour:min:sec = "<<hour<<":"<<min<<":"<<sec<<endl;
    cout<<"                                         UTC-time = "<<iutct<<endl;
    CFlistC[ntypes+1]=kMCRefN;
    CFlistC[ntypes+2]=date[0];
    CFlistC[ntypes+3]=date[1];
  }
  else{
    utct=time_t(kRefFileN);
    printf("      TofCflistRD-file begin_date: %s",ctime(&utct)); 
    CFlistC[ntypes+1]=kRefFileN;
  }
  vlfile.close();
//
  for(Int_t il=0;il<kNtofl;il++){
    for(Int_t ib=0;ib<kNtofb[il];ib++){
      calstat[il][ib][0]=0;
      calstat[il][ib][1]=0;
    }
  }
//
  return 1;
 }
//-------
  Int_t TofCalib::cwrite(){
//
//---> create  TofCflistMC(RD) file:
//
  Char_t fname[80];
  Char_t name[80];
  Char_t frdate[30];
  Int_t ctyp,ntypes;
  Char_t datt[3];
  Char_t ext[80];
  Int_t date[2],year,mon,day,hour,min,sec;
  UInt_t iutct;
  tm begin;
  time_t utct;
  UInt_t verids[11],verid;
  Int_t cfvn;
  UInt_t StartRun,overid;
  time_t StartTime;
//
  StartRun=RunPar::StartRun;
  StartTime=time_t(StartRun);
  strcpy(frdate,asctime(localtime(&StartTime)));
//
//---> create new Calib-status file TofCStatMC(RD):
//
  if(RunPar::GetMCF()){
    strcpy(datt,"MC");
    overid=CFlistC[2];
    verid=overid+1;//get new MC-versn = old+1 
    CFlistC[2]=verid;//update CStatMC-versn in static store
  }
  else{
    strcpy(datt,"RD");
    overid=CFlistC[1];
    if(overid==StartRun)verid=overid+1;//for safety when rerun over the same Data
    else verid=StartRun;
    CFlistC[1]=verid;//update CStatRD utc in static store
  }
  if (kUseNewSlew) strcpy(name,"TofCStat2");//generic name
  else strcpy(name,"TofCStat");//generic name
  strcat(name,datt);
  strcat(name,".");
  sprintf(ext,"%d",verid);
  strcat(name,ext);
  cout<<"  <-- Opening TofCalibStatus-file for writing : "<<name<<'\n';
  ofstream stfile(name,ios::out|ios::trunc); // open file for writing
  if(!stfile){
    cout <<"<---- Error: Can't write new TofCalibStatus-file: "<<name<<endl;
    exit(1);
  }
//
  for(int il=0;il<kNtofl;il++){   // <-------- loop over layers
  for(int ib=0;ib<kNtofb[il];ib++){  // <-------- loop over bar in layer
    for(int ip=0;ip<2;ip++){
      stfile << " "<< calstat[il][ib][ip];//stat(s1),stat(s2)
    }
    stfile << endl;
  } // --- end of bar loop --->
  stfile << endl;
  } // --- end of layer loop --->
//
  stfile << 12345 << endl;//endoffile label
  stfile << endl;
  stfile << endl<<"======================================================"<<endl;
  stfile << endl<<" First run used for calibration is "<<StartRun<<endl;
  stfile << endl<<" Date of the first event : "<<frdate<<endl;
  stfile.close();
//
  cout <<"  <-- new TofCStat-file "<<name<<" is created !!!"<<endl;
//
//----> Create new TofCflistRD(MC)-file:
//
  ntypes=integer(CFlistC[0]);//7/6->MC/RD
  if (kUseNewSlew) strcpy(name,"TofCflist2");// basic name for vers.list-file  
  else strcpy(name,"TofCflist");// basic name for vers.list-file  
  if(RunPar::GetMCF()){
    strcpy(datt,"MC");
    overid=CFlistC[ntypes+1];
    verid=overid+1;//new vers = old+1
    sprintf(ext,"%d",verid);
  }
  else{
    strcpy(datt,"RD");
    overid=CFlistC[ntypes+1];
    if(overid==StartRun)verid=overid+1;//for safety when rerun over the same Data
    else verid=StartRun;
    sprintf(ext,"%d",verid);//new RD-utc
  }
  strcat(name,datt);
  strcat(name,".");
  strcat(name,ext);
//
  cout<<"  <--  writing new Calib_vers_list-file... "<<name<<'\n';
  ofstream vlfile(name,ios::out|ios::trunc);
  if(!vlfile){
    cout <<"  <-- Error:Can't write new vers.list-file !!? "<<name<<endl;
    return 0;;
  }
  cout << ntypes << endl;
  vlfile << ntypes;// total number of calibr. file types in the list
  vlfile << endl;
  for(int i=0;i<ntypes;i++){
    cout << CFlistC[i+1] << endl;
    vlfile << CFlistC[i+1];
    vlfile << endl;
  }
//---
  UInt_t yyyymmdd,hhmmss;
  Int_t msec[2]={1,1};//begin/end for MC
  Int_t mmin[2]={0,0};
  Int_t mhour[2]={0,0};
  Int_t mday[2]={1,1};
  Int_t mmon[2]={0,0};
  Int_t myear[2]={111,125};//(since year 1900)
  if(RunPar::GetMCF()){//add begin date/time for MC
    year=1900+myear[0];//tempor use fixed begin time
    mon=mmon[0]+1;
    day=mday[0];
    hour=mhour[0];
    min=mmin[0];
    sec=msec[0];
    yyyymmdd=year*10000+mon*100+day;
    hhmmss=hour*10000+min*100+sec;
    vlfile << yyyymmdd;//YYYYMMDD beg.validity of TofCflistMC.ext file
    vlfile << endl;
    vlfile << hhmmss;//HHMMSS 
    vlfile << endl;
  }
  vlfile.close();
  cout <<"  <-- New CFlist-file "<<name<<" is created !!!"<<endl<<endl;
//
  return 1;
  }
//
//
//------------------------------------------
//
Bool_t RunPar::WriteFile(unsigned long long arr[],Int_t nelem,Char_t fname[],UInt_t strun){
  Char_t fext[20];
  sprintf(fext,"%d",strun);
  strcat(fname,fext);
  ofstream cfile(fname,ios::out|ios::trunc);
  if(!cfile)return(kFALSE);
  cfile << nelem << endl;
  for(Int_t i=0;i<nelem;i++){
    cfile << arr[i]<<endl;
  }
  cfile.close();
  return(kTRUE);
}
Bool_t RunPar::ReadFile(unsigned long long arr[],Int_t nelem,Char_t fname[],UInt_t strun){
  Char_t fext[20];
  sprintf(fext,"%d",strun);
  strcat(fname,fext);
  ifstream cfile(fname,ios::in); // open file for reading
  if(!cfile)return(kFALSE);
  cfile >> nelem;
  for(Int_t i=0;i<nelem;i++){
    cfile >> arr[i];
  }
  cfile.close();
  return(kTRUE);
}
//=======================================================================
//
///  This is an example of user class to process AMS Root Files 
class TofCalibPG : public AMSEventR {
   public :

//   static  vector<TH1F*>   h1A;   ///< An (alt) way of having array of histos
   TofCalibPG(TTree *tree=0){ };

   ~TofCalibPG()  {
   }
   virtual void      UBegin();//<-UserFunction called befor event loop
   virtual bool UProcessCut();//<-userFunction to preselect events(not Mandatory)
   virtual void      UProcessFill();//<-UserFunction called for selected entries only
                                    // Entry is the entry number in the current tree.
                                    // Analysis + Fills histograms.
   virtual bool UProcessStatus(unsigned long long status);  				    
   virtual void    UTerminate();//<-Called at the end of a loop on the tree
                                //convenient place to draw/fit histograms,save results
   void TrkAdrSel(Int_t mode, ParticleR * p2prt);
   void TrkAdrOut(Int_t mode);
   void TofTdcCorOut(Int_t mode);
   void TofAnalys1Out(Int_t mode);
   void TofAnalys2Out(Int_t mode);
   Int_t GetTofSensorTemper(int lay, int side, int pnt, int ch, int mode, Float_t &temp);
				
//   AMSEventList* select_list;
//   AMSEventList* badrun_list;
   
   TFile* outfile;
   
   static Bool_t Ready4Calib; 
      
   bool EvPreselOK();
};
//
Bool_t TofCalibPG::Ready4Calib=kFALSE; 

//------------------------------------------------
Int_t proentr[4][10]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//TOF-hist mem.reservation:

    TH1F * tofh1=0;
    TH1F * tofh1a=0;
    TH1F * tofh2=0;
    TH1F * tofh3=0;
    TH1F * tofh7=0;
    TH1F * tofh8=0;
    TH1F * tofh9=0;
//
    TProfile * tofp1=0;
    TProfile * tofp2=0;
    TProfile * tofp3=0;
    TProfile * tofp4=0;
    TH1F * tofh30=0;
    TH1F * tofh31=0;
    TH1F * tofh32=0;
    TH1F * tofh33=0;
    TH1F * tofh34=0;
    TH1F * tofh35=0;
    TH1F * tofh36=0;
    TH1F * tofh37=0;
    
    TProfile * tofp5=0;
    TProfile * tofp6=0;
    TProfile * tofp7=0;
    TProfile * tofp8=0;
    
    TProfile * tofp9[kNTofID]={0,0,0,0,0,0,0,0,0,0};
    
    TProfile * tofp10[kNTofIDr]={0,0,0,0,0,0,0,0,0,0};
    TH1F * tofh10=0;
    TH1F * tofh11=0;
    TH1F * tofht1=0;
    TH1F * tofht2=0;
    
    
    TH1F * tofhtdc1=0;
    TH1F * tofhtdc2=0;
    TH1F * tofhtdc3=0;
    
    TH1F * tofh12=0;
    TH1F * tofh13=0;
    TH1F * tofh14=0;
    TH1F * tofh15=0;
    
    TH2F * tofh16=0;
    TH2F * tofh17=0;
    TProfile * tofp16a[10]={0,0,0,0,0,0,0,0,0,0};
    TProfile * tofp16b[10]={0,0,0,0,0,0,0,0,0,0};
    TH1F * tofh18a[10]={0,0,0,0,0,0,0,0,0,0};
    TH1F * tofh18b[10]={0,0,0,0,0,0,0,0,0,0};
    
    TH1F * tofh20=0;
    TH1F * tofh21=0;
    TH1F * tofh22=0;
    TH1F * tofh23=0;
    TProfile2D * tofh24=0;
    TH1F * tofh25=0;
    TH1F * tofh26=0;
    TH1F * tofh27=0;
    TH2F * tofh28=0;
//
//ANTI-hist reservation:
    TH1F * anth1=0;
    TH1F * anth2=0;
    TH1F * anth3=0;
    TH1F * anth4=0;
    TH1F * anth5=0;
    
//TRD-hist reservation:
    TH1F * trdh1=0;
    TH1F * trdh2=0;
//    TH1F * trdh3=0;
//    TH1F * trdh4=0;
    TH1F * trdh5=0;
    TH1F * trdh6=0;
    TH1F * trdh7=0;
    TH1F * trdh8=0;
    TH1F * trdh9=0;
    TH1F * trdh10=0;
    TH1F * trdh11=0;
    TH1F * trdh12=0;
    TH1F * trdh13=0;
    TH1F * trdh14=0;
    TH1F * trdh15=0;
    TH1F * trdh16=0;
    TH1F * trdh17=0;
//TRK-hist reservation:
    TH1F * trkh1=0;
    TH1F * trkh2=0;
    TH1F * trkh3=0;
    TH1F * trkh4=0;
    TH1F * trkh5=0;
    TH1F * trkh6=0;
    TH2F * trkh7=0;
    TProfile * trkh8=0;
    TH1F * trkh9=0;
    TH1F * trkh10[kNtrkl]={0,0,0,0,0,0,0,0,0};
    TH1F * trkh11[kNtrkl]={0,0,0,0,0,0,0,0,0};
    TH1F * trkh12[kNtrkl]={0,0,0,0,0,0,0,0,0};
    TH1F * trkh13[kNtrkl]={0,0,0,0,0,0,0,0,0};
//MCGen-hist reserv:
    TH1F * mcgh1=0;
    TH1F * mcgh2=0;
    TH1F * mcgh3=0;
    TH1F * mcgh4=0;
    TH1F * mcgh5=0;
    TH1F * mcgh6=0;
    TH1F * mcgh7=0;
    TH1F * mcgh8=0;
    TH1F * mcgh9=0;
    TH1F * mcgh10=0;
    TH1F * mcgh11=0;
    TH1F * mcgh12=0;
    TH1F * mcgh13=0;
//AMS-hist reserv:
    TH1F * amsh1=0;
    TH2F * amsh2=0;
    TH1F * amsh3=0;
    TProfile * amsh3a=0;
    TProfile * amsh3b=0;
    TH1F * amsh4=0;
    TH1F * amsh5=0;
    TH1F * amsh6=0;
    TH1F * amsh7=0;
    TH1F * amsh8=0;
    TH2F * amsh9=0;
    TH2F * amsh10=0;
    TH1F * amsh11=0;
    
    TH1F * amsh15=0;
    TH1F * amsh16=0;
    TH1F * amsh17=0;
    TH1F * amsh18=0;
    TH1F * amsh19=0;
    TH1F * amsh20=0;
    
    TH1F * amsh21=0;
    TH1F * amsh22=0;
    TH1F * amsh23=0;
    TH1F * amsh24=0;
    
    TH1F * amsh25=0;
    TH1F * amsh26=0;
// TrigHistgrams:    
   TH1F * trgh1=0; 
   TH1F * trgh2=0; 
//--- Tdlv-calib hist:
   TH2F * tftdh1[4]={0,0,0,0};
   TH1F * tftdh2=0; 
   TH1F * tftdh3=0; 
   TH1F * tftdh4=0; 
//--- Tzsl-calib hist:
   TH1F * tftzh1=0; 
   TH1F * tftzh2=0; 
   TH1F * tftzh3=0; 
   TH1F * tftzh4=0; 
   TH1F * tftzh5[4]={0,0,0,0}; 
   TH1F * tftzh6[4]={0,0,0,0}; 
   TH1F * tftzh7=0; 
   TH1F * tftzh7a=0; 
   TH1F * tftzh8=0; 
   TH1F * tftzh9=0; 
   TH1F * tftzh10=0; 
   TProfile * tftzh11=0;
   TProfile * tftzh12=0;
   TH2F * tftzh13=0;
   TH2F * tftzh14=0;
   TProfile * tftzh15=0;
   TProfile * tftzh16=0;
   TProfile * tftzh17=0;
   TProfile * tftzh18=0;
   TProfile * tftzh19=0;
   TProfile * tftzh20=0;

//--- Ampl-calib hist:11
   TH1F * tfamh1=0; 
   TH1F * tfamh2=0; 
   TH1F * tfamh3=0; 
   TH1F * tfamh3a=0; 
   TH1F * tfamh4[kTofBTypes]={0,0,0,0,0,0,0,0,0,0,0}; 
   TH1F * tfamh5=0; 
   TH1F * tfamh6=0; 
   TH2F * tfamh7=0;
   TH1F * tfamh8=0; 
   TH1F * tfamh9=0; 
   TH1F * tfamh10[kTofBTypes]={0,0,0,0,0,0,0,0,0,0,0}; 
   TH1F * tfamh11[2*kTofBTypes]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
   TH1F * tfamh12=0; 
   TH1F * tfamh14=0; 
   TH1F * tfamh15=0; 
   TH1F * tfamh16=0; 
   TH1F * tfamh17=0; 
   TH1F * tfamh18=0;
   
   TProfile * tfamh20[68]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                           0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                                                                                              }; 
//------------------------------------------------
void TofCalibPG::UBegin(){
    cout << "----> Enter UBegin "<<endl;
//
  Float_t rigb[kNlogb+1];
//
   char title[127];
   char hname[127];
   UInt_t id;
//                       <-----Book TOF-hist:
   if(tofh1)delete tofh1;
   tofh1=new TH1F("tofh1","TOF:PaddleNumber(UsedByBeta)",40,1.,41.);
   if(tofh1a)delete tofh1a;
   tofh1a=new TH1F("tofh1a","TOF:PaddleNumber(GoodBeta + 2D-TrkMatchOK)",40,1.,41.);
   if(tofh2)delete tofh2;
   tofh2=new TH1F("tofh2","TOF:TotalClusters(WhenBetaFound)",40,0.,40.);
   if(tofh3)delete tofh3;
   tofh3=new TH1F("tofh3","TOF:Paddles/cluster(UsedByBeta)",10,0.,10.);
   if(tofh7)delete tofh7;
   tofh7=new TH1F("tofh7","TOF:EdTrMean1(Mev,NormIncBeta,fixBpow)",200,0.,100.);
   if(tofh8)delete tofh8;
   tofh8=new TH1F("tofh8","TOF:Edep(mev,id=201,center)",80,0.,8.);
   if(tofh9)delete tofh9;
   tofh9=new TH1F("tofh9","TOF:Edep(mev,id=201,perif)",80,0.,8.);
// 
   if(tofp1)delete tofp1;
   tofp1=new TProfile("tofp1","TOF:(LgCoo-TRKCr)<->PadNumb profile, L=1",8,1,9,-10,10);
   if(tofp2)delete tofp2;
   tofp2=new TProfile("tofp2","TOF:(LgCoo-TRKCr)<->PadNumb profile, L=2",8,1,9,-10,10);
   if(tofp3)delete tofp3;
   tofp3=new TProfile("tofp3","TOF:(LgCoo-TRKCr)<->PadNumb profile, L=3",10,1,11,-10,10);
   if(tofp4)delete tofp4;
   tofp4=new TProfile("tofp4","TOF:(LgCoo-TRKCr)<->PadNumb profile, L=4",8,1,9,-10,10);

   if(tofh30)delete tofh30;
   tofh30=new TH1F("tofh30","TOF:TofLcoo-TrkLcoo,L1",80,-16.,16.);
   if(tofh31)delete tofh31;
   tofh31=new TH1F("tofh31","TOF:TofLcoo-TrkLcoo,L2",80,-16.,16.);
   if(tofh32)delete tofh32;
   tofh32=new TH1F("tofh32","TOF:TofLcoo-TrkLcoo,L3",80,-16.,16.);
   if(tofh33)delete tofh33;
   tofh33=new TH1F("tofh33","TOF:TofLcoo-TrkLcoo,L4",80,-16.,16.);
   
   if(tofh34)delete tofh34;
   tofh34=new TH1F("tofh34","TOF:TofTcoo-TrkTcoo,L1",80,-20.,20.);
   if(tofh35)delete tofh35;
   tofh35=new TH1F("tofh35","TOF:TofTcoo-TrkTcoo,L2",80,-20.,20.);
   if(tofh36)delete tofh36;
   tofh36=new TH1F("tofh36","TOF:TofTcoo-TrkTcoo,L3",80,-20.,20.);
   if(tofh37)delete tofh37;
   tofh37=new TH1F("tofh37","TOF:TofTcoo-TrkTcoo,L4",80,-20.,20.);
//
   
   if(tofp5)delete tofp5;
   tofp5=new TProfile("tofp5","TOF:Edep<->PadNumb profile, L=1",8,1,9,0.2,8);
   if(tofp6)delete tofp6;
   tofp6=new TProfile("tofp6","TOF:Edep<->PadNumb profile, L=2",8,1,9,0.2,8);
   if(tofp7)delete tofp7;
   tofp7=new TProfile("tofp7","TOF:Edep<->PadNumb profile, L=3",10,1,11,0.2,8);
   if(tofp8)delete tofp8;
   tofp8=new TProfile("tofp8","TOF:Edep<->PadNumb profile, L=4",8,1,9,0.2,8);
   
   for (int j = 0; j < kNTofID; j++){
     if(tofp9[j])delete tofp9[j];
     sprintf(hname,"tofp9%01d",j);
     sprintf(title,"TOF:Edep<->LongCoo profile, TofID=%03d",kTofID[j]);
     tofp9[j]=new TProfile(hname,title,13,-65,65,0.1,8);
   }
   
   for (int j = 0; j < kNTofIDr; j++){
     if(tofp10[j])delete tofp10[j];
     sprintf(hname,"tofp10%01d",j);
     sprintf(title,"TOF:RawAdc<->LongCoo profile, TofID=%04d",kTofIDr[j]);
     tofp10[j]=new TProfile(hname,title,13,-65,65,1,51);
   }
   if(tofh10)delete tofh10;
   tofh10=new TH1F("tofh10","TOF:RawCl:As1+As2(pC,corrected,id=201)",80,0.,200.);
   if(tofh11)delete tofh11;
   tofh11=new TH1F("tofh11","TOF:RawCl:As1+As2(pC,corrected,id=104)",80,0.,200.);
   
   
   if(tofht1)delete tofht1;
   tofht1=new TH1F("tofht1","SigRelatRest",120,0.,0.3);
   if(tofht2)delete tofht2;
   tofht2=new TH1F("tofht2","MaxRelatDeviation",50,0.,10.);
   
//-------------------------   
   if(tofh12)delete tofh12;
   tofh12=new TH1F("tofh12","TOF:Time Resolution(all counters)",80,-1.5,1.5);
   if(tofh13)delete tofh13;
   tofh13=new TH1F("tofh13","TOF:Time Resolution(outer excluded)",80,-1.5,1.5);
   if(tofh14)delete tofh14;
   tofh14=new TH1F("tofh14","TOF:Velocity(3 layers)",80,0.5,1.3);
   if(tofh15)delete tofh15;
   tofh15=new TH1F("tofh15","TOF:Velocity(4 layers)",80,0.5,1.3);
//toftrig-study:
   if(tofh16)delete tofh16;
   tofh16=new TH2F("tofh16","TOF:TrigTofFlag1 vs FiredTofFlag1",16,-1.,15.,16,-1.,15.);
   tofh16->SetXTitle("TofFlag1: 0=all4,1-4->missL,5=13,6=14,7=23,8=24,9=12,10=34,11-14->1-4");
   if(tofh17)delete tofh17;
   tofh17=new TH2F("tofh17","TOF:Paddle Side-2 vs Side-1 amplitude ",50,0.,150.,50,0.,150.);
   tofh17->SetXTitle(" Side-1 amplitude (adc-ch)");
   tofh17->SetYTitle(" Side-2 amplitude (adc-ch)");
   for(Int_t pad=0;pad<kNtofb[kTofRefL-1];pad++){
     sprintf(hname,"tofp16a%01d",pad);
     sprintf(title,"Tof: FTC TrigPattBit Efficiency for Pad-%01d",pad);
     if(tofp16a[pad])delete tofp16a[pad];
     tofp16a[pad]=new TProfile(hname,title,60,0.,120.,0.,1.1);
     tofp16a[pad]->SetXTitle("Side raw ammplitude (adc-ch)");
   }
   for(Int_t pad=0;pad<kNtofb[kTofRefL-1];pad++){
     sprintf(hname,"tofp16b%01d",pad);
     sprintf(title,"Tof: FTC TrigPattBit Efficiency for Pad-%01d",pad);
     if(tofp16b[pad])delete tofp16b[pad];
     tofp16b[pad]=new TProfile(hname,title,60,0.,120.,0.,1.1);
     tofp16b[pad]->SetXTitle("Side raw ammplitude (adc-ch)");
   }
//
   for(Int_t pad=0;pad<kNtofb[kTofRefL-1];pad++){
     sprintf(hname,"tofh18a%01d",pad);
     sprintf(title,"Tof: Side signal spectrum for Pad-%01d",pad);
     if(tofh18a[pad])delete tofh18a[pad];
     tofh18a[pad]=new TH1F(hname,title,60,0.,120.);
     tofh18a[pad]->SetXTitle("Raw ammplitude (adc-ch)");
   }
   for(Int_t pad=0;pad<kNtofb[kTofRefL-1];pad++){
     sprintf(hname,"tofh18b%01d",pad);
     sprintf(title,"Tof: Side signal spectrum for Pad-%01d",pad);
     if(tofh18b[pad])delete tofh18b[pad];
     tofh18b[pad]=new TH1F(hname,title,60,0.,120.);
     tofh18b[pad]->SetXTitle("Raw ammplitude (adc-ch)");
   }
//----------> Tof-resolution (subsets, public):
   if(tofh20)delete tofh20;
   tofh20=new TH1F("tofh20","PartVelocity(4Layers,centralC)",80,0.8,1.2);
   if(tofh21)delete tofh21;
   tofh21=new TH1F("tofh21","PartVelocity(4Layers,noOuterC)",80,0.8,1.2);
   if(tofh22)delete tofh22;
   tofh22=new TH1F("tofh22","PartVelocity(4Layers,noCorners)",80,0.8,1.2);
   if(tofh23)delete tofh23;
   tofh23=new TH1F("tofh23","PartVelocity(4Layers,all)",80,0.8,1.2);
   if(tofh24)delete tofh24;
   tofh24=new TProfile2D("tofh24","PartVelocityMap, top(L1,2) vs bot(L3,4)",78,0,78,99,0,99,0.8,1.2);
   if(tofh25)delete tofh25;
   tofh25=new TH1F("tofh25","PartVelocity(4Layers,>=1 OuterC)",80,0.8,1.2);
   if(tofh26)delete tofh26;
   tofh26=new TH1F("tofh26","PartVelocity(4Layers,>=1 Corner)",80,0.8,1.2);
   if(tofh27)delete tofh27;
   tofh27=new TH1F("tofh27","TOF-measured Velocity (sea level muons,KSC)",80,0.8,1.2);
   if(tofh28)delete tofh28;
   tofh28=new TH2F("tofh28","TOF: Track ImpPoint(Plane-1) ",50,-25.,25.,50,-25.,25.);
   tofh28->SetXTitle(" ImpPoint-X");
   tofh28->SetYTitle(" ImpPoint-Y");
//---------------------------------------------- 
//                       <-----Book ANTI-hist:
   if(anth1)delete anth1;
   anth1=new TH1F("anth1","Anti:NSectors",9,0.,9.);
   if(anth2)delete anth2;
   anth2=new TH1F("anth2","Anti:Etot(mev)(if>0)",100,0.,100.);
   if(anth3)delete anth3;
   anth3=new TH1F("anth3","Anti:Esect(mev)",100,0.,25.);
   if(anth4)delete anth4;
   anth4=new TH1F("anth4","Anti:SectorAppearFreq(Esect>0.4)",8,1,9);
   if(anth5)delete anth5;
   anth5=new TH1F("anth5","Anti:NSectors(Esect>0.4mev)",9,0.,9.);
   
//
//                       <-----Book TRD-hist:
   if(trdh1)delete trdh1;
   trdh1=new TH1F("trdh1","TRD:NSegments/Track",10,0.,10.);
   if(trdh2)delete trdh2;
   trdh2=new TH1F("trdh2","TRD:TrackChi2(Nsegm>=3)",50,0.,10.);
//   if(trdh3)delete trdh3;
//   trdh3=new TH1F("trdh3","TRD:trkthe-trdthe",50,-0.5,0.5);
//   if(trdh4)delete trdh4;
//   trdh4=new TH1F("trdh4","TRD:trkphi-trdphi",50,-0.5,0.5);
   if(trdh5)delete trdh5;
   trdh5=new TH1F("trdh5","TRD:TrackLikelihood(TrackOK)",120,0.,60.);
   if(trdh6)delete trdh6;
   trdh6=new TH1F("trdh6","TRD:Clusters/segment(TrackOK)",25,0.,25.);
   if(trdh7)delete trdh7;
   trdh7=new TH1F("trdh7","TRD:Hits/cluster(TrackOK)",25,0.,25.);
   if(trdh8)delete trdh8;
   trdh8=new TH1F("trdh8","TRD:HiEnHits/cluster(TrackOk,Ehit>5.9kev)",10,0.,10.);
   if(trdh9)delete trdh9;
   trdh9=new TH1F("trdh9","TRD:ClustersEnergy(kev,TrackOc,mult=1)",50,0.,10.);
   if(trdh10)delete trdh10;
   trdh10=new TH1F("trdh10","TRD:Clusters/Track(TrackOK)",25,0.,25.);
   if(trdh11)delete trdh11;
   trdh11=new TH1F("trdh11","TRD:Clusters/Track(TrackOK,mult=1)",25,0.,25.);
   if(trdh12)delete trdh12;
   trdh12=new TH1F("trdh12","TRD:TotalRawHits",50,0.,100.);
   if(trdh13)delete trdh13;
   trdh13=new TH1F("trdh13","TRD:Clusters/Track(TrackOK,Ecl>5kev)",25,0.,25.);
   if(trdh14)delete trdh14;
   trdh14=new TH1F("trdh14","TRD:TotalClusters(all)",80,0.,80.);
   if(trdh15)delete trdh15;
   trdh15=new TH1F("trdh15","TRD:TotClusters(with Edep>5kev)",80,0.,80.);
   if(trdh16)delete trdh16;
   trdh16=new TH1F("trdh16","TRD:ClusterEnergy(kev,all)",50,0.,50.);
//
//                       <-----Book TRK-hist:
   if(trkh1)delete trkh1;
   trkh1=new TH1F("trkh1","TRKChi2(TrueX+AdvFitOK)",100,0.,50.);
   if(trkh2)delete trkh2;
//   trkh2=new TH1F("trkh2","TRKChi2sz(TrueX+AdvFitOK)",100,0.,50.);
//   if(trkh3)delete trkh3;
   trkh3=new TH1F("trkh3","TRKChi2(TrueX+AdvFitOK,NoMScat)",80,0.,8000.);
   if(trkh4)delete trkh4;
   trkh4=new TH1F("trkh4","TRKHalfRigAss(TrueX+AdvFitOK)",50,-1.,1.);
   if(trkh5)delete trkh5;
   trkh5=new TH1F("trkh5","TRK Rigidity(AllCuts,gv)",100,-10.,10.);
//   if(trkh6)delete trkh6;
//   trkh6=new TH1F("trkh6","TRK dR/R(JA)",50,0.,1.);
//   if(trkh7)delete trkh7;
//   trkh7=new TH2F("trkh7","TRK ClustAy(S) vx ClustAx(K)",50,0.,8000.,50,0.,10000.);
//   if(trkh8)delete trkh8;
//   trkh8=new TProfile("trkh8","TRK ClAy(S) vx ClAx(K) profile",50,0.,8000.,0.,10000.);
   if(trkh9)delete trkh9;
   trkh9=new TH1F("trkh9","dR/R (AllCuts)",50,-0.,1.);
   for (int j = 0; j < kNtrkl; j++){//<--slosest cluster distance
     if(trkh10[j])delete trkh10[j];
     sprintf(hname,"trkh10%01d",j);
     sprintf(title,"TRK:ClosestXclDist, Layer=%01d",j);
     trkh10[j]=new TH1F(hname,title,50,-0.025,0.025);
     if(trkh11[j])delete trkh11[j];
     sprintf(hname,"trkh11%01d",j);
     sprintf(title,"TRK:ClosestYclDist, Layer=%01d",j);
     trkh11[j]=new TH1F(hname,title,50,-0.025,0.025);
   }
   for (int j = 0; j < kNtrkl; j++){//<--residuals
     if(trkh12[j])delete trkh12[j];
     sprintf(hname,"trkh12%01d",j);
     sprintf(title,"TRK:Xresiduals, Layer=%01d",j);
     trkh12[j]=new TH1F(hname,title,50,-0.01,0.01);
     if(trkh13[j])delete trkh13[j];
     sprintf(hname,"trkh13%01d",j);
     sprintf(title,"TRK:Yresiduals, Layer=%01d",j);
     trkh13[j]=new TH1F(hname,title,50,-0.01,0.01);
   }
//
//
//                       <-----Book MCGen-hist:
   if(mcgh1)delete mcgh1;
   mcgh1=new TH1F("mcgh1","MCGen:Log(Rig),input",70,-1.,3.2);
   if(mcgh2)delete mcgh2;
   mcgh2=new TH1F("mcgh2","MCGen:EntryZ(cm)",80,-200.,200.);
   if(mcgh3)delete mcgh3;
   mcgh3=new TH1F("mcgh3","MCGen:EntryCos",50,-1.,1.);
   if(mcgh4)delete mcgh4;
   mcgh4=new TH1F("mcgh4","MCGen:Log(Rig),Part+Track(noCuts)",70,-1.,3.2);
   if(mcgh5)delete mcgh5;
   mcgh5=new TH1F("mcgh5","MCGen:Log(Rig),Part+Track(AllCuts)",70,-1.,3.2);
   if(mcgh6)delete mcgh6;
   mcgh6=new TH1F("mcgh6","Eff(Log(RigMC)),Prt+Trk(AllCuts/MCgen)",70,-1.,3.2);
   if(mcgh7)delete mcgh7;
   mcgh7=new TH1F("mcgh7","Eff(Log(RigMC)),Prt+Trk(All/No-Cuts)",70,-1.,3.2);
   if(mcgh8)delete mcgh8;
   mcgh8=new TH1F("mcgh8","MCGen:EntryX",50,-200.,200.);
   if(mcgh9)delete mcgh9;
   mcgh9=new TH1F("mcgh9","MCGen:EntryY",50,-200.,200.);
   if(mcgh10)delete mcgh10;
   mcgh10=new TH1F("mcgh10","MCGen:EntryZ",50,-200.,200.);
   if(mcgh11)delete mcgh11;
   mcgh11=new TH1F("mcgh11","MCGen:EntryDirX",50,-1.,1.);
   if(mcgh12)delete mcgh12;
   mcgh12=new TH1F("mcgh12","MCGen:EntryDirY",50,-1.,1.);
   if(mcgh13)delete mcgh13;
   mcgh13=new TH1F("mcgh13","MCGen:EntryDirZ",50,-1.,1.);
//
//                       <-----Book AMSreco-hist:
   if(amsh1)delete amsh1;
   amsh1=new TH1F("amsh1","Log(Rig),Part+Track(noCuts)",70,-1.,3.2);
   if(amsh2)delete amsh2;
   amsh2=new TH2F("amsh2","Part vs AMS dir(wrt loc.zenit)",52,-1.04,1.04,52,-1.04,1.04);
   if(amsh3)delete amsh3;
//   amsh3=new TH1F("amsh3","TofBeta",50,-2.5,2.5);
   amsh3=new TH1F("amsh3","TofBeta",80,0.8,1.2);
   if(amsh3a)delete amsh3a;
   amsh3a=new TProfile("amsh3a","TofBeta/MCbeta<->MCbeta(TofTrkMatchOK)",45,0.25,1.15,0.,1.5);
   if(amsh3b)delete amsh3b;
   amsh3b=new TProfile("amsh3b","TofBeta/TrkBeta<->TrkBeta(m/q<-MC, TofTrkMatchOK)",45,0.25,1.15,0.,1.5);
   if(amsh4)delete amsh4;
   amsh4=new TH1F("amsh4","TofBetaChi2t",50,0.,10.);
   if(amsh5)delete amsh5;
   amsh5=new TH1F("amsh5","TofBetaChi2s",50,0.,10.);
   if(amsh6)delete amsh6;
   amsh6=new TH1F("amsh6","TofBetaPattern",10,0.,10.);
   if(amsh7)delete amsh7;
   amsh7=new TH1F("amsh7","Log(Rig),Part+Track(AllCuts)",70,-1.,3.2);
   if(amsh8)delete amsh8;
   amsh8=new TH1F("amsh8","Eff(Log(RigMeas)),Prt+Trk(All/No-Cuts)",70,-1.,3.2);
{
   Int_t iz=kZrefInd;
   if(kZrefInd==0)iz=2;//prot
   Float_t bh[kZindmx]={210.,210.,490.,900.,2100.,2700.,3500.,4200.,5500.,7000.,8400.,9100.,10500.,13800.};
   if(amsh9)delete amsh9;
   amsh9=new TH2F("amsh9","TrkTrMean(NormIncBeta,fixBpow) vs beta",40,0.3,1.1,70,0.,bh[iz-1]);
}
   if(amsh10)delete amsh10;
   amsh10=new TH2F("amsh10","TofBeta vs TrkMom",60,0.,60.,40,0.3,1.1);
//

   if(amsh15)delete amsh15;
   amsh15=new TH1F("amsh15","TrdRawHits",100,0.,100.);
   if(amsh16)delete amsh16;
   amsh16=new TH1F("amsh16","TofRawClusters",100,0.,100.);
   if(amsh17)delete amsh17;
   amsh17=new TH1F("amsh17","TrRecHits",100,0.,100.);
   if(amsh18)delete amsh18;
   amsh18=new TH1F("amsh18","RichHits",100,0.,100.);
   if(amsh19)delete amsh19;
   amsh19=new TH1F("amsh19","EcalHits",200,0.,200.);
   if(amsh20)delete amsh20;
   amsh20=new TH1F("amsh20","LiveTime",100,0.,1.);
//
//---> Trk/Trd matching:
//
   if(amsh21)delete amsh21;
   amsh21=new TH1F("amsh21","TrkX-TrdX in plane z=TrdBotZ",60,-3.,3.);
   if(amsh22)delete amsh22;
   amsh22=new TH1F("amsh22","TrkY-TrdY in plane z=TrdBotZ",80,-4.,4.);
   if(amsh23)delete amsh23;
   amsh23=new TH1F("amsh23","TrkAngleX-TrdAngleX at plane z=TrdBotZ",60,-0.15,0.15);
   if(amsh24)delete amsh24;
   amsh24=new TH1F("amsh24","TrkAngleY-TrdAngleY at plane z=TrdBotZ",60,-0.15,0.15);
//---> Particle pars:
   if(amsh25)delete amsh25;
   amsh25=new TH1F("amsh25","Particle mass(PartMembSelected)",100,0.,25.);
   if(amsh26)delete amsh26;
   amsh26=new TH1F("amsh26","Particle charge(PartMembSelected)",100,0.,25.);
//------------------------------------------------------
//                       <-----Trigger Study histograms:
//
   if(trgh1)delete trgh1;
   trgh1=new TH1F("trgh1","JMembPatt",16,1.,17.);
   if(trgh2)delete trgh2;
   trgh2=new TH1F("trgh2","PhysMembPatt",8,1.,9.);
//------------------------------------------------------
//                       <-----Book Particular Analysis histograms:
   {//Hist. for Tof Tdc-linearity
     if(tofhtdc1)delete tofhtdc1;
     tofhtdc1=new TH1F("Tdc1","Bin-by-bin difference(channels inside TDC)",60,-0.3,0.3);
     if(tofhtdc2)delete tofhtdc2;
     tofhtdc2=new TH1F("Tdc2","Bin-by-bin difference(same channels of two TDCs)",60,-0.3,0.3);
     if(tofhtdc3)delete tofhtdc3;
     tofhtdc3=new TH1F("Tdc3","Random bin difference(same channels of two TDCs)",60,-0.3,0.3);
   }
//------------------------------------------------------------------------
//                   **************** TOF TimeAmpl calibration inits: ******************
//
  if(kTofCalibType[0]>0 || kTofCalibType[1]>0 || kTofCalibType[2]>0){
    Int_t istatus=TofCalib::cinit();//tempor here
    cout<<"<----- CalibInit Done, status="<<istatus<<endl;
  }
//Tdlv
   if(kTofCalibType[0]>0){
     TofCalib::inittd();
     if(kTofCalibPrint[0]>0){
       if(tftdh1[0])delete tftdh1[0];
       tftdh1[0]=new TH2F("tftdh1[0]","TofCalib_Tdelv:SideTimesDiff(ns,L1B4) vs ImpPoint(cm)",65,-65.,65.,80,-20.,20.);
       if(tftdh1[1])delete tftdh1[1];
       tftdh1[1]=new TH2F("tftdh1[1]","TofCalib_Tdelv:SideTimesDiff(ns,L2B4) vs ImpPoint(cm)",65,-65.,65.,80,-20.,20.);
       if(tftdh1[2])delete tftdh1[2];
       tftdh1[2]=new TH2F("tftdh1[2]","TofCalib_Tdelv:SideTimesDiff(ns,L3B5) vs ImpPoint(cm)",65,-65.,65.,80,-20.,20.);
       if(tftdh1[3])delete tftdh1[3];
       tftdh1[3]=new TH2F("tftdh1[3]","TofCalib_Tdelv:SideTimesDiff(ns,L4B4) vs ImpPoint(cm)",65,-65.,65.,80,-20.,20.);
//
       if(tftdh2)delete tftdh2;
       tftdh2=new TH1F("tftdh2","All bins rms",50,0.,2.);
       if(tftdh3)delete tftdh3;
       tftdh3=new TH1F("tftdh3","All channels Chi2",50,0.,10.);
       if(tftdh4)delete tftdh4;
       tftdh4=new TH1F("tftdh4","All channels Slopes(inv Lspeed)",80,0.02,0.12);
     }
   }
//Tzsl
   if(kTofCalibType[1]>0){
     TofCalib::inittz();
     if(kTofCalibPrint[1]>0){
       if(tftzh1)delete tftzh1;
       tftzh1=new TH1F("tftzh1","Tzslw: PaddleNumber(TofRawClusters OK)",40,1.,41.);
       if(tftzh2)delete tftzh2;
       tftzh2=new TH1F("tftzh2","Tzslw: PartCharge(TofRawClusters OK)",10,1.,11.);
       if(tftzh3)delete tftzh3;
       tftzh3=new TH1F("tftzh3","Tzslw: ParticleMomentum(... + Charge OK)",100,-50.,50.);
       if(tftzh4)delete tftzh4;
       tftzh4=new TH1F("tftzh4","Tzslw: TofBeta(... + MomRange/TofBetPresel OK)",80,-1.2,1.2);
       
       for (int j = 0; j < kNtofl; j++) {
         if(tftzh5[j])delete tftzh5[j];
         sprintf(hname,"tftzh5%01d",j);
         sprintf(title,"Tzslw: TofClusLCoo-TRKCr(...+BetaRangeOK),Layer%01d",j+1);
         tftzh5[j]=new TH1F(hname,title,80,-20.,20.);
       } 
       for (int j = 0; j < kNtofl; j++) {
         if(tftzh6[j])delete tftzh6[j];
         sprintf(hname,"tftzh6%01d",j);
         sprintf(title,"Tzslw: TofClusTCoo-TRKCr(...+BetaRangeOK),Layer%01d",j+1);
         tftzh6[j]=new TH1F(hname,title,80,-20.,20.);
       }
        
       if(tftzh7)delete tftzh7;
       tftzh7=new TH1F("tftzh7","Tzslw: TrkBeta(implied mass)",80,0.,1.2);
       if(tftzh7a)delete tftzh7a;
       tftzh7a=new TH1F("tftzh7a","Tzslw: TofEdepTrunc(PartCharge=Requested)",60,0.,12.);
       if(tftzh8)delete tftzh8;
       tftzh8=new TH1F("tftzh8","Tzslw: PaddleNumber(final)",40,1.,41.);
       if(tftzh9)delete tftzh9;
       tftzh9=new TH1F("tftzh9","Tzslw: TofBetaChi2time(...+MomRange/TofBetPresel OK",50,0.,10.);
       if(tftzh10)delete tftzh10;
       tftzh10=new TH1F("tftzh10","Tzslw: TofBetaChi2Space(...+MomRange/TofBetPresel OK",50,0.,10.);
       if(tftzh11)delete tftzh11;
       tftzh11=new TProfile("tftzh11","Tzslw: Beta vs BetaImplied(Prot)",80,0.2,1.,0.2,2.);
       if(tftzh12)delete tftzh12;
       tftzh12=new TProfile("tftzh12","Tzslw: Beta vs BetaImplied(He)",80,0.2,1.,0.2,2.);
       if(tftzh13)delete tftzh13;
       tftzh13=new TH2F("tftzh13","TofBeta vs TrkMom(Pr)",100,0.,15.,50,0.2,1.2);
       if(tftzh14)delete tftzh14;
       tftzh14=new TH2F("tftzh14","TofBeta vs TrkMom(He)",100,0.,40.,50,0.2,1.2);
       if(tftzh15)delete tftzh15;
       tftzh15=new TProfile("tftzh15","TofBeta vs TrkMom(Pr)",100,0.,25.,0.2,1.2);
       if(tftzh16)delete tftzh16;
       tftzh16=new TProfile("tftzh16","TofBeta vs TrkMom(He)",100,0.,50.,0.2,1.2);
       if(tftzh17)delete tftzh17;
       tftzh17=new TProfile("tftzh17","1-TofBeta/ImplBeta vs ImplBeta(Pr)",80,0.2,1.,-0.5,0.5);
       if(tftzh18)delete tftzh18;
       tftzh18=new TProfile("tftzh18","1-TofBeta/ImplBeta vs ImplBeta(He)",80,0.2,1.,-0.5,0.5);
       if(tftzh19)delete tftzh19;
       tftzh19=new TProfile("tftzh19","1-TofBeta/ImplBeta vs TrkMom(Pr)",100,0.,15.,-0.6,0.6);
       if(tftzh20)delete tftzh20;
       tftzh20=new TProfile("tftzh20","1-TofBeta/ImplBeta vs TrkMom(He)",100,0.,50.,-0.6,0.6);
     }
   }
//Ampl
   if(kTofCalibType[2]>0){
     TofCalib::initam();
     if(kTofCalibPrint[2]>0){
       if(tfamh1)delete tfamh1;
       tfamh1=new TH1F("tfamh1","Ampl: Instant An/Dyn(sum) ratio(all bar/sides)",50,0.,25.);
       if(tfamh2)delete tfamh2;
       tfamh2=new TH1F("tfamh2","Ampl: Instant Dyn(p1)/Dyn(p1/2 aver) ratio(all bar/sides, pm1)",50,0.,2.);
       if(tfamh3)delete tfamh3;
       tfamh3=new TH1F("tfamh3","Ampl: Instant Dyn(p2)/Dyn(p1/2 aver) ratio(all bar/sides, pm2)",50,0.,2.);
       if(tfamh3a)delete tfamh3a;
       tfamh3a=new TH1F("tfamh3a","Ampl: Instant Dyn(p3)/Dyn(p1/2/3 aver) ratio(all bar/sides, pm3)",50,0.,2.);
       
       if(kTofCalibPrint[2]>1){
       for (int j = 0; j < kTofBTypes; j++){
         if(tfamh4[j])delete tfamh4[j];
         sprintf(hname,"tfamh4%02d",j);
         sprintf(title,"Ampl: ImpactLongCoo for BarType=%02d",j+1);
         tfamh4[j]=new TH1F(hname,title,130,-65.,65.);
       }
       }
       
       if(tfamh5)delete tfamh5;
       tfamh5=new TH1F("tfamh5","Ampl: Q(pC) RefBar(type2), s1)",50,0.,150.);
       if(tfamh6)delete tfamh6;
       tfamh6=new TH1F("tfamh6","Ampl: Q(pC) RefBar(type2), s2)",50,0.,150.);
       if(tfamh7)delete tfamh7;
       tfamh7=new TH2F("tfamh7","Ampl: QRefBar(type2,c.imp, pass/beta norm) vs Mom/ImplMass",80,0.,40.,80,0.,320.);
       
       if(tfamh8)delete tfamh8;
       tfamh8=new TH1F("tfamh8","Ampl: Anodes RelGainFit Chi2",50,0.,10.);
       if(tfamh9)delete tfamh9;
       tfamh9=new TH1F("tfamh9","Ampl: Anodes Relative(to ref.bar) gains(except ref.bars)",80,0.,4.);
       
       if(kTofCalibPrint[2]>0){
       for (int j = 0; j < kTofBTypes; j++){
         if(tfamh10[j])delete tfamh10[j];
         sprintf(hname,"tfamh10%02d",j);
         sprintf(title,"Ampl: LBinFit Chi2 (any side) for BarType=%02d",j+1);
         tfamh10[j]=new TH1F(hname,title,50,0.,10.);
       }
       }
       
       if(kTofCalibPrint[2]>0){
         for (int j = 0; j < kTofBTypes; j++){
           for (int is = 0; is < 2; is++){
             if(tfamh11[2*j+is])delete tfamh11[2*j+is];
             sprintf(hname,"tfamh11%02d",2*j+is);
             sprintf(title,"Ampl: LongAmplProfile, BarType=%02d, Side=%1d",j+1,is+1);
             tfamh11[2*j+is]=new TH1F(hname,title,67,-67.,67.);
           }
	 }
       }
       
       if(tfamh12)delete tfamh12;
       tfamh12=new TH1F("tfamh12","Ampl: LongSignalProfile FitChi2(11btypes*2sides)",50,0.,10.);
       
       if(tfamh14)delete tfamh14;
       tfamh14=new TH1F("tfamh14","Ampl: AbsNormFit(11 btypes) Chi2",50,0.,10.);
       
       if(tfamh15)delete tfamh15;
       tfamh15=new TH1F("tfamh15","Ampl: A2D(pm-sum) Fit par-1 (all chan)",80,0.,2.);
       if(tfamh16)delete tfamh16;
       tfamh16=new TH1F("tfamh16","Ampl: A2D(pm-sum) Fit par-2 (all chan)",50,0.,1.);
       
       if(tfamh17)delete tfamh17;
       tfamh17=new TH1F("tfamh17","Ampl: Average Dh(pm) rel.gains(all chan/pm)",50,0.,2.);
       if(tfamh18)delete tfamh18;
       tfamh18=new TH1F("tfamh18","Ampl: RelatRMS of aver. Dh(pm) rel.gains(all chan/pm)",50,0.,0.5);
       
       Int_t ii=0;
       for(int i=0;i<kNtofl;i++){
       for(int j=0;j<kNtofb[i];j++){
       for(int k=0;k<2;k++){//side-loop
         if(tfamh20[ii])delete tfamh20[ii];
         sprintf(hname,"tfamh20%02d",ii);
         sprintf(title,"Ampl: Log(A) vs Log(Dsum), Layer=%01d, Bar=%02d, Side=%1d",i,j,k);
         tfamh20[ii]=new TProfile(hname,title,150,0.,3.,0.,3.6);
	 ii+=1;  
       }
       }
       }
     }
   }
//------------------------------------------------------------------------
   RunPar::init();//clear counters
   RunPar::SetMCF0();//set def(real data)
// 
//   select_list = new AMSEventList;
//   badrun_list = new AMSEventList("my_badruns.list");
//
    cout << "<----  Begin ended "<<endl;
}
//--------------------------------------------------
// UProcessCut() is not mandatory
//
bool TofCalibPG::UProcessCut(){
// do not give gain in processing time !!!
//
//  if(badrun_list->Contains(this)) return false;
  if(EvPreselOK()==false) return false;
  return true;
}
//--------------------------------------------------
bool TofCalibPG::UProcessStatus(unsigned long long status){
// may give some gain in processing time !!!
  RunPar::addsev(140);//<--counts Entries
  if((status&(1<<30))==0){ //no event error
    RunPar::addsev(141);//<--counts Event with no errors
//    if(((status&(1<<3))>0 //part.contains tof
//       && (status&(1<<4))>0//part.contains trk
//                        )
//       || kAnalyse[6]==2){
      RunPar::addsev(142);//<--counts Event-status OK (event selected)
//      return true;
//    }
//    else return false;
    return true;
  }
//  else return false;
  return true;
}  				    
//--------------------------------------------------
//   short event-preselection procedure:
//
bool TofCalibPG::EvPreselOK() {
  if(Ready4Calib)return false;
  if(nLevel1()<=0)return false;
//  if (nParticle()==0) return false;
//  if(fHeader.Run < 1265286760)return false;
//add here more checks
  return true;
}
//--------------------------------------------------
void TofCalibPG::UProcessFill() {
// User Function called for selected entries only.
// Entry is the entry number in the current tree.
//
try{
//
  Int_t ii,jj,kk,ix,iy;
  Int_t mcrbin(0),merbin(0);
  Float_t arigmc(0),arigme(0),rrr,dx,dy,dz,x,y,z,xc,yc,zc,t1,t2,t3,t4,dt,tgx,tgy;
  Float_t xloc,yloc,zloc;
  Bool_t cutf[20];
  Float_t mcmom(0),mcharge,mcdir[3],mccoo[3],rigmc(0),mcmass(0),mcbeta(0);
  Float_t mcextr[2];
  Bool_t TofAll4L,Tof1P4L,TofCentral,TofNoOuter,TofNoCorners,TofSpecial1,TofSpecial1a,TofSpecial2;
  Int_t TofTopAddr,TofBotAddr;
//
  RunPar::addsev(143);//<--counts inputs
//
  if(nMCEventg()>0)RunPar::SetMCF1();//MC data
  else RunPar::SetMCF0();//Real data
//  cout<<" nMCEventg="<<nMCEventg()<<endl;
// some header info (status,time):
  UInt_t runn=fHeader.Run;
  UInt_t runt=fHeader.RunType;
  UInt_t evnum=fHeader.Event;
//cout<<"---------> Enter UProcessFill "<<" Run/Event="<<runn<<" "<<evnum<<" seq.ev="<<RunPar::getsev(143)<<endl;
  if((evnum%100000)==0)cout<<"<--- 100000 events processed..."<<endl;
//
  Int_t etime[2];
  etime[0]=fHeader.Time[0];
  etime[1]=fHeader.Time[1];
  Char_t date[30];
  strcpy(date,Time());
//
  int retval(0);
  Float_t tofstemp(-273);
  vector<float> aa;
//--->1st event actions:
//store 1st run#
  if(RunPar::firstev==0){
    gRandom->SetSeed();
    RunPar::StartRun=runn;
    TofCalib::FirstLastEvent[0]=evnum;
    TofCalib::FirstLastRun[0]=runn;
    strcpy(TofCalib::FirstEvDate,date);
    RunPar::firstev=1;
  }
//
//---> Random selection:
  Int_t dummy;
  Float_t ranv;
  ranv=gRandom->Rndm(dummy);  
  if(ranv>kEvsPortionToUse)return;
  RunPar::addsev(158);//<--counts inputs
//---
  TofCalib::FirstLastEvent[1]=evnum;
  TofCalib::FirstLastRun[1]=runn;
  strcpy(TofCalib::LastEvDate,date);
//-------
//
//cout<<"GetMCF="<<RunPar::GetMCF()<<endl;
  if(RunPar::GetMCF()){//<--- mc-data		
    RunPar::addsev(144);//<--counts inputs
    MCEventgR mc_ev=MCEventg(0);//get ref to 1st elem. of MCEventgR vector
    mcmom = mc_ev.Momentum;
    mcharge = mc_ev.Charge;
    mcmass = mc_ev.Mass;
    mcbeta = mcmom/sqrt(mcmom*mcmom+mcmass*mcmass);
    mccoo[0] = mc_ev.Coo[0];
    mccoo[1] = mc_ev.Coo[1];
    mccoo[2] = mc_ev.Coo[2];
    mcdir[0] = mc_ev.Dir[0];
    mcdir[1] = mc_ev.Dir[1];
    mcdir[2] = mc_ev.Dir[2];
     mcgh2->Fill(mccoo[2],1);
     mcgh3->Fill(mcdir[2],1);
     mcgh8->Fill(mccoo[0],1);
     mcgh9->Fill(mccoo[1],1);
     mcgh10->Fill(mccoo[2],1);
     mcgh11->Fill(mcdir[0],1.);
     mcgh12->Fill(mcdir[1],1.);
     mcgh13->Fill(mcdir[2],1.);
//    mcextr[0]=mccoo[0]+mcdir[0]*(66.87-mccoo[3])/mcdir[2];//extrapol.to TOF top honeyc.
//    mcextr[1]=mccoo[1]+mcdir[1]*(66.87-mccoo[3])/mcdir[2];//extrapol.to TOF top honeyc.
//cout<<" mcmom="<<mcmom<<" mccharge="<<mcharge<<endl;
    if(mcharge>0){//charged inp.part
      rigmc=mcmom/mcharge;
      arigmc=fabs(mcmom/mcharge);
      rrr=log10(arigmc)-kRiglmn;
      if(rrr>=0.){//<0.5gv->bin=0(left ovfl)
        mcrbin=floor(rrr/kRiglbn); // <-- Rmc index (log.scale)
        mcrbin+=1;
        if(mcrbin>kNlogb+1)mcrbin=kNlogb+1; // >5012gv->bin=kNlogb+1(right ovfl)
      }
      if(mcrbin<0 || mcrbin>=kNlogb+2)cout<<"mcrbin="<<mcrbin<<" mcmom="<<mcmom<<" mccharge="<<mcharge<<endl;
      RunPar::imcinp(mcrbin);//  MC-gener spectrum
      if(arigmc>0)mcgh1->Fill(log10(arigmc),1);
    }
//
    if(mcharge==0)return;//<=========== require Charged inp.particle when MC !!!!!
//
  }//--->endof "mc-data"
//
  RunPar::addsev(145);//<--counts accepted entries
//
//===============================================> extract LVL-1 params:
  Level1R * p2lev1=pLevel1(0);
  Bool_t LVL1OK(0);
  Int_t TOFTrigFl1(-1),TOFTrigFl2(-1),ECTrigFl(0);
  Bool_t FTCge3(0),FTC2bot(0),FTC2top(0),FTEalone(0),FTEor(0),FTEand(0),FTC(0),FTE(0);
  Bool_t ECnoLev1(0),ECLev1or(0),ECLev1and(0),FTEany(0),ECLev1any(0);
  Bool_t Lut1(0),Lut2(0),BZF(0),FTZ(0),AC0(0),AC1(0),ECFTor(0),ECFTand(0),ECL1or(0),ECL1and(0);
  Bool_t unbTofJM(0),protJM(0),ionJM(0),sionJM(0),elecJM(0),gammJM(0),unbEcJM(0);
  Float_t LiveTime;
  Int_t tofpattft[4]={0,0,0,0};
  Int_t tofpattbz[4]={0,0,0,0};
  Int_t MembPatt16=0;
  Int_t PhysMembPatt(0),PhysMembPattR(0);
  Bool_t TBExt0(0),TBExt1(0);
//cout<<"---> n/N-lev1="<<nLevel1()<<" "<<NLevel1()<<" p2lev1="<<pLevel1(0)<<endl;
  if(nLevel1()>0 && p2lev1>0){
    LVL1OK=1;
    TOFTrigFl1=p2lev1->TofFlag1;
    TOFTrigFl2=p2lev1->TofFlag2;
    ECTrigFl=p2lev1->EcalFlag;//MN, M=0/1/2/3->noFTE/FTEalone/FTEor/FTEand, N=0/1/2/3-> /undef/noLev1/Lev1or/Lev1and
//    if(ECTrigFl!=0)cout<<"TofTrifFl1/2/ECTrigFl="<<TOFTrigFl1<<" "<<TOFTrigFl1<<" "<<ECTrigFl<<endl;
    for(ii=0;ii<4;ii++){
      tofpattft[ii]=p2lev1->TofPatt1[ii];
      tofpattbz[ii]=p2lev1->TofPatt2[ii];
    }
    MembPatt16=p2lev1->JMembPatt;
    PhysMembPatt=p2lev1->PhysBPatt;
    LiveTime=p2lev1->LiveTime;
    amsh20->Fill(LiveTime,1);
//----
    RunPar::addsev(146);//<--Lev1Trig present
  }
//
  FTC=((MembPatt16&1)!=0);//extracted from JMembPatt
  Lut1=((MembPatt16&(1<<1))!=0);
  Lut2=((MembPatt16&(1<<2))!=0);
  FTZ=((MembPatt16&(1<<5))!=0);
  FTE=((MembPatt16&(1<<6))!=0);
  AC0=((MembPatt16&(1<<7))!=0);
  AC1=((MembPatt16&(1<<8))!=0);
  BZF=((MembPatt16&(1<<9))!=0);
  ECFTand=((MembPatt16&(1<<10))!=0);
  ECFTor=((MembPatt16&(1<<11))!=0);
  ECL1and=((MembPatt16&(1<<12))!=0);
  ECL1or=((MembPatt16&(1<<13))!=0);
  TBExt0=((MembPatt16&(1<<14)>0));
  TBExt1=((MembPatt16&(1<<15)>0));
//------
  for(int i=0;i<16;i++)if((MembPatt16&(1<<i))>0)trgh1->Fill(i+1,1);
  for(int i=0;i<8;i++)if((PhysMembPatt&(1<<i))>0)trgh2->Fill(i+1,1);
//  cout<<" FTC/FTE="<<FTC<<" "<<FTE<<" "<<MembPatt16<<endl;
//--->derived from my flags: 
  FTCge3=(TOFTrigFl1>=0 && TOFTrigFl1<=4);//derived from flags
  FTEany=(ECTrigFl/10>=2);
  FTEalone=(ECTrigFl/10==1);
  FTEor=(ECTrigFl/10==2);
  FTEand=(ECTrigFl/10==3);
  ECLev1any=(ECTrigFl%10>=2);
  ECnoLev1=(ECTrigFl/10>=2 && ECTrigFl%10<=1);
  ECLev1or=(ECTrigFl/10>=2 && ECTrigFl%10==2);
  ECLev1and=(ECTrigFl/10>=2 && ECTrigFl%10==3);
  TBExt0=((MembPatt16&(1<<14)>0));
  TBExt1=((MembPatt16&(1<<15)>0));
//---
  if(!FTC)return;//  <========= FTC always should be present
  RunPar::addsev(147);//<--FTC present
//---
  UInt_t itrtime[5];
  Double_t trigt;
//
  if(LVL1OK){
    for(int i=0;i<5;i++)itrtime[i]=pLevel1(0)->TrigTime[i];
    if(itrtime[1]>0)
       trigt=itrtime[2]*0.64+itrtime[3]*pow(2.,32)*0.64+1000000.*(itrtime[1]-1);//mksec
    else trigt=itrtime[2]*0.64+itrtime[3]*pow(2.,32)*0.64;//mksec
  }
//----
//==================================> total Hits in subdets:
  amsh15->Fill(nTrdRawHit(),1);
  amsh16->Fill(nTofRawCluster(),1);
//  amsh17->Fill(nTrRecHit(),1);
  amsh17->Fill(nTrRawCluster(),1);
  amsh18->Fill(nRichHit(),1);
  amsh19->Fill(nEcalHit(),1);
//==========================================================  
//
  if(!LVL1OK)return;// <========= require LVL1
  RunPar::addsev(148);//<--passed Lvl1 check
//
  if(NParticle()==0)return;// <========= Always require Particle
  RunPar::addsev(149);//<--passed Part check
//cout<<"<--- Aft.LVL1..."<<endl;
//======================================> check Particles Members:
  Int_t nparticles;
  Int_t itrktr(-1),itrdtr(-1),iecshow(-1),ircring(-1),pindex(-1),pindextrd(-1);
  Int_t PartMemb[4][5]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//[ipart][imemb]
  Bool_t TRKtrPart(0),TRDtrPart(0),ECALshowPart(0),RICHringPart(0);
  if(NParticle()<5)nparticles=NParticle();
  else nparticles=4;
  for(int i=0;i<nparticles;i++){//particle loop
    itrktr = Particle(i).iTrTrack();//TRKtrack index used by i-th Part(-1 if missing) 
    itrdtr = Particle(i).iTrdTrack();//TRDtrack index used by i-th Part.
//  itrktr=pParticle(i)->iTrTrack();//also work
//  itrdtr=pParticle(i)->iTrdTrack();//also work
    iecshow = Particle(i).iEcalShower();//EcShower index used by i-th Part.
    ircring = Particle(i).iRichRing();//RichRing index used by i-th Part.
    if(itrktr>=0)PartMemb[i][0]=1;
    if(itrdtr>=0)PartMemb[i][1]=1;
    if(iecshow>=0)PartMemb[i][2]=1;
    if(ircring>=0)PartMemb[i][3]=1;
//cout<<" <--- Particle:"<<i<<"itrk/itrd="<<itrktr<<" "<<itrdtr<<" ec:"<<iecshow<<" ric:"<<ircring<<endl;
  }
//cout<<endl;
//
  for(int i=0;i<nparticles;i++){//<=============select 1st particle with needed members
    if(PartMemb[i][0]==1
//                         || PartMemb[i][1]==1		                
//                         || PartMemb[i][2]==1		                
					      ){
      TRKtrPart    = (PartMemb[i][0]==1);
      TRDtrPart    = (PartMemb[i][1]==1);
      ECALshowPart = (PartMemb[i][2]==1);
      RICHringPart = (PartMemb[i][3]==1);
      pindex=i;//store index of 1st Part with needed members
      break;
					       }
  }
  if(TRKtrPart)RunPar::addsev(155);//<--- part with TRK-track
  if(TRKtrPart && ECALshowPart)RunPar::addsev(154);//<--- part with TRK+ECShow
//  if(ECTrigFl!=0)cout<<"  Part.selected Type=trk/trd/ecshow/rcring:"<<TRKtrPart<<
//                       " "<<TRDtrPart<<" "<<ECALshowPart<<" "<<RICHringPart<<
//		       " TofFlag1/EcalFlag="<<TOFTrigFl1<<" "<<ECTrigFl<<
//		       " pindex="<<pindex<<endl;
//
// <======= Select single Particle with needed members:
//
  if(pindex<0)return;//======> no Part. with needed members
  RunPar::addsev(150);//<--- passed part-type check
//----
  Float_t zctof[4]={0,0,0,0};
  Float_t tofcro[4][3]={0,0,0,0,0,0,0,0,0,0,0,0};
  Float_t toftrklen[4]={0,0,0,0};
  Float_t prtthe(0),prtphi(0),prttgx,prttgy;
  for(int i=0;i<4;i++){//<-----track cross.poins with tof-planes from VC
    for(int j=0;j<3;j++){
      tofcro[i][j]=pParticle(pindex)->TOFCoo[i][j];//TOFpln/TRK(trd)track cross.points
      if(j==2)zctof[i]=tofcro[i][j];//store Zcr vs Layer
    }
  }
  if(TRKtrPart){
    for(int i=0;i<4;i++)toftrklen[i]=pParticle(pindex)->TOFTLength[i];//TRK track length till TOFpaddle-crossing
  }
  prtthe=pParticle(pindex)->Theta;//part.track theta(at 1st(last) TRK-layer !!!)
  prtphi=pParticle(pindex)->Phi;//part.track phi
  prttgx=sin(prtthe)*cos(prtphi)/cos(prtthe);// ?????
  prttgy=sin(prtthe)*sin(prtphi)/cos(prtthe);
//
//  <===== "NoSelection" point
//
  Float_t prtmom=Particle(pindex).Momentum;
  Float_t prtmas=Particle(pindex).Mass;
  Float_t prtcha=Particle(pindex).Charge;
  Float_t rigme(0);
//
  amsh25->Fill(prtmas,1);
  amsh26->Fill(prtcha,1);
  if(prtcha!=0){
    rigme=prtmom/prtcha;
    arigme=fabs(prtmom/prtcha);//define Rmeas-bin:
    RunPar::addsev(151);//<--passed RealData "Q>0" test
  }
  else{
    arigme=0.01;
    RunPar::addsev(152);//<--passed RealData "Q=0" test
  }
  rrr=log10(arigme)-kRiglmn;
  merbin=0;
  if(rrr>=0.){//<0.5gv->bin=0(left ovfl)
    merbin=floor(rrr/kRiglbn); // <-- Rmeas index (log.scale)
    merbin+=1;
    if(merbin>kNlogb+1)merbin=kNlogb+1; // >5012gv->bin=kNlogb+1(right ovfl)
    if(merbin<0)merbin=0;
  }
//cout<<"RigMC/RE="<<arigmc<<" "<<arigme<<" pmom="<<prtmom<<endl;
//cout<<"mc/re-bin="<<mcrbin<<" "<<merbin<<endl;
  if(RunPar::GetMCF())RunPar::imcnos(mcrbin);// MCgen Rig-spectrum with PartTrack (NoAnyCuts)
  RunPar::imenos(merbin);// Measured Rig-spectrum with PartTrack (NoAnyCuts)
  if(RunPar::GetMCF())if(arigmc>0)mcgh4->Fill(log10(arigmc),1);
  if(arigme>0)amsh1->Fill(log10(arigme),1);
//
//                         <--- check Header,Part/AMS-pointing:
//
  Float_t erads=fHeader.RadS;
  Float_t ethetas=fHeader.ThetaS;
  Float_t ephis=fHeader.PhiS;
  Float_t eyaw=fHeader.Yaw;
  Float_t epitch=fHeader.Pitch;
  Float_t eroll=fHeader.Roll;
  Float_t ethetam=fHeader.ThetaM;
  Float_t ephim=fHeader.PhiM;
  Float_t csy,sny,csr,snr,csp,snp;
  csy=cos(eyaw);
  sny=sin(eyaw);
  csr=cos(eroll);
  snr=sin(eroll);
  csp=cos(epitch);
  snp=sin(epitch);
  Float_t csznams=-snr*sny*snp+csr*csp;//<-- cos(loc_zenith /\ AMS_Z)(>0 if look into outer space)
  Float_t ethetasi=1.570796-ethetas;// ???
  Float_t ez1=cos(ethetasi);// <-- Z-proj. of vector from earth to AMS(local zenith in GTOD)
  Float_t ex1=sin(ethetasi)*cos(ephis);
  Float_t ey1=sin(ethetasi)*sin(ephis);
  Float_t ez2=cos(Particle(pindex).ThetaGl);// <-- Z-proj. of Part.velocity vector in GTOD
  Float_t ex2=sin(Particle(pindex).ThetaGl)*cos(Particle(pindex).PhiGl);
  Float_t ey2=sin(Particle(pindex).ThetaGl)*sin(Particle(pindex).PhiGl);
  Float_t csznpart=ex1*ex2+ey1*ey2+ez1*ez2;//<-- cos(part_dir/\local_zenith), <0 if part from out.space
//
//cout<<"  csznams/csznpart="<<csznams<<" "<<csznpart<<endl;
  amsh2->Fill(csznams,csznpart,1.); 
//  if((csznams<0) || (csznpart>0))return;// =====> Non "outer space" pointing(ams or particle)
//cout<<"   outer space OK"<<endl;
//
  RunPar::addsev(153);//<--passed "outer space" test
//
//  Bool_t SAAN=((fHeader.ThetaS<0 && fHeader.ThetaS>-1) && (fHeader.PhiS<0.4 || fHeader.PhiS>4.8));
  Float_t phi,theta;
  if (fHeader.PhiS<pi) phi=fHeader.PhiS*180/pi;
  else phi=fHeader.PhiS*180/pi-360;
  theta=fHeader.ThetaS*180/pi;
  Bool_t SAAN=(!TofCalib::saa(phi,theta));
  if(SAAN)return;
  RunPar::addsev(156);//<--passed "SouthAtlAnomaly" test
//
  Bool_t PoleReg=(fHeader.ThetaS>1 || fHeader.ThetaS<-1);
  if(PoleReg)return;
  RunPar::addsev(157);//<--passed "PolesRegions" test
//------------------------------------------------------------------
  if(prtcha<=0)return;//<========== tempor: Particle Charge Cut
  RunPar::addsev(160);
//
//---> check Jorge Charges:
//
  Int_t trkcha(0),tofcha(0),TofCUsed(0),TrkCUsed(0);
  Int_t trdcha(0),riccha(0),TrdCUsed(0),RicCUsed(0);
  Float_t toftre(0),trktre(0);
  Float_t trdtre(0),rictre(0);
  Int_t TofQ(0),TrkQ(0);
  Int_t TrdQ(0),RicQ(0);
//
  if(nCharge()<=0){
    cout<<"<--- No JQ: prtcha="<<prtcha<<endl;
    return;
  }
  RunPar::addsev(161);//<--- passed nCharge check
  ChargeR *JZ = pCharge(0);
  if(JZ<=0){
    cout<<"<--- No JQPointer: prtcha="<<prtcha<<endl;
    return;
  }
  RunPar::addsev(162);//<--- passed JPointer check
//
  ChargeSubDR *zTof=JZ->getSubD("AMSChargeTOF");
  if(zTof){
    tofcha = TMath::Max(Int_t(zTof->ChargeI[0]),1);// <--- Tof-charge
    TofCUsed = (((zTof->Status)>>5)&1);
    toftre =zTof->getAttr("TruncatedMean");//tof Edep truncated average
    TofQ = zTof->Q;
  }
//  cout<<"<--- TofCharge: tofcha="<<tofcha<<"  prtcha="<<prtcha<<"  toftrunce="<<toftre<<" used="<<TofCUsed<<" tofQ="<<TofQ<<endl;
  if(tofcha>0)RunPar::addsev(163);
  ChargeSubDR *zTrk=JZ->getSubD("AMSChargeTracker");
  if(zTrk){
      trkcha = TMath::Max(Int_t(zTrk->ChargeI[0]),1);// <--- Trk-charge
      trktre =zTrk->getAttr("TruncatedMean");//trk Edep truncated average 
      TrkQ = zTrk->Q;
      TrkCUsed = (((zTrk->Status)>>5)&1);
  }
  if(trkcha>0)RunPar::addsev(164);
//
  ChargeSubDR *zTrd=JZ->getSubD("AMSChargeTRD");
  if(zTrd){
      trdcha = TMath::Max(Int_t(zTrd->ChargeI[0]),1);// <--- Trd-charge
//      trdtre =zTrd->getAttr("TruncatedMean");//trd Edep truncated average 
      TrdQ = zTrk->Q;
      TrdCUsed = (((zTrd->Status)>>5)&1);
  }
//  cout<<"<--- TrdCharge: trdcha="<<trdcha<<"  trdtrune="<<trdtre<<" used="<<TrdCUsed<<" TrdQ="<<TrdQ<<endl; 
  if(trdcha>0)RunPar::addsev(165);
//
  ChargeSubDR *zRic=JZ->getSubD("AMSChargeRich");
  if(zRic){
      riccha = TMath::Max(Int_t(zRic->ChargeI[0]),1);// <--- Rich-charge
//      rictre =zTrd->getAttr("TruncatedMean");//trd Edep truncated average 
      RicQ = zRic->Q;
      RicCUsed = (((zRic->Status)>>5)&1);
  }
//  cout<<"<--- RichCharge: riccha="<<riccha<<"  rictrune="<<rictre<<" used="<<RicCUsed<<" RicQ="<<RicQ<<endl; 
  if(trdcha>0)RunPar::addsev(166);
//----------------------------------------------------------------------
//
//<------ check/select  TofRawClusters for TAU-calib :
//
  Bool_t TOFRawClustOK(0);
  Int_t il,ib,is;
  TofRawClusterR *p2tofrcl;
  Int_t ntofrcl=nTofRawCluster();//total tof-rawclusters
  Int_t laymap[4]={0,0,0,0};
  Int_t laysmap[4][2]={0,0,0,0,0,0,0,0};
  Int_t bad(0);
  UInt_t stat(0);
  Float_t tofedep[4]={0,0,0,0};
  Float_t tofsdthr(10);//thresh. on side signal(adc-ch)
  Int_t TofFlag1(-1);
  Float_t radca[2],sidetm[2];
  Float_t tofsd1q[4]={0,0,0,0};
  Float_t tofsd2q[4]={0,0,0,0};
  Float_t toftimr[4]={0,0,0,0}; 
  Float_t toftim[4]={0,0,0,0}; 
  Float_t toftimd[4]={0,0,0,0}; 
  Float_t tofcool[4]={0,0,0,0}; 
  Float_t tofadca[4][2]={0,0,0,0,0,0,0,0};
  Float_t tofadcdr1[4][3]={0,0,0,0,0,0,0,0,0,0,0,0};
  Float_t tofadcdr2[4][3]={0,0,0,0,0,0,0,0,0,0,0,0};
  Int_t tofnbrl[4]={0,0,0,0};
  Int_t tofbrnl[4]={-1,-1,-1,-1};
//
//cout<<"  nTofRawClust="<<ntofrcl<<endl;
  
  Float_t defTzslop;
  if(kUseHe4Calib)defTzslop=kTofDefTzslopHe;
  else defTzslop=kTofDefTzslop;
//---
  for(int ic=0;ic<ntofrcl;ic++){//<-- TofRawClusters loop
    p2tofrcl=pTofRawCluster(ic);
    il=p2tofrcl->Layer;//1-4
    ib=p2tofrcl->Bar;//1-8(10)
//    if(ib==1 || ib==kNtofb[il-1])continue;//skip outer counter
    stat=p2tofrcl->Status;
//cout<<" il/ib="<<il<<" "<<ib<<"  status="<<stat<<endl;

    if((stat&256)==0                                 //not 1-sided
		                  && (stat&512)==0){   //ok for tmeasurement acc.to DB)
//cout<<"  status OK"<<endl;
      radca[0]=p2tofrcl->adca[0];
      radca[1]=p2tofrcl->adca[1];
      if(radca[0]>tofsdthr && radca[1]>tofsdthr){
//cout<<"   SideADCs OK"<<endl;
        tofnbrl[il-1]+=1;
        tofbrnl[il-1]=ib-1;//<--- store bar#0-7(9)
        tofsd1q[il-1]=kTofAdc2Q*radca[0];//side Q(pc) for tzslw
        tofsd2q[il-1]=kTofAdc2Q*radca[1];
	sidetm[0]=p2tofrcl->sdtm[0];
	sidetm[1]=p2tofrcl->sdtm[1];
	toftim[il-1]=p2tofrcl->time;//paddle corrected times (prev.calibration)
	toftimr[il-1]=0.5*(sidetm[0]+sidetm[1]);//paddle raw times
	if (kUseNewSlew) {
	  int index1=kTofFirstSidePos[il-1]+ib-1;
	  int index2=kTofFirstSidePos[il-1]+ib-1+kNtofb[il-1];
	  toftimd[il-1]=0.5*(sidetm[0]-sidetm[1])+(TofTzSlope[index1]/sqrt(tofsd1q[il-1])-TofTzSlope[index2]/sqrt(tofsd2q[il-1]));//corrected side times diff.
	}
	else {
	  toftimd[il-1]=0.5*(sidetm[0]-sidetm[1])+defTzslop*(1/sqrt(tofsd1q[il-1])-1/sqrt(tofsd2q[il-1]));//corrected side times diff.
	}
        tofedep[il-1]=p2tofrcl->edepa;//anode(mev)
	tofcool[il-1]=p2tofrcl->cool;//long.coo(from prev.calib)
	tofadca[il-1][0]=radca[0];//for ampl.cals
	tofadca[il-1][1]=radca[1];
	for(int ip=0;ip<3;ip++){//pmt dynodes
	  tofadcdr1[il-1][ip]=p2tofrcl->adcdr[0][ip];
	  tofadcdr2[il-1][ip]=p2tofrcl->adcdr[1][ip];
	}
      }//--->endof adca1/2>thr
    }//--->endof stat ok
  }//--->endof clust-loop
//cout<<" Nbrnl="<<tofnbrl[0]<<" "<<tofnbrl[1]<<" "<<tofnbrl[2]<<" "<<tofnbrl[3]<<endl;
//
//----->    Select events with enough layers with bars/layer=1 :
//
  Int_t tofngdlrs=0;
  for(ii=0;ii<4;ii++){
    if(tofnbrl[ii]==1)tofngdlrs+=1;
    else{
      tofnbrl[ii]=0;//reset non-empty, but bad( >1 fired bars) layers
      tofbrnl[ii]=-1;
    }  
  }
//cout<<" brnl="<<tofbrnl[0]<<" "<<tofbrnl[1]<<" "<<tofbrnl[2]<<" "<<tofbrnl[3]<<endl;
//
  bad=0;
  if(kTofStrictLPatt){
    if(tofngdlrs<4){
      bad=1;//require 4x1bar layers
    }
  }
  else{
    if(tofnbrl[0]==0 && kTofLref==1)bad=1; // always require 1st layer if it is ref-layer
    if(tofnbrl[1]==0 && kTofLref==2)bad=1; // always require 2nd layer if it is ref-layer
    if((tofnbrl[2]==0 || tofnbrl[3]==0) && kTofLref==2)bad=1; // always require 2 layer for bottom if 1st one is bad
    if(tofngdlrs<3)bad=1;//require at least 3 planes
  }
//cout<<"  Ngdlrs="<<tofngdlrs<<endl;
  if(bad==0)TOFRawClustOK=1;
//-------------
//
//                                 <--- check TOF-beta quality:
// 
  Bool_t TOFBetaOK(0);
  Float_t tofbet(0);
//
//  Int_t ibeta = Particle(pindex).iBeta();//used beta-index of 1st Part with trk-track    
//  BetaR BetaI=Beta(ibeta);//ref to ibeta elem. of Beta vector
  TofClusterR *ptofcl;
  Int_t ntofcls=NTofCluster();//total tof-clusters
//cout<<"   <--- ntofrc/ntofcl="<<ntofrcl<<" "<<ntofcls<<" pindex="<<pindex<<endl;
  BetaR *p2beta =  Particle(pindex).pBeta(); //pointer to TOF-beta, used by 1st Part. with trk-track
  Int_t id;
  Float_t xtof[4],ytof[4];//vs used clust.number
  Float_t etof[4]={0,0,0,0},ttof[4]={0,0,0,0};//vs layer #
  Float_t xtofh[4]={0,0,0,0};//vs layer #
  Float_t ytofh[4]={0,0,0,0};//vs layer #
  Float_t ztofh[4]={0,0,0,0};//vs layer #
  Int_t bltof[4]={0,0,0,0};//vs layer
  Int_t ntofp[4]={0,0,0,0};//vs layer
  Int_t bltofb[4]={0,0,0,0};//bar #(1:) with bad tof/trk matching(vs layer)
  Int_t nbetofc(0);//number of Beta-used tof-clusters
  Int_t ltof[4]={0,0,0,0},btof[4]={0,0,0,0};//vs used-cluster #
  Int_t betapatt(-1);
  TofRawClusterR * p2tofrc[4][2]={0,0,0,0,0,0,0,0};//pointers(il) to raw-clust members(max 2) of TofCluster
//cout<<"   p2beta="<<p2beta<<endl;
//
  if(p2beta>0){//<====  TOF-beta measurement found in particle
  RunPar::addsev(1);//<--passed TOF-beta presence check
//
//
  tofbet=p2beta->Beta;
  betapatt=p2beta->Pattern;
  nbetofc=p2beta->NTofCluster();//number of Beta-used tof-clusters
  if(nbetofc>4){
      cout<<"=====> Fatal error : NBetaUsedClusters>4 !!!"<<endl;
      cout<<" TofBet/Patt="<<tofbet<<" "<<betapatt<<" NclBetaUsed(<=4)="<<nbetofc<<" ntofcl="<<ntofcls<<endl;
      exit(1);
  }
//
  for(int i=0;i<nbetofc;i++){//loop over beta-used tof-clusters(<=4)

    xtof[i]=p2beta->pTofCluster(i)->Coo[0];
    ytof[i]=p2beta->pTofCluster(i)->Coo[1];
    ltof[i]=p2beta->pTofCluster(i)->Layer;//1:4
    btof[i]=p2beta->pTofCluster(i)->Bar;//1:8(10)
    bltof[ltof[i]-1]=btof[i];//bar(1:10) vs layer
    ntofp[ltof[i]-1]=p2beta->pTofCluster(i)->NTofRawCluster();//# cluster members(paddles)
    etof[ltof[i]-1]=p2beta->pTofCluster(i)->Edep;
    ttof[ltof[i]-1]=p2beta->pTofCluster(i)->Time;
    xtofh[ltof[i]-1]=p2beta->pTofCluster(i)->Coo[0];
    ytofh[ltof[i]-1]=p2beta->pTofCluster(i)->Coo[1];
    ztofh[ltof[i]-1]=p2beta->pTofCluster(i)->Coo[2];
    for(int j=0;j<ntofp[ltof[i]-1];j++)p2tofrc[ltof[i]-1][j]=p2beta->pTofCluster(i)
                                                                   ->pTofRawCluster(j);//RawCl-pointers
//    tofh1->Fill(btof[i]+(ltof[i]-1)*10,1);
//    tofh3->Fill(ntofp[i],1);
  }
//
  Bool_t fedvol[4]={1,1,1,1};
  if(bltof[0]>0)fedvol[0]=((bltof[0]>=1 && bltof[0]<=kNtofb[0]));//selection of good counters
  if(bltof[1]>0)fedvol[1]=((bltof[1]>=1 && bltof[1]<=kNtofb[1]));
  if(bltof[2]>0)fedvol[2]=((bltof[2]>=1 && bltof[2]<=kNtofb[2]));
  if(bltof[3]>0)fedvol[3]=((bltof[3]>=1 && bltof[3]<=kNtofb[3]));
  cutf[1]=(fedvol[0]&&fedvol[1]&&fedvol[2]&&fedvol[3]);//true if all bars are good
//
  TofAll4L=(ntofp[0]>0 && ntofp[1]>0 && ntofp[2]>0 && ntofp[3]>0);
  Tof1P4L=(ntofp[0]==1 && ntofp[1]==1 && ntofp[2]==1 && ntofp[3]==1);//1memb-clusters
//
  if(cutf[1]){//<---- TOF-fed.volume check
//
    RunPar::addsev(2);//<--passed "Tof fed.vol" check
//
    Bool_t innvol[4]={1,1,1,1};
    if(bltof[0]>0)innvol[0]=((bltof[0]>1 && bltof[0]<kNtofb[0]));//select center area(without trapez.count)
    if(bltof[1]>0)innvol[1]=((bltof[1]>1 && bltof[1]<kNtofb[1]));
    if(bltof[2]>0)innvol[2]=((bltof[2]>1 && bltof[2]<kNtofb[2]));
    if(bltof[3]>0)innvol[3]=((bltof[3]>1 && bltof[3]<kNtofb[3]));
    cutf[2]=(innvol[0]&&innvol[1]&&innvol[2]&&innvol[3]);//true if all bars are in center area
//  
    amsh3->Fill(p2beta->Beta,1);
    amsh4->Fill(p2beta->Chi2,1);
    amsh5->Fill(p2beta->Chi2S,1);
//    amsh6->Fill(p2beta->Pattern,1);
    tofh2->Fill(ntofcls,1);
//
    if(fabs(p2beta->Beta) < RunPar::GetGenc(1)
	                 && p2beta->Chi2 < RunPar::GetGenc(2)
	                 && p2beta->Chi2S < RunPar::GetGenc(3)
//	                 && betapatt <= RunPar::GetGenc(4)
//		         && ntofcls <= RunPar::GetGenc(5)
	                                         ){//<---- TOF-beta quality check
//
      TOFBetaOK=1;
      RunPar::addsev(3);//<--passed "beta quality" test
//
    }// --- endof "TOF-beta quality check --->
  }// --- endof "TOF fed.vol." check --->
  }// --- endof "TOF in particle" check --->
//cout<<"   beta check passed, tofok="<<TOFBetaOK<<endl;
//---------------------------------------------------------------
//                                <--- check TRK-track quality:
//
  Bool_t TRKtrOK(0);
  AMSPoint TrkP0(0,0,0);//to store point,dir at z=kTrdBotZ
  AMSDir TrkD0(0,0,0);
  Float_t TrkXAng(0),TrkYAng(0);
// 
  Int_t ialgor[9][2]={-1,-1,-1,-1,-1,-1,-1,-1,-1,
                      -1,-1,-1,-1,-1,-1,-1,-1,-1};//indexes(fitcodes) of fit methods[9], yes/noMS[2]
  TrTrackPar algor,thalf,tpext;//refs to TRK-track params.objects
  Int_t ithalf[9][2]={-1,-1,-1,-1,-1,-1,-1,-1,-1,
                      -1,-1,-1,-1,-1,-1,-1,-1,-1};//top/bot_half:indexes to fit methods[9]
  Int_t itpext[9][2]={-1,-1,-1,-1,-1,-1,-1,-1,-1,
                      -1,-1,-1,-1,-1,-1,-1,-1,-1};//drop_ext_pl/2+2:ref.index to fit methods[9]
  double MRigid[9]={-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999};//max 9 methodes rigid.
  Int_t LBitPatt[9]={0,0,0,0,0,0,0,0,0};
  double MErrRinv[9];//max5 methods err on 1/Rig
  double MChisq[9][2]={-1,-1,-1,-1,-1,-1,-1,-1,-1
                       -1,-1,-1,-1,-1,-1,-1,-1,-1};//9meth*(wMS/nMS)
  double MRigidH[9][2]={-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,
                        -9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999,-9999};  
  Bool_t MFitDone[9]={0,0,0,0,0,0,0,0,0};
  Bool_t MFitDoneH[9][2]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//for halfs 
  Float_t trkrig(0),trkrige(0),rerig(0),hrigass(-999),trkch2(-1),trkch2ms(-1);
  Float_t trkthe(0),trkphi(0);
  Int_t trkpatx(0),trkpaty(0);
  Int_t trkaddr[2]={0,0};
  Float_t trkhrig[2]={0.,0.};//2 halves rigs
  Bool_t TrkXOK(0),TrkAfOK(0),TrkIsGood(0);
  Bool_t AddrOK(kTRUE);
//
  itrktr = Particle(pindex).iTrTrack();//TRKtrack index used by i-th Part(-1 if missing) 
//
  if(itrktr>=0){//<---- TRKtrack in Particle presence check
    RunPar::addsev(90);//<--found TRKtrack in part
    TrTrackR trktr=TrTrack(itrktr);//ref to TrkTrack object used by particle
    TrTrackR *p2trktr = Particle(pindex).pTrTrack();//pointer to TRK-track used by Part.
//    TrTrackR *p2trktr = pTrTrack(itrktr);//the same
//  
//---> create refs to track pars.objects:
//
//
      for(int i=0;i<kTrkFitMInUse;i++){//<---fit methode loop(VC/Geane/GeaneK/JA/Chik/ChikF/Linear/Circle/Simple)
      if(i==2 || i==5 || i==6)continue;//skip not developed/needed methodes
      for(int j=0;j<2;j++){//<---with/no MS (+10 ->MS off) || top/bot_halfs || drop_ext_pl/2+2
        ialgor[i][j]=trktr.iTrTrackPar(10*j+i+1,0,0);//w/nMS,0->all_hits, 1->refit_ifnotexist(no refit->0)
        ithalf[i][j]=trktr.iTrTrackPar(i+1,j+1,0);//wMS,top/bot_halfs,refit_ifnotexist
        itpext[i][j]=trktr.iTrTrackPar(i+1,j+3,0);//wMS,drop_extpl/2+2,refit_ifnotexist
	if(ialgor[i][j] >= 0){
          algor=trktr.gTrTrackPar(ialgor[i][j]);//ref.to TrTrackPar object
          MChisq[i][j]=algor.Chisq;//w/n MS
	  if(j==0){
            MRigid[i]=algor.Rigidity;//only withMS
            MErrRinv[i]=algor.ErrRinv;//only withMS
            MFitDone[i]=algor.FitDone;//total Rig
//	    LBitPatt[i]=algor.HitBits;//Used Layers Bit Pattern.
	  }
        }

	if(ithalf[i][j]>=0){  
//cout<<"   -> half="<<j+1<<endl;
          thalf=trktr.gTrTrackPar(ithalf[i][j]);
          MRigidH[i][j]=thalf.Rigidity;//half Rigs
          MFitDoneH[i][j]=thalf.FitDone;//done for half Rigids
//cout<<"     HfitDone="<<MFitDoneH[i][j]<<"  Hrig="<<MRigidH[i][j]<<endl;
        }
//
        if(itpext[i][j]>=0){
	  tpext=trktr.gTrTrackPar(itpext[i][j]);
        }
      }//---> endof half(ms/noms/pat) loop
    }//--->endof methodes loop
//------------------------------------------------------   
//
//---> fill some PG track variables for default methode:
//
    if(MFitDone[kTrkFitMet]){
      trkrig=MRigid[kTrkFitMet];
      trkrige=MErrRinv[kTrkFitMet];
      rerig=trkrige*fabs(trkrig);//abs. dR/R
      trkch2=MChisq[kTrkFitMet][0];//wMS
      trkch2ms=MChisq[kTrkFitMet][1];//noMS
      if(MFitDoneH[kTrkFitMet][0] && MFitDoneH[kTrkFitMet][1]){
        TrkAfOK=1;//advanced(2halfs) fit done
        trkhrig[0]=MRigidH[kTrkFitMet][0];
        trkhrig[1]=MRigidH[kTrkFitMet][1];
        if((trkhrig[0]+trkhrig[1])!=0)hrigass=(trkhrig[0]-trkhrig[1])/(trkhrig[0]+trkhrig[1]);
        if(hrigass>1)hrigass=0.9999;
        if(hrigass<-1)hrigass=-1;
      }
    }
//
//    trkrig=trktr.GetRigidity(TrTrackR::kChoutko);
    trkpatx=trktr.GetPatternX();  
    trkpaty=trktr.GetPatternY();
    TrkXOK=(trkpatx>0);//tempor (? ask Paolo)
//cout<<"  TrkPattX/Y="<<trkpatx<<" "<<trkpaty<<endl;
    if(TrkXOK)RunPar::addsev(91);//<-- counts "true X"
    if(AddrOK)RunPar::addsev(92);//<-- counts "AddrOK"
    if(TrkAfOK)RunPar::addsev(93);//<-- counts "AdvFitDone"
    TrkIsGood=(TrkXOK && TrkAfOK);
    if(TrkIsGood)RunPar::addsev(94);
//
//------> for later TRK-TRD matching check:
//      
    Double_t pathl=p2trktr->Interpolate(kTrdBotZ,TrkP0,TrkD0,0);//interpolate track to plane kTrdBotZ
    TrkXAng=atan(TrkD0[0]/TrkD0[2]);//angle in ZX-proj wrt Zaxe
    TrkYAng=atan(TrkD0[1]/TrkD0[2]);//angle in Zy-proj wrt Zaxe
//cout<<"---> TrkP0="<<TrkP0[0]<<" "<<TrkP0[1]<<" "<<TrkP0[2]<<" PathL="<<pathl<<endl;
//cout<<"     TrkD0="<<TrkD0[0]<<" "<<TrkD0[1]<<" "<<TrkD0[2]<<endl;
//cout<<"     TrkXAng/YAng="<<TrkXAng<<" "<<TrkYAng<<endl;
    if(TrkD0[2]>0){//always invert direction to top->bot
      TrkD0[0]=-TrkD0[0];
      TrkD0[1]=-TrkD0[1];
      TrkD0[2]=-TrkD0[2];
    }
//------  
    if(TrkXOK){//<---- true X
      RunPar::addsev(95);//<--passed "trueX" test
      if(TrkAfOK){//
        RunPar::addsev(96);//<--passed "AdvFitOK" test
//
        trkh1->Fill(trkch2,1);
        trkh3->Fill(trkch2ms,1);
        trkh4->Fill(hrigass,1);
        if(
	   trkch2<RunPar::GetGenc(6)
//            && trkch2ms<RunPar::GetGenc(8)
	    && AddrOK
	                                  ){//<---- chi2's check
//
          trkh5->Fill(trkrig,1);
          trkh9->Fill(rerig,1);//dR/R
//
          TRKtrOK=1;
          RunPar::addsev(97);//<--passed "Chi2's" check
//
        }//--- endof "chi2's" check --->
      }//--- endof "AdvFitOK" check --->
    }//--- endof "trueX" check --->
  }// --- endof "TRKtr in Particle" check --->
//cout<<"   trkcheck passed, trkok="<<TRKtrOK<<endl;
//--------------------------------------------------------
//
//                      <---- Check TOF-TRKtrack matching:
//
  Bool_t TOFTRKmatchOK(kFALSE);
  Bool_t TOFTRKTmatchOK[4]={kFALSE,kFALSE,kFALSE,kFALSE};
  Bool_t TOFTRKLmatchOK[4]={kFALSE,kFALSE,kFALSE,kFALSE};
  Int_t NTofLmatch(0),NTofTmatch(0);
  Float_t dclg[4]={999.,999.,999.,999.};//dcoo-long vs layer
  Float_t dctr[4]={999.,999.,999.,999.};//dcoo-tran vs layer
  Float_t longimp[4]={0,0,0,0};
  Bool_t MatGoog4Tdlv(kFALSE);
  Bool_t MatGoog4Tzsl(kFALSE);
  Int_t Ntoftrmatch(0);
//
//cout<<"  TofBetOK/TrkOK="<<TOFBetaOK<<" "<<TRKtrOK<<endl;
  if(TOFBetaOK && TRKtrOK){//<---- TOFbeta/TRKtr presence check
    RunPar::addsev(10);//<-- count matching tests
// 
//---> calc/store long/trans mismatch(based on TofClust(!) used in Beta) :
//
    for(il=0;il<4;il++){//<--- layer loop
      ib=bltof[il];//1:8(10), =0 if missing
      if(ib==0 || ntofp[il]>1)continue;//skip missing or 2-memb clusters
      dx=xtofh[il]-tofcro[il][0];
      dy=ytofh[il]-tofcro[il][1];
      if(il==0 || il==3){//x=long.coo
        dclg[il]=dx;
        dctr[il]=dy;
	longimp[il]=tofcro[il][0];
        if(il==0){
	  tofp1->Fill(ib,dx,1.);
	  tofh30->Fill(dx,1.);
	  tofh34->Fill(dy,1.);
	}
        if(il==3){
	  tofp4->Fill(ib,dx,1.);
	  tofh33->Fill(dx,1.);
	  tofh37->Fill(dy,1.);
	}
      }
      else{//y=long.coo
        dclg[il]=dy;
        dctr[il]=dx;
	longimp[il]=tofcro[il][1];
        if(il==1){
	  tofp2->Fill(ib,dy,1.);
	  tofh31->Fill(dy,1.);
	  tofh35->Fill(dx,1.);
	}
        if(il==2){
	  tofp3->Fill(ib,dy,1.);
	  tofh32->Fill(dy,1.);
	  tofh36->Fill(dx,1.);
	}
      }
    }
//
//---> set mismatchOK flags:
//
    Float_t toftc,toflc;
    cutf[6]=kTRUE;// good tof-track matching(both long and tran in all fired layers) 
    for(il=0;il<4;il++){//<--- layer loop
      ib=bltof[il];//1:8(10), =0 if missing
      if(ib==0 || ntofp[il]>1)continue;//skip missing or 2-memb clusters
      if(ib>1 && ib<kNtofb[il]){//<--- set dx/dy-cuts for center
        toflc=RunPar::GetGenc(9);
	if(kTofLcRelax==1)toflc*=3;
        toftc=kTofIntBWid/2-0.3;
      }
      else{//<---... for outer 
        toflc=1.5*RunPar::GetGenc(9);
	if(kTofLcRelax==1)toflc*=3;
        toftc=kTofExtBWid[il]/2-1.;
      }
//
      if(fabs(dclg[il])<toflc && fabs(longimp[il])<TofCalib::brlen(il,ib-1)/2){// trk.imp.p have to be within bar length 
	TOFTRKLmatchOK[il]=kTRUE;
	NTofLmatch+=1;
      }
      else{
        cutf[6]=kFALSE;//bad long. tof/trk-matching
        bltofb[il]=ib;
      }
      if(fabs(dctr[il])<toftc){
	TOFTRKTmatchOK[il]=kTRUE;
	NTofTmatch+=1;
      }
      else{
        cutf[6]=kFALSE;//bad tran. tof/trk-matching
        bltofb[il]=ib;
      }
      if(fabs(dclg[il])<toflc && fabs(dctr[il])<toftc)Ntoftrmatch+=1;//good match in both proj.
      if(bltofb[il]==0)tofh1a->Fill(ib+il*10,1);//map of 2D-matched paddles
    }//--->endof layer loop
    
    if(cutf[6]){//<---- Good Tof-TrkTrack matching in ALL!!! beta-used tof-layers  
//
      TOFTRKmatchOK=1;
      RunPar::addsev(11);//<--passed "Tof-TrkTrack matching" test
//
    }//--- endof "good matching" check --->
  }//--- endof "TOFbeta/TRKtr-presence" check --->
//
  if(kTofStrictLPatt){
    MatGoog4Tdlv=(NTofTmatch==4);
  }
  else{
    MatGoog4Tdlv=(NTofTmatch>=3);
  }
//
//cout<<"  TOFTRKmatchOK="<<TOFTRKmatchOK<<" badLayers="<<bltofb[0]<<" "<<bltofb[1]<<" "<<bltofb[2]<<" "<<bltofb[3]<<endl;
//cout<<" TransMatch="<<TOFTRKTmatchOK[0]<<" "<<TOFTRKTmatchOK[1]<<" "<<TOFTRKTmatchOK[2]<<" "<<TOFTRKTmatchOK[3]<<endl;
//cout<<" LongMatch="<<TOFTRKLmatchOK[0]<<" "<<TOFTRKLmatchOK[1]<<" "<<TOFTRKLmatchOK[2]<<" "<<TOFTRKLmatchOK[3]<<endl;
//cout<<"  GoogMatch4Tdlv="<<MatGoog4Tdlv<<endl;
//-----------------------------------------------------------
//
//                       <---- Create some TOF-Edep TRK_pass/beta-normalized arrays :
//
  Int_t tofmsk[4]={1,1,1,1};//to keep layer mask after removing highest layer
  Int_t lemx(0),ntrtof(0);
  Float_t edmx(0),TofEdNorm[4]={0.,0.,0.,0.};//TOF Edep normalized to passlength and beta
  Float_t tofedpn[4]={0.,0.,0.,0.};//TOF Edep norm to pass
  Float_t tofraw[2]={0,0};
  Float_t tofcs[2]={0,0};//Tof/track cross. cos12,cos34
  Float_t betctof,betctrk,etrtof(0),absbet,clong[4]={0,0,0,0};
  Float_t etrtof1(0);
  absbet=fabs(tofbet);
  Float_t betptof,betptrk;
// ===> Beta-corr for general analysis(Z~1)
  
  betptof=kBpowTof[1];//Beta-power in dE/dX 1/beta**pow beta-dependence (as for prot)  
  betptrk=kBpowTrk[1];//...                                             (as for prot)  
  if(absbet<kBeta0){
    betctof=pow(absbet,betptof)
                       /pow(Float_t(kBeta0),betptof);//TOF beta-corr for general analysis
  }
  else{
     betctof=1;
  }
  if(absbet<kBeta0tk){
    betctrk=pow(absbet,betptrk)
                       /pow(Float_t(kBeta0),betptrk);//TRK beta-corr for general analysis
  }
  else{
     betctrk=1;
  }
  Bool_t TOFTRKnormOK(0);
//
  if(nbetofc>=3 && Ntoftrmatch>=3){
//---> calc. trk-crossing cos with top/bot TOF's
    dx=tofcro[0][0]-tofcro[1][0];//tofcro's exist for all 4 layers by definition
    dy=tofcro[0][1]-tofcro[1][1];
    dz=tofcro[0][2]-tofcro[1][2];
    tofcs[0]=dz/sqrt(dx*dx+dy*dy+dz*dz);//L12 cos
    dx=tofcro[2][0]-tofcro[3][0];
    dy=tofcro[2][1]-tofcro[3][1];
    dz=tofcro[2][2]-tofcro[3][2];
    tofcs[1]=dz/sqrt(dx*dx+dy*dy+dz*dz);//L34 cos
//---> create some tof-EdepNorm arrays
    for(int il=0;il<4;il++){//create some tof-EdepNorm arrays
      int itb=il/2;
      if(bltof[il]>0){
        tofedpn[il]=etof[il]*tofcs[itb];//norm to norm. inc.
        TofEdNorm[il]=etof[il]*tofcs[itb]*betctof;//norm to norm.inc. and MIP area
        tofraw[0]=tofcs[itb]*betctof*(p2tofrc[il][0]->adca[0]);//s1-adc, norm.inc, mip area
        tofraw[1]=tofcs[itb]*betctof*(p2tofrc[il][0]->adca[1]);//s2-adc, norm.inc, mip area
        ib=bltof[il];//1:10
        id=100*(il+1)+ib;//tof id (LBB)
        if(il==0 || il==3)clong[il]=tofcro[il][0];
        else clong[il]=tofcro[il][1];
        if(TofEdNorm[il]>edmx){
          edmx=TofEdNorm[il];
	  lemx=il;
        }
      }
      else tofmsk[il]=0;
    }
    if(edmx>0){
      tofmsk[lemx]=0;//remove highest layer for trunc.mean calc.(-1 procedure)
    }
//---> fill some hist:
    for(int il=0;il<4;il++){//<---
      ib=bltof[il];//1:10
//      tofp11->Fill(absbet,tofedpn[il],1.);//Edep(PassNorm) vs Beta profile
        if(tofmsk[il]==1){
          ntrtof+=1;
          etrtof1+=tofedpn[il];
        }
    }//--->
    etrtof1/=ntrtof;//It is TOFTruncMean(-1) (3of4,2of3,1of2 mixt)
//---
    if(ntrtof==3){//make 2of4 (to use in beta-fit procedure only)
      edmx=0;
      for(int il=0;il<4;il++){//<---
        if(tofmsk[il]>0 && TofEdNorm[il]>edmx){
          edmx=TofEdNorm[il];
	  lemx=il;
        }
      }
      if(edmx>0){
        tofmsk[lemx]=0;//remove highest layer for trunc.mean calc.(for beta-fit)
      }
      ntrtof=0;
      etrtof=0;
      for(int il=0;il<4;il++){//<---
        if(tofmsk[il]==1){
          ntrtof+=1;
          etrtof+=tofedpn[il];
        }
      }
      etrtof/=ntrtof;
    }
//---
    tofh7->Fill(etrtof*betctof,1);//trunk.mean(2of4)
//
    TOFTRKnormOK=1;
    RunPar::addsev(20);//<--good TOF/TRK-normalization
  }
//-------------------------------------------------------------------------------
//                                <---- check ANTI-counters activity:
//
  Bool_t ANTIActive(0);
//
  RunPar::addsev(12);//<--- Entries to ANTIActive-test
  Float_t Eantit(0),eanti;
  Int_t nasects=NAntiCluster();
  Int_t nrws=NAntiRawSide();
//cout<<"   found ANTISectors="<<nasects<<endl;
  Int_t nasthr(0);
  anth1->Fill(nasects,1);
  AntiClusterR *p;
//
  Int_t sect;
  for(int is=0;is<nasects;is++){
    eanti=pAntiCluster(is)->Edep;
    sect=pAntiCluster(is)->Sector;
    anth3->Fill(eanti,1);
    Eantit+=eanti;
    if(eanti>RunPar::GetGenc(11)){
      anth4->Fill(sect,1);
      nasthr+=1;
    }
  }
  if(Eantit>0)anth2->Fill(Eantit,1);
  anth5->Fill(nasthr,1);
//cout<<"    Eantit="<<Eantit<<"   above thrsh.sectors="<<nasthr<<endl;
//
  if(nasthr>RunPar::GetGenc(12)){//<--- Found Anti-activity(NactiveSects>1)
//
    ANTIActive=1;
    RunPar::addsev(13);//<--passed "Anti-activity" test
  }//--- endof "ANTI-active" test --->
//cout<<" ANTIActive="<<ANTIActive<<endl;
//
//=======================================> check TRDtrack:
//
  Bool_t TRDtrOK(0);
  Bool_t TrkTrdMatchOK(0);
//
  Float_t dphi(0),dthe(0);
  Float_t trdv[3],trdext[3];
  Float_t trdcl1e[25]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  Int_t ntrdcl(0),ntrdcl1(0),ntrdc,ltrdcl,ntrdclhe(0);
  Float_t etrdcl(0);
  Int_t ntrdhecls(0);//trd hi-energy clusters
  Float_t cledep(0);
  Float_t trdllh(0);
  Float_t TrkTrdCos(1);//cos between TRD/TRK at plane kTrdBotZ
  Float_t TrkTrdDx(0),TrkTrdDy(0);//dx/dy ......
  Float_t TrkTrdDax(0),TrkTrdDay(0);
  Float_t TrdXAng(0),TrdYAng(0);
  TrdSegmentR * p2trdsg;
  TrdClusterR * p2trdcl;
  TrdRawHitR *p2trdheh;//pointer (from cluster) to trd-hit with highest ampl
//
//cout<<"   check TRD..."<<endl;
  Int_t trdrhits=nTrdRawHit();
  trdh12->Fill(trdrhits,1);
  Int_t ntrdcls=nTrdCluster();
  trdh14->Fill(ntrdcls,1);
  if(TRDtrPart){//<---- particle contains TRD-track
    RunPar::addsev(14);//<--- count Part. with TRD-track
    TrdTrackR *p2trdtr = Particle(pindex).pTrdTrack();//pointer to part-used TRD-track
    
    trdllh=Particle(pindex).TRDHLikelihood;
    UInt_t trdsta=p2trdtr->Status;
    
    Float_t trdcoo[3];
    for(int i=0;i<3;i++)trdcoo[i]=p2trdtr->Coo[i];
    Float_t trdchi2=p2trdtr->Chi2;
    Float_t trdthe=p2trdtr->Theta;//(rads)
    Float_t trdphi=p2trdtr->Phi;
    trdv[0]=sin(trdthe)*cos(trdphi);//TRD direction Cos's
    trdv[1]=sin(trdthe)*sin(trdphi);
    trdv[2]=cos(trdthe);
    TrdXAng=atan(trdv[0]/trdv[2]);//angle in ZX-proj vrt Zaxe
    TrdYAng=atan(trdv[1]/trdv[2]);//angle in ZX-proj vrt Zaxe
    trdext[0]=trdcoo[0]+(kTrdBotZ-trdcoo[2])*trdv[0]/trdv[2];//extrapolation to z=kTrdBotZ plane
    trdext[1]=trdcoo[1]+(kTrdBotZ-trdcoo[2])*trdv[1]/trdv[2];
    trdext[2]=kTrdBotZ;
//    if(evnum==44 || evnum==104){
//cout<<"---> TrdP="<<trdcoo[0]<<" "<<trdcoo[1]<<" "<<trdcoo[2]<<endl;
//cout<<"     TrdPr="<<trdext[0]<<" "<<trdext[1]<<" "<<trdext[2]<<endl;
//cout<<"     TrdV="<<trdv[0]<<" "<<trdv[1]<<" "<<trdv[2]<<endl;
//cout<<"     TrdXAng/YAng="<<TrdXAng<<" "<<TrdYAng<<endl;
//cout<<"     the/phi="<<trdthe<<" "<<trdphi<<endl;
//} 
    
    if(trdphi<0)trdphi=6.28318+trdphi;// TRD phi was in +-pi range !!!
    Int_t ntrdseg=p2trdtr->NTrdSegment();
//cout<<"   found TRD-track:chi2="<<trdchi2<<" nsenments="<<ntrdseg<<endl;
//
//--- look at clusters:
//
    for(int ic=0;ic<ntrdcls;ic++){//trd clusters loop
      p2trdcl=pTrdCluster(ic);
      cledep=p2trdcl->EDep;//clust.Edep(kev);
      trdh16->Fill(cledep,1);
      if(cledep>5)ntrdhecls+=1;//total(comp.with trdh13)
    }
    trdh15->Fill(ntrdhecls,1);
//
//--- look at segments:
//
    trdh1->Fill(ntrdseg,1);
    if(ntrdseg >= RunPar::GetGenc(13)){// TrdTrack Nsegm
      trdh2->Fill(trdchi2,1);
      RunPar::addsev(15);//<-- Nsegn OK
      if(trdchi2 < RunPar::GetGenc(14)){//<--- Good TrdTrack chi2 
        RunPar::addsev(16);//<-- chi2 ok
//
        trdh5->Fill(trdllh,1);
        for(int is=0;is<ntrdseg;is++){//segment loop
          p2trdsg=p2trdtr->pTrdSegment(is);
          if(p2trdsg>0){
            ntrdc=p2trdsg->NTrdCluster();//trd-clusters per segment
            trdh6->Fill(ntrdc,1);
            ntrdcl+=ntrdc;
            for(int ic=0;ic<ntrdc;ic++){//cluster loop
              p2trdcl=pTrdCluster(ic);
	      ltrdcl=p2trdcl->Layer;//clust.layer(0->bot;19->top !!!)
	      etrdcl=p2trdcl->EDep;//kev
	      if(etrdcl>5)ntrdclhe+=1;
	      Int_t mult=p2trdcl->Multip;
	      Int_t hmult=p2trdcl->HMultip;//mult.with E>5.9kev
              trdh7->Fill(mult,1);
              trdh8->Fill(hmult,1);
	      if(mult==1){
	        if(ntrdcl1<25)trdcl1e[ntrdcl1]=etrdcl;//store ClustEnergies when mult=1
	        trdh9->Fill(etrdcl,1);
	        ntrdcl1+=1;
	      }
            }
          }
        }
        trdh10->Fill(ntrdcl,1);
        trdh11->Fill(ntrdcl1,1);
        trdh13->Fill(ntrdclhe,1);//TotalHiQuality(comp.with trdh15)
        TRDtrOK=1;
//
      }//--- endof "Chi2" check --->
    }//--- endof "Nsegm" check --->
//--
  }//--- endof TRD-track presence check --->
//
  if(TRDtrOK)RunPar::addsev(17);//<-- count TRD-track quality OK
//cout<<"  TRDtrOK="<<TRDtrOK<<endl;
//
//====================================> check Trk/Trd matching:
//
  if(TRKtrOK && TRDtrOK){//TRK&TRD-tracks OK
    RunPar::addsev(18);
//cout<<"---> TrkP0="<<TrkP0[0]<<" "<<TrkP0[1]<<" "<<TrkP0[2]<<endl;
//cout<<"     TrkD0="<<TrkD0[0]<<" "<<TrkD0[1]<<" "<<TrkD0[2]<<endl;
//cout<<"     TrkXAng/YAng="<<TrkXAng<<" "<<TrkYAng<<endl;
//cout<<"---> TrdExtr="<<trdext[0]<<" "<<trdext[1]<<" "<<trdext[2]<<endl;
//cout<<"     TrdV="<<trdv[0]<<" "<<trdv[1]<<" "<<trdv[2]<<endl;
//cout<<"     TrdXAng/YAng="<<TrdXAng<<" "<<TrdYAng<<endl;
    TrkTrdCos=TrkD0[0]*trdv[0]+TrkD0[1]*trdv[1]+TrkD0[2]*trdv[2];//cos
    TrkTrdDx=TrkP0[0]-trdext[0];//dx
    TrkTrdDy=TrkP0[1]-trdext[1];//dy
    TrkTrdDax=TrkXAng-TrdXAng;//x-angle diff
    TrkTrdDay=TrkYAng-TrdYAng;//y-angle diff
//if(fabs(TrkTrdDy)>6){
//cout<<"--->Big dY, run/ev="<<runn<<" "<<evnum<<endl;
//cout<<"---> dx/dy="<<TrkTrdDx<<" "<<TrkTrdDy<<endl;
//cout<<"---> adx/ady="<<TrkTrdDax<<" "<<TrkTrdDay<<endl;
//}
//      dthe=trkthe-trdthe;
//      dphi=trkphi-trdphi;
//      if(fabs(dphi)>3.14159){
//        if(dphi>0)dphi=6.28318-dphi;
//        if(dphi<0)dphi=6.28318+dphi;
//      }
//    trdh3->Fill(dthe,1);
//    trdh4->Fill(dphi,1);
    amsh21->Fill(TrkTrdDx,1);
    amsh22->Fill(TrkTrdDy,1);
    amsh23->Fill(TrkTrdDax,1);
    amsh24->Fill(TrkTrdDay,1);
    if(fabs(TrkTrdDx) < RunPar::GetGenc(15) &&  fabs(TrkTrdDy) < RunPar::GetGenc(16)
       && fabs(TrkTrdDax) < RunPar::GetGenc(23) && fabs(TrkTrdDay) < RunPar::GetGenc(24)
  		                                                                        ){//<--- 
      TrkTrdMatchOK=1;//when no good TRKtr
    }//--- endof good TRK/TRD-track matching
  }//--- endof "TRK-track ok" --->
  if(TrkTrdMatchOK)RunPar::addsev(19);//<-- count TRK/TRD-track match OK
//
//
// ============================================================> Start here phys.proc. selection cuts:
//
//==================> Tof Calibration:
//
  Bool_t AccOK4A2D=(nasthr<7);
  Bool_t AccOK4TG=(nasthr<2);
  Bool_t DownPart=(tofbet>0.2);
  Bool_t PrCharge=(trkcha==1 
                   && tofcha==1 
		   && (trdcha==0 || trdcha==1) 
		   && (riccha==0 || riccha==1) 
		                            );
  Bool_t HeCharge=(trkcha==2 
                   && tofcha==2 
		   && (trdcha==0 || trdcha==2) 
		   && (riccha==0 || riccha==2) 
		                            );
  Float_t BetaImpl(0),bhcutpr,bhcuthe,blcutpr,blcuthe;
//----> few common rates:
  RunPar::addsev(30);
  if(!TRKtrOK)return;
  RunPar::addsev(31);
  if(!TOFBetaOK)return;
  RunPar::addsev(32);
  if(!TOFRawClustOK)return;
  RunPar::addsev(33);
  if(AccOK4A2D)RunPar::addsev(34);
  if(AccOK4TG)RunPar::addsev(35);
  if(PrCharge)RunPar::addsev(36);
  if(HeCharge)RunPar::addsev(37);
//
  Bool_t TopProt(0);
  Bool_t TopHelium(0);
  if(PrCharge && tofbet>0 && prtmom>0){
    BetaImpl=fabs(prtmom)/sqrt(prtmom*prtmom+kProtMass*kProtMass);
    bhcutpr=1.2*BetaImpl+0.05;
    blcutpr=0.8*BetaImpl-0.05;
    TopProt=(tofbet<bhcutpr && tofbet>blcutpr);
    if(TopProt){
      tftzh11->Fill(BetaImpl,tofbet,1.);
      tftzh13->Fill(prtmom,tofbet,1.);
      tftzh15->Fill(prtmom,tofbet,1.);
      tftzh17->Fill(BetaImpl,1-tofbet/BetaImpl,1.);
      tftzh19->Fill(prtmom,1-tofbet/BetaImpl,1.);
    }
  }
  if(HeCharge && tofbet>0 && prtmom>0){
    BetaImpl=fabs(prtmom)/sqrt(prtmom*prtmom+kHeliumMass*kHeliumMass);
    bhcuthe=1.2*BetaImpl+0.05;
    blcuthe=0.8*BetaImpl-0.05;
    TopHelium=(tofbet<bhcuthe && tofbet>blcuthe);
    if(TopHelium){
      tftzh12->Fill(BetaImpl,tofbet,1.);
      tftzh14->Fill(prtmom,tofbet,1.);
      tftzh16->Fill(prtmom,tofbet,1.);
      tftzh18->Fill(BetaImpl,1-tofbet/BetaImpl,1.);
      tftzh20->Fill(prtmom,1-tofbet/BetaImpl,1.);
    }
  }
//  
//---------------> Tdlv-calib:
//
  Float_t td,coo;
  if(kTofCalibType[0]>0){//<--- Do Tdlv-calib
    RunPar::addsev(40);
    if(TOFRawClustOK){//<--- 1rawcl/layer
      RunPar::addsev(41);
      if(((!kUseHe4Calib && TopProt)||(kUseHe4Calib && TopHelium))
                 && prtmom>kTdlvMomLims[kSpaceCalib][0]
		                                        ){//<--- chargeOK(=1/2,positive =prot/He from top)
        RunPar::addsev(42);
        if(TOFBetaOK && (tofbet>0.7 && tofbet<1.15)){//<--- prev.cal beta ok
          RunPar::addsev(43);
	  if(MatGoog4Tdlv){//<--- good tof/trk match for Tdlv				    
            RunPar::addsev(44);
            for(il=0;il<kNtofl;il++){
              ib=tofbrnl[il];//rawclus (-1 means bad)
              if(ib>=0 && TOFTRKTmatchOK[il]){//<--- combine info on RawClust + Match(Clust-based)
	        td=toftimd[il];
	        coo=longimp[il];
//cout<<"  Fill il/ib="<<il+1<<" "<<ib+1<<"  coo="<<coo<<" td="<<td<<endl;
	        TofCalib::filltd(il, ib, td, coo);
	      }
            }//--->endof layer loop
	  }//--->endof "good tof/trk match for Tdlv"
        }//--->endof "prev.cal beta range ok
      }//--->endof "chargeOK"
    }//--->endof "RawClust ok:   
  }//--->endof "Tdlv-calib"
//
//---------------> Tzslw-calib:
//
//---> define ref.beta for TZSL-calib and check mom-range:
  Float_t pmas(0.938),mumas(0.1057);
  Float_t cvel(29.979);
  Float_t pmom,bet,chi2,betm,beta,betgam;
  Bool_t TzslMomOK(kFALSE);
  Bool_t AmplMomOK(kFALSE);
  Bool_t MomMeasExist(kFALSE);
  Float_t imass;//implied mass
  Double_t trlen[kNtofl-1]={0,0,0};
  Double_t ram[kNtofl],ramm[kNtofl];
  Double_t dum[kNtofl-1],tld[kNtofl-1],tdm[kNtofl-1];
  Double_t tdrm[kNtofl-1];
  Int_t brnlw[kNtofl];
  Float_t trlr0;
  Int_t n2dml(0);
  Int_t TzslFinLPat[4]={0,0,0,0};
//
//---> get trk-based beta, check mom-range (imply prot/mu),...
//
  if(kSpaceCalib==0)imass=kMuMass;//earth calib (mu)
  else{//space calib
    if(kUseHe4Calib)imass=kHeliumMass;
    else imass=kProtMass;
  }
  Float_t TofTruncEd(0);
  if(kUseHe4Calib){
    betptof=kBpowTof[2];
    if(absbet<kBeta0){
      betctof=pow(absbet,betptof)
                       /pow(Float_t(kBeta0),betptof);//TOF beta-corr for calub (used in cut for Tzslw)
    }
    else{
      betctof=1;
    }
  }
  if(TOFBetaOK && TRKtrOK && TOFTRKnormOK)TofTruncEd=etrtof1*betctof;
//
  betm=1; 
  if(TRKtrOK){
    if(kSpaceCalib==0)pmom=fabs(prtmom);//to use pos/neg particle(mu+-)
    else pmom=prtmom;//only pos.particles(pr) will give TzslMomOK
    MomMeasExist=kTRUE;
    betm=fabs(pmom)/sqrt(pmom*pmom+imass*imass);//measured (by Trk) beta  
    TzslMomOK=(pmom>=kTzslMomLims[kSpaceCalib][0] && pmom<=kTzslMomLims[kSpaceCalib][1]);//check low/high mom.
    AmplMomOK=(pmom>=kAmplMomLims[kSpaceCalib][0] && pmom<=kAmplMomLims[kSpaceCalib][1]);//check low/high mom.
    betgam=fabs(pmom)/imass;//beta*gamma=mom/mass
  }
  betm=0.998;
//
//---> check final TofClust/Trk-matching AND TofRawClust:
//
  for(il=0;il<kNtofl;il++){//<--check match in layers
    ib=tofbrnl[il];//rawclus (-1 means bad)
    if(ib>=0 && TOFTRKLmatchOK[il] && TOFTRKTmatchOK[il]){//<--- combine info on RawClust + Matching(beta/Clust-based)
      n2dml+=1;
      TzslFinLPat[il]=1;
    }
    else{//<--- reset bad layer in arrays for calibration 
      tofnbrl[il]=0;
      tofbrnl[il]=-1;
      toftrklen[il]=0;
      tofsd1q[il]=0;
      tofsd2q[il]=0;
      toftimr[il]=0;
    }
  }
  MatGoog4Tzsl=(((kTofLref==1 && TzslFinLPat[0]==1) || (kTofLref==2 && TzslFinLPat[1]==1))
		      && ((kTofStrictLPatt && n2dml==4) || (!kTofStrictLPatt && n2dml>=3)));
//
//--->Check trapezoidal counters if present :
//
  Bool_t TzslBadTrapCounter(kFALSE);
  for(il=0;il<kNtofl;il++){//check presence of trapez.counters
    if(TzslFinLPat[il]==1){
      ib=tofbrnl[il];
      if(kTofTzFitm==1 && (ib==0 || (ib+1)==kNtofb[il]))TzslBadTrapCounter=kTRUE;
      if((ib==0 || (ib+1)==kNtofb[il]) && fabs(longimp[il])>25)TzslBadTrapCounter=kTRUE;//if trap.counter,use only central Longit-impact
    }
  }
//--- fill some test counters:
  if(TOFBetaOK && TRKtrOK){//<---- TOFbeta/TRKtr presence check
    RunPar::addsev(50);//<-- count matching tests
    if(TOFRawClustOK){
      RunPar::addsev(51);
      if(MatGoog4Tzsl){
        RunPar::addsev(52);
        if(!TzslBadTrapCounter){
	  RunPar::addsev(53);
	  if(TzslMomOK){
	    RunPar::addsev(65);
	    if(tofbet>0.7 && tofbet<1.3){
	      RunPar::addsev(66);
	    }
	  }
	}
      }
    }
  }
//
//---> prepare input arrays for Tzsl:
//
  if(MatGoog4Tzsl && !TzslBadTrapCounter){
//
//---> create trlen[3] array(ref->2,ref->3,ref->4(ref=1 when L1-ok(otherwise ref=2), trlen[i]=0 when related pair is missing)
//
    if(kTofLref==1)trlr0=toftrklen[0];
    else trlr0=toftrklen[1];
    for(int i=0;i<kNtofl-1;i++){
      if(toftrklen[i+1]!=0.)trlen[i]=(toftrklen[i+1]-trlr0);//r->2,r->3,r->4(=0 for missing pair)
    }
//---> measured track-lengthes:
//
    for(il=0;il<kNtofl-1;il++)tld[il]=trlen[il];//r->2,r->3,r->4(=0 for missing pair), tld>0
//
//---> measured time-differences:
    Double_t times0(0);
    if(kTofLref==1)times0=toftimr[0];//r=1
    else times0=toftimr[1];//r=2
    for(int i=0;i<kNtofl-1;i++){
      tdm[i]=0.;
      if(toftimr[i+1]!=0.)tdm[i]=times0-toftimr[i+1];//r->2,r->3,r->4(=0 for missing pair), tdm>0 for bet>0
    }
//----
    for(il=0;il<kNtofl;il++){//new parametrization(also ready for indiv.slopes)
      ramm[il]=0.;
      if(tofnbrl[il]==0)continue;//skip missing layer
      ib=tofbrnl[il];
      if (kUseNewSlew) {
	int index1=kTofFirstSidePos[il]+ib;
	int index2=kTofFirstSidePos[il]+ib+kNtofb[il];
	ramm[il]=(TofTzSlope[index1]/sqrt(tofsd1q[il])+TofTzSlope[index2]/sqrt(tofsd2q[il]));//works sl.better
      }
      else {
	ramm[il]=(1/sqrt(tofsd1q[il])+1/sqrt(tofsd2q[il]));//works sl.better
      }
    }
//cout<<"In ramm="<<ramm[0]<<" "<<ramm[1]<<" "<<ramm[2]<<" "<<ramm[3]<<endl;
//----
      Double_t ramm0(0);
      if(kTofLref==1)ramm0=ramm[0];//r=1
      else ramm0=ramm[1];//r=2
      for(int i=0;i<kNtofl-1;i++){
        dum[i]=0.;
        if(ramm[i+1]!=0.)dum[i]=ramm[i+1]-ramm0;//2-r,3-r,4-4 (=0 for missing pairs)
      }
//
      Double_t tdr[kNtofl-1];
      for(int i=0;i<kNtofl-1;i++)tdr[i]=tld[i]/betm/cvel;//real(reference) time-diffs
      for(int i=0;i<kNtofl-1;i++)tdrm[i]=tdr[i]-tdm[i];//(ref.-meas.) time-diffs
//
  }//---> endof input arrays preparation
//
//---> start calibration:
//    
  if(kTofCalibType[1]>0){//<--- Do Tzslw-calib
    RunPar::addsev(55);
    if(TOFRawClustOK){//<--- 1rawcl/layer
      if(kTofCalibPrint[1]>0){
        for(il=0;il<kNtofl;il++){
	  if(tofbrnl[il]>=0)tftzh1->Fill(tofbrnl[il]+1+il*10,1);
	  tftzh2->Fill(prtcha,1);
	}
      }
      RunPar::addsev(56);
//      if(prtcha==1){//<--- select Pr/Hel from top
      if((!kUseHe4Calib && TopProt)||(kUseHe4Calib && TopHelium)){//<--- select Pr/Hel from top
        if(kTofCalibPrint[1]>0)tftzh3->Fill(prtmom,1);
        RunPar::addsev(57);
        if(TzslMomOK){//<--- MomRangeOK (select only pos. for space)
          if(kTofCalibPrint[1]>0){
	    if(TOFBetaOK){
	      tftzh4->Fill(tofbet,1);
	      tftzh9->Fill(p2beta->Chi2,1);
	      tftzh10->Fill(p2beta->Chi2S,1);
	    }
	  }
          RunPar::addsev(58);
          if(TOFBetaOK && (tofbet>0.7 && tofbet<1.3)){//<--- prev.cal beta ok
            if(kTofCalibPrint[1]>0){
              for(il=0;il<4;il++){//<--- layer loop
                ib=bltof[il];//1:8(10), =0 if missing
                if(ib==0 || ntofp[il]>1)continue;
                dx=xtofh[il]-tofcro[il][0];
                dy=ytofh[il]-tofcro[il][1];
                if(il==0 || il==3){//x=long.coo
                  tftzh5[il]->Fill(dx,1);
                  tftzh6[il]->Fill(dy,1);
                }
                else{//y=long.coo
                  tftzh5[il]->Fill(dy,1);
                  tftzh6[il]->Fill(dx,1);
                }
              }
	    }
            RunPar::addsev(59);
	    if(MatGoog4Tzsl && !TzslBadTrapCounter && TOFTRKnormOK){//<--- matchOK,TrapCok,normOK
	      if(kTofCalibPrint[1]>0){
	        tftzh7->Fill(betm,1);
	        tftzh7a->Fill(TofTruncEd,1);
                for(il=0;il<kNtofl;il++){
	          if(tofbrnl[il]>=0)tftzh8->Fill(tofbrnl[il]+1+il*10,1);
	        }
              }
              RunPar::addsev(60);
	      if((!kUseHe4Calib && TofTruncEd<4.5)||(kUseHe4Calib && TofTruncEd>5.)){
                RunPar::addsev(61);
//cout<<"-----> Entry2filltz: event="<<evnum<<" Bars:"<<tofbrnl[0]<<" "<<tofbrnl[1]<<" "<<tofbrnl[2]<<" "<<tofbrnl[3]<<endl;
//cout<<"       trlen:"<<toftrklen[0]<<" "<<toftrklen[1]<<" "<<toftrklen[2]<<" "<<toftrklen[3]<<endl;
//cout<<"       times:"<<toftimr[0]<<" "<<toftimr[1]<<" "<<toftimr[2]<<" "<<toftimr[3]<<endl;
                for(int i=0;i<kNtofl;i++)brnlw[i]=tofbrnl[i];//copy for Tzslw(because it can change brnl from inside)
//cout<<"       brnlw:"<<brnlw[0]<<" "<<brnlw[1]<<" "<<brnlw[2]<<endl;
//cout<<"       tdrm:"<<tdrm[0]<<" "<<tdrm[1]<<" "<<tdrm[2]<<endl;
//cout<<"       dum:"<<dum[0]<<" "<<dum[1]<<" "<<dum[2]<<"  betm="<<betm<<endl;
                TofCalib::filltz(brnlw,tdrm,dum); // fill calib.working arrays
	      }
            }//--->endof "matchOK,TrapCok"
          }//--->endof "prev.cal beta range ok
	}//--->endof MomOk
      }//--->endof "chargeOK"
    }//--->endof "RawClust ok:   
  }//--->endof Tzsl-calib
//
//---------------> Ampl-calib:
//
  Float_t ainp[2],dinp[2][kTofPmtMX],cinp;
  Float_t trkcos[4],qnorm1[4],qnorm2[4];
// ---> normalize charge-responce to normal incidence (for rel.gains/abs.norm-calib):
//
  dx=tofcro[0][0]-tofcro[1][0];//tofcro's exist for all 4 layers by definition
  dy=tofcro[0][1]-tofcro[1][1];
  dz=tofcro[0][2]-tofcro[1][2];
  trkcos[0]=dz/sqrt(dx*dx+dy*dy+dz*dz);//L12 cos
  trkcos[1]=trkcos[0];
  dx=tofcro[2][0]-tofcro[3][0];
  dy=tofcro[2][1]-tofcro[3][1];
  dz=tofcro[2][2]-tofcro[3][2];
  trkcos[2]=dz/sqrt(dx*dx+dy*dy+dz*dz);//L34 cos
  trkcos[3]=trkcos[2];
  for(il=0;il<kNtofl;il++){
    qnorm1[il]=tofsd1q[il]*fabs(trkcos[il]);
    qnorm2[il]=tofsd2q[il]*fabs(trkcos[il]);
  }
//
  if(kTofCalibType[2]>0){//<--- Do Ampl-calib
    RunPar::addsev(70);
    if(
        TOFBetaOK && 
	              TRKtrOK){//<--- An2Dyn-calib
      RunPar::addsev(71);
//
// ---> Fill arrays for a2dr/dgains calculations(no mip-selection upto here,
//                           will use bar mid-bin only) and do calibration:
      for(il=0;il<kNtofl;il++){
        ib=tofbrnl[il];
        if(ib < 0)continue;
	if(!TOFTRKTmatchOK[il])continue;//need transv.matching  only ?(just to increase statistics) 
        cinp=longimp[il];//longit.coo of trk crossing, paddle loc.r.s.!!!
        ainp[0]=tofadca[il][0];//ADC-ch
        ainp[1]=tofadca[il][1];
        for(int ip=0;ip<kTofPmtMX;ip++){
          dinp[0][ip]=tofadcdr1[il][ip];
          dinp[1][ip]=tofadcdr2[il][ip];
        }
        if(ainp[0]>tofsdthr && ainp[1]>tofsdthr){
          TofCalib::filla2dg(il,ib,cinp,ainp,dinp);//for Anode/Sum(Dynode(ip)), Dgain(ip)
        }
      }
    }//--->endof An2Dyn-calib 
//---
//
//
// ------> fill working arrays for Anode Relat.gains/Att.Length calib :
//
    if(prtcha==1){//<--- require Q=1
    RunPar::addsev(75);
    if(AmplMomOK){//<--- require pos.part in needed mom.range(prot)
    RunPar::addsev(76);
    if(tofbet>0.7 && tofbet<1.15){//<--- require top.hemisph
      RunPar::addsev(77);
      for(il=0;il<kNtofl;il++){
        ib=tofbrnl[il];
        if(ib < 0)continue;
        if(!(TOFTRKLmatchOK[il] && TOFTRKTmatchOK[il]))continue;//need both matching
        ainp[0]=qnorm1[il];//q(pC)
        ainp[1]=qnorm2[il];
        cinp=longimp[il];// longit.coo of trk crossing, paddle loc.r.s.!!!
        if(ainp[0]>(tofsdthr*kTofAdc2Q) && ainp[1]>(tofsdthr*kTofAdc2Q)){
          TofCalib::fillam(il,ib,ainp,cinp);//for relat.gains/LongFits(using Anode)
        } 
      }//--->endof layer loop
    }
    }
    }
//
// ------> fill working arrays for Mip abs.normalization calib :
//
    Int_t ibt,id,idr;
    if(prtcha==1){//<--- require Q=1
    RunPar::addsev(80);
    if(AmplMomOK){//<--- require pos.part in needed mom.range(prot)
    RunPar::addsev(81);
    if(tofbet>0.7 && tofbet<1.15){//<--- require top.hemisph
      RunPar::addsev(82);
      for(il=0;il<kNtofl;il++){
        ib=tofbrnl[il];
        if(ib < 0)continue;
        if(!(TOFTRKLmatchOK[il] && TOFTRKTmatchOK[il]))continue;//need both matching
        ainp[0]=qnorm1[il]*betctof;//beta-correction for tof(prot)
        ainp[1]=qnorm2[il]*betctof;
        cinp=longimp[il];// longit.coo of trk crossing, paddle loc.r.s.!!!
        ibt=TofCalib::brtype(il,ib);// bar type (1-11)
        id=100*(il+1)+ib+1;
        if(kTofCalibPrint[2]>0){
	  if(fabs(cinp)<15)tfamh7->Fill(betgam,(ainp[0]+ainp[1]));
	}
        if(ainp[0]>(tofsdthr*kTofAdc2Q) && ainp[1]>(tofsdthr*kTofAdc2Q)){
          TofCalib::fillabs(il,ib,ainp,cinp);//for abs.normalization(using Anode)
        }	  
      }//--->endof layer loop
    }
    }
    }
  }//--->endof Ampl-calib
//
//----> try to set Ready4Calib flag :
//
  Ready4Calib=(RunPar::getsev(44)>RunPar::GetGenc(38))
           &&(RunPar::getsev(61)>RunPar::GetGenc(39))
           &&(RunPar::getsev(77)>RunPar::GetGenc(40))
           &&(RunPar::getsev(82)>RunPar::GetGenc(40));
  if(Ready4Calib)cout<<endl<<"<==== Ready4Calib,Nevs(Td/Tz/RgLp/Abn): "<<RunPar::getsev(44)<<" "<<RunPar::getsev(61)<<
                                                                     " "<<RunPar::getsev(77)<<" "<<RunPar::getsev(82)<<endl;
//=============================================================
//  
  if(RunPar::GetMCF()){
    RunPar::imcsel(mcrbin);// MCgen Rig-spectrum after all Cuts
//    mcgh5->Sumw2();
    mcgh5->Fill(log10(arigmc),1);
  }
  RunPar::imesel(merbin);// Measured Rig-spectrum after all cuts
//  amsh7->Sumw2();
  amsh7->Fill(log10(arigme),1);
// 
//cout<<"----> Fill ended"<<endl;
//------- 
} 
 catch (...)
    {
     cout <<"  TofCalibPG exception catched "<<" "<<Event()<<endl;
    return ;
    }
//-------
}//---> endof Fill
//
//======================================================================
void TofCalibPG::UTerminate(){
  Int_t calibsta[3]={0,0,0};//(Td/Tz/Am),1=bad
  Int_t statusf(0);//111 if all ok
  cout<<"----> Enter UTerminate"<<endl;
  RunPar::prtselc();//<-print selection counters
//---
  if(RunPar::GetMCF()){
    mcgh5->Sumw2();
    mcgh6->Divide(mcgh5,mcgh1,1.,1.,"B");
    mcgh7->Divide(mcgh5,mcgh4,1.,1.,"B");
  }
//------>Tof--analysis1:
  if(kAnalyse[6]==1)TofAnalys1Out(0);
  if(kAnalyse[6]==3)TofAnalys2Out(0);
//
//---> Tdlv-calib:
  if(kTofCalibType[0]>0)calibsta[0]=TofCalib::fittd();//hist.printed inside
//---> Tzsl-calib:
  if(kTofCalibType[1]>0){
    calibsta[1]=TofCalib::fittz();
    if(kTofCalibPrint[1]>0)TofCalib::TzslwHist();
  }
//---> Ampl-calib:
  if(kTofCalibType[2]>0){
    calibsta[2]=TofCalib::fitam();
    if(kTofCalibPrint[2]>0)TofCalib::AmplHist();
  }
//---> create/update new Cflist-file:
  if(kTofCalibType[0]>0 || kTofCalibType[1]>0 || kTofCalibType[2]>0){
    Int_t wstatus=TofCalib::cwrite();
    cout<<"<----- TofCalib:ResultsBlock :"<<endl;
    if(calibsta[0]==0)statusf+=100;
    if(calibsta[1]==0)statusf+=10;
    if(calibsta[2]==0)statusf+=1;
    cout<<statusf<<endl;
    cout<<"<----- TofCalib:EndOfResultsBlock."<<endl;
//
    cout<<"       First/Last processed runs:"<<TofCalib::FirstLastRun[0]<<" "<<TofCalib::FirstLastRun[1]<<endl;
    cout<<"       First/Last processed Event:"<<TofCalib::FirstLastEvent[0]<<" "<<TofCalib::FirstLastEvent[1]<<endl;
    cout<<"       First EventDate:"<<TofCalib::FirstEvDate;
    cout<<"       Last  EventDate:"<<TofCalib::LastEvDate<<endl;
    cout<<"<==== CalibIsOver,Nevs(Td/Tz/RgLp/Abn): "<<RunPar::getsev(44)<<" "<<RunPar::getsev(61)<<
                                                 " "<<RunPar::getsev(77)<<" "<<RunPar::getsev(82)<<endl;
  }
//---------------
//--->TofTdcCorr
  if(kAnalyse[0]>0)TofTdcCorOut(0);
//---------------  
// Write histograms into a ROOT file
//  outfile->Write();

// Write the list of selected events
//   select_list->Write("select.list");

// Write all selected events into a new root file 
// TFile* another_file = new TFile("anotherfile.root","RECREATE");
// select_list->Write(Tree(), another_file);
      
// Write only Header and Particle branches of selected events
// (added to the default output file in this example)
//Tree()->SetBranchStatus("*",0);
//Tree()->SetBranchStatus("ev.fHeader",1);
//Tree()->SetBranchStatus("ev.fParticle",1);
//select_list->Write(Tree(), outfile);
//
cout<<"----> UTerminate ended"<<endl;
}
//======================================================================
//-------------------------------
// TRK-track address study:
void TofCalibPG::TrkAdrSel(Int_t mode, ParticleR * p2prt){
  Float_t trkcro[9][3]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  Float_t cstrk[9]={1,1,1,1,1,1,1,1,1};
  Float_t dx,dy,dz,xc0,yc0,tgx,tgy;
  Int_t trklrs[9]={0,0,0,0,0,0,0,0,0};
  Int_t il,i;
//---> extract crossing points with tracker layers
  for(int i=0;i<9;i++){
    for(int j=0;j<3;j++){
      trkcro[i][j]=p2prt->TrCoo[i][j];//TRKpln/TRKtrack cross.points vs layer
    }
  }
//---> evaluate crossing cos's
  dx=trkcro[0][0]-trkcro[1][0];
  dy=trkcro[0][1]-trkcro[1][1];
  dz=trkcro[0][2]-trkcro[1][2];
  cstrk[0]=dz/sqrt(dx*dx+dy*dy+dz*dz);//L12 cos
  dx=trkcro[1][0]-trkcro[2][0];
  dy=trkcro[1][1]-trkcro[2][1];
  dz=trkcro[1][2]-trkcro[2][2];
  cstrk[1]=dz/sqrt(dx*dx+dy*dy+dz*dz);//L23 cos
  cstrk[2]=cstrk[1];
  dx=trkcro[3][0]-trkcro[4][0];
  dy=trkcro[3][1]-trkcro[4][1];
  dz=trkcro[3][2]-trkcro[4][2];
  cstrk[3]=dz/sqrt(dx*dx+dy*dy+dz*dz);//L45 cos
  cstrk[4]=cstrk[3];
  xc0=0.5*(trkcro[3][0]+trkcro[4][0]);//xcr in plane "z=0"
  yc0=0.5*(trkcro[3][1]+trkcro[4][1]);//ycr in plane "z=0"
  tgx=dx/dz;//tanX of Z/\Trk angle
  tgy=dy/dz;//tanY
  dx=trkcro[5][0]-trkcro[6][0];
  dy=trkcro[5][1]-trkcro[6][1];
  dz=trkcro[5][2]-trkcro[6][2];
  cstrk[5]=dz/sqrt(dx*dx+dy*dy+dz*dz);//L67 cos
  cstrk[6]=cstrk[5];
  dx=trkcro[6][0]-trkcro[7][0];
  dy=trkcro[6][1]-trkcro[7][1];
  dz=trkcro[6][2]-trkcro[7][2];
  cstrk[7]=dz/sqrt(dx*dx+dy*dy+dz*dz);//L78 cos
  dx=trkcro[7][0]-trkcro[8][0];
  dy=trkcro[7][1]-trkcro[8][1];
  dz=trkcro[7][2]-trkcro[8][2];
  cstrk[8]=dz/sqrt(dx*dx+dy*dy+dz*dz);//L89 cos
/*
//
//---> extract track/hits parameters :
//
  TrTrackR *p2trk = p2prt->pTrTrack();//pointer to TRK-track used by Part.
  Int_t ntrkh(0);
  ntrkh=p2trk->NTrRecHit();//# TrkTr-hits
  Int_t patt=p2trk->Pattern;
//--->Read Vitaly's TRK-track long address and create:
  unsigned long long addr=p2trk->Addressl;
//
  static Int_t first(1);
  Int_t laddn[kNtrkl]={0,0,0,0,0,0,0,0,0};//fired ladd.numbers
  Int_t laddh[kNtrkl]={0,0,0,0,0,0,0,0,0};//fired ladd.halfs
  Float_t TrkTrClAx[kNtrkl]={0,0,0,0,0,0,0,0,0};
  Float_t TrkTrClAy[kNtrkl]={0,0,0,0,0,0,0,0,0};
  if(ntrkh>0){
    TrRecHitR * p2trh;
    TrClusterR * p2trcl;
    Float_t kssum,ksass;
    Int_t clid,lad,hal,lay;
    for(int i=0;i<ntrkh;i++){
      p2trh=p2trk->pTrRecHit(i);
      kssum=p2trh->Sum;//Ax+Ay(i.e. K+S)
      ksass=p2trh->DifoSum;//(Ax-Ay)/(Ax+Ay)
      il=(p2trh->Id)%10;//trk-layer (1-8)
      trklrs[i]=il;//store layer# for each hit
      TrkTrClAx[i]=cstrk[il-1]*kssum*(1+ksass)/2;//K-signal,angle-corrected
      TrkTrClAy[i]=cstrk[il-1]*kssum*(1-ksass)/2;//S-signal,angle-corrected
      p2trcl=p2trh->pTrCluster('x');
      if(p2trcl>0){
        TrkTrClAx[i]=p2trcl->Sum;
	clid=p2trcl->Idsoft;
	lay=clid%10;//1-8
	lad=(clid/10)%100;//1-max
	hal=(clid/1000)%10;//1st half ->0, 2nd->1
      }
      p2trcl=p2trh->pTrCluster('y');
      if(p2trcl>0){
        TrkTrClAy[i]=p2trcl->Sum;
	clid=p2trcl->Idsoft;
	lay=clid%10;
	lad=(clid/10)%100;
	hal=(clid/1000)%10-2;//1st half ->0, 2nd->1
        laddn[il-1]=lad;//ladder #
        laddh[il-1]=hal;//halfs(0/1)
      }
      trkh7->Fill(TrkTrClAx[i],TrkTrClAy[i],1.);
      if(TrkTrClAx[i]>0 && TrkTrClAy[i]>0)trkh8->Fill(TrkTrClAx[i],TrkTrClAy[i],1.);
    }
  }
//--->create current event track-address using ladders/halsf info:
//  addr=TrkTradd(laddn,laddh);
//--->select event and fill address list:
  if(ntrkh==9){
    RunPar::addsev(61);//<--passed pattern test
    if(RunPar::trkaddn<4){//print few first events
      cout<<"---> AddrLow/AddrHi="<<addlo<<" "<<addhi<<endl;
      cout<<" 9-hits pattern="<<patt<<" original ladders/halfs:"<<endl;
      for(i=0;i<9;i++)cout<<laddn[i]<<" ";
      cout<<endl;
      for(i=0;i<9;i++)cout<<laddh[i]<<" ";
      cout<<endl<<endl;
      for(i=0;i<9;i++){
        laddn[i]=0;
        laddh[i]=0;
      }
      cout<<"                    decoded ladders/halfs:"<<endl;
      addr.decode(laddn,laddh);
      for(i=0;i<9;i++)cout<<laddn[i]<<" ";
      cout<<endl;
      for(i=0;i<9;i++)cout<<laddh[i]<<" ";
      cout<<endl;
    }
    for(i=0;i<RunPar::trkaddn;i++){//<--loop over previous events trk-addresses list
      if(addr==RunPar::trkadd[i]){
        if(RunPar::trkadde[i]<kNtremx){//store needed trk-params for given address
	  RunPar::trxc0[i][RunPar::trkadde[i]]=xc0;
	  RunPar::tryc0[i][RunPar::trkadde[i]]=yc0;
	  RunPar::trtgx[i][RunPar::trkadde[i]]=tgx;
	  RunPar::trtgy[i][RunPar::trkadde[i]]=tgy;
	}
        RunPar::trkadde[i]+=1;
	goto exit1;//skip addr-list filling because current address is already present in the list
      }
    }
    if(RunPar::trkaddn<kNtradd){//add current address to the addr-list as new
      RunPar::trkadd[RunPar::trkaddn]=addr;//add new addr to list
      RunPar::trxc0[RunPar::trkaddn][0]=xc0;//store params for new address(1st event)
      RunPar::tryc0[RunPar::trkaddn][0]=yc0;
      RunPar::trtgx[RunPar::trkaddn][0]=tgx;
      RunPar::trtgy[RunPar::trkaddn][0]=tgy;
      RunPar::trkadde[RunPar::trkaddn]+=1;
      RunPar::trkaddn+=1;//counts addr-list members
    }
    else{
      cout<<"TRK:ovfl of address list at length="<<RunPar::trkaddn<<endl;
      RunPar::trkadde[RunPar::trkaddn]+=1;
    } 
exit1:
    i=0;
//    cout<<"TRK:new addr is already in the list"<<endl;;
  }
*/
}
//---
void TofCalibPG::TrkAdrOut(Int_t mode){
// mode>0 -> write bad addr.list to file
  Bool_t ordr(kTRUE);//to sort in decreasing order
  Int_t i,imax,iadd,ovfl,n90(0),ntot(0),nbins;
  Float_t accin;
  cout<<"TrkAdrOut:ListLength/OutOfListEvents="<<RunPar::trkaddn<<" "<<RunPar::trkadde[kNtradd]<<endl;
  TMath::Sort(RunPar::trkaddn,RunPar::trkadde,RunPar::trkaddp,ordr);//sort in decreasing order
  cout<<"50 most popular addresses frequency:"<<endl;
  imax=TMath::Min(50,RunPar::trkaddn);
  for(i=0;i<imax;i++){
    cout<<"Events/Addr: "<<RunPar::trkadde[RunPar::trkaddp[i]]<<endl;    
  }
//
  for(i=0;i<RunPar::trkaddn;i++){//<-- create sorted trk-addresses list
    RunPar::trkadds[i]=RunPar::trkadd[RunPar::trkaddp[i]];
  }  
//
  char title[127];
  char hname[127];
  char text[127];
  TH2F * trkhi1=0;
  TH2F * trkhi2=0;
  TH1F * trkhi3=0;
  TH2F * trkhi4=0;
  TH2F * trkhi5=0;
  iadd=1;//member(in the ordered address list) used for histogramming
  sprintf(title,"TRK:Yc0 vs Xc0, OrderedAddr=%02d",iadd);
  if(trkhi1)delete trkhi1;
  trkhi1=new TH2F("trkhi1",title,50,-50.,50.,40,-20.,20.);
  if(trkhi2)delete trkhi2;
  sprintf(title,"TRK:TgY vs TgX, OrderedAddr=%02d",iadd);
  trkhi2=new TH2F("trkhi2",title,50,-0.75,0.75,50,-0.75,0.75);
//--->find 90% statistics boundary in the ordered addr.list
  n90=0;
  ntot=RunPar::getsev(61);//<--get "pattern"-selected events number
  for(i=0;i<RunPar::trkaddn;i++){
    n90+=RunPar::trkadde[RunPar::trkaddp[i]];
    imax=i;
    if(float(n90)/ntot>=0.9)break;
  }
  ovfl=ntot-n90;
  nbins=imax+1;
  accin=float(n90)/ntot;
//--->book/fill "population"-histogram:
  if(trkhi3)delete trkhi3;
  trkhi3=new TH1F("trkhi3","TRK:events per address(90% of most popular ones)",nbins,1,nbins+1);
  for(i=1;i<=imax;i++)trkhi3->SetBinContent(i,RunPar::trkadde[RunPar::trkaddp[i-1]]);//set bin content
  cout<<"TRKAddr: events Total/90%="<<ntot<<"/"<<n90<<" 90%events AddrList-sublength ="<<imax<<endl;
  trkhi3->SetBinContent(nbins+1,ovfl);//set ovfl
//--->fill event-by-event xcr/ycr/tgx/tgy hist for ordered-list member "iadd"(=1 for most populated):
  imax=TMath::Min(kNtremx,RunPar::trkadde[RunPar::trkaddp[iadd-1]]);
  for(i=0;i<imax;i++){
    trkhi1->Fill(RunPar::trxc0[RunPar::trkaddp[iadd-1]][i],RunPar::tryc0[RunPar::trkaddp[iadd-1]][i],1.);//yc0 vs xc0
    trkhi2->Fill(RunPar::trtgx[RunPar::trkaddp[iadd-1]][i],RunPar::trtgy[RunPar::trkaddp[iadd-1]][i],1.);//tgy vs tgx
  }
//--->book/fill averxcr/ycr/tgx/tgy hist:
  if(trkhi4)delete trkhi4;
  trkhi4=new TH2F("trkhi4","Average Ycr vs Xcr for most pop.addresses",50,-50.,50.,50,-50.,50.);
  if(trkhi5)delete trkhi5;
  trkhi5=new TH2F("trkhi5","Average Tgy vs Tgx for most pop.addresses",50,-1.,1.,50,-1.,1.);
  Double_t xc,xc2,yc,yc2,txc,tyc,txc2,tyc2;
  Float_t xrms,yrms,txrms,tyrms;
  for(Int_t iad=0;iad<nbins;iad++){//loop over most popular addresses (containing 90% of statistics)
    xc=0;
    yc=0;
    txc=0;
    tyc=0;
    xc2=0;
    yc2=0;
    txc2=0;
    tyc2=0;
    imax=TMath::Min(kNtremx,RunPar::trkadde[RunPar::trkaddp[iad]]);
    for(i=0;i<imax;i++){
      xc+=RunPar::trxc0[RunPar::trkaddp[iad]][i];
      yc+=RunPar::tryc0[RunPar::trkaddp[iad]][i];
      xc2+=RunPar::trxc0[RunPar::trkaddp[iad]][i]*RunPar::trxc0[RunPar::trkaddp[iad]][i];
      yc2+=RunPar::tryc0[RunPar::trkaddp[iad]][i]*RunPar::tryc0[RunPar::trkaddp[iad]][i];
      txc+=RunPar::trtgx[RunPar::trkaddp[iad]][i];
      tyc+=RunPar::trtgy[RunPar::trkaddp[iad]][i];
      txc2+=RunPar::trtgx[RunPar::trkaddp[iad]][i]*RunPar::trtgx[RunPar::trkaddp[iad]][i];
      tyc2+=RunPar::trtgy[RunPar::trkaddp[iad]][i]*RunPar::trtgy[RunPar::trkaddp[iad]][i];
    }
    if(imax>1)xrms=TMath::Sqrt(TMath::Max((imax*xc2-xc*xc)/imax/(imax-1),0.));
    else xrms=0;
    xc/=imax;
    if(imax>1)yrms=TMath::Sqrt(TMath::Max((imax*yc2-yc*yc)/imax/(imax-1),0.));
    else yrms=0;
    yc/=imax;
    trkhi4->Fill(xc,yc,1.);//ycaver vs xcaver address-by-address
    
    if(imax>1)txrms=TMath::Sqrt(TMath::Max((imax*txc2-txc*txc)/imax/(imax-1),0.));
    else txrms=0;
    txc/=imax;
    if(imax>1)tyrms=TMath::Sqrt(TMath::Max((imax*tyc2-tyc*tyc)/imax/(imax-1),0.));
    else tyrms=0;
    tyc/=imax;
    trkhi5->Fill(txc,tyc,1.);//tgy.aver vs tgx.aver address-by-address
    if(iad==0){
      cout<<"Nev="<<imax<<"x/sx="<<xc<<" "<<xrms<<" y/sy="<<yc<<" "<<yrms<<endl;
      cout<<"             tx/txrms="<<txc<<" "<<txrms<<" ty/tyrms="<<tyc<<" "<<tyrms<<endl;
    }
  }  
//  
  TCanvas *trkc1=new TCanvas("Trkc1","Test1",1);
  trkc1->Divide(2,1);
  trkc1->cd(1);
  gPad->SetGrid();
  trkhi1->SetMarkerStyle(20);
  trkhi1->SetMarkerSize(0.5);
  trkhi1->Draw("P");
  trkc1->cd(2);
  gPad->SetGrid();
  trkhi2->SetMarkerStyle(20);
  trkhi2->SetMarkerSize(0.5);
  trkhi2->Draw("P");
  
  TCanvas *trkc2=new TCanvas("Trkc2","Test2",1);
  trkc2->Divide(1,1);
  trkc2->cd(1);
  gStyle->SetOptStat(111111);
  gPad->SetGrid();
  gPad->SetLogy(1);
  gPad->SetLogx(1);
  trkhi3->Draw();
  TText *t=new TText();
  t->SetTextFont(32);
  t->SetTextColor(1);
  t->SetTextSize(0.05);
  sprintf(text,"Acceptance = %5.3f",accin);
  t->DrawText(60.,20.,text);
  
  TCanvas *trkc3=new TCanvas("Trkc3","Test3",1);
  trkc3->Divide(2,1);
  trkc3->cd(1);
  gPad->SetGrid();
  trkhi4->SetMarkerStyle(20);
  trkhi4->SetMarkerSize(0.5);
  trkhi4->Draw("P");
  trkc3->cd(2);
  gPad->SetGrid();
  trkhi5->SetMarkerStyle(20);
  trkhi5->SetMarkerSize(0.5);
  trkhi5->Draw("P");
//
//--->write TrkAddr sorted list to file:
// 
  if(mode>0){
    Char_t fn[80];
    strcpy(fn,"TrkAddrList");
    Bool_t OK=RunPar::WriteFile(RunPar::trkadds,RunPar::trkaddn,fn,RunPar::StartRun);
    if(OK)cout<<"=====> TrkTrAddrList is written to file "<<fn<<endl;
    else cout<<"=====> Problem to write TrkTrAddrList to file "<<fn<<" !!!"<<endl;
  }
}
//-------------------------------
//---------------------------------------------------------------
void TofCalibPG::TofTdcCorOut(Int_t mode){
//
  Int_t crat,slot,chan,bin;
  Float_t buff[1024],horc[1024];
  for(bin=0;bin<1024;bin++){
    buff[bin]=kTofTbw*RunPar::tdcorr[0][0][2][bin];//ch-2
    horc[bin]=kTofTbw*bin;
  }
  TGraph *gr1 = new TGraph(1024,horc,buff);
  for(bin=0;bin<1024;bin++)buff[bin]=kTofTbw*RunPar::tdcorr[0][0][5][bin];
  TGraph *gr2 = new TGraph(1024,horc,buff);
//  
  TCanvas *c1=new TCanvas("c1","TofTdc(Crate_0/SFET_00) linearity over 25ns period (1024 bins)",1);
  c1->Divide(1,2);
  c1->cd(1);
  gr1->SetFillColor(8);
  gr1->GetXaxis()->SetTitle("Input time(ns)");
  gr1->GetXaxis()->CenterTitle();
  gr1->GetYaxis()->SetTitle("Deviation from linearity (ns)");
  gr1->GetYaxis()->CenterTitle();
  gr1->SetTitle("Linearity graph for channel_2");
  gr1->Draw("AB");
  c1->cd(2);
  gr2->SetFillColor(8);
  gr2->GetXaxis()->SetTitle("Input time (ns)");
  gr2->GetXaxis()->CenterTitle();
  gr2->GetYaxis()->SetTitle("Deviation from linearity (ns)");
  gr2->GetYaxis()->CenterTitle();
  gr2->SetTitle("Linearity graph for channel_5(FastTrig)");
  gr2->Draw("AB");
  c1->Update();
//---
  Float_t cdiff;
  crat=0;
  for(slot=0;slot<4;slot++){
    for(chan=1;chan<6;chan++){
      if(RunPar::tdchmap[crat][slot][chan]==0)continue;
      for(bin=0;bin<1024;bin++){
        cdiff=kTofTbw*(RunPar::tdcorr[0][slot][chan][bin]-RunPar::tdcorr[0][slot][0][bin]);
	tofhtdc1->Fill(cdiff,1);
      }
    }  
  }
//
  Int_t slot1;
  Int_t slot2;
  crat=0;
  for(slot1=0;slot1<4;slot1++){
  for(slot2=slot1+1;slot2<4;slot2++){
    for(chan=0;chan<6;chan++){
      if(RunPar::tdchmap[crat][slot1][chan]==0 || RunPar::tdchmap[crat][slot2][chan]==0)continue;
      for(bin=0;bin<1024;bin++){
        cdiff=kTofTbw*(RunPar::tdcorr[0][slot1][chan][bin]-RunPar::tdcorr[0][slot2][chan][bin]);
        tofhtdc2->Fill(cdiff,1);
      }
    }
  }
  }
//
  slot1=0;
  slot2=1;
  Int_t bin1,bin2,evs;
  Float_t rnd1,rnd2;
  for(chan=0;chan<6;chan++){
    if(RunPar::tdchmap[crat][slot1][chan]==0 || RunPar::tdchmap[crat][slot2][chan]==0)continue;
    for(evs=0;evs<1000;evs++){
      rnd1=gRandom->Rndm();
      bin1=floor(rnd1*1023);
      rnd2=gRandom->Rndm();
      bin2=floor(rnd2*1023);
      cdiff=kTofTbw*(RunPar::tdcorr[0][slot1][chan][bin1]-RunPar::tdcorr[0][slot2][chan][bin2]);
      tofhtdc3->Fill(cdiff,1);
    }
  }
//  
  TCanvas *c2=new TCanvas("c2","TofTdc(Crate_0/SFET_00->04) inter-channels linearity",1);
  c2->Divide(1,3);
  c2->cd(1);
  gPad->SetGrid();
  gPad->SetLogy(1);
  gStyle->SetOptStat(1111111);
  tofhtdc1->SetFillColor(8);
  tofhtdc1->GetXaxis()->SetTitle("Two channels bin-time difference(ns)");
  tofhtdc1->GetXaxis()->CenterTitle();
  tofhtdc1->Draw();
  c2->cd(2);
  gPad->SetGrid();
  gPad->SetLogy(1);
  gStyle->SetOptStat(1111111);
  tofhtdc2->SetFillColor(8);
  tofhtdc2->GetXaxis()->SetTitle("Two channels bin-time difference(ns)");
  tofhtdc2->GetXaxis()->CenterTitle();
  tofhtdc2->Draw();
  c2->cd(3);
  gPad->SetGrid();
//  gPad->SetLogy(1);
  gStyle->SetOptStat(1111111);
  tofhtdc3->SetFillColor(2);
  tofhtdc3->GetXaxis()->SetTitle("Two channels bin-time difference(ns)");
  tofhtdc3->GetXaxis()->CenterTitle();
  tofhtdc3->Draw();
  c2->Update();
//
}
//-----------------------------------------------------------------------
void TofCalibPG::TofAnalys1Out(Int_t mode){
  char title[127];
  char hname[127];
  char text[127];
  TCanvas *c1=new TCanvas("c1","Time resolution",1);
  c1->Divide(2,2);
  c1->cd(1);
  gPad->SetGrid();
  gStyle->SetOptStat(11);
  gStyle->SetOptFit(111);
  tofh12->SetMarkerStyle(20);
  tofh12->SetMarkerSize(0.5);
  tofh12->Fit("gaus","","",-1.1,1.1);
  tofh12->GetXaxis()->SetTitle("(TimeL3-TimeL1)-(TimeL4-Timel2) [ns]");
  tofh12->GetXaxis()->CenterTitle();
  tofh12->Draw("P");
  TText *t1=new TText();
  t1->SetTextFont(32);
  t1->SetTextColor(2);
  t1->SetTextSize(0.05);
  strcpy(text,"164ps/pad");
  t1->DrawText(-0.3,200.,text);
  
  c1->cd(2);
  gPad->SetGrid();
  gStyle->SetOptStat(11);
  gStyle->SetOptFit(111);
  tofh13->SetMarkerStyle(20);
  tofh13->SetMarkerSize(0.5);
  tofh13->Fit("gaus","","",-1.1,1.1);
  tofh13->GetXaxis()->SetTitle("(TimeL3-TimeL1)-(TimeL4-Timel2) [ns]");
  tofh13->GetXaxis()->CenterTitle();
  tofh13->Draw("P");
  TText *t2=new TText();
  t2->SetTextFont(32);
  t2->SetTextColor(2);
  t2->SetTextSize(0.05);
  strcpy(text,"152ps/pad");
  t2->DrawText(-0.3,70.,text);
  
  c1->cd(3);
  gPad->SetGrid();
  gStyle->SetOptStat(11);
  gStyle->SetOptFit(111);
  tofh14->SetMarkerStyle(20);
  tofh14->SetMarkerSize(0.5);
  tofh14->Fit("gaus","","",0.8,1.2);
  tofh14->GetXaxis()->SetTitle("Beta (Velocity/LightVelocity)");
  tofh14->GetXaxis()->CenterTitle();
  tofh14->Draw("P");
  TText *t3=new TText();
  t3->SetTextFont(32);
  t3->SetTextColor(2);
  t3->SetTextSize(0.05);
  strcpy(text,"Resolution 5.6% at Beta=1");
  t3->DrawText(0.53,300.,text);
  
  c1->cd(4);
  gPad->SetGrid();
  gStyle->SetOptStat(11);
  gStyle->SetOptFit(111);
  tofh15->SetMarkerStyle(20);
  tofh15->SetMarkerSize(0.5);
  tofh15->Fit("gaus","","",0.84,1.16);
  tofh15->GetXaxis()->SetTitle("Beta (Velocity/LightVelocity)");
  tofh15->GetXaxis()->CenterTitle();
  tofh15->Draw("P");
  TText *t4=new TText();
  t4->SetTextFont(32);
  t4->SetTextColor(2);
  t4->SetTextSize(0.05);
  strcpy(text,"Resolution 4.4% at Beta=1");
  t4->DrawText(0.53,1000.,text);
}
//-----------------------------------------------------------------------
//
Double_t tofbetfit(Double_t *x,Double_t *par){//to fit beta-resol
  Double_t fitval(0);
  fitval=par[0]*exp(-pow((x[0]-par[2])/2/par[3],2))+par[1]*exp(-pow((x[0]-par[2])/2/par[4],2));
  return fitval;
}
//
void TofCalibPG::TofAnalys2Out(Int_t mode){
  char title[127];
  char hname[127];
  char text[127];
  cout<<"---------> Beta-fit(1) for TOF-paddles subsets :"<<endl;
  TCanvas *c1=new TCanvas("c1","Beta resolution",1);
  c1->Divide(2,2);
  c1->cd(1);
  gPad->SetGrid();
  gStyle->SetOptStat(11);
  gStyle->SetOptFit(111);
  tofh20->SetMarkerStyle(20);
  tofh20->SetMarkerSize(0.5);
  tofh20->Fit("gaus","","",0.9,1.1);
  tofh20->GetXaxis()->SetTitle("TofVelocity/LightVelocity");
  tofh20->GetXaxis()->CenterTitle();
  tofh20->Draw("P");
//  TText *t1=new TText();
//  t1->SetTextFont(32);
//  t1->SetTextColor(2);
//  t1->SetTextSize(0.05);
//  strcpy(text,"Resolution 5.6% at Beta=1");
//  t1->DrawText(0.53,300.,text);
  
  c1->cd(2);
  gPad->SetGrid();
  gStyle->SetOptStat(11);
  gStyle->SetOptFit(111);
  tofh21->SetMarkerStyle(20);
  tofh21->SetMarkerSize(0.5);
  tofh21->Fit("gaus","","",0.9,1.1);
  tofh21->GetXaxis()->SetTitle("TofVelocity/LightVelocity");
  tofh21->GetXaxis()->CenterTitle();
  tofh21->Draw("P");
  
  c1->cd(3);
  gPad->SetGrid();
  gStyle->SetOptStat(11);
  gStyle->SetOptFit(111);
  tofh22->SetMarkerStyle(20);
  tofh22->SetMarkerSize(0.5);
  tofh22->Fit("gaus","","",0.9,1.1);
  tofh22->GetXaxis()->SetTitle("TofVelocity/LightVelocity");
  tofh22->GetXaxis()->CenterTitle();
  tofh22->Draw("P");
  
  c1->cd(4);
  gPad->SetGrid();
  gStyle->SetOptStat(11);
  gStyle->SetOptFit(111);
  tofh23->SetMarkerStyle(20);
  tofh23->SetMarkerSize(0.5);
  tofh23->Fit("gaus","","",0.9,1.1);
  tofh23->GetXaxis()->SetTitle("TofVelocity/LightVelocity");
  tofh23->GetXaxis()->CenterTitle();
  tofh23->Draw("P");

//  cout<<"---------> Beta-fit(2) for TOF-paddles subsets :"<<endl;
//  TCanvas *c2=new TCanvas("c2","Beta resolution",1);
//  c2->Divide(2,2);
//  c2->cd(1);
//  gPad->SetGrid();
//  gStyle->SetOptStat(11);
//  gStyle->SetOptFit(111);
//  tofh25->SetMarkerStyle(20);
//  tofh25->SetMarkerSize(0.5);
//  tofh25->Fit("gaus","","",0.9,1.1);
//  tofh25->GetXaxis()->SetTitle("TofVelocity/LightVelocity");
//  tofh25->GetXaxis()->CenterTitle();
//  tofh25->Draw("P");
//  c2->cd(2);
//  gPad->SetGrid();
//  gStyle->SetOptStat(11);
//  gStyle->SetOptFit(111);
//  tofh26->SetMarkerStyle(20);
//  tofh26->SetMarkerSize(0.5);
//  tofh26->Fit("gaus","","",0.9,1.1);
//  tofh26->GetXaxis()->SetTitle("TofVelocity/LightVelocity");
//  tofh26->GetXaxis()->CenterTitle();
//  tofh26->Draw("P");

//  TCanvas *c3=new TCanvas("c3","Beta Map",1);
//  c3->Divide(1,1);
//  c3->cd(1);
//  gPad->SetGrid();
//  tofh24->SetStats(kFALSE);
//  tofh24->SetMinimum(0.9);
//  tofh24->SetMaximum(1.1);
//  tofh24->Draw("COLZ");
  
  cout<<"---------> Beta-fit(public) for TOF-paddles subsets :"<<endl;
  TCanvas *c4=new TCanvas("c4","Beta resolution",1);
  c4->Divide(1,1);
  c4->cd(1);
  gPad->SetGrid();
//  gStyle->SetOptStat(11);
  tofh27->SetStats(kFALSE);
//  gStyle->SetOptFit(111);
  tofh27->SetMarkerStyle(20);
  tofh27->SetMarkerSize(0.9);
/*
  TF1 *p2fun=new TF1("betafit",tofbetfit,0.8,1.2,5);//range(l,h),Npar
  p2fun->SetParNames("w1","w2","aver","sig1","sig2");
  p2fun->SetParLimits(0,0.,10000.);
  p2fun->SetParLimits(1,0.,10000.);
  p2fun->SetParLimits(2,0.95,1.05);
  p2fun->SetParLimits(3,0.01,0.06);
  p2fun->SetParLimits(4,0.01,0.2);
  p2fun->SetParameter(0,4000);
  p2fun->SetParameter(1,0);
  p2fun->SetParameter(2,0);
  p2fun->SetParameter(3,0.05);
  p2fun->SetParameter(4,0.05);
//    p2fun->GetParameters(par)
  tofh27->Fit("betafit");
*/  
//  
  tofh27->Fit("gaus","","",0.9,1.1);
  tofh27->GetXaxis()->SetTitle("TOFvelocity/LightVelocity");
  tofh27->GetXaxis()->CenterTitle();
  tofh27->Draw("P");
  TText *t1=new TText();
  t1->SetTextFont(32);
//  t1->SetTextColor(2);
  t1->SetTextSize(0.04);
  strcpy(text,"Resolution 4.0%");
  t1->DrawText(0.958,500.,text);
  
//  TCanvas *c5=new TCanvas("c5","Track Impact point(TofPl1)",1);
//  c5->Divide(2,2);
//  c5->cd(1);
//  gPad->SetGrid();
//  gStyle->SetOptStat(11);
//  tofh28->Draw("COLZ");
  
  TCanvas *c6=new TCanvas("c6","TOF/TRK LongCooMatch",1);
  c6->Divide(2,2);
  for(int i=0;i<4;i++){
    c6->cd(1+i);
    gPad->SetGrid();
    gStyle->SetOptStat(1111111);
    if(i==0)tofh30->Draw();
    if(i==1)tofh31->Draw();
    if(i==2)tofh32->Draw();
    if(i==3)tofh33->Draw();
  }
    
//  TCanvas *c7=new TCanvas("c7","TOF/TRK TrsCooMatch",1);
//  c7->Divide(2,2);
//  for(int i=0;i<4;i++){
//    c7->cd(1+i);
//    gPad->SetGrid();
//    gStyle->SetOptStat(1111111);
//    if(i==0)tofh34->Draw();
//    if(i==1)tofh35->Draw();
//    if(i==2)tofh36->Draw();
//    if(i==3)tofh37->Draw();
//  }
}
//
//------------------
Int_t TofCalibPG::GetTofSensorTemper(int lay, int side, int pnt, int chain, int mode, Float_t &temper){
// lay=1-4, side=1-2, point=0,1-4(0->Aver(1-3),(1-3)->indiv.pnts, 4->SFEC, chain=1,2(A,B), mode=1
// For indiv.point: retval==retval_of_GetData:
//return  0   success
//        1  no elementname found
//        2  no nodename found
//        3  no dt and/or st found
//        4  outside of bound method 1
//        5  outside of bounds methods 0,1
//        6  element table empty
//        7  wrong method
//       -1  internal error
//
// For averaging: retval=0(ok); =-n(n=1,2,3 is number of missing points); =-4(wrong input) 
//sens.numbers vs il,is,ipnt(1-3,4=sfec):
  int SensNumber[4][2][4]={
                       3, 2, 1, 0,   6, 5, 4, 7,   //<--L1(s1,s2)
		      11,10, 8, 9,  15,13,12,14,   //<--L2(s1,s2)
		       1, 2, 3, 0,   4, 5, 6, 7,   //<--L3(s1,s2)
		       9,10,11, 8,  12,13,14,15};  //<--L4(s1,s2)
// ElemNames [sens#] (+16 for JPD):
  char ElemNames[32][22]={
                          "M-2X:0:TOF-1 SFEC_00", //(0)<--- MPD
			  "M-2X:1:TOF-1 106n1",   //(1)
			  "M-2X:2:TOF-1 104n1",   //(2)
			  "M-2X:3:TOF-1 102n1",   //(3)
			  "M-2X:4:TOF-1 108p2",   //(4)
			  "M-2X:5:TOF-1 106p2",   //(5)
			  "M-2X:6:TOF-1 104p2",   //(6)
			  "M-2X:7:TOF-1 SFEC_10", //(7)
			  "M-2X:8:TOF-2 208n2",   //(8)
			  "M-2X:9:TOF-2 SFEC_11", //(9)
			  "M-2X:10:TOF-2 204n1",  //(10)
			  "M-2X:11:TOF-2 201n1",  //(11)
			  "M-2X:12:TOF-2 208p2",  //(12)
			  "M-2X:13:TOF-2 204p1",  //(13)
			  "M-2X:14:TOF-2 SFEC_01",//(14)
			  "M-2X:15:TOF-2 201p1",  //(15)
			  "TOF-3 SFEC_30",         //(0)<--- JPD
			  "TOF-3 302n1",           //(1)
			  "TOF-3 305n2",           //(2)
			  "TOF-3 309n2",           //(3)
			  "TOF-3 301p2",           //(4)
			  "TOF-3 305p2",           //(5)
			  "TOF-3 309p2",           //(6)
			  "TOF-3 SFEC_20",         //(7)
			  "TOF-4 SFEC_31",         //(8)
			  "TOF-4 402n2",           //(9)
			  "TOF-4 404n2",           //(10)
			  "TOF-4 406n2",           //(11)
			  "TOF-4 401p1",           //(12)
			  "TOF-4 404p1",           //(13)
			  "TOF-4 406p1",           //(14)
			  "TOF-4 SFEC_21"          //(15)
			                       };
// NodeNames [chain]:
  char NodeNames[4][9]={
                        "USCM_M_A", //top
			"USCM_M_B",
			"JPD_A",    //bot
			"JPD_B"
                       };
//
  Int_t retval(-5);
  int rvarr[3]={0,0,0};
  Float_t avtemp(0);
  int ngpnt(0);
  vector<float> var;
  int methode(1);
  char elemname[22];
  char nodename[9];
  int sensnum;
  int bot(0);
  if(lay>2)bot=1;
//
  cout<<"<-- TempRequest for lay/side="<<lay<<" "<<side<<" chain="<<chain<<endl;
//---
  if(pnt>0 && pnt<=4){//<---indiv.sensors
    sensnum=SensNumber[lay-1][side-1][pnt-1];
    strcpy(elemname,ElemNames[sensnum+16*bot]);
    strcpy(nodename,NodeNames[chain-1+2*bot]);
    retval=getsetup()->fSlowControl.GetData(elemname,UTime(),Frac(),var,methode,nodename);
    temper=var[0];
//  cout<<" point:"<<pnt<<" sensn="<<sensnum<<" elemname="<<elemname<<" nodename="<<nodename<<" retval="<<retval<<" temper="<<temper<<endl;
    return retval;
  }
//---
  else if(pnt==0){//<--- side average
    for(int pt=0;pt<3;pt++){
      sensnum=SensNumber[lay-1][side-1][pt];
      strcpy(elemname,ElemNames[sensnum+16*bot]);
      strcpy(nodename,NodeNames[chain-1+2*bot]);
      retval=getsetup()->fSlowControl.GetData(elemname,UTime(),Frac(),var,methode,nodename);
//  cout<<"Average: point:"<<pnt<<" sensn="<<sensnum<<" elemname="<<elemname<<" nodename="<<nodename<<" retval="<<retval<<" temper="<<var[0]<<endl;
      rvarr[pt]=retval;
      if(retval==0){
        avtemp+=var[0];
	ngpnt+=1;
      }
    }
    if(ngpnt>0){
      temper=avtemp/ngpnt;
      if(ngpnt==3)return 0;
      if(ngpnt<3){
        cout<<"<--- GetTofSensorTemper::Warning-Average over Npositions="<<ngpnt<<" retvals="<<rvarr[0]<<"/"<<rvarr[1]<<"/"<<rvarr[2]<<endl;
        return -(3-ngpnt);
      }
    }
    else{
      cout<<"<--- GetTofSensorTemper::Error-Average failed, all 3 positions are bad, retvals="<<rvarr[0]<<"/"<<rvarr[1]<<"/"<<rvarr[2]<<endl;
      temper=-273;
      return -3; 
    }
  }
//---
  else{
    cout<<"<--- GetTofSensorTemper::Error-invalid input for sensor position "<<pnt<<endl;
    return -4;
  }
//
  return retval;
}
//-----------------------------------------
//Tdlv-functions:
//
void TofCalib::inittd(){ // ----> initialization for TDIF-calibration
//
cout<<"<---------- InitTdlv-calib"<<endl;
 for(int i=0;i<kTofNBars;i++){
    for(int j=0;j<kTdLBins;j++){
      _tdiff[i][j]=0.;
      _tdif2[i][j]=0.;
      _clong[i][j]=0.;
      _nevnt[i][j]=0;
    }
  }
}
//---
void TofCalib::filltd(Int_t il,Int_t ib, Float_t td, Float_t co){//--->fill arrays
//co-abs.long.coord=loc.coo(neglect by Honeycomb x/y-shifts)
  Int_t chan,nbin,btyp;
  Float_t bin,hlen,col;
//
  hlen=0.5*brlen(il,ib);
  btyp=brtype(il,ib);//1->...
  bin=2*hlen/_nbins[btyp-1];
// convert abs.coord. to local one:
  col=co;
  if(kTofCalibPrint[0]>0){
    if((il+1)==1 && (ib+1)==4)tftdh1[0]->Fill(col,td,1.);
    if((il+1)==2 && (ib+1)==4)tftdh1[1]->Fill(col,td,1.);
    if((il+1)==3 && (ib+1)==5)tftdh1[2]->Fill(col,td,1.);
    if((il+1)==4 && (ib+1)==4)tftdh1[3]->Fill(col,td,1.);
  }
  if((col < -(hlen-3)) || (col >= (hlen-3)))goto Exit;// cr.point is out of range(paddle length)
  if(fabs(td)>19.)goto Exit;//remove obviously unrealistic td
  nbin=Int_t(floor((col+hlen)/bin));
  if(ib==0 || (ib+1)==kNtofb[il]){//trepezoidal counters
    if(nbin<=2 || nbin>=(_nbins[btyp-1]-3))goto Exit;//do not use 1st/last 3 bins
  }
  else{
    if(nbin<=0 || nbin>=(_nbins[btyp-1]-1))goto Exit;//do not use 1st/last bins
  }
  chan=barseqn(il,ib);
  if(chan<0 || chan>33){
    cout<<"<---- filltd:-F- WrongChannel,il/ib="<<il+1<<" "<<ib+1<<" ch="<<chan<<endl;
    goto Exit;
  }
  if(_nevnt[chan][nbin]<999){
    _clong[chan][nbin]+=col;
    _tdiff[chan][nbin]=_tdiff[chan][nbin]+td;
    _tdif2[chan][nbin]+=(td*td);
    _nevnt[chan][nbin]+=1;
  }
Exit:
  nbin=0;
}
//--- 
Int_t TofCalib::fittd(){//--->Tdelv-calib: get the slope,td0,chi2
  Int_t lspflg(1);//0/1->use single/array  for Lspeed
  Int_t il,ib,chan,nb,btyp,nev,bins,binsl[4];
  integer ich;
  Float_t bin,len,co,t,t2,dis,sig,sli,meansl(0),bintot(0),speedl,avsll[4];
  Double_t sl[kTofNBars],t0[kTofNBars],sumc,sumc2,sumt,sumt2,sumct,sumid,chi2[kTofNBars];
  Float_t tdf[kTdLBins];
  Int_t gchan,gsbins;
  Float_t gsbchan; 
  char fname[80];
  char frdate[30];
  char in[2]="0";
  char inum[11];
  char vers1[3]="MC";
  char vers2[3]="RD";
  char fext[20];
  Int_t cfvn;
  UInt_t StartRun,overid,verid;
  time_t StartTime;
  Int_t cstatus=1;//bad
  Double_t t0d[34]={
                    11.36,  11.73,  12.21,  12.02,  12.58,  13.07,  13.10,  13.45, 
                   -13.98, -13.18, -12.30, -10.86, -11.89, -11.82, -11.24, -10.47, 
                    -8.30,  -8.27,  -9.70, -10.25, -11.26, -10.79, -11.46, -11.67, -13.23, -12.68, 
                    -9.00, -10.18,  -9.51, -10.80, -10.73, -11.67, -11.99, -12.08
		                                                                 };//def.t0 

//
//--> get run/time of the first event(set at 1st event valid-stage using related AMSEvent values) 
//
  StartRun=RunPar::StartRun;
  StartTime=time_t(StartRun);
  strcpy(frdate,asctime(localtime(&StartTime)));
//
//--> create name for output file
// 
  if (kUseNewSlew) strcpy(fname,"TofTdelv2");
  else strcpy(fname,"TofTdelv");
  if(RunPar::GetMCF()){//MC
    strcat(fname,vers1);
    overid=CFlistC[3];
    verid=overid+1;// new=old+1
    sprintf(fext,"%d",verid);//MC-versn
    CFlistC[3]=verid;//update Tdelv-calib version in static store
  }
  else{
    strcat(fname,vers2);
    overid=CFlistC[2];
    if(overid==StartRun)verid=overid+1;//for safety when rerun over the same Data
    else verid=StartRun;
    sprintf(fext,"%d",verid);//RD-Run# = UTC-time of 1st "on-board" event
    CFlistC[2]=verid;//update Tdelv-calib version in static store
  }
  strcat(fname,".");
  strcat(fname,fext);
//
  cout<<"-----> Start Tdlv-calib fit..."<<endl;
//
  chan=0;
  gchan=0;
  gsbchan=0;
  for(il=0;il<4;il++){
    avsll[il]=0.;
    binsl[il]=0;
    for(ib=0;ib<kNtofb[il];ib++){
//      ich=barseqn(il,ib);
      len=brlen(il,ib);
      btyp=brtype(il,ib);//1->...
      bin=len/_nbins[btyp-1];
      sumc=0;
      sumt=0;
      sumct=0;
      sumc2=0;
      sumt2=0;
      sumid=0;
      bins=0;
      gsbins=0;
      sl[chan]=0;
      t0[chan]=t0d[chan];//def.t0
      chi2[chan]=0;
      if(kTofCalibPrint[1]>1){
        cout<<"<===== Events/bin in lay/bar="<<(il+1)<<" "<<(ib+1)<<"  ===> ";
        for(nb=0;nb<_nbins[btyp-1];nb++)cout<<_nevnt[chan][nb]<<" ";
        cout<<endl;
      }
      for(nb=0;nb<kTdLBins;nb++)tdf[nb]=0.;
//
      for(nb=0;nb<_nbins[btyp-1];nb++){//<--- bin-loop
        nev=_nevnt[chan][nb];
	if(nev>0){
	  _tdiff[chan][nb]/=nev;// mean td
          _tdif2[chan][nb]/=nev;//mean td2
	  _clong[chan][nb]/=nev;// mean co
	  tdf[nb]=geant(_tdiff[chan][nb]);
	}
        if(nev>=20){//min.cut on event number in bin
	  gsbins+=1;
          t=_tdiff[chan][nb];
          co=_clong[chan][nb];
	  t2=_tdif2[chan][nb];
          dis=t2-(t*t);
          if(dis>=0. && kTofCalibPrint[0]>0)tftdh2->Fill(sqrt(dis),1.);
          if(dis>=0. && sqrt(dis)<=0.8){//max.cut on bin-rms
            dis=dis/(nev-1);
            sumc+=(co/dis);
            sumt+=(t/dis);
            sumid+=(1/dis);
            sumct+=(co*t/dis);
            sumc2+=(co*co/dis);
            sumt2+=(t*t/dis);
            bins+=1;
          }
	  else{
	    cout<<"   BadBinRms:td/td2="<<t<<" "<<t2<<" dis="<<dis<<" coo="<<co<<" nev="<<nev<<endl;
	  }
        }
      }// ---> end of bins loop
//
      if(kTofCalibPrint[0]>2){
        for(nb=0;nb<_nbins[btyp-1];nb++)cout<<_tdiff[chan][nb]<<" ";
        cout<<endl;
        for(nb=0;nb<_nbins[btyp-1];nb++)cout<<_tdif2[chan][nb]<<" ";
        cout<<endl;
        for(nb=0;nb<_nbins[btyp-1];nb++)cout<<_clong[chan][nb]<<" ";
        cout<<endl;
      }
      gsbchan+=(Float_t(gsbins)/_nbins[btyp-1]);//% of good statistics bins in given channel
      if(kTofCalibPrint[0]>2)cout<<"  ---> NgoodBins="<<bins<<" sumc/sumt="<<sumc<<" "<<sumt<<" sumct="<<sumct<<" sumc2/t2="<<sumc2<<" "<<sumt2<<" sumis="<<sumid<<endl;
//
      if(bins>=5){
        t0[chan]=(sumt*sumc2-sumct*sumc)/(sumid*sumc2-(sumc*sumc));
        sl[chan]=(sumct*sumid-sumc*sumt)/(sumid*sumc2-(sumc*sumc));
        chi2[chan]=sumt2+t0[chan]*t0[chan]*sumid+sl[chan]*sl[chan]*sumc2
         -2*t0[chan]*sumt-2*sl[chan]*sumct+2*t0[chan]*sl[chan]*sumc;
        chi2[chan]/=(bins-2);
        if(chi2[chan]<18. &&
                     fabs(sl[chan])>0.05 &&
                     fabs(sl[chan])<0.08){//only good for averaging
          bintot+=1;
          meansl+=sl[chan];
          avsll[il]+=sl[chan];
          binsl[il]+=1;
	  gchan+=1;
        }
        if(kTofCalibPrint[0]>0  && fabs(sl[chan])>0.){
	  tftdh3->Fill(chi2[chan],1.);
	  tftdh4->Fill(fabs(sl[chan]),1.);
	}
	if(kTofCalibPrint[0]>1)cout<<"  ---> Chi2="<<chi2[chan]<<"  t0/slop="<<t0[chan]<<" "<<sl[chan]<<endl;
      }
      else{
        cout<<"  ---> Not enought bins for L/B="<<il+1<<" "<<ib+1<<" nbins="<<bins<<endl;
//	calstat[il][ib][0]=1000000;//bad Tdlv-calib
//	calstat[il][ib][1]=1000000;//bad Tdlv-calib
      }
      chan+=1;
    }//<------ end of bar loop
//
    if(binsl[il]>0)avsll[il]=fabs(avsll[il])/Float_t(binsl[il]);
    if(avsll[il]>0.)avsll[il]=1./avsll[il];
    else avsll[il]=15.2;//def.value
  }//<------ end of layer loop
//
  if((Float_t(gchan))/chan>0.88){//<4 bad channels(bars)
    cstatus=0;;//ok
  }
  if(bintot>0)meansl/=bintot; // mean slope
  if(meansl!=0)speedl=fabs(1./meansl);// mean light speed
//
  cout<<endl<<endl;
 if(kTofCalibPrint[0]>1){//<--- print tables:
  cout<<"TofCalibTdlv_fit: for bar 1-12  Tdiff@center (ns):"<<endl<<endl;
  chan=0;
  for(il=0;il<4;il++){
    for(ib=0;ib<kNtofb[il];ib++){
      cout<<" "<<t0[chan];
      chan+=1;
    }
    cout<<endl;
  }
  cout<<endl;
  cout<<"TofCalibTdlv_fit: for bar 1-12  Light speed (cm/ns):"<<endl<<endl;
  cout<<"Average Lspeed = "<<speedl<<"  , vs layer : "<<avsll[0]<<" "<<avsll[1]
                                   <<" "<<avsll[2]<<" "<<avsll[3]<<endl<<endl;
  chan=0;
  for(il=0;il<4;il++){
    for(ib=0;ib<kNtofb[il];ib++){
      sli=0;
      if(sl[chan]!=0)sli=1./fabs(sl[chan]);
      cout<<" "<<sli;
      chan+=1;
    }
    cout<<endl;
  }
  cout<<endl;
  cout<<"TofCalibTdlv_fit: for bar 1-12  Chi2 :"<<endl<<endl;
  chan=0;
  for(il=0;il<4;il++){
    for(ib=0;ib<kNtofb[il];ib++){
      cout<<" "<<chi2[chan];
      chan+=1;
    }
    cout<<endl;
  }
  cout<<endl;
 }
//
  if(kTofCalibPrint[0]>0){//<--print histogr  
    TCanvas *td1=new TCanvas("TD1","Tof SideTimeDiff vs Trk ImpPoint",1);
    td1->Divide(2,2);
    td1->cd(1);
    gPad->SetGrid();
    tftdh1[0]->SetMarkerStyle(20);
    tftdh1[0]->SetMarkerSize(0.25);
    tftdh1[0]->Draw("P");
    td1->cd(2);
    gPad->SetGrid();
    tftdh1[1]->SetMarkerStyle(20);
    tftdh1[1]->SetMarkerSize(0.25);
    tftdh1[1]->Draw("P");
    td1->cd(3);
    gPad->SetGrid();
    tftdh1[2]->SetMarkerStyle(20);
    tftdh1[2]->SetMarkerSize(0.25);
    tftdh1[2]->Draw("P");
    td1->cd(4);
    gPad->SetGrid();
    tftdh1[3]->SetMarkerStyle(20);
    tftdh1[3]->SetMarkerSize(0.25);
    tftdh1[3]->Draw("P");
    td1->Update();
    
    TCanvas *td2=new TCanvas("TD2","BinRms/Chi2/Slope",1);
    td2->Divide(2,2);
    td2->cd(1);
    gPad->SetGrid();
    gStyle->SetOptStat(1111111);
    tftdh2->Draw();
    td2->cd(2);
    gPad->SetGrid();
    gStyle->SetOptStat(1111111);
    tftdh3->Draw();
    td2->cd(3);
    gPad->SetGrid();
    gStyle->SetOptStat(1111111);
    tftdh4->Draw();
    td2->Update();
  }
//
// ---> write mean light speed and Tdif's to file:
// 
  ofstream tcfile(fname,ios::out|ios::trunc);
  if(!tcfile){
    cerr<<"TofCalibTdlv:error opening file for output"<<fname<<'\n';
    return 1;
  }
  cout<<"Open file for TDLV-calibration output, fname:"<<fname<<'\n';
  cout<<"Lspeed and individual Tdif's will be written !"<<'\n';
  cout<<" First run used for calibration is "<<StartRun<<endl;
  cout<<" Date of the first event : "<<frdate<<endl;
  tcfile.setf(ios::fixed);
  tcfile.width(6);
  tcfile.precision(2);// precision for Lspeed and Tdiff's
  if(lspflg){
    chan=0;
    for(il=0;il<4;il++){
      for(ib=0;ib<kNtofb[il];ib++){
        if(sl[chan]!=0.)tcfile << 1./Float_t(fabs(sl[chan]))<<" ";
        else tcfile << avsll[il]<<" ";
	chan+=1;
      }
      tcfile << endl;
    }
  }
  else tcfile << speedl<<endl;
  tcfile << endl;
//
  chan=0;
  for(il=0;il<4;il++){
    for(ib=0;ib<kNtofb[il];ib++){
      tcfile << Float_t(t0[chan])<<" ";
      chan+=1;
    }
    tcfile << endl;
  }
  tcfile << endl;
//
  tcfile << 12345 << endl;//endoffile label
  tcfile << endl;
  tcfile << endl<<"======================================================"<<endl;
  tcfile << endl<<" First run used for calibration is "<<StartRun<<endl;
  tcfile << endl<<" Date of the first event : "<<frdate<<endl;
  tcfile.close();
  cout<<"TofCalibTdlv:output file closed !"<<endl<<endl<<endl;
  return cstatus;
}
//-----------------------------------------
//Tzslw-functions:
//
void TofCalib::inittz(){ // ----> initialization for TDIF-calibration
//
  cout<<"<---------- InitTzslw-calib"<<endl;
  Int_t i,j,il,ib;
  if (!kUseNewSlew) slope=0.;
  s0=0.;
  s1=0.;
  s4=0.;
  s8=0.;
  events=0.;
  for(ib=0;ib<kTofLBars;ib++){
    for(il=0;il<4;il++){
      tzero[il][ib]=0.;
      s3[il][ib]=0.;
      s30[il][ib]=0.;
    }
    for(i=0;i<3;i++){
      s6[i][ib]=0.;
      s15[i][ib]=0.;
    }
    s7[ib]=0.;
    s16[ib]=0.;
    for(j=0;j<kTofLBars;j++){
      s12[ib][j]=0.;
      s13[ib][j]=0.;
      s14[ib][j]=0.;
    }
  }

  if (kUseNewSlew) {
    //
    //---> open file with TOF Slewing Parameters
    //
   Char_t fname[80];
    Int_t endflab;
    Char_t ext[80];
    //    strcpy(fname,"/f2users/lquadran/tofrfcal_new/TofSlwSlopeRD.");
    //    sprintf(ext,"%d",kTofSlwRunID);
    strcpy(fname,"TofSlwSlopeRD.");
    sprintf(ext,"%d",CFlistC[7]);
    strcat(fname,ext);
    cout << "      Opening TofSlwSlope-file: "<<fname<<endl; 
    ifstream crfile(fname,ios::in); // open file for reading
    if(!crfile){
      cout <<"<---- Error: Missing TofSlwSlope-file !!? "<<fname<<endl;
      exit(1);
    }
    int seqnum=0;
    for(il=0;il<kNtofl;il++){
      for (int is=0; is<2; is++) {
	for(ib=0;ib<kNtofb[il];ib++){
	  crfile >> TofTzSlope[seqnum];
	  cout << TofTzSlope[seqnum] << " ";
	  seqnum++;
	}
	cout << endl;
      }
    }    
    crfile >> endflab;//read endfile-label
    crfile.close();
    if(endflab==12345){
      cout<<"      TofSlwSlope-file is successfully read !"<<endl;
    }
    else{
      cout<<"<---- Error: broken structure in TofSlwSlope-file !!!"<<endl;
      exit(1);
    }
  }

//
}
//---
void TofCalib::filltz(Int_t ib[4],Double_t dtr[3],Double_t du[3]){
// To fill arrays, used by FCN :
  Int_t i,j,ibr,refl,tarl;
// ---> note: imply missing layer has ib=-1; corresponding diffs=0
// du[i]=dtr[i]=0 simultaneously (when any r->2,3,4 pair is missing)
//
  refl=(kTofBref/100);//reflayer(1)/2, =2 means 1st layer in not functional, 2nd used as ref
  ibr=ib[refl-1];//bar in ref.layer
  if((ib[0]<0 && refl==1) || (ib[1]<0 && refl==2)){//ref-layer have to be present
    cout<<"<--- TzslwInit: Error - ref.layer missing:refl="<<refl<<" BarL1/2="<<ib[0]+1<<" "<<ib[1]+1<<endl;
    goto Exit;
  }
  if(refl==2)ib[0]=-1;//to avoid summing L1 when ref.layer=2 (important for s30-counting)
//cout<<" refl/b="<<refl<<" "<<ibr+1<<endl;
//cout<<"ib:"<<endl<<ib[0]<<" "<<ib[1]<<" "<<ib[2]<<" "<<ib[3]<<endl;
//cout<<"dtr:"<<endl<<dtr[0]<<" "<<dtr[1]<<" "<<dtr[2]<<endl;
//cout<<"du:"<<endl<<du[0]<<" "<<du[1]<<" "<<du[2]<<endl;
  events+=1.;
//---
    for(i=0;i<kNtofl;i++)if(ib[i]>=0)s30[i][ib[i]]+=1.;
    for(int ipr=0;ipr<kNtofl-1;ipr++){
      if(dtr[ipr]!=0){//given pair is exist
        tarl=2+ipr;//target layer in given pair(refl->tarl)
        s0+=1;//counts all !=0 pairs (r->2,r->3,r->4)
        s1+=dtr[ipr]*dtr[ipr];
        s3[tarl-1][ib[tarl-1]]+=1;
        s3[refl-1][ibr]+=1;
        s4+=du[ipr]*du[ipr];
        s6[ipr][ib[tarl-1]]+=dtr[ipr];
        s7[ibr]+=dtr[ipr];
        s8+=dtr[ipr]*du[ipr];
        if(tarl==2)s12[ibr][ib[tarl-1]]+=1.;//r->2 (s12=0(not filled) if refl=2 because dtr[0]=0 & tarl>2)
        if(tarl==3)s13[ibr][ib[tarl-1]]+=1.;//r->3
        if(tarl==4)s14[ibr][ib[tarl-1]]+=1.;//r->4
	s15[ipr][ib[tarl-1]]+=du[ipr];
	s16[ibr]+=du[ipr];
      }
    }
Exit:
    refl=0;//dummy action
}
//---
//
void TofCalib::mfuntz(Int_t &np, Double_t grad[], Double_t &f, Double_t x[], Int_t flg){
// This is standard Minuit FCN for Tzslw-calib:
//
  Int_t i,j,il,ib,seqnum;
  Int_t id,ibt,idr,ibtr;
  Int_t refl=(kTofBref/100);//reflayer(1)/2, =2 means 1st layer in not functional, 2nd used as ref
  Int_t nprsmx=4-refl;//max pairs in sums (=3 for normal case lref=1, =2 if lref=2)
//  static int first(0);
  Double_t f3(0.),f6(0.),f7(0.),f8(0.),f10(0.);
  Double_t f1[kNtofl];
  Double_t f2[kNtofl-1];
  Double_t f9[kNtofl-1];
  Float_t w,tz,tzr;
  Char_t fname[80];
  Char_t frdate[30];
  Char_t in[2]="0";
  Char_t inum[11];
  Char_t vers1[3]="MC";
  Char_t vers2[3]="RD";
  Char_t fext[20];
  Int_t cfvn;
  UInt_t StartRun,overid,verid;
  time_t StartTime;
  Int_t dig;
//
//
  for(i=0;i<kNtofl;i++)f1[i]=0.;
  for(i=0;i<kNtofl-1;i++){
    f2[i]=0.;
    f9[i]=0.;
  }
  //
  seqnum=0;
  for(il=0;il<kNtofl;il++){
    f1[il]=0.;
    for(ib=0;ib<kNtofb[il];ib++){
      if (kUseNewSlew) f1[il]+=s3[il][ib]*x[seqnum]*x[seqnum];
      else f1[il]+=s3[il][ib]*x[1+seqnum]*x[1+seqnum];//l=1,4 (s3[0][ib]=0 when ref.layer=2) 
      seqnum+=1;
    }
  }
  //
  for(i=0;i<kNtofl-1;i++){//loop over pairs Lr->Lk, k=(2),3,4
    f2[i]=0.;    
    il=i+1;
    if(il>(refl-1)){//skip missing pair Lr->L2 if refl=2
      for(ib=0;ib<kNtofb[il];ib++) {
	if (kUseNewSlew) f2[i]+=s6[i][ib]*x[barseqn(il,ib)];
	else f2[i]+=s6[i][ib]*x[1+barseqn(il,ib)];//l=(2),3,4
      }
    }
  }
  //
  for(ib=0;ib<kNtofb[refl-1];ib++){
    if (kUseNewSlew) f3+=s7[ib]*x[barseqn(refl-1,ib)];
    else f3+=s7[ib]*x[1+barseqn(refl-1,ib)];//l=r
  }
  //
  if(refl==1){// f6=0(skipped) if refl=2
    for(i=0;i<kNtofb[refl-1];i++){
      for(j=0;j<kNtofb[1];j++) {
	if (kUseNewSlew) f6+=s12[i][j]*x[barseqn(refl-1,i)]*x[barseqn(1,j)];
	else f6+=s12[i][j]*x[1+barseqn(refl-1,i)]*x[1+barseqn(1,j)];//Lr*(2)
      }
    }
  }
  //
  for(i=0;i<kNtofb[refl-1];i++){//loop over bars in refl
    for(j=0;j<kNtofb[2];j++) {
      if (kUseNewSlew) f7+=s13[i][j]*x[barseqn(refl-1,i)]*x[barseqn(2,j)];
      else f7+=s13[i][j]*x[1+barseqn(refl-1,i)]*x[1+barseqn(2,j)];//Lr*3
    }
  }
  //
  for(i=0;i<kNtofb[refl-1];i++){
    for(j=0;j<kNtofb[3];j++) {
      if (kUseNewSlew) f8+=s14[i][j]*x[barseqn(refl-1,i)]*x[barseqn(3,j)];
      else f8+=s14[i][j]*x[1+barseqn(refl-1,i)]*x[1+barseqn(3,j)];//Lr*4
    }
  }
  //
  for(i=0;i<kNtofl-1;i++){ 
    f9[i]=0.;
    il=i+1;
    if(il>(refl-1)){//skip missing pair Lr->L2 if refl=2
      for(ib=0;ib<kNtofb[il];ib++) {
	if (kUseNewSlew) f9[i]+=s15[i][ib]*x[barseqn(il,ib)];
	else f9[i]+=s15[i][ib]*x[1+barseqn(il,ib)];//l=(2),3,4
      }
    }
  }
  //
  for(ib=0;ib<kNtofb[refl-1];ib++) {
    if (kUseNewSlew) f10+=s16[ib]*x[barseqn(refl-1,ib)];
    else f10+=s16[ib]*x[1+barseqn(refl-1,ib)];//l=r
  }
  //
  if (kUseNewSlew) {
    f=s1
      +f1[0]+f1[1]+f1[2]+f1[3]
      +s4
      +2.*(f2[0]+f2[1]+f2[2])
      -2.*f3
      +2.*s8
      -2.*(f6+f7+f8)
      +2.*(f9[0]+f9[1]+f9[2])
      -2.*f10;
  }
  else {
    f=s1
      +f1[0]+f1[1]+f1[2]+f1[3]
      +s4*x[0]*x[0]
      +2.*(f2[0]+f2[1]+f2[2])
      -2.*f3
      +2.*s8*x[0]
      -2.*(f6+f7+f8)
      +2.*(f9[0]+f9[1]+f9[2])*x[0]
      -2.*f10*x[0];
  }
  //----------------------------------
  if(flg==3){
    cout<<"----> TofCalib::fittz: Final call to FCN(user actions):"<<endl;
    //
    //--> get run/time of the first event(set at 1st event valid-stage using related AMSEvent values) 
    //
    StartRun=RunPar::StartRun;
    StartTime=time_t(StartRun);
    strcpy(frdate,asctime(localtime(&StartTime)));
    //
    //--> create name for output file
    // 
    if (kUseNewSlew) strcpy(fname,"TofTzero");
    else strcpy(fname,"TofTzslw");
    if(RunPar::GetMCF()){
      strcat(fname,vers1);
      overid=CFlistC[4];
      verid=overid+1;//new MC-versn = old+1
      sprintf(fext,"%d",verid);
      CFlistC[4]=verid;//update Tzslw-calib version in static store
    }
    else{
      strcat(fname,vers2);
      overid=CFlistC[3];
      if(overid==StartRun)verid=overid+1;//for safety when rerun over the same Data
      else verid=StartRun;
      sprintf(fext,"%d",verid);//RD-Run# = UTC-time of 1st "on-board" event
      CFlistC[3]=verid;//update Tzslw-calib version in static store
    }
    strcat(fname,".");
    strcat(fname,fext);
    //
    // write parameters to ext.file:
    resol=-1.;
    resol1=-1.;
    if(f>=0. && events>0)resol=sqrt(f/events);
    if(f>=0. && s0>0)resol1=sqrt(f/s0);//to have rms/pair
    //
    cout<<"<---- Open file for Tzslw-calibration output, fname:"<<fname<<'\n';
    ofstream tcfile(fname,ios::out|ios::trunc);
    if(!tcfile){
      cout<<"<---- error opening file for output"<<fname<<'\n';
      exit(8);
    }
    if (kUseNewSlew) cout<<"      Individual T0's will be written !"<<'\n';
    else cout<<"      Slope and individual T0's will be written !"<<'\n';
    cout<<"      First run used for calibration is "<<StartRun<<endl;
    cout<<"      Date of the first event : "<<frdate<<endl;
    tcfile.setf(ios::fixed);
    tcfile.width(6);
    if (!kUseNewSlew) {
      tcfile.precision(2);// precision for slope
      slope=geant(x[0]);
      tcfile << slope<<endl;
    }
    tcfile.precision(3);// precision for T0
    seqnum=0;
    for(il=0;il<kNtofl;il++){
      for(ib=0;ib<kNtofb[il];ib++){
	if (kUseNewSlew) tzero[il][ib]=geant(x[seqnum]);
        else tzero[il][ib]=geant(x[1+seqnum]);
        tcfile <<" "<<tzero[il][ib];
	seqnum+=1;
      }
      tcfile << endl;
    }
    tcfile << endl;
    //
    tcfile << 12345 << endl;//endoffile label
    tcfile << endl;
    tcfile << endl<<"======================================================"<<endl;
    tcfile << endl<<" First run used for calibration is "<<StartRun<<endl;
    tcfile << endl<<" Date of the first event : "<<frdate<<endl;
    tcfile.close();
    cout<<"<==== TofCalib(Tzslw) completed, output file is written !!!"<<endl;
  }
}
//------
Int_t TofCalib::fittz(){  // Tzslw-calibr. fit procedure, f.results->slope,tzero[][]->file
  Int_t i,id,ii,j,ier,il,ib,npar(kTofNBars),nparr,seqnum;
  Int_t cstatus(1);//bad
//
  Char_t pnm[6];
  Char_t p1nam[6];
  Char_t p2nam[3];
  Char_t inum[11];
  Char_t in[2]="0";
  Int_t ifit[kTofNBars+1];
  Char_t pnam[kTofNBars+1][6];
  Int_t ifit2[kTofNBars];
  Char_t pnam2[kTofNBars][6];
  Double_t argl[10];
  Int_t iargl[10];
  Double_t start[kTofNBars+1],step[kTofNBars+1];
  Double_t plow[kTofNBars+1],phigh[kTofNBars+1];
  Double_t start2[kTofNBars],step2[kTofNBars];
  Double_t plow2[kTofNBars],phigh2[kTofNBars];
//
  strcpy(p1nam,"tslop");
  strcpy(p2nam,"tz");
  strcpy(inum,"0123456789");
//
  Float_t deft0[34]={//side-A (1305815610)
    2.365,  0.557,  1.036,  0.000,  0.051,  0.874,  0.875,  2.294,
   -4.148, -3.549, -3.476, -2.150, -2.652, -3.146, -3.269, -3.642,
   -7.969, -9.731, -7.885, -9.239, -8.825, -8.767, -8.990, -9.444, -9.562, -8.831,
   -4.554, -6.586, -6.596, -6.792, -6.307, -6.629, -5.726, -5.167
                                                                };
/*
  Float_t deft0[34]={//side-A
    2.375,  0.558,  1.067,  0.000,  0.053,  0.874,  0.887,  2.356,
   -4.038, -3.405, -3.353, -2.022, -2.514, -3.021, -3.139, -3.477,
   -7.837, -9.596, -7.757, -9.094, -8.693, -8.633, -8.800, -9.257, -9.445, -8.661,
   -4.514, -6.509, -6.575, -6.722, -6.245, -6.545, -5.631, -5.039
                                                                };
//
  Float_t deft0[34]={//side-B
    2.555, 0.806, 0.986, 0.000, 0.026, 0.817, 0.595, 2.235,
   -4.533, -3.572, -4.021, -2.311, -3.332, -3.349, -3.556, -3.931,
   -7.638, -9.507, -7.957, -8.946, -9.034, -8.413, -9.141, -8.623, -9.817, -7.985,
   -4.391, -5.820, -6.536, -6.319, -6.257, -6.288, -5.965, -4.597
                                                                };
*/
// -----------> set parameter defaults:
  if (kUseNewSlew) {
    ii=0;
    for(il=0;il<kNtofl;il++){
      for(ib=0;ib<kNtofb[il];ib++){
	id=(il+1)*100+ib+1;
	start2[ii]=deft0[ii];//tempor def.T0's from current calibration
	if(id == kTofBref && (kTofBref/100)==1)
	  start2[ii]=kTofTzref;//def.T0(0.) for ref.counter
	step2[ii]=1.;
	plow2[ii]=-40.;
	phigh2[ii]=40.;
	ifit2[ii]=0;//means fix
	strcpy(pnm,p2nam);
	in[0]=inum[il];
	strcat(pnm,in);
	i=ib/10;
	j=ib%10;
	in[0]=inum[i];
	strcat(pnm,in);
	in[0]=inum[j];
	strcat(pnm,in);
	strcpy(pnam2[ii],pnm);
	ii+=1;//use sequential solid numbering of counters
      }
    }
  }
  else {
    //slope:
    strcpy(pnam[0],p1nam); // for slope
    //  start[0]=kTofDefTzslop;// def. slope 
    if(kUseHe4Calib) start[0]=kTofDefTzslopHe;
    else start[0]=kTofDefTzslop;
    step[0]=1.;
    plow[0]=0.;
    phigh[0]=40.;
    ifit[0]=kTofTzSlopRel;// fix/release slope 
    if(kTofTzFitm==2)ifit[0]=0;

    //T0's
    ii=0;
    for(il=0;il<kNtofl;il++){
      for(ib=0;ib<kNtofb[il];ib++){
	id=(il+1)*100+ib+1;
	//      start[ii+1]=TOF2Brcal::scbrcal[il][ib].gettzero();// def.T0's from current calibration
	start[ii+1]=deft0[ii];//tempor def.T0's from current calibration
	if(id == kTofBref && (kTofBref/100)==1)
	  start[ii+1]=kTofTzref;//def.T0(0.) for ref.counter
	step[ii+1]=1.;
	plow[ii+1]=-40.;
	phigh[ii+1]=40.;
	ifit[ii+1]=0;//means fix
	strcpy(pnm,p2nam);
	in[0]=inum[il];
	strcat(pnm,in);
	i=ib/10;
	j=ib%10;
	in[0]=inum[i];
	strcat(pnm,in);
	in[0]=inum[j];
	strcat(pnm,in);
	strcpy(pnam[ii+1],pnm);
	ii+=1;//use sequential solid numbering of counters
      }
    }
  }
// ------------> release T0-parameters with good statistics:
// 
  printf("====> TofCalib::fittz:Collected events (total) : %9.0f\n",events);
  printf("      Collected events per sc. bar : \n");
  for(il=0;il<kNtofl;il++){
    for(ib=1;ib<=kNtofb[il];ib++){
      if(ib%kNtofb[il] !=0)
                     printf("% 7.0f",s30[il][ib-1]);
      else
                     printf("% 7.0f\n",s30[il][ib-1]);
    }
  }
//
  nparr=0;
  seqnum=0;
  Bool_t missrefc(kFALSE);
  for(il=0;il<kNtofl;il++){
    for(ib=0;ib<kNtofb[il];ib++){
      id=(il+1)*100+ib+1;
      if(s30[il][ib]>=10.){
        if(kTofTzFitm==0 
	     || (kTofTzFitm==1 && (ib>0 || (ib+1)<kNtofb[il]))
	     || (kTofTzFitm==2 && (ib==0 || (ib+1)==kNtofb[il]))){
	  if (kUseNewSlew) ifit2[seqnum]=1;
          else ifit[1+seqnum]=1;//bar with high statist.-> release
	  nparr+=1;
	}
        if(id == kTofBref){
	  if (kUseNewSlew) ifit2[seqnum]=0;
          else ifit[1+seqnum]=0;//fix, if ref counter
	  nparr-=1;
        }
      }
      else{
//        if(calstat[il][ib][0]==1000000)calstat[il][ib][0]=3000000;//add bad Tzslw-calib.stat(in add. to bad Tdlv)
//	else if(calstat[il][ib][0]==0)calstat[il][ib][0]=2000000;//set bad Tzslw-calib.stat
//        if(calstat[il][ib][1]==1000000)calstat[il][ib][1]=3000000;//add bad Tzslw-calib.stat(in add. to bad Tdlv)
//	else if(calstat[il][ib][1]==0)calstat[il][ib][1]=2000000;//set bad Tzslw-calib.stat
        if(id == kTofBref){
          cout<<" <--- Too low statistics in ref.counter "<<id<<" "<<s30[il][ib]<<'\n';
	  missrefc=kTRUE;
        }
        if (kUseNewSlew) ifit2[seqnum]=0;
	else ifit[1+seqnum]=0;//bar with low statist.-> fix
      }
      seqnum+=1;
    }
  }
//
  printf("      Sufficient statistics to fit  %d counters(+1ref) \n",nparr);
//
  if(nparr<2 || missrefc){
    cout<<"<---- Not enough counters for Minuit-Fit !!!"<<'\n';
    goto exit;// not enough counters for FIT -> return
  }
//  TOF2JobStat::cqual(1,0)=geant(nparr+1)/seqnum;//% counters with good stat (for cal-quality evaluation)
//
  if (kUseNewSlew) npar=seqnum;
  else npar=seqnum+1;//T0's_of_counters + slope_parameter
//
// ------------> initialize parameters for Minuit:
//
  TMinuit *gMinuit = new TMinuit(npar);//init minuit for max npar params
//  gMinuit->SetPrintLevel(0);
  gMinuit->SetPrintLevel(-1);

  gMinuit->SetFCN(TofCalib::mfuntz);
  
  argl[0]=1;//UP
  gMinuit->mnexcm("SET ERR", argl,1,ier);//set chi2 methode(0.5->likelih)
  for(i=0;i<npar;i++){//<--- preset/fix parameters
    if (kUseNewSlew) strcpy(pnm,pnam2[i]);
    else strcpy(pnm,pnam[i]);
    ier=0;
    if (kUseNewSlew) gMinuit->mnparm(i,pnm,start2[i],step2[i],plow2[i],phigh2[i],ier);
    else gMinuit->mnparm(i,pnm,start[i],step[i],plow[i],phigh[i],ier);
    if(ier!=0){
      if (kUseNewSlew) cout<<"<---- Fatal: Param-init problem for par-id="<<pnam2[i]<<'\n';
      else cout<<"<---- Fatal: Param-init problem for par-id="<<pnam[i]<<'\n';
      goto exit;
    }
    argl[0]=i+1;
    if((kUseNewSlew && ifit2[i]==0) || (!kUseNewSlew && ifit[i]==0)) {
      ier=0;
      gMinuit->mnexcm("FIX",argl,1,ier);
      if(ier!=0){
	if (kUseNewSlew) cout<<"<---- Fatal: Param-fix problem for par-id="<<pnam2[i]<<'\n';
        else cout<<"<---- Fatal: Param-fix problem for par-id="<<pnam[i]<<'\n';
        goto exit;
      }
    }
  }
//----
  argl[0]=1500;//max.calls
  argl[1]=1.;//tolerance(stop minim, when vert.dist < 0.001*toler*UP)
  ier=0;
  gMinuit->mnexcm("MIGRAD",argl,2,ier);
  if(ier!=0){
    cout<<"<---- Fatal: MIGRAD problem !"<<'\n';
    goto exit;
  }
//  
  argl[0]=3;
  ier=0;
  gMinuit->mnexcm("CALL FCN",argl,1,ier);
  if(ier!=0){
    cout<<"<---- Fatal: final CALL_FCN problem !"<<'\n';
    goto exit;
  }
//--->print results:
  Double_t amin,edm,errdef;
  Int_t nvpar,nparx,icstat;
  if(kTofCalibPrint[1]>0){
    gMinuit->mnstat(amin,edm,errdef,nvpar,nparx,icstat);
    gMinuit->mnprin(3,amin);
  }
//----
  printf("<==== TofCalib::fittz: Minuit ended with parameters:\n");
  printf("      -----------------------------\n");
  printf("      Resolution(ns) : %6.3e\n",resol);
  printf("      Resolution1(ns): %6.3e\n",resol1);
  if (!kUseNewSlew) printf("      Slope          : %6.3e\n",slope);
//
  if(resol1<0.35)cstatus=0;//good final resolution
exit:
  cout<<"----> TofCalib:fittz is over !"<<endl;
  return cstatus;
}
//-----------------------------------------
void TofCalib::TzslwHist(){  // Tzslw-calibr. histograms
  TCanvas *tz1=new TCanvas("TZ1","Tzslw preselection-1",1);
  tz1->Divide(2,2);
  tz1->cd(1);
  gPad->SetGrid();
  gStyle->SetOptStat(1111111);
  tftzh1->Draw();
  tz1->cd(2);
  gPad->SetGrid();
  gStyle->SetOptStat(1111111);
  tftzh2->Draw();
  tz1->cd(3);
  gPad->SetGrid();
  gStyle->SetOptStat(1111111);
  tftzh3->Draw();
  tz1->cd(4);
  gPad->SetGrid();
  gStyle->SetOptStat(1111111);
  tftzh4->Draw();

  TCanvas *tz1a=new TCanvas("TZ1a","Tzslw preselection-1a",1);
  tz1a->Divide(2,2);
  tz1a->cd(1);
  gPad->SetGrid();
  gStyle->SetOptStat(1111111);
  tftzh9->Draw();
  tz1a->cd(2);
  gPad->SetGrid();
  gStyle->SetOptStat(1111111);
  tftzh10->Draw();
  
  
  TCanvas *tz2=new TCanvas("TZ2","Tzslw preselection-2",1);
  tz2->Divide(2,2);
  tz2->cd(1);
  gPad->SetGrid();
  gStyle->SetOptStat(1111111);
  tftzh5[0]->Draw();
  tz2->cd(2);
  gPad->SetGrid();
  gStyle->SetOptStat(1111111);
  tftzh5[1]->Draw();
  tz2->cd(3);
  gPad->SetGrid();
  gStyle->SetOptStat(1111111);
  tftzh5[2]->Draw();
  tz2->cd(4);
  gPad->SetGrid();
  gStyle->SetOptStat(1111111);
  tftzh5[3]->Draw();
  
  TCanvas *tz3=new TCanvas("TZ3","Tzslw preselection-3",1);
  tz3->Divide(2,2);
  tz3->cd(1);
  gPad->SetGrid();
  gStyle->SetOptStat(1111111);
  tftzh6[0]->Draw();
  tz3->cd(2);
  gPad->SetGrid();
  gStyle->SetOptStat(1111111);
  tftzh6[1]->Draw();
  tz3->cd(3);
  gPad->SetGrid();
  gStyle->SetOptStat(1111111);
  tftzh6[2]->Draw();
  tz3->cd(4);
  gPad->SetGrid();
  gStyle->SetOptStat(1111111);
  tftzh6[3]->Draw();
  
  TCanvas *tz4=new TCanvas("TZ4","Tzslw preselection-4",1);
  tz4->Divide(2,2);
  tz4->cd(1);
  gPad->SetGrid();
  gStyle->SetOptStat(1111111);
  tftzh7->Draw();
  tz4->cd(2);
  gPad->SetGrid();
  gStyle->SetOptStat(1111111);
  tftzh7a->Draw();
  tz4->cd(3);
  gPad->SetGrid();
  gStyle->SetOptStat(1111111);
  tftzh8->Draw();
  
  TCanvas *tz5=new TCanvas("TZ5","Tzslw preselection-5",1);
  tz5->Divide(2,2);
  tz5->cd(1);
  gPad->SetGrid();
  gStyle->SetOptStat(111);
  tftzh11->SetMarkerStyle(20);
  tftzh11->SetMarkerSize(0.25);
  tftzh11->Draw("P");
  tz5->cd(2);
  gPad->SetGrid();
  gStyle->SetOptStat(111);
  tftzh12->SetMarkerStyle(20);
  tftzh12->SetMarkerSize(0.25);
  tftzh12->Draw("P");
  tz5->cd(3);
  gPad->SetGrid();
  gStyle->SetOptStat(111);
//  tftzh13->SetMarkerStyle(20);
//  tftzh13->SetMarkerSize(0.25);
  tftzh13->Draw();
  tz5->cd(4);
  gPad->SetGrid();
  gStyle->SetOptStat(111);
//  tftzh14->SetMarkerStyle(20);
//  tftzh14->SetMarkerSize(0.25);
  tftzh14->Draw();
  
  TCanvas *tz6=new TCanvas("TZ6","Tzslw preselection-6",1);
  tz6->Divide(2,2);
  tz6->cd(1);
  gPad->SetGrid();
  gStyle->SetOptStat(111);
  tftzh15->SetMarkerStyle(20);
  tftzh15->SetMarkerSize(0.5);
  tftzh15->SetMinimum(0.6);
  tftzh15->SetMaximum(1.1);
  tftzh15->Draw("P");
  tz6->cd(2);
  gPad->SetGrid();
  gStyle->SetOptStat(111);
  tftzh16->SetMarkerStyle(20);
  tftzh16->SetMarkerSize(0.5);
  tftzh16->SetMinimum(0.6);
  tftzh16->SetMaximum(1.1);
  tftzh16->Draw("P");
  tz6->cd(3);
  gPad->SetGrid();
  gStyle->SetOptStat(111);
  tftzh17->SetMarkerStyle(20);
  tftzh17->SetMarkerSize(0.5);
  tftzh17->SetMinimum(-0.2);
  tftzh17->SetMaximum(0.2);
  tftzh17->Draw("P");
  tz6->cd(4);
  gPad->SetGrid();
  gStyle->SetOptStat(111);
  tftzh18->SetMarkerStyle(20);
  tftzh18->SetMarkerSize(0.5);
  tftzh18->SetMinimum(-0.2);
  tftzh18->SetMaximum(0.2);
  tftzh18->Draw("P");
  
  TCanvas *tz7=new TCanvas("TZ7","Tzslw preselection-7",1);
  tz7->Divide(1,2);
  tz7->cd(1);
  gPad->SetGrid();
  gStyle->SetOptStat(111);
  tftzh19->SetMarkerStyle(20);
  tftzh19->SetMarkerSize(0.5);
  tftzh19->SetMinimum(-0.2);
  tftzh19->SetMaximum(0.2);
  tftzh19->Draw("P");
  tz7->cd(2);
  gPad->SetGrid();
  gStyle->SetOptStat(111);
  tftzh20->SetMarkerStyle(20);
  tftzh20->SetMarkerSize(0.5);
  tftzh20->SetMinimum(-0.2);
  tftzh20->SetMaximum(0.2);
  tftzh20->Draw("P");

}
//========================================== Ampl-calib zone:
//
void TofCalib::initam(){ // ----> initialization for AMPL-calibration 
  Int_t i,j,il,ib,ii,jj,id,nadd,nbnr,chan;
  Float_t blen,dd,bw,bl,bh,hll,hhl;
  Int_t stbns(4);// number of standard bins
  Float_t bwid[4]={5.,6.,7.,8.};// bin width (first "stbns" bins, closed to the edge(sum=26cm)
//                                (other bin width should be < or about 10cm) 
  Char_t htit1[60];
  Char_t htit2[60];
  Char_t htit3[7];
  Char_t inum[11];
  Char_t in[2]="0";
//
  strcpy(inum,"0123456789");
//
  for(i=0;i<kTofChanMx;i++){
    nevenc[i]=0;
    gains[i]=0.;
    a2dr[i]=0;
    a2dr2[i]=0;
    neva2d[i]=0;
    nevdgn[i]=0;
    for(int ip=0;ip<kTofPmtMX;ip++){
      d2sdr[i][ip]=0;
      d2sdr2[i][ip]=0;
    }
    for(j=0;j<kAmEvStore;j++){
      amchan[i][j]=0.;
    }
  }
//
  for(i=0;i<kTofBTypes;i++){//bar type loop
    nevrfc[i]=0;
    ammrfc[i]=0;
    nrefb[i]=0;
  for(j=0;j<kAmEvStore;j++){
    arefb[i][j]=0.;
  }
  }
cout<<"<---------- InitAmpl-calib"<<endl;
//
  for(i=0;i<kAmBrtBins;i++){
    nevenb1[i]=0;
    nevenb2[i]=0;
    btamp[0][i]=0.;
    btamp[1][i]=0.;
  for(j=0;j<kAmEvStore;j++){
    ambin1[i][j]=0.;
    ambin2[i][j]=0.;
  }
  }
  for(i=0;i<kTofBTypes;i++){ // prepare profile bin width array
    id=rbls[i];
    if(id==0)continue;//skip dummy bar types(<-> ref.bars)
    il=id/100-1;
    ib=id%100-1;
    blen=brlen(il,ib);
    nbnr=nprbn[i];//real tot.numb. of bins
    dd=0.;
    for(j=0;j<stbns;j++){ // bin width for the first/last "stbns" standard ones
      dd+=bwid[j];
      profb[i][j]=bwid[j];
      profb[i][nbnr-j-1]=bwid[j];
    }
    nadd=nbnr-2*stbns;// number of additional bins of THE SAME WIDTH !
    bw=(blen-2*dd)/nadd;// width of additional bins
cout<<"<--- BrType="<<i+1<<" BarLen="<<blen<<" Nbins="<<nbnr<<" equal bars width="<<bw<<endl;
    for(j=0;j<nadd;j++){ // complete bin width array
      profb[i][stbns+j]=bw;
    }
// for(int ibn=0;ibn<nbnr;ibn++)cout<<" "<<profb[i][ibn];
// cout<<endl;
  }
//
  for(i=0;i<kTofBTypes;i++){ // prepare mid.points positions (loc.r.s) of profile bins
    id=rbls[i];
    if(id==0)continue;//skip dummy bar types(<-> ref.bars)
    il=id/100-1;
    ib=id%100-1;
    blen=brlen(il,ib);
    nbnr=nprbn[i];//real tot.numb. of bins
    dd=0.;
    for(j=0;j<nbnr;j++){
      profp[i][j]=dd+0.5*profb[i][j]-0.5*blen;// -blen/2 to go to loc.r.s.
      dd+=profb[i][j];
    }
  }
//
  for(i=0;i<kTofBTypes;i++){// book hist. for longit.imp.point distr.(ref.bar only)
    strcpy(htit1,"Impact point distr.for ref.bar(LBB) ");
    id=rbls[i];
    if(id==0)continue;//skip dummy bar types(<-> ref.bars)
    il=id/100;
    in[0]=inum[il];
    strcat(htit1,in);
    ib=id%100;
    ii=ib/10;
    in[0]=inum[ii];
    strcat(htit1,in);
    jj=ib%10;
    in[0]=inum[jj];
    strcat(htit1,in);
    blen=brlen(il-1,ib-1);
    bl=-0.5*blen;
    bh=bl+blen;
    aprofp[i][0]=50.;//defaults param.(A) for A-profile fit(s1)
    aprofp[i][1]=195.;//              (Lh)
    aprofp[i][2]=0.;//                (Wl)
    aprofp[i][3]=10.;//               (Ll)
    aprofp[i][4]=0.;//                (Abk)
    aprofp[i][5]=10.;//               (Lbk)
    aprofp[i][6]=aprofp[i][0];//defaults param. for A-profile fit(s2)
    aprofp[i][7]=aprofp[i][1];
    aprofp[i][8]=aprofp[i][2];
    aprofp[i][9]=aprofp[i][3];
    aprofp[i][10]=aprofp[i][4];               
    aprofp[i][11]=aprofp[i][5];               
  }
}
//-------
void TofCalib::filla2dg(Int_t il, Int_t ib, Float_t cin, Float_t ain[2], Float_t din[2][kTofPmtMX]){
// (prog.to fill arrays for a2dr and Dgain[ip] calculations)
//
  Int_t ip,is,chnum,npmts,ngdp[2],chn;
  Float_t dsum[2],mina,maxa,mind,maxd;
  Float_t r,cbin(12.5);//sentral area +-cbin to have unform PM-signals sharing
  Float_t ccut;
//
  npmts=npmtps(il,ib);
  chnum=2*barseqn(il,ib);//sequential channels numbering (s=1)
//
  for(is=0;is<2;is++){//shift to the middle of the adc-bin
    if(ain[is]>0)ain[is]+=0.5;
    for(ip=0;ip<npmts;ip++){
      if(din[is][ip]>0)din[is][ip]+=0.5;
    }
  }
//
    for(is=0;is<2;is++){//<-- side loop
      if(is==0 && cin<-cbin)continue;//take far away impact from given side
      if(is==1 && cin>cbin)continue;
      chn=chnum+is;
      dsum[is]=0;
      ngdp[is]=0;
      mina=200;//tempor(to have(at default_a2d=5-10) Ad1,2,3>20 to avoid nonlin of beg.of D-signals scale)
      maxa=kTofAdcMX-50;//to avoid ovfls(anode saturation area)
      if(ain[is]>mina && ain[is]<maxa){// Ah ok
        for(ip=0;ip<npmts;ip++){
          mind=20;//~4sig
	  maxd=kTofAdcMX-mind;
          if(din[is][ip]>mind && din[is][ip]<maxd)ngdp[is]+=1;//Dh[ip] ok
        }
      }
      if(ngdp[is]==npmts){// <-- all adc's OK -> fill arrays
        for(ip=0;ip<npmts;ip++){
	  dsum[is]+=din[is][ip];
        }
	r=ain[is]/dsum[is];
	tfamh20[chn]->Fill(log10(dsum[is]),log10(ain[is]),1.);//inst.Log(Ah) vs Log(Dyn)
        if(kTofCalibPrint[2]>0){
	  tfamh1->Fill(r,1.);//inst.Ah/Dh(pm-sum) for bar/sides
	} 
	a2dr[chn]+=r;
	a2dr2[chn]+=r*r;
	neva2d[chn]+=1;
	dsum[is]/=geant(npmts);//Dh average over npmts
	for(ip=0;ip<npmts;ip++){
	  r=din[is][ip]/dsum[is];
          if(kTofCalibPrint[2]>0){
            if(ip==0)tfamh2->Fill(r,1.);//inst.Dg(pm) rel.gain for all bar/sides, pm=1
            if(ip==1)tfamh3->Fill(r,1.);//inst.Dg(pm) rel.gain for all bar/sides, pm=2
            if(ip==2)tfamh3a->Fill(r,1.);//inst.Dg(pm) rel.gain for all bar/sides, pm=3
	  }
	  d2sdr[chn][ip]+=r;
	  d2sdr2[chn][ip]+=r*r;
	}
	nevdgn[chn]+=1;
      }//-->endof "adc's OK" check
    }//--->endof side-loop
}
//-------
void TofCalib::fillam(Int_t il, Int_t ib, Float_t am[2], Float_t coo){
//   ---> program to accumulate data for anodes relat.gains-calibration
//        and atten.length calib(long.bins for ref.bars):
  Int_t i,id,idr,idh,ibt,btyp,nbn,nb,nbc,isb,chan,nev,bchan;
  Float_t r,bl,bh,qthrd;
  Float_t cbin(12.5);// centr. bin half-width for gain calibr.
//
  isb=barseqn(il,ib);//bar sequential number(0->...)
  chan=2*isb;//side-1 seq. number(2*isb+1 for side-2)
  ibt=brtype(il,ib);// bar type (1-11)
  if(ibt<1 || ibt>11){
    cout<<"<------- fillam: wrong btype="<<ibt<<endl;
    goto Exit;
  }
  btyp=ibt-1;
  id=100*(il+1)+ib+1;
  idr=rbls[btyp];// ref.bar id for given bar
  if(idr==0){
    cout<<"<------- TofCalib::fillam:illegal ref.bar id ! "<<id<<endl;
    goto Exit;
  }
  nbn=nprbn[btyp];// number of long.bins
//
  nbc=-1;
  for(nb=0;nb<nbn;nb++){ // define bin number for current coo (for profiles fitting)
    bl=profp[btyp][nb]-0.5*profb[btyp][nb];
    bh=bl+profb[btyp][nb];
    if(coo>=bl && coo<bh){
      nbc=nb;
      break;
    }
  }
  if(nbc<0){//<--- impact coo was out of bins range: show limits and coo 
    bl=profp[btyp][0]-0.5*profb[btyp][0];
    bh=profp[btyp][nbn-1]+0.5*profb[btyp][nbn-1];
    if((bl-coo)>2. || (coo-bh)>2.){//big "outrange"->give warning...
      cout<<"<-------- fillam:-W- LongCrosCoo out_of_Blen !,coo="<<coo<<" l/h="<<bl<<" "<<bh<<" btyp="<<ibt<<" brlen="<<brlen(il,ib)<<endl;
    }
    goto Exit;
  }
  bchan=kAmProfBins*btyp+nbc;
//                             ---> fill arrays/hist. for gains:
//
  if(fabs(coo) < cbin){// select only central incidence(+- cbin cm)
    nev=nevenc[chan];
    if(nev<kAmEvStore && am[0]>0){  
      amchan[chan][nev]=am[0];
      nevenc[chan]+=1;
    }
    nev=nevenc[chan+1];
    if(nev<kAmEvStore && am[1]>0){  
      amchan[chan+1][nev]=am[1];
      nevenc[chan+1]+=1;
    }
  }
//                             ---> fill profile arrays/hist. for ref. bars:
  if(id == idr){// only for ref. sc. bars
    nev=nevenb1[bchan];
    if(nev<kAmEvStore && am[0]>0){
      ambin1[bchan][nev]=am[0];
      nevenb1[bchan]+=1;
    }
    nev=nevenb2[bchan];
    if(nev<kAmEvStore && am[1]>0){
      ambin2[bchan][nev]=am[1];
      nevenb2[bchan]+=1;
    }
    if(kTofCalibPrint[2]>0){
      if(kTofCalibPrint[2]>1)tfamh4[btyp]->Fill(coo,1.);// longit.imp.point distribution
      if(ibt == 2 && fabs(coo) < cbin){
        tfamh5->Fill(am[0],1.);// Q-distr. for ref.bar type=2, s=1
        tfamh6->Fill(am[1],1.);// Q-distr. for ref.bar type=2, s=2
      }
    }
  }
Exit:
  cbin=0;//dummy action
}
//---------------
void TofCalib::fillabs(Int_t il, Int_t ib, Float_t am[2], Float_t coo){
//            ---> program to accumulate data for abs.normalization:
//
  Int_t i,id,idr,ibt,btyp,nev;
  Float_t amt,mcut[2];
  Float_t cbin(15.);// centr. bin half-width for abs-norm calibr.
//
  ibt=brtype(il,ib);// bar type (1-11)
  btyp=ibt-1;
  id=100*(il+1)+ib+1;
  idr=rbls[btyp];// ref.bar id for given bar
//
  if(id != idr)goto Exit;//only for ref.counters
  if(fabs(coo) > cbin)goto Exit;// select only central incidence(+- cbin cm)
//
  amt=(am[0]+am[1]);
//
//
  nevrfc[btyp]+=1;
  ammrfc[btyp]+=amt;
  nev=nrefb[btyp];
  if(nev<kAmEvStore){
    arefb[btyp][nev]=(am[0]+am[1]);
    nrefb[btyp]+=1;
  }
Exit:
  ibt=1;
}
//----------------
void TofCalib::AmplHist(){  // Ampl-calibr. histograms
  
  TCanvas *am1=new TCanvas("AM1","Ampl preselection-1",1);
  am1->Divide(2,2);
  am1->cd(1);
  gPad->SetGrid();
  gStyle->SetOptStat(1111111);
  tfamh1->Draw();
  am1->cd(2);
  gPad->SetGrid();
  gStyle->SetOptStat(1111111);
  tfamh2->Draw();
  am1->cd(3);
  gPad->SetGrid();
  gStyle->SetOptStat(1111111);
  tfamh3->Draw();
  am1->cd(4);
  gPad->SetGrid();
  gStyle->SetOptStat(1111111);
  tfamh3a->Draw();

  
  TCanvas *am2=new TCanvas("AM2","Ampl preselection-2",1);
  am2->Divide(2,2);
  am2->cd(1);
  gPad->SetGrid();
  gStyle->SetOptStat(1111111);
  tfamh5->Draw();
  am2->cd(2);
  gPad->SetGrid();
  gStyle->SetOptStat(1111111);
  tfamh6->Draw();
  
  TCanvas *am3=new TCanvas("AM3","Ampl preselection-3",1);
  am3->Divide(1,2);
  am3->cd(1);
  gPad->SetGrid();
  gStyle->SetOptStat(1111111);
  tfamh7->Draw();
  
  if(kTofCalibPrint[2]>1){
  TCanvas *am4=new TCanvas("AM4","Ampl Trk ImpPoint vs BarType",1);
  am4->Divide(3,2);
  for(int bt=0;bt<6;bt++){
    am4->cd(1+bt);
    gPad->SetGrid();
    gStyle->SetOptStat(11);
    tfamh4[bt]->Draw();
  }
  TCanvas *am4a=new TCanvas("AM4a","Ampl Trk ImpPoint vs BarType",1);
  am4a->Divide(3,2);
  for(int bt=0;bt<kTofBTypes-6;bt++){
    am4a->cd(1+bt);
    gPad->SetGrid();
    gStyle->SetOptStat(11);
    tfamh4[6+bt]->Draw();
  }
  }
  
  TCanvas *am5=new TCanvas("AM5","Ampl: AnodeGainFit",1);
  am5->Divide(1,2);
  am5->cd(1);
  gPad->SetGrid();
  gStyle->SetOptStat(1111111);
  tfamh8->Draw();
  am5->cd(2);
  gPad->SetGrid();
  gStyle->SetOptStat(1111111);
  tfamh9->Draw();

  if(kTofCalibPrint[2]>0){
  TCanvas *am6=new TCanvas("AM6","Ampl LBinFit Chi2 vs BarType",1);
  am6->Divide(3,2);
  for(int bt=0;bt<6;bt++){
    am6->cd(1+bt);
    gPad->SetGrid();
    gStyle->SetOptStat(11);
    tfamh10[bt]->Draw();
  }
  TCanvas *am6a=new TCanvas("AM6a","Ampl LBinFit Chi2 vs BarType(cont)",1);
  am6a->Divide(3,2);
  for(int bt=0;bt<kTofBTypes-6;bt++){
    am6a->cd(1+bt);
    gPad->SetGrid();
    gStyle->SetOptStat(11);
    tfamh10[6+bt]->Draw();
  }
  }
  
  if(kTofCalibPrint[2]>0){
    for(int is = 0; is < 2; is++){
    if(is==0){
      TCanvas *am7=new TCanvas("AM7","Ampl LongResponce Profiles vs BarTypes(s1)",1);
      am7->Divide(4,3);
      for(int j = 0; j < kTofBTypes; j++){
        am7->cd(j+1); 
        gPad->SetGrid();
        gStyle->SetOptStat(11);
        tfamh11[2*j+is]->Draw();
      }
      am7->Update();
    }
    if(is==1){
      TCanvas *am8=new TCanvas("AM8","Ampl LongResponce Profiles vs BarTypes(s2)",1);
      am8->Divide(4,3);
      for(int j = 0; j < kTofBTypes; j++){
        am8->cd(j+1); 
        gPad->SetGrid();
        gStyle->SetOptStat(11);
        tfamh11[2*j+is]->Draw();
      }
      am8->Update();
    }
    }
  }

  TCanvas *am9=new TCanvas("AM9","Ampl LBinFit Chi2,allBtypes/sides",1);
  am9->Divide(2,1);
  am9->cd(1);
  gPad->SetGrid();
  gStyle->SetOptStat(111111);
  tfamh12->Draw();
  am9->Update();
  
  TCanvas *am10=new TCanvas("AM10","Ampl AbsNorm CentBin-fit Chi2, allBtypes",1);
  am10->Divide(2,1);
  am10->cd(1);
  gPad->SetGrid();
  gStyle->SetOptStat(111111);
  tfamh14->Draw();
  am10->Update();
  
  
  TCanvas *am11=new TCanvas("AM11","Ampl Aver.Ah/D and relar Rms, allChannels",1);
  am11->Divide(2,1);
  am11->cd(1);
  gPad->SetGrid();
  gStyle->SetOptStat(111111);
  tfamh15->Draw();
  am11->cd(2);
  gPad->SetGrid();
  gStyle->SetOptStat(111111);
  tfamh16->Draw();
  am11->Update();
  
  TCanvas *am12=new TCanvas("AM12","Ampl Aver.Ah/D and relar Rms, allChannels",1);
  am12->Divide(2,1);
  am12->cd(1);
  gPad->SetGrid();
  gStyle->SetOptStat(111111);
  tfamh17->Draw();
  am12->cd(2);
  gPad->SetGrid();
  gStyle->SetOptStat(111111);
  tfamh18->Draw();
  am12->Update();
}
//----------------
Int_t TofCalib::fitam(){
//---> program to get final AMPL-calibration:
//
  Int_t il,ib,is,i,j,k,n,ii,jj,id,idr,btyp;
  Int_t ic,ich;
  Int_t ibt,ibn,nbnr,chan,bchan,nev,nm,nmax,nmin;
  Float_t aref[kTofBTypes][2],ar,aabs[kTofBTypes],mip2q[kTofBTypes];
  Double_t *pntr[kAmEvStore];
  Double_t workarr[kAmEvStore];
  Int_t indarr[kAmEvStore];
  Bool_t ordr(kFALSE);//to sort in increasing order
  TH1F * workhis=0;
  Double_t aver;
  Float_t step[10],pmin[10],pmax[10],sigp[10];
  Int_t nev1,nev2,npar(2);
  Char_t htit1[60];
  Char_t fname[80];
  Char_t frdate[30];
  Char_t in[2]="0";
  Char_t inum[11];
  Char_t vers1[3]="MC";
  Char_t vers2[3]="RD";
  Char_t fext[20];
  Int_t cfvn;
  UInt_t StartRun,overid,verid;
  time_t StartTime;
  int dig;
  Int_t cstatus=1;//bad
  Int_t badstage[3]={1,1,1};//(arefgn/)1=bad stage
  strcpy(inum,"0123456789");
//
//--> get run/time of the first event
//
  StartRun=RunPar::StartRun;
  StartTime=time_t(StartRun);
  strcpy(frdate,asctime(localtime(&StartTime)));
//
//--> create name for output file
// 
  if (kUseNewSlew) strcpy(fname,"TofAmplf2");
  else strcpy(fname,"TofAmplf");
  if(RunPar::GetMCF()){//MC
    strcat(fname,vers1);
    overid=CFlistC[5];
    verid=overid+1;//new MC-versn old+1
    sprintf(fext,"%d",verid);
    CFlistC[5]=verid;//update Ampl-calib version in static store
  }
  else{
    strcat(fname,vers2);
    overid=CFlistC[4];
    if(overid==StartRun)verid=overid+1;//for safety when rerun over the same Data
    else verid=StartRun;
    sprintf(fext,"%d",verid);//RD-Run# = UTC-time of 1st "on-board" event
    CFlistC[4]=verid;//update Ampl-calib version in static store
  }
  strcat(fname,".");
  strcat(fname,fext);
//
//
// ---> print "gain"-hist. (side-signals for center bin)
//
//
// ---> print "profile"-hist. (tot.signal vs long.bin for ref.counters):
//
//---------------------------------------------------------------------
// ---> Calculate(fit) most prob. ampl. for each channel (X=0):
//
  Int_t ierp,ip;
  Int_t fbadch,fgoodch;  
  Int_t ifitp[kLandfPars];
  Double_t llim,hlim,pval,perr;
  Char_t prnam[kLandfPars][6],prnm[6];
  Float_t trcut=0.92;
  Double_t arglp[10];
  Int_t iarglp[10];
  Double_t pri[kLandfPars],prs[kLandfPars],prl[kLandfPars],prh[kLandfPars],maxv;
  strcpy(prnam[0],"anor1");
  strcpy(prnam[1],"mprob");
  strcpy(prnam[2],"scalf");
  strcpy(prnam[3],"speed");
  pri[0]=10.;
  pri[1]=40.;
  pri[2]=10.;
  pri[3]=0.5;
//
  prs[0]=5.;
  prs[1]=10.;
  prs[2]=1.;
  prs[3]=0.25;
//
  prl[0]=1.;
  prl[1]=1.;
  prl[2]=0.1;
  prl[3]=0.;
//
  prh[0]=1500.;
  prh[1]=500.;
  prh[2]=100.;
  prh[3]=10.;
//
  for(i=0;i<kLandfPars;i++)ifitp[i]=1;
  ifitp[3]=0;
  cout<<endl;
  cout<<"-------------> Start Mp-fits for side signals (at X=0) for rel.gains: <--------------"<<endl;
  cout<<endl;
//
  npar=kLandfPars;
  TMinuit *gMinuit = new TMinuit(npar);//init minuit for max npar params
  gMinuit->SetPrintLevel(-1);
  gMinuit->SetFCN(TofCalib::melfun);
  arglp[0]=1;//UP
  gMinuit->mnexcm("SET ERR", arglp,1,ierp);//set chi2 methode(0.5->likelih)
  for(ip=0;ip<kLandfPars;ip++){// <----- initialize param.
    strcpy(prnm,prnam[ip]);
    ierp=0;
    gMinuit->mnparm(ip,prnm,pri[ip],prs[ip],prl[ip],prh[ip],ierp);
    if(ierp!=0){
      cout<<"Ampl: Mp-side-calib: Param-init problem for par-id="<<prnam[ip]<<'\n';
      exit(10);
    }
    arglp[0]=ip+1;
    if(ifitp[ip]==0){
      ierp=0;
      gMinuit->mnexcm("FIX",arglp,1,ierp);
      if(ierp!=0){
        cout<<"Ampl: Mp-side-calib: Param-fix problem for par-id="<<prnam[ip]<<'\n';
        exit(10);
      }
    }
  }// <----- end of param. init.
//----
  char choice[5]=" ";
  Int_t bnn,jmax;
  Int_t goodch;
  Float_t rbnn,bnw,bnl,bnh;
  ic=0;
  goodch=0;
  fbadch=0;
  fgoodch=0;
  for(il=0;il<kNtofl;il++){
    for(ib=0;ib<kNtofb[il];ib++){
      for(is=0;is<2;is++){
        nev=nevenc[ic];
        aver=0;
        if(nev>=100){
          for(k=0;k<nev;k++)workarr[k]=amchan[ic][k];//copy event-signals of chan=ic 
	  TMath::Sort(nev,workarr,indarr,ordr);//sort ampls. in increasing order
          nmax=int(floor(nev*trcut));// to keep (100*trcut)% of lowest amplitudes
          nmin=int(floor(nev*0.015));// to remove 1.5 % of lowest amplitudes
          if(nmin==0)nmin=1;
          strcpy(htit1,"X=0 signals distr. for chan(LBBS) ");
          in[0]=inum[il+1];
          strcat(htit1,in);
          ii=(ib+1)/10;
          jj=(ib+1)%10;
          in[0]=inum[ii];
          strcat(htit1,in);
          in[0]=inum[jj];
          strcat(htit1,in);
          in[0]=inum[is+1];
          strcat(htit1,in);
          bnn=20;//        <<--- select limits/binwidth for hist.
	  bnl=workarr[indarr[nmin]];
          bnh=workarr[indarr[nmax]];
          bnw=(bnh-bnl)/bnn;
          if(bnw<1.){
            bnw=1.;
            bnn=int((bnh-bnl)/bnw);
            bnh=bnl+bnw*bnn;
          }
          if(workhis)delete workhis;
          workhis=new TH1F("workhis",htit1,bnn,bnl,bnh);
          for(j=nmin;j<nmax;j++)workhis->Fill(workarr[indarr[j]],1.);
          for(j=0;j<bnn;j++)binsta[j]=workhis->GetBinContent(j+1);//j=0->underfl, j=bnn+1->overfl
          for(j=0;j<bnn;j++)bincoo[j]=bnl+0.5*bnw+bnw*j;
          maxv=0.;
          jmax=0;
          for(j=0;j<bnn;j++){// find bin with max.stat.
            if(binsta[j]>maxv){
              maxv=binsta[j];
              jmax=j;
            }
          }
          strcpy(eltit,htit1);
//---
          prl[1]=bnl;// some more realistic init.values from histogr.
          prh[1]=bnh;
          pri[1]=bnl+0.5*bnw+bnw*jmax;//Mp
          prs[1]=bnw;
          pri[0]=maxv;// A
	  prl[0]=1.;
	  prh[0]=2.*maxv;
	  prs[0]=maxv/10;
          elbt=bnn;
//---
          for(ip=0;ip<2;ip++){// <----- reinitialize these 2 parameters:
            strcpy(prnm,prnam[ip]);
            ierp=0;
            gMinuit->mnparm(ip,prnm,pri[ip],prs[ip],prl[ip],prh[ip],ierp);
            if(ierp!=0){
              cout<<"Ampl:Mp-side-calib: Param-reinit problem for par-id="<<prnam[ip]<<'\n';
              exit(10);
            }
          }// <----- end of param. reinit.
//---

//----
          arglp[0]=1500;//max.calls
          arglp[1]=1.;//tolerance(stop minim, when vert.dist < 0.001*toler*UP)
          ierp=0;
          gMinuit->mnexcm("MIGRAD",arglp,2,ierp);
          if(ierp!=0){
	    fbadch+=1;
            cout<<"<---- Ampl:CentBin Mp-fit: MIGRAD problem !"<<'\n';
            continue;
          }
//  
          arglp[0]=3;
          ierp=0;
          gMinuit->mnexcm("CALL FCN",arglp,1,ierp);
          if(ierp!=0){
	  fbadch+=1;
            cout<<"<---- Ampl:CentBin Mp-fit: final CALL_FCN problem !"<<'\n';
            continue;
          }
//--->print results:
          Double_t amin,edm,errdef;
          Int_t nvpar,nparx,icstat;
	  if(kTofCalibPrint[2]>0)tfamh8->Fill(melfunchi2,1.);
          if(kTofCalibPrint[2]>1){
            gMinuit->mnstat(amin,edm,errdef,nvpar,nparx,icstat);
//            gMinuit->mnprin(3,amin);
	    cout<<"---> amin="<<amin<<"  Nvarnpars="<<nvpar<<"  stat="<<icstat<<" Abins="<<bnn<<"  nev="<<nev<<"  nmim/max="<<nmin<<"/"<<nmax<<" chi2="<<melfunchi2<<endl;
	    
          }
          gains[ic]=elfitp[1];
	  goodch+=1;
//----
        }// ---> endof min.events check
	else{
	  fbadch+=1;
	  cout<<"<--- Ampl:AnGainFit:TooSmallNevs="<<nev<<"  for il/ib/is="<<il+1<<" "<<ib+1<<" "<<is+1<<endl;
	}
//
	ic+=1;//sequential numbering of bar sides
//
      }// ---> endof side loop
    }// ---> endof bar loop
  }// ---> endof layer loop
  cout<<"<----------- Ampl: AnGainFit finished with good/bad channels:"<<goodch<<" "<<fbadch<<" from 68 total."<<endl;
//---
//----------------------------------------------
//
// ---> extract most prob. ampl for ref.bar:
//
  Int_t gdrefb(0);
  for(i=0;i<kTofBTypes;i++){
    id=rbls[i];
    if(id==0)continue;//skip dummy bar types
    il=id/100-1;
    ib=id%100-1;
    chan=2*barseqn(il,ib);
    nev1=nevenc[chan];
    nev2=nevenc[chan+1];
    if(nev1>=200 && gains[chan]>0 && nev2>=200 && gains[chan]>0){//req.more statistics for ref.counter
      aref[i][0]=gains[chan];
      aref[i][1]=gains[chan+1];
      gdrefb+=1;
    }
    else{
      cout<<"   <--- Ampl:AnGainFit: TooSmallStat for ref.bar IL/IB="<<il+1<<" "<<ib+1<<" stat="<<nev1<<"/"<<nev2<<" g1/g2="<<gains[chan]<<" "<<gains[chan+1]<<endl;
      aref[i][0]=0;
      aref[i][1]=0;
    }
  }
  cout<<"<--- Ampl:AnGainFit: found "<<gdrefb<<" good RefBTypes from total "<<kTofBTypes<<endl;
//
// ---> calc. relative gains:
//
  ic=0;
  fbadch=0;
  for(il=0;il<kNtofl;il++){
    for(ib=0;ib<kNtofb[il];ib++){
      btyp=brtype(il,ib)-1;
      id=100*(il+1)+ib+1;
      idr=rbls[btyp];// ref.bar id for given bar
      if(idr==0)continue;//skip dummy bar types
      chan=2*barseqn(il,ib);
      for(i=0;i<2;i++){
        ar=aref[btyp][i];
        if(ar>0. && gains[chan+i]>0.)gains[chan+i]/=ar;
        else {
	  gains[chan+i]=1.;//default value
//	  calstat[il][ib][i]=1000;//tempor
          fbadch+=1; 
	}
        if((id != idr) && kTofCalibPrint[2]>0)tfamh9->Fill(gains[chan+i],1.);
	ic+=1;
      }
    }
  }
//
  if(ic>0 && Float_t(fbadch)/ic<0.9)badstage[0]=0;//ok for anodes rel.gains stage
//-------------------------------------------------------------
//
// ---> Calculate(fit) most prob. ampl. for each X-bin of ref.bars(s1/s2):
//
  Int_t j1,j2;
  pri[1]=80.;
  pri[2]=20.;
  prh[1]=1000.;
  prh[2]=100.;
  cout<<endl;
  cout<<"-------------> Start Mp-fit in RefBar bins: <--------------"<<endl;
  cout<<endl;
// ------------> initialize Minuit:
//
  npar=kLandfPars;
  TMinuit *bMinuit = new TMinuit(npar);//init minuit for max npar params
  bMinuit->SetPrintLevel(-1);
  bMinuit->SetFCN(TofCalib::melfun);
  arglp[0]=1;//UP
  bMinuit->mnexcm("SET ERR", arglp,1,ierp);//set chi2 methode(0.5->likelih)
  for(ip=0;ip<kLandfPars;ip++){// <----- initialize param.
    strcpy(prnm,prnam[ip]);
    ierp=0;
    bMinuit->mnparm(ip,prnm,pri[ip],prs[ip],prl[ip],prh[ip],ierp);
    if(ierp!=0){
      cout<<"Ampl: Mp-side-calib: Param-init problem for par-id="<<prnam[ip]<<'\n';
      exit(10);
    }
    arglp[0]=ip+1;
    if(ifitp[ip]==0){
      ierp=0;
      bMinuit->mnexcm("FIX",arglp,1,ierp);
      if(ierp!=0){
        cout<<"Ampl: Mp-side-calib: Param-fix problem for par-id="<<prnam[ip]<<'\n';
        exit(10);
      }
    }
  }// <----- end of param. init.
//----
  Int_t ngbins[kTofBTypes][2];
  for(ibt=0;ibt<kTofBTypes;ibt++){// <---loop over bar-types
    nbnr=nprbn[ibt];//real numb. of bins for bar-type ibt
    if(nbnr==0)continue;//skip dummy bar types
    for(int isd=0;isd<2;isd++){// <---side loop
    ngbins[ibt][isd]=0;
    iside=isd;
    for(ibn=0;ibn<nbnr;ibn++){ // <--- loop over longit.bins
      bchan=ibt*kAmProfBins+ibn;
      if(isd==0)nev=nevenb1[bchan];
      else nev=nevenb2[bchan];
      aver=0;
      if(nev>=80){
        if(isd==0)for(k=0;k<nev;k++)workarr[k]=ambin1[bchan][k];//pointers to event-signals of chan=bchan
        else for(k=0;k<nev;k++)workarr[k]=ambin2[bchan][k];//pointers to event-signals of chan=bchan
	TMath::Sort(nev,workarr,indarr,ordr);//sort ampls. in increasing order
        nmax=int(floor(nev*trcut));// to keep (100*trcut)% of lowest amplitudes
        nmin=int(floor(nev*0.02));// to remove 2 % of lowest amplitudes
        if(nmin==0)nmin=1;
//        for(k=nmin;k<nmax;k++)aver+=(*pntr[k]);
//        if((nmax-nmin)>0)btamp[bchan]=geant(aver/(nmax-nmin));
//
        if(isd==0)strcpy(htit1,"Q-s1 for ref.bar type/bin(TBB) ");
	else strcpy(htit1,"Q-s2 for ref.bar type/bin(TBB) ");
	ii=ibt+1;
        in[0]=inum[ii/10];
        strcat(htit1,in);
	in[0]=inum[ii%10];
        strcat(htit1,in);
        ii=(ibn+1)/10;
        jj=(ibn+1)%10;
        in[0]=inum[ii];
        strcat(htit1,in);
        in[0]=inum[jj];
        strcat(htit1,in);
        bnn=20;//        <<--- select limits/binwidth for hist.
	bnl=workarr[indarr[nmin]];
        bnh=workarr[indarr[nmax]];
        bnw=(bnh-bnl)/bnn;
        if(bnw<1.){
          bnw=1.;
          bnn=int((bnh-bnl)/bnw);
          bnh=bnl+bnw*bnn;
        }
        if(workhis)delete workhis;
        workhis=new TH1F("workhis",htit1,bnn,bnl,bnh);
        for(j=nmin;j<nmax;j++)workhis->Fill(workarr[indarr[j]],1.);
        for(j=0;j<bnn;j++)binsta[j]=workhis->GetBinContent(j+1);//j=0->underfl, j=bnn+1->overfl
        for(j=0;j<bnn;j++)bincoo[j]=bnl+0.5*bnw+bnw*j;
        maxv=0.;
        jmax=0;
        for(j=0;j<bnn;j++){// find bin with max.stat.
          if(binsta[j]>maxv){
            maxv=binsta[j];
            jmax=j;
          }
        }
//        if(TFCAFFKEY.hprintf>1 || (ibt==0 || ibt==2 || ibt==4 || ibt==7)&&(ibn<3 || ibn>nbnr-4))HPRINT(1599);
        strcpy(eltit,htit1);
//---
        prl[1]=bnl;// some more realistic init.values from histogr.
        prh[1]=bnh;
        pri[1]=bnl+0.5*bnw+bnw*jmax;//Mp
        prs[1]=bnw;
        pri[0]=maxv;// A
	prl[0]=1.;
	prh[0]=2.*maxv;
        prs[0]=maxv/10;
        elbt=bnn;
//---
        for(ip=0;ip<2;ip++){// <----- reinitialize these 2 parameters:
          strcpy(prnm,prnam[ip]);
          ierp=0;
          bMinuit->mnparm(ip,prnm,pri[ip],prs[ip],prl[ip],prh[ip],ierp);
          if(ierp!=0){
            cout<<"Ampl:Mp-side-calib: Param-reinit problem for par-id="<<prnam[ip]<<'\n';
            exit(10);
          }
        }// <----- end of param. reinit.
//---
//----
        arglp[0]=1500;//max.calls
        arglp[1]=1.;//tolerance(stop minim, when vert.dist < 0.001*toler*UP)
        ierp=0;
        bMinuit->mnexcm("MIGRAD",arglp,2,ierp);
        if(ierp!=0){
          cout<<"<---- Ampl:RefBarBin Mp-fit: MIGRAD problem !"<<'\n';
          continue;
        }
//  
        arglp[0]=3;
        ierp=0;
        bMinuit->mnexcm("CALL FCN",arglp,1,ierp);
        if(ierp!=0){
          cout<<"<---- Ampl:RefBarBin Mp-fit: final CALL_FCN problem !"<<'\n';
          continue;
        }
        Int_t rv=bMinuit->GetParameter(1,pval,perr);//Mp value
        btamp[isd][bchan]=elfitp[1];//store Mp 
        ebtamp[isd][bchan]=perr;//store Mp error 
//--->print results:
          Double_t amin,edm,errdef;
          Int_t nvpar,nparx,icstat;
	if(kTofCalibPrint[2]>0)tfamh10[ibt]->Fill(melfunchi2,1.);
        if(kTofCalibPrint[2]>1){
          bMinuit->mnstat(amin,edm,errdef,nvpar,nparx,icstat);
//          bMinuit->mnprin(3,amin);
	  cout<<"---> amin="<<amin<<"  Nvarnpars="<<nvpar<<"  stat="<<icstat<<" Abins="<<bnn<<"  nev="<<nev<<"  nmim/max="<<nmin<<"/"<<nmax<<" chi2="<<melfunchi2<<endl;
        }
	ngbins[ibt][isd]+=1;
//------------
//
      }//--->end of nev check
      else{
        cout<<"  -> Ampl:RefBarBinFit-Err- LowStat="<<nev<<" for BType/side="<<ibt+1<<"/"<<isd+1<<" BinDiv="<<ibn+1<<endl;
      }
      if(kTofCalibPrint[2]>0)tfamh11[2*ibt+isd]->Fill(profp[ibt][ibn],btamp[isd][bchan]);
    }//---> end of loop for longit.bins
    }//---> end of side loop
    
  }//---> end of loop for bar-types
//----------------------------------
//
//--------> fit Impact-point profiles(slopes) for ref. bars(for nonuniform.corr.):
//
  Int_t ier;  
  Int_t ifit[kAmProfPars];
  Char_t pnam[kAmProfPars][6],pnm[6];
  Double_t argl[10];
  Int_t iargl[10];
  Double_t start[kAmProfPars],pstep[kAmProfPars],plow[kAmProfPars],phigh[kAmProfPars];
  strcpy(pnam[0],"aforw");
  strcpy(pnam[1],"lenhi");
  strcpy(pnam[2],"wmilo");
  strcpy(pnam[3],"lenlo");
  strcpy(pnam[4],"aback");
  strcpy(pnam[5],"lenbk");
//
  start[0]=50.;
  start[1]=195.;
  start[2]=0.;
  start[3]=18.;
  start[4]=0.;
  start[5]=11.;
//
//  if(AMSJob::gethead()->isMCData()){ // tempor solution for MC
//    start[1]=190.;
//    start[3]=5.;
//  }
//
  pstep[0]=5.;
  pstep[1]=10.;
  pstep[2]=0.1;
  pstep[3]=3.;
  pstep[4]=5.;
  pstep[5]=3.;
//
  plow[0]=1.;
  plow[1]=50.;
  plow[2]=-0.001;
  plow[3]=2.;
  plow[4]=0.;
  plow[5]=2.;
//
  phigh[0]=500.;
  phigh[1]=800.;
  phigh[2]=1.;
  phigh[3]=50.;
  phigh[4]=500.;
  phigh[5]=50.;
//
  for(i=0;i<kAmProfPars;i++)ifit[i]=1;//release all parms
// ------------> initialize some parameters for Minuit:
//
  npar=kAmProfPars;
  TMinuit *pMinuit = new TMinuit(npar);//init minuit for max npar params
  pMinuit->SetPrintLevel(-1);
  pMinuit->SetFCN(TofCalib::mfunam);
  argl[0]=1;//UP
  pMinuit->mnexcm("SET ERR", argl,1,ierp);//set chi2 methode(0.5->likelih)
//
//------> start fit for each bar-type:
//
  Int_t nevbns[kAmProfBins];
  ic=1;
  goodch=0;
  for(ibt=0;ibt<kTofBTypes;ibt++){// <------ loop over bar-types
    id=rbls[ibt];
    if(id==0)continue;//skip dummy bar types
    nbnr=nprbn[ibt];//numb. of bins for given bar type
    il=id/100-1;
    ib=id%100-1;
    clent=brlen(il,ib);
    cbtyp=ibt+1;
    if(kTofCalibPrint[2]>1)cout<<endl;
//
    for(int isd=0;isd<2;isd++){// <----- side loop
      iside=isd;
//
      nbinr=0;
      for(j=0;j<nbnr;j++){ // <--- loop over longit.bins to load coo,charge..
        bchan=ibt*kAmProfBins+j;
        if(isd==0)nev=nevenb1[bchan];
        else nev=nevenb2[bchan];
        if(nev>=80){
          mcharge[nbinr]=btamp[isd][bchan];
          emchar[nbinr]=ebtamp[isd][bchan];
          mcoord[nbinr]=profp[ibt][j];
	  nevbns[nbinr]=nev;
          nbinr+=1;
        }
      }// ---> endof bins loop
//
      if(kTofCalibPrint[2]>1)cout<<"---> Start minim. for btyp/length="<<cbtyp<<" "<<clent<<" GoodStatBins="<<nbinr<<" side-"<<isd+1<<endl;
      if(nbinr<5)goto nextsd;//too few bins, skip this side
      if(kTofCalibPrint[2]>1){
        cout<<"   nevs/q/err/coo per good bin:"<<endl;
        for(j=0;j<nbinr;j++){
          cout<<"     "<<nevbns[j];
        }
        cout<<endl;
        for(j=0;j<nbinr;j++){
          cout<<"  "<<mcharge[j];
        }
        cout<<endl;
        for(j=0;j<nbinr;j++){
          cout<<" "<<emchar[j];
        }
        cout<<endl;
        for(j=0;j<nbinr;j++){
          cout<<"   "<<mcoord[j];
        }
        cout<<endl<<endl;
      }
//
      start[0]=50.;
      start[1]=195.;
      start[2]=0.;
      start[3]=18.;//Llow= specific to normal count.
      if(ibt==0 || ibt==2 || ibt==4 || ibt==7)start[3]=7.;//Llow= specific to trapezoidal
      start[4]=0.;
      start[5]=11.;//Lback= specific to trapez. count
//      
      for(i=0;i<kAmProfPars;i++){//<--- initialize all parameters
        strcpy(pnm,pnam[i]);
	if(i==0){// more realistic "start" value for 1st param.
	  if(isd==0)start[i]=mcharge[0];
	  if(isd==1)start[i]=mcharge[nbinr-1];
	}
	if(i==4){// more realistic "start" value for 5th param("aback").
	  if(ibt==2 || ibt==4 || ibt==7){//non-zero 5th param("aback") for trapez.counters
	    if(isd==0)start[i]=mcharge[nbinr-1];
	    if(isd==1)start[i]=mcharge[0];
	  }
	}
	if(kTofCalibPrint[2]>1)cout<<"Init:StartParN/val="<<i+1<<" "<<start[i]<<" pl/ph="<<plow[i]<<" "<<phigh[i]<<endl;
        ier=0;
        pMinuit->mnparm(i,pnm,start[i],pstep[i],plow[i],phigh[i],ier);
        if(ier!=0){
          cout<<"Ampl: LprofFit(slopes): Param-init problem for par-id="<<pnam[i]<<'\n';
          goto nextsd;
        }
      }// ---> endof initialize
//
      if(ibt==0 || ibt==2 || ibt==4 || ibt==7){//<--- fix/rel some pars. for trapezoidal counters
        fitflg=1;//use formula with "back" (reflection) part
	if(ifit[1]==0){//was fixed
          ifit[1]=1;//<--- release it
          argl[0]=2;//lenhi
          ier=0;
          pMinuit->mnexcm("RELEASE",argl,1,ier);
          if(ier!=0){
            cout<<"Ampl: LProfParam(SLOPES)-release problem for par-id="<<pnam[1]<<'\n';
            goto nextsd;
          }
	}
	if(ifit[3]==1){//was released
          ifit[3]=0;//<--- fix it
          argl[0]=4;//"lenlo"
          ier=0;
          pMinuit->mnexcm("FIX",argl,1,ier);
          if(ier!=0){
            cout<<"Ampl: LProfParam(slopes)-fix problem for par-id="<<pnam[3]<<'\n';
            goto nextsd;
          }
	}
	if(ifit[4]==0){//was fixed
          ifit[4]=1;//<--- release it
          argl[0]=5;//"aback"
          ier=0;
          pMinuit->mnexcm("RELEASE",argl,1,ier);
          if(ier!=0){
            cout<<"Ampl: LProfParam(slopes)-release problem for par-id="<<pnam[4]<<'\n';
            goto nextsd;
          }
	}
	if(ifit[5]==1){//was released
          ifit[5]=0;//<--- fix it
          argl[0]=6;//"lenbk
          ier=0;
          pMinuit->mnexcm("FIX",argl,1,ier);
          if(ier!=0){
            cout<<"Ampl: LProfParam(slopes)-fix problem for par-id="<<pnam[5]<<'\n';
            goto nextsd;
          }
	}
      }
//
      else{//                          <--- Fix/rel some params. for other types counters
        fitflg=0;//use formula with "front" second slope
	
	if(ifit[1]==0){//was fixed
          ifit[1]=1;//<--- release it
          argl[0]=2;//"lenhi"
          ier=0;
          pMinuit->mnexcm("RELEASE",argl,1,ier);
          if(ier!=0){
            cout<<"TOF-calib: LProfileParam-release problem for par-id="<<pnam[1]<<'\n';
            goto nextsd;
          }
	}
	
	if(ifit[3]==1){//was released
          ifit[3]=0;//<--- fix it
          argl[0]=4;//"lenlo"
          ier=0;
          pMinuit->mnexcm("FIX",argl,1,ier);
          if(ier!=0){
            cout<<"Ampl: LProfParam(slopes)-fix problem for par-id="<<pnam[3]<<'\n';
            goto nextsd;
          }
	}
	if(ifit[4]==1){//was released
          ifit[4]=0;//<--- fix it
          argl[0]=5;//"aback"
          ier=0;
          pMinuit->mnexcm("FIX",argl,1,ier);
          if(ier!=0){
            cout<<"Ampl: LProfParam(slopes)-fix problem for par-id="<<pnam[4]<<'\n';
            goto nextsd;
          }
	}
	if(ifit[5]==1){//was released
          ifit[5]=0;//<--- fix it
          argl[0]=6;//"lenbk"
          ier=0;
          pMinuit->mnexcm("FIX",argl,1,ier);
          if(ier!=0){
            cout<<"Ampl: LProfParam(slopes)-fix problem for par-id="<<pnam[5]<<'\n';
            goto nextsd;
          }
	}
      }
      if(kTofCalibPrint[2]>1)cout<<"Param-release mask(1->6):"<<100000*ifit[0]+10000*ifit[1]+1000*ifit[2]+100*ifit[3]+10*ifit[4]+ifit[5]<<endl;
//
// ------> start minimization:
      argl[0]=1500;//max calls
      argl[1]=1;//tolerance(stop minim, when vert.dist < 0.001*toler*UP)
      ier=0;
      pMinuit->mnexcm("MIGRAD",argl,2,ier);
      if(ier!=0){
        cout<<"Ampl:LprofFit(slopes): MIGRAD problem !"<<'\n';
        goto nextsd;
      }  
      argl[0]=3;
      ier=0;
      pMinuit->mnexcm("CALL FCN",argl,1,ier);
      if(ier!=0){
        cout<<"TOF-calib:LprofileFit: final CALL_FCN problem !"<<'\n';
        goto nextsd;
      }
//--->print results:
      goodch+=1;
      Double_t amin,edm,errdef;
      Int_t nvpar,nparx,icstat;
      if(kTofCalibPrint[2]>0)tfamh12->Fill(mfunamchi2,1.);
      if(kTofCalibPrint[2]>1){
        pMinuit->mnstat(amin,edm,errdef,nvpar,nparx,icstat);
        pMinuit->mnprin(3,amin);
	cout<<"---> amin="<<amin<<"  Nvarnpars="<<nvpar<<"  stat="<<icstat<<endl;
      }
 nextsd:
      if(kTofCalibPrint[2]>1)cout<<endl;
      ic+=1;
    } // -----> endof side loop
  }// ------> endof btype loop
//
  if(ic>0 && Float_t(goodch)/ic>0.8)badstage[1]=0;//ok for LongProfiles stage
//------------------------------------------------------------------
//
//----> calc. mean charge for ref.counters (for abs.calibr):
//
  geant elref(1.65);// ref. Elos(Mp(mev),norm.incidence) for mip-region
//
  strcpy(prnam[0],"anor1");
  strcpy(prnam[1],"mprob");
  strcpy(prnam[2],"scalf");
  strcpy(prnam[3],"speed");
  pri[0]=50.;
  pri[1]=80.;
  pri[2]=10.;
  pri[3]=0.5;
//
  prs[0]=5.;
  prs[1]=10.;
  prs[2]=1.;
  prs[3]=0.25;
//
  prl[0]=1.;
  prl[1]=1.;
  prl[2]=0.1;
  prl[3]=0.;
//
  prh[0]=1500.;
  prh[1]=1000.;
  prh[2]=100.;
  prh[3]=10.;
//
  for(i=0;i<kLandfPars;i++)ifitp[i]=1;
  ifitp[3]=0;
//  
  cout<<endl;
  cout<<"-------------> Start AbsNormalization Mp-fit(X=0) for ref.bars: <---------------"<<endl;
  cout<<endl;
// ------------> initialize Minuit:
//
  npar=kLandfPars;
  TMinuit *nMinuit = new TMinuit(npar);//init minuit for max npar params
  nMinuit->SetPrintLevel(-1);
  nMinuit->SetFCN(TofCalib::melfun);
  arglp[0]=1;//UP
  nMinuit->mnexcm("SET ERR", arglp,1,ierp);//set chi2 methode(0.5->likelih)
  for(ip=0;ip<kLandfPars;ip++){// <----- initialize param.
    strcpy(prnm,prnam[ip]);
    ierp=0;
    nMinuit->mnparm(ip,prnm,pri[ip],prs[ip],prl[ip],prh[ip],ierp);
    if(ierp!=0){
      cout<<"Ampl: AbsNorm: Param-init problem for par-id="<<prnam[ip]<<'\n';
      exit(10);
    }
    arglp[0]=ip+1;
    if(ifitp[ip]==0){
      ierp=0;
      nMinuit->mnexcm("FIX",arglp,1,ierp);
      if(ierp!=0){
        cout<<"Ampl: AbsNorm: Param-fix problem for par-id="<<prnam[ip]<<'\n';
        exit(10);
      }
    }
  }// <----- end of param. init.
//----
  integer btypeok(0),btypes(0);
  for(ibt=0;ibt<kTofBTypes;ibt++){//loop over bar types
    id=rbls[ibt];//ref.bar id
    if(id==0)continue;//skip dummy bar types
    btypes+=1;
    aabs[ibt]=0.;
    mip2q[ibt]=65.;//default value
    nev=nrefb[ibt];
    if(kTofCalibPrint[2]>1){
      cout<<endl;
      cout<<"Abs.calib.fit: ibt/events="<< (ibt+1)<<" "<<nev<<endl;
      cout<<endl;
    }
    if(nev>=100){
      for(k=0;k<nev;k++)workarr[k]=arefb[ibt][k];//copy event-signals of chan=ic 
      TMath::Sort(nev,workarr,indarr,ordr);//sort ampls. in increasing order
      nmax=int(floor(nev*trcut));// to keep (100*trcut)% of lowest amplitudes
      nmin=int(floor(nev*0.015));// to remove 1.5 % of lowest amplitudes
      if(nmin==0)nmin=1;
      strcpy(htit1,"Q-tot(X=0) for ref. bar type");
      in[0]=inum[ibt+1];
      strcat(htit1,in);
      bnn=20;//        <<--- select limits/binwidth for hist.
      bnl=workarr[indarr[nmin]];
      bnh=workarr[indarr[nmax]];
      bnw=(bnh-bnl)/bnn;
      if(bnw<1.){
        bnw=1.;
        bnn=int((bnh-bnl)/bnw);
        bnh=bnl+bnw*bnn;
      }
      if(workhis)delete workhis;
      workhis=new TH1F("workhis",htit1,bnn,bnl,bnh);
      for(j=nmin;j<nmax;j++)workhis->Fill(workarr[indarr[j]],1.);
      for(j=0;j<bnn;j++)binsta[j]=workhis->GetBinContent(j+1);//j=0->underfl, j=bnn+1->overfl
      for(j=0;j<bnn;j++)bincoo[j]=bnl+0.5*bnw+bnw*j;
      maxv=0.;
      jmax=0;
      for(j=0;j<bnn;j++){// find bin with max.stat.
        if(binsta[j]>maxv){
          maxv=binsta[j];
          jmax=j;
        }
      }
      strcpy(eltit,htit1);
//---
      prl[1]=bnl;// some more realistic init.values from histogr.
      prh[1]=bnh;
      pri[1]=bnl+0.5*bnw+bnw*jmax;//Mp
      prs[1]=bnw;
      pri[0]=maxv;// A
      prl[0]=1.;
      prh[0]=2.*maxv;
      prs[0]=maxv/10;
      elbt=bnn;
//---
      for(ip=0;ip<2;ip++){// <----- reinitialize these 2 param.
        strcpy(prnm,prnam[ip]);
        ierp=0;
        nMinuit->mnparm(ip,prnm,pri[ip],prs[ip],prl[ip],prh[ip],ierp);
        if(ierp!=0){
          cout<<"Ampl:AbsNorm: Param-reinit problem for par-id="<<prnam[ip]<<'\n';
          exit(10);
	}
      }// <----- end of param. reinit.
//---
      arglp[0]=1500;//max.calls
      arglp[1]=1.;//tolerance(stop minim, when vert.dist < 0.001*toler*UP)
      ierp=0;
      nMinuit->mnexcm("MIGRAD",arglp,2,ierp);
      if(ierp!=0){
        cout<<"<---- Ampl:AbsNorm CentBin Mp-fit: MIGRAD problem !"<<'\n';
        continue;
      }
//  
      arglp[0]=3;
      ierp=0;
      nMinuit->mnexcm("CALL FCN",arglp,1,ierp);
      if(ierp!=0){
        cout<<"<---- Ampl:AbsNorm CentBin Mp-fit: final CALL_FCN problem !"<<'\n';
        continue;
      }
//--->print results:
      Double_t amin,edm,errdef;
      Int_t nvpar,nparx,icstat;
      if(kTofCalibPrint[2]>0)tfamh14->Fill(melfunchi2,1.);
      if(kTofCalibPrint[2]>1){
        nMinuit->mnstat(amin,edm,errdef,nvpar,nparx,icstat);
//        nMinuit->mnprin(3,amin);
        cout<<"---> amin="<<amin<<"  Nvarnpars="<<nvpar<<"  stat="<<icstat<<" Abins="<<bnn<<"  nev="<<nev<<"  nmim/max="<<nmin<<"/"<<nmax<<" chi2="<<melfunchi2<<endl;
      }
      aabs[ibt]=elfitp[1];
      mip2q[ibt]=aabs[ibt]/elref;//(pC/mev)
      btypeok+=1;
//
    }
  }
//
  if(Float_t(btypeok)/kTofBTypes>0.8)badstage[2]=0;//ok for AbsNorm stage
//-------------------------------------------------------
//
// ---> calculate/print Anode/Dynode chan.gain ratios:
//
  number a2d[kTofChanMx],a2ds[kTofChanMx],avr,avr2,a2dsig,rsig;
  Double_t p0,p1;
  Int_t p0i,p1i;
  Float_t a2dcom,a2dgchan;
//
// ---> calculate/print Ah2D(sum) ratios:
  TCanvas *a2d1=new TCanvas("a2d1","Ampl A2D Fit L1",1);
  a2d1->Divide(4,4);
  TCanvas *a2d2=new TCanvas("a2d2","Ampl A2D Fit L2",1);
  a2d2->Divide(4,4);
  TCanvas *a2d3=new TCanvas("a2d3","Ampl A2D Fit L3",1);
  a2d3->Divide(5,4);
  TCanvas *a2d4=new TCanvas("a2d4","Ampl A2D Fit L4",1);
  a2d4->Divide(4,4);
//
  Int_t gchan(0);

  /*
  chan=0;
  for(il=0;il<kNtofl;il++){
    for(ib=0;ib<kNtofb[il];ib++){
      for(i=0;i<2;i++){
	a2dcom=152650;//default value
        if(neva2d[chan]>=10){
	  if(il==0)a2d1->cd(2*ib+i+1);
	  if(il==1)a2d2->cd(2*ib+i+1);
	  if(il==2)a2d3->cd(2*ib+i+1);
	  if(il==3)a2d4->cd(2*ib+i+1);
          gPad->SetGrid();
          gStyle->SetOptStat(11);
          gStyle->SetOptFit(111);
	  tfamh20[chan]->Fit("pol1","","", 1.7,2.7);
          tfamh20[chan]->SetMarkerStyle(20);
          tfamh20[chan]->SetMarkerSize(0.7);
          tfamh20[chan]->SetMinimum(1.);
          tfamh20[chan]->SetMaximum(4.);
          tfamh20[chan]->Draw("P");
	  p0=tfamh20[chan]->GetFunction("pol1")->GetParameter(0);
	  p1=tfamh20[chan]->GetFunction("pol1")->GetParameter(1);
	  tfamh15->Fill(p0,1.);
	  tfamh16->Fill(p1,1.);
	  p0i=floor(p0*100+0.5);
	  p1i=floor(p1*1000+0.5);
	  if(p1i>999 || p0i>999){
	    cout<<"<Ampl:A2DFit-Err-ParOvfl:"<<p0i<<" "<<p1i<<" L/B/S="<<il+1<<" "<<ib+1<<" "<<is+1<<endl;
	  }
	  a2dcom=Float_t(p0i*1000+p1i);
//	  cout<<"<---FitPars:Ch/L/B/S="<<chan<<" "<<il<<" "<<ib<<" "<<i<<" p0/1="<<p0<<" "<<p1<<" a2d="<<a2dcom<<endl;
	  gchan+=1;
	}
	else{
	  cout<<"  <-- Ampl:A2DFit: LowStat="<<neva2d[chan]<<" for L/B/S="<<il+1<<" "<<ib+1<<" "<<is+1<<endl;
	  cout<<"      Default value will be used "<<a2dcom<<endl;
	}
        a2d[chan]=a2dcom;
	chan+=1;//seq.numbering of bar sides
      }
    }
    gPad->Update();
  }
  if(chan>0){
    a2dgchan=Float_t(gchan)/chan;//for cal-quality
    cout<<"<--- Ampl:A2DFit:good channel "<<gchan<<" from total "<<chan<<endl;
  }
  */

  chan=0;
  for(il=0;il<kNtofl;il++){
    for(ib=0;ib<kNtofb[il];ib++){
      for(i=0;i<2;i++){
        a2d[chan]=6;//default value
        a2ds[chan]=0;
        if(neva2d[chan]>=10){
	  avr=a2dr[chan]/neva2d[chan];//aver x
	  avr2=a2dr2[chan]/neva2d[chan];//aver x**2
	  a2dsig=avr2-avr*avr;
	  if(a2dsig>0){
	    a2dsig=sqrt(a2dsig);//rms
	    rsig=a2dsig/avr;//rel. rms
	    if(kTofCalibPrint[2]>0){
	      tfamh15->Fill(avr,1.);
	      tfamh16->Fill(rsig,1.);
	    }
	    if(rsig<0.25){//good measurement, tempor
	      a2d[chan]=avr;
	      a2ds[chan]=a2dsig;
	      gchan+=1;
	    }
	    else{
	      a2ds[chan]=99;//tempor later def.val
	    }
	  }
	}
	chan+=1;//seq.numbering of bar sides
      }
    }
  }

//
  if(kTofCalibPrint[2]>1){
  printf("\n");
  printf(" ------->  An/Dyn(sum)  nevents/ratios/sigmas tables :\n");
  printf("\n");
  ich=0;
  for(il=0;il<kNtofl;il++){
    printf("Layer= %2d\n",(il+1));
    for(ib=0;ib<kNtofb[il];ib++){
      chan=ich+2*ib;
      printf("%6d",neva2d[chan]);
    }
    printf("\n");
    for(ib=0;ib<kNtofb[il];ib++){
      chan=ich+2*ib+1;
      printf("%6d",neva2d[chan]);
    }
    printf("\n");
    printf("\n");
    
    for(ib=0;ib<kNtofb[il];ib++){
      chan=ich+2*ib;
      printf("%6.2f",a2d[chan]);
    }
    printf("\n");
    for(ib=0;ib<kNtofb[il];ib++){
      chan=ich+2*ib+1;
      printf("%6.2f",a2d[chan]);
    }
    printf("\n");
    printf("\n");
/*    
    for(ib=0;ib<kNtofb[il];ib++){
      chan=ich+2*ib;
      printf("%6.2f",a2ds[chan]);
    }
    printf("\n");
    for(ib=0;ib<kNtofb[il];ib++){
      chan=ich+2*ib+1;
      printf("%6.2f",a2ds[chan]);
    }
    printf("\n");
    printf("\n");
*/
    ich+=2*kNtofb[il];//skip to next layer
  }
  }
    
//-------------------------------------------------------
//
// ---> calculate Dynode-pmts relat(to side aver) gains:
//
  Float_t dpmg[kTofChanMx][kTofPmtMX],dpmgs[kTofChanMx][kTofPmtMX];
  Float_t dpmsig,dnor;
  Int_t npmts;
//
// ---> calculate Dyn rel.gains:
//
//chan-sequence: L1:B1(s1,s2),...Bn(s1,s2),  L2:.......  L4:B1(s1,s2),...Bn(s1,s2)
  chan=0;
  for(il=0;il<kNtofl;il++){//<---plane-loop
    for(ib=0;ib<kNtofb[i];ib++){//<---bar-loop
      npmts=npmtps(il,ib);
      for(i=0;i<2;i++){//<---side-loop
//
	for(ip=0;ip<kTofPmtMX;ip++){
	  dpmg[chan][ip]=1;
	  dpmgs[chan][ip]=0;
	}
        if(nevdgn[chan]>5){//<--check stat
          for(ip=0;ip<npmts;ip++){//<--pmt-loop
	    avr=d2sdr[chan][ip]/nevdgn[chan];//aver x
	    avr2=d2sdr2[chan][ip]/nevdgn[chan];//aver x**2
	    dpmsig=avr2-avr*avr;
	    if(dpmsig>0 && avr>0){
	      dpmsig=sqrt(dpmsig);//rms
	      rsig=dpmsig/avr;//rel. rms
	      if(kTofCalibPrint[2]>0){
	        tfamh17->Fill(avr,1.);
	        tfamh18->Fill(rsig,1.);
	      }
	      if(rsig<2){//good measurement
	        dpmg[chan][ip]=avr;
	        dpmgs[chan][ip]=dpmsig;
	      }
	      else{
	        dpmg[chan][ip]=1;
	        dpmgs[chan][ip]=99;
	      }
	    }
	    else{
	      dpmg[chan][ip]=1;
	      dpmgs[chan][ip]=99;
	    }
	  }//-->endof pm-loop
	  dnor=0;
	  for(ip=0;ip<npmts;ip++)dnor+=dpmg[chan][ip];
	  if(dnor>0){//normalize to have Sum(dpmg[ip])=npmts
	    dpmg[chan][ip]*=(geant(npmts)/dnor);
	  }
	}//-->endof stat-check
	chan+=1;//seq.numbering of bar sides
      }//-->endof side-loop
//
    }//-->endof bar-loop
  }//-->endof layer-loop
//------
  if(kTofCalibPrint[2]>1){
  printf("\n");
  printf(" =================> Dynodes relative(to Dsum) gains tables :\n");
  printf("\n");
//
  ich=0;
  for(il=0;il<kNtofl;il++){
    printf("Layer= %2d\n",(il+1));
    for(ib=0;ib<kNtofb[il];ib++){//s1
      chan=ich+2*ib;
      printf("%6d",nevdgn[chan]);
    }
    printf("\n");
    for(ib=0;ib<kNtofb[il];ib++){//s2
      chan=ich+2*ib+1;
      printf("%6d",nevdgn[chan]);
    }
    printf("\n");
    printf("\n");
    
    for(ib=0;ib<kNtofb[il];ib++){
      chan=ich+2*ib;
      for(ip=0;ip<kTofPmtMX;ip++)printf("%5.2f",dpmg[chan][ip]);
      printf(" ");
    }
    printf("\n");
    for(ib=0;ib<kNtofb[il];ib++){
      chan=ich+2*ib+1;
      for(ip=0;ip<kTofPmtMX;ip++)printf("%5.2f",dpmg[chan][ip]);
      printf(" ");
    }
    printf("\n");
    printf("\n");
    
    for(ib=0;ib<kNtofb[il];ib++){
      chan=ich+2*ib;
      for(ip=0;ip<kTofPmtMX;ip++)printf("%5.2f",dpmgs[chan][ip]);
      printf(" ");
    }
    printf("\n");
    for(ib=0;ib<kNtofb[il];ib++){
      chan=ich+2*ib+1;
      for(ip=0;ip<kTofPmtMX;ip++)printf("%5.2f",dpmgs[chan][ip]);
      printf(" ");
    }
    printf("\n");
    printf("\n");
    ich+=2*kNtofb[il];
  }
  printf("\n");
  printf("\n");
//
//------------------------------------------------------
//
  printf("\n");
  printf(" ===============>  Anode relative gains tables :\n");
  printf("\n");
  ic=0;
  for(il=0;il<kNtofl;il++){
    printf("Layer= %2d\n",(il+1));
    for(ib=0;ib<kNtofb[il];ib++){
      chan=ic+2*ib;
      printf("%6d",nevenc[chan]);
    }
    printf("\n");
    for(ib=0;ib<kNtofb[il];ib++){
      chan=ic+2*ib+1;
      printf("%6d",nevenc[chan]);
    }
    printf("\n");
    printf("\n");
    for(ib=0;ib<kNtofb[il];ib++){
      chan=ic+2*ib;
      printf("%6.3f",gains[chan]);
    }
    printf("\n");
    for(ib=0;ib<kNtofb[il];ib++){
      chan=ic+2*ib+1;
      printf("%6.3f",gains[chan]);
    }
    printf("\n");
    printf("\n");
    ic+=2*kNtofb[il];
  }
//
// ---------
  cout<<" Absolute calibration results :"<<endl;
  cout<<endl;
  cout<<"Events in ref.bars :"<<endl;
  for(i=0;i<kTofBTypes;i++)printf("%6d",nrefb[i]);
  printf("\n");
  printf("\n");
  cout<<"Mean charge in ref.bars (centr.bin):"<<endl;
  for(i=0;i<kTofBTypes;i++)printf("%7.2f",aabs[i]);
  printf("\n");
  printf("\n");
  cout<<"Convertion factors mip2q (pC/MeV):"<<endl;
  for(i=0;i<kTofBTypes;i++)printf("%7.2f",mip2q[i]);
  printf("\n");
  printf("\n");
  cout<<endl;
  }//--->endof ptinttables check
// ------------------------------------------------------------- 
//
// ---> write Anode-gains,A/D slops/offs,Anode-mip2q's to file:
//
  ofstream tcfile(fname,ios::out|ios::trunc);
  if(!tcfile){
    cerr<<"TofTmAmCalib::Ampl:error opening file for output "<<fname<<'\n';
    return 1;
  }
  cout<<"Open file for AMPL-calibration output, fname:"<<fname<<'\n';
  cout<<"Indiv.channel gain's,a2dr's,h2l's,mip2q's,profile_par's will be written !"<<'\n';
  cout<<" First run used for calibration is "<<StartRun<<endl;
  cout<<" Date of the first event : "<<frdate<<endl;
//
  tcfile.setf(ios::fixed);
  tcfile.width(6);
//                       <------- write Anode relat. gains:
//
  tcfile.precision(3);// precision for gains
  ic=0;
  for(il=0;il<kNtofl;il++){ 
    for(ib=0;ib<kNtofb[il];ib++){
      chan=ic+2*ib;
      tcfile << gains[chan]<<" ";
    }
    tcfile << endl;
    for(ib=0;ib<kNtofb[il];ib++){
      chan=ic+2*ib+1;
      tcfile << gains[chan]<<" ";
    }
    tcfile << endl;
    ic+=2*kNtofb[il];
  }
  tcfile << endl;
//
//                      <------ write Anode/Sum(Dynode(ipm))-ratios 
//
    tcfile.width(7);
    tcfile.precision(2);// precision for A/D ratio
    ic=0;
    for(il=0;il<kNtofl;il++){ //
      for(ib=0;ib<kNtofb[il];ib++){
        chan=ic+2*ib;
        tcfile << a2d[chan]<<" ";
      }
      tcfile << endl;
      for(ib=0;ib<kNtofb[il];ib++){
        chan=ic+2*ib+1;
        tcfile << a2d[chan]<<" ";
      }
      tcfile << endl;
      ic+=2*kNtofb[il];
    }
    tcfile << endl;
//
//                      <------ write Dynode-pmts rel.gains :
//
    tcfile.width(6);
    tcfile.precision(2);// precision for  D-gains
    ic=0;
    for(il=0;il<kNtofl;il++){//<---layer-loop
      for(is=0;is<2;is++){//<--- side-loop
        for(ib=0;ib<kNtofb[il];ib++){//<---bar-loop
	  chan=ic+2*ib+is;
          for(ip=0;ip<kTofPmtMX;ip++)tcfile << dpmg[chan][ip]<<" ";
	  tcfile << " ";
        }//--->endof bar-loop
        tcfile << endl;
      }//--->endof side-loop
      ic+=2*kNtofb[il];
    }//--->endof layer-loop
    tcfile << endl;
//
//                       <------ write mip2q's :
//
  tcfile.precision(2);// precision for mip2q's
  for(btyp=0;btyp<kTofBTypes;btyp++){ // <--- mip2q's
    tcfile << mip2q[btyp]<<" ";
  }  
  tcfile << endl;
  tcfile << endl;
//
//                       <-------- write A-profile fit-parameters:
//
  for(btyp=0;btyp<kTofBTypes;btyp++){
    for(i=0;i<2*kAmProfPars;i++)tcfile << aprofp[btyp][i]<<" ";
    tcfile << endl;
  }  
  tcfile << endl;
//
  tcfile << 12345;// write end-file label
  tcfile << endl;
//
//
  tcfile << endl<<"======================================================"<<endl;
  tcfile << endl<<" First run used for calibration is "<<StartRun<<endl;
  tcfile << endl<<" Date of the first event : "<<frdate<<endl;
  tcfile.close();
  cout<<"AMPLcalib:output file closed !"<<endl;
//
  cout<<"<======= AMPLcalib:BadStagesMap="<<badstage[0]<<" "<<badstage[1]<<" "<<badstage[2]<<endl<<endl;
  if(badstage[0]==0 && badstage[1]==0 && badstage[2]==0)cstatus=0;//all ok
  return cstatus;
//
}
//---------------------
void TofCalib::mfunam(Int_t &np, Double_t grad[], Double_t &f, Double_t x[], Int_t flg){
// This is standard Minuit FCN for Ampl-uniformity(along the paddle) fit:
  Int_t i,j;
  Double_t ff,fff[kAmProfBins];
  f=0.;
  if(iside==0){
    for(i=0;i<nbinr;i++){
      if(fitflg==0)ff=(mcharge[i]-(x[0]*((1-x[2])*exp(-(clent/2.+mcoord[i])/x[1])
                     +x[2]*exp(-(clent/2.+mcoord[i])/x[3])))
		                                            )/emchar[i];//normal counters
      else         ff=(mcharge[i]-(x[0]*((1-x[2])*exp(-(clent/2.+mcoord[i])/x[1])
                     +x[2]*exp(-(clent/2.+mcoord[i])/x[3]))
		     +x[4]*exp(-(clent/2.-mcoord[i])/x[5]))
		                                            )/emchar[i];//some trapez.counters
      f+=(ff*ff);
    }
  }
  else{
    for(i=0;i<nbinr;i++){
      if(fitflg==0)ff=(mcharge[i]-(x[0]*((1-x[2])*exp(-(clent/2.-mcoord[i])/x[1])
                     +x[2]*exp(-(clent/2.-mcoord[i])/x[3])))
		                                            )/emchar[i];//normal counters
      else         ff=(mcharge[i]-(x[0]*((1-x[2])*exp(-(clent/2.-mcoord[i])/x[1])
                     +x[2]*exp(-(clent/2.-mcoord[i])/x[3]))
		     +x[4]*exp(-(clent/2.+mcoord[i])/x[5]))
		                                            )/emchar[i];//some trapez.counters
      f+=(ff*ff);
    }
  }
  if(flg==3){
    f=sqrt(f/nbinr);
    mfunamchi2=f;
      if(kTofCalibPrint[2]>1)
         cout<<"    FitResult::Btype="<<cbtyp<<" Fitflg="<<fitflg<<" side="<<iside+1<<"  funct:nbins="<<f<<endl;
      for(i=0;i<kAmProfPars;i++){
        aprofp[cbtyp-1][iside*kAmProfPars+i]=x[i];
        if(kTofCalibPrint[2]>1)cout<<"    parnumb/par="<<i<<" "<<x[i]<<endl;
      }
      if(kTofCalibPrint[2]>1)cout<<"      Function values vs coo:"<<endl;
      for(i=0;i<nbinr;i++){
        if(iside==0){
         if(fitflg==0)ff=x[0]*((1-x[2])*exp(-(clent/2.+mcoord[i])/x[1])
                        +x[2]*exp(-(clent/2.+mcoord[i])/x[3]));//normal counters
         else         ff=x[0]*((1-x[2])*exp(-(clent/2.+mcoord[i])/x[1])
                        +x[2]*exp(-(clent/2.+mcoord[i])/x[3]))
		        +x[4]*exp(-(clent/2.-mcoord[i])/x[5]);       
        }
        else{
         if(fitflg==0)ff=x[0]*((1-x[2])*exp(-(clent/2.-mcoord[i])/x[1])
                        +x[2]*exp(-(clent/2.-mcoord[i])/x[3]));//normal counters
         else         ff=x[0]*((1-x[2])*exp(-(clent/2.-mcoord[i])/x[1])
                        +x[2]*exp(-(clent/2.-mcoord[i])/x[3]))
	    	        +x[4]*exp(-(clent/2.+mcoord[i])/x[5]);
        }
	fff[i]=ff;
      }
      if(kTofCalibPrint[2]>1)for(i=0;i<nbinr;i++)cout<<"  "<<mcoord[i];
      if(kTofCalibPrint[2]>1)cout<<endl;
      if(kTofCalibPrint[2]>1)for(i=0;i<nbinr;i++)cout<<" "<<fff[i];
      if(kTofCalibPrint[2]>1)cout<<endl;
  }
}
//---------------------
void TofCalib::melfun(Int_t &np, Double_t grad[], Double_t &f, Double_t x[], Int_t flg){
// This is standard Minuit FCN for Landau fit:
  Int_t i,j;
  Double_t fun,sig2,lam;
  f=0.;
  for(i=0;i<elbt;i++){// loop over hist.bins
    sig2=binsta[i];
    if(sig2==0.)sig2=1.;
    lam=(bincoo[i]-x[1])/x[2];
    fun=x[0]*exp(x[3])*exp(-x[3]*(lam+exp(-lam)));
    f+=pow((binsta[i]-fun),2)/sig2;
  }
  if(flg==3){
    f=sqrt(f/number(elbt));
    melfunchi2=f;
    if(kTofCalibPrint[2]>1)cout<<endl<<endl<<eltit<<"  Quality of fit (funct/nbins)->"<<f<<endl;
    for(i=0;i<kLandfPars;i++){
      elfitp[i]=x[i];
      if(kTofCalibPrint[2]>1)cout<<" par.number/par="<<i<<" "<<elfitp[i]<<endl;
    }
  }
}

Bool_t TofCalib::saa(Float_t phi,Float_t theta) {
  // phi, theta geographic                                                                                            
  Bool_t ssa_good=true;
  if(phi>=-74 && phi<-72 && theta>=-23 && theta<-18) ssa_good=false;
  if(phi>=-72 && phi<-70 && theta>=-27 && theta<-15) ssa_good=false;
  if(phi>=-70 && phi<-68 && theta>=-31 && theta<-13) ssa_good=false;
  if(phi>=-68 && phi<-66 && theta>=-34 && theta<-12) ssa_good=false;
  if(phi>=-66 && phi<-64 && theta>=-36 && theta<-11) ssa_good=false;
  if(phi>=-64 && phi<-62 && theta>=-38 && theta<-10) ssa_good=false;
  if(phi>=-62 && phi<-60 && theta>=-40 && theta<-10) ssa_good=false;
  if(phi>=-60 && phi<-58 && theta>=-40 && theta<-9) ssa_good=false;
  if(phi>=-58 && phi<-56 && theta>=-42 && theta<-8) ssa_good=false;
  if(phi>=-56 && phi<-54 && theta>=-43 && theta<-8) ssa_good=false;
  if(phi>=-54 && phi<-52 && theta>=-43 && theta<-8) ssa_good=false;
  if(phi>=-52 && phi<-50 && theta>=-43 && theta<-8) ssa_good=false;
  if(phi>=-50 && phi<-48 && theta>=-43 && theta<-8) ssa_good=false;
  if(phi>=-48 && phi<-46 && theta>=-44 && theta<-8) ssa_good=false;
  if(phi>=-46 && phi<-44 && theta>=-44 && theta<-8) ssa_good=false;
  if(phi>=-44 && phi<-42 && theta>=-44 && theta<-9) ssa_good=false;
  if(phi>=-42 && phi<-40 && theta>=-43 && theta<-9) ssa_good=false;
  if(phi>=-40 && phi<-38 && theta>=-43 && theta<-11) ssa_good=false;
  if(phi>=-38 && phi<-36 && theta>=-42 && theta<-13) ssa_good=false;
  if(phi>=-36 && phi<-34 && theta>=-42 && theta<-12) ssa_good=false;
  if(phi>=-34 && phi<-32 && theta>=-42 && theta<-14) ssa_good=false;
  if(phi>=-32 && phi<-30 && theta>=-41 && theta<-16) ssa_good=false;
  if(phi>=-30 && phi<-28 && theta>=-40 && theta<-17) ssa_good=false;
  if(phi>=-28 && phi<-26 && theta>=-40 && theta<-18) ssa_good=false;
  if(phi>=-26 && phi<-24 && theta>=-39 && theta<-19) ssa_good=false;
  if(phi>=-24 && phi<-22 && theta>=-38 && theta<-20) ssa_good=false;
  if(phi>=-22 && phi<-20 && theta>=-37 && theta<-21) ssa_good=false;
  if(phi>=-20 && phi<-18 && theta>=-36 && theta<-22) ssa_good=false;
  if(phi>=-18 && phi<-16 && theta>=-35 && theta<-24) ssa_good=false;
  if(phi>=-16 && phi<-14 && theta>=-34 && theta<-25) ssa_good=false;
  if(phi>=-14 && phi<-12 && theta>=-32 && theta<-26) ssa_good=false;
  if(phi>=-12 && phi<-10 && theta>=-31 && theta<-27) ssa_good=false;
  if(phi>=-10 && phi<-8 && theta>=-28 && theta<-27) ssa_good=false;
  return ssa_good;

}

//
//==============> Main:
//
//#include "/f2users/lquadran/AMS/include/amschain.h"
#include "/Offline/vdev/include/amschain.h"
#include <getopt.h>
#include <signal.h>
#include <unistd.h>
void (handler)(int);
AMSChain *pchain=0;

int main(int argc, char *argv[]){

  *signal(SIGTERM, handler);
  *signal(SIGINT, handler);
  *signal(SIGUSR1, handler);

  int option_index = 0;
  char xtname[]="";
  char brname[]="";
  int  brnumb=0;
  char ername[]="";
  int  ernumb=0;
  int events=3400000;
  int thread=1;
  long long order=0;
  int mem=0;
  
  static struct option long_options[] = {
        {"begrun",  1, 0, 'b'},
        {"endrun",  2, 0, 'e'},
        {"xtname",  2, 0, 'x'},
        {"nevent",  2, 0, 'n'},
        {"help",    0, 0, 'h'},
        {"thread",  2, 0, 't'},
        {"order",   2, 0, 'o'},
        {0, 0, 0, 0}
	};
//
  while (1) {
        int c = getopt_long (argc, argv, "b:e:x:n:t:hH?o:", long_options, &option_index);
        if (c == -1) break;//all options already scanned
        switch (c) {
            case 'b':             
             strcpy(brname,optarg);
	     brnumb=atoi(optarg);
             break;
            case 'e':             
             strcpy(ername,optarg);
	     ernumb=atoi(optarg);
             break;
            case 'x':             
             strcpy(xtname,optarg);
             break;
            case 'o':
             order=atoll(optarg);
             break;
            case 'n':             
             events=atol(optarg);
             break;
            case 't':
             int t=atoi(optarg);
             if(t>0 && t<omp_get_num_procs())thread=t;
             break;
            case 'm':
             mem=1;
             break;
            case 'h':
            case 'H':
            case '?':
            default:         
                cout<<"TofCalib_mt  -begrun[r] -endrun[e] -xtname[x] -nevent[n] -thread[t] --thickmemory[m] --order[o]" <<endl;
                return 0;
                break;
        }
  }
//-----
  AMSEventR::fgThickMemory=mem;
//
  AMSEventR::BadRunList.push_back(1305819288);
  AMSEventR::BadRunList.push_back(1305808241);
  AMSEventR::BadRunList.push_back(1305814074);
  AMSEventR::BadRunList.push_back(1305811707);
  AMSEventR::BadRunList.push_back(1305813443);
  AMSEventR::BadRunList.push_back(1305806831);
  AMSEventR::BadRunList.push_back(1305805651);
//
  AMSEventR::MinRun=brnumb;
  if(ernumb==0)AMSEventR::MaxRun=brnumb+86400;//+24h
  else AMSEventR::MaxRun=ernumb+1;
//
  unsigned long long pp=1;
//
  if(order==0 || order/pp%10==1){
    AMSChain chain("AMSRoot",thread,sizeof(TofCalibPG));
    chain.Add("/afs/cern.ch/ams//Offline/DataSetsDir/Data/AMS02/2011B/ISS.B550/std/*.*.root");
    //    chain.Add("rfio:/castor/cern.ch/ams/Data/AMS02/2011B/ISS.B538/std/13*.*.root");//

    string fileoutput(brname);
    fileoutput+=xtname;
    fileoutput+=".his";
    cout<<"BegRunN="<<brnumb<<" EndRunN="<<ernumb<< "ExtName="<<xtname<<" Nevs="<<events<<" Thrds="<<thread<<" outf="<<fileoutput.c_str()<<endl;
    pchain=&chain;
    chain.Process(new TofCalibPG[thread],fileoutput.c_str(),events);
  }
//---
  return 0;
}




void (handler)(int sig){
  switch(sig){
  case SIGTERM: case SIGINT:
#pragma omp master
{
   cerr <<" SIGTERM intercepted"<<endl;
   if(pchain){cerr<<" resetting "<<endl;pchain->nentries=-1;pchain->ntree=0;} 
}
  break;
  case SIGUSR1:
#pragma omp master
{
   cerr<<"   SIGUSR1 intercepted gracefully terminating"<<endl;
   if(pchain){pchain->ntree=0;}
}
  }
}

