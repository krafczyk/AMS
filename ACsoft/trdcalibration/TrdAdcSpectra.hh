#ifndef TRDADCSPECTRA_HH
#define TRDADCSPECTRA_HH

#include <string>

#include "SimpleGraphLookup.hh"
#include "TrdPdfLookup.hh"


class TH2F;
class TH3F;

namespace ACsoft {

namespace Utilities {
  class ConfigHandler;
}


namespace Analysis {
  class TrdCandidateMatching;
  class Particle;
}

namespace Calibration {

/** Calibration class for storing %TRD ADC spectra in histograms.
  *
  * The resulting histograms (dE/dx vs layer or rigidity and time) are then processed further by
  * the \c produce_pdf_file tool.
  */
class TrdAdcSpectra
{

public:

  TrdAdcSpectra( Utilities::ConfigHandler*, unsigned short version, bool AddNearTrackHits );

  void Process( const Analysis::Particle& p );

  void Initialize();

  void WriteResultsToCurrentFile();

private:

  void StoreADCSpectra( const TTimeStamp& time, int particleID, double dedx, double absoluteRigidity, short layer );
  void CreateADCSpectraHistos();

private:

  Analysis::TrdCandidateMatching* fTrdCandidateMatching;

  bool fIsInitialized;
  unsigned short fVersion;
  bool fAddNearTrackHits;

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

  TH3F* fADCSpectrum_vs_Rigidity[Utilities::TrdPdfLookup::fNumberOfParticles];
  TH3F* fADCSpectrum_vs_Layer[Utilities::TrdPdfLookup::fNumberOfParticles];

};

}

}

#endif
