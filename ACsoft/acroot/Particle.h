#ifndef Particle_h
#define Particle_h

#include "Tools.h"

namespace AC {

/** AMS %Particle data
  */
class Particle {
  WTF_MAKE_FAST_ALLOCATED;
public:
  /** A vector of char (8bit) numbers */
  typedef Vector<Char_t, 3> ChargesVector;

  /** A vector of float numbers */
  typedef Vector<Float_t, 3> ChargesProbabilityVector;

  Particle()
    : fID(0)
    , fIDProbability(0)
    , fMomentum(0)
    , fMomentumError(0)
    , fMass(0)
    , fMassError(0)
    , fBeta(0)
    , fBetaError(0)
    , fTOFBetaIndex(0)
    , fTRDVTrackIndex(0)
    , fTRDHTrackIndex(0)
    , fTrackerTrackIndex(0)
    , fRICHRingIndex(0)
    , fECALShowerIndex(0) {

  }

  /** Helper method dumping an %Particle object to the console
    */
  void Dump() const;

  /** Return charge with the highest probability
   */
  Int_t MostProbableCharge() const;

  /** %Particle Identifier a-la Geant3.
    * See http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/root.h?view=markup for details.
    */
  Int_t ID() const { return fID; }

  /** Probability that the predicted type is correct.
    */
  Float_t IDProbability() const { return fIDProbability; }

  /** Momentum [GeV/c].
    * Note: particles with negative charge will have negative momentum.
    */
  Float_t Momentum() const { return fMomentum; }

  /** Momentum error [GeV/c].
    */
  Float_t MomentumError() const { return fMomentumError; }

  /** Mass [GeV/c^2].
    * Note: particles with abs(Beta())>1 will have negative mass.
    */
  Float_t Mass() const { return fMass; }

  /** Mass error [GeV/c^2].
    */
  Float_t MassError() const { return fMassError; }

  /** Normalized velocity: beta.
    */
  Float_t Beta() const { return fBeta; }

  /** Beta error.
    */
  Float_t BetaError() const { return fBetaError; }

  /** Associated TOFBeta index.
    */
  Short_t TOFBetaIndex() const { return fTOFBetaIndex; }

  /** Associated TRDVTrack index.
    */
  Short_t TRDVTrackIndex() const { return fTRDVTrackIndex; }

  /** Associated TRDHTrack index.
    */
  Short_t TRDHTrackIndex() const { return fTRDHTrackIndex; }

  /** Associated TrackerTrack index.
    */
  Short_t TrackerTrackIndex() const { return fTrackerTrackIndex; }

  /** Associated RICHRing index.
    */
  Short_t RICHRingIndex() const { return fRICHRingIndex; }

  /** Associated ECALShower index.
    */
  Short_t ECALShowerIndex() const { return fECALShowerIndex; }

  /** Charge indexes sorted in descending probability (0:e, 1:H, 2:He ...)
    * Our reduced data only contains the first two entries.
    */
  const ChargesVector& Charges() const { return fCharges; }

  /** Probabilities for the entries in the Charges() list.
    */
  const ChargesProbabilityVector& ChargesProbability() const { return fChargesProbability; }

private:
  Int_t fID;                                    // Particle.Particle (Geant3-PID)
  Float_t fIDProbability;                       // Particle.Prob[0] Probability for ID
  Float_t fMomentum;                            // Particle.Momentum
  Float_t fMomentumError;                       // Particle.ErrMomentum
  Float_t fMass;                                // Particle.Mass
  Float_t fMassError;                           // Particle.ErrMass
  Float_t fBeta;                                // Particle.Beta   (Tof + Rich if available)
  Float_t fBetaError;                           // Particle.ErrBeta
  Short_t fTOFBetaIndex;                        // Particle.iBeta()
  Short_t fTRDVTrackIndex;                      // Particle.iTrdTrack()
  Short_t fTRDHTrackIndex;                      // Particle.iTrdHTrack()
  Short_t fTrackerTrackIndex;                   // Particle.iTrTrack()
  Short_t fRICHRingIndex;                       // Particle.iRichRingB()
  Short_t fECALShowerIndex;                     // Particle.iEcalShower()
  ChargesVector fCharges;                       // Particle.pCharge.ChargeI[0..1]
  ChargesProbabilityVector fChargesProbability; // Particle.pCharge.Prob[0..1]

  REGISTER_CLASS_WITH_TABLE(Particle)
};

}

#endif
