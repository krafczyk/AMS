#include "StraightLineTrack.hh"

#include "pathlength_functions.hh"

#define DEBUG 0
#define INFO_OUT_TAG "StraightLineTrack> "
#include <debugging.hh>

ACsoft::Analysis::StraightLineTrack::StraightLineTrack() :
  fSlopeXZ(0.),
  fOffsetXZ(0.),
  fSlopeYZ(0.),
  fOffsetYZ(0.),
  fReferenceZ(0.)
{
}

ACsoft::Analysis::StraightLineTrack::StraightLineTrack( double x0, double y0, double z0, double slopeXZ, double slopeYZ ) :
  fSlopeXZ(slopeXZ),
  fOffsetXZ(x0),
  fSlopeYZ(slopeYZ),
  fOffsetYZ(y0),
  fReferenceZ(z0)
{
}


ACsoft::Analysis::StraightLineTrack ACsoft::Analysis::StraightLineTrack::FromThetaAndPhi( double x0, double y0, double z0, double theta, double phi ) {

  TVector3 dir(cos(phi)*sin(theta),
               sin(phi)*sin(theta),
               cos(theta));
  if( dir.Z() == 0. ){
    WARN_OUT << "Error in initialization of StraightLineTrack: " << x0 << " " << y0 << " " << z0 << " " << theta << " " << phi << std::endl;
  }
  dir = 1.0/dir.Z() * dir;

  return ACsoft::Analysis::StraightLineTrack(x0,y0,z0,dir.X(),dir.Y());
}


/** Track position at given \p z. */
TVector3 ACsoft::Analysis::StraightLineTrack::Eval( double z ) const {

  return TVector3( EvalX(z), EvalY(z), z );
}


/** Calculate the distance at closest approach to line defined by the given position and direction. */
double ACsoft::Analysis::StraightLineTrack::DistanceToLine( const TVector3& linePosition, const TVector3& lineDirection ) const {

  return Distance(ReferencePosition(),DirectionVector(),linePosition,lineDirection);
}


/** Dump to console. */
void ACsoft::Analysis::StraightLineTrack::Dump() const {

  std::cout << "[StraightLineTrack]" << std::endl;
  std::cout << "  Offset XZ: " << fOffsetXZ << "  Offset YZ: " << fOffsetYZ << "  zref=" << fReferenceZ << std::endl;
  std::cout << "  Slope XZ: " << fSlopeXZ << "  Slope XY: " << fSlopeYZ << std::endl;
  std::cout << "  Theta: " << Theta()*TMath::RadToDeg() << "deg, Phi: " << Phi()*TMath::RadToDeg() << "deg" << std::endl;

}


