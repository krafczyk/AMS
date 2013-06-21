#ifndef TRDMISSINGHIT_HH
#define TRDMISSINGHIT_HH

#include <TVector3.h>

#include <TRDRawHit.h>

class TEllipse;

namespace ACsoft {

namespace Analysis {

/** A %TRD missing hit.
  *
  * A missing hit occurs when there is no raw hit in two neighbouring TRD straws and a track (defined, e.g.
  * by the tracker or the rest of the TRD) passes right in the middle between the two straws.
  *
  * \sa Analysis::TrdTracking
  */
class TrdMissingHit
{

public:

  TrdMissingHit( int globalSublayer, TVector3 pos, TVector3 dir, double gap, AC::MeasurementMode orientation );

  int GlobalSublayer() const { return fGlobalSublayerNumber; }
  TVector3 Position() const { return fPosition; }
  TVector3 Direction() const { return fDirection; }
  double GapWidth() const { return fGapWidth; }

  void Draw( bool rotatedSystem = false ) const;
  void Dump() const;

private:

  int fGlobalSublayerNumber;
  TVector3 fPosition;   ///< global position
  TVector3 fDirection;  ///< global direction
  double fGapWidth;     ///< width of gap between neighbouring straw tube walls (cm)
  AC::MeasurementMode fOrientation; ///< describes how missing hit is oriented

  mutable TEllipse* fEllipse;    ///< ellipsis used for drawing

};
}

}

#endif // TRDMISSINGHIT_HH
