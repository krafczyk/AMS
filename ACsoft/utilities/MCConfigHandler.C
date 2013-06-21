#include "MCConfigHandler.hh"

#include "Binning.hh"
#include "ConfigHandler.hh"

#include <iomanip>
#include <assert.h>
#include <TFile.h>
#include <TH1D.h>

#define DEBUG 0
#define INFO_OUT_TAG "MCConfigHandler> "
#include "debugging.hh"

namespace ACsoft {

MCSpeciesID MCSpeciesForName(const std::string& name) {

  if (name == "Proton")
    return MCProton;
  if (name == "Helium")
    return MCHelium;
  if (name == "Electron")
    return MCElectron;
  if (name == "Positron")
    return MCPositron;
  if (name == "Antiproton")
    return MCAntiproton;
  return MCUnknown;
}

std::string NameForMCSpecies(MCSpeciesID speciesId) {

  switch (speciesId) {
  case MCProton:
    return "Proton";
  case MCHelium:
    return "Helium";
  case MCElectron:
    return "Electron";
  case MCPositron:
    return "Positron";
  case MCAntiproton:
    return "Antiproton";
  default:
    break;
  }

  return std::string();
}


MCConfigHandler::MCConfigHandler(Utilities::ConfigHandler* handler)
  : fHandler(handler) {

}

bool MCConfigHandler::ParseSpecies(const std::string& speciesName) {

  Species species;
  species.speciesIndex = MCSpeciesForName(speciesName);
  fHandler->GetValue(speciesName, "Abundance", species.abundance, "Abundance of the species (0,1]");
  fHandler->GetValue(speciesName, "DataSets", species.datasets, "Set of ACQT files belonging to this species");
  fSpecies.push_back(species);
  return true;
}
 
bool MCConfigHandler::Parse() {

  std::vector<std::string> speciesNames;
  fHandler->GetValue("MC", "Species", speciesNames, "List of species");
  if (speciesNames.empty()) {
    WARN_OUT << "The number of species must be greater or equal than 1." << std::endl;
    return false;
  }

  for (unsigned int i = 0; i < speciesNames.size(); ++i) {
    if (!ParseSpecies(speciesNames[i]))
      return false;
  }

  return true;
}

bool MCConfigHandler::CreateNewHistograms() {

  std::vector<double> ecalBinning = Utilities::Binning::EcalBinning();
  for (unsigned int i = 0; i < fSpecies.size(); ++i) {
    Species& currentSpecies = fSpecies.at(i);
    std::stringstream name;
    name << "hMCRigidity_" << currentSpecies.speciesIndex;
    if (currentSpecies.generatedRigidity) {
      WARN_OUT << " You already called either LoadHistogramsFromFile or CreateNewHistograms, this is wrong!" << std::endl;
      return false;
    }

    currentSpecies.generatedRigidity = new TH1D(name.str().c_str(), name.str().c_str(), ecalBinning.size() - 1, ecalBinning.data());
  }

  return true;
}

bool MCConfigHandler::LoadHistogramsFromFile(TFile* dataFile) {

  assert(dataFile);
  for (unsigned int i = 0; i < fSpecies.size(); ++i) {
    Species& currentSpecies = fSpecies.at(i);
    std::stringstream speciesHistogramName;
    speciesHistogramName << "hMCRigidity_" << currentSpecies.speciesIndex;
    currentSpecies.generatedRigidity = (TH1D*)dataFile->Get(speciesHistogramName.str().c_str());

    if (!currentSpecies.generatedRigidity) {
      WARN_OUT << "Couldn't find histogram for species '" << NameForMCSpecies(currentSpecies.speciesIndex) << "' in data file!";
      return false;
    }
  }

  return true;
}

void MCConfigHandler::Dump() {

  // Row 1
  std::cout << " #-----#----------------";
  for (unsigned int i = 0; i < fSpecies.size(); ++i)
    std::cout << "-#-------------------#---------";
  std::cout << "-#" << std::endl;

  // Row 2 
  std::cout << " # Bin # Energy (GeV)   ";
  for (unsigned int i = 0; i < fSpecies.size(); ++i) {
    const Species& currentSpecies = fSpecies.at(i);
    std::cout << " # " << std::setw(17) << NameForMCSpecies(currentSpecies.speciesIndex);
    std::cout << " # " << std::setw(8) << "corr.";
  }
  std::cout << " #" << std::endl;

  // Row 3
  std::cout << " #-----#----------------";
  for (unsigned int i = 0; i < fSpecies.size(); ++i)
    std::cout << "-#-------------------#---------";
  std::cout << "-#" << std::endl;
 
  // Row 4+
  std::vector<double> ecalBinning = Utilities::Binning::EcalBinning();
  for (unsigned int i = 0; i < ecalBinning.size() - 1; ++i) {
    std::stringstream binRange;
    binRange << ecalBinning.at(i) << " - " << ecalBinning.at(i + 1);
    std::cout << " # " << std::setw(3) << i << " # " << std::setw(15) << binRange.str();

    for (unsigned int j = 0; j < fSpecies.size(); ++j) {
      const Species& currentSpecies = fSpecies.at(j);
      double count = currentSpecies.generatedRigidity->GetBinContent(i + 1);
      double weight = WeightForSpecies(currentSpecies.speciesIndex, currentSpecies.generatedRigidity->GetBinCenter(i + 1));
      std::stringstream countAndWeight;
      countAndWeight << count << " (" << std::setprecision(2) << std::scientific << weight << ")";

      std::stringstream weighted;
      weighted << std::fixed << std::setprecision(1) << weight * count;
      std::cout << " # " << std::setw(17) << countAndWeight.str()
                << " # " << std::setw(8) << weighted.str();
    }

    std::cout << " #" << std::endl;
  }

  std::cout << std::endl;
}

double MCConfigHandler::WeightForSpecies(MCSpeciesID speciesId, double mcRigidity) const {

  if (speciesId == MCUnknown)
    return 0;

  Species species;
  TH1* protonHistogram = 0;

  for (unsigned int i = 0; i < fSpecies.size(); ++i) {
    const Species& currentSpecies = fSpecies.at(i);
    if (currentSpecies.speciesIndex == MCProton)
      protonHistogram = currentSpecies.generatedRigidity;

    if (currentSpecies.speciesIndex == speciesId)
      species = currentSpecies;

    if (species.generatedRigidity && protonHistogram)
      break;
  }

  if (!protonHistogram) {
    WARN_OUT << "Species 'Proton' not found in config file" << std::endl;
    return 0;
  }

  TH1* speciesHistogram = species.generatedRigidity;  
  if (!speciesHistogram) {
    WARN_OUT << "Species '" << NameForMCSpecies(speciesId) << "' not found in config file" << std::endl;
    return 0;
  }

  assert(protonHistogram->GetXaxis()->GetNbins() == speciesHistogram->GetXaxis()->GetNbins());

  int targetBin = protonHistogram->FindBin(mcRigidity);
  assert(targetBin == speciesHistogram->FindBin(mcRigidity));
  if (!targetBin)
    return 0;

  double speciesInBin = speciesHistogram->GetBinContent(targetBin);
  if (!speciesInBin)
    return 0;
  
  return protonHistogram->GetBinContent(targetBin) * species.abundance / speciesInBin;
}

}
