#include "Cut.hh"
#include "TimeHistogramManager.hh"

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <sstream>

#define DEBUG 0
#define INFO_OUT_TAG "Cut> "
#include <debugging.hh>

#ifndef __CINT__
#include "Event.h"
#include "AnalysisParticle.hh"
#endif

int Cuts::Cut::sCutCounter=0; // initialize Cut instance counter

Cuts::Cut::Cut( std::string description ) :
  TObject(),
  fDescription(description),
  fTotalCounter(0),
  fPassedCounter(0),
  fFailedCounter(0),
  fFailedOnlyThisCutCounter(0),
  fFailedAfterPassingAllPreviousCutsCounter(0),
  fIsStandaloneCut(true),
  fUsesRigidityBookkeeping(false),
  fUsesChargeBookkeeping(false),
  fPassedTimeHisto(0),
  fFailedTimeHisto(0) {

  fCutCount = sCutCounter;
  sCutCounter++;
}

Cuts::Cut::~Cut() {

}

bool Cuts::Cut::Passes( const Analysis::Particle& evt ) {

  bool passes = TestCondition(evt);

  ++fTotalCounter;

  if(!fPassedTimeHisto){
    std::stringstream name, title;
    name << "PassedTimeHisto_" << TString(fDescription.c_str()).ReplaceAll(" ","_").ReplaceAll(":","").Data() << "_" << fCutCount;
    title << "Particles passed vs time: " << fDescription;
    DEBUG_OUT << "Book time histos for cut-count: " << fCutCount << ":" << name.str() << std::endl;
    fPassedTimeHisto = Utilities::TimeHistogramManager::MakeNewTimeHistogram1D<TH1F>( name.str().c_str(), title.str().c_str() );
  }
  if(!fFailedTimeHisto){
    std::stringstream name, title;
    name << "FailedTimeHisto_" << TString(fDescription.c_str()).ReplaceAll(" ","_").ReplaceAll(":","").Data() << "_" << fCutCount;
    title << "Particles failed vs time: " << fDescription;
    fFailedTimeHisto = Utilities::TimeHistogramManager::MakeNewTimeHistogram1D<TH1F>( name.str().c_str(), title.str().c_str() );
  }

  if( passes ) {
    ++fPassedCounter;
    fPassedTimeHisto->Fill(evt.RawEvent()->EventHeader().TimeStamp());
    return true;
  }
  else{
    ++fFailedCounter;
    fFailedTimeHisto->Fill(evt.RawEvent()->EventHeader().TimeStamp());

   return false;
  }
}


void Cuts::Cut::PrintSummaryHeaderLine( std::ostream& out ) {

  int columnwidth = 10;

  std::stringstream header;
  header << std::setw(50) << std::left << "Description" << "  ";
  header << std::setw(columnwidth) << std::right << "passed";
  header << " / ";
  header << std::setw(columnwidth) << std::left << "total";
  header << "  ";
  header << std::setw(columnwidth) << " " << " | ";
  header << std::setw(2*columnwidth) << std::left << "fail only this cut";
  header << "  | ";
  header << std::setw(columnwidth) << std::left << "fail after passing all previous cuts";
  out << header.str() << std::endl;
}


void Cuts::Cut::PrintSummary( std::ostream& out ) const {

  int columnwidth = 10;

  char passedPercentage[20];
  char failedOnlyThisCutPercentage[20];
  sprintf( passedPercentage, "(%.2f%%)", ( fTotalCounter ? 100.*float(fPassedCounter)/float(fTotalCounter) : 0. ) );
  sprintf( failedOnlyThisCutPercentage, "(%.2f%%)", ( fTotalCounter ? 100.*float(fFailedOnlyThisCutCounter)/float(fTotalCounter) : 0. ) );

  std::stringstream summary;
  summary << std::setw(50) << std::left << Description() << ": "
          << std::setw(columnwidth) << std::right << fPassedCounter << " / " << std::setw(columnwidth) << std::left << fTotalCounter
          << "  "
          << std::setw(columnwidth) << std::right << passedPercentage << " | ";
  if( fIsStandaloneCut )
    summary << " N/A (standalone cut)";
  else {
    summary << std::setw(columnwidth) << fFailedOnlyThisCutCounter << " "
            << std::setw(columnwidth) << std::left << failedOnlyThisCutPercentage << " | "
            << std::setw(columnwidth) << std::right << fFailedAfterPassingAllPreviousCutsCounter << " ";
  }

  out << summary.str() << std::endl;
}

void Cuts::Cut::MergeStatisticsFrom( const Cut& other ) {

  if( Description() != other.Description() ){
    WARN_OUT << "Descriptions of this cut (" << Description() << ") and second cut (" << other.Description() << ") do not match!" << std::endl;
  }

  fTotalCounter                       += other.fTotalCounter;
  fPassedCounter                      += other.fPassedCounter;
  fFailedCounter                      += other.fFailedCounter;
  fFailedOnlyThisCutCounter                 += other.fFailedOnlyThisCutCounter;
  fFailedAfterPassingAllPreviousCutsCounter += other.fFailedAfterPassingAllPreviousCutsCounter;
}


/** Replace time-histogram of passed particles. Needed by ac_merge." */
void Cuts::Cut::ReplacePassedHisto( TH1F* histo ) {

  if( fPassedTimeHisto ) delete fPassedTimeHisto;
  fPassedTimeHisto = histo;
}

/** Replace time-histogram of failed particles. Needed by ac_merge." */
void Cuts::Cut::ReplaceFailedHisto( TH1F* histo ) {

  if( fFailedTimeHisto ) delete fFailedTimeHisto;
  fFailedTimeHisto = histo;
}


ClassImp(Cuts::Cut)
