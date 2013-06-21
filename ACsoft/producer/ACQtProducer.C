#include "ACQtProducer-include.C"

#include "Settings.h"
#include <TH3.h>

ACsoft::ACQtProducer::ACQtProducer()
  : fRunHeader(0)
  , fEvent(0)
  , fAMSEvent(0)
  , fIsProcessingEventsFromStandardRun(true)
  , fAlgorithm(-1)
  , fPattern(-1)
  , fRefit(-1) {

  timer_ini   = new TStopwatch();
  timer_read  = new TStopwatch();   timer_read->Stop();
  timer_head  = new TStopwatch();   timer_head->Stop();
  timer_txal  = new TStopwatch();   timer_txal->Stop();
  timer_ami   = new TStopwatch();   timer_ami->Stop();
  timer_trig  = new TStopwatch();   timer_trig->Stop();
  timer_daq   = new TStopwatch();   timer_daq->Stop();
  timer_part  = new TStopwatch();   timer_part->Stop();
  timer_btrc  = new TStopwatch();   timer_btrc->Stop();
  timer_galc  = new TStopwatch();   timer_galc->Stop();
  timer_tof   = new TStopwatch();   timer_tof->Stop();
  timer_acc   = new TStopwatch();   timer_acc->Stop();
  timer_trk   = new TStopwatch();   timer_trk->Stop();
  timer_trkl  = new TStopwatch();   timer_trkl->Stop();
  timer_trdk  = new TStopwatch();   timer_trdk->Stop();
  timer_trd   = new TStopwatch();   timer_trd->Stop();
  timer_ecal  = new TStopwatch();   timer_ecal->Stop();
  timer_ebdt4 = new TStopwatch();   timer_ebdt4->Stop();
  timer_ebdt5 = new TStopwatch();   timer_ebdt5->Stop();
  timer_ese2  = new TStopwatch();   timer_ese2->Stop();
  timer_ese3  = new TStopwatch();   timer_ese3->Stop();
  timer_echi2  = new TStopwatch();  timer_echi2->Stop();
  timer_rich  = new TStopwatch();   timer_rich->Stop();
  timer_chrg  = new TStopwatch();   timer_chrg->Stop();
  timer_mc    = new TStopwatch();   timer_mc->Stop();
  timer_fill  = new TStopwatch();   timer_fill->Stop();
  timer_end   = new TStopwatch();   timer_end->Stop();
  nBTRC=0;
  nGALC=0;
  nECAL12=0;
  nECAL3=0;

  ::AC::CurrentACQtVersion() = ACROOTVERSION;
  TrkLH::gethead(); // Initialize tracker likelihood.

  // initialise the RICH calibration
  RichRingR::setBetaCorrection(RichRingR::fullUniformityCorrection);
  RichRingR::loadChargeUniformityCorrection = true;
  RichRingR::useTemperatureCorrections = true;

  // initialize Hainos latest tracker alignment.
  TkDBc::UseLatest();

 // Control Histograms
#ifndef DISABLE_ACQT_CONTROL_PLOTS
  // ACQtProducer-TRD 
  h1_nTrdRawHit_All    = new TH1F("h1_nTrdRawHit_All",   "h1_nTrdRawHit_All",   4000, 0, 4000); DB_Histograms.Add(h1_nTrdRawHit_All);
  h1_nTrdRawHit_Used   = new TH1F("h1_nTrdRawHit_Used",  "h1_nTrdRawHit_Used",  4000, 0, 4000); DB_Histograms.Add(h1_nTrdRawHit_Used);
  h1_nTrdRawHit_Extra  = new TH1F("h1_nTrdRawHit_Extra", "h1_nTrdRawHit_Extra", 4000, 0, 4000); DB_Histograms.Add(h1_nTrdRawHit_Extra);
  h1_nTrdRawHit_UsPlEx = new TH1F("h1_nTrdRawHit_UsPlEx","h1_nTrdRawHit_UsPlEx",4000, 0, 4000); DB_Histograms.Add(h1_nTrdRawHit_UsPlEx);
  h1_nTrdRawHit_Saved  = new TH1F("h1_nTrdRawHit_Saved", "h1_nTrdRawHit_Saved", 4000, 0, 4000); DB_Histograms.Add(h1_nTrdRawHit_Saved);

  // ACQtProducer-Tracker
  for(int i=0; i<9;i++){
    TString hnam = "h1_TrRecHitResidJlay";
    hnam = hnam + (Long_t)(i+1);
    h1_TrRecHitResidJLay[i] = new TH1F(hnam,hnam, 5000, -10000, 10000);                         DB_Histograms.Add(h1_TrRecHitResidJLay[i]);
  }

  h2_TrLH_vs_EovR = new TH2F("h2_TrLH_vs_EovR", "h2_TrLH_vs_EovR",200,-2.0,2.0, 200,0.0,1.0);   DB_Histograms.Add(h2_TrLH_vs_EovR);

  h2_relerrR_vs_R_choutko = new TH2F("h2_relerrR_vs_R_choutko","h2_relerrR_vs_R_choutko", 200, 0.0, 100.0, 200, 0.0, 1.0); DB_Histograms.Add(h2_relerrR_vs_R_choutko);
  h2_relerrR_vs_R_chikani = new TH2F("h2_relerrR_vs_R_chikani","h2_relerrR_vs_R_chikani", 200, 0.0, 100.0, 200, 0.0, 1.0); DB_Histograms.Add(h2_relerrR_vs_R_chikani);
  h1_LRep = new TH1F("h1_LRep","h1_LRep",500, 0.0, 5.0);                                                                   DB_Histograms.Add(h1_LRep); 
#endif
  
}

ACsoft::ACQtProducer::~ACQtProducer() {

  fAMSEvent = 0;

  delete fEvent;
  fEvent = 0;

  delete fRunHeader;
  fRunHeader = 0;
}

bool ACsoft::ACQtProducer::IsMCDataRun() const {

  assert(fAMSEvent);
  return fAMSEvent->nMCEventg() > 0;
}

bool ACsoft::ACQtProducer::IsISSDataRun() const {

  assert(fAMSEvent);
  return !IsMCDataRun() && fAMSEvent->Run() > 1300000000;
}

bool ACsoft::ACQtProducer::ProduceFullEvent(AMSEventR* amsEvent, bool produceRunHeader, int entriesInRun) {

  PrepareProductionForEvent(amsEvent);
  if (produceRunHeader)
    ProduceRunHeader(entriesInRun);

  bool skip = false;
  for (unsigned int i = ProduceNothing + 1; i < ProduceLast; ++i) {
    if (!ProducePart(static_cast<ProductionSelection>(i))) {
      skip = true;
      break;
    }
  }

  FinishProductionForEvent(amsEvent);
  return !skip;
}

void ACsoft::ACQtProducer::PrepareProductionForEvent(AMSEventR* amsEvent) {

  assert(amsEvent);
  if (fAMSEvent) {
    WARN_OUT << "PrepareProductionForEvent() called before calling FinishProductionForEvent()." << std::endl;
    return;
  }

  if (amsEvent && amsEvent->Version() < 620) TofRecH::ReBuild(); // needed for <B620 - kills pBetaH->GetBetaC()

  fAMSEvent = amsEvent;

  if (fEvent)
    fEvent->Clear();
  else
    fEvent = new AC::Event;
  
  fAlgorithm = -1;
  fPattern = -1;
  fRefit = -1;
}

void ACsoft::ACQtProducer::FinishProductionForEvent(AMSEventR* amsEvent) {

  assert(amsEvent);
  assert(fAMSEvent);
  assert(fAMSEvent == amsEvent);

  if (fRunHeader)
    fEvent->SetRunHeader(fRunHeader);

  if (fAMSEvent != amsEvent) {
    WARN_OUT << "PrepareProductionForEvent() called with another event, than FinishProductionForEvent()." << std::endl;
    return;
  }

  fAMSEvent = 0; 
  fAlgorithm = -1;
  fPattern = -1;
  fRefit = -1;
  fTRDRawHitIndices.clear();
  fTRDExtraHits.clear();
}

bool ACsoft::ACQtProducer::ProducePart(ProductionSelection selection) {

  switch (selection) {
  case ProduceACCData:
    return ProduceACC();
  case ProduceEventHeaderData:
    return ProduceEventHeader();
  case ProduceECALData:
    return ProduceECAL();
 case ProduceParticlesData:
    return ProduceParticles();
 case ProduceRICHData:
    return ProduceRICH();
  case ProduceTrackerData:
    return ProduceTracker();
  case ProduceTriggerData:
    return ProduceTrigger();
  case ProduceDAQData:
    return ProduceDAQ();
  case ProduceTOFData:
    return ProduceTOF();
  case ProduceTRDData:
    return ProduceTRD();
  case ProduceMCData:
    return ProduceMC();
  case ProduceNothing:
  case ProduceLast:
     WARN_OUT << "You can not use ProduceNothing/ProduceLast with the ProducePart() method." << std::endl;
     return false;
   }

  assert(false);
  return false;
}
