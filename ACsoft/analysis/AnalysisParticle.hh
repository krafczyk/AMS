#ifndef ANALYSIS_PARTICLE_HH
#define ANALYSIS_PARTICLE_HH

// This file is named AnalysisParticle.hh to avoid clashes with Particle.h from acroot/.
// When building ACsoft within AMS both Particle.C files would produce Particle.o
// objects - and linking would fail.

#include <vector>
#include <Rtypes.h>

namespace AC {
class Event;
class Particle;
class TrackerTrack;
class TrackerTrackFit;
class ECALShower;
class TOFBeta;
class TRDVTrack;
}

namespace Analysis {

class ParticleFactory;
class SplineTrack;
class TrdHit;

/** High-level analysis object.
  *
  * Filled by ParticleFactory.
  */
class Particle {

public:

  Particle();
  Particle( const AC::Event* rawevent, const AC::Particle* amsParticle, const AC::TrackerTrack* trackerTrack,
            const AC::TrackerTrackFit* trackerTrackFit, const AC::ECALShower* shower, const AC::TOFBeta* beta,
            const AC::TRDVTrack* trdvtrk );

  virtual ~Particle();

  const AC::Event* RawEvent() const { return fRawEvent; }

  const AC::Particle* MainAmsParticle() const { return fMainAmsParticle; }
  const AC::TrackerTrack* MainTrackerTrack() const { return fMainTrackerTrack; }
  const AC::TrackerTrackFit* MainTrackerTrackFit() const { return fMainTrackerTrackFit; }
  int TrackerTrackFitID() const { return fTrackerTrackFitID; }
  const AC::ECALShower* MainEcalShower() const { return fMainEcalShower; }
  const AC::TOFBeta* TofBeta() const { return fTofBeta; }
  const AC::TRDVTrack* MainTrdVTrack() const { return fMainTrdVTrack; }

  const std::vector<TrdHit>& TrdHits() const { return fTrdHits; }
  const SplineTrack* GetSplineTrack() const { return fSplineTrack; }

  void Dump() const;

  bool IsEmpty() const;


  Float_t TofBetaMeanEnergy() const;
  Float_t TofBetaMaxEnergy() const;


protected:

  friend class ParticleFactory;

  /// pointer to underlying raw ACQt event
  const AC::Event* fRawEvent;

  const AC::Particle* fMainAmsParticle;
  const AC::TrackerTrack* fMainTrackerTrack;
  const AC::TrackerTrackFit* fMainTrackerTrackFit;
  /// a number that can be used to identify which track fit was chosen as the main fit (used e.g. by EventWriter)
  int fTrackerTrackFitID;
  const AC::ECALShower* fMainEcalShower;
  const AC::TOFBeta* fTofBeta;
  const AC::TRDVTrack* fMainTrdVTrack;


  std::vector<TrdHit> fTrdHits; ///< vector of TrdHit objects, calculated from objects selected for this particle
  SplineTrack* fSplineTrack;    ///< spline track corresponding to \c fMainTrackerTrackFit

};
}

#endif
