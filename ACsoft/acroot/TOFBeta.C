#include "TOFBeta.h"
#include "Statistics.hh"
#include "Event.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_TABLE(TOFBeta)
  COLE("Beta\t\t\t\t\t",                                                  Float_t,              Beta)
  NEWTABLE
  COL( "ChargesNew\t\t\t\t\t\t\t\t", ChargesNewVector,   ChargesNew)
  COL( "TOFCluster\t\t\t",           ClusterIndexVector, TOFClusterIndex)
END_DEBUG_TABLE

bool TOFBeta::ChargeInLayer(const AC::Event* event, int layer, Float_t& charge) const {

  assert(event);
  assert(fTOFClusterIndex.size() == fChargesNew.size());

  for (unsigned int i = 0; i < fChargesNew.size(); ++i) {
    if (event->TOF().Clusters()[fTOFClusterIndex[i]].Layer() + 1 == layer) {
      charge = fChargesNew[i];
      return true;
    }
  }

  charge = 0;
  return false;
}

static inline Utilities::MomentsMode MomentsModeForTOFChargeType(AC::TOFBeta::TOFChargeType type) {

  switch (type) {
  case AC::TOFBeta::GoodLayers:
  case AC::TOFBeta::GoodUpperTOFLayers:
  case AC::TOFBeta::GoodLowerTOFLayers:
    return Utilities::OnlyPositiveEntries;
  default:
    break;
  }

  return Utilities::AbsOfEntries;
}

const AC::TOFBeta::ChargesNewVector& TOFBeta::FilterChargesForType(const AC::Event* event, TOFChargeType type) const {

  // Filter out certain TOF charges, as described by the TOFChargeType.
  fSelectedCharges.clear();

  if (type == AllLayers || type == GoodLayers)
    return fChargesNew;

  // If only (good) upper or (good) lower layers are selected, build a temporary charges vector 'fSelectedCharges'
  // with only those charges that we're interessted in.
  bool selectUpperOnly = (type == AllUpperTOFLayers || type == GoodUpperTOFLayers);
  if (!selectUpperOnly) // Sanity check
    assert(type == AllLowerTOFLayers || type == GoodLowerTOFLayers);

  for (unsigned int i = 0; i < fChargesNew.size(); ++i) { 
    Int_t layer = event->TOF().Clusters()[fTOFClusterIndex[i]].Layer();
    if ((layer <= 1 && !selectUpperOnly) || (layer >= 2 && selectUpperOnly))
      fSelectedCharges.append(fChargesNew[i]);
  }

  return fSelectedCharges;
}

ChargeAndError TOFBeta::GetChargeAndErrorNew(const AC::Event* event, TOFChargeType type) const {

  assert(event);

  Utilities::Moments moments = Utilities::CalculateMoments(FilterChargesForType(event, type), MomentsModeForTOFChargeType(type));
  return ChargeAndError(moments.mean, moments.meanError);
}

}

}
