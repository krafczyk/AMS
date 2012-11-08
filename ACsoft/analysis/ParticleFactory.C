#include "ParticleFactory.hh"

#include "Event.h"
#include "AnalysisParticle.hh"

#include "SplineTrack.hh"
#include "TrdHit.hh"
#include "TrackFactory.hh"
#include "TrdHitFactory.hh"

#define DEBUG 0
#define INFO_OUT_TAG "ParticleFactory> "
#include <debugging.hh>

/** Constructor.
  *
  * \param[in]  stepsForTrdHits Steps to be taken in production of TrdHits (see TrdHitFactory).
  */
Analysis::ParticleFactory::ParticleFactory( int stepsForTrdHits ) :
  fStepsForTrdHits(stepsForTrdHits)
{
  fTrackFactory  = new Analysis::TrackFactory;
  fTrdHitFactory = new Analysis::TrdHitFactory;
}


Analysis::ParticleFactory::~ParticleFactory()
{
  delete fTrackFactory;
  delete fTrdHitFactory;
}


/** Select default components for %particle: Take objects associated with first AMS %particle.
  *
  * This will only fill those parts of the event that are needed for a preselection.
  * You have to call FillHighLevelObjects() later.
  */
void Analysis::ParticleFactory::PrepareWithDefaultObjects( const AC::Event& rawEvt, Analysis::Particle& particle ) {

  // store pointer to raw event
  particle.fRawEvent = &rawEvt;

  DefaultMainAmsParticle( particle );
  DefaultMainTrackerTrack( particle );
  DefaultMainTrackerTrackFit( particle );
  DefaultEcalShower( particle );
  DefaultTofBeta( particle );
  DefaultMainTrdVTrack( particle );
}

bool Analysis::ParticleFactory::DefaultMainAmsParticle( Analysis::Particle& particle ) const {

  if( !particle.RawEvent()->Particles().size() ) return false;
  particle.fMainAmsParticle = &(particle.RawEvent()->Particles().at(0));
  return true;
}

bool Analysis::ParticleFactory::DefaultMainTrackerTrack ( Analysis::Particle& particle ) const {

  if( !particle.MainAmsParticle() ) return false;
  Short_t trackIndex = particle.MainAmsParticle()->TrackerTrackIndex();
  if (trackIndex<0 || trackIndex>=Short_t(particle.RawEvent()->Tracker().Tracks().size()))
    return false;

  particle.fMainTrackerTrack = &(particle.RawEvent()->Tracker().Tracks().at(trackIndex));
  return true;
}

bool Analysis::ParticleFactory::DefaultMainTrackerTrackFit ( Analysis::Particle& particle ) const {

  const AC::TrackerTrack* trk = particle.MainTrackerTrack();
  if( !trk ) return false;

  // preferred fit
  int iFit = trk->GetFit(1,0,5); // Fit[0]  Choutko algorithm (P3=1)  All Hits (P4=0) with MA ext alignment (P5=5):
  particle.fTrackerTrackFitID = 1;
  if(iFit<0){
    iFit = trk->GetFit(1,3,3);   // Fit[0]  Choutko algorithm (P3=1)  Inn Hits (P4=3) Refit with new alignment (P5=3)
    particle.fTrackerTrackFitID = 2;
  }
  if(iFit<0){
    iFit = trk->GetFit(1,3,1);   // Fit[0]  Choutko algorithm (P3=1)  Inn Hits (P4=3) default (P5=1)
    particle.fTrackerTrackFitID = 3;
  }
  if (iFit<0) {
    particle.fTrackerTrackFitID = -1;
    return false;
  }

  if( size_t(iFit) >= trk->TrackFits().size() ) return false;

  particle.fMainTrackerTrackFit = &(trk->TrackFits().at(iFit));
  return true;
}

bool Analysis::ParticleFactory::DefaultEcalShower ( Analysis::Particle& particle ) const {

  if( !particle.MainAmsParticle() ) return false;

  Short_t index = particle.MainAmsParticle()->ECALShowerIndex();

  if( index < 0 || size_t(index) >= particle.RawEvent()->ECAL().Showers().size() ) return false;

  particle.fMainEcalShower = &(particle.RawEvent()->ECAL().Showers().at(index));
  return true;
}

bool Analysis::ParticleFactory::DefaultTofBeta ( Analysis::Particle& particle ) const {

  if( !particle.MainAmsParticle() ) return false;

  Short_t index = particle.MainAmsParticle()->TOFBetaIndex();

  if( index < 0 || size_t(index) >= particle.RawEvent()->TOF().Betas().size() ) return false;

  particle.fTofBeta = &(particle.RawEvent()->TOF().Betas().at(index));
  return true;
}

bool Analysis::ParticleFactory::DefaultMainTrdVTrack ( Analysis::Particle& particle ) const {

  if( !particle.MainAmsParticle() ) return false;

  Short_t index = particle.MainAmsParticle()->TRDVTrackIndex();

  if( index < 0 || size_t(index) >= particle.RawEvent()->TRD().VTracks().size() ) return false;

  particle.fMainTrdVTrack = &(particle.RawEvent()->TRD().VTracks().at(index));
  return true;
}


/** Calculate the high-level objects in particle.
  *
  * You have to create an empty particle on the stack first. Either select components of the %particle
  * manually via the respective constructor, or call PrepareWithDefaultObjects() on the empty %particle.
  *
  * In this way, we minimize copying of complex objects.
  *
  * \param[out] particle Particle to be filled.
  *
  */
void Analysis::ParticleFactory::FillHighLevelObjects( Analysis::Particle& particle ) {

  if( !particle.IsEmpty() ){
    WARN_OUT << "Attempt to fill non-empty particle. Aborting!" << std::endl;
    return;
  }

  if( !particle.RawEvent() ){
    WARN_OUT << "No raw event has been set in particle. Aborting!" << std::endl;
    return;
  }

  // store SplineTrack
  if( particle.MainTrackerTrackFit() ){
    Analysis::SplineTrack* splineTrack = fTrackFactory->CreateSplineTrackFrom(*(particle.MainTrackerTrackFit()));
    particle.fSplineTrack = splineTrack;

    // store TrdHits
    for( unsigned int i=0 ; i<particle.RawEvent()->TRD().RawHits().size(); ++i ){

      const AC::TRDRawHit& rhit = particle.RawEvent()->TRD().RawHits()[i];

      Analysis::TrdHit hit = fTrdHitFactory->ProduceTrdHitFrom(rhit,*splineTrack,
                                                               particle.RawEvent()->EventHeader(),fStepsForTrdHits);
      particle.fTrdHits.push_back(hit);

      DEBUG_OUT << hit << std::endl;
    }
  }
}


