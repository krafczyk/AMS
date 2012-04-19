// $Id: TofRecon.h,v 1.2 2012/04/19 16:11:45 oliva Exp $

#ifndef __TofRecon__
#define __TofRecon__

#include "TrTrack.h"
#include "TofTrack.h"
#include "TofGeometry.h"

#include "TObject.h"
#include "TH2.h"
#include "TFile.h"

#include <vector>
#include <cmath>

// this declaration avoids problems with dictionary construction
class AMSEventR;
class AMSPoint;
class AMSDir;
class HistoMan;

//! A class to manage the UTOF and LTOF candidates for the TOF track reconstruction
/*!
 *  A TofCandidate is a couple of good clusters on two different layers of UTOF (or LTOF).
 *  A TofTrack is constructed from UTOF and LTOF candidates.
 *  The TofCandidate could be constructed by two cluster, or just one. Use GetNClusters() to understand this.
 */
class TofCandidate {
 private:
  //! Upper ('U') or Lower ('L') candidate 
  char type;
  //! Upper cluster
  TofClusterR* cl_upper;
  //! Lower cluster
  TofClusterR* cl_lower;
  //! Number of clusters 
  int nclusters;
 public:
  //! c-tor 
  TofCandidate(char kind, TofClusterR* u, TofClusterR* l) { type = kind; nclusters = 2; cl_upper = u; cl_lower = l; }
  //! c-tor (for single clusters) 
  TofCandidate(int kind, TofClusterR* s) { type = kind; nclusters = 1; cl_upper = s; cl_lower = s; }
  //! d-tor
  ~TofCandidate() { type = ' '; nclusters = 0; cl_upper = 0; cl_lower = 0; }
  //! Get type name 
  char GetType() { return type; }
  //! get number of independent clusters
  int GetNClusters() { return nclusters; } 
  //! get upper or lower cluster (the same in case of single clusters)
  TofClusterR* GetCluster(int ii) { return (ii==0) ? cl_upper : cl_lower; }
  //! dX in cm (0 in case of single cluster)
  float GetdX()       { return ( (cl_upper)&&(cl_lower) ) ? cl_upper->Coo[0] - cl_lower->Coo[0] : 0; }
  //! dY in cm (0 in case of single cluster)
  float GetdY()       { return ( (cl_upper)&&(cl_lower) ) ? cl_upper->Coo[1] - cl_lower->Coo[1] : 0; }
  //! Distance (cm), 0 in case of single cluster
  float GetDistance() { return sqrt(GetdX()*GetdX() + GetdY()*GetdY()); }
  //! Get upper cluster signal estimation
  float GetUpperSignal() { return (cl_upper) ? cl_upper->Edep : 0; }
  //! Get lower cluster signal estimation
  float GetLowerSignal() { return (cl_lower) ? cl_lower->Edep : 0; }
  //! Get average signal 
  float GetAverageSignal() { return (GetNClusters()>0) ? (GetUpperSignal()+GetLowerSignal())/2 : 0; }
  //! Print
  void  Print() { for (int i=0; i<nclusters; i++) if (GetCluster(i)) printf("%1d %1d %2d | ",i,GetCluster(i)->Layer,GetCluster(i)->Bar); printf("\n"); }

  //! ROOT
  ClassDef(TofCandidate,1);
};


//! Data structure for the TofRecon ntuple filling. 
/*!
 *  Used only for monitoring and performances purposes of TofTrack.
 */
typedef struct {
  //! Number of points 
  Int_t   nPoints;
  //! TOF pattern
  Int_t   Pattern;
  //! X (cm)
  Float_t X;
  //! Y (cm)
  Float_t Y;
  //! Theta (rad)
  Float_t Theta;
  //! Phi (rad)
  Float_t Phi;
  //! Reduced spatial chi-squared
  Float_t ChiSqXY;
  //! Beta
  Float_t Beta;
  //! Reduced time chi-squared 
  Float_t ChiSqT;
  //! Anode truncated mean (c.u.)
  Float_t AnodeTruncMean;
  //! Anode RMS (c.u.)
  Float_t AnodeRMS;  
  //! Dynode truncated mean (c.u.)
  Float_t DynodeTruncMean;
  //! Dynode RMS (c.u.)
  Float_t DynodeRMS;
} TOF_TRACK;


//! A class to reconstruct TOF tracks.
/*!
 *  The class provides also the possibility of filling histograms with reconstruction properties,
 *  or the writing of a tree with the main reconstructed properties of the TOF track.
 *  There is also some methods for the comparison with the GBATCH beta class. 
*/
class TofRecon {

 public:

  ///////////////////////////////////////////
  /// Reconstruction parameters
  ///////////////////////////////////////////

  //! Minimum number of hits requested to reconstruct a track
  static int   MinNHits; 
  //! Maximum TOF candidate distance to be accepted (cm)
  static float MaxDistance; 
  //! Charge compatibility between TOF candidates (sqrt(MeV) offset)
  static float OffsetCharge1; 
  //! Charge compatibility between UTOF and LTOF (sqrt(MeV) offset)
  static float OffsetCharge2;
  //! Maximum time chisquare allowed
  static float MaxRedChiSqT;
  //! Maximum spatial (standalone) chisquared allowed 
  static float MaxRedChiSqXY;
  //! Maximum anode charge relative error allowed
  static float MaxRelErrAnode;
  //! Maximum dynode charge relative error allowed (if possible)
  static float MaxRelErrDynode;
  //! Trk-Tof match: Minimum difference on Y at Tracker layer 2 Z 
  static float MindYForGoodMatch;
  //! Trk-Tof match: Minimum charge difference 
  static float MindQForGoodMatch;

  ///////////////////////////////////////////
  /// Containers
  ///////////////////////////////////////////

  //! TOF Tracks Container 
  static vector<TofTrack*> TofTracksList;
  //! Association of TofTracks to TrTrack
  static vector< vector<TrTrackR*> > TrTracksList;

  ///////////////////////////////////////////
  /// Enable/Disable ancillary stuff
  ///////////////////////////////////////////

  //! Enable the histogram filling/writing
  static bool      EnabledHistograms;
  //! Histogram manager 
  static HistoMan  TofReconHisto;
  //! Enable the ntuple filling/writing
  static bool      EnabledNtuple;
  //! TTree for TOF track related quantities
  static TTree*    TofReconTree;
  //! Structure for the TOF track related info storing
  static TOF_TRACK Tof_Track;

 public:

  /** @name TOF Standalone Reconstruction  
    * 
    * Methods for the TofTrack building. The procedure select first good hits
    * then combines them into possible tracks. Finally loose cuts are applied.
    * The list of contructed TofTrack is available via NTofTrack() and pTofTrack(i)
    * methods.
    */
  /**@{*/
  //! Reconstruct TofTracks (level = 0: My standalone reconstruction, 1: Convert \c BetaR objects into ToF track objects, 2: Both (BetaR before)) 
  static int       BuildTofTracks(AMSEventR* event, int level = 0);
  //! Clean the list of TofTrack
  static void      ClearTofTracks();
  //! Get number of TofTracks in the list 
  static int       NTofTrack() { return int(TofTracksList.size()); }
  //! Get a TofTrack track by index
  static TofTrack* pTofTrack(int i) { return ((i<NTofTrack())&&(i>=0)) ? TofTracksList.at(i) : 0; }  
  //! Reorder TofTracks by number of hits and charge 
  static void      ReorderTofTracks();
  //! Number of hits comparison between two tracks 
  static bool      CompareTofTracksNHits(TofTrack* a, TofTrack* b) { return (a->GetNhits()>b->GetNhits()); }
  //! Charge comparison between two tracks 
  static bool      CompareTofTracksCharge(TofTrack* a, TofTrack* b) { return (a->GetQ()>b->GetQ()); }
  //! Good TofRawCluster for reconstruction/analysis purposes: 
  /*!
   * - no 1-side measurement, no bad history, no bad as marked by DB
   * - each single paddle has to be part of the tigger (LT and HT consistency)
   * - time consistency check (lenght of the paddle)
   */
  static bool      GoodTofRawCluster(TofRawClusterR* tofcluster, Level1R* lvl1 = 0, char* add_name = "default");
  //! Good TofCluster for reconstruction/analysis purposes (all the raw cluster belonging to TOF cluster should be good for GoodTofRawCluster)
  static bool      GoodTofCluster(TofClusterR* tofcluster, Level1R* lvl1 = 0, char* add_name = "default");
  //! Good TofCandidate for reconstruction/analysis purposes:
  /*!
   * No cuts if it is made by 1 cluster. If it is made by two clusters: 
   * - inside a minimum distance (parameters in datacard) 
   * - a good charge compatibility (parameters in datacard)
   */
  static bool      GoodTofCandidate(TofCandidate* candidate, char* add_name = "default");
  //! Distance comparison between two TOF candidates
  static bool      CompareTofCandidateDistance(TofCandidate* a, TofCandidate* b) { return (a->GetDistance()<b->GetDistance()); }
  //! Good combination between two candidates (charge compatibility cut)
  static bool      GoodTofCandidateCombination(TofCandidate* candidate1, TofCandidate* candidate2, char* add_name = "default");
  //! Loose selection for the last step of reconstruction (parameters in datacard)
  /*!
   *  - no error in the computation of TOF track standalone quantities
   *  - loose cut on timing chisq2 
   *  - loose cut on spatial chisq2
   *  - loose cut on charge reconstruction relative error
   */
  static bool      GoodTofTrackLooseSelection(TofTrack* tof_track, char* add_name = "default");
  /**@}*/


  /** @name Association of TofTracks to the TrTracks list 
    * 
    * Matching and other methods. All this stuff has the caveat of X-side multiplicity, in order to be 
    * usable in the process of the overall reconstruction. 
    */
  /**@{*/
  //! Builds everything (build tof tracks, associate to tracker tracks, return the first tof track associated to a tracker track)
  static TofTrack* BuildAll(AMSEventR* event, int id = 0);
  //! Make the list of best associated Tracker tracks with TOF tracks
  static bool  AssociateTrTracksToTofTracks(AMSEventR* event, int id = 0);
  //! Calculate the position and angular difference between TOF standalone track and Tracker track at given height 
  static bool  CalculateSpatialDifferenceAtZ(TofTrack* tof_track, TrTrackR* trk_track, float z_match, float& dx, float& dy, float& costheta, int id = 0);
  //! Check if there is a good spatial match along Y (only Y position at first Tracker layer) TO-BE-IMPROVED 
  static bool  IsAGoodSpatialMatchAlongY(TofTrack* tof_track, TrTrackR* trk_track, int id = 0);
  //! Calculate the raw charge (without beta correction) difference between TOF track and Inner Tracker track  
  static bool  CalculateChargeDifference(TofTrack* tof_track, TrTrackR* trk_track, float& dQ, int id = 0);
  //! Check if there is a good charge match (using floating point estimators) 
  static bool  IsAGoodChargeMatch(TofTrack* tof_track, TrTrackR* trk_track, int id = 0);
  //! Calculate the cluster Y position chi-squared with respect to tracker track
  static float CalculateRedChiSqY(TofTrack* tof_track, TrTrackR* trk_track, int id = 0);
  //! Try to reconstruct a single TofTrack starting from a Tracker track (attention: delete is left to user)
  static TofTrack* BuildATofTrackFromTrTrack(AMSEventR* event, TrTrackR* trk_track);
  /**@}*/


  /** @name TofTrack Quality Test 
    * 
    * Methods for the TofTrack quality test. Mainly thought for analysis purposes.
    */
  /**@{*/
  //! 
  static bool      GoodTofTrack(TofTrack* tof_track, Level1R* lvl1, char* add_name = "default"); 
  //! Good TofClusters associated to TOF track (all clusters belonging to TofTrack should be good for GoodTofCluster)
  static bool      GoodTofTrackClusters(TofTrack* tof_track, Level1R* lvl1 = 0, char* add_name = "default");
  //! 
  static int       DropBadTofTrackClusters(TofTrack* tof_track, Level1R* lvl1 = 0, char* add_name = "default"); 
  //! Check if the TrTrack passes through all the TOF paddles (using the pathlength calculator of \c TofGeometry class) 
  static bool      IsInsidePaddles(TofTrack* tof_track, TrTrackR* trk_track, int id = 0);
  //!
  static int       DropOutsideTofTrackClusters(TofTrack* tof_track, TrTrackR* trk_track, int id = 0);
  //! Good match between TrTrack and TofTrack (based on cluster chisq wrt to the TrTrack interpolation)
  static bool      GoodTofTrackTrTrackMatch(TofTrack* tof_track, TrTrackR* trk_track, int id = 0, char* add_name = "default");

  //! Ask for a no FT before the last one (the one promoted to LVL1), inside a fiducial time window (from 1.04 mus up to 9 mus)    
  static bool      NoPreviousFT(AMSEventR* event, float time = 1.5 /*mus*/);

  //! Apply the build rules to a TOF track and determine if it is following the recon. rules (==good) 
  static bool      GoodTofTrackWithBuildRules(TofTrack* tof_track, Level1R* lvl1);
  //! Get the TOF track with the highest charge 
  static TofTrack* GetHighestChargeTofTrack(int nminclusters = 4);
  /**@}*/


  /*
  static bool  IsInsidePaddlesYZ(TofTrack* tof_track, TrTrackR* trk_track, int id = 0);
  static TrTrackR* GetClosestTrTrack(TofTrack* tof_track, AMSEventR* event);
  static bool GoodMatchTofTrackAndTrTrack(TofTrack* tof_track, TrTrackR* trk_track);
  static void GetClosestTrTracks(AMSEventR* event, int fittype);
  */


  /** @name Recalculating Clusters Variables
    */
  /**@{*/
  //  Times are recalculated trough TofTimeCalibration 
  //  Recalculate position after some alignment?
  //! Recalculate TOF energy depositions according to my calibration 
  //  static void RecalculateTofClusterEdep(AMSEventR* event) {}
  /**@}*/


  /** @name Comparison with GBATCH BetaR class
    */
  /**@{*/
  //! Get the GBATCH Beta included in the TOF track 
  static BetaR* GetIncludedBeta(TofTrack* tof_track, AMSEventR* event);
  //! Fill comparison plots of a TOF track with a Beta 
  static void   FillComparisonBetweenBetaAndTofTrack(TofTrack* tof_track, BetaR* beta);
  //! Fill comparison plots of a TOF track with the included Beta 
  static void   FillComparisonBetweenBetaAndTofTrack(TofTrack* tof_track, AMSEventR* event) {
    FillComparisonBetweenBetaAndTofTrack(tof_track,GetIncludedBeta(tof_track,event));
  }
  //! Fill comparison plots of TOF track statistics with GBATCH Beta statistics 
  static void   FillComparisonBetweenBetaAndTofTrackStatistics(AMSEventR* event);
  //! Fill residual plots
  static void   FillTofTrackResiduals(TofTrack* tof_track, char* add_name = "default");
  /**@}*/


  /** @name Association with other subdetectors
    */
  /**@{*/
  //! Find the closest ECAL shower
  static EcalShowerR* GetClosestEcalShower(TofTrack* tof_track, AMSEventR* event);
  //! Find the closest TRD track
  static TrdTrackR* GetClosestTrdTrack(TofTrack* tof_track, AMSEventR* event);
  //! Find the closest RICH track
  static RichRingR* GetClosestRichRing(TofTrack* tof_track, AMSEventR* event);
  /**@}*/


  /** @name Ancillary methods 
    * 
    * These methods are able to enable/disable the histogram and ntuple
    * automatically generated by the TofRecon methods.
    */
  /**@{*/ 
  //! Print on screen
  static void Print(int verbosity=0);
  //! Dump TofClusterR container
  static void DumpTofClustersContainer(AMSEventR* event);
  //! Dump TofRawClusterR container
  static void DumpTofRawClustersContainer(AMSEventR* event);
  //! Enable the histogram filling
  static void EnableHistograms();
  //! Disable the histogram filling
  static void DisableHistograms();
  //! Create/fill 2D histogram (maybe should be propagated to HistoMan)
  /*!
   *  This function will create an histogram on the TofRecon histogram manager
   *  and fills it with the passed values \c x and \c y.
   */ 
  static void Histogram(char* name, char* title, int nxbin, float xmin, float xmax, int nybin, float ymin, float ymax, float x, float y);
  //! Create/fill 1D histogram (maybe should be propagated to HistoMan)
  static void Histogram(char* name, char* title, int nxbin, float xmin, float xmax, float x);
  //! Enable the ntupple filling
  static void EnableNtuple()  { EnabledNtuple = true; }
  //! Disable the ntple filling
  static void DisableNtuple() { EnabledNtuple = false; }
  //! Initialize the ntuple
  static void InitNtuple();
  //! Histogram of properties of reconstructed TOF tracks  
  static void FillTofTrackHistograms(TofTrack* tof_track, char* add_name  = "default");
  //! Tree of properties of reconstructed TOF tracks
  static void FillTofTrackNtuple(TofTrack* tof_track);
  //! Write plots and/or ntuple on a file
  static void Write(TFile* file);
  /**@}*/


  //! ROOT
  ClassDef(TofRecon,1);
};


#endif
