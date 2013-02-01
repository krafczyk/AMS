/* This file was generated by acqt_streamer_generator from the corresponding IDL file. DO NOT EDIT MANUALLY! */

#ifndef AC_RICHRing_Streamer_h
#define AC_RICHRing_Streamer_h

#define AC_RICHRing_Variables \
private: \
  UInt_t fStatus; \
  UShort_t fNumberOfHits; \
  Float_t fBeta; \
  Float_t fBetaError; \
  Float_t fX; \
  Float_t fY; \
  Float_t fZ; \
  Float_t fTheta; \
  Float_t fPhi; \
  Float_t fProbability; \
  Float_t fChargeEstimate; \
  ChargesVector fCharges; \
  ChargesProbabilityVector fChargesProbability; \
public: \
  RICHRing() \
    : fStatus(0) \
    , fNumberOfHits(0) \
    , fBeta(0) \
    , fBetaError(0) \
    , fX(0) \
    , fY(0) \
    , fZ(0) \
    , fTheta(0) \
    , fPhi(0) \
    , fProbability(0) \
    , fChargeEstimate(0) \
    , fCharges() \
    , fChargesProbability() \
  { \
  }

#endif // AC_RICHRing_Streamer_h