#ifndef SPLINETRACK_HH
#define SPLINETRACK_HH

#include <TSpline.h>
#include <TVector3.h>

#include <vector>

namespace Analysis {

class TrackFactory;

/// Source for track information.
enum TrackSource { none = 1<<0, InnerTracker = 1<<1, TrackerLayer1 = 1<<2, TrackerLayer9 = 1<<3, Trd = 1<<4 };

/** A generic track object that is interpolatable by a spline technique.
  *
  */
class SplineTrack
{
public:

  SplineTrack();

  TVector3 InterpolateToZ( Double_t z ) const;

  void CalculateLocalPositionAndDirection( Double_t z, TVector3& pos, TVector3& dir ) const;
  float PathLength3D(int D, const TVector3& position) const;

  Double_t TrackResidual( int D, Double_t Z, Double_t XY ) const;
  Double_t DeltaXY( int D, Double_t Z, Double_t XY ) const;

  bool SourceContains( TrackSource test ) const { return ((fSource & test) == test); }

  void Dump() const;

protected:

  friend class TrackFactory;

  // include TRKtrk and TRKfit stuff

  TrackSource fSource; ///< where does the track information originate?

  std::vector<TVector3> fPoints; ///< "spline knots" = coordinate points of track (cm)

  TSpline3 fSplineZX; ///< spline for x-z interpolation
  TSpline3 fSplineZY; ///< spline for y-z interpolation

  Float_t fRigidity; ///< rigidity that was determined for the track (GeV)
  Float_t fRigidityUncertainty; ///< error on rigidity (GeV)

};
}

#endif // SPLINETRACK_HH
