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

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if (p.HasRawEventData())
      return p.RawEvent()->Trigger().HasData();

    // For ACROOT files trigger information are available if both life time & rate is available.
    return p.TriggerLiveTime() != -1 && p.TriggerRate() != -1;
  }

  ClassDef(Cuts::CutTriggerInformationAvailable,1)
};

/** %Cut on trigger live time.
  * See Marco's select_3.C from the AMS collaboration (HasGoodLiveTime).
  */
class CutTriggerLiveTime : public Cut {
public:
  CutTriggerLiveTime() : Cut( "Trigger LiveTime" ) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    return p.TriggerLiveTime() == -1 || p.TriggerLiveTime() > 0.65;
  }

  ClassDef(Cuts::CutTriggerLiveTime,1)
};

/** %Cut on trigger rate. */
class CutTriggerRate : public Cut {
public:
  CutTriggerRate() : Cut( "Trigger rate") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    return p.TriggerRate() == -1 || p.TriggerRate() < 4000.0;
  }

  ClassDef(Cuts::CutTriggerRate,1)
};

}

#endif // TRIGGERCUTS_HH
