#include <RunTagDecoder.hh>
#include <iostream>

namespace ACsoft {

namespace Utilities {
  RunTagDecoder::RunTagDecoder( const UShort_t& RunTag ) {
    fNominal = RunTagDecoder::IsNominal( RunTag );
    fDebug   = RunTagDecoder::IsDebug( RunTag );
    fTRDRefill = RunTagDecoder::IsTRDRefill( RunTag );
    fUndefined = !( fNominal | fDebug | fTRDRefill );
    if( fNominal | fDebug ) {
      fNumber = 0x0FFF & RunTag;
    } else if( fTRDRefill ) {
      fNumber = 0x00FF & RunTag;
    } else {
      fNumber = 0xFFFF & RunTag;
    }
  }
  
  bool RunTagDecoder::IsNominal( const UShort_t& RunTag ) {
    if( (RunTag & 0xF000) == 0xF000 ) {
      return true;
    }
    return false;
  }
  
  bool RunTagDecoder::IsDebug( const UShort_t& RunTag ) {
    if( (RunTag & 0xF000) == 0xD000 ) {
      return true;
    }
    return false;
  }
  
  bool RunTagDecoder::IsTRDRefill( const UShort_t& RunTag ) {
    if( (RunTag & 0xFF00) == 0xED00 ) {
      return true;
    }
    return false;
  }
  
  bool RunTagDecoder::IsUndefined( const UShort_t& RunTag ) {
    bool nominal = IsNominal( RunTag );
    bool debug   = IsDebug( RunTag );
    bool refill  = IsTRDRefill( RunTag );
    return !(nominal | debug | refill);
  }
}


}
