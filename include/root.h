//  $Id: root.h,v 1.62 2002/05/22 09:01:43 alexei Exp $
#ifndef __AMSROOT__
#define __AMSROOT__

#ifdef __WRITEROOT__
#include <TObject.h>
#include <TTree.h>
#include <TFile.h>
#include <TROOT.h>
#else
#include <tkdbc.h>
#include <trddbc.h>
#include <ecaldbc.h>
#endif

class AMSNtuple;
namespace root{
const int MAXBETA02    = 50;
const int MAXCHARGE02  = 30;
const int MAXPART02    =  10;
const int MAXTOF     =    40;
const int MAXTOFRAW  =    40;
const int MAXTOFMC   =   200;
const int MAXANTICL  =    16;
const int MAXANTIRAW =    32;
const int MAXANTIMC  =   200;
const int MAXTRCL    =   200;
const int MAXTRRAW   =   300;
const int MAXTRCLMC  =   200;
const int MAXTRRH02    =   666;
const int MAXTRTR02    =   100;
const int MAXMCG02     =    100;
const int MAXECSHOW =    5;
const int MAXECCLUST =    60;
const int MAXEC2DCLUST =    10;
const int MAXECHITS  =   500;
const int MAXLVL3    =     2;
const int MAXLVL1    =     2;
const int MAXRICMC   =   300;
const int MAXRICHITS =   100;
const int MAXRICHRIN =   10;
const int MAXTRDCLMC   =   200;
const int MAXTRDRHT   =   200;
const int MAXTRDCL   =   100;
const int MAXTRDSEG   =   100;
const int MAXTRDTRK   =   40;
const int MAXMCVOL   =   150;
};
using namespace root;

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
  int          fNbeta;
  TClonesArray *fBeta; 

  int          fNcharge; 
  TClonesArray *fCharge;  

  int          fNparticle; 
  TClonesArray *fParticle;  

  int          fNTOFcluster; 
  TClonesArray *fTOFcluster;  

  int          fNECALshower;
  TClonesArray *fECALshower;

  int          fNECALcluster;
  TClonesArray *fECALcluster;

  int          fNECAL2Dcluster;
  TClonesArray *fECAL2Dcluster;

  int          fNECALhit;
  TClonesArray *fECALhit;

  int          fNTOFMCcluster;
  TClonesArray *fTOFMCcluster;

  int          fNTrCluster;
  TClonesArray *fTrCluster;

  int          fNTrMCCluster;
  TClonesArray *fTrMCCluster;

  int          fNTRDMCCluster;
  TClonesArray *fTRDMCCluster;

  int          fNTRDrawhit;
  TClonesArray *fTRDrawhit;

  int          fNTRDcluster;
  TClonesArray *fTRDcluster;

  int          fNTRDsegment;
  TClonesArray *fTRDsegment;

  int          fNTRDtrack;
  TClonesArray *fTRDtrack;

  int          fNTRrechit;
  TClonesArray *fTRrechit;

  int          fNTRtrack;
  TClonesArray *fTRtrack;

  int          fNMCtrtrack;
  TClonesArray *fMCtrtrack;

  int          fNMCeventg;
  TClonesArray *fMCeventg;

  int          fNAntiCluster;
  TClonesArray *fAntiCluster;

  int          fNAntiMCCluster;
  TClonesArray *fAntiMCCluster;

  int          fNLVL3;
  TClonesArray *fLVL3;

  int          fNLVL1;
  TClonesArray *fLVL1;

  int          fNTrRawCluster;
  TClonesArray *fTrRawCluster;

  int          fNAntiRawCluster;
  TClonesArray *fAntiRawCluster;

  int          fNTOFRawCluster;
  TClonesArray *fTOFRawCluster;

  int          fNRICMC;
  TClonesArray *fRICMC;

  int          fNRICEvent;
  TClonesArray *fRICEvent;

  int          fNRICRing;
  TClonesArray *fRICRing;

  TClonesArray *Get_fbeta()  {return fBeta;}
  int           Get_fNbeta() {return fNbeta;}
  void          Set_fNbeta(int N) {fNbeta=N;}

  TClonesArray *Get_fcharge()  {return fCharge;}
  int           Get_fNcharge() {return fNcharge;}
  void          Set_fNcharge(int N) {fNcharge=N;}

  TClonesArray *Get_fparticle()  {return fParticle;}
  int           Get_fNparticle() {return fNparticle;}
  void          Set_fNparticle(int N) {fNparticle=N;}

  TClonesArray *Get_ftofcluster()  {return fTOFcluster;}
  int           Get_fNtofcluster() {return fNTOFcluster;}
  void          Set_fNtofcluster(int N) {fNTOFcluster=N;}

  TClonesArray *Get_fecalshower()  {return fECALshower;}
  int           Get_fNecalshower() {return fNECALshower;}
  void          Set_fNecalshower(int N) {fNECALshower=N;}

  TClonesArray *Get_fecalcluster()  {return fECALcluster;}
  int           Get_fNecalcluster() {return fNECALcluster;}
  void          Set_fNecalcluster(int N) {fNECALcluster=N;}

  TClonesArray *Get_fecal2dcluster()  {return fECAL2Dcluster;}
  int           Get_fNecal2dcluster() {return fNECAL2Dcluster;}
  void          Set_fNecal2dcluster(int N) {fNECAL2Dcluster=N;}

  TClonesArray *Get_fecalhit()  {return fECALhit;}
  int           Get_fNecalhit() {return fNECALhit;}
  void          Set_fNecalhit(int N) {fNECALhit=N;}

  TClonesArray *Get_ftofmccluster()  {return fTOFMCcluster;}
  int           Get_fNtofmccluster() {return fNTOFMCcluster;}
  void          Set_fNtofmccluster(int N) {fNTOFMCcluster=N;}

  TClonesArray *Get_ftrcluster()  {return fTrCluster;}
  int           Get_fNtrcluster() {return fNTrCluster;}
  void          Set_fNtrcluster(int N) {fNTrCluster=N;}

  TClonesArray *Get_ftrmccluster()  {return fTrMCCluster;}
  int           Get_fNtrmccluster() {return fNTrMCCluster;}
  void          Set_fNtrmccluster(int N) {fNTrMCCluster=N;}

  TClonesArray *Get_ftrdmccluster()  {return fTRDMCCluster;}
  int           Get_fNtrdmccluster() {return fNTRDMCCluster;}
  void          Set_fNtrdmccluster(int N) {fNTRDMCCluster=N;}

  TClonesArray *Get_ftrdrawhit()  {return fTRDrawhit;}
  int           Get_fNtrdrawhit() {return fNTRDrawhit;}
  void          Set_fNtrdrawhit(int N) {fNTRDrawhit=N;}

  TClonesArray *Get_ftrdcluster()  {return fTRDcluster;}
  int           Get_fNtrdcluster() {return fNTRDcluster;}
  void          Set_fNtrdcluster(int N) {fNTRDcluster=N;}

  TClonesArray *Get_ftrdsegment()  {return fTRDsegment;}
  int           Get_fNtrdsegment() {return fNTRDsegment;}
  void          Set_fNtrdsegment(int N) {fNTRDsegment=N;}

  TClonesArray *Get_ftrdtrack()  {return fTRDtrack;}
  int           Get_fNtrdtrack() {return fNTRDtrack;}
  void          Set_fNtrdtrack(int N) {fNTRDtrack=N;}

  TClonesArray *Get_ftrrechit()  {return fTRrechit;}
  int           Get_fNtrrechit() {return fNTRrechit;}
  void          Set_fNtrrechit(int N) {fNTRrechit=N;}

  TClonesArray *Get_ftrtrack()  {return fTRtrack;}
  int           Get_fNtrtrack() {return fNTRtrack;}
  void          Set_fNtrtrack(int N) {fNTRtrack=N;}
  void          Inc_fNtrtrack() {fNTRtrack++;}

  TClonesArray *Get_fmctrtrack()  {return fMCtrtrack;}
  int           Get_fNmctrtrack() {return fNMCtrtrack;}
  void          Set_fNmctrtrack(int N) {fNMCtrtrack=N;}

  TClonesArray *Get_fmceventg()  {return fMCeventg;}
  int           Get_fNmeventg()  {return fNMCeventg;}
  void          Set_fNmceventg(int N) {fNMCeventg=N;}

  TClonesArray *Get_fanticluster()  {return fAntiCluster;}
  int           Get_fNanticluser()  {return fNAntiCluster;}
  void          Set_fNanticluster(int N) {fNAntiCluster=N;}

  TClonesArray *Get_fantimccluster()  {return fAntiMCCluster;}
  int           Get_fNantimccluser()  {return fNAntiMCCluster;}
  void          Set_fNantimccluster(int N) {fNAntiMCCluster=N;}

  TClonesArray *Get_flvl3()   {return fLVL3;}
  int           Get_fNlvl3()  {return fNLVL3;}
  void          Set_fNlvl3(int N) {fNLVL3=N;}

  TClonesArray *Get_flvl1()   {return fLVL1;}
  int           Get_fNlvl1()  {return fNLVL1;}
  void          Set_fNlvl1(int N) {fNLVL1=N;}

  TClonesArray *Get_ftrrawcluster()   {return fTrRawCluster;}
  int           Get_fNtrrawcluster()  {return fNTrRawCluster;}
  void          Set_fNtrrawcluster(int N) {fNTrRawCluster = N;}

  TClonesArray *Get_fantirawcluster()   {return fAntiRawCluster;}
  int           Get_fNantirawcluster()  {return fNAntiRawCluster;}
  void          Set_fNantirawcluster(int N) {fNAntiRawCluster = N;}

  TClonesArray *Get_ftofrawcluster()   {return fTOFRawCluster;}
  int           Get_fNtofrawcluster()  {return fNTOFRawCluster;}
  void          Set_fNtofrawcluster(int N) {fNTOFRawCluster = N;}

  TClonesArray *Get_fricmc()   {return fRICMC;}
  int           Get_fNricmc()  {return fNRICMC;}
  void          Set_fNricmc(int N) {fNRICMC = N;}

  TClonesArray *Get_fricevent()   {return fRICEvent;}
  int           Get_fNricevent()  {return fNRICEvent;}
  void          Set_fNricevent(int N) {fNRICEvent = N;}

  TClonesArray *Get_fricring()   {return fRICRing;}
  int           Get_fNricring()  {return fNRICRing;}
  void          Set_fNricring(int N) {fNRICRing = N;}
#endif
#ifdef __WRITEROOT__
EventNtuple02();
ClassDef(EventNtuple02,1)       //EventNtuple02
#else
friend class AMSEvent;
friend class AMSNtuple;
#endif
};

#ifdef __WRITEROOT__
class BetaRoot02 : public TObject {
#else
class BetaRoot02  {
#endif
 public:
  int Status;
  int Pattern;
  float Beta;
  float BetaC;
  float Error;
  float ErrorC;
  float Chi2;
  float Chi2S;
  int NTOF;
  int pTOF[4];
  int pTr;

#ifdef __WRITEROOT__
  ~BetaRoot02() {};
   BetaRoot02();
   BetaRoot02(int status, int pattern, float beta, float betaC, float ErrBeta,
                float ErrBetaC, float chi2, float chi2S, int ntof, int ptof[],
                int ptr);

   ClassDef(BetaRoot02,1)       
#else
  friend class AMSBeta;
  friend class AMSNtuple;
#endif
};

class BetaNtuple02 {
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
friend class AMSBeta;
friend class AMSNtuple;
};

#ifdef __WRITEROOT__
class ChargeRoot02 : public TObject {
#else
class ChargeRoot02 {
#endif
public:
  int Status;
  int BetaP;
  int ChargeTOF;
  int ChargeTracker;
  float ProbTOF[4];
  int ChInTOF[4];
  float ProbTracker[4];
  int ChInTracker[4];
  float ProbAllTracker;
  float TrunTOF;
  float TrunTOFD;
  float TrunTracker;

#ifdef __WRITEROOT__
  ChargeRoot02();
  ChargeRoot02(int status, int betap, int chargetof, int chargetr, float probtof[],
               int chintof[], float probtr[], int chintr[], 
               float proballtr, float truntof, float truntofd, float truntr);
ClassDef(ChargeRoot02,1)       //ChargeRoot

#else
friend class AMSCharge;
friend class AMSNtuple;
#endif
};


class ChargeNtuple02 {
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

friend class AMSCharge;
friend class AMSNtuple;
};




#ifdef __WRITEROOT__
class ParticleRoot02 : public TObject {
#else
class ParticleRoot02 {
#endif
public:
  int   BetaP;
  int   ChargeP;
  int   TrackP;
  int   TRDP;
  int   RICHP;
  int   EcalP;
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
  float RichLength;
  ParticleRoot02();
  ~ParticleRoot02(){};
#ifdef __WRITEROOT__
  ParticleRoot02(int betap, int chargep, int trackp, int trdp, int richp, int ecalp,
                 int particle, int particlevice, float prob[], float fitmom, 
                 float mass, float errmass, float momentum, float errmomentum,
                 float beta, float errbeta, float charge, float theta, float phi,
                 float thetagl, float phigl, float coo[], float cutoff, 
                 float tofcoo[4][3], float anticoo[2][3], float ecalcoo[3][3], 
                 float trcoo[8][3], float trdcoo[], float richcoo[2][3], 
                 float richpath[], float richlength);
  ClassDef(ParticleRoot02,1)       //ParticleNtuple
#else
friend class AMSParticle;
friend class AMSNtuple;
#endif
};


class ParticleNtuple02 {
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
#ifdef __WRITEROOT__
  float TrCoo[MAXPART02][8][3];
#else
  float TrCoo[MAXPART02][trconst::maxlay][3];
#endif
  float TRDCoo[MAXPART02][3];
  float RichCoo[MAXPART02][2][3];
  float RichPath[MAXPART02][2];
  float RichLength[MAXPART02];
friend class AMSParticle;
friend class AMSNtuple;
};

//-------------------------
class TOFClusterNtuple {
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
};

#ifdef __WRITEROOT__
class TOFClusterRoot : public TObject {
#else
class TOFClusterRoot {
#endif
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

#ifdef __WRITEROOT__
  TOFClusterRoot();
  ~TOFClusterRoot(){};
  TOFClusterRoot(int status, int layer, int bar, int nmemb, float edep, float edepd,
                 float time, float errtime, float coo[], float errcoo[]);

ClassDef(TOFClusterRoot,1)       //TOFClusterRoot
#else
friend class AMSTOFCluster;
friend class AMSNtuple;
#endif
};
//--------------------------


class EcalShowerNtuple {
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
  float Orp2DEnergy[MAXECSHOW];
  float Chi2Profile[MAXECSHOW];
  float ParProfile[MAXECSHOW][4];
  float Chi2Trans[MAXECSHOW];
//  float TransProfile[MAXECSHOW][3];
  float SphericityEV[MAXECSHOW][3];
  int   p2DCl[MAXECSHOW][2];
friend class EcalShower;
friend class AMSNtuple;
};

#ifdef __WRITEROOT__
class EcalShowerRoot : public TObject {
#else
class EcalShowerRoot {
#endif
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
  float OrpLeak;
  float Orp2DEnergy;
  float Chi2Profile;
  float ParProfile[4];
  float Chi2Trans;
//  float TransProfile[3];
  float SphericityEV[3];
  int   p2DCl[2];
#ifdef __WRITEROOT__
  EcalShowerRoot();
  ~EcalShowerRoot(){};
  EcalShowerRoot(int status, float dir[], float emdir[], float entry[], 
                 float exit[], float cofg[], float ertheta, float chi2dir,
                 float firstlayeredep, float energyc, 
                 float energy3c, float energy5c, float energy9c, 
                 float erenergyc, float difosum, float sideleak, float rearleak,
                 float deadleak, float orpleak, float orp2denergy, 
                 float chi2profile, float  parprofile[], float chi2trans,
                 float sphericityev[], int p2dcl[]);

ClassDef(EcalShowerRoot,1)       //EcalShowerRoot
#else
friend class EcalShower;
friend class AMSNtuple;
#endif

};



class EcalClusterNtuple {
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
friend class Ecal1DCluster;
friend class AMSNtuple;
};

#ifdef __WRITEROOT__
class EcalClusterRoot : public TObject {
#else
class EcalClusterRoot {
#endif
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
  int pLeft;
  int NHits;
#ifdef __WRITEROOT__
  EcalClusterRoot();
  ~EcalClusterRoot(){};
  EcalClusterRoot(int status, int proj, int plane, int left, int center, int right,
                  float edep, float sideleak, float deadleak, float coo[],
                  int pleft, int nhits);
ClassDef(EcalClusterRoot,1)       //EcalClusterRoot
#else
friend class Ecal1DCluster;
friend class AMSNtuple;
#endif
};


class Ecal2DClusterNtuple {
public:
  int Nec2dcl;
  int Status[MAXEC2DCLUST];
  int Proj[MAXEC2DCLUST];
  int Nmemb[MAXEC2DCLUST];
  float Edep[MAXEC2DCLUST];
//  float SideLeak[MAXEC2DCLUST];
//  float DeadLeak[MAXEC2DCLUST];
  float Coo[MAXEC2DCLUST];
  float Tan[MAXEC2DCLUST];
  float Chi2[MAXEC2DCLUST];
  int   pCl[MAXEC2DCLUST][18];
friend class Ecal2DCluster;
friend class AMSNtuple;
};

#ifdef __WRITEROOT__
class Ecal2DClusterRoot : public TObject {
#else
class Ecal2DClusterRoot {
#endif
public:
  int Status;
  int Proj;
  int Nmemb;
  float Edep;
  float Coo;
  float Tan;
  float Chi2;
  int   pCl[18];
#ifdef __WRITEROOT__
  Ecal2DClusterRoot();
  ~Ecal2DClusterRoot(){};
  Ecal2DClusterRoot(int status, int proj, int nmemb, float edep, float coo,
                    float tan, float chi2, int pcl[]);
ClassDef(Ecal2DClusterRoot,1)       //Ecal2DClusterRoot
#else
friend class Ecal2DCluster;
friend class AMSNtuple;
#endif
};


//--------------------------
class EcalHitNtuple {
public:
  int Necht;
  int Status[MAXECHITS];
  int Idsoft[MAXECHITS];
  int Proj[MAXECHITS];
  int Plane[MAXECHITS];
  int Cell[MAXECHITS];
  float Edep[MAXECHITS];
  float Coo[MAXECHITS][3];

friend class AMSEcalHit;
friend class AMSNtuple;
};

#ifdef __WRITEROOT__
class EcalHitRoot : public TObject {
#else
class EcalHitRoot {
#endif
public:
  int   Status;
  int   Idsoft;
  int   Proj;
  int   Plane;
  int   Cell;
  float Edep;
  float Coo[3];

#ifdef __WRITEROOT__
  EcalHitRoot();
  ~EcalHitRoot(){};
  EcalHitRoot(int status, int idsoft, int proj, int plane, int cell, 
              float edep, float coo[]);
ClassDef(EcalHitRoot,1)       //EcalHitRoot
#else
friend class AMSEcalHit;
friend class AMSNtuple;
#endif
};
//-------------------------- 
class TOFMCClusterNtuple {
public:
  int Ntofmc;
  int Idsoft[MAXTOFMC];
  float Coo[MAXTOFMC][3];
  float TOF[MAXTOFMC];
  float Edep[MAXTOFMC];

friend class AMSTOFMCCluster;
friend class AMSNtuple;
};

#ifdef __WRITEROOT__
class TOFMCClusterRoot : public TObject {
#else
class TOFMCClusterRoot {
#endif
public:
  int   Idsoft;
  float Coo[3];
  float TOF;
  float Edep;

#ifdef __WRITEROOT__
  TOFMCClusterRoot();
  ~TOFMCClusterRoot(){};
  TOFMCClusterRoot(int idsoft, float coo[], float tof, float edep);
ClassDef(TOFMCClusterRoot,1)       //TOFMCClusterRoot
#else
friend class AMSTOFMCCluster;
friend class AMSNtuple;
#endif
};
//--------------------------
class TrClusterNtuple {
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
};

#ifdef __WRITEROOT__
class TrClusterRoot : public TObject {
#else
class TrClusterRoot {
#endif
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

friend class AMSTrCluster;
friend class AMSNtuple;
#ifdef __WRITEROOT__
  ~TrClusterRoot(){};
  TrClusterRoot();
  TrClusterRoot(int idsoft, int status, int neleml, int nelemr, float sum,
                float sigma, float mean, float rms, float errmean, float amlitude[]);
ClassDef(TrClusterRoot,1)       //TrClusterRoot
#endif
};

class TrMCClusterNtuple {
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
};

#ifdef __WRITEROOT__
class TrMCClusterRoot : public TObject {
#else
class TrMCClusterRoot {
#endif
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

#ifdef __WRITEROOT__
  TrMCClusterRoot();
  ~TrMCClusterRoot(){};
  TrMCClusterRoot(int idsoft, int trackno, int left[], int center[], int right[],
                  float ss[2][5], float xca[], float xcb[], float xgl[], float sum);
ClassDef(TrMCClusterRoot,1)       //TrMCClusterRoot
#else
friend class AMSTrMCCluster;
friend class AMSNtuple;
#endif
};

class TRDMCClusterNtuple {
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
 
friend class AMSTRDMCCluster;
friend class AMSNtuple;
};

#ifdef __WRITEROOT__
class TRDMCClusterRoot : public TObject {
#else
class TRDMCClusterRoot {
#endif
public:
  int   Layer;
  int   Ladder;
  int   Tube;
  int   TrackNo;
  float Edep;
  float Ekin;
  float Xgl[3];
  float Step;
 
#ifdef __WRITEROOT__
  TRDMCClusterRoot();
  ~TRDMCClusterRoot(){};
  TRDMCClusterRoot(int layer, int ladder, int tube, int trackno, 
                   float edep, float ekin, float xgl[], float step);
ClassDef(TRDMCClusterRoot,1)       //TRDMCClusterRoot
#else
friend class AMSTRDMCCluster;
friend class AMSNtuple;
#endif
};


class TRDRawHitNtuple {
public:
  int Ntrdht;
  int Layer[MAXTRDRHT];
  int Ladder[MAXTRDRHT];
  int Tube[MAXTRDRHT];
  float Amp[MAXTRDRHT];
friend class AMSTRDRawHit;
friend class AMSNtuple;
};

#ifdef __WRITEROOT__
class TRDRawHitRoot : public TObject {
#else
class TRDRawHitRoot {
#endif
public:
  int Layer;
  int Ladder;
  int Tube;
  float Amp;
#ifdef __WRITEROOT__
  TRDRawHitRoot();
  ~TRDRawHitRoot(){};
  TRDRawHitRoot(int layer, int ladder, int tube, float amp);
ClassDef(TRDRawHitRoot,1)       //TRDRawHitRoot
#else
friend class AMSTRDRawHit;
friend class AMSNtuple;
#endif
};


class TRDClusterNtuple {
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
 
friend class AMSTRDCluster;
friend class AMSNtuple;
};

#ifdef __WRITEROOT__
class TRDClusterRoot : public TObject {
#else
class TRDClusterRoot {
#endif
public:
  int   Status;
  float Coo[3];
  int   Layer;
  float CooDir[3];
  int   Multip;
  int   HMultip;
  float EDep;
  int   pRawHit;
 
#ifdef __WRITEROOT__
  TRDClusterRoot();
  ~TRDClusterRoot(){};
  TRDClusterRoot(int status, float coo[], int layer, float coodir[], int multip, 
                 int hmultip, float edep, int prawhit);
ClassDef(TRDClusterRoot,1)       //TRDClusterRoot
#else
friend class AMSTRDCluster;
friend class AMSNtuple;
#endif
};


class TRDSegmentNtuple {
public:
  int Ntrdseg;
  int   Status[MAXTRDSEG];
  int   Orientation[MAXTRDSEG];
  float FitPar[MAXTRDSEG][2];
  float Chi2[MAXTRDSEG];
  int Pattern[MAXTRDSEG];
  int Nhits[MAXTRDSEG];
#ifdef __WRITEROOT__
  int PCl[MAXTRDSEG][12];
#else
  int PCl[MAXTRDSEG][trdconst::maxhits];
#endif

friend class AMSTRDSegment;
friend class AMSNtuple;
};

#ifdef __WRITEROOT__
class TRDSegmentRoot : public TObject {
#else
class TRDSegmentRoot{
#endif
public:
  int   Status;
  int   Orientation;
  float FitPar[2];
  float Chi2;
  int   Pattern;
  int   Nhits;
#ifdef __WRITEROOT__
  int PCl[12];
#else
  int PCl[trdconst::maxhits];
#endif
 
#ifdef __WRITEROOT__
  TRDSegmentRoot();
  ~TRDSegmentRoot(){};
  TRDSegmentRoot(int status, int orientation, float fitpar[], float chi2,
                 int pattern, int nhits, int pcl[]);
ClassDef(TRDSegmentRoot,1)       //TRDSegmentRoot
#else
friend class AMSTRDSegment;
friend class AMSNtuple;
#endif
};



class TRDTrackNtuple {
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
#ifdef __WRITEROOT__
  int   pSeg[MAXTRDTRK][5];
#else
  int   pSeg[MAXTRDTRK][trdconst::maxseg];
#endif
 
friend class AMSTRDTrack;
friend class AMSNtuple;
};

#ifdef __WRITEROOT__
class TRDTrackRoot : public TObject {
#else
class TRDTrackRoot {
#endif
public:
  int   Status;
  float Coo[3];
  float ErCoo[3];
  float Phi;
  float Theta;
  float Chi2;
  int   NSeg;
  int   Pattern;
#ifdef __WRITEROOT__
  int   pSeg[5];
#else
  int   pSeg[trdconst::maxseg];
#endif
 
#ifdef __WRITEROOT__
  TRDTrackRoot();
  ~TRDTrackRoot(){};
  TRDTrackRoot(int status, float coo[], float ercoo[], float phi, float theta, 
               float chi2, int nseg, int pattern, int pseg[]);

ClassDef(TRDTrackRoot,1)       //TRDTrackRoot
#else
friend class AMSTRDTrack;
friend class AMSNtuple;
#endif
};




class TrRecHitNtuple02 {
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

friend class AMSTrRecHit;
friend class AMSNtuple;
};

#ifdef __WRITEROOT__
class TrRecHitRoot02 : public TObject {
#else
class TrRecHitRoot02 {
#endif
public:
  int   pX;
  int   pY;
  int   Status;
  int   Layer;
  float Hit[3];
  float EHit[3];
  float Sum;
  float DifoSum;
  float CofgX;
  float CofgY;

#ifdef __WRITEROOT__
  TrRecHitRoot02();
  ~TrRecHitRoot02(){};
  TrRecHitRoot02(int px, int py, int status, int layer, float hit[], float ehit[],
                 float sum, float difosum, float cofgx, float cofgy);
ClassDef(TrRecHitRoot02,1)       //TrRecHitRoot02
#else
friend class AMSTrRecHit;
friend class AMSNtuple;
#endif
};


class TrTrackNtuple02 {
public:
  int Ntrtr;
  int Status[MAXTRTR02];
  int Pattern[MAXTRTR02];
  int Address[MAXTRTR02];
  int NHits[MAXTRTR02];
#ifdef __WRITEROOT__
  int pHits[MAXTRTR02][8];
#else
  int pHits[MAXTRTR02][trconst::maxlay];
#endif
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

friend class AMSTrTrack;
friend class AMSNtuple;
};

#ifdef __WRITEROOT__
class TrTrackRoot02 : public TObject {
#else
class TrTrackRoot02 {
#endif
public:
  int Status;
  int Pattern;
  int Address;
  int NHits;
#ifdef __WRITEROOT__
  int pHits[8];
#else
  int pHits[trconst::maxlay];
#endif
  float LocDBAver;
  int   GeaneFitDone;
  int   AdvancedFitDone;
  float Chi2StrLine;
  float Chi2Circle;
  float CircleRidgidity;
  float Chi2FastFit;
  float Ridgidity;
  float ErrRidgidity;
  float Theta;
  float Phi;
  float P0[3];
  float GChi2;
  float GRidgidity;
  float GErrRidgidity;
  float HChi2[2];
  float HRidgidity[2];
  float HErrRidgidity[2];
  float HTheta[2];
  float HPhi[2];
  float HP0[2][3];
  float FChi2MS;
  float PiErrRig;
  float RidgidityMS;
  float PiRigidity;

#ifdef __WRITEROOT__
  TrTrackRoot02();
  ~TrTrackRoot02(){};
  TrTrackRoot02(int status, int pattern, int address, int nhits, int phits[],
                float locdbaver, int geanefitdone, int advancedfitdone, 
                float chi2strline, float chi2circle, float circleridgidity,
                float chi2fastfit, float ridgidity, float errridgidity,
                float theta, float phi, float p0[], float gchi2, 
                float gridgidity, float gerrridgidity,
                float hchi2[], float hridgidity[], float herrridgidity[],
                float htheta[], float hphi[], float hp0[2][3], 
                float fchi2ms, float pierrrig, float ridgidityms, float pirigidity);
ClassDef(TrTrackRoot02,1)       //TrTrackRoot02
#else
friend class AMSTrTrack;
friend class AMSNtuple;
#endif
};

class MCTrackNtuple {
public:
int Nmct;
float radl[MAXMCVOL];
float absl[MAXMCVOL];
float pos[MAXMCVOL][3];
char  vname[MAXMCVOL][4];
};

#ifdef __WRITEROOT__
class MCTrackRoot : public TObject {
#else
class MCTrackRoot {
#endif
public:
float _radl;
float _absl;
float _pos[3];
char  _vname[4];
#ifdef __WRITEROOT__
 MCTrackRoot();
 ~MCTrackRoot(){};
 MCTrackRoot(float radl, float absl, float pos[], char vname[]);
ClassDef(MCTrackRoot,1)       //MCTrackRoot
#endif
};


class MCEventGNtuple02 {
public:
  int Nmcg;
  int Nskip[MAXMCG02];
  int Particle[MAXMCG02];
  float Coo[MAXMCG02][3];
  float Dir[MAXMCG02][3];
  float Momentum[MAXMCG02];
  float Mass[MAXMCG02];
  float Charge[MAXMCG02];

friend class AMSmceventg;
friend class AMSNtuple;
};

#ifdef __WRITEROOT__
class MCEventGRoot02 : public TObject {
#else
class MCEventGRoot02 {
#endif
public:
  int Nskip;
  int Particle;
  float Coo[3];
  float Dir[3];
  float Momentum;
  float Mass;
  float Charge;

#ifdef __WRITEROOT__
  MCEventGRoot02();
  ~MCEventGRoot02(){};
  MCEventGRoot02(int nskip, int particle, float coo[], float dir[], 
                 float momentum, float mass, float charge);
ClassDef(MCEventGRoot02,1)       //MCEventGNtuple
#else
friend class AMSmceventg;
friend class AMSNtuple;
#endif
};



class AntiClusterNtuple {
public:
  int Nanti;
  int Status[MAXANTICL];
  int Sector[MAXANTICL];
  float Edep[MAXANTICL];
  float Coo[MAXANTICL][3];   // R, phi, Z
  float ErrorCoo[MAXANTICL][3];

friend class AMSAntiCluster;
friend class AMSNtuple;
};

#ifdef __WRITEROOT__
class AntiClusterRoot : public TObject {
#else
class AntiClusterRoot {
#endif
public:
  int   Status;
  int   Sector;
  float Edep;
  float Coo[3];   // R, phi, Z
  float ErrorCoo[3];

#ifdef __WRITEROOT__
  AntiClusterRoot();
  ~AntiClusterRoot(){};
  AntiClusterRoot(int status, int sector, float edep, float coo[], float errorcoo[]);
ClassDef(AntiClusterRoot,1)       //AntiClusterRoot
#else
friend class AMSAntiCluster;
friend class AMSNtuple;
#endif
};

class ANTIMCClusterNtuple {
public:
  int Nantimc;
  int Idsoft[MAXANTIMC];
  float Coo[MAXANTIMC][3];
  float TOF[MAXANTIMC];
  float Edep[MAXANTIMC];

friend class AMSAntiMCCluster;
friend class AMSNtuple;
};

#ifdef __WRITEROOT__
class ANTIMCClusterRoot : public TObject {
#else
class ANTIMCClusterRoot {
#endif
public:
  int   Idsoft;
  float Coo[3];
  float TOF;
  float Edep;

#ifdef __WRITEROOT__
  ANTIMCClusterRoot();
  ~ANTIMCClusterRoot(){};
  ANTIMCClusterRoot(int idsoft, float coo[], float tof, float edep);
ClassDef(ANTIMCClusterRoot,1)       //ANTIMCClusterRoot
#else
friend class AMSAntiMCCluster;
friend class AMSNtuple;
#endif
};


#ifdef __WRITEROOT__
class LVL3Root02 : public TObject {
#else
class LVL3Root02 {
#endif
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
#ifdef __WRITEROOT__
  LVL3Root02();
  LVL3Root02(int toftr, unsigned int trdtr, unsigned int trackertr, unsigned int maintr,
               int direction, int ntrhits, int npatfound, int pattern[],
               double residual[], double ftime, double eloss, int trdhits, 
               int hmult, float trdpar[], int ecemag, int ecematc, float ectofc[]);
 ~LVL3Root02(){};
ClassDef(LVL3Root02,1)       //LVL3Root02
#else
friend class TriggerLVL302;
friend class AMSNtuple;
#endif
};
class LVL3Ntuple02 {
public:
  int Nlvl3;
  int TOFTr[MAXLVL3];
  int TRDTr[MAXLVL3];
  int TrackerTr[MAXLVL3];
  int MainTr[MAXLVL3];
  int Direction[MAXLVL3];
  int NTrHits[MAXLVL3];
  int NPatFound[MAXLVL3];
  int Pattern[MAXLVL3][2];
  float Residual[MAXLVL3][2];
  float Time[MAXLVL3];
  float ELoss[MAXLVL3];
  int   TRDHits[MAXLVL3];
  int   HMult[MAXLVL3];
  float TRDPar[MAXLVL3][2];
  int ECemag[MAXLVL3];
  int ECmatc[MAXLVL3];
  float ECTOFcr[MAXLVL3][4];
friend class TriggerLVL302;
friend class AMSNtuple;
};



class LVL1Ntuple02 {
public:
  int Nlvl1;
  int Mode[MAXLVL1];
  int TOFlag[MAXLVL1];
  int TOFPatt[MAXLVL1][4];
  int TOFPatt1[MAXLVL1][4];
  int AntiPatt[MAXLVL1];
  int ECALflag[MAXLVL1];
  float ECALtrsum[MAXLVL1];

friend class Trigger2LVL1;
friend class AMSNtuple;
};

#ifdef __WRITEROOT__
class LVL1Root02 : public TObject {
#else
class LVL1Root02 {
#endif
public:
  int   Mode;
  int   TOFlag;
  int   TOFPatt[4];
  int   TOFPatt1[4];
  int   AntiPatt;
  int   ECALflag;
  float ECALtrsum;

#ifdef __WRITEROOT__
  LVL1Root02();
  ~LVL1Root02(){};
  LVL1Root02(unsigned int mode, int tofflag, unsigned int tofpatt[], 
             unsigned int tofpatt1[], 
             unsigned int antipatt, unsigned int ecalflag, float ecaltrsum);
ClassDef(LVL1Root02,1)       //LVL1Root02
#else
friend class Trigger2LVL1;
friend class AMSNtuple;
#endif
};


class TrRawClusterNtuple {
public:
  int Ntrraw;
  int address[MAXTRRAW];
  int nelem[MAXTRRAW];
  float s2n[MAXTRRAW];
friend class AMSTrRawCluster;
friend class AMSNtuple;
};

#ifdef __WRITEROOT__
class TrRawClusterRoot : public TObject {
#else
class TrRawClusterRoot {
#endif
public:
  int address;
  int nelem;
  float s2n;
#ifdef __WRITEROOT__
  TrRawClusterRoot();
  ~TrRawClusterRoot(){};
  TrRawClusterRoot(int _address, int _nelem, float _s2n);
ClassDef(TrRawClusterRoot,1)       //TrRawClusterRoot
#else
friend class AMSTrRawCluster;
friend class AMSNtuple;
#endif
};

class AntiRawClusterNtuple {
public:
  int Nantiraw;
  int Status[MAXANTIRAW];
  int Sector[MAXANTIRAW];
  int UpDown[MAXANTIRAW];
  float Signal[MAXANTIRAW];

friend class AMSAntiRawCluster;
friend class AMSNtuple;
};

#ifdef __WRITEROOT__
class AntiRawClusterRoot : public TObject {
#else
class AntiRawClusterRoot {
#endif
public:
  int   Status;
  int   Sector;
  int   UpDown;
  float Signal;

#ifdef __WRITEROOT__
  AntiRawClusterRoot();
  ~AntiRawClusterRoot(){};
  AntiRawClusterRoot(int status, int sector, int updown, float signal);
ClassDef(AntiRawClusterRoot ,1)       //AntiRawClusterRoot
#else
friend class AMSAntiRawCluster;
friend class AMSNtuple;
#endif
};


class TOFRawClusterNtuple {
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

friend class TOF2RawCluster;
friend class AMSNtuple;
};

#ifdef __WRITEROOT__
class TOFRawClusterRoot : public TObject {
#else
class TOFRawClusterRoot {
#endif
public:
  int   Status;
  int   Layer;
  int   Bar;
  float tovta[2];
  float tovtd[2];
  float sdtm[2];
  float edepa;
  float edepd;
  float time;
  float cool;

#ifdef __WRITEROOT__
  TOFRawClusterRoot();
  ~TOFRawClusterRoot(){};
  TOFRawClusterRoot(int status, int layer, int bar, float _tovta[],
                    float _tovtd[], float _sdtm[], float _edepa,
                    float _edepd, float _time, float _cool);
ClassDef(TOFRawClusterRoot ,1)       //TOFRawClusterRoot 
#else
friend class TOF2RawCluster;
friend class AMSNtuple;
#endif
};


class RICMCNtuple {
public:
  int NMC;                     // Number of detected photons
  int id[MAXRICMC];            // Particle id.
  float origin[MAXRICMC][3];   // Particle origin
  float direction[MAXRICMC][3];// Original direction
  int status[MAXRICMC];        // Status=10*number of reflections+(have it rayleigh?1:0)
  int numgen;                  // Number of generated photons
  int eventpointer[MAXRICMC];  // Pointer to detected hit
friend class AMSRichMCHit;
friend class AMSNtuple;
};

#ifdef __WRITEROOT__
class RICMCRoot : public TObject {
#else
class RICMCRoot {
#endif
public:
  int   id;            // Particle id.
  float origin[3];     // Particle origin
  float direction[3];  // Original direction
  int   status;        // Status=10*number of reflections+(have it rayleigh?1:0)
  int   numgen;        // Number of generated photons
  int   eventpointer;  // Pointer to detected hit
#ifdef __WRITEROOT__
  RICMCRoot();
  ~RICMCRoot(){};
  RICMCRoot(int _id, float _origin[], float _direction[], int _status, int _numgen,
            int _eventpointer);
ClassDef(RICMCRoot,1)       // RICMCRoot
#else
friend class AMSRichMCHit;
friend class AMSNtuple;
#endif
};


class RICEventNtuple {
public:
  int Nhits;
  int channel[MAXRICHITS];
  int adc[MAXRICHITS];
  float x[MAXRICHITS];
  float y[MAXRICHITS];

friend class AMSRichRawEvent;
friend class AMSNtuple;
};

#ifdef __WRITEROOT__
class RICEventRoot : public TObject {
#else
class RICEventRoot {
#endif
public:
  int   channel;
  int   adc;
  float x;
  float y;

#ifdef __WRITEROOT__
  RICEventRoot();
  ~RICEventRoot(){};
  RICEventRoot(int _channel, int _adc, float _x, float _y);
ClassDef(RICEventRoot,1)       // RICEventRoot
#else
friend class AMSRichRawEvent;
friend class AMSNtuple;
#endif
};


class RICRing {
public:
  int NRings;
  int track[MAXRICHRIN];
  int used[MAXRICHRIN];
  int mused[MAXRICHRIN];
  float beta[MAXRICHRIN];
  float errorbeta[MAXRICHRIN];
  float quality[MAXRICHRIN];
  int Z[MAXRICHRIN];


friend class AMSRichRing;
friend class AMSNtuple;
}; 

#ifdef __WRITEROOT__
class RICRingRoot : public TObject {
#else
class RICRingRoot {
#endif
public:
  int   track;
  int   used;
  int   mused;
  float beta;
  float errorbeta;
  float quality;
  int   Z;


#ifdef __WRITEROOT__
  RICRingRoot();
  ~RICRingRoot(){};
  RICRingRoot(int _track, int _used, int _mused, 
              float _beta, float _errbeta, float _quality, float _z);
ClassDef(RICRingRoot,1)           // RICRingRoot
#else
friend class AMSRichRing;
friend class AMSNtuple;
#endif
}; 


#endif
