/* This file was generated by acqt_streamer_generator from the corresponding IDL file. DO NOT EDIT MANUALLY! */

#ifndef AC_MCTRDCluster_Streamer_h
#define AC_MCTRDCluster_Streamer_h

#define AC_MCTRDCluster_Variables \
private: \
  Short_t fID; \
  UShort_t fLLT; \
  EnergyLossVector fEnergyLoss; \
  Float_t fKineticEnergy; \
  Float_t fDepositedEnergy; \
  Float_t fStep; \
public: \
  MCTRDCluster() \
    : fID(0) \
    , fLLT(0) \
    , fEnergyLoss() \
    , fKineticEnergy(0) \
    , fDepositedEnergy(0) \
    , fStep(0) \
  { \
  }

#endif // AC_MCTRDCluster_Streamer_h