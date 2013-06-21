#ifndef Utilities_DeadStrawLookup_hh
#define Utilities_DeadStrawLookup_hh

#include <vector>
#include <TTimeStamp.h>

namespace ACsoft {

namespace Utilities {

  struct DeadStraw {
    DeadStraw()
      : straw(-1)
      , deadBegin(-999)
      , deadEnd(-999) {

    }

    short straw;
    double deadBegin;
    double deadEnd;
    TTimeStamp timeBegin;
    TTimeStamp timeEnd;
  };

  /** This class loads the TrdQt dead straw identifiers lookup file and allows to query for dead straws.
    */
  class DeadStrawLookup {
  public:
    static DeadStrawLookup* Self() {

      static DeadStrawLookup* gInstance = 0;
      if (!gInstance)
        gInstance = new DeadStrawLookup();
      return gInstance;
    }

    bool IsStrawDead(unsigned short straw, double secondCoordinate, const TTimeStamp&);
    void DumpDeadStraws();

  private:
    DeadStrawLookup();
    bool ReadDeadStrawLookupFile();

    std::vector<DeadStraw> fDeadStraws;
  };
}

}

#endif
