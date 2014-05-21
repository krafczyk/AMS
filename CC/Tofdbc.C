//  $Id$

//Athor Qi Yan 2012/01/05 new Tof database IHEP Version
// ------------------------------------------------------------
//      History
//        Modified:  Adding phseamp  2012/05/16
//        Modified:  DB update       2010/05/28
//        Modified:  TOF New Calibration TDV Template 2012/06/12
//        Modified:  Simple Geometry Adding            2012/06/23
//        Modified:  Reconstruction  Par Adding        2012/07/08
//        Modified:  TDV Manager         Adding        2012/07/19
//        Modified:  Attnuation Calibration+Anode Gain 2012/08/20
//        Modified:  Dynode Gain Calibration           2012/08/31
// -----------------------------------------------------------
#ifndef __ROOTSHAREDLIBRARY__
#include "cern.h"
#include "amsdbc.h"
#include <string>
#include <stdlib.h>
#include <math.h>
#include "ntuple.h"
#include "amsstl.h"
#include "commons.h"
#include "tofsim02.h"
#include "mccluster.h"
#include "mceventg.h"
#include "tofid.h"
#include "job.h"
#endif
#include "Tofdbc.h"
#include "commonsi.h"
#include "timeid.h"
#include <fstream>
#include "Tofcharge_ihep.h"

const float TofRecPar::AttLenLimit=50;
const float TofRecPar::TkLSMatch=0.3;//0.3cm
const float TofRecPar::TkLSMatch2=0.5;//0.5cm
const float TofRecPar::TRDLSMatch2=5;//5cm
//Overlap



const float TOFGeom::Overlapw=0.5;//cm
const float TOFGeom::Normw=12.;//cm
const float TOFGeom::Nthick=1.;//cm
//Bar number
const int  TOFGeom::Nbar[NLAY]={8,8,10,8};
//Bar Projection(Along 0x1y)
const int  TOFGeom::Proj[NLAY]={0,1,1,0};
//PMT number
const int  TOFGeom::Npmt[NLAY][NBAR]={
   3, 2, 2, 2, 2, 2, 2, 3, 0, 0,
   2, 2, 2, 2, 2, 2, 2, 2, 0, 0,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
   3, 2, 2, 2, 2, 2, 2, 3, 0, 0
};
//Sci_width
const float TOFGeom::Sci_w[NLAY][NBAR]={
       22.5, 12., 12., 12., 12., 12., 12., 22.5, 0.,   0.,
       25.5, 12., 12., 12., 12., 12., 12., 25.5, 0.,   0.,
 18.5, 12.,  12., 12., 12., 12., 12., 12., 12.,  18.5,
       26.,  12., 12., 12., 12., 12., 12., 26.,  0.,   0.
};
//Sci_length
const float TOFGeom::Sci_l[NLAY][NBAR]={
       126.5, 130.5, 130.5, 130.5, 130.5, 130.5, 130.5, 126.5,  0.,  0.,
       117.2, 127.0, 132.2, 132.2, 132.2, 132.2, 127.0, 117.2,  0.,  0.,
110.0, 117.2, 127.0, 132.2, 132.2, 132.2, 132.2, 127.0, 117.2,  110.0,
       130.0, 134.0, 134.0, 134.0, 134.0, 134.0, 134.0, 130.0,  0.,  0.
};

//Sci_cut_width
const float TOFGeom::Sci_wc[NLAY][NBAR]={
      5.,  0.,0.,0.,0.,0.,0.,5.,  0., 0.,
      13.5,0.,0.,0.,0.,0.,0.,13.5,0., 0.,
 6.5, 0.,  0.,0.,0.,0.,0.,0.,0.,  6.5,
      8.5, 0.,0.,0.,0.,0.,0.,8.5, 0.,0.,
};

//Sci_cut_length
const float TOFGeom::Sci_lc[NLAY][NBAR]={
       25., 0.,0.,0.,0.,0.,0.,25., 0, 0.,
       24.3,0.,0.,0.,0.,0.,0.,24.3,0, 0.,
  20., 0.,  0.,0.,0.,0.,0.,0.,0.,  20.,
       26.9,0.,0.,0.,0.,0.,0.,26.9,0.,0.
};

//Sci_thick
const float TOFGeom::Sci_t[NLAY][NBAR]={//cm
   1., 1., 1., 1., 1., 1., 1., 1., 0., 0.,
   1., 1., 1., 1., 1., 1., 1., 1., 0., 0.,
1.,1., 1., 1., 1., 1., 1., 1., 1., 1.,
   1., 1., 1., 1., 1., 1., 1., 1., 0., 0.
};
//Sci short(Trans) central Pos
const float TOFGeom::Sci_pt[NLAY][NBAR]={
      -45.5,  -28.75, -17.25, -5.75, 5.75, 17.25, 28.75, 45.5,  0., 0.,
      -47.,   -28.75, -17.25, -5.75, 5.75, 17.25, 28.75, 47.,   0., 0.,
-55., -40.25, -28.75, -17.25, -5.75, 5.75, 17.25, 28.75, 40.25, 55.,
      -47.25, -28.75, -17.25, -5.75, 5.75, 17.25, 28.75, 47.25, 0., 0.,
};
//Sci_z position
const float TOFGeom::Sci_pz[NLAY][NBAR]={//counter central cm
         64.425, 65.975, 64.425, 65.975, 64.425, 65.975, 64.425, 65.975, 0., 0., //!  __-_-_-_--
         61.325, 62.875, 61.325, 62.875, 61.325, 62.875, 61.325, 62.875, 0., 0., //!   _-_-_-_-
-62.875,-61.325,-62.875,-61.325,-62.875,-61.325,-62.875,-61.325,-62.875,-61.325, //! _-_-_-_-_-_
        -64.425,-65.975,-64.425,-65.975,-64.425,-65.975,-64.425,-65.975, 0., 0., //!  --_-_-_-__
};
//honeycomp shift
const float TOFGeom::Honshift[2][2]={//topxy +bowxy 0.1cm 0.1
  0., 0.,
  0., 0.
};
//---Trapzoid or not
bool TOFGeom::IsTrapezoid(int ilay,int ibar){
  return ((ibar==0)||(ibar==Nbar[ilay]-1));
}

//--Bar Coo(include shift)
AMSPoint TOFGeom::GetBarCoo(int ilay,int ibar){
    AMSPoint coo(0,0,0);
//--other direction should+pt
   coo[1-Proj[ilay]]=Sci_pt[ilay][ibar]+Honshift[ilay/2][1-Proj[ilay]];
   coo[Proj[ilay]]=Honshift[ilay/2][Proj[ilay]];
   coo[2]=Sci_pz[ilay][ibar];
   return coo;
}
//--Global To Local Coo(of ilay ibar) //Along 0=>bar along x 1=>bar along y(local)
AMSPoint  TOFGeom::GToLCoo(int ilay,int ibar,AMSPoint gpos){
   AMSPoint coo=GetBarCoo(ilay,ibar);
   coo=gpos-coo;//glbal-barcoo
   return coo;
}
//--Local To Global Coo(of ilay ibar)
AMSPoint  TOFGeom::LToGCoo(int ilay,int ibar,AMSPoint lpos){
   AMSPoint coo=GetBarCoo(ilay,ibar);
   coo=lpos+coo;//local+barcoo
   return coo;
}
//--to Judge whether Inside Counter
bool TOFGeom::IsInSideBar(int ilay,int ibar,float x,float y,float &trapdis,float z){
   AMSPoint rcoo(x,y,z);
   AMSPoint coo=GToLCoo(ilay,ibar,rcoo);
   bool lok=0,sok=0,zok=1,tok=1;
   if(fabs(coo[Proj[ilay]])<Sci_l[ilay][ibar]/2.) lok=1;//y
   if(fabs(coo[1-Proj[ilay]])<Sci_w[ilay][ibar]/2.)sok=1;//x
//-----
   if(IsTrapezoid(ilay,ibar)&&lok){//Trapzoid Case
      double xn=coo[1-Proj[ilay]];//X Project
      double yn=coo[Proj[ilay]];//Y Project
//---
      double x0,y0,x1,y1,kk,bb;
//----All Bar=0 
       x0=-Sci_w[ilay][ibar]/2.;
       y0=Sci_l[ilay][ibar]/2.-Sci_lc[ilay][ibar]; 
       x1=-(Sci_w[ilay][ibar]/2.-Sci_wc[ilay][ibar]);
       y1=Sci_l[ilay][ibar]/2.; 
 //--Trapzoid
      if(ibar==Nbar[ilay]-1){
        x0=-x0;
        x1=-x1;
      } 
      if(yn<0){
        y0=-y0;
        y1=-y1;
      }
      kk=(y0-y1)/(x0-x1);
      bb=(x0*y1-x1*y0)/(x0-x1);
//---
      if     (ibar==0&&yn>=0)           tok=(kk*xn+bb-yn>0);//accept sy
      else if(ibar==0&&yn<0)            tok=(kk*xn+bb-yn<0);//accept by
      else if(ibar==Nbar[ilay]-1&&yn>=0)tok=(kk*xn+bb-yn>0);//accept sy
      else if(ibar==Nbar[ilay]-1&&yn<0) tok=(kk*xn+bb-yn<0);//accept by
      double kk1=-1./kk; 
      double bb1=yn-kk1*xn;
      double xt=-(bb1-bb)/(kk1-kk);
      double yt=(-bb1*kk+bb*kk1)/(kk1-kk);
      trapdis=sqrt(pow(xt-xn,2.)+pow(yt-yn,2.));
   }
   else trapdis=9999999;
//-----
   if(z!=0){zok=(fabs(coo[2])<Sci_t[ilay][ibar]/2.)?1:0;}
   bool sumok=(lok&&sok&&zok&&tok);
   return sumok;
}
//--To Judge whether Inside TOF Layer
bool TOFGeom::IsInSideTOF(int ilay,float x,float y,float z){
  bool inside=0;
  float bdis;
  for(int ibar=0;ibar<Nbar[ilay];ibar++){if(IsInSideBar(ilay,ibar,x,y,bdis,z)){inside=1;break;}}
  return inside;
}
//--Find Nearest Counter
int TOFGeom::FindNearBar(int ilay,float x,float y,float &dis,bool &isinbar,float z){
    float mindis=10000000;int minbar=-1;
    AMSPoint gcoo(x,y,z),lcoo;
    for(int ibar=0;ibar<Nbar[ilay];ibar++){
      lcoo=GToLCoo(ilay,ibar,gcoo);
      float nowdis=fabs(lcoo[1-Proj[ilay]])-Sci_w[ilay][ibar]/2.;
      if(z==0){ if(nowdis<mindis){dis=lcoo[1-Proj[ilay]],mindis=nowdis;minbar=ibar;} }
      else    { if(lcoo.norm()<mindis){dis=lcoo.norm(),mindis=dis,minbar=ibar;}}
    }
    float bdis=0;
    isinbar=IsInSideBar(ilay,minbar,x,y,bdis,z);
    if(!isinbar){
      if(minbar>0&&IsInSideBar(ilay,minbar-1,x,y,bdis,z)){isinbar=1;minbar=minbar-1;}
      if(minbar<Nbar[ilay]-1&&IsInSideBar(ilay,minbar+1,x,y,bdis,z)){isinbar=1;minbar=minbar+1;}
    }
    return minbar;
}
//--Is in  Overlap region
bool TOFGeom::IsInOverlap(int ilay,float x, float y,int nexcl){
    float dis; bool isinbar;
    int minbar=FindNearBar(ilay,x,y,dis,isinbar);
    if(minbar<0)return 0;
    bool selfch=0;
    if(((minbar==0)&&(dis<0))||((minbar==Nbar[ilay]-1)&&(dis>0)))selfch=0;
    else {
      selfch=((fabs(dis)>Sci_w[ilay][minbar]/2.-Overlapw*nexcl)&&isinbar);
    }
   return selfch;
}
//--Find Bar Edge
void TOFGeom::GetBarEdge(int ilay,int ibar,float x[3][2]){
  AMSPoint barpos=GetBarCoo(ilay,ibar);
  x[1-Proj[ilay]][0]=barpos[1-Proj[ilay]]-Sci_w[ilay][ibar]/2.;
  x[1-Proj[ilay]][1]=barpos[1-Proj[ilay]]+Sci_w[ilay][ibar]/2.;
  x[Proj[ilay]][0]=barpos[Proj[ilay]]-Sci_l[ilay][ibar]/2.;
  x[Proj[ilay]][1]=barpos[Proj[ilay]]+Sci_l[ilay][ibar]/2.;
  x[2][0]=barpos[2]-Sci_t[ilay][ibar]/2.;
  x[2][1]=barpos[2]+Sci_t[ilay][ibar]/2.;
}
//--Find Layer Max Edge
void TOFGeom::GetLayEdge(int ilay,float x[3][2]){
  x[0][0]=x[0][1]=x[1][0]=x[1][1]=x[2][0]=x[2][1]=0;
  float x1[3][2];
  for(int ibar=0;ibar<Nbar[ilay];ibar++){
     GetBarEdge(ilay,ibar,x1);
     for(int ipr=0;ipr<3;ipr++){
     if(x1[ipr][0]<x[ipr][0])x[ipr][0]=x1[ipr][0];
     if(x1[ipr][1]>x[ipr][1])x[ipr][1]=x1[ipr][1];
    }
  }
}

//--GetMeanZ of iLayer
float TOFGeom::GetMeanZ(int ilay){
  float  meanz=0;
  for(int ibar=0;ibar<Nbar[ilay];ibar++){meanz+=Sci_pz[ilay][ibar];}
  meanz=meanz/Nbar[ilay];
  return meanz;
}

//--Judge Pool Charge PathLength Geometry Region
bool TOFGeom::IsGoodQGeom(int ilay,int ibar,AMSPoint tkpos){

  double cutvol[NLAY][2][NBAR+1]={
       -57.1, -35.15,-23.4, -12.1,-0.5,   11.1, 22.4, 34.2,  56.4, 0, 0,
       -56.2, -33.9, -22.8, -11.,  0.5,   11.9, 23.6, 35.1,  57.1, 0, 0,
//---
       -61.15,-35.1, -23.3, -12.1,-0.55, 10.9, 22.45, 34.25, 59.2, 0, 0,
       -59.65,-33.9,-22.75, -11,   0.45, 11.8, 23.55, 35.13, 60.15,0, 0,
//---
-64.65,-46.6, -35.15, -23.3, -12.1,-0.4,  11.1, 22.5, 34.35,45.5, 63.8,
-63.7, -45.85,-33.95, -22.7, -11.,  0.6,  12,   23.6, 35.3, 46.6, 64.65,
//----
       -60.6, -35.1, -24.2, -11.95,  -0.4,  11.3, 22.5, 34.35, 59.9, 0, 0,
       -59.8, -33.9, -23,   -11.1,    0.4,  11.75, 23.5,35.05, 60.7, 0, 0,
      };

   bool isgvol=1;
///----Pool PathLength Edge Region
   isgvol=(tkpos[1-Proj[ilay]]>cutvol[ilay][1][ibar]&&tkpos[1-Proj[ilay]]<cutvol[ilay][0][ibar+1]);
///---Trapzoid Bad Region
   if(ibar==0||ibar==Nbar[ilay]-1){
      if(ilay==1){
        if(fabs(tkpos[1])+5./3.*fabs(tkpos[0])-325./3.>0)isgvol=0;
      }
     else if(ilay==3){
        if(fabs(tkpos[1])+1./3.*fabs(tkpos[0])-72.>0)isgvol=0;
     }
   }

  return isgvol;
}


//------same x width arr
TOFHist::TOFHist(int nbin,geant bl,geant bh,geant arr[])
:_xbn(nbin),_xbl(bl),_xbh(bh){
  _y.clear();_yi.clear(); 
  geant tot=0;
  _y.push_back(0);//downflow
  for(int i=1;i<=nbin;i++){
    tot+=arr[i-1];
    _y.push_back(arr[i-1]);
  }
 _y.push_back(0);//upflow
//---
  _yi.push_back(_y[0]/tot);  
  for(int i=1;i<=nbin+1;i++)_yi.push_back(_yi[i-1]+_y[i]/tot);
  _xbw=(_xbh-_xbl)/geant(_xbn);
}


//---------
geant TOFHist::getx(geant rand){
  int ib=1;
  for(  ;ib<=_xbn;ib++){
    if(rand<=_yi[ib])break;
   }
  geant x=_xbl+_xbw*(ib-1);
  if(ib==_xbn)return x;
  if(rand>_yi[ib-1]&&(_yi[ib]!=_yi[ib-1])){
     x+=_xbw*(rand-_yi[ib-1])/(_yi[ib]-_yi[ib-1]);
  } 
  return x;  
}

//---------//convert to ununiform bin to uniform bin //expand bin number to nbinx
TOFHist::TOFHist(int nbin,geant x[],geant arr[],const int nbinx){
  geant dx=(x[nbin-1]-x[0])/(nbinx-1);
  geant bl=x[0]-dx/2;
  geant bh=x[nbin-1]+dx/2;
 
  geant arr1[nbinx],xc1=x[0];
  int ibin=1;
//---
  arr1[0]=arr[0];
  arr1[nbinx-1]=arr[nbin-1];
  for(int i=1;i<nbinx-1;i++){
     xc1+=dx;
     while(xc1>x[ibin]){ibin++;}
     arr1[i]=arr[ibin-1]+(arr[ibin]-arr[ibin-1])*(xc1-x[ibin-1])/(x[ibin]-x[ibin-1]);    
   }
  new (this) TOFHist(nbinx,bl,bh,arr1);
//---
}

//---
const TOFHist& TOFHist::operator=(const TOFHist &right){
  _xbn=right._xbn;
  _xbl=right._xbl;
  _xbh=right._xbh;
  _xbw=right._xbw;
  _y=  right._y;
  _yi= right._yi;
  return *this;
}


//-----//get y information
geant TOFHist::gety(int ibin){
  if(ibin<=_xbn&&ibin>=1)return _y[ibin];
  else return 0;
}

#ifndef __ROOTSHAREDLIBRARY__
//----
TOFHist TOFPMT::pmpulse;
TOFHist TOFPMT::pmamp;
geant   TOFPMT::pmgain [TOF2GC::SCLRS][TOF2GC::SCMXBR][2][TOF2GC::PMTSMX];
geant   TOFPMT::pmgaind[TOF2GC::SCLRS][TOF2GC::SCMXBR][2][TOF2GC::PMTSMX];
geant   TOFPMT::pmvol  [TOF2GC::SCLRS][TOF2GC::SCMXBR][2][TOF2GC::PMTSMX];
geant   TOFPMT::pmttm  [TOF2GC::SCLRS][TOF2GC::SCMXBR][2][TOF2GC::PMTSMX];
geant   TOFPMT::pmtts  [TOF2GC::SCLRS][TOF2GC::SCMXBR][2][TOF2GC::PMTSMX];
geant   TOFPMT::daqthr [TOF2GC::SCLRS][TOF2GC::SCMXBR][2][3];
geant   TOFPMT::ansat  [TOF2GC::SCLRS][TOF2GC::SCMXBR][2][2];
geant   TOFPMT::emitt  [2]={0.9,2.1};//TOF rise time+ decay time(ns)

void  TOFPMT::init(){
//---read file
   char fname[1000];
   char *ldir="/afs/ams.cern.ch/user/qyan/Offline/AMSTOF/tofdata/";
   if(TFMCFFKEY.g4tfdir==1)strcpy(fname,ldir);
   else {
       strcpy(fname,AMSDATADIR.amsdatadir);
       strcat(fname,"tofdata/");
    }
   char vers[200];
   UHTOC(TFMCFFKEY.g4tfdb,100,vers,200);
   strcat(fname,vers);
//---

   cout<<"<------TOFPMT::init: Opening  file : "<<fname<<endl;
   ifstream vlfile(fname,ios::in);
   if(!vlfile){
    cerr <<"<---- Error: missing TOFPMT-file !!: "<<fname<<endl;
    exit(1);
   }
//--read gain anode //unit 10^6
   //cout<<"TOF-pmt gain"<<endl;
   for(int ilay=0;ilay<TOF2GC::SCLRS;ilay++){
      for(int is=0;is<2;is++){
        for(int ibar=0;ibar<TOF2GC::SCMXBR;ibar++){
           for(int ipm=0;ipm<TOF2GC::PMTSMX;ipm++){
             vlfile>>pmgain[ilay][ibar][is][ipm];
             //cout<<pmgain[ilay][ibar][is][ipm]<<" ";
           }
         } 
       }
    }
    //cout<<endl;
//--read ref gain dynode
 //  cout<<"TOF-pmt dynode gain"<<endl;
   for(int ilay=0;ilay<TOF2GC::SCLRS;ilay++){
      for(int is=0;is<2;is++){
        for(int ibar=0;ibar<TOF2GC::SCMXBR;ibar++){
           float pmgainas=0;
           for(int ipm=0;ipm<TOF2GC::PMTSMX;ipm++){
              pmgainas+=pmgain[ilay][ibar][is][ipm];//sum anode gain
           }
           for(int ipm=0;ipm<TOF2GC::PMTSMX;ipm++){
             vlfile>>pmgaind[ilay][ibar][is][ipm];//sum anode gain/dynode gain
             if(pmgaind[ilay][ibar][is][ipm]>1)pmgaind[ilay][ibar][is][ipm]=pmgainas/pmgaind[ilay][ibar][is][ipm];
             else                              pmgaind[ilay][ibar][is][ipm]=0.;//impossible dynode>anode sum
             //cout<<pmgaind[ilay][ibar][is][ipm]<<" ";
           }
         }
       }
    }
    //cout<<endl;
//---read hv
    //cout<<"pmt hv"<<endl;
    for(int ilay=0;ilay<TOF2GC::SCLRS;ilay++){
      for(int is=0;is<2;is++){
        for(int ibar=0;ibar<TOF2GC::SCMXBR;ibar++){
           for(int ipm=0;ipm<TOF2GC::PMTSMX;ipm++){
             vlfile>>pmvol[ilay][ibar][is][ipm];
               //cout<<pmvol[ilay][ibar][is][ipm]<<" ";   
           }
         }
       }
    }
    //cout<<endl;
//--LT HT SHT value
  for(int ish=0;ish<3;ish++){
    //cout<<"TOF Threshold: "<<ish<<endl;
    for(int ilay=0;ilay<TOF2GC::SCLRS;ilay++){
      for(int is=0;is<2;is++){
        for(int ibar=0;ibar<TOF2GC::SCMXBR;ibar++){
             vlfile>>daqthr[ilay][ibar][is][ish];
             daqthr[ilay][ibar][is][ish]*=TFMCFFKEY.threref[ish]; 
            // cout<<daqthr[ilay][ibar][is][ish]<<" ";
        }
      }
    }
    //cout<<endl;
  }

//---Tof Anode Saturation Par
  for(int ipar=0;ipar<2;ipar++){
    for(int ilay=0;ilay<TOF2GC::SCLRS;ilay++){
      for(int is=0;is<2;is++){
        for(int ibar=0;ibar<TOF2GC::SCMXBR;ibar++){
             vlfile>>ansat[ilay][ibar][is][ipar];
        }
      }
    }
  }

//---endflag
   int endflag;
   vlfile>> endflag;
   vlfile.close();
   if(endflag==12345){
     cout<<"<----TOFPMT::file: "<<fname<<" successfully read !"<<endl;
   }
   else{
     cerr<<"<----Error: read problems TOFPMT-file!! "<<fname<<endl;
     exit(1);
   }
//--- 

}
void  TOFPMT::build(){
 cout<<"TOFPMT build begin"<<endl;
 init();

//Hamamatsu PMT R5946--SE pulse build
 const int pulnb=12;
 geant pulset[pulnb]={
    0.,1.,2.25,3.5,4.75,6.,8.5,11.,13.5,16.,18.5,21.
  };
  geant pulseh[pulnb]={
    0.,0.132,0.85,1.,0.83,0.68,0.35,0.15,0.078, 0.059,0.029,0.
  };
 
  int pulnbc=int((pulset[pulnb-1]- pulset[0])/(TOFCSN::FLTDCBW*TFMCFFKEY.fladctbref))+1;
  pmpulse =TOFHist(pulnb,pulset,pulseh,pulnbc);
//  cout<<"<<--TOF SE pulse init"<<endl;
 /* for(int i=0;i<pmpulse.getnb();i++){cout<<pmpulse.getxc(i+1)<<" ";}
  cout<<endl;
  cout<<"pulse y"<<endl;
  for(int i=0;i<pmpulse.getnb();i++){cout<<pmpulse.gety(i+1)<<" ";}
  cout<<endl;
 */

//--convert anode gain to refgain  
 geant sumgain=0;
 geant refgain=1.;
 for(int ilay=0;ilay<TOF2GC::SCLRS;ilay++){
    for(int ibar=0;ibar<TOF2GC::SCMXBR;ibar++){
      for(int is=0;is<2;is++){
         for(int ipm=0;ipm<TOF2GC::PMTSMX;ipm++){
             pmgain[ilay][ibar][is][ipm]/=refgain;
             //cout<<pmgain[ilay][ibar][is][ipm]<<" ";
          }
        }
      }
   }

//--PMT transmit time+transmit time spread 
  geant hv;
  for(int ilay=0;ilay<TOF2GC::SCLRS;ilay++){
    for(int ibar=0;ibar<TOF2GC::SCMXBR;ibar++){
       for(int ipm=0;ipm<TOF2GC::PMTSMX;ipm++){
         for(int is=0;is<2;is++){
           hv=pmvol[ilay][ibar][is][ipm];
           if(hv!=0){
             hv=pmvol[ilay][ibar][is][ipm];
             pmttm[ilay][ibar][is][ipm]=7.16*pow(hv/2000.,-0.567); 
             pmtts[ilay][ibar][is][ipm]=0.357*pow(hv/2000.,-0.412)+TFMCFFKEY.phtsmear;
           }
           else {
             pmttm[ilay][ibar][is][ipm]=0;
             pmtts[ilay][ibar][is][ipm]=0;   
           }
           //cout<<pmttm[ilay][ibar][is][ipm]<<" "<<pmtts[ilay][ibar][is][ipm]<<" ";
        }
      }
    }
  }

   cout<<"TOFPMT build begin"<<endl;  
}

//----------------------------------------------------------
geant TOFPMT::phriset(){
   // tau1: rise time and tau2: decay time
        geant tau1=emitt[0];
        geant tau2=emitt[1];
        while(1) {
          // two random numbers
          geant ran1 = RNDM(-1);
          geant ran2 = RNDM(-1);
          //
          // exponential distribution as envelope function: very efficient
          //
          geant d = (tau1+tau2)/tau2;
          // make sure the envelope function is 
          // always larger than the bi-exponential
          geant t = -1.0*tau2*std::log(1-ran1);
           
          geant g = d*std::exp(-1.0*t/tau2)/tau2;;
          if (ran2 <= std::exp(-1.0*t/tau2)*(1-std::exp(-1.0*t/tau1))/tau2/tau2*(tau1+tau2)/g) return t;
        }
        return -1.0;
}

//----------------------------------------------------------
geant TOFPMT::phseamp(integer nph){
  //--SE Amplitude part
 geant semean = 2.8421*TFMCFFKEY.seamref;//amp Mean value~ 5mV*ref
 geant seres=semean*TFMCFFKEY.seamres;//amp RMS value 0.9*mean
 geant selref =     TFMCFFKEY.selamref;//low amp part Mean amp 0.2
 geant selpec =     TFMCFFKEY.selampec;//low amp radio 0.2
 geant am=-1;

//---SE Amp-Sample
if(nph==1){
  do{
    am=semean+seres*rnormx();
   }while(am<=0);

   if(RNDM(-1)<selpec)am=am*selref;//low amp part
 }
 else {
     number nphl=nph*selpec;
     number nphh=nph*(1.-selpec);
//---low photon
     number meanl=nphl*(semean*selref);
     number sigmal=sqrt(nphl)*(seres*selref);
//--high photon
     number meanh=nphh*semean;
     number sigmah=sqrt(nphh)*seres;
//--adding tow part
     am=meanl+sigmal*rnormx()+meanh+sigmah*rnormx();
  }

  return am;
} 

//===========================================================
TOFTpointsN::TOFTpointsN(int nbin, geant bnl, geant bnw, geant arr[]){//constructor
  int i,it,nb,nbf,fst,nbl;
  geant cint,low,hig,tot(0.);
  geant bl,bh,cdis[TOFCSN::SCANMT];

//
//  cout<<"arr"<<endl;
  fst=0;
  for(i=0;i<nbin;i++){
    if(fst==0 && arr[i]>0.){
      fst=1;
      nbf=i;// 1st nonzero bin
    }
    if(arr[i]>0.)nbl=i;// last nonzero bin
//    cout<<arr[i]<<" ";
    tot+=arr[i];// calc. integral
  }
//  cout<<endl;
  cdis[0]=arr[0]/tot;             // prepare cumul. spectrum
  for(i=1;i<nbin;i++)cdis[i]=cdis[i-1]+arr[i]/tot;
  bini=1./geant(TOFCSN::SCANMT);
//--
  times[0]=bnl+bnw*nbf;
  times[TOFCSN::SCANMT]=bnl+bnw*(nbl+1);
  nb=nbf;
//--
  for(it=1;it<TOFCSN::SCANMT;it++){// <--- loop over integral bins
    cint=it*bini;
    while(nb<=nbl){// <--- loop over cumul.distr. bins
      bl=bnl+nb*bnw;
      bh=bl+bnw;
      if(nb==nbf)low=0.;
      else low=cdis[nb-1];
      hig=cdis[nb];
      if(cint>=low && cint<hig){
        times[it]=bl+bnw*(cint-low)/(hig-low);
        break;
      }
      else nb+=1;
    }
  }
/*  cout<<"time"<<endl;
  for(it=0;it<=TOFCSN::SCANMT;it++){
   cout<<times[it]<<" ";
  }  
   cout<<endl;
  cout<<"baijijiji"<<endl;
*/
}

//===========================================================
TOF2ScanN::TOF2ScanN(const integer nscp, const int _indsp[], const geant _scanp[],
                         const geant _eff1[], const geant _eff2[],
                         const geant _rgn1[], const geant _rgn2[],
                         const geant _psh1[][TOF2GC::PMTSMX],const geant _psh2[][TOF2GC::PMTSMX],
                         const TOFTpointsN _tdist1[], const TOFTpointsN  _tdist2[])
//                         const TOFHist _tdist1[], const TOFHist  _tdist2[])
{
   nscpnts=nscp;
    int i,j,k;
    for(i=0;i<nscp;i++)indsp[i]=_indsp[i];
    for(i=0;i<nscp;i++)scanp[i]=_scanp[i];
    for( j=0;j<nscp;j++)eff1[j]=_eff1[j];
    for( k=0;k<nscp;k++)eff2[k]=_eff2[k];
    for( j=0;j<nscp;j++)rgn1[j]=_rgn1[j];
    for( k=0;k<nscp;k++)rgn2[k]=_rgn2[k];
    for( k=0;k<nscp;k++)for(i=0;i<TOF2GC::PMTSMX;i++)psh1[k][i]=_psh1[k][i];
    for( k=0;k<nscp;k++)for(i=0;i<TOF2GC::PMTSMX;i++)psh2[k][i]=_psh2[k][i];
    for(int l=0;l<nscp;l++)tdist1[l]=_tdist1[l];
    for(int m=0;m<nscp;m++)tdist2[m]=_tdist2[m];
};
//===========================================================
void TOF2ScanN::getybin(const geant y, int &i1, int &i2, geant &r){
 integer i=AMSbins(scanp,y,nscpnts);
  if(i==0){
    i1=0;
    i2=1;
    r=(y-scanp[i1])/(scanp[i2]-scanp[i1]);//r<0 -> y below 1st sc.point
  }
  else if(i<0){
    if(i==-nscpnts){// r>1 -> y above last sc.point
      i1=nscpnts-2;
      i2=nscpnts-1;
      r=(y-scanp[i1])/(scanp[i2]-scanp[i1]);
      }
    else{// <- normal case when y is inside of sc.points array
      i1=-i-1;
      i2=-i;
      r=(y-scanp[i1])/(scanp[i2]-scanp[i1]);
    }
  }
  else{ // exact matching inside sc.points array(i1=i2, r=0.)
    i1=i-1;
    i2=i1;
    r=0.;
  }
}

//------------------------------------------------------------------
geant TOF2ScanN::gettm1(const geant r, const int i1, const int i2){
  geant rnd,t1,t2,dummy(-1);
  rnd=RNDM(dummy);
//  t1=tdist1[i1].getx(rnd);
//  t2=tdist1[i2].getx(rnd);
  t1=tdist1[i1].gettime(rnd);
  t2=tdist1[i2].gettime(rnd);
  return t1+(t2-t1)*r>0?t1+(t2-t1)*r:0;
};
geant TOF2ScanN::gettm2(const geant r, const int i1, const int i2){
  geant rnd,t1,t2,dummy(-1);
  rnd=RNDM(dummy);
//  t1=tdist2[i1].getx(rnd);
//  t2=tdist2[i2].getx(rnd);
  t1=tdist2[i1].gettime(rnd);
  t2=tdist2[i2].gettime(rnd);
  return t1+(t2-t1)*r>0?t1+(t2-t1)*r:0;
};
//------------------------------------------------------------------
TOFWScanN::TOFWScanN(const int npm, const int nwdvs,const geant _dxl[], const geant _dxh[],const TOF2ScanN  _sdsc[]){
   npmts=npm;nwdivs=nwdvs;
   int i;
   for(i=0;i<nwdvs;i++){
     divxl[i]=_dxl[i];
     divxh[i]=_dxh[i];
     wscan[i]=_sdsc[i];
   }
};
int TOFWScanN::getwbin(const geant x){
    int div(0);
    if((divxl[0]-x)>0.01)return(-1);
    if((x-divxh[nwdivs-1])>0.01)return(-1);
    if(x<divxl[0])return(div);
    if(x>=divxl[nwdivs-1])return(nwdivs-1);
    for(int i=0;i<nwdivs-1;i++){
      if(x>=divxl[i] && x<divxl[i+1])div=i;
    }
    return(div);
}
//----------------------------------------------------------------
TOFWScanN TOFWScanN::scmcscan1[TOF2GC::SCLRS][TOF2GC::SCMXBR];
//----------------------------------------------------------------
void TOFWScanN::build(){

   char fname[1000],name[2000];
   char *ldir="/afs/ams.cern.ch/user/qyan/Offline/AMSTOF/tofdata/";
   if(TFMCFFKEY.g4tfdir==1)strcpy(fname,ldir);
   else   {
      strcpy(fname,AMSDATADIR.amsdatadir);
      strcat(fname,"tofdata/");
    }
   int tsfdbv=TFMCFFKEY.simfvern/100%10;

//--------                       <-- now read t-distr. files
 int i,j,ila,ibr,brt,ibrm,isp,nsp,ibt,cnum,dnum,dnumm,mult,dnumo;
 integer nb;
 int iscp[TOFCSN::SCANMY];
 geant scp[TOFCSN::SCANMY];
 geant nft,bl,bw;
 geant arr[TOFCSN::SCANMT];
 geant ef1[TOFCSN::SCANMY],ef2[TOFCSN::SCANMY];
 geant rg1[TOFCSN::SCANMY],rg2[TOFCSN::SCANMY];
 geant ps1[TOFCSN::SCANMY][TOF2GC::PMTSMX],ps2[TOFCSN::SCANMY][TOF2GC::PMTSMX];
// TOFHist td1[TOFCSN::SCANMY];
// TOFHist td2[TOFCSN::SCANMY];
 TOFTpointsN td1[TOFCSN::SCANMY];
 TOFTpointsN td2[TOFCSN::SCANMY];
 int nwdivs(0);
 int npmts(0);
 TOF2ScanN swdscan[TOFCSN::SCANMX];
 geant wdivxl[TOFCSN::SCANMX];
 geant wdivxh[TOFCSN::SCANMX];
 integer idiv;
 geant eff1,eff2;
 cout<<"      Reading New MC-TimeScan files ...."<<endl;
//--
 for(int ilay=0;ilay<TOFCSN::SCLRS;ilay++){
     for(int ibar=0;ibar<TOFCSN::NBAR[ilay];ibar++){
       sprintf(name,"%stofg4l%db%d_%d.tsf",fname,ilay,ibar,tsfdbv);
       cout<<"      Open file : "<<name<<'\n';
       ifstream tcfile(name,ios::in); // open needed t-calib. file for reading
       if(!tcfile){
         cout <<"<---- missing MC-TimeScan file !!! "<<name<<endl;
         exit(1);
       }
      tcfile >> npmts; // read numb.of PMTs per side
      tcfile >> nwdivs; // read tot.number of X(width) divisions
      for(int div=0;div<nwdivs;div++){// <--- divisions loop
//         cout<<"idiv="<<div<<endl;
        tcfile >> idiv; // read numb.of current div.
        tcfile >> nsp; // read tot.number of scan-point in this div.
        if(nsp>TOFCSN::SCANMY){
          cout<<"<---- wrong number of MC-TimeScan Y-points ! "<<nsp<<'\n';
          exit(1);
        }
        tcfile >> wdivxl[div]; // read xl
        tcfile >> wdivxh[div]; // read xh
       for(isp=0;isp<nsp;isp++){ // <--- sp. points loop to prepare arr. of t-distr
//          cout<<"isp="<<isp<<endl;
          tcfile >> iscp[isp]; //read No.yl
          tcfile >> scp[isp]; //y coo
          tcfile >> nft;   // for PM-1
          tcfile >> nb;    //time bin
          tcfile >> bl;    //time low bin
          tcfile >> bw;    //time width
          tcfile >> ef1[isp];//side1 photon eff
          tcfile >> rg1[isp];//side1 ref.gain
//          cout<<"rg1="<<rg1[isp]<<endl;
          for(i=0;i<TOF2GC::PMTSMX;i++)tcfile >> ps1[isp][i];//pmt share factor
          for(i=0;i<nb;i++){arr[i]=0.;}
          for(i=0;i<nb;i++){tcfile >> arr[i];}//cout<<"arr="<<arr[i]<<endl;}//arriving time prob for each bin
//          td1[isp]=TOFHist(nb,bl,(bl+bw*nb),arr);
//          cout<<"ilay ibar idivx idivy side0="<<ilay<<" "<<ibar<<" "<<div<<" "<<isp<<endl;
          td1[isp]=TOFTpointsN(nb,bl,bw,arr);
          tcfile >> nft;   // for PM-2
          tcfile >> nb;
          tcfile >> bl;
          tcfile >> bw;
          tcfile >> ef2[isp];
          tcfile >> rg2[isp];
//          cout<<"rg2="<<rg2[isp]<<endl;
          for(i=0;i<TOF2GC::PMTSMX;i++)tcfile >> ps2[isp][i];
          for(i=0;i<nb;i++){arr[i]=0.;}
          for(i=0;i<nb;i++){tcfile >> arr[i];}
//          td2[isp]=TOFHist(nb,bl,(bl+bw*nb),arr);
//          cout<<"ilay ibar idivx idivy side1="<<ilay<<" "<<ibar<<" "<<div<<" "<<isp<<endl;
          td2[isp]=TOFTpointsN(nb,bl,bw,arr);
        } // <--- end of scan points loop -----
        swdscan[div]=TOF2ScanN(nsp,iscp,scp,ef1,ef2,rg1,rg2,ps1,ps2,td1,td2);// 
    } // <--- end of divisions loop -----
      tcfile.close(); // close file
//      cout<<"ilay="<<ilay<<" ibar="<<ibar<<endl;
      scmcscan1[ilay][ibar]=TOFWScanN(npmts,nwdivs,wdivxl,wdivxh,swdscan);// create complete scan obj
    } // --- end of bar loop --->
  }//---end of lay
  cout<<"<---- TOFWScanN::build: succsessfully done !"<<endl<<endl;
}
#endif

//===========================================================
template  <typename T1>
int TofTDVTool<T1>::ReadTDV(int rtime,int real){
   if(!TDVBlock || !TDVName){
      cerr<<"--->>TDV Read Problem!!!"<<endl;
      return -1;
   }
    time_t time=rtime;
    time_t brun=time+1;
    time_t erun=time;
    tm begin;
    tm end;
    gmtime_r(&brun, &begin);
    gmtime_r(&erun, &end);
    
//--------Already load
    AMSTimeID* tdv=new AMSTimeID(AMSID(TDVName,real),begin,end,
                              TDVSize,
                              TDVBlock,
                              AMSTimeID::Standalone,1);
    
    int status=tdv->validate(time);
    LoadTDVPar();
    delete tdv;
    return status;
}

//===========================================================
template  <typename T1>
int TofTDVTool<T1>::WriteTDV(int brun,int erun,int real){
   if(!TDVBlock|| ! TDVName) {
     cerr<<"---->>TDV Write Problem!!!"<<endl;
     return -1;
   }

     time_t br=brun;
     time_t er=erun;
     tm begin;
     tm end;
     gmtime_r(&br, &begin);
     gmtime_r(&er, &end);
     cout << "Begin: " <<(int)brun<<"  " <<asctime(&begin)<<endl;
     cout << "End  : " <<(int)erun<<"  " <<asctime(&end  )<<endl;
//--1 realdata  1--vertify  
     AMSTimeID *tdv=new AMSTimeID(AMSID(TDVName, real),begin,end,
                                  TDVSize,
                                  TDVBlock,
                                  AMSTimeID::Standalone,1
                                 );
     tdv->UpdateMe();
     int status=tdv->write(AMSDATADIR.amsdatabase);
     delete tdv;
     return status;
}

// **************************************************************
// Tof Time+TCoo Calbration 15days per Calibration
// **************************************************************
TofTAlignPar* TofTAlignPar::Head=0;

TofTAlignPar *TofTAlignPar::GetHead(){
  if(!Head)Head = new TofTAlignPar();
  return Head;
}

//===========================================================
TofTAlignPar::TofTAlignPar(){
  TDVName="TofAlign2";
  TDVParN=(TOFCSN::NBARN*2+TOFCSN::NBARN+1+TOFCSN::NBARN*3);
  TDVBlock=new float[TDVParN];
  TDVSize=TDVParN*sizeof(float);
}

//===========================================================
TofTAlignPar::TofTAlignPar(float *arr,int brun,int erun){
  TDVName="TofAlign2";
  TDVParN=(TOFCSN::NBARN*2+TOFCSN::NBARN+1+TOFCSN::NBARN*3);
  TDVBlock=arr;
  TDVSize=TDVParN*sizeof(float);
  BRun=brun;
  ERun=erun;
  LoadTDVPar();
}

//===========================================================
void   TofTAlignPar::LoadTDVPar(){
   int iblock=0;
//----load par
   for(int ilay=0;ilay<TOFCSN::SCLRS;ilay++){
      for(int ibar=0;ibar<TOFCSN::NBAR[ilay];ibar++){//SN
       int id=ilay*1000+ibar*100;
        slew[id]=TDVBlock[iblock++];
      }
      for(int ibar=0;ibar<TOFCSN::NBAR[ilay];ibar++){//SP
       int id=ilay*1000+ibar*100+10;
        slew[id]=TDVBlock[iblock++];
     }
   }
   for(int ilay=0;ilay<TOFCSN::SCLRS;ilay++){
      for(int ibar=0;ibar<TOFCSN::NBAR[ilay];ibar++){//Const
       int id=ilay*1000+ibar*100;
        delay[id]=TDVBlock[iblock++];
      }
   }
   powindex=TDVBlock[iblock++];
//---Coo Par
   for(int ipar=0;ipar<3;ipar++){
     for(int ilay=0;ilay<TOFCSN::SCLRS;ilay++){
       for(int ibar=0;ibar<TOFCSN::NBAR[ilay];ibar++){//Const
         int id=ilay*1000+ibar*100;
         if     (ipar==0)vel[id]=TDVBlock[iblock++];
         else if(ipar==1)dt1[id]=TDVBlock[iblock++];
         else if(ipar==2)dslew[id]=TDVBlock[iblock++];
       }
     }
   }
   
   Isload=1;
}
//==========================================================
int TofTAlignPar::LoadFromFile(char *file){
   ifstream vlfile(file,ios::in);
   if(!vlfile){
    cerr <<"<---- Error: missing "<<file<<"--file !!: "<<endl;
    return -1;
   }
//---load
   vlfile>>BRun>>ERun;
   int ib=0;
   for(int i=0;i<TDVParN+1;i++){
     if(i==TOFCSN::NBARN*2+TOFCSN::NBARN+2)ib--;
     vlfile>>TDVBlock[ib++];
   } 
   LoadTDVPar();
   vlfile.close();
   return 0;
}

//==========================================================
void TofTAlignPar::PrintTDV(){
 cout<<"<<----Print TofTAlignPar"<<endl;
 for(int i=0;i<TDVParN;i++){cout<<TDVBlock[i]<<" ";}
 cout<<'\n';
 cout<<"<<----end of Print TofTAlignPar"<<endl;
}

// **************************************************************
// Tof TDC Non_linear Calibration
// **************************************************************
TofTdcCorN TofTdcCorN::tdccor[TOF2GC::SCCRAT][TOF2GC::SCFETA-1];

// **************************************************************
TofTdcPar* TofTdcPar::Head=0;

TofTdcPar *TofTdcPar::GetHead(){
  if(!Head)Head = new TofTdcPar();
  return Head;
}

//===========================================================
TofTdcPar::TofTdcPar(){
  TDVName="TofTdcPar";
  TDVParN=(TOFCSN::NCRATE*TOFCSN::NSLOT*TOFCSN::NCHANNEL*1024);
  TDVBlock=new float[TDVParN];
  TDVSize=TDVParN*sizeof(float);
  ntime=0;
}

//===========================================================
TofTdcPar::TofTdcPar(float *arr,int brun,int erun){
  TDVName="TofTdcPar";
  TDVParN=(TOFCSN::NCRATE*TOFCSN::NSLOT*TOFCSN::NCHANNEL*1024);
  TDVBlock=arr;
  TDVSize=TDVParN*sizeof(float);
  BRun=brun;
  ERun=erun;
  LoadTDVPar();
  ntime=0;
}

//===========================================================
void TofTdcPar::LoadTDVPar(){
//----load par
   int iblock=0;
   for(int icrate=0;icrate<TOFCSN::NCRATE;icrate++){
     for(int islot=0;islot<TOFCSN::NSLOT;islot++){
       for(int ich=0;ich<TOFCSN::NCHANNEL;ich++){
          for(int i=0;i<1024;i++)ncor[icrate][islot][ich][i]=TDVBlock[iblock++];
      }
    } 
  }
//---
  Isload=1;
}

//==========================================================
int  TofTdcPar::LoadFromFile(char *file){
   ifstream vlfile(file,ios::in);
   if(!vlfile){
    cerr <<"<---- Error: missing "<<file<<"--file !!: "<<endl;
    return -1;
   }
//---load
   vlfile>>BRun>>ERun;
   int ib=0;
   for(int i=0;i<TDVParN;i++){
     vlfile>>TDVBlock[ib++];
   }
   LoadTDVPar();
   vlfile.close();
   return 0;
}

//==========================================================
void TofTdcPar::PrintTDV(){
 cout<<"<<----Print "<<TDVName<<endl;
 for(int i=0;i<TDVParN;i++){cout<<TDVBlock[i]<<" ";}
 cout<<'\n';
 cout<<"<<----end of Print "<<TDVName<<endl;
}

//==========================================================
float TofTdcPar::getcor(int icrate,int islot,int ichan, int tdc){
   int t10=(tdc&(0x3FFL));//10 lsb of TDC-count(time measurement)
   if(t10<=0){return(0);}
   return ncor[icrate][islot][ichan][t10-1];
}

//==========================================================
int  TofTdcPar::btoc(int ilay,int ibar,int is,int opt,int &icrate,int &islot,int &ichan){
   int csid=TOFCSN::CSId[ilay][is][ibar];
   icrate=csid/10-1;
   islot=csid%10-1;
   ichan=(opt==0)?ibar/2:opt;
   return 0;
}

//==========================================================
float TofTdcPar::getcorb(int ilay,int ibar,int is,int opt,int tdc){
  int icrate,islot,ichan;
  btoc(ilay,ibar,is,opt,icrate,islot,ichan);
  return getcor(icrate,islot,ichan,tdc);
}

//==========================================================
bool TofTdcPar::IsValidate(){
  if(ntime>=1385487767)return true; 
  else                 return false;
}



// **************************************************************
// Tof Scintillator Attenutation Calbration 15days per Calibration
// **************************************************************
TofAttAlignPar* TofAttAlignPar::Head=0;

TofAttAlignPar *TofAttAlignPar::GetHead(){
  if(!Head)Head = new TofAttAlignPar();
  return Head;
}

//===========================================================
TofAttAlignPar::TofAttAlignPar(){
  TDVName="TofAttAlign";
  TDVParN=(TOFCSN::NBARN*TOFCSN::NSIDE*nattpar);
  TDVBlock=new float[TDVParN];
  TDVSize=TDVParN*sizeof(float);
}

//===========================================================
TofAttAlignPar::TofAttAlignPar(float *arr,int brun,int erun){
  TDVName="TofAttAlign";
  TDVParN=(TOFCSN::NBARN*TOFCSN::NSIDE*nattpar);
  TDVBlock=arr;
  TDVSize=TDVParN*sizeof(float);
  BRun=brun;
  ERun=erun;
  LoadTDVPar();
}

//===========================================================
void   TofAttAlignPar::LoadTDVPar(){
   int iblock=0;
//----load par
  for(int ipar=0;ipar<nattpar;ipar++){
//----npar
    for(int ilay=0;ilay<TOFCSN::SCLRS;ilay++){
      for(int isid=0;isid<TOFCSN::NSIDE;isid++){
        for(int ibar=0;ibar<TOFCSN::NBAR[ilay];ibar++){//N+P
            int id=ilay*1000+ibar*100+isid*10;
            attpar[ipar][id]=TDVBlock[iblock++];;
        }
      }
    }
//---
  }
  Isload=1;
}
//==========================================================
int  TofAttAlignPar::LoadFromFile(char *file){
   ifstream vlfile(file,ios::in);
   if(!vlfile){
    cerr <<"<---- Error: missing "<<file<<"--file !!: "<<endl;
    return -1;
   }
//---load
   vlfile>>BRun>>ERun;
   int ib=0;
   for(int i=0;i<TDVParN;i++){
     vlfile>>TDVBlock[ib++];
   }
   LoadTDVPar();
   vlfile.close();
   return 0;
}

//==========================================================
void TofAttAlignPar::PrintTDV(){
 cout<<"<<----Print "<<TDVName<<endl;
 for(int i=0;i<TDVParN;i++){cout<<TDVBlock[i]<<" ";}
 cout<<'\n';
 cout<<"<<----end of Print "<<TDVName<<endl;
}

// **************************************************************
// Tof PMT Calibration 
//     1: Anode Gain 3days per calibration
// **************************************************************
TofPMAlignPar* TofPMAlignPar::Head=0;

TofPMAlignPar* TofPMAlignPar::GetHead(){
  if(!Head)Head = new TofPMAlignPar();
  return Head;
}

//===========================================================
TofPMAlignPar::TofPMAlignPar(){
  TDVName="TofPMAlign";
  TDVParN=(TOFCSN::NBARN*TOFCSN::NSIDE*nalign);
  TDVBlock=new float[TDVParN];
  TDVSize=TDVParN*sizeof(float);
}

//===========================================================
TofPMAlignPar::TofPMAlignPar(float *arr,int brun,int erun){
  TDVName="TofPMAlign";
  TDVParN=(TOFCSN::NBARN*TOFCSN::NSIDE*nalign);
  TDVBlock=arr;
  TDVSize=TDVParN*sizeof(float);
  BRun=brun;
  ERun=erun;
  LoadTDVPar();
}
//===========================================================
void  TofPMAlignPar::LoadTDVPar(){
   int iblock=0;
//----load par
   for(int ilay=0;ilay<TOFCSN::SCLRS;ilay++){
      for(int isid=0;isid<TOFCSN::NSIDE;isid++){
        for(int ibar=0;ibar<TOFCSN::NBAR[ilay];ibar++){//N+P
            int id=ilay*1000+ibar*100+isid*10;
            gaina[id]=TDVBlock[iblock++];;
        }
      }
  }
  Isload=1;
}
//==========================================================
int  TofPMAlignPar::LoadFromFile(char *file){
   ifstream vlfile(file,ios::in);
   if(!vlfile){
    cerr <<"<---- Error: missing "<<file<<"--file !!: "<<endl;
    return -1;
   }
//---load
   vlfile>>BRun>>ERun;
   int ib=0;
   for(int i=0;i<TDVParN;i++){
     vlfile>>TDVBlock[ib++];
   }
   LoadTDVPar();
   vlfile.close();
   return 0;
}

//==========================================================
void TofPMAlignPar::PrintTDV(){
 cout<<"<<----Print "<<TDVName<<endl;
 for(int i=0;i<TDVParN;i++){cout<<TDVBlock[i]<<" ";}
 cout<<'\n';
 cout<<"<<----end of Print "<<TDVName<<endl;
}

// **************************************************************
// Tof PMT Dynode Calibration //Using Carbon 
//     1: Dynode Gain 7.5days per calibration 
//        gaind=0 Mean Mask Bad Channel
// **************************************************************
TofPMDAlignPar* TofPMDAlignPar::Head=0;
const int TofPMDAlignPar::dpmid[ndead]={2011};

TofPMDAlignPar* TofPMDAlignPar::GetHead(){
  if(!Head)Head = new TofPMDAlignPar();
  return Head;
}

//===========================================================
TofPMDAlignPar::TofPMDAlignPar(){
  TDVName="TofPMDAlign";
  TDVParN=(TOFCSN::NBARN*TOFCSN::NSIDE*TOFCSN::NPMTM)*2;//mean+sigma
  TDVBlock=new float[TDVParN];
  TDVSize=TDVParN*sizeof(float);
}

//===========================================================
TofPMDAlignPar::TofPMDAlignPar(float *arr,int brun,int erun){
  TDVName="TofPMDAlign";
  TDVParN=(TOFCSN::NBARN*TOFCSN::NSIDE*TOFCSN::NPMTM)*2;
  TDVBlock=arr;
  TDVSize=TDVParN*sizeof(float);
  BRun=brun;
  ERun=erun;
  LoadTDVPar();
}
//===========================================================
void  TofPMDAlignPar::LoadTDVPar(){
   int iblock=0;
//----load carbon mip mean
   for(int ilay=0;ilay<TOFCSN::SCLRS;ilay++){
      for(int isid=0;isid<TOFCSN::NSIDE;isid++){
        for(int ibar=0;ibar<TOFCSN::NBAR[ilay];ibar++){//N+P
          for(int ipm=0;ipm<TOFCSN::NPMTM;ipm++){
            int id=ilay*1000+ibar*100+isid*10+ipm;
//----mask dead PMT gaind to 0
            for(int idead=0;idead<ndead;idead++){
              if(id==dpmid[idead])TDVBlock[iblock]=0;
            }
//------
            gaind[id]=TDVBlock[iblock++];
//------
          }
        }
     }
  }
//---load carbon mip sigma
  for(int ilay=0;ilay<TOFCSN::SCLRS;ilay++){
      for(int isid=0;isid<TOFCSN::NSIDE;isid++){
        for(int ibar=0;ibar<TOFCSN::NBAR[ilay];ibar++){//N+P
          for(int ipm=0;ipm<TOFCSN::NPMTM;ipm++){
            int id=ilay*1000+ibar*100+isid*10+ipm;
            gainds[id]=TDVBlock[iblock++];
          }
        }
     }
  }
//---

  Isload=1;
}
//==========================================================
int  TofPMDAlignPar::FindPMBlock(int fpmid){

   int fblock=-1;
   int iblock=0;
//----find block pos
   for(int ilay=0;ilay<TOFCSN::SCLRS;ilay++){
      for(int isid=0;isid<TOFCSN::NSIDE;isid++){
        for(int ibar=0;ibar<TOFCSN::NBAR[ilay];ibar++){//N+P
          for(int ipm=0;ipm<TOFCSN::NPMTM;ipm++){
            int id=ilay*1000+ibar*100+isid*10+ipm;
            if(id==fpmid){fblock=iblock;break;}
            else          iblock++;
          }
        }
     }
  }
//----
  return fblock;
}

//==========================================================
int  TofPMDAlignPar::LoadFromFile(char *file){
   ifstream vlfile(file,ios::in);
   if(!vlfile){
    cerr <<"<---- Error: missing "<<file<<"--file !!: "<<endl;
    return -1;
   }
//---load
   vlfile>>BRun>>ERun;
   int ib=0;
   for(int i=0;i<TDVParN;i++){
     vlfile>>TDVBlock[ib++];
     if(TDVBlock[i]<0)cerr<<"<<-----Error Load Minus Par"<<endl;
   }
   LoadTDVPar();
   vlfile.close();
   return 0;
}
//==========================================================
void TofPMDAlignPar::PrintTDV(){
 cout<<"<<----Print "<<TDVName<<endl;
 for(int i=0;i<TDVParN;i++){cout<<TDVBlock[i]<<" ";}
 cout<<'\n';
 cout<<"<<----end of Print "<<TDVName<<endl;
}


// **************************************************************
// Tof Charge+EDep Reconstruction Par
// **************************************************************
TofCAlignPar* TofCAlignPar::Head=0;

TofCAlignPar* TofCAlignPar::GetHead(){
  if(!Head)Head = new TofCAlignPar();
  return Head;
}

const int TofCAlignPar::BetaCh[TofCAlignPar::nBetaCh]={
   1,2,3,4,5,6,7,8,10,12,14,16,20,26,
};

//===========================================================
const float TofCAlignPar::def_birk[2][TOFCSN::SCLRS][TOFCSN::SCMXBR]={
0.0031363, 0.0045102, 0.0041224, 0.0044827, 0.0043729, 0.0037915, 0.0044720, 0.0031602, 0., 0.,
0.0034984, 0.0045616, 0.0044301, 0.0043610, 0.0041285, 0.0041880, 0.0045415, 0.0039832, 0., 0.,
0.0042435, 0.0048270, 0.0044886, 0.0046643, 0.0046901, 0.0046957, 0.0045282, 0.0044864, 0.0043669, 0.0036504,
0.0031673, 0.0045797, 0.0047612, 0.0042844, 0.0048720, 0.0047736, 0.0044743, 0.0032529, 0., 0.,

0.9635189, 1.1887178, 1.0276626, 1.1166632, 1.0774540, 1.0364363, 1.1200749, 0.9920545, 0., 0.,
0.9546345, 1.0739068, 1.0247164, 0.9840805, 1.0318513, 1.0141042, 1.0611019, 1.0044100, 0., 0.,
1.1168552, 1.2151288, 1.1411203, 1.1768930, 1.1882606, 1.1138064, 1.1451443, 1.1490825, 1.1690836, 1.0700063,
1.0416341, 1.1053152, 1.0950616, 1.0774803, 1.0909635, 1.1447011, 1.1754051, 0.9630379, 0., 0.,
};
const float TofCAlignPar::def_dycor[TOFCSN::SCLRS][TOFCSN::NSIDE][TOFCSN::SCMXBR][TOFCSN::NPMTM]={
1.0968488, 1.0845329, 1.0745927,  1.1349114, 1.1436553, 0.0000000,  1.1202177, 1.1217999, 0.0000000,  1.1332975, 1.1355378, 0.0000000,  1.1426946, 1.1267978, 0.0000000,  1.1056151, 1.1208747, 0.0000000,  1.1341042, 1.1401625, 0.0000000,  1.1064207, 1.1055920, 1.0781818, 0.,0.,0., 0.,0.,0.,
1.0775647, 1.0845235, 1.1015781,  1.1335049, 1.1353019, 0.0000000,  1.1167765, 1.1169721, 0.0000000,  1.1424113, 1.1291678, 0.0000000,  1.1321910, 1.1245311, 0.0000000,  1.1130728, 1.1035968, 0.0000000,  1.1370057, 1.1394732, 0.0000000,  1.0727683, 1.1011557, 1.0769244, 0.,0.,0., 0.,0.,0.,
1.0931263, 1.0946197, 0.0000000,  1.1355666, 1.1532258, 0.0000000,  1.1247759, 1.1269354, 0.0000000,  1.1237526, 1.1289133, 0.0000000,  1.1167561, 1.1192142, 0.0000000,  1.1164631, 1.1390759, 0.0000000,  1.1347499, 1.1382586, 0.0000000,  1.1079241, 1.1137814, 0.0000000, 0.,0.,0., 0.,0.,0.,
1.0959291, 1.1000559, 0.0000000,  1.1391538, 1.1448357, 0.0000000,  1.1291345, 1.1301722, 0.0000000,  1.1398854, 1.1279965, 0.0000000,  1.1228725, 1.1216896, 0.0000000,  1.1122968, 1.1034834, 0.0000000,  1.1352257, 1.1345220, 0.0000000,  1.1126656, 1.1149350, 0.0000000, 0.,0.,0., 0.,0.,0.,
1.1158370, 1.1105816, 0.0000000,  1.1574359, 1.1603521, 0.0000000,  1.1394677, 1.1390362, 0.0000000,  1.1578065, 1.1417080, 0.0000000,  1.1481139, 1.1491014, 0.0000000,  1.1486593, 1.1442004, 0.0000000,  1.1390684, 1.1417961, 0.0000000,  1.1389841, 1.1351996, 0.0000000,  1.1280150, 1.1192330, 0.0000000,  1.1112082, 1.0979660, 0.0000000,
1.1187236, 0.0000000, 0.0000000,  1.1509455, 1.1397133, 0.0000000,  1.1370052, 1.1281301, 0.0000000,  1.1452599, 1.1418809, 0.0000000,  1.1547490, 1.1447720, 0.0000000,  1.1471812, 1.1469846, 0.0000000,  1.1450506, 1.1295079, 0.0000000,  1.1369098, 1.1426551, 0.0000000,  1.1352474, 1.1548489, 0.0000000,  1.1131174, 1.0789461, 0.0000000,
1.0711741, 1.0751316, 1.0696835,  1.1334278, 1.1415893, 0.0000000,  1.1489427, 1.1513936, 0.0000000,  1.1222135, 1.1211376, 0.0000000,  1.1502289, 1.1618166, 0.0000000,  1.1443826, 1.1636261, 0.0000000,  1.1306187, 1.1292037, 0.0000000,  1.0929302, 1.0778476, 1.1059071, 0.,0.,0., 0.,0.,0.,
1.0662692, 1.0720497, 1.0901281,  1.1329209, 1.1303928, 0.0000000,  1.1399657, 1.1448210, 0.0000000,  1.1147345, 1.1306032, 0.0000000,  1.1496262, 1.1392366, 0.0000000,  1.1428988, 1.1428513, 0.0000000,  1.1269662, 1.1263711, 0.0000000,  1.0671484, 1.0896088, 1.0883199, 0.,0.,0., 0.,0.,0.,
};
const float TofCAlignPar::def_dypmw[TOFCSN::SCLRS][TOFCSN::NSIDE][TOFCSN::SCMXBR][TOFCSN::NPMTM]={
0.1085019, 0.0886208, 0.0993699,  0.0726674, 0.0772448, 0.0000000,  0.0774278, 0.0822079, 0.0000000,  0.0719588, 0.0717576, 0.0000000,  0.0784183, 0.0705233, 0.0000000,  0.0809693, 0.0992476, 0.0000000,  0.0693978, 0.0794090, 0.0000000,  0.1086738, 0.0995611, 0.0973869, 0.,0.,0., 0.,0.,0.,
0.0934224, 0.0857703, 0.1075311,  0.0661984, 0.0710111, 0.0000000,  0.0734443, 0.0734147, 0.0000000,  0.0738730, 0.0716660, 0.0000000,  0.0748463, 0.0697479, 0.0000000,  0.0811270, 0.0756794, 0.0000000,  0.0740314, 0.0767472, 0.0000000,  0.0903411, 0.0898284, 0.0883199, 0.,0.,0., 0.,0.,0.,
0.1369371, 0.0964603, 0.0000000,  0.0668537, 0.0681504, 0.0000000,  0.0746466, 0.0768797, 0.0000000,  0.0736538, 0.0740538, 0.0000000,  0.0731169, 0.0750668, 0.0000000,  0.0836490, 0.1058806, 0.0000000,  0.0745215, 0.0745701, 0.0000000,  0.0860110, 0.1039841, 0.0000000, 0.,0.,0., 0.,0.,0.,
0.1225823, 0.0905565, 0.0000000,  0.0713265, 0.0727963, 0.0000000,  0.0749480, 0.0762487, 0.0000000,  0.0911247, 0.0855741, 0.0000000,  0.0800629, 0.0819167, 0.0000000,  0.0811428, 0.0752835, 0.0000000,  0.0739273, 0.0723969, 0.0000000,  0.0882718, 0.1118316, 0.0000000, 0.,0.,0., 0.,0.,0.,
0.0806216, 0.0859936, 0.0000000,  0.0657463, 0.0712145, 0.0000000,  0.0680264, 0.0688388, 0.0000000,  0.0725222, 0.0670632, 0.0000000,  0.0618991, 0.0697105, 0.0000000,  0.0672435, 0.0681230, 0.0000000,  0.0695323, 0.0684988, 0.0000000,  0.0685786, 0.0641038, 0.0000000,  0.0664131, 0.0656628, 0.0000000,  0.1038786, 0.1006057, 0.0000000,
0.0840200, 0.0000000, 0.0000000,  0.0638568, 0.0636975, 0.0000000,  0.0674898, 0.0678903, 0.0000000,  0.0651318, 0.0688437, 0.0000000,  0.0653565, 0.0661774, 0.0000000,  0.0679625, 0.0653888, 0.0000000,  0.0929473, 0.0695421, 0.0000000,  0.0657068, 0.0721340, 0.0000000,  0.0694512, 0.0841961, 0.0000000,  0.1045947, 0.0794113, 0.0000000,
0.0978309, 0.0947135, 0.1090633,  0.0766076, 0.0788681, 0.0000000,  0.0771597, 0.0757777, 0.0000000,  0.0793132, 0.0797093, 0.0000000,  0.0767610, 0.0880478, 0.0000000,  0.0821739, 0.0752982, 0.0000000,  0.0801715, 0.0777096, 0.0000000,  0.1256462, 0.1220001, 0.1214934, 0.,0.,0., 0.,0.,0.,
0.0950433, 0.0971318, 0.1088881,  0.0772794, 0.0834216, 0.0000000,  0.0744868, 0.0741841, 0.0000000,  0.0800880, 0.0888987, 0.0000000,  0.0751293, 0.0796956, 0.0000000,  0.0811491, 0.0736308, 0.0000000,  0.0770489, 0.0839550, 0.0000000,  0.1213091, 0.1197510, 0.115482, 0.,0.,0., 0.,0.,0.,
};
const float  TofCAlignPar::def_ansat[nansat][TOFCSN::SCLRS][TOFCSN::NSIDE][TOFCSN::SCMXBR]={
102.5347010,103.8446545, 103.9405850,52.1289887, 100.6385838, 99.7064122 ,88.6561970, 114.7721441,0, 0,
105.3823058, 65.7273985, 60.1013107 ,83.2094808, 77.5001545 , 81.3363651 ,68.7445468, 98.7939020 ,0, 0,
102.9276627, 54.4850136, 52.6192353 ,78.5761282, 78.7784021 , 109.8302363,83.5184074, 61.5993685 ,0, 0,
99.3647272,  79.4371529, 110.5024852,99.3882503, 64.2806738 , 57.7865561 ,45.8046258, 86.6891254 ,0, 0,
67.5642773,  65.8865273, 63.4373387 ,73.9318346, 79.9193985 , 53.7898053 ,94.8761005, 86.8406409 ,55.2729983, 95.5665299,
72.3588240, 47.7259430 , 72.6416105 ,87.1143043, 54.5976540 , 85.1760659 ,96.7603867, 85.6055136 ,98.7126162, 80.6580185,
67.4806989, 91.2154263 , 70.3339852 ,60.2290798, 75.0972569 , 63.6631950 ,44.7335529, 72.8059044 ,0, 0,
84.4449051, 64.7046834 , 72.8641890 ,89.0470369, 45.6489480 , 77.6402214 ,88.3986155, 88.0676147 ,0, 0,

0.0062828, 0.0077937, 0.0076552, 0.0131398, 0.0082479, 0.0078092, 0.0088990, 0.0065267, 0, 0,
0.0068177, 0.0112673, 0.0111492, 0.0092878, 0.0099030, 0.0093566, 0.0112709, 0.0072998, 0, 0,
0.0073920, 0.0124189, 0.0131293, 0.0097495, 0.0099936, 0.0066092, 0.0091482, 0.0118567, 0, 0,
0.0074832, 0.0095696, 0.0071993, 0.0078329, 0.0105896, 0.0125690, 0.0154907, 0.0092845, 0, 0,
0.0105059, 0.0104198, 0.0115260, 0.0098816, 0.0098280, 0.0129604, 0.0084227, 0.0089455, 0.0134642, 0.0081498,
0.0095519, 0.0152684, 0.0098733, 0.0085722, 0.0128197, 0.0093759, 0.0087464, 0.0094747, 0.0083433, 0.0097331,
0.0097625, 0.0083429, 0.0103601, 0.0112625, 0.0099013, 0.0113624, 0.0162545, 0.0088187, 0, 0,
0.0081522, 0.0111571, 0.0106481, 0.0083607, 0.0143510, 0.0102375, 0.0087728, 0.0074106, 0, 0,

108.1174629,106.8177325,107.1847782,54.8821298,103.2796775, 102.9277292, 91.6149371, 118.9210451,0, 0,
109.5276097,68.1531569 ,63.3206830 ,85.6972533 ,80.0848315, 84.2716015 , 71.0684790, 102.9079983,0, 0,
106.3146336,57.1735287 ,55.2363140 ,81.2939396 ,81.1325841, 114.6598469, 86.9758389, 64.8397300 ,0, 0,
103.1930322,82.1742706 ,113.5953533,102.4207462,68.0111084, 60.1007438 , 48.1144205, 89.9366414 ,0, 0,
70.4894567 ,69.2187984 ,65.9576235 ,76.5717096 ,82.6929869, 56.1804668 , 97.6883668, 89.8514208 ,57.7545657, 99.5773612,
75.8475668 ,49.6707878 ,75.6903670 ,89.8882038 ,56.9699790, 87.5395277 , 98.5788032, 87.9805775 ,101.7171133, 83.0775394,
71.3697136 ,94.2039126 ,73.8988854 ,63.3363262 ,78.7787264, 66.5958603 , 46.9085056, 77.1465474 ,0, 0,
88.3416063 ,67.0717138 ,75.2898055 ,91.8573482 ,48.4460660, 80.3344686 , 90.8340843, 92.2943432 ,0, 0,
};

const float  TofCAlignPar::def_reanti[5][TOFCSN::SCLRS][TOFCSN::SCMXBR]={
6.07636, 6.05788, 6.0687 , 6.05737, 6.0808 , 6.07691, 6.07215, 6.10081, 0,0,
6.06774, 6.04839, 6.07082, 6.05768, 6.05395, 6.05228, 6.04669, 6.05948, 0,0,
6.01025, 6.04628, 6.03276, 6.05793, 6.00826, 6.02511, 6.05345, 6.0387 , 6.02857, 5.96302,
6.00787, 6.12956, 6.04458, 6.04805, 6.05036, 6.03289, 6.12907, 6.04285, 0,0,

-0.00381032 ,-0.00273721 ,-0.00131218 ,-0.00121866, -0.000151099, -0.00192109, -0.00276097 ,-0.00162753, 0,0,
-0.00204938 ,-0.000305215, 0.00310134 , 0.00474221,  0.000299193, -0.00580165, -0.000280736, 0.00122697, 0,0,
-0.000127784, 0.000638969, 0.000624366, 0.00172185, -0.00114992 , 0.00107044 , -0.00198804 , 0.00102259, -0.000169496, 0.00397209,
 0.00123767 ,-0.00276707 ,-0.000846506, 0.00381077, -0.00188077 , 0.000540594,  0.00103016 , 0.00551123, 0,0,

-9.22364e-05, -7.99335e-05, -8.48724e-05, -6.42846e-05, -9.70359e-05, -9.28482e-05, -0.000103559, -0.000153206, 0,0,
2.41725e-05 , -6.35358e-05, -8.6513e-05 , -6.90582e-05, -3.86272e-05, -3.10755e-05, -6.39479e-05, 0.000124099 , 0,0,
-0.000172392, -0.000104932, -6.31734e-05, -6.9416e-05 , -1.08782e-05, -3.47317e-05, -8.03532e-05, -4.91326e-05, -3.51565e-05, 0.000163921,
3.54171e-05 , -7.74182e-05, -3.38913e-05, -4.85985e-05, -6.26621e-05, -4.20918e-05, -9.93432e-05, 5.67616e-05 , 0,0,

1.87858e-06 , 3.79976e-07,  2.38812e-07,  3.25596e-07, -1.51566e-07,-1.92542e-08,  4.68642e-07, -2.10138e-07, 0,0,
4.36849e-07 , 1.32296e-07, -1.91217e-07, -6.68299e-07, -2.29307e-07, 3.15055e-07, -1.04964e-07, -2.01822e-06, 0,0,
1.03419e-06 ,-1.80383e-07, -1.56225e-07, -2.7278e-07 , 3.50599e-08 ,-4.90815e-08,  1.52437e-08, -3.24771e-07, -7.99015e-09, -2.28116e-06,
-7.65509e-07, 1.94187e-07,  2.93981e-08, -3.4982e-07 , 1.45341e-07 , 8.27521e-08, -4.29231e-07, -1.88915e-06, 0,0,

3.39076e-08 ,1.65031e-08, 1.26911e-08,  1.00035e-08,  2.1609e-08 , 1.38951e-08, 1.63574e-08,  5.51578e-08,  0,0,
-3.28741e-08,1.10828e-08, 3.66975e-09, -8.69872e-10, -1.85929e-09, -3.1355e-09, 8.00364e-09, -1.23939e-07,  0,0,
2.29164e-07 ,3.4186e-08 , 1.63521e-08,  1.25575e-09, -4.71368e-09, 5.25615e-10, 1.43999e-09,  1.50445e-09, 4.65479e-09, 4.69522e-08,
-4.81407e-09,1.11388e-08, 8.68974e-09, -1.29657e-09,  1.2047e-08 , 8.13436e-09, 1.23899e-08,  3.8669e-08 ,  0,0,
};

const float  TofCAlignPar::def_betac[TofCAlignPar::nBetaCh][3][TOFCSN::SCLRS][TOFCSN::SCMXBR]={
1.0000000, 1.0000000, 1.0000000, 1.0000000, 1.0000000, 1.0000000, 1.0000000, 1.0000000, 0.0000000, 0.0000000,
1.0000000, 1.0000000, 1.0000000, 1.0000000, 1.0000000, 1.0000000, 1.0000000, 1.0000000, 0.0000000, 0.0000000,
1.0000000, 1.0000000, 1.0000000, 1.0000000, 1.0000000, 1.0000000, 1.0000000, 1.0000000, 1.0000000, 1.0000000,
1.0000000, 1.0000000, 1.0000000, 1.0000000, 1.0000000, 1.0000000, 1.0000000, 1.0000000, 0.0000000, 0.0000000,
                                                                                                             
1.5049261, 1.5759557, 1.5851727, 1.5844587, 1.6048164, 1.5870994, 1.5743000, 1.5293318, 0.0000000, 0.0000000,
1.6247224, 1.6132079, 1.6144581, 1.6027123, 1.6443934, 1.5754395, 1.6276835, 1.6694508, 0.0000000, 0.0000000,
1.7922582, 1.7506965, 1.7713034, 1.7481126, 1.7566416, 1.7532813, 1.7809424, 1.7874930, 1.8074733, 1.8297497,
1.8270385, 1.8191591, 1.8437468, 1.7964623, 1.8015110, 1.8351608, 1.8009561, 1.7094566, 0.0000000, 0.0000000,
                                                                                                             
0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000,
0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000,
0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000,
0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000,
//---2
4.0000000, 4.0000000, 4.0000000, 4.0000000, 4.0000000, 4.0000000, 4.0000000, 4.0000000, 0.0000000, 0.0000000,
4.0000000, 4.0000000, 4.0000000, 4.0000000, 4.0000000, 4.0000000, 4.0000000, 4.0000000, 0.0000000, 0.0000000,
4.0000000, 4.0000000, 4.0000000, 4.0000000, 4.0000000, 4.0000000, 4.0000000, 4.0000000, 4.0000000, 4.0000000,
4.0000000, 4.0000000, 4.0000000, 4.0000000, 4.0000000, 4.0000000, 4.0000000, 4.0000000, 0.0000000, 0.0000000,
                                                                                                             
1.4097602, 1.4669590, 1.4292231, 1.4077710, 1.5038204, 1.4843572, 1.4455228, 1.4780498, 0.0000000, 0.0000000,
1.5264707, 1.3924678, 1.4378906, 1.4561106, 1.4526356, 1.4073806, 1.4288450, 1.5465060, 0.0000000, 0.0000000,
1.6066721, 1.4865098, 1.5326747, 1.5465549, 1.5324147, 1.5571180, 1.6428788, 1.6242520, 1.6298867, 1.7424765,
1.6110611, 1.5796376, 1.6073930, 1.5142479, 1.5121322, 1.5831674, 1.5493710, 1.4780715, 0.0000000, 0.0000000,
                                                                                                             
0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000,
0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000,
0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000,
0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000,
//---3
9.0000000, 9.0000000, 9.0000000, 9.0000000, 9.0000000, 9.0000000, 9.0000000, 9.0000000, 0.0000000, 0.0000000,
9.0000000, 9.0000000, 9.0000000, 9.0000000, 9.0000000, 9.0000000, 9.0000000, 9.0000000, 0.0000000, 0.0000000,
0.6618840, 0.5671864, 0.5703734, 0.5727187, 0.5993464, 0.5852279, 0.6498284, 0.6227875, 0.6427158, 0.6557751,
0.6227867, 0.6428230, 0.6528718, 0.6041634, 0.6168562, 0.6489466, 0.6406255, 0.6085130, 0.0000000, 0.0000000,
                     
1.1505687 , 1.2821276 ,  1.2209310 , 1.2723965 , 1.3098394 , 1.2688861 , 1.2204610 , 1.3117137 , 0.0000000 ,0.0000000,
1.2379830 , 1.2492575 ,  1.2687087 , 1.2277614 , 1.2399185 , 1.2304264 , 1.2315660 , 1.2589331 , 0.0000000 ,0.0000000,
13.1710828, 16.0602012,  15.8629718, 15.9333603, 14.9628239, 15.7847762, 13.9774175, 14.4066876, 13.9569026,14.0815343,
14.4065586, 13.7523481,  13.2294036, 14.4802669, 14.0244740, 13.2098725, 13.9395455, 14.0165671, 0.0000000 ,0.0000000,
                     
0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000,
0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000,
0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000,
0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000,
//---4
16.0000000, 16.0000000, 16.0000000,16.0000000, 16.0000000,16.0000000,16.0000000,16.0000000,0.0000000, 0.0000000,
16.0000000, 16.0000000, 16.0000000,16.0000000, 16.0000000,16.0000000,16.0000000,16.0000000,0.0000000, 0.0000000,
0.6445531 , 0.6320967 , 0.6556118 ,0.6253235 , 0.6805644 ,0.6865677 ,0.6619630 ,0.6360972 ,0.6510451, 0.6344086,
0.6987403 , 0.6913688 , 0.6426913 ,0.6578084 , 0.6633844 ,0.8109992 ,0.7185505 ,0.6398512 ,0.0000000, 0.0000000,
           
1.1215683 , 1.2199053 , 1.2150477 , 1.2700495 , 1.2495494 , 1.2352229 ,1.1842460 ,1.2782796 , 0.0000000 ,0.0000000,
1.1884877 , 1.2338860 , 1.2238827 , 1.2006125 , 1.2221958 , 1.1552978 ,1.1904995 ,1.2200626 , 0.0000000 ,0.0000000,
14.1822759, 14.0514448, 13.3455636, 14.3203220, 12.5718600, 12.6476421,13.3090276,13.8805996, 13.5012890,14.4117349,
12.6098211, 12.3831165, 13.4364167, 13.1840008, 12.6657348, 9.3425730 ,11.7814790,13.6061154, 0.0000000 ,0.0000000,
           
0.0000000 , 0.0000000,  0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000,
0.0000000 , 0.0000000,  0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000,
0.0000000 , 0.0000000,  0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000,
0.0000000 , 0.0000000,  0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000,
//---5
25.0000000, 25.0000000, 25.0000000, 25.0000000, 25.0000000, 25.0000000, 25.0000000, 25.0000000, 0.0000000,   0.0000000,
25.0000000, 25.0000000, 25.0000000, 25.0000000, 25.0000000, 25.0000000, 25.0000000, 25.0000000, 0.0000000,   0.0000000,
0.7466988 , 0.7218810 , 0.7542056 , 0.7560512 , 0.7463329 , 0.7596627 , 0.7501317 , 0.7412088 , 0.7350025,   0.7474187,
0.8160413 , 0.7583486 , 0.7318781 , 0.7513603 , 0.7250653 , 0.7307974 , 0.7574532 , 0.7766995 , 0.0000000,   0.0000000,
                                                                                                                       
1.0947022 , 1.0477947 , 1.0561407 , 1.0648750 , 1.0639259 , 1.0635160 , 1.0092041 , 1.1671849 , 0.0000000,   0.0000000,
1.0283655 , 1.0475032 , 1.0614034 , 1.0510989 , 1.0715165 , 1.0553065 , 1.0349922 , 1.0220114 , 0.0000000,   0.0000000,
11.1138133, 11.2325742, 10.6113706, 10.6212899, 10.6509273, 10.5182747, 10.7447535, 10.8777554, 11.0032104,  11.1121576,
9.7185979 , 10.3996549, 10.8583782, 10.5457691, 10.9250665, 10.7119161, 10.3646913, 10.1674047, 0.0000000,   0.0000000,
                                                                                                                       
0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000,   0.0000000,
0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000,   0.0000000,
0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000,   0.0000000,
0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000,   0.0000000,
//---6
36.0000000, 36.0000000, 36.0000000, 36.0000000, 36.0000000, 36.0000000, 36.0000000, 36.0000000, 0.0000000,  0.0000000,
36.0000000, 36.0000000, 36.0000000, 36.0000000, 36.0000000, 36.0000000, 36.0000000, 36.0000000, 0.0000000,  0.0000000,
0.7374506 , 0.7242740 , 0.7715660 , 0.7700455 , 0.7602413 , 0.7713475 , 0.7680331 , 0.7551694 , 0.7471065,  0.7353278,
0.8015440 , 0.7546327 , 0.7384427 , 0.7507732 , 0.7350588 , 0.7327127 , 0.7435574 , 0.7817277 , 0.0000000,  0.0000000,
                                                                                                                      
0.8702414 , 0.8017414 , 0.8235706 , 0.8195129 , 0.8305488 , 0.8313120 , 0.7819038 , 0.9490380 , 0.0000000,  0.0000000,
0.8032678 , 0.8074016 , 0.8469423 , 0.8424098 , 0.8528843 , 0.8836901 , 0.8257136 , 0.7732310 , 0.0000000,  0.0000000,
10.6467881, 10.5687970, 9.6535342 , 9.7649657 , 9.7789755 , 9.6465151 , 9.8419704 , 10.0382710, 10.1687727, 10.7554597,
9.2823912 , 9.8582542 , 10.1545335, 9.9328797 , 10.1962491, 10.0835137, 10.0037710, 9.5002857 , 0.0000000,  0.0000000,
                                                                                                                      
0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000,  0.0000000,
0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000,  0.0000000,
0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000,  0.0000000,
0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000,  0.0000000,
//---7
49.0000000, 49.0000000, 49.0000000, 49.0000000, 49.0000000, 49.0000000, 49.0000000, 49.0000000, 0.0000000, 0.0000000,
49.0000000, 49.0000000, 49.0000000, 49.0000000, 49.0000000, 49.0000000, 49.0000000, 49.0000000, 0.0000000, 0.0000000,
0.8105737 , 0.8284488 , 0.8403706 , 0.8494089 , 0.8378600 , 0.8528373 , 0.8557621 , 0.8547513 , 0.8387146, 0.8392559,
0.8401580 , 0.8003877 , 0.7962000 , 0.8119818 , 0.8180894 , 0.8033233 , 0.8207069 , 0.8251997 , 0.0000000, 0.0000000,
                                                                                                                     
-5.6661860, -5.8075657, -5.1274285, -5.3922832, -5.3098053, -5.0753329, -5.7168983, -4.3256891, 0.0000000, 0.0000000,
-6.4678107, -6.8758734, -6.5489463, -6.5738452, -6.4428075, -6.3155963, -6.8878208, -6.5796009, 0.0000000, 0.0000000,
8.7954475 , 8.3648351 , 8.2132237 , 8.0547479 , 8.1344009 , 8.0186420 , 7.9648756 , 8.0443229 , 8.2279793, 8.3661594,
8.1029060 , 8.4146630 , 8.5129561 , 8.1990277 , 8.2081732 , 8.2961640 , 8.0923102 , 8.3053604 , 0.0000000, 0.0000000,
                                                                                                                     
2.6885485 , 2.8173115 , 2.4162121 , 2.5938189 , 2.5418442 , 2.3887783 , 2.7895127 , 1.8982793 , 0.0000000, 0.0000000,
3.2451624 , 3.4772611 , 3.2414799 , 3.2697898 , 3.1758030 , 3.0927519 , 3.4484944 , 3.3005321 , 0.0000000, 0.0000000,
0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000, 0.0000000,
0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000, 0.0000000,
//---8
64.0000000, 64.0000000, 64.0000000, 64.0000000, 64.0000000, 64.0000000, 64.0000000, 64.0000000, 0.0000000, 0.0000000,
64.0000000, 64.0000000, 64.0000000, 64.0000000, 64.0000000, 64.0000000, 64.0000000, 64.0000000, 0.0000000, 0.0000000,
0.8165882 , 0.8400704 , 0.8638319 , 0.8697353 , 0.8553569 , 0.8585388 , 0.8783274 , 0.8723043 , 0.8558448, 0.8345352,
0.8208971 , 0.8013424 , 0.7962886 , 0.8089952 , 0.8034824 , 0.7925532 , 0.7953193 , 0.8051807 , 0.0000000, 0.0000000,
                                                                                                                     
-4.8743717, -5.5555253, -4.8940982, -4.9586936, -4.8522595, -4.6271331, -5.1209185, -3.7732496, 0.0000000, 0.0000000,
-6.0170335, -6.5035984, -6.4005320, -6.2935364, -6.1902646, -5.9444395, -6.4729992, -6.1433035, 0.0000000, 0.0000000,
8.3088071 , 7.8570004 , 7.5247093 , 7.4364588 , 7.5775344 , 7.5929736 , 7.3334637 , 7.4656673 , 7.6310335, 8.0355927,
7.9896303 , 8.0607401 , 8.1839878 , 7.8736329 , 8.1048606 , 8.1161587 , 8.1347959 , 8.2520201 , 0.0000000, 0.0000000,
                                                                                                                     
2.4112901 , 2.8219295 , 2.4320446 , 2.4844344 , 2.4161869 , 2.2735839 , 2.5876062 , 1.7609323 , 0.0000000, 0.0000000,
3.1280771 , 3.4028610 , 3.2981066 , 3.2562211 , 3.1797731 , 3.0302263 , 3.3521419 , 3.1963451 , 0.0000000, 0.0000000,
0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000, 0.0000000,
0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000 , 0.0000000, 0.0000000,
//----10
100.0000000, 100.0000000, 100.0000000, 100.0000000, 100.0000000, 100.0000000, 100.0000000, 100.0000000, 0.0000000, 0.0000000,
100.0000000, 100.0000000, 100.0000000, 100.0000000, 100.0000000, 100.0000000, 100.0000000, 100.0000000, 0.0000000, 0.0000000,
0.8021756  , 0.8253679  , 0.8236429  , 0.8209466  , 0.8124167  , 0.8137699  , 0.8439031  , 0.8215583  , 0.8349880, 0.8308222,
0.8235444  , 0.8121288  , 0.8163080  , 0.8243002  , 0.8304511  , 0.8070833  , 0.8313149  , 0.8454708  , 0.0000000, 0.0000000,
                                                                                                                             
-6.0428602 , -6.3795542 , -6.1123812 ,-5.9040450  , -4.4690931 , -5.3560091 , -6.4499361 , -5.7956100 , 0.0000000, 0.0000000,
-7.4625260 , -7.8029798 , -6.9819034 ,-7.3180527  , -6.0705973 , -6.2609853 , -7.1241852 , -7.2773175 , 0.0000000, 0.0000000,
8.3117913  , 7.9976005  , 7.9253892  ,7.9938649   , 8.1140002  , 8.1429647  , 7.6352240  , 8.0353725  , 7.7552266, 7.8680397,
7.6259980  , 7.7774834  , 7.7731737  ,7.4640238   , 7.5847324  , 7.7696142  , 7.4488115  , 7.4351761  , 0.0000000, 0.0000000,
                                                                                                                             
3.1564275  , 3.3578861  , 3.2245110  ,3.1095257   , 2.3150389  , 2.8103860  , 3.4273344  , 3.0576550  , 0.0000000, 0.0000000,
4.0775527  , 4.2228401  , 3.7204703  ,3.9271558   , 3.2389297  , 3.3209782  , 3.8070604  , 3.9733499  , 0.0000000, 0.0000000,
0.0000000  , 0.0000000  , 0.0000000  ,0.0000000   , 0.0000000  , 0.0000000  , 0.0000000  , 0.0000000  , 0.0000000, 0.0000000,
0.0000000  , 0.0000000  , 0.0000000  ,0.0000000   , 0.0000000  , 0.0000000  , 0.0000000  , 0.0000000  , 0.0000000, 0.0000000,
//----12
144.0000000, 144.0000000, 144.0000000, 144.0000000, 144.0000000, 144.0000000, 144.0000000, 144.0000000, 0.0000000, 0.0000000,
144.0000000, 144.0000000, 144.0000000, 144.0000000, 144.0000000, 144.0000000, 144.0000000, 144.0000000, 0.0000000, 0.0000000,
0.8157519  , 0.8259921  , 0.8388611  , 0.8655632  , 0.8594878  , 0.8661335  , 0.8593635  , 0.8459438  , 0.8574043, 0.8341737,
0.8228096  , 0.8205539  , 0.8212187  , 0.8213028  , 0.8515554  , 0.8194119  , 0.8326113  , 0.8318593  , 0.0000000, 0.0000000,
                                                                                                                             
-6.4707862 ,-5.6688516  , -6.3863859 , -5.8355598 , -6.1010731 , -5.7078361 , -6.3739943 , -5.9420656 ,0.0000000 , 0.0000000,
-8.1939972 ,-7.6614868  , -7.3481033 , -7.4896430 , -7.2648772 , -6.7996795 , -7.1140328 , -7.5704891 ,0.0000000 , 0.0000000,
7.8776148  ,7.9421324   , 7.5642600  , 7.2638154  , 7.3566525  , 7.3238831  , 7.2948348  , 7.5277047  ,7.2856726 , 7.4799567,
7.2663391  ,7.5441997   , 7.5882554  , 7.3744878  , 7.1801898  , 7.5193661  , 7.3227236  , 7.3073386  ,0.0000000 , 0.0000000,
                                                                                                                             
3.5202735  ,3.0180464   , 3.4703606  , 3.1406681  , 3.3155147  , 3.1157096  , 3.4481877  , 3.2826145  ,0.0000000 , 0.0000000,
4.5977893  ,4.2033290   , 4.0061446  , 4.1183175  , 3.9902543  , 3.6943557  , 3.8687080  , 4.2320259  ,0.0000000 , 0.0000000,
0.0000000  ,0.0000000   , 0.0000000  , 0.0000000  , 0.0000000  , 0.0000000  , 0.0000000  , 0.0000000  ,0.0000000 , 0.0000000,
0.0000000  ,0.0000000   , 0.0000000  , 0.0000000  , 0.0000000  , 0.0000000  , 0.0000000  , 0.0000000  ,0.0000000 , 0.0000000,
//----14
196.0000000, 196.0000000, 196.0000000, 196.0000000, 196.0000000, 196.0000000, 196.0000000, 196.0000000, 0.0000000, 0.0000000,
196.0000000, 196.0000000, 196.0000000, 196.0000000, 196.0000000, 196.0000000, 196.0000000, 196.0000000, 0.0000000, 0.0000000,
0.8273196  , 0.8652962  , 0.8601997  , 0.8809768  , 0.8931630  , 0.8704044  , 0.8785584  , 0.8728478  , 0.8541773, 0.8067643,
0.8142918  , 0.8500479  , 0.8396634  , 0.8327686  , 0.8639716  , 0.8425351  , 0.8192505  , 0.8242338  , 0.0000000, 0.0000000,
                                                                                                                             
-4.4958563 , -5.2287147 ,-5.4151759  , -5.2023366 ,-5.4553738  , -5.8514886 , -5.5624664 ,-6.6781471  , 0.0000000, 0.0000000,
-8.1744212 , -6.6153990 ,-7.0032981  , -7.0235799 ,-6.8735400  , -6.3620390 , -6.3615970 ,-7.3674102  , 0.0000000, 0.0000000,
7.7816386  , 7.4972432  ,7.3279567   , 7.0971840  ,6.9921231   , 7.3225838  , 7.0781816  ,7.1800538   , 7.4005954, 7.8824546,
7.2945424  , 7.2025238  ,7.3653809   , 7.2990134  ,7.0401437   , 7.2813775  , 7.6157098  ,7.3216423   , 0.0000000, 0.0000000,
                                                                                                                             
2.5159597  , 2.8041565  ,2.9724712   ,2.8218484   ,2.9888779   , 3.2558177  , 3.0159715  ,3.7685869   , 0.0000000, 0.0000000,
4.6443903  , 3.6300107  ,3.8543893   ,3.9099286   ,3.8156380   , 3.4969952  , 3.4789490  ,4.1664150   , 0.0000000, 0.0000000,
0.0000000  , 0.0000000  ,0.0000000   ,0.0000000   ,0.0000000   , 0.0000000  , 0.0000000  ,0.0000000   , 0.0000000, 0.0000000,
0.0000000  , 0.0000000  ,0.0000000   ,0.0000000   ,0.0000000   , 0.0000000  , 0.0000000  ,0.0000000   , 0.0000000, 0.0000000,
//---16
256.0000000, 256.0000000, 256.0000000, 256.0000000, 256.0000000, 256.0000000, 256.0000000, 256.0000000, 0.0000000, 0.0000000,
256.0000000, 256.0000000, 256.0000000, 256.0000000, 256.0000000, 256.0000000, 256.0000000, 256.0000000, 0.0000000, 0.0000000,
0.8655113  , 0.9005591  , 0.9189821  , 0.9396177  , 0.9464130  , 0.9431624  , 0.9531406  , 0.9425348  , 0.8747536, 0.8243135,
0.8176180  , 0.9085652  , 0.8896930  , 0.9216216  , 0.9065874  , 0.8912664  , 0.9217346  , 0.9058818  , 0.0000000, 0.0000000,
                                                                                                                             
-3.2825606 ,-3.1847600  , -3.6747258 ,-2.4676114  ,-2.7413846  , -4.1705094 , -3.6158317 , -3.0340393 , 0.0000000, 0.0000000,
-5.5173580 ,-3.0220220  , -3.5562158 ,-4.5568427  ,-4.1604840  , -3.6694339 , -4.7278982 , -4.5541583 , 0.0000000, 0.0000000,
7.6465351  , 7.2460895  , 6.8462220  ,6.6796476   ,6.6453571   , 6.7077446  , 6.5027413  , 6.5610791  , 7.3228776, 7.9882015,
7.5497330  , 6.7256461  , 7.0258524  ,6.4815071   ,6.8043334   , 6.9730523  , 6.5293695  , 6.4962105  , 0.0000000, 0.0000000,
                                                                                                                             
1.6855257  ,1.4588387   ,1.8436737   ,1.1346374   ,1.3254778   , 2.1818059  , 1.7573584  , 1.6237319  , 0.0000000, 0.0000000,
3.0382683  ,1.4469125   ,1.7635735   ,2.3744792   ,2.1399475   , 1.8344759  , 2.3950346  , 2.4659418  , 0.0000000, 0.0000000,
0.0000000  ,0.0000000   ,0.0000000   ,0.0000000   ,0.0000000   , 0.0000000  , 0.0000000  , 0.0000000  , 0.0000000, 0.0000000,
0.0000000  ,0.0000000   ,0.0000000   ,0.0000000   ,0.0000000   , 0.0000000  , 0.0000000  , 0.0000000  , 0.0000000, 0.0000000,
//---20
400.0000000, 400.0000000, 400.0000000, 400.0000000, 400.0000000, 400.0000000 , 400.0000000, 400.0000000, 0.0000000  , 0.0000000,
400.0000000, 400.0000000, 400.0000000, 400.0000000, 400.0000000, 400.0000000 , 400.0000000, 400.0000000, 0.0000000  , 0.0000000,
400.0000000, 400.0000000, 400.0000000, 400.0000000, 400.0000000, 400.0000000 , 400.0000000, 400.0000000, 400.0000000, 400.0000000,
400.0000000, 400.0000000, 400.0000000, 400.0000000, 400.0000000, 400.0000000 , 400.0000000, 400.0000000, 0.0000000  , 0.0000000,
                                                                                                                                
-3.0238292 , -1.6570516 , -3.2472434 , -2.4791592 , -3.0545880 , -3.4101854  , -5.6095730 , -5.1883491 , 0.0000000  , 0.0000000,
-5.1807246 , -3.5444127 , -5.6541403 , -5.1701717 , -6.8483364 , -3.4354385  , -3.6812187 , -4.7312031 , 0.0000000  , 0.0000000,
-9.5480036 , -8.1628357 , -10.2188606, -7.2599546 , -7.4280224 , -8.1701367  , -8.3021560 , -8.7035071 ,-9.6614344  , -11.9381577,
-15.0798114, -10.2818313, -10.4163249, -11.5944123, -10.4586910, -9.4046494  , -11.4197200, -14.2980463, 0.0000000  , 0.0000000,
                                                                                                                                
1.4865356  , 0.7157629  , 1.5836030  , 1.1709243  , 1.5075218  , 1.7397479   , 2.8427364  , 2.7896253  , 0.0000000  , 0.0000000,
2.8348394  , 1.7681630  , 2.9747165  , 2.7548938  , 3.6487413  , 1.7419800   , 1.8687771  , 2.5585267  , 0.0000000  , 0.0000000,
4.9164827  , 4.1508289  , 5.3114021  , 3.6715982  , 3.7670151  , 4.1636069   , 4.2690778  , 4.4991782  , 5.0474642  , 6.2407161,
8.1976530  , 5.5027875  , 5.5572288  , 6.2031171  , 5.5885041  , 4.9704778   , 6.1115827  , 7.7722219  , 0.0000000  , 0.0000000,
//---26                                                                                                                           
676.0000000, 676.0000000, 676.0000000, 676.0000000, 676.0000000,  676.0000000, 676.0000000, 676.0000000, 0.0000000  , 0.0000000,
676.0000000, 676.0000000, 676.0000000, 676.0000000, 676.0000000,  676.0000000, 676.0000000, 676.0000000, 0.0000000  , 0.0000000,
676.0000000, 676.0000000, 676.0000000, 676.0000000, 676.0000000,  676.0000000, 676.0000000, 676.0000000, 676.0000000, 676.0000000,
676.0000000, 676.0000000, 676.0000000, 676.0000000, 676.0000000,  676.0000000, 676.0000000, 676.0000000, 0.0000000  , 0.0000000,
                                                                                                                                
-7.0369625 , -9.4189290 , -8.5353249 , -9.5851193 , -9.2938038 , -8.2401640  , -8.1094346 , -7.6182752 ,  0.0000000 , 0.0000000,
-11.3898189, -10.2814964, -11.0920759, -12.0168424, -10.0358423, -9.7108011  , -10.0582831, -11.3589213,  0.0000000 , 0.0000000,
-11.9293599, -11.8891287, -10.9595271, -10.9179695, -12.1976964, -11.2602066 , -10.9363006, -11.1911498, -11.8083981, -11.5685188,
-16.3270021, -18.6787839, -17.5264097, -16.5928250, -18.1762073, -17.8181797 , -17.4010303, -15.7057185,  0.0000000 , 0.0000000,
                                                                                                                                
3.9580721  , 5.3046541  , 4.7846282  , 5.3970427  ,  5.2325196 , 4.6242581   , 4.5729090  , 4.2890073  ,  0.0000000 , 0.0000000,
6.4768133  , 5.8094986  , 6.2640736  , 6.8217132  ,  5.6589704 , 5.5004637   , 5.7018962  , 6.5094674  ,  0.0000000 , 0.0000000,
6.3597949  , 6.4014509  , 5.8529627  , 5.8672133  ,  6.6037052 , 6.0666768   , 5.8846211  , 6.0122781  ,  6.3302642 , 6.1324337,
8.8431215  , 10.2838359 , 9.6468837  , 9.0918608  ,  9.9910355 , 9.7655024   , 9.5281649  , 8.5550446  ,  0.0000000 , 0.0000000,
};

//===========================================================
const float TofCAlignPar::def_betac1[2][TofCAlignPar::nBetaCh][TOFCSN::SCLRS][TOFCSN::SCMXBR]={
0.9798325, 1.0000163, 1.0067221, 1.0108180, 1.0077246, 0.9987227, 1.0001682, 0.9807914, 0,0,
0.9935245, 1.0070796, 0.9967012, 0.9903594, 1.0078620, 0.9847397, 1.0098703, 1.0153905, 0,0,
1.0273912, 0.9973700, 0.9959865, 0.9870390, 0.9927478, 0.9945166, 0.9845579, 0.9924247, 0.9924399, 1.0307544,
0.9739988, 0.9844974, 0.9872112, 0.9733495, 0.9816317, 0.9845523, 0.9814385, 0.9349247, 0,0,
//--2                                                                                  
0.9660185, 0.9795693, 0.9893918, 0.9703392, 0.9943178, 0.9898353, 0.9792916, 0.9817967, 0,0,
0.9782581, 0.9672362, 0.9628605, 0.9781148, 0.9818506, 0.9594660, 0.9673773, 0.9910432, 0,0,
0.9648627, 0.9480870, 0.9616153, 0.9594182, 0.9569091, 0.9574695, 0.9732454, 0.9761969, 0.9737092, 0.9885637,
0.9534654, 0.9495739, 0.9601784, 0.9438242, 0.9399217, 0.9557742, 0.9433676, 0.9229907, 0,0,
//--3                                                                                  
0.9727256, 0.9848514, 0.9752637, 0.9766417, 0.9888713, 0.9843404, 0.9812453, 0.9927454, 0,0,
0.9544311, 0.9706188, 0.9763949, 0.9735478, 0.9677640, 0.9796783, 0.9788171, 0.9795900, 0,0,
0.9922827, 0.9852336, 0.9807908, 0.9816660, 0.9848691, 0.9851761, 0.9843712, 0.9857467, 0.9899965, 0.9905913,
0.9710422, 0.9825855, 0.9776573, 0.9752942, 0.9766310, 0.9783471, 0.9833431, 0.9596729, 0,0,
//--4                                                                                  
0.9443573, 0.9842162, 0.9777975, 0.9860584, 0.9847462, 0.9800295, 0.9605703, 0.9511881, 0,0,
0.9557814, 0.9816871, 0.9565310, 0.9598824, 0.9783700, 0.9687328, 0.9562789, 0.9528506, 0,0,
0.9608487, 0.9711040, 0.9734803, 0.9954689, 0.9764287, 0.9781035, 0.9936657, 0.9695689, 0.9951355, 0.9951580,
0.9912876, 0.9655536, 0.9858655, 0.9913397, 0.9902430, 1.0091738, 0.9693263, 0.9843386, 0,0,
//--5                                                                                  
0.9772097, 0.9826137, 0.9828437, 0.9854650, 0.9818085, 0.9851118, 0.9823845, 0.9866974, 0,0,
0.9856333, 0.9817341, 0.9815365, 0.9690818, 0.9807162, 0.9634451, 0.9812292, 0.9812895, 0,0,
0.9988916, 1.0015690, 1.0008754, 1.0041448, 1.0023056, 1.0046399, 0.9988885, 1.0012169, 1.0007007, 1.0016712,
0.9993485, 0.9745114, 0.9583882, 0.9827681, 0.9716339, 0.9953739, 0.9802369, 0.9606116, 0,0,
//--6                                                                                  
0.9848635, 0.9861991, 0.9861760, 0.9861791, 0.9809889, 0.9878329, 0.9856129, 0.9865804, 0,0,
0.9872369, 0.9827249, 0.9801391, 0.9839728, 0.9807003, 0.9848330, 0.9832498, 0.9813004, 0,0,
1.0025119, 1.0053636, 1.0076991, 1.0108500, 1.0081322, 1.0104395, 1.0028433, 1.0066389, 1.0063889, 1.0040402,
1.0044293, 0.9785827, 0.9936376, 1.0088296, 1.0032151, 0.9991293, 0.9871315, 0.9682436, 0,0,
//--7                                                                                  
0.9787028, 0.9871463, 0.9848037, 0.9860269, 0.9863375, 0.9876302, 0.9895818, 0.9652489, 0,0,
0.9928575, 0.9871531, 0.9898787, 0.9893638, 0.9853402, 0.9868307, 0.9896801, 0.9931084, 0,0,
1.0096279, 1.0119104, 1.0064254, 1.0062755, 1.0067284, 1.0046803, 1.0080550, 1.0081910, 1.0098549, 1.0107361,
1.0214610, 1.0215830, 1.0151651, 1.0219530, 1.0149692, 1.0180327, 1.0243776, 1.0158637, 0,0,
//--8                                                                                  
0.9946426, 0.9978181, 0.9949346, 0.9955130, 0.9952050, 0.9967374, 0.9987507, 0.9852542, 0,0,
1.0009665, 0.9960748, 0.9972512, 0.9983383, 0.9952585, 0.9959310, 0.9985341, 1.0027220, 0,0,
1.0141873, 1.0180289, 1.0133016, 1.0130585, 1.0118766, 1.0119346, 1.0140084, 1.0135782, 1.0167990, 1.0175396,
1.0263767, 1.0254208, 1.0201117, 1.0262190, 1.0205376, 1.0211090, 1.0272380, 1.0196011, 0,0,
//--10                                                                                 
0.9834215, 0.9872801, 0.9854785, 0.9866933, 0.9866403, 0.9884318, 0.9901153, 0.9767973, 0,0,
0.9934712, 0.9902167, 0.9905461, 0.9917537, 0.9895898, 0.9902527, 0.9910363, 0.9954329, 0,0,
1.0052457, 1.0077530, 1.0040662, 1.0048704, 1.0030503, 1.0046086, 1.0062604, 1.0054741, 1.0070544, 1.0100691,
1.0123111, 1.0096011, 1.0060554, 1.0111353, 1.0095536, 1.0077006, 1.0113500, 1.0060202, 0,0,
//--12                                                                                 
0.9860834, 0.9901695, 0.9909272, 0.9916242, 0.9943597, 0.9934828, 0.9937906, 0.9801516, 0,0,
0.9995568, 0.9949255, 0.9940598, 0.9955720, 0.9936112, 0.9942566, 0.9955206, 1.0002558, 0,0,
1.0089023, 1.0115113, 1.0100398, 1.0094303, 1.0089967, 1.0093420, 1.0104115, 1.0112281, 1.0133442, 1.0121276,
1.0129766, 1.0120853, 1.0084771, 1.0140256, 1.0126781, 1.0117038, 1.0151290, 1.0023570, 0,0,
//--14                                                                                 
0.9974358, 1.0029011, 0.9994118, 0.9997019, 1.0047626, 1.0064161, 1.0012832, 0.9856590, 0,0,
1.0107336, 1.0002997, 1.0035279, 1.0066981, 1.0014225, 1.0022750, 1.0018210, 1.0095992, 0,0,
1.0028534, 1.0113767, 1.0116045, 1.0126089, 1.0104678, 1.0131848, 1.0125234, 1.0124862, 1.0104011, 1.0040641,
1.0079161, 1.0126677, 1.0076804, 1.0163089, 1.0133295, 1.0101326, 1.0128940, 0.9974512, 0,0,
//--16                                                                                 
0.9577163, 0.9635004, 0.9666458, 0.9647068, 0.9690790, 0.9714468, 0.9657685, 0.9613189, 0,0,
0.9623690, 0.9609312, 0.9660526, 0.9666572, 0.9645367, 0.9678655, 0.9657140, 0.9640925, 0,0,
0.9874540, 1.0041109, 1.0003122, 0.9984796, 0.9975298, 0.9985210, 0.9991956, 1.0032080, 1.0068251, 0.9937103,
0.9926481, 0.9959820, 0.9903030, 0.9991527, 0.9905234, 0.9933300, 0.9996000, 0.9871809, 0,0,
//--20                                                                                 
0.9383117, 0.9695531, 0.9571521, 0.9584181, 0.9563964, 0.9578637, 0.9539768, 0.9541268, 0,0,
0.9550130, 0.9573727, 0.9622230, 0.9589378, 0.9629446, 0.9572896, 0.9576105, 0.9464770, 0,0,
0.9912384, 0.9939851, 0.9982656, 0.9963180, 0.9959290, 0.9968904, 1.0017514, 1.0008775, 1.0008950, 0.9850671,
0.9886749, 0.9882911, 0.9876421, 0.9890950, 0.9950456, 0.9910180, 0.9889252, 0.9877586, 0,0,
//--26                                                                                 
0.9963464, 1.0100479, 1.0060099, 1.0091351, 1.0090429, 1.0112585, 1.0051266, 0.9868350, 0,0,
1.0117179, 1.0026193, 1.0088568, 1.0169979, 1.0058256, 1.0133886, 1.0030798, 1.0121498, 0,0,
1.0051560, 1.0263754, 1.0201825, 1.0223254, 1.0243073, 1.0213517, 1.0193534, 1.0218028, 1.0206492, 1.0033430,
1.0153775, 1.0250570, 1.0233394, 1.0339305, 1.0237142, 1.0177300, 1.0275449, 1.0037857, 0, 0,

///---Par2
1.0146090, 1.0571976, 1.0678822, 1.0758882, 1.0702633, 1.0608661, 1.0563806, 1.0218095, 0,0,
1.0497710, 1.0690961, 1.0559493, 1.0397787, 1.0772398, 1.0424657, 1.0772908, 1.0854477, 0,0,
1.0859705, 1.0645218, 1.0618857, 1.0493026, 1.0613025, 1.0575453, 1.0490234, 1.0564876, 1.0647359, 1.0917335,
1.0460244, 1.0514917, 1.0598792, 1.0378113, 1.0500181, 1.0548147, 1.0470477, 0.9798235, 0,0,
//--2                                                                                  
1.0279988, 1.0447265, 1.0446373, 1.0273521, 1.0685993, 1.0591871, 1.0403345, 1.0477636, 0,0,
1.0306431, 1.0268619, 1.0301124, 1.0401593, 1.0498508, 1.0117313, 1.0252160, 1.0601422, 0,0,
1.0682292, 1.0131692, 1.0415006, 1.0326693, 1.0333891, 1.0349191, 1.0620520, 1.0684783, 1.0662124, 1.1321713,
1.0508724, 1.0397156, 1.0670625, 1.0277776, 1.0259731, 1.0593925, 1.0244981, 0.9896665, 0,0,
//--3                                                                                  
0.9826434, 1.0127288, 0.9680089, 0.9889909, 1.0292271, 1.0197927, 0.9720919, 1.0099723, 0,0,
0.9740076, 0.9746069, 1.0020325, 0.9769642, 0.9883698, 1.0131707, 1.0040998, 0.9767545, 0,0,
0.9664717, 0.9691867, 0.9653008, 0.9780461, 0.9767634, 0.9916658, 1.0194206, 1.0020089, 1.0036557, 1.0342306,
0.9814994, 0.9897017, 0.9833190, 0.9640026, 0.9605629, 0.9813290, 0.9874827, 0.9381665, 0,0,
//--4                                                                                  
0.9522243, 1.0055276, 0.9906824, 1.0150087, 1.0126824, 0.9961067, 0.9984160, 0.9980512, 0,0,
0.9631034, 1.0126817, 1.0053248, 0.9958433, 0.9960040, 0.9705664, 0.9923976, 0.9924527, 0,0,
1.0359882, 1.0115168, 1.0143521, 1.0158482, 1.0142726, 1.0280292, 1.0227637, 1.0094125, 1.0102694, 1.0352265,
1.0355309, 1.0162517, 0.9921460, 1.0482404, 1.0146666, 0.9920917, 1.0602446, 0.9874318, 0,0,
//--5                                                                                  
1.0088258, 1.0098761, 1.0299243, 1.0203998, 1.0342391, 1.0341270, 1.0073707, 1.0339342, 0,0,
0.9815611, 1.0170226, 1.0146497, 1.0122296, 1.0338880, 1.0267670, 1.0267820, 0.9944279, 0,0,
1.0443640, 1.0164930, 1.0206003, 1.0463307, 1.0192025, 1.0274893, 1.0223410, 1.0181000, 1.0203350, 1.0474920,
1.0473033, 1.0212653, 1.0054051, 1.0232832, 1.0063763, 1.0015385, 1.0228425, 1.0252522, 0,0,
//--6                                                                                  
1.0268315, 1.0076371, 1.0135150, 1.0142034, 1.0128208, 1.0173813, 1.0069273, 1.0472784, 0,0,
0.9775022, 1.0153902, 1.0162309, 1.0196445, 1.0175308, 1.0131763, 1.0125722, 0.9933469, 0,0,
1.0223918, 1.0144825, 1.0222676, 1.0261087, 1.0177319, 1.0253248, 1.0242958, 1.0228481, 1.0065892, 1.0252062,
1.0271722, 1.0027789, 0.9892117, 1.0107497, 0.9965482, 1.0099578, 1.0011451, 1.0150889, 0,0,
//--7                                                                                  
1.0100509, 0.9970195, 1.0023715, 0.9976066, 0.9974612, 1.0053560, 0.9974671, 1.0191367, 0,0,
0.9809967, 0.9887833, 1.0061843, 1.0003812, 1.0012960, 1.0071232, 0.9997189, 0.9798880, 0,0,
1.0281051, 1.0085196, 1.0149643, 1.0060271, 1.0029715, 1.0067450, 1.0130051, 1.0124662, 1.0153418, 1.0329369,
1.0348366, 1.0107123, 1.0084479, 1.0064666, 1.0119926, 1.0077736, 1.0133910, 1.0369400, 0,0,
//--8                                                                                  
1.0031962, 0.9832889, 0.9893277, 0.9842008, 0.9864255, 0.9949815, 0.9861086, 1.0178828, 0,0,
0.9706220, 0.9804415, 0.9921487, 0.9902995, 0.9882125, 0.9945822, 0.9867940, 0.9701087, 0,0,
1.0166909, 0.9997760, 1.0000035, 0.9952774, 0.9928158, 0.9968381, 0.9994212, 1.0060261, 1.0044932, 1.0213572,
1.0225233, 0.9967802, 1.0015091, 0.9929370, 1.0015175, 0.9939475, 1.0005251, 1.0317132, 0,0,
//--10                                                                                 
0.9950773, 0.9858198, 0.9805123, 0.9823950, 0.9768696, 0.9803573, 0.9794364, 0.9923124, 0,0,
0.9656639, 0.9827450, 0.9918366, 0.9878266, 0.9858621, 0.9968978, 0.9931235, 0.9608819, 0,0,
1.0021943, 0.9943936, 0.9900742, 0.9845501, 0.9879809, 0.9904512, 0.9901047, 0.9913895, 0.9918529, 1.0026335,
1.0037491, 0.9901311, 0.9945611, 0.9790721, 0.9999390, 0.9901432, 0.9911967, 1.0183505, 0,0,
//--12                                                                                 
0.9829938, 0.9909599, 0.9751403, 0.9837065, 0.9785947, 0.9784534, 0.9850581, 0.9732469, 0,0,
0.9555197, 0.9902962, 0.9968421, 0.9899294, 0.9861400, 1.0025785, 0.9979911, 0.9519637, 0,0,
1.0055753, 1.0046927, 0.9909375, 0.9934383, 1.0014279, 0.9996301, 0.9954107, 0.9981183, 0.9963591, 0.9969479,
0.9856367, 0.9965120, 1.0079587, 0.9845900, 1.0050111, 1.0043653, 1.0029135, 1.0079790, 0,0,
//--14                                                                                 
0.9729385, 1.0018520, 0.9803672, 0.9915217, 0.9836971, 0.9773924, 0.9930450, 0.9733223, 0,0,
0.9513443, 1.0006868, 0.9947620, 0.9885234, 0.9845218, 1.0033633, 1.0005705, 0.9509911, 0,0,
1.0040015, 1.0221766, 1.0020499, 1.0017699, 1.0094974, 1.0097344, 1.0077983, 1.0038449, 1.0080354, 0.9892405,
0.9839553, 1.0124453, 1.0154105, 0.9971247, 1.0164170, 1.0069587, 1.0104041, 0.9917985, 0,0,
//--16                                                                                 
1.0109413, 1.0422652, 1.0224103, 1.0311993, 1.0203748, 1.0173702, 1.0246756, 0.9834465, 0,0,
0.9842635, 1.0518686, 1.0393902, 1.0298289, 1.0307457, 1.0465831, 1.0526420, 0.9971867, 0,0,
1.0367376, 1.0382781, 1.0329489, 1.0375413, 1.0455511, 1.0441298, 1.0418674, 1.0213869, 1.0275271, 1.0209976,
1.0006324, 1.0486909, 1.0439342, 1.0427872, 1.0427063, 1.0518202, 1.0424156, 1.0345618, 0,0,
//--20                                                                                 
1.0128708, 1.0367378, 1.0241935, 1.0249676, 1.0178971, 1.0137467, 1.0298536, 1.0043731, 0,0,
0.9951974, 1.0480168, 1.0403221, 1.0179970, 1.0288445, 1.0401112, 1.0376891, 1.0076793, 0,0,
1.0417211, 1.0404493, 1.0331630, 1.0308998, 1.0391012, 1.0338719, 1.0310751, 1.0251136, 1.0304513, 1.0294231,
1.0068606, 1.0277921, 1.0301169, 1.0281445, 1.0255188, 1.0379186, 1.0303111, 1.0203981, 0,0,
//--26                                                                                 
0.9911052, 0.9693870, 0.9794300, 0.9725260, 0.9729623, 0.9874564, 0.9843770, 0.9926872, 0,0,
0.9803639, 0.9842012, 0.9827713, 0.9779897, 0.9837287, 0.9802375, 0.9802901, 0.9711717, 0,0,
0.9952776, 0.9865870, 0.9932172, 0.9855283, 0.9777992, 0.9841752, 0.9873615, 0.9893833, 0.9893050, 0.9987624,
0.9994668, 0.9626894, 0.9710953, 0.9742877, 0.9717425, 0.9795821, 0.9826099, 1.0032843, 0,0,
};                   

//===========================================================
const float TofCAlignPar::def_betacn[TofCAlignPar::nBetaCh][5][TOFCSN::SCLRS][TOFCSN::SCMXBR]={
//--1
6.8339427   , 6.6478904   , 6.7080978   ,  6.3463618  , 6.9678366   , 6.8444730    , 6.7422413    ,6.6045281    ,  0,0,
8.2203831   , 7.9073323   , 8.1371658   ,  8.1861008  , 8.0441698   , 7.7328598    , 7.9599694    ,8.1102062    ,  0,0,
23.3468898  , 26.8069307  , 29.0822438  ,  28.3713083 , 29.6139538  , 28.0705581   , 31.6093498   ,29.9800409   ,  30.5464173,   23.6207681,
39.6990801  , 36.8165432  , 38.2013710  ,  37.1476033 , 36.1061827  , 37.7561332   , 36.5705572   ,35.6349208   ,  0,0,
                                                                                                                 
-10.7648317 , -8.8447114  , -8.9395933  , -7.4892570  , -9.7063841  , -9.2814362   , -9.0712019   ,-9.3195057   , 0,0,
-14.7637303 , -13.3288204 , -14.0852646 , -14.5377969 , -13.3667463 , -12.8930053  , -13.2341679  ,-13.8450983  , 0,0,
-86.7191550 , -110.7393547, -121.6770714, -118.2861604, -125.1870191, -116.7824198 , -133.9773120 ,-125.4901704 , -127.8441680, -85.8797600,
-174.8166444, -160.5432237, -167.2378203, -161.8760632, -157.4132988, -164.2354899 , -160.0272931 ,-154.8702177 , 0,0,
                                                                                                                 
4.0568206   , 0.9291895   , 0.9866448   , -0.8681089  , 1.7159176   ,  1.1602488   ,  1.1187752   , 1.7539769   , 0,0,
7.9148276   , 6.3252209   , 7.0636671   ,  7.7413930  , 5.8881317   ,  5.6512774   ,  5.9602635   , 6.5717462   , 0,0,
137.1568986 , 192.9675131 , 212.5874867 ,  206.3006086, 220.4042457 ,  203.6241960 ,  234.8548335 , 218.6456693 , 222.4648075,  132.8223411,
311.2140269 , 283.9417528 , 296.8888797 ,  286.0047339, 279.1821844 ,  289.8646277 ,  284.2964280 , 271.6483521 , 0,0,
                                                                                                                 
0.8752104   , 2.3179122   , 2.3098312   ,  3.0823079  , 2.0853838   ,  2.3332768   ,  2.2649267   , 1.9690195   , 0,0,
-0.3426642  , 0.1591625   , -0.0664813  , -0.3544980  , 0.5009727   ,  0.5418365   ,  0.3854691   , 0.2262651   , 0,0,
-102.9941528, -156.9829188, -172.5117769, -167.3389142, -179.8384959,  -165.1934025,  -190.3858169, -176.7641993, -179.6971419, -98.1078742,
-253.3831505, -229.9024955, -241.1989242, -231.3069062, -226.8303929,  -234.2697380,  -231.2568306, -217.9630714, 0,0,
                                                                                                                 
0.0000000   , 0.0000000   , 0.0000000   ,  0.0000000  , 0.0000000   ,  0.0000000   ,  0.0000000   , 0.0000000   , 0,0,
0.0000000   , 0.0000000   , 0.0000000   ,  0.0000000  , 0.0000000   ,  0.0000000   ,  0.0000000   , 0.0000000   , 0,0,
30.2815199  , 49.0102582  , 53.5821205  ,  52.0032958 , 56.0691073  ,  51.3427593  ,  58.9529843  , 54.6876972  , 55.5925246,    28.6140926,
78.3247887  , 70.7105506  , 74.3964837  ,  71.0623652 , 70.0007325  ,  71.9360405  ,  71.4413267  , 66.4977088  , 0,0,
//--2
5.3069340   ,  6.5924490   , 6.3665300   , 7.1046803   , 6.4852682    ,6.2160380    , 6.4843771   , 5.7963616   , 0,0,
6.6994230   ,  8.2459842   , 8.0168234   , 6.9051815   , 7.3897971    ,7.6517427    , 7.9896998   , 6.5570077   , 0,0,
28.1001170  ,  28.4239013  , 30.3893153  , 28.8580877  , 29.1818923   ,28.9521471   , 28.0936783  , 27.4373367  , 28.8171672,   24.1432721,
36.6542947  ,  35.0655393  , 33.6249501  , 37.0435498  , 34.0022895   ,34.2614127   , 35.3649104  , 31.8673574  , 0,0,
                                                                                                                 
-7.9427546  ,  -12.6305165 , -12.0553627 , -14.9729846 , -12.0419057  ,-11.0990618  , -12.4039218 , -9.5420913  , 0,0,
-13.2441300 ,  -19.0772830 , -17.7736065 , -13.6141313 , -15.7754308  ,-16.4479229  , -17.7941842 , -12.6782208 , 0,0,
-118.6843159,  -120.3939603, -132.1846760, -124.3629667, -125.1734294 ,-123.6184446 , -119.8524839, -117.7400489, -123.5698868, -97.8799747,
-162.9222193,  -154.9597409, -148.8819923, -165.8315240, -150.2396491 ,-152.1370370 , -155.5032023, -140.6596874, 0,0,
                                                                                                                 
3.3978101   ,  9.2577109   , 8.7842046   , 12.3891385  ,  8.4729001   , 7.3598383   , 9.1157768   ,  5.3626421  , 0,0,
10.0606388  ,  17.1886120  , 15.2341234  , 10.2362719  ,  13.2009472  , 13.5246855  , 15.3431769  ,  9.4265890  , 0,0,
204.7972349 ,  207.9755529 , 233.2306064 , 218.7319492 ,  218.7638613 , 215.5095879 , 210.4252114 ,  208.1119433, 216.9888107,  166.0251528,
289.6696292 ,  274.6521614 , 265.6401565 , 296.4444995 ,  266.6182111 , 271.7913755 , 273.9913145 ,  249.6691623, 0,0,
                                                                                                                 
0.2533093   , -2.1786550   , -2.0556350  , -3.5018329  ,  -1.8557588  , -1.4177896  , -2.1579056  ,  -0.5828673 ,  0,0,
-2.5109255  , -5.3397285   , -4.4542292  , -2.4891755  ,  -3.7763088  , -3.7247644  , -4.5215889  ,  -2.2710176 ,  0,0,
-161.9292209, -164.5143467 , -187.4795906, -175.8047611,  -174.7270091, -171.9378129, -169.5170410,  -168.5917450, -174.4154598, -130.8665576,
-233.5541643, -221.1992721 , -215.4837908, -240.1468782,  -215.0143338, -220.6234139, -219.4255240,  -201.5328439, 0,0,
                                                                                                                 
0.0000000   , 0.0000000    , 0.0000000   , 0.0000000   ,  0.0000000   , 0.0000000   , 0.0000000   ,  0.0000000  ,  0,0,
0.0000000   , 0.0000000    , 0.0000000   , 0.0000000   ,  0.0000000   , 0.0000000   , 0.0000000   ,  0.0000000  ,  0,0,
48.7647771  , 49.5183690   , 57.0874585  , 53.6176238  ,  52.9928799  , 52.1353096  , 51.9259120  ,  51.8572124 ,  53.2467639,   39.6782149,
71.1966025  , 67.4594668   , 66.1611225  , 73.5268651  ,  65.6582434  , 67.7688965  , 66.5811586  ,  61.6102521 ,  0,0,
//--3
6.4940504   ,  5.7400286  , 5.8568192   , 5.5895432   , 5.9322748    ,6.4338301    , 5.9054750   , 6.7785624   ,0,0, 
7.9192926   ,  6.2799370  , 6.6262942   , 6.6702071   , 6.8005492    ,6.4486764    , 6.9738425   , 6.7906947   ,0,0,
21.2224299  ,  22.0591976 , 24.1133592  , 24.1509683  , 25.0153907   ,24.5615519   , 25.2134445  , 23.1795443  ,25.7624512, 23.3405645,
27.3885090  ,  30.6422996 , 32.0574529  , 31.1255024  , 32.8165495   ,30.7938447   , 28.5628187  , 30.5357530  ,0,0,
                                                                                                                
-13.1836001 , -9.4729370  , -9.9829771  , -8.5532028  , -10.1183771  ,-12.1165390  , -10.1578832 , -13.4985216 , 0,0,
-18.3330252 , -10.7699880 , -12.2861959 , -12.5199781 , -12.8852122  ,-11.9222241  , -13.6110711 , -13.7705591 , 0,0,
-82.1546727 , -87.8847056 , -97.0744202 , -97.7035855 , -102.1220086 ,-100.2681416 , -102.6277660, -92.0159646 ,-105.4914378, -92.8258332,
-111.7641554, -131.4233250, -136.4945540, -133.3965725, -141.2672434 ,-131.5391225 , -120.9996227, -127.3826730, 0,0,
                                                                                                                
10.1084315  , 5.1665254   , 5.7130425   , 3.7098451   ,  5.8742335   , 8.3941603   , 6.0027716   , 10.1688317  ,  0,0,
16.3407160  , 5.9310493   , 8.0227949   , 8.2721725   ,  8.5958400   , 7.8673994   , 9.6778569   , 10.4206557  ,  0,0,
135.4670872 , 147.7629836 , 163.6358836 , 165.7760345 ,  173.6636656 , 171.4484050 , 173.7490828 , 153.0160029 ,179.0127979, 154.5327165,
188.1111584 , 229.1885359 , 235.5588971 , 232.6461962 ,  246.1539564 , 228.1228345 , 209.9336643 , 214.9650689 ,  0,0,
                                                                                                                
-2.4564530  , -0.4329875  , -0.6072304  , 0.2355551   ,  -0.6713337  , -1.7049859  , -0.7593058  , -2.4296211  ,  0,0,
-4.9766292  , -0.4736127  , -1.3753577  , -1.4330568  ,  -1.5350098  , -1.3903956  , -2.0472855  , -2.4892833  ,  0,0,
-105.6613490, -116.2300637, -128.6315180, -131.1558057,  -137.0731776, -136.3304561, -136.4064564, -118.6554194, -140.5457796, -119.9415222,
-147.1579898, -183.4213869, -186.4209269, -186.5186191,  -196.4456563, -181.5974785, -167.7367058, -166.8347225,  0,0,
                                                                                                                
0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   ,  0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   ,  0,0,
0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   ,  0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   ,  0,0,
32.0817531  , 35.2607523  , 38.9544770  , 39.9493114  ,  41.5316831  , 41.6202320  , 41.1224291  , 35.5029857  , 42.2925895,   35.9079994,
44.3922916  , 55.9826227  , 56.3071555  , 57.1066953  ,  59.7365354  , 55.1950295  , 51.2118909  , 49.6253021  ,  0,0,
//--4
3.0623514   , 3.4620647   , 3.5025697   , 3.4816674   , 4.9777679   , 3.9312548   , 3.7150800   , 4.1117572   , 0,0,
5.6336553   , 4.5197740   , 4.9276708   , 4.3755292   , 4.5019801   , 4.5495611   , 4.5599224   , 4.7085748   , 0,0,
19.7282684  , 22.3214793  , 21.8422815  , 22.6000723  , 24.4522887  , 23.8588370  , 22.7804243  , 18.2883657  , 23.0693729, 21.5831019,
33.3268441  , 33.1679281  , 31.3487625  , 36.4715228  , 32.5534548  , 33.8476084  , 30.7991868  , 33.6009837  , 0,0,
                                                                                                               
-0.4517623  , -2.1525804  , -2.1637794  , -2.1181585  , -7.5654429  , -3.8163675  , -3.1427959  , -4.1602720  , 0,0,
-10.0820981 , -5.3574325  , -6.8022288  , -4.9144368  , -5.2507313  , -5.4227325  , -5.4508628  , -6.6693256  , 0,0,
-77.7843150 , -91.3319608 , -89.0033220 , -92.8121616 , -101.7349544, -99.6206806 , -93.1525658 , -70.8334133 , -94.4356406, -87.5297137,
-145.4907020, -144.5705615, -135.8797066, -160.8240799, -141.3764555, -147.8442962, -133.4105736, -147.7822394, 0,0,
                                                                                                               
-5.2517069  , -2.7862768  , -2.9461048  , -2.9163262  , 3.5978821   , -0.8552335  , -1.5777302  , -0.7345975  , 0,0,
6.6892075   , 0.3234335   , 2.0324871   , -0.1450559  , 0.1533625   , 0.2188392   , 0.3584107   , 2.4766886   , 0,0,
131.7488555 , 156.6800188 , 152.6834942 , 159.8536453 , 176.3020692 , 173.1508295 , 159.5360782 , 118.2904214 , 161.4316781, 150.4660594,
257.0160619 , 254.6288389 , 238.5287244 , 285.0172909 , 248.0687621 , 260.5559807 , 234.2935886 , 261.9573767 , 0,0,
                                                                                                               
3.5872738   , 2.4705011   , 2.5920791   , 2.5554457   , -0.0109837  , 1.7288067   , 1.9925327   , 1.7679423   , 0,0,
-1.2787104  , 1.5161646   , 0.8370435   , 1.6681760   , 1.5809917   , 1.6276984   , 1.5169481   , 0.4543575   , 0,0,
-105.0080331, -124.5721397, -121.7013216, -127.6402058, -141.2005191, -138.8447483, -126.7066396, -93.1117052 , -127.8586535, -120.7295997,
-207.4167166, -204.7649106, -191.4388622, -230.0246925, -198.9297775, -209.5307311, -188.1292245, -211.9308635, 0,0,
                                                                                                               
0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   ,  0.0000000  , 0.0000000   , 0.0000000   , 0.0000000   , 0,0,
0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   ,  0.0000000  , 0.0000000   , 0.0000000   , 0.0000000   , 0,0,
32.3374707  , 37.9107705  , 37.1931086  , 39.0151087  ,  43.2256182 , 42.4852358  , 38.5685103  , 28.3769521  , 38.8010918, 37.2271597,
63.5929894  , 62.5674124  , 58.4271421  , 70.4133954  ,  60.6707328 , 64.0039297  , 57.4550912  , 65.1054843  , 0,0,
//--5
2.7838359   , 2.8351704   , 3.0013452   , 2.9119558   , 3.0637121   , 3.0244331   , 3.0483142   , 2.6696777   , 0,0,
4.3122203   , 3.3947915   , 3.8455152   , 3.6063114   , 3.8093195   , 3.6993168   , 3.8589321   , 3.7974976   , 0,0,
18.2661218  , 20.6751216  , 20.2071692  , 19.7840996  , 20.8550169  , 19.7445114  , 21.8464703  , 19.9576901  , 21.1145496, 17.3136405,
31.8521351  , 32.1056071  , 31.4569133  , 33.5585628  , 33.4976293  , 32.9199345  , 34.5192377  , 30.9693176  , 0,0,
                                                                                                               
-0.3646615  , -0.7015269  , -1.0521103  , -0.9869004  , -1.3282808  , -1.1350298  , -1.2997874  , 0.1445306   , 0,0,
-6.3639273  , -2.2560440  , -3.6118276  , -2.7129518  , -3.4460254  , -2.8461405  , -3.8436300  , -4.3669679  , 0,0,
-72.6688109 , -84.7345674 , -82.5692521 , -80.3368792 , -85.5583001 , -80.0609569 , -90.3178984 , -80.9106367 , -86.5492689, -67.3373583,
-139.7621626, -141.7639879, -137.9828116, -148.2978973, -147.7522606, -145.3929517, -153.4628210, -135.4863203, 0,0,
                                                                                                               
-4.4927678  , -3.9097422  , -3.6278339  , -3.5267896  , -3.2550952  , -3.5072584  , -3.2890919  , -5.0924765  , 0,0,
3.0468355   , -2.5775458  , -1.1109638  , -2.2240108  , -1.3400970  , -2.2771025  , -0.7679543  , 0.4944393   , 0,0,
124.3765137 , 145.8700837 , 142.0725536 , 137.8910348 , 147.2418533 , 137.0166696 , 155.8055400 , 138.4239483 , 148.6803349, 113.5983871,
247.2973363 , 251.4008715 , 243.7056032 , 262.9672394 , 261.4122647 , 257.6215804 , 272.5174835 , 238.9842179 , 0,0,
                                                                                                               
3.0614507   , 2.7715180   , 2.6980880   , 2.6082184   , 2.5403155   , 2.6422817   , 2.5561885   , 3.2911121   , 0,0,
-0.0380758  , 2.4451582   , 1.9000453   , 2.3517477   , 2.0004872   , 2.4387034   , 1.7511548   , 1.0420652   , 0,0,
-99.8245956 , -116.2228018, -113.2672937, -109.8352086, -117.2680789, -108.7219180, -124.0671024, -109.8813741, -118.1999731, -90.4014310,
-199.2762010, -202.6695625, -195.9790039, -211.9598372, -210.2854335, -207.5038204, -219.4631039, -192.1534011, 0,0,
                                                                                                               
0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0,0,
0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0,0,
30.8811046  , 35.4242016  , 34.5749487  , 33.5234898  , 35.7501389  , 33.0494052  , 37.7564026  , 33.4272022  , 35.9698492, 27.8551404,
60.9248751  , 61.9244929  , 59.7966236  , 64.7571103  , 64.1348488  , 63.3561912  , 66.8934979  , 58.6675410  , 0,0,
//--6
1.8265111   , 2.2433367   , 2.2349277   , 2.2642507   , 2.1857717   , 2.2523188   , 2.3105270   , 1.8880534   , 0,0,
3.3847101   , 2.7641773   , 3.0093308   , 2.7871678   , 2.8588775   , 2.9235494   , 3.0701534   , 3.0074057   , 0,0,
18.4363571  , 25.0766341  , 21.1026280  , 20.4173977  , 22.2471624  , 20.2747910  , 21.3799396  , 20.0987488  , 21.3882039, 17.0585434,
34.0421100  , 35.2839371  , 35.8275569  , 35.7760155  , 37.5983759  , 36.2988098  , 37.8892733  , 31.5053533  , 0,0,
                                                                                                               
2.2846913   , 0.7038855   , 0.7502560   , 0.6323821   , 0.8634135   , 0.6530882   , 0.4521526   , 1.9534510   , 0,0,
-3.7919848  , -0.7309980  , -1.4677806  , -0.6992786  , -0.9812421  , -0.9052492  , -1.5975274  , -2.3047058  , 0,0,
-74.8963882 ,-107.1602774 , -87.5678031 , -84.3232682 , -93.0718587 , -83.3662010 , -88.5438982 , -82.3124710 , -88.2313206, -67.7492080,
-151.0573046,-157.4838977 , -159.8756556, -159.2996367, -167.9778306, -162.0467809, -169.8068619, -139.0059765, 0,0,
                                                                                                               
-7.0133922  , -5.0362094  ,-5.0842732   , -4.9353426  , -5.1813321  , -4.9275466  , -4.7448740  , -6.4368069  , 0,0,
0.4952051   , -3.8496087  ,-3.0795342   , -3.9549953  , -3.5981967  , -3.9562426  , -3.0186005  , -1.4529976  , 0,0,
129.5634403 , 187.4129639 ,151.4147296  , 145.7475712 , 161.2866045 , 143.4188976 , 152.7047502 , 141.4152268 , 151.6567442, 115.9634384,
267.6555788 , 279.4014706 ,283.4012702  , 282.1752437 , 297.5793403 , 287.1654733 , 301.3100490 , 245.8501663 , 0,0,
                                                                                                               
3.9124238   , 3.0868581   , 3.1045982   , 3.0443600   , 3.1320518   , 3.0287442   , 2.9794494   , 3.6204681   , 0,0,
0.8651925   , 2.8237690   , 2.5434726   , 2.8748264   , 2.7275845   , 2.9515546   , 2.5524658   , 1.7038230   , 0,0,
-104.3935987, -149.7952558, -120.6036602, -116.1853306, -128.4687543, -113.8157134, -121.3695222, -112.2785363, -120.2620590, -93.0632549,
-215.0564093, -224.4350430, -227.3992691, -226.4052515, -238.5244117, -230.3277539, -241.7234491, -197.6488504, 0,0,
                                                                                                               
0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0,0,
0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0,0,
32.3005544  , 45.4783947  , 36.6768446  , 35.3720846  , 39.0287952  , 34.5149675  , 36.8543275  , 34.0997586  , 36.4699881, 28.8008429,
65.4346435  , 68.2130092  , 69.0283627  , 68.7613548  , 72.3206926  , 69.8954122  , 73.3175543  , 60.2728650  , 0,0,
//--7
2.1672597   ,  2.2111986  , 2.0969472   , 1.9727601   , 2.0950412   , 2.0802178   , 2.1938340   , 2.0745011   ,  0,0,  
3.0262152   ,  2.5258678  , 2.6805508   , 2.6079203   , 2.6444505   , 2.5809804   , 2.7209857   , 2.8860773   ,  0,0,
19.1025589  ,  20.3707559 , 22.2139025  , 20.2637002  , 23.3790183  , 21.4525368  , 22.0165418  , 21.3852381  , 19.8885958, 19.8487602,
40.9699673  ,  40.9724278 , 39.2003194  , 40.0419361  , 41.2214502  , 40.3078570  , 42.3517233  , 39.1760811  ,  0,0,
                                                                                                               
0.6137828   ,  0.4554684  , 0.7506479   , 1.2223815   , 0.7573267   , 0.9019796   , 0.4767314   , 0.8031683   , 0,0,
-2.4994909  , -0.3862226  , -0.8225336  , -0.6104179  , -0.7699749  , -0.4203550  , -0.9177104  , -2.0118681  , 0,0,
-78.3823979 , -83.8050773 , -93.4939131 , -83.7577872 , -98.8398297 , -89.3957390 , -91.9368012 , -88.8319418 , -81.1992245, -82.6354633,
-184.6678629, -184.8191346, -175.9680575, -180.4269057, -186.3704080, -181.4922725, -192.4291872, -175.7111428, 0,0,
                                                                                                               
-4.7996260  , -4.5479081  , -4.7515430  , -5.3765414  , -4.8096431  , -5.0388840  , -4.5512073  , -4.8992315  , 0,0,
-1.1609425  , -3.9665594  , -3.5020812  , -3.7251675  , -3.4914173  , -4.0310388  , -3.4408473  , -1.6844292  , 0,0,
135.9733618 , 143.6994501 , 162.3924044 , 144.1131018 , 171.4135351 , 154.1050984 , 158.4797843 , 152.7526212 , 138.5490086, 144.6688482,
328.5284091 , 328.3032451 , 312.0948649 , 320.8097059 , 331.9748229 , 322.3908873 , 343.6821609 , 311.5354297 , 0,0,
                                                                                                               
3.0065200   ,  2.8656978  , 2.8939115   , 3.1667371   , 2.9405008   , 3.0489867   , 2.8673192   , 3.0159705   , 0,0,
1.6022854   ,  2.8039888  , 2.6354834   , 2.7158476   , 2.6040935   , 2.8651478   , 2.6266201   , 1.7757076   , 0,0,
-109.4913126, -113.5843994, -129.3796602, -114.1434907, -136.0809616, -121.9614033, -125.4121670, -120.6223951, -109.2140104, -117.1329272,
-263.8982115, -263.1511937, -250.1138179, -257.5475909, -266.7826404, -258.6227854, -276.6520752, -249.6897400, 0,0,
                                                                                                               
0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0,0,
0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0,0,
33.8164718  , 34.3278321  , 39.2822758  , 34.5334978  , 41.1370704  , 36.8088963  , 37.8705202  , 36.3288690  , 32.9890993, 36.2675928,
80.1039712  , 79.6921281  , 75.7990204  , 78.1436504  , 80.9775080  , 78.4313424  , 84.0571749  , 75.6966927  , 0,0,
//---8
1.1663461   , 1.4355792   , 1.6429054   , 1.6744965   , 1.5608627   , 1.6040751   , 1.7218362   , 1.5813267   ,  0,0,
2.3060014   , 2.0445524   , 2.3621236   , 2.0710638   , 2.1851203   , 2.0683038   , 2.2929260   , 2.3915320   ,  0,0,
23.1050158  , 22.3758646  , 22.4671022  , 22.6606287  , 23.6399665  , 22.4018472  , 23.6912279  , 22.1800824  , 23.0053373, 22.7936574,
45.5816785  , 44.1057184  , 48.7496044  , 44.4198908  , 48.1589657  , 46.4722556  , 46.9849290  , 46.3963823  ,  0,0,
                                                                                                               
3.5845663   , 2.6750361   , 1.7799442   , 1.6870260   , 2.1016576   , 1.9508232   , 1.4954194   , 1.9539654   , 0,0,
-0.5020670  , 0.8266385   , -0.2656515  , 0.7508844   , 0.3349288   , 0.8351607   , 0.0626920   , -0.8538570  , 0,0,
-99.2316336 , -93.8969352 , -94.1765209 , -95.6122495 , -99.7781386 , -93.7769472 , -100.5742816, -93.0866815 , -96.9744450, -98.4077508,
-206.7042094, -198.6751420, -222.5425246, -200.3363700, -219.5569623, -210.7761043, -213.7361382, -210.8582427, 0,0,
                                                                                                               
-7.9195590  , -6.8017448  , -5.5792727  , -5.5092921  , -6.0161164  , -5.8171781  , -5.2544875  , -5.8916050  , 0,0,
-3.1344057  , -5.0809167  , -3.7583698  , -4.9720863  , -4.4619216  , -5.1316143  , -4.2289987  , -2.6220143  , 0,0,
175.3550579 , 162.0277213 , 162.0147170 , 165.3379917 , 172.1271619 , 161.1154924 , 174.4889010 , 160.5890295 , 167.5790824, 175.0786064,
367.1136136 , 350.6329015 , 396.3656035 , 354.0789014 , 390.8264055 , 373.8214472 , 379.7259241 , 374.8543843 , 0,0,
                                                                                                               
4.1537338   , 3.6649174   , 3.1349003   ,  3.1215904  , 3.3256207   , 3.2457162   , 3.0125650   , 3.3553759   , 0,0,
2.2878543   , 3.1766933   , 2.6402782   ,  3.1283998  , 2.9198247   , 3.2136283   , 2.8515644   , 2.0374609   , 0,0,
-141.9130879, -128.1345183, -127.6548963, -130.7653282, -135.8592249, -126.7546408, -138.4103477, -126.8859200, -132.6062020, -142.6418969,
-293.5952538, -278.8969708, -317.4748380, -282.0457879, -312.9314781, -298.5089818, -303.5162639, -300.0597961, 0,0,
                                                                                                               
0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0,0,
0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0,0,
43.6911530  , 38.6253234  , 38.3511747  , 39.3766844  , 40.8710448  , 38.0137213  , 41.8134735  , 38.2078025  , 40.0000571, 44.1865877,
88.6292760  , 83.8205470  , 95.9117001  , 84.8925544  , 94.5192156  , 89.9959648  , 91.5390850  , 90.6718469  , 0,0,
//--10
3.1879977   ,  3.1237545  , 3.0014329   , 3.0349081   , 2.9998985   , 3.0100438   , 3.0419400   , 3.1435301   , 0,0,
3.3732743   ,  3.6068889  , 3.6436070   , 3.5464266   , 3.0837031   , 3.5121286   , 3.6059266   , 3.3033660   , 0,0,
28.5530246  ,  26.8334668 , 24.8899989  , 24.6309320  , 23.8066472  , 26.0943734  , 26.9985768  , 21.3078330  , 29.2910861, 25.8922364,
51.4504510  ,  44.5033980 , 51.1745149  , 49.6947685  , 48.4405730  , 49.1443806  , 48.4732672  , 42.5857058  , 0,0,
                                                                                                               
-4.5507711  , -4.2914049  , -4.0390819  , -4.0888543  , -4.0171181  , -4.0563104  , -4.1054853  , -4.4988279  , 0,0,
-5.0538034  , -5.4666240  , -5.5136525  , -5.3113170  , -4.2338726  , -5.2175458  , -5.4201807  , -4.8377457  , 0,0,
-125.2810611, -114.2907587, -104.9471315, -103.7002641, -98.9562849 , -111.2855328, -115.7398514, -87.3725861 ,-126.4850740, -112.8721953,
-233.5647676, -197.5753959, -230.8501708, -222.8009726, -216.8881687, -221.1298644, -217.4240916, -189.6399609, 0,0,
                                                                                                               
2.3270852   , 2.1263858   , 1.9957397   , 2.0115527   , 1.9694616   , 2.0068234   , 2.0203264   , 2.3126613   , 0,0,
2.6224863   , 2.8138611   , 2.8351443   , 2.7266306   , 2.1016616   , 2.6721153   , 2.7803641   , 2.4695950   , 0,0,
220.6267268 , 196.1883177 , 179.1248511 , 176.7610960 , 167.3376842 , 191.5766460 , 199.7120606 , 147.0270228 , 218.5312248, 198.8120467,
412.1271360 , 343.3928454 , 405.1093862 , 389.0908942 , 378.8871044 , 388.0628941 , 380.2732008 , 331.1799008 , 0,0,
                                                                                                               
0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0,0,
0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0,0,
-176.2638342, -153.2246506, -139.3823065, -137.2977124, -129.4349427, -150.1015023, -156.7258697, -113.4367020, -171.3104005, -159.1193969,
-326.7929386, -269.4166696, -319.7685020, -305.9753123, -298.1802327, -306.7905224, -299.6263399, -261.1499345, 0,0,
                                                                                                               
0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0,0,
0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0,0,
53.3577745  , 45.4854522  , 41.3030741  , 40.5937524  , 38.2381264  , 44.7114103  , 46.7513402  , 33.4644939  , 50.9643390, 48.2764460,
97.7850863  , 80.0716122  , 95.3345495  , 90.9874421  , 88.7500616  , 91.7114805  , 89.2887980  , 78.0155607  , 0,0,
//---12                    
3.1019572   , 3.1004736   , 3.0501648   , 3.1022444   , 2.9838077   , 2.9520737   , 3.0662604   , 2.8943160   , 0,0,
3.3639077   , 3.6006784   , 3.7121678   , 3.5300328   , 3.4867551   , 3.5731167   , 3.6904498   , 3.2428424   , 0,0,
31.8977548  , 31.3085492  , 25.7472562  , 26.7775686  , 31.2447780  , 26.6698221  , 25.9636717  , 28.0348590  , 31.9676655, 30.5261311,
50.2123774  , 64.8081495  , 68.4346895  , 65.6129856  , 65.8134606  , 67.2469693  , 59.7929451  , 56.9061838  , 0,0,
                                                                                                               
-4.5087102  , -4.3632163  , -4.2943493  , -4.3804280  , -4.1343540  , -4.0884070  , -4.2919321  , -4.1229057  , 0,0,
-5.1725852  , -5.5859419  , -5.8229236  , -5.4321832  , -5.3282524  , -5.5299481  , -5.7452583  , -4.8178143  , 0,0,
-141.2817887, -134.4323889, -107.4651954, -112.8035431, -133.7957792, -111.6336898, -108.5183664, -119.2959798, -137.9156483, -133.9048553,
-222.1821346, -290.9962607, -311.3064925, -295.3536446, -299.1186441, -305.4840458, -267.5419365, -253.9530635, 0,0,
                                                                                                               
2.3581178   , 2.2279776   , 2.2001725   , 2.2405604   , 2.1046069   , 2.0892999   , 2.1873016   , 2.1684388   ,  0,0,
2.7399346   , 2.9462416   , 3.0789227   , 2.8641148   , 2.7954293   , 2.9274540   , 3.0260018   , 2.5003808   ,  0,0,
249.8304740 , 230.0966080 , 181.0911338 , 191.1702446 , 228.3266809 , 188.0836032 , 183.0787986 , 203.6382470 ,  236.5208647, 234.7645252,
382.5827714 , 503.6564854 , 545.4838740 , 512.6639086 , 524.3506707 , 535.1814998 , 462.8431703 , 438.7346216 ,  0,0,
                                                                                                               
0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0,0,
0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0,0,
-200.4342523, -178.6207529, -139.2853258, -147.5100413, -176.8600696, -144.3717573, -141.0373451, -158.0484321,  -183.7957189, -186.8343147,
-296.9098861, -391.1189634, -428.6020819, -399.3601187, -412.2978972, -420.7368723, -359.7853092, -340.8547352, 0,0,
                                                                                                               
0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   ,0.0000000    , 0,0,
0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   ,0.0000000    , 0,0,
60.9863240  , 52.6525934  , 40.9069429  , 43.3649458  , 52.0953319  , 42.2604318  , 41.5166310  ,46.6716476   ,  54.2184454, 56.4381784,
87.2838712  , 114.6330756 , 127.0069603 , 117.4399255 , 122.2756152 , 124.8088360 , 105.6830560 ,100.1473443  , 0,0,
//---14
2.8389445   ,  2.9657788  , 2.9389343   , 2.9874093   , 2.9922812   , 2.9016788   , 2.9705953   , 2.8823485   , 0,0,
3.3658865   ,  3.5076100  , 3.5990450   , 3.3416630   , 3.3880471   , 3.6455992   , 3.6649715   , 3.3147028   , 0,0,
39.6969198  ,  30.9935368 , 26.4129631  , 23.1532594  , 28.0447681  , 29.5111445  , 23.1526266  , 30.5618099  , 29.9902230, 35.4184248,
86.0435507  ,  74.0928503 , 78.4148015  , 83.5675990  , 67.6524319  , 68.1729495  , 94.1398524  , 51.7597525  , 0,0,
                                                                                                               
-4.0172129  , -4.1002038  , -4.1118974  , -4.2002452  , -4.2177371  , -4.0535074  , -4.1538892  , -4.1789216  , 0,0,
-5.2704445  , -5.4332533  , -5.6298394  , -5.0636066  , -5.1750082  , -5.7798445  , -5.7723289  , -5.0693736  , 0,0,
-177.4338840, -131.4949574, -108.4962477, -92.7588751 , -116.0729023, -123.3398754, -92.6059432 , -128.8850873, -126.6972934, -157.1112585,
-386.8013221, -333.6943257, -355.3082562, -376.9000315, -302.4991874, -304.1402535, -428.5405676, -223.5357543, 0,0,
                                                                                                               
2.1239102   , 2.1066811   , 2.1325509   , 2.1851235   , 2.1871087   , 2.1068661   , 2.1575313   , 2.2268143   , 0,0,
2.8352756   , 2.8996020   , 3.0037672   , 2.6832755   , 2.7436433   , 3.1083033   , 3.0887666   , 2.6808406   , 0,0,
311.7099630 , 222.6451790 , 179.5245126 , 150.9808181 , 192.9260488 , 205.6727364 , 150.6220984 , 216.5652297 , 213.9644617, 275.7983014,
664.7726522 , 577.2337959 , 617.8286109 , 651.0543564 , 521.4660429 , 523.1770090 , 744.8009944 , 374.7013311 , 0,0,
                                                                                                               
0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0,0,
0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0,0,
-246.8606790, -171.2532658, -135.7011006, -112.6396425, -146.3294258, -155.6755419, -112.4920337, -165.1943883, -164.4318905, -218.9776155,
-510.9654200, -447.4632363, -481.2372118, -503.4497300, -403.5997600, -404.2390066, -578.4823768, -283.1899273, 0,0,
                                                                                                               
0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0,0,
0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0,0,
73.8918632  , 50.1340193  , 39.2700286  , 32.2768464  , 42.4597434  , 44.8512830  , 32.3381225  , 47.9637031  , 48.1832407, 65.8662133,
147.9311673 , 130.8332693 , 141.3339703 , 146.7496497 , 118.0153344 , 118.0564235 , 169.0933043 , 81.2346162  , 0,0,
//--16
3.4514586   , 3.3454516   , 3.2678229   , 3.6243753   , 3.2646422   , 3.3289799   , 3.3683121   , 3.4178581   , 0,0,
3.8849952   , 3.3387339   , 3.9736190   , 4.0109270   , 4.2597566   , 4.0212208   , 4.0171530   , 3.7535606   , 0,0,
78.2383817  , 13.8737664  , 19.7059070  , 48.9311668  , 53.4850458  , 30.8473091  , 40.5411416  , 16.6692982  , 44.2412401, 25.0707865,
67.6338732  , 62.4101078  , 70.5313428  , 59.4291217  , 84.8817296  , 87.3043170  , 89.7214734  , 68.0084305  , 0,0,
                                                                                                               
-5.5871102  , -5.1507443  , -5.0349718  , -5.8121243  , -4.9675313  , -5.1745793  , -5.2126374  , -5.4993277  , 0,0,
-6.6615482  , -5.2362747  , -6.6427594  , -6.7915544  , -7.3543776  , -6.7824142  , -6.7623076  , -6.2757769  , 0,0,
-361.4143690, -48.7833920 , -75.6736135 , -219.0572532, -238.8838717, -130.6396735, -175.6106452, -59.6784470 , -193.9069471, -104.3022513,
-297.2042552, -274.3349077, -313.1076811, -259.8974202, -380.4016110, -393.3579329, -399.9244692, -298.7164420, 0,0,
                                                                                                               
3.1151157   , 2.8243226   , 2.7682310   , 3.2032477   , 2.7014620   , 2.8386875   , 2.8590496   , 3.0425039   , 0,0,
3.7416246   , 2.9138594   , 3.6847257   , 3.7884719   , 4.0994151   , 3.7783258   , 3.7729012   , 3.4857678   , 0,0,
638.4671489 , 74.3902748  , 120.3213617 , 381.8522470 , 414.1012783 , 221.0756687 , 298.9000694 , 89.3536408  , 332.7619872, 176.7395792,
504.4526513 , 466.5681393 , 535.4050376 , 441.1312596 , 653.5722817 , 679.1427582 , 682.0599439 , 505.9966579 , 0,0,
                                                                                                               
0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0,0,
0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0,0,
-502.9417119, -54.3161985 , -88.9387087 , -299.1472661, -322.4280729, -170.1952332, -229.8976079, -62.7145762 , -257.6585731, -137.7409614,
-385.6266923, -357.4125989, -411.1971544, -337.8708594, -503.1981357, -525.3431567, -520.9105902, -385.6999520, 0,0,
                                                                                                               
0.0000000   , 0.0000000   ,0.0000000    , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0,0,
0.0000000   , 0.0000000   ,0.0000000    , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0,0,
148.6870131 , 15.8802646  ,25.6191207   , 88.4655746  , 94.7815698  , 49.9613347  , 67.1104714  , 17.4006375  , 75.5933381, 41.2463927,
111.7446133 , 103.7913991 ,119.4095725  , 98.2516797  , 146.1956463 , 153.3102360 , 150.0803933 , 111.4125427 , 0,0,
//--20
3.6298544   , 3.3749303   ,3.2860625    , 3.4929563   , 3.6518418   , 3.1082009   , 3.6210215   , 3.6788689   , 0,0,
3.9252678   , 4.1505798   ,4.3572586    , 4.0249701   , 4.4590459   , 4.4551080   , 4.4222033   , 4.6042389   , 0,0,
68.0856766  , 22.2367484  ,24.1698771   , 17.8155033  , 26.2892471  , 19.5382260  , 22.6339412  , 25.3910645  , 27.3524456, 59.1216679,
45.3444646  , 59.9319658  ,56.2424735   , 47.2687518  , 77.6837091  , 58.9689357  , 56.0218250  , 36.3375056  , 0,0,
                                                                                                               
-6.0161168  , -5.2756502  ,-5.1315112   , -5.6041391  , -5.9564355  , -4.7383533  , -5.8286435  , -6.1054290  , 0,0,
-6.8141690  , -7.1952770  ,-7.6508990   , -6.9230523  , -7.9091976  , -7.8817815  , -7.8014543  , -8.3118049  , 0,0,
-307.4560186, -85.9276379 ,-97.3576067  , -66.2211050 , -106.5724423, -72.8608275 , -88.6522563 , -101.2779920, -112.2794916, -260.7702778,
-187.4067962,-259.7310897 ,-240.5905166 , -194.3548546, -338.5073149, -255.8096523, -235.2897245, -143.2298500, 0,0,
                                                                                                               
3.3782152   , 2.9237273   ,2.8577138    , 3.1285457   , 3.3087393   , 2.6292166   , 3.2278879   , 3.4027156   , 0,0,
3.8690148   , 4.0801713   ,4.3193245    , 3.9049288   , 4.4664087   , 4.4518051   , 4.4109235   , 4.6894281   , 0,0,
533.5164233 , 135.5253728 ,159.5919720  , 102.7405530 , 174.3191497 , 111.5236306 , 141.5345486 , 162.8023731 , 185.7568807, 444.7103390,
303.9496339 , 435.4840309 ,399.1721996  , 312.0767222 , 566.3057238 , 430.4316329 , 383.2353182 , 222.6274883 , 0,0,
                                                                                                               
0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0,0,
0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0,0,
-414.0657000, -98.7314427 , -120.4706959, -74.5890869 , -130.6799532, -78.9512324 , -104.2153017, -119.7779829, -140.8787038, -340.6616855,
-224.6541662, -329.0270871, -298.9477873, -227.4396354, -425.2223300, -326.8080855, -282.0217833, -158.3797712, 0,0,
                                                                                                               
0.0000000   , 0.0000000   ,0.0000000    , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0,0,
0.0000000   , 0.0000000   ,0.0000000    , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000   , 0,0,
120.9496820 , 27.9312078  ,35.1027620   , 21.2907121  , 37.6868127  , 21.7879273  , 29.7319490  , 33.8881451  , 41.0772370, 98.6151505,
63.7671956  , 94.3468037  ,85.1483340   , 63.4794744  , 120.7668223 , 94.2579596  , 79.0679498  , 43.6292689  , 0,0,
//---26
4.0727997   , 2.8415715  , 3.4031266   , 3.3052221   , 3.3887938   , 3.6220290  , 3.4939811   ,4.0418955    , 0,0,
4.3867455   , 4.8360564  , 3.8507530   , 4.2015091   , 4.3213554   , 4.2891759  , 4.3694671   ,4.4141105    , 0,0,
50.5038841  , -0.9351704 , 29.6586286  , 22.5651791  , 7.0912448   , 16.4482721 , 27.3824829  ,28.0289189   ,17.9987288, 8.7661350,
13.3804948  , 25.7739751 , 10.8238262  , 44.2689202  , 54.7104707  , 24.3141070 , 69.8943446  ,35.0495066   , 0,0,
                                                                                                             
-7.0400074  , -4.2205983 , -5.4274727  , -5.2318270  , -5.4209598  , -5.9387471 , -5.6876423  , -6.9865686  , 0,0,
-7.9109699  , -8.7620185 , -6.5432212  , -7.3576069  , -7.5848241  , -7.5515692 , -7.7475616  , -7.9673768  , 0,0,
-218.0460519, 23.7590564 , -116.1710543, -85.3732200 , -12.0546554 , -56.3087187, -107.7039326, -110.5892067, -62.8684588, -25.6416062,
-37.2248244 , -91.5312737, -27.1419880 , -173.9219875, -224.3923348, -87.7682121, -293.4484317, -139.9045995, 0,0,
                                                                                                             
3.9447218   , 2.3385036  , 2.9960036   , 2.8918018   , 2.9949746   , 3.2954014  , 3.1695050   , 3.9144526   , 0,0,
4.4887395   , 4.9038039  , 3.6558692   , 4.1121938   , 4.2358018   , 4.2378748  , 4.3545890   , 4.5038138   , 0,0,
365.2144390 , -59.6408627, 179.8364455 , 129.2784771 , 0.3299386   , 77.8506720 , 168.7426598 , 173.0848983 , 88.9819578, 34.3836381,
40.1228385  , 126.9969933, 23.7414601  , 264.9800594 , 355.3976563 , 126.0807393, 473.0267736 , 220.9554428 , 0,0,
                                                                                                             
0.0000000   , 0.0000000  , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000  , 0.0000000   , 0.0000000   , 0,0,
0.0000000   , 0.0000000  , 0.0000000   , 0.0000000   , 0.0000000   , 0.0000000  , 0.0000000   , 0.0000000   , 0,0,
-274.9832679, 55.6838445 , -126.0501066, -88.8470414 , 10.9106032  , -48.6973531, -120.3673926, -122.7913102, -57.6760107, -23.4321329,
-19.0541392 ,-79.4048605 , -6.2303930  , -182.1284684, -253.4443348, -83.0763893, -342.3501209, -159.7784893, 0,0,
                                                                                                             
0.0000000   ,0.0000000   ,0.0000000    , 0.0000000   ,0.0000000    , 0.0000000  , 0.0000000   , 0.0000000   , 0,0,
0.0000000   ,0.0000000   ,0.0000000    , 0.0000000   ,0.0000000    , 0.0000000  , 0.0000000   , 0.0000000   , 0,0,
78.2985830  ,-17.8840602 ,33.7196082   , 23.3617086  ,-5.2935140   , 11.6929965 , 32.9408553  , 33.2542290  , 14.5553509, 6.9144919,
3.7722488   ,19.1049977  ,-0.2233304   , 47.7844730  ,68.7036669   , 21.4313204 , 93.8439734  , 44.6519908  , 0,0,
};
 
//===========================================================
TofCAlignPar::TofCAlignPar(){
  TDVName="TofCAlign2";
  TDVParN=0;
  TDVParN+=TOFCSN::NBARN*TOFCSN::NSIDE*TOFCSN::NPMTM;//dycor
  TDVParN+=2*TOFCSN::NBARN; //birk
  TDVParN+=TOFCSN::NBARN*TOFCSN::NSIDE*TOFCSN::NPMTM;//dyweigt  
  TDVParN+=nansat*TOFCSN::NBARN*TOFCSN::NSIDE;//anode saturation par
  TDVParN+=5*TOFCSN::NBARN;///attuantion Re-Correcion Using Carbon
  TDVParN+=nBetaCh*3*TOFCSN::NBARN;//beta correction par
  TDVParN+=nBetaCh*2*TOFCSN::NBARN;//beta amp correction par  
  TDVParN+=nBetaCh*5*TOFCSN::NBARN;//beta new recor 
  TDVBlock=new float[TDVParN];
  TDVSize=TDVParN*sizeof(float);
  LoadOptPar(1);
}

//===========================================================
TofCAlignPar::TofCAlignPar(float *arr,int brun,int erun){
  TDVName="TofCAlign2";
  TDVParN=0;
  TDVParN+=TOFCSN::NBARN*TOFCSN::NSIDE*TOFCSN::NPMTM;//dycor
  TDVParN+=2*TOFCSN::NBARN; //birk
  TDVParN+=TOFCSN::NBARN*TOFCSN::NSIDE*TOFCSN::NPMTM;//dyweigt  
  TDVParN+=nansat*TOFCSN::NBARN*TOFCSN::NSIDE;//anode saturation par
  TDVParN+=5*TOFCSN::NBARN;///attuantion Re-Correcion Using Carbon
  TDVParN+=nBetaCh*3*TOFCSN::NBARN;
  TDVParN+=nBetaCh*2*TOFCSN::NBARN;//beta amp correction par
  TDVParN+=nBetaCh*5*TOFCSN::NBARN;//beta new recor
  TDVBlock=arr;
  TDVSize=TDVParN*sizeof(float);
  BRun=brun;
  ERun=erun;
  LoadTDVPar();
}
//===========================================================
void  TofCAlignPar::LoadOptPar(int opt){
    int iblock=0;
//----load dynode correction par
    for(int ilay=0;ilay<TOFCSN::SCLRS;ilay++){
      for(int isid=0;isid<TOFCSN::NSIDE;isid++){
        for(int ibar=0;ibar<TOFCSN::NBAR[ilay];ibar++){//N+P
          for(int ipm=0;ipm<TOFCSN::NPMTM;ipm++){
              int id=ilay*1000+ibar*100+isid*10+ipm;
              if(opt==0)dycor[id]=TDVBlock[iblock++];
              else    {
                  dycor[id]=def_dycor[ilay][isid][ibar][ipm];
                  TDVBlock[iblock++]=dycor[id];   
               }
          }
        }
     }
  }
//---load birk const
  for(int ipar=0;ipar<2;ipar++){
     for(int ilay=0;ilay<TOFCSN::SCLRS;ilay++){
       for(int ibar=0;ibar<TOFCSN::NBAR[ilay];ibar++){//N+P
         int id=ilay*1000+ibar*100;
         if(opt==0)birk[ipar][id]=TDVBlock[iblock++];
         else      {
           birk[ipar][id]=def_birk[ipar][ilay][ibar];
           TDVBlock[iblock++]=birk[ipar][id];
        }
      }
    }
  }
//---dynode weight
   for(int ilay=0;ilay<TOFCSN::SCLRS;ilay++){
      for(int isid=0;isid<TOFCSN::NSIDE;isid++){
        for(int ibar=0;ibar<TOFCSN::NBAR[ilay];ibar++){//N+P
          for(int ipm=0;ipm<TOFCSN::NPMTM;ipm++){
            int id=ilay*1000+ibar*100+isid*10+ipm;
            if(opt==0)dypmw[id]=TDVBlock[iblock++];
            else      {
                dypmw[id]=def_dypmw[ilay][isid][ibar][ipm];
                TDVBlock[iblock++]=dypmw[id];
              }
          }
        }
     }
  }
//--anode sat
  for(int ipar=0;ipar<nansat;ipar++){
    for(int ilay=0;ilay<TOFCSN::SCLRS;ilay++){
       for(int isid=0;isid<TOFCSN::NSIDE;isid++){
         for(int ibar=0;ibar<TOFCSN::NBAR[ilay];ibar++){//N+P
             int id=ilay*1000+ibar*100+isid*10;
             if(opt==0)ansat[ipar][id]=TDVBlock[iblock++];
             else      {
                ansat[ipar][id]=def_ansat[ipar][ilay][isid][ibar];
                TDVBlock[iblock++]=ansat[ipar][id];
              }
        }
      }
    }
  }
//--attentuation recor 
   for(int ipar=0;ipar<5;ipar++){
      for(int ilay=0;ilay<TOFCSN::SCLRS;ilay++){
        for(int ibar=0;ibar<TOFCSN::NBAR[ilay];ibar++){
           int id=ilay*1000+ibar*100;
           if(opt==0)reanti[ipar][id]=TDVBlock[iblock++];
           else {
              reanti[ipar][id]=def_reanti[ipar][ilay][ibar];
              TDVBlock[iblock++]=reanti[ipar][id];
           }
        }
      }
   }

//--beta cor par
  for(int ich=0;ich<nBetaCh;ich++){
    for(int ipar=0;ipar<3;ipar++){
      for(int ilay=0;ilay<TOFCSN::SCLRS;ilay++){
        for(int ibar=0;ibar<TOFCSN::NBAR[ilay];ibar++){//N+P
          int id=ilay*1000+ibar*100;
          if(opt==0)betacor[ich][ipar][id]=TDVBlock[iblock++];
          else      {
            betacor[ich][ipar][id]=def_betac[ich][ipar][ilay][ibar];
            TDVBlock[iblock++]=betacor[ich][ipar][id];
           }
        }
      }
    }
  }

//---beta amp cor
  for(int ipar=0;ipar<2;ipar++){
    for(int ich=0;ich<nBetaCh;ich++){
      for(int ilay=0;ilay<TOFCSN::SCLRS;ilay++){
        for(int ibar=0;ibar<TOFCSN::NBAR[ilay];ibar++){//N+P
          int id=ilay*1000+ibar*100;
          if(opt==0)betacor1[ipar][ich][id]=TDVBlock[iblock++];
          else      {
            betacor1[ipar][ich][id]=def_betac1[ipar][ich][ilay][ibar];
            TDVBlock[iblock++]=betacor1[ipar][ich][id];
           }
        }
      }
    }
  }
//--beta cor2 
  for(int ich=0;ich<nBetaCh;ich++){
    for(int ipar=0;ipar<5;ipar++){
      for(int ilay=0;ilay<TOFCSN::SCLRS;ilay++){
        for(int ibar=0;ibar<TOFCSN::NBAR[ilay];ibar++){//N+P
        int id=ilay*1000+ibar*100;
        if(opt==0)betacorn[ich][ipar][id]=TDVBlock[iblock++];
        else {
          betacorn[ich][ipar][id]=def_betacn[ich][ipar][ilay][ibar];
          TDVBlock[iblock++]=betacorn[ich][ipar][id];
         }
       }
      }
    }
  }
 

  Isload=1;
}

//==========================================================
void TofCAlignPar::LoadTDVPar(){
  return LoadOptPar();
}

//==========================================================
int  TofCAlignPar::LoadFromFile(char *file){
   ifstream vlfile(file,ios::in);
   if(!vlfile){
    cerr <<"<---- Error: missing "<<file<<"--file !!: "<<endl;
    return -1;
   }
//---load
   int ib=0;
   for(int i=0;i<TDVParN;i++){
     vlfile>>TDVBlock[ib++];
   }
   LoadTDVPar();
   vlfile.close();
   return 0;
}

//==========================================================
void TofCAlignPar::PrintTDV(){
  cout<<"<<----Print "<<TDVName<<endl;
  for(int i=0;i<TDVParN;i++){cout<<TDVBlock[i]<<" ";}
  cout<<'\n';
  cout<<"<<----end of Print "<<TDVName<<endl;
}

// **************************************************************
// Tof Charge+EDep Reconstruction Par for Ion Additional
// **************************************************************
TofCAlignIonPar* TofCAlignIonPar::Head=0;

TofCAlignIonPar* TofCAlignIonPar::GetHead(){
  if(!Head)Head = new TofCAlignIonPar();
  return Head;
}

//=======================================================
const int TofCAlignIonPar::RigCh[TofCAlignIonPar::nRigCh]={
   3,4,5,6,7,8
};

//=======================================================
TofCAlignIonPar::TofCAlignIonPar(){
   TDVName="TofCAlignIon";
   TDVParN=0;
   TDVParN+=nRigCh*7*TOFCSN::NBARN;//Anode
   TDVParN+=nRigCh*7*TOFCSN::NBARN;//Dynode
   TDVBlock=new float[TDVParN];
   TDVSize=TDVParN*sizeof(float);
   LoadOptPar(1);
}

//=======================================================
TofCAlignIonPar::TofCAlignIonPar(float *arr,int brun,int erun){
   TDVName="TofCAlignIon";
   TDVParN=0;
   TDVParN+=nRigCh*7*TOFCSN::NBARN;//Anode
   TDVParN+=nRigCh*7*TOFCSN::NBARN;//Dynode
   TDVBlock=arr;
   TDVSize=TDVParN*sizeof(float);
   BRun=brun;
   ERun=erun;
   LoadTDVPar();
}

//===========================================================
void  TofCAlignIonPar::LoadOptPar(int opt){
  int iblock=0;
//----load dynode correction par
 for(int ich=0;ich<nRigCh;ich++){
    for(int ilay=0;ilay<TOFCSN::SCLRS;ilay++){
     for(int ibar=0;ibar<TOFCSN::NBAR[ilay];ibar++){//N+P
       for(int ipar=0;ipar<7;ipar++){
          int id=ilay*1000+ibar*100;
          if(opt==0)rigcor[0][ich][ipar][id]=TDVBlock[iblock++];
          else      {
            TDVBlock[iblock++]=rigcor[0][ich][ipar][id];
           }
        }
      }
    }
  }

//----load anode correction par
  for(int ich=0;ich<nRigCh;ich++){
    for(int ilay=0;ilay<TOFCSN::SCLRS;ilay++){
     for(int ibar=0;ibar<TOFCSN::NBAR[ilay];ibar++){//N+P
       for(int ipar=0;ipar<7;ipar++){
          int id=ilay*1000+ibar*100;
          if(opt==0)rigcor[1][ich][ipar][id]=TDVBlock[iblock++];
          else      {
            TDVBlock[iblock++]=rigcor[1][ich][ipar][id];
           }
        }
      }
    }
  }
 
   Isload=1;
}
//==========================================================
void TofCAlignIonPar::LoadTDVPar(){
  return LoadOptPar();
}

//==========================================================
int  TofCAlignIonPar::LoadFromFile(char *file,char *file1){
   ifstream vlfile(file,ios::in);
   if(!vlfile){
    cerr <<"<---- Error: missing "<<file<<"--file !!: "<<endl;
    return -1;
   }
//---load
   int ib=0;
   for(int i=0;i<TDVParN/2;i++){
     vlfile>>TDVBlock[ib++];
   }
   vlfile.close();
//---load2
   vlfile.open(file1,ios::in); 
   if(!vlfile){
    cerr <<"<---- Error: missing "<<file1<<"--file !!: "<<endl;
    return -1;
   }
   for(int i=0;i<TDVParN/2;i++){
     vlfile>>TDVBlock[ib++];
   }
   LoadTDVPar();
   vlfile.close();
   return 0;
}

//==========================================================
void TofCAlignIonPar::PrintTDV(){
  cout<<"<<----Print "<<TDVName<<endl;
  for(int i=0;i<TDVParN;i++){cout<<TDVBlock[i]<<" ";}
  cout<<'\n';
  cout<<"<<----end of Print "<<TDVName<<endl;
}


// **************************************************************
// Tof PDF-TDV-Par
// **************************************************************
TofPDFPar* TofPDFPar::Head=0;

TofPDFPar* TofPDFPar::GetHead(){
  if(!Head)Head = new TofPDFPar();
  return Head;
}

//=======================================================
TofPDFPar::TofPDFPar(){
  TDVName="TofPDFAlign";
  TDVParN=0;
  TDVParN+=2*nPDFCh*nPDFVel*nPDFPar*TOFCSN::NBARN;//Anode+Dynode PDF Par
  TDVBlock=new float[TDVParN];
  TDVSize=TDVParN*sizeof(float);
//---Load
  for(int iblock=0;iblock<TDVParN;iblock++)TDVBlock[iblock++]=0;
  LoadOptPar(0);
  Isload=0;
};

//=======================================================
void   TofPDFPar::LoadOptPar(int opt){

   int iblock=0;
//--Anode PDF Par
   for (int ich=0;ich<nPDFCh;ich++){//Charge 
     for(int iv=0;iv<nPDFVel;iv++){//Vel
       for(int ipar=0;ipar<nPDFPar;ipar++){ //Par
        for(int ilay=0;ilay<TOFCSN::SCLRS;ilay++){
          for(int ibar=0;ibar<TOFCSN::NBAR[ilay];ibar++){//N+P
///----
           if(opt==0)pdfpara[ich][iv][ilay][ibar][ipar]=TDVBlock[iblock++];
           else      TDVBlock[iblock++]=pdfpara[ich][iv][ilay][ibar][ipar];
///----
         }
        }
      }
    }
  }
//--Dyndoe PDF Par
   for (int ich=0;ich<nPDFCh;ich++){//Charge 
     for(int iv=0;iv<nPDFVel;iv++){//Vel
       for(int ipar=0;ipar<nPDFPar;ipar++){ //Par
        for(int ilay=0;ilay<TOFCSN::SCLRS;ilay++){
          for(int ibar=0;ibar<TOFCSN::NBAR[ilay];ibar++){//N+P
            if(opt==0)pdfpard[ich][iv][ilay][ibar][ipar]=TDVBlock[iblock++];
            else      TDVBlock[iblock++]=pdfpard[ich][iv][ilay][ibar][ipar];
         }
        }
      }
    }
  }

  Isload=1;
}

//=======================================================
void   TofPDFPar::LoadTDVPar(){
   return LoadOptPar(0);
}

//=======================================================
int  TofPDFPar::LoadFromFile(const char *fpdf,int ida,int ichl,int ichh,int nv,int npar){//ida Dnode0 Anode1

//--PDFPar
   ifstream vlfile(fpdf,ios::in);
   if(!vlfile){
    cerr <<"<---- Error: missing "<<fpdf<<"--file !!: "<<endl;
    return -1;
   }

   for(int ich=ichl;ich<=ichh;ich++){
     for(int iv=0;iv<nv;iv++){
       for(int ipar=0;ipar<npar;ipar++){
         for(int ilay=0;ilay<TOFCSN::SCLRS;ilay++){
           for(int ibar=0;ibar<TOFCSN::NBAR[ilay];ibar++){
              if(ida==1)vlfile>>pdfpara[ich][iv][ilay][ibar][ipar];
              else      vlfile>>pdfpard[ich][iv][ilay][ibar][ipar];
           }
         }
       }
     }
   }
   vlfile.close();

   return 0;
}

//==========================================================
void TofPDFPar::PrintTDV(){
 cout<<"<<----Print TofPDFAlign"<<endl;
 for(int i=0;i<TDVParN;i++){cout<<TDVBlock[i]<<" ";}
 cout<<'\n';
 cout<<"<<----end of Print TofPDFAlign"<<endl;
}

//==========================================================
const int TofPDFPar::ZPDFgate[ZType]={2,14,1000};

//==========================================================
const int TofPDFPar::PDFCh[nPDFCh]={
 1,2,3,4,5,6,7,8,9,10,11,12,13,14,16,18,20,22,26,
};

//=======================================================
const float TofPDFPar::pdfvel[nPDFCh][nPDFVel]={
//Z=1
 0.975, 0.929, 0.878, 0.826, 0.753, 0.570,//>0.95 0.9~0.95 0.85~0.9 0.8~0.85 0.7~0.8 <0.7
//Z=2
 0.976, 0.928, 0.877, 0.827, 0.756, 0.615,
//Z=3
 0.975, 0.927, 0.876, 0.826, 0.756, 0.630,
//Z=4
 0.975, 0.927, 0.876, 0.826, 0.758, 0.633,
//Z=5
 0.975, 0.927, 0.876, 0.826, 0.756, 0.625,
//Z=6
 0.975, 0.927, 0.876, 0.826, 0.756, 0.625,
//Z=7
 0.973, 0.926, 0.876, 0.826, 0.754, 0.630,
//Z=8
 0.972, 0.926, 0.876, 0.825, 0.753, 0.633,
//Z=9 (One Bin)
  1., 2, 2, 2, 2, 2,
//Z=10
  0.972, 0.926, 0.876, 0.826, 0.730,  2., //>0.95 0.9~0.95 0.85~0.9 0.8~0.85 <0.8
//Z=11
  1,  2, 2, 2, 2, 2,
//Z=12 
  0.972, 0.926, 0.876, 0.826, 0.728, 2.,
//Z=13
  1,  2, 2, 2, 2, 2,
//Z=14
  0.972, 0.926, 0.854, 0.726, 2., 2, //>0.95 0.9~0.95 0.8~0.9 <0.8
//Z=16
  1,  2, 2, 2, 2, 2,
//Z=18
  1,  2, 2, 2, 2, 2,
//Z=20
  1,  2, 2, 2, 2, 2,
//Z=22
  1,  2, 2, 2, 2, 2,
//Z=26
  0.974, 0.927, 0.850, 2., 2, 2., //>0.95 0.9~0.95 <0.9
};

//==========================================================
const float TofPDFPar::DAgate[TOFCSN::SCLRS][TOFCSN::SCMXBR]={
//--Gaus
 36.1924508, 25.,        59.1081215,     16.,        36.2870294, 64.,        36.,       36.3128519,  0., 0.,
 15.3553412, 25.,        25.,            36.4819246, 40.9775920, 27.8336000, 25.,       25.,         0., 0.,
 36.,        25.,        25.,            20.1030702, 25.,        34.3711522, 33.,       30.,36.9542741, 38.2348669,
 36.,        25.,        22.,            25.,        25.,        22.,        25.,       25.,         0., 0.,
};

//==========================================================


// **************************************************************
// Tof BetaH Reconstruction Par
// **************************************************************
int   TofRecPar::iLay=0;
int   TofRecPar::iBar=0;
int   TofRecPar::iSide=0;
int   TofRecPar::Idsoft=0;
const float TofRecPar::FTgate[2]={40,640};
const float TofRecPar::FTgate2[2]={40,640};
const float TofRecPar::LHgate[2]={3,11};
const float TofRecPar::BetaHReg[2]={3,3};
const int   TofRecPar::BetaHMinL[2]={2,2};

float TofRecPar::TimeSigma[MaxCharge]={
 0.154481, 0.0773964, 0.0586908, 0.0499433, 0.0476004, 0.0478878,
};

float TofRecPar::CooSigma[MaxCharge][TOFCSN::SCLRS][TOFCSN::SCMXBR]={
3.25507, 2.8842,  2.66315, 2.63824, 2.7531,  2.75176, 2.58442, 3.05875, 0, 0,
3.83415, 2.60723, 2.6913,  2.6913,  2.82717, 3.72971, 2.60723, 3.83415, 0, 0,
2.82826, 2.46584, 2.37538, 2.68855, 2.3448,  2.46346, 2.49862, 2.24448, 2.46584, 3.20122,
2.88308, 2.46085, 2.28233, 2.24353, 2.19463, 2.18898, 2.4568,  2.88308, 0, 0,
//---
1.45753, 1.58077, 1.33174, 1.42192, 1.35561, 1.39362, 1.37249, 1.55297, 0, 0,
2.05656, 1.36711, 1.41875, 1.50639, 1.46159, 1.81865, 1.45122, 1.91586, 0, 0,
1.50083, 1.25307, 1.18671, 1.34842, 1.18093, 1.28697, 1.27937, 1.18699, 1.26868, 1.66538,
1.44617, 1.2738,  1.15831, 1.18266, 1.14706, 1.15023, 1.22711, 1.6046,  0, 0,
//---
1.2329,  1.35222, 1.15642, 1.18622, 1.16908, 1.15795, 1.13978, 1.26498, 0, 0,
1.56636, 1.14664, 1.17725, 1.19131, 1.19126, 1.38302, 1.19158, 1.47778, 0, 0,
1.1791,  0.95298, 0.905048,0.988531,0.901161,0.976683,0.966865,0.894139, 0.967044, 1.24895,
1.14533, 0.975832,0.90301, 0.909189,0.887032,0.895167,0.924072,1.27119, 0, 0,
//---
1.26565, 1.27029, 1.1532,  1.16409, 1.16504, 1.11047, 1.09136, 1.16111, 0, 0,
1.40672, 1.11285, 1.1123,  1.11708, 1.10369, 1.21967, 1.09768, 1.34836, 0, 0,
1.04596, 0.857,   0.813815,0.838709,0.802042,0.838531,0.848379,0.784046,0.89223, 1.04039,
1.05691, 0.849945,0.823333,0.807377,0.785874,0.824712,0.872404,1.14996, 0, 0,
//---
1.29522, 1.28288,  1.1612,   1.15142,  1.2033,  1.16542,  1.12234,  1.1589,   0, 0,
1.35438, 1.10934,  1.08271,  1.09266,  1.09041, 1.20029,  1.07862,  1.28574,  0, 0,
1.03885, 0.880019, 0.844713, 0.859569, 0.843528,0.842734, 0.842961, 0.808523, 0.882262, 0.985495,
1.02509, 0.845895, 0.830401, 0.767124, 0.767147,0.780407, 0.859888, 1.07066,  0, 0,
//--
1.43736, 1.33237,  1.18379,  1.17217,  1.23553,  1.19975,  1.1741,   1.28247,  0, 0,
1.32117, 1.06011,  1.05362,  1.12995,  1.05163,  1.19808,  1.0665,   1.29463,  0, 0,
1.13876, 0.993655, 0.943758, 0.928445, 0.940307, 0.926447, 0.933092, 0.907828, 0.937873, 0.98154,
0.969542,0.792691, 0.822776, 0.755908, 0.730854, 0.755261, 0.837725, 1.04195,  0, 0,
};

//==========================================================
float TofRecPar::GetTimeSigma(int id,int icha){
  IdCovert(id);
  return (icha<=MaxCharge)?TimeSigma[icha-1]:TimeSigma[MaxCharge-1];
}
//==========================================================
float TofRecPar::GetCooSigma(int id,int icha){
   IdCovert(id);
   return (icha<=MaxCharge)?CooSigma[icha-1][iLay][iBar]:CooSigma[MaxCharge-1][iLay][iBar];
}

//==========================================================
TofAlignManager *TofAlignManager::Head=0;

TofAlignManager *TofAlignManager::GetHead(int real){
    if(!Head){Head = new TofAlignManager(real);}
    else if(real!=Head->isreal){
        delete Head;
        Head = new TofAlignManager(real);
     }
    return Head;
}
//==========================================================
TofAlignManager::TofAlignManager(int real){
//---validate time
    isreal=real;
    ntime=0;
    time_t brun=1;
    time_t erun=0;
    tm begin;
    tm end;
    gmtime_r(&brun, &begin);
    gmtime_r(&erun, &end);
    AMSTimeID::CType server=AMSTimeID::Standalone;

//----Insert Tof TAlign
    TofTAlignPar *TofTAlign=TofTAlignPar::GetHead();
    AMSTimeID *tdv=new AMSTimeID(AMSID(TofTAlign->TDVName,isreal),begin,end,
                              TofTAlign->TDVSize,
                              TofTAlign->TDVBlock,
                              server,1,TofTAlignPar::HeadLoadTDVPar);
    tdvmap.insert(pair<string,AMSTimeID*>(TofTAlign->TDVName,tdv));
//---Tdc correction //Only For Real Data
    if(real==1){
      integer TOFCRSL=TOF2GC::SCCRAT*(TOF2GC::SCFETA-1);
      tdv= new AMSTimeID(AMSID("TofTdcCor",isreal),begin,end,
                           TOFCRSL*sizeof(TofTdcCorN::tdccor[0][0]),
                           (void*)&TofTdcCorN::tdccor[0][0],
                           server,1);
      tdvmap.insert(pair<string,AMSTimeID*>("TofTdcCor",tdv));
    }
//---Attunation correction
     TofAttAlignPar *TofAttAlign=TofAttAlignPar::GetHead();
     tdv= new AMSTimeID(AMSID(TofAttAlign->TDVName,isreal),begin,end,
                           TofAttAlign->TDVSize,
                           TofAttAlign->TDVBlock,
                           server,1,TofAttAlignPar::HeadLoadTDVPar);
     tdvmap.insert(pair<string,AMSTimeID*>(TofAttAlign->TDVName,tdv));
//---Anode Gain
     TofPMAlignPar *TofPMAlign=TofPMAlignPar::GetHead();
     tdv= new AMSTimeID(AMSID(TofPMAlign->TDVName,isreal),begin,end,
                           TofPMAlign->TDVSize,
                           TofPMAlign->TDVBlock,
                           server,1,TofPMAlignPar::HeadLoadTDVPar);
     tdvmap.insert(pair<string,AMSTimeID*>(TofPMAlign->TDVName,tdv));
//---Dynode Gain
     TofPMDAlignPar *TofPMDAlign=TofPMDAlignPar::GetHead();
     tdv= new AMSTimeID(AMSID(TofPMDAlign->TDVName,isreal),begin,end,
                           TofPMDAlign->TDVSize,
                           TofPMDAlign->TDVBlock,
                           server,1,TofPMDAlignPar::HeadLoadTDVPar);
     tdvmap.insert(pair<string,AMSTimeID*>(TofPMDAlign->TDVName,tdv));
//---Charge Par
     TofCAlignPar *TofCAlign=TofCAlignPar::GetHead();

/*     tdv= new AMSTimeID(AMSID(TofCAlign->TDVName,isreal),begin,end,
                           TofCAlign->TDVSize,
                           TofCAlign->TDVBlock,
                           server,1,TofCAlignPar::HeadLoadTDVPar);
     tdvmap.insert(pair<string,AMSTimeID*>(TofCAlign->TDVName,tdv));
  */
//---Charge Ion Par
    TofCAlignIonPar *TofCAlignIon=TofCAlignIonPar::GetHead();
    tdv= new AMSTimeID(AMSID(TofCAlignIon->TDVName,isreal),begin,end,
                           TofCAlignIon->TDVSize,
                           TofCAlignIon->TDVBlock,
                           server,1,TofCAlignIonPar::HeadLoadTDVPar);
    tdvmap.insert(pair<string,AMSTimeID*>(TofCAlignIon->TDVName,tdv));

//---Charge PDF Par
    TofPDFPar *TofPDFAlign=TofPDFPar::GetHead();
    tdv= new AMSTimeID(AMSID(TofPDFAlign->TDVName,isreal),begin,end,
                           TofPDFAlign->TDVSize,
                           TofPDFAlign->TDVBlock,
                           server,1,TofPDFPar::HeadLoadTDVPar);
    tdvmap.insert(pair<string,AMSTimeID*>(TofPDFAlign->TDVName,tdv));

//---TDC Linear New Calib
    if(real==1){
      TofTdcPar *TdcPar=TofTdcPar::GetHead(); 
      tdv= new AMSTimeID(AMSID(TdcPar->TDVName,isreal),begin,end,
                            TdcPar->TDVSize,
                            TdcPar->TDVBlock,
                            server,1,TofTdcPar::HeadLoadTDVPar);
      tdvmap.insert(pair<string,AMSTimeID*>(TdcPar->TDVName,tdv));
    }
 

}

//==========================================================
int  TofAlignManager::Validate(unsigned int time){

  static int tdvstat=-1;
  if(time==ntime){return tdvstat;}
  else {
    tdvstat=0;
    time_t vtime=time;
    for(map<string,AMSTimeID*>::iterator it=tdvmap.begin();it!=tdvmap.end();it++)
        if(!it->second->validate(vtime))tdvstat=-1;
    ntime=time;    
    return tdvstat;
  }
}

//==========================================================
void  TofAlignManager::Clear(){
   for(map<string,AMSTimeID*>::iterator it=tdvmap.begin();it!=tdvmap.end();it++){
     delete it->second;
   }
   tdvmap.clear();
}

//=========================================================



