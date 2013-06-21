#include "ACQtProducer-include.C"

#include "Settings.h"

bool ACsoft::ACQtProducer::ProduceRunHeader(int entries) {

  assert(fAMSEvent);

  delete fRunHeader;
  fRunHeader = new AC::RunHeader;

  fRunHeader->fRun = fAMSEvent->Run();
  fRunHeader->fTotalAMSEvents = (int)entries;
  fRunHeader->fRunTag = fAMSEvent->fHeader.RunType & 0xFFFF;
  fRunHeader->fTRDHighVoltage = 0; // not yet available
  fRunHeader->fAMSPatchVersion = fAMSEvent->fHeader.Version; 
  fRunHeader->fACRootVersion = ACROOTVERSION;
  fRunHeader->fProducerJobTimeStamp = AC::ConstructTimeStamp(time(NULL), 0);

  // All other fields of the run header are directly set by the ac_producer.
  return true;
}

