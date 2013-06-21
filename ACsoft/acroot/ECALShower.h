#ifndef ECALShower_h
#define ECALShower_h

#include <TMath.h>
#include <TVector3.h>

#include "Tools.h"
#include "ECALShower-Streamer.h"

namespace ACsoft {

namespace AC {

/** Electromagnetic Calorimeter shower data
  */
class ECALShower {
  WTF_MAKE_FAST_ALLOCATED;
public:
  /** A vector of float numbers */
  typedef Vector<Float_t, 2> EstimatorsVector;

  AC_ECALShower_Variables

  /** Helper method dumping an ECALShower object to the console
    */
  void Dump() const;
  
  /** Returns ECAL shower status information. 
    * See http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/CC/amsdbc.C?view=markup for possible status codes.
   */
  UInt_t Status() const { return fStatus; }

  /** Number of hits.
    */ 
  UShort_t NumberOfHits() const { return fNumberOfHits; }
 
  /** Deposited energy including all corrections [MeV].
    * EnergyD in AMS terms.
    * See https://twiki.cern.ch/twiki/bin/view/AMS/ECALReconstruction for more details.
    */
  Float_t DepositedEnergy() const { return fDepositedEnergy; }

  /** Energy at +/-3cm divided by energy ratio.
    */
  Float_t EnergyAt3CMRatio() const { return fEnergyAt3CMRatio; }

  /** Reconstructed shower energy [GeV].
    * EnergyE in AMS terms.
    */
  Float_t ReconstructedEnergy() const { return fReconstructedEnergy; }

  /** Error of reconstructed shower energy [GeV].
    */
  Float_t ReconstructedEnergyError() const { return fReconstructedEnergyError; }

  /** Reconstructed shower energy, to be used for ECAL standalone estimator v2/v3 [GeV].
    * EnergyA in AMS terms.
    */
  Float_t ReconstructedEnergyStandaloneEstimator() const { return fReconstructedEnergyStandaloneEstimator; }


  /** Reconstructed shower energy - if it was an electron/positron [GeV].
    * GetCorrecteEnergy() with EnergyE mode and a electron/positron hypothesis, in AMS terms.
    */
  Float_t ReconstructedEnergyElectron() const { return fReconstructedEnergyElectron; }

  /** Reconstructed shower energy - if it was an photon [GeV].
    * GetCorrecteEnergy() with EnergyE mode and a electron hypothesis, in AMS terms.
    */
  Float_t ReconstructedEnergyPhoton() const { return fReconstructedEnergyPhoton; }

  /** Relative rear leak. See Section 5 of http://cdsweb.cern.ch/record/687490/files/note03_001.pdf.
    * The lead tungstate crystals used in the ECAL have a depth of 23 cm or 25.84 radiation lengths.
    * This places a limit on the size of an electromagnetic cascade that can be totally contained,
    * leading to leakage of the cascade from the back of the crystal. The quantity of energy lost
    * through this mechanism is directly proportional to the incident particle energy and the
    * position where the cascade is initiated inside the crystal.
    * \todo Verify documentation.
    */
  Float_t RelativeRearLeak() const { return fRelativeRearLeak; }

  /** Shower maximum [cm].
    * The shower maximum is the maximum of the energy deposition
    * profile - the point where the maximum amount of energy is deposited.
    * \todo Verify documentation.
    */
  Float_t ShowerMaximum() const { return fShowerMaximum; }

  /** Chi-Square of the shower profile fit.
    */
  Float_t ChiSquareProfile() const { return fChiSquareProfile; }

  /** Zenith angle theta [rad].
    * See AMSDir::gettheta() (http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/point.h?view=markup).
    */
  Float_t Theta() const { return fTheta; }

  /** Azimutal angle phi [rad].
    * See AMSDir::getphi() (http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/point.h?view=markup).
    */
  Float_t Phi() const { return fPhi; }

  /** X position in 'center of gravity' [cm].
    * The energy center of gravity (c.o.g) is calculated in a core 5 cells wide around the cluster center.
    * See https://twiki.cern.ch/twiki/bin/view/AMS/ECALReconstruction.
    */
  Float_t X() const { return fX; }

  /** Y position in 'center of gravity' [cm].
    * The energy center of gravity (c.o.g) is calculated in a core 5 cells wide around the cluster center.
    * See https://twiki.cern.ch/twiki/bin/view/AMS/ECALReconstruction.
    */
  Float_t Y() const { return fY; }

  /** Z position in 'center of gravity' [cm].
    * The energy center of gravity (c.o.g) is calculated in a core 5 cells wide around the cluster center.
    * See https://twiki.cern.ch/twiki/bin/view/AMS/ECALReconstruction.
    */
  Float_t Z() const { return fZ; }

  /** Centre of gravity.
    * \sa X()
    */
  TVector3 CentreOfGravity() const { return TVector3(X(),Y(),Z()); }

  /** X position in 'entry point' [cm].
    */
  Float_t EntryX() const { return fEntryX; }

  /** Y position in 'entry point' [cm].
    */
  Float_t EntryY() const { return fEntryY; }

  /** Z position in 'entry point' [cm].
    */
  Float_t EntryZ() const { return fEntryZ; }

  /** X position in 'exit point' [cm].
    */
  Float_t ExitX() const { return fExitX; }

  /** Y position in 'exit point' [cm].
    */
  Float_t ExitY() const { return fExitY; }

  /** Z position in 'exit point' [cm].
    */
  Float_t ExitZ() const { return fExitZ; }

  /** Returns a list of estimators for this shower.
    * Entry 1: Pisa function to obtain the Boosted Decision Tree Classifier for shower, trained on ISS.B552 data. 
    * \todo Ask Torsten why the second entry, the ecal standalone estimator, is not written.
    */
  const EstimatorsVector& Estimators() const { return fEstimators; }

private:
  REGISTER_CLASS_WITH_TABLE(ECALShower)
};

}

}

#endif
