#include "TrdHit.hh"

#include <assert.h>
#include <iostream>

#include <TEllipse.h>

#include "TRDRawHit.h"
#include "TRDVTrack.h"
#include "TrackerTrack.h"
#include "AMSGeometry.h"
#include "DetectorManager.hh"
#include "TrdDetector.hh"
#include "TrdStraw.hh"
#include "TrdAlignment.hh"
#include "TrdGainCalibration.hh"
#include "pathlength_functions.hh"
#include "SplineTrack.hh"
#include "dumpstreamers.hh"
#include "Palettes.hh"

#define DEBUG 0
#define INFO_OUT_TAG "TrdHit> "
#include <debugging.hh>

/** Initialize variables from hit.
  *
  */
ACsoft::Analysis::TrdHit::TrdHit( const AC::TRDRawHit& rhit ) :
  fEllipse(0)
{

  Lay = rhit.Layer();
  Lad = rhit.Ladder();
  Tub = rhit.Tube();
  fGlobalStrawNumber = rhit.Straw();
  Mod = rhit.Module();
  GC  = rhit.GasCircuit();
  GG  = rhit.GasGroup();

  fOrientation = ( Lay <= 3 || Lay >= 16 ? AC::YZMeasurement : AC::XZMeasurement ); // FIXME take from straw?

  Amp_raw = rhit.DepositedEnergy();
  Amp_calib = Amp_raw;
  IsGainCorrected = false;

  Detector::DetectorManager* dm = Detector::DetectorManager::Self();
  const Detector::TrdStraw* alignedStraw = dm->GetAlignedTrd()->GetTrdStraw(fGlobalStrawNumber);
  TubeDir   = alignedStraw->GlobalWireDirection();
  XYZ_align = alignedStraw->GlobalPosition();

  XYZ_along = XYZ_align;
  IsSecondCoordSet = false;

  fPathlength3D = 0.0;
  fTrackDistance = 0.0;
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
void ACsoft::Analysis::TrdHit::ApplyGainCorrection( double time, Double_t referenceGain ) {

  assert(!IsGainCorrected);

  Double_t lookupGain = Calibration::TrdGainParametersLookup::Self()->GetGainValue(Mod,time);

  Double_t gainCorrectionFactor = ( lookupGain > 0.0 ? referenceGain / lookupGain : 1.0 );

  Amp_calib = Amp_raw * gainCorrectionFactor;

  DEBUG_OUT << "Mod " << Mod << ": lookupGain " << lookupGain << " ref " << referenceGain
            << " corr factor " << gainCorrectionFactor << ": amp " << Amp_raw << " -> " << Amp_calib << std::endl;

  IsGainCorrected = true;
}



/** Fill pathlength, distance to track and coordinate along wire from track interpolation through tube.
  *
  */
void ACsoft::Analysis::TrdHit::FillInformationFromTrack( const ACsoft::Analysis::SplineTrack& track ) {

  assert( !IsSecondCoordSet );

  ::TVector3 trackPos, trackDir;
  track.CalculateLocalPositionAndDirection(Z(),trackPos,trackDir);

  DEBUG_OUT << "track at z=" << Z() << " trackPos: " << trackPos << " trackDir: " << trackDir << std::endl;

  fPathlength3D = Pathlength3d(AC::AMSGeometry::TRDTubeRadius,XYZ_align,TubeDir,trackPos,trackDir);

  fTrackDistance = Distance(XYZ_align,TubeDir,trackPos,trackDir);

  XYZ_along = BasePoint1(XYZ_align,TubeDir,trackPos,trackDir);

  IsSecondCoordSet = true;

  DEBUG_OUT << *this << std::endl;
}


void ACsoft::Analysis::TrdHit::Draw( bool rotatedSystem ) {

  static int FirstColor = ::Utilities::Palette::UseNicePaletteWithLessGreen();
  static int nColors    = 255;
  const static float maxAmp = 240.0; // ADC counts (corrected)

  if(fEllipse) delete fEllipse;

  float xy = fOrientation == AC::XZMeasurement ? Position3D().X() : Position3D().Y();

  float xPad = rotatedSystem ? -Position3D().Z() : xy;
  float yPad = rotatedSystem ? xy : Position3D().Z();
  fEllipse = new TEllipse(xPad,yPad,AC::AMSGeometry::TRDTubeRadius);
  float colorFraction = GetAmplitude()/maxAmp;
  if(colorFraction>1.0) colorFraction = 1.0;
  if(colorFraction<0.0) colorFraction = 0.0;
  int color = FirstColor + TMath::Nint(colorFraction*(nColors-1));
  DEBUG_OUT << "Amp: " << GetAmplitude() << " frac " << colorFraction << " color " << color << std::endl;
  fEllipse->SetFillColor(color);
  fEllipse->Draw();
}


/** Sort function.
  *
  * Sort by layer number, then ladder number, then tube number, and finally descending amplitude.
  *
  */
bool ACsoft::Analysis::operator<( const ACsoft::Analysis::TrdHit& left, const ACsoft::Analysis::TrdHit& right ){

  if( left.Lay < right.Lay ) return true;
  if( left.Lay > right.Lay ) return false;

  if( left.Lad < right.Lad ) return true;
  if( left.Lad > right.Lad ) return false;

  if( left.Tub < right.Tub ) return true;
  if( left.Tub > right.Tub ) return false;

  return( left.Amp_raw > right.Amp_raw );
}


std::ostream& ACsoft::Analysis::operator<<( std::ostream& out, const ACsoft::Analysis::TrdHit& h ) {

  out << "Lay " << h.Lay << " Lad " << h.Lad << " Tub " << h.Tub  << " Straw " << h.GlobalStrawNumber()
      << " Mod " << h.Mod << " D: " << ( h.fOrientation == AC::XZMeasurement ? "XZ" : "YZ" )
      << " AMP: raw " << h.Amp_raw << " calib " << h.Amp_calib << "\n"
      << " POS: align " << h.XYZ_align << " along " << h.XYZ_along << " tube dir " << h.TubeDir
      << " track dist " << h.fTrackDistance << " 3D pathlength " << h.fPathlength3D;

  return out;
}


