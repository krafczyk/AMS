#ifndef __TrSim__
#define __TrSim__

//////////////////////////////////////////////////////////////////////////
///
///\class TrSim
///\brief Procedures for the Tracker Simulation  
///
/// Three Simulation are available:
/// 1) GBATCH default (old one)
/// 2) Fast effective simulation (SH)
/// 3) Detailed simulation (AO)
///
//////////////////////////////////////////////////////////////////////////

#include "TrSimSensor.h"
#include "TrSimCluster.h"

#include "TrLadCal.h"

#include "TkSens.h"
#include "TkDBc.h"
#include "TrTrack.h"
#include "TrMap.h"
#include "TrMCCluster.h"
#include "VCon.h"
#include "HistoMan.h"

#include <cmath>
#include <vector>
#include <map>
#include <algorithm>

#define VERBOSE 0
#define WARNING 0

#define DEADSTRIPADC -1000

using namespace std;

class TrSim {

 public: 

  enum ESimulationType {
    /// Raw Simulation (old GBATCH default)
    kRawSim = 0,
    /// Skip Raw Simulation (no TrMCCluster, fast-effective simulation)
    kNoRawSim = 1,
    /// TrSim2010 (new simulation with some detail)
    kTrSim2010 = 2
  };

 private:

  //! Self pointer 
  static TrSim* Head;

  //! Sensor map
  vector<TrSimSensor*> _sensors;
  //! TkId of current ladder
  int       _tkid;
  //! Calibration of current ladder
  TrLadCal* _ladcal;
  //! Buffer to expand signal of current ladder
  double    _ladbuf[1024];
  //! Global 2 Local coordinate
  TkSens*   _glo2loc;

  //! A map for the MC Cluster search
  TrMap<TrMCClusterR> MCClusterTkIdMap;

  //! Particle List (the 18/05/2010 one)
  static float _g3mass[213];
  static float _g3charge[213];

 public:

  ////////////////////////
  // Common Methods 
  ////////////////////////

  //! Constructor (simulator initialization)
  TrSim();
  //! Destructor 
  ~TrSim() {}
  //! GetHead (constructor or getter ...)
  static TrSim* GetHead();

  //! Build the TrMCCluster (one per side, if (SkipRawSim!=1)) 
  void sitkhits(int idsoft, float vect[], float edep, float step, int itra);
  //! Generate Non Gaussians Noise Cluster (dummy for the moment)
  void sitknoise(){}
  //! Generate the TrRawClusters from the TrMCCluster and from a calibration
  void sitkdigi();
  //! DSP behaviour
  static void DSP_Clusterize(int tkid,float *buf);  

  ////////////////////////
  // Methods for RawSimulation
  ////////////////////////

  //! The original GBATCH digitization method
  static void sitkdigiold();	

  ////////////////////////
  // Methods for SkipRawSimulation
  ////////////////////////

  //! Fast simulation (generate TrRawCluster directly from Geant infos) 
  void gencluster(int idsoft, float vect[], float edep, float step, int itra);
  
  //! Impact positions
  static AMSPoint sitkrefp[trconst::maxlay];
  //! Impact angles
  static AMSPoint sitkangl[trconst::maxlay];
  //! Resolution from simulation 
  static void fillreso(TrTrackR *track);
  
  ////////////////////////
  // Methods for TrSim2010
  ////////////////////////

  //! Get the sensor simulator for a given ladder/side
  TrSimSensor* GetTrSimSensor(int side, int tkid);

  //! Makes the TrRawClusters starting from the MC clusters
  int  BuildTrRawClusters();
  //! Makes the TrRawClusters starting from the ladder buffer (1024 strips)
  int  BuildTrRawClustersWithDSP();
  //! Makes the TrRawClusters starting from the ladder buffer for a side (0: n-side, 1: p-side)
  int  BuildTrRawClustersOnSide(int iside);
  //! Create the MC Cluster TkId map
  void CreateMCClusterTkIdMap();
  //! Clean the ladder buffer 
  void CleanBuffer();
  //! Print buffer
  void PrintBuffer();
  //! Add noise of the TkId ladder on the ladder buffer (from current calibration)
  void AddNoiseOnBuffer();
  //! Produce clusters and put them on the ladder buffer 
  void AddSimulatedClustersOnBuffer();

};

#endif

