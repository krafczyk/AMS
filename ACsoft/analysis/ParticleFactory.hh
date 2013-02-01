#ifndef PARTICLEFACTORY_HH
#define PARTICLEFACTORY_HH

namespace ACsoft {

namespace AC {
  class Event;
};

namespace IO {
  class ReducedEvent;
};

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
  * Analysis::Particle particle;
  * pfactory->PrepareWithDefaultObjects(fileManager.Event(), fileManager.EventReduced() particle); //optional step
  * // (use suitable Selector for preselection)
  * pfactory->FillHighLevelObjects(particle);
  * // (do analysis, more cuts)
  * \endcode
  */
class ParticleFactory
{
public:

  ParticleFactory( int stepsForTrdHits, bool fullDetectorTrack = true );
  virtual ~ParticleFactory();

  void PrepareWithDefaultObjects( const AC::Event*, const IO::ReducedEvent*, Analysis::Particle& );
  void PrepareWithDefaultObjects( const AC::Event*, Analysis::Particle& );
  void PrepareWithDefaultObjects( const IO::ReducedEvent*, Analysis::Particle& );
  void FillHighLevelObjects( Analysis::Particle& );

protected:

  bool DefaultMainAmsParticle( Analysis::Particle& ) const;
  bool DefaultMainTrackerTrack ( Analysis::Particle& ) const;
  bool DefaultMainTrackerTrackFit ( Analysis::Particle& ) const;
  bool DefaultEcalShower ( Analysis::Particle& ) const;
  bool DefaultTofBeta ( Analysis::Particle& ) const;
  bool DefaultMainTrdVTrack ( Analysis::Particle& ) const;
  bool DefaultMainRichRing ( Analysis::Particle& ) const;


protected:

  TrdHitFactory* fTrdHitFactory;
  TrackFactory*  fTrackFactory;

  int fStepsForTrdHits;
  bool fFullDetectorTrack;
};
}

}

#endif
