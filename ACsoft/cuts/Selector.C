#include "Selector.hh"

#include <Cut.hh>

#include "AnalysisParticle.hh"

#define DEBUG 0
#define INFO_OUT_TAG "Selector> "
#include <debugging.hh>

Cuts::Selector::Selector( std::string description ) :
  TNamed(description.c_str(),description.c_str()), // FIXME remove spaces and other unusual characters from object name
  fDescription(description),
  fTotalCounter(0),
  fPassedCounter(0),
  fFailedCounter(0) {

}


Cuts::Selector::~Selector() {

  unsigned int nCuts = fCuts.size();
  for( unsigned int i=0 ; i<nCuts ; ++i )
    delete fCuts[i];
}

void Cuts::Selector::RegisterCut( Cuts::Cut* cut )  {

  cut->SetIsStandaloneCut(false);
  fCuts.push_back( cut );
}


bool Cuts::Selector::Passes( const Analysis::Particle& evt ) {

  ++fTotalCounter;

  unsigned int nCuts = fCuts.size();
  bool passesAllCuts = true;
  bool passesAllPreviousCuts = true;
  unsigned int nCutsPassed = 0;

  // this is for the N-1 statistics of the individual cuts:
  // if in the end, the number of passed cuts will be equal to
  // nCuts - 1, we know that the cut at this position is the one
  // that exclusively caused the event to fail
  unsigned int lastCutFailed = 0;

  for( unsigned int i=0 ; i<fCuts.size() ; ++i ) {

    Cuts::Cut* cut = fCuts[i];

    bool passes = cut->Passes(evt);
    if( passes ) ++nCutsPassed;
    else lastCutFailed = i;

    passesAllCuts &= passes;

    if( !passes && passesAllPreviousCuts ){
      cut->FailedAfterPassingAllPreviousCuts();
      passesAllPreviousCuts = false;
    }

    if( !passesAllCuts && ( nCutsPassed == nCuts - 1 ) )
      fCuts.at(lastCutFailed)->FailedOnlyThisCut();
  }

  if( passesAllCuts ) ++fPassedCounter;
  else ++fFailedCounter;

  return passesAllCuts;
}


void Cuts::Selector::PrintSummary() const {

  INFO_OUT << "Summary for Selector \"" << fDescription << "\":" << std::endl;

  Cuts::Cut::PrintSummaryHeaderLine(std::cout);

  for( unsigned int i=0 ; i<fCuts.size() ; ++i )
    fCuts[i]->PrintSummary(std::cout);

  char passedPercentage[20];
  char failedPercentage[20];
  sprintf( passedPercentage, "(%.2f%%)", ( fTotalCounter ? 100.*float(fPassedCounter)/float(fTotalCounter) : 0. ) );
  sprintf( failedPercentage, "(%.2f%%)", ( fTotalCounter ? 100.*float(fFailedCounter)/float(fTotalCounter) : 0. ) );

  INFO_OUT << fDescription << ": " << fTotalCounter << " events, "
           << fPassedCounter << " passed " << passedPercentage << ", "
           << fFailedCounter << " failed " << failedPercentage << "." << std::endl;

}

bool Cuts::Selector::MergeStatisticsFrom( const Selector& other ) {

  if( fDescription != other.fDescription ){
    WARN_OUT << "Descriptions of this selector (" << fDescription << ") and second selector (" << other.fDescription << ") do not match!" << std::endl;
    return false;
  }

  if( fCuts.size() != other.fCuts.size() ){
    WARN_OUT << "Number of cuts in this selector (" << fCuts.size() << ") and second selector (" << other.fCuts.size() << ") do not match!" << std::endl;
    return false;
  }

  fTotalCounter  += other.fTotalCounter;
  fPassedCounter += other.fPassedCounter;
  fFailedCounter += other.fFailedCounter;

  for( unsigned int i=0 ; i<fCuts.size() ; ++i ){

    Cuts::Cut* myCut = fCuts.at(i);
    Cuts::Cut* otherCut = other.fCuts.at(i);

    myCut->MergeStatisticsFrom(*otherCut);
  }

  return true;
}

ClassImp(Cuts::Selector)
