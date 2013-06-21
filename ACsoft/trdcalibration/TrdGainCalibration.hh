#ifndef TRDGAINCALIBRATION_HH
#define TRDGAINCALIBRATION_HH

#include <string>

#include <TTimeStamp.h>

#include "SimpleGraphLookup.hh"
#include "Settings.h"
#include "TrdHitFactory.hh"

class TH1;
class TH2I;

namespace ACsoft {

namespace Utilities {
  class ConfigHandler;
}

namespace Analysis {
  class Particle;
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

  void Process( const Analysis::Particle&, bool checkMode );

  void Initialize();

  void WriteResultsToCurrentFile();


  static int AnalyzeGainHistograms( std::string inputfile,
                                    std::string histogram,
                                    int requiredNumberOfEntries,
                                    bool interactive = false,
                                    bool snapshot = false,
                                    std::string resultfile = "fitresults.root",
                                    int testbin = 0 );

private:

  void CreateDeDxHistos();
  void StoreDeDx( unsigned int moduleNumber, const TTimeStamp& time, Double_t dedx );


protected:

  bool fIsInitialized;
  Analysis::TrdHitFactory fTrdHitFactory;
  std::vector<TH1*> fDeDxHistos;

};


/** Lookup for gain calibration values.
  *
  */
class TrdGainParametersLookup {
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
  Double_t GetGainValue( unsigned int module, double time ) const {

    const Utilities::SimpleGraphLookup& lookup = Utilities::IsBeamTestTime(time) ? fBeamTestLookup : fISSLookup;
    Utilities::Quantity result = lookup.Query(module,time,false);
    fLastQueryOk = lookup.LastQueryOk();
    return result.value;
  }

  bool LastQueryOk() const { return fLastQueryOk; }


private:
  TrdGainParametersLookup()
    : fISSLookup(::AC::Settings::gTrdQtGainFileName,
                 ::AC::Settings::gTrdQtGainFileNameExpectedGitSHA,
                 ::AC::Settings::gTrdQtGainFileNameExpectedVersion,
                 "fitMpvGraphGoodFit", "", 0, 327)
    , fBeamTestLookup(::AC::Settings::gTrdQtBeamTestGainFileName,
                      ::AC::Settings::gTrdQtBeamTestGainFileNameExpectedGitSHA,
                      ::AC::Settings::gTrdQtBeamTestGainFileNameExpectedVersion,
                      "fitMpvGraphGoodFit", "", 0, 327)
    , fLastQueryOk(false){

  }

  Utilities::SimpleGraphLookup fISSLookup;
  Utilities::SimpleGraphLookup fBeamTestLookup;

  mutable bool fLastQueryOk;

};

}

}

#endif // TRDGAINCALIBRATION_HH
