#ifndef __AMSNtuple__
#define __AMSNtuple__
#include <TTree.h>
#include <TNamed.h>
#include <TH2.h>
#include <TH1.h>
#include <TFile.h>
#include <TPad.h>
#include "AMSTrackerHist.h"
#include "AMSTOFHist.h"
#include "AMSAntiHist.h"
#include "AMSCTCHist.h"
#include "AMSAxAMSHist.h"
#include "AMSLVL1Hist.h"
#include "AMSLVL3Hist.h"
#include <time.h>
const Int_t maxpart=20;
const Int_t maxantiraw=32;
const Int_t maxanti=16;
const Int_t maxtof=200;
const Int_t maxtrraw=500;
const Int_t maxctc=50;
const Int_t MaxSiHits = 500;
const Int_t maxtrcl = 200;
class AMSNtuple : public TNamed {
protected:
  class Event{
  public:
   Int_t Run;
   Int_t EventNo;
   Int_t RunType;
   time_t Time;
  };
  class AxAMS{
   public:
   Int_t           npart;
   Int_t           pctcp[maxpart][2];
   Int_t           pbetap[maxpart];
   Int_t           pchargep[maxpart];
   Int_t           ptrackp[maxpart];
   Int_t           pid[maxpart];
   Float_t         pmass[maxpart];
   Float_t         perrmass[maxpart];
   Float_t         pmom[maxpart];
   Float_t         perrmom[maxpart];
   Float_t         pcharge[maxpart];
   Float_t         ptheta[maxpart];
   Float_t         pphi[maxpart];
   Float_t         pcoo[maxpart][3];
   Float_t         signalctc[maxpart][2];
   Float_t         betactc[maxpart][2];
   Float_t         errorbetactc[maxpart][2];
   Float_t         cooctc[maxpart][2][3];
   Float_t         cootof[maxpart][4][3];
   Int_t           nbeta;
   Float_t         beta[maxpart];
  };
  class TOF{
   public:
  Int_t           NToF;
  Int_t           Status[maxtof];
  Int_t           Plane[maxtof];
  Int_t           Bar[maxtof];
  Float_t         Edep[maxtof];
  Float_t         Time[maxtof];
  Float_t         ErTime[maxtof];
  Float_t         Coo[maxtof][3];
  Float_t         ErCoo[maxtof][3];

  };   
  class ANTI{
   public:
   Int_t           nanti;
   Int_t           AntiStatus[maxanti];
   Int_t           AntiSector[maxanti];
   Float_t         AntiCoo[maxanti][3];
   Float_t         AntiErCoo[maxanti][3];
   Float_t         AntiSignal[maxanti];
   Int_t           nantiraw;
   Int_t           AntirawStatus[maxantiraw];
   Int_t           AntirawSector[maxantiraw];
   Int_t         Antirawupdown[maxantiraw];
   Float_t         AntirawSignal[maxantiraw];

  };   
  class CTC{
   public:
   Int_t        nhit;
   Int_t        layer[maxctc];
   Int_t        x[maxctc];
   Int_t        y[maxctc];
   Float_t      signal[maxctc];
  };   
  class Tracker{
   public:
        //Raw
        Int_t ntrraw;
        Int_t rawaddress[maxtrraw];
        Int_t rawlength[maxtrraw];
        //RecHits

   Int_t           ntrrh;
   Int_t           px[MaxSiHits];
   Int_t           py[MaxSiHits];
   Int_t           statusr[MaxSiHits];
   Int_t           Layer[MaxSiHits];
   Float_t         hitr[MaxSiHits][3];
   Float_t         ehitr[MaxSiHits][3];
   Float_t         sumr[MaxSiHits];
   Float_t         difosum[MaxSiHits];
       //Clusters
   Int_t          ntrcl;
   Int_t          idsoft[maxtrcl];
   Int_t          statust[maxtrcl];  
   Int_t          neleml[maxtrcl];        
   Int_t          nelemr[maxtrcl];        
   Float_t          sumt[maxtrcl];        
   Float_t          sigmat[maxtrcl];        
   Float_t          meant[maxtrcl];        
   Float_t          rmst[maxtrcl];        
  };   
  class LVL1{
   public:
    Int_t TOF[4];
    Int_t TOFZ[4];
    Int_t Anti;
  };   
  class LVL3{
   public:
    Int_t nlvl3;
    Int_t TOFTr;
    Int_t AntiTr;
    Int_t TrackerTr;
    Int_t NtrHits;
    Int_t NPat;
    Int_t Pattern[2];
    Float_t Residual[2]; 
    Float_t ELoss; 
  };   
 TTree * _h1;
 AxAMS _AxAMS; 
 TOF _TOF;
 LVL1  _LVL1; 
 LVL3  _LVL3;
 CTC  _CTC;
 ANTI  _ANTI;
 Tracker  _Tracker;
 Event _Event;    
public:
AMSNtuple(TFile * file);
Int_t EventNo()const {return _Event.EventNo;}
Int_t GetRun()const {return _Event.Time;}
//char* GetTime()const {return ctime(&_Event.Time);}
char* GetTime()const {return ctime(&_Event.Run);}
Int_t ReadOneEvent(int event){return _h1?_h1->GetEvent(event):0;}
friend class AMSTOFHist;
friend class AMSAntiHist;
friend class AMSCTCHist;
friend class AMSTrackerHist;
friend class AMSLVL1Hist;
friend class AMSLVL3Hist;
friend class AMSAxAMSHist;
ClassDef(AMSNtuple,1)           // Ntuple Browser
};
#endif
