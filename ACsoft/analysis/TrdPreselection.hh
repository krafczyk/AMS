#ifndef TRDPRESELECTION_HH
#define TRDPRESELECTION_HH

#include <vector>
#include <TVector2.h>
#include "AMSGeometry.h"


namespace Analysis {

class Particle;
class SplineTrack;

struct TRDCandidateHit {
  TRDCandidateHit(unsigned short _straw, float _pathLength, float _deDx = 0)
    : straw(_straw)
    , pathLength(_pathLength)
    , deDx(_deDx) {
      v_NeighborStraws.clear();
      ActiveStraw = -1;
      residual    = 0.0;
  }

  unsigned short straw;
  float pathLength;
  float deDx;
  float residual;
  std::vector<unsigned short> v_NeighborStraws;
  short ActiveStraw;
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
    * Note: This function returns false if the extrapolated track position is not within
    * the geometrical TRD acceptance.
    */ 
  bool Process( const Analysis::Particle& particle, float MinPathLength=AC::AMSGeometry::TRDTubeMinPathLength );

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

  //typedef std::vector<std::pair<unsigned short, float> > StrawNumberAndPathLengths;

  /** Returns all straws with the associated path lengths that were determined in ProcessTrackerTrack(). */
  std::vector<TRDCandidateHit> CandidateHits() const;

  /** Does the selected track track pass the TRD preselection cuts ? */
  bool PassesPreselectionCuts() const;

  /** Is the selected track track inside the TRD geometrical acceptance, i.e. passes through layer 0 and 19 ? */
  bool IsInsideTrdGeometricalAcceptance() const;

private:

  void ProcessWithLookupTable(const Analysis::Particle& particle, float MinPathLength);
  void ProcessWithoutLookupTable(const Analysis::Particle& particle, float MinPathLength);
  void BuildIsInsideSubLayerInformation();
  void StoreNeighboringStraws(std::vector<TRDCandidateHit> &CandidateHits, std::vector<unsigned short> &strawNumbers);

  const Analysis::SplineTrack* fSplineTrack; // we only borrow this from high-level event, do not delete!
  std::vector<TRDCandidateHit> fCandidateHitsPerSubLayer[AC::AMSGeometry::TRDSubLayers];
};

}

#endif // TRDPRESELECTION_HH
