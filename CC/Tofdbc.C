//  $Id: Tofdbc.C,v 1.20 2012/09/24 22:13:33 qyan Exp $

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
#include <string.h>
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
bool TOFGeom::IsInSideBar(int ilay,int ibar,float x,float y,float z){
   AMSPoint rcoo(x,y,z);
   AMSPoint coo=GToLCoo(ilay,ibar,rcoo);
   bool lok=0,sok=0,zok=1;
   if(fabs(coo[Proj[ilay]])<Sci_l[ilay][ibar]/2.)lok=1;
   if(fabs(coo[1-Proj[ilay]])<Sci_w[ilay][ibar]/2.)sok=1;
   if(z!=0){zok=(fabs(coo[2])<Sci_t[ilay][ibar]/2.)?1:0;}
   return (lok&&sok&&zok);
}
//--To Judge whether Inside TOF Layer
bool TOFGeom::IsInSideTOF(int ilay,float x,float y,float z){
  bool inside=0;
  for(int ibar=0;ibar<Nbar[ilay];ibar++){if(IsInSideBar(ilay,ibar,x,y,z)){inside=1;break;}}
  return inside;
}
//--Find Nearest Counter
int TOFGeom::FindNearBar(int ilay,float x,float y,float &dis,bool &isinbar,float z){
    float mindis=10000000;int minbar=-1;
    AMSPoint gcoo(x,y,z),lcoo;
    for(int ibar=0;ibar<Nbar[ilay];ibar++){
      lcoo=GToLCoo(ilay,ibar,gcoo);
      if(z!=0){ if(fabs(lcoo[1-Proj[ilay]])<mindis){dis=lcoo[1-Proj[ilay]],mindis=fabs(dis);minbar=ibar;} }
      else    { if(lcoo.norm()<mindis){dis=lcoo.norm(),mindis=dis,minbar=ibar;}}
    }
    isinbar=IsInSideBar(ilay,minbar,x,y,z);
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
 
  int pulnbc=int((pulset[pulnb-1]- pulset[0])/TOF2DBc::fladctb())+1;
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

//===========================================================
TofCAlignPar::TofCAlignPar(){
  TDVName="TofCAlign";
  TDVParN=0;
  TDVParN+=TOFCSN::NBARN*TOFCSN::NSIDE*TOFCSN::NPMTM;//dycor
  TDVParN+=2*TOFCSN::NBARN; //birk
  TDVParN+=TOFCSN::NBARN*TOFCSN::NSIDE*TOFCSN::NPMTM;//dyweigt  
  TDVParN+=nansat*TOFCSN::NBARN*TOFCSN::NSIDE;//anode saturation par
  TDVBlock=new float[TDVParN];
  TDVSize=TDVParN*sizeof(float);
  LoadOptPar(1);
}

//===========================================================
TofCAlignPar::TofCAlignPar(float *arr,int brun,int erun){
  TDVName="TofCAlign";
  TDVParN=0;
  TDVParN+=TOFCSN::NBARN*TOFCSN::NSIDE*TOFCSN::NPMTM;//dycor
  TDVParN+=2*TOFCSN::NBARN; //birk
  TDVParN+=TOFCSN::NBARN*TOFCSN::NSIDE*TOFCSN::NPMTM;//dyweigt  
  TDVParN+=nansat*TOFCSN::NBARN*TOFCSN::NSIDE;//anode saturation par
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
//---
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
//---Tdc correction
    integer TOFCRSL=TOF2GC::SCCRAT*(TOF2GC::SCFETA-1);
    tdv= new AMSTimeID(AMSID("TofTdcCor",isreal),begin,end,
                           TOFCRSL*sizeof(TofTdcCorN::tdccor[0][0]),
                           (void*)&TofTdcCorN::tdccor[0][0],
                           server,1);
     tdvmap.insert(pair<string,AMSTimeID*>("TofTdcCor",tdv));
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

}

//==========================================================
int  TofAlignManager::Validate(unsigned int time){
  int tdvstat=0;
  time_t vtime=time;
  for(map<string,AMSTimeID*>::iterator it=tdvmap.begin();it!=tdvmap.end();it++)
        if(!it->second->validate(vtime))tdvstat=-1;
  return tdvstat;
}

//==========================================================
void  TofAlignManager::Clear(){
   for(map<string,AMSTimeID*>::iterator it=tdvmap.begin();it!=tdvmap.end();it++){
     delete it->second;
   }
   tdvmap.clear();
}

//=========================================================



