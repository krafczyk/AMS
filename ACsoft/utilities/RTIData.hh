#ifndef IO_RTIData_hh
#define IO_RTIData_hh

namespace ACsoft {

namespace Utilities {

/** RTI data format:
  * It maps the unix time against all the informations stored in the rti file. ( Also time! See getter functions)
  * if there was no data taking, only the unixtime is available. Therefore a default RTIdata object is returned in this case.
  * The default values are -1. to test if the full information is available test if run > 0 or something comparable.
  *
  */
class RTIData {
public:
  RTIData();
  
  void SetData(int time,
               int run,
               int evbegin,
               double livetime,
               double cutoff25N,
               double cutoff25P,
               double cutoff30N,
               double cutoff30P,
               double cutoff35N,
               double cutoff35P,
               double cutoff40N,
               double cutoff40P,
               double MPVHERIG,
               double thetaS,
               double phiS,
               double radS,
               double zenith,
               double gLat,
               double gLong,
               int events,
               int missedEvents,
               int level1Trigger,
               int parts,
               short good);

  /** set to default state, all -1 */
  void Clear();

  /** getter for the unix time */
  int GetTime() const { return fTime; }

  /** getter for the run number */
  int GetRun() const { return fRun;}

  /** getter for the event index since the begin of the run */
  int GetEvBegin() const { return fEvbegin; }

  /** getter for the LiveTime */
  double GetLiveTime() const { return fLivetime;}

  /** getter for the stoermer cone cutoff with an 25 degree opening angle considering negative particle. */
  double GetCutoff25N() const { return fCutoff25N;}

  /** getter for the stoermer cone cutoff with an 25 degree opening angle considering positive particle. */
  double GetCutoff25P() const { return fCutoff25P; }

  /** getter for the stoermer cone cutoff with an 25 degree opening angle considering negative particle. */
  double GetCutoff30N() const { return fCutoff30N; }

  /** getter for the stoermer cone cutoff with an 25 degree opening angle considering positive particle. */
  double GetCutoff30P() const { return fCutoff30P; }

  /** getter for the stoermer cone cutoff with an 25 degree opening angle considering negative particle. */
  double GetCutoff35N() const { return fCutoff35N; }

  /** getter for the stoermer cone cutoff with an 25 degree opening angle considering positive particle. */
  double GetCutoff35P() const { return fCutoff35P; }

  /** getter for the stoermer cone cutoff with an 25 degree opening angle considering negative particle. */
  double GetCutoff40N() const { return fCutoff40N; }

  /** getter for the stoermer cone cutoff with an 25 degree opening angle considering positive particle. */
  double GetCutoff40P() const { return fCutoff40P; }

  /** getter for the most propable rigidity under the assumtion of helium particles. */
  double GetMPVHERIG() const { return fMPVHERIG; }

  /** getter for the geographic Latitude. */
  double GetThetaS() const { return fThetaS;}

  /** getter for the gegraphic Longitude */
  double GetPhiS() const { return fPhiS;}

  /** getter for the Altitude ( above earth center assuming R_e = 6378137m ) in km */
  double GetRadS() const { return fRadS;}

  /** getter for the Zenith angle */
  double GetZenith() const { return fZenith; }

  /** getter for Galactic Latitude */
  double GetGLat() const { return fGLat;}

  /** getter for Galactic Longitude */
  double GetGLong() const { return fGLong;}

  /** getter for the number events recorded in the unix second. */
  int GetEvents() const { return fEvents; }

  /** getter for the number of missed Events in the unix second */
  int GetMissedEvents() const { return fMissedEvents; }

  /** getter for the number of level 1 trigger in the unix second */
  int GetLevel1Trigger() const { return fLevel1Trigger; }

  /** getter for the number of particles recorded in the unix second ( tracker + tof + trd + ecal associated ) */
  int GetParticles() const { return fParts;}

  /** getter for the status (0 : good ; -1 : no events in this second) */
  short IsGood() const  {return fGood;}

private:
  int fTime;
  int fRun;
  int fEvbegin;
  double fLivetime;
  double fCutoff25N;
  double fCutoff25P;
  double fCutoff30N;
  double fCutoff30P;
  double fCutoff35N;
  double fCutoff35P;
  double fCutoff40N;
  double fCutoff40P;
  double fMPVHERIG;
  double fThetaS;
  double fPhiS;
  double fRadS;
  double fZenith;
  double fGLat;
  double fGLong;
  int fEvents;
  int fMissedEvents;
  int fLevel1Trigger;
  int fParts;
  short fGood;
};

};

}

#endif // IO_RTIData_hh
