// $Id: TrRecon.h,v 1.1 2008/12/18 11:19:24 pzuccon Exp $ 
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
///
/// $Date: 2008/12/18 11:19:24 $
///
/// $Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////
#include "typedefs.h"
#include "point.h"
#include "cont.h"
#include "TrRawCluster.h"
#include "TrCluster.h"
#include "TrTrack.h"
#include "TrRecHit.h"
#include "TkDBc.h"
#include "TrFit.h"
#include "TkDBc.h"
#include "VCon.h"
#include "TMath.h"

#include <cmath>
#include <vector>
#include <map>
#include <algorithm>



class TrRecon {

 public:  
 /// Number of maximum layers
  enum { MAXLAY = 8 };
  /// Status bits
  enum { AMBIG = 4, USED = 32 };

 protected: 
  /// for the Track finding
  AMSTrTrack* _ptest;
  AMSTrTrack* _ptrack;
  double  _Par[2][4];

  /// Minimum chisquare in the scan
  static double _csqmin;

  /// Number of hit patterns
  static int NHitPatterns;
  /// Hit pattern masked hit bits
  static int *HitPatternMask;
  /// Hit pattern attribute ( = N1*10000 +N2*1000 +N3*100 +N4*10 +N5)
  static int *HitPatternAttrib;

  /// Map association between TkId and 2 p/n side AMSTrClusters lists 
  map< int, pair< vector<AMSTrCluster*>, vector<AMSTrCluster*> > > _ClusterTkIdMap;
  typedef map< int, pair< vector<AMSTrCluster*>, vector<AMSTrCluster*> > >::iterator ITClusterMap;
  /// TrRecHits list for a given layer
  map< int, vector<AMSTrRecHit*> > _RecHitLayerMap;
   
  /// --- Parameters --- ///
public:
  /// Clustering - Seed S/N threshold 
  static float ThrSeed[2];
  /// Clustering - Neighboring S/N strips threshold 
  static float ThrNeig[2];
  /// Clustering - Minimum distance between two seeds
  static int   SeedDist[2];

  /// Correlation - Double gaussian distribution parameters (for muons/protons)
  static float GGpars[6];
  /// Correlation - Double gaussian distribution normalization (for muons/protons)
  static float GGintegral;
  /// Correlation - Probability of correlation threshold
  static float ThrProb;


 protected:

  /// --- Analysis Structures --- ///
  /// clustering - size of ADC/signal/status temporary buffers
  enum { BUFSIZE = 1024 };
  /// clustering - ADC buffer to expand raw clusters
  static float _adcbuf[BUFSIZE];
  static float _sigbuf[BUFSIZE];
  /// clustering - status buffer to expand raw clusters
  static int   _stabuf[BUFSIZE];
  /// clustering - list of the seeds in the subbuffer
  static vector<int> _seedaddresses;   

  static float _adcbuf2[BUFSIZE];
  static float _sigbuf2[BUFSIZE];
  static int   _stabuf2[BUFSIZE];
private:
  //! Private constructor, class is a singleton
  TrRecon(){Clear();  cal=0;}
  //! do not want copy constructor
  TrRecon(TrRecon& orig){}

  static TrCalDB* _trcaldb;
  TrLadCal* cal;
public:
  //! static pointer to the single instance
  static TrRecon*  Head;
  //! reurnrs the static pointer to the single instance
  static TrRecon*  gethead() {return Head;}
  //! create the single instance
  static TrRecon*  Create(int recreate=0);
  //! std destructor
  ~TrRecon();
  /// Clear data members
  void   Clear(Option_t *option = "");
  /// It sets the Reconstruction Paramenters from the data card
  void SetParFromDataCards();



 /// Using this calibration file
  static void UsingTrCalDB(TrCalDB* trcaldb) { _trcaldb = trcaldb; }
  /// Get the current calibration database
  TrCalDB*    GetTrCalDB() { return _trcaldb; }




  
  // --- Clustering Methods --- // 
  /// Set the seed S/N threshold
  void   SetThrSeed(int side, float thr) { ThrSeed[side] = thr; }
  /// Set the neigtboring strips S/N threshold
  void   SetThrNeig(int side, float thr) { ThrNeig[side] = thr; }
  /// Set the mimnimum strip distance between two seeds
  void   SetSeedDist(int side, int dist) { SeedDist[side] = dist; }

  /// Clear buffer
  void ClearBuffer();
  /// Expand the TrRawCluster in the buffer structure
  int  ExpandClusterInBuffer(AMSTrRawCluster* cluster);


  /// Builds all the TrClusters (returns the number of TrCluster built) 
  int  BuildTrClusters(int rebuild=0);


  /// Builds the TrClusters in a buffer interval (cyclicity if !=0) 
  int  BuildTrClustersInSubBuffer(int tkid, int first, int last, int cyclicity = 0);
  /// Builds the seed list in the buffer interval (cyclicity if !=0) 
  int  BuildSeedListInSubBuffer(int first, int last, int cyclicity = 0);
  /// Search for the addresses of the boundaries (cyclicity if !=0), returns 10000*leftaddress+rightaddress 
  int  GetBoundariesInSubBuffer(int index, int first, int last, int cyclicity = 0);
  /// Get a well defined address inside an interval (imposing cyclicity if !=0) 
  int  GetAddressInSubBuffer(int address, int first, int last, int ciclicity = 0);


 // --- Hit Signal Correlation (only muons for the moment) --- //
  // --- No reordering implemented --- // 

  /// Set the correlation probability threshold
  void  SetThrProb(float prob) { ThrProb = prob; }
  /// Get correlation between 2 signal (p/n) (protons)
  float GetCorrelation(float n, float p) { return (p - n)/(p + n); }
  float GetCorrelation(TrClusterR* cln, TrClusterR* clp);
  /// Get probability of correlation between 2 signal (with all corrections p/n) (protons)
  float GetProbToBeCorrelated(float n, float p);
  float GetProbToBeCorrelated(TrClusterR* cln, TrClusterR* clp);
 
  /// Builds the Cluster TkId map (for the binary fast search of clusters)
  void  BuildClusterTkIdMap();
  /// Get the number of active (with cluster) ladders by side
  int   GetnActiveLadders() { return _ClusterTkIdMap.size(); } 
 
  /// Get the number of clusters in the tkid/side ladder
  int   GetnTrClusters(int tkid, int side);
  /// Get the cluster in the tkid/side by index
  TrClusterR* GetTrCluster(int tkid, int side, int iclus);
  /// Builds all the TrRecHits (combinations and spares) (returns the number of TrRecHits built)
  int   BuildTrRecHits(int rebuild=0);


  /// Initialize hit patterns, recursively called
  void BuildHitPatterns(int n = -1, int i = 0, int mask = 0);

  /// Get HitPatternMask
  int GetHitPatternMask(int i) {
    return (HitPatternMask && 0 <= i && i < NHitPatterns) ? HitPatternMask[i] : 0;
  }
  /// Get HitPatternAttrib
  int GetHitPatternAttrib(int i) {
    return (HitPatternAttrib && 0 <= i && i < NHitPatterns) ? HitPatternAttrib[i] : 0;
  }

  /// Reconstruct tracks, returns number of tracks reconstructed
  int BuildTrTracks(int refit = 0);

  /// Builds up the map RecHit Layer
  void BuildRecHitLayerMap();

  /// Scan hits, recursively called, returns 1 if a track found
  void ScanHit(int *layer, int *iscan = 0, int *imult = 0, int i = 0);


  static VCon* TRCon;

private:

 static void DSP_Clusterize(int tkid,float *buf);

public: 
  //! Generate the TrMCCluster from the geant info
  static void sitkhits(int idsoft, float vect[],
		      float edep, float step, int itra);
  //! Generate Non Gaussians Noise Cluster (dummy for the moment)
  static void sitknoise(){}

  //! Generate the TrRawClusters from the MC info and from a calibration
  static void sitkdigi(); 
};

#endif


