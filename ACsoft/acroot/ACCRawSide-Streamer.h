/* This file was generated by acqt_streamer_generator from the corresponding IDL file. DO NOT EDIT MANUALLY! */

#ifndef AC_ACCRawSide_Streamer_h
#define AC_ACCRawSide_Streamer_h

#define AC_ACCRawSide_Variables \
private: \
  UChar_t fHWAddress; \
  UChar_t fStatus; \
  Float_t fTemperature; \
  Float_t fADC; \
  ACCHitVector fFTDCHits; \
  ACCTimeVector fTDCTimes; \
public: \
  ACCRawSide() \
    : fHWAddress(0) \
    , fStatus(0) \
    , fTemperature(0) \
    , fADC(0) \
    , fFTDCHits() \
    , fTDCTimes() \
  { \
  }

#endif // AC_ACCRawSide_Streamer_h