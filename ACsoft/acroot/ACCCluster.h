#ifndef ACCCluster_h
#define ACCCluster_h

#include "Tools.h"

namespace AC {

/** Anti Coincidence Counter cluster event data
  */
class ACCCluster {
  WTF_MAKE_FAST_ALLOCATED;
public:
  ACCCluster()
    : fStatus(0)
    , fPhi(0)
    , fZ(0)
    , fTime(0)
    , fEnergy(0) {

  }

  /** Helper method dumping an ACCCluster object to the console
    */
  void Dump() const;

  /** Returns ACC cluster status information. 
    * See http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/root.h?view=markup for details.
    * \li \c Bit \c "128": No FT-coinc. on 2 sides;
    * \li \c Bit \c "256": 1sideSector;
    * \li \c Bit \c "1024":  miss.side#.
    * \todo Verify documentation.
    */
  UInt_t Status() const { return fStatus; }
 
  /** Cluster angle Phi [deg].
    */ 
  Float_t Phi() const { return fPhi; }
  
  /** Cluster Z coordinate [cm].
    */
  Float_t Z() const { return fZ; }

  /** Cluster time [ns].
    */
  Float_t Time() const { return fTime; }

  /** Desposited energy in cluster [MeV].
    */
  Float_t Energy() const { return fEnergy; }

private:
  UInt_t fStatus;  // AntiCluster->Status
  Float_t fPhi;    // AntiCluster->Coo[1]
  Float_t fZ;      // AntiCluster->Coo[2]
  Float_t fTime;   // AntiCluster->Times[i] closest to 0
  Float_t fEnergy; // AntiCluster->Edep

  REGISTER_CLASS_WITH_TABLE(ACCCluster)
};

}

#endif
