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
    *
    * Details from $AMSWD/include/Tofdbc.h: with comments [] from t.s.
    *
    *[bit  hex-mask]
    *   0  00000001 SIDEN=1;
    *   1  00000002 SIDEP=2;
    *   2  00000004 NOHTRECOVCAD=4;//NO HT  recover Candidate
    *   3  00000008 NOMATCHRECOVCAD=8;// LT HT Not Match recover Candidate
    *   4  00000010 BAD=16;
    *   5  00000020 USED=32;         [for any TofBetaH ??]
    *   6  00000040 BADN=64;
    *   7  00000080 BADP=128;
    *   8  00000100 BADTCOO=256;
    *   9  00000200 BADTIME=512;   //Bad Time
    *  10  00000400 NOWINDOWLT=1024;//NoLT (>40ns befFT)&&(<640 befFT)
    *  11  00000800 NOWINDOWHT=2048;
    *  12  00001000 NOWINDOWSHT=4096;
    *  13  00002000 LTMANY=8192;
    *  14  00004000 HTMANY=          16384;
    *  15  00008000 NOADC=           16384*2; 
    *  16  00010000 RECOVERED=       16384*2*2;//RECOVER
    *  17  00020000 LTREFIND=        16384*2*2*2; //LTREFIND for RECOVERCAD
    *  18  00040000 TKTRACK=         16384*2*2*2*2; //TRACK BETAH
    *  19  00080000 NOFT=            16384*2*2*2*2*2;//NO FT
    *  20  00100000 BETABADCHIT=     16384*2*2*2*2*2*2;//BAD Beta-ChisT
    *  21  00200000 DOVERFLOW=       16384*2*2*2*2*2*2*2;//Dynode ADC overflow
    *  22  00400000 AOVERFLOW=       16384*2*2*2*2*2*2*2*2;//Anode ADC overflow
    *  23  00800000 TRDTRACK=        16384*2*2*2*2*2*2*2*2*2;//TRD TRACK BetaH
    *  24  01000000 BETAOVERFLOW=    16384*2*2*2*2*2*2*2*2*2*2;//Beta Fit overflow
    *  25  02000000 ECALTRACK=       16384*2*2*2*2*2*2*2*2*2*2*2;//ECAL TRACK BetaH
    *  26  04000000 TOFTRACK=        16384*2*2*2*2*2*2*2*2*2*2*2*2;//TOF TRACK BetaH
    *  27  08000000 DOVERFLOWNONLC=  16384*2*2*2*2*2*2*2*2*2*2*2*2*2;//Dynode NonLear-Correction Overflow
    *  28  10000000 AOVERFLOWNONLC=  16384*2*2*2*2*2*2*2*2*2*2*2*2*2*2;//Aynode NonLear-Correction Overflow
    *  29  20000000 DOVERFLOWBIRKC=  16384*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2;//Dynode Birk-Correction Overflow
    *  30  40000000 AOVERFLOWBIRKC=  16384*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2;//Aynode Birk-Correction Overflow
    * 
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

  /** Charge estimate from all good PMTs without path length and beta correction
   */
  Float_t Charge() const { return fCharge; }

private:
  /** Helper function to generate UChar_t fLayerBar - used by the producer only.
    */
  static UChar_t GenerateBitsFromFlags(Int_t layer, Int_t bar) {

    return ((layer & 0xf) << 4) | (bar & 0xf);
  }

  REGISTER_CLASS_WITH_TABLE(TOFCluster)
};

}

}

#endif
