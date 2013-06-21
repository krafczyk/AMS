#include "Cut.hh"
#include "TimeHistogramManager.hh"
#include "Binning.hh"
#include <TCanvas.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <TEfficiency.h>

#define DEBUG 0
#define INFO_OUT_TAG "Cut> "
#include <debugging.hh>

#ifndef __CINT__
#include "Event.h"
#include "AnalysisParticle.hh"
#endif

int  Cuts::Cut::sCutCounter = 0; // initialize Cut instance counter
bool Cuts::Cut::sFillHistograms = true;

Cuts::Cut::Cut( std::string description ) :
  TObject(),
  fDescription(description),
  fTotalCounter(0),
  fPassedCounter(0),
  fFailedCounter(0),
  fFailedOnlyThisCutCounter(0),
  fFailedAfterPassingAllPreviousCutsCounter(0),
  fIsStandaloneCut(true),
  fPassedTimeHisto(0),
  fFailedTimeHisto(0) {

  fCutCount = sCutCounter;
  sCutCounter++;
}

Cuts::Cut::~Cut() {

}

bool Cuts::Cut::Passes( const ACsoft::Analysis::Particle& particle ) {

  bool passes = TestCondition(particle);

  ++fTotalCounter;

  if(!fPassedTimeHisto && sFillHistograms){
    std::stringstream name, title;
    name << "PassedTimeHisto_" << TString(fDescription.c_str()).ReplaceAll(" ","_").ReplaceAll(":","").Data() << "_" << fCutCount;
    title << "Particles passed vs time: " << fDescription;
    DEBUG_OUT << "Book time histos for cut-count: " << fCutCount << ":" << name.str() << std::endl;
    fPassedTimeHisto = ACsoft::Utilities::TimeHistogramManager::MakeNewTimeHistogram1D<TH1F>( name.str().c_str(), title.str().c_str() );
  }
  if(!fFailedTimeHisto && sFillHistograms){
    std::stringstream name, title;
    name << "FailedTimeHisto_" << TString(fDescription.c_str()).ReplaceAll(" ","_").ReplaceAll(":","").Data() << "_" << fCutCount;
    title << "Particles failed vs time: " << fDescription;
    fFailedTimeHisto = ACsoft::Utilities::TimeHistogramManager::MakeNewTimeHistogram1D<TH1F>( name.str().c_str(), title.str().c_str() );
  }

  if( passes ) {
    ++fPassedCounter;
    if(fPassedTimeHisto)
      fPassedTimeHisto->Fill(particle.EventTime());
    return true;
  }
  else{
    ++fFailedCounter;
    if(fFailedTimeHisto)
      fFailedTimeHisto->Fill(particle.EventTime());

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

TEfficiency* Cuts::Cut::ProduceTimeEfficiency(unsigned int mergeBins) const {

  TH1F* allTimeHisto = (TH1F*)fPassedTimeHisto->Clone();
  allTimeHisto->SetName("allTimeHisto");
  allTimeHisto->Add(fFailedTimeHisto);

  TH1F* passedTimeHisto = (TH1F*)fPassedTimeHisto->Clone();
  if (mergeBins > 1) {
    passedTimeHisto->Rebin(mergeBins);
    allTimeHisto->Rebin(mergeBins);
  }

  std::stringstream name, title;
  name << "TimeEfficiency_" << TString(fDescription.c_str()).ReplaceAll(" ","_").ReplaceAll(":","").Data() << "_" << fCutCount;
  title << "Efficiency '" << fDescription << "' vs. Time;Time;Efficiency";
  assert(TEfficiency::CheckConsistency(*passedTimeHisto, *allTimeHisto));
  TEfficiency* efficiency = new TEfficiency(*passedTimeHisto, *allTimeHisto);
  efficiency->SetName(name.str().c_str());
  efficiency->SetTitle(title.str().c_str());
  return efficiency;
}

/** Replace all histograms. Needed by ac_merge, to replace the statistics
  * of a single Cut with those merged from different runs.
  */
void Cuts::Cut::ReplaceHistograms(TH1F* passedHisto, TH1F* failedHisto) {

  delete fPassedTimeHisto;
  fPassedTimeHisto = passedHisto;

  delete fFailedTimeHisto;
  fFailedTimeHisto = failedHisto;
}

void Cuts::Cut::AssureCutIsAppliedToACQtFile( const ACsoft::Analysis::Particle& p ) {

  if (p.HasRawEventData())
    return;

  WARN_OUT << "Cut \"" << Description() << "\" can only be used on ACQt files. Aborting!" << std::endl;
  throw std::runtime_error("Cut can only be used on ACQt files.");
}

ClassImp(Cuts::Cut)
