//  $Id: AMSNtuple.h,v 1.5 2003/06/17 07:39:54 choutko Exp $
#ifndef __AMSNtuple__
#define __AMSNtuple__
#include <TChain.h>
#include <TNamed.h>
#include <TH2.h>
#include <TH1.h>
#include <TFile.h>
#include <TPad.h>
#include <TSystem.h>
#include "AMSTrackerHist.h"
#include "AMSTOFHist.h"
#include "AMSAntiHist.h"
#include "AMSAxAMSHist.h"
#include "AMSLVL1Hist.h"
#include "AMSLVL3Hist.h"
#include "AMSGenHist.h"
#include <time.h>
#include "../include/root.h"
#include "AMSNtupleHelper.h"
extern void* gAMSUserFunction;
class AMSNtupleR : public AMSEventR {
protected:
int fCurrentTree;
int fEntries;
//TProcessEventTimer * fTimer;  
//bool fTimerDone;
public:
AMSNtupleR(TChain * chain);
Int_t EventNo()const {return Event();}
Int_t GetRun()const {return Run();}
char* GetTime()const {return ctime((time_t*)&fHeader.Time[0]);}
int ReadOneEvent(int event);
int & Entries() {return fEntries;} 
friend class AMSTOFHist;
friend class AMSAntiHist;
friend class AMSTrackerHist;
friend class AMSLVL1Hist;
friend class AMSLVL3Hist;
friend class AMSAxAMSHist;
friend class AMSGenHist;
friend class AMSNtupleHelper;
ClassDef(AMSNtupleR,1)           // Ntuple Browser
};
#endif
