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
  };
  class TOF{
   public:
  };   
  class ANTI{
   public:
  };   
  class CTC{
   public:
  };   
  class Tracker{
   public:
  };   
  class LVL1{
   public:
    Int_t TOF[4];
    Int_t TOFZ[4];
    Int_t Anti;
  };   
  class LVL3{
   public:
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
Int_t GetRun()const {return _Event.Run;}
char* GetTime()const {return ctime(&_Event.Time);}
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
