/* This file was generated by acqt_streamer_generator from the corresponding IDL file. DO NOT EDIT MANUALLY! */

#ifndef AC_TOFBeta_Streamer_h
#define AC_TOFBeta_Streamer_h

#define AC_TOFBeta_Variables \
private: \
  Float_t fBeta; \
  Float_t fBetaError; \
  Char_t fTrackerTrackIndex; \
  ChargesVector fCharges; \
  ChargesProbabilityVector fChargesProbability; \
  ChargesNewVector fChargesNew; \
  ClusterIndexVector fTOFClusterIndex; \
public: \
  TOFBeta() \
    : fBeta(0) \
    , fBetaError(0) \
    , fTrackerTrackIndex(0) \
    , fCharges() \
    , fChargesProbability() \
    , fChargesNew() \
    , fTOFClusterIndex() \
  { \
  }

#endif // AC_TOFBeta_Streamer_h
