// $Id: TrRecon.h,v 1.44 2011/04/16 09:11:04 shaino Exp $ 
#ifndef __TrRecon__
#define __TrRecon__

//////////////////////////////////////////////////////////////////////////
///
///
///\class TrRecon
///\brief Procedures for the Tracker Reconstruction 
///
///\date  2008/02/16 AO  First version
///\date  2008/02/21 AO  BuildTrClusters(), BuildTrRecHits()
///\date  2008/02/24 AO  BuildPatterns(), some bug fixed
///\date  2008/02/26 AO  BuildTrTracks()
///\date  2008/02/28 AO  Bug fixing
///\date  2008/03/11 AO  Some change in clustering methods 
///\date  2008/06/19 AO  Updating TrCluster building 
///\date  2008/07/01 PZ  Global review and various improvements 
///\date  2009/12/17 SH  TAS reconstruction added
///
/// $Date: 2011/04/16 09:11:04 $
///
/// $Revision: 1.44 $
///
//////////////////////////////////////////////////////////////////////////
#include "typedefs.h"
#include "point.h"

#include "TrRawCluster.h"
#include "TrCluster.h"
#include "TrTrack.h"
#include "TrRecHit.h"
#include "TrFit.h"
#include "TkSens.h"
#include "TMath.h"
#include "TrTasDB.h"
#include "Vertex.h"
#include "TrSim.h"
#include "amsdbc.h"
#include <cmath>
#include <vector>
#include <map>
#include <algorithm>
#include "tkpatt.h"
#include "TObject.h"
#define _SCANLAY 8




class TrReconPar : public TObject {

public:
  // --- Parameters for reclusterization ---

  /// Clustering - Seed S/N threshold for 0:n(X) 1:p(Y) side
  float ThrSeed[2][9];
  /// Clustering - Neighboring S/N strips threshold for 0:n(X) 1:p(Y) side
  float ThrNeig[2];
  /// Clustering - Minimum distance between two seeds for 0:n(X) 1:p(Y) side
  int   SeedDist[2];

  /// Correlation - Double gaussian distribution parameters (for muons/protons)
  float GGpars[6];
  /// Correlation - Double gaussian distribution normalization (for muons/protons)
  float GGintegral;
  /// Correlation - Probability of correlation threshold
  float ThrProb;


  // --- Parameters for the full reconstruction ---

  /// Maximum number of raw clusters allowed for the next step
  int MaxNrawCls;
  /// lowdt Threeshold (usec)
  int lowdt;
  /// Maximum number of clusters allowed for the next step
  int MaxNtrCls_ldt;
  /// Maximum number of clusters allowed for the next step
  int MaxNtrCls;
  /// Maximum number of hits allowed for the next step
  int MaxNtrHit;
  /// Maximum CPU time (s)  for tracking
  float TrTimeLim;


  // --- Performance tuning parameters for track reconstruction ---

  /// Maximum number of tracks allowed to search for
  int MaxNtrack;
  /// Minimum number of hits with X- and Y- clusters required for a track
  int MinNhitXY;
  /// Minimum number of X(p)-side-clusters required for a track
  int MinNhitX;
  /// Minimum number of Y(n)-side-clusters required for a track
  int MinNhitY;
  /// PatAllowOption bits
  enum { NO_EXTP = 1, NO_INTP = 2, ALL_5L = 4};
  /// Exception to allow pattern
  int PatAllowOption;
  /// Range in cm to search for Ladders used in ScanLadders
  double LadderScanRange;
  /// Range in cm to search for clusters used in ScanClusters
  double ClusterScanRange;
  /// Maximum chisquare allowed for Ladder scan
  double MaxChisqForLScan;
  /// Maximum chisquare allowed for Cluster scan
  double MaxChisqAllowed;
  /// Fitting error in X used for fast fitting in the scan
  double ErrXForScan;
  /// Fitting error in Y used for fast fitting in the scan
  double ErrYForScan;
  /// Seed SN threshold in track finding for 0:n(X) 1:p(Y) side
  float TrackThrSeed[2];
  /// N-sigma threshold to merge low seed SN clusters
  float NsigmaMerge;

  /// Counter for BuildTrTracks
  int NbuildTrack;
  /// Counter for MaxNrawCls cut
  int NcutRaw;
  /// Counter for MaxNtrCls_ldt cut
  int NcutLdt;
  /// Counter for MaxNtrCls cut
  int NcutCls;
  /// Counter for MaxNtrHit cut
  int NcutHit;
  /// Counter for Cpulimit
  int NcutCpu;

  TrReconPar();
  void SetParFromDataCards();

  ClassDef(TrReconPar, 1);
};

class TrRecon {

public:
  static TrReconPar RecPar;

protected:
  /// Map association between TkId and 2 p/n side TrClusters lists 
  map< int, pair< vector<TrClusterR*>, vector<TrClusterR*> > > _ClusterTkIdMap;
  typedef map< int, pair< vector<TrClusterR*>, vector<TrClusterR*> > >::iterator ITClusterMap;
  /// TrRecHits list for a given layer
  map< int, vector<TrRecHitR*> > _RecHitLayerMap;
   
  /// --- Analysis Structures --- ///
  /// clustering - size of ADC/signal/status temporary buffers
  enum { BUFSIZE = 1024 };
  /// clustering - ADC buffer to expand raw clusters
  float _adcbuf[BUFSIZE];
  
  float _sigbuf[BUFSIZE];
  
  /// clustering - status buffer to expand raw clusters
  int   _stabuf[BUFSIZE];

  /// clustering - list of the seeds in the subbuffer
  vector<int> _seedaddresses;   
  
  static float _adcbuf2[BUFSIZE];
  
  static float _sigbuf2[BUFSIZE];
  
  static int   _stabuf2[BUFSIZE];
  
  

private:
  
  //! do not want copy constructor
  TrRecon(TrRecon&){}

  static TrCalDB* _trcaldb;

public:
  //!  constructor
  TrRecon(){Clear();  }
  static void Init();
//   //! static pointer to the single instance
//   static TrRecon*  Head;
//   //! reurnrs the static pointer to the single instance
//   static TrRecon*  gethead() {return Head;}
//   //! create the single instance
//   static TrRecon*  Create(int recreate=0);
  //! std destructor
  ~TrRecon();
  /// Clear data members
  void   Clear(Option_t *option = "");
  /// It sets the Reconstruction Paramenters from the data card
  static void SetParFromDataCards();



 /// Using this calibration file
  static void UsingTrCalDB(TrCalDB* trcaldb) { _trcaldb = trcaldb; }
  /// Get the current calibration database
  static TrCalDB*    GetTrCalDB() { return _trcaldb; }


  /*!
    \brief Start full track reconstruction (TrCluster, TrRecHit and TrTrack)

    \param flag reconstruction flag (as defined in TRCLFFKEY.recflag)
    \li    0  No recon
    \li    1  TrCluster
    \li   11  TrCluster and TrRecHit
    \li  111  TrCluster, TrRecHit and TrTrack
    \li 1111  TrCluster, TrRecHit and TrTrack with BuildTrTracksSimple

    \param rebuild existing Tr** objects are cleared

    \param hist Histograms are filled

    \return status of the reconstruction, to be passed to EventId::trstat 
  */
  int Build(int flag = 111, int rebuild = 0, int hist = 1);
  
  // --- Clustering Methods --- // 
  /// Set the seed S/N threshold
  static void   SetThrSeed(int side,int layer, float thr) { RecPar.ThrSeed[side][layer-1] = thr; }
  /// Set the neigtboring strips S/N threshold
  static void   SetThrNeig(int side, float thr) { RecPar.ThrNeig[side] = thr; }
  /// Set the mimnimum strip distance between two seeds
  static void   SetSeedDist(int side, int dist) { RecPar.SeedDist[side] = dist; }

  static void InitBuffer();
  /// Clear buffer
  void ClearBuffer();
  /// Expand the TrRawCluster in the buffer structure
  int  ExpandClusterInBuffer(TrRawClusterR* cluster);


  /// Builds all the TrClusters (returns the number of TrCluster built) 
  int  BuildTrClusters(int rebuild=0);


  /// Builds the TrClusters in a buffer interval (cyclicity if !=0) 
  int  BuildTrClustersInSubBuffer(int tkid, int first, int last, int cyclicity = 0);
  /// Builds the seed list in the buffer interval (cyclicity if !=0) 
  int  BuildSeedListInSubBuffer(int tkid, int first, int last, int cyclicity = 0);
  /// Search for the addresses of the boundaries (cyclicity if !=0), returns 10000*leftaddress+rightaddress 
  int  GetBoundariesInSubBuffer(int index, int first, int last, int cyclicity = 0);
  /// Get a well defined address inside an interval (imposing cyclicity if !=0) 
  int  GetAddressInSubBuffer(int address, int first, int last, int ciclicity = 0);


 // --- Hit Signal Correlation (only muons for the moment) --- //
  // --- No reordering implemented --- // 

  /// Set the correlation probability threshold
  void  SetThrProb(float prob) { RecPar.ThrProb = prob; }
 
  /// Builds the Cluster TkId map (for the binary fast search of clusters)
  void  BuildClusterTkIdMap();
  /// Get the number of active (with cluster) ladders by side
  int   GetnActiveLadders() { return _ClusterTkIdMap.size(); } 
 
  /// Get the number of clusters in the tkid/side ladder
  int   GetnTrClusters(int tkid, int side);
  /// Get the cluster in the tkid/side by index
  TrClusterR* GetTrCluster(int tkid, int side, int iclus);

public:
  /// Builds all the TrRecHits (combinations and spares) (returns the number of TrRecHits built)
  int  BuildTrRecHits(int rebuild = 0);

  /// Fild a hit with the combination of xcls and ycls
  TrRecHitR* FindHit(TrClusterR* xcls,TrClusterR* ycls);

////////////////////////////////////////////////////////////////
// -------------------- TAS reconstruction ------------------ //
////////////////////////////////////////////////////////////////
  /// Activate switch for the TAS reconstruction
  static int TasRecon;

  /// Current TrTasPar
  TrTasPar *_TasPar;

  /// Build TrClusters specialized for TAS data
  int BuildTrTasClusters(int rebuild = 0);

  /// Build TrRecHits specialized for TAS data
  int BuildTrTasHits(int rebuild = 0);

  /// Build TrTracks specialized for TAS data
  int BuildTrTasTracks(int rebuild = 0);

////////////////////////////////////////////////////////////////
// ------------------------- Vertex ------------------------- //
////////////////////////////////////////////////////////////////

  int BuildVertex(int refit=0);

////////////////////////////////////////////////////////////////
// ------------------------- TRACKS ------------------------- //
////////////////////////////////////////////////////////////////

//========================================================
// Performance tuning parameters for track reconstruction
//========================================================
public:  


  /// Status bits (temporary)
  enum { AMBIG = 0x04, USED = 0x20 };


//========================================================
// Temporary buffers for fast hit scanning
//========================================================
protected:
  /// Array of vectors of TkId at each layer
  vector<int> _LadderHitMap[_SCANLAY];
  /// Number of ladders with both p(X) and n(Y) clusters at each layer
  int _NladderXY[_SCANLAY];

  /// Virtual 2D array to store TrRecHits at [iclx][icly]
  class Hits2DArray : public vector<TrRecHitR*> {
  public:
    int Ncls[2];        ///< Number of X and Y clusters
    /// Get index of a hit at [iclx][icly]
    int index(int iclx, int icly) const { 
      return (0 <= iclx && iclx < Ncls[0] && 
              0 <= icly && icly < Ncls[1]) ? icly*Ncls[0]+iclx : -1; 
    }
    /// Get a hit at [iclx][icly]
    TrRecHitR *at(int iclx, int icly) const { 
      int i = index(iclx, icly);
      return (0 <= i && i < size()) ? vector<TrRecHitR*>::at(i) : 0;
    }
  };

  /// Map of TrRecHits with a key as TkId
  typedef map<int, Hits2DArray> HitsTkIdMap;
  /// Iterator of HitsTkIdMap
  typedef HitsTkIdMap::iterator ITHitsTkIdMap;

  /// Map of virtual 2D array to store TrRecHits at [tkid][iclx][icly]
  HitsTkIdMap _HitsTkIdMap;

  /// Get Hits2DArray with tkid
  const Hits2DArray *GetHits2DArray(int tkid) const {
    HitsTkIdMap::const_iterator it = _HitsTkIdMap.find(tkid);
    return (it == _HitsTkIdMap.end()) ? 0 : &it->second;
  }

  /// Get Hits2DArray with tkid (non-const.)
  Hits2DArray *GetHits2DArray(int tkid) {
    HitsTkIdMap::iterator it = _HitsTkIdMap.find(tkid);
    return (it == _HitsTkIdMap.end()) ? 0 : &it->second;
  }

  /// A predicate to sort hits in descending order of prob.
  class CompProb {
  public:
    bool operator() ( TrRecHitR* lhit,  TrRecHitR* rhit) const {
      return lhit->GetProb() > rhit->GetProb();
    }
  };

  /// Minimum number of hits with X- and Y- clusters required for a track
  int _MinNhitXY;
  /// Minimum number of X(p)-side-clusters required for a track
  int _MinNhitX;
  /// Minimum number of Y(n)-side-clusters required for a track
  int _MinNhitY;

  //
  /// For debug - number of trials in ScanLadders
  int Ntrials1;
  /// For debug - number of trials in ScanClusters
  int Ntrials2;
  /// Debug switch
  static int TrDEBUG;
  static int PZDEBUG;

  // Tools for CPU time consumption
  // Note: it works only without __ROOTSHAREDLIBRARY__
  //
  /// Start time marker
  float _StartTime;

  /// CPU time up flag
  bool _CpuTimeUp;

  /// CPU time consumption in the last BuildTrTracks
  float _CpuTime;

  /// Total CPU time consumption
  static float _CpuTimeTotal;

  /// Set _StartTime marker
  void  _StartTimer();

  /// Get a time difference between now and _StartTime
  float _CheckTimer() const;

public:
  /// Flag for SIGTERM/SIGINT;
  static bool SigTERM;
  
  /// Get CPU time up flag
  bool CpuTimeUp() const { return _CpuTimeUp | SigTERM; }

  /// Get CPU time consumption in the last BuildTrTracks
  float GetCpuTime() const { return _CpuTime; }

  /// Builds up the map RecHit Layer
  void BuildHitsTkIdMap();

  /// Builds up LadderHitMap of TrRecHits, used for standard reconstruction
  void BuildLadderHitMap();

  /// Builds up LadderHitMap of TrClusters, used for TDisplay
  void BuildLadderClusterMap();

  /// Get number of ladders with cluster signals at the layer
  int GetNladder(int layer) const { 
    return (0 < layer && layer <= patt->GetSCANLAY()) ? _LadderHitMap[layer-1].size() : 0;
  }
  /// Get tkid from _LadderHitMap
  int GetLadderHit(int layer, int i) const {
    return (0 <= i && i < GetNladder(layer)) ? _LadderHitMap[layer-1][i] : 0;
  }

  /// Get number of hits for all X-Y combinations with tkid
  int GetnTrRecHits(int tkid) const {
    const Hits2DArray *hits = GetHits2DArray(tkid);
    return (hits) ? hits->size() : 0;
  }
  /// Get number of projection hits with tkid and side
  int GetnTrRecHits(int tkid, int side) const {
    const Hits2DArray *hits = GetHits2DArray(tkid);
    return (hits && (side == 0 || side == 1)) ? hits->Ncls[side] : 0;
  }
  /// Get TrRecHit with tkid, iclx and icly
  TrRecHitR *GetTrRecHit(int tkid, int iclx, int icly) const {
    const Hits2DArray *hits = GetHits2DArray(tkid);
    return (hits) ? hits->at(iclx, icly) : 0;
  }
  /// Remove a row of hits from _HitsTkIdMap;
  void RemoveHits(int tkid, int icls, int side);
  /// Remove a row of hits from _HitsTkIdMap;
  void RemoveHits(TrRecHitR *hit);

//========================================================
// Structures
//========================================================
public:
  /// Structure of hit iterators
  typedef struct TrHitIterStruct {
    int    mode;               ///< Scan mode (1: Ladder 2: Hit)
    int    pattern;            ///< Hit pattern of current scan
    int    side;               ///< Side of current scan
    int    nlayer;             ///< Number of layers to be scanned
    int    ilay [_SCANLAY];     ///< Scanning order for effective pre-selection
    int    tkid [_SCANLAY];     ///< TkId list
    int    iscan[_SCANLAY][2];  ///< Current candidate hit index
    int    imult[_SCANLAY];     ///< Current candidate multiplicity index
    AMSPoint coo[_SCANLAY];     ///< Current candidate 3D-coordinate
    double psrange;            ///< Pre-selection range
    double param[4];           ///< Pre-selection parameter
    double chisq[2];           ///< Chisquare in X and Y
    int    nhitc;              ///< Number of hits with X and Y cls.
    int &Iscan(int i) { return iscan[i][side]; }
  } TrHitIter;

  /// The best candidate found so far in ScanLadders
  TrHitIter _itcand;

  /// The best candidate found so far in ScanHits
  TrHitIter _itchit;

//========================================================
// Track reconstruction methods
//========================================================
public:
  /// Reconstruct tracks, returns number of tracks reconstructed
  int BuildTrTracks(int rebuild = 0);

  /// Reconstruct tracks, simple version
  int BuildTrTracksSimple(int rebuild = 0);
 
  /// Merge hits shared by two tracks
//int MergeSharedHits(TrTrackR *track, int fit_method);

  /// Merge hits with seed SN clusters lower than TrackThrSeed
  int MergeLowSNHits(TrTrackR *track, int fit_method);

  /// Merge hits on the external layers
  int MergeExtHits(TrTrackR *track, int fit_method);

  /// Build one track from the best candidate iterator
  int BuildATrTrack(TrHitIter &cand);

  /// Process track
  int ProcessTrack(TrTrackR *track, int merge_low = 1);

  /// Try to drop one hit in case ChisqX is very large
  int TryDropX(TrTrackR *track, int fit_method);

  /// Purge "ghost" hits and assign hit index to tracks
  void PurgeGhostHits();

  /// Purge unused hits and assign hit index to tracks
  void PurgeUnusedHits();

  /// Number of track counters
  enum { NTrackCounter = 10 };

  /// Track counters 0:total 1:anyT 2:allP 3:hL1N 4:hL9 5:full
  static int fTrackCounter[NTrackCounter];

  /// Reset track counters
  void ResetTrackCounter() {
    for (int i = 0; i < NTrackCounter; i++) fTrackCounter[i] = 0;
  }

  /// Check track quality and count events
  int CountTracks(int trstat = 0);

  /// Fill track histograms
  int FillHistos(int trstat = 0, int refit = 0);

  /// Print statics
  static void PrintStats();

  /// Get current event number
  static int GetEventID();

  /* [0]-Tcalib.counter
     [1]-Treset.counter
     [2]-[3]-0.64mks Tcounter(32lsb+8msb)
     [4]-time_diff in mksec                    
  */
  /// Access to Trigger2LVL1/Level1R and get TrigTime[i], where 0<= i <5
  static float GetTrigTime(int i);

  /// Access to Trigger2LVL1/Level1R and get i=0:TofFlag1 or i=1:TofFlag2
  static int GetTofFlag(int i = 0);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
  /// Try to extend to external planes for AMS-02P
  void MatchTRDandExtend();


 /// Check the Match between Tracker and TRD tracks
  /// Returns an AMSPoint with
  /// [0] X distance
  /// [1] Y distance
  /// [2] Cos(angle)
  static AMSPoint BasicTkTRDMatch(TrTrackR* ptrack, 
				  AMSPoint& trdcoo, AMSDir& trddir, 
				  int fit_id=0);

  /// Check the match Tracker and TRD tracks and shift TrTrack
  static bool TkTRDMatch(TrTrackR* ptrack, 
			 AMSPoint& trdcoo, AMSDir& trddir);


  /// Try to ajust the X multiplicity using a road from other detectors
  static bool MoveTrTrack(TrTrackR* ptr, AMSPoint& pp, AMSDir& dir, float err);

  
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

  /// General recursion function for Ladder/Hit scan
  /*!
   *\param[in]  idx    Layer index, it should be 0 for the first call
   *
   *\param[in]  it     Scan iterator. it.pattern, it.side, it.nlayer, 
   *                    it.tkid, and it.ilay must be filled before calling. 
   *                    it.tkid can be like (layer)*100. 
   *                    The other elements to be filled during the recursion.
   */
  int ScanRecursive(int idx, TrHitIter &it);

  /// Pre-selection (interpolation check) on the current scan
  bool PreScan(int nlay, TrHitIter &iter) const;
  /// Fill the order of scanning layers to make pre-selection most effectie
  int SetLayerOrder(TrHitIter &iter) const;

//   /// Scan ladders, returns 1 and _itcand is filled if candidate found
//   int ScanLadders(int pattern);
  /// Coord.manager for ladders scan to be put in ScanRecursive
  int LadderCoordMgr(int idx, TrHitIter &it, int mode) const;
  /// Evaluator for Ladder scan to be put in ScanRecursive
  int LadderScanEval(TrHitIter &it);

  /// Scan X and Y hits, returns 1 and _itcand is filled if candidate found
  int ScanHits(const TrHitIter &it);
  /// Coord.manager for hits scan to be put in ScanRecursive
  int HitCoordMgr(int idx, TrHitIter &it, int mode) const;
  /// Evaluator for hits scan to be put in ScanRecursive
  int HitScanEval(const TrHitIter &it);

  /// Estimate multiplicity and readout strip by interpolation
  TkSens EstimateXCoord(AMSPoint coo, int tkid = 0) const;
  /// Estimate multiplicity and readout strip by interpolation
  int EstimateXCoord(int il, TrHitIter &iter) const;

  /// Polynomial interpolation (pol2)
  static double Intpol2(double x1, double x2, double x3,
			double y1, double y2, double y3, double x) {
    return (x-x2)*(x-x3)/(x1-x2)/(x1-x3)*y1
          +(x-x1)*(x-x3)/(x2-x1)/(x2-x3)*y2
          +(x-x1)*(x-x2)/(x3-x1)/(x3-x2)*y3;
  }

//========================================================
// Utilities for debugging
//========================================================
public:
  /// FFKEY.init() called from ROOT CINT
  static void InitFFKEYs(int magstat);
  /// TrField::Read() called from ROOT CINT
  static int ReadMagField(const char *fname, 
			  float fscale = 1, int magstat = 1);
  /// TrField::GuFld() called from ROOT CINT
  static void GetMagField(float *pos, float *bf);
  /// TrField::TkFld() called from ROOT CINT
  static void GetTkFld(float *pos, float **hxy);
  /// Add magnetic field correction
//  static void MagFieldCorr(AMSPoint pp, AMSPoint bc);

  /// Get Tracker Data size in current event
  static int GetTrackerSize();
};


#endif

