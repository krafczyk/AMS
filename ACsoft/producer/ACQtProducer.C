#include "ACQtProducer-include.C"

#include "TH3F.h"

ACsoft::ACQtProducer::ACQtProducer()
  : fRunHeader(0)
  , fEvent(0)
  , fAMSEvent(0)
  , fUseTrackerExternalAlignment(true)
  , fIsProcessingEventsFromStandardRun(true)
  , fAlgorithm(-1)
  , fPattern(-1)
  , fRefit(-1)
  , fMaxTrackerTracks(-1)
  , fMaxRICHRings(-1)
  , fMaxECALShowers(-1)  {

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
  timer_trdk  = new TStopwatch();   timer_trdk->Stop();
  timer_trd   = new TStopwatch();   timer_trd->Stop();
  timer_ecal  = new TStopwatch();   timer_ecal->Stop();
  timer_ebdt  = new TStopwatch();   timer_ebdt->Stop();
  timer_ese2  = new TStopwatch();   timer_ese2->Stop();
  timer_rich  = new TStopwatch();   timer_rich->Stop();
  timer_chrg  = new TStopwatch();   timer_chrg->Stop();
  timer_mc    = new TStopwatch();   timer_mc->Stop();
  timer_fill  = new TStopwatch();   timer_fill->Stop();
  timer_end   = new TStopwatch();   timer_end->Stop();
  nBTRC=0;
  nGALC=0;
  nECAL12=0;
  nECAL3=0;
 
  // variables for ACsoft::ACQtProducer::GetCustomGeoCutoff
  CutOffNegPos[0] = 0.0;
  CutOffNegPos[1] = 0.0;

  // variables for ACsoft::ACQtProducer::DoCustomBacktracing
  BT_status = 0;

  // variables for StoermerMaxCutOff in AMS acceptance cone
  MaxCutOffCone[0]   = 0.0;
  MaxCutOffCone[1]   = 0.0;
  UTimeCutOffConeMax = 0;

 
 // Control Histograms

#ifndef AMS_ACQT_INTERFACE
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
#endif
}

ACsoft::ACQtProducer::~ACQtProducer() {

  fAMSEvent = 0;

  delete fEvent;
  fEvent = 0;

  delete fRunHeader;
  fRunHeader = 0;
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

  if (fAMSEvent) {
    WARN_OUT << "PrepareProductionForEvent() called before calling FinishProductionForEvent()." << std::endl;
    return;
  }

  if (amsEvent->Version() < 620) TofRecH::ReBuild(); // needed for <B620 - kills pBetaH->GetBetaC()

  fAMSEvent = amsEvent;

  if (fEvent)
    fEvent->Clear();
  else
    fEvent = new AC::Event;
  
  fAlgorithm = -1;
  fPattern = -1;
  fRefit = -1;
  fMaxTrackerTracks = -1;
  fMaxRICHRings = -1;
  fMaxECALShowers = -1;
}

void ACsoft::ACQtProducer::FinishProductionForEvent(AMSEventR* amsEvent) {

  Q_ASSERT(amsEvent);
  Q_ASSERT(fAMSEvent);
  Q_ASSERT(fAMSEvent == amsEvent);

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
  fMaxTrackerTracks = -1;
  fMaxRICHRings = -1;
  fMaxECALShowers = -1;
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

  Q_ASSERT(false);
  return false;
}
