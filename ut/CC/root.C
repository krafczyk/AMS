//  $Id: root.C,v 1.32 2003/01/07 18:39:06 jorgec Exp $
//  Last Edit : Nov 19, 2002. A.Klimentov
//              check Root classes
//              ? TrGammaRoot02 - commented
//
#include <root.h>
#include <ntuple.h>
#include <antirec02.h>
#include <beta.h>
#include <ecalrec.h>
#include <mccluster.h>
#include <mceventg.h>
#include <particle.h>
#include <richrec.h>
#include <tofrec02.h>
#include <trdrec.h>
#include <trigger102.h>
#include <trigger302.h>
#include <trrawcluster.h>
#include <trrec.h>
//-- #include <gamma.h>

#ifdef __WRITEROOT__

ClassImp(AMSEventHeaderRoot)
ClassImp(EventRoot02)

ClassImp(BetaRoot02)
ClassImp(ChargeRoot02)
ClassImp(ParticleRoot02)
ClassImp(TOFClusterRoot)
ClassImp(EcalShowerRoot)
ClassImp(EcalClusterRoot)
ClassImp(Ecal2DClusterRoot)
ClassImp(EcalHitRoot)
ClassImp(TOFMCClusterRoot)
ClassImp(TrClusterRoot)
ClassImp(TrMCClusterRoot)
ClassImp(TRDMCClusterRoot)
ClassImp(TRDRawHitRoot)
ClassImp(TRDClusterRoot)
ClassImp(TRDSegmentRoot)
ClassImp(TRDTrackRoot)
ClassImp(TrRecHitRoot02)
ClassImp(TrTrackRoot02)
//-- ClassImp(TrGammaRoot02)
ClassImp(MCTrackRoot)
ClassImp(MCEventGRoot02)
ClassImp(AntiClusterRoot)
ClassImp(ANTIMCClusterRoot)
ClassImp(LVL3Root02)
ClassImp(LVL1Root02)
ClassImp(TrRawClusterRoot)
ClassImp(AntiRawClusterRoot)
ClassImp(TOFRawClusterRoot)
ClassImp(RICMCRoot)
ClassImp(RICEventRoot)
ClassImp(RICRingRoot)

//------------------- constructors -----------------------

AntiClusterRoot::AntiClusterRoot(){};
AntiClusterRoot::AntiClusterRoot(AMSAntiCluster *ptr)
{
  Status = ptr->_status;
  Sector = ptr->_sector;
  Edep   = ptr->_edep;
  for (int i=0; i<3; i++) Coo[i] = ptr->_coo[i];
  for (int i=0; i<3; i++) ErrorCoo[i] = ptr->_ecoo[i];
}

ANTIMCClusterRoot::ANTIMCClusterRoot(){};
ANTIMCClusterRoot::ANTIMCClusterRoot(AMSAntiMCCluster *ptr)
{
  Idsoft = ptr->_idsoft;
  for (int i=0; i<3; i++) Coo[i]=ptr->_xcoo[i];
  TOF    = ptr->_tof;
  Edep   = ptr->_edep;
}

AntiRawClusterRoot::AntiRawClusterRoot(){};
AntiRawClusterRoot::AntiRawClusterRoot(AMSAntiRawCluster *ptr)
{

  Status = ptr->_status;
  Sector = ptr->_sector;
  UpDown = ptr->_updown;
  Signal = ptr->_signal;
}

BetaRoot02::BetaRoot02()
{
  fTrTrack    = 0;
  fTOFCluster =  new TRefArray;
}
BetaRoot02::~BetaRoot02(){
  delete fTOFCluster; fTOFCluster = 0;
}
BetaRoot02::BetaRoot02(AMSBeta *ptr)
{
  Status   = ptr->_status;
  Pattern  = ptr->_Pattern;
  Beta     = ptr->_Beta;
  BetaC    = ptr->_BetaC;
  Error    = ptr->_InvErrBeta;
  ErrorC   = ptr->_InvErrBetaC;
  Chi2     = ptr->_Chi2;
  Chi2S    = ptr->_Chi2Space;
  //
  fTrTrack    = 0;
  fTOFCluster =  new TRefArray;
  //
}

ChargeRoot02::ChargeRoot02() { fBeta = 0;}
ChargeRoot02::~ChargeRoot02() {};
ChargeRoot02::ChargeRoot02(AMSCharge *ptr, float probtof[],int chintof[], 
                           float probtr[], int chintr[], float probrc[],
                           int chinrc[], float proballtr)
{
  Status = ptr->_status;
  BetaP  = ptr->_pbeta->getpos();
  RichP  = ptr->_pring?ptr->_pring->getpos():-1;
  ChargeTOF     = ptr->_ChargeTOF;
  ChargeTracker = ptr->_ChargeTracker;
  ChargeRich    = ptr->_ChargeRich;
  for (int i=0; i<4; i++) {
    ProbTOF[i] = probtof[i];
    ChInTOF[i] = chintof[i];
    ProbTracker[i] = probtr[i];
    ChInTracker[i] = chintr[i];
    ProbRich[i] = probrc[i];
    ChInRich[i] = chinrc[i];
  }
  ProbAllTracker= proballtr;
  TrunTOF       = ptr->_TrMeanTOF;
  TrunTOFD      = ptr->_TrMeanTOFD;
  TrunTracker   = ptr->_TrMeanTracker;
  fBeta         = 0;
}

Ecal2DClusterRoot::Ecal2DClusterRoot(){ fEcal1DCluster = new TRefArray;}
Ecal2DClusterRoot::~Ecal2DClusterRoot(){ delete fEcal1DCluster; fEcal1DCluster=0;}
Ecal2DClusterRoot::Ecal2DClusterRoot(Ecal2DCluster *ptr) 
{
  Status = ptr->getstatus();
  Proj   = ptr->_proj;
  Nmemb  = ptr->_NClust;
  Edep   = ptr->_EnergyC;
  Coo    = ptr->_Coo;
  Tan    = ptr->_Tan;
  Chi2   = ptr->_Chi2;

  fEcal1DCluster = new TRefArray;
}

EcalClusterRoot::EcalClusterRoot()  {fEcalHit = new TRefArray;}
EcalClusterRoot::~EcalClusterRoot() {delete fEcalHit; fEcalHit = 0;}
EcalClusterRoot::EcalClusterRoot(Ecal1DCluster *ptr)
{
  Status  = ptr->_status;
  Proj    = ptr->_proj;
  Plane   = ptr->_plane;
  Left    = ptr->_Left;
  Center  = ptr->_MaxCell;
  Right   = ptr->_Right;
  Edep    = ptr->_EnergyC;
  SideLeak = ptr->_SideLeak;
  DeadLeak = ptr->_DeadLeak;
  for (int i=0; i<3; i++) {Coo[i] = ptr->_Coo[i];}
  NHits = ptr->_NHits;

  fEcalHit = new TRefArray;
}

EcalShowerRoot::~EcalShowerRoot()
{
  if (fEcal2DCluster) {
    delete fEcal2DCluster;
  }
  fEcal2DCluster=0;
}
EcalShowerRoot::EcalShowerRoot() {   
fEcal2DCluster = new TRefArray;
};
EcalShowerRoot::EcalShowerRoot(EcalShower *ptr)
{
  Status      = ptr->_status;
  for (int i=0; i<3; i++) {
   Dir[i]   = ptr->_Dir[i];
   EMDir[i] = ptr->_EMDir[i];
   Entry[i] = ptr->_EntryPoint[i];
   Exit[i]  = ptr->_ExitPoint[i];
   CofG[i]  = ptr->_CofG[i];
  }
  ErTheta   = ptr->_Angle3DError;
  Chi2Dir   = ptr->_AngleTrue3DChi2;
  FirstLayerEdep = ptr->_FrontEnergyDep;
  EnergyC   =   ptr->_EnergyC;
  Energy3C[0] = ptr->_Energy3C;
  Energy3C[1] = ptr->_Energy5C;
  Energy3C[2] = ptr->_Energy9C;
  ErEnergyC   = ptr->_ErrEnergyC;
  DifoSum     = ptr->_DifoSum;
  SideLeak    = ptr->_SideLeak;
  RearLeak    = ptr->_RearLeak;
  DeadLeak    = ptr->_DeadLeak;
  AttLeak     = ptr->_AttLeak;
  OrpLeak     = ptr->_OrpLeak;
  Orp2DEnergy = ptr->_Orp2DEnergy;
  Chi2Profile = ptr->_ProfilePar[4+ptr->_Direction*5];
  for (int i=0; i<4; i++) ParProfile[i] = ptr->_ProfilePar[i+ptr->_Direction*5];
  Chi2Trans = ptr->_TransFitChi2;
  for (int i=0; i<3; i++) SphericityEV[i] = ptr->_SphericityEV[i];

  N2dCl       = ptr->_N2dCl;

  fEcal2DCluster = new TRefArray;

}

LVL1Root02::LVL1Root02(){};
LVL1Root02::LVL1Root02(Trigger2LVL1 *ptr)
{
  Mode   = ptr->_LifeTime;
  TOFlag = ptr->_tofflag;
  for (int i=0; i<4; i++) {
    TOFPatt[i]  = ptr->_tofpatt[i];
    TOFPatt1[i] = ptr->_tofpatt1[i];
  }
  AntiPatt = ptr->_antipatt;
  ECALflag = ptr->_ecalflag;
  ECALtrsum= ptr->_ectrsum;
}

LVL3Root02::LVL3Root02(){};
LVL3Root02::LVL3Root02(TriggerLVL302 *ptr)
{
  TOFTr     = ptr->_TOFTrigger;
  TRDTr     = ptr->_TRDTrigger;
  TrackerTr = ptr->_TrackerTrigger;
  MainTr    = ptr->_MainTrigger;
  Direction = ptr->_TOFDirection;
  NTrHits   = ptr->_NTrHits;
  NPatFound = ptr->_NPatFound;
  for (int i=0; i<2; i++) {Pattern[i] = ptr->_Pattern[i];}
  for (int i=0; i<2; i++) {Residual[i]= ptr->_Residual[i];}
  Time      = ptr->_Time;
  ELoss     = ptr->_TrEnergyLoss;
  TRDHits   = ptr->TRDAux._NHits[0]+ptr->TRDAux._NHits[1];;
  HMult     = ptr->TRDAux._HMult;;
  for (int i=0; i<2; i++) {TRDPar[i] = ptr->TRDAux._Par[i][0];}
  ECemag    = ptr->_ECemag;
  ECmatc    = ptr->_ECmatc;
  for (int i=0; i<4; i++) {ECTOFcr[i] = ptr->_ECtofcr[i];}

}

MCEventGRoot02::MCEventGRoot02(){};
MCEventGRoot02::MCEventGRoot02(AMSmceventg *ptr)
{
  Nskip    = ptr->_nskip;
  Particle = ptr->_ipart;
  for (int i=0; i<3; i++) Coo[i] = ptr->_coo[i];
  for (int i=0; i<3; i++) Dir[i] = ptr->_dir[i];
  Momentum = ptr->_mom;
  Mass     = ptr->_mass;
  Charge   = ptr->_charge;
}           

MCTrackRoot::MCTrackRoot(){};
MCTrackRoot::MCTrackRoot(AMSmctrack *ptr)
{
  _radl = ptr->_radl;
  _absl = ptr->_absl;
  for (int i=0; i<3; i++) _pos[i]   = ptr->_pos[i];
  for (int i=0; i<4; i++) _vname[i] = ptr->_vname[i];
}

ParticleRoot02::ParticleRoot02(){ 
  fBeta   = 0;
  fCharge = 0;
  fTrack  = 0;
  fTRD    = 0;
  fRich   = 0;
  fShower = 0;
};

ParticleRoot02::ParticleRoot02(AMSParticle *ptr, float phi, float phigl)
{
  Phi      = phi;
  PhiGl    = phigl;

  fBeta   = 0;
  fCharge = 0;
  fTrack  = 0;
  fTRD    = 0;
  fRich   = 0;
  fShower = 0;

  Particle     = ptr->_gpart[0];
  ParticleVice = ptr->_gpart[1];
  for (int i=0; i<2; i++) {Prob[i] = ptr->_prob[i];}
  FitMom   = ptr->_fittedmom[0];
  Mass     = ptr->_Mass;
  ErrMass  = ptr->_ErrMass;
  Momentum = ptr->_Momentum;
  ErrMomentum = ptr->_ErrMomentum;
  Beta     = ptr->_Beta;
  ErrBeta  = ptr->_ErrBeta;
  Charge   = ptr->_Charge;
  Theta    = ptr->_Theta;
  ThetaGl  = ptr->_ThetaGl;
  for (int i=0; i<3; i++) {Coo[i] = ptr->_Coo[i];}
  Cutoff   = ptr->_CutoffMomentum;
  for (int i=0; i<4; i++) {
    for (int j=0; j<3; j++) {
      TOFCoo[i][j] = ptr->_TOFCoo[i][j];
    }
  }
  for (int i=0; i<2; i++) {
    for (int j=0; j<3; j++) {
      AntiCoo[i][j] = ptr->_AntiCoo[i][j];
    }
  }
  for (int i=0; i<3; i++) {
    for (int j=0; j<3; j++) {
      EcalCoo[i][j] = ptr->_EcalSCoo[i][j];
    }
  }
  for (int i=0; i<8; i++) {
    for (int j=0; j<3; j++) {
      TrCoo[i][j] = ptr->_TrCoo[i][j];
    }
  }
  for (int i=0; i<8; i++)  Local[i] = ptr->_Local[i];

  for (int i=0; i<3; i++) {TRDCoo[i] = ptr->_TRDCoo[i];}
  for (int i=0; i<2; i++) {
    for (int j=0; j<3; j++) {
      RichCoo[i][j] = ptr->_RichCoo[i][j];
    }
  }
  for (int i=0; i<2; i++) {RichPath[i] = ptr->_RichPath[i];
                           RichPathBeta[i] = ptr->_RichPathBeta[i];}
  RichLength = ptr->_RichLength;

  TRDLikelihood = ptr->_TRDLikelihood;
}




TOFClusterRoot::TOFClusterRoot() {};
TOFClusterRoot::TOFClusterRoot(AMSTOFCluster *ptr, int p2memb[])
{
  Status = ptr->_status;
  Layer  = ptr->_ntof;
  Bar    = ptr->_plane;
  Nmemb  = ptr->_nmemb;
  for (int i=0; i<3; i++) {
    P2memb[i] = p2memb[i];
  }
  Edep   = ptr->_edep;
  Edepd  = ptr->_edepd;
  Time   = ptr->_time;
  ErrTime= ptr->_etime;
  for (int i=0; i<3; i++) {
    Coo[i] = ptr->_Coo[i];
    ErrorCoo[i] = ptr->_ErrorCoo[i];
  }
}

TOFMCClusterRoot::TOFMCClusterRoot() {};
TOFMCClusterRoot::TOFMCClusterRoot(AMSTOFMCCluster *ptr)
{
  Idsoft = ptr->idsoft;
  for (int i=0; i<3; i++) {Coo[i] = ptr->xcoo[i];}
  TOF = ptr->tof;
  Edep= ptr->edep;
}

TOFRawClusterRoot::TOFRawClusterRoot(){};
TOFRawClusterRoot::TOFRawClusterRoot(TOF2RawCluster *ptr)
{

  Status = ptr->_status;
  Layer  = ptr->_ntof;
  Bar    = ptr->_plane;
  for (int i=0; i<2; i++) tovta[i]=ptr->_adca[i];
  for (int i=0; i<2; i++) tovtd[i]=ptr->_adcd[i];
  for (int i=0; i<2; i++) sdtm[i] =ptr->_sdtm[i];
  edepa  = ptr->_edepa;
  edepd  = ptr->_edepd;
  time   = ptr->_time;
  cool   = ptr->_timeD;
}

TRDClusterRoot::TRDClusterRoot(){fTRDRawHit = 0;};
TRDClusterRoot::TRDClusterRoot(AMSTRDCluster *ptr)
{
  Status = ptr->_status;
  Layer  = ptr->_layer;
  for (int i=0; i<3; i++) Coo[i]= ptr->_Coo[i];
  for (int i=0; i<3; i++) CooDir[i]= ptr->_CooDir[i];
  Multip = ptr->_Multiplicity;
  HMultip= ptr->_HighMultiplicity;
  EDep   = ptr->_Edep;
  
  fTRDRawHit = 0;
}

TRDMCClusterRoot::TRDMCClusterRoot(){};
TRDMCClusterRoot::TRDMCClusterRoot(AMSTRDMCCluster *ptr)
{
  Layer  = ptr->_idsoft.getlayer();
  Ladder = ptr->_idsoft.getladder();
  Tube   = ptr->_idsoft.gettube();
  TrackNo= ptr->_itra;
  Edep   = ptr->_edep;
  Ekin   = ptr->_ekin;
  for (int i=0; i<3; i++) {Xgl[i] = ptr->_xgl[i];}
  Step   = ptr->_step;
}

TRDRawHitRoot::TRDRawHitRoot(){};
TRDRawHitRoot::TRDRawHitRoot(AMSTRDRawHit *ptr)
{
  Layer  = ptr->_id.getlayer();
  Ladder = ptr->_id.getladder();
  Tube   = ptr->_id.gettube();
  Amp    = ptr->Amp();
}

TRDSegmentRoot::TRDSegmentRoot(){   fTRDCluster =  new TRefArray;}
TRDSegmentRoot::~TRDSegmentRoot(){   delete fTRDCluster;  fTRDCluster = 0;}
TRDSegmentRoot::TRDSegmentRoot(AMSTRDSegment *ptr)
{
  Status        = ptr->_status;
  Orientation   = ptr->_Orientation;
  for (int i=0; i<2; i++) {FitPar[i] = ptr->_FitPar[i];}
  Chi2          = ptr->_Chi2;
  Pattern       = ptr->_Pattern;
  Nhits         = ptr->_NHits;

  fTRDCluster =  new TRefArray;
}

TRDTrackRoot::TRDTrackRoot(){fTRDSegment =  new TRefArray;};
TRDTrackRoot::~TRDTrackRoot(){delete fTRDSegment; fTRDSegment = 0;};
TRDTrackRoot::TRDTrackRoot(AMSTRDTrack *ptr)
{
  Status = ptr->_status;
  for (int i=0; i<3; i++) {
    Coo[i]   = ptr->_StrLine._Coo[i];
    ErCoo[i] = ptr->_StrLine._ErCoo[i];
  }
  Phi   = ptr->_StrLine._Phi;
  Theta = ptr->_StrLine._Theta;
  Chi2  = ptr->_StrLine._Chi2;
  NSeg  = ptr->_BaseS._NSeg;
  Pattern = ptr->_BaseS._Pattern;

  fTRDSegment =  new TRefArray;
}

TrClusterRoot::TrClusterRoot(){};
TrClusterRoot::TrClusterRoot(AMSTrCluster *ptr, float amplitude[])
{
  Idsoft = ptr-> _Id.cmpt();
  Status = ptr->_status;
  NelemL = ptr->_NelemL;
  NelemR = ptr->_NelemR;
  Sum    = ptr->_Sum;
  Sigma  = ptr->_Sigma;
  Mean   = ptr->_Mean;
  RMS    = ptr->_Rms;
  ErrorMean = ptr->_ErrorMean;
  for (int i=0; i<5; i++) {Amplitude[i] = amplitude[i];}
}

TrMCClusterRoot::TrMCClusterRoot(){};
TrMCClusterRoot::TrMCClusterRoot(AMSTrMCCluster *ptr)
{
  Idsoft  = ptr->_idsoft;
  TrackNo = ptr->_itra;
  for (int i=0; i<2; i++) {
         Left[i]   = ptr->_left[i];
         Center[i] = ptr->_center[i];
         Right[i]  = ptr->_right[i];
  }
  for (int i=0; i<3; i++) {
    Xca[i] = ptr->_xca[i];
    Xcb[i] = ptr->_xcb[i];
    Xgl[i] = ptr->_xgl[i];
  }
  Sum = ptr->_sum;
  for (int i=0; i<2; i++) {
    for (int j=0; j<5; j++) {
      SS[i][j] = ptr->_ss[i][j];
    }
  }
}

TrRawClusterRoot::TrRawClusterRoot(){};
TrRawClusterRoot::TrRawClusterRoot(AMSTrRawCluster *ptr, int addr)
{
  address = addr;
  nelem   = ptr->_nelem;
  s2n     = ptr->_s2n;
}

TrRecHitRoot02::TrRecHitRoot02(){
  fTrClusterX = 0;
  fTrClusterY = 0;
};
TrRecHitRoot02::TrRecHitRoot02(AMSTrRecHit *ptr)
{
  fTrClusterX = 0;
  fTrClusterY = 0;

  Status= ptr->_status;
  Layer = ptr->_Layer;
  for (int i=0; i<3; i++) Hit[i]  = ptr->_Hit[i];
  for (int i=0; i<3; i++) EHit[i] = ptr->_EHit[i];
  Sum     = ptr->_Sum;
  DifoSum = ptr->_DifoSum;
  CofgX   = ptr->_cofgx;
  CofgY   = ptr->_cofgy;
}

TrTrackRoot02::TrTrackRoot02(){ fTrRecHit = new TRefArray;};
TrTrackRoot02::~TrTrackRoot02(){ delete fTrRecHit; fTrRecHit=0;}
TrTrackRoot02::TrTrackRoot02(AMSTrTrack *ptr)
{
  Status    = ptr->_status;
  Pattern   = ptr->_Pattern;
  Address   = ptr->_Address;
  NHits     = ptr->_NHits;

  LocDBAver       = ptr->_Dbase[0];
  GeaneFitDone    = ptr->_GeaneFitDone;
  AdvancedFitDone = ptr->_AdvancedFitDone;
  Chi2StrLine     = ptr->_Chi2StrLine;
  Chi2Circle      = ptr->_Chi2Circle;
  CircleRigidity  = ptr->_CircleRidgidity;
  Chi2FastFit     = ptr->_Chi2FastFit;
  Rigidity        = ptr->_Ridgidity;
  ErrRigidity     = ptr->_ErrRidgidity;
  Theta           = ptr->_Theta;
  Phi             = ptr->_Phi;
  for (int i=0; i<3; i++) P0[i] = ptr->_P0[i];
  GChi2           = (float)ptr->_GChi2;
  GRigidity       = (float)ptr->_GRidgidity;;
  GErrRigidity    = (float)ptr->_GErrRidgidity;
  for (int i=0; i<2; i++) {
        HChi2[i]        = (float)ptr->_HChi2[i];
        HRigidity[i]    = (float)ptr->_HRidgidity[i];
        HErrRigidity[i] = (float)ptr->_HErrRidgidity[i];
        HTheta[i]       = (float)ptr->_HTheta[i];
        HPhi[i]         = (float)ptr->_HPhi[i];
        for (int j=0; j<3; j++)  HP0[i][j] = (float)ptr->_HP0[i][j];
  }
  FChi2MS         = ptr->_Chi2MS;
  PiErrRig        = ptr->_PIErrRigidity;
  RigidityMS      = ptr->_RidgidityMS;
  PiRigidity      = ptr->_PIRigidity;

  fTrRecHit = new TRefArray;
}


RICMCRoot::RICMCRoot(){};
RICMCRoot::RICMCRoot(AMSRichMCHit *ptr, int _numgen)
{
  id        = ptr->_id;
  for (int i=0; i<i; i++) {
   origin[i]    = ptr->_origin[i];
   direction[i] = ptr->_direction[i];
  }
  status       = ptr->_status;
  eventpointer = ptr->_hit;
  numgen       = _numgen;
}

RICEventRoot::RICEventRoot(){};
RICEventRoot::RICEventRoot(AMSRichRawEvent *ptr, float x, float y)
{
  if (ptr) {
   _channel = ptr->_channel;
   _counts  = ptr->_counts;
   _status  = ptr->_status;
   _npe     = ptr->getnpe();
   _x      = x;
   _y      = y;
  } else {
    cout<<"RICEventRoot -E- AMSRichRawEvent ptr is NULL"<<endl;
  }
}
RICRingRoot::RICRingRoot(){ fTrack = 0;}
RICRingRoot::RICRingRoot(AMSRichRing *ptr) 
{
  fTrack = 0;
  if (ptr) {
    used  = ptr->_used;
    mused = ptr->_mused;
    beta  = ptr->_beta;
    errorbeta = ptr->_errorbeta;
    quality   = ptr->_quality;
    status    = ptr->_status;
    // betablind=ptr->_betablind;
    collected_npe= ptr->_collected_npe;
    npexp     = ptr->_npexp;
    probkl    = ptr->_probkl;
   
    npexpg    = ptr->_npexpg;
    npexpr    = ptr->_npexpr;
    npexpb    = ptr->_npexpb;

  } else {
    cout<<"RICRingRoot -E- AMSRichRing ptr is NULL"<<endl;
  }
}

EcalHitRoot::EcalHitRoot(){};
EcalHitRoot::EcalHitRoot(AMSEcalHit *ptr) 
{
  Status = ptr->_status;
  Idsoft = ptr->_idsoft;
  Proj   = ptr->_proj;
  Plane  = ptr->_plane;
  Cell   = ptr->_cell;
  Edep   = ptr->_edep;
  EdCorr = ptr->_edepc;
  if (Proj) {
   Coo[0]= ptr->_cool;
   Coo[1]= ptr->_coot;
  }
  else{     //<-- x-proj
    Coo[0]= ptr->_coot;
    Coo[1]= ptr->_cool;
  }
  Coo[2]  = ptr->_cooz;

  AttCor  = ptr->_attcor;
  AMSECIdSoft ic(ptr->getid());
  ADC[0] = ptr->_adc[0];
  ADC[1] = ptr->_adc[1]*ic.gethi2lowr();
  ADC[2] = ptr->_adc[2]*ic.getan2dyr();

  Ped[0] = ic.getped(0);
  Ped[1] = ic.getped(1);
  Ped[2] = ic.getpedd();
  Gain   = ic.getgain();

}


void EventRoot02::Set(AMSEvent *ptr, int rawwords) 
{
  if (ptr) {
   Header.Set(ptr, rawwords);
  } else {
    cout<<"EventRoot02 -W- AMSEvent *ptr is NULL"<<endl;
  }
}

void AMSEventHeaderRoot::Set(AMSEvent *ptr, int rawwords)
{
  if (ptr) {

    _Run           = ptr->_run;
    _Time[0]       = ptr->_time;
    _Time[1]       = AMSJob::gethead()->isRealData()?ptr->_usec:ptr->_usec/1000;
    _RunType       = ptr->_runtype;
    _Eventno       = ptr-> _id;
    _EventStatus[0]= ptr->getstatus()[0];
    _EventStatus[1]= ptr->getstatus()[1];
    _RawWords      = rawwords;
    
    _StationRad   = ptr->_StationRad;    
    _StationTheta = ptr->_StationTheta;
    _StationPhi   = ptr->_StationPhi;
    _NorthPolePhi = ptr->_NorthPolePhi;
    _Yaw          = ptr->_Yaw;
    _Pitch        = ptr->_Pitch;
    _Roll         = ptr->_Roll;
    _StationSpeed = ptr->_StationSpeed;
    _SunRad       = ptr->_SunRad;
    _VelTheta     = ptr->_VelTheta;
    _VelPhi       = ptr->_VelPhi;

  }
}

/*
TrGammaRoot02::TrGammaRoot02() {
  fTrTrack = new TRefArray;
}
TrGammaRoot02::~TrGammaRoot02() {
 if (fTrTrack) delete fTrTrack;
 fTrTrack=0;
}

TrGammaRoot02::TrGammaRoot02(AMSTrTrackGamma *ptr)
{
  Pgam     =  ptr->_PGAMM;
  ErrPgam  =  ptr->_ErrPGAMM;
  Massgam  =  ptr->_MGAM;
  Thetagam =  ptr->_PhTheta;
  Phigam   =  ptr->_PhPhi;

  for(int i=0;i<3;i++) {
     Vert[i]= ptr->_Vertex[i];
  }
  Distance    = ptr->_TrackDistance;
  Charge      = ptr->_Charge;

  GammaStatus = ptr->_status;


  PtrLeft = -1;
  if(ptr->_pntTrL->checkstatus(AMSDBc::NOTRACK)) {PtrLeft = -1;}
  if(ptr->_pntTrL->checkstatus(AMSDBc::GAMMALEFT)) {
    PtrLeft = ptr->_pntTrL->getpos();}
  PtrRight = -1;
  if(ptr->_pntTrR->checkstatus(AMSDBc::NOTRACK)) {PtrRight = -1;}
  if(ptr->_pntTrR->checkstatus(AMSDBc::GAMMARIGHT))   {
    PtrRight = ptr->_pntTrR->getpos();
  }
  Jthetal = (geant)ptr->_GThetaMSL;
  Jphil   = (geant)ptr->_GPhiMSL;
  Jthetar = (geant)ptr->_GThetaMSR;
  Jphir   = (geant)ptr->_GPhiMSR;
  for(int i=0;i<3;i++) {Jp0l[i] = (geant)ptr->_GP0MSL[i];}
  for(int i=0;i<3;i++) {Jp0r[i] = (geant)ptr->_GP0MSR[i];}

  fTrTrack = new TRefArray;
}
*/

//------------- AddAMSObject 
void EventRoot02::AddAMSObject(AMSAntiCluster *ptr)
{
  if (ptr) {
  if (fAntiCluster) {
   TClonesArray &clones =  *fAntiCluster;
   ptr->SetClonePointer(new (clones[fAntiCluster->GetLast()+1]) AntiClusterRoot(ptr));
   }
  }  else {
    cout<<"AddAMSObject -E- AMSAntiCluster ptr is NULL"<<endl;
  }
}

void EventRoot02::AddAMSObject(AMSAntiMCCluster *ptr)
{
  if (ptr) {
    if (fAntiMCCluster) {
   TClonesArray &clones =  *fAntiMCCluster;
   ptr->SetClonePointer(new (clones[fAntiMCCluster->GetLast()+1]) ANTIMCClusterRoot(ptr));
    }
  }  else {
    cout<<"AddAMSObject -E- AMSAntiMCCluster ptr is NULL"<<endl;
  }
}

void EventRoot02::AddAMSObject(AMSAntiRawCluster *ptr)
{
  if (ptr) {
    if (fAntiRawCluster) {
   TClonesArray &clones =  *fAntiRawCluster;
   ptr->SetClonePointer(new (clones[fAntiRawCluster->GetLast()+1]) AntiRawClusterRoot(ptr));
    }
  }  else {
    cout<<"AddAMSObject -E- AMSAntiRawCluster ptr is NULL"<<endl;
  }
}

void EventRoot02::AddAMSObject(AMSBeta *ptr)
{
  if (ptr) {
    if (fBeta) {
    TClonesArray &clones =  *fBeta;
     ptr->SetClonePointer(new (clones[fBeta->GetLast()+1]) BetaRoot02(ptr));
    } else {
      //     cout<<"AddAMSObject -W- *fBeta is NULL"<<endl;
    }
   }  else {
     cout<<"AddAMSObject -E- AMSBeta ptr is NULL"<<endl;
  }
}

void EventRoot02::AddAMSObject(AMSCharge *ptr, float probtof[],int chintof[], 
                               float probtr[], int chintr[], float probrc[], 
                               int chinrc[], float proballtr)
{
  if (ptr) {
    if (fCharge) {
   TClonesArray &clones =  *fCharge;
   ptr->SetClonePointer(new (clones[fCharge->GetLast()+1]) 
       ChargeRoot02(ptr, probtof, chintof, probtr, chintr, probrc, chinrc, proballtr)); 
    }
  }  else {
    cout<<"AddAMSObject -E- AMSCharge ptr is NULL"<<endl;
  }
}

void EventRoot02::AddAMSObject(AMSEcalHit *ptr)
{
  if (fECALhit) {
  TClonesArray &clones =  *fECALhit;
  ptr->SetClonePointer(new (clones[fECALhit->GetLast()+1]) EcalHitRoot(ptr));
  //new (fECALhit[fECALhit.GetLast()+1]) EcalHitRoot(ptr);
  //cout<<"  fECALhit->GetLast()+1 "<<fECALhit->GetLast()+1<<endl;
  }
}

void EventRoot02::AddAMSObject(AMSmceventg *ptr)
{
  if (ptr) {
    if (fMCeventg) {
   TClonesArray &clones =  *fMCeventg;
   ptr->SetClonePointer(new (clones[fMCeventg->GetLast()+1]) MCEventGRoot02(ptr));
    }
  }  else {
    cout<<"AddAMSObject -E- AMSmceventg ptr is NULL"<<endl;
  }
}


void EventRoot02::AddAMSObject(AMSmctrack *ptr)
{
  if (ptr) {
    if (fMCtrtrack) {
   TClonesArray &clones =  *fMCtrtrack;
   ptr->SetClonePointer(new (clones[fMCtrtrack->GetLast()+1]) MCTrackRoot(ptr));
    }
  }  else {
    cout<<"AddAMSObject -E- AMSmctrack ptr is NULL"<<endl;
  }
}

void EventRoot02::AddAMSObject(AMSParticle *ptr, float phi, float phigl)
{
  if (ptr) {
    if (fParticle) {
   TClonesArray &clones =  *fParticle;
   ptr->SetClonePointer(new (clones[fParticle->GetLast()+1]) 
       ParticleRoot02(ptr, phi, phigl));
    }
  }  else {
   cout<<"AddAMSObject -E- AMSParticle ptr is NULL"<<endl;
  }
}

void EventRoot02::AddAMSObject(AMSRichRawEvent *ptr, float x, float y)
{
  if (ptr) {
    if (fRICEvent) {
   TClonesArray &clones =  *fRICEvent;
   ptr->SetClonePointer(new (clones[fRICEvent->GetLast()+1]) RICEventRoot(ptr, x, y));
    }
  }  else {
    cout<<"AddAMSObject -E- AMSRichRawEvent ptr is NULL"<<endl;
  }
}

void EventRoot02::AddAMSObject(AMSRichMCHit *ptr, int _numgen)
{
  if (fRICMC) {
  TClonesArray &clones =  *fRICMC;
  ptr->SetClonePointer(new (clones[fRICMC->GetLast()+1]) RICMCRoot(ptr, _numgen));
  }
}
void EventRoot02::AddAMSObject(AMSRichRing *ptr)
{
  if (fRICRing) {
  TClonesArray &clones =  *fRICRing;
  ptr->SetClonePointer(new (clones[fRICRing->GetLast()+1]) RICRingRoot(ptr));
  }
}

void EventRoot02::AddAMSObject(Ecal1DCluster *ptr) 
{
  if (ptr) {
    if (fECALcluster) {
   TClonesArray &clones =  *fECALcluster;
   ptr->SetClonePointer(new (clones[fECALcluster->GetLast()+1]) EcalClusterRoot(ptr));
    }
  }  else {
   cout<<"AddAMSObject -E- Ecal1DCluster ptr is NULL"<<endl;
  }
}

void EventRoot02::AddAMSObject(Ecal2DCluster *ptr) 
{
  if (ptr) {
    if (fECAL2Dcluster) {
   TClonesArray &clones =  *fECAL2Dcluster;
   ptr->SetClonePointer(new (clones[fECAL2Dcluster->GetLast()+1]) Ecal2DClusterRoot(ptr));
    }
  }  else {
   cout<<"AddAMSObject -E- (Ecal2DCluster ptr is NULL"<<endl;
  }
}

void EventRoot02::AddAMSObject(EcalShower *ptr) 
{
  if (ptr) {
    if (fECALshower) {
   TClonesArray &clones =  *fECALshower;
   ptr->SetClonePointer(new (clones[fECALshower->GetLast()+1]) EcalShowerRoot(ptr));
    }
  }  else {
   cout<<"AddAMSObject -E- EcalShower ptr is NULL"<<endl;
  }
}


void EventRoot02::AddAMSObject(Trigger2LVL1 *ptr)
{
  if (ptr) {
    if (fLVL1) {
   TClonesArray &clones =  *fLVL1;
   ptr->SetClonePointer(new (clones[fLVL1->GetLast()+1]) LVL1Root02(ptr));
    }
  }  else {
   cout<<"AddAMSObject -E- Trigger2LVL1 ptr is NULL"<<endl;
  }
}

void EventRoot02::AddAMSObject(TriggerLVL302 *ptr)
{
  if (ptr) {
    if (fLVL3) {
     TClonesArray &clones =  *fLVL3;
     ptr->SetClonePointer(new (clones[fLVL3->GetLast()+1]) LVL3Root02(ptr));
    }
  }  else {
   cout<<"AddAMSObject -E- TriggerLVL302 ptr is NULL"<<endl;
  }
}

void EventRoot02::AddAMSObject(AMSTOFCluster *ptr, int p2memb[])
{
  if (ptr) {
    if (fTOFcluster) {
   TClonesArray &clones =  *fTOFcluster;
   ptr->SetClonePointer(new (clones[fTOFcluster->GetLast()+1]) TOFClusterRoot(ptr,p2memb));
    }
  }  else {
   cout<<"AddAMSObject -E- AMSTOFCluster ptr is NULL"<<endl;
  }
}

void EventRoot02::AddAMSObject(AMSTOFMCCluster *ptr)
{
  if (ptr) {
    if (fTOFMCcluster) {
   TClonesArray &clones =  *fTOFMCcluster;
   ptr->SetClonePointer(new (clones[fTOFMCcluster->GetLast()+1]) TOFMCClusterRoot(ptr));
    }
  }  else {
   cout<<"AddAMSObject -E- AMSTOFMCCluster ptr is NULL"<<endl;
  }
}

void EventRoot02::AddAMSObject(TOF2RawCluster *ptr)
{
  if (ptr) {
    if (fTOFRawCluster) {
   TClonesArray &clones =  *fTOFRawCluster;
   ptr->SetClonePointer(new (clones[fTOFRawCluster->GetLast()+1]) TOFRawClusterRoot(ptr));
    }
  }  else {
   cout<<"AddAMSObject -E- AMSTOF2RawCluster ptr is NULL"<<endl;
  }
}


void EventRoot02::AddAMSObject(AMSTrCluster *ptr, float ampl[])
{
  if (ptr) {
    if (fTrCluster) {
   TClonesArray &clones =  *fTrCluster;
   ptr->SetClonePointer(new (clones[fTrCluster->GetLast()+1]) TrClusterRoot(ptr, ampl));
    }
  }  else {
   cout<<"AddAMSObject -E- AMSTrCluster ptr is NULL"<<endl;
  }
}

void EventRoot02::AddAMSObject(AMSTrMCCluster *ptr)
{
  if (ptr) {
    if (fTrMCCluster) {
   TClonesArray &clones =  *fTrMCCluster;
   ptr->SetClonePointer(new (clones[fTrMCCluster->GetLast()+1]) TrMCClusterRoot(ptr));
    }
  }  else {
   cout<<"AddAMSObject -E- AMSTrMCCluster ptr is NULL"<<endl;
  }
}

void EventRoot02::AddAMSObject(AMSTrRawCluster *ptr, int addr)
{
  if (ptr) {
    if (fTrRawCluster) {
   TClonesArray &clones =  *fTrRawCluster;
   ptr->SetClonePointer(new (clones[fTrRawCluster->GetLast()+1]) TrRawClusterRoot(ptr,addr));
    }
  }  else {
   cout<<"AddAMSObject -E- TrRawCluster ptr is NULL"<<endl;
  }
}


void EventRoot02::AddAMSObject(AMSTRDMCCluster *ptr)
{
  if (ptr) {
    if (fTRDMCCluster) {
   TClonesArray &clones =  *fTRDMCCluster;
   ptr->SetClonePointer(new (clones[fTRDMCCluster->GetLast()+1]) TRDMCClusterRoot(ptr));
    }
  }  else {
   cout<<"AddAMSObject -E- AMSTRDMCCluster ptr is NULL"<<endl;
  }
}

void EventRoot02::AddAMSObject(AMSTRDRawHit *ptr)
{
  if (ptr) {
    if (fTRDrawhit) {
   TClonesArray &clones =  *fTRDrawhit;
   ptr->SetClonePointer(new (clones[fTRDrawhit->GetLast()+1]) TRDRawHitRoot(ptr));
    }
  }  else {
   cout<<"AddAMSObject -E- AMSTRDRawHit ptr is NULL"<<endl;
  }
}

void EventRoot02::AddAMSObject(AMSTRDCluster *ptr)
{
  if (ptr) {
    if (fTRDcluster) {
   TClonesArray &clones =  *fTRDcluster;
   ptr->SetClonePointer(new (clones[fTRDcluster->GetLast()+1]) TRDClusterRoot(ptr));
    }
  }  else {
   cout<<"AddAMSObject -E- AMSTRDCluster ptr is NULL"<<endl;
  }
}

void EventRoot02::AddAMSObject(AMSTRDSegment *ptr)
{
  if (ptr) {
    if (fTRDsegment) {
   TClonesArray &clones =  *fTRDsegment;
   ptr->SetClonePointer(new (clones[fTRDsegment->GetLast()+1]) TRDSegmentRoot(ptr));
    }
  }  else {
   cout<<"AddAMSObject -E- AMSTRDSegment ptr is NULL"<<endl;
  }
}


void EventRoot02::AddAMSObject(AMSTRDTrack *ptr)
{
  if (ptr) {
    if (fTRDtrack) {
   TClonesArray &clones =  *fTRDtrack;
   ptr->SetClonePointer(new (clones[fTRDtrack->GetLast()+1]) TRDTrackRoot(ptr));
    }
  }  else {
   cout<<"AddAMSObject -E- AMSTRDTrack ptr is NULL"<<endl;
  }
}

void EventRoot02::AddAMSObject(AMSTrRecHit *ptr)
{
  if (ptr) {
    if (fTRrechit) {
   TClonesArray &clones =  *fTRrechit;
   ptr->SetClonePointer(new (clones[fTRrechit->GetLast()+1]) TrRecHitRoot02(ptr));
    }
  }  else {
   cout<<"AddAMSObject -E- AMSTrRecHit ptr is NULL"<<endl;
  }
}

/*
void EventRoot02::AddAMSObject(AMSTrTrackGamma *ptr)
{
  if (ptr) {
    if (fTrTrack) {
   TClonesArray &clones =  *fTrTrack;
   ptr->SetClonePointer(new (clones[fTRtrack->GetLast()+1]) TrGammaRoot02(ptr));
    }
  }  else {
   cout<<"AddAMSObject -E- AMSTrTrackGamma ptr is NULL"<<endl;
  }
}
*/

void EventRoot02::AddAMSObject(AMSTrTrack *ptr)
{
  if (ptr) {
    if (fTRtrack) {
   TClonesArray &clones =  *fTRtrack;
   ptr->SetClonePointer(new (clones[fTRtrack->GetLast()+1]) TrTrackRoot02(ptr));
    }
  }  else {
   cout<<"AddAMSObject -E- AMSTrTrack ptr is NULL"<<endl;
  }
}
#endif

