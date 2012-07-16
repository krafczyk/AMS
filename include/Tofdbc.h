//  $Id: Tofdbc.h,v 1.6.2.1 2012/07/16 23:52:37 qyan Exp $

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

//--TOF Const 
namespace TOFCSN{
   const integer SCLRS=4;
   const integer NBAR[SCLRS]={8,8,10,8};
   const integer SCANMT=700;
   const integer SCANMY=50;   
   const integer SCANMX=10;
   const integer NBARN=34;
   const integer NSIDE=2;
}

namespace TOFDBcN{
  const uinteger BAD=16;
  const uinteger USED=32;
  const uinteger BADHIS=128;
  const uinteger ONESIDE=256;
  const uinteger BADTIME=512;
  const uinteger TRDTRACK=16384*2*2*2*2*2*2*2*2*2;
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
};


//===========================================================
class TofTAlignPar: public TofTDVTool<float>{
  public:
     std::map<int, float>slew; //id LBS0
     std::map<int, float>delay;//id LB00
     float powindex;
//----
  public:
     TofTAlignPar();
     TofTAlignPar(float *arr,int brun,int erun);//load 
     static TofTAlignPar *Head;
     static TofTAlignPar *GetHead();
     static void HeadLoadTDVPar(){GetHead()->LoadTDVPar();}
     void LoadTDVPar();//copy TDV to class 
     int  LoadFromFile(char *file);//read data from file->Block data
};  

void TofTAlignInit();
//---
#endif
