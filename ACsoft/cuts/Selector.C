#include "Selector.hh"

#include "Cut.hh"
#include "Binning.hh"
#include "Utilities.hh"

#include <TH2F.h>
#include <TGraphAsymmErrors.h>
#include <TLegend.h>

#include <sstream>

#define DEBUG 0
#define INFO_OUT_TAG "Selector> "
#include <debugging.hh>

Cuts::Selector::Selector(std::string description) :
  TNamed(description.c_str(),description.c_str()), // FIXME remove spaces and other unusual characters from object name
  fDescription(description),
  fTotalCounter(0),
  fPassedCounter(0),
  fFailedCounter(0) {

}

Cuts::Selector::~Selector() {

  for( unsigned int i=0 ; i<fCuts.size() ; ++i )
    delete fCuts[i];
}

void Cuts::Selector::RegisterCut(Cuts::Cut* cut) {

  assert(cut);
  cut->SetIsStandaloneCut(false);
  fCuts.push_back(cut);
}

void Cuts::Selector::RegisterCut(Cuts::Cut* cut, const std::vector<double>& rigidityOrEnergyBinning, const std::vector<double>& cutValueBinning, NMinusOneMode mode) {

  RegisterCut(cut);
  if (cut->IsNMinusOneCut())
    static_cast<NMinusOneCut*>(cut)->CreateNMinusOneHistograms(rigidityOrEnergyBinning, cutValueBinning, mode);
}

bool Cuts::Selector::Passes( const ACsoft::Analysis::Particle& particle, bool fillNMinusOneHistogramsIfPossible ) {

  ++fTotalCounter;

  unsigned int nCuts = fCuts.size();
  bool passesAllCuts = true;
  bool passesAllPreviousCuts = true;
  unsigned int nCutsPassed = 0;

  // this is for the N-1 statistics of the individual cuts:
  // if in the end, the number of passed cuts will be equal to
  // nCuts - 1, we know that the cut at this position is the one
  // that exclusively caused the particle to fail
  unsigned int lastCutFailed = 0;

  std::vector<bool> status;
  status.assign(fCuts.size(), false);

  bool hasCutsWithNMinusOneLogic = false;
  for( unsigned int i=0 ; i<fCuts.size() ; ++i ) {

    Cuts::Cut* cut = fCuts[i];
    if (cut->IsNMinusOneCut()) {
      hasCutsWithNMinusOneLogic = true;
      // Don't call Reset() on ManualNMinusOneCuts, as that would reset the previously stored values from Examine()
      // which is called before this function is calle.d
      if (!static_cast<NMinusOneCut*>(cut)->IsManualNMinusOneCut())
        static_cast<NMinusOneCut*>(cut)->Reset();
    }

    bool passes = cut->Passes(particle);
    status.at(i) = passes;
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

  // Early exit if N-1 histograms shall not be filled.
  if (!fillNMinusOneHistogramsIfPossible || !hasCutsWithNMinusOneLogic)
    return passesAllCuts;

  for( unsigned int i = 0; i < fCuts.size(); ++i) {
    bool passedAllOther = true;
    for( unsigned int j = 0; j < fCuts.size(); ++j) {
      if (j != i)
        passedAllOther &= status.at(j);
    }

    if( passedAllOther ) {
      Cuts::Cut* cut = fCuts.at(i);
      if (cut->IsNMinusOneCut())
        static_cast<Cuts::NMinusOneCut*>(cut)->FillNMinusOneDistribution(status.at(i));
    }
  }

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

  INFO_OUT << fDescription << ": " << fTotalCounter << " particles, "
           << fPassedCounter << " passed " << passedPercentage << ", "
           << fFailedCounter << " failed " << failedPercentage << "." << std::endl;

}

/** Present the Time Efficiencies in a single plot with axis titles, legend, etc.
 *
 */
TCanvas* Cuts::Selector::MakeTimeEfficiencyCanvas(const std::string& canvasName, const std::string& canvasTitle, unsigned int mergeBins) {

  TCanvas* c = new TCanvas(canvasName.c_str(), canvasTitle.c_str());

  Cuts::Cut* firstCut = fCuts.at(0);
  assert(firstCut);

  TEfficiency* efficiency = firstCut->ProduceTimeEfficiency(mergeBins);
  efficiency->SetMarkerStyle(20);
  efficiency->SetMarkerColor(1);
  efficiency->Paint("");

  TGraphAsymmErrors* graph = ACsoft::Utilities::GraphFromTEfficiency(efficiency);
  graph->GetXaxis()->SetTitle("Time");
  graph->GetXaxis()->SetTimeDisplay(1);
  if (mergeBins >= 72)
    graph->GetXaxis()->SetTimeFormat("%y-%m-%d %F1970-01-01 00:00:00");
  else
    graph->GetXaxis()->SetTimeFormat("%y-%m-%d %H:%M:%S%F1970-01-01 00:00:00");
  graph->GetYaxis()->SetTitle("Fraction of events passing cuts");
  graph->GetYaxis()->SetRangeUser(0, 1);
  graph->Draw("AP");

  for( unsigned int i = 1; i < fCuts.size(); ++i) {
    Cuts::Cut* cut = fCuts.at(i);
    assert(cut);
    TEfficiency* efficiency = cut->ProduceTimeEfficiency(mergeBins);
    efficiency->SetMarkerStyle(20);
    efficiency->SetMarkerColor(i + 1);
    efficiency->Draw("SAME P");
  }

  c->BuildLegend()->SetFillColor(kWhite);

  std::stringstream titleString;
  titleString << "Fraction of events passing '" << fDescription << "' vs. time";
  graph->SetTitle(titleString.str().c_str());
  return c;
}

TCanvas* Cuts::Selector::MakeNMinusOneEfficiencyCanvas(NMinusOneMode mode, const std::string& canvasName, const std::string& canvasTitle) {

  if (!fCuts.size())
    return 0;

  for (unsigned int i = 0; i < fCuts.size(); ++i) {
    if (!fCuts[i]->IsNMinusOneCut()) {
      WARN_OUT << "All cuts of your selector must be N-1 cuts, otherwhise this function/logic is not applicable." << std::endl;
      return 0;
    }
  }

  TCanvas* c = new TCanvas(canvasName.c_str(), canvasTitle.c_str());
  c->SetLogx();

  Cuts::NMinusOneCut* firstCut = static_cast<Cuts::NMinusOneCut*>(fCuts[0]);
  assert(firstCut);

  TEfficiency* efficiency = firstCut->ProduceNMinusOneEfficiency();
  efficiency->SetMarkerStyle(20);
  efficiency->SetMarkerColor(1);
  efficiency->Paint("");

  std::string title = NMinusOneCut::NMinusOneValueTitle(mode);
  TGraphAsymmErrors* graph = ACsoft::Utilities::GraphFromTEfficiency(efficiency);
  graph->GetXaxis()->SetTitle(title.c_str());
  graph->GetYaxis()->SetTitle("Efficiency");
  graph->GetYaxis()->SetRangeUser(0, 1);
  graph->Draw("AP");

  for (unsigned int i = 1; i < fCuts.size(); ++i) {
    Cuts::NMinusOneCut* cut = static_cast<Cuts::NMinusOneCut*>(fCuts[i]);
    assert(cut);
    TEfficiency* efficiency = cut->ProduceNMinusOneEfficiency();
    efficiency->SetMarkerStyle(20);
    efficiency->SetMarkerColor(i + 1);
    efficiency->Draw("SAME P");
  }

  c->BuildLegend();

  std::stringstream titleString;
  titleString << "N-1 Efficiency '" << fDescription  << "'";
  graph->SetTitle(titleString.str().c_str());
  return c;
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
