#include "Particle.h"

namespace AC {

BEGIN_DEBUG_TABLE(Particle)
  COL( "ID  ",                   Int_t,                ID)
  COL( "Probability",            Float_t,              IDProbability)
  COL( "CMop",                   Int_t,                MostProbableCharge)
  COLE("Momentum [GeV/c]\t\t",   Float_t,              Momentum)
  COLE("Mass [GeV/c^2]\t\t\t\t", Float_t,              Mass)
  COLE("Beta\t\t\t\t\t\t",       Float_t,              Beta)
  NEWTABLE
  COL( "TOFB",                Short_t,                  TOFBetaIndex)
  COL( "TRDV",                Short_t,                  TRDVTrackIndex)
  COL( "TRDH",                Short_t,                  TRDHTrackIndex)
  COL( "TRKT",                Short_t,                  TrackerTrackIndex)
  COL( "RIRI",                Short_t,                  RICHRingIndex)
  COL( "ESHWR",               Short_t,                  ECALShowerIndex)
  COL( "Charges\t\t\t\t",     ChargesVector,            Charges)
  COL( "Probabilities\t\t\t", ChargesProbabilityVector, ChargesProbability)
END_DEBUG_TABLE

Int_t Particle::MostProbableCharge() const {

  float maxProbability = 0;
  Int_t maxCharge = 0;
  for( unsigned int i=0; i<fCharges.size() ; ++i ) {
    if( fChargesProbability[i]<=maxProbability ) continue;
    maxProbability = fChargesProbability[i];
    maxCharge = fCharges[i];
  }
  return maxCharge;
}

/** Writes this object into a QDataStream, which is used to produce ACQt files */
QDataStream& operator<<(QDataStream& stream, const Particle& object) {

  stream << object.fID << object.fIDProbability << object.fMomentum << object.fMomentumError << object.fMass << object.fMassError
         << object.fBeta << object.fBetaError << object.fCharges << object.fChargesProbability << object.fTOFBetaIndex << object.fTRDVTrackIndex
         << object.fTRDHTrackIndex << object.fTrackerTrackIndex << object.fRICHRingIndex << object.fECALShowerIndex;
  return stream;
}

/** Reads this object from a QDataStream, which is used to construct AC objects from ACQt files */
QDataStream& operator>>(QDataStream& stream, Particle& object) {

  stream >> object.fID >> object.fIDProbability;

  FloatArrayStream<6> floatStream(stream);
  object.fMomentum = floatStream.read();
  object.fMomentumError = floatStream.read();
  object.fMass = floatStream.read();
  object.fMassError = floatStream.read();
  object.fBeta = floatStream.read();
  object.fBetaError = floatStream.read();

  stream >> object.fCharges >> object.fChargesProbability;

  ShortArrayStream<6> shortStream(stream);
  object.fTOFBetaIndex = shortStream.read();
  object.fTRDVTrackIndex = shortStream.read();
  object.fTRDHTrackIndex = shortStream.read();
  object.fTrackerTrackIndex = shortStream.read();
  object.fRICHRingIndex = shortStream.read();
  object.fECALShowerIndex = shortStream.read();

  return stream;
}

}
