/* This file was generated by acqt_streamer_generator from the corresponding IDL file. DO NOT EDIT MANUALLY! */

#ifndef AC_TRDVTrack_Streamer_h
#define AC_TRDVTrack_Streamer_h

#define AC_TRDVTrack_Variables \
private: \
  UShort_t fStatus; \
  Float_t fChiSquare; \
  Float_t fTheta; \
  Float_t fPhi; \
  Float_t fX; \
  Float_t fY; \
  Float_t fZ; \
  UInt_t fPattern; \
  ChargesVector fCharges; \
  ChargesProbabilityVector fChargesProbability; \
  ChargesNewVector fChargesNew; \
public: \
  TRDVTrack() \
    : fStatus(0) \
    , fChiSquare(0) \
    , fTheta(0) \
    , fPhi(0) \
    , fX(0) \
    , fY(0) \
    , fZ(0) \
    , fPattern(0) \
    , fCharges() \
    , fChargesProbability() \
    , fChargesNew() \
  { \
  }

#endif // AC_TRDVTrack_Streamer_h