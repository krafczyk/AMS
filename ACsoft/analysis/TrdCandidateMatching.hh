#ifndef TRDCANDIDATEMATCHING_HH
#define TRDCANDIDATEMATCHING_HH

#include <vector>
#include "Settings.h"
#include "TrdPreselection.hh"
#include "TrdHit.hh"

namespace ACsoft {

namespace Analysis {

class Particle;

struct TrdTrackPoint {
  TrdTrackPoint(short _id, float _xy, float _z, float _sigmaXY, short _layer=-1, short _iHit=-1)
    : id(_id)
    , xy(_xy)
    , z(_z)
    , sigmaXY(_sigmaXY)
    , layer(_layer)
    , iHit(_iHit) {
    Chi2  = -1;
    Resid =  0;
  }
  // id=0: point from tracker fit at z from upper ToF and Tracker Plane 1
  // id=1: point from candidate hit
  // id=2: point from missing hit
  short id;
  float xy, z, sigmaXY;
  short layer, iHit;
  float Chi2;
  float Resid;
};

struct TrackFitResult {
  TrackFitResult(double _Chi2, int _Ndof)
    : Chi2(_Chi2)
    , Ndof(_Ndof) {
    Position(0);
    Direction(0);
  }
  double Chi2;
  int Ndof;
  TVector3 Position, Direction;
};

/** This class holds all data related to match real TRD events with the expected candidates.
  */
class TrdCandidateMatching {
public:
  TrdCandidateMatching();
  ~TrdCandidateMatching();

  /** Process a given particle.
    */ 
  bool Process(const Analysis::Particle& particle, bool AddNearTrackHits);

  /** Returns all straws with the associated path lengths that were determined in ProcessTrackerTrack(). */
  const std::vector<TRDCandidateHit>& CandidateHits() const;

  /** Returns all assigned TRD hits within the region of interest, i.e. distance to tracker track < 1.5 cm with gain calibration and alignment corrections applied. */
  const std::vector<TrdHit>& AssignedHits() const { return fAssignedHits; }

  /** Returns all unassigned TRD hits within the region of interest, i.e. distance to tracker track < 1.5 cm with gain calibration and alignment corrections applied. */
  const std::vector<TrdHit>& UnassignedHits() const { return fUnassignedHits; }

  /** Returns the number of unassigned hits. */
  unsigned int NumberOfUnassignedHits() const { return fUnassignedHits.size(); }

  /** Does the track track pass the preselection cuts? */
  bool PassesTrdPreselectionCuts() const {return fPreselection.PassesPreselectionCuts();}

  /** Is tracker track suitable for Trd particle identification? */
  bool UsefulForTrdParticleId() const;

  /** Number of matched hits */
  unsigned short MatchedHits() const { return fNumberOfMatchedHits; }

  /** Access the preselection object */
  const TrdPreselection& Preselection() const { return fPreselection; }

  /** Returns the number of active layers, those candidate layers with straws that have an ADC count greater than 0.*/
  unsigned short GetNumberOfActiveLayers() const;

  /** Returns the total path length in the TRD in cm for those candidate straws with an ADC value greater than 0. */
  float GetActivePathLength() const;

  /** Returns the number of active straws, those candidate straws with an ADC value greater than 0.*/
  unsigned short GetNumberOfActiveStraws() const;

  /** Returns the result from the TRD internal track fit combined withe the seed tracker track in the XZ-Plane for direction=0 and in the YZ-Plane for direction=1*/
  const TrackFitResult& GetTrdTrackCombinedFit(unsigned int direction) const;

  /** Returns the result from the TRD internal track fit in the XZ-Plane for direction=0 and in the YZ-Plane for direction=1*/
  const TrackFitResult& GetTrdTrackStandAloneFit(unsigned int direction) const;

  /** Returns the result from the fit of the spline tracker track with a straight line XZ-Plane for direction=0 and in the YZ-Plane for direction=1*/
  const TrackFitResult& GetTrkTrackFit(unsigned int direction) const;

  /** Returns the angle between the TRD stand alone track and the tracker track at the upper ToF in the XZ-Plane for direction=0 and in the YZ-Plane for direction=1*/
  float GetTrdTrkAngle(unsigned int direction) const;

private:
  void DetermineActiveHitsPerLayer(const Analysis::Particle&);
  void DetermineCandidateHitsPerLayer();
  void MatchActiveAndCandidateHits(const Analysis::Particle&, bool AddNearTrackHits);
  void TrackerPointInPlane1(const Analysis::Particle&);

  // called from MatchActiveAndCandidateHits
  void PrintMatching(const Analysis::Particle&);
  void LinearRegression(const Analysis::Particle&, int& Reason, std::vector<TrdTrackPoint>& TrdTrackPointsXZ, std::vector<TrdTrackPoint>& TrdTrackPointsYZ);
  void CalculateDeDx(bool AddNearTrackHits, int iFlag);

  // called from LinearRegression
  void FitTrdTrackWithStraightLine(unsigned int direction, std::vector<TrdTrackPoint>& FitPoints, unsigned int first, unsigned int last, TrackFitResult& Result);
  void FitTrdTrackCombinedWithSeedTrack(std::vector<TrdTrackPoint>& TrdTrackPointsXZ, std::vector<TrdTrackPoint>& TrdTrackPointsYZ);
  void FitTrdTrackStandAlone(std::vector<TrdTrackPoint>& TrdTrackPointsXZ, std::vector<TrdTrackPoint>& TrdTrackPointsYZ);

  void BuildTrdTrackPoints(const Analysis::Particle&, std::vector<TrdTrackPoint>& TrdTrackPointsXZ, std::vector<TrdTrackPoint>& TrdTrackPointsYZ);
  void testSplineTracksDeviationFromStraighLine(const Analysis::Particle&);
  bool RemoveActiveHits();
  bool AddActiveHits();
  bool GenerateMissingHits(const Analysis::Particle&);
  void RecalculatePathLength(unsigned int direction);

  TrdPreselection              fPreselection;

  std::vector<TRDCandidateHit> fCandidateHits;
  std::vector<TrdHit>          fUnassignedHits;
  std::vector<TrdHit>          fAssignedHits;
  unsigned short               fNumberOfMatchedHits;
  float                        fTrdAdcScaleFactor;

  float                        fMeanPathLength;

  float                        fXhitLayer1, fYhitLayer1;
  float                        fXfitLayer1, fYfitLayer1;
  bool                         fHitLayer1;

  TrackFitResult               fTrdStandAloneFitXZ, fTrdCombinedFitXZ, fTrkFitXZ;
  TrackFitResult               fTrdStandAloneFitYZ, fTrdCombinedFitYZ, fTrkFitYZ;

  std::vector< std::vector<Analysis::TrdHit> > fActiveHitsPerLayer;
  std::vector< std::vector<TRDCandidateHit> >  fCandidateHitsPerLayer;
  std::vector< std::vector<TRDMissingHit> >    fMissingHitsPerLayer;

  // Chi2 of the active hits with respect to the TRD track fit
  std::vector<float> fActiveHitsChi2[AC::AMSGeometry::TRDLayers];

  // book keeping; if an active hit has been matched to a candidate hit set to true
  std::vector<bool>  fActiveHitsMatched[AC::AMSGeometry::TRDLayers];

};

}

}

#endif // TRDCANDIDATEMATCHING_HH
