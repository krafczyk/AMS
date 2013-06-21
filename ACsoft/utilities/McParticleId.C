#include "McParticleId.hh"

#include <assert.h>

#include <TColor.h>

#define DEBUG 0
#define INFO_OUT_TAG "McParticleId> "
#include <debugging.hh>

QMap<ACsoft::Utilities::McParticleId::McSpecies, ACsoft::Utilities::McParticleId::Identifier > ACsoft::Utilities::McParticleId::s_particleIdentifiers;

ACsoft::Utilities::McParticleId::McSpecies ACsoft::Utilities::McParticleId::Species(int id) {
  Construct();
  for (QMap<McSpecies, Identifier>::iterator iter = s_particleIdentifiers.begin(); iter != s_particleIdentifiers.end(); ++iter) {
    if(iter.value().id == id)
      return iter.key();
  }
//  assert(id > 0);
  return NoSpecies;
}

int ACsoft::Utilities::McParticleId::Id(McSpecies species) {
  Construct();
  assert(s_particleIdentifiers.contains(species));
  return s_particleIdentifiers[species].id;
}

ACsoft::Utilities::McParticleId::McSpecies ACsoft::Utilities::McParticleId::Species(const std::string& name) {
  Construct();
  for (QMap<McSpecies, Identifier>::iterator iter = s_particleIdentifiers.begin(); iter != s_particleIdentifiers.end(); ++iter) {
    if(iter.value().name == name)
      return iter.key();
  }
  assert(0);
  return NoSpecies;
}

std::string ACsoft::Utilities::McParticleId::Name(McSpecies species) {
  Construct();
  assert(s_particleIdentifiers.contains(species));
  return s_particleIdentifiers[species].name;
}

double ACsoft::Utilities::McParticleId::Mass(McSpecies species) {
  Construct();
  assert(s_particleIdentifiers.contains(species));
  return s_particleIdentifiers[species].mass;
}

double ACsoft::Utilities::McParticleId::Charge(McSpecies species) {
  Construct();
  assert(s_particleIdentifiers.contains(species));
  return s_particleIdentifiers[species].charge;
}

double ACsoft::Utilities::McParticleId::Color(McSpecies species) {
  Construct();
  assert(s_particleIdentifiers.contains(species));
  return s_particleIdentifiers[species].color;
}


void ACsoft::Utilities::McParticleId::Construct() {
  if (!s_particleIdentifiers.empty())
    return;
  // See: http://wwwasdoc.web.cern.ch/wwwasdoc/geant/node72.html
  s_particleIdentifiers.insert(NoSpecies, Init(-1, "No species", 0, 0));
  s_particleIdentifiers.insert(Gamma, Init(1, "Gamma",0 , 0, kYellow));
  s_particleIdentifiers.insert(Positron, Init(2, "Positron",0.00051099906, 1, kRed));
  s_particleIdentifiers.insert(Electron, Init(3, "Electron", 0.00051099906, -1, kGreen));
  s_particleIdentifiers.insert(Neutrino, Init(4, "Neutrino", 0, 0));
  s_particleIdentifiers.insert(MuPlus, Init(5, "Muon+", 0.105658389, 1));
  s_particleIdentifiers.insert(MuMinus, Init(6, "Muon-", 0.105658389, -1));
  s_particleIdentifiers.insert(Pi0, Init(7, "Pion0",0.1349764, 0));
  s_particleIdentifiers.insert(PiPlus, Init(8, "Pion+",0.1395700, 1));
  s_particleIdentifiers.insert(PiMinus, Init(9, "Pion-",0.1395700, -1));
  s_particleIdentifiers.insert(KaonPlus, Init(11, "Kaon+",0.493677, 1));
  s_particleIdentifiers.insert(KaonMinus, Init(12, "Kaon-",0.493677, -1));
  s_particleIdentifiers.insert(Neutron, Init(13, "Neutron",0.93956563, 0));
  s_particleIdentifiers.insert(Proton, Init(14, "Proton",0.93827231, 1, kBlue));
  s_particleIdentifiers.insert(Antiproton, Init(15, "Antiproton",0.93827231, -1, kOrange));
  s_particleIdentifiers.insert(Kaon0long, Init(10, "Kaon0 long",0.497672, 0));
  s_particleIdentifiers.insert(Kaon0short, Init(16, "Kaon0 short",0.497672, 0));
  s_particleIdentifiers.insert(Deuteron, Init(45, "Deuteron",1.875613, 1));
  s_particleIdentifiers.insert(Tritium, Init(46, "Tritium",2.80925, 1));
  s_particleIdentifiers.insert(Alpha, Init(47, "Alpha",3.727417, 2, kMagenta));
  s_particleIdentifiers.insert(Geantino, Init(48, "Geantino",0, 0));
  s_particleIdentifiers.insert(He3, Init(49, "He3",2.80923, 2));
  s_particleIdentifiers.insert(Li6, Init(61, "Li6",5.60305, 3));
  s_particleIdentifiers.insert(Li7, Init(62, "Li7",6.53536, 3));
  s_particleIdentifiers.insert(Be7, Init(63, "Be7",6.53622, 4));
  s_particleIdentifiers.insert(Be9, Init(64, "Be9",8.39479, 4));
  s_particleIdentifiers.insert(B10, Init(65, "B10",9.32699, 5));
  s_particleIdentifiers.insert(B11, Init(66, "B11",10.25510, 5));
  s_particleIdentifiers.insert(C12, Init(67, "C12",11.17793, 6));
  s_particleIdentifiers.insert(N14, Init(68, "N14",13.04378, 7));
  s_particleIdentifiers.insert(O16, Init(69, "O16",14.89917, 8));
  s_particleIdentifiers.insert(F19, Init(70, "F19",17.69690, 9));
  s_particleIdentifiers.insert(Ne20, Init(71, "Ne20",18.62284, 10));
  s_particleIdentifiers.insert(Na23, Init(72, "Na23",21.41483, 11));
  s_particleIdentifiers.insert(Mg24, Init(73, "Mg24",22.34193, 12));
  s_particleIdentifiers.insert(Al27, Init(74, "Al27",25.13314, 13));
  s_particleIdentifiers.insert(Si28, Init(75, "Si28",26.06034, 14));
  s_particleIdentifiers.insert(P31, Init(76, "P31",28.85188, 15));
  s_particleIdentifiers.insert(S32, Init(77, "S32",29.78180, 16));
  s_particleIdentifiers.insert(Cl35, Init(78, "Cl35",32.57328, 17));
  s_particleIdentifiers.insert(Ar36, Init(79, "Ar36",33.50356, 18));
  s_particleIdentifiers.insert(K39, Init(80, "K39",36.29447, 19));
  s_particleIdentifiers.insert(Ca40, Init(81, "Ca40",37.22492, 20));
  s_particleIdentifiers.insert(Sc45, Init(82, "Sc45",41.87617, 21));
  s_particleIdentifiers.insert(Ti48, Init(83, "Ti48",44.66324, 22));
  s_particleIdentifiers.insert(V51, Init(84, "V51",47.45401, 23));
  s_particleIdentifiers.insert(Cr52, Init(85, "Cr52",48.38228, 24));
  s_particleIdentifiers.insert(Mn55, Init(86, "Mn55",51.17447, 25));
  s_particleIdentifiers.insert(Fe56, Init(87, "Fe56",52.10307, 26));
  s_particleIdentifiers.insert(Co59, Init(88, "Co59",54.89593, 27));
  s_particleIdentifiers.insert(Ni58, Init(89, "Ni58",53.96644, 28));
  s_particleIdentifiers.insert(Cu63, Init(90, "Cu63",58.61856, 29));
  s_particleIdentifiers.insert(Zn64, Init(91, "Zn64",59.54963, 30));
}
