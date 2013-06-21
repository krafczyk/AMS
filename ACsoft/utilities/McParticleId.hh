#ifndef McParticleId_hh
#define McParticleId_hh

#include <QMap>
#include <QPair>

#include <string>

#include <TColor.h>

namespace ACsoft {

namespace Utilities {

  enum McBinningMode {
    MomentumBinning,
    KineticEnergyBinning
  };

  class McParticleId {
  public:
    enum McSpecies {
      NoSpecies,
      Gamma,
      Positron,
      Electron,
      Neutrino,
      MuPlus,
      MuMinus,
      Pi0,
      PiPlus,
      PiMinus,
      KaonPlus,
      KaonMinus,
      Neutron,
      Proton,
      Antiproton,
      Kaon0long,
      Kaon0short,
      Deuteron,
      Tritium,
      Alpha,
      Geantino,
      He3,
      Li6,
      Li7,
      Be7,
      Be9,
      B10,
      B11,
      C12,
      N14,
      O16,
      F19,
      Ne20,
      Na23,
      Mg24,
      Al27,
      Si28,
      P31,
      S32,
      Cl35,
      Ar36,
      K39,
      Ca40,
      Sc45,
      Ti48,
      V51,
      Cr52,
      Mn55,
      Fe56,
      Co59,
      Ni58,
      Cu63,
      Zn64,
      SpeciesEnd
    };

    static McSpecies Species(int);
    static int Id(McSpecies);

    static McSpecies Species(const std::string&);
    static std::string Name(McSpecies);
    static double Mass(McSpecies);
    static double Charge(McSpecies);
    static double Color(McSpecies);

  private:

    struct Identifier {
      int id;
      std::string name;
      double mass;
      double charge;
      int color;
    };

    static struct Identifier Init(int id, std::string name, double mass, double charge, int color = kBlack)
    {
      struct Identifier newIdentifier = {id, name, mass, charge, color};
      return newIdentifier;
    }

    static QMap<McSpecies, Identifier> s_particleIdentifiers;
    static void Construct();

  };
}

}

#endif
