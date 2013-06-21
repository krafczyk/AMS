#ifndef STRAIGHTLINETRACK_HH
#define STRAIGHTLINETRACK_HH

#include <TVector2.h>
#include <TVector3.h>
#include <TMath.h>

namespace ACsoft {

namespace Analysis {

class TrdTracking;

/** A 3D straght-line track.
  *
  * The track is modelled as a straight line according to the following formula:
  * \f[
  * x(z) = x_{0} + dx/dz * (z-z_{ref})
  * y(z) = y_{0} + dy/dz * (z-z_{ref})
  * \f]
  *
  *
  */
class StraightLineTrack
{

public:

  StraightLineTrack();
  StraightLineTrack( double x0, double y0, double z0, double slopeXZ, double slopeYZ );
  static StraightLineTrack FromThetaAndPhi( double x0, double y0, double z0, double theta, double phi );

  virtual double EvalX( double z ) const { return fOffsetXZ + fSlopeXZ*( z - fReferenceZ ); }
  virtual double EvalY( double z ) const { return fOffsetYZ + fSlopeYZ*( z - fReferenceZ ); }

  virtual TVector3 Eval( double z ) const;

  virtual double OffsetXZ() const { return fOffsetXZ; }
  virtual double OffsetYZ() const { return fOffsetYZ; }
  virtual double Zref() const { return fReferenceZ; }
  virtual double SlopeXZ() const { return fSlopeXZ; }
  virtual double SlopeYZ() const { return fSlopeYZ; }

  virtual double Theta() const { return TVector3(fSlopeXZ,fSlopeYZ,1.0).Theta(); }
  virtual double Phi()   const { return TVector3(fSlopeXZ,fSlopeYZ,1.0).Phi(); }
  /** Angle to z-axis in XZ projection. */
  virtual double ProjAngleX() const { return TMath::PiOver2() - TVector2(fSlopeXZ,1.0).Phi(); }
  /** Angle to z-axis in YZ projection. */
  virtual double ProjAngleY() const { return TMath::PiOver2() - TVector2(fSlopeYZ,1.0).Phi(); }

  virtual TVector3 DirectionVector() const { return TVector3(fSlopeXZ,fSlopeYZ,1.0).Unit(); }
  virtual TVector3 ReferencePosition() const { return TVector3(fOffsetXZ,fOffsetYZ,fReferenceZ); }

  virtual double DistanceToLine( const TVector3& linePosition, const TVector3& lineDirection ) const;

  virtual void Dump() const;

protected:

  friend class TrdTracking;

  double fSlopeXZ;    ///< dx/dz
  double fOffsetXZ;   ///< x offset (at reference z)
  double fSlopeYZ;    ///< dy/dz
  double fOffsetYZ;   ///< y offset (at reference z)
  double fReferenceZ; ///< reference z in straight line formula


};
}
}

#endif // STRAIGHTLINETRACK_HH
