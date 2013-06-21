#ifndef Vertex_h
#define Vertex_h

#include "Tools.h"
#include "Vertex-Streamer.h"

namespace ACsoft {

namespace AC {

/** %Vertex from photon conversion
  */
class Vertex {
  WTF_MAKE_FAST_ALLOCATED;
public:
  AC_Vertex_Variables

  /** Helper method dumping a Vertex object to the console */
  void Dump() const;

  /** Vertex position in X / cm */
  Float_t X() const {return fX;}

  /** Vertex position in Y / cm */
  Float_t Y() const {return fY;}

  /** Vertex position in Z / cm */
  Float_t Z() const {return fZ;}

  /** Reconstructed photon direction: Polar angle Theta / rad */
  Float_t Theta() const {return fTheta;}
  
  /** Reconstructed photon direction: Azimuth angle Phi / rad */
  Float_t Phi() const {return fPhi;}
  
  /** Total momentum at the vertex / GeV */
  Float_t Momentum() const {return fMomentum;}

  /** Galactic longitude calculated from fTheta and fPhi / deg */
  Float_t GalacticLongitude() const {return fGalacticLongitude;}

  /** Galactic latitude calculated from fTheta and fPhi / deg */
  Float_t GalacticLatitude() const {return fGalacticLatitude;}

  /** Index in Tracker TrackFits vector for the first track associated with this vertex */
  UChar_t FirstTrackIndex() const {return fFirstTrackIndex;}

  /** Index in Tracker TrackFits vector for the second track associated with this vertex */
  UChar_t SecondTrackIndex() const {return fSecondTrackIndex;}

private:
  REGISTER_CLASS_WITH_TABLE(Vertex)

};

}

}

#endif /* Vertex_h */
