#include "NMinusOneCut.hh"
#include "AnalysisParticle.hh"
#include <sstream>

#define DEBUG 0
#define INFO_OUT_TAG "NMinusOneCut> "
#include <debugging.hh>

const float Cuts::NMinusOneCut::gUnsetValue = FLT_MAX - 1; // Can't use std::numeric_limits<float> otherwhise rootcint complains :-(

Cuts::NMinusOneCut::NMinusOneCut( std::string description ) :
  Cuts::Cut(description),
  fNMinusOneDistribution(0),
  fAllNMinusOneHisto(0),
  fPassedNMinusOneHisto(0),
  fLastCutValue(gUnsetValue),
  fNMinusOneMode(NMinusOneVsRigidity),
  fLastRigidityOrEnergy(gUnsetValue) {

}

Cuts::NMinusOneCut::~NMinusOneCut() {

}

/** Setter function for the N-1 Histogram of the cut.
 * called by RegisterCut from the Selector class
 */
void Cuts::NMinusOneCut::CreateNMinusOneHistograms(const std::vector<double>& rigidityOrEnergyBinning, const std::vector<double>& cutValueBinning, NMinusOneMode mode) {

  assert(!rigidityOrEnergyBinning.empty());
  assert(!cutValueBinning.empty());
  fNMinusOneMode = mode;

  std::stringstream distName, distTitle;
  distName << "N-1Histo_" << TString(Description().c_str()).ReplaceAll(" ","_").ReplaceAll(":","").Data()<< "_" << fCutCount;
  distTitle << "N-1 Distribution: " << Description();

  std::stringstream allName, allTitle;
  allName << "N-1All_" << TString(Description().c_str()).ReplaceAll(" ","_").ReplaceAll(":","").Data()<< "_" << fCutCount;
  allTitle << "N-1 All: " << Description();

  std::stringstream passedName, passedTitle;
  passedName << "N-1Passed_" << TString(Description().c_str()).ReplaceAll(" ","_").ReplaceAll(":","").Data()<< "_" << fCutCount;
  passedTitle << "N-1 Passed: " << Description();

  fNMinusOneDistribution = new TH2F(distName.str().c_str(), distTitle.str().c_str(), rigidityOrEnergyBinning.size() - 1, rigidityOrEnergyBinning.data(), cutValueBinning.size() - 1, cutValueBinning.data());
  fAllNMinusOneHisto = new TH1F(allName.str().c_str(), allTitle.str().c_str(), rigidityOrEnergyBinning.size() - 1, rigidityOrEnergyBinning.data());
  fPassedNMinusOneHisto = new TH1F(passedName.str().c_str(), passedName.str().c_str(), rigidityOrEnergyBinning.size() - 1, rigidityOrEnergyBinning.data());

  std::string title = NMinusOneValueTitle(mode);
  fNMinusOneDistribution->GetYaxis()->SetTitle(Description().c_str());
  fNMinusOneDistribution->GetXaxis()->SetTitle(title.c_str());
  fAllNMinusOneHisto->GetXaxis()->SetTitle(title.c_str());
  fPassedNMinusOneHisto->GetXaxis()->SetTitle(title.c_str());
}

TEfficiency* Cuts::NMinusOneCut::ProduceNMinusOneEfficiency() const {

  assert(TEfficiency::CheckConsistency(*fPassedNMinusOneHisto, *fAllNMinusOneHisto));
  TEfficiency* efficiency = new TEfficiency(*fPassedNMinusOneHisto, *fAllNMinusOneHisto);
  std::stringstream name, title;
  name << "NMinusOneEfficiency_" << TString(fDescription.c_str()).ReplaceAll(" ","_").ReplaceAll(":","").Data() << "_" << fCutCount;

  std::string xTitle = NMinusOneValueTitle(fNMinusOneMode);
  title << "N-1 Efficiency '" << fDescription << "';" << xTitle << ";Efficiency";
  efficiency->SetName(name.str().c_str());
  efficiency->SetTitle(title.str().c_str());
  return efficiency;
}

/** Replace all histograms. Needed by ac_merge, to replace the statistics
  * of a single Cut with those merged from different runs.
  */
void Cuts::NMinusOneCut::ReplaceNMinusOneHistograms(TH1F* passedNMinusOneHisto, TH1F* allNMinusOneHisto, TH2F* NMinusOneDistribution) {

  delete fPassedNMinusOneHisto;
  fPassedNMinusOneHisto = passedNMinusOneHisto;

  delete fAllNMinusOneHisto;
  fAllNMinusOneHisto = allNMinusOneHisto;

  delete fNMinusOneDistribution;
  fNMinusOneDistribution = NMinusOneDistribution;
}

std::string Cuts::NMinusOneCut::NMinusOneValueTitle(NMinusOneMode mode) {
  std::string title = "";
  switch (mode) {
    case NMinusOneVsRigidity:
      title = "Rigidity (GV)";
      break;
    case NMinusOneVsAbsRigidity:
      title = "|Rigidity| (GV)";
      break;
    case NMinusOneVsEnergy:
      title = "ECAL Energy (GeV)";
      break;
    case NMinusOneVsAbsMcMomentum:
      title = "|Momentum_{MC}| (GeV)";
      break;
    default:
      WARN_OUT << "No string for specified NMinusOneMode fNMinusOneMode!" << std::endl;
      assert(0);
      break;
  }
  return title;
}

double Cuts::NMinusOneCut::NMinusOneValue(const ACsoft::Analysis::Particle& particle, NMinusOneMode mode) {
  switch (mode) {
    case NMinusOneVsRigidity:
      return particle.Rigidity();
    case NMinusOneVsAbsRigidity:
      return fabs(particle.Rigidity());
    case NMinusOneVsEnergy:
      return particle.EcalEnergy();
    case NMinusOneVsAbsMcMomentum:
      return fabs(particle.McMomentum());
    default:
      WARN_OUT << "No return value specified for the desired NMinusOneMode fNMinusOneMode!" << std::endl;
      assert(0);
      break;
  }
}

ClassImp(Cuts::NMinusOneCut)
