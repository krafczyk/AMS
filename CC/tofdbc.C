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
  geant TOFDBc::_shrtim=0.5;     // MC shaper pulse rise time (ns)(exp)
  geant TOFDBc::_shftim=50.;     // MC shaper pulse fall time (ns)(exp)
  geant TOFDBc::_strflu=0.05;    // Stretcher "end-mark" time fluctuations (ns)
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
    20.,    // (12)pulse width of FT_coincidence-signal(ns) (as dummy gap in s-TDC pulse)
    0.,     // spare 
    0.      // spare 
  };
  geant TOFDBc::_trigtb=0.25;  // MC time-bin in logic(trig) pulse handling (ns)
  geant TOFDBc::_di2anr=0.1;  // dinode->anode signal ratio (default,mc-data)
  geant TOFDBc::_strrat=40.;  // stretcher ratio (default,mc-data)
  geant TOFDBc::_strjit1=0.025;  // "start"-pulse jitter at stretcher input
  geant TOFDBc::_strjit2=0.025;  // "stop"(FT)-pulse jitter at stretcher input
  geant TOFDBc::_accdel=5000.;//Lev-1 signal delay with respect to FT (ns)
  geant TOFDBc::_ftdelf=45.;  // FastTrigger (FT) fixed (by h/w) delay (ns)
  geant TOFDBc::_ftdelm=100.; // FT max delay (allowed by stretcher logic) (ns)
  geant TOFDBc::_fstdcd=5.;   // Same hit(up-edge) relative delay in fast-slow TDC
  geant TOFDBc::_fatdcd=5.;   // Same hit(up-edge) relative delay in fast-A(D) TDC
  integer TOFDBc::_pbonup=1;  // set phase-bit for leading(up) edge (yes/no->1/0) 
//
//  member functions :
//
// ---> function to read geomconfig-files ( fn[3] - hollerith file name)
//                                                 Called from TOFgeom :
  void TOFDBc::readgconf(integer fn[3]){
    int i;
    char fname[80];
    char name[19];
    char vers1[3]="01";
    char vers2[3]="02";
    UHTOC(fn,4,name,12);
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
//    strcpy(fname,AMSDATADIR.amsdatadir);    
    strcpy(fname,"/afs/cern.ch/user/c/choumilo/public/ams/AMS/tofca/");//tempor
    strcat(fname,name);
    cout<<"Open file : "<<fname<<'\n';
    ifstream tcfile(fname,ios::in); // open needed config-file for reading
    if(!tcfile){
      cerr <<"TOFgeom-read: Error open geomconfig-file "<<fname<<endl;
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
  if(il==1)
    zc=_supstr[0]-_plnstr[1]-dz/2.;
  if(il==2)
    zc=_supstr[1]+_plnstr[1]+dz/2.;
  if(il==3)
    zc=_supstr[1]-_supstr[6]-_plnstr[0]-dz/2.;
  if(il==0)zc=zc+(ib%2)*_plnstr[2];
  if(il==3)zc=zc-(ib%2)*_plnstr[2];
  if(il==1)zc=zc-((ib+1)%2)*_plnstr[2];
  if(il==2)zc=zc+((ib+1)%2)*_plnstr[2];
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
 integer i,j,ila,ibr,ibrm,isp,nsp,ibt,cnum,dnum,mult;
 geant scp[SCANPNT];
 geant rlo[SCANPNT];// relat.(to Y=0) light output
 integer lps=1000;
 geant ef1[SCANPNT],ef2[SCANPNT];
 integer i1,i2,sta[2];
 geant r,eff1,eff2;
 integer sid,brt;
 geant gna[2],gnd[2],qath,qdth,a2dr[2],tth,strat[2];
 geant slope,fstrd,tzer,tdif,mip2q,speedl;
 geant tzerf[SCLRS][SCMXBR],slpf,strf[SCBLMX][2],tdiff[SCBLMX];
 geant an2di[SCBLMX][2],gaina[SCBLMX][2],gaind[SCBLMX][2],m2q[SCBTPN]; 
 char fname[80];
 char name[80];
 geant td2p[2];
 char vers1[3]="mc";
 char vers2[3]="rl";
 int mrfp;
//
 geant asatl=20.;//(mev,~10MIPs),if E-dinode(1-end) higher - use it instead
//                                 of anode measurements
//---> TovT-electronics calibration for MC:
//
 geant aip[2][3]={ // def.param. for anode integrator(shft,t0(qthr=exp(t0/shft)),qoffs)
                   {50.,62.6,1.3},
                   {50.,62.6,1.3}
                 };
 geant dip[2][3]={              // default param. for dinode integrator
                   {50.,62.6,1.3},
                   {50.,62.6,1.3}
                 };
//
//------------------------------
//
//   --->  Read (lspeed/tdiffs + slope/tzeros) calibration file :
//
 UHTOC(TOFRECFFKEY.tzerca,4,name,12);
 if(AMSJob::gethead()->isMCData()) //      for MC-event
 {
       cout <<" TOFBrcal_build: MC-T0/Tdif-calibration is used"<<endl;
       strcat(name,vers1);
 }
 else                              //      for Real events
 {
       cout <<" TOFBrcal_build: REAL-T0/Tdif-calibration is used"<<endl;
       strcat(name,vers2);
 }
//
 strcat(name,".dat");
// strcpy(fname,AMSDATADIR.amsdatadir);    
 strcpy(fname,"/afs/cern.ch/user/c/choumilo/public/ams/AMS/tofca/");//tempor
 strcat(fname,name);
 cout<<"Open file : "<<fname<<'\n';
 ifstream tcfile(fname,ios::in); // open  file for reading
 if(!tcfile){
   cerr <<"TOFBrcal_build: Error open Lspeed/Tdif/Tzero/Slope-file "<<fname<<endl;
   exit(1);
 }
//
// ---> read Lspeed/Tdiffs:
 tcfile >> speedl;
 for(ila=0;ila<SCLRS;ila++){   
 for(ibr=0;ibr<SCMXBR;ibr++){  
   cnum=ila*SCMXBR+ibr; // sequential counters numbering(0-55)
   tcfile >> tdiff[cnum];
 }
 }
// ---> read Slope/Tzeros:
 tcfile >> slpf;
 for(ila=0;ila<SCLRS;ila++){ 
 for(ibr=0;ibr<SCMXBR;ibr++){
   tcfile >> tzerf[ila][ibr];
 } 
 }
 tcfile.close();
//
//-------------------------------------------- 
//
//   --->  Read stretcher-ratio calib.file :
//
 UHTOC(TOFRECFFKEY.strrca,4,name,12);
 if(AMSJob::gethead()->isMCData())           // for MC-event
 {
   cout <<" TOFBrcal_build: str_ratio-calib. for MC-events selected."<<endl;
   for(ila=0;ila<SCLRS;ila++){   // <-------- loop over layers
   for(ibr=0;ibr<SCMXBR;ibr++){  // <-------- loop over bar in layer
     cnum=ila*SCMXBR+ibr; // sequential counters numbering(0-55)
     strf[cnum][0]=TOFDBc::strrat();// const.,fixed = MC-default
     strf[cnum][1]=TOFDBc::strrat();
   } // --- end of bar loop --->
   } // --- end of layer loop --->
 }
 else                                       // for Real events
 {
   cout <<" TOFBrcal_build: str_ratio-calib. for Real-events selected."<<endl;
   strcat(name,vers2);
   strcat(name,".dat");
   strcpy(fname,AMSDATADIR.amsdatadir);    
//   strcpy(fname,"/afs/cern.ch/user/c/choumilo/public/ams/AMS/tofca/");
   strcat(fname,name);
   cout<<"Open file : "<<fname<<'\n';
   ifstream scfile(fname,ios::in); // open str_ratio-file for reading
   if(!scfile){
     cerr <<"TOFBrcal_build: Error open str_ratio-file "<<fname<<endl;
     exit(1);
   }
   for(ila=0;ila<SCLRS;ila++){   // <-------- loop over layers
   for(ibr=0;ibr<SCMXBR;ibr++){  // <-------- loop over bar in layer
     cnum=ila*SCMXBR+ibr; // sequential counter numbering(0-55)
     scfile >> strf[cnum][0];
     scfile >> strf[cnum][1];
   } // --- end of bar loop --->
   } // --- end of layer loop --->
   scfile.close();
 }
//---------------------------------------------
//
//   --->  Read a2d-ratios, gains and mip2q's calib.file :
//
 UHTOC(TOFRECFFKEY.amplca,4,name,12);
 if(AMSJob::gethead()->isMCData())           // for MC-event
 {
   cout <<" TOFBrcal_build: a2d_ratio/gain-calib. for MC-events selected."<<endl;
   strcat(name,vers1);
 }
 else                                       // for Real events
 {
   cout <<" TOFBrcal_build: a2d_ratio/gain-calib. for Real-events selected."<<endl;
   strcat(name,vers2);
 }
   strcat(name,".dat");
//   strcpy(fname,AMSDATADIR.amsdatadir);    
   strcpy(fname,"/afs/cern.ch/user/c/choumilo/public/ams/AMS/tofca/");//tempor
   strcat(fname,name);
   cout<<"Open file : "<<fname<<'\n';
   ifstream gcfile(fname,ios::in); // open a2d_ratio/gain/mip2q-file for reading
   if(!gcfile){
     cerr <<"TOFBrcal_build: Error open a2d_ratio/gain/mip2q-file "<<fname<<endl;
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
   gcfile.close();
//   
//---------------------------------------------
//   ===> fill TOFBrcal bank :
//
 if(AMSJob::gethead()->isMCData()){ //            =====> For MC data:
//
  for(ila=0;ila<SCLRS;ila++){   // <-------- loop over layers
  for(ibr=0;ibr<SCMXBR;ibr++){  // <-------- loop over bar in layer
    brt=TOFDBc::brtype(ila,ibr);
    if(brt==0)continue; // skip missing counters
    cnum=ila*SCMXBR+ibr; // sequential counter numbering(0-55)
    scmcscan[cnum].getscp(scp);//read scan-points from scmcscan-object
// read from file or DB:
    gna[0]=gaina[cnum][0];
    gna[1]=gaina[cnum][1];
    gnd[0]=gna[0];// tempor
    gnd[1]=gna[1];
    tth=tofvpar.daqthr(0); // (mV), time-discr. threshold
    qath=tofvpar.daqthr(3); // (pC) thresh. at shaper inp.(anode) (may be diff.
//              from tofvpar.daqthr(3) in reality !!!, but proportional to him)
    qdth=tofvpar.daqthr(4); // ...................        (dinode) ................
    mip2q=m2q[brt-1];//(pC/mev),dE(mev)_at_counter_center->Q(pC)_at_PM_anode(2x3-sum)
    a2dr[0]=1./TOFDBc::di2anr();// anode_to_dinode signal ratio from MC
    a2dr[1]=1./TOFDBc::di2anr();// same for side-2
    fstrd=TOFDBc::fstdcd();//(ns),same hit(up-edge) relat.delay in fast-slow TDC
    mrfp=SCANPNT/2+1;
    scmcscan[cnum].getefarr(ef1,ef2);//read eff1/2 from scmcscan-object
    for(isp=0;isp<SCANPNT;isp++){ // fill 2-ends rel. l.output at scan-points
      rlo[isp]=(ef1[isp]+ef2[isp])/(ef1[mrfp]+ef2[mrfp]);
    }
  //
    sid=100*(ila+1)+(ibr+1);
    strat[0]=strf[cnum][0];
    strat[1]=strf[cnum][1];
    slope=slpf;//was read from ext. file
    tzer=tzerf[ila][ibr];//was read from ext. file
    tdif=tdiff[cnum];//was read from ext. file 
    td2p[0]=speedl;//mean speed of the light was read from external file
    td2p[1]=1.3; // error on longit. coord. measurement(cm)
    sta[0]=0;//status ok, really should be taken from slow-control data or manually
    sta[1]=0;
    scbrcal[ila][ibr]=TOFBrcal(sid,sta,gna,gnd,qath,qdth,a2dr,asatl,tth,
                              strat,fstrd,tzer,slope,tdif,td2p,mip2q,scp,rlo,
                              aip,dip);
//
  } // --- end of bar loop --->
  } // --- end of layer loop --->
//
 } // <--- end of MC branch --
//--------------------------------------
 else{ //                                       =====> For Real Data :
//
  for(ila=0;ila<SCLRS;ila++){   // <-------- loop over layers
  for(ibr=0;ibr<SCMXBR;ibr++){  // <-------- loop over bar in layer
    brt=TOFDBc::brtype(ila,ibr);
    if(brt==0)continue; // skip missing counters
    cnum=ila*SCMXBR+ibr; // sequential counter numbering(0-55)
    scmcscan[cnum].getscp(scp);//read sc.point from scmcscan-object
// read from file or DB:
    gna[0]=gaina[cnum][0];
    gna[1]=gaina[cnum][1];
    gnd[0]=gna[0];// tempor
    gnd[1]=gna[1];
    tth=tofvpar.daqthr(0); // (mV), time-discr. threshold
    qath=tofvpar.daqthr(3); // (pC) thresh. at shaper inp.(anode) (may be diff.
//              from tofvpar.daqthr(3) in reality !!!, but proportional to him)
    qdth=tofvpar.daqthr(4); // ...................        (dinode) ................
    mip2q=m2q[brt-1];//(pC/mev),dE(mev)_at_counter_center->Q(pC)_at_PM_anode(2x3-sum)
    a2dr[0]=an2di[cnum][0];// from ext.file
    a2dr[1]=an2di[cnum][1];
    fstrd=TOFDBc::fstdcd();//(ns),same hit(up-edge) relat.delay in fast-slow TDC
    mrfp=SCANPNT/2+1;
    scmcscan[cnum].getefarr(ef1,ef2);//read eff1/2 from scmcscan-object
    for(isp=0;isp<SCANPNT;isp++){ // fill 2-ends rel. l.output at scan-points
      rlo[isp]=(ef1[isp]+ef2[isp])/(ef1[mrfp]+ef2[mrfp]);
    }
  //
    sid=100*(ila+1)+(ibr+1);
    strat[0]=strf[cnum][0];
    strat[1]=strf[cnum][1];
    slope=slpf;//was read from ext. file
    tzer=tzerf[ila][ibr];//was read from ext. file
    tdif=tdiff[cnum];//was read from ext. file 
    td2p[0]=speedl;//mean speed of the light was read from external file
    td2p[1]=1.3; // error on longit. coord. measurement(cm)
    sta[0]=0;//status ok, really should be taken from slow-control data or manually
    sta[1]=0;
    scbrcal[ila][ibr]=TOFBrcal(sid,sta,gna,gnd,qath,qdth,a2dr,asatl,tth,
                              strat,fstrd,tzer,slope,tdif,td2p,mip2q,scp,rlo,
                              aip,dip);
//
  } // --- end of bar loop --->
  } // --- end of layer loop --->
//
 } // <--- end of REAL branch --
}
//------------------------------------------------------------
geant TOFBrcal::ama2mip(number amf[2]){ // side A-Tovt's(ns) -> Etot(Mev)
  number q(0),qt(0);
  for(int isd=0;isd<2;isd++){
    q2t2q(1,isd,0,amf[isd],q);
    qt+=(q/gaina[isd]);// Qa->Qa_gain_corrected
  }
  qt=qt/mip2q; // Q(pC)->Mev
  return geant(qt);
}
//------
void TOFBrcal::ama2q(number amf[2], number qs[2]){// side A-Tovt's(ns) -> Q(pC)
//                                                 to use in calibr. program 
  for(int isd=0;isd<2;isd++)
                            q2t2q(1,isd,0,amf[isd],qs[isd]);
}
//------
void TOFBrcal::q2t2q(int cof, int sdf, int adf, number &tovt, number &q){  
// Q(pC) <-> Tovt(ns) to use in sim./rec. programs (cof=0/1-> Q->Tovt/Tovt->Q)
//                                                 (sdf=0/1-> bar side 1/2   )
//                                                 (adf=0/1-> for anode/dinode)
  number qoffs,shft,qthr;
  if(adf==0){
    shft=aipar[sdf][0];
    qthr=exp(aipar[sdf][1]/shft);//to match old parametrization
    qoffs=aipar[sdf][2];
  }
  else{
    shft=dipar[sdf][0];
    qthr=exp(dipar[sdf][1]/shft);
    qoffs=dipar[sdf][2];
  }
// 
  if(cof==0){ // q->tovt
    if(q>qoffs)tovt=shft*log((q-qoffs)/qthr);
    else tovt=0.;
  }
  else{       // tovt->q
    q=qoffs+qthr*exp(tovt/shft);
  }
}
//------
geant TOFBrcal::amd2mip(number amf[2]){ // side A-Tovt's(ns) -> Etot(Mev)
  number q(0),qt(0);
  for(int isd=0;isd<2;isd++){
    q2t2q(1,isd,1,amf[isd],q);
    qt+=(q*an2dir[isd]/gaind[isd]);// Qd->Qd_gain_corrected
  }
  qt=qt/mip2q; // Q(pC)->Mev
  return geant(qt);
}
//-----
void TOFBrcal::amd2q(number amf[2], number qs[2]){// side A-Tovt's(ns) -> Q(pC)
//                                                 to use in calibr. program 
  number q,qt(0);
  for(int isd=0;isd<2;isd++)
                            q2t2q(1,isd,1,amf[isd],qs[isd]);
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
  number time;
  shft=TOFDBc::shftim();
  time=0.5*(tmf[0]+tmf[1])+tzero
                     +slope*(exp(-amf[0]/shft)+exp(-amf[1]/shft));
  return geant(time); 
}
//-----
void TOFBrcal::tmd2p(number tmf[2], number amf[2],
                              geant &co, geant &eco){//(time-diff)->loc.coord/err(cm)
  geant shft;
  number time,coo;
  shft=TOFDBc::shftim();
  coo=-(0.5*(tmf[0]-tmf[1])+slope*(exp(-amf[0]/shft)-exp(-amf[1]/shft))-yctdif);  
//common "-" is due to the fact that Tmeas=Ttrig-Tabs and coo-loc is prop. to Tabs1-Tabs2
  co=td2pos[0]*geant(coo);//coo(ns)->cm                    
  eco=td2pos[1];
}
//-----
void TOFBrcal::td2ctd(number tdo, number amf[2],
                              number &tdn){//td0=0.5*(t[0]-t[1])->tdn (A-corrected)
  geant shft;
  shft=TOFDBc::shftim();
  tdn=tdo+slope*(exp(-amf[0]/shft)-exp(-amf[1]/shft));
}
//==========================================================================
//
//   TOFJobStat static variables definition (memory reservation):
//
integer TOFJobStat::mccount[SCJSTA];
integer TOFJobStat::recount[SCJSTA];
integer TOFJobStat::chcount[SCCHMX][SCCSTA];
integer TOFJobStat::brcount[SCBLMX][SCCSTA];
//
// function to print Job-statistics at the end of JOB(RUN):
//
void TOFJobStat::print(){
  int il,ib,ic;
  geant rc;
  printf("\n");
  printf("    ============ JOB TOF-statistics =============\n");
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
  printf(" TZSl: beta OK           : % 6d\n",recount[9]);
  printf(" Entries to AMPL-calibr. : % 6d\n",recount[10]);
  printf(" AMPL: multiplicity OK   : % 6d\n",recount[11]);
  printf(" AMPL: no interaction    : % 6d\n",recount[12]);
  printf(" AMPL: matching OK       : % 6d\n",recount[13]);
  printf(" Entries to STRR-calibr. : % 6d\n",recount[15]);
  printf(" Entries to TDIF-calibr. : % 6d\n",recount[17]);
  printf(" TDIF: multiplicity OK   : % 6d\n",recount[18]);
  printf(" TDIF: matching OK       : % 6d\n",recount[19]);
  printf("\n\n");
//
  if(!AMSJob::gethead()->isRealData() && TOFMCFFKEY.fast==1)return;
//
  printf("==========> Bars reconstruction report :\n\n");
//
  printf("Bar H/w-2-sides status OK :\n\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR+ib;
      printf(" % 6d",brcount[ic][0]);
    }
    printf("\n\n");
  }
//
  printf("In-Bar mult. 'OK' :\n\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR+ib;
      rc=geant(brcount[ic][0]);
      if(rc>0.)rc=geant(brcount[ic][1])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
  }
//
  printf("In-Bar history 'OK' :\n\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR+ib;
      rc=geant(brcount[ic][0]);
      if(rc>0.)rc=geant(brcount[ic][2])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
  }
//

if(TOFRECFFKEY.reprtf[0]==0)return;
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
//------------------------------------------------------
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
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][1])/rc;
      printf("% 5.3f",rc);
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
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][2])/rc;
      printf("% 5.3f",rc);
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
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][3])/rc;
      printf("% 5.3f",rc);
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
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][4])/rc;
      printf("% 5.3f",rc);
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
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][5])/rc;
      printf("% 5.3f",rc);
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
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][6])/rc;
      printf("% 5.3f",rc);
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
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][7])/rc;
      printf("% 5.3f",rc);
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
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][8])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
  }
//
}
//==========================================================================

