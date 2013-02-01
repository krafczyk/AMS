#ifndef CUT_HH
#define CUT_HH

#include <string>
#include <ostream>

#include <TObject.h>
#include <TH1F.h>

namespace ACsoft {

namespace Analysis {
  class Particle;
}

}

namespace Cuts {

class Selector;

/** Base class for cuts on event and particle data.
  *
  * All cuts used to select particles should inherit from this class
  * and implement the TestCondition() method.
  *
  * A Cut can either be used as part of an Selector or as a standalone cut.
  * In the former case, advanced bookkeeping is done including N-1 statistics to study the
  * overlap between different cuts. The latter case is useful in situations that are strictly
  * sequential, i.e. one condition can only be tested if another one has been fulfilled before.
  *
  * Every cut is implemented as a thin sub-class that inherits from this class, for example:
  * \code
  * class CutSingleParticle : public Cut {
  * public:
  * CutSingleParticle() : Cut("SingleParticle") { }
  *
  * virtual bool TestCondition( const ACsoft::Analysis::Particle& p ) { return p.RawEvent()->Particles().size() == 1; }
  *
  * ClassDef(Cuts::CutSingleParticle,1)
  * };
  * \endcode
  *
  * Cuts operate on ACsoft::Analysis::Particle objects to give the user fine-grained control over the analyis. While this is natural for
  * most of the cuts that we do, a few (especially preselecion) cuts only make sense in the context of an entire event (e.g., a cut on the
  * number of tracker tracks), but this is a minor logical inconsistency that should not hurt analyses in any way.
  *
  * \attention Every time the Passes() function is called, the cut does its internal bookkeeping. The function must
  * therefore only be called in the main event loop or by the Selector.
  *
  * \note Often, a cut has to rely on certain conditions that the particle has to fulfill before the cut can be applied.
  * It is important that the TestCondition() functions returns \c true if these conditions are not fulfilled, lest we
  * do not get meaningful cut statistics if the cut is registered with a Selector. The following example regarding a cut on ECAL energy
  * illustrates the point:
  * \code
  * class CutEcalEnergy : public TwoSidedCut {
  * (...)
  * virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {
  *
  *   if( !p.MainEcalShower() ) return true;
  *   return ValueIsInRange( p.MainEcalShower()->ReconstructedEnergy());
  * }
  * (...)
  * };
  * \endcode
  * We want to check if the %ECAL energy in the shower associated with the given particle velocity falls inside a predefined range.
  * However, we have to test before whether there even exists an associated shower. If there is, we can apply the test on energy
  * and let the particle pass or fail based on the result. But if the shower does not exist, this cut cannot be applied and should
  * not interfere with the other cuts. (Imagine you want to select proton candidates: If they do not have an %ECAL shower, all the better. If they do, we can cut on the energy
  * and get rid of particles with a high (not proton-like) energy deposition in the %ECAL.) Instead, if we only want to consider particles that have an %ECAL shower,
  * we should use a dedicated cut before, e.g. a CutHasEcalShower, which makes sure
  * that we do have a main shower in all particles that eventually pass the selection. This procedure also makes the N-1 statistics
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
    * the individual cut to see if the particle survives the cut. In addition,
    * it does the bookkeeping on particle statistics.
    *
    */
  bool Passes( const ACsoft::Analysis::Particle& );

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
  virtual bool TestCondition( const ACsoft::Analysis::Particle& ) = 0;

  /** Helper function which aborts the program if a Cut that needs
    * full ACQt data structures is applied to ACROOT files.
    */
  void AssureCutIsAppliedToACQtFile( const ACsoft::Analysis::Particle& );

protected:

  /// a short explanation of what the cut does
  std::string fDescription;

  /// number of particles that have been subjected to the cut
  long fTotalCounter;
  /// number of particles that have passed the cut
  long fPassedCounter;
  /// number of particles that have failed the cut
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
