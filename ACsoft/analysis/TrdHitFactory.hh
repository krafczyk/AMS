#ifndef TRDHITFACTORY_HH
#define TRDHITFACTORY_HH

#include <vector>
#include <TrdHit.hh>

namespace ACsoft {

namespace AC {
  class Event;
  class TRDRawHit;
}

namespace Analysis {

class SplineTrack;
class TrdTrack;

/** Steps used by TrdHitFactory in production of TrdHit objects.
  */
enum TrdHitProductionSteps { TrackInfo = 1<<1, GainCorrection = 1<<2 };

/** Produce TRD hits for analysis from raw hits and additional sources.
  *
  */
class TrdHitFactory
{
public:

  TrdHitFactory();

  void   ProduceTrdHitsFrom( const AC::Event& event, const SplineTrack& splineTrack, int steps, std::vector<TrdHit>& resultVector );
  void   ProduceTrdHitsFrom( const AC::Event& event, const TrdTrack& trdTrack, int steps, std::vector<TrdHit>& resultVector );
  TrdHit ProduceTrdHitFrom( const AC::TRDRawHit& rhit, const SplineTrack& splineTrack, int steps );
  TrdHit ProduceTrdHitFrom( const AC::TRDRawHit& rhit, const TrdTrack& trdTrack, int steps );

  void   ProduceBasicTrdHitsFrom( const AC::Event& event, std::vector<TrdHit>& resultVector);

private:

  bool TestOption( int opt, TrdHitProductionSteps step ) const { return ( (opt & step) == step ); }

private:

  /// perform gain correction such that Landau MPV of proton dE/dx distribution at momentum fixed in TrdGainCalibration arrives at this value (ADC / cm)
  const Double_t fReferenceGain;
};
}
}

#endif // TRDHITFACTORY_HH
