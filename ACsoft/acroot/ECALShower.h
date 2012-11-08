#ifndef ECALShower_h
#define ECALShower_h

#include <TMath.h>
#include "Tools.h"

namespace AC {

/** Electromagnetic Calorimeter shower data
  */
class ECALShower {
  WTF_MAKE_FAST_ALLOCATED;
public:
  /** A vector of float numbers */
  typedef Vector<Float_t, 1> EstimatorsVector;

  ECALShower()
    : fStatus(0)
    , fNumberOfHits(0)
    , fDepositedEnergy(0)
    , fEnergyAt3CMRatio(0)
    , fReconstructedEnergy(0)
    , fReconstructedEnergyError(0)
    , fRelativeRearLeak(0)
    , fShowerMaximum(0)
    , fChiSquareProfile(0)
    , fTheta(0)
    , fPhi(0)
    , fX(0)
    , fY(0)
    , fZ(0)
    , fEntryX(0)
    , fEntryY(0)
    , fEntryZ(0)
    , fExitX(0)
    , fExitY(0)
    , fExitZ(0) {

  }

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

  /** Zenith angle theta [deg].
    * See AMSDir::gettheta() (http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/point.h?view=markup).
    */
  Float_t Theta() const { return fTheta * TMath::RadToDeg(); }

  /** Azimutal angle phi [deg].
    * See AMSDir::getphi() (http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/point.h?view=markup).
    */
  Float_t Phi() const { return fPhi * TMath::RadToDeg(); }

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
  UInt_t fStatus;                    // EcalShower->Status
  UShort_t fNumberOfHits;            // EcalShower->NHits
  Float_t fDepositedEnergy;          // EcalShower->EnergyD
  Float_t fEnergyAt3CMRatio;         // EcalShower->Energy3C[0]
  Float_t fReconstructedEnergy;      // EcalShower->EnergyE
  Float_t fReconstructedEnergyError; // EcalShower->ErEnergyE
  Float_t fRelativeRearLeak;         // EcalShower->RearLeak
  Float_t fShowerMaximum;            // EcalShower->ParProfile[1]
  Float_t fChiSquareProfile;         // EcalShower->Chi2Profile
  Float_t fTheta;                    // EcalShower->EMDir           acos(Dir[2])
  Float_t fPhi;                      // EcalShower->EMDir           atan2(Dir[1],Dir[0])
  Float_t fX;                        // EcalShower->CofG[0]
  Float_t fY;                        // EcalShower->CofG[1]
  Float_t fZ;                        // EcalShower->CofG[2]
  Float_t fEntryX;                   // EcalShower->Entry[0]
  Float_t fEntryY;                   // EcalShower->Entry[1]
  Float_t fEntryZ;                   // EcalShower->Entry[2]
  Float_t fExitX;                    // EcalShower->Exit[0]
  Float_t fExitY;                    // EcalShower->Exit[1]
  Float_t fExitZ;                    // EcalShower->Exit[2]
  EstimatorsVector fEstimators;      // Estimators [0]:GetEcalBDT() [1]:EcalStandaloneEstimator()

  REGISTER_CLASS_WITH_TABLE(ECALShower)
};

}

#endif
