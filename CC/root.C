//  $Id: root.C,v 1.20 2002/05/22 09:01:38 alexei Exp $
#include <root.h>
#include <ntuple.h>
#ifdef __WRITEROOT__

ClassImp(EventNtuple02)

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

EventNtuple02::EventNtuple02(){}

BetaRoot02::BetaRoot02(){}

BetaRoot02::BetaRoot02(int status, int pattern, float beta, float betaC,
                           float ErrBeta, float ErrBetaC, float chi2, float chi2S,
                           int ntof, int ptof[], int ptr)
{
  Status   = status;
  Pattern  = pattern;
  Beta     = beta;
  BetaC    = betaC;
  Error    = ErrBeta;
  ErrorC   = ErrBetaC;
  Chi2     = chi2;
  Chi2S    = chi2S;
  NTOF     = ntof;
  for (int i=0; i<4; i++) pTOF[i] = ptof[i];
  pTr      = ptr;
}
ChargeRoot02::ChargeRoot02() {};
ChargeRoot02::ChargeRoot02(int status, int betap, int chargetof, int chargetr, 
              float probtof[],int chintof[], float probtr[], int chintr[],
              float proballtr, float truntof, float truntofd, float truntr)
{
  Status = status;
  BetaP  = betap;
  ChargeTOF = chargetof;
  ChargeTracker = chargetr;
  for (int i=0; i<4; i++) {
    ProbTOF[i] = probtof[i];
    ChInTOF[i] = chintof[i];
    ProbTracker[i] = probtr[i];
    ChInTracker[i] = chintr[i];
  }
  ProbAllTracker=proballtr;
  TrunTOF = truntof;
  TrunTOFD= truntofd;
  TrunTracker = truntr;
}
ParticleRoot02::ParticleRoot02(){};
ParticleRoot02::ParticleRoot02(
               int betap, int chargep, int trackp, int trdp, int richp, int ecalp,
               int particle, int particlevice, float prob[], float fitmom,
               float mass, float errmass, float momentum, float errmomentum,
               float beta, float errbeta, float charge, float theta, float phi,
               float thetagl, float phigl, float coo[], float cutoff,
               float tofcoo[4][3], float anticoo[2][3], float ecalcoo[3][3],
               float trcoo[8][3], float trdcoo[], float richcoo[2][3],
               float richpath[], float richlength)
{
  BetaP    = betap;
  ChargeP  = chargep;
  TrackP   = trackp;
  TRDP     = trdp;
  RICHP    = richp;
  EcalP    = ecalp;
  Particle = particle;
  ParticleVice = particlevice;
  for (int i=0; i<2; i++) {Prob[i] = prob[i];}
  FitMom   = fitmom;
  Mass     = mass;
  ErrMass  = errmass;
  Momentum = momentum;
  ErrMomentum = errmomentum;
  Beta     = beta;
  ErrBeta  = errbeta;
  Charge   = charge;
  Theta    = theta;
  Phi      = phi;
  ThetaGl  = thetagl;
  PhiGl    = phigl;
  for (int i=0; i<3; i++) {Coo[i] = coo[i];}
  Cutoff   = cutoff;
  for (int i=0; i<4; i++) {
    for (int j=0; j<3; j++) {
      TOFCoo[i][j] = tofcoo[i][j];
    }
  }
  for (int i=0; i<2; i++) {
    for (int j=0; j<3; j++) {
      AntiCoo[i][j] = anticoo[i][j];
    }
  }
  for (int i=0; i<3; i++) {
    for (int j=0; j<3; j++) {
      EcalCoo[i][j] = ecalcoo[i][j];
    }
  }
  for (int i=0; i<8; i++) {
    for (int j=0; j<3; j++) {
      TrCoo[i][j] = trcoo[i][j];
    }
  }
  for (int i=0; i<3; i++) {TRDCoo[i] = trdcoo[i];}
  for (int i=0; i<2; i++) {
    for (int j=0; j<3; j++) {
      RichCoo[i][j] = richcoo[i][j];
    }
  }
  for (int i=0; i<2; i++) {RichPath[i] = richpath[i];}
  RichLength = richlength;
}
/*
ParticleRoot02::ParticleRoot02(
               int betap, int chargep, int trackp, int trdp, int richp, int ecalp,
               int particle, int particlevice, float prob[], float fitmom,
               float mass, float errmass, float momentum, float errmomentum,
               float beta, float errbeta, float charge, float theta, float phi,
               float thetagl, float phigl, AMSPoint coo, float cutoff,
               float tofcoo[4][3], float anticoo[2][3], float ecalcoo[3][3],
               float trcoo[8][3], float trdcoo[], float richcoo[2][3],
               float richpath[], float richlength)
{
  BetaP    = betap;
  ChargeP  = chargep;
  TrackP   = trackp;
  TRDP     = trdp;
  RICHP    = richp;
  EcalP    = ecalp;
  Particle = particle;
  ParticleVice = particlevice;
  for (int i=0; i<2; i++) {Prob[i] = prob[i];}
  FitMom   = fitmom;
  Mass     = mass;
  ErrMass  = errmass;
  Momentum = momentum;
  ErrMomentum = errmomentum;
  Beta     = beta;
  ErrBeta  = errbeta;
  Charge   = charge;
  Theta    = theta;
  Phi      = phi;
  ThetaGl  = thetagl;
  PhiGl    = phigl;
  for (int i=0; i<3; i++) Coo[i]      = coo[i];
  Cutoff   = cutoff;
  for (int i=0; i<4; i++) {
    for (int j=0; j<3; j++) {
      TOFCoo[i][j] = tofcoo[i][j];
    }
  }
  for (int i=0; i<2; i++) {
    for (int j=0; j<3; j++) {
      AntiCoo[i][j] = anticoo[i][j];
    }
  }
  for (int i=0; i<3; i++) {
    for (int j=0; j<3; j++) {
      EcalCoo[i][j] = ecalcoo[i][j];
    }
  }
  for (int i=0; i<8; i++) {
    for (int j=0; j<3; j++) {
      TrCoo[i][j] = trcoo[i][j];
    }
  }
  for (int i=0; i<3; i++) {TRDCoo[i] = trdcoo[i];}
  for (int i=0; i<2; i++) {
    for (int j=0; j<3; j++) {
      RichCoo[i][j] = richcoo[i][j];
    }
  }
  for (int i=0; i<2; i++) {RichPath[i] = richpath[i];}
  RichLength = richlength;
}
*/
TOFClusterRoot::TOFClusterRoot() {};
TOFClusterRoot::TOFClusterRoot(
                               int status, int layer, int bar, int nmemb, float edep, 
                               float edepd, float time, float errtime, float coo[], 
                               float errcoo[])
{
  Status = status;
  Layer  = layer;
  Bar    = bar;
  Nmemb  = nmemb;
  Edep   = edep;
  Edepd  = edepd;
  Time   = time;
  ErrTime= errtime;
  for (int i=0; i<3; i++) {
    Coo[i] = coo[i];
    ErrorCoo[i] = errcoo[i];
  }
}
EcalShowerRoot::EcalShowerRoot() {};
EcalShowerRoot::EcalShowerRoot(
               int status, float dir[], float emdir[], float entry[],
               float exit[], float cofg[], float ertheta, float chi2dir,
               float firstlayeredep, float energyc, 
               float energy3c, float energy5c, float energy9c,
               float erenergyc, float difosum, float sideleak, float rearleak,
               float deadleak, float orpleak, float orp2denergy,
               float chi2profile, float  parprofile[], float chi2trans,
               float sphericity[], int p2dcl[])
{
  Status      = status;
  for (int i=0; i<3; i++) {
   Dir[i]   = dir[i];
   EMDir[i] = emdir[i];
   Entry[i] = entry[i];
   Exit[i]  = exit[i];
   CofG[i]  = cofg[i];
  }
  ErTheta   = ertheta;
  Chi2Dir   = chi2dir;
  FirstLayerEdep = firstlayeredep;
  EnergyC   = energyc;
  Energy3C[0] = energy3c;
  Energy3C[1] = energy5c;
  Energy3C[2] = energy9c;
  ErEnergyC = erenergyc;
  DifoSum   = difosum;
  SideLeak  = sideleak;
  RearLeak  = rearleak;
  DeadLeak  = deadleak;
  OrpLeak   = orpleak;
  Orp2DEnergy = orp2denergy;
  Chi2Profile = chi2profile;
  for (int i=0; i<4; i++) {ParProfile[i] = parprofile[i];}
  Chi2Trans = chi2trans;
  for (int i=0; i<3; i++) {SphericityEV[i] = sphericity[i];}
  for (int i=0; i<2; i++) {p2dcl[i]      = p2dcl[i];}
}
EcalClusterRoot::EcalClusterRoot() {};
EcalClusterRoot::EcalClusterRoot(
                                 int status, int proj, int plane, int left, int center,
                                 int right,
                                 float edep, float sideleak, float deadleak, float coo[],
                                 int pleft, int nhits)
{
  Status  = status;
  Proj    = proj;
  Plane   = plane;
  Left    = left;
  Center  = center;
  Right   = right;
  Edep    = edep;
  SideLeak = sideleak;
  DeadLeak = deadleak;
  for (int i=0; i<3; i++) {Coo[i] = coo[i];}
  pLeft = pleft;
  NHits = nhits;
}
Ecal2DClusterRoot::Ecal2DClusterRoot(){};
Ecal2DClusterRoot::Ecal2DClusterRoot
                 (int status, int proj, int nmemb, float edep, float coo,
                  float tan, float chi2, int pcl[])
{
  Status = status;
  Proj   = proj;
  Nmemb  = nmemb;
  Edep   = edep;
  Coo    = coo;
  Tan    = tan;
  Chi2   = chi2;
  for (int i=0; i<18; i++) {pCl[i] = pcl[i];}
}

EcalHitRoot::EcalHitRoot(){};
EcalHitRoot::EcalHitRoot(int status, int idsoft, int proj, int plane, int cell,
                         float edep, float coo[])
{
  Status = status;
  Idsoft = idsoft;
  Proj   = proj;
  Plane  = plane;
  Cell   = cell;
  Edep   = edep;
  for (int i=0; i<3; i++) {Coo[i] = coo[i];}
}

TOFMCClusterRoot::TOFMCClusterRoot() {};
TOFMCClusterRoot::TOFMCClusterRoot(int idsoft, float coo[], float tof, float edep)
{
  Idsoft = idsoft;
  for (int i=0; i<3; i++) {Coo[i] = coo[i];}
  TOF = tof;
  Edep= edep;
}
TrClusterRoot::TrClusterRoot(){};
TrClusterRoot::TrClusterRoot(
                             int idsoft, int status, int neleml, int nelemr, 
                             float sum, float sigma, float mean, float rms, 
                             float errmean, float amplitude[])
{
  Idsoft = idsoft;
  Status = status;
  NelemL = neleml;
  NelemR = nelemr;
  Sum    = sum;
  Sigma  = sigma;
  Mean   = mean;
  RMS    = rms;
  ErrorMean = errmean;
  for (int i=0; i<5; i++) {Amplitude[i] = amplitude[i];}
}
TrMCClusterRoot::TrMCClusterRoot(){};
TrMCClusterRoot::TrMCClusterRoot(
                                 int idsoft, int trackno, 
                                 int left[], int center[], int right[],
                                 float ss[2][5], 
                                 float xca[], float xcb[], float xgl[], float sum)
{
  Idsoft  = idsoft;
  TrackNo = trackno;
  for (int i=0; i<2; i++) {
         Left[i]   = left[i];
         Center[i] = center[i];
         Right[i]  = right[i];
  }
  for (int i=0; i<3; i++) {
    Xca[i] = xca[i];
    Xcb[i] = xcb[i];
    Xgl[i] = xgl[i];
  }
  Sum = sum;
  for (int i=0; i<2; i++) {
    for (int j=0; j<5; j++) {
      SS[i][j] = ss[i][j];
    }
  }
}
TRDMCClusterRoot::TRDMCClusterRoot(){};
TRDMCClusterRoot::TRDMCClusterRoot(
                                   int layer, int ladder, int tube, int trackno,
                                   float edep, float ekin, float xgl[], float step)
{
  Layer  = layer;
  Ladder = ladder;
  Tube   = tube;
  TrackNo= trackno;
  Edep   = edep;
  Ekin   = ekin;
  for (int i=0; i<3; i++) {Xgl[i] = xgl[i];}
  Step   = step;
}

TRDRawHitRoot::TRDRawHitRoot(){};
TRDRawHitRoot::TRDRawHitRoot(int layer, int ladder, int tube, float amp)
{
  Layer  = layer;
  Ladder = ladder;
  Tube   = tube;
  Amp    = amp;
}

TRDClusterRoot::TRDClusterRoot(){};
TRDClusterRoot::TRDClusterRoot(int status, float coo[], int layer, float coodir[], 
                               int multip, int hmultip, float edep, int prawhit)
{
  Status = status;
  Layer  = layer;
  for (int i=0; i<3; i++) Coo[i]=coo[i];
  for (int i=0; i<3; i++) CooDir[i]=coodir[i];
  Multip = multip;
  HMultip= hmultip;
  EDep   = edep;
  pRawHit= prawhit;
}

TRDSegmentRoot::TRDSegmentRoot(){};
TRDSegmentRoot::TRDSegmentRoot(
               int status, int orientation, float fitpar[], float chi2,
               int pattern, int nhits, int pcl[])
{
  Status        = status;
  Orientation   = orientation;
  for (int i=0; i<2; i++) {FitPar[i] = fitpar[i];}
  Chi2          = chi2;
  Pattern       = pattern;
  Nhits         = nhits;
  for (int i=0; i<12; i++) {PCl[i] = pcl[i];}
}
TRDTrackRoot::TRDTrackRoot(){};
TRDTrackRoot::TRDTrackRoot(int status, float coo[], float ercoo[], 
                           float phi, float theta, float chi2, int nseg, 
                           int pattern, int pseg[])
{
  Status = status;
  for (int i=0; i<3; i++) {
    Coo[i]   = coo[i];
    ErCoo[i] = ercoo[i];
  }
  Phi   = phi;
  Theta = theta;
  Chi2  = chi2;
  NSeg  = nseg;
  Pattern = pattern;
  for (int i=0; i<5; i++) {pSeg[i] = pseg[i];}
}

TrRecHitRoot02::TrRecHitRoot02(){};
TrRecHitRoot02::TrRecHitRoot02(int px, int py, int status, int layer, float hit[], 
                               float ehit[], float sum, float difosum, 
                               float cofgx, float cofgy)
{
  pX    = px;
  pY    = py;
  Status= status;
  Layer = layer;
  for (int i=0; i<3; i++) Hit[i]=hit[i];
  for (int i=0; i<3; i++) EHit[i] = ehit[i];
  Sum     = sum;
  DifoSum = difosum;
  CofgX   = cofgx;
  CofgY   = cofgy;
}
TrTrackRoot02::TrTrackRoot02(){};
TrTrackRoot02::TrTrackRoot02
               (int status, int pattern, int address, int nhits, int phits[],
                float locdbaver, int geanefitdone, int advancedfitdone,
                float chi2strline, float chi2circle, float circleridgidity,
                float chi2fastfit, float ridgidity, float errridgidity,
                float theta, float phi, float p0[], float gchi2,
                float gridgidity, float gerrridgidity,
                float hchi2[], float hridgidity[], float herrridgidity[],
                float htheta[], float hphi[], float hp0[2][3],
                float fchi2ms, float pierrrig, float ridgidityms, float pirigidity
                )
{
  Status    = status;
  Pattern   = pattern;
  Address   = address;
  NHits     = nhits;
  for (int i=0; i<8; i++)  pHits[i]=phits[i];
  LocDBAver       = locdbaver;;
  GeaneFitDone    = geanefitdone;
  AdvancedFitDone = advancedfitdone;
  Chi2StrLine     = chi2strline;
  Chi2Circle      = chi2circle;
  CircleRidgidity = circleridgidity;
  Chi2FastFit     = chi2fastfit;
  Ridgidity       = ridgidity;
  ErrRidgidity    = errridgidity;
  Theta           = theta;
  Phi             = phi;
  for (int i=0; i<3; i++) P0[i] = p0[i];
  GChi2           = gchi2;
  GRidgidity      = gridgidity;
  GErrRidgidity   = gerrridgidity;
  for (int i=0; i<2; i++) {
        HChi2[i]        = hchi2[i];
        HRidgidity[i]   = hridgidity[i];
        HErrRidgidity[i]= herrridgidity[i];
        HTheta[i]       = htheta[i];
        HPhi[i]         = hphi[i];
        for (int j=0; j<3; j++)  HP0[i][j] = hp0[i][j];
  }
  FChi2MS = fchi2ms;
  PiErrRig= pierrrig;
  RidgidityMS = ridgidityms;
  PiRigidity  = pirigidity;
}
MCTrackRoot::MCTrackRoot(){};
MCTrackRoot::MCTrackRoot(float radl, float absl, float pos[], char vname[])
{
  _radl = radl;
  _absl = absl;
  for (int i=0; i<3; i++) _pos[i] = pos[i];
  for (int i=0; i<3; i++) {
     if (vname[i]) _vname[i] = vname[i];
  }
}
MCEventGRoot02::MCEventGRoot02(){};
MCEventGRoot02::MCEventGRoot02(int nskip, int particle, float coo[], float dir[],
                               float momentum, float mass, float charge)
{
  Nskip = nskip;
  Particle = particle;
  for (int i=0; i<3; i++) Coo[i] = coo[i];
  for (int i=0; i<3; i++) Dir[i] = dir[i];
  Momentum = momentum;
  Mass     = mass;
  Charge   = charge;
}           
AntiClusterRoot::AntiClusterRoot(){};
AntiClusterRoot::AntiClusterRoot(int status, int sector, float edep, 
                             float coo[], float errorcoo[])
{
  Status = status;
  Sector = sector;
  Edep   = edep;
  for (int i=0; i<3; i++) Coo[i] = coo[i];
  for (int i=0; i<3; i++) ErrorCoo[i] = errorcoo[i];
}
ANTIMCClusterRoot::ANTIMCClusterRoot(){};
ANTIMCClusterRoot::ANTIMCClusterRoot(int idsoft, float coo[], float tof, float edep)
{
  Idsoft = idsoft;
  for (int i=0; i<3; i++) Coo[i]=coo[i];
  TOF    = tof;
  Edep   = edep;
}
LVL3Root02::LVL3Root02(){};
LVL3Root02::LVL3Root02(int toftr, uint trdtr, uint trackertr, uint maintr,
                       int direction, int ntrhits, int npatfound, int pattern[],
                       double residual[], number ftime, number eloss, int trdhits,
                       int hmult, float trdpar[], int ecemag, int ecematc, 
                       float ectofc[])
{
  TOFTr     = toftr;
  TRDTr     = trdtr;
  TrackerTr = trackertr;
  MainTr    = maintr;
  Direction = direction;
  NTrHits   = ntrhits;
  NPatFound = npatfound;
  for (int i=0; i<2; i++) {Pattern[i] = pattern[i];}
  for (int i=0; i<2; i++) {Residual[i]= residual[i];}
  Time      = ftime;
  ELoss     = eloss;
  TRDHits   = trdhits;
  HMult     = hmult;
  for (int i=0; i<2; i++) {TRDPar[i] = trdpar[i];}
  ECemag    = ecemag;
  ECmatc    = ecematc;
  for (int i=0; i<4; i++) {ECTOFcr[i] = ectofc[i];}

}
LVL1Root02::LVL1Root02(){};
LVL1Root02::LVL1Root02(uinteger mode, int toflag, uinteger tofpatt[], uinteger tofpatt1[],
                       uinteger antipatt, uinteger ecalflag, geant ecaltrsum)
{
  Mode   = mode;
  TOFlag = toflag;
  for (int i=0; i<4; i++) {
    TOFPatt[i]  = tofpatt[i];
    TOFPatt1[i] = tofpatt1[i];
  }
  AntiPatt = antipatt;
  ECALflag = ecalflag;
  ECALtrsum= ecaltrsum;

}
TrRawClusterRoot::TrRawClusterRoot(){};
TrRawClusterRoot::TrRawClusterRoot(int _address, int _nelem, float _s2n)
{
  address = _address;
  nelem   = _nelem;
  s2n     = _s2n;
}
AntiRawClusterRoot::AntiRawClusterRoot(){};
AntiRawClusterRoot::AntiRawClusterRoot(int status, int sector, int updown, 
                                       float signal)
{
  Status = status;
  Sector = sector;
  UpDown = updown;
  Signal = signal;
}
TOFRawClusterRoot::TOFRawClusterRoot(){};
TOFRawClusterRoot::TOFRawClusterRoot(int status, int layer, int bar, float _tovta[],
                                     float _tovtd[], float _sdtm[], float _edepa,
                                     float _edepd, float _time, float _cool)
{
  Status = status;
  Layer  = layer;
  Bar    = bar;
  for (int i=0; i<2; i++) tovta[i]=_tovta[i];
  for (int i=0; i<2; i++) tovtd[i]=_tovtd[i];
  for (int i=0; i<2; i++) sdtm[i]=_sdtm[i];
  edepa  = _edepa;
  edepd  = _edepd;
  time   = _time;
  cool   = _cool;
}
RICMCRoot::RICMCRoot(){};
RICMCRoot::RICMCRoot(int _id, float _origin[], float _direction[], int _status, 
                     int _numgen, int _eventpointer)
{
  id = _id;
  for (int i=0; i<3; i++) {
    origin[i] = _origin[i];
    direction[i] = _direction[i];
  }
  status = _status;
  numgen = _numgen;
  eventpointer = _eventpointer;
}
RICEventRoot::RICEventRoot(){};
RICEventRoot::RICEventRoot(int _channel, int _adc, float _x, float _y)
{
  channel = _channel;
  adc     = _adc;
  x       = _x;
  y       = _y;
}
RICRingRoot::RICRingRoot(){};
RICRingRoot::RICRingRoot(int _track, int _used, int _mused, 
                         float _beta, float _errbeta, float _quality, float _z)
{
  track = _track;
  used  = _used;
  mused = _mused;
  beta  = _beta;
  errorbeta = _errbeta;
  quality   = _quality;
  Z         = _z;
}
#endif

