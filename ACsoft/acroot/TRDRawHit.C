#include "TRDRawHit.h"

namespace AC {

BEGIN_DEBUG_TABLE(TRDRawHit)
  COL( "Lay",          Short_t,         Layer)
  COL( "Lad",          Short_t,         Ladder)
  COL( "Tub",          Short_t,         Tube)
  COL( "E_depo [MeV]", Float_t,         DepositedEnergy)
  COL( "Dir",          MeasurementMode, Direction)
  COL( "X [cm]\t",     Float_t,         X)
  COL( "Y [cm]\t",     Float_t,         Y)
  COL( "Z [cm]\t",     Float_t,         Z)
  COL( "Mod",          Short_t,         Module)
  COL( "GCir",         Short_t,         GasCircuit)
  COL( "GGrp",         Short_t,         GasGroup)
END_DEBUG_TABLE

/** Writes this object into a QDataStream, which is used to produce ACQt files */
QDataStream& operator<<(QDataStream& stream, const TRDRawHit& object) {

  stream << object.fHWAddress << object.fADC;
  return stream;
}

/** Reads this object from a QDataStream, which is used to construct AC objects from ACQt files */
QDataStream& operator>>(QDataStream& stream, TRDRawHit& object) {

  UShortArrayStream<2> ushortStream(stream);
  object.fHWAddress = ushortStream.read();
  object.fADC = ushortStream.read();
  return stream;
}

bool operator<(const TRDRawHit& left, const TRDRawHit& right) {

  if (left.Layer() < right.Layer())
    return true;
  if (left.Layer() > right.Layer())
    return false;
  if (left.Ladder() < right.Ladder())
    return true;
  if (left.Ladder() > right.Ladder())
    return false;
  if (left.Tube() < right.Tube())
    return true;
  if (left.Tube() > right.Tube())
    return false;
  return left.DepositedEnergy() > right.DepositedEnergy();
}

}
