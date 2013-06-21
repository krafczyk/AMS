#ifndef RICHRing_h
#define RICHRing_h

#include <bitset>
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

  std::bitset<32> StatusBitset() const { return std::bitset<32>(fStatus);}
  /** Number of hits in the ring cluster.
    */ 
  UShort_t NumberOfHits() const { return fNumberOfHits; }

  /** Number of hits in the ring cluster used for ring reconstruction.
    */ 
  UShort_t NumberOfUsedHits() const { return fNumberOfUsedHits; }

  /** Normalized velocity: beta.
    */
  Float_t Beta() const { return fBeta; }

  /** Beta error.
    */
  Float_t BetaError() const { return fBetaError; }

  /** Beta consistency is the difference between the reconstructed beta of the two reconstruction methods from LIP and CIEMAT.
    */
  Float_t BetaConsistency() const { return fBetaConsistency; }

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

  /** Continuous Z estimate for this ring.
    */
  Float_t ChargeEstimate() const { return fChargeEstimate; }

  /** Number of detected photoelectrons in this ring.
    */
  Float_t NPhotoElectrons() const { return fNPhotoElectrons; }

  /** Number of expected photoelectrons in this ring.
    */
  Float_t NExpectedPhotoElectrons() const { return fNExpectedPhotoElectrons; }

  /** Number of collected photoelectrons in this event.
    */
  Float_t NCollectedPhotoElectrons() const { return fNCollectedPhotoElectrons; }

  /** Was the NaF radiator used?
   */
  bool IsNaF() const { return fIsNaF; }

  /** Width of this ring (residuals between hit position and ring)
   */
  Float_t Width() const { return fWidth;}

  /** Mean of the 1/distance^2 distribution of unused hits to the PMT hit by the incoming particle
   */
  Float_t WidthUnusedHits() const { return fWidthUnusedHits; }

  /** Distance of the impact point of the incoming particle to the Aerogel tile border. The quality of the AGL is worse at the edges.
   */
  Float_t DistanceToTileBorder() const { return fDistanceToTileBorder; }


private:
  REGISTER_CLASS_WITH_TABLE(RICHRing)
};

}

}

#endif
