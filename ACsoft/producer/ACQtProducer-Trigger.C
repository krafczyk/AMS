#include "ACQtProducer-include.C"

bool ACQtProducer::ProduceTrigger() {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);

  if (fAMSEvent->nLevel1() != 1) {
    WARN_OUT << "Problem building AC::Trigger: AMS ROOT File is broken! "
             << "nLevel1() returned: " << fAMSEvent->nLevel1() << " expected: 1." << std::endl;
    return false;
  }

  Level1R* pLevel1 = fAMSEvent->pLevel1(0);
  if (!pLevel1) {
    WARN_OUT << "Problem building AC::Trigger: AMS ROOT File is broken! "
             << "pLevel1(0) is null." << std::endl;
    return false;
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
