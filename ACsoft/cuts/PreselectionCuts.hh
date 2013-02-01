#ifndef PRESELECTIONCUTS_HH
#define PRESELECTIONCUTS_HH

#include "Cut.hh"
#include "TwoSidedCut.hh"

#ifndef __CINT__
#include "Event.h"
#include "AnalysisParticle.hh"
#endif

namespace Cuts {

/** Make sure there is exactly one AMS Particle in the event. */
class CutSingleParticle : public Cut {
public:
  CutSingleParticle() : Cut("Single Particle") { }

  virtual bool TestCondition( const ACsoft::Analysis::Particle& p ) { return p.NumberOfParticles() == 1; }

  ClassDef(Cuts::CutSingleParticle,1)
};

/** Make sure there is at least one AMS Particle in the event. */
class CutAtLeastOneParticle : public Cut {
public:
  CutAtLeastOneParticle() : Cut("At least one Particle") { }

  virtual bool TestCondition( const ACsoft::Analysis::Particle& p ) { return p.NumberOfParticles() >= 1; }

  ClassDef(Cuts::CutAtLeastOneParticle,1)
};

/** Make sure there is exactly one TrackerTrack in event. */
class CutSingleTrackerTrack : public Cut {
public:
  CutSingleTrackerTrack() : Cut( "Single Track" ) { }

  virtual bool TestCondition( const ACsoft::Analysis::Particle& p ) { return p.NumberOfTrackerTracks() == 1; }

  ClassDef(Cuts::CutSingleTrackerTrack,1)
};

/** Make sure that there is an ECALShower object associated to the event. */
class CutHasEcalShower : public Cut {
public:
  CutHasEcalShower() : Cut( "Has ECAL Shower" ) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) { return p.HasMainEcalShower(); }

  ClassDef(Cuts::CutHasEcalShower,1)
};

/** Make sure that there is not too much ACC activity in the event. */
class CutAccActivity : public Cut {
public:
  CutAccActivity() : Cut( "ACC activity (at most 2 hits)" ) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) { return ( p.NumberOfAccHits() < 3 ); }

  ClassDef(Cuts::CutAccActivity,1)
};

/** Make sure that there is a TrackerTrack object associated to the event. */
class CutHasTrackerTrack : public Cut {
public:
  CutHasTrackerTrack() : Cut( "Has Tracker Track" ) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) { return p.HasMainTrackerTrack(); }

  ClassDef(Cuts::CutHasTrackerTrack,1)
};

/** Make sure that there is a TrackerTrackFit object associated to the event. */
class CutHasTrackerTrackFit : public Cut {
public:
  CutHasTrackerTrackFit() : Cut( "Has Tracker track fit" ) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) { return p.HasMainTrackerTrackFit(); }

  ClassDef(Cuts::CutHasTrackerTrackFit,1)
};

/** Make sure that there is a TOFBeta object associated to the event. */
class CutHasTofBeta : public Cut {
public:
  CutHasTofBeta() : Cut( "Has TOF beta" ) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) { return p.HasTofBeta(); }

  ClassDef(Cuts::CutHasTofBeta,1)
};


/** Make sure that there is a TRDVTrack object associated to the event. */
class CutHasTRDVTrack : public Cut {
public:
  CutHasTRDVTrack() : Cut( "Has TRD V-track" ) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    AssureCutIsAppliedToACQtFile(p);
    return p.MainTrdVTrack() != 0;
  }

  ClassDef(Cuts::CutHasTRDVTrack,1)
};


/** Make sure the selected event indicates a downgoing particle.
  * Using |1/beta - 1| < 0.2 as decided in AMS-A preselection meeting on Sep 29, 2012.
  */
class CutIsDowngoing : public TwoSidedCut {
public:
  CutIsDowngoing() : TwoSidedCut( "Downgoing (1/beta - 1)", 0, 0.2) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasTofBeta() ) return true;
    return ValueIsInRange( fabs( 1.0 / p.BetaTof() - 1.0) );
  }

  ClassDef(Cuts::CutIsDowngoing,1)
};

/** Check event consistency.
  * \sa ACsoft::AC::Event::CheckConsistency()
  */
class CutEventConsistency : public Cut {
public:
  CutEventConsistency() : Cut("Event consistency") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if (p.HasRawEventData())
      return p.RawEvent()->CheckConsistency();
    return true; // ACROOT files are never inconsistent.
  }

  ClassDef(Cuts::CutEventConsistency,1)
};

/** %Cut to make sure that rigidity of selected track fit in event is above geomagnetic cutoff rigidity for both charge signs. */
class CutRigidityAboveGeomagneticCutoff : public Cut {
public:
  CutRigidityAboveGeomagneticCutoff()
    : Cut( "Rigidity above geomagnetic cutoffs") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    return ( fabs(p.Rigidity()) > 1.3 * fabs(p.GeomagneticMaxCutOff()) );
  }

  ClassDef(Cuts::CutRigidityAboveGeomagneticCutoff,1)
};

}

#endif // PRESELECTIONCUTS_HH
