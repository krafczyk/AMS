#ifndef TRDPRESELECTION_HH
#define TRDPRESELECTION_HH

#include <vector>
#include <TVector2.h>

#include "AMSGeometry.h"
#include "Settings.h"

namespace ACsoft {

namespace Analysis {

class Particle;
class SplineTrack;

struct TRDCandidateHit {
  TRDCandidateHit(unsigned short _straw, float _pathLength, float _deDx = 0)
    : straw(_straw)
    , pathLength(_pathLength)
    , deDx(_deDx) {
      v_NeighborStraws.clear();
      v_NeighborXY.clear();
      v_NeighborZ.clear();
      ActiveStraw        = -1;
      residual           = 0.0;
      xy                 = 0;
      z                  = 0;
      d                  = -1;
      Chi2               = -1;
      pathLengthTrdTrack = -1;
  }
  // FIXME We do not really need this structure, TrdHit can easily be expanded to do the same!

  unsigned short straw; // global straw number (0..5247)
  float pathLength;
  float pathLengthTrdTrack;
  float deDx;
  float residual;
  float xy, z;
  short d;
  float Chi2;
  std::vector<unsigned short> v_NeighborStraws;
  std::vector<float> v_NeighborXY;
  std::vector<float> v_NeighborZ;
  short ActiveStraw; ///< number of a straw with an actual energy deposition, matched to this candidate
};

struct TRDMissingHit {
  TRDMissingHit(float _x, float _y, float _z)
    : x(_x)
    , y(_y)
    , z(_z) {
    straw1 = -1;
    straw2 = -1;
    d      = -1;
    Chi2   = -1;
    sigma  = -1.;
  }
  float x, y, z;
  short straw1, straw2;
  short d;
  float Chi2;
  float sigma;
};

/** This class holds all data related to select "good" TRD particles for the use in alignment/calibration etc.
  */
class TrdPreselection {
public:
  TrdPreselection();
  ~TrdPreselection();

  /** Process a given particle and fill the
    * candidate straws vector. Calling PathLengthInGasVolumes()/CandidateLayers()/etc.. after
    * this will return the information from the last processed particle.
    *
    * Note: This function returns true if the extrapolated track position is within
    * the geometrical TRD acceptance.
    */
  bool Process( const Analysis::Particle& particle, bool AddNearTrackHitsToCandidateList, bool ExcludeDeadStraws );

  /** Returns the sum of path length in all layers for a given track fit. */
  float GetCandidatePathLength() const;

  /** Returns the number of candidate layers for a given track fit.
    * In a candidate layer, the track passes through the gas volume of the straws.
    */
  unsigned short GetNumberOfCandidateLayers() const;

  /** Returns the number of candidate straws for a given track fit.
    */
  unsigned short GetNumberOfCandidateStraws() const;

  /** Returns the coordinate extrapolated from the tracker track into a given TRD layer.
    */
  TVector2 PointInLayer(unsigned short layer) const;

  /** Determines whether the given given track is inside the layer.
    * Could be either inside gas volume or inside carbon fiber stringers.
    */
  bool IsInsideSubLayer(unsigned short sublayer) const;

  /** Returns all straws with the associated path lengths that were determined in ProcessTrackerTrack(). */
  void GetCandidateHits(std::vector<Analysis::TRDCandidateHit>& result) const;

  /** Does the selected track track pass the TRD preselection cuts ? */
  bool PassesPreselectionCuts() const;

  /** Is the selected track track inside the TRD geometrical acceptance, i.e. passes through layer 0 and 19 ? */
  bool IsInsideTrdGeometricalAcceptance() const;

private:
  void ProcessWithLookupTable(const Analysis::Particle&, bool AddNearTrackHitsToCandidateList, bool ExcludeDeadStraws);
  void BuildIsInsideSubLayerInformation();
  void StoreNeighboringStraws(std::vector<TRDCandidateHit>& CandidateHits, const std::vector<unsigned short>& strawNumbers);

  const Analysis::SplineTrack* fSplineTrack; // we only borrow this from high-level event, do not delete!

  std::vector<TRDCandidateHit> fCandidateHitsPerSubLayer[AC::AMSGeometry::TRDSubLayers];

  std::vector<unsigned short> strawNumbers;
  std::vector<TRDCandidateHit> CandidateHitsPerSubLayerOnTrack[AC::AMSGeometry::TRDSubLayers];
  std::vector<TRDCandidateHit> CandidateHitsPerSubLayerNearTrack[AC::AMSGeometry::TRDSubLayers];
  std::vector<TRDCandidateHit> CandidateHitsOnTrack;
  std::vector<TRDCandidateHit> CandidateHitsNearTrack;
};

}

}

#endif // TRDPRESELECTION_HH
