// Author E.Choumilov 14.06.96.
#include <typedefs.h>
#include <math.h>
#include <commons.h>
#include <job.h>
#include <tofdbc.h>
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <tofsim.h>
#include <tofrec.h>
#include <tofcalib.h>
//
extern AMSTOFScan scmcscan[SCBLMX];// TOF MC time/eff-distributions
extern TOFVarp tofvpar; // TOF general parameters (not const !)
TOFBrcal scbrcal[SCLRS][SCMXBR];// TOF individual sc.bar parameters 
//-----------------------------------------------------------------------
//  =====> TOFDBc class variables definition :
//
integer TOFDBc::debug=1;
//
//
//======> memory reservation for _brtype :
// (real values are initialized from ext. geomconfig-file in amsgeom.c) 
integer TOFDBc::_brtype[SCBLMX]={
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
//
// Initialize TOF geometry (defaults, REAL VALUES are read FROM geomconfig-file)
//
//---> bar lengthes (cm) for each of SCBTPN types :
geant TOFDBc::_brlen[SCBTPN]={
  72.6,99.6,118.6,130.6,135.4
};
//---> plane rotation mask for 4 layers (1/0 -> rotated/not):
integer TOFDBc::_plrotm[SCLRS]={
  1,0,0,1
};
//---> honeycomb supporting str. data:
geant TOFDBc::_supstr[10]={
   59.9,    // i=1    Z-coo of TOP honeycomb (BOT-side)
  -59.9,    //  =2    Z-coo    BOT           (TOP     )
   0.,0.,  //  =3,4  X/Y-shifts of TOP honeycomb centre in Mother r.s.
   0.,0.,  //  =5,6                BOT
   10.1,82.,//  =7,8  DZ/R-outer of both honeycomb supports
   0.,0.   //  =9,10 spare
};
//---> sc. plane design parameters:
geant TOFDBc::_plnstr[15]={
  0.,0.,  // i=1,2  Z-gap between honeycomb and outer/inner scint. planes
    1.5,  // i=3    Z-shift of even/odd bars in plane (0 -> flat plane)
     0.88,  // i=4    X(Y)-overlaping(incl. sc_cover) of even/odd bars (0 for flat)
     11., // i=5    sc. bar width
      1., // i=6    sc. bar thickness
    0.07, // i=7    sc_cover thickness on "sc.thickness"(Carb.fiber+mylar?)
 10.,10.,2.5, // i=8-10 width/length/thickness (x/y/z) of PMT shielding box.
  5.,         // i=11 dist. in x-y plane from sc. edge to PMT box.
  0.7,        // i=12 Z-shift of sc.bar/PMT-box centers (depends on par. i=10)
  0.19,       // i=13 sc_cover thickness on "sc.width"(2xCarb.fiber+mylar?)
  0.,0.    // i=14-15 spare
};
//---> Initialize TOF MC/RECO "time-stable" parameters :
//
  geant TOFDBc::_edep2ph={8000.};// edep(Mev)-to-Photons convertion
  geant TOFDBc::_seresp=1.45;    // PMT Single elec.responce (MP=Mean,mV)
  geant TOFDBc::_seresv=0.8;     // PMT Single elec.responce variance(relative to Mean)
  geant TOFDBc::_fladctb=0.1;    // MC "flash-ADC" internal time binning (ns)
  geant TOFDBc::_shaptb=1.;      // MC shaper internal time binning (ns)
  geant TOFDBc::_shrtim=1.0;     // MC shaper pulse rise time (ns)(exp)
  geant TOFDBc::_shctim=600.;    // MC shaper pulse cut-time(gate width)(ns)
  geant TOFDBc::_shftim=63.;     // MC shaper pulse fall time (ns)(exp). MC only !!!
  geant TOFDBc::_strflu=2.;    // Stretcher "end-mark" time fluctuations (ns)
  geant TOFDBc::_tdcbin[4]={
    1.,                            // pipe/line TDC binning for fast-tdc meas.
    1.,                            // pipe/line TDC binning for slow-tdc meas.
    1.,                            // pipe/line TDC binning for adc-anode meas.
    1.                             // pipe/line TDC binning for adc-dinode meas.
  };
  geant TOFDBc::_daqpwd[15]={
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
    0.05,   // (10)fast discr.(comparator) internal accuracy(ns) + (?) to have exp.resol
    2.,     // (11)min. pulse duration (ns) of fast discr.(comparator) (for dinode also)
    18.,    // (12)(as dummy gap in s-TDC pulse)
    10.36,  // (13)thresh.(pC) in A-integrator(TovT conversion) 
    10.36   // (14)thresh.(pC) in D-integrator(TovT conversion) 
  };
  geant TOFDBc::_trigtb=0.25;  // MC time-bin in logic(trig) pulse handling (ns)
  geant TOFDBc::_di2anr=0.2;  // dinode->anode signal ratio (def,mc-data) not used now !
  geant TOFDBc::_strrat=40.;  // stretcher ratio (default,mc-data) not used now !
  geant TOFDBc::_strjit1=0.035;  // "start"-pulse jitter at stretcher input
  geant TOFDBc::_strjit2=0.035;  // "stop"(FT)-pulse jitter at stretcher input
  geant TOFDBc::_accdel=5000.;//Lev-1 signal delay with respect to FT (ns)
  geant TOFDBc::_ftdelf=65.;  // FastTrigger (FT) fixed (by h/w) delay (ns)
  geant TOFDBc::_ftdelm=200.; // FT max delay (allowed by stretcher logic) (ns)
  geant TOFDBc::_fstdcd=28.;   // Same hit(up-edge) relative delay in fast-slow TDC
  geant TOFDBc::_fatdcd=5.;   // Same hit(up-edge) relative delay in fast-A(D) TDC
  integer TOFDBc::_pbonup=1;  // set phase-bit for leading(up) edge (yes/no->1/0) 
//
//  member functions :
//
// ---> function to read geomconfig-files 
//                  Called from TOFgeom :
  void TOFDBc::readgconf(){
    int i;
    char fname[80];
    char name[80]="geomconf";
    char vers1[3]="01";
    char vers2[3]="02";
    if(strstr(AMSJob::gethead()->getsetup(),"AMSSTATION")==0)
    {
          cout <<" TOFGeom-I-Shuttle setup selected."<<endl;
          strcat(name,vers1);
    }
    else
    {
          cout <<" TOFGeom-I-Station setup selected."<<endl;
          strcat(name,vers2);
    }
    strcat(name,".dat");
    if(TOFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
    if(TOFCAFFKEY.cafdir==1)strcpy(fname,"");
    strcat(fname,name);
    cout<<"TOFDBc::readgconf: Open file : "<<fname<<'\n';
    ifstream tcfile(fname,ios::in); // open needed config-file for reading
    if(!tcfile){
      cerr <<"TOFgeom-read: missing geomconfig-file "<<fname<<endl;
      exit(1);
    }
    for(int ic=0;ic<SCBLMX;ic++) tcfile >> _brtype[ic];
    for(i=0;i<SCLRS;i++) tcfile >> _plrotm[i];
    for(i=0;i<SCBTPN;i++) tcfile >> _brlen[i];
    for(i=0;i<10;i++) tcfile >> _supstr[i];
    for(i=0;i<15;i++) tcfile >> _plnstr[i];
  }
//---
  integer TOFDBc::brtype(integer ilay, integer ibar){
    #ifdef __AMSDEBUG__
      if(TOFDBc::debug){
        assert(ilay>=0 && ilay < SCLRS);
        assert(ibar>=0 && ibar < SCBRS[ilay]);
      }
    #endif
    int cnum;
    cnum=SCMXBR*ilay+ibar;
    return _brtype[cnum];
  }
//
  geant TOFDBc::brlen(integer ilay, integer ibar){
    int cnum;
    cnum=SCMXBR*ilay+ibar;
    int btyp=_brtype[cnum];
    #ifdef __AMSDEBUG__
      if(TOFDBc::debug){
        assert(btyp>0 && btyp <= SCBTPN);
      }
    #endif
    return _brlen[btyp-1];
  }
//
  integer TOFDBc::plrotm(integer ilay){
    #ifdef __AMSDEBUG__
      if(TOFDBc::debug){
        assert(ilay>=0 && ilay < SCLRS);
      }
    #endif
    return _plrotm[ilay];
  }
//
  geant TOFDBc::supstr(integer i){
    #ifdef __AMSDEBUG__
      if(TOFDBc::debug){
        assert(i>0 && i <= 10);
      }
    #endif
    return _supstr[i-1];
  }
//
  geant TOFDBc::plnstr(integer i){
    #ifdef __AMSDEBUG__
      if(TOFDBc::debug){
        assert(i>0 && i <= 15);
      }
    #endif
    return _plnstr[i-1];
  }
//   function to get Z-position of scint. bar=ib in layer=il
  geant TOFDBc::getzsc(integer il, integer ib){
    #ifdef __AMSDEBUG__
      if(TOFDBc::debug){
        assert(il>=0 && il < SCLRS);
        assert(ib>=0 && ib < SCBRS[il]);
      }
    #endif
  static geant dz;
  geant zc;
  dz=_plnstr[5]+2.*_plnstr[6];// counter thickness
  if(il==0)
    zc=_supstr[0]+_supstr[6]+_plnstr[0]+dz/2.;
  else if(il==1)
    zc=_supstr[0]-_plnstr[1]-dz/2.;
  else if(il==2)
    zc=_supstr[1]+_plnstr[1]+dz/2.;
  else if(il==3)
    zc=_supstr[1]-_supstr[6]-_plnstr[0]-dz/2.;
  if(il==0)zc=zc+(ib%2)*_plnstr[2];
  else if(il==2){
          if((AMSJob::gethead()->isRealData()) && (AMSEvent::gethead() && AMSEvent::gethead()->getrun()>889992398))
          zc=zc+(ib%2)*_plnstr[2];//new (correct)
          else zc=zc+((ib+1)%2)*_plnstr[2];//old (wrong)
  }
  else if(il==3)zc=zc-(ib%2)*_plnstr[2];
  else if(il==1)zc=zc-((ib+1)%2)*_plnstr[2];
  return(zc);
  }  
// function to get transv. position of scint. bar=ib in layer=il
  geant TOFDBc::gettsc(integer il, integer ib){
    #ifdef __AMSDEBUG__
      if(TOFDBc::debug){
        assert(il>=0 && il < SCLRS);
        assert(ib>=0 && ib < SCBRS[il]);
      }
    #endif
  static geant dx;
  geant x,co[2],dxt;
  dx=_plnstr[4]+2.*_plnstr[12];//width of sc.counter(bar+cover)
  dxt=(SCBRS[il]-1)*(dx-_plnstr[3]);//first-last sc.count. bars distance
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
  geant TOFDBc::edep2ph(){return _edep2ph;}
  geant TOFDBc::fladctb(){return _fladctb;}
  geant TOFDBc::shaptb(){return _shaptb;}
  geant TOFDBc::shrtim(){return _shrtim;}
  geant TOFDBc::shftim(){return _shftim;}
  geant TOFDBc::shctim(){return _shctim;}
//
  geant TOFDBc::strrat(){return _strrat;}
//
  geant TOFDBc::strflu(){return _strflu;}
//
  geant TOFDBc::strjit1(){return _strjit1;}
//
  geant TOFDBc::strjit2(){return _strjit2;}
//
  geant TOFDBc::di2anr(){return _di2anr;}
//
  geant TOFDBc::accdel(){return _accdel;}
  geant TOFDBc::ftdelf(){return _ftdelf;}
  geant TOFDBc::ftdelm(){return _ftdelm;}
  geant TOFDBc::fstdcd(){return _fstdcd;}
  geant TOFDBc::fatdcd(){return _fatdcd;}
  geant TOFDBc::seresp(){return _seresp;}
  geant TOFDBc::seresv(){return _seresv;}
  integer TOFDBc::pbonup(){return _pbonup;}
//
  geant TOFDBc::tdcbin(int i){
    #ifdef __AMSDEBUG__
      if(TOFDBc::debug){
        assert(i>=0 && i<4);
      }
    #endif
    return _tdcbin[i];}
//
  geant TOFDBc::daqpwd(int i){
    #ifdef __AMSDEBUG__
      if(TOFDBc::debug){
        assert(i>=0 && i<15);
      }
    #endif
    return _daqpwd[i];}
//
  geant TOFDBc::trigtb(){return _trigtb;}
//===============================================================================
//  TOFBrcal class functions :
//
void TOFBrcal::build(){// create scbrcal-objects for each sc.bar
//
 int lsflg(1);//0/1->use common/individual values for Lspeed 
 integer i,j,k,ila,ibr,ip,ibrm,isd,isp,nsp,ibt,cnum,dnum,mult;
 geant scp[SCANPNT];
 geant rlo[SCANPNT];// relat.(to Y=0) light output
 integer lps=1000;
 geant ef1[SCANPNT],ef2[SCANPNT];
 integer i1,i2,sta[2];
 geant r,eff1,eff2;
 integer sid,brt;
 geant gna[2],gnd[2],qath,qdth,a2dr[2],tth,strat[2][2];
 geant slope,slpf,fstrd,tzer,tdif,mip2q,speedl,lspeeda[SCLRS][SCMXBR];
 geant tzerf[SCLRS][SCMXBR],tdiff[SCBLMX],stat[SCBLMX][2];
 geant slops[2],slops1[SCLRS][SCMXBR],slops2[SCLRS][SCMXBR];
 geant strf[SCBLMX][2],strof[SCBLMX][2];
 geant an2di[SCBLMX][2],gaina[SCBLMX][2],gaind[SCBLMX][2],m2q[SCBTPN];
 geant ipara[SCCHMX][SCIPAR]; 
 geant ipard[SCCHMX][SCIPAR];
 geant aprofp[SCBTPN][SCPROFP],hblen,p1,p2,p3,p4,p5,nom,denom; 
 char fname[80];
 char name[80];
 geant td2p[2];
 char vers1[3]="mc";
 char vers2[3]="rl";
 int mrfp;
//
 geant asatl=20.;//(mev,~20MIPs),if E-dinode(1-end) higher - use it instead
//                                 of anode measurements
//---> TovT-electronics calibration for MC:
//
 geant aip[2][SCIPAR]={ // def.for anode integrator(shft,t0(qthr=exp(t0/shft)),qoffs)
                   {50.,62.6,1.3},
                   {50.,62.6,1.3}
                      };
 geant dip[2][SCIPAR]={              // default param. for dinode integrator
                   {50.,62.6,1.3},
                   {50.,62.6,1.3}
                      };
//------------------------------
  char in[2]="0";
  char inum[11];
  int ctyp,ntypes,mcvern[10],rlvern[10];
  int mcvn,rlvn,dig;
//
  strcpy(inum,"0123456789");
  for(i=0;i<SCCHMX;i++){
    for(j=0;j<SCIPAR;j++){
      ipara[i][j]=0.;
      ipard[i][j]=0.;
    }
  }
//
// ---> read file-versions file :
//
  integer cfvn;
  cfvn=TOFCAFFKEY.cfvers%100;
  strcpy(name,"tofverslist");// basic name for tofverslistNN.dat file
  dig=cfvn/10;
  in[0]=inum[dig]; 
  strcat(name,in);
  dig=cfvn%10;
  in[0]=inum[dig]; 
  strcat(name,in);
  strcat(name,".dat");
//
  if(TOFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(TOFCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"TOFBrcal::build: Open file  "<<fname<<'\n';
  ifstream vlfile(fname,ios::in); // open needed tdfmap-file for reading
  if(!vlfile){
    cerr <<"TOFBrcal_build:: missing verslist-file "<<fname<<endl;
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
 strcpy(name,"tdvcalib");
 mcvn=mcvern[ctyp-1]%100;
 rlvn=rlvern[ctyp-1]%100;
 if(AMSJob::gethead()->isMCData()) //      for MC-event
 {
       cout <<" TOFBrcal_build: MC-Tdif-calibration is used"<<endl;
       dig=mcvn/10;
       in[0]=inum[dig];
       strcat(name,in);
       dig=mcvn%10;
       in[0]=inum[dig];
       strcat(name,in);
       strcat(name,vers1);
 }
 else                              //      for Real events
 {
       cout <<" TOFBrcal_build: REAL-Tdif-calibration is used"<<endl;
       dig=rlvn/10;
       in[0]=inum[dig];
       strcat(name,in);
       dig=rlvn%10;
       in[0]=inum[dig];
       strcat(name,in);
       strcat(name,vers2);
 }
//
 strcat(name,".dat");
 if(TOFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
 if(TOFCAFFKEY.cafdir==1)strcpy(fname,"");
 strcat(fname,name);
 cout<<"Open file : "<<fname<<'\n';
 ifstream tdcfile(fname,ios::in); // open  file for reading
 if(!tdcfile){
   cerr <<"TOFBrcal_build: missing Lspeed/Tdif-file "<<fname<<endl;
   exit(1);
 }
//
// ---> read Lspeed/Tdiffs:
//
 if(lsflg){// read bar indiv.Lspeed
   for(ila=0;ila<SCLRS;ila++){   
     for(ibr=0;ibr<SCMXBR;ibr++){  
       tdcfile >> lspeeda[ila][ibr];
     }
   }
 }
 else tdcfile >> speedl;// read average Lspeed
 for(ila=0;ila<SCLRS;ila++){   
   for(ibr=0;ibr<SCMXBR;ibr++){  
     cnum=ila*SCMXBR+ibr; // sequential counters numbering(0-55)
     tdcfile >> tdiff[cnum];
   }
 }
 tdcfile.close();
//------------------------------
//
//   --->  Read slope/tzeros calibration file :
//
 ctyp=6;
 strcpy(name,"tzscalib");
 mcvn=mcvern[ctyp-1]%100;
 rlvn=rlvern[ctyp-1]%100;
 if(AMSJob::gethead()->isMCData()) //      for MC-event
 {
       cout <<" TOFBrcal_build: MC-T0-calibration is used"<<endl;
       dig=mcvn/10;
       in[0]=inum[dig];
       strcat(name,in);
       dig=mcvn%10;
       in[0]=inum[dig];
       strcat(name,in);
       strcat(name,vers1);
 }
 else                              //      for Real events
 {
       cout <<" TOFBrcal_build: REAL-T0-calibration is used"<<endl;
       dig=rlvn/10;
       in[0]=inum[dig];
       strcat(name,in);
       dig=rlvn%10;
       in[0]=inum[dig];
       strcat(name,in);
       strcat(name,vers2);
 }
//
 strcat(name,".dat");
 if(TOFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
 if(TOFCAFFKEY.cafdir==1)strcpy(fname,"");
 strcat(fname,name);
 cout<<"Open file : "<<fname<<'\n';
 ifstream tzcfile(fname,ios::in); // open  file for reading
 if(!tzcfile){
   cerr <<"TOFBrcal_build: missing Tzero/Slope(s)-file "<<fname<<endl;
   exit(1);
 }
//
// ---> read Slope/Tzero's:
 tzcfile >> slpf;
 for(ila=0;ila<SCLRS;ila++){ 
   for(ibr=0;ibr<SCMXBR;ibr++){
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
 strcpy(name,"srscalib");
 mcvn=mcvern[ctyp-1]%100;
 rlvn=rlvern[ctyp-1]%100;
 if(AMSJob::gethead()->isMCData())           // for MC-event
 {
   cout <<" TOFBrcal_build: str_ratio/status data for MC-events are selected."<<endl;
   dig=mcvn/10;
   in[0]=inum[dig];
   strcat(name,in);
   dig=mcvn%10;
   in[0]=inum[dig];
   strcat(name,in);
   strcat(name,vers1);
 }
 else                                       // for Real events
 {
   cout <<" TOFBrcal_build: str_ratio-calib. for Real-events selected."<<endl;
   dig=rlvn/10;
   in[0]=inum[dig];
   strcat(name,in);
   dig=rlvn%10;
   in[0]=inum[dig];
   strcat(name,in);
   strcat(name,vers2);
 }
   strcat(name,".dat");
   if(TOFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
   if(TOFCAFFKEY.cafdir==1)strcpy(fname,"");
   strcat(fname,name);
   cout<<"Open file : "<<fname<<'\n';
   ifstream scfile(fname,ios::in); // open str_ratio-file for reading
   if(!scfile){
     cerr <<"TOFBrcal_build: missing str_ratio/status-file "<<fname<<endl;
     exit(1);
   }
//---> read str_ratio/status:
//
   for(ila=0;ila<SCLRS;ila++){   // <-------- loop over layers
   for(ibr=0;ibr<SCMXBR;ibr++){  // <-------- loop over bar in layer
     cnum=ila*SCMXBR+ibr; // sequential counter numbering(0-55)
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
//   --->  Read a2d-ratios, gains, mip2q and A-profile param. calib.file :
//
 ctyp=4;
 strcpy(name,"anacalib");
 mcvn=mcvern[ctyp-1]%100;
 rlvn=rlvern[ctyp-1]%100;
 if(AMSJob::gethead()->isMCData())           // for MC-event
 {
   cout <<" TOFBrcal_build: a2d_ratio/gain-calib. for MC-events selected."<<endl;
   dig=mcvn/10;
   in[0]=inum[dig];
   strcat(name,in);
   dig=mcvn%10;
   in[0]=inum[dig];
   strcat(name,in);
   strcat(name,vers1);
 }
 else                                       // for Real events
 {
   cout <<" TOFBrcal_build: a2d_ratio/gain-calib. for Real-events selected."<<endl;
   dig=rlvn/10;
   in[0]=inum[dig];
   strcat(name,in);
   dig=rlvn%10;
   in[0]=inum[dig];
   strcat(name,in);
   strcat(name,vers2);
 }
   strcat(name,".dat");
   if(TOFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
   if(TOFCAFFKEY.cafdir==1)strcpy(fname,"");
   strcat(fname,name);
   cout<<"Open file : "<<fname<<'\n';
   ifstream gcfile(fname,ios::in); // open a2d_ratio/gain/mip2q-file for reading
   if(!gcfile){
     cerr <<"TOFBrcal_build: missing a2d_ratio/gain/mip2q-file "<<fname<<endl;
     exit(1);
   }
// ---> read a2d-ratios:
//
   for(ila=0;ila<SCLRS;ila++){   // <-------- loop over layers
     for(ibr=0;ibr<SCMXBR;ibr++){  // read side-1
       cnum=ila*SCMXBR+ibr; 
       gcfile >> an2di[cnum][0];
     }
     for(ibr=0;ibr<SCMXBR;ibr++){  // read side-2
       cnum=ila*SCMXBR+ibr; 
       gcfile >> an2di[cnum][1];
     }
   } // --- end of layer loop --->
//
// ---> read gains(anode):
//
   for(ila=0;ila<SCLRS;ila++){   // <-------- loop over layers
     for(ibr=0;ibr<SCMXBR;ibr++){  // read side-1
       cnum=ila*SCMXBR+ibr; 
       gcfile >> gaina[cnum][0];
     }
     for(ibr=0;ibr<SCMXBR;ibr++){  // read side-2
       cnum=ila*SCMXBR+ibr; 
       gcfile >> gaina[cnum][1];
     }
   } // --- end of layer loop --->
//
// ---> read mip2q's:
//
   for(ibt=0;ibt<SCBTPN;ibt++){  // <-------- loop over bar-types
     gcfile >> m2q[ibt];
   }
//
// ---> read A-prof. parameters:
//
 if(AMSJob::gethead()->isRealData()){// tempor only for Real events
   for(ibt=0;ibt<SCBTPN;ibt++){  // <-------- loop over bar-types
     for(i=0;i<SCPROFP;i++)gcfile >> aprofp[ibt][i];
   }
 }
//
   gcfile.close();
//   
//---------------------------------------------
//
//   --->  Read anode/dinode integrator parameters calib.file :
//
 ctyp=5;
 strcpy(name,"incalib");
 mcvn=mcvern[ctyp-1]%100;
 rlvn=rlvern[ctyp-1]%100;
 if(AMSJob::gethead()->isMCData())           // for MC-event
 {
   cout <<" TOFBrcal_build: a/d_integrator-calib. for MC-events selected."<<endl;
   dig=mcvn/10;
   in[0]=inum[dig];
   strcat(name,in);
   dig=mcvn%10;
   in[0]=inum[dig];
   strcat(name,in);
   strcat(name,vers1);
 }
 else                                       // for Real events
 {
   cout <<" TOFBrcal_build: a/d_integrator-calib. for Real-events selected."<<endl;
   dig=rlvn/10;
   in[0]=inum[dig];
   strcat(name,in);
   dig=rlvn%10;
   in[0]=inum[dig];
   strcat(name,in);
   strcat(name,vers2);
 }
 strcat(name,".dat");
 if(TOFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
 if(TOFCAFFKEY.cafdir==1)strcpy(fname,"");
 strcat(fname,name);
 cout<<"Open file : "<<fname<<'\n';
 ifstream icfile(fname,ios::in); // open a/d_integrator_param-file for reading
 if(!icfile){
   cerr <<"TOFBrcal_build: missing a/d_integrator_param-file "<<fname<<endl;
   exit(1);
 }
//
 int16u swid,crat,sfet,tofc;
 for(i=0;i<SCCRAT;i++){//<--- crate loop (0-7)
   icfile >> crat;// crate-number
   for(j=0;j<SCSFET;j++){//<--- SFET card loop (0-3)
     icfile >> sfet;// sfet-number
     for(k=0;k<SCTOFC;k++){//<--- tof-ch loop (1tofc->side, 4 per SFET)(0-3)
       icfile >> tofc;// tofc-number
       swid=AMSTOFRawEvent::hw2swid(crat-1,sfet-1,tofc-1);//LBBS
       if(swid==0)continue;// non-existing tofc (occupied by temperatures or epty)
       ila=swid/1000-1;
       ibr=(swid%1000)/10-1;
       isd=(swid%1000)%10-1;
       cnum=2*SCMXBR*ila+2*ibr+isd;
       for(ip=0;ip<SCIPAR;ip++)icfile >> ipara[cnum][ip];//read anode parameters
       for(ip=0;ip<SCIPAR;ip++)icfile >> ipard[cnum][ip];//.... dinode .........
     }
   }
 }
 icfile.close();
//---------------------------------------------
//   ===> fill TOFBrcal bank :
//
  for(ila=0;ila<SCLRS;ila++){   // <-------- loop over layers
  for(ibr=0;ibr<SCMXBR;ibr++){  // <-------- loop over bar in layer
    brt=TOFDBc::brtype(ila,ibr);
    if(brt==0)continue; // skip missing counters
    hblen=0.5*TOFDBc::brlen(ila,ibr);
    cnum=ila*SCMXBR+ibr; // sequential counter numbering(0-55)
    scmcscan[cnum].getscp(scp);//read scan-points from scmcscan-object
// read from file or DB:
    gna[0]=gaina[cnum][0];
    gna[1]=gaina[cnum][1];
    gnd[0]=gna[0];// tempor
    gnd[1]=gna[1];
    tth=tofvpar.daqthr(0); // (mV), time-discr. threshold
    mip2q=m2q[brt-1];//(pC/mev),dE(mev)_at_counter_center->Q(pC)_at_PM_anode(2x3-sum)
    a2dr[0]=an2di[cnum][0];// from ext.file
    a2dr[1]=an2di[cnum][1];
    fstrd=tofvpar.sftdcd();//(ns),same hit(up-edge)delay in f/sTDC(const. for now)
//
//-->prepare position correction array (valid for local !!! r.c.):
//
    mrfp=SCANPNT/2;//central point (coo=0.)
    if(AMSJob::gethead()->isMCData()){// pos.corrections for MC
      scmcscan[cnum].getefarr(ef1,ef2);//read eff1/2 from scmcscan-object
      for(isp=0;isp<SCANPNT;isp++){ // fill 2-ends rel. l.output at scan-points
        rlo[isp]=(ef1[isp]+ef2[isp])/(ef1[mrfp]+ef2[mrfp]);
      }
    }
    else{// pos.correction for Real
      p1=aprofp[brt-1][0];
      p2=aprofp[brt-1][1];
      p3=aprofp[brt-1][2];
      p4=aprofp[brt-1][3];
      p5=aprofp[brt-1][4];
      denom=p1*(exp(-(hblen+scp[mrfp])/p4)+p3*exp(-(hblen+scp[mrfp])/p5))
           +p2*(exp(-(hblen-scp[mrfp])/p4)+p3*exp(-(hblen-scp[mrfp])/p5));
      for(isp=0;isp<SCANPNT;isp++){ // fill 2-ends rel. l.output at scan-points
        nom=p1*(exp(-(hblen+scp[isp])/p4)+p3*exp(-(hblen+scp[isp])/p5))
           +p2*(exp(-(hblen-scp[isp])/p4)+p3*exp(-(hblen-scp[isp])/p5));
        rlo[isp]=nom/denom;
      }
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
    td2p[1]=tofvpar.lcoerr();//error on longit. coord. measurement(cm)
    for(ip=0;ip<SCIPAR;ip++)aip[0][ip]=ipara[2*cnum][ip];
    for(ip=0;ip<SCIPAR;ip++)aip[1][ip]=ipara[2*cnum+1][ip];
    for(ip=0;ip<SCIPAR;ip++)dip[0][ip]=ipard[2*cnum][ip];
    for(ip=0;ip<SCIPAR;ip++)dip[1][ip]=ipard[2*cnum+1][ip];
    if(aip[0][0]==0.)sta[0]=-1;// set status -1(bad) for missing channels (tauf==0)
    if(aip[1][0]==0.)sta[1]=-1;
    scbrcal[ila][ibr]=TOFBrcal(sid,sta,gna,gnd,a2dr,asatl,tth,
                              strat,fstrd,tzer,slope,slops,tdif,td2p,mip2q,scp,rlo,
                              aip,dip);
//
  } // --- end of bar loop --->
  } // --- end of layer loop --->
//
}
//-----------------------------------------------------------------------
//
geant TOFBrcal::ama2mip(number amf[2]){ // side A-Tovt's(ns) -> Etot(Mev)
  number q(0),qt(0);
  for(int isd=0;isd<2;isd++){
    if(status[isd]>=0){
      q2t2q(1,isd,0,amf[isd],q);
      qt+=(q/gaina[isd]);// Qa->Qa_gain_corrected
    }
  }
  qt=qt/mip2q; // Q(pC)->Mev
  return geant(qt);
}
//------
void TOFBrcal::ama2q(number amf[2], number qs[2]){// side A-Tovt's(ns) -> Q(pC)
//                                                 to use in calibr. program 
  for(int isd=0;isd<2;isd++){
    qs[isd]=0.;
    if(status[isd]>=0)q2t2q(1,isd,0,amf[isd],qs[isd]);
  }
}
//------
void TOFBrcal::q2t2q(int cof, int sdf, int adf, number &tovt, number &q){  
// Q(pC) <-> Tovt(ns) to use in sim./rec. programs (cof=0/1-> Q->Tovt/Tovt->Q)
//                                                 (sdf=0/1-> bar side 1/2   )
//                                                 (adf=0/1-> for anode/dinode)
  number qoffs,shft,qthr,p1,p2,dt;
  if(adf==0){
    shft=aipar[sdf][0];
    qthr=exp(aipar[sdf][1]/shft);//to match old parametrization
    qoffs=aipar[sdf][2];
  }
  else{
//    shft=dipar[sdf][0];//tempor (to accept tempor.parametrization for dynode) 
//    qthr=exp(dipar[sdf][1]/shft);
//    qoffs=dipar[sdf][2];
    shft=aipar[sdf][0];//anode param.needed for dynode reco
    qthr=exp(aipar[sdf][1]/shft);
    qoffs=aipar[sdf][2];
    p1=dipar[sdf][0];//dynode param.
    p2=dipar[sdf][1];
  }
// 
  if(cof==0){ // q->tovt
    if(q>qoffs)tovt=shft*log((q-qoffs)/qthr);
    else tovt=0.;
  }
  else{       // tovt->q
    if(adf==0)q=qoffs+qthr*exp(tovt/shft);//for anode
    else{// for dynode
      dt=(tovt-p1)/p2;
      q=qoffs+qthr*exp(dt/shft);
    }
  }
}
//------
geant TOFBrcal::amd2mip(number amf[2]){ // side A-Tovt's(ns) -> Etot(Mev)
  number q(0),qt(0);
  for(int isd=0;isd<2;isd++){
    if(status[isd]>=0){
      q2t2q(1,isd,1,amf[isd],q);
//      qt+=(q*an2dir[isd]/gaind[isd]);// Qd->Qd_gain_corrected
      qt+=(q/gaind[isd]);// (for new Dynode integ.parametrization don't need an2dir !!!) 
    }
  }
  qt=qt/mip2q; // Q(pC)->Mev
  return geant(qt);
}
//-----
void TOFBrcal::amd2q(number amf[2], number qs[2]){// side A-Tovt's(ns) -> Q(pC)
//                                                 to use in calibr. program 
  number q,qt(0);
  for(int isd=0;isd<2;isd++){
    qs[isd]=0.;
    if(status[isd]>=0)q2t2q(1,isd,1,amf[isd],qs[isd]);
  }
}
//-----
geant TOFBrcal::poscor(geant point){
//(return light-out corr.factor, input 'point' is Y-coord. in bar loc.r.s.)
  static int nmx=SCANPNT-1;
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
geant TOFBrcal::tm2t(number tmf[2], number amf[2]){//(2-sides_times/Tovt)->Time (ns)
  geant shft;
  number time,qs,uv(0);
  shft=TOFDBc::shftim();
  if(status[0]>=0 && status[1]>=0){
    for(int isd=0;isd<2;isd++){
      q2t2q(1,isd,0,amf[isd],qs);// TovT->Q
//      uv+=slops[isd]/qs;// summing slops/Q
      uv+=slops[isd]/sqrt(qs);// summing slops/sqrt(Q), works sl.better
    }
  }
  time=0.5*(tmf[0]+tmf[1])+tzero+slope*uv;
  return geant(time); 
}
//-----
void TOFBrcal::tmd2p(number tmf[2], number amf[2],
                              geant &co, geant &eco){//(time-diff)->loc.coord/err(cm)
  geant shft;
  number time,coo,qs,uv(0);
  shft=TOFDBc::shftim();
  if(status[0]>=0 && status[1]>=0){
    for(int isd=0;isd<2;isd++){
      q2t2q(1,isd,0,amf[isd],qs);// TovT->Q
//      uv+=(1-2*isd)*slops[isd]/qs;// subtr slops/Q
      uv+=(1-2*isd)*slops[isd]/sqrt(qs);// subtr slops/sqrt(Q)
    }
  }
  coo=-(0.5*(tmf[0]-tmf[1])+slope*uv-yctdif);
//  coo=-(0.5*(tmf[0]-tmf[1])-yctdif);// tempor no A-correction
//common "-" is due to the fact that Tmeas=Ttrig-Tabs and coo-loc is prop. to Tabs1-Tabs2
  co=td2pos[0]*geant(coo);//coo(ns)->cm                    
  eco=td2pos[1];
}
//-----
void TOFBrcal::td2ctd(number tdo, number amf[2],
                              number &tdc){//td0=0.5*(t[0]-t[1])->tdc (A-corrected)
  geant shft;
  number qs,uv(0);
  shft=TOFDBc::shftim();
  if(status[0]>=0 && status[1]>=0){
    for(int isd=0;isd<2;isd++){
      q2t2q(1,isd,0,amf[isd],qs);// TovT->Q
//      uv+=(1-2*isd)*slops[isd]/qs;// subtr slops/Q
      uv+=(1-2*isd)*slops[isd]/sqrt(qs);// subtr slops/sqrt(Q)
    }
  }
  tdc=tdo+slope*uv;
}
//==========================================================================
//
//   TOFJobStat static variables definition (memory reservation):
//
integer TOFJobStat::mccount[SCJSTA];
integer TOFJobStat::recount[SCJSTA];
integer TOFJobStat::chcount[SCCHMX][SCCSTA];
integer TOFJobStat::brcount[SCBLMX][SCCSTA];
integer TOFJobStat::scdaqbc1[SCCRAT][2];
integer TOFJobStat::scdaqbc2[SCCRAT][2];
integer TOFJobStat::scdaqbc3[SCCRAT][2];
integer TOFJobStat::scdaqbc4[SCCRAT][2];
//
// function to print Job-statistics at the end of JOB(RUN):
//
void TOFJobStat::printstat(){
  int il,ib,ic;
  geant rc;
//
  printf("\n");
  printf("    ======================= JOB DAQ-statistics ====================\n");
  printf("\n");
  printf(" ------- node(block) number -------->"); 
  for(ic=0;ic<SCCRAT;ic++){
      printf("          %1d          ",ic);
  }
  printf("\n");
  printf(" S-block events                         : ");
  for(ic=0;ic<SCCRAT;ic++){ 
      printf("  r/c:%7d %7d",scdaqbc1[ic][0],scdaqbc1[ic][1]);
  }
  printf("\n");
  printf(" S-block events (nonempty)              : ");
  for(ic=0;ic<SCCRAT;ic++){
      printf("  r/c:%7d %7d",scdaqbc2[ic][0],scdaqbc2[ic][1]);
  }
  printf("\n");
  printf(" S-block errors (block length mismatch) : ");
  for(ic=0;ic<SCCRAT;ic++){
      printf("  r/c:%7d %7d",scdaqbc3[ic][0],scdaqbc3[ic][1]);
  }
  printf("\n");
  printf(" S-block errors(conflict with swid-map) : ");
  for(ic=0;ic<SCCRAT;ic++){
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
  printf(" Entries to TZSl-calibr. : % 6d\n",recount[6]);
  printf(" TZSl: multiplicity OK   : % 6d\n",recount[7]);
  printf(" TZSl: no interactions   : % 6d\n",recount[8]);
  printf(" TZSl: Tracker mom. OK   : % 6d\n",recount[9]);
  printf(" TZSl: TOF-Tr.match. OK  : % 6d\n",recount[10]);
  printf(" TZSl: TOF-self.match. OK: % 6d\n",recount[20]);
  printf(" Entries to AMPL-calibr. : % 6d\n",recount[11]);
  printf(" AMPL: multiplicity OK   : % 6d\n",recount[12]);
  printf(" AMPL: no interaction    : % 6d\n",recount[13]);
  printf(" AMPL: Track momentum OK : % 6d\n",recount[14]);
  printf(" AMPL: TOF-TRK match OK  : % 6d\n",recount[15]);
  printf(" Entr to STRR/AVSD-calibr: % 6d\n",recount[16]);
  printf(" Entries to TDIF-calibr. : % 6d\n",recount[17]);
  printf(" TDIF: multiplicity OK   : % 6d\n",recount[18]);
  printf(" TDIF: Tracker OK        : % 6d\n",recount[19]);
  printf("\n\n");
//
  if(!AMSJob::gethead()->isRealData() && TOFMCFFKEY.fast==1)return;
  if(TOFRECFFKEY.reprtf[0]==0)return;
//
//----------------------------------------------------------
//
  printf("===========> Channels validation report :\n\n");
//
  printf("H/w-status OK (validation) :\n\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      printf(" % 6d",chcount[ic][11]);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      printf(" % 6d",chcount[ic][11]);
    }
    printf("\n\n");
  }
//
  printf("Hist-TDC wrong up/down sequence (percentage) :\n");
  printf("\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      rc=geant(chcount[ic][11]);
      if(rc>0.)rc=100.*geant(chcount[ic][12])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][11]);
      if(rc>0.)rc=100.*geant(chcount[ic][12])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Stretcher-TDC wrong up/down sequence (percentage) :\n");
  printf("\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      rc=geant(chcount[ic][11]);
      if(rc>0.)rc=100.*geant(chcount[ic][13])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][11]);
      if(rc>0.)rc=100.*geant(chcount[ic][13])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("A-TDC wrong up/down sequence (percentage) :\n");
  printf("\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      rc=geant(chcount[ic][11]);
      if(rc>0.)rc=100.*geant(chcount[ic][14])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][11]);
      if(rc>0.)rc=100.*geant(chcount[ic][14])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("D-TDC wrong up/down sequence (percentage) :\n");
  printf("\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      rc=geant(chcount[ic][11]);
      if(rc>0.)rc=100.*geant(chcount[ic][15])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][11]);
      if(rc>0.)rc=100.*geant(chcount[ic][15])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
//----------------------------------------------------------
  if(TOFRECFFKEY.relogic[0]==1)return;// no reco for strr-calibr
//
  printf("==========> Bars reconstruction report :\n\n");
//
  printf("Bar H/w-status OK (at least 1 side is found) :\n\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR+ib;
      printf(" % 6d",brcount[ic][0]);
    }
    printf("\n\n");
  }
//
  printf("Bar 3-measurements found (at least for 1 side) :\n\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR+ib;
      rc=geant(brcount[ic][0]);
      if(rc>0.)rc=geant(brcount[ic][1])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Bar 1-side history 'OK' (for each of the EXISTING 3-measurements sides) :\n\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR+ib;
      rc=geant(brcount[ic][0]);
      if(rc>0.)rc=geant(brcount[ic][2])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Bar 2-sides history 'OK'(if there are 2x3meas. with good history) :\n\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR+ib;
      rc=geant(brcount[ic][0]);
      if(rc>0.)rc=geant(brcount[ic][3])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//---------------------------------------------------------
  printf("============> Channels reconstruction report :\n\n");
//
  printf("H/w-status OK :\n\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      printf(" % 6d",chcount[ic][0]);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      printf(" % 6d",chcount[ic][0]);
    }
    printf("\n\n");
  }
//
  printf("Hist-TDC 'ON' :\n");
  printf("\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][1])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][1])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Slow-TDC 'ON' :\n");
  printf("\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][2])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][2])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Anode-ADC 'ON' :\n");
  printf("\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][3])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][3])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Dinode-ADC 'ON' :\n");
  printf("\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][4])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][4])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("History-TDC '1 hit' :\n");
  printf("\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][5])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][5])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Slow-TDC '1 hit' :\n");
  printf("\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][6])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][6])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Anode-ADC '1 hit' :\n");
  printf("\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][7])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][7])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Dinode-ADC '1 hit' :\n");
  printf("\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][8])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][8])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("3-measurements(S-tds + F-tdc + A-tdc) :\n");
  printf("\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][9])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][9])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
}
//------------------------------------------
void TOFJobStat::bookhist(){
  int i,j,k,ich,il,ib,ii,jj,ic,is;
  char htit1[60];
  char inum[11];
  char in[2]="0";
//
  strcpy(inum,"0123456789");
//
  if(TOFRECFFKEY.reprtf[2]!=0 || // Reconstruction histograms
     (AMSJob::gethead()->isMonitoring() & (AMSJob::MTOF | AMSJob::MAll))){ // Monitoring Job
// Book reco-hist
    HBOOK1(1100,"Fast-Slow hit time-difference(single hist/slow-hit measurements)",80,0.,80.,0.);
    HBOOK1(1105,"Anode-Slow hit time-difference(single an/slow-hit measurements)",80,-40.,120.,0.);
    HBOOK1(1106,"Dynode-Slow hit time-difference(single dyn/slow-hit measurements)",80,-40.,120.,0.);
    HBOOK1(1107,"TOF+CTC+ANTI data length (16-bit words)",80,1.,1001.,0.);
    HBOOK1(1101,"Time_history:befor_hit dist(ns)",80,0.,160.,0.);
    HBOOK1(1102,"Time_history:after_hit dist(ns)",80,0.,1600.,0.);
    HBOOK1(1103,"Time_history: TovT(ns)",80,0.,80.,0.);
    HBOOK1(1104,"Anode TovT: TovT(ns)",80,0.,640.,0.);
    HBOOK1(1110,"RawClusterLevel:Total fired layers per event",5,0.,5.,0.);
    HBOOK1(1111,"RawClusterLevel:Layer appearence frequency",4,0.5,4.5,0.);
    HBOOK1(1112,"RawClusterLevel:Configuration(<2;2;>2->missingL)",10,-1.,9.,0.);
    HBOOK1(1113,"RawClusterLevel:SingleBarLayer Configuration(<2;2;>2->missingL)",10,-1.,9.,0.);
    HBOOK1(1114,"RawClusterLevel:Single2SidedBarLayer Configuration(<2;2;>2->missingL)",10,-1.,9.,0.);
    HBOOK1(1115,"Fast-Slow hit time-difference(all hist/slow-hit meas.",80,-40.,120.,0.);
    HBOOK1(1116,"dEdX vs bar (norm.inc.,L=1)",14,0.,14.,0.);
    HBOOK1(1117,"dEdX vs bar (norm.inc.,L=2)",14,0.,14.,0.);
    HBOOK1(1118,"dEdX vs bar (norm.inc.,L=3)",14,0.,14.,0.);
    HBOOK1(1119,"dEdX vs bar (norm.inc.,L=4)",14,0.,14.,0.);
    if(TOFRECFFKEY.reprtf[2]>1){
      HBOOK1(1529,"L=1,Edep_anode(mev),corr,ideal evnt",80,0.,16.,0.);
      HBOOK1(1526,"L=1,Edep_anode(mev),corr,ideal evnt",80,0.,80.,0.);
      HBOOK1(1531,"L=1,Edep_dinode(mev),corr,ideal evnt",80,0.,16.,0.);
      HBOOK1(1528,"L=1,Edep_dinode(mev),corr,ideal evnt",80,0.,80.,0.);
      for(il=0;il<SCLRS;il++){
        for(ib=0;ib<SCMXBR;ib++){
    	  strcpy(htit1,"dE/dX (norm.inc) for bar(LBB) ");
  	  in[0]=inum[il+1];
  	  strcat(htit1,in);
  	  ii=(ib+1)/10;
	  jj=(ib+1)%10;
	  in[0]=inum[ii];
	  strcat(htit1,in);
	  in[0]=inum[jj];
	  strcat(htit1,in);
	  ich=SCMXBR*il+ib;
	  HBOOK1(1140+ich,htit1,50,0.,15.,0.);
        }
      }
    }
    if(TOFRECFFKEY.reprtf[2]>1){
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
    if(TOFRECFFKEY.reprtf[4]>0){ // Str.pulse width for selected channel
      ich=TOFRECFFKEY.reprtf[4];// LBBS
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
    HBOOK1(1532,"(T1-T3)(ns),corr,trl-normalized,ideal evnt",80,1.,9.,0.);
//    HBOOK1(1533,"L=1,side1/2 raw T-diff(ns),ideal evnt",100,-2.,2.,0.);
//    HBOOK1(1543,"L=1,Y-local(longit.coord),ideal evnt",100,-50.,50.,0.);
    HBOOK1(1534,"(T2-T4)(ns),corr,trl-normalized,ideal evnt",80,1.,9.,0.);
    HBOOK1(1544,"(T1-T3)-(T2-T4),(ns),corr,ideal evnt",80,-4.,4.,0.);
    HBOOK1(1535,"L=1,TOF Eclust(mev)",80,0.,24.,0.);
    HBOOK1(1536,"L=3,TOF Eclust(mev)",80,0.,24.,0.);
    HBOOK1(1537,"L=1,TOF Eclust(mev)",80,0.,240.,0.);
    HBOOK1(1538,"L=3,TOF Eclust(mev)",80,0.,240.,0.);
    HBOOK1(1539,"L=2,TOF Eclust(mev)",80,0.,24.,0.);
    HBOOK1(1540,"L=4,TOF Eclust(mev)",80,0.,24.,0.);
    HBOOK1(1541,"L=2,TOF Eclust(mev)",80,0.,240.,0.);
    HBOOK1(1542,"L=4,TOF Eclust(mev)",80,0.,240.,0.);
    if(TOFRECFFKEY.relogic[0]==1){ // STRR-calibration
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
      if(TOFCAFFKEY.dynflg==1){ // for special(Contin's) Dynode calibr.
        HBOOK1(1240,"Slope in Td vs Ta correlation",50,0.,2.,0.);
        HBOOK1(1241,"Offset in Td vs Ta correlation",50,-200.,50.,0.);
        HBOOK1(1242,"Chi2 in Td vs Ta correlation",50,0.,10.,0.);
// hist.1800-1911 are booked in init-function for Tin vs Tout correl.!!!
      }
    }
    if(TOFRECFFKEY.relogic[0]==2){// TDIF-calibr. runs
      HBOOK1(1500,"Part.rigidity from tracker(gv)",80,0.,32.,0.);
    }
    if(TOFRECFFKEY.relogic[0]==3){ // TZSL-calibration
      HBOOK1(1500,"Part.rigidity from tracker(gv)",80,0.,24.,0.);
      HBOOK1(1501,"Particle beta(tracker)",80,0.6,1.,0.);
      HBOOK1(1506,"Tracks multipl. in calib.events",10,0.,10.,0.);
      HBOOK1(1200,"Res_long.coo(track-sc),L=1",50,-10.,10.,0.);
      HBOOK1(1201,"Res_long.coo(track-sc),L=2",50,-10.,10.,0.);
      HBOOK1(1202,"Res_long.coo(track-sc),L=3",50,-10.,10.,0.);
      HBOOK1(1203,"Res_long.coo(track-sc),L=4",50,-10.,10.,0.);
      HBOOK2(1204,"Res_long. vs track coord.,L1",50,-50.,50.,50,-10.,10.,0.);
      HBOOK2(1205,"Res_long. vs track coord.,L2",50,-50.,50.,50,-10.,10.,0.);
      HBOOK2(1206,"Res_long. vs track coord.,L3",50,-50.,50.,50,-10.,10.,0.);
      HBOOK2(1207,"Res_long. vs track coord.,L4",50,-50.,50.,50,-10.,10.,0.);
      HBOOK1(1210,"Res_transv.coo(track-sc),L=1",50,-20.,20.,0.);
      HBOOK1(1211,"Res_transv.coo(track-sc),L=2",50,-20.,20.,0.);
      HBOOK1(1212,"Res_transv.coo(track-sc),L=3",50,-20.,20.,0.);
      HBOOK1(1213,"Res_transv.coo(track-sc),L=4",50,-20.,20.,0.);
      HBOOK1(1215,"(Cos_tr-Cos_sc)/Cos_tr",50,-1.,1.,0.);
      HBOOK1(1217,"Cos_sc",50,0.5,1.,0.);
      HBOOK1(1218,"TOF track-fit chi2-x",50,0.,5.,0.);
      HBOOK1(1219,"TOF track-fit chi2-y",50,0.,5.,0.);
      HBOOK1(1503,"Anticounter energy(4Lx1bar events)(mev)",80,0.,20.,0.);
      HBOOK1(1505,"Qmax ratio",80,0.,16.,0.);
      HBOOK1(1507,"T0-difference inside bar-types 5",80,-0.4,0.4,0.);
      HBOOK2(1514,"Layer-1,T vs SUM(1/Q)",50,0.,0.5,50,1.,11.,0.);
      HBOOK1(1524,"TRlen13-TRlen24",80,-4.,4.,0.);
//      HBOOK1(1550,"Bar-time(corected),L=1",80,24.,26.,0.);
//      HBOOK1(1551,"Bar-time(corected),L=2",80,23.5,25.5,0.);
//      HBOOK1(1552,"Bar-time(corected),L=3",80,19.5,21.5,0.);
//      HBOOK1(1553,"Bar-time(corected),L=4",80,19.,21.,0.);
    }
    if(TOFRECFFKEY.relogic[0]==4){ // AMPL-calibration
      HBOOK1(1506,"Tracks multipl. in calib.events",10,0.,10.,0.);
      HBOOK1(1500,"Part.rigidity from tracker(gv)",80,0.,32.,0.);
      HBOOK1(1501,"Particle beta(tracker)",80,0.5,1.,0.);
      HBOOK1(1502,"Particle beta(tof)",80,0.7,1.2,0.);
      HBOOK1(1503,"Anticounter energy(4Lx1bar events)(mev)",80,0.,40.,0.);
      HBOOK1(1200,"Res_long.coo(track-sc),L=1",50,-10.,10.,0.);
      HBOOK1(1201,"Res_long.coo(track-sc),L=2",50,-10.,10.,0.);
      HBOOK1(1202,"Res_long.coo(track-sc),L=3",50,-10.,10.,0.);
      HBOOK1(1203,"Res_long.coo(track-sc),L=4",50,-10.,10.,0.);
      HBOOK1(1204,"Mass**2",80,-1.6,3.2,0.);
      HBOOK1(1207,"Mass**2 for beta<0.92",80,-1.6,3.2,0.);
      HBOOK1(1208,"Ptr/impl.mass",80,0.,16.,0.);
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
      HBOOK1(1250,"Ref.bar(type=5) Q-distr.(s=1,centre)",80,0.,240.,0.);        
      HBOOK1(1251,"Ref.bar(type=5) Q-distr.(s=2,centre)",80,0.,240.,0.);
      HBOOK1(1252,"Relative anode-gains(all channels)",80,0.5,2.,0.);
      HBOOK1(1254,"Ref.bar A-profile (type-1)",70,-70.,70.,0.);        
      HBOOK1(1255,"Ref.bar A-profile (type-2)",70,-70.,70.,0.);        
      HBOOK1(1256,"Ref.bar A-profile (type-3)",70,-70.,70.,0.);        
      HBOOK1(1257,"Ref.bar A-profile (type-4)",70,-70.,70.,0.);        
      HBOOK1(1258,"Ref.bar A-profile (type-5)",70,-70.,70.,0.);        
      HBOOK1(1259,"Anode_to_Dinode signal ratio(all channels)",80,9.,11.,0.);
      HBOOK1(1260,"Anode_to_Dinode ratio error(all channels)",80,0.,0.4,0.);
    }
    if(TOFRECFFKEY.reprtf[3]!=0){//TDC-hit multiplicity histograms
      for(il=0;il<SCLRS;il++){
	for(ib=0;ib<SCMXBR;ib++){
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
	    ich=2*SCMXBR*il+2*ib+i;
	    HBOOK1(1300+ich,htit1,80,0.,80.,0.);
	  }
	}
      }
    }
  }
}
//-----------------------------
void TOFJobStat::bookhistmc(){
    if(TOFMCFFKEY.mcprtf[2]!=0){ // Book mc-hist
      HBOOK1(1050,"Geant-hits in layer-1",80,0.,80.,0.);
      HBOOK1(1051,"Geant-hits in layer-2",80,0.,80.,0.);
      HBOOK1(1052,"Geant-hits in layer-3",80,0.,80.,0.);
      HBOOK1(1053,"Geant-hits in layer-4",80,0.,80.,0.);
      HBOOK1(1060,"Geant-Edep(mev) in layer-1",80,0.,24.,0.);
      HBOOK1(1061,"Geant-Edep(mev) in layer-1",80,0.,240.,0.);
      HBOOK1(1062,"Geant-Edep(mev) in layer-3",80,0.,24.,0.);
      HBOOK1(1063,"Geant-Edep(mev) in layer-3",80,0.,240.,0.);
      HBOOK2(1070,"Log(Qa) vs TovT,all channels",70,0.,280.,70,0.,8.4,0.);
      HBOOK1(1071,"Total bar pulse-charge(pC),L-1",80,0.,1600.,0.);
      HBOOK1(1072,"Total bar pulse-charge(pC),L-1",80,0.,16000.,0.);
      HBOOK1(1073,"PMT-pulse amplitude(mV,id=108,s1)",80,0.,1000.,0.);
      HBOOK1(1074,"Shaper amplitude (pC),all channels",80,0.,160.,0.);
      HBOOK1(1075,"A-Shaper TovT (ns)",80,0.,640.,0.);
      HBOOK1(1076,"D-Shaper TovT (ns)",80,0.,640.,0.);
      HBOOK2(1077,"TovT vs Q, MC A-Integrator",80,0.,4000,70,0.,490.,0.);
    }
}
//----------------------------
void TOFJobStat::outp(){
  int i,j,k,ich;
  geant dedx[SCMXBR],dedxe[SCMXBR];
       if(TOFRECFFKEY.reprtf[2]!=0){ // print RECO-hists
         HPRINT(1535);
         HPRINT(1536);
         HPRINT(1537);
         HPRINT(1538);
         HPRINT(1539);
         HPRINT(1540);
         HPRINT(1541);
         HPRINT(1542);
         HPRINT(1100);
         HPRINT(1115);
         HPRINT(1105);
         HPRINT(1106);
         HPRINT(1107);
         HPRINT(1101);
         HPRINT(1102);
         HPRINT(1103);
         HPRINT(1104);
         HPRINT(1110);
         HPRINT(1111);
         HPRINT(1112);
         HPRINT(1113);
         HPRINT(1114);
         if(TOFRECFFKEY.reprtf[2]>1){
           HPRINT(1529);
           HPRINT(1526);
           HPRINT(1528);
           HPRINT(1531);
           for(i=0;i<SCLRS;i++){
             for(j=0;j<SCMXBR;j++){
               ich=SCMXBR*i+j;
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
           if(TOFRECFFKEY.reprtf[2]>1){
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
         if(TOFRECFFKEY.reprtf[4]>0){
           HPRINT(1138);
           HPRINT(1139);
         }
           HPRINT(1532);
//           HPRINT(1533);
//           HPRINT(1543);
           HPRINT(1544);
           HPRINT(1534);
         if(TOFRECFFKEY.reprtf[3]!=0){//TDC-hit multiplicity histograms
           for(i=0;i<SCLRS;i++){
             for(j=0;j<SCMXBR;j++){
               for(k=0;k<2;k++){
                 ich=2*SCMXBR*i+2*j+k;
                 HPRINT(1300+ich);
               }
             }
           }
         }
       }
// ---> calibration specific :
       if(TOFRECFFKEY.relogic[0]==3){// for TZSL-calibr. runs
         HPRINT(1500);
         HPRINT(1501);
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
           TOFTZSLcalib::mfit();
           HPRINT(1507);
       }
//
       if(TOFRECFFKEY.relogic[0]==4){// for AMPL-calibr. runs
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
             TOFAMPLcalib::fit();
           HPRINT(1259);
           HPRINT(1260);
           HPRINT(1252);
           HPRINT(1254);
           HPRINT(1255);
           HPRINT(1256);
           HPRINT(1257);
           HPRINT(1258);
       }
//
//
       if(TOFRECFFKEY.relogic[0]==1){// for STRR-calibr. runs
           TOFSTRRcalib::outp();
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
           if(TOFCAFFKEY.dynflg==1){
             TOFAVSDcalib::fit();
             HPRINT(1240);
             HPRINT(1241);
             HPRINT(1242);
           }
       }
//
       if(TOFRECFFKEY.relogic[0]==2){// for TDIF-calibr. runs
           HPRINT(1500);
           TOFTDIFcalib::fit();
       }
//
}
//----------------------------
void TOFJobStat::outpmc(){
       if(TOFMCFFKEY.mcprtf[2]!=0 && TOFMCFFKEY.fast==0){ // print MC-hists
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
         HPRINT(1076);
         HPRINT(1077);
         if(TOFCAFFKEY.mcainc)AMSTOFTovt::aintfit();
       }
}
//==========================================================================

