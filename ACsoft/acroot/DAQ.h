#ifndef DAQ_h
#define DAQ_h

#include "Tools.h"
#include "DAQ-Streamer.h"

namespace ACsoft {

namespace AC {

/** %DAQ data
  */
class DAQ {
  WTF_MAKE_FAST_ALLOCATED;
public:
  /* A vector of unsigned short numbers */
  typedef Vector<UShort_t, 4> JINJStatusVector;

  /* A vector of 8-bit (UChar_t) numbers */
  typedef Vector<UChar_t, 24> SlaveStatusVector;

  AC_DAQ_Variables
 
  /** Accessor for the L3 error codes.
    * \todo Add real documentation and accessors for the relevant variables instead of this.
    */
  ULong64_t L3Errors() const { return fL3Errors; }

  /** Accessor for the JINJ error codes.
    * \todo Add real documentation and accessors for the relevant variables instead of this.
    */
  const JINJStatusVector& JINJStatus() const { return fJINJStatus; }

  /** Accessor for the JINF error codes.
    * \todo Add real documentation and accessors for the relevant variables instead of this.
    */
  const SlaveStatusVector& SlaveStatus() const { return fSlaveStatus; }

  /** Helper method dumping an DAQ object to the console
    */
  void Dump() const;

  /** Clear object to the initial empty state, as it was just constructed.
    */
  void Clear();

private:
  REGISTER_CLASS_WITH_TABLE(DAQ)
};

}

}

#endif

