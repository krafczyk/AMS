#ifndef SELECTOR_HH
#define SELECTOR_HH

#include <string>
#include <vector>

#include <TNamed.h>

namespace ACsoft {

namespace Analysis {
  class Particle;
}

}

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
  * \sa Cuts::Cut
  */
class Selector : public TNamed {
public:

  Selector( std::string description = "void" );
  virtual ~Selector();

  /** Register a cut to the selector.
    *
    */
  void RegisterCut( Cuts::Cut* cut );

  /** Decide whether particle passes all cuts and do the internal bookkeeping of the individual cuts.
    *
    */
  bool Passes( const ACsoft::Analysis::Particle& );

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

  ClassDef(Cuts::Selector,1)
};
}

#endif
