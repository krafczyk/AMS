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

  int L1TrMemPatt = -1;
  int PhysTrPatt = -1;

  if(!IsMCDataRun()) {
    // FIXME: Is this also needed for BT data? Then this is correct, otherwise the check shall use IsISSData() explicitely.
    pLevel1->RestorePhysBPat();
    PhysTrPatt = pLevel1->PhysBPatt;
    L1TrMemPatt = pLevel1->JMembPatt;
  }else{ // MC
    pLevel1->RebuildTrigPatt(L1TrMemPatt, PhysTrPatt);
  }

  fEvent->fTrigger.fBits = AC::Trigger::GenerateBitsFromFlags(PhysTrPatt, pLevel1->TofFlag1, pLevel1->TofFlag2, pLevel1->AntiPatt, pLevel1->EcalFlag);

  if (fEvent->fTrigger.PhysBFlags() != PhysTrPatt)
    WARN_OUT << "Problem building AC::Trigger: PhysBFlags() don't match, got: "
             << fEvent->fTrigger.PhysBFlags() << " expected: " << PhysTrPatt << std::endl;

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

  if (L1TrMemPatt & 0xFFFF0000) {
    WARN_OUT << "Problem building AC::Trigger: JMembPatt upper 16 bits not 0: ";
    printf("%8X\n",L1TrMemPatt);
  }

  fEvent->fTrigger.fTimeDifference = min((unsigned int)0xffff, pLevel1->TrigTime[4]);
  fEvent->fTrigger.fJMembPatt = (L1TrMemPatt) & 0xFFFF;
  fEvent->fTrigger.fLiveTime = fAMSEvent->LiveTime();                                 // replaces pLevel1->LiveTime;
  fEvent->fTrigger.fTriggerRateFT = min(65535,(int)round(pLevel1->TrigRates[0]));
  fEvent->fTrigger.fTriggerRateFTC = min(65535,(int)round(pLevel1->TrigRates[1]));
  fEvent->fTrigger.fTriggerRateFTZ = min(65535,(int)round(pLevel1->TrigRates[2]));
  fEvent->fTrigger.fTriggerRateFTE = min(65535,(int)round(pLevel1->TrigRates[3]));
  fEvent->fTrigger.fTriggerRateLV1 = min(65535,(int)round(pLevel1->TrigRates[5]));

  fEvent->fTrigger.fTofPatt1       = AC::Trigger::GenerateBitsFromTofPatt(pLevel1->TofPatt1);   // TOF-Pattern1: FTC (fast-trigger-charge >= 1)

  // debug TofPatt:
  // char number[2];
  // for( int layer=0; layer<4; layer++ ) {
  //   printf("TOF Layer %1d FTC Lv1 %8X ACQt: ", layer, pLevel1->TofPatt1[layer]);
  //   for( int j=(layer==2?9:7); j>=0; j--) { 
  //     sprintf(number,"%1d",j);
  //     printf("%1s",fEvent->fTrigger.HasCoincidenceFromTofLayerBar(layer,j)?number:".");
  //   }
  //   printf("\n"); 
  // }

  return true;
}
