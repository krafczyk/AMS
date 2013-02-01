#ifndef ACCCluster_h
#define ACCCluster_h

#include "Tools.h"
#include "ACCCluster-Streamer.h"

namespace ACsoft {

namespace AC {

/** Anti Coincidence Counter cluster event data
  */
class ACCCluster {
  WTF_MAKE_FAST_ALLOCATED;
public:
  AC_ACCCluster_Variables

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
  REGISTER_CLASS_WITH_TABLE(ACCCluster)
};

}

}

#endif
