#ifndef MCEventGenerator_h
#define MCEventGenerator_h

#include <TMath.h>
#include "Tools.h"
#include "MCEventGenerator-Streamer.h"

#ifndef __CINT__
#include <QDataStream>
#endif

namespace ACsoft {

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

#ifndef __CINT__
// QDataStream operators
inline QDataStream& operator<<(QDataStream& stream, const MCProcess& object) {

  stream << (UChar_t) object;
  return stream;
}

inline QDataStream& operator>>(QDataStream& stream, MCProcess& object) {

  UChar_t process;
  stream >> process;
  object = (MCProcess) process;
  return stream;
}
#endif

/** Monte-Carlo event generator data
  */
class MCEventGenerator {
  WTF_MAKE_FAST_ALLOCATED;
public:
  AC_MCEventGenerator_Variables

  /** Helper method dumping an MCEventGenerator object to the console
    */
  void Dump() const;

  /** Particle Identifier a la Geant3.
    * See http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/root.h?view=markup for details.
    */
  Short_t ParticleID() const { return fParticleID; }

  /** Geant4 Track ID
    */
  UShort_t TrackID() const { return fTrackID; }

  /** Index of mother particle.
    * \todo Nobody uses this at the moment.
    * \todo Verify documentation.
    */
  UShort_t MotherParticle() const { return fMotherParticle; }

  /** Returns creation process of secondary particle
    * \todo Nobody uses this at the moment.
    */
  MCProcess Process() const { return static_cast<MCProcess>(fProcess); }

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
  REGISTER_CLASS_WITH_TABLE(MCEventGenerator)
};

}

}

#endif
