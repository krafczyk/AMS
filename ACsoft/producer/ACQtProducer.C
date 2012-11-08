#include "ACQtProducer-include.C"

ACQtProducer::ACQtProducer()
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
  , fMaxECALShowers(-1) {

}

ACQtProducer::~ACQtProducer() {

  fAMSEvent = 0;

  delete fEvent;
  fEvent = 0;

  delete fRunHeader;
  fRunHeader = 0;
}

bool ACQtProducer::ProduceFullEvent(AMSEventR* amsEvent, bool produceRunHeader, int entriesInRun) {

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

void ACQtProducer::PrepareProductionForEvent(AMSEventR* amsEvent) {

  if (fAMSEvent) {
    WARN_OUT << "PrepareProductionForEvent() called before calling FinishProductionForEvent()." << std::endl;
    return;
  }

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

void ACQtProducer::FinishProductionForEvent(AMSEventR* amsEvent) {

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

bool ACQtProducer::ProducePart(ProductionSelection selection) {

  switch (selection) {
  case ProduceACCData:
    return ProduceACC();
  case ProduceChargesData:
    return ProduceCharges();
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
