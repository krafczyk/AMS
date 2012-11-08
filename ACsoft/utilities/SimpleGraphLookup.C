#include "SimpleGraphLookup.hh"
#include "Settings.h"

#include <math.h>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <assert.h>
#include <iomanip>

#include <TFile.h>
#include <TGraph.h>
#include <TString.h>
#include <TMath.h>
#include <TTree.h>
#include <TTimeStamp.h>

#define DEBUG 0
#define INFO_OUT_TAG "SimpleGraphLookup> "
#include <debugging.hh>

/** Construct a SimpleGraphLookup from given filename.
  *
  * Use the environment variable \c ACROOTLOOKUPS to construct the full filename, i.e. \c $ACROOTLOOKUPS/lookupfile
  *
  * \param namePrefix Prefix of TGraph name to be used for lookup.
  * \param uncertaintyPrefix Name of TGraph containing the corresponding uncertainties. Set to empty string (\c "") to disable handling of uncertainties.
  * \param nMin Lowest number to be appended to \p namePrefix.
  * \param nMax Highest number to be appended to \p namePrefix.
  * \param mod2D For handling simple 2-dimensional lookups (see below). Set to zero (the default value) for the usual case of 1-D lookups.
  *
  * Example: The following code will instantiate a lookup that reads TGraph objects \c myGraph_1, \c myGraph_2, ..., \c myGraph_328 and
  * corresponding graphs \c myError_1, ..., of uncertainties from
  * the file \c file.root:
  *
  * For simple two-dimensional lookups, graph names are of the form \c prefix_X_Y, where X is in the range 0..Xmax, Y is in the range 0..Ymax.
  * In this case, the graphs can still be stored in a 1D array structure, and the correct suffixes when reading the graphs are created if the \p mod2D
  * parameter is set to \c Ymax+1. The user then has to make sure that the correct graphs are queried by calculating the number argument from \c X and \c Y.
  * For an example, see Utilities::TrdPdfLookup.
  *
  * \code
  * Utilities::SimpleGraphLookup("file.root", "myGraph", "myError", 1, 328);
  * \endcode
  *
  */
Utilities::SimpleGraphLookup::SimpleGraphLookup( std::string lookupfile, std::string expectedGitSHA, unsigned short expectedVersion, std::string namePrefix, std::string uncertaintyPrefix, unsigned int nMin, unsigned int nMax, unsigned int mod2D ) :
  fNumberOffset(nMin),
  fModulus2D(mod2D),
  fHasUncertainties(false),
  fLastQueryOk(true)
{

  const char* acrootlookups = getenv("ACROOTLOOKUPS");
  if( !acrootlookups ){
    WARN_OUT << "ACROOTLOOKUPS variable not set!" << std::endl;
    throw std::runtime_error("ACROOTLOOKUPS variable not set!");
  }

  std::stringstream filename;
  filename << acrootlookups << "/" << lookupfile;
  INFO_OUT << "Opening lookup file " << filename.str() << " to read \"" << namePrefix << "\"" << std::endl;
  TFile* file = new TFile( filename.str().c_str(), "READ" );
  if( !file->IsOpen() ){
    WARN_OUT << "ERROR opening lookup file " << filename <<  std::endl;
    throw std::runtime_error("ERROR opening lookup file.");
  }

  // FIXME: Once we've produced new lookups, always rely on the existance of a 'ACQtVersion' object!
  AC::ACQtLookupFileIdentifier* acqtVersion = (AC::ACQtLookupFileIdentifier*) file->Get("ACQtVersion");
  if (acqtVersion) {
    if (acqtVersion->fGitSHA != expectedGitSHA) {
      WARN_OUT << "ERROR validating lookup file " << filename << ". File mismatch! Expected Git SHA: \""
               << expectedGitSHA << "\". Actual Git SHA: \"" << acqtVersion->fGitSHA << "\"" << std::endl;
      throw std::runtime_error("ERROR validating lookup file.");
    }

    if (acqtVersion->fVersion != expectedVersion) {
      WARN_OUT << "ERROR validating lookup file " << filename << ". File mismatch! Expected version: \""
               << expectedVersion << "\". Actual version: \"" << acqtVersion->fVersion << "\"" << std::endl;
      throw std::runtime_error("ERROR validating lookup file.");
    }
  }

  LoadGraphs( file, namePrefix, nMin, nMax, fGraphs );

  if( uncertaintyPrefix != "" ){
    fHasUncertainties = true;
    LoadGraphs( file, uncertaintyPrefix, nMin, nMax, fUncertaintyGraphs );
  }

  if( !TestConsistency() ) fHasUncertainties = false;

  delete file; file = 0;
}

/** Construct a lookup from a tree stored in a ROOT file.
 *
 */
Utilities::SimpleGraphLookup::SimpleGraphLookup( std::string treefile, std::string expectedGitSHA, unsigned short expectedVersion,
                                                 std::string treename, std::string nameOfYBranch ) :
  fNumberOffset(0),
  fHasUncertainties(false),
  fLastQueryOk(true)
{

  // FIXME: Convert into SlowControlLookup proper!

  const char* acrootlookups = getenv("ACROOTLOOKUPS");
  if( !acrootlookups ){
    WARN_OUT << "ACROOTLOOKUPS variable not set!" << std::endl;
    throw std::runtime_error("ACROOTLOOKUPS variable not set!");
  }

  std::stringstream filename;
  filename << acrootlookups << "/" << treefile;
  INFO_OUT << "Using lookup file " << filename.str() << std::endl;

  // open file
  TFile* file = new TFile(filename.str().c_str(), "READ");
  if (!file->IsOpen()) {
    WARN_OUT << "Error reading file " << filename.str() << std::endl;
    throw std::runtime_error("ERROR opening lookup file!");
  }

  // FIXME: Once we've produced new lookups, always rely on the existance of a 'ACQtVersion' object!
  AC::ACQtLookupFileIdentifier* acqtVersion = (AC::ACQtLookupFileIdentifier*) file->Get("ACQtVersion");
  if (acqtVersion) {
    if (acqtVersion->fGitSHA != expectedGitSHA) {
      WARN_OUT << "ERROR validating lookup file " << filename << ". File mismatch! Expected Git SHA: \""
               << expectedGitSHA << "\". Actual Git SHA: \"" << acqtVersion->fGitSHA << "\"" << std::endl;
      throw std::runtime_error("ERROR validating lookup file.");
    }

    if (acqtVersion->fVersion != expectedVersion) {
      WARN_OUT << "ERROR validating lookup file " << filename << ". File mismatch! Expected version: \""
               << expectedVersion << "\". Actual version: \"" << acqtVersion->fVersion << "\"" << std::endl;
      throw std::runtime_error("ERROR validating lookup file.");
    }
  }

  // get tree
  TTree* tree = (TTree*)file->Get(treename.c_str());
  if (!tree) {
    WARN_OUT << "ERROR reading tree " << treename << " from file " << treefile << std::endl;
    throw std::runtime_error("ERROR getting tree!");
  }

  // set branches
  unsigned long long t; // should be of type time_t, but doesn't work well with CINT
  double value;
  tree->SetBranchAddress("time", &t);
  tree->SetBranchAddress(nameOfYBranch.c_str(), &value);


  // loop over entries
  TGraph* gr     = new TGraph;
  gr->SetName(nameOfYBranch.c_str());
  gr->SetTitle(nameOfYBranch.c_str());

  unsigned int nEntries = tree->GetEntries();

  tree->GetEntry(0);
  TTimeStamp tStart = TTimeStamp(t);
  tree->GetEntry(nEntries-1);
  TTimeStamp tEnd     = TTimeStamp(t);
  INFO_OUT << "Reading branch " << nameOfYBranch << " between:" << std::endl;
  tStart.Print();
  tEnd.Print();

  for (unsigned int i = 0; i < nEntries; i++) {
    tree->GetEntry(i);

    gr->SetPoint(gr->GetN(), t, value);
  }

  fGraphs.push_back(gr);

  file->Close();
}


Utilities::SimpleGraphLookup::~SimpleGraphLookup() {

  for( unsigned int i=0 ; i<fGraphs.size() ; ++i )
    if( fGraphs[i] )
      delete fGraphs[i];

  for( unsigned int i=0 ; i<fUncertaintyGraphs.size() ; ++i )
    if( fUncertaintyGraphs[i] )
      delete fUncertaintyGraphs[i];
}


void Utilities::SimpleGraphLookup::LoadGraphs( TFile* file, std::string prefix, unsigned int nMin, unsigned int nMax, std::vector<TGraph*>& target ) {

  assert(nMin<=nMax);

  target.assign(nMax-nMin+1,0);

  for( unsigned int i=nMin ; i<=nMax ; ++i ){
    std::stringstream name;
    name << prefix << "_" << Suffix(i);
    DEBUG_OUT << "Reading graph " << name.str() << std::endl;
    TGraph* gr = dynamic_cast<TGraph*>(file->Get(name.str().c_str()));
    if( !gr ){
      WARN_OUT << "Missing graph " << name.str() << std::endl;
      continue;
    }

#ifndef AMS_ACQT_SILENCE_COMMON_WARNINGS
    if( gr->GetN() == 0)
      WARN_OUT << "Zero points in graph " << name.str() << std::endl;
#endif

    unsigned int index = i - fNumberOffset;
    target.at(index) = gr;
  }
}


/** Construct the correct suffix for given number.
  *
  * This can be used to represent a 2D lookup structure in our 1D-vector of TGraphs.
  *
  */
std::string Utilities::SimpleGraphLookup::Suffix( unsigned int graphNumber ) {

  std::stringstream suf;

  // usual (1D) case
  if(!fModulus2D)
    suf << graphNumber;
  else{
    unsigned int firstN  = graphNumber / fModulus2D;
    unsigned int secondN = graphNumber % fModulus2D;
    suf << firstN << "_" << secondN;
  }

  DEBUG_OUT << "Suffix for number " << graphNumber << ": " << suf.str() << std::endl;

  return suf.str();
}


/** Make sure that value and uncertainty lookups (if any) are consistent.
 *
 * Check that number of points in graphs and x positions match.
 */
bool Utilities::SimpleGraphLookup::TestConsistency() {

  const Double_t tolerance = 1.e-4;

  if( fUncertaintyGraphs.empty() ) return true;

  if( fUncertaintyGraphs.size() != fGraphs.size() ){
    WARN_OUT << "Number of graphs does not match!" << std::endl;
    return false;
  }

  for( unsigned int gr=0 ; gr < fGraphs.size() ; ++gr ){

    TGraph* valueGraph = fGraphs.at(gr);
    TGraph* errorGraph = fUncertaintyGraphs.at(gr);

    if( valueGraph->GetN() != errorGraph->GetN() ){
      WARN_OUT << "Number of points for graphs #" << gr << " do not match!" << std::endl;
      return false;
    }

    for( Int_t n=0 ; n<valueGraph->GetN() ; ++n ){
      Double_t x1,y1,x2,y2;
      valueGraph->GetPoint(n,x1,y1);
      errorGraph->GetPoint(n,x2,y2);
      if( fabs(x1-x2) > tolerance ) {
        WARN_OUT << "Mismatch in x values for graph #" << gr << ": " << x1 << " vs " << x2 << std::endl;
        return false;
      }
    }
  }

  return true;
}

/** Query the lookup.
  *
  * Perform linear interpolation between the two points bracketing \p x.
  *
  * \param number Integer used in the name of the lookup graph, i.e. must be within the interval [\c nMin,\c nMax] used when constructing the lookup.
  * \param x Point on x-axis of lookup graph where value should be retrieved.
  * \param needUncertainty If only the value itself is needed, but not the associated uncertainty, set this parameter to \c false, in order to save CPU time.
  *
  * An out-of-bounds check is performed for \p x (but not for \p number), and the first (or last) y-value is returned if \p x is below (or above) the first (or last) point.
  * Returns zero in case lookup graph has no entries.
  *
  */
Quantity Utilities::SimpleGraphLookup::Query( unsigned int number, double x, bool needUncertainty ) const {

  unsigned int index = number - fNumberOffset;
  TGraph* gr    = fGraphs.at(index);
  TGraph* uncgr = ( fHasUncertainties && needUncertainty ? fUncertaintyGraphs.at(index) : 0 );

  if(!gr->GetN()){
    DEBUG_OUT << "Zero points in graph number " << number << std::endl;
    fLastQueryOk = false;
    return Quantity(0.,0.);
  }

  // perform binary search for bracketing points
  Long64_t iLow = TMath::BinarySearch(gr->GetN(), gr->GetX(), x);

  // if uncertainty graphs are present, we can assume that the x-positions of the points are the same as for the
  // value graphs (TestConsistency() made sure that this is indeed the case)

  // retrieve numbers needed for interpolation
  Double_t x0,x1,y0,y1,dummy;
  Double_t e0 = 0.;
  Double_t e1 = 0.;
  if( iLow >= 0 && iLow < (gr->GetN()-1) ){
    gr->GetPoint(iLow,x0,y0);
    gr->GetPoint(iLow+1,x1,y1);
    if(uncgr){
      uncgr->GetPoint(iLow,dummy,e0);
      uncgr->GetPoint(iLow+1,dummy,e1);
    }
  }
  else if( iLow < 0 ){
#ifndef AMS_ACQT_SILENCE_COMMON_WARNINGS
    if(fNumberOfWarnings[number] < 2)
      WARN_OUT << "Value x=" << std::setprecision(10) << x << " too low in number " << number << " !" << std::endl;
    fNumberOfWarnings[number]++;
    if(fNumberOfWarnings[number] == 2)
      WARN_OUT << "Suppressing further warnings for number " << number << std::endl;
#endif

    gr->GetPoint(0,x0,y0);
    gr->GetPoint(0,x1,y1);
    if(uncgr){
      uncgr->GetPoint(0,dummy,e0);
      uncgr->GetPoint(0,dummy,e1);
    }
  }
  else if( iLow >= (gr->GetN()-1) ){
#ifndef AMS_ACQT_SILENCE_COMMON_WARNINGS
    if(fNumberOfWarnings[number] < 2)
      WARN_OUT << "Value x=" << std::setprecision(10) << x << " too high in number " << number << " !" << std::endl;
    fNumberOfWarnings[number]++;
    if(fNumberOfWarnings[number] == 2)
      WARN_OUT << "Suppressing further warnings for number " << number << std::endl;
#endif

    int lastPoint = gr->GetN()-1;
    gr->GetPoint(lastPoint,x0,y0);
    gr->GetPoint(lastPoint,x1,y1);
    if(uncgr){
      uncgr->GetPoint(lastPoint,dummy,e0);
      uncgr->GetPoint(lastPoint,dummy,e1);
    }
  }

  // linear interpolation
  Double_t value = ( x0 == x1 ? y0 : (y1*(x-x0) - y0*(x-x1)) / (x1-x0) );

  Double_t uncertainty = e0; // value for x0==x1
  if( x0 != x1 && needUncertainty ){
    // assume two Gaussian random variables with given value and uncertainty at the two corner points
    // and calculate the variance of the interpolated random variable.
    double d = x0-x1;
    double n = e0*e0*(x1*x1 - 2.*x1*x + x*x) + e1*e1*(x*x - 2.*x*x0 + x0*x0);
    uncertainty = sqrt(n/d/d);
  }

  Quantity result(value,uncertainty);

  DEBUG_OUT << "n " << number << " x " << std::setprecision(10) << x << ": "
            << gr->GetName() << " -> x0=" << std::setprecision(10) << x0
            << " x1=" << std::setprecision(10) << x1
            << " y0=" << y0 << "+-" << e0
            << " y1=" << y1 << "+-" << e1 << " -> "
            << result << std::endl;

  fLastQueryOk = true;
  return result;
}



/** Find the distance between the two points used for interpolation at point \p x for graph identified by \p number.
  *
  * \param number Integer used in the name of the lookup graph, i.e. must be within the interval [\c nMin,\c nMax] used when constructing the lookup.
  * \param x Point on x-axis of lookup graph where distance to neighbouring points in graph should be retrieved.
  *
  * \returns Distance (in units of x-axis values), or a negative value if two neighbouring points cannot be found.
  */
Double_t Utilities::SimpleGraphLookup::DistanceBetweenPoints( unsigned int number, double x ) const {

  unsigned int index = number - fNumberOffset;
  TGraph* gr    = fGraphs.at(index);

  if(!gr->GetN()){
    DEBUG_OUT << "Zero points in graph number " << number << std::endl;
    return -2.0;
  }

  // perform binary search for bracketing points
  Long64_t iLow = TMath::BinarySearch(gr->GetN(), gr->GetX(), x);

  Double_t x0,x1,y0,y1;
  if( iLow >= 0 && iLow < (gr->GetN()-1) ){
    gr->GetPoint(iLow,x0,y0);
    gr->GetPoint(iLow+1,x1,y1);

    // the standard case
    return (x1-x0);
  }
  else if( iLow < 0 ){
    DEBUG_OUT << "Value x=" << std::setprecision(10) << x << " too low in number " << number << " !" << std::endl;
    return -3.0;
  }
  else if( iLow >= (gr->GetN()-1) ){
    DEBUG_OUT << "Value x=" << std::setprecision(10) << x << " too high in number " << number << " !" << std::endl;
    return -4.0;
  }

  return -1.0;
}



/** Find the number of entries in graph identified by \p number.
  *
  * \param number Integer used in the name of the lookup graph, i.e. must be within the interval [\c nMin,\c nMax] used when constructing the lookup.
  */
Int_t Utilities::SimpleGraphLookup::NumberOfEntries( unsigned int number ) const {

  unsigned int index = number - fNumberOffset;
  TGraph* gr = fGraphs.at(index);

  return gr->GetN();
}


/** Find the x-value of the first entry in graph identified by \p number.
  *
  * \param number Integer used in the name of the lookup graph, i.e. must be within the interval [\c nMin,\c nMax] used when constructing the lookup.
  */
Double_t Utilities::SimpleGraphLookup::FirstEntryX( unsigned int number ) const {

  unsigned int index = number - fNumberOffset;
  TGraph* gr = fGraphs.at(index);

  if(!gr->GetN()){
#ifndef AMS_ACQT_SILENCE_COMMON_WARNINGS
    WARN_OUT << "Zero points in graph number " << number << std::endl;
#endif
    return 0.;
  }

  Double_t x,y;
  gr->GetPoint(0,x,y);

  return x;
}


/** Find the x-value of the last entry in graph identified by \p number.
  *
  * \param number Integer used in the name of the lookup graph, i.e. must be within the interval [\c nMin,\c nMax] used when constructing the lookup.
  */
Double_t Utilities::SimpleGraphLookup::LastEntryX( unsigned int number ) const {

  unsigned int index = number - fNumberOffset;
  TGraph* gr = fGraphs.at(index);

  if(!gr->GetN()){
#ifndef AMS_ACQT_SILENCE_COMMON_WARNINGS
    WARN_OUT << "Zero points in graph number " << number << std::endl;
#endif
    return 0.;
  }

  Double_t x,y;
  gr->GetPoint(gr->GetN()-1,x,y);

  return x;
}
