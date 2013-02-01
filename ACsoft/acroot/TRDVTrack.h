#ifndef TRDVTrack_h
#define TRDVTrack_h

#include <TMath.h>
#include "Tools.h"
#include "TRDRawHit.h"
#include "TRDVTrack-Streamer.h"

namespace ACsoft {

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

  AC_TRDVTrack_Variables

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
  REGISTER_CLASS_WITH_TABLE(TRDVTrack)
};

}

}

#endif
