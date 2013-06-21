
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


/** %Cut on trigger physics bit for charged particles
  *
  * - bit1: unbiased TOF-trig(i.e. FTC= z>=1)
  * - bit2: Z>=1(FTC+anti) "protons"
  * - bit3: Z>=2(FTC & BZ) ions
  * - bit4: SlowZ>=2 (FTZ) slow ions
  * - bit5: electrons(FTC & FTE)
  * - bit6: gammas(FTE & ECLVL1("shower angle")
  * - bit7: unbECAL(FTE)
  * - bit8: External
  */
class CutPhysicsTriggerChargedParticles: public Cut {
public:
  CutPhysicsTriggerChargedParticles() : Cut( "Trigger Physics bits for charged particles") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    return (p.TriggerFlags()&0x1E)>0?true:false;
  }

  ClassDef(Cuts::CutPhysicsTriggerChargedParticles,1)
};


class CutHasAnyPhysicsTrigger: public Cut {
public:
  CutHasAnyPhysicsTrigger() : Cut("Tests for any physics bit") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {
    int physb = p.TriggerFlags();

    bool HasAny = false;
    HasAny |= physb&0x0002;
    HasAny |= physb&0x0004;
    HasAny |= physb&0x0008;
    HasAny |= physb&0x0010;
    HasAny |= physb&0x0020;

    return HasAny;
  }

  ClassDef(Cuts::CutHasAnyPhysicsTrigger,1)
};


}

#endif // TRIGGERCUTS_HH
