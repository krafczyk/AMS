//  $Id: root.h,v 1.81 2003/05/08 16:42:13 choutko Exp $
#ifndef __AMSROOT__
#define __AMSROOT__

#ifdef __WRITEROOT__
#include <TObject.h>
#include <TTree.h>
#include <TFile.h>
#include <TROOT.h>
#include <TBranch.h>
#include <list>
#include <vector>
#include <iostream>
using namespace std;
#ifndef __ROOTSHAREDLIBRARY__
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
class EventNtuple02;
#else
class AMSAntiCluster{};
class AMSAntiMCCluster{};
class AMSBeta{};
class AMSCharge{};
class AMSEvent{};
class AMSEcalHit{};
class AMSmceventg{};
class AMSmctrack{};
class AMSParticle{};
class AMSRichMCHit{};
class AMSRichRawEvent{};
class AMSRichRing{};
class AMSNtuple{};
class AMSTOFCluster{};
class AMSTOFMCCluster{};
class AMSTRDCluster{};
class AMSTRDMCCluster{};
class AMSTRDRawHit{};
class AMSTRDSegment{};
class AMSTRDTrack{};
class AMSTrCluster{};
class AMSTrMCCluster{};
class AMSTrRawCluster{};
class AMSTrTrack{};
class AMSTrTrackGamma{};
class AMSTrRecHit{};
class Ecal1DCluster{};
class Ecal2DCluster{};
class EcalShower{};
class TOF2RawCluster{};
class Trigger2LVL1{};
class TriggerLVL302{};
class EventNtuple02{};
#endif
class HeaderR: public TObject {
 public:

// general info

  unsigned int Run;
  unsigned int RunType;
  unsigned int Event;
  unsigned int Status[2];  //event status
  int Raw;            //raw event length
  int Version;        //program build number
  int Time[2];        // unix time + usec time

// shuttle parameters

   float RadS;    // iss orbit altitude cm
   float ThetaS;  
   float PhiS;
   float NorthPolePhi;
   float Yaw;
   float Pitch;
   float Roll;
   float VelocityS;     // velocity 
   float VelTheta;
   float VelPhi;
   float ThetaM;        // magnetic theta
   float PhiM;

// counters

int   EcalHits;
int   EcalClusters;
int   Ecal2DClusters;
int   EcalShowers;
int   RichHits;
int   RichRings;
int   TofRawClusters;
int   TofClusters;  
int   AntiClusters;
int   TrRawClusters;
int   TrClusters;
int   TrRecHits;
int   TrTracks;
int   TrdRawHits;
int   TrdClusters;
int   TrdSegments;
int   TrdTracks;
int   Level1s;
int   Level3s;
int   Betas; 
int   Vertexs; 
int   Charges;  
int   Particles;  
int   AntiMCClusters;
int   TrMCClusters;
int   TofMCClusters;
int   TrdMCClusters;
int   RichMCClusters;
int   MCTracks;
int   MCEventgs;


  HeaderR(){};
  void   Set(EventNtuple02 *ptr);
  ClassDef(HeaderR,1)       //HeaderR
};



class EcalHitR : public TObject {
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
  EcalHitR(AMSEcalHit *ptr);
  EcalHitR(){};
ClassDef(EcalHitR,1)       //EcalHitR
};


class EcalClusterR : public TObject {
public:
  int Status;
  int Proj;
  int Plane;
  int Left;
  int Center;
  int Right;
  float Edep;
  float SideLeak;
  float DeadLeak;
  float Coo[3];
  int NHits;
  vector <int> fEcalHitR;
  EcalClusterR(){};
  EcalClusterR(Ecal1DCluster *ptr);

ClassDef(EcalClusterR,1)       //EcalClusterR
};


class Ecal2DClusterR: public TObject {
public:
  int Status;
  int Proj;
  int Nmemb;
  float Edep;
  float Coo;
  float Tan;
  float Chi2;

  vector <int> fEcalClusterR;
  Ecal2DClusterR(){};
  Ecal2DClusterR(Ecal2DCluster *ptr);
ClassDef(Ecal2DClusterR,1)       //Ecal2DClusterR
};

class EcalShowerR : public TObject {
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
  float SphericityEV[3];
  int   N2dCl;

  vector <int> fEcal2DClusterR;
  EcalShowerR(){};
  EcalShowerR(EcalShower *ptr);

ClassDef(EcalShowerR,1)       //EcalShowerR

};




class RichHitR : public TObject {
public:
  int   _channel;
  int   _counts;
  float _npe;
  unsigned int _status;
  float _x;
  float _y;

  RichHitR(){};
  RichHitR(AMSRichRawEvent *ptr, float x, float y);
ClassDef(RichHitR,1)       // RichHitR
};

class RichRingR : public TObject {
public:

  int   used;
  int   mused;
  float beta;
  float errorbeta;
  float quality;
  unsigned int status;
  float probkl;
  float npexp;
  float collected_npe;

  float npexpg;
  float npexpr;
  float npexpb;

  int fTrTrack;   //pionter to TrTrack
  RichRingR(){};
  RichRingR(AMSRichRing *ptr);
  ClassDef(RichRingR,1)           // RichRingR
}; 



class TofRawClusterR : public TObject {
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
  TofRawClusterR(){};
  TofRawClusterR(TOF2RawCluster *ptr);

  ClassDef(TofRawClusterR ,1)       //TofRawClusterR
};


class TofClusterR : public TObject {
public:
  int Status;
  int Layer;
  int Bar;
  int Nmemb;
  float Edep;
  float Edepd;
  float Time;
  float ErrTime;
  float Coo[3];
  float ErrorCoo[3];
  vector<int> fTofRawCluster;
  TofClusterR(){};
  TofClusterR(AMSTOFCluster *ptr );

ClassDef(TofClusterR,1)       //TofClusterR
};


class AntiClusterR : public TObject {
public:
  int   Status;
  int   Sector;
  int   Ntimes;
  int   Npairs;
  float Times[16];
  float Timese[16];
  float Edep;
  float Coo[3];   // R, phi, Z
  float ErrorCoo[3];

  AntiClusterR(){};
  AntiClusterR(AMSAntiCluster *ptr);
ClassDef(AntiClusterR,1)       //AntiClusterR
};


class TrRawClusterR : public TObject {
public:
  int address;
  int nelem;
  float s2n;

  TrRawClusterR(){};
  TrRawClusterR(AMSTrRawCluster *ptr);

ClassDef(TrRawClusterR,1)       //TrRawClusterR
};


class TrClusterR : public TObject {
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
  vector<float> Amplitude;   // NelemR-NelemL

  TrClusterR(){};
  TrClusterR(AMSTrCluster *ptr);
ClassDef(TrClusterR,1)       //TrClusterR
};



class TrRecHitR : public TObject {
public:
  int   Status;
  int   Layer;
  float Hit[3];
  float EHit[3];
  float Sum;
  float DifoSum;
  float CofgX;
  float CofgY;

  int  fTrClusterX;   //x proj pointer
  int  fTrClusterY;   //y proj pointer

  TrRecHitR(){};
  TrRecHitR(AMSTrRecHit *ptr);
ClassDef(TrRecHitR,1)       //TrRecHitR
};



class TrTrackR : public TObject {
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
  TrTrackR(){};
  TrTrackR(AMSTrTrack *ptr);
protected:
  vector<int> fTrRecHit;
public:
  int TrRecHit(unsigned int i){return i<fTrRecHit.size()?fTrRecHit[i]:-1;}
ClassDef(TrTrackR,1)       //TrTrackR
};

class TrdRawHitR : public TObject {
public:
  int Layer;
  int Ladder;
  int Tube;
  float Amp;

  TrdRawHitR(){};
  TrdRawHitR(AMSTRDRawHit *ptr);

ClassDef(TrdRawHitR,1)       //TrdRawHitR
};

class TrdClusterR : public TObject {
public:
  int   Status;
  float Coo[3];
  int   Layer;
  float CooDir[3];
  int   Multip;
  int   HMultip;
  float EDep;

  int fTrdRawHit;

  TrdClusterR(){};
  TrdClusterR(AMSTRDCluster *ptr);
ClassDef(TrdClusterR,1)       //TrdClusterR
};

class TrdSegmentR : public TObject {
public:
  int   Status;
  int   Orientation;
  float FitPar[2];
  float Chi2;
  int   Pattern;
  int   Nhits;
 
  vector<int> fTrdCluster;
  TrdSegmentR(){};
  TrdSegmentR(AMSTRDSegment *ptr);

ClassDef(TrdSegmentR,1)       //TrdSegmentR
};


class TrdTrackR : public TObject {
public:
  int   Status;
  float Coo[3];
  float ErCoo[3];
  float Phi;
  float Theta;
  float Chi2;
  int   NSeg;
  int   Pattern;
  vector<int> fTrdSegment;
  TrdTrackR(AMSTRDTrack *ptr);
  TrdTrackR(){};
ClassDef(TrdTrackR,1)       //TrdTrackR
};


class Level1R : public TObject {
public:
  int   Mode;
  int   TOFlag;
  int   TOFPatt[4];
  int   TOFPatt1[4];
  int   AntiPatt;
  int   ECALflag;
  float ECALtrsum;

  Level1R(){};
  Level1R(Trigger2LVL1 *ptr);
ClassDef(Level1R,1)       //Level1R
};


class Level3R : public TObject {
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

  Level3R(){};
  Level3R(TriggerLVL302 *ptr);
ClassDef(Level3R,1)       //Level3R
};







class BetaR : public TObject {
 public:
  int       Status;
  int       Pattern;
  float     Beta;
  float     BetaC;
  float     Error;
  float     ErrorC;
  float     Chi2;
  float     Chi2S;
  int   fTrTrack;
  vector<int> fTofCluster;
   BetaR(){};
   BetaR(AMSBeta *ptr);
   ClassDef(BetaR,1)         //BetaR
};



class ChargeR : public TObject {
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
  int  fBeta;
  int  fRich;
  ChargeR(){};
  ChargeR(AMSCharge *ptr, float probtof[],int chintof[],
               float probtr[], int chintr[], 
               float probrc[], int chinrc[], float proballtr);
ClassDef(ChargeR,1)       //ChargeR

};

class ParticleR : public TObject {
public:
  int Status;
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

  int  fBeta;
  int  fCharge;
  int  fTrack;      // pointer to track;
  int  fTrd;        // pointer to trd track
  int  fRich;       // pointer to rich ring
  int  fShower;     // pointer to shower;

  ParticleR(){};
  ParticleR(AMSParticle *ptr, float phi, float phigl);
  ClassDef(ParticleR,1)       //ParticleR
};


class AntiMCClusterR : public TObject {
public:
  int   Idsoft;
  float Coo[3];
  float TOF;
  float Edep;

  AntiMCClusterR(){};
  AntiMCClusterR(AMSAntiMCCluster *ptr);
ClassDef(AntiMCClusterR,1)       //AntiMCClusterR
};

class TrMCClusterR : public TObject {
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

  TrMCClusterR(){};
  TrMCClusterR(AMSTrMCCluster *ptr);
ClassDef(TrMCClusterR,1)       //TrMCClusterR
};


class TofMCClusterR : public TObject {
public:
  int   Idsoft;
  float Coo[3];
  float TOF;
  float Edep;
  TofMCClusterR(){};
  TofMCClusterR(AMSTOFMCCluster *ptr);
ClassDef(TofMCClusterR,1)       //TOFMCClusterRoot
};


class TrdMCClusterR : public TObject {
public:
  int   Layer;
  int   Ladder;
  int   Tube;
  int   ParticleNo;
  float Edep;
  float Ekin;
  float Xgl[3];
  float Step;
 
  TrdMCClusterR(){};
  TrdMCClusterR(AMSTRDMCCluster *ptr);
ClassDef(TrdMCClusterR,1)       //TrdMCClusterR
};


class RichMCClusterR : public TObject {
public:
  int   id;            // Particle id.
  float origin[3];     // Particle origin
  float direction[3];  // Original direction
  int   status;        // Status=10*number of reflections+(have it rayleigh?1:0)
  int   numgen;        // Number of generated photons
  int   eventpointer;  // Pointer to detected hit

  RichMCClusterR(){};
  RichMCClusterR(AMSRichMCHit *ptr, int _numgen);
  ClassDef(RichMCClusterR,1)       // RichMCClusterR
};



class MCTrackR : public TObject {
public:
float _radl;
float _absl;
float _pos[3];
char  _vname[4];

 MCTrackR(){};
 MCTrackR(AMSmctrack *ptr);
ClassDef(MCTrackR,1)       //MCTrackR
};


class MCEventgR : public TObject {
public:
  int Nskip;
  int Particle;
  float Coo[3];
  float Dir[3];
  float Momentum;
  float Mass;
  float Charge;
  MCEventgR(){};
  MCEventgR(AMSmceventg *ptr);
ClassDef(MCEventgR,1)       //MCEventgR
};



class AMSEventR: public  TObject {

protected:

static TBranch*  bHeader;
static TBranch*  bEcalHit;
static TBranch*  bEcalCluster;
static TBranch*  bEcal2DCluster;
static TBranch*  bEcalShower;
static TBranch*  bRichHit;
static TBranch*  bRichRing;
static TBranch*  bTofRawCluster;
static TBranch*  bTofCluster;
static TBranch*  bAntiCluster;
static TBranch*  bTrRawCluster;
static TBranch*  bTrCluster;
static TBranch*  bTrRecHit;
static TBranch*  bTrTrack;
static TBranch*  bTrdRawHit;
static TBranch*  bTrdCluster;
static TBranch*  bTrdSegment;
static TBranch*  bTrdTrack;
static TBranch*  bLevel1;
static TBranch*  bLevel3;
static TBranch*  bBeta;
static TBranch*  bVertex;
static TBranch*  bCharge;
static TBranch*  bParticle;
static TBranch*  bAntiMCCluster;
static TBranch*  bTrMCCluster;
static TBranch*  bTofMCCluster;
static TBranch*  bTrdMCCluster;
static TBranch*  bRichMCCluster;
static TBranch*  bMCTrack;
static TBranch*  bMCEventg;
static AMSEventR * _Head;
static int         _Entry;
public:

 static char      * _Name;
 void SetBranchA(TTree *tree);
 void GetBranchA(TTree *tree);
 void SetCont(); 
 int & Entry(){return _Entry;}
public:


 
  HeaderR  fHeader;
  void ReadHeader(int Entry);
  protected:
  
  //ECAL 

  vector<EcalHitR> fEcalHit;
  vector<EcalClusterR> fEcalCluster;
  vector<Ecal2DClusterR> fEcal2DCluster;
  vector<EcalShowerR> fEcalShower;


  //RICH
  vector<RichHitR> fRichHit;
  vector<RichRingR> fRichRing;



  //TOF
  vector<TofRawClusterR> fTofRawCluster;
  vector<TofClusterR> fTofCluster;  


  //Anti

  vector<AntiClusterR> fAntiCluster;

  void _RAntiCluster();

  //Tracker

  vector<TrRawClusterR> fTrRawCluster;
  vector<TrClusterR> fTrCluster;
  vector<TrRecHitR> fTrRecHit;
  vector<TrTrackR> fTrTrack;

  //TRD

  vector<TrdRawHitR> fTrdRawHit;
  vector<TrdClusterR> fTrdCluster;
  vector<TrdSegmentR> fTrdSegment;
  vector<TrdTrackR> fTrdTrack;


  //Triggers

  vector<Level1R> fLevel1;
  vector<Level3R> fLevel3;


  //AxAMS
  vector<BetaR> fBeta; 
  vector<ChargeR> fCharge;  
  vector<ParticleR> fParticle;  



  //MC SubDet
  vector<AntiMCClusterR> fAntiMCCluster;
  vector<TrMCClusterR>   fTrMCCluster;
  vector<TofMCClusterR>  fTofMCCluster;
  vector<TrdMCClusterR>  fTrdMCCluster;
  vector<RichMCClusterR> fRichMCCluster;


  //MC General

  vector<MCTrackR>       fMCTrack;
  vector<MCEventgR>      fMCEventg;


  //Aux

   vector<float>         fAux;

   public:


      unsigned int   NEcalHit()  {
        if(fHeader.EcalHits && fEcalHit.size()==0)bEcalHit->GetEntry(_Entry);
        return fEcalHit.size();
      }
      vector<EcalHitR> & EcalHit()  {
        if(fHeader.EcalHits && fEcalHit.size()==0)bEcalHit->GetEntry(_Entry);
         return  fEcalHit;
       }

       EcalHitR &   EcalHit(unsigned int l) {
        if(fHeader.EcalHits && fEcalHit.size()==0)bEcalHit->GetEntry(_Entry);
         return fEcalHit.at(l);
      }

      EcalHitR *   pEcalHit(unsigned int l) {
        if(fHeader.EcalHits && fEcalHit.size()==0)bEcalHit->GetEntry(_Entry);
        return l<fEcalHit.size()?&(fEcalHit[l]):0;
      }



      unsigned int   NTrRecHit()  {
        if(fHeader.TrRecHits && fTrRecHit.size()==0)bTrRecHit->GetEntry(_Entry);
        return fTrRecHit.size();
      }
      vector<TrRecHitR> & TrRecHit()  {
        if(fHeader.TrRecHits && fTrRecHit.size()==0)bTrRecHit->GetEntry(_Entry);
         return  fTrRecHit;
       }

       TrRecHitR &   TrRecHit(unsigned int l) {
        if(fHeader.TrRecHits && fTrRecHit.size()==0)bTrRecHit->GetEntry(_Entry);
         return fTrRecHit.at(l);
      }

      TrRecHitR *   pTrRecHit(unsigned int l) {
        if(fHeader.TrRecHits && fTrRecHit.size()==0)bTrRecHit->GetEntry(_Entry);
        return l<fTrRecHit.size()?&(fTrRecHit[l]):0;
      }


      unsigned int   NTrTrack()  {
        if(fHeader.TrTracks && fTrTrack.size()==0)bTrTrack->GetEntry(_Entry);
        return fTrTrack.size();
      }
      vector<TrTrackR> & TrTrack()  {
        if(fHeader.TrTracks && fTrTrack.size()==0)bTrTrack->GetEntry(_Entry);
         return  fTrTrack;
       }

       TrTrackR &   TrTrack(unsigned int l) {
        if(fHeader.TrTracks && fTrTrack.size()==0)bTrTrack->GetEntry(_Entry);
         return fTrTrack.at(l);
      }

      TrTrackR *   pTrTrack(unsigned int l) {
        if(fHeader.TrTracks && fTrTrack.size()==0)bTrTrack->GetEntry(_Entry);
        return l<fTrTrack.size()?&(fTrTrack[l]):0;
      }


      unsigned int   NTrdTrack()  {
        if(fHeader.TrdTracks && fTrdTrack.size()==0)bTrdTrack->GetEntry(_Entry);
        return fTrdTrack.size();
      }
      vector<TrdTrackR> & TrdTrack()  {
        if(fHeader.TrdTracks && fTrdTrack.size()==0)bTrdTrack->GetEntry(_Entry);
         return  fTrdTrack;
       }

       TrdTrackR &   TrdTrack(unsigned int l) {
        if(fHeader.TrdTracks && fTrdTrack.size()==0)bTrdTrack->GetEntry(_Entry);
         return fTrdTrack.at(l);
      }

      TrdTrackR *   pTrdTrack(unsigned int l) {
        if(fHeader.TrdTracks && fTrdTrack.size()==0)bTrdTrack->GetEntry(_Entry);
        return l<fTrdTrack.size()?&(fTrdTrack[l]):0;
      }



      unsigned int   NBeta()  {
        if(fHeader.Betas && fBeta.size()==0)bBeta->GetEntry(_Entry);
        return fBeta.size();
      }
      vector<BetaR> & Beta()  {
        if(fHeader.Betas && fBeta.size()==0)bBeta->GetEntry(_Entry);
         return  fBeta;
       }

       BetaR &   Beta(unsigned int l) {
        if(fHeader.Betas && fBeta.size()==0)bBeta->GetEntry(_Entry);
         return fBeta.at(l);
      }

      BetaR *   pBeta(unsigned int l) {
        if(fHeader.Betas && fBeta.size()==0)bBeta->GetEntry(_Entry);
        return l<fBeta.size()?&(fBeta[l]):0;
      }



      unsigned int   NParticle()  {
        if(fHeader.Particles && fParticle.size()==0)bParticle->GetEntry(_Entry);
        return fParticle.size();
      }
      vector<ParticleR> & Particle()  {
        if(fHeader.Particles && fParticle.size()==0)bParticle->GetEntry(_Entry);
         return  fParticle;
       }

       ParticleR &   Particle(unsigned int l) {
        if(fHeader.Particles && fParticle.size()==0)bParticle->GetEntry(_Entry);
         return fParticle.at(l);
      }

      ParticleR *   pParticle(unsigned int l) {
        if(fHeader.Particles && fParticle.size()==0)bParticle->GetEntry(_Entry);
        return l<fParticle.size()?&(fParticle[l]):0;
      }




      unsigned int   NMCEventg()  {
        if(fHeader.MCEventgs && fMCEventg.size()==0)bMCEventg->GetEntry(_Entry);
        return fMCEventg.size();
      }
      vector<MCEventgR> & MCEventg()  {
        if(fHeader.MCEventgs && fMCEventg.size()==0)bMCEventg->GetEntry(_Entry);
         return  fMCEventg;
       }

       MCEventgR &   MCEventg(unsigned int l) {
        if(fHeader.MCEventgs && fMCEventg.size()==0)bMCEventg->GetEntry(_Entry);
         return fMCEventg.at(l);
      }

      MCEventgR *   pMCEventg(unsigned int l) {
        if(fHeader.MCEventgs && fMCEventg.size()==0)bMCEventg->GetEntry(_Entry);
        return l<fMCEventg.size()?&(fMCEventg[l]):0;
      }











AMSEventR();

virtual ~AMSEventR(){_Head=0;};

void clear();

#ifndef __ROOTSHAREDLIBRARY__
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
void         AddAMSObject(AMSTOFCluster *ptr);
void         AddAMSObject(AMSTOFMCCluster *ptr);
void         AddAMSObject(AMSTrRecHit *ptr);
void         AddAMSObject(AMSTRDCluster *ptr);
void         AddAMSObject(AMSTRDMCCluster *ptr);
void         AddAMSObject(AMSTRDRawHit *ptr);
void         AddAMSObject(AMSTRDSegment *ptr);
void         AddAMSObject(AMSTRDTrack *ptr);
void         AddAMSObject(AMSTrCluster *ptr);
void         AddAMSObject(AMSTrMCCluster *ptr);
void         AddAMSObject(AMSTrRawCluster *ptr);
void         AddAMSObject(AMSTrTrack *ptr);
void         AddAMSObject(Ecal1DCluster *ptr);
void         AddAMSObject(Ecal2DCluster  *ptr);
void         AddAMSObject(EcalShower  *ptr);
void         AddAMSObject(TOF2RawCluster *ptr);
void         AddAMSObject(Trigger2LVL1 *ptr);
void         AddAMSObject(TriggerLVL302 *ptr);
#endif

ClassDef(AMSEventR,1)       //AMSEventR
};






#endif
#endif














