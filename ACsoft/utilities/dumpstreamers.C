
#include "dumpstreamers.hh"

#include <TVector3.h>
#include <TRotation.h>
#include <TMath.h>

std::ostream& operator<<( std::ostream& out, const TVector3& v ){

  out << "(" << v.X() << "," << v.Y() << "," << v.Z() << ")";

  return out;
}


std::ostream& operator<<( std::ostream& out, const TRotation& r ){

  Double_t angle;
  TVector3 axis;
  r.AngleAxis(angle,axis);

  out << angle*TMath::RadToDeg() << " deg around " << axis;

  return out;
}

