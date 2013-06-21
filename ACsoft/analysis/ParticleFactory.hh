#ifndef PARTICLEFACTORY_HH
#define PARTICLEFACTORY_HH

#include <TTimeStamp.h>

namespace ACsoft {

namespace AC {
  class Event;
}

namespace IO {
  class ReducedEvent;
}

namespace Utilities {
  class McSpectrumScaler;
}

namespace Analysis {

class Particle;
class TrdHitFactory;
class TrackFactory;
class TrdTracking;

/** Steps used by ParticleFactory in production of high-level objects.
  */
enum ParticleFactoryProductionSteps { CreateSplineTrack = 1<<1, TrdHitsFromSplineTrack = 1<<2,
                                      CreateTrdTrack = 1<<3, TrdHitsFromTrdTrack = 1<<4 };

/** Factory for high-level analysis particles.
  *
  * Combines the different factories for high-level analysis objects to
  * create the high-level object that is Analysis::Particle.
  *
  * Example:
  * \code
  * (top level:)
  * Analysis::ParticleFactory* pfactory = new Analysis::ParticleFactory( );
  *
  * (in event loop:)
  *
  * Analysis::Particle particle;
  * pfactory->PrepareWithDefaultObjects(fileManager.Event(), fileManager.EventReduced() particle);
  * // (use suitable Selector for preselection)
  * pfactory->FillHighLevelObjects(particle);
  * // (do analysis, more cuts)
  * \endcode
  *
  * The behaviour of the FillHighLevelObjects() function is governed by the ParticleFactoryProductionSteps set for
  * the ParticleFactory object.
  *
  */
class ParticleFactory
{
public:

  ParticleFactory( Utilities::McSpectrumScaler* mcScaler = 0 );
  virtual ~ParticleFactory();

  void SetParticleFactoryProductionSteps( int steps ) { fProductionSteps = steps; }

  void PrepareDummyParticle( const TTimeStamp&, Analysis::Particle& );
  void PrepareWithDefaultObjects( const AC::Event*, const IO::ReducedEvent*, Analysis::Particle& );
  void PrepareWithDefaultObjects( const AC::Event*, Analysis::Particle& );
  void PrepareWithDefaultObjects( const IO::ReducedEvent*, Analysis::Particle& );
  void FillHighLevelObjects( Analysis::Particle& );
  void OverrideDefaultTrackFit(int algoritm, int pattern, int refit);

protected:

  bool DefaultAmsParticle( Analysis::Particle& ) const;
  bool DefaultTrackerTrack ( Analysis::Particle& ) const;
  bool DefaultTrackerTrackFit ( Analysis::Particle& ) const;
  bool DefaultEcalShower ( Analysis::Particle& ) const;
  bool DefaultTofBeta ( Analysis::Particle& ) const;
  bool DefaultTrdVTrack ( Analysis::Particle& ) const;
  bool DefaultRichRing ( Analysis::Particle& ) const;


private:

  bool TestOption( ParticleFactoryProductionSteps step ) const { return ( (fProductionSteps & step) == step ); }

protected:

  TrdHitFactory* fTrdHitFactory;
  TrackFactory*  fTrackFactory;
  TrdTracking*   fTrdTracking;

  int fProductionSteps; ///< ParticleFactoryProductionSteps set for this factory, governs the behaviour of FillHighLevelObjects()
  bool fTrackFitOverridden;
  int fAlgorithm;
  int fPattern;
  int fRefit;

  Utilities::McSpectrumScaler* fMcScaler;

};
}

}

#endif
