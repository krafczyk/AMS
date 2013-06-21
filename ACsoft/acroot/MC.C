#include "MC.h"
#include "Tools.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_OUTPUT(MC)
  DUMP(EventGenerators)
  DUMP(TRDClusters)
  DUMP(TrackerClusters)
END_DEBUG_OUTPUT

void MC::Clear() {

  fNumberOfEventGenerators = -1;
  fNumberOfTrackerCluster = -1;
  fNumberOfTRDCluster = -1;
  fNumberOfTOFCluster = -1;
  fNumberOfACCCluster = -1;
  fNumberOfECALHits = -1;
  fNumberOfRICHCluster = -1;
  fUpperTOFEnergyDeposition = -1;
  fLowerTOFEnergyDeposition = -1;
  fTrackerEnergyDeposition = -1;
  fTRDEnergyDeposition = -1;
  fACCEnergyDeposition = -1;
  fECALEnergyDeposition = -1;

  fEventGenerators.clear();
  fTRDClusters.clear();
  fTrackerClusters.clear();
}

const MCEventGenerator* MC::PrimaryEventGenerator() const {

  for (unsigned int i = 0; i < EventGenerators().size(); ++i) {
    const int id = EventGenerators().at(i).ParticleID();
    if (id > 0) {
      return &EventGenerators().at(i);
    }
  }
  return 0;
}

}

}
