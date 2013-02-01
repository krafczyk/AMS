#ifndef Particle_h
#define Particle_h

#include "Tools.h"
#include "Particle-Streamer.h"
#include <limits>

namespace ACsoft {

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

  AC_Particle_Variables

  /** Helper method dumping an %Particle object to the console
    */
  void Dump() const;

  /** Defines status bits that may be queried via the individual accessor functions below.
    */
  enum StatusBits {
    FlagIsInSolarArrayShadow               = 1 << 0,
    FlagTrackerTrackIndexFromBetaH         = 1 << 1,
    FlagTRDVTrackIndexFromBetaH            = 1 << 2,
    FlagECALShowerIndexFromBetaH           = 1 << 3,
    FlagGalacticCoordinatesFromBacktracing = 1 << 4,
    FlagGalacticCoordinatesFromStarTracker = 1 << 5,
    FlagGalacticCoordinatesFromOrbit       = 1 << 6
  };

  /** Backtracing Status for negative rigidity
    *  0: not available
    *  1: from SPACE
    *  2: from ATMOSPHERE
    *  3: TRAPPED (neither 1 nor 2 after 100 steps) 
    */
  int BacktracingStatusNegativeRigidity() const { return (fStatus>>8)&3; }

  /** Backtracing Status for positive rigidity
    *  0: not available
    *  1: from SPACE
    *  2: from ATMOSPHERE
    *  3: TRAPPED (neither 1 nor 2 after 100 steps) 
    */
  int BacktracingStatusPositiveRigidity() const { return (fStatus>>10)&3; }

  /** Backtracing Status for negative energy
    *  0: not available
    *  1: from SPACE
    *  2: from ATMOSPHERE
    *  3: TRAPPED (neither 1 nor 2 after 100 steps) 
    */
  int BacktracingStatusNegativeEnergy() const { return (fStatus>>12)&3; }

  /** Backtracing Status for positive energy
    *  0: not available
    *  1: from SPACE
    *  2: from ATMOSPHERE
    *  3: TRAPPED (neither 1 nor 2 after 100 steps) 
    */
  int BacktracingStatusPositiveEnergy() const { return (fStatus>>14)&3; }

  /** Returns true if AMS is covered by ISS solar arrays.
    */
  bool IsInSolarArrayShadow() const { return (fStatus & FlagIsInSolarArrayShadow) == FlagIsInSolarArrayShadow; }

  /** Returns true if the TrackerTrack index points to the one linked in the AMS BetaH class.
    * This is always the case if the TOFBetaHIndex() is not equal to -1.
    */
  bool TrackerTrackIndexFromBetaH() const { return (fStatus & FlagTrackerTrackIndexFromBetaH) == FlagTrackerTrackIndexFromBetaH; }

  /** Returns true if the TRDVTrack index points to the one linked in the AMS BetaH class.
    * This is always the case if the TOFBetaHIndex() is not equal to -1.
    */
  bool TRDVTrackIndexFromBetaH() const { return (fStatus & FlagTRDVTrackIndexFromBetaH) == FlagTRDVTrackIndexFromBetaH; }

  /** Returns true if the ECALShower index points to the one linked in the AMS BetaH class.
   * This is always the case if the TOFBetaHIndex() is not equal to -1.
   */
  bool ECALShowerIndexFromBetaH() const { return (fStatus & FlagECALShowerIndexFromBetaH) == FlagECALShowerIndexFromBetaH; }

  /** Returns true if galactic coordinates were computed from backtracing with BTstatus==1.
   */
  bool GalacticCoordinatesFromBacktracing() const { return (fStatus & FlagGalacticCoordinatesFromBacktracing) == FlagGalacticCoordinatesFromBacktracing; }

  /** Returns true if galactic coordinates originate from the star tracker.
   */
  bool GalacticCoordinatesFromStarTracker() const { return (fStatus & FlagGalacticCoordinatesFromStarTracker) == FlagGalacticCoordinatesFromStarTracker; }

  /** Returns true if galactic coordinates were sucessfully computed from ISS-Orbit parameters (limited accurancy).
   */
  bool GalacticCoordinatesFromOrbit() const { return (fStatus & FlagGalacticCoordinatesFromOrbit) == FlagGalacticCoordinatesFromOrbit; }

  /** Stoermer cut off for negative rigidities/energies.
    */
  Float_t StoermerCutoffNegative() const { return fStoermerCutoffNegative; }

  /** Stoermer cut off for positive rigidities/energies.
    */
  Float_t StoermerCutoffPositive() const { return fStoermerCutoffPositive; }

  /** Maximum Stoermer cutoff returns >0: from positive or <0: from negative particle
    */
  Float_t StoermerMaxCutoff() const { return fabs(fStoermerCutoffNegative) > fStoermerCutoffPositive ? fStoermerCutoffNegative : fStoermerCutoffPositive;  }

  /** Galactic Longitude from Backtracing [deg] if GalacticCoordinatesFromBacktracing() == true  -  else  Float_Max-1. 
    * Particle.BT_glong   if BT_status==1 from AMSEventR::DoBacktracing(Particle Momentum/Beta/Charge/Theta/Phi) 
    */
  Float_t GalacticLongitudeFromBacktracing() const {

    if (GalacticCoordinatesFromBacktracing())
       return fGalacticLongitudeFromBacktracing; 
     return std::numeric_limits<float>::max() - 1.0;
  }

  /** Galactic Latitude from Backtracing [deg] if GalacticCoordinatesFromBacktracing() == true  -  else  Float_Max-1.
    * Particle.BT_glat    if BT_status==1 from AMSEventR::DoBacktracing(Particle Momentum/Beta/Charge/Theta/Phi) 
    */
  Float_t GalacticLatitudeFromBacktracing() const {

    if (GalacticCoordinatesFromBacktracing())
       return fGalacticLatitudeFromBacktracing; 
     return std::numeric_limits<float>::max() - 1.0;
  }

  /** Galactic Longitude from Orbit [deg] if GalacticCoordinatesFromOrbit() == true  -  else  Float_max-1.
    * glong from ISS orbit frame transformation AMSEvent::GetGalCoo(result, glong, glatt, Particle.Theta, Particle.Phi)
    */
  Float_t GalacticLongitudeFromOrbit() const {

    if (GalacticCoordinatesFromOrbit())
       return fGalacticLongitudeFromOrbit; 
     return std::numeric_limits<float>::max() - 1.0;
  }

  /** Galactic Latitude from Orbit [deg] if GalacticCoordinatesFromOrbit() == true  -  else  Float_Max-1.
    * glat from ISS orbit orame transformation AMSEvent::GetGalCoo(result, glong, glat, Particle.Theta, Particle.Phi)
    */
  Float_t GalacticLatitudeFromOrbit() const {

    if (GalacticCoordinatesFromOrbit())
       return fGalacticLatitudeFromOrbit;
     return std::numeric_limits<float>::max() - 1.0;
  }

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

private:
  REGISTER_CLASS_WITH_TABLE(Particle)
};

}

}

#endif
