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

  //! A map for the MC Cluster search
  static TrMap<TrMCClusterR> MCClusterTkIdMap;
  
public:

  ////////////////////////
  // Common Methods 
  ////////////////////////

  //! Constructor (simulator initialization)
  TrSim(){}
  //! Destructor 
  ~TrSim() {MCClusterTkIdMap.Clear();}

  //! Build the TrMCCluster (one per side, if (SkipRawSim!=1)) 
  static void sitkhits(int idsoft, float vect[], float edep, float step, int itra);
  //! Generate Non Gaussians Noise Cluster (dummy for the moment)
  static  void sitknoise(){}
  //! Generate the TrRawClusters from the TrMCCluster and from a calibration
  static  void sitkdigi();
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
  static void gencluster(int idsoft, float vect[], float edep, float step, int itra);
  
  //! Impact positions
  static AMSPoint sitkrefp[trconst::maxlay];
  //! Impact angles
  static AMSPoint sitkangl[trconst::maxlay];
  //! Resolution from simulation 
  static void fillreso(TrTrackR *track);
  
  ////////////////////////
  // Methods for TrSim2010
  ////////////////////////

  //! Makes the TrRawClusters starting from the MC clusters
  static   int  BuildTrRawClusters();
  //! Makes the TrRawClusters starting from the ladder buffer (1024 strips)(0: n-side, 1: p-side)
  static int BuildTrRawClustersWithDSP(const int iside, const int _tkid,  TrLadCal* _ladcal,double * _ladbuf);

  //! Create the MC Cluster TkId map
  static   void CreateMCClusterTkIdMap();
  //! Print buffer
  static   void PrintBuffer(double *_ladbuf);
  //! Add noise of the TkId ladder on the ladder buffer (from current calibration)
  static   void AddNoiseOnBuffer(double* _ladbuf,TrLadCal * _ladcal);
  //! Produce clusters and put them on the ladder buffer 
  static   int  AddSimulatedClustersOnBuffer(int _tkid,double* _ladbuf);



  static int AddSimClusterNew(TrMCClusterR* cluster,double* _ladbuf);

};

#endif

