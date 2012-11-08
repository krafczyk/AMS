#include "TrackerTrack.h"
#include "Tools.h"

namespace AC {

BEGIN_DEBUG_TABLE(TrackerTrack)
  COL( "HitsX",                    Int_t,                    NumberOfHitsX)
  COL( "HitsY",                    Int_t,                    NumberOfHitsY)
  COL( "Charges\t\t\t\t",          ChargesVector,            Charges)
  COL( "Probabilities\t\t",        ChargesProbabilityVector, ChargesProbability)
  COL( "SCalibProb\t\t\t\t\t\t\t", SCalibProbabilityVector,  SCalibProbability)
  NEWTABLE
  COL( "ChargesNew\t\t\t\t\t\t\t\t\t\t\t\t", ChargesNewVector, ChargesNew)
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

/** Writes this object into a QDataStream, which is used to produce ACQt files */
QDataStream& operator<<(QDataStream& stream, const TrackerTrack& object) {

  stream << object.fStatus << object.fNumberOfHitsX << object.fNumberOfHitsY << object.fCharges
         << object.fChargesProbability << object.fSCalibProbability << object.fChargesNew
         << object.fTrackFits << object.fReconstructedHits << object.fClusterDistances;
  return stream;
}

/** Reads this object from a QDataStream, which is used to construct AC objects from ACQt files */
QDataStream& operator>>(QDataStream& stream, TrackerTrack& object) {

  stream >> object.fStatus >> object.fNumberOfHitsX >> object.fNumberOfHitsY
         >> object.fCharges >> object.fChargesProbability >> object.fSCalibProbability
         >> object.fChargesNew >> object.fTrackFits >> object.fReconstructedHits >> object.fClusterDistances;

  return stream;
}

#ifndef  DOXYGEN_SHOULD_SKIP_THIS
inline std::ostream& operator<<( std::ostream& stream, const PatternChargeAndError& object) {

  stream << "p=" << (UInt_t) object.pattern << " q=" << object.charge << " +/- " << object.error;
  return stream;
}
#endif

}
