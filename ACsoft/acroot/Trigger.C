#include "Trigger.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_TABLE(Trigger)
  COL( "DTime [us]",  UShort_t,        TimeDifference)
  COL( "LTime\t",     Float_t,         LiveTime)
  COL( "FT  ",        UShort_t,        TriggerRateFT)
  COL( "FTC ",        UShort_t,        TriggerRateFTC)
  COL( "FTZ ",        UShort_t,        TriggerRateFTZ)
  COL( "FTE ",        UShort_t,        TriggerRateFTE)
  COL( "LV1 ",        UShort_t,        TriggerRateLV1)
  COL( "PhysB\t",     std::bitset<8>,  PhysBFlagsBitset)
  COL( "TOF1",        Int_t,           TOFFlags1)
  COL( "TOF2",        Int_t,           TOFFlags2)
  COL( "ACC \t",      std::bitset<8>,  ACCFlagsBitset)
  COL( "ECAL \t",     std::bitset<8>,  ECALFlagsBitset)
  COL( "JMemb\t\t\t", std::bitset<16>, JMembFlagsBitset)
END_DEBUG_TABLE

void Trigger::Clear() {

  fTimeDifference = 0;
  fBits = 0;
  fJMembPatt = 0;
  fLiveTime = 0;
  fTriggerRateFT = 0;
  fTriggerRateFTC = 0;
  fTriggerRateFTZ = 0;
  fTriggerRateFTE = 0;
  fTriggerRateLV1 = 0;
}

}

}
