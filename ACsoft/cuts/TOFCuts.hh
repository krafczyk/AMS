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

  virtual bool TestCondition(const Analysis::Particle& p) {

    if( !p.TofBeta() ) return true;
    return p.TofBeta()->ChargesNew().size() > 0; }

  ClassDef(Cuts::CutTofChargeAvailable,1)
};

/** Cut on TOF charge of TOFBeta object. */
class CutTofCharge : public TwoSidedCut {
public:
  CutTofCharge(float minimumCharge , float maximumCharge )
    : TwoSidedCut( "TOF charge", minimumCharge, maximumCharge) { }

  virtual bool TestCondition(const Analysis::Particle& p) {

    if( !p.TofBeta() ) return true;
    if( !p.TofBeta()->ChargesNew().size() ) return true;
    return ValueIsInRange(p.TofBeta()->ChargesNew()[0]);
  }

  CutTofCharge() : TwoSidedCut( "", 0., 0. ) {}
  ClassDef(Cuts::CutTofCharge,1)
};


/** Cut on TOF mean energy (MeV), taking clusters in TOFbeta object associated with particle into account. */
class CutTofMeanEnergy : public TwoSidedCut {
public:
  CutTofMeanEnergy(float minimumValue , float maximumValue )
    : TwoSidedCut( "TOF mean energy deposition", minimumValue, maximumValue) { }

  virtual bool TestCondition(const Analysis::Particle& p) {

    const AC::TOFBeta* beta = p.TofBeta();
    if( !beta ) return true;
    return ValueIsInRange(p.TofBetaMeanEnergy());
  }

  CutTofMeanEnergy() : TwoSidedCut( "", 0., 0. ) {}
  ClassDef(Cuts::CutTofMeanEnergy,1)
};

/** Cut on TOF maximum energy (MeV), taking clusters in TOFbeta object associated with particle into account. */
class CutTofMaxEnergy : public TwoSidedCut {
public:
  CutTofMaxEnergy(float minimumValue, float maximumValue )
    : TwoSidedCut( "TOF maximum energy deposition", minimumValue, maximumValue) { }

  virtual bool TestCondition(const Analysis::Particle& p) {

    const AC::TOFBeta* beta = p.TofBeta();
    if( !beta ) return true;
    return ValueIsInRange(p.TofBetaMaxEnergy());
  }

  CutTofMaxEnergy() : TwoSidedCut( "", 0., 0. ) {}
  ClassDef(Cuts::CutTofMaxEnergy,1)
};

/** Cut on maximum energy (MeV) in \b all TOF clusters. */
class CutTofTotalMaxEnergy : public TwoSidedCut {
public:
  CutTofTotalMaxEnergy(float minimumValue, float maximumValue )
    : TwoSidedCut( "TOF total maximum energy", minimumValue, maximumValue) { }

  virtual bool TestCondition(const Analysis::Particle& p) {

    const AC::TOF& tof = p.RawEvent()->TOF();
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
