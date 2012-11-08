#include "TOFBeta.h"
#include "Statistics.hh"

namespace AC {

BEGIN_DEBUG_TABLE(TOFBeta)
  COLE("Beta\t\t\t\t\t",                                                  Float_t,              Beta)
  COL( "Charges\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t",                         ChargesVector,        Charges)
  NEWTABLE
  COL( "Probability\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t", ChargesProbabilityVector, ChargesProbability)
  NEWTABLE
  COL( "ChargesNew\t\t\t\t\t\t\t\t", ChargesNewVector,   ChargesNew)
  COL( "TOFCluster\t\t\t",           ClusterIndexVector, TOFClusterIndex)
END_DEBUG_TABLE

ChargeAndError TOFBeta::GetChargeAndErrorNew() const {

  Utilities::Moments moments = Utilities::CalculateMoments(fChargesNew);
  return ChargeAndError(moments.mean, moments.meanError);
}

/** Writes this object into a QDataStream, which is used to produce ACQt files */
QDataStream& operator<<(QDataStream& stream, const TOFBeta& object) {

  stream << object.fBeta << object.fBetaError << object.fCharges << object.fChargesProbability
         << object.fChargesNew << object.fTOFClusterIndex;
  return stream;
}

/** Reads this object from a QDataStream, which is used to construct AC objects from ACQt files */
QDataStream& operator>>(QDataStream& stream, TOFBeta& object) {

  FloatArrayStream<2> floatStream(stream);
  object.fBeta = floatStream.read();
  object.fBetaError = floatStream.read();

  stream >> object.fCharges >> object.fChargesProbability >> object.fChargesNew >> object.fTOFClusterIndex;
  return stream;
}

}
