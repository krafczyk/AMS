#ifndef BadRunManager_h
#define BadRunManager_h

#include <string>
#include <map>
#include <vector>

#include <Rtypes.h>

namespace ACsoft {

namespace Analysis {

class Particle;

/** Manage AMS BadRun lists.
  *
  * Class is implemented as a singleton.
  *
  * Bad run lists have to be stored in $ACROOTLOOKUPS/BadRuns.
  *
  * \todo Add more documentation: where to find bad run lists, how they are maintained/produced, an example how to use this class
  */
class BadRunManager {

public:
  BadRunManager();

  /** Returns an instance of the BadRunManager singleton. */
  static BadRunManager* Self() {
    static BadRunManager* gInstance = 0;
    if (!gInstance)
      gInstance = new BadRunManager;
    return gInstance;
  }

public:

  // FIXME move outside class so that documentation becomes visible in doxygen
  /** Subdetector for bad run lists. */
  enum SubD{  General = 0,
              DAQ     = 1,
              TRD     = 2,
              TOF     = 3,
              ACC     = 4,
              Tracker = 5,
              Rich    = 6,
              Ecal    = 7 };


  std::string SubdToString( SubD s ) const;
  SubD StringToSubd( std::string s ) const;

  int  AddBadRunList( const std::string& fileName );

  bool IsBad( SubD subdetector, long run, time_t UTime );
  bool IsBad( SubD subdetector, const Analysis::Particle& particle );


private:

  std::string fBadRunFilesDir;

  long   fCurrentRun;
  time_t fCurrentUTime;

  std::map<std::string,SubD> SubDNameToEnum;
  std::map<SubD,std::string> EnumToSubDName;

  void AddDefaultBadRunLists();

  void UpdateBadUTimes( const long Run );
  void UpdateBadStatus( const time_t UTime );

  static const int nSubD = 8;
  typedef std::vector<std::pair<long,long> >   BadUTimeRangeVector;

  std::map<long,BadUTimeRangeVector> BadRuns[nSubD]; // Run -> (UT0,UT1)[i]   for nSubDs

  BadUTimeRangeVector BadUTimes[nSubD];              // (UT0,UT1)[i] for nSubDs

  bool BadStatus[nSubD];                             // Bad-Status   for nSubDs

};

}

}

#endif
