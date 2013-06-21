#ifndef TRDTRACK_HH
#define TRDTRACK_HH

#include <TMath.h>
#include <vector>

#include "StraightLineTrack.hh"

namespace ACsoft {

namespace AC {
class Event;
class TRDRawHit;
}

namespace Analysis {

class TrdSegment;
class TrdTracking;

/** %TRD track.
  *
  *
  * Contains the results of the following steps:
  *  - segments identified in the track finding
  *  - best-fit parameters from likelihood fit
  *  - actual hits on track (defined as having non-zero pathlength) according to best-fit line parameters
  *
  */
class TrdTrack : public StraightLineTrack
{

public:

  TrdTrack( TrdSegment* xzSegment, TrdSegment* yzSegment );
  virtual ~TrdTrack();

  const std::vector<const AC::TRDRawHit*>& ActualHitsOnTrackXZ() const { return fActualHitsOnTrackXZ; }
  const std::vector<const AC::TRDRawHit*>& ActualHitsOnTrackYZ() const { return fActualHitsOnTrackYZ; }

  const TrdSegment* SegmentXZ() const { return fSegmentXZ; }
  const TrdSegment* SegmentYZ() const { return fSegmentYZ; }


  double SigmaOffsetXZ() const { return fSigmaOffsetXZ; }
  double SigmaOffsetYZ() const { return fSigmaOffsetYZ; }
  double SigmaSlopeXZ() const { return fSigmaSlopeXZ; }
  double SigmaSlopeYZ() const { return fSigmaSlopeYZ; }

  /** Uncertainty on angle to z-axis in XZ projection, from error propagation on uncertainty on \c fSlopeXZ from fit */
  double SigmaProjAngleX() const { return SigmaSlopeXZ() / (1.0 + SlopeXZ()*SlopeXZ()); }
  /** Uncertainty on angle to z-axis in YZ projection, from error propagation on uncertainty on \c fSlopeYZ from fit */
  double SigmaProjAngleY() const { return SigmaSlopeYZ() / (1.0 + SlopeYZ()*SlopeYZ()); }

  std::vector<int> GetHitPattern() const;
  unsigned int NumberOfHitsOnTrack() const { return fActualHitsOnTrackXZ.size() + fActualHitsOnTrackYZ.size(); }
  unsigned int NumberOfLayersWithHit() const;
  unsigned int NumberOfSublayersWithHit() const;

  int FirstLayerNumberFromTop() const;
  int LastLayerNumberFromBottom() const;

  virtual void Dump() const;
  void DumpIndividualHits() const;

  double MinimizationValue() const { return fMinimizationValue; }
  double MinimizationNumberOfCalls() const { return fMinimizationNumberOfCalls; }

  double MinusLogL_XZ() const { return fMinusLogL_XZ; }
  double MinusLogL_YZ() const { return fMinusLogL_YZ; }

  bool HasActualHitInStraw( Short_t globalStrawNumber ) const;
  bool HasActualHitInSublayer( Short_t globalSublayerNumber ) const;

  int NumberOfHitsWithinOneTubeDiameter() const { return fNumberOfHitsWithinOneTubeDiameter; }
  int NumberOfHitsWithinTwoTubeDiameters() const { return fNumberOfHitsWithinTwoTubeDiameters; }
  int NumberOfHitsNearTrackOnMissingSublayers() const { return fNumberOfHitsNearTrackOnMissingSublayers; }

private:

  unsigned int FillHitInformation( const AC::Event& event );

  friend class TrdTracking;

private:

  TrdSegment* fSegmentXZ; ///< XZ segment identified in track finding
  TrdSegment* fSegmentYZ; ///< YZ segment identified in track finding


  double fSigmaSlopeXZ;  ///< uncertainty on dx/dz from fit
  double fSigmaOffsetXZ; ///< uncertainty on x offset uncertainty on from fit
  double fSigmaSlopeYZ;  ///< uncertainty on dy/dz from fit
  double fSigmaOffsetYZ; ///< uncertainty on y offset from fit

  double fMinosSigmaSlopeXZ;  ///< uncertainty on dx/dz from MINOS
  double fMinosSigmaOffsetXZ; ///< uncertainty on x offset uncertainty on from MINOS
  double fMinosSigmaSlopeYZ;  ///< uncertainty on dy/dz from MINOS
  double fMinosSigmaOffsetYZ; ///< uncertainty on y offset from MINOS

  double fMinimizationValue;      ///< -log(L) value at minimum from fit
  unsigned int fMinimizationNumberOfCalls; ///< number of calls in minimization procedure

  double fMinusLogL_XZ; ///< contribution to -log(L) from XZ hits and missing hits only
  double fMinusLogL_YZ; ///< contribution to -log(L) from YZ hits and missing hits only

  /// pointers to the TRD raw hits with non-zero pathlength according to best-fit line parameters (XZ direction)
  std::vector<const AC::TRDRawHit*> fActualHitsOnTrackXZ;
  /// pointers to the TRD raw hits with non-zero pathlength according to best-fit line parameters (YZ direction)
  std::vector<const AC::TRDRawHit*> fActualHitsOnTrackYZ;

   /// number of %TRD hits that are not on this track but with straw positions closer than one %TRD tube diameter to the track
  int fNumberOfHitsWithinOneTubeDiameter;
  /// number of %TRD hits that are not on this track but with straw positions closer than two %TRD tube diameters to the track
  int fNumberOfHitsWithinTwoTubeDiameters;
  /// number of %TRD hits within 1.5 %TRD tube diameters of the track and on sublayers where no on-track hit is present
  int fNumberOfHitsNearTrackOnMissingSublayers;

};

}

}

#endif // TRDTRACK_HH
