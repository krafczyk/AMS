//  $Id: root.h,v 1.47 2001/08/24 15:53:09 kscholbe Exp $
#ifndef __AMSROOT__
#define __AMSROOT__

#ifdef __WRITEROOT__
#include <TObject.h>
#include <TTree.h>
#include <TFile.h>
#include <TROOT.h>
#endif
#include <tkdbc.h>
#include <trddbc.h>
#include <ecaldbc.h>
class AMSNtuple;
namespace root{
const int MAXBETA    =   150;
const int MAXBETA02    = 50;
const int MAXCHARGE  =   150;
const int MAXCHARGE02  = 30;
const int MAXPART    =   100;
const int MAXPART02    =  20;
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
const int MAXTRRH02    =   666;
const int MAXTRTR    =   200;
const int MAXTRTR02    =   100;
const int MAXMCG     =    20;
const int MAXMCG02     =    100;
const int MAXCTCCL   =    20;
const int MAXCTCHT   =    50;
const int MAXCTCCLMC =   200;
const int MAXECSHOW =    5;
const int MAXECCLUST =    60;
const int MAXEC2DCLUST =    10;
const int MAXECHITS  =   500;
const int MAXLVL3    =     2;
const int MAXLVL1    =     2;
const int MAXRICMC   =   300;
const int MAXRICHITS =   100;
const int MAXTRDCLMC   =   200;
const int MAXTRDRHT   =   200;
const int MAXTRDCL   =   100;
const int MAXTRDSEG   =   100;
const int MAXTRDTRK   =   40;
const int MAXMCVOL   =   150;
};
using namespace root;
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

#ifdef __WRITEROOT__
  EventNtuple();
ClassDef(EventNtuple,1)       //EventNtuple
#else
friend class AMSEvent;
friend class AMSNtuple;  
#endif
};

#ifdef __WRITEROOT__
class EventNtuple02: public TObject {
#else
class EventNtuple02 {
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
  int Tracks;
  int TrRecHits;
  int TrClusters;
  int TrRawClusters;
  int TrMCClusters;
  int TOFClusters;
  int TOFMCClusters;
  int AntiMCClusters;
  int TRDMCClusters;
  int AntiClusters;
  int EcalClusters;
  int EcalHits;
  int RICMCClusters;//CJM
  int RICHits;//CJM
  int TRDRawHits;
  int TRDClusters;
  int TRDSegments;
  int TRDTracks;
  int EventStatus[2];
  
#ifdef __WRITEROOT__
  EventNtuple02();
ClassDef(EventNtuple02,1)       //EventNtuple02
#else
friend class AMSEvent;
friend class AMSNtuple;
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

#ifdef __WRITEROOT__
 BetaNtuple();
ClassDef(BetaNtuple,1)       //BetaNtuple
#else
friend class AMSBeta;
friend class AMSNtuple;
#endif
};

#ifdef __WRITEROOT__
class BetaNtuple02 : public TObject {
#else
class BetaNtuple02 {
#endif
public:
  int Nbeta;
  int Status[MAXBETA02];
  int Pattern[MAXBETA02];
  float Beta[MAXBETA02];
  float BetaC[MAXBETA02];
  float Error[MAXBETA02];
  float ErrorC[MAXBETA02];
  float Chi2[MAXBETA02];
  float Chi2S[MAXBETA02];
  int NTOF[MAXBETA02];
  int pTOF[MAXBETA02][4];
  int pTr[MAXBETA02];

#ifdef __WRITEROOT__
   BetaNtuple02();
   ClassDef(BetaNtuple02,1)       //BetaNtuple
#else
friend class AMSBeta;
friend class AMSNtuple;
#endif
};


#ifdef __WRITEROOT__
class ChargeNtuple02 : public TObject {
#else
class ChargeNtuple02 {
#endif
public:
  int Ncharge;
  int Status[MAXCHARGE02];
  int BetaP[MAXCHARGE02];
  int ChargeTOF[MAXCHARGE02];
  int ChargeTracker[MAXCHARGE02];
  float ProbTOF[MAXCHARGE02][4];
  int ChInTOF[MAXCHARGE02][4];
  float ProbTracker[MAXCHARGE02][4];
  int ChInTracker[MAXCHARGE02][4];
  float ProbAllTracker[MAXCHARGE02];
  float TrunTOF[MAXCHARGE02];
  float TrunTOFD[MAXCHARGE02];
  float TrunTracker[MAXCHARGE02];

#ifdef __WRITEROOT__
  ChargeNtuple02();
ClassDef(ChargeNtuple02,1)       //ChargeNtuple
#else
friend class AMSCharge;
friend class AMSNtuple;
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
  int ChInTOF[MAXCHARGE][4];
  float ProbTracker[MAXCHARGE][4];
  int ChInTracker[MAXCHARGE][4];
  float ProbAllTracker[MAXCHARGE];
  float TrunTOF[MAXCHARGE];
  float TrunTOFD[MAXCHARGE];
  float TrunTracker[MAXCHARGE];

#ifdef __WRITEROOT__
  ChargeNtuple();
ClassDef(ChargeNtuple,1)       //ChargeNtuple
#else
friend class AMSCharge;
friend class AMSNtuple;
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
//  float ATCbeta[MAXPART];
  float Cutoff[MAXPART];
  float CooCTC[MAXPART][2][3];
  float TOFCoo[MAXPART][4][3];
  float AntiCoo[MAXPART][2][3];
  float TrCoo[MAXPART][6][3];
#ifdef __WRITEROOT__
  ParticleNtuple();
ClassDef(ParticleNtuple,1)       //ParticleNtuple
#else
friend class AMSParticle;
friend class AMSNtuple;
#endif
};

#ifdef __WRITEROOT__
class ParticleNtuple02 : public TObject {
#else
class ParticleNtuple02 {
#endif
public:
  int Npart;
  int   BetaP[MAXPART02];
  int   ChargeP[MAXPART02];
  int   TrackP[MAXPART02];
  int   TRDP[MAXPART02];
  int   RICHP[MAXPART02];
  int   EcalP[MAXPART02];
  int   Particle[MAXPART02];
  int   ParticleVice[MAXPART02];
  float Prob[MAXPART02][2];
  float FitMom[MAXPART02];
  float Mass[MAXPART02];
  float ErrMass[MAXPART02];
  float Momentum[MAXPART02];
  float ErrMomentum[MAXPART02];
  float Beta[MAXPART02];
  float ErrBeta[MAXPART02];
  float Charge[MAXPART02];
  float Theta[MAXPART02];
  float Phi[MAXPART02];
  float ThetaGl[MAXPART02];
  float PhiGl[MAXPART02];
  float Coo[MAXPART02][3];
  float Cutoff[MAXPART02];
  float TOFCoo[MAXPART02][4][3];
  float AntiCoo[MAXPART02][2][3];
  float EcalCoo[MAXPART02][3][3];
  float TrCoo[MAXPART02][trconst::maxlay][3];
  float TRDCoo[MAXPART02][3];
#ifdef __WRITEROOT__
  ParticleNtuple02();
ClassDef(ParticleNtuple02,1)       //ParticleNtuple
#else
friend class AMSParticle;
friend class AMSNtuple;
#endif
};

//-------------------------
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

#ifdef __WRITEROOT__
  TOFClusterNtuple();
ClassDef(TOFClusterNtuple,1)       //TOFClusterNtuple
#else
friend class AMSTOFCluster;
friend class AMSNtuple;
#endif
};
//--------------------------


#ifdef __WRITEROOT__
class EcalShowerNtuple : public TObject {
#else
class EcalShowerNtuple {
#endif
public:
  int Necsh;
  int   Status[MAXECSHOW];
  float Dir[MAXECSHOW][3];
  float EMDir[MAXECSHOW][3];
  float Entry[MAXECSHOW][3];
  float Exit[MAXECSHOW][3];
  float CofG[MAXECSHOW][3];
  float ErTheta[MAXECSHOW];
  float Chi2Dir[MAXECSHOW];
  float FirstLayerEdep[MAXECSHOW];
  float EnergyC[MAXECSHOW];
  float Energy3C[MAXECSHOW][3];
  float ErEnergyC[MAXECSHOW];
  float DifoSum[MAXECSHOW];
  float SideLeak[MAXECSHOW];
  float RearLeak[MAXECSHOW];
  float DeadLeak[MAXECSHOW];
  float OrpLeak[MAXECSHOW];
  float Chi2Profile[MAXECSHOW];
  float ParProfile[MAXECSHOW][4];
  float Chi2Trans[MAXECSHOW];
  float TransProfile[MAXECSHOW][3];
  int   p2DCl[MAXECSHOW][2];
#ifdef __WRITEROOT__
  EcalShowerNtuple();
ClassDef(EcalShowerNtuple,1)       //EcalShowerNtuple
#else
friend class EcalShower;
friend class AMSNtuple;
#endif

};



#ifdef __WRITEROOT__
class EcalClusterNtuple : public TObject {
#else
class EcalClusterNtuple {
#endif
public:
  int Neccl;
  int Status[MAXECCLUST];
  int Proj[MAXECCLUST];
  int Plane[MAXECCLUST];
  int Left[MAXECCLUST];
  int Center[MAXECCLUST];
  int Right[MAXECCLUST];
  float Edep[MAXECCLUST];
//  float RMS[MAXECCLUST];
  float SideLeak[MAXECCLUST];
  float DeadLeak[MAXECCLUST];
  float Coo[MAXECCLUST][3];
  int pLeft[MAXECCLUST];
  int NHits[MAXECCLUST];
#ifdef __WRITEROOT__
  EcalClusterNtuple();
ClassDef(EcalClusterNtuple,1)       //EcalClusterNtuple
#else
friend class Ecal1DCluster;
friend class AMSNtuple;
#endif
};


#ifdef __WRITEROOT__
class Ecal2DClusterNtuple : public TObject {
#else
class Ecal2DClusterNtuple {
#endif
public:
  int Nec2dcl;
  int Proj[MAXEC2DCLUST];
  int Nmemb[MAXEC2DCLUST];
  float Edep[MAXEC2DCLUST];
//  float SideLeak[MAXEC2DCLUST];
//  float DeadLeak[MAXEC2DCLUST];
  float Coo[MAXEC2DCLUST];
  float Tan[MAXEC2DCLUST];
  float Chi2[MAXEC2DCLUST];
  int   pCl[MAXEC2DCLUST][18];
#ifdef __WRITEROOT__
  Ecal2DClusterNtuple();
ClassDef(Ecal2DClusterNtuple,1)       //Ecal2DClusterNtuple
#else
friend class Ecal2DCluster;
friend class AMSNtuple;
#endif
};


//--------------------------
#ifdef __WRITEROOT__
class EcalHitNtuple : public TObject {
#else
class EcalHitNtuple {
#endif
public:
  int Necht;
  int Status[MAXECHITS];
  int Idsoft[MAXECHITS];
  int Proj[MAXECHITS];
  int Plane[MAXECHITS];
  int Cell[MAXECHITS];
  float Edep[MAXECHITS];
  float Coo[MAXECHITS][3];

#ifdef __WRITEROOT__
  EcalHitNtuple();
ClassDef(EcalHitNtuple,1)       //EcalHitNtuple
#else
friend class AMSEcalHit;
friend class AMSNtuple;
#endif
};
//-------------------------- 
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

#ifdef __WRITEROOT__
  TOFMCClusterNtuple();
ClassDef(TOFMCClusterNtuple,1)       //TOFMCClusterNtuple
#else
friend class AMSTOFMCCluster;
friend class AMSNtuple;
#endif
};
//--------------------------
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

#ifdef __WRITEROOT__
  TrClusterNtuple();
ClassDef(TrClusterNtuple,1)       //TrClusterNtuple
#else
friend class AMSTrCluster;
friend class AMSNtuple;
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

#ifdef __WRITEROOT__
  TrMCClusterNtuple();
ClassDef(TrMCClusterNtuple,1)       //TrMCClusterNtuple
#else
friend class AMSTrMCCluster;
friend class AMSNtuple;
#endif
};

#ifdef __WRITEROOT__
class TRDMCClusterNtuple : public TObject {
#else
class TRDMCClusterNtuple {
#endif
public:
  int Ntrdclmc;
  int Layer[MAXTRDCLMC];
  int Ladder[MAXTRDCLMC];
  int Tube[MAXTRDCLMC];
  int TrackNo[MAXTRDCLMC];
  float Edep[MAXTRDCLMC];
  float Ekin[MAXTRDCLMC];
  float Xgl[MAXTRDCLMC][3];
  float Step[MAXTRDCLMC];
 
#ifdef __WRITEROOT__
  TRDMCClusterNtuple();
ClassDef(TRDMCClusterNtuple,1)       //TRDMCClusterNtuple
#else
friend class AMSTRDMCCluster;
friend class AMSNtuple;
#endif
};


#ifdef __WRITEROOT__
class TRDRawHitNtuple : public TObject {
#else
class TRDRawHitNtuple {
#endif
public:
  int Ntrdht;
  int Layer[MAXTRDRHT];
  int Ladder[MAXTRDRHT];
  int Tube[MAXTRDRHT];
  float Amp[MAXTRDRHT];
#ifdef __WRITEROOT__
  TRDRawHitNtuple();
ClassDef(TRDRawHitNtuple,1)       //TRDRawHitNtuple
#else
friend class AMSTRDRawHit;
friend class AMSNtuple;
#endif
};


#ifdef __WRITEROOT__
class TRDClusterNtuple : public TObject {
#else
class TRDClusterNtuple {
#endif
public:
  int Ntrdcl;
  int   Status[MAXTRDCL];
  float Coo[MAXTRDCL][3];
  int    Layer[MAXTRDCL];
  float CooDir[MAXTRDCL][3];
  int Multip[MAXTRDCL];
  int HMultip[MAXTRDCL];
  float EDep[MAXTRDCL];
  int   pRawHit[MAXTRDCL];
 
#ifdef __WRITEROOT__
  TRDClusterNtuple();
ClassDef(TRDClusterNtuple,1)       //TRDClusterNtuple
#else
friend class AMSTRDCluster;
friend class AMSNtuple;
#endif
};


#ifdef __WRITEROOT__
class TRDSegmentNtuple : public TObject {
#else
class TRDSegmentNtuple {
#endif
public:
  int Ntrdseg;
  int   Status[MAXTRDSEG];
  int   Orientation[MAXTRDSEG];
  float FitPar[MAXTRDSEG][2];
  float Chi2[MAXTRDSEG];
  int Pattern[MAXTRDSEG];
  int Nhits[MAXTRDSEG];
  int PCl[MAXTRDSEG][trdconst::maxhits];
 
#ifdef __WRITEROOT__
  TRDSegmentNtuple();
ClassDef(TRDSegmentNtuple,1)       //TRDSegmentNtuple
#else
friend class AMSTRDSegment;
friend class AMSNtuple;
#endif
};



#ifdef __WRITEROOT__
class TRDTrackNtuple : public TObject {
#else
class TRDTrackNtuple {
#endif
public:
  int Ntrdtrk;
  int   Status[MAXTRDTRK];
  float Coo[MAXTRDTRK][3];
  float ErCoo[MAXTRDTRK][3];
  float Phi[MAXTRDTRK];
  float Theta[MAXTRDTRK];
  float Chi2[MAXTRDTRK];
  int   NSeg[MAXTRDTRK];
  int   Pattern[MAXTRDTRK];
  int   pSeg[MAXTRDTRK][trdconst::maxseg];
 
#ifdef __WRITEROOT__
  TRDTrackNtuple();
ClassDef(TRDTrackNtuple,1)       //TRDTrackNtuple
#else
friend class AMSTRDTrack;
friend class AMSNtuple;
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

#ifdef __WRITEROOT__
  TrRecHitNtuple();
ClassDef(TrRecHitNtuple,1)       //TrRecHitNtuple
#else
friend class AMSTrRecHit;
friend class AMSNtuple;
#endif
};
#ifdef __WRITEROOT__
class TrRecHitNtuple02 : public TObject {
#else
class TrRecHitNtuple02 {
#endif
public:
  int Ntrrh;
  int pX[MAXTRRH02];
  int pY[MAXTRRH02];
  int Status[MAXTRRH02];
  int Layer[MAXTRRH02];
  float Hit[MAXTRRH02][3];
  float EHit[MAXTRRH02][3];
  float Sum[MAXTRRH02];
  float DifoSum[MAXTRRH02];
  float CofgX[MAXTRRH02];
  float CofgY[MAXTRRH02];

#ifdef __WRITEROOT__
  TrRecHitNtuple02();
ClassDef(TrRecHitNtuple02,1)       //TrRecHitNtuple
#else
friend class AMSTrRecHit;
friend class AMSNtuple;
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
  float LocDBAver[MAXTRTR];
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

#ifdef __WRITEROOT__
  TrTrackNtuple();
ClassDef(TrTrackNtuple,1)       //TrTrackNtuple
#else
friend class AMSTrTrack;
friend class AMSNtuple;
#endif
};

#ifdef __WRITEROOT__
class TrTrackNtuple02 : public TObject {
#else
class TrTrackNtuple02 {
#endif
public:
  int Ntrtr;
  int Status[MAXTRTR02];
  int Pattern[MAXTRTR02];
  int Address[MAXTRTR02];
  int NHits[MAXTRTR02];
  int pHits[MAXTRTR02][trconst::maxlay];
  float LocDBAver[MAXTRTR02];
  int GeaneFitDone[MAXTRTR02];
  int AdvancedFitDone[MAXTRTR02];
  float Chi2StrLine[MAXTRTR02];
  float Chi2Circle[MAXTRTR02];
  float CircleRidgidity[MAXTRTR02];
  float Chi2FastFit[MAXTRTR02];
  float Ridgidity[MAXTRTR02];
  float ErrRidgidity[MAXTRTR02];
  float Theta[MAXTRTR02];
  float Phi[MAXTRTR02];
  float P0[MAXTRTR02][3];
  float GChi2[MAXTRTR02];
  float GRidgidity[MAXTRTR02];
  float GErrRidgidity[MAXTRTR02];
//  float GTheta[MAXTRTR02];
//  float GPhi[MAXTRTR02];
//  float GP0[MAXTRTR02][3];
  float HChi2[MAXTRTR02][2];
  float HRidgidity[MAXTRTR02][2];
  float HErrRidgidity[MAXTRTR02][2];
  float HTheta[MAXTRTR02][2];
  float HPhi[MAXTRTR02][2];
  float HP0[MAXTRTR02][2][3];
  float FChi2MS[MAXTRTR02];
  float PiErrRig[MAXTRTR02];
  float RidgidityMS[MAXTRTR02];
  float PiRigidity[MAXTRTR02];

#ifdef __WRITEROOT__
  TrTrackNtuple02();
ClassDef(TrTrackNtuple02,1)       //TrTrackNtuple
#else
friend class AMSTrTrack;
friend class AMSNtuple;
#endif
};
#ifdef __WRITEROOT__
class MCTrackNtuple : public TObject {
#else
class MCTrackNtuple {
#endif
public:
int Nmct;
float radl[MAXMCVOL];
float absl[MAXMCVOL];
float pos[MAXMCVOL][3];
char  vname[MAXMCVOL][4];
#ifdef __WRITEROOT__
 MCTrackNtuple();
ClassDef(MCTrackNtuple,1)       //MCTrackNtuple
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

#ifdef __WRITEROOT__
  MCEventGNtuple();
ClassDef(MCEventGNtuple,1)       //MCEventGNtuple
#else
friend class AMSmceventg;
friend class AMSNtuple;
#endif
};

#ifdef __WRITEROOT__
class MCEventGNtuple02 : public TObject {
#else
class MCEventGNtuple02 {
#endif
public:
  int Nmcg;
  int Nskip[MAXMCG02];
  int Particle[MAXMCG02];
  float Coo[MAXMCG02][3];
  float Dir[MAXMCG02][3];
  float Momentum[MAXMCG02];
  float Mass[MAXMCG02];
  float Charge[MAXMCG02];

#ifdef __WRITEROOT__
  MCEventGNtuple02();
ClassDef(MCEventGNtuple02,1)       //MCEventGNtuple
#else
friend class AMSmceventg;
friend class AMSNtuple;
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

#ifdef __WRITEROOT__
  CTCClusterNtuple();
ClassDef(CTCClusterNtuple,1)       //CTCClusterNtuple
#else
friend class AMSCTCCluster;
friend class AMSNtuple;
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

#ifdef __WRITEROOT__
  CTCMCClusterNtuple();
ClassDef(CTCMCClusterNtuple,1)       //CTCMCClusterNtuple
#else
friend class AMSCTCMCCluster;
friend class AMSNtuple;
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

#ifdef __WRITEROOT__
  AntiClusterNtuple();
ClassDef(AntiClusterNtuple,1)       //AntiClusterNtuple
#else
friend class AMSAntiCluster;
friend class AMSNtuple;
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

#ifdef __WRITEROOT__
  ANTIMCClusterNtuple();
ClassDef(ANTIMCClusterNtuple,1)       //ANTIMCClusterNtuple
#else
friend class AMSAntiMCCluster;
friend class AMSNtuple;
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

#ifdef __WRITEROOT__
  LVL3Ntuple();
ClassDef(LVL3Ntuple,1)       //LVL3Ntuple
#else
friend class TriggerLVL3;
friend class AMSNtuple;
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

#ifdef __WRITEROOT__
  LVL1Ntuple();
ClassDef(LVL1Ntuple,1)       //LVL1Ntuple
#else
friend class TriggerLVL1;
friend class AMSNtuple;
#endif
};


#ifdef __WRITEROOT__
class LVL1Ntuple02 : public TObject {
#else
class LVL1Ntuple02 {
#endif
public:
  int Nlvl1;
  int Mode[MAXLVL1];
  int TOFlag[MAXLVL1];
  int TOFPatt[MAXLVL1][4];
  int TOFPatt1[MAXLVL1][4];
  int AntiPatt[MAXLVL1];
  int ECALflag[MAXLVL1];

#ifdef __WRITEROOT__
  LVL1Ntuple02();
ClassDef(LVL1Ntuple02,1)       //LVL1Ntuple02
#else
friend class Trigger2LVL1;
friend class AMSNtuple;
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


#ifdef __WRITEROOT__
  CTCHitNtuple();
ClassDef(CTCHitNtuple,1)       //CTCHitNtuple
#else
friend class AMSCTCRawHit;
friend class AMSNtuple;
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
#ifdef __WRITEROOT__
  TrRawClusterNtuple();
ClassDef(TrRawClusterNtuple,1)       //TrRawClusterNtuple
#else
friend class AMSTrRawCluster;
friend class AMSNtuple;
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

#ifdef __WRITEROOT__
  AntiRawClusterNtuple();
ClassDef(AntiRawClusterNtuple ,1)       //AntiRawClusterNtuple 
#else
friend class AMSAntiRawCluster;
friend class AMSNtuple;
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

#ifdef __WRITEROOT__
  TOFRawClusterNtuple();
ClassDef(TOFRawClusterNtuple ,1)       //TOFRawClusterNtuple 
#else
friend class AMSTOFRawCluster;
friend class AMSNtuple;
#endif
};


#ifdef __WRITEROOT__
class RICMCNtuple : public TObject {
#else
class RICMCNtuple {
#endif
public:
  int NMC;                     // Number of detected photons
  int id[MAXRICMC];            // Particle id.
  float origin[MAXRICMC][3];   // Particle origin
  float direction[MAXRICMC][3];// Original direction
  int status[MAXRICMC];        // Status=10*number of reflections+(have it rayleigh?1:0)
  int numgen;                  // Number of generated photons
  int eventpointer[MAXRICMC];  // Pointer to detected hit
#ifdef __WRITEROOT__
  RICMCNtuple();
ClassDef(RICMCNtuple,1)
#else
friend class AMSRichMCHit;
friend class AMSNtuple;
#endif
};


#ifdef __WRITEROOT__
class RICEventNtuple : public TObject {
#else
class RICEventNtuple {
#endif
public:
  int Nhits;
  int channel[MAXRICHITS];
  int adc[MAXRICHITS];
  float x[MAXRICHITS];
  float y[MAXRICHITS];

#ifdef __WRITEROOT__
  RICEventNtuple();
ClassDef(RICEventNtuple,1)
#else
friend class AMSRichRawEvent;
friend class AMSNtuple;
#endif
};


#ifdef __WRITEROOT__
class RICRing : public TObject {
#else
class RICRing {
#endif
public:
  int NRings;
  int track[MAXTRTR02];
  int used[MAXTRTR02];
  float beta[MAXTRTR02];
  float errorbeta[MAXTRTR02];
  float quality[MAXTRTR02];
  int Z[MAXTRTR02];


#ifdef __WRITEROOT__
  RICRing();
ClassDef(RICRing,1)
#else
friend class AMSRichRing;
friend class AMSNtuple;
#endif
}; 


#endif
