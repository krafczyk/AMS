#ifndef MCTRDHit_h
#define MCTRDHit_h

#include "Tools.h"
#include "MCTRDHit-Streamer.h"

namespace ACsoft {

namespace AC {

/** Monte-Carlo %TRD hit data
  */
class MCTRDHit {
  WTF_MAKE_FAST_ALLOCATED;
public:
  // FIXME: Obtain reasonable inline capacities from MC files (ask Antje).

  /** A vector of short numbers */
  typedef Vector<Float_t, 1> EnergyLossVector;

  AC_MCTRDHit_Variables

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
  REGISTER_CLASS_WITH_TABLE(MCTRDHit)
};

}

}

#endif
