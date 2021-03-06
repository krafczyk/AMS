#ifndef __TrSim__
#define __TrSim__

//////////////////////////////////////////////////////////////////////////
///
///\class TrSim
///\brief Procedures for the Tracker Simulation/Digitization   
///
/// Three digitizations are available:
/// 1. GBATCH default (rewritten in C++, PZ)
/// 2. Fast effective simulation (SH)
/// 3. A detailed simulation (AO)
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

#include "tkdcards.h"

#include <cmath>
#include <vector>
#include <map>
#include <algorithm>

#define VERBOSE 0
#define WARNING 0

#define DEADSTRIPADC -1000


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

  //! Impact positions
  static AMSPoint sitkrefp[trconst::maxlay];
#pragma omp threadprivate (sitkrefp)

  //! Impact angles
  static AMSPoint sitkangl[trconst::maxlay];
#pragma omp threadprivate (sitkangl)

 private:

  //! Sensors 
  static TrSimSensor _sensors[3]; 

 public:

  //! Constructor  
  TrSim() {}
  //! Destructor 
  virtual ~TrSim() {}
  //! Initialize TrSimSensors
  static void InitSensors() { for (int i = 0; i < 3; i++) _sensors[i] = TrSimSensor(i); }
  //! Get a pointer to datacard (to be used in ROOT CINT)
  static TRMCFFKEY_DEF *GetFFKEY() { return &TRMCFFKEY; }

  //! Build the TrMCClusters (one per side) 
  static void  sitkhits(int idsoft, float vect[], float edep, float step, int itra, int gtrkid=-2, int status=0);
  //! Fast simulation TrRawCluster creation (generate TrRawCluster directly from step infos) 
  static void  gencluster(int idsoft, float vect[], float edep, float step, int itra);  
  //! Generate fake noise cluster (where there is no full simulation)
  static void  sitknoise(int nsimladders,TrMap<TrMCClusterR> *MCClusterTkIdMap );
  //! Generate the TrRawClusters from the TrMCCluster (calls the specific simulation type)
  static void  sitkdigi();
  //! Add noise of the TkId ladder on the ladder buffer (from current calibration)
  static void  AddNoiseOnBuffer(double* ladbuf, TrLadCal * ladcal);
  //! Put MC ideal clusters on the ladder buffer (TrSim2010) 
  static int   AddTrSimClustersOnBuffer(TrMCClusterR* cluster, double* ladbuf);
  //! Put MC ideal clusters on the ladder buffer (GBATCH)
  static int   AddOldSimulationSignalOnBuffer(TrMCClusterR* cluster, double* ladbuf);
  //! Makes the TrRawClusters starting from a ladder buffer (1024 strips) (0: n-side, 1: p-side)
  static int   BuildTrRawClustersWithDSP(const int iside, const int tkid, TrLadCal* ladcal, double * ladbuf);

  //! Resolution from simulation 
  static void fillreso(TrTrackR *track);  
  //! Returns the sensor simulator
  static TrSimSensor* GetTrSimSensor(int side, int tkid); 
  //! Print buffer
  static void  PrintBuffer(double *_ladbuf);
  //! Merge clusters
  static void MergeMCCluster();
  //! Trying to improve the merge clusters method
  static void MergeMCCluster2(); 

  //! See simulation parameters
  static void PrintSimPars();

};

#endif

