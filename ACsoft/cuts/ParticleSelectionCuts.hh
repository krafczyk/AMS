#ifndef PARTICLESELECTIONCUTS_HH
#define PARTICLESELECTIONCUTS_HH

#include "TwoSidedCut.hh"
#include <TGraph.h>

#ifndef __CINT__
#include "Event.h"
#include "AnalysisParticle.hh"
#include "TrackerCharges.hh"
#endif

namespace Cuts {

short ParticleID(float trackerCharge, float rigidity);

// FIXME the following can be done via Selectors of existing cuts (almost....)

/** Select proton candidates **/
class CutProtonCandidate : public Cut {
public:
  CutProtonCandidate() : Cut( "Proton candidate" ) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( p.HasTrackerTrack() && p.HasTrackerTrackFit() && p.Rigidity() > 0. )
      return fabs(p.TrackerCharge() - TrackerCharges::GetMean(0)) < 3 * TrackerCharges::GetSigma(0);
    return false;
  }

  ClassDef(Cuts::CutProtonCandidate,1)
};

/** Remove anti-nuclei **/
class CutRemoveAntiNuclei  : public Cut {
public:
  CutRemoveAntiNuclei() : Cut("Remove anti-nuclei") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasTrackerTrackFit() ) return true;
    if (p.Rigidity() >= 0.0) return true;
    if( !p.HasTrackerTrack() ) return true;
    return p.TrackerCharge() < TrackerCharges::GetMean(0) + 3 * TrackerCharges::GetSigma(0);
  }

  ClassDef(Cuts::CutRemoveAntiNuclei,1)
};

/** Remove heavy-nuclei **/
class CutRemoveHeavyNuclei : public Cut {
public:
  CutRemoveHeavyNuclei(): Cut("Remove heavy nuclei") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasTrackerTrack() ) return true;
    return p.TrackerCharge() < TrackerCharges::GetMean(5) + 3 * TrackerCharges::GetSigma(5);
  }

  ClassDef(Cuts::CutRemoveHeavyNuclei,1)
};


/** Make sure charge in tracker layer 1 and upper TOF are consistent with Z=1, but only if measurements are available.
  * If a layer1 charge measurement is not available, it's not taken into account for this cut.
  * Same for the upper tof charges.
  **/
class CutProbablyChargeOneBeforeInnerTracker : public Cut {
public:
  CutProbablyChargeOneBeforeInnerTracker() : Cut("Probably Z=1 in front of inner tracker") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    // If tracker layer 1 charge measurement is available, and not within the expected range, bail out.
    Float_t TrkCLay1 = p.TrackerChargeFor(ACsoft::Analysis::Particle::TrkLay1);
    if (TrkCLay1 >= 0 && !(TrkCLay1 > 0.8 && TrkCLay1 < 1.5))
      return false;

    Float_t TofCLay1 = p.TofChargeFor(ACsoft::Analysis::Particle::TofLay1);
    if (TofCLay1 >= 0 && !(TofCLay1 > 0.8 && TofCLay1 < 1.3))
      return false;

    Float_t TofCLay2 = p.TofChargeFor(ACsoft::Analysis::Particle::TofLay2);
    if (TofCLay2 >= 0 && !(TofCLay2 > 0.8 && TofCLay2 < 1.3))
      return false;
    return true;
  }

  ClassDef(Cuts::CutProbablyChargeOneBeforeInnerTracker,1)
};


/** Make sure charge in tracker layer 1 and upper TOF are consistent with Z=1.
  *
**/
class CutChargeOneBeforeInnerTracker : public Cut {
public:
  CutChargeOneBeforeInnerTracker() : Cut("Z=1 in front of inner tracker") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    Float_t TofCLay1 = p.TofChargeFor(ACsoft::Analysis::Particle::TofLay1);
    Float_t TofCLay2 = p.TofChargeFor(ACsoft::Analysis::Particle::TofLay2);
    Float_t TrkCLay1 = p.TrackerChargeFor(ACsoft::Analysis::Particle::TrkLay1);

    return ( TofCLay1 > 0.8 && TofCLay1<1.3 &&
             TofCLay2 > 0.8 && TofCLay2<1.3 &&
             TrkCLay1 > 0.8 && TrkCLay1<1.5 ); // FIXME Do I have to use ValueIsInRange here? Why call it ValueIsInRange when it checks a boolean???
  }

  ClassDef(Cuts::CutChargeOneBeforeInnerTracker,1)
};



/** Reduce misidentified helium.
  *
  * Cut acts on events that have Z(tracker) clearly larger than proton-like.
  * Events with Z(TOF) in the proton-like range are then removed.
  *
**/
class CutReduceMisidentifiedHelium : public Cut {
public:
  CutReduceMisidentifiedHelium() : Cut("Reduce misidentified helium") { } // FIXME obsolete?

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if(!p.HasTofBeta()) return true;

    if( !p.HasTrackerTrack() ) return true;
    float trackerCharge = p.TrackerCharge();
    if (trackerCharge > TrackerCharges::GetMean(0) + 3 * TrackerCharges::GetSigma(0))
      return true;

    // FIXME: This needs to be seperated into different cuts, otherwhise there's no way to calculate the efficiency of such a cu.
    if (p.TofCharge() > 1.8 && p.TofChargeError() < 0.2)
      return false;

    float emean = p.TofMeanEnergy();
    float emax  = p.TofMaxEnergy();
    return emean <= 4 && emax <= 6.5;
  }

  ClassDef(Cuts::CutReduceMisidentifiedHelium,1)
};


/** Cut on ECAL energy over (absolute value of) Tracker rigidity. */
class CutEnergyOverRigidity : public TwoSidedCut {
public:
  CutEnergyOverRigidity(float minimumRatio, float maximumRatio)
    : TwoSidedCut( "E/R", minimumRatio, maximumRatio) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasTrackerTrack() ) return true;
    if( !p.HasTrackerTrackFit() ) return true;
    if( !p.HasEcalShower() ) return true;
    return ValueIsInRange(p, p.EcalEnergy() / fabs(p.Rigidity()));
  }

  CutEnergyOverRigidity() : TwoSidedCut( "", 0.0, 0.0 ) {}
  ClassDef(Cuts::CutEnergyOverRigidity,1)
};

/** Is main AMS particle a electron candidate?
 *
 *  Check:
 *   - negative rigidity (negative charged particles)
 *   - ECAL Shower BDT estimator
 *
 * Comment from Marco Incagli: "If you look at the general rigidity distribution,
 * you will see a peak at zero which indicates poorly reconstructed tracks.
 * Therefore we apply a "preselection cut" of p<-2GeV, instead of p<0 .
 * This part needs some more understanding."
 *
 * \todo Can we improve this?
 * \todo Should change the preselection cut on the rigidity to -2 GV
 */
class CutElectronCandidate : public Cut {
public:
  CutElectronCandidate() : Cut( "Electron candidate" ) {

    fCut90PercentEfficiency = new TGraph("$ACROOTSOFTWARE/acroot/data/bdt_cut90_072612.dat");
  }

  virtual ~CutElectronCandidate() { delete fCut90PercentEfficiency; }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    AssureCutIsAppliedToACQtFile(p);

    const ACsoft::AC::TrackerTrackFit* trackFit = p.TrackerTrackFit();
    if( !trackFit ) return true;
    if( trackFit->Rigidity() > -2.0 ) return true;

    const ACsoft::AC::ECALShower* shower = p.EcalShower();
    if( !shower ) return true;
    if( !shower->Estimators().size() ) return true;

    // Use Pisa BDT estimator to get a electron within 90% of the CI.
    // FIXME explain the factor 1e-3
    if (shower->Estimators()[0] <= fCut90PercentEfficiency->Eval(shower->DepositedEnergy() * 1e-3))
      return false;

    // Calculate energy over rigidity and check if it is between 0.6 and 2.
    float ratio = shower->ReconstructedEnergy() / fabs(trackFit->Rigidity());
    return ratio > 0.6 && ratio < 2.0;
  }

  ClassDef(Cuts::CutElectronCandidate,1)

private:
  TGraph* fCut90PercentEfficiency;
};

}

#endif // PARTICLESELECTIONCUTS_HH
