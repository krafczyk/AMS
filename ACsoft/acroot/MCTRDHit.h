#ifndef MCTRDHit_h
#define MCTRDHit_h

#include "Tools.h"

namespace AC {

/** Monte-Carlo %TRD hit data
  */
class MCTRDHit {
  WTF_MAKE_FAST_ALLOCATED;
public:
  // FIXME: Obtain reasonable inline capacities from MC files (ask Antje).

  /** A vector of short numbers */
  typedef Vector<Float_t, 1> EnergyLossVector;

  MCTRDHit()
    : fID(0), fLLT(0), fKineticEnergy(0), fDepositedEnergy(0), fStep(0)
  {

  }

  /** Helper method dumping an MCTRDHit object to the console
    */
  void Dump() const;

  /** Particle Identifier a la Geant3. Negative if secondary particle
    * See http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/root.h?view=markup for details.
    */
  Short_t ID() const { return fID; }

  /** Returns Layer/Ladder/Tube encoded in following format:
    * (Layer<<11)+(Ladder)<<6+(Tube)
    * \todo Eventually add individual accessors for them.
    */
  UShort_t LLT() const { return fLLT; }

  /** Energy loss data
          * Fill [0] with TrdMCCluster->Edep for now
    * Note: [+Eprim;[-Etr];+Step];[+Esec];[-Edray]
    * \todo Verify documentation.
    */
  const EnergyLossVector& EnergyLoss() const { return fEnergyLoss; }

  /** Kinetic energy of particle [GeV]
                */
  Float_t KineticEnergy() const {return fKineticEnergy;}

  /** Deposited energy of particle [GeV]
                */
  Float_t DepositedEnergy() const {return fDepositedEnergy;}

  /** Step size [cm] between two processes
                */
  Float_t StepSize() const {return fStep;}

private:
  Short_t fID;                  // TrdMCCluster->ParticleNo
  UShort_t fLLT;                // TrdMCCluster-> (Layer<<11)+(Ladder)<<6+(Tube)
  EnergyLossVector fEnergyLoss; // dE/Dx: E/keV*320 S/cm*32000  [+Eprim;[-Etr];+Step];[+Esec];[-Edray]
  Float_t fKineticEnergy;       // TrdMCCluster->Ekin; Kinetic energy of particle [GeV]
  Float_t fDepositedEnergy;     // TrdMCCluster->Edep; Deposited energy of particle [GeV]
  Float_t fStep;                // TrdMCCluster->Step; Step size [cm] between processes

  REGISTER_CLASS_WITH_TABLE(MCTRDHit)
};

}

#endif
