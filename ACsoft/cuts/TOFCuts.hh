#ifndef TOFCUTS_HH
#define TOFCUTS_HH

#include "TwoSidedCut.hh"

#ifndef __CINT__
#include "Event.h"
#include "AnalysisParticle.hh"
#endif

namespace Cuts {

/** Cut on TOF charge availability of selected particle in the event. */
class CutTofChargeAvailable : public Cut {
public:
  CutTofChargeAvailable() : Cut( "TOF charge available" ) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasTofBeta() ) return true;
    return p.HasTofCharge(); }

  ClassDef(Cuts::CutTofChargeAvailable,1)
};

/** Cut on TOF charge of TOFBeta object. */
class CutTofCharge : public TwoSidedCut {
public:
  CutTofCharge(float minimumCharge , float maximumCharge )
    : TwoSidedCut( "TOF charge", minimumCharge, maximumCharge) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasTofBeta() ) return true;
    if( !p.HasTofCharge() ) return true;
    return ValueIsInRange(p.TofCharge());
  }

  CutTofCharge() : TwoSidedCut( "", 0., 0. ) {}
  ClassDef(Cuts::CutTofCharge,1)
};


/** Cut on TOF mean energy (MeV), taking clusters in TOFbeta object associated with particle into account. */
class CutTofMeanEnergy : public TwoSidedCut {
public:
  CutTofMeanEnergy(float minimumValue , float maximumValue )
    : TwoSidedCut( "TOF mean energy deposition", minimumValue, maximumValue) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasTofBeta() ) return true;
    return ValueIsInRange(p.TofMeanEnergy());
  }

  CutTofMeanEnergy() : TwoSidedCut( "", 0., 0. ) {}
  ClassDef(Cuts::CutTofMeanEnergy,1)
};

/** Cut on TOF maximum energy (MeV), taking clusters in TOFbeta object associated with particle into account. */
class CutTofMaxEnergy : public TwoSidedCut {
public:
  CutTofMaxEnergy(float minimumValue, float maximumValue )
    : TwoSidedCut( "TOF maximum energy deposition", minimumValue, maximumValue) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasTofBeta() ) return true;
    return ValueIsInRange(p.TofMaxEnergy());
  }

  CutTofMaxEnergy() : TwoSidedCut( "", 0., 0. ) {}
  ClassDef(Cuts::CutTofMaxEnergy,1)
};

/** Cut on maximum energy (MeV) in \b all TOF clusters. */
class CutTofTotalMaxEnergy : public TwoSidedCut {
public:
  CutTofTotalMaxEnergy(float minimumValue, float maximumValue )
    : TwoSidedCut( "TOF total maximum energy", minimumValue, maximumValue) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    AssureCutIsAppliedToACQtFile(p);

    const ACsoft::AC::TOF& tof = p.RawEvent()->TOF();
    float TofMaxE = 0.0;
    for (unsigned int i=0; i<tof.Clusters().size(); i++){
      Float_t E = tof.Clusters().at(i).Energy();
      if( E > TofMaxE ) TofMaxE = E;
    }

    return ValueIsInRange(TofMaxE);
  }

  CutTofTotalMaxEnergy() : TwoSidedCut( "", 0., 0. ) {} // default constructor needed by ROOT, do not use!
  ClassDef(Cuts::CutTofTotalMaxEnergy,1)
};

}

#endif
