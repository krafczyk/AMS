//
// Author V. Choutko 24-may-1996
// CTC codes added 29-sep-1996 by E.Choumilov 
//
 
#include <job.h>
#include <amsgobj.h>
#include <cern.h>
#include <math.h>
#include <commons.h>
#include <amsdbc.h>
#include <tofdbc.h>
#include <trid.h>
#include <mccluster.h>
#include <extC.h>
#include <event.h>
#include <charge.h>
#include <ctcdbc.h>
AMSJob* AMSJob::_Head=0;
AMSNodeMap AMSJob::JobMap;
integer AMSJob::debug=1;
void AMSJob::data(){
#ifdef __HPUX__
  AMSTrIdSoft::init();
  AMSCharge::init();
  AMSgObj::GTrMedMap=*(new  AMSNodeMap() );
  AMSgObj::GVolMap= *(new  AMSNodeMap() );
  AMSgObj::BookTimer= *(new AMSStat() );
#endif  
  SELECTFFKEY.Run=0;
  SELECTFFKEY.Event=0;
  FFKEY("SELECT",(float*)&SELECTFFKEY,sizeof(SELECTFFKEY_DEF)/sizeof(integer),"MIXED");

  AMSFFKEY.Simulation=0; // Simulation
  AMSFFKEY.Reconstruction=1; // Reconstruction
  AMSFFKEY.Jobtype=0; // Default
  AMSFFKEY.Debug=1;
  AMSFFKEY.CpuLimit=60;
  AMSFFKEY.Read=0;
  AMSFFKEY.Write=1;
  VBLANK(AMSFFKEY.Jobname,40);
  VBLANK(AMSFFKEY.Setupname,40);
  AMSFFKEY.ZeroSetupOk=0;
  // Set Defaults
FFKEY("AMSJOB",(float*)&AMSFFKEY+2,sizeof(AMSFFKEY_DEF)/sizeof(integer),"MIXED");
_siamsdata();
_reamsdata();
}

void AMSJob::_siamsdata(){
IOPA.hlun=0;
VBLANK(IOPA.hfile,40);
IOPA.ntuple=1;
IOPA.TriggerI=1;
IOPA.WriteAll=0;
VBLANK(IOPA.TriggerC,40);
char amsp[12]="AMSParticle";
UCTOH(amsp,IOPA.TriggerC,4,12);
FFKEY("IOPA",(float*)&IOPA,sizeof(IOPA_DEF)/sizeof(integer),"MIXED");

_sitkdata();
_signdata();
_sitofdata();
_sitrddata();
_sictcdata();
}

void AMSJob::_sitkdata(){
TRMCFFKEY.alpha=230;
TRMCFFKEY.beta=1;
TRMCFFKEY.gamma=0.13;
TRMCFFKEY.fastswitch=5.e-5;  // inverse linear density of primary electrons
TRMCFFKEY.dedx2nprel=0.33e6;
TRMCFFKEY.ped[0]=100;
TRMCFFKEY.ped[1]=100;
TRMCFFKEY.gain[0]=1;
TRMCFFKEY.gain[1]=1;
TRMCFFKEY.sigma[1]=110/20/sqrt(3.); // sig/noise ratio is about 20 for y
TRMCFFKEY.sigma[0]=TRMCFFKEY.sigma[1]*1.41;   // x strip two times larger y
TRMCFFKEY.thr[0]=17;      // should be sync with sigma approx 4*sigma
TRMCFFKEY.thr[1]=12;      // ------------------------
TRMCFFKEY.neib[0]=2;
TRMCFFKEY.neib[1]=2;
TRMCFFKEY.cmn[0]=50;
TRMCFFKEY.cmn[1]=50;
TRMCFFKEY.adcoverflow=32767;
TRMCFFKEY.NoiseOn=1;

FFKEY("TRMC",(float*)&TRMCFFKEY,sizeof(TRMCFFKEY_DEF)/sizeof(integer),"MIXED");
}

void AMSJob::_signdata(){
CCFFKEY.coo[0]=-1.e10;
CCFFKEY.coo[1]=-1.e10;
CCFFKEY.coo[2]=-1.e10;
CCFFKEY.coo[3]=1.e10;
CCFFKEY.coo[4]=1.e10;
CCFFKEY.coo[5]=1.e10;
CCFFKEY.dir[0]=-1.;
CCFFKEY.dir[1]=-1.;
CCFFKEY.dir[2]=-1.;
CCFFKEY.dir[3]=1.;
CCFFKEY.dir[4]=1.;
CCFFKEY.dir[5]=1.;
CCFFKEY.momr[0]=-1.e10;
CCFFKEY.momr[1]=1.e10;
CCFFKEY.fixp=0;
CCFFKEY.albedor=0.01;
CCFFKEY.albedocz=0.05;
CCFFKEY.npat=1;
CCFFKEY.run=100;
CCFFKEY.low=0;
FFKEY("MCGEN",(float*)&CCFFKEY,sizeof(CCFFKEY_DEF)/sizeof(integer),"MIXED");
}
//==========================================================================
void AMSJob::_sitofdata(){
  TOFMCFFKEY.TimeSigma=1.e-10; // time resolution(sec) for simplified algorithm
  TOFMCFFKEY.padl=10.5;        // sc. bar transv. step ........................
  TOFMCFFKEY.Thr=0.1;          // Sc.bar Elos-thresh.(Mev) to participate in Reco   
//
  TOFMCFFKEY.mcprtf[0]=0;     // TOF MC print flag for init arrays
  TOFMCFFKEY.mcprtf[1]=0;     // TOF MC print flag for MC pulses
  TOFMCFFKEY.mcprtf[2]=0;     // ...................... histograms
  TOFMCFFKEY.mcprtf[3]=0;     // spare
  TOFMCFFKEY.mcprtf[4]=0;     // spare
  TOFMCFFKEY.trlogic[0]=0; // MC trigger logic flag (=0/1-> two-sides-AND/OR of counter) 
  TOFMCFFKEY.trlogic[1]=0; // spare 
  TOFMCFFKEY.fast=1;
FFKEY("TOFMC",(float*)&TOFMCFFKEY,sizeof(TOFMCFFKEY_DEF)/sizeof(integer),"MIXED");
}
//=======================================================================================

void AMSJob::_sictcdata(){


  CTCGEOMFFKEY.wallth=0.03;    // reflecting wall and separators thickness(cm)
  CTCGEOMFFKEY.agap=0.02;      // typical "air" gaps for aerogel
  CTCGEOMFFKEY.wgap=0.01;      // typical "air" gaps for WLS's
  CTCGEOMFFKEY.agsize[0]=120.;  // max aerogel x-size
  CTCGEOMFFKEY.agsize[1]=90.; // max aerogel y-size
  CTCGEOMFFKEY.agsize[2]=10.;  // max aerogel z-size
  CTCGEOMFFKEY.wlsth=0.3;      // WLS thickness
  CTCGEOMFFKEY.hcsize[0]=165.; // supp. honeycomb plate X-size
  CTCGEOMFFKEY.hcsize[1]=165.; // supp. honeycomb plate Y-size
  CTCGEOMFFKEY.hcsize[2]=5.;   // supp. honeycomb plate Z-size
  CTCGEOMFFKEY.supzc=-84.;     // supp. honeycomb plate front face Z-pos
  CTCGEOMFFKEY.aegden=0.24;    // aerogel density
  CTCGEOMFFKEY.wlsden=1.03;    // WLS density
  CTCGEOMFFKEY.nblk=12;        // number of aer. blocks (X-div.)(=1 for solid)
  CTCGEOMFFKEY.nwls=12;        // number of wls blocks
  CTCMCFFKEY.Refraction[0]=1.05;   // Refraction indexes
  CTCMCFFKEY.Refraction[1]=1.58;
  CTCMCFFKEY.Path2PhEl[0]=23;   // Path to photoelectrons conv fact (was 34)
  CTCMCFFKEY.Path2PhEl[1]=28;
  CTCMCFFKEY.AbsLength[0]=15;   // Abs Length in cm  for hor readout (was 4.9)
  CTCMCFFKEY.AbsLength[1]=100;
  CTCMCFFKEY.Edep2Phel[0]=0;   // Agel is not a scint
  CTCMCFFKEY.Edep2Phel[1]=184e3;  // WLS is scint if vertical readout
FFKEY("CTCGEOM",(float*)&CTCGEOMFFKEY,sizeof(CTCGEOMFFKEY_DEF)/sizeof(integer),"MIXED");
FFKEY("CTCMC",(float*)&CTCMCFFKEY,sizeof(CTCMCFFKEY_DEF)/sizeof(integer),"MIXED");
}
//=============================================================================

void AMSJob::_sitrddata(){
}

void AMSJob:: _reamsdata(){
_retkdata();
_retofdata();
_retrddata();
_rectcdata();
_reaxdata();

}

void AMSJob::_retkdata(){

TRCLFFKEY.ThrClA[1]=45;
TRCLFFKEY.Thr1A[1] =25;
TRCLFFKEY.Thr2A[1] =7;

TRCLFFKEY.ThrClS[1]=10;
TRCLFFKEY.Thr1S[1] =7;
TRCLFFKEY.Thr2S[1] =7;

TRCLFFKEY.ThrClR[1]=8;
TRCLFFKEY.Thr1R[1] =5;
TRCLFFKEY.Thr2R[1] =1.;
TRCLFFKEY.Thr3R[1] =-2.;

TRCLFFKEY.ThrClNMin[1]=2;
TRCLFFKEY.ThrClNEl[1]=3;

TRCLFFKEY.ThrClA[0]=45;
TRCLFFKEY.Thr1A[0] =30;
TRCLFFKEY.Thr2A[0] =6;

TRCLFFKEY.ThrClS[0]=14;
TRCLFFKEY.Thr1S[0] =10;
TRCLFFKEY.Thr2S[0] =10;

TRCLFFKEY.ThrClR[0]=7;
TRCLFFKEY.Thr1R[0] =6;
TRCLFFKEY.Thr2R[0] =1.;
TRCLFFKEY.Thr3R[0] =-2.;

TRCLFFKEY.ThrClNMin[0]=1;
TRCLFFKEY.ThrClNEl[0]=3;

TRCLFFKEY.ErrX=30.e-4;
TRCLFFKEY.ErrY=10.e-4;
TRCLFFKEY.ErrZ=30.e-4;
TRCLFFKEY.ThrDSide=1.;

TRCLFFKEY.CorFunParA[0][0]=400e-4;
TRCLFFKEY.CorFunParB[0][0]=0.85;
TRCLFFKEY.CorFunParA[0][5]=400e-4;
TRCLFFKEY.CorFunParB[0][5]=0.85;
int k;
for (k=1;k<5;k++){
 TRCLFFKEY.CorFunParA[0][k]=400e-4;
 TRCLFFKEY.CorFunParB[0][k]=0.85;
}
for ( k=0;k<6;k++){
 TRCLFFKEY.CorFunParA[1][k]=65e-4;
 TRCLFFKEY.CorFunParB[1][k]=0.5;
}

for(k=0;k<6;k++){
  for(int l=0;l<3;l++){
   TRCLFFKEY.ResFunX[l][k]=0;
   TRCLFFKEY.ResFunY[l][k]=0;
  }
}

FFKEY("TRCL",(float*)&TRCLFFKEY,sizeof(TRCLFFKEY_DEF)/sizeof(integer),"MIXED");

// Fit Par
TRFITFFKEY.pattern[0]=1;
TRFITFFKEY.pattern[1]=1;
TRFITFFKEY.pattern[2]=1;
TRFITFFKEY.pattern[3]=1;
TRFITFFKEY.pattern[4]=1;
TRFITFFKEY.pattern[5]=0;
TRFITFFKEY.pattern[6]=0;
TRFITFFKEY.pattern[7]=0;
TRFITFFKEY.pattern[8]=0;
TRFITFFKEY.pattern[9]=0;
TRFITFFKEY.pattern[10]=0;
TRFITFFKEY.pattern[11]=0;
TRFITFFKEY.pattern[12]=0;
TRFITFFKEY.pattern[13]=0;
TRFITFFKEY.pattern[14]=0;
TRFITFFKEY.pattern[15]=0;
TRFITFFKEY.pattern[16]=0;
TRFITFFKEY.pattern[17]=0;
TRFITFFKEY.pattern[18]=0;
TRFITFFKEY.UseTOF=1;
TRFITFFKEY.Chi2FastFit=10;
TRFITFFKEY.Chi2StrLine=20;
TRFITFFKEY.Chi2Circle=20;
TRFITFFKEY.ResCutFastFit=0.5;
TRFITFFKEY.ResCutStrLine=0.5;
TRFITFFKEY.ResCutCircle=0.5;
TRFITFFKEY.SearchRegFastFit=1;
TRFITFFKEY.SearchRegStrLine=0.5;
TRFITFFKEY.SearchRegCircle=1;
TRFITFFKEY.RidgidityMin=0.2;
TRFITFFKEY.FullReco=0;
FFKEY("TRFIT",(float*)&TRFITFFKEY,sizeof(TRFITFFKEY_DEF)/sizeof(integer),"MIXED");
TKFINI();
}

void AMSJob::_retofdata(){
  TOFRECFFKEY.Thr1=0.45;// Threshold (mev) on peak bar energy 
  TOFRECFFKEY.ThrS=0.9; // Threshold (mev) on total cluster energy
//
  TOFRECFFKEY.reprtf[0]=0; // RECO print flag 
  TOFRECFFKEY.reprtf[1]=0; // RECO print flag 
  TOFRECFFKEY.reprtf[2]=0; // RECO print flag for histograms
  TOFRECFFKEY.reprtf[3]=0; // RECO print flag 
  TOFRECFFKEY.reprtf[4]=0; // RECO print flag
  TOFRECFFKEY.relogic[0]=0;// 0/1 -> normal/calibr. run. 
  TOFRECFFKEY.relogic[1]=0;// RECO logic flag 
  TOFRECFFKEY.relogic[2]=0;// RECO logic flag 
  TOFRECFFKEY.relogic[3]=0;// RECO logic flag 
  TOFRECFFKEY.relogic[4]=0;// RECO logic flag 
  FFKEY("TOFREC",(float*)&TOFRECFFKEY,sizeof(TOFRECFFKEY_DEF)/sizeof(integer),"MIXED");
}

void AMSJob::_rectcdata(){
  CTCRECFFKEY.Thr1=1.5;
  CTCRECFFKEY.ThrS=3;
  FFKEY("CTCREC",(float*)&CTCRECFFKEY,sizeof(CTCRECFFKEY_DEF)/sizeof(integer),"MIXED");
}

void AMSJob::_reaxdata(){
// Fit beta & charge
CHARGEFITFFKEY.Thr=1.;
BETAFITFFKEY.pattern[0]=1;
BETAFITFFKEY.pattern[1]=1;
BETAFITFFKEY.pattern[2]=1;
BETAFITFFKEY.pattern[3]=1;
BETAFITFFKEY.pattern[4]=1;
BETAFITFFKEY.pattern[5]=0;
BETAFITFFKEY.pattern[6]=0;
BETAFITFFKEY.pattern[7]=0;
BETAFITFFKEY.pattern[8]=0;
BETAFITFFKEY.Chi2=3;
BETAFITFFKEY.SearchReg[0]=3.;
BETAFITFFKEY.SearchReg[1]=3.;
BETAFITFFKEY.SearchReg[2]=3.;
FFKEY("BETAFIT",(float*)&BETAFITFFKEY,sizeof(BETAFITFFKEY_DEF)/sizeof(integer),"MIXED");
FFKEY("CHARGEFIT",(float*)&CHARGEFITFFKEY,sizeof(CHARGEFITFFKEY_DEF)/sizeof(integer),"MIXED");
}

void AMSJob::_retrddata(){
}



void AMSJob::udata(){
char jobname[160];
char setupname[160];
char triggername[160];
UHTOC(AMSFFKEY.Jobname,40,jobname,160);
UHTOC(AMSFFKEY.Setupname,40,setupname,160);
UHTOC(IOPA.TriggerC,40,triggername,160);
jobname[159]='\0';
setupname[159]='\0';
triggername[159]='\0';
int i;
//+
for (i=158; i>0; i--) {        // should be at least 1 char
 if(jobname[i] == ' ') jobname[i]='\0';
 else break;
}
for (i=158; i>=0; i--) {
 if(setupname[i] == ' ') setupname[i]='\0';
 else break;
}

//-
int len;
for(i=158;i>=0;i--){
   if(triggername[i]==' '){
    triggername[i]='\0';
    len=i+1;
   }
   else break;
}
setsetup(setupname);
if(getsetup())setname(strcat(jobname,getsetup()));
else{
  cerr<<"AMSJOB::udata-F-NULLSETUP- Setup not defined"<<endl;
  exit(1);
}
integer ntrig=0;
integer nold=0;
integer or=0;
for (i=0;i<len;i++){
  if(triggername[i]=='|' || triggername[i]=='\0' || triggername[i]=='&'){
    // new trigger found
       if(triggername[i]=='|')or=1;
       triggername[i]='\0';
       if(i-nold>0)settrigger(triggername+nold,ntrig++,IOPA.TriggerI,or);
       nold=i+1;
  }
}
_jobtype=AMSFFKEY.Jobtype;
//
// Read/Write Synchronization
if(AMSFFKEY.Read > 10 && AMSFFKEY.Read%2==0)AMSFFKEY.Read++;
if(AMSFFKEY.Write > 0 && AMSFFKEY.Write%2==0)AMSFFKEY.Write++;
}


void AMSJob::init(){
if(_jobtype ==AMSFFKEY.Simulation)_siamsinitjob();
_reamsinitjob();
cout << *this;
}
void AMSJob::_siamsinitjob(){
  AMSEvent::debug=AMSFFKEY.Debug;
  _sitkinitjob();
  _signinitjob();
  _sitofinitjob();
  _sitrdinitjob();
  _sictcinitjob();
}

void AMSJob::_sitkinitjob(){
     for(int l=0;l<2;l++){
       for (int i=0;i<AMSDBc::nlay();i++){
         for (int j=0;j<AMSDBc::nlad(i+1);j++){
           for (int s=0;s<2;s++){
            AMSTrIdSoft id(i+1,j+1,s,l);
            number oldone=0;
            for(int k=0;k<AMSDBc::NStripsDrp(i+1,l);k++){
             id.upd(k);
             id.setped()=TRMCFFKEY.ped[l];
             id.setstatus(0);
             id.setsig()=TRMCFFKEY.sigma[l];
             id.setgain()=TRMCFFKEY.gain[l];
             id.setcmnnoise()=TRMCFFKEY.cmn[l];
             id.setindnoise()=oldone+
             AMSTrMCCluster::sitknoiseprob(id ,TRMCFFKEY.thr[l]);
             oldone=id.getindnoise();
            }
           }
         }
       }
     }

     AMSgObj::BookTimer.book("GEANTTRACKING");
     AMSgObj::BookTimer.book("SITKHITS");
     AMSgObj::BookTimer.book("SITKNOISE");
     AMSgObj::BookTimer.book("SITKDIGIa");
     AMSgObj::BookTimer.book("SITKDIGIb");
     AMSgObj::BookTimer.book("SITKDIGIc");
     AMSgObj::BookTimer.book("TrMCCluster");
}

void AMSJob::_signinitjob(){

  AMSmceventg::setcuts(CCFFKEY.coo,CCFFKEY.dir,CCFFKEY.momr,
  CCFFKEY.fixp,CCFFKEY.albedor,CCFFKEY.albedocz);


}
//========================================================================
void AMSJob::_sitofinitjob(){
  if(TOFMCFFKEY.fast==1)cout <<"_sitofinit-I-Fast/Crude TOF simulation algorithm selected."<<endl;
 else cout <<"_sitofinit-I-Slow/Accurate TOF simulation algorithm selected."<<endl;
     AMSgObj::BookTimer.book("SITOFDIGI");
     AMSgObj::BookTimer.book("TOF:Ghit->Tovt");
     AMSgObj::BookTimer.book("TOF:Tovt->RwEv");
     AMSgObj::BookTimer.book("TovtPM1loop");
     AMSgObj::BookTimer.book("TovtPM2loop");
     AMSgObj::BookTimer.book("TovtPM2sloopscpmesp");
     AMSgObj::BookTimer.book("TovtPM2sloopscmcscan");
     AMSgObj::BookTimer.book("TovtPM2sloopsum");
     AMSgObj::BookTimer.book("TovtOther");
    if(TOFMCFFKEY.mcprtf[2]!=0){ // Book mc-hist
      HBOOK1(1050,"Geant-hits in layer-1",80,0.,80.,0.);
      HBOOK1(1051,"Geant-hits in layer-2",80,0.,80.,0.);
      HBOOK1(1052,"Geant-hits in layer-3",80,0.,80.,0.);
      HBOOK1(1053,"Geant-hits in layer-4",80,0.,80.,0.);
      HBOOK1(1060,"Geant-Edep(mev) in layer-1",80,0.,24.,0.);
      HBOOK1(1061,"Geant-Edep(mev) in layer-1",80,0.,240.,0.);
      HBOOK1(1062,"Geant-Edep(mev) in layer-3",80,0.,24.,0.);
      HBOOK1(1063,"Geant-Edep(mev) in layer-3",80,0.,240.,0.);
      HBOOK1(1070,"Log(PulseTotCharge(pC)),Sd-1,L-1",50,0.,10.,0.);
      HBOOK1(1071,"Total bar pulse-charge(pC),L-1",80,100.,1700.,0.);
      HBOOK1(1072,"Total bar pulse-charge(pC),L-1",80,1000.,17000.,0.);
#ifdef __AMSDEBUG__
#endif
    }
}
//========================================================================
void AMSJob::_sictcinitjob(){
     AMSgObj::BookTimer.book("SICTCDIGI");
     AMSCTCRawCluster::init();
}

void AMSJob::_sitrdinitjob(){
}


void AMSJob::_reamsinitjob(){
_retkinitjob();
_retofinitjob();
_retrdinitjob();
_rectcinitjob();
_reaxinitjob();
}

void AMSJob::_retkinitjob(){
AMSgObj::BookTimer.book("RETKEVENT");
AMSgObj::BookTimer.book("TrCluster");
AMSgObj::BookTimer.book("TrRecHit");
AMSgObj::BookTimer.book("TrTrack");
}
//====================================================================
void AMSJob::_retofinitjob(){
    AMSgObj::BookTimer.book("RETOFEVENT");
    AMSgObj::BookTimer.book("TOF:DAQ->RwEv");
    AMSgObj::BookTimer.book("TOF:RwEv->RwCl");
    AMSgObj::BookTimer.book("TOF:RwCl->Cl");
    if(TOFRECFFKEY.reprtf[2]!=0){ // Book reco-hist
      HBOOK1(1100,"The same hit (Tf-Ts),(ns)",80,-40.,40.,0.);
      HBOOK1(1101,"Time_history:befor_hit dist(ns)",80,0.,160.,0.);
      HBOOK1(1102,"Time_history:after_hit dist(ns)",80,0.,160.,0.);
      HBOOK1(1529,"L=1,Edep_anode(mev),corr,ideal evnt",80,0.,24.,0.);
      HBOOK1(1526,"L=1,Edep_anode(mev),corr,ideal evnt",80,0.,240.,0.);
      HBOOK1(1530,"L=3,Edep_anode(mev),corr,ideal evnt",80,0.,24.,0.);
      HBOOK1(1527,"L=3,Edep_anode(mev),corr,ideal evnt",80,0.,240.,0.);
      HBOOK1(1531,"L=1,Edep_dinode(mev),corr,ideal evnt",80,0.,24.,0.);
      HBOOK1(1528,"L=1,Edep_dinode(mev),corr,ideal evnt",80,0.,240.,0.);
      HBOOK1(1532,"(T1-T3)(ns),corr,ideal evnt",50,3.,6.,0.);
      HBOOK1(1533,"L=1,side1/2 Tdiff(ns),ideal evnt",100,-2.5,2.5,0.);
      HBOOK1(1534,"(T2-T4)(ns),corr,ideal evnt",50,3.,6.,0.);
      HBOOK1(1535,"L=1,TOF Eclust(mev)",80,0.,24.,0.);
      HBOOK1(1536,"L=3,TOF Eclust(mev)",80,0.,24.,0.);
      HBOOK1(1537,"L=1,TOF Eclust(mev)",80,0.,240.,0.);
      HBOOK1(1538,"L=3,TOF Eclust(mev)",80,0.,240.,0.);
      HBOOK1(1539,"L=2,TOF Eclust(mev)",80,0.,24.,0.);
      HBOOK1(1540,"L=4,TOF Eclust(mev)",80,0.,24.,0.);
      HBOOK1(1541,"L=2,TOF Eclust(mev)",80,0.,240.,0.);
      HBOOK1(1542,"L=4,TOF Eclust(mev)",80,0.,240.,0.);
      if(TOFRECFFKEY.relogic[0]==1){
        HBOOK1(1500,"Part.rigidity from tracker(gv)",80,0.,32.,0.);
        HBOOK1(1501,"Proton beta",80,0.9,1.1,0.);
        HBOOK1(1506,"Tracks multipl. in calib.events",10,0.,10.,0.);
        HBOOK2(1502,"Layer-1,T vs exp(-A)",50,0.,0.3,50,23.,26.,0.);
        HBOOK2(1503,"Layer-2,T vs exp(-A)",50,0.,0.3,50,23.,26.,0.);
        HBOOK2(1504,"Layer-3,T vs exp(-A)",50,0.,0.3,50,19.,22.,0.);
        HBOOK2(1505,"Layer-4,T vs exp(-A)",50,0.,0.3,50,19.,22.,0.);
        HBOOK1(1508,"T1-T3, not corrected",50,3.,6.,0.);
        HBOOK1(1509,"T2-T4, not corrected",50,3.,6.,0.);
        HBOOK1(1510,"Layer-1 PM-1 s-time,noncor",80,18.,28.,0.);
        HBOOK1(1511,"Layer-1 PM-2 s-time,noncor",80,18.,28.,0.);
        HBOOK1(1512,"Layer-1 PM-1 a-ampl,noncor",80,50.,290.,0.);
        HBOOK1(1513,"Layer-1 PM-2 a-ampl,noncor",80,50.,290.,0.);
         HBOOK1(1514,"Layer-2 PM-1 s-time,noncor",80,18.,28.,0.);
         HBOOK1(1515,"Layer-2 PM-2 s-time,noncor",80,18.,28.,0.);
         HBOOK1(1516,"Layer-2 PM-1 a-ampl,noncor",80,50.,290.,0.);
         HBOOK1(1517,"Layer-2 PM-2 a-ampl,noncor",80,50.,290.,0.);
        HBOOK1(1518,"Layer-3 PM-1 s-time,noncor",80,18.,28.,0.);
        HBOOK1(1519,"Layer-3 PM-2 s-time,noncor",80,18.,28.,0.);
        HBOOK1(1520,"Layer-3 PM-1 a-ampl,noncor",80,50.,290.,0.);
        HBOOK1(1521,"Layer-3 PM-2 a-ampl,noncor",80,50.,290.,0.);
         HBOOK1(1522,"Layer-4 PM-1 s-time,noncor",80,18.,28.,0.);
         HBOOK1(1523,"Layer-4 PM-2 s-time,noncor",80,18.,28.,0.);
         HBOOK1(1524,"Layer-4 PM-1 a-ampl,noncor",80,50.,290.,0.);
         HBOOK1(1525,"Layer-4 PM-2 a-ampl,noncor",80,50.,290.,0.);
      }
    }
//
//  Clear JOB-statistics counters for SIM/REC :
//
    TOFJobStat::clear();
}
//====================================================================
void AMSJob::_rectcinitjob(){
AMSgObj::BookTimer.book("RECTCEVENT");
}

void AMSJob::_reaxinitjob(){
AMSgObj::BookTimer.book("REAXEVENT");
}

void AMSJob::_retrdinitjob(){
}

AMSgvolume * AMSJob::getgeom(AMSID id){
  if(id.getname() ==0 && id.getid()==0){
    // Default
    for(int i=0;;){
     AMSNode *p=JobMap.getid(i++);
     if(!p){
      cerr << "AMSJob::getgeom-F-nodefault geom"<<endl;
      break;
     }
     if(strncmp(p->getname(),"AMSG",4)==0)
     return  (AMSgvolume*)p;
    }
  }
  else    return (AMSgvolume*)AMSJob::JobMap.getp(id);
  return 0;
}


AMSgmat * AMSJob::getmat(AMSID id){
  if(id.getname() ==0 && id.getid()==0){
    // Default
    for(int i=0;;){
     AMSNode *p=JobMap.getid(i++);
     if(!p){
      cerr << "AMSJob::getmat-F-nomaterials found"<<endl;
      exit(1);
      break;
     }
     if(strncmp(p->getname(),"Materials:",10)==0)
     return  (AMSgmat*)p;
    }
  }
  else    return (AMSgmat*)AMSJob::JobMap.getp(id);
  return 0;
}

AMSgtmed * AMSJob::getmed(AMSID id){
  if(id.getname() ==0 && id.getid()==0){
    // Default
    for(int i=0;;){
     AMSNode *p=JobMap.getid(i++);
     if(!p){
      cerr << "AMSJob::getgeom-F-notracking media found"<<endl;
      exit(1);
      break;
     }
     if(strncmp(p->getname(),"TrackingMedia:",14)==0)
     return  (AMSgtmed*)p;
    }
  }
  else    return (AMSgtmed*)AMSJob::JobMap.getp(id);
  return 0;
}

void AMSJob::setsetup(char *setup){
  if(setup && (strlen(setup)>1 || AMSFFKEY.ZeroSetupOk))strcpy(_Setup,setup);
  else strcpy(_Setup,"AMSSHUTTLE STRPLANES CTCHor");   //defaults
  
}
void AMSJob::settrigger(char *setup, integer N, integer I,integer or){
  assert(N < maxtrig);
  if(setup){
    strcpy(_TriggerC[N],setup);
  }
  _TriggerI=I;
  _TriggerOr=or;
  _TriggerN=N+1;
}
