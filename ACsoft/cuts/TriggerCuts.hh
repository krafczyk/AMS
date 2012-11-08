#ifndef TRIGGERCUTS_HH
#define TRIGGERCUTS_HH

#include "Cut.hh"

#ifndef __CINT__
#include "Event.h"
#include "AnalysisParticle.hh"
#endif

namespace Cuts {

/** Make sure there is trigger information available in event. */
class CutTriggerInformationAvailable : public Cut {
public:
  CutTriggerInformationAvailable() : Cut( "Trigger information available" ) { }

  virtual bool TestCondition(const Analysis::Particle& p) { return p.RawEvent()->Trigger().HasData(); }

  ClassDef(Cuts::CutTriggerInformationAvailable,1)
};

/** %Cut on trigger live time.
  * See Marco's select_3.C from the AMS collaboration (HasGoodLiveTime).
  */
class CutTriggerLiveTime : public Cut {
public:
  CutTriggerLiveTime() : Cut( "Trigger livetime" ) { }

  virtual bool TestCondition(const Analysis::Particle& p) {

    if( !p.RawEvent()->Trigger().HasData() ) return true;
    return p.RawEvent()->Trigger().LiveTime() > 0.65; }

  ClassDef(Cuts::CutTriggerLiveTime,1)
};

/** %Cut on trigger rate. */
class CutTriggerRate : public Cut {
public:
  CutTriggerRate() : Cut( "Trigger rate") { }

  virtual bool TestCondition(const Analysis::Particle& p) {

    if( !p.RawEvent()->Trigger().HasData() ) return true;
    return p.RawEvent()->Trigger().LiveTime() > 0.5 && p.RawEvent()->Trigger().TriggerRateFT() < 4000.0;
  }

  ClassDef(Cuts::CutTriggerRate,1)
};

}

#endif // TRIGGERCUTS_HH
