// Author Choumilov.E. 10.07.96.
#include <iostream.h>
#include <stdio.h>
#include <typedefs.h>
#include <cern.h>
#include <extC.h>
#include <math.h>
#include <point.h>
#include <amsgobj.h>
#include <event.h>
#include <amsstl.h>
#include <commons.h>
#include <tofdbc.h>
#include <tofsim.h>
#include <mccluster.h>
//
extern AMSTOFScan scmcscan[SCBTPN];
//
float AMSDistr::getrand(const float &rnd)
{
  float val;
  integer i=AMSbins(distr,rnd,nbin);
  if(i==0){val=bnl+bnw*rnd/distr[i];}
  else if(i>0){val=bnl+bnw*i;}
  else
  {
    i=-i;
    val=bnl+bnw*i+bnw*(rnd-distr[i-1])/(distr[i]-distr[i-1]);
  }
  return val;
}
//------------------------------------------------------------------------------
//  <--- for inp. X define position in scan array (i1/i2) and correction ratio R.
//
void AMSTOFScan::getxbin(const float x, integer &i1, integer &i2, float &r)
{
  integer i=AMSbins(scanp,x,SCANPNT);
  if(i==0){
    i1=0;
    i2=1;
    r=(x-scanp[i1])/(scanp[i2]-scanp[i1]);
  }
  else if(i<0){
    if(i==-SCANPNT){
      i1=SCANPNT-2;
      i2=SCANPNT-1;
      r=(x-scanp[i1])/(scanp[i2]-scanp[i1]);
      }
    else{
      i1=-i-1;
      i2=-i;
      r=(x-scanp[i1])/(scanp[i2]-scanp[i1]);
    }
  }
  else{
    i1=i-1;
    i2=i1;
    r=0.;
  }
}
//------------------------------------------------------------------
// <--- functions to get efficiency ( getxbin should be called first!!!)
float AMSTOFScan::getef1(const float r, const int i1, const int i2)
{
  if(i1==i2){return eff1[i1];}
  else{return (eff1[i1]+(eff1[i2]-eff1[i1])*r);}
}
float AMSTOFScan::getef2(const float r, const int i1, const int i2)
{
  if(i1==i2){return eff2[i1];}
  else{return (eff2[i1]+(eff2[i2]-eff2[i1])*r);}
}
//===================================================================
//
void AMSTOFTovt::build()
{
//      <--- prepare PM single electron pulse height distribution:
  static float arr[19]={5.,20.,39.7,46.5,52.3,54.6,52.7,47.6,40.7,
                          32.5,23.9,16.7,11.,7.,4.5,3.,1.5,1.,0.5};
  static AMSDistr scpmsesp(19,0.,0.909,arr);// p/h spectrum ready
//                   ( scale in mV@50ohm to have 5mV as m.p. value!!!)
//
  integer i1,i2,id,idN,idd,ibar,ilay,ibtyp;
  static float tslice1[SCTBMX+1]; //  flash ADC array for side-1
  static float tslice2[SCTBMX+1]; //  flash ADC array for side-2
  int i,ii,j,jj,k,stat(0),nelec,ierr(0);
  integer nhitl[SCLRS];
  float edepl[SCLRS],edepb;
  number dummy(-1);
  float time,x,y,z,am,r,rnd,eff,de,tm,eps(0.002);
  float vl,vh;
  float nel0,nel,nphot;
  AMSPoint cloc(999.,999.,999.);
  AMSgvolume *p;
  AMSTOFMCCluster *ptr;
  AMSTOFMCCluster *ptrN;
//
  static integer first=0;
  static integer npshbn;
  static float pmplsh[200];  // PM s.e. pulse shape
  static float fadcb,ifadcb,trts,tmax,zlmx,convr;
//
  if(first++==0){
    AMSTOFTovt::inipsh(npshbn,pmplsh); // prepare PM s.e. pulse shape arr.
    HBOOK1(1111,"Single electron spectrum,mV",50,0.,25.,0.);
    for(i=0;i<1000;i++){
      rnd=RNDM(dummy);
      am=scpmsesp.getrand(rnd);//amplitude from single elect. spectrum
      HF1(1111,am,1.);
    }
    HPRINT(1111);
    HDELET(1111);
    for(i=0;i<SCTBMX+1;i++)tslice1[i]=0.;// clear flash ADC arrays
    for(i=0;i<SCTBMX+1;i++)tslice2[i]=0.;
    fadcb=TOFDBc::fladctb();
    ifadcb=1./fadcb;
    trts=TOFDBc::trtspr();
    tmax=SCTBMX*fadcb;
    zlmx=TOFDBc::plnstr(6)/2.+eps;
    convr=1000.*TOFDBc::edep2ph()*TOFDBc::pmqeff();
  }
//
  ptr=(AMSTOFMCCluster*)AMSEvent::gethead()->
                                      getheadC("AMSTOFMCCluster",0,1);
//                                        ( sort by idsoft done)
//
  for(i=0;i<SCLRS;i++){
    nhitl[i]=0;
    edepl[i]=0.;
  }
//
  edepb=0.;// Edep in given bar
  while(ptr){//       <------------- loop over geant hits
    id=ptr->idsoft; 
    ilay=id/100-1;
    nhitl[ilay]+=1;
    ibar=id%100-1;
    de  =ptr->edep;
    edepl[ilay]+=de;
    edepb+=de;
    time=(1.e+9)*(ptr->tof); // conv. to "ns"
    x=ptr->xcoo[0];
    y=ptr->xcoo[1];
    z=ptr->xcoo[2];
//       cerr<<"TOFPhel::Geant hits:"<<'\n'; // tempor
//       cerr<<"id/il/ib= "<<id<<" "<<ilay<<" "<<ibar<<'\n';
//       cerr<<"de/time= "<<de<<" "<<time<<'\n';
//       cerr<<"Glob.coord= "<<x<<" "<<y<<" "<<z<<'\n';
    AMSPoint cglo(x,y,z);
    p=AMSJob::gethead()->
           getgeomvolume(AMSID("TOFS",id));// pointer to volume "TOFS",id
    assert(p != NULL);
    cloc=p->gl2loc(cglo);// convert global coord. to local
    x=cloc[0];
    y=cloc[1];
    z=cloc[2];
//       cerr<<"Loc.coord= "<<x<<" "<<y<<" "<<z<<'\n'; // tempor
    if(time>=tmax||de>1.){
//      cerr<<"TOF: Bad G-hit, id="<<id<<" t/de="<<time<<" "<<de<<'\n';
      ptr=ptr->next(); // next hit
      continue;
    }
    ibtyp=TOFDBc::brtype(ilay,ibar)-1;
    scmcscan[ibtyp].getxbin(y,i1,i2,r);//y-bin # (longit.(x !)-coord. in LTRANS )
    nel0=de*convr;
// PM-1 actions --->
    eff=scmcscan[ibtyp].getef1(r,i1,i2);//eff for PM-1
    nel=nel0*eff;// mean number of photoelectrons
    POISSN(nel,nelec,ierr);// fluctuations
//    <-------- Loop over photoelectrons(PM-1) ---<<<
    for(i=1;i<=nelec;i++){
      rnd=RNDM(dummy);
      tm=scmcscan[ibtyp].gettm1(rnd,r,i1,i2);//ph.arrival time from interpol.distr.
      tm=tm+time+(3.+rnormx())*trts;// TOF+fixed_trans_time_delay+fluct.
//      if(ilay==2 && ibar==7)tm=tm+1.;//tempor to test calibr
      rnd=RNDM(dummy);
      am=scpmsesp.getrand(rnd);//amplitude from single elect. spectrum
//----->  summing of all photoelectrons as fixed shape pulses:
      ii=tm/fadcb;
      for(j=0;j<npshbn;j++){
        jj=ii+j;
        if(jj<SCTBMX)
             tslice1[jj]+=am*pmplsh[j];
        else{ 
             tslice1[SCTBMX]+=am*pmplsh[j];
            }
      }
    } // >>>----- end of PM-1 loop ------>
//
// PM-2 actions --->
    eff=scmcscan[ibtyp].getef2(r,i1,i2);//eff for PM-2
    nel=nel0*eff;// mean number of photoelectrons
    POISSN(nel,nelec,ierr);// fluctuations
//    <-------- Loop over photoelectrons(PM-2) ---<<<
    for(i=1;i<=nelec;i++){
      rnd=RNDM(dummy);
      tm=scmcscan[ibtyp].gettm2(rnd,r,i1,i2);//ph.arrival time from interpol. distr.
      tm=tm+time+(3.+rnormx())*trts;// TOF+fixed_trans_time_delay+fluct.
//      if(ilay==2 && ibar==7)tm=tm+1.;//tempor to test calibr
      rnd=RNDM(dummy);
      am=scpmsesp.getrand(rnd);//amplitude from single elect. spectrum
//----->  summing of all photoelectrons as fixed shape pulses:
      ii=tm/fadcb;
      for(j=0;j<npshbn;j++){
        jj=ii+j;
        if(jj<SCTBMX)
             tslice2[jj]+=am*pmplsh[j];
        else {
             tslice2[SCTBMX]+=am*pmplsh[j];
             }
      }
    } // >>>----- end of PM-2 loop ------>
//-----------------------------------
      ptrN=ptr->next();
      idN=0; 
      if(ptrN)idN=ptrN->getid();// id of the next G-hit
      if(idN != id){ // --> last or new bar -> create Tovt-objects :
        edepb*=1000.;
        idd=id*10+1;
        if(edepb>TOFMCFFKEY.Thr)
                 AMSTOFTovt::totovt(idd,edepb,tslice1);// Tovt-obj for side(PM)-1
        if(TOFMCFFKEY.mcprtf[1] != 0)
                      AMSTOFTovt::displ_a(idd,20,tslice1);//print PMT pulse
        idd=id*10+2;
        if(edepb>TOFMCFFKEY.Thr)
                 AMSTOFTovt::totovt(idd,edepb,tslice2);// Tovt-obj for side(PM)-2
        if(TOFMCFFKEY.mcprtf[1] != 0)
                      AMSTOFTovt::displ_a(idd,20,tslice2);//print PMT pulse
        for(i=0;i<SCTBMX+1;i++)tslice1[i]=0.;//clear flash ADC arrays for next bar
        for(i=0;i<SCTBMX+1;i++)tslice2[i]=0.;
        edepb=0.;// clear Edep
      }
//
    ptr=ptr->next();
  }// ------ end of geant hits loop ---->
//
  if(TOFMCFFKEY.mcprtf[2]!=0){
    for(i=0;i<SCLRS;i++)HF1(1050+i,float(nhitl[i]),1.);
    HF1(1060,1000.*edepl[0],1.);
    HF1(1061,1000.*edepl[0],1.);
    HF1(1062,1000.*edepl[2],1.);
    HF1(1063,1000.*edepl[2],1.);
  }
//
}
//=========================================================================
//
// function below creates PMT pulse shape array arr[] with binning fladctb :
//  peak value = 1.(e.g. 1mV@50ohm)
void AMSTOFTovt::inipsh(integer &nbn, float arr[])
{
  static float pmpsh[15]={0.,0.026,0.175,0.422,0.714,0.942,1.,0.935,
       0.753,0.584,0.442,0.305,0.182,0.078,0.};// pulse hights at time points
  static float pmpsb[15]={0.,0.5,1.,1.5,2.,2.5,3.,3.5,4.,4.5,5.,
       5.5,6.,6.5,7.}; // time points (min. step should be above fladctb)
  integer i,io,ib;
  float bl,bh,bol,boh,ao1,ao2,tgo,al,ah,tota;
  tota=0.;
  io=0;
  for(ib=0;ib<200;ib++){
    bl=ib*TOFDBc::fladctb();
    bh=bl+TOFDBc::fladctb();
    bol=pmpsb[io];
    boh=pmpsb[io+1];
    arr[ib]=0.;
    if(bl>=bol && bh<boh){
      ao1=pmpsh[io];
      ao2=pmpsh[io+1];
      tgo=(ao2-ao1)/(boh-bol);
      al=ao1+tgo*(bl-bol);
      ah=ao1+tgo*(bh-bol);
      arr[ib]=(al+ah)/2.;
      tota+=arr[ib];
    }
    else if(bl<boh && bh>=boh){
      ao1=pmpsh[io];
      ao2=pmpsh[io+1];
      tgo=(ao2-ao1)/(boh-bol);
      al=ao1+tgo*(bl-bol);
      io+=1;
      if((io+1)<15){
        bol=pmpsb[io];
        boh=pmpsb[io+1];
        ao1=pmpsh[io];
        ao2=pmpsh[io+1];
        tgo=(ao2-ao1)/(boh-bol);
        ah=ao1+tgo*(bh-bol);
        arr[ib]=(al+ah)/2.;
        tota+=arr[ib];
      }
      else{                 // last bin
        ah=0.;
        arr[ib]=(al+ah)/2.;
        tota+=arr[ib];
        nbn=ib+1;
        if(TOFMCFFKEY.mcprtf[0] != 0){
          tota=tota*TOFDBc::fladctb()/50.;
          printf("PM pulse shape:nbins=% 3d, Integral(pC) = %4.2e\n",nbn,tota);
          for(i=1;i<=nbn;i++){
            if(i%10 != 0)
                         printf("% 4.2e",arr[i-1]);
            else
                         printf("% 4.2e\n",arr[i-1]);
          }
          if(nbn%10 !=0)printf("\n");
        }
        return;
      }
    }
  }
  cerr<<"AMSTOFTovt-inipsh-Fatal: wrong pulse shape !?"<<'\n';
  exit(1);
}
//--------------------------------------------------------------------------
//
// function below creates shaper stand. pulse shape array arr[] with bin shaptb :
// (this array is shaper responce to A=1 input pulse with width=shaptb;
//  shaper rise/fall times are shrtim<<shftim; nbn is array length,defined as
//  boundary where the rest of sh.pulse integral is less than 0.0001 of current
//  integral value)
//
void AMSTOFTovt::inishap(integer &nbn, float arr[])
{
  float tr,tf,tmb,toti,rest,t1,t2,anr;
  integer i,imax,j,ifl(0);
  tr=TOFDBc::shrtim();
  tf=TOFDBc::shftim();
  tmb=TOFDBc::shaptb();
  imax=SCSBMX/2.;
  arr[0]=1.-exp(-tmb/tr);// fast charge (neglect by discharge !)
  for(i=1;i<imax;i++){ // <--- time loop ---
    t1=i*tmb;
    arr[i]=arr[0]*exp(-t1/tf);
    if(arr[i]<(0.0001*arr[0])){
      nbn=i+1;
      if(TOFMCFFKEY.mcprtf[0] != 0){
        printf("Shaper pulse shape ready : nbins=% 3d\n",nbn);
        for(j=1;j<=nbn;j++){
          if(j%10 != 0)
                       printf("% 4.2e",arr[j-1]);
          else
                       printf("% 4.2e\n",arr[j-1]);
        }
        if(nbn%10 !=0)printf("\n");
      }
      return;
    }
  } // --- end if time loop --->
  cerr<<"AMSTOFTovt-inishap-Fatal: bad shaper rise/fall times ?!"<<'\n';
  cerr<<"tr/tf= "<<tr<<" "<<tf<<" nbins="<<i<<'\n';
  exit(1);
}
//--------------------------------------------------------------------------
//
// function below simulate PMT saturation, it returns nonsat. factor <=1. :
//
float AMSTOFTovt::pmsatur(const float am){
  float gain=1.; //tempor. no saturation
  return gain;
}
//--------------------------------------------------------------------------
//
// function to display PMT pulse (flash-ADC array arr[]) :
//
void AMSTOFTovt::displ_a(const int id, const int mf, const float arr[]){
  integer i;
  static integer ifrs(0);
  float tm,a(0.);
  static float tb,tbi;
  if(ifrs++==0){
    tb=float(mf)*TOFDBc::fladctb();
    tbi=TOFDBc::fladctb();
    HBOOK1(1000,"PMT flash-ADC pulse (MC)",80,0.,80*tb,0.);
  }
  HRESET(1000," ");
  cerr<<"PMT:counter-id = "<<id<<" (LBBS, L-layer, BB-bar, S-side)"<<'\n';
  for(i=1;i<=SCTBMX;i++){
    if(i%mf==0){
      a+=arr[i-1];
      tm=i*tbi-0.5*tb;
      HF1(1000,tm,a/float(mf));
      a=0.;
    }
    else{
      a+=arr[i-1];
    }
  }
  HPRINT(1000);
  return ;
}
//--------------------------------------------------------------------------
//
// function to display SHAPER pulse ( array arr[]) :
//
void AMSTOFTovt::displ_as(const int id, const int mf, const float arr[]){
  integer i;
  static integer ifrs(0);
  float tm,a(0.);
  static float tb,tbi;
  if(ifrs++==0){
    tb=float(mf)*TOFDBc::shaptb();
    tbi=TOFDBc::shaptb();
    HBOOK1(1001,"Shaper pulse (MC)",80,0.,80*tb,0.);
  }
  cerr<<"SHP:counter-id = "<<id<<" (LBBS, L-layer, BB-bar, S-side)"<<'\n';
  HRESET(1001," ");
  for(i=1;i<=SCSBMX;i++){
    if(i%mf==0){
      a+=arr[i-1];
      tm=i*tbi-0.5*tb;
      HF1(1001,tm,a/float(mf));
      a=0.;
    }
    else{
      a+=arr[i-1];
    }
  }
  HPRINT(1001);
  return ;
}
//--------------------------------------------------------------------------
//
void AMSTOFTovt::totovt(integer idd, float edepb, float tslice[])
{
  integer i,j,ij,ilay,id,_sta,stat(0);
  float tm,a,am,amd,tmp,amp;
  integer _ntr1,_ntr2,_ntr3,_nftdc,_nstdc,_nadca,_nadcd;
  number _ttr1[SCTHMX1],_ttr2[SCTHMX1],_ttr3[SCTHMX1];
  number  _tftdc[SCTHMX2],_tstdc[SCTHMX3],_tadca[SCTHMX4],_tadcd[SCTHMX4];
  int upd1,upd2,upd3; // up/down flags for 3 fast discr. (z>=1;z>1;z>2)
  int upd11,upd12,upd13,upd21,upd31;
  int updsh;
  int imax;
  float tshd,tshup,charge;
  float td1b1,td1b2,td1b3;
  float td2b1,td3b1;
  float tmd1u,tmd1d,tbn,w,bo1,bo2,bn1,bn2;
  float tshap1[SCSBMX+1]; // temporary shaper pulse array(anode)
  float tshap2[SCSBMX+1]; // temporary shaper pulse array(dinode)
  static integer first=0;
  static integer nshbn,mxcon,mxshc;
  static float fladcb,shapb,cconv,d2a,daqt0,daqt3,daqt4;
  static float daqp0,daqp3,daqp4,daqp5,daqp6,daqp7;
  static float shplsh[SCSBMX/2];  // Shaper standard pulse shape array
//
  if(first++==0){
    AMSTOFTovt::inishap(nshbn,shplsh); // prepare Shaper stand. pulse shape arr.
    fladcb=TOFDBc::fladctb();
    shapb=TOFDBc::shaptb();
    cconv=fladcb/50.; // mV->pC
    d2a=TOFDBc::di2anr();
    daqt0=TOFDBc::daqthr(0);
    daqt3=TOFDBc::daqthr(3);
    daqt4=TOFDBc::daqthr(4);
    daqp0=TOFDBc::daqpwd(0);
    daqp3=TOFDBc::daqpwd(3);
    daqp4=TOFDBc::daqpwd(4);
    daqp5=TOFDBc::daqpwd(5);
    daqp6=TOFDBc::daqpwd(6);
    daqp7=TOFDBc::daqpwd(7);
  }
//
  id=idd/10;// LBB
  ilay=id/100-1;
// -----> create/fill summary Tovt-object for idsoft=idd :
//
        if(tslice[SCTBMX]>0.){
          cerr<<"SITOFTovt-warning: out of time scale, id="<<idd<<
             "  A-last= "<<tslice[SCTBMX]<<'\n';
          cerr<<tslice[SCTBMX-1]<<" "<<tslice[SCTBMX-2]<<'\n';
          if(TOFMCFFKEY.mcprtf[1])AMSTOFTovt::displ_a(idd,20,tslice);//print PMT pulse
        }
        for(i=SCTBMX-1;i>=0;i--)if(tslice[i]!=0)break;// find useful range
        imax=i+1;
        charge=0.;
        for(i=0;i<imax;i++)charge+=tslice[i];
        charge=charge*fladcb/50.; // total charge (pC)
//
        _ntr1=0;
        _ntr2=0;
        _ntr3=0;
        _nftdc=0;
        _nstdc=0;
        upd1=0; //up/down flag for discr.1
        tmd1d=-9999.;
        upd2=0;
        upd3=0;
        td1b1=-9999.;
        td1b2=-9999.;
        td1b3=-9999.;
        upd11=0;// up/down flag for z>=1 branch of discr.1
        upd12=0;// ............ fast TDC .................
        upd13=0;// ............ slow TDC .................
        upd21=0;// up/down flag for z>1 branch of discr.2
        td2b1=-9999.;
        upd31=0;// up/down flag for z>2 branch of discr.3 (dinode)
        td3b1=-9999.;
//
        amp=0.;
        tmp=0.;
        for(i=0;i<imax;i++){  //  <--- time bin loop for time measurements ---
          am=tslice[i];
//--------------------------          
// discr.1 up/down setting :
          if(am>=daqt0){
            if(upd1 ==0){
              tm=(i+1)*fladcb;
              tmp=tm-fladcb;
              if(i>0)amp=tslice[i-1];
              tmd1u=tmp+fladcb*(daqt0-amp)/(am-amp);// up time of discr.1
              upd1=1;
            }
          }
          else{
            if(upd1!=0){ // down time of discr.1
              tm=(i+1)*fladcb;
              upd1=0;
              tmd1d=tm;
            }
          }
//--------------------------
// try set all branches when discr-1 up: 
          if(upd1>0){
            tm=(i+1)*fladcb;
//
// branch-1 "z>=1 logic" :
            if(upd11==0){
              if((tm-td1b1)>daqp7){//dead time check for z>=1 signal
                upd11=1;  // set flag for branch z>=1
                if(_ntr1<SCTHMX1){
                  _ttr1[_ntr1]=tmd1u;
                  _ntr1+=1;
                }
                else{
                  cerr<<"AMSTOFTovt::build-warning: trig1 hits ovfl"<<'\n';
                  upd11=2;//blockade on overflow
                }
              } 
            }
//        
// branch-2 "fast TDC logic" :        
            if(upd12==0){
              if((tm-tmd1d)>daqp3){ //dbl-pulse res. check, f-TDC channel
                upd12=1;  // set flag for branch f-TDC,if ready.
                if(_nftdc<SCTHMX2){
                  td1b2=tm;
                  _tftdc[_nftdc]=tmd1u;
                  _nftdc+=1;
                }
                 else{
                   cerr<<"AMSTOFTovt::build-warning: f-TDC hits ovfl"<<'\n';
                   upd12=2;//blockade on overflow
                }
              }
            } 
//        
// branch-3 "slow TDC logic" :        
            if(upd13==0){
              if((tmd1u-td1b3)>daqp4){ // "buzy" check for f-TDC channel
                upd13=1;  // set flag for branch s-TDC,if ready 
                if(_nstdc<SCTHMX3){
                  td1b3=tm;
                  _tstdc[_nstdc]=tmd1u;
                  _nstdc+=1;
                }
                else{
                  cerr<<"AMSTOFTovt::build-warning: s-TDC hits ovfl"<<'\n';
                  upd13=2;//blockade on overflow
                }
              } 
            }
          }//<-- end of discr-1 up check
//---------------------------
// try reset all branches:
//
          if(upd11==1){// "z>=1 logic" reset
            tm=(i+1)*fladcb;
            if((tm-_ttr1[_ntr1-1])>daqp0){//pulse width check
              upd11=0;                            // self clear
              td1b1=tm;
            }
          } 
          if(upd12==1){ // "fast TDC" clear when discr.1 down
            if(upd1 ==0)upd12=0;
          }
          if(upd13==1){ // "slow TDC" clear "buzy"
            tm=(i+1)*fladcb;
            if((tm-td1b3)>daqp4)upd13=0;
          }
//---------------------------        
        } //      --- end of time bin loop for time measurements --->         
//----------------------------------------------------------------------
        mxcon=imax*(fladcb/shapb)+1;// flash adc time range in shaper scale
        mxshc=mxcon+nshbn;// max. useful shaper channels
//
//    convert fast flash ADC array tslice to shaper pulse (anode):
//                 
// rebinning of tslice[] into shaper binned tshap1[]
        for(j=0;j<mxcon;j++)tshap1[j]=0.; // clear shaper input array
        j=0;                 
        for(i=0;i<imax;i++){
          am=tslice[i];
//          am=a*AMSTOFTovt::pmsatur(a); //instant saturation for anode signal
          bo1=i*fladcb;
          bo2=bo1+fladcb;
          bn1=j*shapb;
          bn2=bn1+shapb;
          if(bo1>=bn1 && bo2<bn2)tshap1[j]+=am;
          else if(bo1<bn2 && bo2>=bn2){
            w=(bn2-bo1)/fladcb;
            tshap1[j]+=w*am;
            tshap1[j+1]+=(1.-w)*am;
            j+=1;
          }
        }
// tshap1[] --- shaping ---> tshap2[] conversion :        
        for(i=0;i<(SCSBMX+1);i++)tshap2[i]=0.;
        for(i=0;i<mxcon;i++){
          am=tshap1[i]*cconv;// mV->pC for time-bin shapb
          if(am>0.){
            for(j=0;j<nshbn;j++){
              if((i+j)<mxshc)
                             tshap2[i+j]+=am*shplsh[j];
              else
                             tshap2[mxshc]+=am*shplsh[j];
            }
          }
        }
//
//
// Time_over_threshold measurement for anode :
        _nadca=0;
        updsh=0;
        tshd=-9999.;
        for(i=0;i<mxshc;i++){  //  <--- time bin loop ---
          am=tshap2[i];
          if(am>=daqt3){
            if(updsh==0){
              tm=(i+1)*shapb;
              if((tm-tshd)>daqp5){ //dead time check
                updsh=1;
                tshup=tm;
              }
            }
          }
          else{
            if(updsh==1){
              tm=(i+1)*shapb;
              updsh=0;
              tshd=tm;
              if(_nadca<SCTHMX4){
                _tadca[_nadca]=tm-tshup;
                _nadca+=1;
              }
              else{
                cerr<<"AMSTOFTovt::build-warning: a-ADC hits ovfl"<<'\n';
                updsh=2;//blockade on overflow
              }
            } 
          }
        }                    // --- end of time bin loop --->
// 
        if(TOFMCFFKEY.mcprtf[1] != 0)
                      AMSTOFTovt::displ_as(idd,2,tshap2);//print Shaper pulse
//------------------------------  
//     convert fast flash ADC array tslice to shaper pulse (dinode):
//                 
// rebinning of tslice[] into shaper binned tshap1[]
        for(j=0;j<mxcon;j++)tshap1[j]=0.; // clear shaper input array
        j=0;                 
        for(i=0;i<imax;i++){
          a=tslice[i];
          am=a*d2a; //no saturation for dinode signal, just reduction
          bo1=i*fladcb;
          bo2=bo1+fladcb;
          bn1=j*shapb;
          bn2=bn1+shapb;
          if(bo1>=bn1 && bo2<bn2)tshap1[j]+=am;
          else if(bo1<bn2 && bo2>=bn2){
            w=(bn2-bo1)/fladcb;
            tshap1[j]+=w*am;
            tshap1[j+1]+=(1.-w)*am;
            j+=1;
          }
        }
// tshap1[] --- shaping ---> tshap2[] conversion :        
        for(i=0;i<(SCSBMX+1);i++)tshap2[i]=0.;
        for(i=0;i<mxcon;i++){
          am=tshap1[i]*cconv;// mV->pC
          if(am>0.){
            for(j=0;j<nshbn;j++){
              if((i+j)<mxshc)
                             tshap2[i+j]+=am*shplsh[j];
              else
                             tshap2[mxshc]+=am*shplsh[j];
            }
          }
        }
//                      
// Time_over_threshold measurement for dinode :
        _nadcd=0;
        updsh=0;
        tshd=-9999.;
        for(i=0;i<mxshc;i++){  //  <--- time bin loop ---
          am=tshap2[i];
          if(am>=daqt4){
            if(updsh==0){
              tm=(i+1)*shapb;
              if((tm-tshd)>daqp6){ //dead time check
                updsh=1;
                tshup=tm;
              }
            }
          }
          else{
            if(updsh==1){
              tm=(i+1)*shapb;
              updsh=0;
              tshd=tm;
              if(_nadcd<SCTHMX4){
                _tadcd[_nadcd]=tm-tshup;
                _nadcd+=1;
              }
              else{
                cerr<<"AMSTOFTovt::build-warning: d-ADC hits ovfl"<<'\n';
                updsh=2;//blockade on overflow
              }
            } 
          }
        }                    // --- end of time bin loop --->
//
        if(TOFMCFFKEY.mcprtf[1] != 0)
                      AMSTOFTovt::displ_as(idd,2,tshap2);//print Shaper pulse
//------------------------------
// now create/fill AMSTOFTovt object (id=idd) with above digi-data:
        _sta=0.;// tempor (to be taken from DB later)    
        stat=0;
        stat=AMSEvent::gethead()->addnext(AMSID("AMSTOFTovt",ilay), new
             AMSTOFTovt(idd,_sta,charge,edepb,_ntr1,_ttr1,
                _nftdc,_tftdc,_nstdc,_tstdc,_nadca,_tadca,_nadcd,_tadcd));
        return;
//
//
}
//---------------------------------------------------------------------
//
// function to get absolute time of the FIRST trigger(coincidence) "z>=1";
// trcode - trigger code (pattern) =-1/n, n>=0 -> missing plane #, 
// =-1 -> no trigger.
//
number AMSTOFTovt::tr1time(int &trcode){
  int i1,i2,isds(0);
  integer i,j,ilay,ntr,idd,id,idN,stat;
  number ttr[SCTHMX1],t1,t2;
  AMSBitstr trbs[2];
  AMSBitstr trbc;
  AMSBitstr trbl[SCLRS];
  AMSTOFTovt *ptr;
  AMSTOFTovt *ptrN;
//
  for(ilay=0;ilay<SCLRS;ilay++){// <--- layers loop (Tovt containers) ---
    ptr=(AMSTOFTovt*)AMSEvent::gethead()->
                               getheadC("AMSTOFTovt",ilay,0);
    isds=0;
    while(ptr){// <--- Tovt-hits loop in layer ---
      idd=ptr->getid();
      id=idd/10;// short id=LBB
      stat=ptr->getstat();
      ntr=ptr->gettr1(ttr);// get number and times of trig1 ("z>=1")
//      cerr<<"id= "<<idd<<" Ntrpulses= "<<ntr<<'\n'; // tempor
      for(j=0;j<ntr;j++){// <--- trig-hits loop ---
        t1=ttr[j];
        t2=t1+TOFDBc::daqpwd(0);
        i1=t1/TOFDBc::trigtb();
        i2=t2/TOFDBc::trigtb();
        if(i1>1023)i1=1023;
        if(i2>1023)i2=1023;
        if(isds<2)
          if(stat==0)trbs[isds].bitset(i1,i2);//set bits acc. to pulse width
        else
            cerr<<"AMSTOFTovt::tr1time: error: >2sides !!!"<<'\n';
        isds+=1; 
      }//             --- end of trig-hits loop --->
      ptrN=ptr->next();
      idN=0;
      if(ptrN)idN=(ptrN->getid())/10; //next hit short id
//
      if(idN != id){ // next hit is OTHER counter hit or last hit,
//       so create both side (counter) bit pattern for CURRENT counter
        if(TOFMCFFKEY.trlogic[0] == 0)
                                  trbc=trbs[0] & trbs[1];// 2-sides AND
        else
                                  trbc=trbs[0] | trbs[1];// 2-sides OR
        trbl[ilay]=trbl[ilay] | trbc; // summing OR within the layer
        isds=0; // reset c.sides variables ...
        trbs[0].bitclr(1,0);
        trbs[1].bitclr(1,0);
      }
      ptr=ptr->next();// take next Tovt-hit
// 
    }//         --- end of Tovt-hits loop in layer --->
//
    trbl[ilay].testbit(i1,i2);// check ilay-plane "OR"
    if(i2>=i1){// make stand.logic pulse of daqpwd[0] width
      i2=TOFDBc::daqpwd(0)/TOFDBc::trigtb();
      i2+=i1;
      trbl[ilay].bitclr(1,0);
      if(i2>1023)i2=1023;
      trbl[ilay].bitset(i1,i2);
    }
  } //                               --- end of layer loop --->
//
  trcode=-1;
  AMSBitstr coinc1234,rr1,rr2;
  rr1=trbl[0]&trbl[1];
  rr2=trbl[2]&trbl[3];
  coinc1234=rr1&rr2;
  coinc1234.testbit(i1,i2);
  if(i2>=i1){// have 4-fold coincidence
    trcode=0;
    t1=i1*TOFDBc::trigtb();
    return t1;
  }
  else{   // check 3-fold coincidence:
    int imin=9999;
    AMSBitstr coinc0234=trbl[1]&trbl[2]&trbl[3];
    AMSBitstr coinc1034=trbl[0]&trbl[2]&trbl[3];
    AMSBitstr coinc1204=trbl[0]&trbl[1]&trbl[3];
    AMSBitstr coinc1230=trbl[0]&trbl[1]&trbl[2];
    coinc0234.testbit(i1,i2);
    if((i2>=i1) && (i1<imin)){
      imin=i1;
      trcode=1;
    }
    coinc1034.testbit(i1,i2);
    if((i2>=i1) && (i1<imin)){
      imin=i1;
      trcode=2;
    }
    coinc1204.testbit(i1,i2);
    if((i2>=i1) && (i1<imin)){
      imin=i1;
      trcode=3;
    }
    coinc1230.testbit(i1,i2);
    if((i2>=i1) && (i1<imin)){
      imin=i1;
      trcode=4;
    }
    t1=0.;
    if(imin!=9999)t1=i1*TOFDBc::trigtb();
    return t1;
  }
}
//=====================================================================
//
//  function to set bits in AMSBitstr objects (from bit il till bit ih):
//   (bits are counting starting from 0 (left edge of bit-array))
//   (if il>ih, function sets all available bits)
//
void AMSBitstr::bitset(const int il, const int ih){
//
  static unsigned short int allzer(0x0000);
  static unsigned short int allone(0xFFFF);
  static unsigned short int setone[16]={
       0x8000,0x4000,0x2000,0x1000,0x0800,0x0400,0x0200,0x0100,
       0x0080,0x0040,0x0020,0x0010,0x0008,0x0004,0x0002,0x0001};
  int i,iw1,iw2,i1,i2,nw;
  int max=1024;
  assert(il>=0 && ih>=0);
  assert(il<max && ih<max);
  iw1=il/16;
  i1=il-16*iw1;
  if(il==ih){
    bitstr[iw1]|=setone[i1];
    return;
  }
  if(il>ih){
    for(i=0;i<bslen;i++)bitstr[i]=allone;//set all bits
    return;
  }
  iw2=ih/16;
  i2=ih-16*iw2;
  nw=iw2-iw1;
  if(nw==0){
    for(i=i1;i<=i2;i++)bitstr[iw1]|=setone[i];
  }
  else{
    for(i=i1;i<=15;i++)bitstr[iw1]|=setone[i];
    for(i=0;i<=i2;i++)bitstr[iw2]|=setone[i];
    for(i=iw1+1;i<iw2;i++){bitstr[i]=allone;}
  }
  return;
}
//----------------------------------------------------------------------
//  function to clear bits in AMSBitstr objects (from bit il upto bit ih):
//   (bits are counting starting from 0 (left edge of bit-array))
//   (if il>ih, function clear all available bits)
//
void AMSBitstr::bitclr(const int il, const int ih){
//
  static unsigned short int allzer(0x0000);
  static unsigned short int setzer[16]={
        0x7FFF,0xBFFF,0xDFFF,0xEFFF,0xF7FF,0xFBFF,0xFDFF,0xFEFF,
        0xFF7F,0xFFBF,0xFFDF,0xFFEF,0xFFF7,0xFFFB,0xFFFD,0xFFFE};
  int i,iw1,iw2,i1,i2,nw;
  int max=1024;
  assert(il>=0 && ih>=0);
  assert(il<max && ih<max);
  iw1=il/16;
  i1=il-16*iw1;
  if(il==ih){
    bitstr[iw1]&=setzer[i1];
    return;
  }
  if(il>ih){
    for(i=0;i<bslen;i++)bitstr[i]=allzer;//clear all bits
    return;
  }
  iw2=ih/16;
  i2=ih-16*iw2;
  nw=iw2-iw1;
  if(nw==0){
    for(i=i1;i<=i2;i++)bitstr[iw1]&=setzer[i];
  }
  else{
    for(i=i1;i<=15;i++)bitstr[iw1]&=setzer[i];
    for(i=0;i<=i2;i++)bitstr[iw2]&=setzer[i];
    for(i=iw1+1;i<iw2;i++){bitstr[i]=allzer;}
  }
  return;
}
//-----------------------------------------------------------------------
// function to test "up" bits in AMSBitstr object. It returns i1/i2 as
// first/last "up" bits of the FIRST continious bunch of "up" bits 
// in the object.  i1>i2 (1>0) if no "up" bits.
//
void AMSBitstr::testbit(int &i1, int &i2){
  static unsigned short int setone[16]={
       0x8000,0x4000,0x2000,0x1000,0x0800,0x0400,0x0200,0x0100,
       0x0080,0x0040,0x0020,0x0010,0x0008,0x0004,0x0002,0x0001};
  int i,iw;
  int ifl(0);
  unsigned short int ia;
  i1=1;
  i2=0;
  for(iw=0;iw<bslen;iw++){
    for(i=0;i<16;i++){
      ia=bitstr[iw] & setone[i];
      if((ia != 0) && (ifl == 0)){
        i1=iw*16+i;
        ifl=1;
      }
      if((ia == 0) && (ifl == 1)){
        i2=iw*16+i-1;
        ifl=0;
        return;
      }
    }
  }
  if(ifl==1){// case when bits are "up" till the end of bitstream
    i2=16*bslen-1;
    return;
  }
}
//-----------------------------------------------------------------------
AMSBitstr operator &(const AMSBitstr &b, const AMSBitstr &c){
  int lenb=b.bslen;
  int lenc=c.bslen;
  int len=lenb;
  unsigned short int arr[64];
  if(lenb != lenc){
    cout<<"AMSBitstr:AND-wrong_length "<<lenb<<" "<<lenc<<'\n';
    if(lenc=64)len=lenc; // tempor solution
    else if(lenb=64)len=lenb;
    else exit(1);
  }
  for(int i=0;i<len;i++)arr[i]= b.bitstr[i] & c.bitstr[i];
  return AMSBitstr(len,arr);
} 
//-----------------------------------------------------------------------
AMSBitstr operator |(const AMSBitstr &b, const AMSBitstr &c){
  int lenb=b.bslen;
  int lenc=c.bslen;
  int len=lenb;
  unsigned short int arr[64];
  if(lenb != lenc){
    cout<<"AMSBitstr:OR-wrong_length"<<lenb<<" "<<lenc<<'\n';
    if(lenc=64)len=lenc; // tempor solution
    else if(lenb=64)len=lenb;
    else exit(1);
  }
  for(int i=0;i<len;i++)arr[i] = b.bitstr[i] | c.bitstr[i];
  return AMSBitstr(len,arr);
} 
//-----------------------------------------------------------------------
void AMSBitstr::display(char comment[]){
    printf("AMSBitstr: %s ;  length=%d\n",comment,bslen);
    for(int i=1;i<=bslen;i++){
      if(i%10 != 0)
                  printf(" %#x",bitstr[i-1]);
      else 
                  printf(" %#x\n",bitstr[i-1]);
    }
    if(bslen%10 !=0)printf("\n");
  }
//
//=====================================================================
//
//  function to build RawEvent-objects from MC Tovt-objects
//               (very preliminary !!!)
void AMSTOFRawEvent::mc_build()
{
  integer i,j,ilay,last,ibar,id,idd,iddN,isd,_sta,stat(0);
  integer nftdc,nstdc,nadca,nadcd;
  int16u _nftdc,_nstdc,_nadca,_nadcd;
  number  tftdc[SCTHMX2],tstdc[SCTHMX3],tadca[SCTHMX4],tadcd[SCTHMX4];
  int16u  ftdc[SCTHMX2],stdc[SCTHMX3],adca[SCTHMX4],adcd[SCTHMX4];
  number t,ttrig1,dt;
  float charge,edep;
  int trcode;
  int16u it;
  AMSTOFTovt *ptr;
  AMSTOFTovt *ptrN;
//
  ttrig1=AMSTOFTovt::tr1time(trcode);//get abs. trig1("z>=1" accept) signal time
  if(trcode<0)return; // no trigger
//  
  for(ilay=0;ilay<SCLRS;ilay++){// <--- layers loop (Tovt containers) ---
    ptr=(AMSTOFTovt*)AMSEvent::gethead()->
                               getheadC("AMSTOFTovt",ilay,0);
    while(ptr){// <--- Tovt-hits loop in layer ---
      idd=ptr->getid();
      id=idd/10;// short id=LBB
      isd=idd%10;
      stat=ptr->getstat();
      charge=ptr->getcharg();
      edep=ptr->getedep();
//
      nftdc=ptr->getftdc(tftdc);// get number and times of fast-TDC hits
      _nftdc=0;
      for(j=0;j<nftdc;j++){// <--- ftdc-hits loop ---
        t=tftdc[j];
        dt=ttrig1+TOFDBc::accdel(0)-t;//const.delay of lev-1 "accept" signal is added
        if(dt<=TOFDBc::accdelmx(0)){ // check max. delay of lev-1 "accept" signal
          it=dt/TOFDBc::tdcbin(0); // conv. to fast-TDC (history) t-binning
          ftdc[_nftdc]=it;
          _nftdc+=1;
        }
      }//--- end of ftdc-hits loop --->
//
      nstdc=ptr->getstdc(tstdc);// get number and times of slow-TDC hits
      _nstdc=0;
      for(j=0;j<nstdc;j++){// <--- stdc-hits loop ---
        t=tstdc[j];
        dt=ttrig1+TOFDBc::accdel(1)-t;// const.delay is added
        if(dt<=TOFDBc::accdelmx(1)){ // check max. delay of "accept" signal
          it=dt*TOFDBc::strrat()/
                TOFDBc::tdcbin(1);//conv. to stratcher-TDC t-binning 
          stdc[_nstdc]=it;
          _nstdc+=1;
        }
      }//--- end of stdc-hits loop --->
//
      _nadca=0;
      nadca=ptr->getadca(tadca);// get number and times of anode-ADC hits
//      cerr<<"adca hits "<<nadca<<'\n'; // tempor
      for(j=0;j<nadca;j++){// <--- adca-hits loop ---
        t=tadca[j];
        it=t/TOFDBc::tdcbin(2);
        adca[j]=it;
        _nadca+=1;
      }//--- end of adca-hits loop --->
//
      _nadcd=0;
      nadcd=ptr->getadcd(tadcd);// get number and times of dinode-ADC hits
      for(j=0;j<nadcd;j++){// <--- adcd-hits loop ---
        t=tadcd[j];
        it=t/TOFDBc::tdcbin(3);
        adcd[j]=it;
        _nadcd+=1;
      }//--- end of adcd-hits loop --->
//
//     ---> create/fill RawEvent-object :
      stat=0;
      stat=AMSEvent::gethead()->addnext(AMSID("AMSTOFRawEvent",0), new
           AMSTOFRawEvent(idd,stat,charge,edep,_nftdc,ftdc,_nstdc,stdc,
                               _nadca,adca,_nadcd,adcd));
//
      ptr=ptr->next();// take next Tovt-hit
// 
    }//         --- end of Tovt-hits loop in layer --->
//
  } //                               --- end of layer loop --->
//
}
