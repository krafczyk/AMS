#include "TRDRawHit.h"
#include "Settings.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_TABLE(TRDRawHit)
  COL( "Lay",          Short_t,         Layer)
  COL( "Lad",          Short_t,         Ladder)
  COL( "Tub",          Short_t,         Tube)
  COL( "E_depo [ADC]", Float_t,         DepositedEnergy)
  COL( "Mod",          Short_t,         Module)
  COL( "GCir",         Short_t,         GasCircuit)
  COL( "GGrp",         Short_t,         GasGroup)
END_DEBUG_TABLE

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


// FIXME: The way I determine the ACQt version is not correct. We have to determine the ACQt version for every file.
// We'd need to pass in an AC::Event* pointer here, so we can access the ACQt version used
// for that file. Otherwise it will not be possible to mix different FileManagers, loading
// different ACQt versions. For now this is sufficient, as nobody wants to mix.

Short_t TRDRawHit::Tube() const {

  if (::AC::CurrentACQtVersion() <= 55) {

    if( !GeometryForHardwareAddress()[2] )
      return ((fHWAddress % 100) % 16);     // Tube 0..15 in +x/+y order

    return 15 - ((fHWAddress % 100) % 16);  // UFE on other side

  } else {

    if( !GeometryForHardwareAddress()[2] )
      return (fHWAddress & 0xF);     // Tube 0..15 in +x/+y order

    return 15 - (fHWAddress & 0xF);  // UFE on other side

  }

}

Int_t* TRDRawHit::GeometryForHardwareAddress() const {

  if (::AC::CurrentACQtVersion() <= 55) {
    return AMSGeometry::Self()->GetGeo[(fHWAddress/10000)][(fHWAddress/1000)%10][(fHWAddress/100)%10][(fHWAddress%100)/16];
  } else {
    return AMSGeometry::Self()->GetGeo[(fHWAddress>>12)&1][(fHWAddress>>9)&7][(fHWAddress>>6)&7][(fHWAddress>>4)&3];
  }

}


bool TRDRawHit::FromTRDVTrack() const {
  if (::AC::CurrentACQtVersion() <= 55) return false;
  return (fHWAddress & (1<<13)) != 0;
}


bool TRDRawHit::FromTRDHSeg() const {
  if (::AC::CurrentACQtVersion() <= 55) return false;
  return (fHWAddress & (1<<14)) != 0;
}


bool TRDRawHit::FromTrackerTrack() const {
  if (::AC::CurrentACQtVersion() <= 55) return false;
  return (fHWAddress & (1<<15)) != 0;
}

}


}
