#ifndef TRDHITFACTORY_HH
#define TRDHITFACTORY_HH

#include <TrdHit.hh>

namespace AC {
  class EventHeader;
  class TRDRawHit;
}

namespace Analysis {

class SplineTrack;

/** Steps used by TrdHitFactory in production of TrdHit objects.
  */
enum TrdHitProductionSteps { SecondCoord = 1<<1, ShimGlobalAlign = 1<<2, AlignmentCorrection = 1<<3, Pathlength = 1<<4, GainCorrection = 1<<5 };

/** Produce TRD hits for analysis from raw hits and additional sources.
  *
  */
class TrdHitFactory
{
public:

  TrdHitFactory();

  TrdHit ProduceTrdHitFrom( const AC::TRDRawHit& rhit, const SplineTrack& splineTrack, const AC::EventHeader& header, int steps );

private:

  bool TestOption( int opt, TrdHitProductionSteps step ) const { return ( (opt & step) == step ); }

private:

  /// perform gain correction such that Landau MPV of proton dE/dx distribution at momentum fixed in TrdGainCalibration arrives at this value (ADC / cm)
  const Double_t fReferenceGain;
};
}
#endif // TRDHITFACTORY_HH
