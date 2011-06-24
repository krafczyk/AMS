//  $Id: tofdbc02.C,v 1.88 2011/06/24 09:16:20 choumilo Exp $
// Author E.Choumilov 14.06.96.
#include "typedefs.h"
#include <math.h>
#include "commons.h"
#include "job.h"
#include "tofdbc02.h"
#include <stdio.h>
#include "tofid.h"
#include "tofsim02.h"
#include "tofrec02.h"
#include "tofcalib02.h"
#include "charge.h"
//
using namespace AMSChargConst;
//
TOF2Varp TOF2Varp::tofvpar; // mem.reserv. TOF general parameters 
TofSlowTemp TofSlowTemp::tofstemp; // mem.reserv. TOF slow temperatures 
TOF2Brcal TOF2Brcal::scbrcal[TOF2GC::SCLRS][TOF2GC::SCMXBR];// mem.reserv. TOF indiv.bar param.
uinteger TOF2Brcal::CFlistC[11]; 
TOFBrcalMS TOFBrcalMS::scbrcal[TOF2GC::SCLRS][TOF2GC::SCMXBR];// the same for "MC Seeds" 
TOFBPeds TOFBPeds::scbrped[TOF2GC::SCLRS][TOF2GC::SCMXBR];//mem.reserv. TOF-bar pedestals/sigmas/...
TofElosPDF TofElosPDF::TofEPDFs[MaxZTypes];
TofTdcCor TofTdcCor::tdccor[TOF2GC::SCCRAT][TOF2GC::SCFETA-1];
TofTdcCorMS TofTdcCorMS::tdccor[TOF2GC::SCCRAT][TOF2GC::SCFETA-1];
//-----------------------------------------------------------------------
//  =====> TOF2DBc class variables definition :
//
integer TOF2DBc::debug=1;
//
//
//======> memory reservation for _brtype :
// (real values are initialized from ext. geomconfig-file in amsgeom.c) 
integer TOF2DBc::_brtype[TOF2GC::SCBLMX]={
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
};
//
// Initialize TOF geometry (defaults, REAL VALUES are read FROM geomconfig-file)
//
//---> bar lengthes (cm) for each of TOF2GC::SCBTPN types :
geant TOF2DBc::_brlen[TOF2GC::SCBTPN]={
  0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.
};
//---> bar eff.light-guide lengthes (cm) for TOF2GC::SCBTPN types :
geant TOF2DBc::_lglen[TOF2GC::SCBTPN]={
  0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.
};
//---> plane rotation mask for 4 layers (1/0 -> rotated/not):
integer TOF2DBc::_plrotm[TOF2GC::SCLRS]={
  1,0,0,1
};
//---> real planes:
integer TOF2DBc::_planes=0;
//
//---> real bars/plane for 4 planes:
integer TOF2DBc::_bperpl[TOF2GC::SCLRS]={
  0,0,0,0
};
//---> outer counter parameters(1-4) for 4 planes
geant TOF2DBc::_outcp[TOF2GC::SCLRS][4]={
  0., 0., 0., 0.,
  0., 0., 0., 0.,
  0., 0., 0., 0.,
  0., 0., 0., 0.
};
//---> honeycomb supporting str. data:
geant TOF2DBc::_supstr[12]={
  0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.
};
//---> sc. plane design parameters:
geant TOF2DBc::_plnstr[20]={
  0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.
};
//---> PMT SES-params:
integer TOF2DBc::_nsespar=0;
geant TOF2DBc::_sespar[TOF2GC::SCBTPN][TOF2GC::SESPMX]={
  0,0,0,
  0,0,0,
  0,0,0,
  0,0,0,
  0,0,0,
  0,0,0,
  0,0,0,
  0,0,0,
  0,0,0,
  0,0,0,
  0,0,0
};
//---> Initialize TOF MC/RECO "time-stable" parameters :
//
  geant TOF2DBc::_edep2ph=10000.;//(was 8k for old scint)edep(Mev)-to-Photons convertion
  geant TOF2DBc::_adc2q=1.;       // not used (now taken from TFCA #21)
  geant TOF2DBc::_fladctb=0.1;    // MC "flash-ADC" internal time binning (ns)
  geant TOF2DBc::_tdcscl=50331.;  // max TDC-scale(ns)((11msb+10lsb)=21bit*24ps)
  geant TOF2DBc::_strflu=0.36;     //
  geant TOF2DBc::_tdcbin[4]={
    0.0244141,                        // LTtime/FTtime/SumHTtime-TDC binning(ns).
    0.0244141,                         // LTtime/FTtime/SumHTtime-TDC binning(ns).
    0.5,                           // supl.DAQ binning for charge meas.(in ADC-chan units)
    25.                            // Tof TDC coarse counter bin (ns, corresp. to 40Mhz clock)
  };
  geant TOF2DBc::_daqpwd[15]={
    250.,   // (0)PW of "z>=1(FTC)"(HT-branch) output signal(ACTEL-outp going to SPT)[ns] 
    5.,     // (1)minimal inp.pulse width(TovT) to fire the generic discriminator[its rise time, ns]
    250.,   // (2)PW of "z>=2(FTZ)"(SHT-branch) output signal(ACTEL-outp going to SPT)[ns]
    7.,     // (3)minimal output pulse width[ns] of generic discriminator [i.e. outPW=7ns if
//                    5<=inpTovT<=(5+7), and outPW=inpTovT-5 if inpTovT>(5+7)]
    11.,     // (4)input dead time of generic discr(min dist. of prev_down/next_up edges)[ns]
//                    (i neglect by fall-time of generic discr, so DT=(11+5)ns for output signals !!!)
    240.,   // (5)FTgate-width for z>=1 tof-trig-pattern creation(in SPT2)[ns, FT going from JLV1]
    240.,   // (6)FTgate-width for z>=2 tof-trig-pattern creation(in SPT2)[ns, FT going from JLV1]
    20.,    // (7)dead time of TDC-inputs, the same for LT-/FT-/SumHT-inputs[ns]
    10.,    // (8)dead time of "HT/SHT-trig" branch on ACTEL-outp(SPT-inp)[fall-to-rise min.dist, going to SPT, ns]
//                    (Guido: ACTEL-input is faster than Discr, so no ACTEL-inp dead time check) 
    20.,    // (9)dead time of "SumHT(SHT)" branch on ACTEL output going to TDC[ns]
    0.02,   // (10)generic discr. internal accuracy[ns]
    25.,    // (11)fixed PW of "SumHT(SHT)" branch on ACTEL-output[going to TDC-input, ns] 
    0.005,  // (12)TDC differential nonlinearity(rms,ns)(0.21bin) 
    0.017,  // (13)TDC integral nonlinearity(table-corrected,ns)(0.72bin) 
    10.36   // (14)spare 
  };
  geant TOF2DBc::_trigtb=0.5;  // MC time-bin in logic(trig) pulse handling (ns)
  geant TOF2DBc::_tdctrdel=6000.; // TDC Trig(Lev1) supplementary delay in SFET(A) 
  geant TOF2DBc::_tdctrlat=17000.;// TDC trig. latency setting
  geant TOF2DBc::_tdcmatw=16000.;// TDC matching window
//  geant TOF2DBc::_ftc2cj=0.; // FT-signal crate-to-crate jitter(ns)
//  geant TOF2DBc::_fts2sj=0.; // FT-signal slot-to-slot jitter(ns)
  geant TOF2DBc::_ftc2cj=0.03; // FT-signal crate-to-crate jitter(ns)
  geant TOF2DBc::_fts2sj=0.005; // FT-signal slot-to-slot jitter(ns)
  geant TOF2DBc::_lev1del=1000.;// "Lev1" stand.delay (in JLV1) with respect to FT 
  geant TOF2DBc::_ltagew[2]={40,640};//RECO: LTtime wrt FTtime age-window(ns),(FT-LT)
//                                         "+" means LT is befor(earlier) FT in abs.time-scale
//                                Later may be need to set 60/~300 
  geant TOF2DBc::_ftdelm=250.; //
  geant TOF2DBc::_clkperJLV=40.;  // JLVTrig.electronics(JLV1-crate) clock period(ns)
  geant TOF2DBc::_clkperSPT=20.;  // SPTpreTrig.electronics(S-crates) clock period(ns)
  integer TOF2DBc::_pbonup=1;  // set phase-bit for leading(up) edge (yes/no->1/0)
  geant TOF2DBc::_tofareftem[3]={0.,0.,0.};//Tof(Acc) ref.temperatures for SFET(A),PMT,SFEC 
//
//  member functions :
//
// ---> function to read geomconfig-files 
//                  Called from TOFgeom :
  void TOF2DBc::readgconf(){
    int i,j;
    char fname[80];
    char name[80]="TofGeom";
    char vers0[10]="MC";//not used now
    char vers1[10]="PreAss";//pre-assembly in clean room
    char vers2[10]="Space";//in space
    char vers3[10]="Ass1";//in clean room final assembly-1
//    char vers4[10]="PMag";//perm.magnet
    char vers4[10]="PMagCorr";//perm.magnet, new TofHoneycombsPositions(+-66.62)
    geant ZShift(0);
//
    if(strstr(AMSJob::gethead()->getsetup(),"AMS02D")){
      cout <<"<------ TOFGeom-I- AMS02D configuration is used..."<<endl;
      strcat(name,vers2);//as space
      ZShift=AMSDBc::amsdext;
      cout<<"        ZShift="<<ZShift<<endl;
    }
    
    else if(strstr(AMSJob::gethead()->getsetup(),"AMS02")){
      if(strstr(AMSJob::gethead()->getsetup(),"PreAss")){
        cout <<"<------ TOFGeom-I- AMS02-configuration is used..."<<endl;
        cout <<"      PreAssembly(CleanRoom) setup selected..."<<endl;
        strcat(name,vers1);//clean room
      }
      else if(strstr(AMSJob::gethead()->getsetup(),"Space")){
        cout <<"<------ TOFGeom-I- AMS02-configuration is used..."<<endl;
        cout <<"      Space setup selected..."<<endl;
        strcat(name,vers2);//space
      }
      else if(strstr(AMSJob::gethead()->getsetup(),"Ass1")){
        cout <<"<------ TOFGeom-I- AMS02-configuration is used..."<<endl;
        cout <<"      Assembly_1(CleanRoom) setup selected..."<<endl;
        strcat(name,vers3);//clean room final assembly-1
      }
      else if(strstr(AMSJob::gethead()->getsetup(),"AMS02P")){
        cout <<"<------ TOFGeom-I- AMS02P(perm.magnet)-configuration is used..."<<endl;
        strcat(name,vers4);//perm.magnet
      }
      else{
        cout <<"<------ TOFGeom-I- AMS02-configuration is used..."<<endl;
        cout <<"      Assembly_1(as default) setup selected..."<<endl;
        strcat(name,vers3);//default(Ass1)
      }
    }
    else
    {
          cout <<"       Unknown setup !!!"<<endl;
	  exit(10);
    }
    strcat(name,".dat");
    if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
    if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
    strcat(fname,name);
    cout<<"      Open file : "<<fname<<'\n';
    ifstream tcfile(fname,ios::in); // open needed config-file for reading
    if(!tcfile){
      cout <<"      missing TofGeom-file "<<fname<<endl;
      exit(1);
    }
    tcfile >> _planes;// 1 row
    for(i=0;i<_planes;i++) tcfile >> _bperpl[i];//1 row
    int icm=0;
    for(i=0;i<_planes;i++)icm+=_bperpl[i];
    for(int ic=0;ic<icm;ic++) tcfile >> _brtype[ic];//4 rows
    for(i=0;i<_planes;i++) tcfile >> _plrotm[i];// 1 row
    for(i=0;i<TOF2GC::SCBTPN;i++) tcfile >> _brlen[i];// 1 row
    for(i=0;i<TOF2GC::SCBTPN;i++) tcfile >> _lglen[i];// 1 row
    for(int ipar=0;ipar<4;ipar++){// 4 rows
      for(i=0;i<_planes;i++) tcfile >> _outcp[i][ipar];
    }
    for(i=0;i<12;i++) tcfile >> _supstr[i];
    for(i=0;i<20;i++) tcfile >> _plnstr[i];
    tcfile >> _nsespar;//# of SES params
    for(i=0;i<_nsespar;i++){
      for(j=0;j<TOF2GC::SCBTPN;j++)tcfile >> _sespar[j][i];//SES params. vs btyp
    }
    _supstr[0]+=ZShift;//used for AMS02D-setup !!! for normal one ZShift=0 !!!
    _supstr[1]-=ZShift;
    cout<<"<------ TOF2DBc::readgconf: Setup selection is done !"<<endl<<endl;
  }
//---
  integer TOF2DBc::brtype(integer ilay, integer ibar){
#ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(ilay>=0 && ilay < _planes);
        assert(ibar>=0 && ibar < _bperpl[ilay]);
      }
#endif
    int cnum=0;
    for(int i=0;i<ilay;i++)cnum+=_bperpl[i];
    cnum+=ibar;
    return _brtype[cnum];
  }
//
 integer TOF2DBc::npmtps(integer ilay, integer ibar){
#ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(ilay>=0 && ilay < _planes);
        assert(ibar>=0 && ibar < _bperpl[ilay]);
      }
#endif
    int cnum=0;
    for(int i=0;i<ilay;i++)cnum+=_bperpl[i];
    cnum+=ibar;
    if(_brtype[cnum]==1 || _brtype[cnum]==3)return 3;
    else return 2;
  }
//
  integer TOF2DBc::barseqn(integer ilay, integer ibar){
#ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(ilay>=0 && ilay < _planes);
        assert(ibar>=0 && ibar < _bperpl[ilay]);
      }
#endif
    int cnum=0;
    for(int i=0;i<ilay;i++)cnum+=_bperpl[i];
    cnum+=ibar;
    return cnum;
  }
//
  geant TOF2DBc::brlen(integer ilay, integer ibar){
    int cnum=0;
    int btyp;
    for(int i=0;i<ilay;i++)cnum+=_bperpl[i];
    cnum+=ibar;
    btyp=_brtype[cnum];
#ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(btyp>0 && btyp <= TOF2GC::SCBTPN);
      }
#endif
    return _brlen[btyp-1];
  }
//
  geant TOF2DBc::lglen(integer ilay, integer ibar){
    int cnum=0;
    for(int i=0;i<ilay;i++)cnum+=_bperpl[i];
    cnum+=ibar;
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
        assert(ilay>=0 && ilay < _planes);
      }
#endif
    return _plrotm[ilay];
  }
//
  integer TOF2DBc::getbppl(integer ilay){
#ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(ilay>=0 && ilay < _planes);
      }
#endif
    return _bperpl[ilay];
  }
//
  geant TOF2DBc::outcp(int ilay, int ipr){//ipr=1:4->wd/xc/yc/exwd
#ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(ilay>=0 && ilay < _planes);
	assert(ipr>0 && ipr<=4);
      }
#endif
    return _outcp[ilay][ipr-1];
  }
//
  geant TOF2DBc::supstr(int i){
#ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(i>0 && i <= 12);
      }
#endif
    return _supstr[i-1];
  }
//
  geant TOF2DBc::plnstr(int i){
#ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(i>0 && i <= 20);
      }
#endif
    return _plnstr[i-1];
  }
// ===> function to get Z-position of scint. bar=ib in layer=il
//
  geant TOF2DBc::getzsc(integer il, integer ib){
#ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(il>=0 && il < _planes);
        assert(ib>=0 && ib < _bperpl[il]);
      }
#endif
  geant dz,zc;
  dz=_plnstr[5];// counter thickness
  if(il==0)
    zc=_supstr[0]-_plnstr[0]-dz/2.;//mid-plane of closest to topHC counters
  else if(il==1)
    zc=_supstr[0]-_plnstr[1]-dz/2.;//mid-plane of closest to topHC counters
  else if(il==2)
    zc=_supstr[1]+_plnstr[1]+dz/2.;//mid-plane of closest to botHC counters
  else if(il==3)
    zc=_supstr[1]+_plnstr[0]+dz/2.;//mid-plane of closest to botHC counters
//
//z-biases according to official design:
//
//if(!AMSJob::gethead()->isRealData()){//tempor for MC to use old calibration

  if(il==0)zc=zc-((ib+1)%2)*_plnstr[2];//1st counter is far from topHC
  else if(il==1)zc=zc-((ib+1)%2)*_plnstr[2];//1st counter is far from topHc
  else if(il==2)zc=zc+(ib%2)*_plnstr[2];//1st counter is close botHC
  else if(il==3)zc=zc+((ib+1)%2)*_plnstr[2];//1st counter is far from botHC

//}
//
// z-biases inverted as follows from data:
//else{

//  if(il==0)zc=zc-((ib)%2)*_plnstr[2];//1st counter is close to topHC
//  else if(il==1)zc=zc-((ib)%2)*_plnstr[2];//1st counter is close to topHc
//  else if(il==2)zc=zc+((ib+1)%2)*_plnstr[2];//1st counter is far from botHC
//  else if(il==3)zc=zc+((ib)%2)*_plnstr[2];//1st counter is close to botHC
  
//}
//
  return(zc);
  }  
// ===> function to get transv. position of scint. bar=ib in layer=il
//
  geant TOF2DBc::gettsc(integer il, integer ib){
#ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(il>=0 && il < _planes);
        assert(ib>=0 && ib < _bperpl[il]);
      }
#endif
  geant dxi,dxo;
  geant x,co[2],dxti,dxto;
  dxi=_plnstr[4];//sc.paddle width(inner="normal" counters)
  dxo=_outcp[il][0];//sc.paddle width(outer="trapezoidal" counters)
  dxti=(_bperpl[il]-3)*(dxi-_plnstr[3]);//inner 1st/last sc.padd.center transv.dist 
  dxto=dxti+dxo+dxi-2*_plnstr[3];//1st/last sc.padd.center transv.distance
//
  if(il<2){
    co[0]=_supstr[2];// <--top TOF-subsystem X-shift
    co[1]=_supstr[3];// <--top TOF-subsystem Y-shift
  }
  if(il>1){
    co[0]=_supstr[4];// <--bot TOF-subsystem X-shift
    co[1]=_supstr[5];// <--bot TOF-subsystem Y-shift
  }
//
  if(ib==0)x=-0.5*dxto;
  else if(ib==(_bperpl[il]-1))x=0.5*dxto;
  else x=-0.5*dxti+(ib-1)*(dxi-_plnstr[3]);
//
  if(_plrotm[il]==0){  // <-- unrotated planes
    x=co[0]+x;//honeycomb_RS --> MRS(imply sc.padds are at "0" in Hon.RS)
  }
  if(_plrotm[il]==1){  // <-- rotated planes
    x=co[1]+x;
  }
  return(x);
  }
// functions to get MC/RECO parameters:
//
  geant TOF2DBc::edep2ph(){return _edep2ph;}
  geant TOF2DBc::fladctb(){return _fladctb;}
  geant TOF2DBc::tdcmatw(){return _tdcmatw;}
  geant TOF2DBc::tdcscl(){return _tdcscl;}
//
  geant TOF2DBc::tdctrdel(){return _tdctrdel;}
  geant TOF2DBc::tdctrlat(){return _tdctrlat;}
  geant TOF2DBc::lev1del(){return _lev1del;}
//
  geant TOF2DBc::strflu(){return _strflu;}
//
  geant TOF2DBc::ftc2cj(){return _ftc2cj;}
//
  geant TOF2DBc::fts2sj(){return _fts2sj;}
//
  geant TOF2DBc::ltagew(int i){return _ltagew[i];}
  geant TOF2DBc::ftdelm(){return _ftdelm;}
  geant TOF2DBc::clkperJLV(){return _clkperJLV;}
  geant TOF2DBc::clkperSPT(){return _clkperSPT;}
  
  int TOF2DBc::nsespar(){return _nsespar;}
  geant TOF2DBc::sespar(int ibt, int ip){
#ifdef __AMSDEBUG__
    assert(ibt>=0 && ibt< TOF2GC::SCBTPN);
//    assert(ip>0=0 && ip< TOF2GC::SESPMX);
#endif
    return _sespar[ibt][ip];
  }
  
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
  TOF2Brcal::TOF2Brcal(integer sid, integer npm, integer sta[2], geant gna[2], 
           geant gnd[2][TOF2GC::PMTSMX],
           geant a2d[2], geant asl, geant tth, geant stra[2][2], geant fstd,  
           geant t0, geant sl, geant sls[2], geant tdiff, geant td2p[2], geant mip,
           integer nsp, geant ysc[], geant relo[], geant upar[2*TOF2GC::SCPROFP], 
	   geant a2q){
    int i,j;
    softid=sid;
    npmts=npm;
    status[0]=sta[0];
    status[1]=sta[1];
    gaina[0]=gna[0];
    gaina[1]=gna[1];
    for(i=0;i<TOF2GC::PMTSMX;i++){
      gaind[0][i]=gnd[0][i];
      gaind[1][i]=gnd[1][i];
    }
    a2dr[0]=a2d[0];
    a2dr[1]=a2d[1];
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
    nscanp=nsp;
    for(i=0;i<nsp;i++){
      yscanp[i]=ysc[i];
      relout[i]=relo[i];
    }
    for(i=0;i<2*TOF2GC::SCPROFP;i++)unipar[i]=upar[i];
    adc2qf=a2q;
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
 integer i1,i2,sta[2],stat[TOF2GC::SCBLMX][2],npm;
 geant r,eff1,eff2;
 integer sid,brt,endflab(0);
 geant gna[2],qath,qdth,a2dr[2],tth,strat[2][2],ah2l[2];
 geant slope,slpf,fstrd,tzer,tdif,mip2q,speedl,lspeeda[TOF2GC::SCLRS][TOF2GC::SCMXBR];
 geant tzerf[TOF2GC::SCLRS][TOF2GC::SCMXBR],tdiff[TOF2GC::SCBLMX];
 geant slops[2],slops1[TOF2GC::SCLRS][TOF2GC::SCMXBR],slops2[TOF2GC::SCLRS][TOF2GC::SCMXBR];
 geant strf[TOF2GC::SCBLMX][2],strof[TOF2GC::SCBLMX][2];
 geant an2di[TOF2GC::SCBLMX][2],gaina[TOF2GC::SCBLMX][2],m2q[TOF2GC::SCBTPN];
 geant aprofp[TOF2GC::SCBTPN][2*TOF2GC::SCPROFP],apr[2*TOF2GC::SCPROFP],hblen;
 geant a2drf[TOF2GC::SCBLMX][2];
 geant p1s1,p2s1,p3s1,p4s1,p5s1,p6s1,p1s2,p2s2,p3s2,p4s2,p5s2,p6s2,nom,denom; 
 char fname[80];
 char name[80];
 geant a2q,td2p[2];
 int mrfp;
//
 geant asatl=20.;//(mev,~20MIPs),if E-dinode(1-end) higher - use it instead
//                                 of anode measurements
//
 geant gaind[TOF2GC::SCBLMX][2][TOF2GC::PMTSMX];//buff.for dyn.pmts relat.gains
 geant gnd[2][TOF2GC::PMTSMX];
 int ipm; 
//------------------------------
  int ctyp,ntypes;
  char datt[3];
  char ext[80];
  int date[2],year,mon,day,hour,min,sec;
  uinteger iutct;
  tm begin;
  time_t utct;
  uinteger verids[11],verid;
//
  strcpy(name,"TofCflist");// basic name for vers.list-file  
  if(AMSJob::gethead()->isMCData()){
    strcpy(datt,"MC");
    sprintf(ext,"%d",TFMCFFKEY.calvern);//MC-versn
  }
  else{
    strcpy(datt,"RD");
    sprintf(ext,"%d",TFREFFKEY.calutc);//RD-utc
  }
  strcat(name,datt);
  strcat(name,".");
  strcat(name,ext);
//
  if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"====> TOF2Brcal::build: Opening Calib_vers_list-file "<<fname<<'\n';
  ifstream vlfile(fname,ios::in);
  if(!vlfile){
    cout <<"<---- Error: missing vers.list-file !!? "<<fname<<endl;
    exit(1);
  }
  vlfile >> ntypes;// total number of calibr. file types in the list
  for(int i=0;i<ntypes;i++){
    vlfile >> verids[i];//
    CFlistC[i+1]=verids[i]; 
  }
  CFlistC[0]=ntypes;
  if(AMSJob::gethead()->isMCData()){
    vlfile >> date[0];//YYYYMMDD beg.validity of TofCflistMC.ext file
    vlfile >> date[1];//HHMMSS ......................................
    year=date[0]/10000;//2004->
    mon=(date[0]%10000)/100;//1-12
    day=(date[0]%100);//1-31
    hour=date[1]/10000;//0-23
    min=(date[1]%10000)/100;//0-59
    sec=(date[1]%100);//0-59
    begin.tm_isdst=0;
    begin.tm_sec=sec;
    begin.tm_min=min;
    begin.tm_hour=hour;
    begin.tm_mday=day;
    begin.tm_mon=mon-1;
    begin.tm_year=year-1900;
    utct=mktime(& begin);
    iutct=uinteger(utct);
    cout<<"      TofCflistMC-file begin_date: year:month:day = "<<year<<":"<<mon<<":"<<day<<endl;
    cout<<"                                     hour:min:sec = "<<hour<<":"<<min<<":"<<sec<<endl;
    cout<<"                                         UTC-time = "<<iutct<<endl;
    CFlistC[ntypes+1]=TFMCFFKEY.calvern;
    CFlistC[ntypes+2]=date[0];
    CFlistC[ntypes+3]=date[1];
  }
  else{
    utct=time_t(TFREFFKEY.calutc);
    printf("      TofCflistRD-file begin_date: %s",ctime(&utct)); 
    CFlistC[ntypes+1]=TFREFFKEY.calutc;
  }
  vlfile.close();
//------------------------------------------------
//
//   --->  Read tof-channels calib-status file :
//
  if(AMSJob::gethead()->isMCData())ctyp=2;
  else ctyp=1;
  verid=verids[ctyp-1];//MC-versn or RD-utc
  strcpy(name,"TofCStat");//generic name
  strcat(name,datt);
  strcat(name,".");
  sprintf(ext,"%d",verid);
  strcat(name,ext);
   if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
   if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
   strcat(fname,name);
   cout<<"      Opening TofCalibStatus-file : "<<fname<<'\n';
   ifstream stfile(fname,ios::in); // open file for reading
   if(!stfile){
     cout <<"<---- Error: missing TofCalibStatus-file: "<<fname<<endl;
     exit(1);
   }
//------------------------------
//   --->  Read TOF-channels status values:
//
   cnum=0;
   for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
   for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // <-------- loop over bar in layer
     for(int ipr=0;ipr<2;ipr++){
       stfile >> stat[cnum][ipr];//stat(s1),stat(s2)
     }
     cnum+=1; // sequential counter numbering(0-...)
   } // --- end of bar loop --->
   } // --- end of layer loop --->
//
   stfile >> endflab;//read endfile-label
//
   stfile.close();
//
   if(endflab==12345){
     cout<<"      TofCalibStatus-file is successfully read !"<<endl;
   }
   else{
     cout<<"<---- Error: problems with TofCalibStatus-file !!!"<<endl;
     exit(1);
   }
//--------------------------------------------------
//
//   --->  Read SideTimeDifferences/LightSpeed calibration file :
//
 endflab=0;
 if(AMSJob::gethead()->isMCData())ctyp=3;
 else ctyp=2;
 verid=verids[ctyp-1];//MC-versn or RD-utc
 strcpy(name,"TofTdelv");//generic name
 strcat(name,datt);
 strcat(name,".");
 sprintf(ext,"%d",verid);
 strcat(name,ext);
 if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
 if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
 strcat(fname,name);
 cout<<"      Opening TimeDiff/LightVelosity-file : "<<fname<<'\n';
 ifstream tdcfile(fname,ios::in); // open  file for reading
 if(!tdcfile){
   cout <<"<---- Error: missing TimeDiff/LightVelosity-file !!! "<<fname<<endl;
   exit(1);
 }
//
 if(lsflg){// read bar indiv.Lspeed
   for(ila=0;ila<TOF2DBc::getnplns();ila++){   
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  
       tdcfile >> lspeeda[ila][ibr];
     }
   }
 }
 else tdcfile >> speedl;// read average Lspeed
//
 cnum=0;
 for(ila=0;ila<TOF2DBc::getnplns();ila++){
   for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  
     tdcfile >> tdiff[cnum];
     cnum+=1; // sequential counters numbering(0-...)
   }
 }
//
   tdcfile >> endflab;//read endfile-label
//
   tdcfile.close();
//
   if(endflab==12345){
     cout<<"      TOF TimeDiff/LightVelosity-file is successfully read !"<<endl;
   }
   else{cout<<"<---- Error: problems with TOF TimeDiff/LightVelosity-file !!!"<<endl;
     exit(1);
   }
//-----------------------------------------------------
//
//   --->  Read Tzeros/SlewingCorrections calibration file :
//
 endflab=0;
 if(AMSJob::gethead()->isMCData())ctyp=4;
 else ctyp=3;
 verid=verids[ctyp-1];//MC-versn or RD-utc
 strcpy(name,"TofTzslw");//generic name
 strcat(name,datt);
 strcat(name,".");
 sprintf(ext,"%d",verid);
 strcat(name,ext);
 if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
 if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
 strcat(fname,name);
 cout<<"      Opening TOF T0/SlevCorr-file : "<<fname<<'\n';
 ifstream tzcfile(fname,ios::in); // open  file for reading
 if(!tzcfile){
   cout <<"<---- Error: missing TOF T0/SlevCorr-file !!! "<<fname<<endl;
   exit(1);
 }
//
 tzcfile >> slpf;
 for(ila=0;ila<TOF2DBc::getnplns();ila++){ 
   for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){
     tzcfile >> tzerf[ila][ibr];
   } 
 }
//
   tzcfile >> endflab;//read endfile-label
//
   tzcfile.close();
//
   if(endflab==12345){
     cout<<"      TOF Slewing/T0-params file is successfully read !"<<endl;
   }
   else{
     cout<<"<---- Error: problems with TOF Slewing/T0-params file !!!"<<endl;
     exit(1);
   }
//-------------------------------------------------------
//
//   ---> Read anodes/dynode relat.gains, anode/dynode ratios,
//       mip2q and A-profile param. calib.file :
//
  endflab=0;
  if(AMSJob::gethead()->isMCData())ctyp=5;
   else ctyp=4;
   verid=verids[ctyp-1];//MC-versn or RD-utc
   strcpy(name,"TofAmplf");//generic name
   strcat(name,datt);
   strcat(name,".");
   sprintf(ext,"%d",verid);
   strcat(name,ext);
   if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
   if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
   strcat(fname,name);
   cout<<"      Opening TOF AmplificationParameters-file : "<<fname<<'\n';
   ifstream gcfile(fname,ios::in); // open file for reading
   if(!gcfile){
     cout <<"<---- Error: missing TOF AmplificationParameters-file !!! "<<fname<<endl;
     exit(1);
   }
//
// ----------------> read anode relative(wrt ref.counter) gains:
//
   cnum=0;
   for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // read side-1
       gcfile >> gaina[cnum+ibr][0];
     }
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // read side-2
       gcfile >> gaina[cnum+ibr][1];
     }
     cnum+=TOF2DBc::getbppl(ila);
   } // --- end of layer loop --->
//
// ----------------> read anode/dynode ratios:
//
   cnum=0;
   for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // read side-1
       gcfile >> a2drf[cnum+ibr][0];
     }
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // read side-2
       gcfile >> a2drf[cnum+ibr][1];
     }
     cnum+=TOF2DBc::getbppl(ila);
   } // --- end of layer loop --->
//
// ----------------> read dynode-pmts gains(relat to side average):
//
   cnum=0;
   for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- layers-loop
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  //s1 padles-loop
       for(ipm=0;ipm<TOF2GC::PMTSMX;ipm++)gcfile >> gaind[cnum+ibr][0][ipm]; //pm-loop
     }
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  //s2 padles-loop
       for(ipm=0;ipm<TOF2GC::PMTSMX;ipm++)gcfile >> gaind[cnum+ibr][1][ipm]; //pm-loop
     }
     cnum+=TOF2DBc::getbppl(ila);
   } // --- end of layer loop --->
//
// ----------------> read mip2q's:
//
   for(ibt=0;ibt<TOF2GC::SCBTPN;ibt++){  // <-------- loop over bar-types
     gcfile >> m2q[ibt];
   }
//
// ----------------> read A-prof. parameters:
//
   for(ibt=0;ibt<TOF2GC::SCBTPN;ibt++){  // <-------- loop over bar-types
     for(i=0;i<2*TOF2GC::SCPROFP;i++)gcfile >> aprofp[ibt][i];
   }
// ---------------->
   gcfile >> endflab;//read endfile-label
//
   gcfile.close();
//
   if(endflab==12345){
     cout<<"      TOF AmplificationParameters-file is successfully read !"<<endl;
   }
   else{
     cout<<"<---- Error: problems with TOF AmplificationParameters-file !!!"<<endl;
     exit(1);
   }
//   
//---------------------------------------------
//   ===> fill TOFBrcal bank :
//
  a2q=TFCAFFKEY.adc2q;//from DC !(if variation are high - need special indiv.a2q's file)
  cnum=0;
  for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
  for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // <-------- loop over bar in layer
    brt=TOF2DBc::brtype(ila,ibr);//1->11
    hblen=0.5*TOF2DBc::brlen(ila,ibr);
    nsp=TOFWScan::scmcscan[brt-1].getnscp(0);//get scan-points number for wdiv=1(most long)
    npm=TOFWScan::scmcscan[brt-1].getnpmts();//get pmts/side 
    TOFWScan::scmcscan[brt-1].getscp(0,scp);//get scan-points for wdiv=1(most long)
// read from file or DB:
    gna[0]=gaina[cnum][0];
    gna[1]=gaina[cnum][1];
    tth=TOF2Varp::tofvpar.daqthr(0); // (mV), time-discr. threshold
    mip2q=m2q[brt-1];//(pC/mev),dE(mev)_at_counter_center->Q(pC)_at_PM_anode(2x3-sum)
    fstrd=0;//(ns),same hit(up-edge)delay in f/sTDC(don't need now !!!)
//
//-->prepare position correction array (valid for local !!! r.s.):
//
      for(i=0;i<2*TOF2GC::SCPROFP;i++)apr[i]=aprofp[brt-1][i];
      p1s1=aprofp[brt-1][0];
      p2s1=aprofp[brt-1][1];
      p3s1=aprofp[brt-1][2];
      p4s1=aprofp[brt-1][3];
      p5s1=aprofp[brt-1][4];
      p6s1=aprofp[brt-1][5];
      p1s2=aprofp[brt-1][6];
      p2s2=aprofp[brt-1][7];
      p3s2=aprofp[brt-1][8];
      p4s2=aprofp[brt-1][9];
      p5s2=aprofp[brt-1][10];
      p6s2=aprofp[brt-1][11];
      mrfp=nsp/2;//central point (coo=0.)
      if(brt==1 || brt==3 || brt==5 || brt==8){//trapez.counters
        denom=p1s1*((1-p3s1)*exp(-(hblen+scp[mrfp])/p2s1)+p3s1*exp(-(hblen+scp[mrfp])/p4s1))
	     +p5s1*exp(-(hblen-scp[mrfp])/p6s1)
             +p1s2*((1-p3s2)*exp(-(hblen-scp[mrfp])/p2s2)+p3s2*exp(-(hblen-scp[mrfp])/p4s2))
	     +p5s2*exp(-(hblen+scp[mrfp])/p6s2);//s1+s2 signal at center(long.coo=0)
        for(isp=0;isp<nsp;isp++){ // fill 2-ends rel. l.output at scan-points
          nom=p1s1*((1-p3s1)*exp(-(hblen+scp[isp])/p2s1)+p3s1*exp(-(hblen+scp[isp])/p4s1))
	     +p5s1*exp(-(hblen-scp[isp])/p6s1)
             +p1s2*((1-p3s2)*exp(-(hblen-scp[isp])/p2s2)+p3s2*exp(-(hblen-scp[isp])/p4s2))
	     +p5s2*exp(-(hblen+scp[isp])/p6s2);//s1+s2 signal at long.coo=scp[isp] 
          rlo[isp]=nom/denom;
        }
      }
      else{//normal counters
        denom=p1s1*((1-p3s1)*exp(-(hblen+scp[mrfp])/p2s1)+p3s1*exp(-(hblen+scp[mrfp])/p4s1))
             +p1s2*((1-p3s2)*exp(-(hblen-scp[mrfp])/p2s2)+p3s2*exp(-(hblen-scp[mrfp])/p4s2));
        for(isp=0;isp<nsp;isp++){ // fill 2-ends rel. l.output at scan-points
          nom=p1s1*((1-p3s1)*exp(-(hblen+scp[isp])/p2s1)+p3s1*exp(-(hblen+scp[isp])/p4s1))
             +p1s2*((1-p3s2)*exp(-(hblen-scp[isp])/p2s2)+p3s2*exp(-(hblen-scp[isp])/p4s2));
          rlo[isp]=nom/denom;
        }
      }
//
    sid=100*(ila+1)+(ibr+1);
//    strat[0][0]=strf[cnum][0];//stretcher param. from ext.file
//    strat[1][0]=strf[cnum][1];
    strat[0][0]=25;//tempor: put dummy pars(not used now, keep for possible future appl)
    strat[1][0]=25;
//    strat[0][1]=strof[cnum][0];
//    strat[1][1]=strof[cnum][1];
    strat[0][1]=1100;
    strat[1][1]=1100;
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
    a2dr[0]=a2drf[cnum][0];//an/dyn ratios from ext.file
    a2dr[1]=a2drf[cnum][1];
    for(ipm=0;ipm<TOF2GC::PMTSMX;ipm++){
      gnd[0][ipm]=gaind[cnum][0][ipm];// dyn-pms rel.gains from ext.file
      gnd[1][ipm]=gaind[cnum][1][ipm];// dyn-pms rel.gains from ext.file
    }
    scbrcal[ila][ibr]=TOF2Brcal(sid,npm,sta,gna,gnd,a2dr,asatl,tth,strat,fstrd,tzer,
                      slope,slops,tdif,td2p,mip2q,nsp,scp,rlo,apr,a2q);
//
    cnum+=1;// solid sequential numbering of all counter(0->33)
  } // --- end of bar loop --->
  } // --- end of layer loop --->
//
  cout<<"<---- TOF2Brcal::build: succsessfully done !"<<endl<<endl;
}
//-----------------------------------------------------------------------
//
void TOF2Brcal::q2a2q(int cof, int sdf, int hlf, number &adc, number &q){  
// Q(pC) <-> ADC(ch,float) to use in sim./rec. programs (cof=0/1-> Q->ADC/ADC->Q)
//                                                      (sdf=0/1-> bar side 1/2 )
//                                                       hlf=0/1-> for A/D)
//
// WARNING : for dynode Side-signal(adc or q) is the SUM of PMTs Gain-equilized signals
// 
  if(cof==0){ // <=== Q->ADCch
    if(hlf==0){//      <-- for anode
      adc=q/adc2qf;
    }
    else{// <-- for Dynode
      adc=q/adc2qf/a2dr[sdf];//q->Aadc->Dadc(equilized sum)
    }
  }
  else{       // <=== ADCch->Q
    if(hlf==0){//      <-- for anode
      q=adc*adc2qf;
    }
    else{// <-- for Dynode
      q=adc*a2dr[sdf]*adc2qf;//Dadc(equil.sum)->Aadc(hi)->q
    }
  }
}
//------
geant TOF2Brcal::adc2mip(int hlf, number amf[2]){ //A/D side1+2 ADC(ch) -> Etot(Mev)
// for dynodes amf[2] are side equilized sums !!!
  number q(0),qt(0);
  for(int isd=0;isd<2;isd++){
    q2a2q(1,isd,hlf,amf[isd],q);//ADC->Qa
    qt+=(q/gaina[isd]);// Qa->Qa_gain_corrected
  }
  qt=qt/mip2q; // Qa(pC)->Mev
  return geant(qt);
}
//------
void TOF2Brcal::adc2q(int hlf, number amf[2], number qs[2]){// side ADC(ch) -> Q(pC)
//                                      to use in calibr. program only for anodes
  for(int isd=0;isd<2;isd++){
    qs[isd]=0.;
    q2a2q(1,isd,hlf,amf[isd],qs[isd]);//ADC->Q
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
// for dynodes amf[2] are side equilized sums !!!
  number time,qs,uv(0);
    for(int isd=0;isd<2;isd++){
      q2a2q(1,isd,hlf,amf[isd],qs);// ADCch->Q
      uv+=slops[isd]/sqrt(qs);// summing slops/sqrt(Q), works sl.better
    }
  time=0.5*(tmf[0]+tmf[1])+tzero+slope*uv;
  return geant(time); 
}
//-----
geant TOF2Brcal::tm2tr(number tmf[2]){//tempor, same for raw times(lv3-games)
  number time(0);
  time=0.5*(tmf[0]+tmf[1])+tzero;
  return geant(time); 
}
//-----
void TOF2Brcal::tmd2p(number tmf[2], number amf[2], int hlf,
                              geant &co, geant &eco){//(time-diff)->loc.coord/err(cm)
// for dynodes amf[2] are side equilized sums !!!
  number coo,qs,uv(0);
  for(int isd=0;isd<2;isd++){
    q2a2q(1,isd,hlf,amf[isd],qs);// ADCch->Q
    uv+=(1-2*isd)*slops[isd]/sqrt(qs);// subtr slops/sqrt(Q)
  }
  coo=-(0.5*(tmf[0]-tmf[1])+slope*uv-yctdif);
  if(tmf[0]==tmf[1])coo=0;  // vc 15.04.08
//common "-" is due to the fact that Tmeas=Ttrig-Tabs and coo-loc is prop. to Tabs1-Tabs2
  co=td2pos[0]*geant(coo);//coo(ns)->cm                    
  eco=td2pos[1];
}
//-----
void TOF2Brcal::td2ctd(number tdo, number amf[2], int hlf,
                              number &tdc){//td0=0.5*(t[0]-t[1])->tdc (A-corrected)
  number qs,uv(0);
  for(int isd=0;isd<2;isd++){
    q2a2q(1,isd,hlf,amf[isd],qs);// ADCch->Q
    uv+=(1-2*isd)*slops[isd]/sqrt(qs);// subtr slops/sqrt(Q)
  }
  tdc=tdo+slope*uv;
}
//--------------------------
int TOF2Brcal::setpars(integer cfvers){// set RD scbrcal-objects according to CFversion
//used for "on flight" DB update after calib.job finished
//
 int lsflg(1);//0/1->use common/individual values for Lspeed 
 integer i,j,k,ila,ibr,ip,ibrm,isd,isp,nsp,ibt,cnum,dnum,mult;
 geant scp[TOF2GC::SCANPNT];
 geant rlo[TOF2GC::SCANPNT];// relat.(to Y=0) light output
 integer lps=1000;
 geant ef1[TOF2GC::SCANPNT],ef2[TOF2GC::SCANPNT];
 integer i1,i2,sta[2],stat[TOF2GC::SCBLMX][2],npm;
 geant r,eff1,eff2;
 integer sid,brt,endflab(0);
 geant gna[2],qath,qdth,a2dr[2],tth,strat[2][2],ah2l[2];
 geant slope,slpf,fstrd,tzer,tdif,mip2q,speedl,lspeeda[TOF2GC::SCLRS][TOF2GC::SCMXBR];
 geant tzerf[TOF2GC::SCLRS][TOF2GC::SCMXBR],tdiff[TOF2GC::SCBLMX];
 geant slops[2],slops1[TOF2GC::SCLRS][TOF2GC::SCMXBR],slops2[TOF2GC::SCLRS][TOF2GC::SCMXBR];
 geant strf[TOF2GC::SCBLMX][2],strof[TOF2GC::SCBLMX][2];
 geant an2di[TOF2GC::SCBLMX][2],gaina[TOF2GC::SCBLMX][2],m2q[TOF2GC::SCBTPN];
 geant aprofp[TOF2GC::SCBTPN][2*TOF2GC::SCPROFP],apr[2*TOF2GC::SCPROFP],hblen;
 geant a2drf[TOF2GC::SCBLMX][2];
 geant p1s1,p2s1,p3s1,p4s1,p5s1,p6s1,p1s2,p2s2,p3s2,p4s2,p5s2,p6s2,nom,denom; 
 char fname[80];
 char name[80];
 geant a2q,td2p[2];
 int mrfp;
//
 geant asatl=20.;//(mev,~20MIPs),if E-dinode(1-end) higher - use it instead
//                                 of anode measurements
//
 geant gaind[TOF2GC::SCBLMX][2][TOF2GC::PMTSMX];//buff.for dyn.pmts relat.gains
 geant gnd[2][TOF2GC::PMTSMX];
 int ipm; 
//------------------------------
  int ctyp,ntypes;
  char datt[3];
  char ext[80];
  int date[2],year,mon,day,hour,min,sec;
  uinteger iutct;
  tm begin;
  time_t utct;
  uinteger verids[11],verid;
//
  strcpy(name,"TofCflist");// basic name for vers.list-file  
  strcpy(datt,"RD");
  sprintf(ext,"%d",cfvers);//CFvers-file
  strcat(name,datt);
  strcat(name,".");
  strcat(name,ext);
//
  strcpy(fname,"");
  strcat(fname,name);
  cout<<"====> TOF2Brcal::setpars: Opening Calib_vers_list-file "<<fname<<'\n';
  ifstream vlfile(fname,ios::in);
  if(!vlfile){
    cout <<"<---- Error: missing vers.list-file !!? "<<fname<<endl;
    exit(1);
  }
  vlfile >> ntypes;// total number of calibr. file types in the list
  for(int i=0;i<ntypes;i++){
    vlfile >> verids[i];//
  }
  utct=time_t(cfvers);
  printf("      TofCflistRD-file begin_date: %s",ctime(&utct)); 
  vlfile.close();
//------------------------------------------------
//
//   --->  Read tof-channels calib-status file :
//
  ctyp=1;
  verid=verids[ctyp-1];//MC-versn or RD-utc
  strcpy(name,"TofCStat");//generic name
  strcat(name,datt);
  strcat(name,".");
  sprintf(ext,"%d",verid);
  strcat(name,ext);
  strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Opening TofCalibStatus-file : "<<fname<<'\n';
  ifstream stfile(fname,ios::in); // open file for reading
  if(!stfile){
    cout <<"<---- Error: missing TofCalibStatus-file: "<<fname<<endl;
    return 1;
  }
//------------------------------
//   --->  Read TOF-channels status values:
//
   cnum=0;
   for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
   for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // <-------- loop over bar in layer
     for(int ipr=0;ipr<2;ipr++){
       stfile >> stat[cnum][ipr];//stat(s1),stat(s2)
     }
     cnum+=1; // sequential counter numbering(0-...)
   } // --- end of bar loop --->
   } // --- end of layer loop --->
//
   stfile >> endflab;//read endfile-label
//
   stfile.close();
//
   if(endflab==12345){
     cout<<"      TofCalibStatus-file is successfully read !"<<endl;
   }
   else{
     cout<<"<---- Error: problems with TofCalibStatus-file !!!"<<endl;
     return 2;
   }
//--------------------------------------------------
//
//   --->  Read SideTimeDifferences/LightSpeed calibration file :
//
 endflab=0;
 ctyp=2;
 verid=verids[ctyp-1];//MC-versn or RD-utc
 strcpy(name,"TofTdelv");//generic name
 strcat(name,datt);
 strcat(name,".");
 sprintf(ext,"%d",verid);
 strcat(name,ext);
 strcpy(fname,"");
 strcat(fname,name);
 cout<<"      Opening TimeDiff/LightVelosity-file : "<<fname<<'\n';
 ifstream tdcfile(fname,ios::in); // open  file for reading
 if(!tdcfile){
   cout <<"<---- Error: missing TimeDiff/LightVelosity-file !!! "<<fname<<endl;
   return 1;
 }
//
 if(lsflg){// read bar indiv.Lspeed
   for(ila=0;ila<TOF2DBc::getnplns();ila++){   
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  
       tdcfile >> lspeeda[ila][ibr];
     }
   }
 }
 else tdcfile >> speedl;// read average Lspeed
//
 cnum=0;
 for(ila=0;ila<TOF2DBc::getnplns();ila++){
   for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  
     tdcfile >> tdiff[cnum];
     cnum+=1; // sequential counters numbering(0-...)
   }
 }
//
   tdcfile >> endflab;//read endfile-label
//
   tdcfile.close();
//
   if(endflab==12345){
     cout<<"      TOF TimeDiff/LightVelosity-file is successfully read !"<<endl;
   }
   else{cout<<"<---- Error: problems with TOF TimeDiff/LightVelosity-file !!!"<<endl;
     return 2;
   }
//-----------------------------------------------------
//
//   --->  Read Tzeros/SlewingCorrections calibration file :
//
 endflab=0;
 ctyp=3;
 verid=verids[ctyp-1];//MC-versn or RD-utc
 strcpy(name,"TofTzslw");//generic name
 strcat(name,datt);
 strcat(name,".");
 sprintf(ext,"%d",verid);
 strcat(name,ext);
 strcpy(fname,"");
 strcat(fname,name);
 cout<<"      Opening TOF T0/SlevCorr-file : "<<fname<<'\n';
 ifstream tzcfile(fname,ios::in); // open  file for reading
 if(!tzcfile){
   cout <<"<---- Error: missing TOF T0/SlevCorr-file !!! "<<fname<<endl;
   return 1;
 }
//
 tzcfile >> slpf;
 for(ila=0;ila<TOF2DBc::getnplns();ila++){ 
   for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){
     tzcfile >> tzerf[ila][ibr];
   } 
 }
//
   tzcfile >> endflab;//read endfile-label
//
   tzcfile.close();
//
   if(endflab==12345){
     cout<<"      TOF Slewing/T0-params file is successfully read !"<<endl;
   }
   else{
     cout<<"<---- Error: problems with TOF Slewing/T0-params file !!!"<<endl;
     return 2;
   }
//-------------------------------------------------------
//
//   ---> Read anodes/dynode relat.gains, anode/dynode ratios,
//       mip2q and A-profile param. calib.file :
//
  endflab=0;
   ctyp=4;
   verid=verids[ctyp-1];//MC-versn or RD-utc
   strcpy(name,"TofAmplf");//generic name
   strcat(name,datt);
   strcat(name,".");
   sprintf(ext,"%d",verid);
   strcat(name,ext);
   strcpy(fname,"");
   strcat(fname,name);
   cout<<"      Opening TOF AmplificationParameters-file : "<<fname<<'\n';
   ifstream gcfile(fname,ios::in); // open file for reading
   if(!gcfile){
     cout <<"<---- Error: missing TOF AmplificationParameters-file !!! "<<fname<<endl;
     return 1;
   }
//
// ----------------> read anode relative(wrt ref.counter) gains:
//
   cnum=0;
   for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // read side-1
       gcfile >> gaina[cnum+ibr][0];
     }
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // read side-2
       gcfile >> gaina[cnum+ibr][1];
     }
     cnum+=TOF2DBc::getbppl(ila);
   } // --- end of layer loop --->
//
// ----------------> read anode/dynode ratios:
//
   cnum=0;
   for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // read side-1
       gcfile >> a2drf[cnum+ibr][0];
     }
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // read side-2
       gcfile >> a2drf[cnum+ibr][1];
     }
     cnum+=TOF2DBc::getbppl(ila);
   } // --- end of layer loop --->
//
// ----------------> read dynode-pmts gains(relat to side average):
//
   cnum=0;
   for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- layers-loop
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  //s1 padles-loop
       for(ipm=0;ipm<TOF2GC::PMTSMX;ipm++)gcfile >> gaind[cnum+ibr][0][ipm]; //pm-loop
     }
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  //s2 padles-loop
       for(ipm=0;ipm<TOF2GC::PMTSMX;ipm++)gcfile >> gaind[cnum+ibr][1][ipm]; //pm-loop
     }
     cnum+=TOF2DBc::getbppl(ila);
   } // --- end of layer loop --->
//
// ----------------> read mip2q's:
//
   for(ibt=0;ibt<TOF2GC::SCBTPN;ibt++){  // <-------- loop over bar-types
     gcfile >> m2q[ibt];
   }
//
// ----------------> read A-prof. parameters:
//
   for(ibt=0;ibt<TOF2GC::SCBTPN;ibt++){  // <-------- loop over bar-types
     for(i=0;i<2*TOF2GC::SCPROFP;i++)gcfile >> aprofp[ibt][i];
   }
// ---------------->
   gcfile >> endflab;//read endfile-label
//
   gcfile.close();
//
   if(endflab==12345){
     cout<<"      TOF AmplificationParameters-file is successfully read !"<<endl;
   }
   else{
     cout<<"<---- Error: problems with TOF AmplificationParameters-file !!!"<<endl;
     return 2;
   }
//   
//---------------------------------------------
//   ===> fill TOFBrcal objects :
//
  a2q=TFCAFFKEY.adc2q;//from DC !(if variation are high - need special indiv.a2q's file)
  cnum=0;
  for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
  for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // <-------- loop over bar in layer
    brt=TOF2DBc::brtype(ila,ibr);//1->11
    hblen=0.5*TOF2DBc::brlen(ila,ibr);
    nsp=TOFWScan::scmcscan[brt-1].getnscp(0);//get scan-points number for wdiv=1(most long)
    npm=TOFWScan::scmcscan[brt-1].getnpmts();//get pmts/side 
    TOFWScan::scmcscan[brt-1].getscp(0,scp);//get scan-points for wdiv=1(most long)
// read from file or DB:
    gna[0]=gaina[cnum][0];
    gna[1]=gaina[cnum][1];
    tth=TOF2Varp::tofvpar.daqthr(0); // (mV), time-discr. threshold
    mip2q=m2q[brt-1];//(pC/mev),dE(mev)_at_counter_center->Q(pC)_at_PM_anode(2x3-sum)
    fstrd=0;//(ns),same hit(up-edge)delay in f/sTDC(don't need now !!!)
//
//-->prepare position correction array (valid for local !!! r.s.):
//
      for(i=0;i<2*TOF2GC::SCPROFP;i++)apr[i]=aprofp[brt-1][i];
      p1s1=aprofp[brt-1][0];
      p2s1=aprofp[brt-1][1];
      p3s1=aprofp[brt-1][2];
      p4s1=aprofp[brt-1][3];
      p5s1=aprofp[brt-1][4];
      p6s1=aprofp[brt-1][5];
      p1s2=aprofp[brt-1][6];
      p2s2=aprofp[brt-1][7];
      p3s2=aprofp[brt-1][8];
      p4s2=aprofp[brt-1][9];
      p5s2=aprofp[brt-1][10];
      p6s2=aprofp[brt-1][11];
      mrfp=nsp/2;//central point (coo=0.)
      if(brt==1 || brt==3 || brt==5 || brt==8){//trapez.counters
        denom=p1s1*((1-p3s1)*exp(-(hblen+scp[mrfp])/p2s1)+p3s1*exp(-(hblen+scp[mrfp])/p4s1))
	     +p5s1*exp(-(hblen-scp[mrfp])/p6s1)
             +p1s2*((1-p3s2)*exp(-(hblen-scp[mrfp])/p2s2)+p3s2*exp(-(hblen-scp[mrfp])/p4s2))
	     +p5s2*exp(-(hblen+scp[mrfp])/p6s2);//s1+s2 signal at center(long.coo=0)
        for(isp=0;isp<nsp;isp++){ // fill 2-ends rel. l.output at scan-points
          nom=p1s1*((1-p3s1)*exp(-(hblen+scp[isp])/p2s1)+p3s1*exp(-(hblen+scp[isp])/p4s1))
	     +p5s1*exp(-(hblen-scp[isp])/p6s1)
             +p1s2*((1-p3s2)*exp(-(hblen-scp[isp])/p2s2)+p3s2*exp(-(hblen-scp[isp])/p4s2))
	     +p5s2*exp(-(hblen+scp[isp])/p6s2);//s1+s2 signal at long.coo=scp[isp] 
          rlo[isp]=nom/denom;
        }
      }
      else{//normal counters
        denom=p1s1*((1-p3s1)*exp(-(hblen+scp[mrfp])/p2s1)+p3s1*exp(-(hblen+scp[mrfp])/p4s1))
             +p1s2*((1-p3s2)*exp(-(hblen-scp[mrfp])/p2s2)+p3s2*exp(-(hblen-scp[mrfp])/p4s2));
        for(isp=0;isp<nsp;isp++){ // fill 2-ends rel. l.output at scan-points
          nom=p1s1*((1-p3s1)*exp(-(hblen+scp[isp])/p2s1)+p3s1*exp(-(hblen+scp[isp])/p4s1))
             +p1s2*((1-p3s2)*exp(-(hblen-scp[isp])/p2s2)+p3s2*exp(-(hblen-scp[isp])/p4s2));
          rlo[isp]=nom/denom;
        }
      }
//
    sid=100*(ila+1)+(ibr+1);
//    strat[0][0]=strf[cnum][0];//stretcher param. from ext.file
//    strat[1][0]=strf[cnum][1];
    strat[0][0]=25;//tempor: put dummy pars(not used now, keep for possible future appl)
    strat[1][0]=25;
//    strat[0][1]=strof[cnum][0];
//    strat[1][1]=strof[cnum][1];
    strat[0][1]=1100;
    strat[1][1]=1100;
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
    a2dr[0]=a2drf[cnum][0];//an/dyn ratios from ext.file
    a2dr[1]=a2drf[cnum][1];
    for(ipm=0;ipm<TOF2GC::PMTSMX;ipm++){
      gnd[0][ipm]=gaind[cnum][0][ipm];// dyn-pms rel.gains from ext.file
      gnd[1][ipm]=gaind[cnum][1][ipm];// dyn-pms rel.gains from ext.file
    }
    scbrcal[ila][ibr]=TOF2Brcal(sid,npm,sta,gna,gnd,a2dr,asatl,tth,strat,fstrd,tzer,
                      slope,slops,tdif,td2p,mip2q,nsp,scp,rlo,apr,a2q);
//
    cnum+=1;// solid sequential numbering of all counter(0->33)
  } // --- end of bar loop --->
  } // --- end of layer loop --->
//
  cout<<"<---- TOF2Brcal::setpars: succsessfully done !"<<endl<<endl;
  return 0;
}
//===============================================================================
  TOFBrcalMS::TOFBrcalMS(integer sid, integer sta[2], geant gna[2], 
           geant gnd[2][TOF2GC::PMTSMX], geant a2d[2], geant stra[2][2],
           geant a2q){
    int i,j;
    softid=sid;
    status[0]=sta[0];
    status[1]=sta[1];
    gaina[0]=gna[0];
    gaina[1]=gna[1];
    for(i=0;i<TOF2GC::PMTSMX;i++){
      gaind[0][i]=gnd[0][i];
      gaind[1][i]=gnd[1][i];
    }
    a2dr[0]=a2d[0];
    a2dr[1]=a2d[1];
    strat[0][0]=stra[0][0];
    strat[0][1]=stra[0][1];
    strat[1][0]=stra[1][0];
    strat[1][1]=stra[1][1];
    adc2qf=a2q;
  }
//  ================= TOFBrcalMS class("MC Seeds") functions ======================= :
//
void TOFBrcalMS::build(){// create MC-seed scbrcal-objects for each sc.bar
//
 integer i,j,k,ila,ibr,ip,ibrm,isd,isp,nsp,ibt,cnum,dnum,mult;
 integer lps=1000;
 integer i1,i2,sta[2],stat[TOF2GC::SCBLMX][2],npm;
 geant r,eff1,eff2;
 integer sid,brt,endflab;
 geant gna[2],a2dr[2],strat[2][2],ah2l[2];
 geant strf[TOF2GC::SCBLMX][2],strof[TOF2GC::SCBLMX][2];
 geant an2di[TOF2GC::SCBLMX][2],gaina[TOF2GC::SCBLMX][2],m2q[TOF2GC::SCBTPN];
 geant aprofp[TOF2GC::SCBTPN][2*TOF2GC::SCPROFP],hblen;
 geant a2drf[TOF2GC::SCBLMX][2];
//
 geant gaind[TOF2GC::SCBLMX][2][TOF2GC::PMTSMX];//buff.for dyn.pmts relat.gains
 geant gnd[2][TOF2GC::PMTSMX];
 int ipm; 
 char fname[80];
 char name[80];
 char ext[80];
 geant a2q,td2p[2];
 char vers1[3]="MC";
 char vers2[3]="SD";
 int mrfp;
//------------------------------
  char in[2]="0";
  char inum[11];
  int ctyp,ntypes,mcvern[10],rlvern[10];
  int mcvn,rlvn,dig;
  int date[2],year,mon,day,hour,min,sec;
  uinteger iutct;
  tm begin;
  time_t utct;
//
  strcpy(inum,"0123456789");
  endflab=0;
//
// ---> read file with the list of version numbers for all needed MC-seed barcal.files :
//
  strcpy(name,"TofCflistMC.");// basic name for vers.list-file  
  sprintf(ext,"%d",TFMCFFKEY.calvern);//got TofCflistMC. file extention
  strcat(name,ext);
//
  if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"====> TOFBrcalMS::build: Opening TofCflistMC-file...  "<<fname<<'\n';
  ifstream vlfile(fname,ios::in);
  if(!vlfile){
    cout <<"<---- missing TofCflistMC-file !!! "<<fname<<endl;
    exit(1);
  }
  vlfile >> ntypes;// total number of calibr. file types in the list
  for(i=0;i<ntypes;i++){
    vlfile >> mcvern[i];// vers# of MC-calib files 
  }
  vlfile >> date[0];//YYYYMMDD beg.validity of TofCflistMC.ext file
  vlfile >> date[1];//HHMMSS ......................................
  vlfile.close();
//
  year=date[0]/10000;//2004->
  mon=(date[0]%10000)/100;//1-12
  day=(date[0]%100);//1-31
  hour=date[1]/10000;//0-23
  min=(date[1]%10000)/100;//0-59
  sec=(date[1]%100);//0-59
  begin.tm_isdst=0;
  begin.tm_sec=sec;
  begin.tm_min=min;
  begin.tm_hour=hour;
  begin.tm_mday=day;
  begin.tm_mon=mon-1;
  begin.tm_year=year-1900;
  utct=mktime(& begin);
  iutct=uinteger(utct);
  cout<<"      TofCflistMC-file begin_date: year:month:day = "<<year<<":"<<mon<<":"<<day<<endl;
  cout<<"                                     hour:min:sec = "<<hour<<":"<<min<<":"<<sec<<endl;
  cout<<"                                         UTC-time = "<<iutct<<endl;
//------------------------------------------------
//
//   ---> Prepare to read tof-chan MCSeed calib-status file(used as "MC Seed") :
//
  ctyp=2;//line# corresponding calib-status parameters file
  strcpy(name,"TofCStat");
  mcvn=mcvern[ctyp-1];
  if(TFMCFFKEY.mcseedo==0)strcat(name,vers1);//mc
  else strcat(name,vers2);//sd = copy of rl
  strcat(name,".");
//
  sprintf(ext,"%d",mcvn);//got TofCStatMC. file extention
  strcat(name,ext);
  if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Opening MC_Seed Calib_status file: "<<fname<<'\n';
  ifstream stfile(fname,ios::in); // open file for reading
  if(!stfile){
    cout <<"<---- missing MC_Seed_Calib-status file !!! "<<fname<<endl;
    exit(1);
  }
//------------------------------
//   --->  Read MCSeed TOF-channels status values:
//
  endflab=0;
   cnum=0;
   for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
   for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // <-------- loop over bar in layer
     for(int ipr=0;ipr<2;ipr++){
       stfile >> stat[cnum][ipr];//stat(s1),stat(s2)
     }
     cnum+=1; // sequential counter numbering(0-...)
   } // --- end of bar loop --->
   } // --- end of layer loop --->
//
   stfile >> endflab;//read endfile-label
//
   stfile.close();
//
   if(endflab==12345){
     cout<<"      MC_Seed Calib_status file successfully read !"<<endl;
   }
   else{cout<<"<---- problems with MC_Seed Calib-status file !!!"<<endl;
     exit(1);
   }
//
//------------------------------------------------- 
//
//   ---> Read MCSeed anodes/dynodes relat.gains, anode/dynode ratios,
//       mip2q's, A-profile param.  calib.file :
//
   endflab=0;
  ctyp=5;
   strcpy(name,"TofAmplf");
   mcvn=mcvern[ctyp-1];
   if(TFMCFFKEY.mcseedo==0)strcat(name,vers1);//mc
   else strcat(name,vers2);//sd = copy of rl
   strcat(name,".");
//
   sprintf(ext,"%d",mcvn);//got TofSnormMC. file extention
   strcat(name,ext);
   if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
   if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
   strcat(fname,name);
   cout<<"      Opening TOF MC_Seed AmplificationParameters file : "<<fname<<'\n';
   ifstream gcfile(fname,ios::in); // open file for reading
   if(!gcfile){
     cout <<"<---- missing TOF MC-Seed AmplificationParameters file !!! "<<fname<<endl;
     exit(1);
   }
//
// ----------------> read anode relat.gains:
//
   cnum=0;
   for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // read side-1
       gcfile >> gaina[cnum+ibr][0];
     }
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // read side-2
       gcfile >> gaina[cnum+ibr][1];
     }
     cnum+=TOF2DBc::getbppl(ila);
   } // --- end of layer loop --->
//
// ----------------> read anode/dynode ratios:
//
   cnum=0;
   for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // read side-1
       gcfile >> a2drf[cnum+ibr][0];
     }
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // read side-2
       gcfile >> a2drf[cnum+ibr][1];
     }
     cnum+=TOF2DBc::getbppl(ila);
   } // --- end of layer loop --->
//
// ----------------> read dynode-pmts gains(relat to side average):
//
   cnum=0;
   for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  //padles loop s-1
       for(ipm=0;ipm<TOF2GC::PMTSMX;ipm++)gcfile >> gaind[cnum+ibr][0][ipm]; // read PMs
     }
     for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  //padles loop s-2 
       for(ipm=0;ipm<TOF2GC::PMTSMX;ipm++)gcfile >> gaind[cnum+ibr][1][ipm];// read PMs
     }
     cnum+=TOF2DBc::getbppl(ila);
   } // --- end of layer loop --->
// ---------------->
   gcfile >> endflab;//read endfile-label
//
   gcfile.close();
//
   if(endflab==12345){
     cout<<"      TOF MC-Seed AmplificationParameters file is successfully read !"<<endl;
   }
   else{cout<<"<---- problem with TOF MC-Seed AmplificationParameters file !!!"<<endl;
     exit(1);
   }
//   
//---------------------------------------------
//   ===> fill TOFBrcal bank :
//
  a2q=TFCAFFKEY.adc2q;//from DC ! (if variation are high - need special indiv.adc2q's file)
  cnum=0;
  for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
  for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // <-------- loop over bar in layer
    brt=TOF2DBc::brtype(ila,ibr);
    npm=TOFWScan::scmcscan[brt-1].getnpmts();//get pmts/side 
    hblen=0.5*TOF2DBc::brlen(ila,ibr);
    gna[0]=gaina[cnum][0];
    gna[1]=gaina[cnum][1];
//
    sid=100*(ila+1)+(ibr+1);
//    strat[0][0]=strf[cnum][0];//stretcher param. from ext.file
//    strat[1][0]=strf[cnum][1];
//    strat[0][1]=strof[cnum][0];
//    strat[1][1]=strof[cnum][1];
    strat[0][0]=25;//tempor dummy
    strat[1][0]=25;
    strat[0][1]=1100;
    strat[1][1]=1100;
    sta[0]=stat[cnum][0];
    sta[1]=stat[cnum][1];
    a2dr[0]=a2drf[cnum][0];//an/dyn(hichan) ratios from ext.file
    a2dr[1]=a2drf[cnum][1];
    for(ipm=0;ipm<TOF2GC::PMTSMX;ipm++){
      gnd[0][ipm]=gaind[cnum][0][ipm];// dyn-pms rel.gains from ext.file
      gnd[1][ipm]=gaind[cnum][1][ipm];// dyn-pms rel.gains from ext.file
    }
    scbrcal[ila][ibr]=TOFBrcalMS(sid,sta,gna,gnd,a2dr,strat,a2q);
//
    cnum+=1;// solid sequential numbering of all counter(0->33)
  } // --- end of bar loop --->
  } // --- end of layer loop --->
//
  cout<<"<---- TOF2BrcalMS::build: succsessfully done !"<<endl<<endl;
}
//-----------------------------------------------------------------------
//
void TOFBrcalMS::q2a2q(int cof, int sdf, int hlf, number &adc, number &q){  
// Q(pC) <-> ADC(ch,float) to use in sim./rec. programs (cof=0/1-> Q->ADC/ADC->Q)
//                                                      (sdf=0/1-> bar side 1/2 )
//                                                      (hlf=0/1-> for A/D)
//
// WARNING : for dynode(h) Side-signal(adc or q) is the SUM of PMTs Gain-equilized signals
//
  if(cof==0){ // <=== Q->ADCch
    if(hlf==0){//      <-- for anode
      adc=q/adc2qf;
    }
    else{// <-- for Dynode(high)
      adc=q/adc2qf/a2dr[sdf];//q->Aadc->Dadc(equilized sum)
    }
  }
  else{       // <=== ADCch->Q
    if(hlf==0){//      <-- for anode
      q=adc*adc2qf;
    }
    else{// <-- for Dynode
      q=adc*a2dr[sdf]*adc2qf;//Dadc(equil.sum)->Aadc->q
    }
  }
}
//==========================================================================
//
void TOFBPeds::mcbuild(){// create MC TOFBPeds-objects for each sc.bar
//
  int i,j,ila,ibr,cnum,brt;
  integer sid;
  char fname[80];
  char name[80];
  integer asta[TOF2GC::SCBLMX][2];// array of counter stat
  geant aped[TOF2GC::SCBLMX][2];// array of counter peds
  geant asig[TOF2GC::SCBLMX][2];// array of counter sigmas
  integer dsta[TOF2GC::SCBLMX][2][TOF2GC::PMTSMX];
  geant dped[TOF2GC::SCBLMX][2][TOF2GC::PMTSMX];
  geant dsig[TOF2GC::SCBLMX][2][TOF2GC::PMTSMX];
  integer stata[2];
  geant peda[2],siga[2];
  integer statd[2][TOF2GC::PMTSMX];
  geant pedd[2][TOF2GC::PMTSMX],sigd[2][TOF2GC::PMTSMX];
  int ipm;
//
//
//   --->  Read  default MC-pedestals file :
//
  strcpy(name,"tofp_df_mc");
  cout <<"====> TOFBPeds_mcbuild: default MC-peds file will be used..."<<endl;
// ---> check setup:
//
  if (strstr(AMSJob::gethead()->getsetup(),"AMS02")){
    cout<<"      AMS02 TOF:8/8/10/8-pads/layer setup selected."<<endl;
  }
  else
  {
        cout <<"      Unknown setup !!!"<<endl;
        exit(10);
  }
//
  strcat(name,".dat");
  if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Open file : "<<fname<<'\n';
  ifstream icfile(fname,ios::in); // open pedestals-file for reading
  if(!icfile){
    cout <<"      missing default pedestals-file !!! "<<fname<<endl;
    exit(1);
  }
//---> Read anode:
  cnum=0;
  for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
  for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // <-------- loop over bar in layer
    for(i=0;i<2;i++){// sequence: side1,side2
      icfile >> asta[cnum][i];
      icfile >> aped[cnum][i];
      icfile >> asig[cnum][i];
    }
    cnum+=1;// sequential counter numbering(0-...)
  } // --- end of bar loop --->
  } // --- end of layer loop --->
//
//---> Read dynode:
  cnum=0;
  for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
  for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // <-------- loop over bar in layer
    for(i=0;i<2;i++){//<-- side-loop
      for(ipm=0;ipm<TOF2GC::PMTSMX;ipm++){//<-- pm-loop
        icfile >> dsta[cnum][i][ipm];
        icfile >> dped[cnum][i][ipm];
        icfile >> dsig[cnum][i][ipm];
      }
    }
    cnum+=1;// sequential counter numbering(0-...)
  } // --- end of bar loop --->
  } // --- end of layer loop --->
//
  icfile.close();
//---------------------------------------------
//   ===> fill TOFBPeds bank :
//
  cnum=0;
  for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
  for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // <-------- loop over bar in layer
    brt=TOF2DBc::brtype(ila,ibr);
    sid=100*(ila+1)+(ibr+1);
    for(i=0;i<2;i++){
      stata[i]=asta[cnum][i];
      peda[i]=aped[cnum][i];
      siga[i]=asig[cnum][i];

      for(ipm=0;ipm<TOF2GC::PMTSMX;ipm++){	
        statd[i][ipm]=dsta[cnum][i][ipm];
        pedd[i][ipm]=dped[cnum][i][ipm];
        sigd[i][ipm]=dsig[cnum][i][ipm];
      }
    }
//
    scbrped[ila][ibr]=TOFBPeds(sid,stata,peda,siga,statd,pedd,sigd);
//
    cnum+=1;
  } // --- end of bar loop --->
  } // --- end of layer loop --->
  cout<<"<---- TOFBPeds_mcbuild: succsessfully done !"<<endl<<endl;
}
//==========================================================================
//
void TOFBPeds::build(){// tempor solution for RealData peds.
//
  int i,j,ila,ibr,cnum,brt;
  integer sid;
  char fname[80];
  char name[80];
  integer asta[TOF2GC::SCBLMX][2];// array of counter stat
  geant aped[TOF2GC::SCBLMX][2];// array of counter peds
  geant asig[TOF2GC::SCBLMX][2];// array of counter sigmas
  integer dsta[TOF2GC::SCBLMX][2][TOF2GC::PMTSMX];
  geant dped[TOF2GC::SCBLMX][2][TOF2GC::PMTSMX];
  geant dsig[TOF2GC::SCBLMX][2][TOF2GC::PMTSMX];
  integer stata[2];
  geant peda[2],siga[2];
  integer statd[2][TOF2GC::PMTSMX];
  geant pedd[2][TOF2GC::PMTSMX],sigd[2][TOF2GC::PMTSMX];
  int ipm;
//
//
//   --->  Read pedestals file :
//
  strcpy(name,"tofp_df_rl");
  cout <<"====> TOFBPeds_build: default RD-peds file will be used..."<<endl;
// ---> check setup:
//
  if (strstr(AMSJob::gethead()->getsetup(),"AMS02")){
    cout<<"      AMS02 TOF:8/8/10/8-pads/layer setup selected..."<<endl;
  }
  else
  {
        cout <<"      Unknown setup !!!"<<endl;
        exit(10);
  }
//
  strcat(name,".dat");
  strcpy(fname,"");
#ifndef __TFADBW__
  if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);//use official dir only when not in pedDBWR-mode
#endif
  strcat(fname,name);
  cout<<"      Open file : "<<fname<<'\n';
  ifstream icfile(fname,ios::in); // open pedestals-file for reading
  if(!icfile){
    cout <<"      missing default pedestals-file !!! "<<fname<<endl;
    exit(1);
  }
//---> Read anode:
  cnum=0;
  for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
  for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // <-------- loop over bar in layer
    for(i=0;i<2;i++){// sequence: side1,side2
      icfile >> asta[cnum][i];
      icfile >> aped[cnum][i];
      icfile >> asig[cnum][i];
    }
    cnum+=1;// sequential counter numbering(0-...)
  } // --- end of bar loop --->
  } // --- end of layer loop --->
//
//---> Read dynode:
  cnum=0;
  for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
  for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // <-------- loop over bar in layer
    for(i=0;i<2;i++){//<-- side-loop
      for(ipm=0;ipm<TOF2GC::PMTSMX;ipm++){//<-- pm-loop
        icfile >> dsta[cnum][i][ipm];
        icfile >> dped[cnum][i][ipm];
        icfile >> dsig[cnum][i][ipm];
      }
    }
    cnum+=1;// sequential counter numbering(0-...)
  } // --- end of bar loop --->
  } // --- end of layer loop --->
//
  icfile.close();
//---------------------------------------------
//   ===> fill TOFBPeds bank :
//
  cnum=0;
  for(ila=0;ila<TOF2DBc::getnplns();ila++){   // <-------- loop over layers
  for(ibr=0;ibr<TOF2DBc::getbppl(ila);ibr++){  // <-------- loop over bar in layer
    brt=TOF2DBc::brtype(ila,ibr);
    sid=100*(ila+1)+(ibr+1);
    for(i=0;i<2;i++){
      stata[i]=asta[cnum][i];
      peda[i]=aped[cnum][i];
      siga[i]=asig[cnum][i];
	
      for(ipm=0;ipm<TOF2GC::PMTSMX;ipm++){	
        statd[i][ipm]=dsta[cnum][i][ipm];
        pedd[i][ipm]=dped[cnum][i][ipm];
        sigd[i][ipm]=dsig[cnum][i][ipm];
      }
    }
//
    scbrped[ila][ibr]=TOFBPeds(sid,stata,peda,siga,statd,pedd,sigd);
//
    cnum+=1;
  } // --- end of bar loop --->
  } // --- end of layer loop --->
  cout<<"<---- TOFBPeds_build: succsessfully done !"<<endl<<endl;
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
geant TOF2JobStat::cquality[4][5];
integer TOF2JobStat::daqsf[160];
integer TOF2JobStat::cratr[TOF2GC::SCCRAT][30];
integer TOF2JobStat::cratp[TOF2GC::SCCRAT][30];
integer TOF2JobStat::cratc[TOF2GC::SCCRAT][30];
integer TOF2JobStat::cratm[TOF2GC::SCCRAT][30];
integer TOF2JobStat::sltr[TOF2GC::SCCRAT][TOF2GC::SCSLTM][20];
integer TOF2JobStat::sltp[TOF2GC::SCCRAT][TOF2GC::SCSLTM][20];
integer TOF2JobStat::rdcr1[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
integer TOF2JobStat::rdcr2[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
integer TOF2JobStat::rdcr3[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
integer TOF2JobStat::rdcr4[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
integer TOF2JobStat::rdcp1[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
integer TOF2JobStat::rdcp2[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
integer TOF2JobStat::rdcp3[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
integer TOF2JobStat::rdcp4[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
geant TOF2JobStat::tofantemp[TOF2GC::SCCRAT][TOF2GC::SCFETA];
integer TOF2JobStat::tofstc[40];
//---
  void TOF2JobStat::puttemp(int16u crt, int16u sen, geant t){
    assert(crt<TOF2GC::SCCRAT);
    assert(sen<TOF2GC::SCFETA);
    tofantemp[crt][sen]=t;
  }
  geant TOF2JobStat::gettemp(int16u crt, int16u sen){
    assert(crt<TOF2GC::SCCRAT);
    assert(sen<TOF2GC::SCFETA);
    return tofantemp[crt][sen];
  }
//
  void TOF2JobStat::addtofst(int i){
    assert(i>=0 && i< 40);
#pragma omp critical (addtofst) 
    tofstc[i]+=1;
  }
//---
  void TOF2JobStat::addmc(int i){
    assert(i>=0 && i< TOF2GC::SCJSTA);
#pragma omp critical (addmc) 
    mccount[i]+=1;
  }
  void TOF2JobStat::addre(int i){
    assert(i>=0 && i< TOF2GC::SCJSTA);
#pragma omp critical (addre) 
    recount[i]+=1;
  }
  integer TOF2JobStat::getre(int i){
    assert(i>=0 && i< TOF2GC::SCJSTA);
    return recount[i];
  }
  void TOF2JobStat::addch(int chnum, int i){
    assert(chnum>=0 && i>=0);
    assert(chnum < TOF2GC::SCCHMX && i < TOF2GC::SCCSTA);
#pragma omp critical (addch) 
    chcount[chnum][i]+=1;
  }
  void TOF2JobStat::addbr(int brnum, int i){
    assert(brnum>=0 && i>=0);
    assert(brnum < TOF2GC::SCBLMX && i < TOF2GC::SCCSTA);
#pragma omp critical (addbr) 
    brcount[brnum][i]+=1;
  }
//---
void TOF2JobStat::daqsfr(int16u ie){
  assert(ie<160);
#pragma omp critical (daqsfr) 
  daqsf[ie]+=1;
}
void TOF2JobStat::daqscr(int16u df, int16u crat, int16u ie){
  assert(crat<TOF2GC::SCCRAT && ie<30);
#pragma omp critical (daqscr)
{ 
  if(df==0)cratr[crat][ie]+=1;//raw
  else if(df==1)cratp[crat][ie]+=1;//processed
  else if(df==2)cratc[crat][ie]+=1;//pedcal
  else cratm[crat][ie]+=1;//mixed: raw/comp comparison
}
}
void TOF2JobStat::daqssl(int16u df, int16u crat, int16u slot, int16u ie){
  assert(crat<TOF2GC::SCCRAT && slot<TOF2GC::SCSLTM && ie<20);
#pragma omp critical (daqssl)
{ 
  if(df==0)sltr[crat][slot][ie]+=1;
  else sltp[crat][slot][ie]+=1;
}
}
void TOF2JobStat::daqsch(int16u df, int16u crat, int16u slot, int16u rdch, int16u ie){
  assert(crat<TOF2GC::SCCRAT && slot<TOF2GC::SCSLTM && rdch<TOF2GC::SCRCHM && ie<20);
#pragma omp critical (daqsch)
{ 
  if(df==0)
  {
    if(crat==0)rdcr1[slot][rdch][ie]+=1;
    if(crat==1)rdcr2[slot][rdch][ie]+=1;
    if(crat==2)rdcr3[slot][rdch][ie]+=1;
    if(crat==3)rdcr4[slot][rdch][ie]+=1;
  }
  else
  {
    if(crat==0)rdcp1[slot][rdch][ie]+=1;
    if(crat==1)rdcp2[slot][rdch][ie]+=1;
    if(crat==2)rdcp3[slot][rdch][ie]+=1;
    if(crat==3)rdcp4[slot][rdch][ie]+=1;
  }
}
}

//
// function to print Job-statistics at the end of JOB(RUN):
//
void TOF2JobStat::printstat(){
  int il,ib,ic,icr,isl,ie;
  integer rdcr[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
  char slnames[TOF2GC::SCSLTM][8];
  strcpy(slnames[0],"SDR    ");
  strcpy(slnames[1],"SFET_01");
  strcpy(slnames[2],"SFET_02");
  strcpy(slnames[3],"SPT    ");
  strcpy(slnames[4],"SFET_03");
  strcpy(slnames[5],"SFET_04");
  strcpy(slnames[6],"SFEA_01");
  strcpy(slnames[7],"SFEC_01");
  strcpy(slnames[8],"SFEC_02");
  strcpy(slnames[9],"SFEC_03");
  strcpy(slnames[10],"SFEC_04");
  geant rc;
//
  if(daqsf[0]>0 && MISCFFKEY.dbwrbeg==0){//not empty, not dwwriter job
  printf("\n");
  printf("    ======================= JOB DAQ-decoding statistics ====================\n");
  printf("\n");
  printf("Calls to SDRsegment-decoding : %7d, WrongCrateN : %7d, Rejected due to any fatal errors : %7d\n\n",daqsf[0],daqsf[105],daqsf[1]);
  printf("Format Unkn(OnbP)/Raw/Comp/Mixt : %7d %7d %7d %7d\n\n",daqsf[34],daqsf[35],daqsf[36],daqsf[37]);
  printf("     Crate:                     1       2       3       4\n\n");
  printf("Entries, side=A        :  %7d %7d %7d %7d\n",daqsf[2],daqsf[3],daqsf[4],daqsf[5]);
  printf("Entries, side=B        :  %7d %7d %7d %7d\n",daqsf[101],daqsf[102],daqsf[103],daqsf[104]);
  printf("  WrongFormat|Length   :  %7d %7d %7d %7d\n",daqsf[6],daqsf[7],daqsf[8],daqsf[9]);
  printf("  Format/Length OK     :  %7d %7d %7d %7d\n",daqsf[10],daqsf[11],daqsf[12],daqsf[13]);
  printf("  nonData(inSlaveStat) :  %7d %7d %7d %7d\n",daqsf[22],daqsf[23],daqsf[24],daqsf[25]);
  printf("  PedDataType          :  %7d %7d %7d %7d\n",daqsf[14],daqsf[15],daqsf[16],daqsf[17]);
  printf("  PedDataLength OK     :  %7d %7d %7d %7d\n",daqsf[18],daqsf[19],daqsf[20],daqsf[21]);
  printf("  Err in SlaveStatWord :  %7d %7d %7d %7d\n",daqsf[26],daqsf[27],daqsf[28],daqsf[29]);
  printf("  SlaveStatWord OK     :  %7d %7d %7d %7d\n\n",daqsf[30],daqsf[31],daqsf[32],daqsf[33]);
//
  cout<<"           More details on Reply Status Word Bits(for Data only):"<<endl<<endl;
  cout<<"    CRC-error          : ";
  for(icr=0;icr<4;icr++)cout<<setw(8)<<daqsf[40+8*icr];
  cout<<endl;
  cout<<"    Assembly-error     : ";
  for(icr=0;icr<4;icr++)cout<<setw(8)<<daqsf[41+8*icr];
  cout<<endl;
  cout<<"    AMSWire-error      : ";
  for(icr=0;icr<4;icr++)cout<<setw(8)<<daqsf[42+8*icr];
  cout<<endl;
  cout<<"    TimeOut-error      : ";
  for(icr=0;icr<4;icr++)cout<<setw(8)<<daqsf[43+8*icr];
  cout<<endl;
  cout<<"    FEPower-error      : ";
  for(icr=0;icr<4;icr++)cout<<setw(8)<<daqsf[44+8*icr];
  cout<<endl;
  cout<<"    Sequencer-error    : ";
  for(icr=0;icr<4;icr++)cout<<setw(8)<<daqsf[45+8*icr];
  cout<<endl;
  cout<<"    CDP-node bit set   : ";
  for(icr=0;icr<4;icr++)cout<<setw(8)<<daqsf[46+8*icr];
  cout<<endl<<endl;
  
  printf("\n\n");
//
  if(daqsf[35]>0 || daqsf[37]>0){
  printf("----------------> RawFMT Segment Decoding statistics: \n\n");
  printf("     Crate:                     1         2         3         4\n\n");
  printf("Total Segments         :  %7d   %7d   %7d   %7d\n",cratr[0][0],cratr[1][0],cratr[2][0],cratr[3][0]);
  printf("length ovfl(build)     :  %7d   %7d   %7d   %7d\n",cratr[0][1],cratr[1][1],cratr[2][1],cratr[3][1]);
  printf("length error           :  %7d   %7d   %7d   %7d\n",cratr[0][2],cratr[1][2],cratr[2][2],cratr[3][2]);
  printf("LinkErr in Q-blk       :  %7d   %7d   %7d   %7d\n",cratr[0][3],cratr[1][3],cratr[2][3],cratr[3][3]);
  printf("SlotErr in Q-blk       :  %7d   %7d   %7d   %7d\n",cratr[0][4],cratr[1][4],cratr[2][4],cratr[3][4]);
  printf("NonTP words in TP-blk  :  %7d   %7d   %7d   %7d\n",cratr[0][5],cratr[1][5],cratr[2][5],cratr[3][5]);
  printf("TmOut/CMDerr in TP-blk :  %7d   %7d   %7d   %7d\n",cratr[0][6],cratr[1][6],cratr[2][6],cratr[3][6]);
  printf("LinkErr in Time-blk    :  %7d   %7d   %7d   %7d\n",cratr[0][7],cratr[1][7],cratr[2][7],cratr[3][7]);
  printf("SlotErr in Time-blk    :  %7d   %7d   %7d   %7d\n",cratr[0][8],cratr[1][8],cratr[2][8],cratr[3][8]);
  printf("\n");
  printf("-----> Slots statistics(words counting) :\n\n");
  printf("SFET_1  Q-entries      :  %7d   %7d   %7d   %7d\n",sltr[0][1][0],sltr[1][1][0],sltr[2][1][0],sltr[3][1][0]);
  printf("SFET_2  Q-entries      :  %7d   %7d   %7d   %7d\n",sltr[0][2][0],sltr[1][2][0],sltr[2][2][0],sltr[3][2][0]);
  printf("SFET_3  Q-entries      :  %7d   %7d   %7d   %7d\n",sltr[0][4][0],sltr[1][4][0],sltr[2][4][0],sltr[3][4][0]);
  printf("SFET_4  Q-entries      :  %7d   %7d   %7d   %7d\n",sltr[0][5][0],sltr[1][5][0],sltr[2][5][0],sltr[3][5][0]);
  printf("SFEA_1  Q-entries      :  %7d   %7d   %7d   %7d\n",sltr[0][6][0],sltr[1][6][0],sltr[2][6][0],sltr[3][6][0]);
  printf("SFEC_1    entries      :  %7d   %7d   %7d   %7d\n",sltr[0][7][0],sltr[1][7][0],sltr[2][7][0],sltr[3][7][0]);
  printf("SFEC_2    entries      :  %7d   %7d   %7d   %7d\n",sltr[0][8][0],sltr[1][8][0],sltr[2][8][0],sltr[3][8][0]);
  printf("SFEC_3    entries      :  %7d   %7d   %7d   %7d\n",sltr[0][9][0],sltr[1][9][0],sltr[2][9][0],sltr[3][9][0]);
  printf("SFEC_4    entries      :  %7d   %7d   %7d   %7d\n",sltr[0][10][0],sltr[1][10][0],sltr[2][10][0],sltr[3][10][0]);
  printf("\n");
  printf("SFET_1 TDCbuff-entries :  %7d   %7d   %7d   %7d\n",sltr[0][1][1],sltr[1][1][1],sltr[2][1][1],sltr[3][1][1]);
  printf("SFET_2   ...........   :  %7d   %7d   %7d   %7d\n",sltr[0][2][1],sltr[1][2][1],sltr[2][2][1],sltr[3][2][1]);
  printf("SFET_3   ...........   :  %7d   %7d   %7d   %7d\n",sltr[0][4][1],sltr[1][4][1],sltr[2][4][1],sltr[3][4][1]);
  printf("SFET_4   ...........   :  %7d   %7d   %7d   %7d\n",sltr[0][5][1],sltr[1][5][1],sltr[2][5][1],sltr[3][5][1]);
  printf("SFEA_1   ...........   :  %7d   %7d   %7d   %7d\n",sltr[0][6][1],sltr[1][6][1],sltr[2][6][1],sltr[3][6][1]);
  printf("\n");
  printf("SFET_1  TDCbuff-ovfl   :  %7d   %7d   %7d   %7d\n",sltr[0][1][2],sltr[1][1][2],sltr[2][1][2],sltr[3][1][2]);
  printf("SFET_2   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][2][2],sltr[1][2][2],sltr[2][2][2],sltr[3][2][2]);
  printf("SFET_3   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][4][2],sltr[1][4][2],sltr[2][4][2],sltr[3][4][2]);
  printf("SFET_4   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][5][2],sltr[1][5][2],sltr[2][5][2],sltr[3][5][2]);
  printf("SFEA_1   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][6][2],sltr[1][6][2],sltr[2][6][2],sltr[3][6][2]);
  printf("\n");
  printf("SFET_1  SkipBadWtype   :  %7d   %7d   %7d   %7d\n",sltr[0][1][3],sltr[1][1][3],sltr[2][1][3],sltr[3][1][3]);
  printf("SFET_2   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][2][3],sltr[1][2][3],sltr[2][2][3],sltr[3][2][3]);
  printf("SFET_3   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][4][3],sltr[1][4][3],sltr[2][4][3],sltr[3][4][3]);
  printf("SFET_4   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][5][3],sltr[1][5][3],sltr[2][5][3],sltr[3][5][3]);
  printf("SFEA_1   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][6][3],sltr[1][6][3],sltr[2][6][3],sltr[3][6][3]);
  printf("\n");
  printf("SFET_1   NoTrailer     :  %7d   %7d   %7d   %7d\n",sltr[0][1][4],sltr[1][1][4],sltr[2][1][4],sltr[3][1][4]);
  printf("SFET_2   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][2][4],sltr[1][2][4],sltr[2][2][4],sltr[3][2][4]);
  printf("SFET_3   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][4][4],sltr[1][4][4],sltr[2][4][4],sltr[3][4][4]);
  printf("SFET_4   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][5][4],sltr[1][5][4],sltr[2][5][4],sltr[3][5][4]);
  printf("SFEA_1   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][6][4],sltr[1][6][4],sltr[2][6][4],sltr[3][6][4]);
  printf("\n");
  printf("SFET_1   TrailerAlone  :  %7d   %7d   %7d   %7d\n",sltr[0][1][5],sltr[1][1][5],sltr[2][1][5],sltr[3][1][5]);
  printf("SFET_2   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][2][5],sltr[1][2][5],sltr[2][2][5],sltr[3][2][5]);
  printf("SFET_3   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][4][5],sltr[1][4][5],sltr[2][4][5],sltr[3][4][5]);
  printf("SFET_4   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][5][5],sltr[1][5][5],sltr[2][5][5],sltr[3][5][5]);
  printf("SFEA_1   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][6][5],sltr[1][6][5],sltr[2][6][5],sltr[3][6][5]);
  printf("\n");
  printf("SFET_1  TDC_BadStruct  :  %7d   %7d   %7d   %7d\n",sltr[0][1][6],sltr[1][1][6],sltr[2][1][6],sltr[3][1][6]);
  printf("SFET_2   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][2][6],sltr[1][2][6],sltr[2][2][6],sltr[3][2][6]);
  printf("SFET_3   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][4][6],sltr[1][4][6],sltr[2][4][6],sltr[3][4][6]);
  printf("SFET_4   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][5][6],sltr[1][5][6],sltr[2][5][6],sltr[3][5][6]);
  printf("SFEA_1   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][6][6],sltr[1][6][6],sltr[2][6][6],sltr[3][6][6]);
  printf("\n");
  printf("SFET_1 *** miss Temper :  %7d   %7d   %7d   %7d\n",sltr[0][1][7],sltr[1][1][7],sltr[2][1][7],sltr[3][1][7]);
  printf("SFET_2   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][2][7],sltr[1][2][7],sltr[2][2][7],sltr[3][2][7]);
  printf("SFET_3   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][4][7],sltr[1][4][7],sltr[2][4][7],sltr[3][4][7]);
  printf("SFET_4   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][5][7],sltr[1][5][7],sltr[2][5][7],sltr[3][5][7]);
  printf("SFEA_1   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][6][7],sltr[1][6][7],sltr[2][6][7],sltr[3][6][7]);
  printf("\n");
  printf("SFET_1 *** miss Header :  %7d   %7d   %7d   %7d\n",sltr[0][1][8],sltr[1][1][8],sltr[2][1][8],sltr[3][1][8]);
  printf("SFET_2   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][2][8],sltr[1][2][8],sltr[2][2][8],sltr[3][2][8]);
  printf("SFET_3   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][4][8],sltr[1][4][8],sltr[2][4][8],sltr[3][4][8]);
  printf("SFET_4   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][5][8],sltr[1][5][8],sltr[2][5][8],sltr[3][5][8]);
  printf("SFEA_1   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][6][8],sltr[1][6][8],sltr[2][6][8],sltr[3][6][8]);
  printf("\n");
  printf("SFET_1 *** TimeOut     :  %7d   %7d   %7d   %7d\n",sltr[0][1][9],sltr[1][1][9],sltr[2][1][9],sltr[3][1][9]);
  printf("SFET_2   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][2][9],sltr[1][2][9],sltr[2][2][9],sltr[3][2][9]);
  printf("SFET_3   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][4][9],sltr[1][4][9],sltr[2][4][9],sltr[3][4][9]);
  printf("SFET_4   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][5][9],sltr[1][5][9],sltr[2][5][9],sltr[3][5][9]);
  printf("SFEA_1   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][6][9],sltr[1][6][9],sltr[2][6][9],sltr[3][6][9]);
  printf("\n");
  printf("SFET_1 *** Error Word  :  %7d   %7d   %7d   %7d\n",sltr[0][1][10],sltr[1][1][10],sltr[2][1][10],sltr[3][1][10]);
  printf("SFET_2   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][2][10],sltr[1][2][10],sltr[2][2][10],sltr[3][2][10]);
  printf("SFET_3   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][4][10],sltr[1][4][10],sltr[2][4][10],sltr[3][4][10]);
  printf("SFET_4   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][5][10],sltr[1][5][10],sltr[2][5][10],sltr[3][5][10]);
  printf("SFEA_1   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][6][10],sltr[1][6][10],sltr[2][6][10],sltr[3][6][10]);
  printf("\n");
  printf("SFET_1  TDC_Data_OK    :  %7d   %7d   %7d   %7d\n",sltr[0][1][11],sltr[1][1][11],sltr[2][1][11],sltr[3][1][11]);
  printf("SFET_2   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][2][11],sltr[1][2][11],sltr[2][2][11],sltr[3][2][11]);
  printf("SFET_3   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][4][11],sltr[1][4][11],sltr[2][4][11],sltr[3][4][11]);
  printf("SFET_4   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][5][11],sltr[1][5][11],sltr[2][5][11],sltr[3][5][11]);
  printf("SFEA_1   ..........    :  %7d   %7d   %7d   %7d\n",sltr[0][6][11],sltr[1][6][11],sltr[2][6][11],sltr[3][6][11]);
  printf("\n");
  printf("-----> Charge-channels statistics :\n\n");
  for(icr=0;icr<TOF2GC::SCCRAT;icr++){
    printf("       for Crate-%1d :\n",(icr+1));
    for(isl=0;isl<TOF2GC::SCSLTM;isl++){
      if(isl==0 || isl==3)continue;
      for(ic=0;ic<TOF2GC::SCRCHM;ic++){
	for(ie=0;ie<20;ie++){
	  if(icr==0)rdcr[isl][ic][ie]=rdcr1[isl][ic][ie];
	  if(icr==1)rdcr[isl][ic][ie]=rdcr2[isl][ic][ie];
	  if(icr==2)rdcr[isl][ic][ie]=rdcr3[isl][ic][ie];
	  if(icr==3)rdcr[isl][ic][ie]=rdcr4[isl][ic][ie];
	}
      }
      cout<<"     entrs/INch in "<<slnames[isl]<<" :";
      for(ic=0;ic<TOF2GC::SCRCHM;ic++)cout<<" "<<rdcr[isl][ic][0];
      cout<<endl;
    }
  }
  printf("\n");
  printf("-----> Time-channels statistics :\n\n");
  for(icr=0;icr<TOF2GC::SCCRAT;icr++){
    printf("       for Crate-%1d :\n",(icr+1));
    for(isl=0;isl<TOF2GC::SCSLTM;isl++){
      if(isl==0 || isl==3 || isl>=7)continue;
      for(ic=0;ic<TOF2GC::SCRCHM;ic++){
	for(ie=0;ie<20;ie++){
	  if(icr==0)rdcr[isl][ic][ie]=rdcr1[isl][ic][ie];
	  if(icr==1)rdcr[isl][ic][ie]=rdcr2[isl][ic][ie];
	  if(icr==2)rdcr[isl][ic][ie]=rdcr3[isl][ic][ie];
	  if(icr==3)rdcr[isl][ic][ie]=rdcr4[isl][ic][ie];
	}
      }
      cout<<"     entrs/INch in "<<slnames[isl]<<" :";
      for(ic=0;ic<TOF2GC::SCRCHM;ic++)cout<<" "<<rdcr[isl][ic][1];
      cout<<endl;
    }
  }
  printf("\n");
  printf("----->        Found Nhits overflows in Time-channel :\n\n");
  for(icr=0;icr<TOF2GC::SCCRAT;icr++){
    printf("       for Crate-%1d :\n",(icr+1));
    for(isl=0;isl<TOF2GC::SCSLTM;isl++){
      if(isl==0 || isl==3 || isl>=7)continue;
      for(ic=0;ic<TOF2GC::SCRCHM;ic++){
	for(ie=0;ie<20;ie++){
	  if(icr==0)rdcr[isl][ic][ie]=rdcr1[isl][ic][ie];
	  if(icr==1)rdcr[isl][ic][ie]=rdcr2[isl][ic][ie];
	  if(icr==2)rdcr[isl][ic][ie]=rdcr3[isl][ic][ie];
	  if(icr==3)rdcr[isl][ic][ie]=rdcr4[isl][ic][ie];
	}
      }
      cout<<"     overfls/INch in "<<slnames[isl]<<" :";
      for(ic=0;ic<TOF2GC::SCRCHM;ic++)cout<<" "<<rdcr[isl][ic][2];
      cout<<endl;
    }
  }
  printf("\n");
  }
//----
  if(daqsf[36]>0 || daqsf[37]>0){
  printf("---------------> Compressed(Processed)FMT Segment Decoding statistics: \n\n");
  printf("     Crate:                       1       2       3       4\n");
  printf(" Entries(alone||inMixtFMT):    %7d %7d %7d %7d\n",cratp[0][0],cratp[1][0],cratp[2][0],cratp[3][0]);
  printf(" ...inMix when RawSubs OK :    %7d %7d %7d %7d\n",cratp[0][1],cratp[1][1],cratp[2][1],cratp[3][1]);
  printf(" trunc_len(in MixtFMT,rej):    %7d %7d %7d %7d\n",cratp[0][2],cratp[1][2],cratp[2][2],cratp[3][2]);
  printf(" len_mism(inside Mixt,rej):    %7d %7d %7d %7d\n",cratp[0][4],cratp[1][4],cratp[2][4],cratp[3][4]);
  printf(" len_mism(stand-alone,rej):    %7d %7d %7d %7d\n",cratp[0][5],cratp[1][5],cratp[2][5],cratp[3][5]);
  printf("\n");
  printf(" TrPatt-block IllegalWord :    %7d %7d %7d %7d\n",cratp[0][6],cratp[1][6],cratp[2][6],cratp[3][6]);
  printf(" .................TimeOut :    %7d %7d %7d %7d\n",cratp[0][7],cratp[1][7],cratp[2][7],cratp[3][7]);
  printf("\n");
  printf("StatWords:TruncationFlgOn :    %7d %7d %7d %7d\n",cratp[0][3],cratp[1][3],cratp[2][3],cratp[3][3]);
  printf(" .......VerifMask problems:    %7d %7d %7d %7d\n",cratp[0][8],cratp[1][8],cratp[2][8],cratp[3][8]);
  printf("\n");
  printf(" Q-block LinkHeaderErr    :    %7d %7d %7d %7d\n",cratp[0][9],cratp[1][9],cratp[2][9],cratp[3][9]);
  printf(" IllegSlotNumb in Q-block :    %7d %7d %7d %7d\n",cratp[0][10],cratp[1][10],cratp[2][10],cratp[3][10]);
  printf("\n");
  printf(" T-Blk: Raw->Comp TL-Err  :    %7d %7d %7d %7d\n",cratp[0][11],cratp[1][11],cratp[2][11],cratp[3][11]);
  printf("  Raw->Comp HTS_Err-Link0 :    %7d %7d %7d %7d\n",cratp[0][12],cratp[1][12],cratp[2][12],cratp[3][12]);
  printf("  Raw->Comp HTS_Err-Link1 :    %7d %7d %7d %7d\n",cratp[0][13],cratp[1][13],cratp[2][13],cratp[3][13]);
  printf("  Raw->Comp HTS_Err-Link2 :    %7d %7d %7d %7d\n",cratp[0][14],cratp[1][14],cratp[2][14],cratp[3][14]);
  printf("  Raw->Comp HTS_Err-Link3 :    %7d %7d %7d %7d\n",cratp[0][15],cratp[1][15],cratp[2][15],cratp[3][15]);
  printf("  Raw->Comp HTS_Err-Link4 :    %7d %7d %7d %7d\n",cratp[0][16],cratp[1][16],cratp[2][16],cratp[3][16]);
  printf("  Invalid slot number     :    %7d %7d %7d %7d\n",cratp[0][17],cratp[1][17],cratp[2][17],cratp[3][17]);
  printf("\n");
  printf("-----> Slots statistics(words counting) :\n\n");
  printf("SFET_1  Q-entries      :  %7d   %7d   %7d   %7d\n",sltp[0][1][0],sltp[1][1][0],sltp[2][1][0],sltp[3][1][0]);
  printf("SFET_2  Q-entries      :  %7d   %7d   %7d   %7d\n",sltp[0][2][0],sltp[1][2][0],sltp[2][2][0],sltp[3][2][0]);
  printf("SFET_3  Q-entries      :  %7d   %7d   %7d   %7d\n",sltp[0][4][0],sltp[1][4][0],sltp[2][4][0],sltp[3][4][0]);
  printf("SFET_4  Q-entries      :  %7d   %7d   %7d   %7d\n",sltp[0][5][0],sltp[1][5][0],sltp[2][5][0],sltp[3][5][0]);
  printf("SFEA_1  Q-entries      :  %7d   %7d   %7d   %7d\n",sltp[0][6][0],sltp[1][6][0],sltp[2][6][0],sltp[3][6][0]);
  printf("SFEC_1    entries      :  %7d   %7d   %7d   %7d\n",sltp[0][7][0],sltp[1][7][0],sltp[2][7][0],sltp[3][7][0]);
  printf("SFEC_2    entries      :  %7d   %7d   %7d   %7d\n",sltp[0][8][0],sltp[1][8][0],sltp[2][8][0],sltp[3][8][0]);
  printf("SFEC_3    entries      :  %7d   %7d   %7d   %7d\n",sltp[0][9][0],sltp[1][9][0],sltp[2][9][0],sltp[3][9][0]);
  printf("SFEC_4    entries      :  %7d   %7d   %7d   %7d\n",sltp[0][10][0],sltp[1][10][0],sltp[2][10][0],sltp[3][10][0]);
  printf("\n");
  printf("SFET_1 too low adc-ped :  %7d   %7d   %7d   %7d\n",sltp[0][1][1],sltp[1][1][1],sltp[2][1][1],sltp[3][1][1]);
  printf("SFET_2     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][2][1],sltp[1][2][1],sltp[2][2][1],sltp[3][2][1]);
  printf("SFET_3     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][4][1],sltp[1][4][1],sltp[2][4][1],sltp[3][4][1]);
  printf("SFET_4     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][5][1],sltp[1][5][1],sltp[2][5][1],sltp[3][5][1]);
  printf("SFEA_1     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][6][1],sltp[1][6][1],sltp[2][6][1],sltp[3][6][1]);
  printf("SFEC_1     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][7][1],sltp[1][7][1],sltp[2][7][1],sltp[3][7][1]);
  printf("SFEC_2     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][8][1],sltp[1][8][1],sltp[2][8][1],sltp[3][8][1]);
  printf("SFEC_3     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][9][1],sltp[1][9][1],sltp[2][9][1],sltp[3][9][1]);
  printf("SFEC_4     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][10][1],sltp[1][10][1],sltp[2][10][1],sltp[3][10][1]);
  printf("\n");
  printf("SFET_1  T-entries      :  %7d   %7d   %7d   %7d\n",sltp[0][1][2],sltp[1][1][2],sltp[2][1][2],sltp[3][1][2]);
  printf("SFET_2  T-entries      :  %7d   %7d   %7d   %7d\n",sltp[0][2][2],sltp[1][2][2],sltp[2][2][2],sltp[3][2][2]);
  printf("SFET_3  T-entries      :  %7d   %7d   %7d   %7d\n",sltp[0][4][2],sltp[1][4][2],sltp[2][4][2],sltp[3][4][2]);
  printf("SFET_4  T-entries      :  %7d   %7d   %7d   %7d\n",sltp[0][5][2],sltp[1][5][2],sltp[2][5][2],sltp[3][5][2]);
  printf("SFEA_1  T-entries      :  %7d   %7d   %7d   %7d\n",sltp[0][6][2],sltp[1][6][2],sltp[2][6][2],sltp[3][6][2]);
  printf("\n");
  printf("SFET_1  with no temper :  %7d   %7d   %7d   %7d\n",sltp[0][1][3],sltp[1][1][3],sltp[2][1][3],sltp[3][1][3]);
  printf("SFET_2     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][2][3],sltp[1][2][3],sltp[2][2][3],sltp[3][2][3]);
  printf("SFET_3     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][4][3],sltp[1][4][3],sltp[2][4][3],sltp[3][4][3]);
  printf("SFET_4     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][5][3],sltp[1][5][3],sltp[2][5][3],sltp[3][5][3]);
  printf("SFEA_1     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][6][3],sltp[1][6][3],sltp[2][6][3],sltp[3][6][3]);
  printf("\n");
  printf("SFET_1  with err-word  :  %7d   %7d   %7d   %7d\n",sltp[0][1][4],sltp[1][1][4],sltp[2][1][4],sltp[3][1][4]);
  printf("SFET_2     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][2][4],sltp[1][2][4],sltp[2][2][4],sltp[3][2][4]);
  printf("SFET_3     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][4][4],sltp[1][4][4],sltp[2][4][4],sltp[3][4][4]);
  printf("SFET_4     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][5][4],sltp[1][5][4],sltp[2][5][4],sltp[3][5][4]);
  printf("SFEA_1     ...         :  %7d   %7d   %7d   %7d\n",sltp[0][6][4],sltp[1][6][4],sltp[2][6][4],sltp[3][6][4]);
  printf("\n");
  printf("-----> Charge-channels statistics :\n\n");
  for(icr=0;icr<TOF2GC::SCCRAT;icr++){
    printf("       for Crate-%1d :\n",(icr+1));
    for(isl=0;isl<TOF2GC::SCSLTM;isl++){
      if(isl==0 || isl==3)continue;
      for(ic=0;ic<TOF2GC::SCRCHM;ic++){
	for(ie=0;ie<20;ie++){
	  if(icr==0)rdcr[isl][ic][ie]=rdcp1[isl][ic][ie];
	  if(icr==1)rdcr[isl][ic][ie]=rdcp2[isl][ic][ie];
	  if(icr==2)rdcr[isl][ic][ie]=rdcp3[isl][ic][ie];
	  if(icr==3)rdcr[isl][ic][ie]=rdcp4[isl][ic][ie];
	}
      }
      cout<<"     entrs/INch in "<<slnames[isl]<<" :";
      for(ic=0;ic<TOF2GC::SCRCHM;ic++)cout<<" "<<rdcr[isl][ic][0];
      cout<<endl;
    }
  }
  printf("\n");
  printf("-----> Time-channels statistics :\n\n");
  for(icr=0;icr<TOF2GC::SCCRAT;icr++){
    printf("       for Crate-%1d :\n",(icr+1));
    for(isl=0;isl<TOF2GC::SCSLTM;isl++){
      if(isl==0 || isl==3 || isl>=7)continue;
      for(ic=0;ic<TOF2GC::SCRCHM;ic++){
	for(ie=0;ie<20;ie++){
	  if(icr==0)rdcr[isl][ic][ie]=rdcp1[isl][ic][ie];
	  if(icr==1)rdcr[isl][ic][ie]=rdcp2[isl][ic][ie];
	  if(icr==2)rdcr[isl][ic][ie]=rdcp3[isl][ic][ie];
	  if(icr==3)rdcr[isl][ic][ie]=rdcp4[isl][ic][ie];
	}
      }
      cout<<"     entrs/INch in "<<slnames[isl]<<" :";
      for(ic=0;ic<TOF2GC::SCRCHM;ic++)cout<<" "<<rdcr[isl][ic][1];
      cout<<endl;
    }
  }
  printf("\n");
  printf("----->        Found Nhits overflows in Time-channel :\n\n");
  for(icr=0;icr<TOF2GC::SCCRAT;icr++){
    printf("       for Crate-%1d :\n",(icr+1));
    for(isl=0;isl<TOF2GC::SCSLTM;isl++){
      if(isl==0 || isl==3 || isl>=7)continue;
      for(ic=0;ic<TOF2GC::SCRCHM;ic++){
	for(ie=0;ie<20;ie++){
	  if(icr==0)rdcr[isl][ic][ie]=rdcp1[isl][ic][ie];
	  if(icr==1)rdcr[isl][ic][ie]=rdcp2[isl][ic][ie];
	  if(icr==2)rdcr[isl][ic][ie]=rdcp3[isl][ic][ie];
	  if(icr==3)rdcr[isl][ic][ie]=rdcp4[isl][ic][ie];
	}
      }
      cout<<"     overfls/INch in "<<slnames[isl]<<" :";
      for(ic=0;ic<TOF2GC::SCRCHM;ic++)cout<<" "<<rdcr[isl][ic][2];
      cout<<endl;
    }
  }
  printf("\n");
  }
//---
  if(daqsf[37]>0){
  printf("---------------> MixtFMT: Raw/Compr-SubSegment comparison statistics: \n\n");
  printf("     Crate:                        1       2       3       4\n");
  printf("Crate-entries(Raw/Com OK):    %7d %7d %7d %7d\n",cratm[0][0],cratm[1][0],cratm[2][0],cratm[3][0]);
  printf("sw-ch with Nh/Id mismatch:    %7d %7d %7d %7d\n",cratm[0][1],cratm[1][1],cratm[2][1],cratm[3][1]);
  printf("sw-ch Hit-value mismatch :    %7d %7d %7d %7d\n",cratm[0][2],cratm[1][2],cratm[2][2],cratm[3][2]);
  printf("Errorless Crate-entries  :    %7d %7d %7d %7d\n",cratm[0][3],cratm[1][3],cratm[2][3],cratm[3][3]);
  printf("\n");
  }
//---
  }
//
//
  printf("\n");
//--------------
  if(daqsf[60]>0){
  printf("---------------> ONBoardPedCalFMT Segment Decoding statistics: \n\n");
  printf("Calls to SDRsegment-decoding : %7d, Rejected due to any fatal errors : %7d\n\n",daqsf[60],daqsf[61]);
  printf("\n");
  printf("     Crate:                        1       2       3       4\n");
  printf("Found PedCal blocks      :    %7d %7d %7d %7d\n",cratc[0][0],cratc[1][0],cratc[2][0],cratc[3][0]);
  printf("Bad CalStatus word       :    %7d %7d %7d %7d\n",cratc[0][1],cratc[1][1],cratc[2][1],cratc[3][1]);
  printf("OK,but frozen bits       :    %7d %7d %7d %7d\n",cratc[0][2],cratc[1][2],cratc[2][2],cratc[3][2]);
  printf("Block length error       :    %7d %7d %7d %7d\n",cratc[0][3],cratc[1][3],cratc[2][3],cratc[3][3]);
  printf("Block length OK          :    %7d %7d %7d %7d\n",cratc[0][4],cratc[1][4],cratc[2][4],cratc[3][4]);
  printf("  DatType/Format OK      :    %7d %7d %7d %7d\n",cratc[0][5],cratc[1][5],cratc[2][5],cratc[3][5]);
  printf("Bits in Slave status:\n");
  printf("   CRC error             :    %7d %7d %7d %7d\n",cratc[0][6],cratc[1][6],cratc[2][6],cratc[3][6]);
  printf("   Assembly error        :    %7d %7d %7d %7d\n",cratc[0][7],cratc[1][7],cratc[2][7],cratc[3][7]);
  printf("   AMS-wire error        :    %7d %7d %7d %7d\n",cratc[0][8],cratc[1][8],cratc[2][8],cratc[3][8]);
  printf("   Timeout               :    %7d %7d %7d %7d\n",cratc[0][9],cratc[1][9],cratc[2][9],cratc[3][9]);
  printf("   FE-power error        :    %7d %7d %7d %7d\n",cratc[0][10],cratc[1][10],cratc[2][10],cratc[3][10]);
  printf("   Sequenser error       :    %7d %7d %7d %7d\n",cratc[0][11],cratc[1][11],cratc[2][11],cratc[3][11]);
  printf("   LastLevelNode         :    %7d %7d %7d %7d\n",cratc[0][12],cratc[1][12],cratc[2][12],cratc[3][12]);
  printf("Slave status OK          :    %7d %7d %7d %7d\n",cratc[0][13],cratc[1][13],cratc[2][13],cratc[3][13]);
  printf("Block was requested      :    %7d %7d %7d %7d\n",cratc[0][14],cratc[1][14],cratc[2][14],cratc[3][14]);
  printf("Rejected due wrong slot# :    %7d %7d %7d %7d\n",cratc[0][15],cratc[1][15],cratc[2][15],cratc[3][15]);
  printf("Block was processed      :    %7d %7d %7d %7d\n",cratc[0][16],cratc[1][16],cratc[2][16],cratc[3][16]);
  printf("\n");
  }
  if(MISCFFKEY.dbwrbeg>0)return;//dbwritwr job, don't need any statistics
//--------------
  if(tofstc[0]>0){
  printf("\n");
  printf("---------------> U/LTof Slow Temperatures Decoding statistics: \n\n");
  printf("\n");
  printf("Calls to STemp-decoding(Bus=2) : %7d,    USCM-ReplyStatus OK : %7d\n",tofstc[0],tofstc[1]);
  printf("--> UTof Entries: %7d\n",tofstc[2]);
  printf("    In ShortFmt : %7d\n",tofstc[3]);
  printf("    In LongFmt  : %7d\n",tofstc[4]);
  printf("    BlockLenOK  : %7d\n",tofstc[5]);
  printf("    Side A(P)   : %7d\n",tofstc[6]);
  printf("    Side B(s)   : %7d\n",tofstc[7]);
  
    
  printf("--> LTof Entries: %7d\n",tofstc[22]);
  printf("    In ShortFmt : %7d\n",tofstc[23]);
  printf("    In LongFmt  : %7d\n",tofstc[24]);
  printf("    BlockLenOK  : %7d\n",tofstc[25]);
  printf("    Side A(P)   : %7d\n",tofstc[26]);
  printf("    Side B(s)   : %7d\n",tofstc[27]);
  }
  printf("\n\n");
//
  printf("    ====================== JOB TOF-statistics ======================\n");
  printf("\n");
  printf(" MC: entries                                : % 6d\n",mccount[0]);
  printf("   MCGen FastSel OK                         : % 6d\n",mccount[14]);
  printf("   Ghits->TovT OK(err.stat.follow)          : % 6d\n",mccount[1]);
  printf("        OutOfTime GHits                     : % 6d\n",mccount[11]);
  printf("        out of total GHits                  : % 6d\n",mccount[12]);
  printf("        Bars with OutOfSizeHits             : % 6d\n",mccount[3]);
  printf("        out of bars fired                   : % 6d\n",mccount[4]);
  printf("        FlashADC time-scale overflows       : % 6d\n",mccount[5]);
  printf("        LTtime-hits buffer overflows        : % 6d\n",mccount[25]);
  printf("        SumHTtime-hits buffer overflows     : % 6d\n",mccount[26]);
  printf("        SumSHTtime-hits buffer overflows    : % 6d\n",mccount[27]);
  printf("   TovT->RawEvent(FT) OK(err.stat.follow)   : % 6d\n",mccount[2]);
  printf("        T-hit out of Trig.match window      : % 6d\n",mccount[7]);
  printf("                                            : % 6d\n",mccount[8]);
  printf("        Anode-ADC overflows                 : % 6d\n",mccount[9]);
  printf("        Dynode-ADC overflows                : % 6d\n",mccount[10]);
  printf("   FastTrigStatistics :\n");
  printf("        ParticleTrigger requests            : % 6d\n",mccount[15]);
  printf("        found TOF-FTC(Z>=1)                 : % 6d\n",mccount[16]);
  printf("        found TOF-BZ(Z>=2 when globFT)      : % 6d\n",mccount[17]);
  printf("        found TOF-FTZ(SlowZ>=2)             : % 6d\n",mccount[18]);
  printf("        found TOF-FTZ when FTC missing      : % 6d\n",mccount[23]);
  printf("        found EC-FTE                        : % 6d\n",mccount[19]);
  printf("        found EC-FTE when TOF-FT missing    : % 6d\n",mccount[20]);
  printf("        found globFT(after masking)         : % 6d\n",mccount[21]);
  printf("        ExternalTrigger requests            : % 6d\n",mccount[22]);
  
  
  printf(" RECO-entries                               : % 6d\n",recount[0]);
  printf("   GlobFT(FTC|FTZ|FTE|Ext) found at LVL1    : % 6d\n",recount[1]);
  printf("   TOF_FT(FTC|FTZ) found ...............    : % 6d\n",recount[5]);
  printf("   NoTOF_FT but ECAL_FT(FTE) found .....    : % 6d\n",recount[6]);
  printf("   RawSide-validation OK                    : % 6d\n",recount[2]);
  printf("   Validation:SFEC-tempMissing(on any side) : % 6d\n",recount[55]);
  printf("   Validation:PMT -tempMissing(on any side) : % 6d\n",recount[56]);
  printf("   RawSide->RawCluster  OK                  : % 6d\n",recount[3]);
  printf("   RawCluster->Cluster OK                   : % 6d\n",recount[4]);
  printf("   TofBeta 'OutOfRange' warnings            : % 6d\n",recount[7]);
  if(AMSJob::gethead()->isCalibration() & AMSJob::CTOF){
    if(recount[10]>0){
    printf(" Entries to TofTimeAmpl-calibration     : % 6d\n",recount[10]);
    printf("   Tof-paddles multiplicity OK          : % 6d\n",recount[11]);
    printf("   Anti-sectors multiplicity OK         : % 6d\n",recount[12]);
    printf("   No spike-amplitudes in TOF           : % 6d\n",recount[13]);
    printf("   Primitive TOF-beta range OK          : % 6d\n",recount[14]);
    printf("   Particle(nonEmptyEnvelop) found      : % 6d\n",recount[15]);
    printf("   ........ with AnyTrack               : % 6d\n",recount[16]);
    printf("   ........ with !=0 pTRD               : % 6d\n",recount[17]);
    printf("   Track status: TRDtrack               : % 6d\n",recount[18]);
    printf("   Track status: ECALtrack              : % 6d\n",recount[19]);
    printf("   Track status: NOTrack                : % 6d\n",recount[20]);
    printf("   Track status: BadInterp              : % 6d\n",recount[21]);
    printf("   GoodTrackPart(intok,TRK|TRD)         : % 6d\n",recount[22]);
    printf("   TrkTrack Particle                    : % 6d\n",recount[23]);
    printf("   GoodTrkTrack found                   : % 6d\n",recount[24]);
    printf("   Event with GoodTrPart                : % 6d\n",recount[25]);
    printf("   Event with GoodTrkTrack              : % 6d\n",recount[26]);
    printf("   Event with suitable Track-type       : % 6d\n",recount[27]);
    printf("   Event OK for Tdelv-calib(1D-match OK): % 6d\n",recount[28]);
    printf("   TOF-Track 2D-matching OK             : % 6d\n",recount[29]);
    printf("   TOF beta-fit Chi2 and Beta-range OK  : % 6d\n",recount[30]);
    printf("   Ion event(not good for abs A-norm)   : % 6d\n",recount[31]);
    printf("   Entries to Tzslw-calib section       : % 6d\n",recount[32]);
    printf("       Momentum within TZSL-limits      : % 6d\n",recount[33]);
    printf("       Counters configuration OK        : % 6d\n",recount[34]);
    printf("       Identified as MIP(not Ion)       : % 6d\n",recount[35]);
    printf("   Entries to Ampl-calib(OK for A2D)    : % 6d\n",recount[36]);
    printf("       NonIon(TruncEdep), OK for fillam : % 6d\n",recount[37]);
    printf("       Mass**2 OK                       : % 6d\n",recount[38]);
    printf("       Bet*gam OK for fillabs           : % 6d\n",recount[39]);
    }
//    
    if(recount[45]>0){
    printf(" Entr to PEDS-calibr         : % 6d\n",recount[45]);
    }
//
    if(recount[46]>0){
    printf(" Entr to TDCL-calibr         : % 6d\n",recount[46]);
    }
  }
  else{
    printf(" TOFUser entries                      : % 6d\n",recount[21]);
    printf("   with 4Layer of RawClusters/lay=1   : % 6d\n",recount[30]);
    printf("   with 4Layer of Clusters/layer=1    : % 6d\n",recount[31]);
    printf("   Special TofPatt(4 beta<0 puzzle) OK: % 6d\n",recount[50]);
    printf("   NofAccClSectors Low                : % 6d\n",recount[22]);
    printf("   Events with nonemp Part-envelop    : % 6d\n",recount[24]);
    printf("   Particle with AnyTrack             : % 6d\n",recount[25]);
    printf("   ........ with !=0 pTRD             : % 6d\n",recount[42]);
    printf("   Track status: TRDtrack             : % 6d\n",recount[26]);
    printf("   Track status: ECALtrack            : % 6d\n",recount[27]);
    printf("   Track status: NOTTrack             : % 6d\n",recount[28]);
    printf("   Track status: BadInterp            : % 6d\n",recount[29]);
    printf("   GoodTrackPart(intok,TRK|TRD)       : % 6d\n",recount[40]);
    printf("   TrkTrack Particle                  : % 6d\n",recount[41]);
    printf("   GoodTrkTrack found                 : % 6d\n",recount[43]);
    printf("   Event with GoodTrackPart           : % 6d\n",recount[44]);
    printf("   Event with GoodTrkTrackPart        : % 6d\n",recount[45]);
    printf("   Finally AcceptedTrackPart          : % 6d\n",recount[49]);
    printf("   TrackParamsOK(Mom,Chi2,FalsX,..)   : % 6d\n",recount[46]);
    printf(" Including evs with ACC Cros&Fired    : % 6d\n",recount[48]);
    printf("              +TofClusters/layer OK   : % 6d\n",recount[23]);
    printf("   TOF-Track matching Good            : % 6d\n",recount[47]);
  }
  printf("\n\n");
//
  if(TFREFFKEY.reprtf[0]<2)return;//"More stat-prints" flag is off
//
//----------------------------------------------------------
//
  printf("===========> Channels validation report :\n\n");
//
  printf("Validation entries per channel:\n\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      printf(" % 6d",chcount[ic][12]);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      printf(" % 6d",chcount[ic][12]);
    }
    printf("\n\n");
  }
//
  printf("Missing FTtime info(fatal case)[%]  :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][13])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][13])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Multiple FTtime info[%]  :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][14])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][14])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Missing LTtime info(fatal case)[%] :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][15])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][15])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Missing Anode info[%] :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][16])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][16])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Missing Dynode when Anode is PUX-saturated[%] :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][17])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][17])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Missing History(SumHT) info [%] :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][18])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][18])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Missing LTtime info [%] when Anode(Q) OK:\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][19])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][19])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Missing Anode(Q) info [%] when LTtime OK:\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][20])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][20])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Both Anode(Q) and LTtime info present [%] :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][21])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][12]);
      if(rc>0.)rc=100.*geant(chcount[ic][21])/rc;
      printf("% 6.2f",rc);
    }
    printf("\n\n");
  }
//
//
//----------------------------------------------------------
  if(TFREFFKEY.relogic[0]==1)return;// no reco for tdclinearity-calibr
//---------------------------------------------------------
  printf("============> Channels reconstruction report :\n\n");
//
  printf("RawSide channel entries(FTtime is mandatory) :\n\n");
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
//--
  printf("FT-correlated LTtime-hits found :\n");
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
  printf("SumHTtime-hits found :\n");
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
  printf("LT(FT-correlated)/SumHT-matching Missing(%) ! :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][1]);
      if(rc>0.)rc=100*geant(chcount[ic][22])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][1]);
      if(rc>0.)rc=100*geant(chcount[ic][22])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Before-hits found (in front of TrueLT)(%)  ! :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][1]);
      if(rc>0.)rc=100*geant(chcount[ic][23])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][1]);
      if(rc>0.)rc=100*geant(chcount[ic][23])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("After-hits found (on back of TrueLT)(%)  ! :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][1]);
      if(rc>0.)rc=100*geant(chcount[ic][24])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][1]);
      if(rc>0.)rc=100*geant(chcount[ic][24])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//--
  printf("SumSHTtime-hits found :\n");
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
  printf("AnodeADC found :\n");
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
  printf("DynodeADC found :\n");
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
  printf("FTtdc has '1 hit' :\n");
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
  printf("Single FT-correlated LTtime-hit found :\n");
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
  printf("FT-correlated LTtime-hits AND (Anode|Dynode)-hits found :\n");
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
  printf("As above, but !=0 SumHT-channel is also required :\n");
  printf("\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][11])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR*2+ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][11])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Anode PUX-chip overflows :\n");
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
  printf("Dynode PUX-chip overflows(in >= 1pm/side) :\n");
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
//----------------------------------------------------------
//
  printf("==========> Bars reconstruction report :\n\n");
//
  printf("Bar entries with at least one ALIVE side(having [FTtime+smth] + DBok:\n\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR+ib;
      printf(" % 6d",brcount[ic][0]);
    }
    printf("\n\n");
  }
//
  printf("Bars fraction with at least 1 side with COMPLETE(LT & Q & [SumHT]) measurement:\n\n");
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
  printf("Bars fraction with 'HISTORY-OK' on COMPLETE sides:\n\n");
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
  printf("Bars fraction with 'bestLT-hit MATCHING'(if required)' on COMPLETE sides :\n\n");
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
  printf("Bar fraction with COMPLETE and HISTORY/MATCHING 'OK' measurements on BOTH sides:\n\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR+ib;
      rc=geant(brcount[ic][0]);
      if(rc>0.)rc=geant(brcount[ic][4])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//-------------------------------------------------------
//
  printf("==========> Bars TRK-matching report :\n\n");
//
  printf("Bar matching entries:\n\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR+ib;
      printf(" % 6d",brcount[ic][5]);
    }
    printf("\n\n");
  }
//
  printf("Bars fraction with Transversal matching OK:\n\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR+ib;
      rc=geant(brcount[ic][5]);
      if(rc>0.)rc=geant(brcount[ic][6])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
  printf("Bars fraction with Transv+Longitudinal matching OK:\n\n");
  for(il=0;il<TOF2GC::SCLRS;il++){
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      ic=il*TOF2GC::SCMXBR+ib;
      rc=geant(brcount[ic][5]);
      if(rc>0.)rc=geant(brcount[ic][7])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
  }
//
//
}
//------------------------------------------
void TOF2JobStat::bookhist(){
  int i,j,k,ich,il,ib,ii,jj,ic,is,id;
  char htit1[80];
  char idname[80];
  char inum[11];
  char in[2]="0";
  int thprtf=TFREFFKEY.reprtf[1];
//
  strcpy(inum,"0123456789");
// hist 1290-1299 are used in trigger102.C
//
  if(LVL3FFKEY.histprf>0){
    HBOOK1(1020,"LVL1: TOF  Ttop-Tbot",50,-12.5,12.5,0.);
//   1020-1029 Lev3 hists
  }
//
  if(thprtf>0){// Reconstruction histograms
// Book reco-hist
    HBOOK1(1100,"RawCluster: LT-SumHT time (1-hit in LT and sumHT, all channels)",80,-6.,14.,0.);
    HBOOK1(1107,"TofValid: TOF+ACC data length (16-bit words)",100,1.,1001.,0.);
    HBOOK1(1101,"Time_history:befor_hit dist(ns)",80,0.,2400.,0.);
    HBOOK1(1102,"Time_history:after_hit dist(ns)",80,0.,400.,0.);
//      HBOOK1(1116,"FTtime-diff,slot-by-slot",80,-10.,10.,0.);
    HBOOK1(1104,"RawCluster:Anode signals(adc,id=104,s1)",100,0.,1000.,0.);
    HBOOK1(1117,"RawCluster:Anode signals(adc,id=104,s2)",100,0.,1000.,0.);
    HBOOK1(1105,"RawCluster:Dynode(pm) signals(adc,id=104,s1)",100,0.,200.,0.);
    HBOOK1(1108,"RawCluster:SumHTtime hits multiplicity(all chan)",20,0.,20.,0.);
    HBOOK1(1110,"RawCluster:Total fired layers per event",5,0.,5.,0.);
    HBOOK1(1111,"RawCluster:Layer appearence frequency",4,0.5,4.5,0.);
    HBOOK1(1112,"RawCluster:Layer-Config(-1/0/1.../5--> <2/2/missLay#/All4)",10,-1.,9.,0.);
    HBOOK1(1113,"RawCluster:OneBarPerLayer Layer-Config(<2/2/missLay/All4)",10,-1.,9.,0.);
    HBOOK1(1114,"RawCluster:One2SidedBarPerLayer Layer-Config(<2/2/missLay/All4)",10,-1.,9.,0.);
    HBOOK1(1115,"RawCluster: BestMatch LT-SumHT time(all FTmatched LT-hits, ovfl=noSumHTmatch)",80,-6.,14.,0.);
    HBOOK1(1118,"RawCluster:Side-1 time (allchan, noAcorr)",100,0.,400.,0.);
    HBOOK1(1119,"RawCluster:Side-2 time (allchan, noAcorr)",100,0.,400.,0.);
//
    if(TFREFFKEY.reprtf[4]>0){//more histograms
      HBOOK1(1120,"TofRawClBuild: LTime-SumHTtime(1-Hit case), L1S1",80,-6.,14.,0.);
      HBOOK1(1121,"TofRawClBuild: LTime-SumHTtime(1-Hit case), L1S2",80,-6.,14.,0.);
      HBOOK1(1122,"TofRawClBuild: LTime-SumHTtime(1-Hit case), L2S1",80,-6.,14.,0.);
      HBOOK1(1123,"TofRawClBuild: LTime-SumHTtime(1-Hit case), L2S2",80,-6.,14.,0.);
      HBOOK1(1124,"TofRawClBuild: LTime-SumHTtime(1-Hit case), L3S1",80,-6.,14.,0.);
      HBOOK1(1125,"TofRawClBuild: LTime-SumHTtime(1-Hit case), L3S2",80,-6.,14.,0.);
      HBOOK1(1126,"TofRawClBuild: LTime-SumHTtime(1-Hit case), L4S1",80,-6.,14.,0.);
      HBOOK1(1127,"TofRawClBuild: LTime-SumHTtime(1-Hit case), L4S2",80,-6.,14.,0.);
    }
//
    if(TFREFFKEY.reprtf[4]>0){
      HBOOK1(1128,"TofValid:FTtime-LTime(1-Hits case), L1S1",80,-60.,740.,0.);
      HBOOK1(1129,"TofValid:FTtime-LTime(1-Hits case), L1S2",80,-60.,740.,0.);
      HBOOK1(1130,"TofValid:FTtime-LTime(1-Hits case), L2S1",80,-60.,740.,0.);
      HBOOK1(1131,"TofValid:FTtime-LTime(1-Hits case), L2S2",80,-60.,740.,0.);
      HBOOK1(1132,"TofValid:FTtime-LTime(1-Hits case), L3S1",80,-60.,740.,0.);
      HBOOK1(1133,"TofValid:FTtime-LTime(1-Hits case), L3S2",80,-60.,740.,0.);
      HBOOK1(1134,"TofValid:FTtime-LTime(1-Hits case), L4S1",80,-60.,740.,0.);
      HBOOK1(1135,"TofValid:FTtime-LTime(1-Hits case), L4S2",80,-60.,740.,0.);
      HBOOK1(1137,"TofValid:FTime-LTtime for LBBS=1042(all its LT-hits)",70,-60.,640.,0.);
      HBOOK1(1138,"TofValid:LTtime for LBBS=1042",100,5000.,20000.,0.);
      HBOOK1(1139,"TofValid:FTtime for LBBS=1042",100,5000.,20000.,0.);
    }
    HBOOK1(1136,"RawCluster: FTtime-LTime(all LT-hits, 1st FT-hit if multiple)",80,-60.,740.,0.);
    HBOOK1(1106,"RawCluster: FTtime-LTime(finaly selected LT-hit)",80,-60.,740.,0.);
    HBOOK1(1103,"RawCluster: LTime-SumHTtime(final LT-hit(Single LT case, FT-matched),ovfl=noSumHTmatch)",80,-6.,14.,0.);
    HBOOK1(1109,"RawCluster: LTime-SumHTtime(final LT-hit(best LT of all FT-matched),ovfl=noSumHTmatch)",80,-6.,14.,0.);
//---    
//(hist 1140-1161)
    if(TFREFFKEY.reprtf[4]>0){
      for(int btyp=1;btyp<(TOF2GC::SCBTPN+1);btyp++){
        for(is=0;is<2;is++){
          id=TofTmAmCalib::btyp2id(btyp);
	  id=id*10+is+1;
          strcpy(htit1,"TofValid: Raw Amplitude for RefBarTyp:");
          sprintf(idname,"%d",btyp);
          strcat(htit1,idname);
          strcat(htit1,",LBBS=");
          sprintf(idname,"%d",id);
          strcat(htit1,idname);
          HBOOK1(1140+btyp-1+TOF2GC::SCBTPN*is,htit1,80,0.,320.,0.);
        }
      }
    }
    
//    HBOOK1(1092-1099 are used for trigger-hists in trigger102.C  !!!!)
//    HBOOK1(1010,...    reserved for tofsim02.C internal use !!!!
//    HBOOK1(1011,...    reserved for tofsim02.C internal use !!!!
    if(TFREFFKEY.reprtf[4]>0){
      HBOOK1(1526,"RawCluster:L=1,Eanode(mev),poscorrected,1b/lay evnt",80,0.,24.,0.);
      HBOOK1(1527,"RawCluster:L=3,Eanode(mev),poscorrected,1b/lay evnt",80,0.,24.,0.);
      HBOOK1(1528,"RawCluster:L=1,Edynode(mev),poscorrected,1b/lay evnt",80,0.,24.,0.);
      HBOOK1(1529,"RawCluster:L=3,Edynode(mev),poscorrected,1b/lay evnt",80,0.,24.,0.);
// spare     HBOOK1(1530,"L=1,Ed_dynodeL(mev),pcorr,1b/lay evnt",80,0.,24.,0.);
// spare     HBOOK1(1531,"L=3,Ed_dynodeL(mev),pcorr,1b/lay evnt",80,0.,24.,0.);
//(hist 1165-1198)
      ich=0;
      for(il=0;il<TOF2DBc::getnplns();il++){
        for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
          id=100*(il+1)+ib+1;
          strcpy(htit1,"RawCluster:Anode Edep (poscorrected,norm.inc) for bar(LBB):");
          sprintf(idname,"%d",id);
          strcat(htit1,idname);
          HBOOK1(1165+ich+ib,htit1,50,0.,15.,0.);
        }
        ich+=TOF2DBc::getbppl(il);
      } // --- end of layer loop --->
    }
//---
    HBOOK1(1532,"T1-T3(ns,NormIncidence,1b/L evnt)",80,1.,9.,0.);
    HBOOK1(1534,"T2-T4(ns,NormIncidence,1b/L evnt)",80,1.,9.,0.);
    HBOOK1(1544,"(T1-T3)-(T2-T4),(ns,(1RawClust/Lay)*4 events)",80,-4.,4.,0.);
    HBOOK1(1535,"L=1,TOFClus Edep(mev)",80,0.,24.,0.);
    HBOOK1(1533,"L=1/B4,TOFClus Edep(mev,Nmemb=1)",80,0.,24.,0.);
    HBOOK1(1536,"L=3,TOFClus Edep(mev)",80,0.,24.,0.);
    HBOOK1(1537,"L=1,TOFClus Edep(10Xscaled,mev)",80,0.,240.,0.);
    HBOOK1(1538,"L=3,TOFClus Edep(10Xscaled,mev)",80,0.,240.,0.);
    HBOOK1(1539,"L=2,TOFClus Edep(mev)",80,0.,24.,0.);
    HBOOK1(1540,"L=4,TOFClus Edep(mev)",80,0.,24.,0.);
    HBOOK1(1541,"TOFClus: L1XCoo(long)",100,-50.,50.,0.);
    HBOOK1(1542,"TOFClus: L1YCoo(tran)",100,-50.,50.,0.);
    HBOOK1(1545,"L=1/B5,TOFClus Edep(mev,Nmemb=1)",80,0.,24.,0.);
    HBOOK1(1546,"L=1/B2,TOFClus Edep(mev,Nmemb=1)",80,0.,24.,0.);
    HBOOK1(1548,"TOFClus: 2bars-cand, E-ass(dt,dc ok)",44,-1.1,1.1,0.);
    HBOOK1(1549,"TOFClus: 2bars-cand. LongMatch(cm, Tmatch ok)",80,-40.,40.,0.);
    HBOOK1(1550,"TOFClus: 2bars-cand. T-match(ns)",80,-10.,10.,0.);
//
    if(TFREFFKEY.reprtf[4]>0){ // <==================== TOF-debug
      for(il=0;il<TOF2GC::SCLRS;il++){// RawADC histogr
        for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
          for(is=0;is<2;is++){
            strcpy(htit1,"Raw AnodeADC for chan(LBBS):");
	    id=1000*(il+1)+10*(ib+1)+is+1;
	    sprintf(idname,"%d",id);
	    strcat(htit1,idname);
	    ich=2*TOF2DBc::barseqn(il,ib)+is;//0-67
	    HBOOK1(1400+ich,htit1,50,0.,250.,0.);
//	    HBOOK2(1400+ich,htit1,50,0.,400.,50,0.,100.,0.);
	  }
        }
      }
    }
//
    HBOOK1(1470,"A>Athr Paddles/Side/Layer visibility when TrPattBit On",80,1.,81.,0.);
    HBOOK1(1471,"A>Athr Paddles/Side/Layer visibility when TrPattBit Off",80,1.,81.,0.);
    HBOOK1(1472,"A>Athr,TimeHits>0 Paddles/Side/Layer visibility when TrPattBit Off",80,1.,81.,0.);
    HBOOK1(1473,"A>Athr Paddles/Side/Layer visibility when BZTrPattBit On",80,1.,81.,0.);
    HBOOK1(1474,"A>Athr Paddles/Side/Layer visibility when BZPattBit Off",80,1.,81.,0.);
    HBOOK1(1475,"A>Athr,TimeHits>0 Paddles/Side/Layer visibility when BZPattBit Off",80,1.,81.,0.);
//
    if(TFREFFKEY.relogic[0]>=5){// <==================== TofPed-calibr. runs
//   hist. 1790-1999 are booked in init.function
    }
    if(TFREFFKEY.relogic[0]==1){// <==================== TofTdc-calibr.(TDCL) runs
//   hist. 1600-> 1800 are booked in init.function
    }
    if(TFREFFKEY.relogic[0]==2 || TFREFFKEY.relogic[0]==3
                               || TFREFFKEY.relogic[0]==4
                                                        ){// <======== TDIF/TZSL/AMPL-calibr. runs
//   hist 1600-1644 are booked at init-stage  for TDIF
//   hist 1515-1518 are booked at init-stage  for TZSL
//   hist 1500-1511, 1200-1219 are booked at init-stage as common
    }
//
  }
  if(TFREFFKEY.reprtf[2]>0){//<======================Indep.Validation-stage multiplicity histograms
    for(il=0;il<TOF2GC::SCLRS;il++){
      for(ib=0;ib<TOF2GC::SCMXBR;ib++){
	for(i=0;i<2;i++){
	  strcpy(htit1,"Validation::FTtdc/LTtdc/Aadc/Dadc/SumHT/SumSHT multipl. for chan(LBBS) ");
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
	  HBOOK1(1300+ich,htit1,100,0.,100.,0.);
	}
      }
    }
  }
}
//-----------------------------
void TOF2JobStat::bookhistmc(){
    if(TFMCFFKEY.mcprtf[2]!=0){ // Book mc-hist
//    HBOOK1(1010,...    reserved for tofsim02.C internal use !!!!
//    HBOOK1(1011,...    reserved for tofsim02.C internal use !!!!
//    HBOOK1(1020-29,...    reserved for Lev3 !!!!
      HBOOK1(1050,"SIMU: GHits in layer-1",50,0.,50.,0.);
      HBOOK1(1051,"SIMU: GHits in layer-2",50,0.,50.,0.);
      HBOOK1(1052,"SIMU: GHits in layer-3",50,0.,50.,0.);
      HBOOK1(1053,"SIMU: GHits in layer-4",50,0.,50.,0.);
      HBOOK1(1060,"SIMU: GHit Edeps(mev) in layer-1",80,0.,24.,0.);
      HBOOK1(1061,"SIMU: GHit Edeps(mev) in layer-2",80,0.,24.,0.);
      HBOOK1(1062,"SIMU: GHit Edeps(mev) in layer-3",80,0.,24.,0.);
      HBOOK1(1063,"SIMU: GHit Edeps(mev) in layer-4",80,0.,24.,0.);
      HBOOK1(1070,"SIMU: PM-1 charge(pC,id=104,s1)",80,0.,400.,0.);
      HBOOK1(1071,"SIMU: Number of HT(SHT+10)-trigger pulses(SPT-inp)",20,0.,20.,0.);
      HBOOK1(1072,"SIMU: HT-trig pulse width(SPT-inp, ns)",80,200.,600.,0.);
      HBOOK1(1073,"SIMU: SHT-trig pulse width(SPT-inp, ns)",80,200.,600.,0.);
      HBOOK1(1074,"SIMU: Anode-adc(id=104,s1,NoPeds)",100,0.,500.,0.);
      HBOOK1(1075,"SIMU: Dynode-adc(eq.sum/npm, id=104,s1,NoPeds)",100,0.,100.,0.);
      HBOOK1(1065,"SIMU: FTCTrigPat(HT):S1-frequence(L=1,4)",80,0.,80.,0.);
      HBOOK1(1066,"SIMU: FTCTrigPat(HT):S2-frequence(L=1,4)",80,0.,80.,0.);
      HBOOK1(1067,"SIMU: BZTrigPat(SHT):S1-frequence(L=1,4)",80,0.,80.,0.);
      HBOOK1(1068,"SIMU: BZTrigPat(SHT):S2-frequence(L=1,4)",80,0.,80.,0.);
      HBOOK1(1069,"SIMU: TofFtCodes(/0-14/+20/+40/+60->FTC/FTClatched/BZ-inp/BZ-accepted)",80,0.,80.,0.);
      HBOOK1(1076,"SIMU: ECTrigFlag when TOFTrflag OK",40,0.,40.,0.);
      HBOOK1(1077,"SIMU: TOFFTTime-ECFTTime(when FTC&FTE)",80,-80.,80.,0.);
      HBOOK1(1078,"SIMU: Out-of-width-hit X-excess",50,0.,5.,0.);
      HBOOK1(1079,"SIMU: Out-of-thickness-hit Z-excess",50,0.,5.,0.);
      HBOOK1(1080,"SIMU: GHitTime",100,0.,1000.,0.);
      HBOOK1(1081,"SIMU: Anode pulse-hight(mV,id=101(bt1),s1)",80,0.,1600.,0.);
      HBOOK1(1082,"SIMU: Anode pulse-hight(mV,id=104(bt2),s1)",80,0.,1600.,0.);
      HBOOK1(1083,"SIMU: Anode pulse-hight(mV,id=401(bt3),s1)",80,0.,1600.,0.);
      HBOOK1(1084,"SIMU: Anode pulse-hight(mV,id=404(bt4),s1)",80,0.,1600.,0.);
      HBOOK1(1085,"SIMU: Anode pulse-hight(mV,id=201(bt5),s1)",80,0.,1600.,0.);
      HBOOK1(1086,"SIMU: Anode pulse-hight(mV,id=202(bt6),s1)",80,0.,1600.,0.);
      HBOOK1(1087,"SIMU: Anode pulse-hight(mV,id=204(bt7),s1)",80,0.,1600.,0.);
      HBOOK1(1088,"SIMU: Anode pulse-hight(mV,id=301(bt8),s1)",80,0.,1600.,0.);
      HBOOK1(1089,"SIMU: Anode pulse-hight(mV,id=302(bt9),s1)",80,0.,1600.,0.);
      HBOOK1(1090,"SIMU: Anode pulse-hight(mV,id=303(bt10),s1)",80,0.,1600.,0.);
      HBOOK1(1091,"SIMU: Anode pulse-hight(mV,id=305(bt11),s1)",80,0.,1600.,0.);
      HBOOK1(1031,"SIMU: Nphelectrons/side(id=101(bt1),s1)",80,0.,1600.,0.);
      HBOOK1(1032,"SIMU: Nphelectrons/side(id=104(bt2),s1)",80,0.,3200.,0.);
      HBOOK1(1033,"SIMU: Nphelectrons/side(id=401(bt3),s1)",80,0.,1600.,0.);
      HBOOK1(1034,"SIMU: Nphelectrons/side(id=404(bt4),s1)",80,0.,1600.,0.);
      HBOOK1(1035,"SIMU: Nphelectrons/side(id=201(bt5),s1)",80,0.,1200.,0.);
      HBOOK1(1036,"SIMU: Nphelectrons/side(id=202(bt6),s1)",80,0.,1200.,0.);
      HBOOK1(1037,"SIMU: Nphelectrons/side(id=204(bt7),s1)",80,0.,1200.,0.);
      HBOOK1(1038,"SIMU: Nphelectrons/side(id=301(bt8),s1)",80,0.,1200.,0.);
      HBOOK1(1039,"SIMU: Nphelectrons/side(id=302(bt9),s1)",80,0.,1200.,0.);
      HBOOK1(1040,"SIMU: Nphelectrons/side(id=303(bt10),s1)",80,0.,1200.,0.);
      HBOOK1(1041,"SIMU: Nphelectrons/side(id=305(bt11),s1)",80,0.,1200.,0.);
      HBOOK1(1042,"SIMU: Npe1-Npe2/Npe1+Npe2(id=104(bt2))",80,-0.6,0.6,0.);
      HBOOK1(1043,"SIMU: Nphelectrons/counter(id=104(bt2))",80,0.,6400.,0.);
    }
}
//----------------------------
void TOF2JobStat::outp(){
  int i,j,k,ich,il,ib,is;
  geant dedx[TOF2GC::SCMXBR],dedxe[TOF2GC::SCMXBR];
  char chopt[6]="qb";
  char chfun[6]="g";
  char chopt1[5]="LOGY";
  geant par[3],step[3],pmin[3],pmax[3],sigp[3],chi2;
//---
  if(LVL3FFKEY.histprf>0){
    HPRINT(1020);
  }
//---
  if(TFREFFKEY.reprtf[1]>0){ // print RECO-hists
    HPRINT(1107);
    HPRINT(1535);
    HPRINT(1533);
    HPRINT(1545);
    HPRINT(1546);
    HPRINT(1536);
    HPRINT(1537);
    HPRINT(1538);
    HPRINT(1541);
    HPRINT(1542);
    HPRINT(1539);
    HPRINT(1540);
    HPRINT(1548);
    HPRINT(1549);
    HPRINT(1550);
	 
//    HPRINT(1103);
    HPRINT(1108);
    HPRINT(1104);
    HPRINT(1117);
    HPRINT(1105);
    HPRINT(1110);
    HPRINT(1111);
    HPRINT(1112);
    HPRINT(1113);
    HPRINT(1114);
    HPRINT(1118);
    HPRINT(1119);
    
    if(TFREFFKEY.reprtf[4]>0){
      for(int btyp=1;btyp<(TOF2GC::SCBTPN+1);btyp++){
        for(is=0;is<2;is++){
          HPRINT(1140+btyp-1+TOF2GC::SCBTPN*is);
        }
      }
    }
    
    if(TFREFFKEY.reprtf[4]>0){
      for(i=0;i<8;i++)HPRINT(1128+i);//LT-FT times(Layer/Side)
      HPRINT(1137);// ... id=1041
      HPRINT(1138);
      HPRINT(1139);
    }
//
    HPRINT(1136);//ft-lt in RawClustBuild
    HPRINT(1100);
    if(TFREFFKEY.reprtf[4]>0){
      for(i=0;i<8;i++)HPRINT(1120+i);//LT-SumHT times(Layer/Side)
    }
    HPRINT(1115);
	 
    HPRINT(1106);// final FT-LT for rawClust
    HPRINT(1103);// final LT-sumHT(singe FT-matched) for rawClust
    HPRINT(1109);// final LT-sumHT (best fromFT-matched) for rawClust
    HPRINT(1101);
    HPRINT(1102);
//
    if(TFREFFKEY.reprtf[4]>0){
      HPRINT(1526);
      HPRINT(1527);
      HPRINT(1528);
      HPRINT(1529);
// spare  HPRINT(1530);
//        HPRINT(1531);
      ich=0;
      for(il=0;il<TOF2DBc::getnplns();il++){
        for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
          HPRINT(1165+ich+ib);
        }
        ich+=TOF2DBc::getbppl(il);
      } // --- end of layer loop --->
     }
//
     HPRINT(1532);
     
     par[0]=200.;
     par[1]=0.;
     par[2]=0.3;
     step[0]=50.;
     step[1]=0.1;
     step[2]=0.05;
     pmin[0]=10.;
     pmin[1]=-2.;
     pmin[2]=0.05;
     pmax[0]=10000.;
     pmax[1]=2.;
     pmax[2]=0.5;
     HFITHN(1544,chfun,chopt,3,par,step,pmin,pmax,sigp,chi2);
     cout<<endl<<endl;
     cout<<" TOFRawClust:(T13-T24)-fit: Mp/rms="<<par[1]<<" "<<par[2]<<" chi2="<<chi2<<endl;
     HPRINT(1544);
     
     HPRINT(1534);
//---
     if(TFREFFKEY.reprtf[4]>0){
       for(il=0;il<TOF2GC::SCLRS;il++){// RawADC histogr
         for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
           for(is=0;is<2;is++){
	     ich=2*TOF2DBc::barseqn(il,ib)+is;//0-67
	     HPRINT(1400+ich);
	   }
         }
       }
     }
//
     HPRINT(1470);
     HPRINT(1471);
     HPRINT(1472);
     HPRINT(1473);
     HPRINT(1474);
     HPRINT(1475);
//--- 
   }//--->endof RECO-hist print check
//
   if(TFREFFKEY.reprtf[2]>0){//independent TDC-hit multiplicity histograms
     for(i=0;i<TOF2GC::SCLRS;i++){
       for(j=0;j<TOF2GC::SCMXBR;j++){
         for(k=0;k<2;k++){
           ich=2*TOF2GC::SCMXBR*i+2*j+k;
           HPRINT(1300+ich);
         }
       }
     }
   }
//---------------------------------------------------
// ---> calibration specific :
//
   if(TFREFFKEY.relogic[0]==1){// for TDCL-calibr. runs
//           TOFTdcCalib::outp(0);
   }
//
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
	 HPRINT(1031);
	 HPRINT(1032);
	 HPRINT(1043);
	 HPRINT(1033);
	 HPRINT(1034);
	 HPRINT(1035);
	 HPRINT(1036);
	 HPRINT(1037);
	 HPRINT(1038);
	 HPRINT(1039);
	 HPRINT(1040);
	 HPRINT(1041);
	 HPRINT(1042);
         HPRINT(1081);
         HPRINT(1082);
         HPRINT(1083);
         HPRINT(1084);
         HPRINT(1085);
         HPRINT(1086);
         HPRINT(1087);
         HPRINT(1088);
         HPRINT(1089);
         HPRINT(1090);
         HPRINT(1091);
//         HPRINT(1054);
         HPRINT(1075);
//         HPRINT(1064);
         HPRINT(1065);
         HPRINT(1066);
         HPRINT(1067);
         HPRINT(1068);
         HPRINT(1069);
         HPRINT(1076);
         HPRINT(1077);
         HPRINT(1078);
         HPRINT(1079);
         HPRINT(1080);
//         if(TFCAFFKEY.mcainc)TOF2Tovt::aintfit();
       }
}
//==========================================================================
int TofSlowTemp::gettempC(int crat, int slot, geant & atemp){
//crat=0-3,slot=0-10
//make average of 2 SFEC-sensors corresponding to my slots 8,9 or 10,11
// return 1/0->ok/fail
  #ifdef __AMSDEBUG__
    if(TOF2DBc::debug){
        assert(crat>=0 && crat<=3);
        assert(slot==7 || slot==8 || slot==9 || slot==10);
    }
  #endif
  geant temp;
  atemp=0;
  int sensid,sid1(0),sid2(0),cr,sl,stat,nmem(0);
  cr=crat+1;
  sl=slot+1;
  sid1=cr*100+sl;
  if(sl==8 || sl==10)sid2=cr*100+sl+1;
  if(sl==9 || sl==11)sid2=cr*100+sl-1;
  for(int l=0;l<TOF2GC::SCLRS;l++){
    for(int c=0;c<2;c++){
      for(int n=0;n<8;n++){
        sensid=AMSSCIds::getenvsensid(l,c,n);
	if(sensid>411)continue;//look for CSS
	temp=_stemp[l][n+8*c];
	stat=_sta[l][n+8*c];
	if((sensid==sid1 || sensid==sid2) && stat==1){
	  nmem+=1;
	  atemp+=temp;
	}
      }
    } 
  }
  if(nmem>0){
    atemp/=geant(nmem);
    return 1;
  }
  else return 0; 
}
//---------------------------
int TofSlowTemp::gettempP(int lay, int sid, geant & atemp){
//make average over all sensors in L**S (max 2*3)
// return 1/0->ok/fail
  #ifdef __AMSDEBUG__
    if(TOF2DBc::debug){
        assert(lay>=0 && lay<TOF2GC::SCLRS);
	assert(sid>=0 && sid<=1);
    }
  #endif
  geant temp;
  atemp=0;
  int sensid,ssid,sd,stat,nmem(0);
  sd=sid+1;
//
  for(int c=0;c<2;c++){
    for(int n=0;n<8;n++){
      sensid=AMSSCIds::getenvsensid(lay,c,n);
      if(sensid<10111)continue;//look for LBBSP
      ssid=(sensid%100)/10;//side
      temp=_stemp[lay][n+8*c];
      stat=_sta[lay][n+8*c];
      if(ssid==sd && stat==1){
	nmem+=1;
	atemp+=temp;
      }
    }
  }
// 
  if(nmem>0){
    atemp/=geant(nmem);
    return 1;
  }
  else return 0; 
}
//---------------------------
void TofSlowTemp::init(){
//set defs for temp/stat
  int id;
  for(int l=0;l<TOF2GC::SCLRS;l++){
    for(int c=0;c<2;c++){
      for(int n=0;n<8;n++){
        id=AMSSCIds::getenvsensid(l,c,n);//CSS or LBBSP
        if(id<500){//CSS
	  _stemp[l][n+8*c]=999;//undefined val for TempC
	}
	else{//LBBSPM
	  _stemp[l][n+8*c]=999;//undefined val for TempP
	}
	_sta[l][n+8*c]=1;//sensor ok
      }
    } 
  }
  
}
//==========================================================================
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
    for(int ie=0;ie<160;ie++)daqsf[ie]=0;
//    
    for(int ie=0;ie<30;ie++){
      for(i=0;i<TOF2GC::SCCRAT;i++){
        cratr[i][ie]=0;
        cratp[i][ie]=0;
        cratc[i][ie]=0;
        cratm[i][ie]=0;
      }
    }
    for(int ie=0;ie<20;ie++){
      for(i=0;i<TOF2GC::SCCRAT;i++){
        for(j=0;j<TOF2GC::SCSLTM;j++){
	  sltr[i][j][ie]=0;
	  sltp[i][j][ie]=0;
          for(int ic=0;ic<TOF2GC::SCRCHM;ic++){
	    if(i==0){
	      rdcr1[j][ic][ie]=0;
	      rdcp1[j][ic][ie]=0;
	    }
	    if(i==1){
	      rdcr2[j][ic][ie]=0;
	      rdcp2[j][ic][ie]=0;
	    }
	    if(i==2){
	      rdcr3[j][ic][ie]=0;
	      rdcp3[j][ic][ie]=0;
	    }
	    if(i==3){
	      rdcr4[j][ic][ie]=0;
	      rdcp4[j][ic][ie]=0;
	    }
	  }
	}
      }
    }
//for slow temp:
    for(int ie=0;ie<40;ie++)tofstc[ie]=0;
//for calib-quality:
    for(i=0;i<4;i++)
                   for(j=0;j<5;j++)cquality[i][j]=0;
//formal init of SFET(A)-sensors by undefined value
    for(i=0;i<TOF2GC::SCCRAT;i++)
      for(j=0;j<TOF2GC::SCFETA;j++)tofantemp[i][j]=999;//(TempT,degrees)
  }
//---
  void TOF2JobStat::resettemp(){//for event-by-event option
    int i,j;
    for(i=0;i<TOF2GC::SCCRAT;i++)
      for(j=0;j<TOF2GC::SCFETA;j++)tofantemp[i][j]=999;//(TempT,degrees)
  }
//--------------------------------------------------
TofElosPDF::TofElosPDF(int ich, int ch, geant bp, int nb, geant stp, geant bnl, geant undf, geant ovfl, geant distr[]){
  ichar=ich;
  charge=ch;
  bpow=bp;
  nbins=nb;
  stpx=stp;
  xmin=bnl;
//cout<<"iZ/Z="<<ichar<<" "<<charge<<"  beta_pow="<<bpow<<endl;
//cout<<"Nbns/binw/xmin="<<nbins<<" "<<stpx<<" "<<xmin<<endl;
//cout<<"Undfl/Ovfl="<<undf<<" "<<ovfl<<" dis(0)/dis(n-1)="<<distr[0]<<" "<<distr[nb-1]<<endl;
  number norm(0),suml,sumr,hend;
  for(int i=0;i<nb;i++){
    elpdf[i]=number(distr[i]);
    norm+=elpdf[i];
  }
  suml=number(undf);
  if(suml==0)suml+=1;
  sumr=number(ovfl);
  norm+=(suml+sumr);
//  cout<<"suml/sumr="<<suml<<" "<<sumr<<endl;
  if(sumr==0 || elpdf[nb-1]==0){
    cout<<"TofElosPDF::-E-:Original distribition high-end is badly defined !"<<" iZ="<<ich<<endl;
    exit(1);
  }
  hend=elpdf[nbins-1];
  slope=hend/sumr/stpx;//exp.slope in ovfl-region
  unpdf=suml/xmin/norm;//const.PDF-value in underfl-region(already normalized correctly !!!)
  for(int i=0;i<nb;i++)elpdf[i]/=(norm*stpx);//normalized PDF in central region
}
//---
void TofElosPDF::build(){// create TofElosPDF-objects array for real/mc data
//
  char fname[80];
  char name[80];
  int ctyp,ntypes;
  integer ic,charge,chref,nbns,endflab;
  geant betap,lovfl,rovfl,xmin,binw,distr[TOF2GC::SCPDFBM];
//
// ---> read file with the list of version numbers for all needed calib.files :
//
  char datt[3];
  char ext[80];
  int date[2],year,mon,day,hour,min,sec;
  uinteger iutct;
  tm begin;
  time_t utct;
  uinteger verids[10],verid;
//
  strcpy(name,"TofCflist");// basic name for vers.list-file  
  if(AMSJob::gethead()->isMCData()){
    strcpy(datt,"MC");
    sprintf(ext,"%d",TFMCFFKEY.calvern);//MC-versn
    ctyp=6;
  }
  else{
    strcpy(datt,"RD");
    sprintf(ext,"%d",TFREFFKEY.calutc);//RD-utc
    ctyp=5;// TofSfmap-file is missing in RD list-file
  }
  strcat(name,datt);
  strcat(name,".");
  strcat(name,ext);
//
  if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"====> TofElosPDF::build: Opening Calib_vers_list-file "<<fname<<'\n';
  ifstream vlfile(fname,ios::in);
  if(!vlfile){
    cout <<"<---- Error: missing vers.list-file !!? "<<fname<<endl;
    exit(1);
  }
  vlfile >> ntypes;// total number of calibr. file types in the list
  for(int i=0;i<ntypes;i++){
    vlfile >> verids[i];// 
  }
  if(AMSJob::gethead()->isMCData()){
    vlfile >> date[0];//YYYYMMDD beg.validity of TofCflistMC.ext file
    vlfile >> date[1];//HHMMSS ......................................
    year=date[0]/10000;//2004->
    mon=(date[0]%10000)/100;//1-12
    day=(date[0]%100);//1-31
    hour=date[1]/10000;//0-23
    min=(date[1]%10000)/100;//0-59
    sec=(date[1]%100);//0-59
    begin.tm_isdst=0;
    begin.tm_sec=sec;
    begin.tm_min=min;
    begin.tm_hour=hour;
    begin.tm_mday=day;
    begin.tm_mon=mon-1;
    begin.tm_year=year-1900;
    utct=mktime(& begin);
    iutct=uinteger(utct);
    cout<<"      TofCflistMC-file begin_date: year:month:day = "<<year<<":"<<mon<<":"<<day<<endl;
    cout<<"                                     hour:min:sec = "<<hour<<":"<<min<<":"<<sec<<endl;
    cout<<"                                         UTC-time = "<<iutct<<endl;
  }
  else{
    utct=time_t(TFREFFKEY.calutc);
    printf("      TofCflistRD-file begin_date: %s",ctime(&utct)); 
  }
  vlfile.close();
//------------------------------------------------
  verid=verids[ctyp-1];//MC-versn or RD-utc
  strcpy(name,"TofElosp");//generic particles charge-calib. file-name
  strcat(name,datt);
  strcat(name,".");
  sprintf(ext,"%d",verid);//got TofCflistMC(RD). file extention
  strcat(name,ext);
  if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Opening TofElosPDF-file : "<<fname<<'\n';
  ifstream pdfile(fname,ios::in); // open file for reading
  if(!pdfile){
    cout <<"<---- Error: Missing TofElosPDF-file !!? "<<fname<<endl;
    exit(1);
  }
//
  for(int ich=0;ich<MaxZTypes;ich++){
    pdfile >> ic;
    pdfile >> charge;
    chref=AMSCharge::ind2charge(0,ich+1);//charge from ctandard list("0"->TOF)
    if(charge!=chref || ic!=(ich+1)){
      cout<<"<---- Error: wrong Z-structure of TofElosPDF-file for id="<<ic<<endl;
      exit(1);
    }
    pdfile >> betap;
    pdfile >> nbns;
    pdfile >> lovfl;
    pdfile >> rovfl;
    pdfile >> xmin;
    pdfile >> binw;
    for(int ib=0;ib<nbns;ib++)pdfile >> distr[ib];
    TofEPDFs[ich]=TofElosPDF(ic, charge, betap, nbns, binw, xmin, lovfl, rovfl, distr);
  }
//
  pdfile >> endflab;//read endfile-label
//
  pdfile.close();
//
  if(endflab==12345){
    cout<<"<---- TofElosPDF-file is successfully read !"<<endl<<endl;
  }
  else{
    cout<<"<---- Error: problem with TofElosPDF-file !!?"<<endl<<endl;
    exit(1);
  }
//
}
//---
number TofElosPDF::getlkhd(int nhits, int hstat[], number ehit[], number beta){
  number eh,xmax,hend,betapow,betacor,betamax(0.94),lkhd(0);
  int ia;
  betapow=bpow;//now depends on Z-index
  betacor=ichar?pow(min(fabs(beta/betamax),1.),betapow):1;//corr to "mip"(=1 for ichar=0(electrons))
  xmax=xmin+nbins*stpx;
  hend=elpdf[nbins-1];
  for(int ih=0;ih<nhits;ih++){//hits loop
    if(hstat[ih]>=0){
      eh=ehit[ih]*betacor;
      if(eh<xmin)lkhd+=-log(unpdf);//undfl-region
      else if(eh>=xmax)lkhd+=(-log(hend)+slope*(eh-xmax));//ovfl-region
      else{//central region
        ia=int(floor((eh-xmin)/stpx));
        lkhd+=-log(elpdf[ia]);
      }
    }
  }
  return lkhd;
}
//---------------------------------------------------------------------------
void TofTdcCor::build(){// create TofTdcCor-objects array for real/mc data
//
  char fname[80];
  char name[80];
  int ctyp,ntypes,bmap;
  integer crat,sslt,chan,bin,endflab;
  integer rdcr,rdsl;
//
// ---> read file with the list of version numbers for all needed calib.files :
//
  char datt[3];
  char ext[80];
  int date[2],year,mon,day,hour,min,sec;
  uinteger iutct;
  tm begin;
  time_t utct;
  uinteger verids[10],verid;
  geant corr[TOF2GC::SCTDCCH-2][1024];
//
  for(int i=0;i<TOF2GC::SCTDCCH-2;i++){
    for(int j=0;j<1024;j++)corr[i][j]=0;
  }
//
  strcpy(name,"TofCflist");// basic name for vers.list-file  
  if(AMSJob::gethead()->isMCData()){
    strcpy(datt,"MC");
    sprintf(ext,"%d",TFMCFFKEY.calvern);//MC-versn
    ctyp=7;
  }
  else{
    strcpy(datt,"RD");
    sprintf(ext,"%d",TFREFFKEY.calutc);//RD-utc
    ctyp=6;// because TofSfmap-file is missing in RD list-file
  }
  strcat(name,datt);
  strcat(name,".");
  strcat(name,ext);
//
  if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"====> TofTdcCor::build: Opening Calib_vers_list-file "<<fname<<'\n';
  ifstream vlfile(fname,ios::in);
  if(!vlfile){
    cout <<"<---- Error: missing vers.list-file !!? "<<fname<<endl;
    exit(1);
  }
  vlfile >> ntypes;// total number of calibr. file types in the list
  for(int i=0;i<ntypes;i++){
    vlfile >> verids[i];// 
  }
  if(AMSJob::gethead()->isMCData()){
    vlfile >> date[0];//YYYYMMDD beg.validity of TofCflistMC.ext file
    vlfile >> date[1];//HHMMSS ......................................
    year=date[0]/10000;//2004->
    mon=(date[0]%10000)/100;//1-12
    day=(date[0]%100);//1-31
    hour=date[1]/10000;//0-23
    min=(date[1]%10000)/100;//0-59
    sec=(date[1]%100);//0-59
    begin.tm_isdst=0;
    begin.tm_sec=sec;
    begin.tm_min=min;
    begin.tm_hour=hour;
    begin.tm_mday=day;
    begin.tm_mon=mon-1;
    begin.tm_year=year-1900;
    utct=mktime(& begin);
    iutct=uinteger(utct);
    cout<<"      TofCflistMC-file begin_date: year:month:day = "<<year<<":"<<mon<<":"<<day<<endl;
    cout<<"                                     hour:min:sec = "<<hour<<":"<<min<<":"<<sec<<endl;
    cout<<"                                         UTC-time = "<<iutct<<endl;
  }
  else{
    utct=time_t(TFREFFKEY.calutc);
    printf("      TofCflistRD-file begin_date: %s",ctime(&utct)); 
  }
  vlfile.close();
//----
  verid=verids[ctyp-1];//MC-versn or RD-utc
  strcpy(name,"TofTdcor");//generic TDC-Calib file-name
  strcat(name,datt);
  strcat(name,".");
  sprintf(ext,"%d",verid);//got TofCflistMC(RD). file extention
  strcat(name,ext);
  if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Opening TofTdcCor-file : "<<fname<<'\n';
  ifstream crfile(fname,ios::in); // open file for reading
  if(!crfile){
    cout <<"<---- Error: Missing TofTdcCor-file !!? "<<fname<<endl;
    exit(1);
  }
//----
  for(crat=0;crat<TOF2GC::SCCRAT;crat++){//crates loop
    crfile >> rdcr;
    if(rdcr!=(crat+1)){
      cout<<"<---- Error: Broken structure, crate="<<crat+1<<" red as "<<rdcr<<endl;
      exit(1);
    }
    for(sslt=0;sslt<TOF2GC::SCFETA-1;sslt++){// SFETs loop (no corr. for ACC (SFEA-card))
      crfile >> rdsl;
      if(rdsl!=(sslt+1)){
        cout<<"<---- Error: Broken structure, slot="<<sslt+1<<" red as "<<rdsl<<endl;
        exit(1);
      }
      crfile >> bmap;
      for(chan=0;chan<TOF2GC::SCTDCCH-2;chan++){//Card channels loop
        if((bmap & (1<<chan))==0)continue;//empty or non-correctable channel
        for(bin=0;bin<1024;bin++)crfile >> corr[chan][bin];//read one channel
      }
      tdccor[crat][sslt]=TofTdcCor(bmap,corr);
    }
  }
//
  crfile >> endflab;//read endfile-label
  crfile.close();
//
  if(endflab==12345){
    cout<<"      TofTdcCor-file is successfully read !"<<endl;
  }
  else{
    cout<<"<---- Error: broken structure in TofTdcCor-file !!!"<<endl;
    exit(1);
  }
}
//-------------------------
geant TofTdcCor::getcor(int time, int ch){//ch=0,1,...
  int t10=(time&(0x3FFL));//10 lsb of TDC-count(time measurement)
  if(t10<=0)return(0);
  return _icor[ch][t10-1];
}
//-----------------------------------------------------------------------
void TofTdcCorMS::build(){// create TofTdcCor-objects array for real/mc data
//
  char fname[80];
  char name[80];
  int ctyp,ntypes,bmap;
  integer crat,sslt,chan,bin,endflab;
  integer rdcr,rdsl;
//
// ---> read file with the list of version numbers for all needed calib.files :
//
  char datt[3];
  char ext[80];
  int date[2],year,mon,day,hour,min,sec;
  uinteger iutct;
  tm begin;
  time_t utct;
  uinteger verids[10],verid;
  geant corr[TOF2GC::SCTDCCH-2][1024];
//
  for(int i=0;i<TOF2GC::SCTDCCH-2;i++){
    for(int j=0;j<1024;j++)corr[i][j]=0;
  }
//
  strcpy(name,"TofCflist");// basic name for vers.list-file  
  strcpy(datt,"MC");
  sprintf(ext,"%d",TFMCFFKEY.calvern);//MC-versn
  ctyp=7;
  strcat(name,datt);
  strcat(name,".");
  strcat(name,ext);
//
  if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"====> TofTdcCorMS::build: Opening Calib_vers_list-file "<<fname<<'\n';
  ifstream vlfile(fname,ios::in);
  if(!vlfile){
    cout <<"<---- Error: missing vers.list-file !!? "<<fname<<endl;
    exit(1);
  }
  vlfile >> ntypes;// total number of calibr. file types in the list
  for(int i=0;i<ntypes;i++){
    vlfile >> verids[i];// 
  }
  vlfile >> date[0];//YYYYMMDD beg.validity of TofCflistMC.ext file
  vlfile >> date[1];//HHMMSS ......................................
  year=date[0]/10000;//2004->
  mon=(date[0]%10000)/100;//1-12
  day=(date[0]%100);//1-31
  hour=date[1]/10000;//0-23
  min=(date[1]%10000)/100;//0-59
  sec=(date[1]%100);//0-59
  begin.tm_isdst=0;
  begin.tm_sec=sec;
  begin.tm_min=min;
  begin.tm_hour=hour;
  begin.tm_mday=day;
  begin.tm_mon=mon-1;
  begin.tm_year=year-1900;
  utct=mktime(& begin);
  iutct=uinteger(utct);
  cout<<"      TofCflistMC-file begin_date: year:month:day = "<<year<<":"<<mon<<":"<<day<<endl;
  cout<<"                                     hour:min:sec = "<<hour<<":"<<min<<":"<<sec<<endl;
  cout<<"                                         UTC-time = "<<iutct<<endl;
  vlfile.close();
//----
  verid=verids[ctyp-1];//MC-versn
  strcpy(name,"TofTdcorSD");//generic TDC-Calib MC-Seed file-name
  strcat(name,".");
  sprintf(ext,"%d",verid);//got TofCflistMC(RD). file extention
  strcat(name,ext);
  if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Opening MCSeed TofTdcCor-file : "<<fname<<'\n';
  ifstream crfile(fname,ios::in); // open file for reading
  if(!crfile){
    cout <<"<---- Error: Missing TofTdcCor-file !!? "<<fname<<endl;
    exit(1);
  }
//----
  for(crat=0;crat<TOF2GC::SCCRAT;crat++){//crates loop
    crfile >> rdcr;
    if(rdcr!=(crat+1)){
      cout<<"<---- Error: Broken structure, crate="<<crat+1<<" red as "<<rdcr<<endl;
      exit(1);
    }
    for(sslt=0;sslt<TOF2GC::SCFETA-1;sslt++){// SFETs loop (no corr. for ACC (SFEA-card))
      crfile >> rdsl;
      if(rdsl!=(sslt+1)){
        cout<<"<---- Error: Broken structure, slot="<<sslt+1<<" red as "<<rdsl<<endl;
        exit(1);
      }
      crfile >> bmap;
      for(chan=0;chan<TOF2GC::SCTDCCH-2;chan++){//Card channels loop
        if((bmap & (1<<chan))==0)continue;//empty or non-correctable channel
        for(bin=0;bin<1024;bin++)crfile >> corr[chan][bin];//read one channel
      }
      tdccor[crat][sslt]=TofTdcCorMS(bmap,corr);
    }
  }
//
  crfile >> endflab;//read endfile-label
  crfile.close();
//
  if(endflab==12345){
    cout<<"      MCSeed TofTdcCor-file is successfully read !"<<endl;
  }
  else{
    cout<<"<---- Error: broken structure in MCSeed TofTdcCor-file !!!"<<endl;
    exit(1);
  }
}
//-------------------
int TofTdcCorMS::getbin(number htime, number ttime, int ch){//ch=0,1,...
//on input hit/trig(lev1)-times are inside of 51.2mks(11bits*25ns) range, TDC Coarse and
//TrigTimeTag counters are internally 12bits and sincronized, so when TrigTimeTag subtraction
// is "on" - no problem with OVFLs, when "off" - hit OVFL is simulated
// the match-window is checked, if outside - return(-1) !!!
  integer lsbt,msbt,bin,bn,timeCC,timeTT,low,hiw;
  number t10;
//
  timeTT=integer(floor((ttime+TOF2DBc::tdctrdel())/TOF2DBc::tdcbin(3)));//+"board" delay, TT-counter(<=12msb) not overflowed !
//(imply that tdctrdel < 25mks what is the case !!!)
  timeCC=integer(floor(htime/TOF2DBc::tdcbin(3)));//CCounter(<=12msb) not overflowed !
  low=timeTT-integer(floor(TOF2DBc::tdctrlat()/TOF2DBc::tdcbin(3)));// low-lim for match-window check
  hiw=low+integer(floor(TOF2DBc::tdcmatw()/TOF2DBc::tdcbin(3)));// high-lim for match-window check
  if((timeCC < low) || (timeCC > hiw))return(-1);//out of match-window !!!
//
  t10=(htime-timeCC*number(TOF2DBc::tdcbin(3)))/number(TOF2DBc::tdcbin(1));//fine(PLL+DLL+RC) time(<25ns) -> 10 lsb
  if(TFMCFFKEY.tdclin>0){// find FINE-bin(within 25ns range) taking into account nonlinearity
    for(bn=0;bn<1024;bn++){
      if(t10<(number(bn+1)-number(_icor[ch][bn])))break;
    }
    if(bn>=1024)bn=1023;//pc accur. protection
  }
  else bn=integer(floor(t10));
//
  if(TFMCFFKEY.tdcovf>0)msbt=timeCC-low;//TrTimeTag "subtraction", so OVFL not possible(msbt < 12bits range) 
  else msbt=(timeCC&0x7FFL);//keep only 11bits, so OVFL may happen !!
//
  bin=1024*msbt;  
  return(bin+bn);
}
//------------------------
int TofTdcCorMS::getbins(number htime, number ttime){//no lin-corr, suitable for ATC also !!!
//on input hit/trig(lev1)-times are inside of 51.2mks(11bits*25ns) range, TDC Coarse and
//TrigTimeTag counters are internally 12bits and sincronized, so when TrigTimeTag subtraction
// is "on" - no problem with OVFLs, when "off" - hit OVFL is simulated
// the match-window is checked, if outside - return(-1) !!!
  integer lsbt,msbt,bin,bn,timeCC,timeTT,low,hiw;
  number t10;
//  return integer(floor(htime/TOF2DBc::tdcbin(1)));
//
  timeTT=integer(floor((ttime+TOF2DBc::tdctrdel())/TOF2DBc::tdcbin(3)));//+"board" delay, TT-counter(<=12msb) no overfl !
//(imply that tdctrdel < 25mks what is the case !!!)
  timeCC=integer(floor(htime/TOF2DBc::tdcbin(3)));//CCounter(<=12msb) not overflowed !
  low=timeTT-integer(floor(TOF2DBc::tdctrlat()/TOF2DBc::tdcbin(3)));// low-lim for match-window check
  hiw=low+integer(floor(TOF2DBc::tdcmatw()/TOF2DBc::tdcbin(3)));// high-lim for match-window check
//cout<<"In getbins: htm/ttm="<<htime<<" "<<ttime<<" tTT/tCC="<<timeTT<<" "<<timeCC<<" low/hiw="<<low<<" "<<hiw<<endl;
  if((timeCC < low) || (timeCC > hiw))return(-1);//out of match-window !!!
//  cout<<"   Match OK"<<endl;
//
  t10=htime-timeCC*TOF2DBc::tdcbin(3);//fine(PLL+DLL+RC) time(<25ns) -> 10 lsb
  bn=integer(floor(t10/TOF2DBc::tdcbin(1)));
//  cout<<"   ht/tCC*25="<<htime<<timeCC*TOF2DBc::tdcbin(3)<<" t10/bn="<<t10<<" "<<bn<<endl;
//
  if(TFMCFFKEY.tdcovf>0)msbt=timeCC-low;//TrTimeTag "subtraction", so OVFL not possible(msbt < 12bits range) 
  else msbt=(timeCC&0x7FFL);//keep only 11bits, so OVFL may happen !!
//  cout<<"   msbt="<<msbt<<endl;
//
  bin=1024*msbt;  
  return(bin+bn);
}
//-----------------------------------------------------------------------


