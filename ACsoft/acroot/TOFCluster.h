#ifndef TOFCluster_h
#define TOFCluster_h

#include "Tools.h"
#include "TOFCluster-Streamer.h"

namespace ACsoft {

namespace AC {

/** Time-Of-Flight cluster data
  */
class TOFCluster {
  WTF_MAKE_FAST_ALLOCATED;
public:
  AC_TOFCluster_Variables

  /** Helper method dumping an TOFCluster object to the console
    */
  void Dump() const;
  
  /** Returns TOF cluster status information. 
    * Details:
    * bit3(4)  -> ambig
    * bit5(16) -> BAD((bit9 | bit10) & !bit12))
    * bit8 is ORed over cluster-members
    * See http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/root.h?view=markup for more details.
   */
  UInt_t Status() const { return fStatus; }

  /** TOF layer
    * Details: plane 1 (top) ... 4
    * See http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/root.h?view=markup for more details.
   */ 
  Int_t Layer() const { return (fLayerBar >> 4) & 0xf; }

  /** TOF bar
    * Details: 1 ... 10
    * See http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/root.h?view=markup for more details.
   */
  Int_t Bar() const { return fLayerBar & 0xf; }

  /** Cluster X coordinate [cm].
    */
  Float_t X() const { return fX; }

  /** Cluster Y coordinate [cm].
    */
  Float_t Y() const { return fY; }

  /** Cluster Z coordinate [cm].
    */
  Float_t Z() const { return fZ; }

  /** Cluster time [s].
    */
  Float_t Time() const { return fTime; }

  /** Error of cluster time [s].
    */
  Float_t TimeError() const { return fTimeError; }

  /** Desposited energy in cluster [MeV].
    */
  Float_t Energy() const { return fEnergy; }

  /** Helper function to generate UChar_t fLayerBar - used by the producer only.
    * \todo Remove this logic from here.
    */
  static UChar_t GenerateBitsFromFlags(Int_t layer, Int_t bar) {

    return ((layer & 0xf) << 4) | (bar & 0xf);
  }

private:
  REGISTER_CLASS_WITH_TABLE(TOFCluster)
};

}

}

#endif
