#ifndef __AMSROOT__
#define __AMSROOT__

#ifdef __WRITEROOT__
#include <TObject.h>
#include <TTree.h>
#include <TFile.h>
#include <TROOT.h>
#endif
class AMSNtuple;
const int MAXBETA    =   150;
const int MAXCHARGE  =    150;
const int MAXPART    =    100;
const int MAXTOF     =    20;
const int MAXTOFRAW  =    20;
const int MAXTOFMC   =   200;
const int MAXANTICL  =    16;
const int MAXANTIRAW =    32;
const int MAXANTIMC  =   200;
const int MAXTRCL    =   200;
const int MAXTRRAW   =   300;
const int MAXTRCLMC  =   200;
const int MAXTRRH    =   500;
const int MAXTRTR    =    200;
const int MAXMCG     =    20;
const int MAXCTCCL   =    20;
const int MAXCTCHT   =    50;
const int MAXCTCCLMC =   200;
const int MAXLVL3    =     2;
const int MAXLVL1    =     2;

#ifdef __WRITEROOT__
class EventNtuple: public TObject {
#else
class EventNtuple {
#endif
public:
  int Eventno;
  int Run;
  int RunType;
  int Time[2];
  int RawWords;
  float RadS;
  float ThetaS;
  float PhiS;
  float Yaw;
  float Pitch;
  float Roll;
  float VelocityS;
  float VelTheta;
  float VelPhi;
  float ThetaM;
  float PhiM;
  int Particles;
  int Tracks;
  int Betas;
  int Charges;
  int TrRecHits;
  int TrClusters;
  int TrRawClusters;
  int TrMCClusters;
  int TOFClusters;
  int TOFMCClusters;
  int CTCClusters;
  int CTCMCClusters;
  int AntiMCClusters;
  int AntiClusters;
  int EventStatus; 
  
friend class AMSEvent;
friend class AMSNtuple;
#ifdef __WRITEROOT__
ClassDef(EventNtuple,1)       //EventNtuple
#endif
};

#ifdef __WRITEROOT__
class BetaNtuple : public TObject {
#else
class BetaNtuple {
#endif
public:
  int Nbeta;
  int Status[MAXBETA];
  int Pattern[MAXBETA];
  float Beta[MAXBETA];
  float BetaC[MAXBETA];
  float Error[MAXBETA];
  float ErrorC[MAXBETA];
  float Chi2[MAXBETA];
  float Chi2S[MAXBETA];
  int NTOF[MAXBETA];
  int pTOF[MAXBETA][4];
  int pTr[MAXBETA];

friend class AMSBeta;
friend class AMSNtuple;
#ifdef __WRITEROOT__
ClassDef(BetaNtuple,1)       //BetaNtuple
#endif
};

#ifdef __WRITEROOT__
class ChargeNtuple : public TObject {
#else
class ChargeNtuple {
#endif
public:
  int Ncharge;
  int Status[MAXCHARGE];
  int BetaP[MAXCHARGE];
  int ChargeTOF[MAXCHARGE];
  int ChargeTracker[MAXCHARGE];
  float ProbTOF[MAXCHARGE][4];
  int CharTOF[MAXCHARGE][4];
  float ProbTracker[MAXCHARGE][4];
  int CharTracker[MAXCHARGE][4];
  float ProbAllTracker[MAXCHARGE];
  float TrunTOF[MAXCHARGE];
  float TrunTOFD[MAXCHARGE];
  float TrunTracker[MAXCHARGE];

friend class AMSCharge;
friend class AMSNtuple;
#ifdef __WRITEROOT__
ClassDef(ChargeNtuple,1)       //ChargeNtuple
#endif
};


#ifdef __WRITEROOT__
class ParticleNtuple : public TObject {
#else
class ParticleNtuple {
#endif
public:
  int Npart;
  int   BetaP[MAXPART];
  int   ChargeP[MAXPART];
  int   TrackP[MAXPART];
  int   Particle[MAXPART];
  int   ParticleVice[MAXPART];
  float Prob[MAXPART][2];
  float FitMom[MAXPART];
  float Mass[MAXPART];
  float ErrMass[MAXPART];
  float Momentum[MAXPART];
  float ErrMomentum[MAXPART];
  float Charge[MAXPART];
  float Theta[MAXPART];
  float Phi[MAXPART];
  float ThetaGl[MAXPART];
  float PhiGl[MAXPART];
  float Coo[MAXPART][3];
  int   ATCnbcel[MAXPART][2];
  float ATCnbphe[MAXPART][2];
  int   ATCidcel[MAXPART][2];
  int   ATCdispm[MAXPART][2];
  int   ATCdaero[MAXPART][2];
  int   ATCstatu[MAXPART][2];
  float ATCbeta[MAXPART];
  float CooCTC[MAXPART][2][3];
  float TOFCoo[MAXPART][4][3];
  float AntiCoo[MAXPART][2][3];
  float TrCoo[MAXPART][6][3];
friend class AMSParticle;
friend class AMSNtuple;
#ifdef __WRITEROOT__
ClassDef(ParticleNtuple,1)       //ParticleNtuple
#endif
};


#ifdef __WRITEROOT__
class TOFClusterNtuple : public TObject {
#else
class TOFClusterNtuple {
#endif
public:
  int Ntof;
  int Status[MAXTOF];
  int Layer[MAXTOF];
  int Bar[MAXTOF];
  int Nmemb[MAXTOF];
  float Edep[MAXTOF];
  float Edepd[MAXTOF];
  float Time[MAXTOF];
  float ErrTime[MAXTOF];
  float Coo[MAXTOF][3];
  float ErrorCoo[MAXTOF][3];

friend class AMSTOFCluster;
friend class AMSNtuple;
#ifdef __WRITEROOT__
ClassDef(TOFClusterNtuple,1)       //TOFClusterNtuple
#endif
};

#ifdef __WRITEROOT__
class TOFMCClusterNtuple : public TObject {
#else
class TOFMCClusterNtuple {
#endif
public:
  int Ntofmc;
  int Idsoft[MAXTOFMC];
  float Coo[MAXTOFMC][3];
  float TOF[MAXTOFMC];
  float Edep[MAXTOFMC];

friend class AMSTOFMCCluster;
friend class AMSNtuple;
#ifdef __WRITEROOT__
ClassDef(TOFMCClusterNtuple,1)       //TOFMCClusterNtuple
#endif
};

#ifdef __WRITEROOT__
class TrClusterNtuple : public TObject {
#else
class TrClusterNtuple {
#endif
public:
  int Ntrcl;
  int Idsoft[MAXTRCL];
  int Status[MAXTRCL];
  int NelemL[MAXTRCL];
  int NelemR[MAXTRCL];
  float Sum[MAXTRCL];
  float Sigma[MAXTRCL];
  float Mean[MAXTRCL];
  float RMS[MAXTRCL];
  float ErrorMean[MAXTRCL];
  float Amplitude[MAXTRCL][5];

friend class AMSTrCluster;
friend class AMSNtuple;
#ifdef __WRITEROOT__
ClassDef(TrClusterNtuple,1)       //TrClusterNtuple
#endif
};

#ifdef __WRITEROOT__
class TrMCClusterNtuple : public TObject {
#else
class TrMCClusterNtuple {
#endif
public:
  int Ntrclmc;
  int Idsoft[MAXTRCLMC];
  int TrackNo[MAXTRCLMC];
  int Left[MAXTRCLMC][2];
  int Center[MAXTRCLMC][2];
  int Right[MAXTRCLMC][2];
  float SS[MAXTRCLMC][2][5];
  float Xca[MAXTRCLMC][3];
  float Xcb[MAXTRCLMC][3];
  float Xgl[MAXTRCLMC][3];
  float Sum[MAXTRCLMC];

friend class AMSTrMCCluster;
friend class AMSNtuple;
#ifdef __WRITEROOT__
ClassDef(TrMCClusterNtuple,1)       //TrMCClusterNtuple
#endif
};


#ifdef __WRITEROOT__
class TrRecHitNtuple : public TObject {
#else
class TrRecHitNtuple {
#endif
public:
  int Ntrrh;
  int pX[MAXTRRH];
  int pY[MAXTRRH];
  int Status[MAXTRRH];
  int Layer[MAXTRRH];
  float Hit[MAXTRRH][3];
  float EHit[MAXTRRH][3];
  float Sum[MAXTRRH];
  float DifoSum[MAXTRRH];
  float CofgX[MAXTRRH];
  float CofgY[MAXTRRH];

friend class AMSTrRecHit;
friend class AMSNtuple;
#ifdef __WRITEROOT__
ClassDef(TrRecHitNtuple,1)       //TrRecHitNtuple
#endif
};

#ifdef __WRITEROOT__
class TrTrackNtuple : public TObject {
#else
class TrTrackNtuple {
#endif
public:
  int Ntrtr;
  int Status[MAXTRTR];
  int Pattern[MAXTRTR];
  int Address[MAXTRTR];
  int NHits[MAXTRTR];
  int pHits[MAXTRTR][6];
  int FastFitDone[MAXTRTR];
  int GeaneFitDone[MAXTRTR];
  int AdvancedFitDone[MAXTRTR];
  float Chi2StrLine[MAXTRTR];
  float Chi2Circle[MAXTRTR];
  float CircleRidgidity[MAXTRTR];
  float Chi2FastFit[MAXTRTR];
  float Ridgidity[MAXTRTR];
  float ErrRidgidity[MAXTRTR];
  float Theta[MAXTRTR];
  float Phi[MAXTRTR];
  float P0[MAXTRTR][3];
  float GChi2[MAXTRTR];
  float GRidgidity[MAXTRTR];
  float GErrRidgidity[MAXTRTR];
  float GTheta[MAXTRTR];
  float GPhi[MAXTRTR];
  float GP0[MAXTRTR][3];
  float HChi2[MAXTRTR][2];
  float HRidgidity[MAXTRTR][2];
  float HErrRidgidity[MAXTRTR][2];
  float HTheta[MAXTRTR][2];
  float HPhi[MAXTRTR][2];
  float HP0[MAXTRTR][2][3];
  float FChi2MS[MAXTRTR];
  float GChi2MS[MAXTRTR];
  float RidgidityMS[MAXTRTR];
  float GRidgidityMS[MAXTRTR];

friend class AMSTrTrack;
friend class AMSNtuple;
#ifdef __WRITEROOT__
ClassDef(TrTrackNtuple,1)       //TrTrackNtuple
#endif
};

#ifdef __WRITEROOT__
class MCEventGNtuple : public TObject {
#else
class MCEventGNtuple {
#endif
public:
  int Nmcg;
  int Nskip[MAXMCG];
  int Particle[MAXMCG];
  float Coo[MAXMCG][3];
  float Dir[MAXMCG][3];
  float Momentum[MAXMCG];
  float Mass[MAXMCG];
  float Charge[MAXMCG];

friend class AMSmceventg;
friend class AMSNtuple;
#ifdef __WRITEROOT__
ClassDef(MCEventGNtuple,1)       //MCEventGNtuple
#endif
};


#ifdef __WRITEROOT__
class CTCClusterNtuple : public TObject {
#else
class CTCClusterNtuple {
#endif
public:
  int Nctccl;
  int Status[MAXCTCCL];
  int Layer[MAXCTCCL];
  float Coo[MAXCTCCL][3];
  float ErrCoo[MAXCTCCL][3];
  float RawSignal[MAXCTCCL];
  float Signal[MAXCTCCL];
  float ErrorSignal[MAXCTCCL];

friend class AMSCTCCluster;
friend class AMSNtuple;
#ifdef __WRITEROOT__
ClassDef(CTCClusterNtuple,1)       //CTCClusterNtuple
#endif
};

#ifdef __WRITEROOT__
class CTCMCClusterNtuple : public TObject {
#else
class CTCMCClusterNtuple {
#endif
public:
  int Nctcclmc;
  int Idsoft[MAXCTCCLMC];
  float Coo[MAXCTCCLMC][3];
  float Dir[MAXCTCCLMC][3];
  float Step[MAXCTCCLMC];
  float Charge[MAXCTCCLMC];
  float Beta[MAXCTCCLMC];
  float Edep[MAXCTCCLMC];

friend class AMSCTCMCCluster;
friend class AMSNtuple;
#ifdef __WRITEROOT__
ClassDef(CTCMCClusterNtuple,1)       //CTCMCClusterNtuple
#endif
};

#ifdef __WRITEROOT__
class AntiClusterNtuple : public TObject {
#else
class AntiClusterNtuple {
#endif
public:
  int Nanti;
  int Status[MAXANTICL];
  int Sector[MAXANTICL];
  float Edep[MAXANTICL];
  float Coo[MAXANTICL][3];   // R, phi, Z
  float ErrorCoo[MAXANTICL][3];

friend class AMSAntiCluster;
friend class AMSNtuple;
#ifdef __WRITEROOT__
ClassDef(AntiClusterNtuple,1)       //AntiClusterNtuple
#endif
};

#ifdef __WRITEROOT__
class ANTIMCClusterNtuple : public TObject {
#else
class ANTIMCClusterNtuple {
#endif
public:
  int Nantimc;
  int Idsoft[MAXANTIMC];
  float Coo[MAXANTIMC][3];
  float TOF[MAXANTIMC];
  float Edep[MAXANTIMC];

friend class AMSAntiMCCluster;
friend class AMSNtuple;
#ifdef __WRITEROOT__
ClassDef(ANTIMCClusterNtuple,1)       //ANTIMCClusterNtuple
#endif
};

#ifdef __WRITEROOT__
class LVL3Ntuple : public TObject {
#else
class LVL3Ntuple {
#endif
public:
  int Nlvl3;
  int TOFTr[MAXLVL3];
  int AntiTr[MAXLVL3];
  int TrackerTr[MAXLVL3];
  int NTrHits[MAXLVL3];
  int NPatFound[MAXLVL3];
  int Pattern[MAXLVL3][2];
  float Residual[MAXLVL3][2];
  float Time[MAXLVL3];
  float ELoss[MAXLVL3];

friend class TriggerLVL3;
friend class AMSNtuple;
#ifdef __WRITEROOT__
ClassDef(LVL3Ntuple,1)       //LVL3Ntuple
#endif
};

#ifdef __WRITEROOT__
class LVL1Ntuple : public TObject {
#else
class LVL1Ntuple {
#endif
public:
  int Nlvl1;
  int Mode[MAXLVL1];
  int TOFlag[MAXLVL1];
  int TOFPatt[MAXLVL1][4];
  int TOFPatt1[MAXLVL1][4];
  int AntiPatt[MAXLVL1];

friend class TriggerLVL1;
friend class AMSNtuple;
#ifdef __WRITEROOT__
ClassDef(LVL1Ntuple,1)       //LVL1Ntuple
#endif
};

#ifdef __WRITEROOT__
class CTCHitNtuple : public TObject {
#else
class CTCHitNtuple {
#endif
public:
  int Nctcht;
  int Status[MAXCTCHT];
  int Layer[MAXCTCHT];
  int Column[MAXCTCHT];
  int Row[MAXCTCHT];
  float Signal[MAXCTCHT];


friend class AMSCTCRawHit;
friend class AMSNtuple;
#ifdef __WRITEROOT__
ClassDef(CTCHitNtuple,1)       //CTCHitNtuple
#endif
};

#ifdef __WRITEROOT__
class TrRawClusterNtuple : public TObject {
#else
class TrRawClusterNtuple {
#endif
public:
  int Ntrraw;
  int address[MAXTRRAW];
  int nelem[MAXTRRAW];
  float s2n[MAXTRRAW];
friend class AMSTrRawCluster;
friend class AMSNtuple;
#ifdef __WRITEROOT__
ClassDef(TrRawClusterNtuple,1)       //TrRawClusterNtuple
#endif
};

#ifdef __WRITEROOT__
class AntiRawClusterNtuple : public TObject {
#else
class AntiRawClusterNtuple {
#endif
public:
  int Nantiraw;
  int Status[MAXANTIRAW];
  int Sector[MAXANTIRAW];
  int UpDown[MAXANTIRAW];
  float Signal[MAXANTIRAW];

friend class AMSAntiRawCluster;
friend class AMSNtuple;
#ifdef __WRITEROOT__
ClassDef(AntiRawClusterNtuple ,1)       //AntiRawClusterNtuple 
#endif
};

#ifdef __WRITEROOT__
class TOFRawClusterNtuple : public TObject {
#else
class TOFRawClusterNtuple {
#endif
public:
  int Ntofraw;
  int Status[MAXTOFRAW];
  int Layer[MAXTOFRAW];
  int Bar[MAXTOFRAW];
  float tovta[MAXTOFRAW][2];
  float tovtd[MAXTOFRAW][2];
  float sdtm[MAXTOFRAW][2];
  float edepa[MAXTOFRAW];
  float edepd[MAXTOFRAW];
  float time[MAXTOFRAW];
  float cool[MAXTOFRAW];

friend class AMSTOFRawCluster;
friend class AMSNtuple;
#ifdef __WRITEROOT__
ClassDef(TOFRawClusterNtuple ,1)       //TOFRawClusterNtuple 
#endif
};


#endif
