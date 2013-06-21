#ifndef NMINUSONECUT_HH
#define NMINUSONECUT_HH

#include <assert.h>
#include <math.h>
#include <string>
#include <ostream>

#include <TObject.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TEfficiency.h>
#include "Cut.hh"

namespace Cuts {

/** N-1 histogram of the cut value vs. rigidity/energy.
  */
enum NMinusOneMode {
  NMinusOneVsRigidity,
  NMinusOneVsAbsRigidity,
  NMinusOneVsEnergy,
  NMinusOneVsAbsMcMomentum
};

class Selector;

/** Base class for cuts where N-1 logic can be applied (non-boolean cuts, on a single cut value).
  * You can not instanciate an NMinusOneCut directly, only classes inheriting from it, like TwoSidedCut.
  */
class NMinusOneCut : public Cut {
protected:
  explicit NMinusOneCut( std::string description = "void" );
  virtual ~NMinusOneCut();

  /** Special marker for 'fLastCutValue' / 'fLastRigidityOrEnergy' to indicate that N-1 cut values are not available */
  static const float gUnsetValue;

public:
  virtual bool IsNMinusOneCut() const { return true; }
  virtual bool IsManualNMinusOneCut() const { return false; }

  /** Returns a 2d-histogram of this cut variable vs. rigidity after all other cuts */
  TH2F* GetNMinusOneDistribution() const { return fNMinusOneDistribution; }

  /** Returns a 1d-histogram projecting the N-1 distribution on the rigidity, x-axis for all the events that passed all other cuts */
  TH1F* GetAllNMinusOneHisto() const { return fAllNMinusOneHisto; }

  /** Returns a 1d-histogram projecting the N-1 distribution on the rigidity, x-axis for all the events that passed all other cuts and also this cut. */
  TH1F* GetPassedNMinusOneHisto() const { return fPassedNMinusOneHisto; }

  /** Returns a graph denoting the efficiency of this cut vs rigidity, after applying all previous cuts ("last cut" efficiency). */
  TEfficiency* ProduceNMinusOneEfficiency() const;

  /** Create new N-1 histograms, for a given binning. Called from Selector::RegisterCut. */
  void CreateNMinusOneHistograms(const std::vector<double>& rigidityOrEnergyBinning, const std::vector<double>& cutValueBinning, NMinusOneMode);

  static std::string NMinusOneValueTitle(NMinusOneMode);

  void ReplaceNMinusOneHistograms(TH1F* passedNMinusOneHisto, TH1F* allNMinusOneHisto, TH2F* NMinusOneDistribution);

private:

  friend class Cuts::Selector;

  /** Called on every NMinusOneCut of a selector, before evaluating the cut.
    * If ValueIsInRange ends up _not_ being called (because cut is not applicable) then fLastCutValue/fLastRigidityOrEnergy are properly marked as not available.
   */
  void Reset() {

    fLastCutValue = gUnsetValue;
    fLastRigidityOrEnergy = gUnsetValue;
  }

  /** Called on this cut, if all other cuts of the selector passed.
    */
  void FillNMinusOneDistribution(bool passesThisCut) {

    if (fLastCutValue == gUnsetValue) {
      assert(fLastRigidityOrEnergy == gUnsetValue);
      return;
    }

    if (fAllNMinusOneHisto)
      fAllNMinusOneHisto->Fill(fLastRigidityOrEnergy);

    if (passesThisCut) {
      if (fPassedNMinusOneHisto)
        fPassedNMinusOneHisto->Fill(fLastRigidityOrEnergy);
    }

    if (fNMinusOneDistribution)
      fNMinusOneDistribution->Fill(fLastRigidityOrEnergy, fLastCutValue);
  }

protected:
  static double NMinusOneValue(const ACsoft::Analysis::Particle&, NMinusOneMode);

  /// 2d N-1 Distribution.
  TH2F* fNMinusOneDistribution;
  /// Member N-1 histogram of the Cut
  TH1F* fAllNMinusOneHisto;
  /// Member N-1 histogram of the Cut for all that also pass the last cut.
  TH1F* fPassedNMinusOneHisto;

  /// Member for the last cut value, rigidity/energy that was tested by the TestCondition call.
  float fLastCutValue;
  NMinusOneMode fNMinusOneMode;
  float fLastRigidityOrEnergy;

  ClassDef(Cuts::NMinusOneCut,1)
};

}

#endif // NMINUSONECUT_HH
