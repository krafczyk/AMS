//  $Id: tofdbc02.C,v 1.7 2001/01/22 17:32:22 choutko Exp $
// Author E.Choumilov 14.06.96.
#include <typedefs.h>
#include <math.h>
#include <commons.h>
#include <job.h>
#include <tofdbc02.h>
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <tofsim02.h>
#include <tofrec02.h>
#include <tofcalib02.h>
//
TOF2Varp TOF2Varp::tofvpar; // mem.reserv. TOF general parameters 
TOF2Brcal TOF2Brcal::scbrcal[TOF2GC::SCLRS][TOF2GC::SCMXBR];// mem.reserv. TOF indiv.bar param. 
TOFBPeds TOFBPeds::scbrped[TOF2GC::SCLRS][TOF2GC::SCMXBR];//mem.reserv. TOF-bar pedestals/sigmas/...
TOF2Varp::TOF2Temperature TOF2Varp::tftt;
//-----------------------------------------------------------------------
//  =====> TOF2DBc class variables definition :
//
integer TOF2DBc::debug=1;
//
//
//======> memory reservation for _brtype :
// (real values are initialized from ext. geomconfig-file in amsgeom.c) 
integer TOF2DBc::_brtype[TOF2GC::SCBLMX]={
  0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0
};
//
// Initialize TOF geometry (defaults, REAL VALUES are read FROM geomconfig-file)
//
//---> bar lengthes (cm) for each of TOF2GC::SCBTPN types :
geant TOF2DBc::_brlen[TOF2GC::SCBTPN]={
  0.,0.,0.,0.,0.,0.,0.,0.,0.,0.
};
//---> bar light-guide lengthes (cm) for each of TOF2GC::SCBTPN types :
geant TOF2DBc::_lglen[TOF2GC::SCBTPN]={
  0.,0.,0.,0.,0.,0.,0.,0.,0.,0.
};
//---> plane rotation mask for 4 layers (1/0 -> rotated/not):
integer TOF2DBc::_plrotm[TOF2GC::SCLRS]={
  1,0,0,1
};
//---> honeycomb supporting str. data:
geant TOF2DBc::_supstr[10]={
  0.,0.,0.,0.,0.,0.,0.,0.,0.,0.
};
//---> sc. plane design parameters:
geant TOF2DBc::_plnstr[15]={
  0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.
};
//---> Initialize TOF MC/RECO "time-stable" parameters :
//
  geant TOF2DBc::_edep2ph={8000.};// edep(Mev)-to-Photons convertion
  geant TOF2DBc::_seresp=1.45;    // PMT Single elec.responce (MP=Mean,mV)
  geant TOF2DBc::_seresv=0.87;     // PMT Single elec.responce variance(relative to Mean)
  geant TOF2DBc::_adc2q=1.;       // Generic ADCch->Q conv.factor (pC/chan)
  geant TOF2DBc::_fladctb=0.1;    // MC "flash-ADC" internal time binning (ns)
  geant TOF2DBc::_shaptb=1.;      // MC shaper internal time binning (ns)
  geant TOF2DBc::_shrtim=1.0;     // MC shaper pulse rise time (ns)(exp)
  geant TOF2DBc::_shctim=400.;    // MC max integration time(max.gate width)(ns)
  geant TOF2DBc::_shftim=63.;     // MC shaper pulse fall time (ns)(exp). MC only !!!don't need now
  geant TOF2DBc::_strflu=1.5;     // Stretcher "end-mark" time fluctuations (ns)
  geant TOF2DBc::_tdcbin[4]={
    1.,                            // pipe/line TDC binning for fast-tdc meas.
    1.,                            // pipe/line TDC binning for slow-tdc meas.
    0.5,                           // supl.DAQ binning for charge meas.(in ADC-chan units)
    0.                             // spare
  };
  geant TOF2DBc::_daqpwd[15]={
    50.,    // (0)pulse width of "z>=1(FT)" trig. signal (ns) (anode)
    50.,    // (1)pulse width of "z>1" trig. signal
    50.,    // (2)pulse width of "z>2" trig. signal (dinode)
    10.,    // (3)double pulse resolution of fast(history) TDC (ns)
    2000.,  // (4)min. double pulse resolution of slow TDC (ns)
    500.,   // (5)dead time of anode TovT measurements (ns)
    500.,   // (6)dead time of dinode TovT measurements (ns)
    2.,     // (7)dead time for making of "z>=1(FT)" trig. signal (ns)
    2.,     // (8)dead time for making of "z>1" trig. (ns)
    2.,     // (9)dead time for making of "z>2" trig. signal (ns)
    0.02,   // (10)fast discr. internal accuracy(ns) + (?) to have exp.resolution
    2.,     // (11)min. pulse duration (ns) of fast discr.(comparator) (for dinode also)
    18.,    // (12)(as dummy gap in s-TDC pulse)
    10.36,  // (13)thresh.(pC) in A-integrator(TovT conversion) 
    10.36   // (14)thresh.(pC) in D-integrator(TovT conversion) 
  };
  geant TOF2DBc::_trigtb=0.25;  // MC time-bin in logic(trig) pulse handling (ns)
  geant TOF2DBc::_di2anr=0.2;  // dinode->anode signal ratio (def,mc-data) not used now !
  geant TOF2DBc::_strrat=40.;  // stretcher ratio (default,mc-data) not used now !
  geant TOF2DBc::_strjit1=0.;  // "start"-pulse jitter at stretcher input
  geant TOF2DBc::_strjit2=0.;  // "stop"(FT)-pulse jitter at stretcher input
  geant TOF2DBc::_accdel=5000.;//Lev-1 signal delay with respect to FT (ns)
  geant TOF2DBc::_ftdelf=65.;  // FastTrigger (FT) fixed (by h/w) delay (ns)
  geant TOF2DBc::_ftdelm=200.; // FT max delay (allowed by stretcher logic) (ns)
  geant TOF2DBc::_fstdcd=28.;   // Same hit(up-edge) relative delay in fast-slow TDC
  geant TOF2DBc::_fatdcd=5.;   // Same hit(up-edge) relative delay in fast-A(D) TDC
  integer TOF2DBc::_pbonup=1;  // set phase-bit for leading(up) edge (yes/no->1/0) 
//
//  member functions :
//
// ---> function to read geomconfig-files 
//                  Called from TOFgeom :
  void TOF2DBc::readgconf(){
    int i;
    char fname[80];
    char name[80]="tof2geomv";
    char vers1[3]="1";
    if (strstr(AMSJob::gethead()->getsetup(),"AMS02")){
          cout <<" TOFGeom-I-AMS02 setup selected."<<endl;
          strcat(name,vers1);
    }
    else
    {
          cout <<" TOFGeom-E-Unknown setup !!!"<<endl;
	  exit(10);
    }
    strcat(name,".dat");
    if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
    if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
    strcat(fname,name);
    cout<<"TOF2DBc::readgconf: Open file : "<<fname<<'\n';
    ifstream tcfile(fname,ios::in); // open needed config-file for reading
    if(!tcfile){
      cerr <<"TOFgeom-read: missing geomconfig-file "<<fname<<endl;
      exit(1);
    }
    for(int ic=0;ic<TOF2GC::SCBLMX;ic++) tcfile >> _brtype[ic];
    for(i=0;i<TOF2GC::SCLRS;i++) tcfile >> _plrotm[i];
    for(i=0;i<TOF2GC::SCBTPN;i++) tcfile >> _brlen[i];
    for(i=0;i<TOF2GC::SCBTPN;i++) tcfile >> _lglen[i];
    for(i=0;i<10;i++) tcfile >> _supstr[i];
    for(i=0;i<15;i++) tcfile >> _plnstr[i];
  }
//---
  integer TOF2DBc::brtype(integer ilay, integer ibar){
    #ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(ilay>=0 && ilay < TOF2GC::SCLRS);
        assert(ibar>=0 && ibar < TOF2GC::SCBRS[ilay]);
      }
    #endif
    int cnum;
    cnum=TOF2GC::SCMXBR*ilay+ibar;
    return _brtype[cnum];
  }
//
  geant TOF2DBc::brlen(integer ilay, integer ibar){
    int cnum;
    cnum=TOF2GC::SCMXBR*ilay+ibar;
    int btyp=_brtype[cnum];
    #ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(btyp>0 && btyp <= TOF2GC::SCBTPN);
      }
    #endif
    return _brlen[btyp-1];
  }
//
  geant TOF2DBc::lglen(integer ilay, integer ibar){
    int cnum;
    cnum=TOF2GC::SCMXBR*ilay+ibar;
    int btyp=_brtype[cnum];
    #ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(btyp>0 && btyp <= TOF2GC::SCBTPN);
      }
    #endif
    return _lglen[btyp-1];
  }
//
  integer TOF2DBc::plrotm(integer ilay){
    #ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(ilay>=0 && ilay < TOF2GC::SCLRS);
      }
    #endif
    return _plrotm[ilay];
  }
//
  geant TOF2DBc::supstr(integer i){
    #ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(i>0 && i <= 10);
      }
    #endif
    return _supstr[i-1];
  }
//
  geant TOF2DBc::plnstr(integer i){
    #ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(i>0 && i <= 15);
      }
    #endif
    return _plnstr[i-1];
  }
// ===> function to get Z-position of scint. bar=ib in layer=il
//
  geant TOF2DBc::getzsc(integer il, integer ib){
    #ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(il>=0 && il < TOF2GC::SCLRS);
        assert(ib>=0 && ib < TOF2GC::SCBRS[il]);
      }
    #endif
  geant dz,zc;
  dz=_plnstr[5]+2.*_plnstr[6];// counter thickness
  if(il==0)
    zc=_supstr[0]-_plnstr[0]-dz/2.;
  else if(il==1)
    zc=_supstr[0]-_plnstr[1]-dz/2.;
  else if(il==2)
    zc=_supstr[1]+_plnstr[1]+dz/2.;
  else if(il==3)
    zc=_supstr[1]+_plnstr[0]+dz/2.;
  if(il==0)zc=zc-(ib%2)*_plnstr[2];
  else if(il==3)zc=zc+(ib%2)*_plnstr[2];
  else if(il==1)zc=zc-((ib+1)%2)*_plnstr[2];
  else if(il==2)zc=zc+((ib+1)%2)*_plnstr[2];
  return(zc);
  }  
// ===> function to get transv. position of scint. bar=ib in layer=il
//
  geant TOF2DBc::gettsc(integer il, integer ib){
    #ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(il>=0 && il < TOF2GC::SCLRS);
        assert(ib>=0 && ib < TOF2GC::SCBRS[il]);
      }
    #endif
  static geant dx;
  geant x,co[2],dxt;
  dx=_plnstr[4]+2.*_plnstr[7];//width of sc.counter(bar+cover)
  dxt=(TOF2GC::SCBRS[il]-1)*(dx-_plnstr[3]);//first-last sc.count. bars distance
  if(il<2){
    co[0]=_supstr[2];// <--top TOF-subsystem X-shift
    co[1]=_supstr[3];// <--top TOF-subsystem Y-shift
  }
  if(il>1){
    co[0]=_supstr[4];// <--bot TOF-subsystem X-shift
    co[1]=_supstr[5];// <--bot TOF-subsystem Y-shift
  }
  if(_plrotm[il]==0){  // <-- unrotated planes
    x=co[0]-dxt/2.+ib*(dx-_plnstr[3]);
  }
  if(_plrotm[il]==1){  // <-- rotated planes
    x=co[1]-dxt/2.+ib*(dx-_plnstr[3]);
  }
  return(x);
  }
// functions to get MC/RECO parameters:
//
  geant TOF2DBc::edep2ph(){return _edep2ph;}
  geant TOF2DBc::fladctb(){return _fladctb;}
  geant TOF2DBc::shaptb(){return _shaptb;}
  geant TOF2DBc::shrtim(){return _shrtim;}
  geant TOF2DBc::shftim(){return _shftim;}
  geant TOF2DBc::shctim(){return _shctim;}
//
  geant TOF2DBc::strrat(){return _strrat;}
//
  geant TOF2DBc::strflu(){return _strflu;}
//
  geant TOF2DBc::strjit1(){return _strjit1;}
//
  geant TOF2DBc::strjit2(){return _strjit2;}
//
  geant TOF2DBc::di2anr(){return _di2anr;}
//
  geant TOF2DBc::accdel(){return _accdel;}
  geant TOF2DBc::ftdelf(){return _ftdelf;}
  geant TOF2DBc::ftdelm(){return _ftdelm;}
  geant TOF2DBc::fstdcd(){return _fstdcd;}
  geant TOF2DBc::fatdcd(){return _fatdcd;}
  geant TOF2DBc::seresp(){return _seresp;}
  geant TOF2DBc::seresv(){return _seresv;}
  geant TOF2DBc::adc2q(){return _adc2q;}
  integer TOF2DBc::pbonup(){return _pbonup;}
//
  geant TOF2DBc::tdcbin(int i){
    #ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(i>=0 && i<4);
      }
    #endif
    return _tdcbin[i];}
//
  geant TOF2DBc::daqpwd(int i){
    #ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(i>=0 && i<15);
      }
    #endif
    return _daqpwd[i];}
//
  geant TOF2DBc::trigtb(){return _trigtb;}
//===============================================================================
  TOF2Brcal::TOF2Brcal(integer sid, integer sta[2], geant gna[2], geant gnd[2],
           geant asl, geant tth, geant stra[2][2], geant fstd, geant t0,  
           geant sl, geant sls[2], geant tdiff, geant td2p[2], geant mip,
           integer nsp, geant ysc[], geant relo[], geant upar[5], geant hlr[2],
	   geant a2q){
    softid=sid;
    status[0]=sta[0];
    status[1]=sta[1];
    gaina[0]=gna[0];
    gaina[1]=gna[1];
    gaind[0]=gnd[0];
    gaind[1]=gnd[1];
    asatl=asl;
    tthr=tth;
    strat[0][0]=stra[0][0];
    strat[0][1]=stra[0][1];
    strat[1][0]=stra[1][0];
    strat[1][1]=stra[1][1];
    fstrd=fstd;
    tzero=t0;
    slope=sl;
    slops[0]=sls[0];
    slops[1]=sls[1];
    yctdif=tdiff;
    td2pos[0]=td2p[0];
    td2pos[1]=td2p[1];
    mip2q=mip;
    int i;
    nscanp=nsp;
    for(i=0;i<nsp;i++){
      yscanp[i]=ysc[i];
      relout[i]=relo[i];
    }
    for(i=0;i<5;i++){unipar[i]=upar[i];}
    for(i=0;i<2;i++){ahlcr[i]=hlr[i];}
    adccf=a2q;
  }
//  ================= TOF2Brcal class functions ======================= :
//
void TOF2Brcal::build(){// create scbrcal-objects for each sc.bar
//
 int lsflg(1);//0/1->use common/individual values for Lspeed 
 integer i,j,k,ila,ibr,ip,ibrm,isd,isp,nsp,ibt,cnum,dnum,mult;
 geant scp[TOF2GC::SCANPNT];
 geant rlo[TOF2GC::SCANPNT];// relat.(to Y=0) light output
 integer lps=1000;
 geant ef1[TOF2GC::SCANPNT],ef2[TOF2GC::SCANPNT];
 integer i1,i2,sta[2],stat[TOF2GC::SCBLMX][2];
 geant r,eff1,eff2;
 integer sid,brt;
 geant gna[2],gnd[2],qath,qdth,a2dr[2],tth,strat[2][2];
 geant slope,slpf,fstrd,tzer,tdif,mip2q,speedl,lspeeda[TOF2GC::SCLRS][TOF2GC::SCMXBR];
 geant tzerf[TOF2GC::SCLRS][TOF2GC::SCMXBR],tdiff[TOF2GC::SCBLMX];
 geant slops[2],slops1[TOF2GC::SCLRS][TOF2GC::SCMXBR],slops2[TOF2GC::SCLRS][TOF2GC::SCMXBR];
 geant strf[TOF2GC::SCBLMX][2],strof[TOF2GC::SCBLMX][2];
 geant an2di[TOF2GC::SCBLMX][2],gaina[TOF2GC::SCBLMX][2],gaind[TOF2GC::SCBLMX][2],m2q[TOF2GC::SCBTPN];
 geant aprofp[TOF2GC::SCBTPN][TOF2GC::SCPROFP],apr[TOF2GC::SCPROFP],hblen,p1,p2,p3,p4,p5,nom,denom; 
 char fname[80];
 char name[80];
 geant a2q,td2p[2];
 char vers1[3]="mc";
 char vers2[3]="rl";
 int mrfp;
//
 geant asatl=20.;//(mev,~20MIPs),if E-dinode(1-end) higher - use it instead
//                                 of anode measurements
//---> TovT-electronics calibration for MC:
//
 geant ahlr[2]={10.,10.};
 geant cahlr[TOF2GC::SCBLMX][2];// array of counter hi/lo ch. ratious from ext.file
//------------------------------
  char in[2]="0";
  char inum[11];
  int ctyp,ntypes,mcvern[10],rlvern[10];
  int mcvn,rlvn,dig;
//
  strcpy(inum,"0123456789");
//
// ---> read list of calibration-type-versions  list (menu-file) :
//
  integer cfvn;
  cfvn=TFCAFFKEY.cfvers%1000;
  strcpy(name,"tof2cvlist");// basic name for file of cal-files list 
  dig=cfvn/100;
  in[0]=inum[dig]; 
  strcat(name,in);
  dig=(cfvn%100)/10;
  in[0]=inum[dig]; 
  strcat(name,in);
  dig=cfvn%10;
  in[0]=inum[dig]; 
  strcat(name,in);
  strcat(name,".dat");
//
  if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"TOF2Brcal::build: Open file  "<<fname<<'\n';
  ifstream vlfile(fname,ios::in); // open needed tdfmap-file for reading
  if(!vlfile){
    cerr <<"TOF2Brcal_build:: missing verslist-file "<<fname<<endl;
    exit(1);
  }
  vlfile >> ntypes;// total number of calibr. file types in the list
  for(i=0;i<ntypes;i++){
    vlfile >> mcvern[i];// first number - for mc
    vlfile >> rlvern[i];// second number - for real
  }
  vlfile.close();
//
//------------------------------
//
//   --->  Read lspeed/tdiffs calibration file :
//
 ctyp=3;
 strcpy(name,"tof2tdcf");
 mcvn=mcvern[ctyp-1]%1000;
 rlvn=rlvern[ctyp-1]%1000;
 if(AMSJob::gethead()->isMCData()) //      for MC-event
 {
       cout <<" TOF2Brcal_build: MC-Tdif-calibration is used"<<endl;
       dig=mcvn/100;
       in[0]=inum[dig];
       strcat(name,in);
       dig=(mcvn%100)/10;
       in[0]=inum[dig];
       strcat(name,in);
       dig=mcvn%10;
       in[0]=inum[dig];
       strcat(name,in);
       strcat(name,vers1);
 }
 else                              //      for Real events
 {
       cout <<" TOF2Brcal_build: REAL-Tdif-calibration is used"<<endl;
       dig=rlvn/100;
       in[0]=inum[dig];
       strcat(name,in);
       dig=(rlvn%100)/10;
       in[0]=inum[dig];
       strcat(name,in);
       dig=rlvn%10;
       in[0]=inum[dig];
       strcat(name,in);
       strcat(name,vers2);
 }
//
 strcat(name,".dat");
 if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
 if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
 strcat(fname,name);
 cout<<"Open file : "<<fname<<'\n';
 ifstream tdcfile(fname,ios::in); // open  file for reading
 if(!tdcfile){
   cerr <<"TOF2Brcal_build: missing Lspeed/Tdif-file "<<fname<<endl;
   exit(1);
 }
//
// ---> read Lspeed/Tdiffs:
//
 if(lsflg){// read bar indiv.Lspeed
   for(ila=0;ila<TOF2GC::SCLRS;ila++){   
     for(ibr=0;ibr<TOF2GC::SCMXBR;ibr++){  
       tdcfile >> lspeeda[ila][ibr];
     }
   }
 }
 else tdcfile >> speedl;// read average Lspeed
 for(ila=0;ila<TOF2GC::SCLRS;ila++){   
   for(ibr=0;ibr<TOF2GC::SCMXBR;ibr++){  
     cnum=ila*TOF2GC::SCMXBR+ibr; // sequential counters numbering(0-55)
     tdcfile >> tdiff[cnum];
   }
 }
 tdcfile.close();
//------------------------------
//
//   --->  Read slewing_slope/tzeros calibration file :
//
 ctyp=4;
 strcpy(name,"tof2tzcf");
 mcvn=mcvern[ctyp-1]%1000;
 rlvn=rlvern[ctyp-1]%1000;
 if(AMSJob::gethead()->isMCData()) //      for MC-event
 {
       cout <<" TOF2Brcal_build: MC-T0-calibration is used"<<endl;
       dig=mcvn/100;
       in[0]=inum[dig];
       strcat(name,in);
       dig=(mcvn%100)/10;
       in[0]=inum[dig];
       strcat(name,in);
       dig=mcvn%10;
       in[0]=inum[dig];
       strcat(name,in);
       strcat(name,vers1);
 }
 else                              //      for Real events
 {
       cout <<" TOF2Brcal_build: REAL-T0-calibration is used"<<endl;
       dig=rlvn/100;
       in[0]=inum[dig];
       strcat(name,in);
       dig=(rlvn%100)/10;
       in[0]=inum[dig];
       strcat(name,in);
       dig=rlvn%10;
       in[0]=inum[dig];
       strcat(name,in);
       strcat(name,vers2);
 }
//
 strcat(name,".dat");
 if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
 if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
 strcat(fname,name);
 cout<<"Open file : "<<fname<<'\n';
 ifstream tzcfile(fname,ios::in); // open  file for reading
 if(!tzcfile){
   cerr <<"TOF2Brcal_build: missing Tzero/Slope(s)-file "<<fname<<endl;
   exit(1);
 }
//
// ---> read Slope/Tzero's:
 tzcfile >> slpf;
 for(ila=0;ila<TOF2GC::SCLRS;ila++){ 
   for(ibr=0;ibr<TOF2GC::SCMXBR;ibr++){
     tzcfile >> tzerf[ila][ibr];
   } 
 }
 tzcfile.close();
//
//-------------------------------------------- 
//
//   --->  Read stretcher_ratio/status file :
//
 ctyp=2;
 strcpy(name,"tof2srcf");
 mcvn=mcvern[ctyp-1]%1000;
 rlvn=rlvern[ctyp-1]%1000;
 if(AMSJob::gethead()->isMCData())           // for MC-event
 {
   cout <<" TOF2Brcal_build: str_ratio/status data for MC-events are selected."<<endl;
   dig=mcvn/100;
   in[0]=inum[dig];
   strcat(name,in);
   dig=(mcvn%100)/10;
   in[0]=inum[dig];
   strcat(name,in);
   dig=mcvn%10;
   in[0]=inum[dig];
   strcat(name,in);
   strcat(name,vers1);
 }
 else                                       // for Real events
 {
   cout <<" TOF2Brcal_build: str_ratio-calib. for Real-events selected."<<endl;
   dig=rlvn/100;
   in[0]=inum[dig];
   strcat(name,in);
   dig=(rlvn%100)/10;
   in[0]=inum[dig];
   strcat(name,in);
   dig=rlvn%10;
   in[0]=inum[dig];
   strcat(name,in);
   strcat(name,vers2);
 }
   strcat(name,".dat");
   if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
   if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
   strcat(fname,name);
   cout<<"Open file : "<<fname<<'\n';
   ifstream scfile(fname,ios::in); // open str_ratio-file for reading
   if(!scfile){
     cerr <<"TOF2Brcal_build: missing str_ratio/status-file "<<fname<<endl;
     exit(1);
   }
//---> read str_ratio/status:
//
   for(ila=0;ila<TOF2GC::SCLRS;ila++){   // <-------- loop over layers
   for(ibr=0;ibr<TOF2GC::SCMXBR;ibr++){  // <-------- loop over bar in layer
     cnum=ila*TOF2GC::SCMXBR+ibr; // sequential counter numbering(0-55)
     scfile >> strf[cnum][0];
     scfile >> strof[cnum][0];
     scfile >> stat[cnum][0];
     scfile >> strf[cnum][1];
     scfile >> strof[cnum][1];
     scfile >> stat[cnum][1];
   } // --- end of bar loop --->
   } // --- end of layer loop --->
   scfile.close();
//---------------------------------------------
//
//   --->  Read hi/low-ratios, gains, mip2q and A-profile param. calib.file :
//
 ctyp=5;
 strcpy(name,"tof2chcf");
 mcvn=mcvern[ctyp-1]%1000;
 rlvn=rlvern[ctyp-1]%1000;
 if(AMSJob::gethead()->isMCData())           // for MC-event
 {
   cout <<" TOF2Brcal_build: hilow_ratio/gain-calib. for MC-events selected."<<endl;
   dig=mcvn/100;
   in[0]=inum[dig];
   strcat(name,in);
   dig=(mcvn%100)/10;
   in[0]=inum[dig];
   strcat(name,in);
   dig=mcvn%10;
   in[0]=inum[dig];
   strcat(name,in);
   strcat(name,vers1);
 }
 else                                       // for Real events
 {
   cout <<" TOF2Brcal_build: hilow_ratio/gain/mip2q-calib. for Real-events selected."<<endl;
   dig=rlvn/100;
   in[0]=inum[dig];
   strcat(name,in);
   dig=(rlvn%100)/10;
   in[0]=inum[dig];
   strcat(name,in);
   dig=rlvn%10;
   in[0]=inum[dig];
   strcat(name,in);
   strcat(name,vers2);
 }
   strcat(name,".dat");
   if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
   if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
   strcat(fname,name);
   cout<<"Open file : "<<fname<<'\n';
   ifstream gcfile(fname,ios::in); // open hi_low_r/gain/mip2q-file for reading
   if(!gcfile){
     cerr <<"TOF2Brcal_build: missing hi_low_r/gain/mip2q-file "<<fname<<endl;
     exit(1);
   }
// ---> read hi/low-ratios:
//
   for(ila=0;ila<TOF2GC::SCLRS;ila++){   // <-------- loop over layers
     for(ibr=0;ibr<TOF2GC::SCMXBR;ibr++){  // read side-1
       cnum=ila*TOF2GC::SCMXBR+ibr; 
       gcfile >> cahlr[cnum][0];
     }
     for(ibr=0;ibr<TOF2GC::SCMXBR;ibr++){  // read side-2
       cnum=ila*TOF2GC::SCMXBR+ibr; 
       gcfile >> cahlr[cnum][1];
     }
   } // --- end of layer loop --->
//
// ---> read gains(anode):
//
   for(ila=0;ila<TOF2GC::SCLRS;ila++){   // <-------- loop over layers
     for(ibr=0;ibr<TOF2GC::SCMXBR;ibr++){  // read side-1
       cnum=ila*TOF2GC::SCMXBR+ibr; 
       gcfile >> gaina[cnum][0];
     }
     for(ibr=0;ibr<TOF2GC::SCMXBR;ibr++){  // read side-2
       cnum=ila*TOF2GC::SCMXBR+ibr; 
       gcfile >> gaina[cnum][1];
     }
   } // --- end of layer loop --->
//
// ---> read mip2q's:
//
   for(ibt=0;ibt<TOF2GC::SCBTPN;ibt++){  // <-------- loop over bar-types
     gcfile >> m2q[ibt];
   }
//
// ---> read A-prof. parameters:
//
   for(ibt=0;ibt<TOF2GC::SCBTPN;ibt++){  // <-------- loop over bar-types
     for(i=0;i<TOF2GC::SCPROFP;i++)gcfile >> aprofp[ibt][i];
   }
//
   gcfile.close();
//   
//---------------------------------------------
//   ===> fill TOFBrcal bank :
//
  for(ila=0;ila<TOF2GC::SCLRS;ila++){   // <-------- loop over layers
  for(ibr=0;ibr<TOF2GC::SCMXBR;ibr++){  // <-------- loop over bar in layer
    brt=TOF2DBc::brtype(ila,ibr);
    if(brt==0)continue; // skip missing counters
    hblen=0.5*TOF2DBc::brlen(ila,ibr);
    cnum=ila*TOF2GC::SCMXBR+ibr; // sequential counter numbering(0-55)
    nsp=TOF2Scan::scmcscan[cnum].getnscp();//read number of scan-points from scmcscan-object
    TOF2Scan::scmcscan[cnum].getscp(scp);//read scan-points from scmcscan-object
// read from file or DB:
    gna[0]=gaina[cnum][0];
    gna[1]=gaina[cnum][1];
    gnd[0]=gna[0];// tempor
    gnd[1]=gna[1];
    tth=TOF2Varp::tofvpar.daqthr(0); // (mV), time-discr. threshold
    mip2q=m2q[brt-1];//(pC/mev),dE(mev)_at_counter_center->Q(pC)_at_PM_anode(2x3-sum)
    fstrd=TOF2Varp::tofvpar.sftdcd();//(ns),same hit(up-edge)delay in f/sTDC(const. for now)
//
//-->prepare position correction array (valid for local !!! r.c.):
//
      for(i=0;i<5;i++)apr[i]=aprofp[brt-1][i];
      p1=aprofp[brt-1][0];
      p2=aprofp[brt-1][1];
      p3=aprofp[brt-1][2];
      p4=aprofp[brt-1][3];
      p5=aprofp[brt-1][4];
      mrfp=nsp/2;//central point (coo=0.)
      denom=p1*(exp(-(hblen+scp[mrfp])/p4)+p3*exp(-(hblen+scp[mrfp])/p5))
           +p2*(exp(-(hblen-scp[mrfp])/p4)+p3*exp(-(hblen-scp[mrfp])/p5));
      for(isp=0;isp<nsp;isp++){ // fill 2-ends rel. l.output at scan-points
        nom=p1*(exp(-(hblen+scp[isp])/p4)+p3*exp(-(hblen+scp[isp])/p5))
           +p2*(exp(-(hblen-scp[isp])/p4)+p3*exp(-(hblen-scp[isp])/p5));
        rlo[isp]=nom/denom;
      }
//
    sid=100*(ila+1)+(ibr+1);
    strat[0][0]=strf[cnum][0];//stretcher param. from ext.file
    strat[1][0]=strf[cnum][1];
    strat[0][1]=strof[cnum][0];
    strat[1][1]=strof[cnum][1];
    sta[0]=stat[cnum][0];
    sta[1]=stat[cnum][1];
    slope=slpf;// common slope from ext. file
//    slops[0]=slops1[ila][ibr];// indiv.slopes from ext.file
//    slops[1]=slops2[ila][ibr];
    slops[0]=1.;// default indiv.slopes
    slops[1]=1.;
    tzer=tzerf[ila][ibr];//was read from ext. file
    tdif=tdiff[cnum];//was read from ext. file
    if(lsflg){
      td2p[0]=lspeeda[ila][ibr];//indiv.bar speed of the light from external file
    } 
    else td2p[0]=speedl;//average speed of the light from external file
    td2p[1]=TOF2Varp::tofvpar.lcoerr();//error on longit. coord. measurement(cm)
    ahlr[0]=cahlr[cnum][0];// from ext.file
    ahlr[1]=cahlr[cnum][1];
    a2q=TOF2DBc::adc2q();//ADCch->Q generic factor from MC
    scbrcal[ila][ibr]=TOF2Brcal(sid,sta,gna,gnd,asatl,tth,strat,fstrd,tzer,slope,
                               slops,tdif,td2p,mip2q,nsp,scp,rlo,apr,ahlr,a2q);
//
  } // --- end of bar loop --->
  } // --- end of layer loop --->
//
}
//-----------------------------------------------------------------------
//
geant TOF2Brcal::adc2mip(int hlf, number amf[2]){ // side ADC(ch) -> Etot(Mev)
  number q(0),qt(0);
  for(int isd=0;isd<2;isd++){
    if(status[isd]>=0){
      q2t2q(1,isd,hlf,amf[isd],q);//ADC->Q
      qt+=(q/gaina[isd]);// Q->Q_gain_corrected
    }
  }
  qt=qt/mip2q; // Q(pC)->Mev
  return geant(qt);
}
//------
void TOF2Brcal::adc2q(int hlf, number amf[2], number qs[2]){// side ADC(ch) -> Q(pC)
//                                                 to use in calibr. program 
  for(int isd=0;isd<2;isd++){
    qs[isd]=0.;
    if(status[isd]>=0){
      q2t2q(1,isd,hlf,amf[isd],qs[isd]);//ADC->Q
    }
  }
}
//------
void TOF2Brcal::q2t2q(int cof, int sdf, int hlf, number &adc, number &q){  
// Q(pC) <-> ADC(ch,float) to use in sim./rec. programs (cof=0/1-> Q->ADC/ADC->Q)
//                                                 (sdf=0/1-> bar side 1/2   )
//                                             (hlf=0/1-> for high(anode)/low(dinode))
//
  if(cof==0){ // Q->ADCch
    if(hlf==0){//     <-- for hi_chan(anode)
      adc=q/adccf;
    }
    else{//           <-- for low_ch(dynode)
      adc=q/adccf/ahlcr[sdf];
    }
  }
  else{       // ADCch->Q
    if(hlf==0){//       for hi_chan(anode)
      q=adc*adccf;
    }
    else{//             for low_ch(dynode)
      q=adc*adccf*ahlcr[sdf];
    }
  }
}
//-----
geant TOF2Brcal::poscor(geant point){
//(return light-out corr.factor, input 'point' is Y-coord. in bar loc.r.s.)
  integer nmx=nscanp-1;
  int i;
  geant corr;
  if(point >= yscanp[nmx])corr=relout[nmx]+(relout[nmx]-relout[nmx-1])
                         *(point-yscanp[nmx])/(yscanp[nmx]-yscanp[nmx-1]);
  if(point < yscanp[0])corr=relout[0]+(relout[1]-relout[0])
                         *(point-yscanp[0])/(yscanp[1]-yscanp[0]);
  for(i=0;i<nmx;i++){
    if(point>=yscanp[i] && point<yscanp[i+1])
      corr=relout[i]+(relout[i+1]-relout[i])*(point-yscanp[i])
          /(yscanp[i+1]-yscanp[i]);
  }
  return corr;//you should devide signal by this value later
}
//------
geant TOF2Brcal::tm2t(number tmf[2], number amf[2], int hlf){//(2-sides_times/ADC)->Time (ns)
  number time,qs,uv(0);
  if(status[0]>=0 && status[1]>=0){
    for(int isd=0;isd<2;isd++){
      q2t2q(1,isd,hlf,amf[isd],qs);// ADCch->Q
      uv+=slops[isd]/sqrt(qs);// summing slops/sqrt(Q), works sl.better
    }
  }
//  uv=0.;// tempor
  time=0.5*(tmf[0]+tmf[1])+tzero+slope*uv;
  return geant(time); 
}
//-----
void TOF2Brcal::tmd2p(number tmf[2], number amf[2], int hlf,
                              geant &co, geant &eco){//(time-diff)->loc.coord/err(cm)
  number coo,qs,uv(0);
  if(status[0]>=0 && status[1]>=0){
    for(int isd=0;isd<2;isd++){
      q2t2q(1,isd,hlf,amf[isd],qs);// ADCch->Q
      uv+=(1-2*isd)*slops[isd]/sqrt(qs);// subtr slops/sqrt(Q)
    }
  }
//  uv=0.;//tempor
  coo=-(0.5*(tmf[0]-tmf[1])+slope*uv-yctdif);
//common "-" is due to the fact that Tmeas=Ttrig-Tabs and coo-loc is prop. to Tabs1-Tabs2
  co=td2pos[0]*geant(coo);//coo(ns)->cm                    
  eco=td2pos[1];
}
//-----
void TOF2Brcal::td2ctd(number tdo, number amf[2], int hlf,
                              number &tdc){//td0=0.5*(t[0]-t[1])->tdc (A-corrected)
  number qs,uv(0);
  if(status[0]>=0 && status[1]>=0){
    for(int isd=0;isd<2;isd++){
      q2t2q(1,isd,hlf,amf[isd],qs);// ADCch->Q
      uv+=(1-2*isd)*slops[isd]/sqrt(qs);// subtr slops/sqrt(Q)
    }
  }
  tdc=tdo+slope*uv;
}
//==========================================================================
//
void TOFBPeds::build(){// create TOFBPeds-objects for each sc.bar
//
  int i,j,ila,ibr,cnum,brt;
  integer sid;
  char fname[80];
  char name[80];
  geant cpeds[TOF2GC::SCBLMX][4];// array of counter peds
  geant csigs[TOF2GC::SCBLMX][4];// array of counter sigmas
  geant peds[4],sigs[4];
//
//   --->  Read high/low pedestals file :
//
  strcpy(name,"tof2peds");
  if(AMSJob::gethead()->isMCData())           // for MC-event
  {
    cout <<" TOFBPeds_build: default MC peds-file is used..."<<endl;
    strcat(name,"mc");
  }
  else                                       // for Real events
  {
    cout <<" TOFBPeds_build: default RealData peds-file is used..."<<endl;
    strcat(name,"rl");
  }
  strcat(name,".dat");
  if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"Open file : "<<fname<<'\n';
  ifstream icfile(fname,ios::in); // open pedestals-file for reading
  if(!icfile){
    cerr <<"TOFBPeds_build: missing default pedestals-file "<<fname<<endl;
    exit(1);
  }
//
  for(ila=0;ila<TOF2GC::SCLRS;ila++){   // <-------- loop over layers
  for(ibr=0;ibr<TOF2GC::SCMXBR;ibr++){  // <-------- loop over bar in layer
    cnum=ila*TOF2GC::SCMXBR+ibr; // sequential counter numbering(0-47)
    for(i=0;i<4;i++)icfile >> cpeds[cnum][i];// sequence: side1(h,l),side2(h,l)
    for(i=0;i<4;i++)icfile >> csigs[cnum][i];// sequence: side1(h,l),side2(h,l)
  } // --- end of bar loop --->
  } // --- end of layer loop --->
  icfile.close();
//---------------------------------------------
//   ===> fill TOFBPeds bank :
//
  for(ila=0;ila<TOF2GC::SCLRS;ila++){   // <-------- loop over layers
  for(ibr=0;ibr<TOF2GC::SCMXBR;ibr++){  // <-------- loop over bar in layer
    brt=TOF2DBc::brtype(ila,ibr);
    if(brt==0)continue; // skip missing counters
    cnum=ila*TOF2GC::SCMXBR+ibr; // sequential counter numbering(0-47)
    sid=100*(ila+1)+(ibr+1);
    for(i=0;i<4;i++)peds[i]=cpeds[cnum][i];// from ext.file
    for(i=0;i<4;i++)sigs[i]=csigs[cnum][i];// from ext.file
    scbrped[ila][ibr]=TOFBPeds(sid,peds,sigs);
//
  } // --- end of bar loop --->
  } // --- end of layer loop --->
}
//
//==========================================================================
//
//   TOFJobStat static variables definition (memory reservation):
//
integer TOF2JobStat::mccount[TOF2GC::SCJSTA];
integer TOF2JobStat::recount[TOF2GC::SCJSTA];
integer TOF2JobStat::chcount[TOF2GC::SCCHMX][TOF2GC::SCCSTA];
integer TOF2JobStat::brcount[TOF2GC::SCBLMX][TOF2GC::SCCSTA];
integer TOF2JobStat::scdaqbc1[TOF2GC::SCCRAT][2];
integer TOF2JobStat::scdaqbc2[TOF2GC::SCCRAT][2];
integer TOF2JobStat::scdaqbc3[TOF2GC::SCCRAT][2];
integer TOF2JobStat::scdaqbc4[TOF2GC::SCCRAT][2];
//
// function to print Job-statistics at the end of JOB(RUN):
//
void TOF2JobStat::printstat(){
  int il,ib,ic;
  geant rc;
//
  printf("\n");
  printf("    ======================= JOB DAQ-statistics ====================\n");
  printf("\n");
  printf(" ------- node(block) number -------->"); 
  for(ic=0;ic<TOF2GC::SCCRAT;ic++){
      printf("          %1d          ",ic);
  }
  printf("\n");
  printf(" S-block events                         : ");
  for(ic=0;ic<TOF2GC::SCCRAT;ic++){ 
      printf("  r/c:%7d %7d",scdaqbc1[ic][0],scdaqbc1[ic][1]);
  }
  printf("\n");
  printf(" S-block events (nonempty)              : ");
  for(ic=0;ic<TOF2GC::SCCRAT;ic++){
      printf("  r/c:%7d %7d",scdaqbc2[ic][0],scdaqbc2[ic][1]);
  }
  printf("\n");
  printf(" S-block errors (block length mismatch) : ");
  for(ic=0;ic<TOF2GC::SCCRAT;ic++){
      printf("  r/c:%7d %7d",scdaqbc3[ic][0],scdaqbc3[ic][1]);
  }
  printf("\n");
  printf(" S-block errors(conflict with swid-map) : ");
  for(ic=0;ic<TOF2GC::SCCRAT;ic++){
      printf("  r/c:%7d %7d",scdaqbc4[ic][0],scdaqbc4[ic][1]);
  }
  printf("\n");
//
  printf("\n");
  printf("    ====================== JOB TOF-statistics ======================\n");
  printf("\n");
  printf(" MC: entries             : % 6d\n",mccount[0]);
  printf(" MC: TovT->RawEvent OK   : % 6d\n",mccount[1]);
  printf(" MC: Ghits->RawCluster OK: % 6d\n",mccount[2]);
  printf(" RECO-entries            : % 6d\n",recount[0]);
  printf(" H/W TOF-trigger OK      : % 6d\n",recount[1]);
  printf(" RawEvent-validation OK  : % 6d\n",recount[2]);
  printf(" RawEvent->RawCluster OK : % 6d\n",recount[3]);
  printf(" RawCluster->Cluster OK  : % 6d\n",recount[4]);
  if(AMSJob::gethead()->isCalibration()){
    printf(" Entries to TZSl-calibr. : % 6d\n",recount[6]);
    printf(" TZSl: multiplicity OK   : % 6d\n",recount[7]);
    printf(" TZSl: no Anti,Albd,Spks : % 6d\n",recount[8]);
    printf(" TZSl: Tracker mom. OK   : % 6d\n",recount[9]);
    printf(" TZSl: TOF-Tr.match. OK  : % 6d\n",recount[10]);
    printf(" TZSl: TOF-self.match. OK: % 6d\n",recount[20]);
    printf(" Entries to AMPL-calibr. : % 6d\n",recount[11]);
    printf(" AMPL: multiplicity OK   : % 6d\n",recount[12]);
    printf(" AMPL: no interaction    : % 6d\n",recount[13]);
    printf(" AMPL: Track momentum OK : % 6d\n",recount[14]);
    printf(" AMPL: TOF-TRK match OK  : % 6d\n",recount[15]);
    printf(" AMPL: TOF beta-fit OK   : % 6d\n",recount[30]);
    printf(" AMPL: Final check OK    : % 6d\n",recount[31]);
    printf(" Entr to STRR/AVSD-calibr: % 6d\n",recount[16]);
    printf(" Entries to TDIF-calibr. : % 6d\n",recount[17]);
    printf(" TDIF: multiplicity OK   : % 6d\n",recount[18]);
    printf(" TDIF: Tracker OK        : % 6d\n",recount[19]);
  }
  else{
    printf(" Entries to TOFUser      : % 6d\n",recount[21]);
    printf(" TOFU: no ANTI-sectors   : % 6d\n",recount[22]);
    printf(" TOFU: 1bar/layer OK     : % 6d\n",recount[23]);
    printf(" TOFU: Track momentum OK : % 6d\n",recount[24]);
    printf(" TOFU: TOF-TRK match OK  : % 6d\n",recount[25]);
  }
  printf("\n\n");
//
  if(TFREFFKEY.reprtf[0]==0)return;
//
//----------------------------------------------------------
//
  printf("===========> Channels validation report :\n\n");
//
  printf("H/w-status OK (validation) :\n\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      printf(" % 6d",chcount[ic][11]);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      printf(" % 6d",chcount[ic][11]);
    }
    printf("\n\n");
  }
//
  printf("Hist-TDC wrong up/down sequence (percentage) :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][11]);
      if(rc>0.)rc=100.*geant(chcount[ic][12])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][11]);
      if(rc>0.)rc=100.*geant(chcount[ic][12])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Stretcher-TDC wrong up/down sequence (percentage) :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][11]);
      if(rc>0.)rc=100.*geant(chcount[ic][13])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][11]);
      if(rc>0.)rc=100.*geant(chcount[ic][13])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("A-TDC wrong up/down sequence (percentage) :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][11]);
      if(rc>0.)rc=100.*geant(chcount[ic][14])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][11]);
      if(rc>0.)rc=100.*geant(chcount[ic][14])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("D-TDC wrong up/down sequence (percentage) :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][11]);
      if(rc>0.)rc=100.*geant(chcount[ic][15])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][11]);
      if(rc>0.)rc=100.*geant(chcount[ic][15])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
//----------------------------------------------------------
  if(TFREFFKEY.relogic[0]==1)return;// no reco for strr-calibr
//
  printf("==========> Bars reconstruction report :\n\n");
//
  printf("Bar H/w-status OK (at least 1 side is found) :\n\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR+ib;
      printf(" % 6d",brcount[ic][0]);
    }
    printf("\n\n");
  }
//
  printf("Bar 3-measurements found (at least for 1 side) :\n\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR+ib;
      rc=geant(brcount[ic][0]);
      if(rc>0.)rc=geant(brcount[ic][1])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Bar 1-side history 'OK' (for each of the EXISTING 3-measurements sides) :\n\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR+ib;
      rc=geant(brcount[ic][0]);
      if(rc>0.)rc=geant(brcount[ic][2])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Bar 2-sides history 'OK'(if there are 2x3meas. with good history) :\n\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR+ib;
      rc=geant(brcount[ic][0]);
      if(rc>0.)rc=geant(brcount[ic][3])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Bar a/s-TDC matching 'OK' on alive sides :\n\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR+ib;
      rc=geant(brcount[ic][0]);
      if(rc>0.)rc=geant(brcount[ic][4])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Bar d/s-TDC matching 'OK' on alive sides :\n\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR+ib;
      rc=geant(brcount[ic][0]);
      if(rc>0.)rc=geant(brcount[ic][5])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//---------------------------------------------------------
  printf("============> Channels reconstruction report :\n\n");
//
  printf("H/w-status OK :\n\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      printf(" % 6d",chcount[ic][0]);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      printf(" % 6d",chcount[ic][0]);
    }
    printf("\n\n");
  }
//
  printf("Hist-TDC 'ON' :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][1])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][1])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Slow-TDC 'ON' :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][2])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][2])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Anode-ADC 'ON' :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][3])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][3])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Dinode-ADC 'ON' :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][4])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][4])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("History-TDC '1 hit' :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][5])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][5])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Slow-TDC '1 hit' :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][6])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][6])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Anode-ADC '1 hit' :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][7])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][7])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Dinode-ADC '1 hit' :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][8])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][8])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("3-measurements(S-tds + F-tdc + A-tdc) :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][9])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][9])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("High(anode)-channel overflows :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][10])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][10])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
}
//------------------------------------------
void TOF2JobStat::bookhist(){
  int i,j,k,ich,il,ib,ii,jj,ic,is;
  char htit1[60];
  char inum[11];
  char in[2]="0";
//
  strcpy(inum,"0123456789");
//
  if(TFREFFKEY.reprtf[2]!=0 || // Reconstruction histograms
     (AMSJob::gethead()->isMonitoring() & (AMSJob::MTOF | AMSJob::MAll))){ // Monitoring Job
// Book reco-hist
    HBOOK1(1100,"Fast-Slow hit time-difference(SINGLE hist/slow-hit measurements)",80,0.,80.,0.);
    HBOOK1(1107,"TOF+CTC+ANTI data length (16-bit words)",80,1.,1001.,0.);
    HBOOK1(1101,"Time_history:befor_hit dist(ns)",80,0.,160.,0.);
    HBOOK1(1102,"Time_history:after_hit dist(ns)",80,0.,160.,0.);
    HBOOK1(1103,"Time_history: TovT(ns)",80,0.,120.,0.);
    HBOOK1(1104,"High(anode) ADC(ch) signals",100,0.,1000.,0.);
    HBOOK1(1105,"Low(dinode) ADC(ch) signals",100,0.,100.,0.);
    HBOOK1(1108,"Time-hist hit(2edges) multiplicity(all chan)",20,0.,20.,0.);
    HBOOK1(1110,"RawClusterLevel:Total fired layers per event",5,0.,5.,0.);
    HBOOK1(1111,"RawClusterLevel:Layer appearence frequency",4,0.5,4.5,0.);
    HBOOK1(1112,"RawClusterLevel:Configuration(<2;2;>2->missingL)",10,-1.,9.,0.);
    HBOOK1(1113,"RawClusterLevel:SingleBarLayer Configuration(<2;2;>2->missingL)",10,-1.,9.,0.);
    HBOOK1(1114,"RawClusterLevel:Single2SidedBarLayer Configuration(<2;2;>2->missingL)",10,-1.,9.,0.);
    HBOOK1(1115,"Fast-Slow hit time-difference(ALL hist/slow-hit meas.",80,-40.,120.,0.);
    HBOOK1(1116,"dEdX vs bar (norm.inc.,L=1)",14,0.,14.,0.);
    HBOOK1(1117,"dEdX vs bar (norm.inc.,L=2)",14,0.,14.,0.);
    HBOOK1(1118,"dEdX vs bar (norm.inc.,L=3)",14,0.,14.,0.);
    HBOOK1(1119,"dEdX vs bar (norm.inc.,L=4)",14,0.,14.,0.);
    HBOOK1(1095,"Side time diff",50,-5.,5.,0.);
    HBOOK1(1096,"Time diff",50,-5.,5.,0.);
    HBOOK1(1097,"Coord. diff",50,-15.,15.,0.);
    HBOOK1(1098,"Edep. diff",50,-5.,5.,0.);
    if(TFREFFKEY.reprtf[2]>1){
      HBOOK1(1529,"L=1,Edep_anode(mev),corr,ideal evnt",80,0.,24.,0.);
      HBOOK1(1526,"L=1,Edep_anode(mev),corr,ideal evnt",80,0.,240.,0.);
      HBOOK1(1531,"L=1,Edep_dinode(mev),corr,ideal evnt",80,0.,24.,0.);
      HBOOK1(1528,"L=1,Edep_dinode(mev),corr,ideal evnt",80,0.,240.,0.);
      for(il=0;il<TOF2GC::SCLRS;il++){
        for(ib=0;ib<TOF2GC::SCMXBR;ib++){
    	  strcpy(htit1,"dE/dX (norm.inc) for bar(LBB) ");
  	  in[0]=inum[il+1];
  	  strcat(htit1,in);
  	  ii=(ib+1)/10;
	  jj=(ib+1)%10;
	  in[0]=inum[ii];
	  strcat(htit1,in);
	  in[0]=inum[jj];
	  strcat(htit1,in);
	  ich=TOF2GC::SCMXBR*il+ib;
	  HBOOK1(1140+ich,htit1,50,0.,15.,0.);
        }
      }
    }
    if(TFREFFKEY.reprtf[2]>1){
      HBOOK1(1120,"STR-tmp-reference in Crate-1",50,1980.,2030.,0.);
      HBOOK1(1121,"STR-tmp-reference in Crate-2",50,1940.,1990.,0.);
      HBOOK1(1122,"STR-tmp-reference in Crate-3",50,1940.,1990.,0.);
      HBOOK1(1123,"STR-tmp-reference in Crate-4",50,1950.,2000.,0.);
      HBOOK1(1124,"STR-tmp-reference in Crate-5",50,1940.,1990.,0.);
      HBOOK1(1125,"STR-tmp-reference in Crate-6",50,4030.,4080.,0.);
      HBOOK1(1126,"STR-tmp-reference in Crate-7",50,1970.,2020.,0.);
      HBOOK1(1127,"STR-tmp-reference in Crate-8",50,1980.,2030.,0.);
      HBOOK1(1130,"ANODE-tmp-reference in Crate-1",50,0.,100.,0.);
      HBOOK1(1131,"ANODE-tmp-reference in Crate-2",50,0.,100.,0.);
      HBOOK1(1132,"ANODE-tmp-reference in Crate-3",50,0.,100.,0.);
      HBOOK1(1133,"ANODE-tmp-reference in Crate-4",50,0.,100.,0.);
      HBOOK1(1134,"ANODE-tmp-reference in Crate-5",50,0.,100.,0.);
      HBOOK1(1135,"ANODE-tmp-reference in Crate-6",50,0.,100.,0.);
      HBOOK1(1136,"ANODE-tmp-reference in Crate-7",50,0.,100.,0.);
      HBOOK1(1137,"ANODE-tmp-reference in Crate-8",50,0.,100.,0.);
    }
    if(TFREFFKEY.reprtf[4]>0){ // Str.pulse width for selected channel
      ich=TFREFFKEY.reprtf[4];// LBBS
      ic=ich/10;//LBB
      il=ic/100;
      ib=ic%100;
      is=ich%10;
      strcpy(htit1,"Stretcher pulse width-1 for LBBS=");
      in[0]=inum[il];
      strcat(htit1,in);
      in[0]=inum[ib/10];
      strcat(htit1,in);
      in[0]=inum[ib%10];
      strcat(htit1,in);
      in[0]=inum[is];
      strcat(htit1,in);
      HBOOK1(1138,htit1,50,0.,200.,0.);
      strcpy(htit1,"Stretcher pulse width-3 for LBBS=");
      in[0]=inum[il];
      strcat(htit1,in);
      in[0]=inum[ib/10];
      strcat(htit1,in);
      in[0]=inum[ib%10];
      strcat(htit1,in);
      in[0]=inum[is];
      strcat(htit1,in);
      HBOOK1(1139,htit1,60,0.,6000.,0.);
    }
//
    HBOOK1(1532,"(T1-T3)(ns),corr,trl-corr,1b/L evnt",80,1.,9.,0.);
//
    HBOOK1(1533,"L=1,side1/2 raw T-diff(ns),ideal evnt",100,-2.,2.,0.);
    HBOOK1(1554,"L=2,side1/2 raw T-diff(ns),ideal evnt",100,-2.,2.,0.);
    HBOOK1(1543,"L=1,Y-local(longit.coord),ideal evnt",100,-50.,50.,0.);
    HBOOK1(1545,"L=2,Y-local(longit.coord),ideal evnt",100,-50.,50.,0.);
    HBOOK1(1546,"L=3,Y-local(longit.coord),ideal evnt",100,-50.,50.,0.);
    HBOOK1(1547,"L=4,Y-local(longit.coord),ideal evnt",100,-50.,50.,0.);
    HBOOK1(1560,"Bar-time(corected),L=1",80,60.,80.,0.);
    HBOOK1(1561,"Bar-time(corected),L=2",80,60.,80.,0.);
    HBOOK1(1562,"Bar-time(corected),L=3",80,55.,75.,0.);
    HBOOK1(1563,"Bar-time(corected),L=4",80,55.,75.,0.);

    HBOOK1(1534,"(T2-T4)(ns),corr,trl-corr,1b/L evnt",80,1.,9.,0.);
    HBOOK1(1544,"(T1-T3)-(T2-T4),(ns),corr,ideal evnt",80,-4.,4.,0.);
    HBOOK1(1535,"L=1,TOF Eclust(mev)",80,0.,24.,0.);
    HBOOK1(1536,"L=3,TOF Eclust(mev)",80,0.,24.,0.);
    HBOOK1(1537,"L=1,TOF Eclust(10Xscaled,mev)",80,0.,240.,0.);
    HBOOK1(1538,"L=3,TOF Eclust(10Xscaled,mev)",80,0.,240.,0.);
    HBOOK1(1539,"L=2,TOF Eclust(mev)",80,0.,24.,0.);
    HBOOK1(1540,"L=4,TOF Eclust(mev)",80,0.,24.,0.);
    HBOOK1(1541,"L=2,TOF Eclust(10Xscaled,mev)",80,0.,240.,0.);
    HBOOK1(1542,"L=4,TOF Eclust(10Xscaled,mev)",80,0.,240.,0.);
    HBOOK1(1548,"TOFCluster internal E-assimetry",70,-0.2,1.2,0.);
    HBOOK1(1549,"TOFCluster internal Y-match(low ass.,cm)",80,-40.,40.,0.);
    HBOOK2(1550,"Ass vs Dy",80,-40.,40.,60,0.,1.2,0.);
//
    if(TFREFFKEY.relogic[0]==1){ // <==================== STRR-calibration
      HBOOK1(1200,"Stretcher-ratio for indiv. channel,bin.meth",80,35.,55.,0.);
      HBOOK1(1201,"Offsets for indiv. channels,bin.meth",80,-100.,2300.,0.);
      HBOOK1(1202,"Chi2 for indiv. channel,bin.meth",50,0.,10.,0.);
      HBOOK1(1204,"Bin Tin-RMS in Tin-Tout fit,bin.meth",50,0.,10.,0.);
      HBOOK1(1205,"Stretcher-ratio for indiv. channel,pnt.meth",80,35.,55.,0.);
      HBOOK1(1206,"Offsets for indiv. channels,pnt.meth",80,-100.,2300.,0.);
      HBOOK1(1207,"Chi2 for indiv. channel,pnt.meth",50,0.,10.,0.);
      HBOOK1(1208,"Stretcher-ratio for indiv. channel,comb.meth",80,35.,55.,0.);
      HBOOK1(1209,"Offsets for indiv. channels,comb.meth",80,-100.,2300.,0.);
      HBOOK1(1210,"Chi2 for indiv. channel,comb.meth",50,0.,10.,0.);
      
// hist.1600-1711 are booked in init-function for Tin vs Tout correl.!!!(TDLV)
// hist.1720-1790 are booked in init-function for BarRawTime histogr.!!!(TDLV)
    }
    if(TFREFFKEY.relogic[0]==2){// <==================== TDIF-calibr. runs
      HBOOK1(1500,"Part.rigidity from tracker(gv)",80,0.,32.,0.);
    }
    if(TFREFFKEY.relogic[0]==3){ // <==================== TZSL-calibration
      HBOOK1(1500,"Part.rigidity from tracker(gv)",100,0.,25.,0.);
      HBOOK1(1501,"Particle beta(tracker)",100,0.9,1.,0.);
//      HBOOK1(1504,"Particle beta(tracker)",80,0.2,1.,0.);// spare
      HBOOK1(1502,"Particle beta(tof,no angle-corr)",80,0.4,1.2,0.);
      HBOOK1(1220,"Chisq (tof-beta-fit)",50,0.,10.,0.);
      HBOOK1(1221,"Particle beta(tof,mom-cut,angle-corr)",80,0.4,1.2,0.);
      HBOOK1(1506,"Tracks multipl. in calib.events",10,0.,10.,0.);
      HBOOK1(1200,"Res_long.coo(track-sc),L=1",50,-10.,10.,0.);
      HBOOK1(1201,"Res_long.coo(track-sc),L=2",50,-10.,10.,0.);
      HBOOK1(1202,"Res_long.coo(track-sc),L=3",50,-10.,10.,0.);
      HBOOK1(1203,"Res_long.coo(track-sc),L=4",50,-10.,10.,0.);
      HBOOK2(1204,"Res_long. vs track coord.,L1",50,-50.,50.,60,-15.,15.,0.);
      HBOOK2(1205,"Res_long. vs track coord.,L2",50,-50.,50.,60,-15.,15.,0.);
      HBOOK2(1206,"Res_long. vs track coord.,L3",50,-50.,50.,60,-15.,15.,0.);
      HBOOK2(1207,"Res_long. vs track coord.,L4",50,-50.,50.,60,-15.,15.,0.);
      HBOOK1(1210,"Res_transv.coo(track-sc),L=1",50,-20.,20.,0.);
      HBOOK1(1211,"Res_transv.coo(track-sc),L=2",50,-20.,20.,0.);
      HBOOK1(1212,"Res_transv.coo(track-sc),L=3",50,-20.,20.,0.);
      HBOOK1(1213,"Res_transv.coo(track-sc),L=4",50,-20.,20.,0.);
      HBOOK1(1215,"(Cos_tr-Cos_sc)/Cos_tr",50,-1.,1.,0.);
      HBOOK1(1217,"Cos_sc",50,0.5,1.,0.);
      HBOOK1(1218,"TOF track-fit chi2-x",50,0.,5.,0.);
      HBOOK1(1219,"TOF track-fit chi2-y",50,0.,5.,0.);
      HBOOK1(1503,"Anticounter cluster-energy(mev)",80,0.,20.,0.);
      HBOOK1(1505,"Qmax ratio",80,0.,16.,0.);
      HBOOK1(1507,"T0-difference inside bar-types 5",80,-0.4,0.4,0.);
      HBOOK2(1514,"Layer-1,T vs SUM(1/sqrt(Q))",50,0.,0.5,50,1.,11.,0.);
      HBOOK1(1524,"TRlen13-TRlen24",80,-4.,4.,0.);
    }
    if(TFREFFKEY.relogic[0]==4){ // <==================== AMPL-calibration
      HBOOK1(1506,"Tracks multipl. in calib.events",10,0.,10.,0.);
      HBOOK1(1500,"Part.rigidity from tracker(gv)",80,0.,32.,0.);
      HBOOK1(1501,"Particle beta(tracker)",80,0.5,1.,0.);
      HBOOK1(1502,"Particle beta(tof)",80,0.7,1.2,0.);
      HBOOK1(1503,"ANTI-cluster energy(4Lx1bar events)(mev)",80,0.,40.,0.);
      HBOOK1(1200,"Res_long.coo(track-sc),L=1",50,-10.,10.,0.);
      HBOOK1(1201,"Res_long.coo(track-sc),L=2",50,-10.,10.,0.);
      HBOOK1(1202,"Res_long.coo(track-sc),L=3",50,-10.,10.,0.);
      HBOOK1(1203,"Res_long.coo(track-sc),L=4",50,-10.,10.,0.);
      HBOOK1(1204,"Mass**2",80,-1.,19.,0.);
      HBOOK1(1207,"Mass**2 for beta<0.92",80,-1.,19.,0.);
      HBOOK1(1208,"Ptr/impl.mass",80,0.,24.,0.);
      HBOOK1(1205,"Chisq (tof-beta-fit)",50,0.,10.,0.);
      HBOOK1(1206,"Tzer (tof-beta-fit)",50,-2.5,2.5,0.);
      HBOOK1(1210,"Res_transv.coo(track-sc),L=1",50,-20.,20.,0.);
      HBOOK1(1211,"Res_transv.coo(track-sc),L=2",50,-20.,20.,0.);
      HBOOK1(1212,"Res_transv.coo(track-sc),L=3",50,-20.,20.,0.);
      HBOOK1(1213,"Res_transv.coo(track-sc),L=4",50,-20.,20.,0.);
      HBOOK1(1215,"(Cos_tr-Cos_sc)/Cos_tr",50,-1.,1.,0.);
      HBOOK1(1217,"Cos_sc",50,0.5,1.,0.);
      HBOOK2(1218,"TOF-beta vs TRACKER-momentum",80,0.,4.,60,0.5,1.1,0.);
      HBOOK2(1219,"Q(ref.btyp=5) vs (beta*gamma)",50,0.,20.,80,60.,460.,0.);
// hist.# 1220-1239 are reserved for imp.point distr.(later in TOFAMPLcalib.init()
//
      HBOOK1(1240,"Slope in HighG vs LowG correlation",50,5.,15.,0.);// <=== for TOFAVSDcalib
      HBOOK1(1241,"Offset in LowG vs HighG correlation",50,-25.,25.,0.);
      HBOOK1(1242,"Chi2 in LowG vs HighG correlation",50,0.,10.,0.);
// hist.1800-1911 are booked in init-function for ADC-low ws ADC-high correl.!!!
//
//      HBOOK1(1248,"High_to_Low signal ratio(all channels)",50,5.,15.,0.);
//      HBOOK1(1249,"High_to_Low signal ratio error(all channels)",80,0.,0.4,0.);
      HBOOK1(1250,"Ref.bar(type=5) Q-distr.(s=1,centre)",80,0.,240.,0.);        
      HBOOK1(1251,"Ref.bar(type=5) Q-distr.(s=2,centre)",80,0.,240.,0.);
      HBOOK1(1252,"Relative anode-gains(all channels)",80,0.5,2.,0.);
      HBOOK1(1254,"Ref.bar A-profile (type-1)",70,-70.,70.,0.);        
      HBOOK1(1255,"Ref.bar A-profile (type-2)",70,-70.,70.,0.);        
      HBOOK1(1256,"Ref.bar A-profile (type-3)",70,-70.,70.,0.);        
      HBOOK1(1257,"Ref.bar A-profile (type-4)",70,-70.,70.,0.);        
      HBOOK1(1258,"Ref.bar A-profile (type-5)",70,-70.,70.,0.);        
      HBOOK1(1259,"Ref.bar A-profile (type-6)",70,-70.,70.,0.);        
      HBOOK1(1260,"Ref.bar A-profile (type-7)",70,-70.,70.,0.);        
      HBOOK1(1261,"Ref.bar A-profile (type-8)",70,-70.,70.,0.);        
      HBOOK1(1262,"Ref.bar A-profile (type-9)",70,-70.,70.,0.);        
      HBOOK1(1263,"Ref.bar A-profile (type-10)",70,-70.,70.,0.);        
    }
    if(TFREFFKEY.reprtf[3]!=0){//TDC-hit multiplicity histograms
      for(il=0;il<TOF2GC::SCLRS;il++){
	for(ib=0;ib<TOF2GC::SCMXBR;ib++){
	  for(i=0;i<2;i++){
	    strcpy(htit1,"FTDC/STDC/ATDC/DTDC multipl. for chan(LBBS) ");
	    in[0]=inum[il+1];
	    strcat(htit1,in);
	    ii=(ib+1)/10;
	    jj=(ib+1)%10;
	    in[0]=inum[ii];
	    strcat(htit1,in);
	    in[0]=inum[jj];
	    strcat(htit1,in);
	    in[0]=inum[i+1];
	    strcat(htit1,in);
	    ich=2*TOF2GC::SCMXBR*il+2*ib+i;
	    HBOOK1(1300+ich,htit1,80,0.,80.,0.);
	  }
	}
      }
    }
  }
}
//-----------------------------
void TOF2JobStat::bookhistmc(){
    if(TFMCFFKEY.mcprtf[2]!=0){ // Book mc-hist
      HBOOK1(1050,"Geant-hits in layer-1",80,0.,80.,0.);
      HBOOK1(1051,"Geant-hits in layer-2",80,0.,80.,0.);
      HBOOK1(1052,"Geant-hits in layer-3",80,0.,80.,0.);
      HBOOK1(1053,"Geant-hits in layer-4",80,0.,80.,0.);
      HBOOK1(1060,"Geant-Edep(mev) in layer-1",80,0.,24.,0.);
      HBOOK1(1061,"Geant-Edep(mev) in layer-1",80,0.,240.,0.);
      HBOOK1(1062,"Geant-Edep(mev) in layer-3",80,0.,24.,0.);
      HBOOK1(1063,"Geant-Edep(mev) in layer-3",80,0.,240.,0.);
      HBOOK1(1070,"SIMU: PM-1 charge(pC,id=107,s1)",80,0.,400.,0.);
      HBOOK1(1071,"SIMU: s1+s2 pulse-charge(pC),L-1",80,0.,160.,0.);
      HBOOK1(1072,"SIMU: s1+s2 pulse-charge(pC),L-1",80,0.,1600.,0.);
      HBOOK1(1073,"SIMU: pm1 pulse-hight(mV,id=107,s1)",80,0.,1000.,0.);
      HBOOK1(1074,"SIMU: pm1 ADC(+noise(ped))(H-chan,id=107,s1)",100,0.,500.,0.);
      HBOOK1(1075,"SIMU: pm1 ADC(+noise(ped))(L-chan,id=107,s1)",100,0.,100.,0.);
    }
}
//----------------------------
void TOF2JobStat::outp(){
  int i,j,k,ich;
  geant dedx[TOF2GC::SCMXBR],dedxe[TOF2GC::SCMXBR];
       if(TFREFFKEY.reprtf[2]!=0){ // print RECO-hists
         HPRINT(1535);
         HPRINT(1536);
         HPRINT(1537);
         HPRINT(1538);
         HPRINT(1539);
         HPRINT(1540);
         HPRINT(1541);
         HPRINT(1542);
         HPRINT(1548);
         HPRINT(1549);
         HPRINT(1550);
         HPRINT(1100);
         HPRINT(1115);
         HPRINT(1101);
         HPRINT(1102);
         HPRINT(1103);
         HPRINT(1108);
         HPRINT(1104);
         HPRINT(1105);
         HPRINT(1110);
         HPRINT(1111);
         HPRINT(1112);
         HPRINT(1113);
         HPRINT(1114);
         HPRINT(1095);
         HPRINT(1096);
         HPRINT(1097);
         HPRINT(1098);
         if(TFREFFKEY.reprtf[2]>1){
           HPRINT(1529);
           HPRINT(1526);
           HPRINT(1528);
           HPRINT(1531);
           for(i=0;i<TOF2GC::SCLRS;i++){
             for(j=0;j<TOF2GC::SCMXBR;j++){
               ich=TOF2GC::SCMXBR*i+j;
               HPRINT(1140+ich);
               dedx[j]=0.;
               dedxe[j]=0.;
               dedx[j]=HSTATI(1140+ich,1," ",0);
               dedxe[j]=HSTATI(1140+ich,2," ",0);
             }
             HPAK(1116+i,dedx);
             HPAKE(1116+i,dedxe);
             HPRINT(1116+i);
           }
         }
           if(TFREFFKEY.reprtf[2]>1){
             HPRINT(1120);
             HPRINT(1121);
             HPRINT(1122);
             HPRINT(1123);
             HPRINT(1124);
             HPRINT(1125);
             HPRINT(1126);
             HPRINT(1127);
             HPRINT(1130);
             HPRINT(1131);
             HPRINT(1132);
             HPRINT(1133);
             HPRINT(1134);
             HPRINT(1135);
             HPRINT(1136);
             HPRINT(1137);
           }
         if(TFREFFKEY.reprtf[4]>0){
           HPRINT(1138);
           HPRINT(1139);
         }
           HPRINT(1532);
           HPRINT(1533);
           HPRINT(1554);
           HPRINT(1543);
           HPRINT(1545);
           HPRINT(1546);
           HPRINT(1547);
	   HPRINT(1560);
	   HPRINT(1561);
	   HPRINT(1562);
	   HPRINT(1563);
           HPRINT(1544);
           HPRINT(1534);
         if(TFREFFKEY.reprtf[3]!=0){//TDC-hit multiplicity histograms
           for(i=0;i<TOF2GC::SCLRS;i++){
             for(j=0;j<TOF2GC::SCMXBR;j++){
               for(k=0;k<2;k++){
                 ich=2*TOF2GC::SCMXBR*i+2*j+k;
                 HPRINT(1300+ich);
               }
             }
           }
         }
       }
// ---> calibration specific :
       if(TFREFFKEY.relogic[0]==3){// for TZSL-calibr. runs
         HPRINT(1500);
         HPRINT(1501);
         HPRINT(1504);
         HPRINT(1502);
         HPRINT(1220);
         HPRINT(1221);
         HPRINT(1506);
           HPRINT(1514);
           HPRINT(1503);
           HPRINT(1505);
           HPRINT(1200);
           HPRINT(1201);
           HPRINT(1202);
           HPRINT(1203);
           HPRINT(1204);
           HPRINT(1205);
           HPRINT(1206);
           HPRINT(1207);
           HPRINT(1210);
           HPRINT(1211);
           HPRINT(1212);
           HPRINT(1213);
           HPRINT(1215);
           HPRINT(1217);
           HPRINT(1218);
           HPRINT(1219);
           HPRINT(1524);
//           HPRINT(1550);
//           HPRINT(1551);
//           HPRINT(1552);
//           HPRINT(1553);
           TOF2TZSLcalib::mfit();
           HPRINT(1507);
       }
//
       if(TFREFFKEY.relogic[0]==4){// for AMPL-calibr. runs
           HPRINT(1506);
           HPRINT(1500);
           HPRINT(1501);
           HPRINT(1502);
           HPRINT(1205);
           HPRINT(1206);
           HPRINT(1503);
           HPRINT(1200);
           HPRINT(1201);
           HPRINT(1202);
           HPRINT(1203);
           HPRINT(1210);
           HPRINT(1211);
           HPRINT(1212);
           HPRINT(1213);
           HPRINT(1215);
           HPRINT(1217);
           HPRINT(1218);
           HPRINT(1219);
           HPRINT(1204);
           HPRINT(1207);
           HPRINT(1208);
           HPRINT(1250);
           HPRINT(1251);
             TOF2AMPLcalib::fit();//call TOFAVSDcalib::fit inside
//           HPRINT(1248);
//           HPRINT(1249);
           HPRINT(1252);
           HPRINT(1254);
           HPRINT(1255);
           HPRINT(1256);
           HPRINT(1257);
           HPRINT(1258);
           HPRINT(1259);
           HPRINT(1260);
           HPRINT(1261);
           HPRINT(1262);
           HPRINT(1263);
       }
//
//
       if(TFREFFKEY.relogic[0]==1){// for STRR-calibr. runs
           TOF2STRRcalib::outp();
           HPRINT(1200);
           HPRINT(1201);
           HPRINT(1202);
           HPRINT(1204);
           HPRINT(1205);
           HPRINT(1206);
           HPRINT(1207);
           HPRINT(1208);
           HPRINT(1209);
           HPRINT(1210);
       }
//
       if(TFREFFKEY.relogic[0]==2){// for TDIF-calibr. runs
           HPRINT(1500);
           TOF2TDIFcalib::fit();
       }
//
}
//----------------------------
void TOF2JobStat::outpmc(){
       if(TFMCFFKEY.mcprtf[2]!=0){ // print MC-hists
         HPRINT(1050);
         HPRINT(1051);
         HPRINT(1052);
         HPRINT(1053);
         HPRINT(1060);
         HPRINT(1061);
         HPRINT(1062);
         HPRINT(1063);
         HPRINT(1070);
         HPRINT(1071);
         HPRINT(1072);
         HPRINT(1073);
         HPRINT(1074);
         HPRINT(1075);
//         if(TFCAFFKEY.mcainc)TOF2Tovt::aintfit();
       }
}
//==========================================================================


geant TOF2Varp::getmeantoftemp(int crate){
geant tsum=0;
switch (crate){
case 1:
return float(tftt.tofav[0])/10.;
case 31:
return float(tftt.tofav[1])/10.;
case 41:
return float(tftt.tofav[2])/10.;
case 71:
return float(tftt.tofav[3])/10.;
case 3:
return float(tftt.tofav[4])/10.;
case 33:
return float(tftt.tofav[5])/10.;
case 43:
return float(tftt.tofav[6])/10.;
case 73:
return float(tftt.tofav[7])/10.;
case 0:
int i;
for(i=0;i<8;i++){
 tsum+=tftt.tofav[i]/10.;
}
return tsum;
default:
cerr <<"TOF2Varp::getmeantoftemp-E-NoCrateFound "<<crate<<endl;
return 0.;
}

}

void TOF2Varp::init(geant daqth[5], geant cuts[10]){

    int i;
    for(i=0;i<5;i++)_daqthr[i]=daqth[i];
    for(i=0;i<10;i++)_cuts[i]=cuts[i];
}


  void TOF2JobStat::clear(){
    int i,j;
    for(i=0;i<TOF2GC::SCJSTA;i++)mccount[i]=0;
    for(i=0;i<TOF2GC::SCJSTA;i++)recount[i]=0;
    for(i=0;i<TOF2GC::SCCHMX;i++)
                  for(j=0;j<TOF2GC::SCCSTA;j++)
                                       chcount[i][j]=0;
    for(i=0;i<TOF2GC::SCBLMX;i++)
                  for(j=0;j<TOF2GC::SCCSTA;j++)
                                       brcount[i][j]=0;
    for(j=0;j<2;j++){
      for(i=0;i<TOF2GC::SCCRAT;i++)scdaqbc1[i][j]=0;
      for(i=0;i<TOF2GC::SCCRAT;i++)scdaqbc2[i][j]=0;
      for(i=0;i<TOF2GC::SCCRAT;i++)scdaqbc3[i][j]=0;
    }
  }



