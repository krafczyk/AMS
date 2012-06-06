//Athor Qi Yan 2012/01/05 new Tof database
// ------------------------------------------------------------
//      History
//        Modified:  Adding phseamp  2012/05/16
//        Modified:  DB update       2010/05/28
// -----------------------------------------------------------

#include "typedefs.h"
#include "cern.h"
#include "commons.h"
#include "amsdbc.h"
#include "extC.h"
#include <string.h>
#include <stdlib.h>
#include "tofanticonst.h"
#include "Tofdbc.h"
#include "tofdbc02.h"
#include <math.h>
#include "root.h"
#include "ntuple.h"
#include "event.h"
#include "amsstl.h"
#include "commons.h"
#include "tofsim02.h"
#include "mccluster.h"
#include "mceventg.h"
#include "tofid.h"
#include "job.h"
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
geant TOFPMT::phseamp(){
  //--SE Amplitude part
 geant semean = 2.8421*TFMCFFKEY.seamref;//amp Mean value~ 5mV*ref
 geant seres=semean*TFMCFFKEY.seamres;//amp RMS value 0.9*mean
 geant selref =     TFMCFFKEY.selamref;//low amp part Mean amp 0.2
 geant selpec =     TFMCFFKEY.selampec;//low amp radio 0.2
 geant am=-1;

//---SE Amp-Sample
 do{
   am=semean+seres*rnormx();
  }while(am<=0);

  if(RNDM(-1)<selpec)am=am*selref;//low amp part

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
