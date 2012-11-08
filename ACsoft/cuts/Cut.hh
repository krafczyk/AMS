#ifndef CUT_HH
#define CUT_HH

#include <string>
#include <ostream>

#include <TObject.h>
#include <TH1F.h>

namespace Analysis {
  class Particle;
}

namespace Cuts {

class Selector;

/** Base class for cuts on event data.
  *
  * All cuts used to select events should inherit from this class
  * and implement the TestCondition() method.
  *
  * A Cut can either be used as part of an Selector or as a standalone cut.
  * In the former case, advanced bookkeeping is done including N-1 statistics to study the
  * overlap between different cuts. The latter case is useful in situations that are strictly
  * sequential, i.e. one condition can only be tested if another one has been fulfilled before.
  * Example: First check that a track fit exists, then check the track fit chisquared. In a situation
  * like that, the Selector is not very useful. Instead, a ManualCut will usually be used.
  *
  * Every cut can be implemented as a thin sub-class that inherits from this class, for example:
  * \code
  * class CutSingleParticle : public Cut {
  * public:
  * CutSingleParticle() : Cut("SingleParticle") { }
  *
  * virtual bool TestCondition( const Analysis::Particle& p ) { return p.RawEvent()->Particles().size() == 1; }
  * };
  * \endcode
  *
  *
  * \attention Every time the PassesEvent() function is called, the cut does its internal bookkeeping. The function must
  * therefore only be called in the main event loop or by the Selector.
  *
  * \note Often, a cut has to rely on certain conditions that the event has to fulfill before the cut can be applied.
  * It is important that the TestCondition() functions returns \c true if these conditions are not fulfilled, lest we
  * do not get meaningful cut statistics if the cut is registered with an Selector. The following example illustrates
  * the point:
  * \code
  * class CutDowngoing : public Cut {
  * public:
  * CutDowngoing() : Cut("Downward-going particle") { }
  *
  * virtual bool TestCondition( const Analysis::Particle& p ) {
  *   if( p.RawEvent()->Particles().size() > 0 )
  *     return p.Particles()[0].Momentum() > 0.5;
  *   return true;
  * }
  * };
  * \endcode
  * We want to check if the main particle in the event (\c evt.Particles()[0]) has a velocity that indicates the particle as
  * down-going. However, we have to test before whether evt.Particles()[0] even exists. If it does, we can apply the test on velocity
  * and let the event pass or fail based on the result. But if the particle does not exist, this cut has no meaning and should
  * not interfere with the other cuts. Instead, we should use a dedicated cut before, e.g. a CutSingleParticle, which makes sure
  * that we do have a main event in all events that eventually pass the event selection. This procedure makes the N-1 statistics
  * that the Selector does much more meaningful and precise.
  *
  * \sa Cuts::Selector
  *
  *
  */
class Cut : public TObject {
// FIXME check docu
public:

  explicit Cut( std::string description = "void" );
  virtual ~Cut();

  /** Decide if a particle passes the cut.
    *
    * This function internally calls the TestCondition() function of
    * the individual cut to see if the event survives the cut. In addition,
    * it does the bookkeeping on event statistics.
    *
    */
  bool Passes( const Analysis::Particle& );

  /** Print a summary of the cut statistics.
    *
    */
  virtual void PrintSummary( std::ostream& out ) const;

  /** Print a nice header line that matches the format of PrintSummary().
    *
    */
  static void PrintSummaryHeaderLine( std::ostream& out );

  /** Get the cut description.
    *
    */
  virtual std::string Description() const { return fDescription; }


  TH1F* GetPassedTimeHisto() const { return fPassedTimeHisto; }
  TH1F* GetFailedTimeHisto() const { return fFailedTimeHisto; }

  void ReplacePassedHisto( TH1F* histo );
  void ReplaceFailedHisto( TH1F* histo);

private:

  friend class Cuts::Selector;

  /** Function for use by the Selector.
    *
    */
  void FailedOnlyThisCut() { ++fFailedOnlyThisCutCounter; }

  /** Function for use by the Selector.
    *
    */
  void FailedAfterPassingAllPreviousCuts() { ++fFailedAfterPassingAllPreviousCutsCounter; }

  /** Function for use by the Selector.
    *
    */
  void SetIsStandaloneCut( bool val ) { fIsStandaloneCut = val; }

  /** Merge the cut statistics (and cut histograms, if any) from this cut and a second one.
    *
    */
  void MergeStatisticsFrom( const Cut& other );

protected:

  /** Check if a particle passes this cut.
    *
    * This function has to be implemented by all sub-classes.
    */
  virtual bool TestCondition( const Analysis::Particle& ) = 0;

protected:

  /// a short explanation of what the cut does
  std::string fDescription;

  /// number of particles that have been subjected to the cut
  long fTotalCounter;
  /// number of particles that have passed the cut
  long fPassedCounter;
  /// number of events that have failed the cut
  long fFailedCounter;

  /// number of particles that have failed this cut but passed all others (needs grouping
  /// of cuts by using an Selector)
  long fFailedOnlyThisCutCounter;
  /// Number of particles that have failed this cut but passed all previous ones (needs grouping
  /// of cuts by using an Selector, which remembers the order that cuts were registered in).
  /// This number will only make sense in situations where cuts can be defined in some logical order.
  long fFailedAfterPassingAllPreviousCutsCounter;

  /// should be set to \c true if cut is not part of a group of cuts
  bool fIsStandaloneCut;

  /// \todo implement me
  bool fUsesRigidityBookkeeping;
  /// \todo implement me
  bool fUsesChargeBookkeeping;

  /// fill if Passes() == true
  TH1F* fPassedTimeHisto;
  /// fill if Passes() == false
  TH1F* fFailedTimeHisto;

  /// CutCount fot this cut instance (for unambiguous Root Histogram name)
  int fCutCount;

  /// CutCounter of all instances
  static int sCutCounter;

  ClassDef(Cuts::Cut,1)
};
}

#endif // CUT_HH
