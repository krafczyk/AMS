#include "MCTRDHit.h"
#include "Tools.h"

namespace AC {

BEGIN_DEBUG_TABLE(MCTRDHit)
  COL( "ID",            Short_t,          ID)
  COL( "LLT\t",         UShort_t,         LLT)
  COL( "Loss",          EnergyLossVector, EnergyLoss)
  COL( "Ekin [GeV]\t",  Float_t,          KineticEnergy)
  COL( "Edep [GeV]\t",  Float_t,          DepositedEnergy)
  COL( "Step [cm]\t",   Float_t,          StepSize)
END_DEBUG_TABLE

/** Writes this object into a QDataStream, which is used to produce ACQt files */
QDataStream& operator<<(QDataStream& stream, const MCTRDHit& object) {

  stream << object.fID << object.fLLT << object.fEnergyLoss << object.fKineticEnergy << object.fDepositedEnergy << object.fStep;
  return stream;
}

/** Reads this object from a QDataStream, which is used to construct AC objects from ACQt files */
QDataStream& operator>>(QDataStream& stream, MCTRDHit& object) {

  stream >> object.fID >> object.fLLT >> object.fEnergyLoss;

  FloatArrayStream<3> floatStream(stream);
  object.fKineticEnergy = floatStream.read();
  object.fDepositedEnergy = floatStream.read();
  object.fStep = floatStream.read();

  return stream;
}

}
