#include "TOF.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_OUTPUT(TOF)
  DUMP(Betas)
  DUMP(Clusters)
END_DEBUG_OUTPUT

void TOF::Clear() {

  fBetas.clear();
  fClusters.clear();
}

Float_t TOF::CalculateMeanEnergy(const TOFBeta& beta) const {

  unsigned int size = beta.TOFClusterIndex().size();
  if (!size) return 0.;

  Float_t sum = 0.;
  for (unsigned int i = 0; i < beta.TOFClusterIndex().size(); ++i) sum += fClusters[beta.TOFClusterIndex()[i]].Energy();
  return sum / Float_t(size);
}

Float_t TOF::CalculateMaximumEnergy(const TOFBeta& beta) const {

  Float_t max = 0;
  for (unsigned int i = 0; i < beta.TOFClusterIndex().size(); ++i) {
    Float_t energy = fClusters[beta.TOFClusterIndex()[i]].Energy();
    if (energy > max)
      max = energy;
  }
  return max;
}

}

}
