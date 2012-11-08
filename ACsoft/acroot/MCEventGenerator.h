#ifndef MCEventGenerator_h
#define MCEventGenerator_h

#include <TMath.h>
#include "Tools.h"

namespace AC {

enum MCProcess {
  Unknown = 0,
  Ionisation,
  DeltaRay,
  PhotoEffect,
  TransitionRadiation,
  Bremsstrahlung,
  PairProduction,
  Annihilation,
  ComptonScattering,
  HadronicInteraction,
  RadioactiveDecay,
  CherenkovRadiation
};

/** Monte-Carlo event generator data
  */
class MCEventGenerator {
  WTF_MAKE_FAST_ALLOCATED;
public:
  MCEventGenerator()
    : fID(0)
    , fMotherParticle(0)
    , fProcess(Unknown)
    , fMomentum(0)
    , fMass(0)
    , fCharge(0)
    , fX0(0)
    , fY0(0)
    , fZ0(0)
    , fTheta(0)
    , fPhi(0) {

  }

  /** Helper method dumping an MCEventGenerator object to the console
    */
  void Dump() const;

  /** Particle Identifier a la Geant3.
    * See http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/root.h?view=markup for details.
    */
  UShort_t ID() const { return fID; }

  /** Index of mother particle.
    * \todo Nobody uses this at the moment.
    * \todo Verify documentation.
    */
  UShort_t MotherParticle() const { return fMotherParticle; }

  /** Returns creation process of secondary particle
    * \todo Nobody uses this at the moment.
    */
  MCProcess Process() const { return fProcess; }

  /** Momentum [GeV/c].
   */
  Float_t Momentum() const { return fMomentum; }

  /** Mass [GeV/c].
   */
  Float_t Mass() const { return fMass; }

  /** Charge (signed).
   */
  Float_t Charge() const { return fCharge; }

  /** X0 coordinate [cm].
    */
  Float_t X0() const { return fX0; }

  /** Y0 coordinate [cm].
    */
  Float_t Y0() const { return fY0; }

  /** Z0 coordinate [cm].
    */
  Float_t Z0() const { return fZ0; }

  /** Zenith angle theta [deg].
    * See AMSDir::gettheta() (http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/point.h?view=markup).
    */
  Float_t Theta() const { return fTheta * TMath::RadToDeg(); }

  /** Azimutal angle phi [deg].
    * See AMSDir::getphi() (http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/point.h?view=markup).
    */
  Float_t Phi() const { return fPhi * TMath::RadToDeg(); }

private:
  UShort_t fID;            	// MCEventg->Particle
  UShort_t fMotherParticle; // Index to Mother-Particle, not yet implemented
  MCProcess fProcess;      	// Process, not yet implemented
  Float_t fMomentum;       	// MCEventg->Momentum
  Float_t fMass;           	// MCEventg->Mass
  Float_t fCharge;         	// MCEventg->Charge
  Float_t fX0;             	// MCEventg->Coo[0]
  Float_t fY0;             	// MCEventg->Coo[1]
  Float_t fZ0;             	// MCEventg->Coo[2]
  Float_t fTheta;          	// MCEventg->acos(Dir[2])
  Float_t fPhi;            	// MCEventg->atan2(Dir[1],Dir[0])

  REGISTER_CLASS_WITH_TABLE(MCEventGenerator)
};

}

#endif
