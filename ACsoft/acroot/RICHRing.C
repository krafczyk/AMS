#include "RICHRing.h"

namespace AC {

BEGIN_DEBUG_TABLE(RICHRing)
  COL( "Hits",           UShort_t, NumberOfHits)
  COLE("Beta\t\t\t\t\t", Float_t,  Beta)
  COL( "X [cm]\t",       Float_t,  X)
  COL( "Y [cm]\t",       Float_t,  Y)
  COL( "Z [cm]\t",       Float_t,  Z)
  NEWTABLE
  COL( "Theta [deg]",             Float_t,                  Theta)
  COL( "Phi [deg]",               Float_t,                  Phi)
  COL( "Prob\t\t",                Float_t,                  Probability)
  COL( "C2Estim\t",               Float_t,                  ChargeSquareEstimate)
  COL( "Charges\t\t\t",           ChargesVector,            Charges)
  COL( "Probabilities\t\t\t\t\t", ChargesProbabilityVector, ChargesProbability)
END_DEBUG_TABLE

/** Writes this object into a QDataStream, which is used to produce ACQt files */
QDataStream& operator<<(QDataStream& stream, const RICHRing& object) {

  stream << object.fStatus << object.fNumberOfHits << object.fBeta << object.fBetaError << object.fX
         << object.fY << object.fZ << object.fTheta << object.fPhi << object.fProbability << object.fChargeSquareEstimate
         << object.fCharges << object.fChargesProbability;
  return stream;
}

/** Reads this object from a QDataStream, which is used to construct AC objects from ACQt files */
QDataStream& operator>>(QDataStream& stream, RICHRing& object) {

  stream >> object.fStatus >> object.fNumberOfHits;

  FloatArrayStream<9> floatStream(stream);
  object.fBeta = floatStream.read();
  object.fBetaError = floatStream.read();
  object.fX = floatStream.read();
  object.fY = floatStream.read();
  object.fZ = floatStream.read();
  object.fTheta = floatStream.read();
  object.fPhi = floatStream.read();
  object.fProbability = floatStream.read();
  object.fChargeSquareEstimate = floatStream.read();

  stream >> object.fCharges >> object.fChargesProbability;
  return stream;
}

}
