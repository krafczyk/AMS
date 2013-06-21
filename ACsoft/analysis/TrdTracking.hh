#ifndef TRDTRACKING_HH
#define TRDTRACKING_HH

#include <vector>

#include "TRDRawHit.h"
#include "TrdSegment.hh"

#include <TStopwatch.h>
#include <Math/Minimizer.h>
#include <Math/Factory.h>

namespace Utilities {
class TrackfindingHistogram;
}

namespace ACsoft {

namespace AC {
class Event;
}

namespace Analysis {

class SplineTrack;
class TrdTrack;

/** TRD track finding and fitting.
  *
  * This class is the main interface for standalone TRD tracking.
  *
  * The general workflow proceeds as follows:
  *
  * - Identify segments in XZ and YZ projections separately, based on matching slope and offset calculated for pairs of %TRD raw hits
  * - Remove overlapping segments
  * - Add missing hits to segments
  * - Create tracks from segments (either as standalone, possible if exactly 1 segment has been identified in XZ and YZ each, or based on an external
  *   reference such as a tracker track of MC generator info)
  * - Maximum-likelihood fit of track parameters
  *
  *
  * The user only has to call the FindTrdSegments() and one of the CreateTrdTracksFromSegments methods explicitly.
  *
  * \attention For performance reasons, do not create a new TrdTracking instance for each event!
  *
  */
class TrdTracking
{

public:

  TrdTracking( int verboseLevel, bool fillMinosErrors );

  void FindTrdSegments( const AC::Event& event );

  const std::vector<TrdSegment>& SegmentsXZ() const { return fSegmentsXZ; }
  const std::vector<TrdSegment>& SegmentsYZ() const { return fSegmentsYZ; }
  const std::vector<TrdSegment>& SegmentsYZ_bottom() const { return fSegmentsYZ_bottom; }

  void DrawTrackFindingHistograms();
  void SetRefillTrackfindingHistos( bool val ) { fRefillTrackfindingHistos = val; }

  int CreateTrdTracksFromSegments_TrdStandalone( std::vector<TrdTrack>& resultVector,
                                                 const AC::Event& event );
  int CreateTrdTracksFromSegments_SplineTrack( const SplineTrack* splineTrack,
                                               std::vector<TrdTrack>& resultVector,
                                               const AC::Event& event );
  int CreateTrdTracksFromSegments_GeneratorTrack( std::vector<TrdTrack>& resultVector,
                                                  const AC::Event& event );


  void PrintProcessingTimes() const;

private:

  int  AddMissingHitsToSegment( const std::vector<const AC::TRDRawHit*>& trdRawHits, TrdSegment& segment );
  bool FitAndAddResults( TrdTrack& track, const AC::Event& event );
  void FindTrdSegmentsForGivenDirection( const std::vector<const AC::TRDRawHit*>& trdRawHits,
                                         ::Utilities::TrackfindingHistogram* trackFindingHisto,
                                         const AC::MeasurementMode dir,
                                         bool addMissingHits, float maxDistance, double refZ,
                                         std::vector<Analysis::TrdSegment>& resultVector );
  void FillTrackfindingHistogram( const std::vector<const AC::TRDRawHit*>& trdRawHits, const AC::MeasurementMode dir,
                                  double refZ,
                                  ::Utilities::TrackfindingHistogram* trackFindingHisto );

private:

  int fVerboseLevel;
  bool fFillMinosErrors;
  bool fRefillTrackfindingHistos; ///< set this flag to true if track finding histograms are to be drawn, but do not use for production since refilling the histos costs some CPU time

  int fNumberOfPairsRequiredXZ;
  int fNumberOfPairsRequiredYZ;
  float fMaxDistance; ///< maximum horizontal (xy) distance for a hit to be added as candidate hit in track finding stage
  float fMaxDistanceCoarse; ///< maximum horizontal (xy) distance for coarse segments

  ::Utilities::TrackfindingHistogram* fTrackFindingHistoXZ;
  ::Utilities::TrackfindingHistogram* fTrackFindingHistoYZ;
  ::Utilities::TrackfindingHistogram* fTrackFindingHistoYZ_bottom; ///< coarser binning: search for hints of a segment in the lower four planes in YZ

  std::vector<Analysis::TrdSegment> fSegmentsXZ; ///< segments identified in XZ projection
  std::vector<Analysis::TrdSegment> fSegmentsYZ; ///< segments identified in YZ projection
  std::vector<Analysis::TrdSegment> fSegmentsYZ_bottom; ///< coarse segments identified in last four layers in YZ projection

  /// to minimize correlations between slope and offset, calculate all offsets with respect to z(center of TRD)
  Double_t fZref_full;
  /// reference z used for coarse segment finding on last four layers
  Double_t fZref_bottom;

  /// minimizer for track fits
  ROOT::Math::Minimizer* fMinimizer;

  TStopwatch fStopwatchSegmentFinding;
  TStopwatch fStopwatchTrackfindingHistoFilling;
  TStopwatch fStopwatchMaximaFinding;
  TStopwatch fStopwatchSegmentRemoving;
  TStopwatch fStopwatchForTesting;
  TStopwatch fStopwatchFit;
  TStopwatch fStopwatchMinos;
  TStopwatch fStopwatchMissingHits;

};

}

}

#endif // TRDTRACKING_HH
