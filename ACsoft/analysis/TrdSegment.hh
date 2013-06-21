#ifndef TRDSEGMENT_HH
#define TRDSEGMENT_HH

#include "TRDRawHit.h"

#include "TrdMissingHit.hh"

#include <vector>

class TGraph;

namespace ACsoft {

namespace Analysis {

class TrdTracking;

/** A potential %TRD track in one projection (XZ or YZ) as identified by the %TRD tracking.
  *
  *
  *
  * \sa Analysis::TrdTracking
  */
class TrdSegment
{

public:

  TrdSegment(AC::MeasurementMode direction);

  void Draw( float zmin, float zmax, bool rotatedSystem = false, Color_t lineColour = kRed, int nPoints=1000 ) const;

  AC::MeasurementMode Direction() const { return fDirection; }

  unsigned int NumberOfRawHits() const { return fTrdRawHits.size(); }
  const AC::TRDRawHit* RawHit( unsigned int index ) const { return fTrdRawHits.at(index); }
  bool HasRawHitOnSublayer( int globalSublayerNumber ) const;
  int LastSublayerInSegment() const;
  int FirstSublayerInSegment() const;
  int NumberOfSublayersInSegment() const;

  unsigned int NumberOfMissingHits() const { return fTrdMissingHits.size(); }
  const TrdMissingHit& MissingHit( unsigned int index ) const { return fTrdMissingHits.at(index); }

  int NumberOfPairsCloseToMaximum() const { return fNumberOfPairsCloseToMaximum; }


  float EvalXYFromTrackFinding( float z ) const { return fSlopeTrackFinding*(z-fOffsetReferenceZ) + fOffsetTrackFinding; }
  bool OverlapsWith( const TrdSegment& other ) const;

  float SlopeTrackFinding() const { return fSlopeTrackFinding; }
  float OffsetTrackFinding() const { return fOffsetTrackFinding; }
  float OffsetReferenceZ() const { return fOffsetReferenceZ; }

  void Dump() const;

private:

  friend class TrdTracking;

  AC::MeasurementMode fDirection; ///< determines whether segment belongs to XZ or YZ projection

  float fSlopeTrackFinding;   ///< dx/dz or dy/dz, respectively, from location of peak in trackfinding histogram
  float fOffsetTrackFinding;  ///< offset at reference z, from location of peak in trackfinding histogram
  float fOffsetReferenceZ;    ///< pivot z used in parametrization of straight line

  /// a measure for the strength or significance of this peak: number of hit pairs found within a radius around the maximum in the trackfinding histogram
  int fNumberOfPairsCloseToMaximum;

  std::vector<const AC::TRDRawHit*> fTrdRawHits; ///< list of candidate raw hits belonging to the segment
  std::vector<TrdMissingHit> fTrdMissingHits; ///< list of missing hits

  mutable TGraph* fDrawGraphTrackFinding; ///< graph used for drawing purposes

};

}

}

#endif // TRDSEGMENT_HH
