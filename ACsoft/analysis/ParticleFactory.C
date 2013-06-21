#include "ParticleFactory.hh"

#include "Event.h"
#include "ReducedEvent.hh"
#include "AnalysisParticle.hh"
#include "Settings.h"
#include "SplineTrack.hh"
#include "TrdHit.hh"
#include "TrackFactory.hh"
#include "TrdHitFactory.hh"
#include "TrdTracking.hh"
#include "TrdTrack.hh"

#define DEBUG 0
#define INFO_OUT_TAG "ParticleFactory> "
#include <debugging.hh>

/** Constructor.
  *
  * The default production step is only ACsoft::Analysis::CreateSplineTrack. If you want to access TrdHits you have to enable additional steps,
  * using the SetParticleFactoryProductionSteps() function before the event loop.
  *
  * \param[mcScaler]  Optional. Pass in a McSpectrumScaler, if you plan to use McWeight()/etc.. methods on MC data.
  */
ACsoft::Analysis::ParticleFactory::ParticleFactory( ACsoft::Utilities::McSpectrumScaler* mcScaler ) :
  fTrackFitOverridden(false),
  fAlgorithm(0),
  fPattern(0),
  fRefit(0),
  fMcScaler(mcScaler)
{
  fProductionSteps = ACsoft::Analysis::CreateSplineTrack;

  fTrackFactory  = new ACsoft::Analysis::TrackFactory;
  fTrdHitFactory = new ACsoft::Analysis::TrdHitFactory;
  fTrdTracking   = new ACsoft::Analysis::TrdTracking(0,false);
}


ACsoft::Analysis::ParticleFactory::~ParticleFactory()
{
  delete fTrackFactory;
  delete fTrdHitFactory;
  delete fTrdTracking;
}

/** Return a dummy particle that can be used to check RTI cuts & bad run cut selectors without an underlying event. */
void ACsoft::Analysis::ParticleFactory::PrepareDummyParticle( const TTimeStamp& overrideTimeStamp, ACsoft::Analysis::Particle& particle ) {

  particle.fOverrideTimeStamp = overrideTimeStamp;
}


/** Select default components for %particle: Take objects associated with first AMS %particle.
  * You have to pass either an ACsoft::AC::Event or IO::ReducedEvent object.
  *
  * This will only fill those parts of the event that are needed for a preselection.
  * You have to call FillHighLevelObjects() later.
  */
void ACsoft::Analysis::ParticleFactory::PrepareWithDefaultObjects( const ACsoft::AC::Event* event, const IO::ReducedEvent* reducedEvent, ACsoft::Analysis::Particle& particle ) {

  assert(event || reducedEvent);
  assert(!(event && reducedEvent));

  particle.Clear();

  // store pointer to raw event
  particle.fRawEvent = event;
  particle.fReducedEvent = reducedEvent;

  particle.fMcScaler = fMcScaler;

  DefaultAmsParticle( particle );
  DefaultTrackerTrack( particle );
  DefaultTrackerTrackFit( particle );
  DefaultEcalShower( particle );
  DefaultTofBeta( particle );
  DefaultTrdVTrack( particle );
  DefaultRichRing( particle );
}


/** Select default components for %particle: Take objects associated with first AMS %particle.
  *
  * This will only fill those parts of the event that are needed for a preselection.
  * You have to call FillHighLevelObjects() later.
  */
void ACsoft::Analysis::ParticleFactory::PrepareWithDefaultObjects( const ACsoft::AC::Event* event, ACsoft::Analysis::Particle& particle ) {

  PrepareWithDefaultObjects( event, 0, particle );
}

/** Select default components for %particle: Take objects associated with first AMS %particle.
  *
  * This will only fill those parts of the event that are needed for a preselection.
  * You have to call FillHighLevelObjects() later.
  */
void ACsoft::Analysis::ParticleFactory::PrepareWithDefaultObjects( const IO::ReducedEvent* event, ACsoft::Analysis::Particle& particle ) {

  PrepareWithDefaultObjects( 0, event, particle );
}


bool ACsoft::Analysis::ParticleFactory::DefaultAmsParticle( ACsoft::Analysis::Particle& particle ) const {

  if( !particle.RawEvent()) return false;
  if( !particle.RawEvent()->Particles().size() ) return false;
  particle.fAmsParticle = &(particle.RawEvent()->Particles().at(0));
  return true;
}

bool ACsoft::Analysis::ParticleFactory::DefaultTrackerTrack ( ACsoft::Analysis::Particle& particle ) const {

  if( !particle.AmsParticle() ) return false;
  Short_t trackIndex = particle.AmsParticle()->TrackerTrackIndex();
  if (trackIndex<0 || trackIndex>=Short_t(particle.RawEvent()->Tracker().Tracks().size()))
    return false;

  particle.fTrackerTrack = &(particle.RawEvent()->Tracker().Tracks().at(trackIndex));
  return true;
}

bool ACsoft::Analysis::ParticleFactory::DefaultTrackerTrackFit ( ACsoft::Analysis::Particle& particle ) const {

  const ACsoft::AC::TrackerTrack* trk = particle.TrackerTrack();
  if( !trk ) return false;

  // Synchronize any change here with ACQtProducer-Tracker.C / AnalysisParticle / ParticleFactory.
  int forceRefitPG = ::AC::CurrentACQtVersion() >= 57 ? 3 : 2;
  int forceRefitMA = forceRefitPG + 10;

  // Check if default TrackFit has been overridden and use that fit.
  int iFit = -1;
  if (fTrackFitOverridden) {
    iFit = trk->GetFit(fAlgorithm,fPattern,fRefit);
    particle.fTrackerTrackFitID = 5;
  }

  // Try default fit, 1|0|2 (all hits, choutko algorithm, with refit, perugia alignment)
  if (iFit < 0) {
    iFit = trk->GetFit(1, 0, forceRefitPG);
    particle.fTrackerTrackFitID = 0;
  }
  // Try with madria alignment, if perugia alignment is not available.
  if (iFit < 0) {
    iFit = trk->GetFit(1, 0, forceRefitMA);
    particle.fTrackerTrackFitID = 1;
  }

  // If full-span fits are not available, try requesting 1|3|2 (inner hits, choutko algorithm, with refit, perugia alignment)
  if (iFit < 0) {
    iFit = trk->GetFit(1, 3, forceRefitPG);
    particle.fTrackerTrackFitID = 2;
  }

  // Try with madria alignment, if perugia alignment is not available.
  if (iFit < 0) {
    iFit = trk->GetFit(1, 3, forceRefitMA);
    particle.fTrackerTrackFitID = 3;
  }

  // If everything fails, try 0|0|0 (default track fit from AMS ROOT file) as last resort.
  if (iFit < 0) {
    iFit = trk->GetFit(0, 0, 0);
    particle.fTrackerTrackFitID = 4;
  }

  if (iFit < 0) {
    particle.fTrackerTrackFitID = -1;
    return false;
  }

  if( size_t(iFit) >= trk->TrackFits().size() ) return false;

  particle.fTrackerTrackFit = &(trk->TrackFits().at(iFit));
  return true;
}

bool ACsoft::Analysis::ParticleFactory::DefaultEcalShower ( ACsoft::Analysis::Particle& particle ) const {

  if( !particle.AmsParticle() ) return false;

  Short_t index = particle.AmsParticle()->ECALShowerIndex();

  if( index < 0 || size_t(index) >= particle.RawEvent()->ECAL().Showers().size() ) return false;

  particle.fEcalShower = &(particle.RawEvent()->ECAL().Showers().at(index));
  return true;
}

bool ACsoft::Analysis::ParticleFactory::DefaultTofBeta ( ACsoft::Analysis::Particle& particle ) const {

  if( !particle.AmsParticle() ) return false;

  Short_t index = particle.AmsParticle()->TOFBetaIndex();

  if( index < 0 || size_t(index) >= particle.RawEvent()->TOF().Betas().size() ) return false;

  particle.fTofBeta = &(particle.RawEvent()->TOF().Betas().at(index));
  return true;
}

bool ACsoft::Analysis::ParticleFactory::DefaultTrdVTrack ( ACsoft::Analysis::Particle& particle ) const {

  if( !particle.AmsParticle() ) return false;

  Short_t index = particle.AmsParticle()->TRDVTrackIndex();

  if( index < 0 || size_t(index) >= particle.RawEvent()->TRD().VTracks().size() ) return false;

  particle.fTrdVTrack = &(particle.RawEvent()->TRD().VTracks().at(index));
  return true;
}

bool ACsoft::Analysis::ParticleFactory::DefaultRichRing ( ACsoft::Analysis::Particle& particle ) const {

  if( !particle.AmsParticle() ) return false;

  Short_t index = particle.AmsParticle()->RICHRingIndex();

  if( index < 0 || size_t(index) >= particle.RawEvent()->RICH().Rings().size() ) return false;

  particle.fRichRing = &(particle.RawEvent()->RICH().Rings().at(index));
  return true;
}

/** Calculate the high-level objects in particle.
  *
  * You have to create an empty particle on the stack first. Either select components of the %particle
  * manually via the respective constructor, or call PrepareWithDefaultObjects() on the empty %particle.
  *
  * In this way, we minimize copying of complex objects.
  *
  * The behaviour of this function, concerning the types of high-level objects created,
  * is governed by the ParticleFactoryProductionSteps set for this ParticleFactory. They can be changed by the
  * SetParticleFactoryProductionSteps() function.
  *
  * \param[out] particle Particle to be filled.
  *
  */
void ACsoft::Analysis::ParticleFactory::FillHighLevelObjects( ACsoft::Analysis::Particle& particle ) {

  particle.fTrdTrack = 0;

  if( !particle.IsEmpty() ){
    WARN_OUT << "Attempt to fill non-empty particle. Aborting!" << std::endl;
    return;
  }

  if( !particle.HasRawEventData() )
    return;

  if( !particle.RawEvent() ){
    WARN_OUT << "No raw event has been set in particle. Aborting!" << std::endl;
    return;
  }

  if( !particle.TrackerTrackFit() )
    return;

  if( TestOption(ACsoft::Analysis::CreateSplineTrack) ) {
    // store SplineTrack
    ACsoft::Analysis::SplineTrack* splineTrack = fTrackFactory->CreateSplineTrackFrom(*(particle.TrackerTrackFit()));
    particle.fSplineTrack = splineTrack;

    int stepsForTrdHits = ACsoft::Analysis::TrackInfo | ACsoft::Analysis::GainCorrection;

    if( TestOption(ACsoft::Analysis::TrdHitsFromSplineTrack) ) {
      // store TrdHits based on tracker track with non-zero pathlength
      unsigned int nTrdHits = particle.RawEvent()->TRD().RawHits().size();
      particle.fTrdHitsFromTrackerTrack.clear();
      particle.fTrdHitsFromTrackerTrack.reserve(nTrdHits);
      for( unsigned int i=0 ; i<nTrdHits; ++i ){

        const ACsoft::AC::TRDRawHit& rhit = particle.RawEvent()->TRD().RawHits()[i];
        ACsoft::Analysis::TrdHit hit = fTrdHitFactory->ProduceTrdHitFrom(rhit, *splineTrack, stepsForTrdHits);
        if( hit.GetDeDx() > 0. && hit.Pathlength3D() > 0. )
          particle.fTrdHitsFromTrackerTrack.push_back(hit);
      }
    }

    // create TrdTrack as well? only possible if tracker spline track is present
    if( TestOption(ACsoft::Analysis::CreateTrdTrack ) ){

      fTrdTracking->FindTrdSegments(*(particle.RawEvent()));
      std::vector<ACsoft::Analysis::TrdTrack> trdTracks;
      fTrdTracking->CreateTrdTracksFromSegments_SplineTrack(splineTrack,trdTracks,*(particle.RawEvent()));
      if( trdTracks.size() > 0 ){

        const ACsoft::Analysis::TrdTrack& trdTrack = trdTracks.front();
        particle.fTrdTrack = new ACsoft::Analysis::TrdTrack(trdTrack);

        if( TestOption(ACsoft::Analysis::TrdHitsFromTrdTrack) ) {

          // TRD hits based on TRD track
          std::vector<ACsoft::Analysis::TrdHit>& trdTrackHits = particle.fTrdHitsFromTrdTrack;

          trdTrackHits.clear();
          trdTrackHits.reserve(trdTrack.ActualHitsOnTrackXZ().size()+trdTrack.ActualHitsOnTrackYZ().size());

          for( unsigned int i=0 ; i<trdTrack.ActualHitsOnTrackXZ().size() ; ++i ){
            const ACsoft::AC::TRDRawHit* rhit = trdTrack.ActualHitsOnTrackXZ().at(i);
            trdTrackHits.push_back(fTrdHitFactory->ProduceTrdHitFrom(*rhit,trdTrack,stepsForTrdHits));
          }
          for( unsigned int i=0 ; i<trdTrack.ActualHitsOnTrackYZ().size() ; ++i ){
            const ACsoft::AC::TRDRawHit* rhit = trdTrack.ActualHitsOnTrackYZ().at(i);
            trdTrackHits.push_back(fTrdHitFactory->ProduceTrdHitFrom(*rhit,trdTrack,stepsForTrdHits));
          }
        }
      }
    }
  }
}

/** Override default TrackFit used when filling particle
 * If specified TrackFit is available, it will be used and Particle::TrackerTrackFitID() will return 5.
 * If specified TrackFit is not available, default fits are tried resulting in TrackerTrackFitID of 0-4 as specified in source code of this class.
 *
 * This function should be called before the first call to PrepareWithDefaultObjects
 *
 * \param[in] algorithm TrackFit algorithm to be used
 * \param[in] pattern TrackFit pattern to be used
 * \param[in] refit TrackFit refit parameter to be used
 */
void ACsoft::Analysis::ParticleFactory::OverrideDefaultTrackFit(int algorithm, int pattern, int refit) {
  fTrackFitOverridden = true;
  fAlgorithm = algorithm;
  fPattern = pattern;
  fRefit = refit;
}
