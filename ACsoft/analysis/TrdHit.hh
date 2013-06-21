#ifndef TRDHIT_HH
#define TRDHIT_HH

#include <ostream>

#include "TRDRawHit.h"

#include <TVector3.h>

class TEllipse;

namespace ACsoft {

namespace AC {
  class TRDHTrack;
  class TRDVTrack;
}

namespace Analysis {

class SplineTrack;
class TrdTrack;

/** TRD hit for use in calibration and analysis.
  *
  * Allows access to geometry information and gain-corrected amplitude and dE/dx.
  *
  * The hit can be based on a track, e.g. a tracker SplineTrack or a %TRD track, so that
  * the relevant quantities for calibration and analysis (full 3D hit position, 3D pathlength and
  * distance to track) can be calculated and are subsequently stored in the TrdHit.
  *
  * \note TrdHit objects must be created using the TrdHitFactory.
  */
class TrdHit
{
public:

  /** Return the x or y coordinate of the hit, i.e. the one approximately perpendicular to the wire,
    * depending on orientation of the module the hit belongs to.
    *
    * \attention This function should be used with care! In general, it is better to use
    *            the position of the wire center and the direction of the wire, all in full 3D.
    */
  Double_t R() const { return( fOrientation==AC::XZMeasurement ? XYZ_along.X() : XYZ_along.Y() ); }

  /** Return the y or x coordinate of the hit, i.e. the one approximately parallel to the wire,
    * depending on orientation of the module the hit belongs to.
    *
    * \attention This function should be used with care! In general, it is better to use
    *            the position of the wire center and the direction of the wire, all in full 3D.
    */
  Double_t L() const { return( fOrientation==AC::XZMeasurement ? XYZ_along.Y() : XYZ_along.X() ); }
  Double_t Z() const { return( XYZ_along.Z() ); }

  /** Full 3D position after alignment and with the coordinate along the wire set by track. */
  TVector3 Position3D() const { return XYZ_along; }

  /** Position of the wire center post-alignment. */
  TVector3 PositionOfWireCenter() const { return XYZ_align; }

  TVector3 TubeDirection() const { return TubeDir; }

  Float_t GetRawAmplitude() const { return Amp_raw; }
  Float_t GetAmplitude() const { return Amp_calib; }

  Float_t GetRawDeDx() const { return ( fPathlength3D > 0.0 ? Amp_raw / fPathlength3D : 0.0 ); }
  Float_t GetDeDx() const { return ( fPathlength3D > 0.0 ? Amp_calib / fPathlength3D : 0.0 ); }

  Short_t Layer() const { return Lay; }
  Short_t Ladder() const { return Lad; }
  Short_t Tube() const { return Tub; }
  Short_t Module() const { return Mod; }
  Short_t GasCircuit() const { return GC; }
  Short_t GasGroup() const { return GG; }

  AC::MeasurementMode Orientation() const { return fOrientation; }

  Int_t GlobalStrawNumber() const { return fGlobalStrawNumber; }

  void Draw( bool rotatedSystem=false);

  /** 3D Path length as calculated from track (cm). */
  Double_t Pathlength3D() const { return fPathlength3D; }

  /** Distance calculated to track (cm). */
  Double_t DistanceToTrack() const { return fTrackDistance; }

  friend bool operator<( const Analysis::TrdHit& left, const Analysis::TrdHit& right );
  friend std::ostream& operator<<( std::ostream& out, const Analysis::TrdHit& h );

private:

  void ApplyGainCorrection( Double_t referenceGain );

  void FillInformationFromTrack( const SplineTrack& track );
  void FillInformationFromTrack( const TrdTrack& track );

private:

  friend class TrdHitFactory;
  TrdHit( const AC::TRDRawHit& ); // make sure everybody uses TrdHitFactory to make TrdHits
  TrdHit();

  Short_t     Lay;                ///< Layer number
  Short_t     Lad;                ///< Ladder number
  Short_t     Tub;                ///< Straw number
  Int_t       fGlobalStrawNumber; ///< Global straw number (0..5247)
  Short_t     Mod;                ///< Module (0..327)
  Short_t      GC;                ///< Manifold Gas-Circuit 1..41
  Short_t      GG;                ///< Manifold Gas-Group   1..5:W3   6..10:W7
  AC::MeasurementMode fOrientation; ///< Describes orientation of module

  Float_t     Amp_raw;          ///< Raw amplitude (unit? probably ADC counts)
  Float_t     Amp_calib;        ///< Amplitude after gain correction
  bool        IsGainCorrected;

  Double_t    fPathlength3D;      ///< calculated three-dimensional track pathlength
  Double_t    fTrackDistance;     ///< calculated distance to track

  TVector3   TubeDir;           ///< unit vector in direction of straw tube wire after alignment
  TVector3   XYZ_align;         ///< coordinates of straw center after alignment
  TVector3   XYZ_along;         ///< same as XYZ_align but with coordinate along wire set from external source (usually tracker track)
  bool       IsSecondCoordSet;

  TEllipse*  fEllipse; ///< for drawing the hit
};

bool operator<( const Analysis::TrdHit& left, const Analysis::TrdHit& right );

std::ostream& operator<<( std::ostream& out, const Analysis::TrdHit& h );

}

}

#endif // TRDHIT_HH
