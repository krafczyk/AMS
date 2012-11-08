#include "TrdHit.hh"

#include <assert.h>
#include <iostream>

#include "TRDRawHit.h"
#include "TRDVTrack.h"
#include "TrackerTrack.h"

#include <TrdAlignment.hh>
#include <TrdGainCalibration.hh>
#include <pathlength_functions.hh>
#include <SplineTrack.hh>

#include "dumpstreamers.hh"

#define DEBUG 0
#define INFO_OUT_TAG "TrdHit> "
#include <debugging.hh>

/** Initialize variables from hit.
  *
  */
Analysis::TrdHit::TrdHit( const AC::TRDRawHit& rhit ) {

  Lay = rhit.Layer();
  Lad = rhit.Ladder();
  Tub = rhit.Tube();
  Mod = rhit.Module();
  GC  = rhit.GasCircuit();
  GG  = rhit.GasGroup();
  D   = rhit.Direction();

  Amp_raw = rhit.DepositedEnergy();
  Amp_calib = Amp_raw;
  IsGainCorrected = false;

  XYZ_nom = TVector3(rhit.X(), rhit.Y(), rhit.Z());
  XYZ_along = XYZ_nom;
  XYZ_shim  = XYZ_nom;
  XYZ_align = XYZ_nom;

  IsSecondCoordSet = false;
  IsShimmingApplied = false;
  IsAligned = false;

  fPathlength2D = 0.0;
  fPathlength3D = 0.0;
}


/** Apply the gain correction.
  *
  * Hit amplitude is scaled by a correction factor, such that
  * the Landau MPV of the dE/dx distribution of protons at the
  * reference rigidity used in TrdGainCalibration equals the
  * given \p referenceGain.
  *
  * \param time Event time (seconds since epoch).
  * \param referenceGain reference dE/dx (ADC/cm) as described above.
  *
  */
void Analysis::TrdHit::ApplyGainCorrection( double time, Double_t referenceGain ) {

  assert(!IsGainCorrected);

  Double_t lookupGain = Calibration::TrdGainParametersLookup::Self()->GetGainValue(Mod,time);

  Double_t gainCorrectionFactor = ( lookupGain > 0.0 ? referenceGain / lookupGain : 1.0 );

  Amp_calib = Amp_raw * gainCorrectionFactor;

  DEBUG_OUT << "Mod " << Mod << ": lookupGain " << lookupGain << " ref " << referenceGain
            << " corr factor " << gainCorrectionFactor << ": amp " << Amp_raw << " -> " << Amp_calib << std::endl;

  IsGainCorrected = true;
}


/** Set the coordinate along the wire from an external source.
  *
  * The external source will usually be a tracker track.
  *
  * The internal bookkeeping is updated as well.
  */
void Analysis::TrdHit::SetCoordinateAlongWire( Double_t xy ) {

  assert( !IsSecondCoordSet && !IsShimmingApplied && !IsAligned );

  if( D==0 )
    XYZ_along.SetY(xy);
  else
    XYZ_along.SetX(xy);

  IsSecondCoordSet = true;
  XYZ_shim = XYZ_along;
  XYZ_align = XYZ_shim;
}


/** Apply shimming corrections and global TRD shift and rotation.
  *
  * The internal bookkeeping is updated as well.
  */
void Analysis::TrdHit::ApplyShimmingCorrection() {

  assert( !IsShimmingApplied && !IsAligned );

  Float_t Dx,Dy,Dz;
  float secondCoordinate = XYZ_along[1-D];
  AC::AMSGeometry::Self()->ApplyShimmingCorrection(GetGlobalStrawNumber(), secondCoordinate, Dx, Dy, Dz);

  XYZ_shim = TVector3(XYZ_along.X() + Dx, XYZ_along.Y() + Dy, XYZ_along.Z() + Dz);

  IsShimmingApplied = true;
  XYZ_align = XYZ_shim;
}


/** Apply alignment correction.
  *
  * Shift coordinate perpendicular to wire and z by effective alignment shift.
  *
  * \param time Event time (seconds since epoch).
  */
void Analysis::TrdHit::ApplyAlignmentCorrection( double time ) {

  assert( !IsAligned );

  Double_t shift = Calibration::TrdAlignmentShiftLookup::Self()->GetAlignmentShift(Mod,time);

  DEBUG_OUT << "Mod " << Mod << ": applying shift " << shift << std::endl;

  if(D==0)
    XYZ_align.SetX(XYZ_align.X()-shift);
  else
    XYZ_align.SetY(XYZ_align.Y()-shift);

  IsAligned = true;
}

/** Fill pathlength from track interpolation through tube.
  *
  */
void Analysis::TrdHit::FillPathlengthFromTrack( const Analysis::SplineTrack& track ) {

  /// \todo fill fPathlength2D
  /// \todo get doxygen to print documentation of auxiliary functions
  fPathlength3D = track.PathLength3D(D, XYZ_align);
}


/** Sort function.
  *
  * Sort by layer number, then ladder number, then straw number, and finally descending amplitude.
  *
  */
bool Analysis::operator<( const Analysis::TrdHit& left, const Analysis::TrdHit& right ){

  if( left.Lay < right.Lay ) return true;
  if( left.Lay > right.Lay ) return false;

  if( left.Lad < right.Lad ) return true;
  if( left.Lad > right.Lad ) return false;

  if( left.Tub < right.Tub ) return true;
  if( left.Tub > right.Tub ) return false;

  return( left.Amp_raw > right.Amp_raw );
}

/** Find Distance of Hit to H Track in cm
 * \todo Simplify the three DistanceToTrack methods once TrackFactory is done.
 * \todo HTrack does not "exist" at the moment!
 * currently it is a dummy and simply returns 5000.0
 */
Double_t Analysis::TrdHit::DistanceToTrack(const AC::TRDHTrack&){

  // FIXME: Implement this!
  return -666;
}

/** Find Distance of Hit to V Track in cm
 *
 */
Double_t Analysis::TrdHit::DistanceToTrack(const AC::TRDVTrack& trk){

  return (R() - trk.Extrapolate_to_z(Z(), (AC::MeasurementMode)D));
}

/** Find Distance of Hit to TRK Track in cm
 *
 */
Double_t Analysis::TrdHit::DistanceToTrack(const Analysis::SplineTrack& trk) const {

 return trk.TrackResidual(D, Z(), R());
}

/** Calculate the global Straw number 0 to 5247 according to Hcalib.
 * \todo Obsolete, once geometry classes are fully implemented.
 */
Int_t Analysis::TrdHit::GetGlobalStrawNumber() const {

  Int_t StrawNumber=-1;

  Int_t layer =Layer();
  Int_t ladder=Ladder();
  Int_t tube  =Tube();

  if(layer>3)ladder+=14;
  if(layer>7)ladder+=16;
  if(layer>11)ladder+=16;
  if(layer>15)ladder+=18;

  StrawNumber = (ladder*4+layer%4)*16+tube;

  return StrawNumber;
}

std::ostream& Analysis::operator<<( std::ostream& out, const Analysis::TrdHit& h ) {

  out << "Lay " << h.Lay << " Lad " << h.Lad << " Tub " << h.Tub << " Mod " << h.Mod << " D: "
      << ( h.D == 0 ? "XZ" : "YZ" ) << " AMP: raw " << h.Amp_raw << " calib " << h.Amp_calib << "\n"
      << " POS: nom " << h.XYZ_nom << " along " << h.XYZ_along << " shim " << h.XYZ_shim
      << " align " << h.XYZ_align << " pathlengths: 2D " << h.fPathlength2D << " 3D " << h.fPathlength3D;

  return out;
}

