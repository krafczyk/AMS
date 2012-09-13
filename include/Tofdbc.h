//  $Id: Tofdbc.h,v 1.17 2012/09/13 19:52:30 qyan Exp $

//Athor Qi Yan 2012/01/05 for new Tof database qyan@cern.ch

#ifndef __TOFDBCQ__
#define __TOFDBCQ__ 1

#ifndef __ROOTSHAREDLIBRARY__
#include <string.h>
#include <stdlib.h>
#endif
#include "typedefs.h"
#include "tofanticonst.h"
#include "point.h"
#include <map>

class AMSTimeID;

//--TOF Const 
namespace TOFCSN{
   const integer SCLRS=4;
   const integer SCMXBR=10;
   const integer NBAR[SCLRS]={8,8,10,8};
   const integer SCANMT=700;
   const integer SCANMY=50;   
   const integer SCANMX=10;
   const integer NBARN=34;
   const integer NSIDE=2;
   const integer NPMTM=3;
}

namespace TOFDBcN{
  const uinteger SIDEN=1;
  const uinteger SIDEP=2;
  const uinteger NOHTRECOVCAD=4;//NO HT  recover Candidate
  const uinteger NOMATCHRECOVCAD=8;// LT HT Not Match recover Candidate
  const uinteger BAD=16;
  const uinteger USED=32;
  const uinteger BADN=64;
  const uinteger BADP=128;
  const uinteger BADTCOO=256;
  const uinteger BADTIME=512;   //Bad Time
  const uinteger NOWINDOWLT=1024;//NoLT (>40ns befFT)&&(<640 befFT)
  const uinteger NOWINDOWHT=2048;
  const uinteger NOWINDOWSHT=4096;
  const uinteger LTMANY=8192;
  const uinteger HTMANY=          16384;
  const uinteger NOADC=           16384*2; 
  const uinteger RECOVERED=       16384*2*2;//RECOVER
  const uinteger LTREFIND=        16384*2*2*2; //LTREFIND for RECOVERCAD
  const uinteger TKTRACK=         16384*2*2*2*2; //TRACK BETAH
  const uinteger DOVERFLOW=       16384*2*2*2*2*2*2*2;//Dynode ADC overflow
  const uinteger AOVERFLOW=       16384*2*2*2*2*2*2*2*2;//Anode ADC overflow
  const uinteger TRDTRACK=        16384*2*2*2*2*2*2*2*2*2;//TRD TRACK BetaH
  const uinteger BETAOVERFLOW=    16384*2*2*2*2*2*2*2*2*2*2;//Beta Fit overflow
  const uinteger ECALTRACK=       16384*2*2*2*2*2*2*2*2*2*2*2;//ECAL TRACK BetaH
  const uinteger TOFTRACK=        16384*2*2*2*2*2*2*2*2*2*2*2*2;//TOF TRACK BetaH
  const uinteger DOVERFLOWNONLC=  16384*2*2*2*2*2*2*2*2*2*2*2*2*2;//Dynode NonLear-Correction Overflow
  const uinteger AOVERFLOWNONLC=  16384*2*2*2*2*2*2*2*2*2*2*2*2*2*2;//Aynode NonLear-Correction Overflow
  const uinteger DOVERFLOWBIRKC=  16384*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2;//Dynode Birk-Correction Overflow
  const uinteger AOVERFLOWBIRKC=  16384*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2;//Aynode Birk-Correction Overflow
}

//---TOF Simple Geometry
class TOFGeom{
  public:
   static const int NLAY=4;
   static const int NBAR=10;
   static const int NSIDE=2;
   static const int NPMTM=3;
   static const int Nbar[NLAY];
   static const int Proj[NLAY];
   static const int Npmt[NLAY][NBAR];
   static const float Overlapw;
   static const float Normw;
   static const float Nthick;
   static const float Honshift[2][2];
   static const float Sci_w[NLAY][NBAR];
   static const float Sci_l[NLAY][NBAR];
   static const float Sci_t[NLAY][NBAR];
   static const float Sci_pt[NLAY][NBAR];
   static const float Sci_pz[NLAY][NBAR];

  public:
   TOFGeom(){};
   static bool      IsTrapezoid(int ilay,int ibar){return ((ibar==0)||(ibar==Nbar[ibar]));}
   static AMSPoint  GetBarCoo(int ilay,int ibar);
   static AMSPoint  GToLCoo(int ilay,int ibar,AMSPoint gpos);//Global To Local Coo
   static AMSPoint  LToGCoo(int ilay,int ibar,AMSPoint lpos);//Local To Global Coo
   static bool      IsInSideBar(int ilay,int ibar,float x, float y,float z=0);//z=0 z not use
   static bool      IsInSideTOF(int ilay,float x, float y,float z=0);
   static int       FindNearBar(int ilay,float x, float y, float &dis,bool &isinbar,float z=0);
   static bool      IsInOverlap(int ilay,float x, float y,int nexcl=1);//
   static void      GetBarEdge(int ilay,int ibar,float x[3][2]);//first 3 xyz //second 2 lh
   static void      GetLayEdge(int ilay,float x[3][2]);//first 3 xyz //second 2 lh(max size)
};


//----get random x from a[] distribution
class TOFTool{
  TOFTool(){};
  static int rand(const int n,geant a[],geant rand){
     geant tot=0;int i;
     for(i=0;i<n;i++)tot+=a[i];
     geant b[n];
     b[0]=a[0]/tot;
     for(i=1;i<n;i++)b[i]=b[i-1]+a[i]/tot;
     for(i=0;i<n;i++)if(b[i]>=rand)break;
     return i; 
   }
};


//----store arr[] to hist 
class TOFHist{
private:
   int _xbn;//nbin
   geant _xbl;//x low
   geant _xbh;//x high
   geant _xbw;//x bin width
   vector<geant> _y;//y arr 
   vector<geant> _yi;//y intergral arr
//   void init();
public:
   TOFHist(){};
   TOFHist(int nbin,geant bl,geant bh,geant arr[]);//same x width arr
   TOFHist(int nbin,geant x[],geant arr[],int nbinx);//not same width arr
//   TOFHist(int nbin,geant x[],geant arr[],geant nbinw);//not same width arr
   geant gety(int ibin);//find y num
   geant getx(geant rand);//distribution to get random x
   int   getnb(){return _xbn;}
   geant getxc(int ibin){return _xbl+(ibin-1+0.5)*_xbw;}
   geant getxl(){return _xbl;}
   geant getxh(){return _xbh;}
   const TOFHist& operator=(const TOFHist &right);
};

#ifndef __ROOTSHAREDLIBRARY__
//--pmt information hist
class TOFPMT{
  public:
     TOFPMT(){}
     static void init();
     static void build();
     static geant eff[TOF2GC::SCLRS][TOF2GC::SCMXBR][2][TOF2GC::PMTSMX];//2 side eff 
     static TOFHist pmpulse;//SE-pulse
     static TOFHist pmamp;//SE-Amp distribution   
     //static geant  pmapar[TOF2GC::SCLRS][TOF2GC::SCMXBR][2][TOF2GC::PMTSMX][3];
     static geant pmgain[TOF2GC::SCLRS][TOF2GC::SCMXBR][2][TOF2GC::PMTSMX];//gain
     static geant pmgaind[TOF2GC::SCLRS][TOF2GC::SCMXBR][2][TOF2GC::PMTSMX];//gain
     static geant pmvol[TOF2GC::SCLRS][TOF2GC::SCMXBR][2][TOF2GC::PMTSMX];//voltage
     static geant daqthr[TOF2GC::SCLRS][TOF2GC::SCMXBR][2][3];//LT HT SHT threshold
     static geant pmttm[TOF2GC::SCLRS][TOF2GC::SCMXBR][2][TOF2GC::PMTSMX];//PMT trasmit time
     static geant pmtts[TOF2GC::SCLRS][TOF2GC::SCMXBR][2][TOF2GC::PMTSMX];
     static geant ansat[TOF2GC::SCLRS][TOF2GC::SCMXBR][2][2];//Anode 2saturation par
     static geant emitt[2];
     static geant phriset();
     static geant phseamp(integer nph=1);
};  

//=======================================================================
//  <--- to store time-points, corresp. to integral 0-1 with bin 1/(SCTPNMX-1)
class TOFTpointsN{
//
private:
  geant bini;// bin width in integr.distr
  geant times[TOFCSN::SCANMT+1]; // array of time points
//
public:
  TOFTpointsN(){};
 ~TOFTpointsN(){};
  TOFTpointsN(int _nbin, geant _bnl, geant _bnw, geant arr[]);
//
//function to get time-point for given value of integral r(0-1) :
  geant gettime(geant r){
    integer i;
    geant ri,t;
    ri=r/bini;
    i=integer(ri);
    if(i>TOFCSN::SCANMT)i=TOFCSN::SCANMT;
    if(i<TOFCSN::SCANMT)t=times[i]+(times[i+1]-times[i])*(ri-geant(i));
    else t=times[i];
    return t;
  }
//
  void getarr(geant arr[]){
    for(int i=0;i<=TOFCSN::SCANMT;i++)arr[i]=times[i];
  }
  geant getbini(){ return bini;}
};
//=====================================================================

//=======================================================================
class TOF2ScanN{
//
private:
  int nscpnts;      // real number of scan points in given w-division
  int indsp[TOFCSN::SCANMY]; //array of long.scan-points indexes(refered to wdiv=1)
  geant scanp[TOFCSN::SCANMY]; //array of longit. scan points
  geant eff1[TOFCSN::SCANMY];  //effic. for -X side PMT
  geant eff2[TOFCSN::SCANMY];  //effic. for +X side PMT
  geant rgn1[TOFCSN::SCANMY];  //PM rel.gain for -X side PMT
  geant rgn2[TOFCSN::SCANMY];  //PM rel.gain for +X side PMT
  geant psh1[TOFCSN::SCANMY][TOF2GC::PMTSMX];// ph.el. sharing over PMTs (s1)
  geant psh2[TOFCSN::SCANMY][TOF2GC::PMTSMX];// ph.el. sharing over PMTs (s2)
//  TOFHist tdist1[TOFCSN::SCANMY];// t-points arrays for -X side
//  TOFHist tdist2[TOFCSN::SCANMY];// t-points arrays for +X side
   TOFTpointsN tdist1[TOFCSN::SCANMY];// t-points arrays for -X side
   TOFTpointsN tdist2[TOFCSN::SCANMY];// t-points arrays for +X side
public:
  TOF2ScanN(){};
  TOF2ScanN(const integer nscp, const int _indsp[], const geant _scanp[],
                         const geant _eff1[], const geant _eff2[],
                         const geant _rgn1[], const geant _rgn2[],
                         const geant _psh1[][TOF2GC::PMTSMX],const geant _psh2[][TOF2GC::PMTSMX],
//                         const TOFHist _tdist1[], const TOFHist _tdist2[]);
                         const TOFTpointsN _tdist1[], const TOFTpointsN  _tdist2[]);
  void getybin(const geant , int &, int &, geant &);// calc. index of Y-position
// <--- functions to get efficiency ( getxbin should be called first!!!)
  geant getef1(const geant r, const int i1, const int i2){
    if(i1==i2){return eff1[i1];}
     else{return (eff1[i1]+(eff1[i2]-eff1[i1])*r);}
  }
  geant getps1(const int ipm, const geant r, const int i1, const int i2){
    if(i1==i2){return psh1[i1][ipm];}
    else{return (psh1[i1][ipm]+(psh1[i2][ipm]-psh1[i1][ipm])*r);}
  }
  geant getef2(const geant r, const int i1, const int i2){
    if(i1==i2){return eff2[i1];}
    else{return (eff2[i1]+(eff2[i2]-eff2[i1])*r);}
  }
  geant getps2(const int ipm, const geant r, const int i1, const int i2){
    if(i1==i2){return psh2[i1][ipm];}
    else{return (psh2[i1][ipm]+(psh2[i2][ipm]-psh2[i1][ipm])*r);}
  }
  geant gettm1(const geant r, const int i1, const int i2);
  geant gettm2(const geant r, const int i1, const int i2);
};

//=====================================================================
//        <--- general class for counter MC-scan(width+leng.scan.points)
class TOFWScanN{
//
private:
  int npmts; //real number of PMTs per side
  int nwdivs; // real number of width-divisions
  geant divxl[TOFCSN::SCANMX];//division xlow
  geant divxh[TOFCSN::SCANMX];//division xhigh
  TOF2ScanN wscan[TOFCSN::SCANMX];//array of single-division scans
public:
  static TOFWScanN scmcscan1[TOF2GC::SCLRS][TOF2GC::SCMXBR];
  TOFWScanN(){};
  TOFWScanN(const int npm, const int nwdvs,const geant _dxl[],
           const geant _dxh[],const TOF2ScanN  _sdsc[]);
//
  int getnpmts(){return(npmts);}
  int getndivs(){return(nwdivs);}
//   Func.to calc.X(width)-division index for given X(counter local r.s)
  int getwbin(const geant x);
//   Func. to calc.Y-pos indexes for division div:
  void getybin(const int div, const geant y, int &i1, int &i2, geant &r){wscan[div].getybin(y, i1, i2, r);}
//   Func. to calc. pm1/pm2 eff for division div:
  geant getef1(const int div, const geant r, const int i1, const int i2){return(wscan[div].getef1(r, i1, i2));}
  geant getef2(const int div, const geant r, const int i1, const int i2){return(wscan[div].getef2(r, i1, i2));}
  geant getps1(const int ipm, const int div, const geant r, const int i1, const int i2){return(wscan[div].getps1(ipm, r, i1, i2));}
  geant getps2(const int ipm, const int div, const geant r, const int i1, const int i2){return(wscan[div].getps2(ipm, r, i1, i2));}
//   Func. to calc. pm1/pm2 times for division div:
  geant gettm1(const int div, const geant r, const int i1, const int i2){return(wscan[div].gettm1(r, i1, i2));}
  geant gettm2(const int div, const geant r, const int i1, const int i2){return(wscan[div].gettm2(r, i1, i2));}
  static void build();
};
#endif

//Tof Time Alignment pars
//===========================================================
template <typename T1>
class TofTDVTool{
 public:
   int   TDVSize;
   int   TDVParN;
   T1*   TDVBlock;//
   char *TDVName;
   bool  Isload;//has load or not
   int   BRun;//begin run id
   int   ERun;//end   run id
//----
 public:
   TofTDVTool():TDVSize(0),TDVBlock(0),TDVName(0),Isload(0),BRun(0),ERun(0),TDVParN(0){};
   virtual void LoadTDVPar(){};//copy TDV to class
   virtual void GetTDVPar(){};//copy class to TDV
   virtual int  ReadTDV(int rtime,int real=1);
   virtual int  WriteTDV(int brun,int erun,int real=1);
   virtual int  WriteTDV(int real=1){return WriteTDV(BRun,ERun,real);}
   virtual void PrintTDV(){};
};


//===========================================================
class TofTAlignPar: public TofTDVTool<float>{
  public:
//---Time Part
     std::map<int, float>slew; //id LBS0 ///Time Slewing
     std::map<int, float>delay;//id LB00 ///Time Delay
     float powindex;           ///Shape Index
//---Coo Part
     std::map<int, float>dt1; //id LB00 ///Coo Time T1
     std::map<int, float>dslew; //id LB00 ///Coo LR Slew Compensate
     std::map<int, float>vel; //id LB00 ///Coo Vel
//----
  public:
     TofTAlignPar();
     TofTAlignPar(float *arr,int brun,int erun);//load 
     static TofTAlignPar *Head;
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp threadprivate (Head)   
#endif
     static TofTAlignPar *GetHead();
     static void HeadLoadTDVPar(){GetHead()->LoadTDVPar();}
     void LoadTDVPar();//copy TDV to class 
     int  LoadFromFile(char *file);//read data from file->Block data
     void PrintTDV();
};

//===========================================================
class TofTdcCorN{
private:
  int _bmap;//bit-map of abs.chan.numbers presented in calib.data ( lsb -> 1st ch.)
  geant _icor[TOF2GC::SCTDCCH-2][1024];//integr.nonlin. corrections, based on 10 lsb of TDC-count(bin#)
public:
  TofTdcCorN(){};
  geant getcor(int time, int ch){ 
    int t10=(time&(0x3FFL));//10 lsb of TDC-count(time measurement)
    if(t10<=0)return(0);
    return _icor[ch][t10-1];
  }
//
  bool truech(int ch){
    return ((_bmap & (1<<ch))!=0);
  }
  static TofTdcCorN tdccor[TOF2GC::SCCRAT][TOF2GC::SCFETA-1];
};

//==========================================================
class TofAttAlignPar: public TofTDVTool<float>{ //Scintillator Attenuation Algin
  public:
     static const int nattpar=7;
     std::map<int, float>attpar[nattpar]; //id LBS0 ///attenuation correction
//----
  public:
     TofAttAlignPar();
     TofAttAlignPar(float *arr,int brun,int erun);//load 
     static TofAttAlignPar *Head;
#pragma omp threadprivate (Head)   
     static TofAttAlignPar *GetHead();
     static void HeadLoadTDVPar(){GetHead()->LoadTDVPar();}
     void LoadTDVPar();//copy TDV to class 
     int  LoadFromFile(char *file);//read data from file->Block data
     void PrintTDV();
};

//==========================================================
class TofPMAlignPar: public TofTDVTool<float>{// PMT Anode Gain Align
   public:
     static const int nalign=1; 
     std::map<int, float>gaina; //id LBSP ///anode gain //reference to 1
//----
  public:
     TofPMAlignPar();
     TofPMAlignPar(float *arr,int brun,int erun);//load 
     static TofPMAlignPar *Head;
#pragma omp threadprivate (Head)   
     static TofPMAlignPar *GetHead();
     static void HeadLoadTDVPar(){GetHead()->LoadTDVPar();}
     void LoadTDVPar();//copy TDV to class 
     int  LoadFromFile(char *file);//read data from file->Block data
     void PrintTDV();
};

//==========================================================
class TofPMDAlignPar: public TofTDVTool<float>{// PMT Dynode Gain Align
   public:
     static const int ndead=1;//Dead Dynode PMT Number
     static const int dpmid[ndead];//Dynode PMT-ID
     std::map<int, float>gaind; //id LBS0 ///dynode gain //refer to carbon
     std::map<int, float>gainds;//dynode MIP sigma
//----
  public:
     TofPMDAlignPar();
     TofPMDAlignPar(float *arr,int brun,int erun);//load 
     static TofPMDAlignPar *Head;
#pragma omp threadprivate (Head)   
     static TofPMDAlignPar *GetHead();
     static void HeadLoadTDVPar(){GetHead()->LoadTDVPar();}
     void LoadTDVPar();//copy TDV to class 
     int  LoadFromFile(char *file);//read data from file->Block data
     void PrintTDV();
};

//==========================================================
class TofCAlignPar: public TofTDVTool<float>{
  public:
     static const int nansat=3;// Anode Saturation Par Number
     static const float ProEdep=1.67;//Proton Edep Norm to 1.67MeV
     static const int   RecMinPmD=2; //Min Dynode PM Requirement
     std::map<int, float>dycor; //id LBSP DynodeQ=dycor*Z^2/(1+birk*Z^2);
     std::map<int, float>birk;  //id LB00 Birk Const
//--Dynode PMT Resolution weight factor for Counter Edep Calculation//Tunning According to Carbon
     std::map<int, float>dypmw;
     std::map<int, float>ansat[nansat];
//--default par 
     static const float def_birk[TOFCSN::SCLRS][TOFCSN::SCMXBR];
     static const float def_dycor[TOFCSN::SCLRS][TOFCSN::NSIDE][TOFCSN::SCMXBR][TOFCSN::NPMTM];
     static const float def_dypmw[TOFCSN::SCLRS][TOFCSN::NSIDE][TOFCSN::SCMXBR][TOFCSN::NPMTM];
     static const float def_ansat[nansat][TOFCSN::SCLRS][TOFCSN::NSIDE][TOFCSN::SCMXBR];
//----
  public:
     TofCAlignPar();
     TofCAlignPar(float *arr,int brun,int erun);//load 
     static TofCAlignPar *Head;
#pragma omp threadprivate (Head)   
     static TofCAlignPar *GetHead();
     static void HeadLoadTDVPar(){GetHead()->LoadTDVPar();}
     void LoadOptPar(int opt=0);//copy TDV to class 0 From TDV 1 Read From Default
     void LoadTDVPar();//copy TDV to class
     int  LoadFromFile(char *file);//read data from file->Block data
     void PrintTDV();
};


//==========================================================
class TofRecPar: public TofTDVTool<float>{
  public:
     static const  double Tdcbin=0.0244141;
     static const  float FTgate[2];//LT HT relate FTtime age-window(ns) //(FT-LT)ns [80, 200] 
     static const  float FTgate2[2];//Tight cut windows  if no HT SHT
     static const  float LHgate[2]; //LT relate HT MPV //(LT-HT)ns [3, 12]    
     static const  float LHMPV=4.5;  //(LT-HT-MPV) pair window ns
     static const  int   PUXMXCH=3700; //MAX. value of PUX-chip//an(dy)adc chan
     static const  int   MaxCharge=6;     
     static const  int   BetaHLMatch=0;//require Longitude match or not
     static const  float BetaHReg[2];//Seach Region of TMatch LMatch(N Sigma)
     static const  int   BetaHMinL[2];//Min X+Y Match Layer//U+D Match Layer
     static const  float Dynodegate=5;//Dynode ADC should >5
//---BetaH Self Recontruction Par
     static const  float PairQDA=6;//Pair Energy Match Using Anode when Q<6
     static const  float PairQgate=0.8;//Pair Min Energy Q 
     static const  float PairQRgate=1.8;//Q1/Q0<1.8
     static const  float PairLSMatch=9.;//Longitude+Tran Coo 9.cm Match windows 
     static const  float PairTMatch=0.5;//Time Match <0.5ns (3*0.16ns)
     static const  float TFEcalAMatch=10.;//TOF EcalShow Max Match Angle(6 deg)
     static const  float DPairChi2TCut=100;//ChisT Cut
     static const  float DPairChi2CCut=200;//ChisC Cut
     static const  float NonTkBetaCutL=0.3;//beta low
     static const  float NonTkBetaCutU=1.5;//beta ligh
//---
     static float TimeSigma[MaxCharge];
     static float CooSigma[MaxCharge][TOFCSN::SCLRS][TOFCSN::SCMXBR];
     static int   iLay;
     static int   iBar;
     static int   iSide;   
     static int   Idsoft;
#pragma omp threadprivate (iLay,iBar,iSide,Idsoft)
 public:
     TofRecPar(){};
     static void  IdCovert(int id){iLay=id/1000%10;iBar=id/100%10;iSide=id/10%10;}
     static void  IdCovert(int ilay,int ibar,int is=0,int ipm=0){Idsoft=1000*ilay+100*ibar+is*10+ipm;}
     static float GetTimeSigma(int id, int icha=1);
     static float GetCooSigma(int id, int icha=1);
};

//===========================================================
/// class to Manager All Tof Align
class TofAlignManager {
  public:
   /// all tdv map
    map<string,AMSTimeID*> tdvmap; 
    int isreal;//real par or mc
   
  public:
   TofAlignManager(int real);
   ~TofAlignManager(){};
   static TofAlignManager *Head;
   static TofAlignManager *GetHead(int real);
   static void ClearHead(){if(Head){Head->Clear();delete Head;Head=0;}}

  public:
    int    Validate(unsigned int time);
    void   Clear();  
};

//===========================================================
#endif
