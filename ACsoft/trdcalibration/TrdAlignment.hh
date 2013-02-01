#ifndef TRDALIGNMENT_HH
#define TRDALIGNMENT_HH

#include <string>

#include <TTimeStamp.h>

#include <SimpleGraphLookup.hh>
#include "Settings.h"

class TH1;
class TH2I;

namespace ACsoft {

namespace Utilities {
  class ConfigHandler;
}

namespace Analysis {
  class Particle;
}

/// Classes for managing TRD calibration workflow and results.
namespace Calibration {

/** General manager class for TRD alignment.
  *
  * Principal steps:
  *   -# Use the ProcessEvent() and WriteResultsToCurrentFile() functions to generate histograms of effective module shifts. This will typically be done on small chunks of runs in parallel.
  *   -# Merge the resulting files (with the ac_merge tool).
  *   -# Call the AnalyzeAlignmentShiftHistograms() static helper function to fit projections of the merged histograms and store the results.
  *   -# Produce a lookup file from the fit results.
  *   -# The resulting lookup file can then be accessed by the TrdAlignmentShiftLookup class.
  */
class TrdAlignment
{
public:
  explicit TrdAlignment( Utilities::ConfigHandler* );

  void Process( const Analysis::Particle& );

  void Initialize();

  void WriteResultsToCurrentFile();


  static int AnalyzeAlignmentShiftHistograms( std::string inputfile,
                                              std::string histogram,
                                              int requiredNumberOfEntries = 10000,
                                              bool interactive = false,
                                              bool snapshot = false,
                                              std::string resultfile = "fitresults.root",
                                              int testbin = 0 );

private:

  void CreateAlignmentShiftHistos();
  void StoreAlignmentShift( unsigned int module, const TTimeStamp& time, Double_t shift );

protected:

  bool fIsInitialized;

  std::vector<TH1*> fAlignmentShiftHistos;
  TH2I* fNumberOfTrdHitsWithinCut;
  TH2I* fNumberOfTrdLayersWithHitWithinCut;
  TH2I* fNumberOfHitsOnLayersWithHitWithinCut;
  TH2I* fPathlengthHisto;
  TH2I* fNumberOfTrdLayersWithAtLeastOneHitWithNonzeroPathlength;
  TH2I* fNumberOfTrdLayersWithAtLeastOneHitWithPathlengthAboveCut;
  TH2I* fEventTimeHisto;
  TH2I* fIssPositionHisto;
};


/** Lookup for effective alignment shifts.
  *
  * \todo relax cuts on fit results, because some modules have zero entries in the good-fit graph
  */
class TrdAlignmentShiftLookup : public Utilities::SimpleGraphLookup {
public:
  static TrdAlignmentShiftLookup* Self() {

    static TrdAlignmentShiftLookup* gInstance = 0;
    if (!gInstance)
      gInstance = new TrdAlignmentShiftLookup;
    return gInstance;
  }

  /** Find alignment shift for given module number and time.
    *
    * If the uncertainty on the alignment shift is needed, the Utilities::SimpleGraphLookup::Query() function can also be used directly.
    *
    * \param module module number (0..327)
    * \param time time (unix time, seconds since epoch)
    *
    * \returns effective alignment shift in transverse (y or x) direction (cm)
    */
  virtual Double_t GetAlignmentShift( unsigned int module, double time ) const {
    Utilities::Quantity result = Utilities::SimpleGraphLookup::Query(module,time,false);
    return result.value;
  }

private:
  TrdAlignmentShiftLookup()
    : Utilities::SimpleGraphLookup(::AC::Settings::gTrdQtAlignmentFileName,
                                   ::AC::Settings::gTrdQtAlignmentFileNameExpectedGitSHA,
                                   ::AC::Settings::gTrdQtAlignmentFileNameExpectedVersion,
                                   "fitMeanGraphGoodFit", "", 0, 327) { }
};

}

}

#endif // TRDALIGNMENT_HH
