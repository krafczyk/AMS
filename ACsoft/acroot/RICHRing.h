#ifndef RICHRing_h
#define RICHRing_h

#include <TMath.h>
#include "Tools.h"
#include "RICHRing-Streamer.h"

namespace ACsoft {

namespace AC {

/** Ring-Imaging Cherenkov %Detector per-ring data
  */
class RICHRing {
  WTF_MAKE_FAST_ALLOCATED;
public:
  /** A vector of char (8bit) numbers */
  typedef Vector<Char_t, 3> ChargesVector;

  /** A vector of float numbers */
  typedef Vector<Float_t, 3> ChargesProbabilityVector;

  AC_RICHRing_Variables

  /** Helper method dumping an RICHRing object to the console
    */
  void Dump() const;
  
  /** Returns RICH ring status information.
    * Details:
    * Ring status bits (counting from 1 to 32)
    * \li 1 - Ring has been rebuild after cleaning PMTs apparently crossed by a charged particle.
    *       If the rebuilding has been succesful it is stored in the next ring in the rings container.
    *       However to confirm that next ring is indeed a rebuilt one, both tracks, the one from the
    *       current and the one from the next, should be the same. Otherwise the rebuilding was unsuccesful.
    * \li 2 - Ring reconstructed using the NaF radiator in the double radiator configuration
    * \li 14 - Associated to a particle
    * \li 16-30 reserved
    * \li 31 - Associated to a track used in a gamma reconstruction.
    *
    * See http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/root.h?view=markup for more details.
    * \todo Add documentation.
   */
  UInt_t Status() const { return fStatus; }

  /** Number of hits in the ring cluster.
    */ 
  UShort_t NumberOfHits() const { return fNumberOfHits; }

  /** Normalized velocity: beta.
    */
  Float_t Beta() const { return fBeta; }

  /** Beta error.
    */
  Float_t BetaError() const { return fBetaError; }

  /** X coordinate [cm].
    * Note: These are radiator crossing track parameters.
    */
  Float_t X() const { return fX; }

  /** Y coordinate [cm].
    * Note: These are radiator crossing track parameters.
    */
  Float_t Y() const { return fY; }

  /** Z coordinate [cm].
    * Note: These are radiator crossing track parameters.
    */
  Float_t Z() const { return fZ; }

  /** Zenith angle theta [deg].
    * Note: These are radiator crossing track parameters.
    * See AMSDir::gettheta() (http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/point.h?view=markup).
    */
  Float_t Theta() const { return fTheta * TMath::RadToDeg(); }

  /** Azimutal angle phi [deg].
    * Note: These are radiator crossing track parameters.
    * See AMSDir::getphi() (http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/point.h?view=markup).
    */
  Float_t Phi() const { return fPhi * TMath::RadToDeg(); }

  /** Kolmogorov test probability.
    * Quality parameter, providing the probability result of applying a Kolmogorov test to the distribution of charge along the ring.
    * This quantity is almost uniformly distributed between 0 and 1 for rings correctly reconstructed, and peaks at 0 for incorrectly reconstructed ones.
    */
  Float_t Probability() const { return fProbability; }

  /** Continuous Z^2 estimate for this ring.
    * \todo Find out which unit this uses.
    */
  Float_t ChargeEstimate() const { return fChargeEstimate; }

private:
  REGISTER_CLASS_WITH_TABLE(RICHRing)
};

}

}

#endif
