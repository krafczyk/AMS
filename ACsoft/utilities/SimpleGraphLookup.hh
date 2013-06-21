#ifndef SIMPLEGRAPHLOOKUP_HH
#define SIMPLEGRAPHLOOKUP_HH

#include <string>
#include <vector>
#include <map>
#include <TObject.h>

#include "Quantity.hh"

class TFile;
class TGraph;

namespace ACsoft {

namespace Utilities {

/** A lookup that operates on TGraph objects with common name prefix and stored in the same ROOT file.
  *
  * Graph names are expected to be \c namePrefix_N where \c N is an integer number in the interval [\c nMin,\c nMax].
  *
  * Graphs are read from the lookup file at construction time and stored in the \c fGraphs vector.
  *
  * An additional TGraph containing the uncertainties to the values in the lookup graph can optionally be considered.
  *
  * The lookup can be queried using the Query() function.
  * When the lookup is queried, interpolation in x is done. Since the number of points is expected to be large, we have
  * to be smarter than simply looping over all points (as done by ROOT) when finding the two points closest to \c x for
  * the interpolation.
  *
  * After a query, a call to the LastQueryOk() function will reveal if none of the following problems has occurred:
  *  - Missing lookup graph
  *  - Empty lookup graph
  *  - x value outside range of lookup graph
  *
  *
  * \test \c utilities/test/test_SimpleGraphLookup.C
  */
class SimpleGraphLookup
{
public:
    SimpleGraphLookup( TFile* lookupfile,
                       std::string namePrefix, std::string uncertaintyPrefix, unsigned int nMin, unsigned int nMax, unsigned int mod2D = 0 );
    SimpleGraphLookup( std::string lookupfile, std::string expectedGitSHA, unsigned short expectedVersion,
                       std::string namePrefix, std::string uncertaintyPrefix, unsigned int nMin, unsigned int nMax, unsigned int mod2D = 0 );
    SimpleGraphLookup( std::string treefile, std::string expectedGitSHA, unsigned short expectedVersion,
                       std::string treename, std::string nameOfYBranch );

    virtual ~SimpleGraphLookup();

    virtual Quantity Query( unsigned int number, double x, bool needUncertainty = true ) const;
    virtual bool     LastQueryOk() const { return fLastQueryOk; }

    /// convenience function
    virtual double QueryTreeGraph( double time ) const { return Query(0,time,false).value; } // FIXME: to be removed

    virtual Double_t DistanceBetweenPoints( unsigned int number, double x ) const;
    virtual Int_t    NumberOfEntries( unsigned int number ) const;
    virtual Double_t FirstEntryX( unsigned int number ) const;
    virtual Double_t LastEntryX( unsigned int number ) const;

private:

    void LoadGraphs( TFile* file, std::string prefix, unsigned int nMin, unsigned int nMax, std::vector<TGraph*>& target );
    std::string Suffix( unsigned int graphNumber );

    bool TestConsistency();

protected:

    /// used to find the correct vector element, in case \c nMin \> 0
    unsigned int fNumberOffset;
    /// for handling simple 2-dimensional lookups
    unsigned int fModulus2D;
    /// lookup graphs
    std::vector<TGraph*> fGraphs;
    /// graphs containing corresponding uncertainty (using a separate TGraph instead of one
    /// TGraphErrors saves the data structure for the x-errors (which are presumably zero))
    std::vector<TGraph*> fUncertaintyGraphs;
    /// flag for fast test if uncertainty graphs are present
    bool fHasUncertainties;
    /// keep track of the number of warnings issued for each graph
    mutable std::map<unsigned int, unsigned int> fNumberOfWarnings;

    /// keep track of whether the last query had any problems (e.g. graph with zero number of points)
    mutable bool fLastQueryOk;
};

bool IsBeamTestTime(double time);

}

}

#endif // SIMPLEGRAPHLOOKUP_HH
