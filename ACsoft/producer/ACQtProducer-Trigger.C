#include "ACQtProducer-include.C"

bool ACsoft::ACQtProducer::ProduceTrigger() {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);

  // Don't raise the broken event counter if no level1 trigger information are available, to keep processing the file.
  // Happens on some of the earlier runs, where we exit after 100 broken events. Instead keep processing the file
  // and hope it will recover again, if not, we'll throw the problematic events away anyway with our trigger-info-available
  // preselection cut.
  if (fAMSEvent->nLevel1() != 1) {
    fEvent->fTrigger.Clear();
    return true;
  }

  Level1R* pLevel1 = fAMSEvent->pLevel1(0);
  if (!pLevel1) {
    fEvent->fTrigger.Clear();
    return true;
  }

  fEvent->fTrigger.fBits = AC::Trigger::GenerateBitsFromFlags(pLevel1->PhysBPatt, pLevel1->TofFlag1, pLevel1->TofFlag2, pLevel1->AntiPatt, pLevel1->EcalFlag);

  if (fEvent->fTrigger.PhysBFlags() != pLevel1->PhysBPatt)
    WARN_OUT << "Problem building AC::Trigger: PhysBFlags() don't match, got: "
             << fEvent->fTrigger.PhysBFlags() << " expected: " << pLevel1->PhysBPatt << std::endl;

  if (fEvent->fTrigger.TOFFlags1() != (pLevel1->TofFlag1 & 15))
    WARN_OUT << "Problem building AC::Trigger: TOFFlags1() don't match, got: "
             << fEvent->fTrigger.TOFFlags1() << " expected: " << (pLevel1->TofFlag1 & 15) << std::endl;

  if (fEvent->fTrigger.TOFFlags2() != (pLevel1->TofFlag2 & 15))
    WARN_OUT << "Problem building AC::Trigger: TOFFlags2() don't match, got: "
             << fEvent->fTrigger.TOFFlags2() << " expected: " << (pLevel1->TofFlag2 & 15) << std::endl;

  if (fEvent->fTrigger.ACCFlags() != pLevel1->AntiPatt)
    WARN_OUT << "Problem building AC::Trigger: ACCFlags() don't match, got: "
             << fEvent->fTrigger.ACCFlags() << " expected: " << pLevel1->AntiPatt << std::endl;

  if (fEvent->fTrigger.ECALFlags() != pLevel1->EcalFlag)
    WARN_OUT << "Problem building AC::Trigger: ECALFlags() don't match, got: "
             << fEvent->fTrigger.ECALFlags() << " expected: " <<  pLevel1->EcalFlag << std::endl;

  fEvent->fTrigger.fTimeDifference = min((unsigned int)0xffff, pLevel1->TrigTime[4]);
  fEvent->fTrigger.fJMembPatt = pLevel1->JMembPatt & 0xFFFF;
  fEvent->fTrigger.fLiveTime = pLevel1->LiveTime;
  fEvent->fTrigger.fTriggerRateFT = min(65535,(int)round(pLevel1->TrigRates[0]));
  fEvent->fTrigger.fTriggerRateFTC = min(65535,(int)round(pLevel1->TrigRates[1]));
  fEvent->fTrigger.fTriggerRateFTZ = min(65535,(int)round(pLevel1->TrigRates[2]));
  fEvent->fTrigger.fTriggerRateFTE = min(65535,(int)round(pLevel1->TrigRates[3]));
  fEvent->fTrigger.fTriggerRateLV1 = min(65535,(int)round(pLevel1->TrigRates[5]));

  return true;
}
