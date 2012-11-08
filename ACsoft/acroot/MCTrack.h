#ifndef MCTrack_h
#define MCTrack_h

#include "Tools.h"

namespace AC {

/** Monte-Carlo track data
  */
class MCTrack {
  WTF_MAKE_FAST_ALLOCATED;
public:
  MCTrack()
    : fX0(0)
    , fY0(0)
    , fZ0(0) {

  }

  /** Helper method dumping an MCTrack object to the console
    */
  void Dump() const;

  /** Name of volume where track originates.
    */
  const Char_t* VolumeName() const { return fVolumeName; }

  /** X0 coordinate [cm].
    */
  Float_t X0() const { return fX0; }

  /** Y0 coordinate [cm].
    */
  Float_t Y0() const { return fY0; }

  /** Z0 coordinate [cm].
    */
  Float_t Z0() const { return fZ0; }

private:
  Char_t fVolumeName[5]; // MCtrack->VolName
  Float_t fX0;           // MCtrack->Pos[0]
  Float_t fY0;           // MCtrack->Pos[1]
  Float_t fZ0;           // MCtrack->Pos[2]

  REGISTER_CLASS_WITH_TABLE(MCTrack)
};

}

#endif
