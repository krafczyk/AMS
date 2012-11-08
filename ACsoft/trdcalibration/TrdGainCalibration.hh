#ifndef TRDGAINCALIBRATION_HH
#define TRDGAINCALIBRATION_HH

#include <string>

#include <SimpleGraphLookup.hh>
#include "Settings.h"

class TH2I;

namespace Utilities {
  class ConfigHandler;
}

namespace Analysis {
  class Particle;
}

namespace Detector {
  class Trd;
}


namespace Calibration {

/** General manager class for TRD gain calibration.
  *
  * Principal steps:
  *   -# Perform TRD alignment first.
  *   -# Use the ProcessEvent() and WriteResultsToCurrentFile() functions to generate histograms of dE/dx values (use a suitable proton selection).
  *      This will typically be done on small chunks of runs in parallel.
  *   -# Merge the resulting files (with the ac_merge tool).
  *   -# Call the AnalyzeGainHistograms() static helper function to fit projections of the merged histograms and store the results.
  *   -# Produce a lookup file from the fit results.
  *   -# The resulting lookup file can then be accessed by the TrdGainParametersLookup class.
  */
class TrdGainCalibration
{

public:

  explicit TrdGainCalibration( Utilities::ConfigHandler* );

  void Process( const Analysis::Particle& );

  void Initialize();

  void WriteResultsToCurrentFile();


  static int AnalyzeGainHistograms( std::string inputfile,
                                    std::string histogram,
                                    int requiredNumberOfEntries,
                                    bool interactive = false,
                                    std::string resultfile = "fitresults.root",
                                    int testbin = 0 );
protected:

  Detector::Trd* fTrd;

  bool fIsInitialized;

};


/** Lookup for gain calibration values.
  *
  */
class TrdGainParametersLookup : public Utilities::SimpleGraphLookup {
public:
  static TrdGainParametersLookup* Self() {

    static TrdGainParametersLookup* gInstance = 0;
    if (!gInstance)
      gInstance = new TrdGainParametersLookup;
    return gInstance;
  }

  /** Find gain for given module number and time.
    *
    * If the uncertainty on the gain calibration factor is needed, the Utilities::SimpleGraphLookup::Query() function can also be used directly.
    *
    * \param module module number (0..327)
    * \param time time (unix time, seconds since epoch)
    *
    * \returns MPV of Landau fit to dE/dx distribution of proton events at the reference momentum used by TrdGainCalibration (ADC/cm)
    */
  virtual Double_t GetGainValue( unsigned int module, double time ) const {
    Quantity result = Utilities::SimpleGraphLookup::Query(module,time,false);
    return result.value;
  }

private:
  TrdGainParametersLookup()
    : Utilities::SimpleGraphLookup(AC::Settings::gTrdQtGainFileName,
                                   AC::Settings::gTrdQtGainFileNameExpectedGitSHA,
                                   AC::Settings::gTrdQtGainFileNameExpectedVersion,
                                   "fitMpvGraphGoodFit", "", 0, 327) { }
};

}

#endif // TRDGAINCALIBRATION_HH
