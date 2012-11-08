#ifndef TRDCANDIDATEMATCHING_HH
#define TRDCANDIDATEMATCHING_HH

#include <vector>
#include "Settings.h"
#include "TrdPreselection.hh"
#include "TrdHit.hh"

namespace Analysis {

class Particle;

/** This class holds all data related to match real TRD events with the expected candidates.
  */
class TrdCandidateMatching {
public:
  TrdCandidateMatching();
  ~TrdCandidateMatching();

  /** Process a given event.
    */ 
  bool Process(const Analysis::Particle& particle, float minPathLength, bool useActiveStraws);

  /** Returns all straws with the associated path lengths that were determined in ProcessTrackerTrack(). */
  const std::vector<TRDCandidateHit>& CandidateHits() const;

  /** Returns all assigned TRD hits within the region of interest, i.e. distance to tracker track < 1.5 cm with gain calibration and alignment corrections applied. */
  const std::vector<TrdHit>& AssignedHits() const { return fAssignedHits; }

  /** Returns all unassigned TRD hits within the region of interest, i.e. distance to tracker track < 1.5 cm with gain calibration and alignment corrections applied. */
  const std::vector<TrdHit>& UnassignedHits() const { return fUnassignedHits; }

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

private:
  TrdPreselection              fPreselection;

  std::vector<TRDCandidateHit> fCandidateHits;
  std::vector<TrdHit>          fUnassignedHits;
  std::vector<TrdHit>          fAssignedHits;
  unsigned short               fNumberOfMatchedHits;
  float                        fTrdAdcScaleFactor;

  void DetermineActiveHitsPerLayer(const Analysis::Particle& particle );
  void DetermineCandidateHitsPerLayer();
  void MatchActiveAndCandidateHits(const Analysis::Particle& particle, bool ActiveStraws);
  void PrintMatching(const Analysis::Particle& particle, std::vector<bool> matched[]);

  std::vector< std::vector<Analysis::TrdHit> > fActiveHitsPerLayer;
  std::vector< std::vector<TRDCandidateHit> >  fCandidateHitsPerLayer;

};

}

#endif // TRDCANDIDATEMATCHING_HH
