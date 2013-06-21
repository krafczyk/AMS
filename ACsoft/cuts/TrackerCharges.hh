#ifndef TRACKERCHARGES_HH
#define TRACKERCHARGES_HH

namespace Cuts {

class TrackerCharges {
public:
  static float GetMean(unsigned int index);
  static float GetSigma(unsigned int index);
  static const unsigned int gTrackerCharges = 8;
  
private:
  static void InitTrackerChargesFile();
  static float gTrackerChargeMean[gTrackerCharges];
  static float gTrackerChargeSigma[gTrackerCharges];
  static bool gTrackerChargeIsInitialized;
};

}

#endif
