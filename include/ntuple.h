#ifndef __AMSNTUPLE__
#define __AMSNTUPLE__

#include <commons.h>
#include <node.h>

const int MAXBETA    =   100;
const int MAXCHARGE  =    10;
const int MAXPART    =    10;
const int MAXTOF     =    20;
const int MAXTOFRAW  =    20;
const int MAXTOFMC   =   200;
const int MAXANTICL  =    16;
const int MAXANTIRAW =    32;
const int MAXANTIMC  =   200;
const int MAXTRCL    =   200;
const int MAXTRRAW   =   500;
const int MAXTRCLMC  =   200;
const int MAXTRRH    =   500;
const int MAXTRTR    =    20;
const int MAXMCG     =    20;
const int MAXCTCCL   =    20;
const int MAXCTCHT   =    50;
const int MAXCTCCLMC =   200;
const int MAXLVL3    =     2;
const int MAXLVL1    =     2;

class EventNtuple {
private:
  int Eventno;
  int Run;
  int RunType;
  int Time[2];
  float GrMedPhi;
  float RadS;
  float ThetaS;
  float PhiS;
  float Yaw;
  float Pitch;
  float Roll;
  float VelocityS;
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

friend class AMSEvent;
friend class AMSNtuple;
};

class BetaNtuple {
private:
  int Nbeta;
  int Status[MAXBETA];
  int Pattern[MAXBETA];
  float Beta[MAXBETA];
  float Error[MAXBETA];
  float Chi2[MAXBETA];
  float Chi2S[MAXBETA];
  int NTOF[MAXBETA];
  int pTOF[MAXBETA][4];
  int pTr[MAXBETA];

friend class AMSBeta;
friend class AMSNtuple;
};

class ChargeNtuple {
private:
  int Ncharge;
  int Status[MAXCHARGE];
  int BetaP[MAXCHARGE];
  int ChargeTOF[MAXCHARGE];
  int ChargeTracker[MAXCHARGE];
  float ProbTOF[MAXCHARGE][7];
  float ProbTracker[MAXCHARGE][7];

friend class AMSCharge;
friend class AMSNtuple;
};


class ParticleNtuple {
private:
  int Npart;
  int CTCP[MAXPART][2];
  int BetaP[MAXPART];
  int ChargeP[MAXPART];
  int TrackP[MAXPART];
  int Particle[MAXPART];
  float Mass[MAXPART];
  float ErrMass[MAXPART];
  float Momentum[MAXPART];
  float ErrMomentum[MAXPART];
  float Charge[MAXPART];
  float Theta[MAXPART];
  float Phi[MAXPART];
  float Coo[MAXPART][3];
  float Value[3][MAXPART][2];
  float CooCTC[MAXPART][2][3];
  float TOFCoo[MAXPART][4][3];
  float AntiCoo[MAXPART][2][3];
  float TrCoo[MAXPART][6][3];
friend class AMSParticle;
friend class AMSNtuple;
};


class TOFClusterNtuple {
private:
  int Ntof;
  int Status[MAXTOF];
  int Layer[MAXTOF];
  int Bar[MAXTOF];
  float Edep[MAXTOF];
  float Time[MAXTOF];
  float ErrTime[MAXTOF];
  float Coo[MAXTOF][3];
  float ErrorCoo[MAXTOF][3];

friend class AMSTOFCluster;
friend class AMSNtuple;
};

class TOFMCClusterNtuple {
private:
  int Ntofmc;
  int Idsoft[MAXTOFMC];
  float Coo[MAXTOFMC][3];
  float TOF[MAXTOFMC];
  float Edep[MAXTOFMC];

friend class AMSTOFMCCluster;
friend class AMSNtuple;
};

class TrClusterNtuple {
private:
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
};

class TrMCClusterNtuple {
private:
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
};


class TrRecHitNtuple {
private:
  int Ntrrh;
  int pX[MAXTRRH];
  int pY[MAXTRRH];
  int Status[MAXTRRH];
  int Layer[MAXTRRH];
  float Hit[MAXTRRH][3];
  float EHit[MAXTRRH][3];
  float Sum[MAXTRRH];
  float DifoSum[MAXTRRH];

friend class AMSTrRecHit;
friend class AMSNtuple;
};

class TrTrackNtuple {
private:
  int Ntrtr;
  int Status[MAXTRTR];
  int Pattern[MAXTRTR];
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
};

class MCEventGNtuple {
private:
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
};


class CTCClusterNtuple {
private:
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
};

class CTCMCClusterNtuple {
private:
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
};

class AntiClusterNtuple {
private:
  int Nanti;
  int Status[MAXANTICL];
  int Sector[MAXANTICL];
  float Edep[MAXANTICL];
  float Coo[MAXANTICL][3];   // R, phi, Z
  float ErrorCoo[MAXANTICL][3];

friend class AMSAntiCluster;
friend class AMSNtuple;
};

class ANTIMCClusterNtuple {
private:
  int Nantimc;
  int Idsoft[MAXANTIMC];
  float Coo[MAXANTIMC][3];
  float TOF[MAXANTIMC];
  float Edep[MAXANTIMC];

friend class AMSAntiMCCluster;
friend class AMSNtuple;
};

class LVL3Ntuple {
private:
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
};

class LVL1Ntuple {
private:
  int Nlvl1;
  int Mode[MAXLVL1];
  int TOFlag[MAXLVL1];
  int TOFPatt[MAXLVL1][4];
  int TOFPatt1[MAXLVL1][4];
  int AntiPatt[MAXLVL1];

friend class TriggerLVL1;
friend class AMSNtuple;
};

class CTCHitNtuple {
private:
  int Nctcht;
  int Status[MAXCTCHT];
  int Layer[MAXCTCHT];
  int Column[MAXCTCHT];
  int Row[MAXCTCHT];
  float Signal[MAXCTCHT];


friend class AMSCTCRawHit;
friend class AMSNtuple;
};

class TrRawClusterNtuple {
private:
  int Ntrraw;
  int address[MAXTRRAW];
  int nelem[MAXTRRAW];
  float s2n[MAXTRRAW];
friend class AMSTrRawCluster;
friend class AMSNtuple;
};

class AntiRawClusterNtuple {
private:
  int Nantiraw;
  int Status[MAXANTIRAW];
  int Sector[MAXANTIRAW];
  int UpDown[MAXANTIRAW];
  float Signal[MAXANTIRAW];

friend class AMSAntiRawCluster;
friend class AMSNtuple;
};

class TOFRawClusterNtuple {
private:
  int Ntofraw;
  int Status[MAXTOFRAW];
  int Layer[MAXTOFRAW];
  int Bar[MAXTOFRAW];
  float tovta[MAXTOFRAW][2];
  float tovtd[MAXTOFRAW][2];
  float sdtm[MAXTOFRAW][2];

friend class AMSTOFRawCluster;
friend class AMSNtuple;
};

class AMSNtuple : public AMSNode {

protected:
  integer _lun;
  EventNtuple _event;
  BetaNtuple _beta;
  ChargeNtuple _charge;
  ParticleNtuple _part;
  TOFClusterNtuple _tof;
  TOFMCClusterNtuple _tofmc;
  TrClusterNtuple _trcl;
  TrMCClusterNtuple _trclmc;
  TrRecHitNtuple _trrh;
  TrTrackNtuple _trtr;
  MCEventGNtuple _mcg;
  CTCClusterNtuple _ctccl;
  CTCMCClusterNtuple _ctcclmc;
  AntiClusterNtuple _anti;
  ANTIMCClusterNtuple _antimc;
  LVL3Ntuple _lvl3;
  LVL1Ntuple _lvl1;
  CTCHitNtuple _ctcht;
  TrRawClusterNtuple _trraw;
  AntiRawClusterNtuple _antiraw;
  TOFRawClusterNtuple _tofraw;

  virtual void _init(){};

public:

  AMSNtuple(integer lun, char* name);
  void reset();
  void write();

  EventNtuple* Get_event() {return &_event;}
  BetaNtuple* Get_beta() { return &_beta;}
  ChargeNtuple* Get_charge() { return &_charge;}
  ParticleNtuple* Get_part() {return &_part;}
  TOFClusterNtuple* Get_tof() {return &_tof;}
  TOFMCClusterNtuple* Get_tofmc() {return &_tofmc;}
  TrClusterNtuple* Get_trcl() {return &_trcl;}
  TrMCClusterNtuple* Get_trclmc() {return &_trclmc;}
  TrRecHitNtuple* Get_trrh() {return &_trrh;}
  TrTrackNtuple* Get_trtr() {return &_trtr;}
  MCEventGNtuple* Get_mcg() {return &_mcg;}
  CTCClusterNtuple* Get_ctccl() { return &_ctccl;}
  CTCMCClusterNtuple* Get_ctcclmc() {return &_ctcclmc;}
  AntiClusterNtuple* Get_anti() {return &_anti;}
  ANTIMCClusterNtuple* Get_antimc() {return &_antimc;}
  LVL3Ntuple* Get_lvl3() {return &_lvl3;}
  LVL1Ntuple* Get_lvl1() {return &_lvl1;}
  CTCHitNtuple* Get_ctcht() {return &_ctcht;}
  TrRawClusterNtuple* Get_trraw() {return &_trraw;}
  AntiRawClusterNtuple* Get_antiraw() {return &_antiraw;}
  TOFRawClusterNtuple* Get_tofraw() {return &_tofraw;}

};

#endif
