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
  * Bad run lists have to be stored in $ACROOTSOFTWARE/acroot/data/BadRuns.
  *
  * Note concerning TRD bad run lists: TRD runs are separated into four disjoint categories:
  *  - A: everything OK
  *  - B: minor problems that could affect separation power
  *  - C: major problems that will likely affect separation power
  *  - D: unusable runs
  *
  *  Details are found in the corresponding files in the BadRuns directory.
  *
  *
  * \todo Add more documentation: how they are maintained/produced, an example how to use this class
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

  /** Subdetector for bad run lists. */
  enum SubD{  General                = 0,
              DAQ                    = 1,
              TRDB                   = 2,
              TRDC                   = 3,
              TRDD                   = 4,
              TOF                    = 5,
              ACC                    = 6,
              Tracker                = 7,
              Rich                   = 8,
              Ecal                   = 9 };
  // WARNING: adjust value of nSubD if you change this

  std::string SubdToString( SubD s ) const;
  SubD StringToSubd( std::string s ) const;

  int  AddBadRunList( const std::string& fileName );

  bool IsBad( SubD subdetector, long run, time_t UTime );
  bool IsBad( SubD subdetector, const Analysis::Particle& particle );


private:

  static const int nSubD = 10;

  std::string fBadRunFilesDir;

  long   fCurrentRun;
  time_t fCurrentUTime;

  std::map<std::string,SubD> SubDNameToEnum;
  std::map<SubD,std::string> EnumToSubDName;

  void AddDefaultBadRunLists();

  void UpdateBadUTimes( const long Run );
  void UpdateBadStatus( const time_t UTime );

  typedef std::vector<std::pair<long,long> >   BadUTimeRangeVector;

  std::map<long,BadUTimeRangeVector> BadRuns[nSubD]; // Run -> (UT0,UT1)[i]   for nSubDs

  BadUTimeRangeVector BadUTimes[nSubD];              // (UT0,UT1)[i] for nSubDs

  bool BadStatus[nSubD];                             // Bad-Status   for nSubDs

};

}

}

#endif
