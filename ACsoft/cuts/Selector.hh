#ifndef SELECTOR_HH
#define SELECTOR_HH

#include <string>
#include <vector>

#include <TNamed.h>
#include <TH1D.h>
#include <TCanvas.h>

#include "NMinusOneCut.hh"

namespace Cuts {
 
class Cut;

/** Select or reject particles based on a group of cuts.
  *
  * Use RegisterCut() with different cuts to assemble a list of selection criteria. If possible, add the cuts in a logical order,
  * from general to more specific. This will make the cut statistics more meaningful in the end.
  *
  * In the main event loop, select particles using the Passes() function.
  *
  * After the event loop, you can use the PrintSummary() function to get detailed cut statistics.
  *
  * Example usage:
  * \code
  * Cuts::Selector preselection("Preselection");
  * preselection.RegisterCut( new Cuts::CutSingleParticle );
  * preselection.RegisterCut( new Cuts::CutSingleTrack );
  * (...)
  * while (filemanager.GetNextEvent()) {
  *   AC::Event* event = fm.Event();
  *
  * ACsoft::Analysis::Particle particle;
  * particlefactory->PrepareWithDefaultObjects(*event,particle);
  *
  * if( !preselection.Passes(particle) ) continue;
  *
  *   (...)
  * }
  * (...)
  * preselection.PrintSummary();
  * \endcode
  *
  * \attention Every time the Passes() function is called, all the cuts do their internal bookkeeping. The function must
  * therefore only be called in the main event loop and once per event.
  *
  * For the N-1 Logic pass a binning to RegisterCut(). You have to give a binning in y for the specific cut variable.
  * If you did that, you can just call the N-1 related functions after the event loop to create efficiency graphs.
  *
  * \sa Cuts::Cut
  * \sa Cuts::Selector::MakeNMinusOneEfficiencyCanvas
  */
class Selector : public TNamed {
public:

  Selector(std::string description = "void" );
  virtual ~Selector();

  /** Register a cut to the selector.
    * Using this function, no N-1 histograms will be created.
    */
  void RegisterCut(Cuts::Cut*);

  /** Register a cut to the selector.
    * N-1 histograms will be registered, which makes it possible to compute efficiencies for this selector.
    */
  void RegisterCut(Cuts::Cut*, const std::vector<double>& rigidityOrEnergyBinning, const std::vector<double>& cutValueBinning, NMinusOneMode);

  /** Decide whether particle passes all cuts and do the internal bookkeeping of the individual cuts.
    * \param fillNMinusOneHistogramsIfPossible Default: true. You can set this to false, to force disabling filling of N-1 histograms.
    *                                          This is useful if you want to fill N-1 histograms, only under a certain condition.
    *                                          (eg. exclude trigger selector from preselection, to study trigger eff., but only fill N-1 histograms, if the selector passed,
    *                                           to study the N-1 distribution of cut variables and the trigger efficiency in the same program, looping only once over the data).
    */
  bool Passes(const ACsoft::Analysis::Particle&, bool fillNMinusOneHistogramsIfPossible = true );

  /** Returns a canvas with all the efficiencies vs. time assosiated with the selector plotted in one canvas.
    * \sa Cuts::ProduceTimeEfficiency for details on the 'mergeBins' parameter.
    */
  TCanvas* MakeTimeEfficiencyCanvas(const std::string& canvasName, const std::string& canvasTitle, unsigned int mergeBins = 1);

  /** Returns a canvas with all the efficiencies vs. rigidity/energy assoiated with the selector plotted in one canvas.
    * Only applicable in the N-1 case.
    */
  TCanvas* MakeNMinusOneEfficiencyCanvas(NMinusOneMode, const std::string& canvasName, const std::string& canvasTitle);

  /** Print a summary of cut statistics for all cuts.
    *
    */
  void PrintSummary() const;

  /** Merge cut statistics of this Selector and a second one.
    *
    * Sums the total numbers of passed and failed events and
    * calls the Cut::MergeStatisticsFrom(const Cut&) function
    * for all individual cuts.
    *
    */
  bool MergeStatisticsFrom( const Selector& other );

  std::string Description() const { return fDescription; }

  unsigned int NumberOfCuts() const { return fCuts.size(); }
  Cut* GetCut( unsigned int number ) { return fCuts.at(number); }

protected:

  /// a short summary of what the present group of cuts does
  std::string fDescription;

  std::vector<Cuts::Cut*> fCuts;

  long fTotalCounter;
  long fPassedCounter;
  long fFailedCounter;

  ClassDef(Cuts::Selector,2)
};
}

#endif
