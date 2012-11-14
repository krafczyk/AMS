#ifndef ECAL_h
#define ECAL_h

#include "ECALShower.h"

namespace AC {

/** Electromagnetic Calorimeter event data
  */
class ECAL {
  WTF_MAKE_FAST_ALLOCATED;
public:
  /** A vector of ECALShower objects */
  typedef Vector<ECALShower, 5> ShowersVector;

  ECAL() { }

  /** Helper method dumping an ECAL object to the console
    */
  void Dump() const;

  /** Clear ECAL to the initial empty state, as it was just constructed.
    */
  void Clear(Option_t * /*option*/ ="");

  /** Accessors for the ECALShower objects.
    */
  const ShowersVector& Showers() const { return fShowers; }

private:
  ShowersVector fShowers;

  REGISTER_CLASS(ECAL)
};

}

#endif