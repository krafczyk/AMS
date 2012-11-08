#ifndef TRDVTrack_h
#define TRDVTrack_h

#include <TMath.h>
#include "Tools.h"
#include "TRDRawHit.h"

namespace AC {

/** %TRD track data (v style)
  * \todo Add documentation.
  */
class TRDVTrack {
  WTF_MAKE_FAST_ALLOCATED;
public:
  /** A vector of char (8bit) numbers */
  typedef Vector<Char_t, 11> ChargesVector;

  /** A vector of float numbers */
  typedef Vector<Float_t, 11> ChargesProbabilityVector;

  /** A vector of float numbers */
  typedef Vector<Float_t, 3> ChargesNewVector;

  TRDVTrack()
    : fStatus(0)
    , fChiSquare(0)
    , fTheta(0)
    , fPhi(0)
    , fX(0)
    , fY(0)
    , fZ(0)
    , fPattern(0) {

  }

  /** Helper method dumping an TRDVTrack object to the console
    */
  void Dump() const;
  
  /** Helper method to calulate the charge & error.
    */
  ChargeAndError GetChargeAndErrorNew() const;

  /** TRDVTrack status information.
    * \todo Add documentation.
   */
  UShort_t Status() const { return fStatus; }

  /** Chi-Square of the track fit.
    * \todo Verify documentation.
    */
  Float_t ChiSquare() const { return fChiSquare; }

  /** Zenith angle theta [deg].
    * See AMSDir::gettheta() (http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/point.h?view=markup).
    */
  Float_t Theta() const { return fTheta * TMath::RadToDeg(); }

  /** Azimutal angle phi [deg].
    * See AMSDir::getphi() (http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/point.h?view=markup).
    */
  Float_t Phi() const { return fPhi * TMath::RadToDeg(); }

  /** X position [cm].
    */
  Float_t X() const { return fX; }

  /** Y position [cm].
    */
  Float_t Y() const { return fY; }

  /** Z position [cm].
    */
  Float_t Z() const { return fZ; }

  /** Pattern
    * \todo Add documentation.    
   */
  UInt_t Pattern() const { return fPattern; }

  /** Charge indexes sorted in descending probability (0:e, 1:H, 2:He ...)
    */
  const ChargesVector& Charges() const { return fCharges; }

  /** Probabilities for the entries in the Charges() list.
    */
  const ChargesProbabilityVector& ChargesProbability() const { return fChargesProbability; }

  /** Measured charges in layers.
    */
  const ChargesNewVector& ChargesNew() const { return fChargesNew; }

  /** Extrapolate_to_z
    * \todo Add documentation.
    */
  float Extrapolate_to_z( float z, MeasurementMode mode ) const {    // D=0: return X   else return Y
    if(mode == XZMeasurement) return fX+(z-fZ)*tan(fTheta)*cos(fPhi);
    else     return fY+(z-fZ)*tan(fTheta)*sin(fPhi);
  }

private:
  UShort_t fStatus;                             // TrdTrack->Status
  Float_t fChiSquare;                           // TrdTrack->Chi2
  Float_t fTheta;                               // TrdTrack->Theta
  Float_t fPhi;                                 // TrdTrack->Phi
  Float_t fX;                                   // TrdTrack->Coo[0]
  Float_t fY;                                   // TrdTrack->Coo[1]
  Float_t fZ;                                   // TrdTrack->Coo[2]
  UInt_t fPattern;                              // TrdTrack->Pattern
  ChargesVector fCharges;                       // Particle.pCharge.ChargeSubD("AMSChargeTRD")->ChargeI[0..1]
  ChargesProbabilityVector fChargesProbability; // Particle.pCharge.ChargeSubD("AMSChargeTRD")->Prob[0..1]
  ChargesNewVector fChargesNew;                 // Kunin ChargeProbs  0:e  1:p  2:He 3:Li 4:Be 5:B 6:C ...

  REGISTER_CLASS_WITH_TABLE(TRDVTrack)
};

}

#endif
