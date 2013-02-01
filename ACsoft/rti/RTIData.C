#include "RTIData.hh"

namespace ACsoft {

namespace RTI {

/** default constructor
  *
  * called by the RTIreader for every entry in the file that is beeing read.*/
RTIData::RTIData() {

  Clear();
}

void RTIData::SetData(int time, int run, int evbegin, double livetime,
                           double cutoff25N, double cutoff25P, double cutoff30N, double cutoff30P,
                           double cutoff35N, double cutoff35P, double cutoff40N, double cutoff40P,
                           double MPVHERIG, double thetaS, double phiS, double radS, double zenith,
                           double gLat, double gLong, int events, int missedEvents,
                           int level1Trigger, int parts, short good) { 

  fTime = time;
  fRun = run;
  fEvbegin = evbegin;
  fLivetime = livetime;
  fCutoff25N = cutoff25N;
  fCutoff25P = cutoff25P;
  fCutoff30N = cutoff30N;
  fCutoff30P = cutoff30P;
  fCutoff35N = cutoff35N;
  fCutoff35P = cutoff35P;
  fCutoff40N = cutoff40N;
  fCutoff40P = cutoff40P;
  fMPVHERIG = MPVHERIG;
  fThetaS = thetaS;
  fPhiS = phiS;
  fRadS = radS;
  fZenith = zenith;
  fGLat = gLat;
  fGLong = gLong;
  fEvents = events;
  fMissedEvents = missedEvents;
  fLevel1Trigger = level1Trigger;
  fParts = parts;
  fGood = good;
}

void RTIData::Clear() {

  fTime = -1;
  fRun = -1;
  fEvbegin = -1;
  fLivetime = -1;
  fCutoff25N = -1;
  fCutoff25P = -1;
  fCutoff30N = -1;
  fCutoff30P = -1;
  fCutoff35N = -1;
  fCutoff35P = -1;
  fCutoff40N = -1;
  fCutoff40P = -1;
  fMPVHERIG = -1;
  fThetaS = -1;
  fPhiS = -1;
  fRadS = -1;
  fZenith = -1;
  fGLat = -1;
  fGLong = -1;
  fEvents = -1;
  fMissedEvents = -1;
  fLevel1Trigger = -1;
  fParts = -1;
  fGood = -1;
}

}

}
