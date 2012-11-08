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

  virtual bool TestCondition( const Analysis::Particle& p ) { return p.RawEvent()->Particles().size() == 1; }

  ClassDef(Cuts::CutSingleParticle,1)
};

/** Make sure there is exactly one TrackerTrack in event. */
class CutSingleTrackerTrack : public Cut {
public:
  CutSingleTrackerTrack() : Cut( "Single Track" ) { }

  virtual bool TestCondition( const Analysis::Particle& p ) { return p.RawEvent()->Tracker().Tracks().size() == 1; }

  ClassDef(Cuts::CutSingleTrackerTrack,1)
};


/** Make sure that there is an ECALShower object associated to the event. */
class CutHasEcalShower : public Cut {
public:
  CutHasEcalShower() : Cut( "Has ECAL Shower" ) { }

  virtual bool TestCondition(const Analysis::Particle& p) { return p.MainEcalShower() != 0; }

  ClassDef(Cuts::CutHasEcalShower,1)
};

/** Make sure that there is a TrackerTrack object associated to the event. */
class CutHasTrackerTrack : public Cut {
public:
  CutHasTrackerTrack() : Cut( "Has Tracker Track" ) { }

  virtual bool TestCondition(const Analysis::Particle& p) { return p.MainTrackerTrack() != 0; }

  ClassDef(Cuts::CutHasTrackerTrack,1)
};

/** Make sure that there is a TrackerTrackFit object associated to the event. */
class CutHasTrackerTrackFit : public Cut {
public:
  CutHasTrackerTrackFit() : Cut( "Has Tracker track fit" ) { }

  virtual bool TestCondition(const Analysis::Particle& p) { return p.MainTrackerTrackFit() != 0; }

  ClassDef(Cuts::CutHasTrackerTrackFit,1)
};

/** Make sure that there is a TOFBeta object associated to the event. */
class CutHasTofBeta : public Cut {
public:
  CutHasTofBeta() : Cut( "Has TOF beta" ) { }

  virtual bool TestCondition(const Analysis::Particle& p) { return p.TofBeta() != 0; }

  ClassDef(Cuts::CutHasTofBeta,1)
};


/** Make sure that there is a TRDVTrack object associated to the event. */
class CutHasTRDVTrack : public Cut {
public:
  CutHasTRDVTrack() : Cut( "Has TRD V-track" ) { }

  virtual bool TestCondition(const Analysis::Particle& p) { return p.MainTrdVTrack() != 0; }

  ClassDef(Cuts::CutHasTRDVTrack,1)
};


/** Make sure the selected event indicates a downgoing particle.
  * Using |1/beta - 1| < 0.2 as decided in AMS-A preselection meeting on Sep 29, 2012.
  */
class CutIsDowngoing : public TwoSidedCut {
public:
  CutIsDowngoing() : TwoSidedCut( "Downgoing (1/beta - 1)", 0, 0.2) { }

  virtual bool TestCondition(const Analysis::Particle& p) {

    if( !p.TofBeta() ) return true;
    return ValueIsInRange( fabs( 1.0 / p.TofBeta()->Beta() - 1.0) );
  }

  ClassDef(Cuts::CutIsDowngoing,1)
};

/** Check event consistency.
  * \sa AC::Event::CheckConsistency()
  */
class CutEventConsistency : public Cut {
public:
  CutEventConsistency() : Cut("Event consistency") { }

  virtual bool TestCondition(const Analysis::Particle& p) { return p.RawEvent()->CheckConsistency(); }

  ClassDef(Cuts::CutEventConsistency,1)
};

}

#endif // PRESELECTIONCUTS_HH
