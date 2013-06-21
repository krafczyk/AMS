#ifndef TRDPDFLOOKUP_HH
#define TRDPDFLOOKUP_HH

#include <TTimeStamp.h>

#include "AMSGeometry.h"
#include "SimpleGraphLookup.hh"

class TF1;

namespace AC {
  class ACQtPDFLookupFileIdentifier;
}

namespace ACsoft {

namespace Utilities {

/** Particle id
  *
  */
enum ParticleId {
  Electron  = 0,
  Proton    = 1,
  Helium    = 2,
  Lithium   = 3,
  Beryllium = 4,
  Boron     = 5,
  Carbon    = 6
};

/** Lookup for energy deposition probability density functions (pdf) in the TRD.
  *
  * \test \c utilities/test/test_PdfLookup.C
  */
class TrdPdfLookup
{

public:
  static TrdPdfLookup* Self() {

    static TrdPdfLookup* gInstance = 0;
    if (!gInstance)
      gInstance = new TrdPdfLookup;
    return gInstance;
  }

  ~TrdPdfLookup();

  static double QueryXenonPressure( const TTimeStamp& time, bool &queryOk );
  static double QueryXenonPressure( double            time, bool &queryOk );

  double GetTrdPdfValue( int XenonBin, unsigned int rigidityBin, unsigned int layer, double dEdX, ParticleId pid ) const;
  double GetTrdPdfValue( double pXe, double rigidity, unsigned int layer, double dEdX, ParticleId pid ) const;

  double GetToymcDedxValue( int XenonBin, unsigned int rigidityBin, unsigned int layer, ParticleId pid );
  double GetToymcDedxValue( double pXe, double rigidity, unsigned int layer, ParticleId pid );

  double GetTrdPvalue( int XenonBin, unsigned int rigidityBin, unsigned int layer, double dEdX, ParticleId pid ) const;

  bool LastQueryOk() const { return fLastQueryOk; }


   /** Was the AddNearTrackHits algorithm chosen for the PDF generation?
    */
  bool AddNearTrackHitsForCandidateMatching() const { return fAddNearTrackHits; }

  static const int fNumberOfParticles = 7;
  static const int fNumberOfXenonBins = 6;
  static const std::string fParticleNames[fNumberOfParticles];
  static const float fRigidityLowerLimits[fNumberOfParticles];
  static const float fRigidityUpperLimits[fNumberOfParticles];
  static const int MaximumNumberOfRigidityBins = 40;
  static const int fNumberOfRigidityBins[fNumberOfParticles];
  static std::vector<double> GetBinningForParticle( ParticleId particle );

  static int GetXenonBin( const TTimeStamp& time, bool& queryOk );
  static int GetXenonBin( double pressureXe );
  static int GetRigidityBin( double rigidity, ParticleId pid );

  /** Returns the settings as they are stored in the active PDF file
    */
  ::AC::ACQtPDFLookupFileIdentifier* Identifier() const { return fIdentifier; }

private:

  TrdPdfLookup();
  void InitializePdfFunctions();

  Double_t FunPDF(Double_t* x, Double_t* par);

private:

  SimpleGraphLookup* PdfLookup[fNumberOfParticles];
  SimpleGraphLookup* pValueLookup[fNumberOfParticles];

  /// helper TF1 objects needed for the calculation of toy MC energy depositions
  TF1* fPDFs[fNumberOfXenonBins][MaximumNumberOfRigidityBins][AC::AMSGeometry::TRDLayers][fNumberOfParticles];
  bool fPdfFunctionsInitialized;

  bool fAddNearTrackHits;
  ::AC::ACQtPDFLookupFileIdentifier* fIdentifier;

  mutable bool fLastQueryOk;
};

}


}

#endif // TRDPDFLOOKUP_HH
