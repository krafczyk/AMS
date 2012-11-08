#ifndef PARTICLEFACTORY_HH
#define PARTICLEFACTORY_HH

namespace AC {
class Event;
}

namespace Analysis {

class Particle;
class TrdHitFactory;
class TrackFactory;

/** Factory for high-level analysis particles.
  *
  * Combines the different factories for high-level analysis objects to
  * create the high-level object that is Analysis::Particle.
  *
  * Example:
  * \code
  * (top level:)
  * int stepsForTrdHits = Analysis::SecondCoord | Analysis::ShimGlobalAlign | Analysis::Pathlength ;
  * Analysis::ParticleFactory* pfactory = new Analysis::ParticleFactory( stepsForTrdHits );
  *
  * (in event loop:)
  *
  * AC::Event* event = filemanager.Event();
  * Analysis::Particle particle;
  * pfactory->PrepareWithDefaultObjects(*event, particle); //optional step
  * // (use suitable Selector for preselection)
  * pfactory->FillHighLevelObjects(particle);
  * // (do analysis, more cuts)
  * \endcode
  */
class ParticleFactory
{
public:

  ParticleFactory( int stepsForTrdHits );
  virtual ~ParticleFactory();

  void PrepareWithDefaultObjects( const AC::Event& rawEvt, Analysis::Particle& particle );
  void FillHighLevelObjects( Analysis::Particle& particle );

protected:

  bool DefaultMainAmsParticle( Analysis::Particle& particle ) const;
  bool DefaultMainTrackerTrack ( Analysis::Particle& particle ) const;
  bool DefaultMainTrackerTrackFit ( Analysis::Particle& particle ) const;
  bool DefaultEcalShower ( Analysis::Particle& particle ) const;
  bool DefaultTofBeta ( Analysis::Particle& particle ) const;
  bool DefaultMainTrdVTrack ( Analysis::Particle& particle ) const;


protected:

  TrdHitFactory* fTrdHitFactory;
  TrackFactory*  fTrackFactory;

  int fStepsForTrdHits;
};
}

#endif
