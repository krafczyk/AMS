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

/** Cut on Tracker number of tracks in the event. */
class CutTrackerNumberOfTracks : public TwoSidedCut {
public:
  CutTrackerNumberOfTracks(float minimumTracks, float maximumTracks) :
    TwoSidedCut( "Number of Tracker tracks", minimumTracks, maximumTracks) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) { return ValueIsInRange(p, p.NumberOfTrackerTracks()); }

  CutTrackerNumberOfTracks() : TwoSidedCut("",0.,0.) {}
  ClassDef(Cuts::CutTrackerNumberOfTracks,1)
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

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) { return p.HasEcalShower(); }

  ClassDef(Cuts::CutHasEcalShower,1)
};

/** Make sure that there is not too much ACC activity in the event. */
class CutAccActivity : public Cut {
public:
  CutAccActivity() : Cut( "ACC activity (at most 2 hits)" ) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) { return p.NumberOfAccHits() < 3; }

  ClassDef(Cuts::CutAccActivity,1)
};

/** Make sure that there is a TrackerTrack object associated to the event. */
class CutHasTrackerTrack : public Cut {
public:
  CutHasTrackerTrack() : Cut( "Has Tracker Track" ) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) { return p.HasTrackerTrack(); }

  ClassDef(Cuts::CutHasTrackerTrack,1)
};

/** Make sure that there is a TrackerTrackFit object associated to the event. */
class CutHasTrackerTrackFit : public Cut {
public:
  CutHasTrackerTrackFit() : Cut( "Has Tracker track fit" ) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    return p.HasTrackerTrackFit();
  }

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
    return p.TrdVTrack() != 0;
  }

  ClassDef(Cuts::CutHasTRDVTrack,1)
};

/** Make sure that we get the correct MC particle ID in MC events. */
class CutMCHasPrimaryEventGenerator : public Cut {
public:
  CutMCHasPrimaryEventGenerator() : Cut( "MC events: Has primary event generator" ) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.IsMC() ) return true;

    return p.HasPrimaryEventGenerator();
  }

  ClassDef(Cuts::CutMCHasPrimaryEventGenerator,1)
};

/** Make sure the selected event indicates a downgoing particle.
  */
class CutIsDowngoing : public Cut {
public:
  CutIsDowngoing() : Cut( "Downgoing (#beta > 0)") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasTofBeta() ) return true;
    return p.BetaTof() > 0;
  }

  ClassDef(Cuts::CutIsDowngoing,1)
};


/** Make sure the selected event indicates a downgoing particle.
  * Using |1/beta - 1| < 0.2 as decided in AMS-A preselection meeting on Sep 29, 2012.
  */
class CutElectronIsDowngoing : public TwoSidedCut {
public:
  CutElectronIsDowngoing() : TwoSidedCut( "Downgoing electrons (|1/#beta - 1|)", 0, 0.2) { }

  // FIXME this should not be a TwoSidedCut

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasTofBeta() ) return true;
    return ValueIsInRange(p, fabs( 1.0 / p.BetaTof() - 1.0) );
  }

  ClassDef(Cuts::CutElectronIsDowngoing,1)
};


/** Check event consistency.
  * \sa ACsoft::AC::Event::CheckConsistency()
  */
class CutEventConsistency : public Cut {
public:
  CutEventConsistency() : Cut("Event consistency") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    return p.HasRawEventData() ? p.RawEvent()->CheckConsistency() : true;
  }

  ClassDef(Cuts::CutEventConsistency,1)
};

/** %Cut to make sure that %ECAL energy of selected track fit in event is above geomagnetic cutoff rigidity for both charge signs. */
class CutEcalEnergyAboveGeomagneticCutoff : public Cut {
public:
  CutEcalEnergyAboveGeomagneticCutoff()
    : Cut( "ECAL energy above geomagnetic cutoffs") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if (p.IsMC()) return true;

    if( !p.HasEcalShower() ) return true;

    return fabs(p.EcalEnergy()) > 1.3 * fabs(p.GeomagneticMaxCutOff());
  }

  ClassDef(Cuts::CutEcalEnergyAboveGeomagneticCutoff,1)
};

/** %Cut to make sure that rigidity of selected track fit in event is above geomagnetic cutoff rigidity for both charge signs. */
class CutRigidityAboveGeomagneticCutoff : public Cut {
public:
  CutRigidityAboveGeomagneticCutoff()
    : Cut( "Rigidity above geomagnetic cutoffs") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if (p.IsMC()) return true;

    if( !p.HasTrackerTrack() ) return true;
    if( !p.HasTrackerTrackFit() ) return true;

    return fabs(p.Rigidity()) > 1.3 * fabs(p.GeomagneticMaxCutOff());
  }

  ClassDef(Cuts::CutRigidityAboveGeomagneticCutoff,1)
};

}

#endif // PRESELECTIONCUTS_HH
