#ifndef __AMSNTUPLE__
#define __AMSNTUPLE__

class AMSNtuple{
private:
int _Event;
public:
int * getaddress() {return &_Event;}
integer & Event(){ return _Event;}
};


class BetaNtuple: public AMSNtuple{
private:
int Pattern;
float Beta;
float Error;
float Chi2;
float Chi2S;
int NTOF;
int pTOF[4];
int pTr;
friend class AMSBeta;

};

class ChargeNtuple: public AMSNtuple{
private:
int BetaP;
int ChargeTOF;
int ChargeTracker;
float ProbTOF[7];
float ProbTracker[7];

friend class AMSCharge;

};


class CTCClusterNtuple: public AMSNtuple{
private:
int Status;
int Layer;
float Coo[3];
float ErrCoo[3];
float RawSignal;
float Signal;
float ErrorSignal;

friend class AMSCTCCluster;

};

class CTCHitNtuple: public AMSNtuple{
private:
int Status;
int Layer;
int Column;
int Row;
float Signal;

friend class AMSCTCRawHit;

};

class EventNtuple: public AMSNtuple{
private:
int Run;
int RunType;
int Time[2];
float PolePhi;
float ThetaS;
float PhiS;
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

};

class TrMCClusterNtuple: public AMSNtuple{
private:
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

friend class AMSTrMCCluster;

};


class TOFMCClusterNtuple: public AMSNtuple{
private:
int Idsoft;
float Coo[3];
float TOF;
float Edep;

friend class AMSTOFMCCluster;
friend class AMSAntiMCCluster;

};

class CTCMCClusterNtuple: public AMSNtuple{
private:
int Idsoft;
float Coo[3];
float Dir[3];
float Step;
float Charge;
float Beta;
float Edep;

friend class AMSCTCMCCluster;

};


class MCEventGNtuple: public AMSNtuple{
private:
int Nskip;
int Particle;
float Coo[3];
float Dir[3];
float Momentum;
float Mass;
float Charge;
friend class AMSmceventg;
};


class ParticleNtuple: public AMSNtuple{
private:
int CTCP[2];
int BetaP;
int ChargeP;
int TrackP;
int Particle;
float Mass;
float ErrMass;
float Momentum;
float ErrMomentum;
float Charge;
float Theta;
float Phi;
float Coo[3];
float Value[3][2];
float CooCTC[2][3];

friend class AMSParticle;


};



class TOFClusterNtuple: public AMSNtuple{
private:
int Status;
int Ntof;
int Plane;
float Edep;
float Time;
float ErrTime;
float Coo[3];
float ErrorCoo[3];

friend class AMSTOFCluster;

};

class AntiClusterNtuple: public AMSNtuple{
private:
int Status;
int Sector;
float Edep;
float Coo[3];   // R, phi, Z
float ErrorCoo[3];

friend class AMSAntiCluster;

};

class AntiRawClusterNtuple: public AMSNtuple{
private:
int Status;
int Sector;
int UpDown;
float Signal;

friend class AMSAntiRawCluster;

};

class TrClusterNtuple: public AMSNtuple{
private:
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
};

class TrRawClusterNtuple: public AMSNtuple{
private:
int address;
int nelem;
friend class AMSTrRawCluster;
};

class TrRecHitNtuple: public AMSNtuple{
private:
int pX;
int pY;
int Status;
int Layer;
float Hit[3];
float EHit[3];
float Sum;
float DifoSum;

friend class AMSTrRecHit;

};

class TrTrackNtuple: public AMSNtuple{
private:
int Status;
int Pattern;
int NHits;
int pHits[6];
int FastFitDone;
int GeaneFitDone;
int AdvancedFitDone;
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
float GTheta;
float GPhi;
float GP0[3];
float HChi2[2];
float HRidgidity[2];
float HErrRidgidity[2];
float HTheta[2];
float HPhi[2];
float HP0_1[3];
float HP0_2[3];
float FChi2MS;
float GChi2MS;
float RidgidityMS;
float GRidgidityMS;

friend class AMSTrTrack;

};

class LVL3Ntuple: public AMSNtuple{
private:
int TOFTr;
int AntiTr;
int TrackerTr;
int NTrHits;
int NPatFound;
int Pattern[2];
float Residual[2];
float Time;
float ELoss;
friend class TriggerLVL3;
};



class LVL1Ntuple: public AMSNtuple{
private:
int Mode;
int TOFlag;
int TOFPatt[4];
int TOFPatt1[4];
int AntiPatt;
friend class TriggerLVL1;
};


#endif
