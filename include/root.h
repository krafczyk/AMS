//  $Id: root.h,v 1.79 2003/05/03 08:44:50 choutko Exp $
#ifndef __AMSROOT__
#define __AMSROOT__

#ifdef __WRITEROOT__
#include <TObject.h>
#include <TTree.h>
#include <TFile.h>
#include <TROOT.h>
#include "TClonesArray.h"
#include "TRefArray.h"
#include "TRef.h"
#include <antidbc02.h>
#include <list>
#include <vector>
using namespace std;
class AMSAntiCluster;
class AMSAntiMCCluster;
class AMSBeta;
class AMSCharge;
class AMSEvent;
class AMSEcalHit;
class AMSmceventg;
class AMSmctrack;
class AMSParticle;
class AMSRichMCHit;
class AMSRichRawEvent;
class AMSRichRing;
class AMSNtuple;
class AMSTOFCluster;
class AMSTOFMCCluster;
class AMSTRDCluster;
class AMSTRDMCCluster;
class AMSTRDRawHit;
class AMSTRDSegment;
class AMSTRDTrack;
class AMSTrCluster;
class AMSTrMCCluster;
class AMSTrRawCluster;
class AMSTrTrack;
class AMSTrTrackGamma;
class AMSTrRecHit;
class Ecal1DCluster;
class Ecal2DCluster;
class EcalShower;
class TOF2RawCluster;
class Trigger2LVL1;
class TriggerLVL302;

class AMSEventHeaderRoot: public TObject {
 public:
// general info
  int _Run;
  int _RunType;
  int _Eventno;
  int _Time[2];
  int _EventStatus[2];
  int _RawWords;
// shuttle parameters
   float _StationRad;    //cm
   float _StationTheta;
   float _StationPhi;
   float _NorthPolePhi;
   float _Yaw;
   float _Pitch;
   float _Roll;
   float _StationSpeed;
   float _SunRad;
   float _VelTheta;
   float _VelPhi;
// counters
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

AMSEventHeaderRoot(){};
void   Set(AMSEvent *ptr, int rawwords);
~AMSEventHeaderRoot(){};
ClassDef(AMSEventHeaderRoot,1)       //AMSEventHeaderRoot
};



class BetaRoot02 : public TObject {
 public:
  int       Status;
  int       Pattern;
  float     Beta;
  float     BetaC;
  float     Error;
  float     ErrorC;
  float     Chi2;
  float     Chi2S;
#ifndef __ROOTPOINTERS__
  TRef      fTrTrack;
  TRefArray *fTOFCluster;
#else
  int   fTrTrack;
  vector<int> fTOFCluster;
#endif
  ~BetaRoot02();
   BetaRoot02();
   BetaRoot02(AMSBeta *ptr);

   ClassDef(BetaRoot02,1)       
};

class ChargeRoot02 : public TObject {
public:
  int Status;
  int ChargeTOF;
  int ChargeTracker;
  int ChargeRich;
  float ProbTOF[4];
  int ChInTOF[4];
  float ProbTracker[4];
  int ChInTracker[4];
  float ProbRich[4];
  int ChInRich[4];
  float ProbAllTracker;
  float TrunTOF;
  float TrunTOFD;
  float TrunTracker;
#ifndef __ROOTPOINTERS__
  TRef  fBeta;
  TRef  fRich;
#else
  int  fBeta;
  int  fRich;
#endif
  ChargeRoot02();
  ~ChargeRoot02();
  ChargeRoot02(AMSCharge *ptr, float probtof[],int chintof[],
               float probtr[], int chintr[], 
               float probrc[], int chinrc[], float proballtr);
ClassDef(ChargeRoot02,1)       //ChargeRoot

};

class ParticleRoot02 : public TObject {
public:
  int   Particle;
  int   ParticleVice;
  float Prob[2];
  float FitMom;
  float Mass;
  float ErrMass;
  float Momentum;
  float ErrMomentum;
  float Beta;
  float ErrBeta;
  float Charge;
  float Theta;
  float Phi;
  float ThetaGl;
  float PhiGl;
  float Coo[3];
  float Cutoff;
  float TOFCoo[4][3];
  float AntiCoo[2][3];
  float EcalCoo[3][3];
  float TrCoo[8][3];
  float TRDCoo[3];
  float RichCoo[2][3];
  float RichPath[2];
  float RichPathBeta[2];
  float RichLength;
  float Local[8];
  float TRDLikelihood;

#ifndef __ROOTPOINTERS__
  TRef  fBeta;
  TRef  fCharge;
  TRef  fTrack;      // pointer to track;
  TRef  fTRD;        // pointer to trd track
  TRef  fRich;       // pointer to rich ring
  TRef  fShower;     // pointer to shower;
#else
  int  fBeta;
  int  fCharge;
  int  fTrack;      // pointer to track;
  int  fTRD;        // pointer to trd track
  int  fRich;       // pointer to rich ring
  int  fShower;     // pointer to shower;
#endif

  ParticleRoot02();
  ~ParticleRoot02(){};
  ParticleRoot02(AMSParticle *ptr, float phi, float phigl);
  ClassDef(ParticleRoot02,1)       //ParticleRoot02
};

class TOFClusterRoot : public TObject {
public:
  int Status;
  int Layer;
  int Bar;
  int Nmemb;
  int P2memb[3];
  float Edep;
  float Edepd;
  float Time;
  float ErrTime;
  float Coo[3];
  float ErrorCoo[3];

  TOFClusterRoot();
  ~TOFClusterRoot(){};
  TOFClusterRoot(AMSTOFCluster *ptr, int p2mem[]);

ClassDef(TOFClusterRoot,1)       //TOFClusterRoot
};

class EcalShowerRoot : public TObject {
public:
  int   Status;
  float Dir[3];
  float EMDir[3];
  float Entry[3];
  float Exit[3];
  float CofG[3];
  float ErTheta;
  float Chi2Dir;
  float FirstLayerEdep;
  float EnergyC;
  float Energy3C[3];
  float ErEnergyC;
  float DifoSum;
  float SideLeak;
  float RearLeak;
  float DeadLeak;
  float AttLeak;
  float NLinLeak;
  float OrpLeak;
  float Orp2DEnergy;
  float Chi2Profile;
  float ParProfile[4];
  float Chi2Trans;
//  float TransProfile[3];
  float SphericityEV[3];
  int   N2dCl;

#ifndef __ROOTPOINTERS__
  TRefArray *fEcal2DCluster;
#else
  vector <int> fEcal2DCluster;
#endif
  EcalShowerRoot();
  ~EcalShowerRoot();
  EcalShowerRoot(EcalShower *ptr);

ClassDef(EcalShowerRoot,1)       //EcalShowerRoot

};

class EcalClusterRoot : public TObject {
public:
  int Status;
  int Proj;
  int Plane;
  int Left;
  int Center;
  int Right;
  float Edep;
//  float RMS;
  float SideLeak;
  float DeadLeak;
  float Coo[3];
  int NHits;

#ifndef __ROOTPOINTERS__
  TRefArray  *fEcalHit;
#else
  vector <int> fEcalHit;
#endif
  EcalClusterRoot();
  ~EcalClusterRoot();
  EcalClusterRoot(Ecal1DCluster *ptr);

ClassDef(EcalClusterRoot,1)       //EcalClusterRoot
};

class Ecal2DClusterRoot : public TObject {
public:
  int Status;
  int Proj;
  int Nmemb;
  float Edep;
  float Coo;
  float Tan;
  float Chi2;

#ifndef __ROOTPOINTERS__
  TRefArray *fEcal1DCluster;
#else
  vector <int> fEcal1DCluster;
#endif
  Ecal2DClusterRoot();
  ~Ecal2DClusterRoot();
  Ecal2DClusterRoot(Ecal2DCluster *ptr);
ClassDef(Ecal2DClusterRoot,1)       //Ecal2DClusterRoot
};

class EcalHitRoot : public TObject {
public:
  int   Status;
  int   Idsoft;
  int   Proj;
  int   Plane;
  int   Cell;
  float Edep;
  float EdCorr;
  float AttCor;
  float Coo[3];
  float ADC[3];
  float Ped[3];
  float Gain;
  EcalHitRoot();
  ~EcalHitRoot(){};
  EcalHitRoot(AMSEcalHit *ptr);
ClassDef(EcalHitRoot,1)       //EcalHitRoot
};

class TOFMCClusterRoot : public TObject {
public:
  int   Idsoft;
  float Coo[3];
  float TOF;
  float Edep;

  TOFMCClusterRoot();
  ~TOFMCClusterRoot(){};
  TOFMCClusterRoot(AMSTOFMCCluster *ptr);
ClassDef(TOFMCClusterRoot,1)       //TOFMCClusterRoot
};

class TrClusterRoot : public TObject {
public:
  int Idsoft;
  int Status;
  int NelemL;
  int NelemR;
  float Sum;
  float Sigma;
  float Mean;
  float RMS;
  float ErrorMean;
  float Amplitude[5];

  ~TrClusterRoot(){};
  TrClusterRoot();
  TrClusterRoot(AMSTrCluster *ptr, float ampl[]);
ClassDef(TrClusterRoot,1)       //TrClusterRoot
};

class TrMCClusterRoot : public TObject {
public:
  int Idsoft;
  int TrackNo;
  int Left[2];
  int Center[2];
  int Right[2];
  float SS[2][5];
  float Xca[3];
  float Xcb[3];
  float Xgl[3];
  float Sum;

  TrMCClusterRoot();
  ~TrMCClusterRoot(){};
  TrMCClusterRoot(AMSTrMCCluster *ptr);
ClassDef(TrMCClusterRoot,1)       //TrMCClusterRoot
};

class TRDMCClusterRoot : public TObject {
public:
  int   Layer;
  int   Ladder;
  int   Tube;
  int   ParticleNo;
  //  int   TrackNo;
  float Edep;
  float Ekin;
  float Xgl[3];
  float Step;
 
  TRDMCClusterRoot();
  ~TRDMCClusterRoot(){};
  TRDMCClusterRoot(AMSTRDMCCluster *ptr);
ClassDef(TRDMCClusterRoot,1)       //TRDMCClusterRoot
};

class TRDRawHitRoot : public TObject {
public:
  int Layer;
  int Ladder;
  int Tube;
  float Amp;

  TRDRawHitRoot();
  ~TRDRawHitRoot(){};
  TRDRawHitRoot(AMSTRDRawHit *ptr);

ClassDef(TRDRawHitRoot,1)       //TRDRawHitRoot
};

class TRDClusterRoot : public TObject {
public:
  int   Status;
  float Coo[3];
  int   Layer;
  float CooDir[3];
  int   Multip;
  int   HMultip;
  float EDep;

#ifndef __ROOTPOINTERS__
  TRef  fTRDRawHit;
#else
  int fTRDRawHit;
#endif 
  TRDClusterRoot();
  ~TRDClusterRoot(){};
  TRDClusterRoot(AMSTRDCluster *ptr);
ClassDef(TRDClusterRoot,1)       //TRDClusterRoot
};

class TRDSegmentRoot : public TObject {
public:
  int   Status;
  int   Orientation;
  float FitPar[2];
  float Chi2;
  int   Pattern;
  int   Nhits;
 
#ifndef __ROOTPOINTERS__
  TRefArray *fTRDCluster;
#else 
  vector<int> fTRDCluster;
#endif
  TRDSegmentRoot();
  ~TRDSegmentRoot();
  TRDSegmentRoot(AMSTRDSegment *ptr);

ClassDef(TRDSegmentRoot,1)       //TRDSegmentRoot
};


class TRDTrackRoot : public TObject {
public:
  int   Status;
  float Coo[3];
  float ErCoo[3];
  float Phi;
  float Theta;
  float Chi2;
  int   NSeg;
  int   Pattern;

#ifndef __ROOTPOINTERS__
  TRefArray *fTRDSegment;
#else
  vector<int> fTRDSegment;
#endif 
  TRDTrackRoot();
  ~TRDTrackRoot();
  TRDTrackRoot(AMSTRDTrack *ptr);

ClassDef(TRDTrackRoot,1)       //TRDTrackRoot
};


class TrRecHitRoot02 : public TObject {
public:
  int   Status;
  int   Layer;
  float Hit[3];
  float EHit[3];
  float Sum;
  float DifoSum;
  float CofgX;
  float CofgY;

#ifndef __ROOTPOINTERS__
  TRef  fTrClusterX;
  TRef  fTrClusterY;
#else
  int  fTrClusterX;
  int  fTrClusterY;
#endif
  TrRecHitRoot02();
  TrRecHitRoot02(AMSTrRecHit *ptr);
ClassDef(TrRecHitRoot02,1)       //TrRecHitRoot02
};

/*
class TrGammaRoot02 : public TObject {
public:
  float Pgam;
  float ErrPgam;
  float Thetagam;
  float Phigam;
  float Massgam;
  float Vert[3];
  float Distance;
  int   Charge;
  int   GammaStatus;
  int   PtrLeft;
  int   PtrRight;
  float Jthetal;
  float Jphil;
  float Jthetar;
  float Jphir;
  float Jp0l[3];
  float Jp0r[3];
  float JChi2l;
  float JChi2r;

#ifndef __ROOTPOINTERS__
  TRefArray *fTrTrack;
#else
  vector<int> fTrTrack;
#endif
  TrGammaRoot02();
  ~TrGammaRoot02();
  TrGammaRoot02(AMSTrTrackGamma *ptr);

ClassDef(TrGammaRoot02,1)       //TrGammaRoot02
};
*/
class TrTrackRoot02 : public TObject {
public:
  int Status;
  int Pattern;
  int Address;
  int NHits;
  float LocDBAver;
  int   GeaneFitDone;
  int   AdvancedFitDone;
  float Chi2StrLine;
  float Chi2Circle;
  float CircleRigidity;
  float Chi2FastFit;
  float Rigidity;
  float ErrRigidity;
  float Theta;
  float Phi;
  float P0[3];
  float GChi2;
  float GRigidity;
  float GErrRigidity;
  float HChi2[2];
  float HRigidity[2];
  float HErrRigidity[2];
  float HTheta[2];
  float HPhi[2];
  float HP0[2][3];
  float FChi2MS;
  float PiErrRig;
  float RigidityMS;
  float PiRigidity;


#ifndef __ROOTPOINTERS__
  TRefArray *fTrRecHit;
#else
  vector<int> fTrRecHit;
#endif
  TrTrackRoot02();
  ~TrTrackRoot02();
  TrTrackRoot02(AMSTrTrack *ptr);

ClassDef(TrTrackRoot02,1)       //TrTrackRoot02
};

class MCTrackRoot : public TObject {
public:
float _radl;
float _absl;
float _pos[3];
char  _vname[4];

 MCTrackRoot();
 ~MCTrackRoot(){};
 MCTrackRoot(AMSmctrack *ptr);
ClassDef(MCTrackRoot,1)       //MCTrackRoot
};


class MCEventGRoot02 : public TObject {
public:
  int Nskip;
  int Particle;
  float Coo[3];
  float Dir[3];
  float Momentum;
  float Mass;
  float Charge;

  MCEventGRoot02();
  ~MCEventGRoot02(){};
  MCEventGRoot02(AMSmceventg *ptr);
ClassDef(MCEventGRoot02,1)       //MCEventGRoot02
};


class AntiClusterRoot : public TObject {
public:
  int   Status;
  int   Sector;
  int   Ntimes;
  int   Npairs;
  float Times[2*ANTI2C::ANTHMX];
  float Timese[2*ANTI2C::ANTHMX];
  float Edep;
  float Coo[3];   // R, phi, Z
  float ErrorCoo[3];

  AntiClusterRoot();
  ~AntiClusterRoot(){};
  AntiClusterRoot(AMSAntiCluster *ptr);
ClassDef(AntiClusterRoot,1)       //AntiClusterRoot
};

class ANTIMCClusterRoot : public TObject {
public:
  int   Idsoft;
  float Coo[3];
  float TOF;
  float Edep;

  ANTIMCClusterRoot();
  ~ANTIMCClusterRoot(){};
  ANTIMCClusterRoot(AMSAntiMCCluster *ptr);
ClassDef(ANTIMCClusterRoot,1)       //ANTIMCClusterRoot
};

class LVL3Root02 : public TObject {
public:
  int   TOFTr;
  int   TRDTr;
  int   TrackerTr;
  int   MainTr;
  int   Direction;
  int   NTrHits;
  int   NPatFound;
  int   Pattern[2];
  float Residual[2];
  float Time;
  float ELoss;
  int   TRDHits;
  int   HMult;
  float TRDPar[2];
  int   ECemag;
  int   ECmatc;
  float ECTOFcr[4];

  LVL3Root02();
  LVL3Root02(TriggerLVL302 *ptr);
 ~LVL3Root02(){};
ClassDef(LVL3Root02,1)       //LVL3Root02
};

class LVL1Root02 : public TObject {
public:
  int   Mode;
  int   TOFlag;
  int   TOFPatt[4];
  int   TOFPatt1[4];
  int   AntiPatt;
  int   ECALflag;
  float ECALtrsum;

  LVL1Root02();
  ~LVL1Root02(){};
  LVL1Root02(Trigger2LVL1 *ptr);
ClassDef(LVL1Root02,1)       //LVL1Root02
};

class TrRawClusterRoot : public TObject {
public:
  int address;
  int nelem;
  float s2n;

  TrRawClusterRoot();
  ~TrRawClusterRoot(){};
  TrRawClusterRoot(AMSTrRawCluster *ptr, int addr);

ClassDef(TrRawClusterRoot,1)       //TrRawClusterRoot
};


class TOFRawClusterRoot : public TObject {
public:
  int   Status;
  int   Layer;
  int   Bar;
  float tovta[2];
  float tovtd[2];
  float tovtdl[2];
  float sdtm[2];
  float edepa;
  float edepd;
  float edepdl;
  float time;
  float cool;

  TOFRawClusterRoot();
  ~TOFRawClusterRoot(){};
  TOFRawClusterRoot(TOF2RawCluster *ptr);

ClassDef(TOFRawClusterRoot ,1)       //TOFRawClusterRoot 
};

class RICMCRoot : public TObject {
public:
  int   id;            // Particle id.
  float origin[3];     // Particle origin
  float direction[3];  // Original direction
  int   status;        // Status=10*number of reflections+(have it rayleigh?1:0)
  int   numgen;        // Number of generated photons
  int   eventpointer;  // Pointer to detected hit

  RICMCRoot();
  ~RICMCRoot(){};
  RICMCRoot(AMSRichMCHit *ptr, int _numgen);
ClassDef(RICMCRoot,1)       // RICMCRoot
};

class RICEventRoot : public TObject {
public:
  int   _channel;
  int   _counts;
  float _npe;
  unsigned int _status;
  float _x;
  float _y;

  RICEventRoot();
  ~RICEventRoot(){};
  RICEventRoot(AMSRichRawEvent *ptr, float x, float y);
ClassDef(RICEventRoot,1)       // RICEventRoot
};

class RICRingRoot : public TObject {
public:

  int   used;
  int   mused;
  float beta;
  float errorbeta;
  float quality;
  unsigned int status;
  // float betablind;
  float probkl;
  float npexp;
  float collected_npe;

  float npexpg;
  float npexpr;
  float npexpb;

#ifndef __ROOTPOINTERS__
  TRef  fTrack;
#else
  int fTrack;
#endif
  RICRingRoot();
  ~RICRingRoot(){};
  RICRingRoot(AMSRichRing *ptr);
ClassDef(RICRingRoot,1)           // RICRingRoot
}; 


class EventRoot02: public TObject {
public:
  AMSEventHeaderRoot  Header;

#ifdef __WRITEROOTCLONES__
  TClonesArray *fBeta; 
  TClonesArray *fCharge;  
  TClonesArray *fParticle;  
  TClonesArray *fTOFcluster;  
  TClonesArray *fECALshower;
  TClonesArray *fECALcluster;
  TClonesArray *fECAL2Dcluster;
  TClonesArray *fECALhit;
  TClonesArray *fTOFMCcluster;
  TClonesArray *fTrCluster;
  TClonesArray *fTrMCCluster;
  TClonesArray *fTRDMCCluster;
  TClonesArray *fTRDrawhit;
  TClonesArray *fTRDcluster;
  TClonesArray *fTRDsegment;
  TClonesArray *fTRDtrack;
  TClonesArray *fTRrechit;
  TClonesArray *fTRtrack;
  TClonesArray *fMCtrtrack;
  TClonesArray *fMCeventg;
  TClonesArray *fAntiCluster;
  TClonesArray *fAntiMCCluster;
  TClonesArray *fLVL3;
  TClonesArray *fLVL1;
  TClonesArray *fTrRawCluster;
  TClonesArray *fTOFRawCluster;
  TClonesArray *fRICMC;
  TClonesArray *fRICEvent;
  TClonesArray *fRICRing;
//  TClonesArray *fTrGamma; 
#else
  vector<BetaRoot02> fBeta; 
  vector<ChargeRoot02> fCharge;  
  vector<ParticleRoot02> fParticle;  
  vector<TOFClusterRoot> fTOFcluster;  
  vector<EcalShowerRoot> fECALshower;
  vector<EcalClusterRoot> fECALcluster;
  vector<Ecal2DClusterRoot> fECAL2Dcluster;
  vector<EcalHitRoot> fECALhit;
  vector<TOFMCClusterRoot> fTOFMCcluster;
  vector<TrClusterRoot> fTrCluster;
  vector<TrMCClusterRoot> fTrMCCluster;
  vector<TRDMCClusterRoot> fTRDMCCluster;
  vector<TRDRawHitRoot> fTRDrawhit;
  vector<TRDClusterRoot> fTRDcluster;
  vector<TRDSegmentRoot> fTRDsegment;
  vector<TRDTrackRoot> fTRDtrack;
  vector<TrRecHitRoot02> fTRrechit;
  vector<TrTrackRoot02> fTRtrack;
  vector<MCTrackRoot> fMCtrtrack;
  vector<MCEventGRoot02> fMCeventg;
  vector<AntiClusterRoot> fAntiCluster;
  vector<ANTIMCClusterRoot> fAntiMCCluster;
  vector<LVL3Root02> fLVL3;
  vector<LVL1Root02> fLVL1;
  vector<TrRawClusterRoot> fTrRawCluster;
  vector<TOFRawClusterRoot> fTOFRawCluster;
  vector<RICMCRoot> fRICMC;
  vector<RICEventRoot> fRICEvent;
  vector<RICRingRoot> fRICRing;
//  vector<TrGammaRoot02> fTrGamma; 
  vector<int>         fAux;
#endif

EventRoot02();
void Set(AMSEvent *ptr, int rawwords);
~EventRoot02(){};
void         AddAMSObject(AMSAntiCluster *ptr);
void         AddAMSObject(AMSAntiMCCluster *ptr);
void         AddAMSObject(AMSBeta *ptr);
void         AddAMSObject(AMSCharge *ptr, float probtof[],int chintof[],
                          float probtr[], int chintr[], float probrc[], 
                          int chinrc[], float proballtr);
void         AddAMSObject(AMSEcalHit *ptr);
void         AddAMSObject(AMSmceventg *ptr);
void         AddAMSObject(AMSmctrack *ptr);
void         AddAMSObject(AMSParticle *ptr, float phi, float phigl);
void         AddAMSObject(AMSRichMCHit *ptr, int numgen);
void         AddAMSObject(AMSRichRing *ptr);
void         AddAMSObject(AMSRichRawEvent *ptr, float x, float y);
void         AddAMSObject(AMSTOFCluster *ptr, int p2memb[]);
void         AddAMSObject(AMSTOFMCCluster *ptr);
void         AddAMSObject(AMSTrRecHit *ptr);
void         AddAMSObject(AMSTRDCluster *ptr);
void         AddAMSObject(AMSTRDMCCluster *ptr);
void         AddAMSObject(AMSTRDRawHit *ptr);
void         AddAMSObject(AMSTRDSegment *ptr);
void         AddAMSObject(AMSTRDTrack *ptr);
void         AddAMSObject(AMSTrCluster *ptr, float ampl[]);
void         AddAMSObject(AMSTrMCCluster *ptr);
void         AddAMSObject(AMSTrRawCluster *ptr, int addr);
void         AddAMSObject(AMSTrTrack *ptr);
//void         AddAMSObject(AMSTrTrackGamma *ptr);
void         AddAMSObject(Ecal1DCluster *ptr);
void         AddAMSObject(Ecal2DCluster  *ptr);
void         AddAMSObject(EcalShower  *ptr);
void         AddAMSObject(TOF2RawCluster *ptr);
void         AddAMSObject(Trigger2LVL1 *ptr);
void         AddAMSObject(TriggerLVL302 *ptr);
void clear();

ClassDef(EventRoot02,4)       //EventRoot02
};

#endif

#endif
