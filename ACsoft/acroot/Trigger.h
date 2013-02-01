#ifndef Trigger_h
#define Trigger_h

#include <bitset>

#include "Tools.h"
#include "Trigger-Streamer.h"

namespace ACsoft {

namespace AC {

/** %Trigger data
  */
class Trigger {
  WTF_MAKE_FAST_ALLOCATED;
public:
  AC_Trigger_Variables

  /** Returns whether trigger information are available.
    */
  bool HasData() const {

    return fTimeDifference != 0
        || fBits != 0
        || fJMembPatt != 0
        || fLiveTime != 0
        || fTriggerRateFT != 0
        || fTriggerRateFTC != 0
        || fTriggerRateFTZ != 0
        || fTriggerRateFTE != 0
        || fTriggerRateLV1 != 0;
  }

  /** Helper method dumping an Trigger object to the console
    */
  void Dump() const;

  /** Clear event to the initial empty state, as it was just constructed.
    */
  void Clear();

  /** 'PhysBPatt' flags.
    * Note:
    * 8 lsbits-> pattern of LVL1 sub-triggers ("predefined physics" branches) 
    * See http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/root.h?view=markup for details.
    */
  Int_t PhysBFlags() const { return (fBits >> 24) & 0xff; }

  /** 'TofFlag1' flags.
    * Note:
    * FTC(z>=1) LayersPatternCode, <0:noFTC,=0:4L,(1-4):1missLnumb,5:1+3,6:1+4,7:2+3,8:2+4,9:1+2,10:3+4,(11-14):1,..4. 
    * See http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/root.h?view=markup for details.
    */
  Int_t TOFFlags1() const { return (fBits >> 20) & 0x0f; }

  /** 'TofFlag2' flags.
    * Note:
    * BZ(z>=2) LayersPatternCode, <0:noBZ,=0:4L,(1-4):1missLnumb,..... as above. 
    * See http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/root.h?view=markup for details.
    */
  Int_t TOFFlags2() const { return (fBits >> 16) & 0x0f; }
  
  /** 'AntiPatt' flags.
    * Note:
    * Antipatt:(1-8)bits->sectors in coincidence with FastTrigger. 
    * See http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/root.h?view=markup for details.
    */
  Int_t ACCFlags() const { return (fBits >> 8) & 0xff; }

  /** 'EcalFlag' flags.
    * Note: EcalFlag=MN, where
    * M=0/1/2/3->FTE(energ=multipl_High):no/no_in1prj@2prj-requir/FTE&in1proj/FTE&in2proj)
    * N=0/1/2/3->LVL1(ShowerAngleSmall):Unknown/in0proj_whenFTEok/in1proj/in2proj
    */
  Int_t ECALFlags() const { return fBits & 0xff; }

  /** 'JMembPatt' flags.
    * Note:
    * bit0:  FTC (TOF FT z>=1)                                   
    * bit1:  LUT-1 decision (used by FTC as OR with LUT-2)       
    * bit2:  LUT-2 .......  (used by FTC as OR with LUT-1)       
    * bit3:  same as  LUT-1, but for subset of central counters  
    * bit4:  same as  LUT-2, but for subset of central counters  
    * bit5:  FTZ (TOF SlowFT z>=2)                               
    * bit6:  FTE (ECAL FT, OR or AND of desisions in 2 proj.)    
    * bit7:  ACC0 (bit set if Nacc < Nthr_1)                     
    * bit8:  ACC1 (bit set if Nacc < Nthr_2)                     
    * bit9:  BZ-flag (LUT-3 decision on z>=2 4-layers config.)   
    * bit10: ECAL FT using AND of 2 projections decisions        
    * bit11: ............. OR ...........................        
    * bit12: ECAL LVL1("shower angle") using AND ........        
    * bit13: ECAL LVL1...................... OR .........        
    * bit14: EXT-trigger_1                                       
    * bit15: EXT-trigger_2                                       
    */
  UShort_t JMembFlags() const { return fJMembPatt; }

  /** Trigger time difference [us].
    */ 
  UShort_t TimeDifference() const { return fTimeDifference; }

  /** Live time.
    * Fraction of [us].
    */ 
  Float_t LiveTime() const { return fLiveTime; }

  /** FT trigger rate [Hz].
    * See http://ams.cern.ch/AMS/Basili/Docs/JLV1/trigger_logic_v02e.pdf for details.
    */
  UShort_t TriggerRateFT() const { return fTriggerRateFT; }

  /** FTC trigger rate [Hz].
    * See http://ams.cern.ch/AMS/Basili/Docs/JLV1/trigger_logic_v02e.pdf for details.
    */
  UShort_t TriggerRateFTC() const { return fTriggerRateFTC; }

  /** FTZ trigger rate [Hz].
    * See http://ams.cern.ch/AMS/Basili/Docs/JLV1/trigger_logic_v02e.pdf for details.
    */
  UShort_t TriggerRateFTZ() const { return fTriggerRateFTZ; }

  /** FTE trigger rate [Hz].
    * See http://ams.cern.ch/AMS/Basili/Docs/JLV1/trigger_logic_v02e.pdf for details.
    */
  UShort_t TriggerRateFTE() const { return fTriggerRateFTE; }

  /** LV1 trigger rate [Hz].
    * See http://ams.cern.ch/AMS/Basili/Docs/JLV1/trigger_logic_v02e.pdf for details.
    */
  UShort_t TriggerRateLV1() const { return fTriggerRateLV1; }

  /** Helper function to generate UInt_t fBits - used by the producer only.
    * \todo Remove this logic from here.
    */
  static UInt_t GenerateBitsFromFlags(Int_t PBP, Int_t TOF1, Int_t TOF2, Int_t ACC, Int_t ECAL) {

    return ((PBP  & 0xff)                     << 24)
          | ((TOF1 < 0 ? 0xf : (TOF1 & 0x0f)) << 20)
          | ((TOF2 < 0 ? 0xf : (TOF2 & 0x0f)) << 16)
          | ((ACC  & 0xff)                     << 8)
          | ((ECAL & 0xff)                         );
  }

private:
  std::bitset<8> PhysBFlagsBitset() const { return std::bitset<8>(PhysBFlags()); }
  std::bitset<16> JMembFlagsBitset() const { return std::bitset<16>(JMembFlags()); }
  std::bitset<8> ACCFlagsBitset() const { return std::bitset<8>(ACCFlags()); }
  std::bitset<8> ECALFlagsBitset() const { return std::bitset<8>(ECALFlags()); }
 
private:
  REGISTER_CLASS_WITH_TABLE(Trigger)
};

}

}

#endif
