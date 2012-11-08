#ifndef CUTFACTORY_HH
#define CUTFACTORY_HH

#include <string>
#include <vector>
#include <TObject.h>

namespace Cuts {

class Cut;

class CutFactory {
public: 
  /** Returns a singleton of the CutFactory class */
  static CutFactory* Self() {
    static CutFactory* gInstance = 0;
    if (!gInstance)
      gInstance = new CutFactory;
    return gInstance;
  }

  /** Create the CutXXX subclass for the given cut identifier */
  Cut* CreateCut(std::string name) const;

private:
  CutFactory() { }
  ~CutFactory() { }
};

}

#endif // CUTFACTORY_HH
