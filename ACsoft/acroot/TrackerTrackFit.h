#ifndef TrackerTrackFit_h
#define TrackerTrackFit_h

#include <TMath.h>
#include "Tools.h"
#include "TRDRawHit.h"
#include "TrackerTrackFit-Streamer.h"

namespace ACsoft {

namespace AC {

/** %Tracker track fit data
  */
class TrackerTrackFit {
  WTF_MAKE_FAST_ALLOCATED;
public:
  AC_TrackerTrackFit_Variables

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

  /** 1cm above Layer 1: X coordinate [cm].
   */
  Float_t XLayer1Plus1cm() const { return fXLayer1Plus1cm; }
  
  /** 1cm above Layer 1: Y coordinate [cm].
   */
  Float_t YLayer1Plus1cm() const { return fYLayer1Plus1cm; }
  
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

  /** 1cm below TOF Upper: X coordinate [cm].
   */
  Float_t XTOFUpperMinus1cm() const { return fXTOFUpperMinus1cm; }
  
  /** 1cm below TOF Upper: Y coordinate [cm].
   */
  Float_t YTOFUpperMinus1cm() const { return fYTOFUpperMinus1cm; }
  
  /** Layer 56 (Z=0): X coordinate [cm].
    */
  Float_t XLayer56() const { return fXLayer56; }

  /** Layer 56 (Z=0): Y coordinate [cm].
    */
  Float_t YLayer56() const { return fYLayer56; }

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
  
  /** 1cm below Layer 9: X coordinate [cm].
   */
  Float_t XLayer9Minus1cm() const { return fXLayer9Minus1cm; }
  
  /** Layer 9: Y coordinate [cm].
   */
  Float_t YLayer9Minus1cm() const { return fYLayer9Minus1cm; }
  
  /** Extrapolation to zECAL coordinate from Tracker Layer 9. Returns either X or Y in cm depending on the MeasurementMode (0: X, 1: Y).
    * FIXME: REMOVE THIS FUNCTION! DO NOT ADD ANY NON DATA ACCCESSOR TO ACROOT/ CLASSES IN FUTURE!
    * THESE CLASSES ARE MEANT AS DATA CONTAINERS, NOT TO CONTAIN FUNCTIONALITY!
    */
  float Extrapolate_to_zECAL( float z, MeasurementMode mode ) const 
  {// D=0: return X   else return Y
    TVector3 v0 = TVector3(fXLayer9,fYLayer9,AC::AMSGeometry::ZTrackerLayer9);
    TVector3 v1 = TVector3(fXLayer9Minus1cm,fYLayer9Minus1cm,AC::AMSGeometry::ZTrackerLayer9 - 1.0);
    TVector3 ex = v0 - (z-AC::AMSGeometry::ZTrackerLayer9)/1.0*(v1-v0);
    if(mode == XZMeasurement) return ex.X();
    else return ex.Y();
  }
private:
  REGISTER_CLASS_WITH_TABLE(TrackerTrackFit)
};

}

}

#endif
