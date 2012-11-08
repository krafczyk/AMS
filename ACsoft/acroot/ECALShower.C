#include "ECALShower.h"

namespace AC {

BEGIN_DEBUG_TABLE(ECALShower)
  COL( "Hits",                 UShort_t, NumberOfHits)
  COL( "E_depo [MeV]",         Float_t,  DepositedEnergy)
  COL( "Energy3CMRatio",       Float_t,  EnergyAt3CMRatio)
  COLE("E_reco [GeV]\t\t\t\t", Float_t,  ReconstructedEnergy)
  COL( "Rear leak",            Float_t,  RelativeRearLeak)
  COL( "Shower max [cm]",      Float_t,  ShowerMaximum)
  COL( "Chi2\t\t",             Float_t,  ChiSquareProfile)
  NEWTABLE
  COL( "Theta [deg]", Float_t, Theta)
  COL( "Phi [deg]",   Float_t, Phi)
  COL( "X [cm]\t",    Float_t, X)
  COL( "Y [cm]\t",    Float_t, Y)
  COL( "Z [cm]\t",    Float_t, Z)
  COL( "EnX [cm]\t",  Float_t, EntryX)
  COL( "EnY [cm]\t",  Float_t, EntryY)
  COL( "EnZ [cm]\t",  Float_t, EntryZ)
  NEWTABLE
  COL( "ExX [cm]\t",         Float_t,          ExitX)
  COL( "ExY [cm]\t",         Float_t,          ExitY)
  COL( "ExZ [cm]\t",         Float_t,          ExitZ)
  COL( "Estimators\t\t\t\t", EstimatorsVector, Estimators)
END_DEBUG_TABLE

/** Writes this object into a QDataStream, which is used to produce ACQt files */
QDataStream& operator<<(QDataStream& stream, const ECALShower& object) {

  stream << object.fStatus << object.fNumberOfHits << object.fDepositedEnergy << object.fEnergyAt3CMRatio
         << object.fReconstructedEnergy << object.fReconstructedEnergyError << object.fRelativeRearLeak
         << object.fShowerMaximum << object.fChiSquareProfile << object.fTheta << object.fPhi
         << object.fX << object.fY << object.fZ << object.fEntryX << object.fEntryY << object.fEntryZ
         << object.fExitX << object.fExitY << object.fExitZ << object.fEstimators;
  return stream;
}

/** Reads this object from a QDataStream, which is used to construct AC objects from ACQt files */
QDataStream& operator>>(QDataStream& stream, ECALShower& object) {

  stream >> object.fStatus >> object.fNumberOfHits;

  FloatArrayStream<18> floatStream(stream);
  object.fDepositedEnergy = floatStream.read();
  object.fEnergyAt3CMRatio = floatStream.read();
  object.fReconstructedEnergy = floatStream.read();
  object.fReconstructedEnergyError = floatStream.read();
  object.fRelativeRearLeak = floatStream.read();
  object.fShowerMaximum = floatStream.read();
  object.fChiSquareProfile = floatStream.read();
  object.fTheta = floatStream.read();
  object.fPhi = floatStream.read();
  object.fX = floatStream.read();
  object.fY = floatStream.read();
  object.fZ = floatStream.read();
  object.fEntryX = floatStream.read();
  object.fEntryY = floatStream.read();
  object.fEntryZ = floatStream.read();
  object.fExitX = floatStream.read();
  object.fExitY = floatStream.read();
  object.fExitZ = floatStream.read();
 
  stream >> object.fEstimators;
  return stream;
}

}
