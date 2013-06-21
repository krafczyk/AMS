#ifndef TOFCUTS_HH
#define TOFCUTS_HH

#include "TwoSidedCut.hh"

#ifndef __CINT__
#include "Event.h"
#include "AnalysisParticle.hh"
#include "Utilities.hh"
#endif

namespace Cuts {

/** Cut on TOF 1/beta of selected particle in the event. */
class CutTofInverseBeta : public TwoSidedCut
{
public:
  CutTofInverseBeta(float minimumInverseBeta, float maximumInverseBeta) : TwoSidedCut("TOF 1/beta", minimumInverseBeta, maximumInverseBeta) {}

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {
    if(!p.HasTofBeta()) return true;
    return ValueIsInRange(p, 1./(p.BetaTof()));
  }

  CutTofInverseBeta() : TwoSidedCut("", 0.0, 0.0) {}
  ClassDef(Cuts::CutTofInverseBeta,1)
};

/** Cut on TOF charge availability of selected particle in the event. */
class CutTofChargeAvailable : public Cut {
public:
  CutTofChargeAvailable() : Cut( "TOF charge available" ) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasTofBeta() ) return true;
    return p.HasTofCharge();
  }

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

    return ValueIsInRange(p, p.TofCharge());
  }

  CutTofCharge() : TwoSidedCut( "", 0., 0. ) {}
  ClassDef(Cuts::CutTofCharge,1)
};

/** Cut on Lower TOF charge of TOFBeta object. */
class CutLowerTofCharge : public TwoSidedCut {
public:
  CutLowerTofCharge(float minimumCharge , float maximumCharge )
  : TwoSidedCut( "Lower TOF charge", minimumCharge, maximumCharge) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasTofBeta() ) return true;
    return ValueIsInRange(p, p.LowerTofCharge());
  }

  CutLowerTofCharge() : TwoSidedCut( "", 0., 0. ) {}
  ClassDef(Cuts::CutLowerTofCharge,1)
};

/** Cut on Upper TOF charge of TOFBeta object. */
class CutUpperTofCharge : public TwoSidedCut {
public:
  CutUpperTofCharge(float minimumCharge , float maximumCharge )
  : TwoSidedCut( "Upper TOF charge", minimumCharge, maximumCharge) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasTofBeta() ) return true;
    return ValueIsInRange(p, p.UpperTofCharge());
  }

  CutUpperTofCharge() : TwoSidedCut( "", 0., 0. ) {}
  ClassDef(Cuts::CutUpperTofCharge,1)
};

/** Cut on TOF mean energy (MeV), taking clusters in TOFbeta object associated with particle into account. */
class CutTofMeanEnergy : public TwoSidedCut {
public:
  CutTofMeanEnergy(float minimumValue , float maximumValue ) // FIXME deprecated cut ?
    : TwoSidedCut( "TOF mean energy deposition", minimumValue, maximumValue) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasTofBeta() ) return true;
    return ValueIsInRange(p, p.TofMeanEnergy());
  }

  CutTofMeanEnergy() : TwoSidedCut( "", 0., 0. ) {}
  ClassDef(Cuts::CutTofMeanEnergy,1)
};

/** Cut on TOF maximum energy (MeV), taking clusters in TOFbeta object associated with particle into account. */
class CutTofMaxEnergy : public TwoSidedCut {
public:
  CutTofMaxEnergy(float minimumValue, float maximumValue ) // FIXME deprecated cut ?
    : TwoSidedCut( "TOF maximum energy deposition", minimumValue, maximumValue) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasTofBeta() ) return true;
    return ValueIsInRange(p, p.TofMaxEnergy());
  }

  CutTofMaxEnergy() : TwoSidedCut( "", 0., 0. ) {}
  ClassDef(Cuts::CutTofMaxEnergy,1)
};

/** Cut on maximum energy (MeV) in \b all TOF clusters. */
class CutTofTotalMaxEnergy : public TwoSidedCut {
public:
  CutTofTotalMaxEnergy(float minimumValue, float maximumValue ) // FIXME deprecated cut ?
    : TwoSidedCut( "TOF total maximum energy", minimumValue, maximumValue) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    AssureCutIsAppliedToACQtFile(p);

    const ACsoft::AC::TOF& tof = p.RawEvent()->TOF();
    float TofMaxE = 0.0;
    for (unsigned int i=0; i<tof.Clusters().size(); i++){
      Float_t E = tof.Clusters().at(i).Energy();
      if( E > TofMaxE ) TofMaxE = E;
    }

    return ValueIsInRange(p, TofMaxE);
  }

  CutTofTotalMaxEnergy() : TwoSidedCut( "", 0., 0. ) {} // default constructor needed by ROOT, do not use!
  ClassDef(Cuts::CutTofTotalMaxEnergy,1)
};

class CutTofBetaProtons : public NMinusOneCut {
public:
  CutTofBetaProtons() : NMinusOneCut("ToF 1/beta for protons") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle & p) {

    if( !p.HasTofBeta()) return true;
    if( !p.HasTrackerTrack()) return true;
    if( !p.HasTrackerTrackFit()) return true;
    const double rigidity = p.Rigidity();
    const double upper = ACsoft::Utilities::OneOverBetaCurves(0.938, rigidity, 1.09, 0.1, 1);
    const double lower = ACsoft::Utilities::OneOverBetaCurves(0.938, rigidity, 1.9, -0.1, -3);
    const double inverseBeta = 1./p.BetaTof();
    fLastCutValue = inverseBeta;
    fLastRigidityOrEnergy = NMinusOneValue(p, fNMinusOneMode);
    return (inverseBeta < upper && inverseBeta > lower);
  }

  ClassDef(Cuts::CutTofBetaProtons, 1)
};

class CutTofBetaHelium : public NMinusOneCut {
public:
  CutTofBetaHelium() : NMinusOneCut("Tof 1/beta for helium") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle &p) {

    if( !p.HasTofBeta()) return true;
    if( !p.HasTrackerTrack()) return true;
    if( !p.HasTrackerTrackFit()) return true;
    const double rigidity = p.Rigidity();
    const double upper = ACsoft::Utilities::OneOverBetaCurves(3.727, rigidity, 2, 0.15, 6);
    const double lower = ACsoft::Utilities::OneOverBetaCurves(3.727, rigidity, 2, -0.15, 6);
    const double inverseBeta = 1./p.BetaTof();
    fLastCutValue = inverseBeta;
    fLastRigidityOrEnergy = NMinusOneValue(p, fNMinusOneMode);
    return (inverseBeta < upper && inverseBeta > lower);
  }

  ClassDef(Cuts::CutTofBetaHelium, 1)
};

}

#endif
