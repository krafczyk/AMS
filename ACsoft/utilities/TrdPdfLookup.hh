#ifndef TRDPDFLOOKUP_HH
#define TRDPDFLOOKUP_HH

#include <TTimeStamp.h>
#include "SimpleGraphLookup.hh"

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

  /** Returns the xenon partial pressure for a given time */
  static double QueryXenonPressure(const TTimeStamp&);

  /** Returns the Trd PDF value for a given XenonBin, rigidityBin, TRD layer, measured dEdX and particle type pid */
  double GetTrdPdfValue( unsigned int XenonBin, unsigned int rigidityBin, unsigned int layer, double dEdX, ParticleId pid ) const;

  /** Returns the Trd p-value for a given XenonBin, rigidityBin, TRD layer, measured dEdX and particle type pid */
  double GetTrdPvalue( unsigned int XenonBin, unsigned int rigidityBin, unsigned int layer, double dEdX, ParticleId pid ) const;

   /** Returns whether the activeStraws algorithm was chosen for the PDF generation
    * \todo Add better documentation about the algorithm.
    */
  bool AddNearTrackHitsForCandidateMatching() const { return fAddNearTrackHits; }

  static const int fNumberOfParticles = 7;
  static const int fNumberOfXenonBins = 6;
  static const std::string fParticleNames[fNumberOfParticles];
  static const float fRigidityLowerLimits[fNumberOfParticles];
  static const float fRigidityUpperLimits[fNumberOfParticles];
  static const int fNumberOfRigidityBins[fNumberOfParticles];
  static std::vector<double> GetBinningForParticle(int particle);

  static int GetXenonBin(const TTimeStamp&);
  static int GetXenonBin(const double pressureXe);

  /** Returns the settings as they are stored in the active PDF file
    */
  ::AC::ACQtPDFLookupFileIdentifier* Identifier() const { return fIdentifier; }

private:
  TrdPdfLookup();

  SimpleGraphLookup* PdfLookup[fNumberOfParticles];
  SimpleGraphLookup* pValueLookup[fNumberOfParticles];

  bool fAddNearTrackHits;
  ::AC::ACQtPDFLookupFileIdentifier* fIdentifier;
};

}


}

#endif // TRDPDFLOOKUP_HH
