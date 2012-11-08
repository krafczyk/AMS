#include "AnalysisParticle.hh"

#include "Event.h"

#include "TrdHit.hh"
#include "SplineTrack.hh"

#include <iostream>

Analysis::Particle::Particle() :
  fRawEvent(0),
  fMainAmsParticle(0),
  fMainTrackerTrack(0),
  fMainTrackerTrackFit(0),
  fTrackerTrackFitID(-1),
  fMainEcalShower(0),
  fTofBeta(0),
  fMainTrdVTrack(0),
  fTrdHits(),
  fSplineTrack(0)
{
  fTrdHits.reserve(40);
}

/** Constructor to manually choose the components that make up the particle.
  *
  * Set some of the objects to null if you do not want to associate any object with this particle.
  *
  * You can later call FillHighLevelObjects() in a suitable ParticleFactory to fill the high-level stuff (TrdHits, SplineTrack).
  *
  * \attention No one keeps you from choosing a certain TrackerTrack and a TrackerTrackFit that belongs to a different track.
  *
  */
Analysis::Particle::Particle( const AC::Event* rawevent, const AC::Particle* amsParticle, const AC::TrackerTrack* trackerTrack,
                              const AC::TrackerTrackFit* trackerTrackFit, const AC::ECALShower* shower, const AC::TOFBeta* beta,
                              const AC::TRDVTrack* trdvtrk ) :
  fRawEvent(rawevent),
  fMainAmsParticle(amsParticle),
  fMainTrackerTrack(trackerTrack),
  fMainTrackerTrackFit(trackerTrackFit),
  fTrackerTrackFitID(-1),
  fMainEcalShower(shower),
  fTofBeta(beta),
  fMainTrdVTrack(trdvtrk),
  fTrdHits(),
  fSplineTrack(0)
{
  fTrdHits.reserve(40);
}

Analysis::Particle::~Particle() {
  if( fSplineTrack )
    delete fSplineTrack;
}

bool Analysis::Particle::IsEmpty() const {

  return( fTrdHits.empty() && !fSplineTrack);
}

void Analysis::Particle::Dump() const {

  std::cout << "[Event]\n";
  if(fRawEvent)
    std::cout << "    (run " << fRawEvent->RunHeader()->Run() << " event " << fRawEvent->EventHeader().Event() << ")" << std::endl;
  else
    std::cout << "    <void>" << std::endl;

  std::cout << "  [TrdHits]" << std::endl;
  for( unsigned int i=0 ; i<fTrdHits.size() ; ++i )
    std::cout << fTrdHits[i] << std::endl;

  if( fSplineTrack ){
    std::cout << "  [SplineTrack]" << std::endl;
    fSplineTrack->Dump();
  }

  std::cout << std::endl;
}


/** Mean energy of clusters contained in TOFBeta object. */
Float_t Analysis::Particle::TofBetaMeanEnergy() const {

  assert(fRawEvent);
  if( !fTofBeta ) return 0.0;

  const AC::TOF& tof = fRawEvent->TOF();
  return tof.CalculateMeanEnergy(*fTofBeta);
}

/** Highest energy of clusters contained in TOFBeta object. */
Float_t Analysis::Particle::TofBetaMaxEnergy() const {

  assert(fRawEvent);
  if( !fTofBeta ) return 0.0;

  const AC::TOF& tof = fRawEvent->TOF();
  return tof.CalculateMaximumEnergy(*fTofBeta);
}
