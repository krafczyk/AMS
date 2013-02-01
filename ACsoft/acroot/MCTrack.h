#ifndef MCTrack_h
#define MCTrack_h

#include "Tools.h"
#include "MCTrack-Streamer.h"

namespace ACsoft {

namespace AC {

/** Monte-Carlo track data
  */
class MCTrack {
  WTF_MAKE_FAST_ALLOCATED;
public:
  AC_MCTrack_Variables

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
  REGISTER_CLASS_WITH_TABLE(MCTrack)
};

}

}

#endif
