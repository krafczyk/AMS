#include "TrackerTrack.h"
#include "Tools.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_TABLE(TrackerTrack)
  COL( "HitsX", Int_t, NumberOfHitsX)
  COL( "HitsY", Int_t, NumberOfHitsY)
  NEWTABLE
  COL( "ChargesNew\t\t\t\t\t\t\t\t\t\t\t\t", ChargesNewVector, ChargesNew)
  NEWTABLE
  COLE("TrdKCharge\t\t\t",                  Float_t,                    TrdKCharge)
  COL( "NhitsQ\t",                          UChar_t,                    TrdKChargeNumberOfHitsForCharge)
  COL( "NhitsN\t",                          UChar_t,                    TrdKChargeNumberOfHitsForNucleiPDF)
  COL( "NhitsD\t",                          UChar_t,                    TrdKChargeNumberOfHitsForDeltaRayPDF)
  COL( "TRDK Likelihoods e/p/He\t\t\t\t\t", TrdKChargeLikelihoodVector, TrdKChargeLikelihood)
  COL( "NhitsU\t",                          UChar_t,                    TrdKChargeNumberOfHitsForLikelihoods)
  COL( "NhitsO\t",                          UChar_t,                    TrdKChargeNumberOfOffTrackHitsForLikelihoods)
  NEWTABLE
  COL( "Cluster distances\t\t\t\t\t\t\t\t\t\t\t\t\t", ClusterDistancesVector, ClusterDistances)

  ADD_SEPERATED_DUMP( "TrackFits",         FitsVector,              TrackFits)
  ADD_SEPERATED_DUMP( "ReconstructedHits", ReconstructedHitsVector, ReconstructedHits)
END_DEBUG_TABLE

/** Writes this object into a QDataStream, which is used to produce ACQt files */
QDataStream& operator<<(QDataStream& stream, const PatternChargeAndError& object) {

  stream << object.pattern << object.charge << object.error;
  return stream;
}

/** Reads this object from a QDataStream, which is used to construct AC objects from ACQt files */
QDataStream& operator>>(QDataStream& stream, PatternChargeAndError& object) {

  stream >> object.pattern >> object.charge >> object.error;
  return stream;
}

#ifndef  DOXYGEN_SHOULD_SKIP_THIS
inline std::ostream& operator<<( std::ostream& stream, const PatternChargeAndError& object) {

  stream << "p=" << (UInt_t) object.pattern << " q=" << object.charge << " +/- " << object.error;
  return stream;
}
#endif

}

}
