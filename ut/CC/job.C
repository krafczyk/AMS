// Author V. Choutko 24-may-1996
// CTC codes added 29-sep-1996 by E.Choumilov 
//
// cut jobname and setupname Oct 10, 1996. ak.
// 
#include <job.h>
#include <amsgobj.h>
#include <cern.h>
#include <math.h>
#include <commons.h>
#include <amsdbc.h>
#include <trid.h>
#include <mccluster.h>
#include <extC.h>
#include <event.h>
#include <charge.h>
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
  FFKEY("SELECT",(float*)(&SELECTFFKEY.Run),2,"MIXED");

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
FFKEY("AMSJOB",(float*)(&AMSFFKEY.Jobtype),86,"MIXED");
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
FFKEY("IOPA",(float*)(&IOPA.hlun),84,"MIXED");

_sitkdata();
_signdata();
_sitofdata();
_sitrddata();
_sictcdata();
}

void AMSJob::_sitkdata(){
TRMCFFKEY.alpha=230;
TRMCFFKEY.beta=2;
TRMCFFKEY.gamma=0.13;
TRMCFFKEY.fastswitch=5.e-5;  // inverse linear density of primary electrons
TRMCFFKEY.dedx2nprel=0.33e6;
TRMCFFKEY.ped[0]=100;
TRMCFFKEY.ped[1]=100;
TRMCFFKEY.gain[0]=1;
TRMCFFKEY.gain[1]=1;
TRMCFFKEY.sigma[1]=36*4/30/sqrt(5.); // signal/noise ratio is about 30 for y
TRMCFFKEY.sigma[0]=TRMCFFKEY.sigma[1]*1.41;   // x strip two times larger y
TRMCFFKEY.thr[0]=10;
TRMCFFKEY.thr[1]=10;
TRMCFFKEY.neib[0]=2;
TRMCFFKEY.neib[1]=2;
TRMCFFKEY.cmn[0]=50;
TRMCFFKEY.cmn[1]=50;

FFKEY("TRMC",&TRMCFFKEY.alpha,17,"MIXED");
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
FFKEY("MCGEN",CCFFKEY.coo,20,"MIXED");
}
//==========================================================================
void AMSJob::_sitofdata(){
  TOFMCFFKEY.TimeSigma=90e-12;  // time resolution
  TOFMCFFKEY.padl=10.5;              // pad width
  TOFMCFFKEY.Thr=0.1;             // threshold in Mev for raw clusters
//
  TOFMCFFKEY.edep2ph=8000.;    // edep(Mev)-to-Photons convertion
  TOFMCFFKEY.pmqeff=0.19;      // PM mean quantum efficiency
  TOFMCFFKEY.trtspr=0.14;      // PM transition time spread (ns)
  TOFMCFFKEY.fladctb=0.05;     // MC flash-ADC time binning (ns)
  TOFMCFFKEY.shaptb=2.;        // MC shaper pulse time binning
  TOFMCFFKEY.di2anr=0.1;       // MC dinode-to-anode signal ratio
  TOFMCFFKEY.shrtim=5.;        // MC shaper pulse rise time (ns)
  TOFMCFFKEY.shftim=50.;       // MC shaper pulse fall time (ns)
  TOFMCFFKEY.accdel[0]=50.;    // "accept" fix-delay(ns) for fast(history) TDC
  TOFMCFFKEY.accdel[1]=20.;    // "accept" fix-delay(ns) for slow TDC(stratcher)
  TOFMCFFKEY.accdelmx[0]=1000.;//max. "accept" delay(ns) for fast(history) TDC
  TOFMCFFKEY.accdelmx[0]=100.; //max. "accept" delay(ns) for slow TDC(stratcher)
  TOFMCFFKEY.strrat=10.;       // stratcher ratio 
  TOFMCFFKEY.tdcbin[0]=1.;     // pipe/line TDC binning for fast-tdc meas.
  TOFMCFFKEY.tdcbin[0]=1.;     // pipe/line TDC binning for slow-tdc meas.
  TOFMCFFKEY.tdcbin[0]=1.;     // pipe/line TDC binning for adc-anode meas.
  TOFMCFFKEY.tdcbin[0]=1.;     // pipe/line TDC binning for adc-dinode meas.
  TOFMCFFKEY.daqthr[0]=5.;   // MC threshold for discr. of "z>=1"-trig,fast/slow_TDC
  TOFMCFFKEY.daqthr[1]=10.;  // MC threshold for discr. of "z>1"-trig
  TOFMCFFKEY.daqthr[2]=20.;  // MC threshold for discr. of "z>2"-trig
  TOFMCFFKEY.daqthr[3]=40.;  // MC threshold for anode Time_over_Thresh(TovT) measurement
  TOFMCFFKEY.daqthr[4]=40.;  // MC threshold for dinode Time_over_thresh measurement
  TOFMCFFKEY.daqpwd[0]=50.;  // pulse width of "z>=1" trig. signal (ns)
  TOFMCFFKEY.daqpwd[1]=50.;  // pulse width of "z>1" trig. signal
  TOFMCFFKEY.daqpwd[2]=50.;  // pulse width of "z>2" trig. signal
  TOFMCFFKEY.daqpwd[3]=10.;  // double pulse resolution of fast(history) TDC (ns)
  TOFMCFFKEY.daqpwd[4]=500.; // min. double pulse resolution of slow TDC (ns)
  TOFMCFFKEY.daqpwd[5]=500.; // dead time of anode TovT measurements (ns)
  TOFMCFFKEY.daqpwd[6]=500.; // dead time of dinode TovT measurements (ns)
  TOFMCFFKEY.daqpwd[7]=2.; // discr. dead time of "z>=1" trig. (ns)
  TOFMCFFKEY.daqpwd[8]=2.; // discr. dead time of "z>1" trig. (ns)
  TOFMCFFKEY.daqpwd[9]=2.; // discr. dead time of "z>2" trig. (ns)
  TOFMCFFKEY.trigtb=0.5;   // MC time binning in logic(trigger) pulse manipulation (ns)
  TOFMCFFKEY.mcprtf=0;     // TOF MC-print flag (=0 -> no_printing)
  TOFMCFFKEY.trlogic[0]=0; // MC trigger logic flag (=0/1-> two-sides-AND/OR of counter) 
  TOFMCFFKEY.trlogic[1]=0; // spare 
FFKEY("TOFMC",&TOFMCFFKEY.TimeSigma,38,"MIXED");
}
//=======================================================================================

void AMSJob::_sictcdata(){
  CTCGEOMFFKEY.wallth=0.03;    // reflecting wall(separators) thickness(cm)
  CTCGEOMFFKEY.agap=0.02;      // typical "air" gaps
  CTCGEOMFFKEY.wgap=0.01;      // typical wls gaps
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
  CTCMCFFKEY.Refraction[0]=1.055;   // Refraction indexes
  CTCMCFFKEY.Refraction[1]=1.59;
  CTCMCFFKEY.Path2PhEl[0]=34;       // Path to photoelectrons conv fact
  CTCMCFFKEY.Path2PhEl[1]=28;
  CTCMCFFKEY.AbsLength[0]=4.9;     // Abs Length in cm
  CTCMCFFKEY.AbsLength[1]=100;
FFKEY("CTCGEOM",&CTCGEOMFFKEY.wallth,15,"MIXED");
FFKEY("CTCMC",CTCMCFFKEY.Refraction,6,"MIXED");
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

TRCLFFKEY.ThrClA[1]=60;
TRCLFFKEY.Thr1A[1] =35;
TRCLFFKEY.Thr2A[1] =10;

TRCLFFKEY.ThrClS[1]=10;
TRCLFFKEY.Thr1S[1] =7;
TRCLFFKEY.Thr2S[1] =7;

TRCLFFKEY.ThrClR[1]=10;
TRCLFFKEY.Thr1R[1] =5;
TRCLFFKEY.Thr2R[1] =3.5;
TRCLFFKEY.Thr3R[1] =-1.;

TRCLFFKEY.ThrClNMin[1]=2;
TRCLFFKEY.ThrClNEl[1]=5;

TRCLFFKEY.ThrClA[0]=60;
TRCLFFKEY.Thr1A[0] =40;
TRCLFFKEY.Thr2A[0] =8;

TRCLFFKEY.ThrClS[0]=10;
TRCLFFKEY.Thr1S[0] =7;
TRCLFFKEY.Thr2S[0] =7;

TRCLFFKEY.ThrClR[0]=10;
TRCLFFKEY.Thr1R[0] =7;
TRCLFFKEY.Thr2R[0] =2.5;
TRCLFFKEY.Thr3R[0] =-1.;

TRCLFFKEY.ThrClNMin[0]=1;
TRCLFFKEY.ThrClNEl[0]=5;


TRCLFFKEY.ThrDSide=1.;

TRCLFFKEY.CorFunParA[0]=400e-4;
TRCLFFKEY.CorFunParA[1]=65e-4;
TRCLFFKEY.CorFunParB[0]=0.85;
TRCLFFKEY.CorFunParB[1]=0.5;


FFKEY("TRCL",TRCLFFKEY.ThrClA,30,"MIXED");

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
FFKEY("TRFIT",(float*)TRFITFFKEY.pattern,30,"MIXED");
TKFINI();
}

void AMSJob::_retofdata(){
  TOFRECFFKEY.Thr1=0.5;
  TOFRECFFKEY.ThrS=1;
  FFKEY("TOFREC",&TOFRECFFKEY.Thr1,2,"MIXED");
}

void AMSJob::_rectcdata(){
  CTCRECFFKEY.Thr1=1.5;
  CTCRECFFKEY.ThrS=3;
  FFKEY("CTCREC",&CTCRECFFKEY.Thr1,2,"MIXED");
}

void AMSJob::_reaxdata(){
// Fit beta
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
FFKEY("BETAFIT",(float*)BETAFITFFKEY.pattern,13,"MIXED");
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

void AMSJob::_sitofinitjob(){
     AMSgObj::BookTimer.book("SITOFDIGI");
}

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

void AMSJob::_retofinitjob(){
AMSgObj::BookTimer.book("RETOFEVENT");
}

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
