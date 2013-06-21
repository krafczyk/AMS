#ifndef MCConfigHandler_hh
#define MCConfigHandler_hh

#include <string>
#include <vector>

class TFile;
class TH1D;

namespace ACsoft {

namespace Utilities {
  class ConfigHandler;
}

enum MCSpeciesID {
  MCUnknown,
  MCProton,
  MCHelium,
  MCElectron,
  MCPositron,
  MCAntiproton,
  MCLastSpecies = MCAntiproton + 1
};

struct Species {
  Species()
    : abundance(0)
    , speciesIndex(MCUnknown)
    , generatedRigidity(0) {

  }

  float abundance;
  MCSpeciesID speciesIndex;
  TH1D* generatedRigidity;
  std::vector<std::string> datasets;
};

MCSpeciesID MCSpeciesForName(const std::string&);
std::string NameForMCSpecies(MCSpeciesID);

class MCConfigHandler {
public:
  MCConfigHandler(Utilities::ConfigHandler*);

  bool Parse();
  void Dump();

  bool CreateNewHistograms();
  bool LoadHistogramsFromFile(TFile*);

  const std::vector<Species>& ListOfSpecies() const { return fSpecies; }
  double WeightForSpecies(MCSpeciesID, double mcRigidity) const;

private:
  bool ParseSpecies(const std::string& speciesName);

  Utilities::ConfigHandler* fHandler;
  std::vector<Species> fSpecies;
};

}

#endif // MCConfigHandler_hh
