#ifndef TRDHIT_HH
#define TRDHIT_HH

#include <ostream>

#include <TVector3.h>

namespace AC {
  class TRDRawHit;
  class TRDHTrack;
  class TRDVTrack;
}

namespace Analysis {

class SplineTrack;

/** TRD hit for use in calibration and analysis.
  *
  * Idea:
  *   - will contain more detailed information after each calibration step is applied
  *   - later: can contain local track information (coord, direction) for more general alignment algorithms
  *
  *  \note At first, all XYZ position vectors are equal, they differ only after corresponding correction has been applied,
  *        XYZ_align is most sophisticated position and is used in the Getter functions R(), L(), and Z().
  */
class TrdHit
{
public:

  TrdHit( const AC::TRDRawHit& );

  void SetCoordinateAlongWire( Double_t xy );
  void ApplyShimmingCorrection();
  void ApplyAlignmentCorrection( double time );
  void ApplyGainCorrection( double time, Double_t referenceGain );

  void FillPathlengthFromTrack( const SplineTrack& track );

  Double_t R() const { return( D==0 ? XYZ_align.X() : XYZ_align.Y() ); }
  Double_t L() const { return( D==0 ? XYZ_align.Y() : XYZ_align.X() ); }
  Double_t Z() const { return( XYZ_align.Z() ); }

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
  Short_t Direction() const { return D; }

private:
  /** 2D Path length [cm].
    * Either in XZ-plane, or YZ-plane depending on straw orientation.
    * \todo Fill 2D pathlength in hit factory...
    */
  Float_t Pathlength2D() const { return fPathlength2D; }
public:

  /** 3D Path length [cm].
    * Threedimensional trajectory length.
    */
  Float_t Pathlength3D() const { return fPathlength3D; }

  Double_t DistanceToTrack(const AC::TRDHTrack&);
  Double_t DistanceToTrack(const AC::TRDVTrack&);
  Double_t DistanceToTrack(const Analysis::SplineTrack&) const;

  Int_t GetGlobalStrawNumber() const;

  friend bool operator<( const Analysis::TrdHit& left, const Analysis::TrdHit& right );
  friend std::ostream& operator<<( std::ostream& out, const Analysis::TrdHit& h );

private:

  TrdHit();

  Short_t     Lay;              ///< Layer number
  Short_t     Lad;              ///< Ladder number
  Short_t     Tub;              ///< Straw number
  Short_t     Mod;              ///< Module (0..327)
  Short_t      GC;              ///< Manifold Gas-Circuit 1..41
  Short_t      GG;              ///< Manifold Gas-Group   1..5:W3   6..10:W7
  Short_t       D;              ///< Orientation  0:XZ-measurement  1:YZ-measurement

  Float_t     Amp_raw;          ///< Raw amplitude (unit? probably ADC counts)
  Float_t     Amp_calib;        ///< Amplitude after gain correction
  bool        IsGainCorrected;

  Float_t    fPathlength2D;      ///< calculated track pathlength in projection perpendicular to wire
  Float_t    fPathlength3D;      ///< calculated three-dimensional track pathlength

  TVector3   XYZ_nom;           ///< nominal coordinates of wire (straw tube center)
  TVector3   XYZ_along;         ///< same as XYZ_nom but with coordinate along wire set from external source (usually tracker track)
  bool       IsSecondCoordSet;
  TVector3   XYZ_shim;          ///< coordinates of hit including shimming correction and global TRD translation and rotation
  bool       IsShimmingApplied;
  TVector3   XYZ_align;         ///< coordinates of hit after alignment correction
  bool       IsAligned;

//  TVector3   Pos_uncertainty;   ///< positional uncertainty


};

bool operator<( const Analysis::TrdHit& left, const Analysis::TrdHit& right );

std::ostream& operator<<( std::ostream& out, const Analysis::TrdHit& h );

}

#endif // TRDHIT_HH
