#ifndef TRDLIKELIHOOD_HH
#define TRDLIKELIHOOD_HH

#include <string>

#include <SimpleGraphLookup.hh>

class TH2F;

namespace Detector {
  class Trd;
}

namespace Utilities {
  class ConfigHandler;
}


namespace Analysis {
  class TrdCandidateMatching;
  class Particle;
}

namespace Calibration {

/** General manager class for TRD likelihood calculations.
  */
class TrdLikelihood
{

public:

  explicit TrdLikelihood( Utilities::ConfigHandler* );

  void Process( const Analysis::Particle& p );

  void Initialize();

  void WriteResultsToCurrentFile();

private:
  Detector::Trd* fTrd;
  Analysis::TrdCandidateMatching* fTrdCandidateMatching;

  bool fIsInitialized;
  unsigned short fVersion;
  float fMinPathLength;
  bool fUseActiveStraws;

  TH2F* fTrdRawHits;
  TH2F* fTrdSignalHeightPerStraw;
  TH2F* fTrdHitCountPerStraw;
  TH2F* fTrdHitsOnTrackerTrack;
  TH2F* fTrdGainPerParticle[3]; // 0: e-, 1: p, 2: He

  TH2F* fTOFCharge_vs_TrackerCharge;
  TH2F* fTOFCharge_vs_rigidity[2];
  TH2F* fTOFCharge_vs_time[2];
  TH2F* fTrackerCharge_vs_rigidity[2];
  TH2F* fTrackerCharge_vs_time[2];
};

}

#endif // TRDLIKELIHOOD_HH
