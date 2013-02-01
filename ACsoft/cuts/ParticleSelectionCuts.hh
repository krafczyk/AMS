#ifndef PARTICLESELECTIONCUTS_HH
#define PARTICLESELECTIONCUTS_HH

#include "TwoSidedCut.hh"
#include <TGraph.h>

#ifndef __CINT__
#include "Event.h"
#include "AnalysisParticle.hh"
#endif

namespace Cuts {

// FIXME document these numbers
// FIXME add instructions how to reproduce and monitor these numbers!
// FIXME move to tracker cuts
const unsigned int gTrackerCharges = 8;
const float gTrackerChargeMean[gTrackerCharges] =  {9.86033e-01, 1.99755e+00, 2.98898e+00, 3.96389e+00, 4.91655e+00, 5.90427e+00, 6.89230e+00, 7.87853e+00};
const float gTrackerChargeSigma[gTrackerCharges] = {6.18135e-02, 8.61350e-02, 1.21828e-01, 1.29896e-01, 1.49056e-01, 1.70152e-01, 1.78035e-01, 2.29481e-01};

// FIXME the following can be done via Selectors of existing cuts (almost....)

/** Select proton candidates **/
class CutProtonCandidate : public Cut {
public:
  CutProtonCandidate() : Cut( "Proton candidate" ) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( p.HasMainTrackerTrack() && p.HasMainTrackerTrackFit() ){
      if( p.Rigidity() > 0. ){
        return fabs(p.TrackerCharge() - gTrackerChargeMean[0]) < 3 * gTrackerChargeSigma[0];
      }
    }

    return false;
  }

  ClassDef(Cuts::CutProtonCandidate,1)
};

/** Remove anti-nuclei **/
class CutRemoveAntiNuclei  : public Cut {
public:
  CutRemoveAntiNuclei() : Cut("Remove anti-nuclei") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasMainTrackerTrackFit() ) return true;
    if (p.Rigidity() >= 0.0)
      return true;

    if( !p.HasMainTrackerTrack() ) return true;
    return p.TrackerCharge() < gTrackerChargeMean[0] + 3 * gTrackerChargeSigma[0];
  }

  ClassDef(Cuts::CutRemoveAntiNuclei,1)
};

/** Remove heavy-nuclei **/
class CutRemoveHeavyNuclei : public Cut {
public:
  CutRemoveHeavyNuclei(): Cut("Remove heavy nuclei") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasMainTrackerTrack() ) return true;

    return p.TrackerCharge() < gTrackerChargeMean[5] + 3 * gTrackerChargeSigma[5];
  }

  ClassDef(Cuts::CutRemoveHeavyNuclei,1)
};


/** Remove positrons **/
class CutRemovePositrons : public Cut {
public:
  CutRemovePositrons() : Cut("Remove positrons") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    AssureCutIsAppliedToACQtFile(p);

    const ACsoft::AC::ECALShower* shower = p.MainEcalShower();
    if( !shower ) return true;

    const ACsoft::AC::TrackerTrackFit* trackFit = p.MainTrackerTrackFit();
    if( !trackFit ) return true;
    if( trackFit->Rigidity() <= 0.0 )
      return true;

    return shower->Estimators().at(0) <= -0.2 && shower->DepositedEnergy() / trackFit->Rigidity() <= 0.5;
  }

  ClassDef(Cuts::CutRemovePositrons,1)
};
  

/** Remove anti-protons. **/
class CutRemoveAntiProtons  : public Cut {
public:
  CutRemoveAntiProtons() : Cut("Remove antiprotons") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    AssureCutIsAppliedToACQtFile(p);

    const ACsoft::AC::ECALShower* shower = p.MainEcalShower();
    if( !shower ) return true;

    const ACsoft::AC::TrackerTrackFit* trackFit = p.MainTrackerTrackFit();
    if( !trackFit ) return true;
    if (trackFit->Rigidity() >= 0.)
      return true;

    float bdt = shower->Estimators().at(0);
    if (bdt >= -1.0 && bdt < 0.0)
      return false;

    if (bdt >= -1.0 && shower->DepositedEnergy() < 1.0)
      return false;

    return true;
  }

  ClassDef(Cuts::CutRemoveAntiProtons,1)
};

/** Reduce misidentified helium.
  *
  * Cut acts on events that have Z(tracker) clearly larger than proton-like.
  * Events with Z(TOF) in the proton-like range are then removed.
  *
**/
class CutReduceMisidentifiedHelium : public Cut {
public:
  CutReduceMisidentifiedHelium() : Cut("Reduce misidentified helium") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if(!p.HasTofBeta()) return true;

    if( !p.HasMainTrackerTrack() ) return true;
    float trackerCharge = p.TrackerCharge();
    if (trackerCharge > gTrackerChargeMean[0] + 3 * gTrackerChargeSigma[0])
      return true;

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

    if( !p.HasMainEcalShower() ) return true;
    if( !p.HasMainTrackerTrackFit()) return true;
    return ValueIsInRange(p.EcalEnergy() / fabs(p.Rigidity()));
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

    const ACsoft::AC::TrackerTrackFit* trackFit = p.MainTrackerTrackFit();
    if( !trackFit ) return true;
    if( trackFit->Rigidity() > -2.0 ) return true;
    
    const ACsoft::AC::ECALShower* shower = p.MainEcalShower();
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
  TGraph* fCut90PercentEfficiency; // FIXME shouldn't this have a //! ?
};

}

#endif // PARTICLESELECTIONCUTS_HH
