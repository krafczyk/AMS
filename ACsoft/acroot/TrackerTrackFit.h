#ifndef TrackerTrackFit_h
#define TrackerTrackFit_h

#include <TMath.h>
#include "Tools.h"
#include <AMSGeometry.h>
#include <TRDRawHit.h>

namespace AC {

/** %Tracker track fit data
  */
class TrackerTrackFit {
  WTF_MAKE_FAST_ALLOCATED;
public:
  TrackerTrackFit()
    : fParameters(0)
    , fRigidity(0)
    , fInverseRigidityError(0)
    , fChiSquareNormalizedX(0)
    , fChiSquareNormalizedY(0)
    , fThetaLayer1(0)
    , fPhiLayer1(0)
    , fXLayer1(0)
    , fYLayer1(0)
    , fXTRDUpper(0)
    , fYTRDUpper(0)
    , fXTRDCenter(0)
    , fYTRDCenter(0)
    , fXTRDLower(0)
    , fYTRDLower(0)
    , fThetaTOFUpper(0)
    , fPhiTOFUpper(0)
    , fXTOFUpper(0)
    , fYTOFUpper(0)
    , fThetaRICH(0)
    , fPhiRICH(0)
    , fXRICH(0)
    , fYRICH(0)
    , fThetaLayer9(0)
    , fPhiLayer9(0)
    , fXLayer9(0)
    , fYLayer9(0) {

  }

  /** Helper method dumping an TrackerTrackFit object to the console
    */
  void Dump() const;

  /** Track fit algorithm
    * \todo Add documentation.
   */
  Int_t Algorithm() const { return (fParameters >> 6) & 3; }

  /** Track fit pattern
    * \todo Add documentation.
   */
  Int_t Pattern() const { return (fParameters >> 3) & 7; }

  /** Track refit mode
    * \todo Add documentation.
   */
  Int_t Refit() const { return fParameters & 7; }

  /** Rigidity [GV].
    * Note: The unit is giga-volts.
    */ 
  Float_t Rigidity() const { return fRigidity; }

  /** Inverse rigidity error [1/GV].
    * Note: The unit is 1/(giga-volts).
    */ 
  Float_t InverseRigidityError() const { return fInverseRigidityError; }

  /** Chi-Square normalized y
    * \todo Add documentation.
    */ 
  Float_t ChiSquareNormalizedX() const { return fChiSquareNormalizedX; }

  /** Chi-Square normalized Y
    * \todo Add documentation.
    */ 
  Float_t ChiSquareNormalizedY() const { return fChiSquareNormalizedY; }

  /** Layer 1: Zenith angle theta [deg].
    * See AMSDir::gettheta() (http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/point.h?view=markup).
    */
  Float_t ThetaLayer1() const { return fThetaLayer1 * TMath::RadToDeg(); }

  /** Layer 1: Azimutal angle phi [deg].
    * See AMSDir::getphi() (http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/point.h?view=markup).
    */
  Float_t PhiLayer1() const { return fPhiLayer1 * TMath::RadToDeg(); }

  /** Layer 1: X coordinate [cm].
    */
  Float_t XLayer1() const { return fXLayer1; }

  /** Layer 1: Y coordinate [cm].
    */
  Float_t YLayer1() const { return fYLayer1; }

  /** TRD Upper: X coordinate [cm].
    */
  Float_t XTRDUpper() const { return fXTRDUpper; }

  /** TRD Upper: Y coordinate [cm].
    */
  Float_t YTRDUpper() const { return fYTRDUpper; }

  /** TRD Center: X coordinate [cm].
    */
  Float_t XTRDCenter() const { return fXTRDCenter; }

  /** TRD Center: Y coordinate [cm].
    */
  Float_t YTRDCenter() const { return fYTRDCenter; }

  /** TRD Lower: X coordinate [cm].
    */
  Float_t XTRDLower() const { return fXTRDLower; }

  /** TRD Lower: Y coordinate [cm].
    */
  Float_t YTRDLower() const { return fYTRDLower; }

  /** TOF Upper: Zenith angle theta [deg].
    * See AMSDir::gettheta() (http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/point.h?view=markup).
    */
  Float_t ThetaTOFUpper() const { return fThetaTOFUpper * TMath::RadToDeg(); }

  /** TOF Upper: Azimutal angle phi [deg].
    * See AMSDir::getphi() (http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/point.h?view=markup).
    */
  Float_t PhiTOFUpper() const { return fPhiTOFUpper * TMath::RadToDeg(); }

  /** TOF Upper: X coordinate [cm].
    */
  Float_t XTOFUpper() const { return fXTOFUpper; }

  /** TOF Upper: Y coordinate [cm].
    */
  Float_t YTOFUpper() const { return fYTOFUpper; }

  /** RICH: Zenith angle theta [deg].
    * See AMSDir::gettheta() (http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/point.h?view=markup).
    */
  Float_t ThetaRICH() const { return fThetaRICH * TMath::RadToDeg(); }

  /** RICH: Azimutal angle phi [deg].
    * See AMSDir::getphi() (http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/point.h?view=markup).
    */
  Float_t PhiRICH() const { return fPhiRICH * TMath::RadToDeg(); }

  /** RICH: X coordinate [cm].
    */
  Float_t XRICH() const { return fXRICH; }

  /** RICH: Y coordinate [cm].
    */
  Float_t YRICH() const { return fYRICH; }

  /** Layer 9: Zenith angle theta [deg].
    * See AMSDir::gettheta() (http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/point.h?view=markup).
    */
  Float_t ThetaLayer9() const { return fThetaLayer9 * TMath::RadToDeg(); }

  /** Layer 9: Azimutal angle phi [deg].
    * See AMSDir::getphi() (http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/point.h?view=markup).
    */
  Float_t PhiLayer9() const { return fPhiLayer9 * TMath::RadToDeg(); }

  /** Layer 9: X coordinate [cm].
    */
  Float_t XLayer9() const { return fXLayer9; }

  /** Layer 9: Y coordinate [cm].
    */
  Float_t YLayer9() const { return fYLayer9; }
  
  /** Extrapolation to zECAL coordinate from Tracker Layer 9. Returns either X or Y in cm depending on the MeasurementMode (0: X, 1: Y).
    */
  float Extrapolate_to_zECAL( float z, MeasurementMode mode ) const 
  {// D=0: return X   else return Y
    if(mode == XZMeasurement) return fXLayer9 + (z - (AC::AMSGeometry::ZTrackerLayer9))*tan(fThetaLayer9)*cos(fPhiLayer9);
    else return fYLayer9 + (z - (AC::AMSGeometry::ZTrackerLayer9))*tan(fThetaLayer9)*sin(fPhiLayer9);
  }

private:
  UChar_t fParameters;           // kAlgo{0..2}<<6 | kPatt{0..7}<<3 | kRefit{0..5}
  Float_t fRigidity;             // TrTrack->GetRigidity(kFit)
  Float_t fInverseRigidityError; // TrTrack->GetErrRinv(kFit)  (error of 1/R)
  Float_t fChiSquareNormalizedX; // TrTrack->GetNormChisqX(kFit)
  Float_t fChiSquareNormalizedY; // TrTrack->GetNormChisqY(kFit)

  Float_t fThetaLayer1;          // TrTrack->Theta  |
  Float_t fPhiLayer1;            // TrTrack->Phi    |   Z = Layer-1N  +159.067 cm
  Float_t fXLayer1;              // TrTrack->Coo[0] |
  Float_t fYLayer1;              // TrTrack->Coo[1] /
  
  Float_t fXTRDUpper;            // TrTrack->Coo[0] |   Z =  TRD-Upper   +136.75 cm
  Float_t fYTRDUpper;            // TrTrack->Coo[1] /
  Float_t fXTRDCenter;           // TrTrack->Coo[0] |   Z =  TRD-Center  +113.55 cm
  Float_t fYTRDCenter;           // TrTrack->Coo[1] /
  Float_t fXTRDLower;            // TrTrack->Coo[0] |   Z =  TRD-Lower    +90.35 cm
  Float_t fYTRDLower;            // TrTrack->Coo[1] /

  Float_t fThetaTOFUpper;        // TrTrack->Theta  |
  Float_t fPhiTOFUpper;          // TrTrack->Phi    |   Z = Upper-TOF  +63.65 cm (between Plane 1/2)
  Float_t fXTOFUpper;            // TrTrack->Coo[0] |
  Float_t fYTOFUpper;            // TrTrack->Coo[1] /
 
  Float_t fThetaRICH;            // TrTrack->Theta  |
  Float_t fPhiRICH;              // TrTrack->Phi    |   Z =  RICH  -73.6 cm (Center of Radiator)
  Float_t fXRICH;                // TrTrack->Coo[0] |
  Float_t fYRICH;                // TrTrack->Coo[1] /

  Float_t fThetaLayer9;          // TrTrack->Theta  |
  Float_t fPhiLayer9;            // TrTrack->Phi    |   Z =  Layer-9   -136.041 cm
  Float_t fXLayer9;              // TrTrack->Coo[0] |
  Float_t fYLayer9;              // TrTrack->Coo[1] /

  REGISTER_CLASS_WITH_TABLE(TrackerTrackFit)
};

}

#endif
