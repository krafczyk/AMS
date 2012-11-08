#include "TRDVTrack.h"
#include "Statistics.hh"

namespace AC {

BEGIN_DEBUG_TABLE(TRDVTrack)
  COL( "ChiSquare",             Float_t,       ChiSquare)
  COL( "Theta [deg]",           Float_t,       Theta)
  COL( "Phi [deg]",             Float_t,       Phi)
  COL( "X [cm]\t",              Float_t,       X)
  COL( "Y [cm]\t",              Float_t,       Y)
  COL( "Z [cm]\t",              Float_t,       Z)
  COL( "Pattern",               UInt_t,        Pattern)
  COL( "Charges\t\t\t\t\t\t\t", ChargesVector, Charges)
  NEWTABLE
  COL( "Probability\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t", ChargesProbabilityVector, ChargesProbability)
  NEWTABLE
  COL( "ChargesNew\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t",  ChargesNewVector, ChargesNew)
END_DEBUG_TABLE

ChargeAndError TRDVTrack::GetChargeAndErrorNew() const {

  Utilities::Moments moments = Utilities::CalculateMoments(fChargesNew);
  return ChargeAndError(moments.mean, moments.meanError);
}

/** Writes this object into a QDataStream, which is used to produce ACQt files */
QDataStream& operator<<(QDataStream& stream, const TRDVTrack& object) {

  stream << object.fStatus << object.fChiSquare << object.fTheta << object.fPhi << object.fX << object.fY
         << object.fZ << object.fPattern << object.fCharges << object.fChargesProbability << object.fChargesNew;
  return stream;
}

/** Reads this object from a QDataStream, which is used to construct AC objects from ACQt files */
QDataStream& operator>>(QDataStream& stream, TRDVTrack& object) {

  stream >> object.fStatus;

  FloatArrayStream<6> floatStream(stream);
  object.fChiSquare = floatStream.read();
  object.fTheta = floatStream.read();
  object.fPhi = floatStream.read();
  object.fX = floatStream.read();
  object.fY = floatStream.read();
  object.fZ = floatStream.read();

  stream >> object.fPattern >> object.fCharges >> object.fChargesProbability >> object.fChargesNew;
  return stream;
}

}
