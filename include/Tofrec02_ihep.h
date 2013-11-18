//  $Id: Tofrec02_ihep.h,v 1.31 2013/11/18 15:36:25 qyan Exp $

//Author Qi Yan 2012/June/09 10:03 qyan@cern.ch  /*IHEP TOF version*/
#ifndef __AMSTOFREC02_IHEP__
#define __AMSTOFREC02_IHEP__
#include "root.h"
#include "TObject.h"
#include "TF1.h"
#ifndef __ROOTSHAREDLIBRARY__
//////////////////////////////////////////////////////////////////////////
class TOF2RawSide;
class AMSCharge;
class AMSTrTrack;
class AMSTRDTrack;
class AMSEcalShow;
#endif
class TofBetaPar;

/*!
 *  TOF IHEP Recontruction+Calibration(BetaH version) Both Support Production and Root Mode Refit(B584 or earier production data)
 *  Root Mode ReFit User Need One function:  TofRecH::ReBuild() 
 *  Example of How to Use:  vdev/example/Tof_BetaH.C (AMSsoft)
 * \author qyan@cern.ch
*/
//////////////////////////////////////////////////////////////////////////
class TofRecH{

/// Data part
protected:
/// light velocity
  static const double cvel=29.9792;//c vel(cm/ns)
/// Fit Beta Par
  static TofBetaPar betapar;
  #pragma omp threadprivate (betapar)
/// Process Event
  static AMSEventR *ev;
  #pragma omp threadprivate (ev)
/// Real or MC
  static int realdata; 
  #pragma omp threadprivate (realdata)
/// TofRawSideR vector for TofClusterH build
  static vector<TofRawSideR> tfraws;
  #pragma omp threadprivate (tfraws)
#ifndef __ROOTSHAREDLIBRARY__
  static vector<TOF2RawSide*> tf2raws;
#endif
public:
  static bool RebuildBetaHInReadHeader;
protected:
/// TofClusterH 4layer vector  pointer
  static vector<TofClusterHR*>tofclh[4];
/// TofClusterH UpPair+DownPair BetaH Raw
  static vector<pair <TofClusterHR*,TofClusterHR* > >tofclp[2];
/// TofClusterH UpPair+DownPair BetaH Select Candidate
  static vector<pair <TofClusterHR*,TofClusterHR* > >tofclc[2];

/// Track vector pointer
  static vector<TrTrackR*> track;
#ifndef __ROOTSHAREDLIBRARY__
  static vector<AMSTrTrack*> amstrack;  
#endif

/// TrdTrack vector
  static vector<TrdTrackR>trdtrack;
  #pragma omp threadprivate (trdtrack)
#ifndef __ROOTSHAREDLIBRARY__
  static vector<AMSTRDTrack*>amstrdtrack;
#endif

/// EcalShower vector
  static vector<EcalShowerR> ecalshow;
  #pragma omp threadprivate (ecalshow)
#ifndef __ROOTSHAREDLIBRARY__
  static vector<AMSEcalShower*>amsecalshow;
#endif

/// Charge Signal Type
public:
  enum  QSignalType{
    /// Convert to MIP Unit
    kQ2=0x1,
    /// Q2(ADC) Non Linear Corr
    kLinearCor=0x2,
    /// Attnueation Corr
    kAttCor=  0x4,
    /// Path Length dz/dl Cor
    kThetaCor=0x8,
    /// Birk Saturation Corr
    kBirkCor= 0x10,
    /// Invert Birk Saturation Corr
    kVBirkCor=0x20,
    /// Convert From Q2 To MeV
    kQ2MeV =  0x40, 
    /// Convert From MeV to Q2
    kMeVQ2 =  0x80,
    /// Conver From Q2 to Q
    kQ2Q =    0x100,
    /// Beta Cor
    kBetaCor= 0x200,
    /// Rigidity Cor
    kRigidityCor=0x400,
    /// Attnueation ReCor
    kReAttCor=0x800,
 };

/** @name Sum ReBuild Part
 * @{
 */
public:
/// default construction
  TofRecH(){};
/// TDV Build Control Key
  static bool BuildKey;
/// Build Option //0-normal Build, 1-Exclude Track-Association Build, 2-Exclude Track+Trd Association, 3-Build Continue After Track Association Finding, 11110 Trk-Trd-Ecal-TOF-Other, 31110 Trk(Track Find Continue)-Trd-Ecal-TOF-Other, -1 Calibration Build
  static int  BuildOpt;
/// TDV initial function
  static int  Init();
/// Root Mode Refit //if want to rebuild BetaH in Root Mode, Only need this function//(charge is dummy)
  static int  ReBuild(int charge=1);
/// Build TofClusterH function
  static int  BuildTofClusterH();
/// Build BetaH function (0 mask output)
  static int  BuildBetaH(int verse=0);
/// Clear TofClusterH build vector
  static int  ClearBuildTofClH();
/// Clear BetaH build vector
  static int  ClearBuildBetaH();
/**@}*/

/** @name TofClusterH Build Part
 * @{
 */
public:
/// Finding LT from TofRawSide 
  static int  TofSideRec(TofRawSideR *ptr,number &adca, integer &nadcd,number adcd[],number &sdtm,uinteger &sstatus,
                        vector<number>&ltdcw, vector<number>&htdcw, vector<number>&shtdcw);
/// Time build from TofRawSide
  static int  TimeCooRec(int idsoft,number sdtm[],number adca[],number tms[2],number &tm,number &etm,number &lcoo,number &elcoo,uinteger &status);
/// Energy build Module
  static int  EdepRec(int idsoft,number adca[],number adcd[][TOF2GC::PMTSMX],number lcoo,number q2pa[],number q2pd[][TOF2GC::PMTSMX],number &edepa,number &edepd,uinteger sstatus[2]);
/// Energy build Module-1
  static int  EdepRecR(int ilay,int ibar,geant adca[],geant adcd[][TOF2GC::PMTSMX],number lcoo,geant q2pa[],geant q2pd[][TOF2GC::PMTSMX],geant &edepa,geant &edepd);
/// Refind LT if many LT not Associate with HT
  static int  LTRefind(int idsoft,number trlcoo,number sdtm[2],number adca[2],uinteger &status,vector<number> ltdcw[ ]);  

/// Get QSignal with different Correction
  static number GetQSignal(int idsoft,int isanode,int optc,number signal,number lcoo=0,number cosz=1,number beta=0.98,number rig=0);
/// Adding Gain to Convert To Q*Q (Proton Mip Unit)
  static number CoverToQ2(int idsoft,int isanode,number adc);
/// Q2(ADC) Non-Linear Correction
  static number NonLinearCor(int idsoft,int isanode,number q2);
/// Scintillator Attunation Correction To Counter Central
  static number SciAttCor(int idsoft,number lpos,number q2);
/// Scintillator Re-Attunation Correction Using Carbon to ReTune
  static number SciReAttCor(int idsoft,number lpos,number q2,int qopt=0);
/// Scintillator Birk Correction (opt=1 normal 0 invert)
  static number BirkCor(int idsoft,number q2,int opt=1);
/// Beta Correction
  static number BetaCor(int idsoft,number q2,number beta,number rig=0);
/// Beta Correction Factor For Charge(opt=0) or Cal Edep Beta(opt=1)
  static number GetBetaCalCh(int idsoft,int opt,number beta,number q2,int charge,number rig=0);
/// Beta Q2-Correction Factor For ChargeIndex(opt=0) or Cal Edep Beta(opt=1)
  static number GetBetaCalI(int idsoft,int opt,number beta,number q2norm,int chindex,number rig=0);
/// Sum Anode Signal To Counter Signal
  static number SumSignalA(int idsoft,number signal[],int useweight=1);
/// Sum Dynode Signal To Counter Signal
  static number SumSignalD(int idsoft,number signal[][TOFCSN::NPMTM],int useweight=1,bool minpmcut=1);
/// Get Proton Anode Mip Adc for local lpos
  static number GetProMipAdc(int idsoft,number lpos);
/// Birk Function
  static TF1 *BirkFun;
#pragma omp threadprivate (BirkFun)
/// Birk Function
  static TF1 *GetBirkFun(int idsoft);
/// BetaCor Function
  static TF1 *BetaFun;
#pragma omp threadprivate (BetaFun)
/// BetaCor Function
  static TF1 *GetBetaFun();


/**@}*/

/** @name BetaH Build Part
 * @{
 */
public:
/// Find TofClusterH for ilay with Track
#if defined (_PGTRACK_) || defined (__ROOTSHAREDLIBRARY__)
  static int  BetaFindTOFCl(TrTrackR *ptrack,   int ilay,TofClusterHR** tfhit,number &tklen,number &tklcoo,number &tkcosz,number cres[2],int &pattern,int opt=0);
#else
  static int  BetaFindTOFCl(AMSTrTrack *ptrack,int ilay,TofClusterHR** tfhit,number &tklen,number &tklcoo,number &tkcosz,number cres[2],int &pattern,int opt=0);
#endif
/// Search TofClusterH
  static int  TOFClSel(TofClusterHR* tfhit[4],TofClusterHR* tfhitb[4],TofClusterHR* tfhits[4],number tklcoo[4],number tklcoob[4],number tkcosz[4],number tkcoszb[4]);

/// Recover Time information if One Side lost Signal
  static int  TRecover(TofClusterHR *tfhit[4],number tklcoo[4],int partten[4],int mode=0);//using hassid to recover other side
  static int  TRecover(int idsoft,geant trlcoo,geant tms[2],geant &tm,geant &etm,uinteger &status,int hassid);
/// ReBuild Attenuation Correction From TkCoo 
  static int EdepTkAtt(TofClusterHR *tfhit[4],number tklcoo[4],number tkcosz[4],TofBetaPar &par);
/// Coo Chi2 Fit function 
  static int  BetaFitC(TofClusterHR *tfhit[4],number res[4][2],int partten[4],TofBetaPar &par,int mode);//
/// Beta Fit function
  static int  BetaFitT(TofClusterHR *tfhit[4],number len[4],int partten[4],TofBetaPar &par,int mode=1,int verse=0);//mode same etime weight(0) or not(1)
  static int  BetaFitT(number time[],number etime[],number len[],const int nhits,TofBetaPar &par,int mode=1,int verse=0);//mode same etime weight(0) or not(1)
/// Beta Check function
  static int  BetaFitCheck(TofClusterHR *tfhit[4],number res[4][2],number lenr[4],int pattern[4],TofBetaPar &par,int mode);//if this is normal value
#ifndef __ROOTSHAREDLIBRARY__
  static number BetaCorr(number zint,number z0,number part,uinteger &status);//to BetaC Vitaly
#endif
/// Mass Cal function
  static int  MassRec(TofBetaPar &par,number rig=0,number charge=0,number evrig=0,int isubetac=0);//evrig=E(1/rig)

/// BetaH Self Track Build Part
/// Up And Down TOFCluster Make Pair
  static int TOFClMakePair(int ilay0,int ilay1,int isdown);
/// Pair PreSelection
  static int TOFPairPreSel(int ud,number coref[],AMSDir diref,number cutangle);
/// Find TOFCl Algorithem
  static int TOFPairSel(int ud,TofClusterHR* tfhit[2]);
/// Ecal Search TOF Down Part
  static int EcalSearchD(TofClusterHR* tfhitu[2], TofClusterHR* tfhitd[2],number cooshow[3]);  
/// TofTrack Fit 
  static int TofTrackFit(TofClusterHR *tfhit[4],TofBetaPar &par,int attrefit=0,int verse=0);
/// Line Fit //y=ax+b
  static int LineFit(int nhits,number x[],number y[],number ey[],number &a,number &b);
/// Up Down Self Match Candidate
  static bool PairMatchUD(pair<TofClusterHR*,TofClusterHR*> upair,pair<TofClusterHR*,TofClusterHR*> dpair,number &edis);
/// Up Down Best Match Pair
  static int  PairSearchUD(pair<TofClusterHR*,TofClusterHR*> sedpair,int sedud,int opt);//sed to search(opt=0 pair opt=1 counter)
/// Erase Hit From vector
  static int  TOFClErase(TofClusterHR* tfhit[4]);
/**@}*/ 

/// Other function
public:
/// Sort TofRawSide accoding to BarId
  static bool SideCompare(const TofRawSideR& a,const TofRawSideR& b){return a.swid<b.swid;}
/// Sort TofRawSide Index 
template <class T>
  static bool IdCompare(const pair<integer,T> &a,const pair<integer,T> &b){return a.second<b.second;}
/// Sort TofClusterHR Pair-Index
  static bool PairCompare(const pair<TofClusterHR*,TofClusterHR*> &a,const pair<TofClusterHR*,TofClusterHR*> &b);
/// ParticleR ChargeR Build Link index to BetaH
  static int  BetaHLink(TrTrackR* ptrack,TrdTrackR *trdtrack,EcalShowerR *ecalshow);
/// ParticleR ReBuild Link index to TofTrack BetaH
  static int  BetaHReLink();


/// friend access
  friend class BetaHR; 
  friend class TofBetaPar; 

  ClassDef(TofRecH,8)
};

/////////////////////////////////////////////////////////////////////////
#endif
