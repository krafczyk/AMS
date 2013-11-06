#ifndef __TrCharge__
#define __TrCharge__

/*! 
 \file  TrCharge.h
 \brief Header file of TrCharge class

 \class TrChargeR
 \brief Full class for the Tracker charge reconstruction

 \class TrCharge
 \brief An almost static class for the Tracker charge reconstruction

 $Date: 2013/11/06 11:17:45 $

 $Revision: 1.13 $
*/

#include "VCon.h"
#include "HistoMan.h"
#include "tkdcards.h"

#include "TrCluster.h"
#include "TrRecHit.h"
#include "TrTrack.h"
#include "TrPdf.h"
#include "TrPdfDB.h"
#include "TrGainDB.h"
#include "TrChargeLossDB.h"
#include "TrLikeDB.h"

#include "TFile.h"
#include "TMath.h"

#include <cmath>
#include <vector>
#include <map>
#include <algorithm>


using namespace std;


//! Class used to retrieve results of a mean calculation
class mean_t {
 public: 
  //! Mean type
  int   Type;
  //! Signal options
  int   Opt; 
  //! Side 
  int   Side;
  //! Number of points 
  int   NPoints;
  //! Mean
  float Mean;
  //! RMS
  float RMS;
  //! Constructor
  mean_t() { clear(); }
  //! Constructor
  mean_t(int t, int o = 0, int s = 0, int n = 0, float m = 0, float r = 0) { 
    clear(); Type = t; Opt = o; Side = s; NPoints = n; Mean = m; RMS = r; 
  }
  //! Copy constructor
  mean_t(const mean_t &that) { copy(that); } 
  //! Assignment operator
  mean_t& operator=(const mean_t& that) { if (this!=&that) copy(that); return *this; }
  //! Destructor
  ~mean_t() { clear(); }
  //! Clear
  void clear() { 
    Type = 0; Opt = 0; Side = 0; NPoints = 0; Mean = 0; RMS = 0; 
  }
  //! Copy
  void copy(const mean_t &that) { 
    Type = that.Type; Opt = that.Opt; Side = that.Side; NPoints = that.NPoints; Mean = that.Mean; RMS = that.RMS;
  }
  //! Print
  void print() {
    printf("Type %4x  Opt %4x  Side %1d  NPoints %1d  Mean %7.2f  RMS %7.2f\n",
           Type,Opt,Side,NPoints,Mean,RMS);
  }
};


//! Class used to retrieve results of a likelihood calculation
class like_t {
 public:
  //! Loglikelihood computation type
  int    Type;
  //! Pdf version (also includes option type)
  int    Ver;
  //! Side 
  int    Side;
  //! Charge hypothesis (0: e, 1: p, 2: He, ...)
  int    Z;
  //! Number of points 
  int    NPoints;
  //! Likelihood
  float  LogLike; 
  //! Probability
  float  Prob;
  //! Mean
  float  Mean;
  //! Constructor
  like_t() { clear(); }
  //! Constructor
  like_t(int t, int v = 0, int s = 0, int z = -1, int n = 0, float ll = -30, float p = 0, float m = 0) { 
    clear(); Type = t; Ver = v; Side = s; Z = z; NPoints = n; LogLike = ll; Prob = p; Mean = m; 
  }
  //! Destructor
  ~like_t() { clear(); }
  //! Copy constructor
  like_t(const like_t &that) { copy(that); }
  //! Assignment operator
  like_t& operator=(const like_t &that) { if (this!=&that) copy(that); return *this; }
  //! Copy
  void copy(const like_t &that) {
    Type = that.Type; Ver = that.Ver; Side = that.Side; Z = that.Z; NPoints = that.NPoints; LogLike = that.LogLike; Prob = that.Prob; Mean = that.Mean;
  }
  //! Relational operator <
  bool operator<(const like_t& that) const { return GetNormLogLike()<that.GetNormLogLike(); }
  //! Relational operator >
  bool operator>(const like_t& that) const { return GetNormLogLike()>that.GetNormLogLike(); }
  //! Relational operator <=
  bool operator<=(const like_t& that) const { return GetNormLogLike()<=that.GetNormLogLike(); }
  //! Relational operator >=
  bool operator>=(const like_t& that) const { return GetNormLogLike()>=that.GetNormLogLike(); }
  //! Clear
  void clear() { 
    Type = 0; Ver = 0; Side = 0; Z = -1; NPoints = 0; LogLike = -30; Prob = 0; Mean = 0; 
  }
  //! Print
  void print() {
    printf("Type %4x  Ver %2d  Side %1d  Z %2d  NPoints %2d  LogLike %7.3f  Prob %7.5f  Mean %7.2f NormLogLike %7.3f\n",
           Type,Ver,Side,Z,NPoints,LogLike,Prob,Mean,GetNormLogLike());
  }
  //! Normalized LogLike
  float GetNormLogLike() const { return (NPoints>0) ? LogLike/NPoints : -30; }
};


//! Class for the Tracker charge reconstruction
class TrCharge {

 public:
 
  //! Pattern used to create list of cluster signals  
  int    PattType;
  //! Cluster signal container (used to speed-up likelihood)
  double ClsSig[2][9];
  //! Cluster category (used to speed-up likelihood)
  int    ClsCat[9];

 public:

  //! Enabling histograms
  static bool     EnabledHistograms;
  //! Histogram manager
  static HistoMan Histograms;

 public:

  /////////////////////////
  // Enumerators
  /////////////////////////

  //! TrRecHit side convention
  enum SideType {
    // only-x 
    kX = 0, 
    // only-y
    kY = 1, 
    // xy weighted mean
    kXY = 2, 
    // xy not weighted
    kXYUnWei = 3 
  };
  //! Pattern types
  enum PattType {
    // add inner tracker
    kInner     = 0x1,
    // add upper layer 
    kUpper     = 0x2,
    // add lower layer
    kLower     = 0x4,
    // all tracker
    kAll       = 0x1|0x2|0x4
  };
  //! Mean types
  enum MeanType {
    // plain mean
    kPlainMean = 0x10, 
    // truncated mean  
    kTruncMean = 0x20, 
    // gaussianized mean
    kGaussMean = 0x40,
    // apply sqrt before average 
    kSqrt      = 0x100
  };
  //! Available likelihood calculations
  enum LikeType { 
    // kTruncatedMean = 0x100, kAllPoints = 0x200, kDropOne = 0x400, kBestFour = 0x800 
  };

 public:

  /** @name Base methods. */
  // c-tor
  TrCharge() { Clear(); }     
  // d-tor
  ~TrCharge() { Clear(); }         
  // Clear computing members
  void Clear();

  /** @name Floating point charge estimator related methods. */
  /**@{*/
  //! Good hit for charge reconstruction
  static bool   GoodChargeReconHit(TrRecHitR* hit, int iside);
  //! Mean generic interface
  static mean_t GetMean(int type, vector<float> signal);
  //! Mean of n signals
  static mean_t GetPlainMean(vector<float> signal);
  //! Truncated mean of n signals
  static mean_t GetTruncMean(vector<float> signal);
  //! Gaussianized mean of n signals (discarding out-of-3-sigma signals)
  static mean_t GetGaussMean(vector<float> signal);
  //! Mean generic method
  static mean_t GetMean(int type, TrTrackR* track, int iside, float beta = 1, int jlayer = -1,
        int opt = TrClusterR::DefaultChargeCorrOpt, int fit_id = -1, float mass_on_Z = 0);
  //! Weighted mean of two sides  
  static mean_t GetCombinedMean(int type, TrTrackR* track, float beta = 1, int jlayer = -1, 
        int opt = TrClusterR::DefaultChargeCorrOpt, int fit_id = -1, float mass_on_Z = 0);
  /**@}*/


  /** @name Integer charge estimator related methods. */
  /**@{*/
  //! Good clusters for the probability calculation
  bool   GoodHitForLogProb(TrRecHitR* hit, int side);
  //! Store in memory relevant information of clusters that will be used in likelihood calculation
  bool   StoreClusterInfo(TrTrackR* track, int type, int iside = 2, double beta = 1, double rigidity = 0, double mass_on_Z = 2*0.93149);
  //! Print current cluster info
  void   PrintClusterInfo();
  //! Compute xy-combined mean charge given the evaluated Z 
  double GetMeanCharge(double qtot_x, double qtot_y, int nq_x, int nq_y, int Z);
  //! Get likelihood to be Z for a given event (0: x-side, 1: y-side, 2: both sides when possible, otherwise y-side)
  // double GetLogLikelihoodToBeZ(int& npoints, double& qmean, int Z, int iside = 2);
  like_t GetLogLikelihoodToBeZ(int Z, int iside = 2);
  //! Get best charge returning the list of likelihood objects from most probable to least probable (with type = 10j get evaluation of layer j) 
  int    GetZ(vector<like_t>& likelihood, TrTrackR* track, int type = kInner, int iside = 2, double beta = 1, double rigidity = 0, double mass_on_Z = 2*0.93149);
  /**@}*/


  /** @name Old methods for floating point charge and integer charge estimators. */
  /**@{*/
  //! The rigidity could be used to estimate beta  
  static float  GetBetaFromRigidity(float rigidity, int Z, float mass);
  //! Get the probability 
  static double GetProbToBeZ(TrRecHitR* hit, int iside, int Z, float beta = 1);
  //! Truncated mean probability (inner tracker), temporary solution for Z estimator
  static like_t GetTruncMeanProbToBeZ(TrTrackR* track, int Z, float beta = 1);
  //! Truncated mean charge (inner tracker) (DEPRECATED)
  static int    GetTruncMeanCharge(TrTrackR* track, float beta = 1);
  //! A first attempt to get a Q evaluator (DEPRECATED) 
  static float  GetQ(TrTrackR* track, int iside, float beta = 1);
  //! Likelihood computation   
  static like_t GetLogLikelihoodToBeZ(int type, TrTrackR* track, int iside, int Z, float beta = 1);
  //! Likelihood charge
  static like_t GetLogLikelihoodCharge(int type, TrTrackR* track, int iside, float beta = 1);
  /**@}*/

  
  /** @name Charge-related methods for reconstruction. */
  /**@{*/
  //! Get mean signal of 4 highest clusters 
  static mean_t GetMeanHighestFourClusters(int type, int iside, int opt = TrClusterR::DefaultCorrOpt);
  /**@}*/


  /** @name Histogramming methods */
  //! Enable the histogram filling
  static void   EnableHistograms()  { Histograms.Enable();  EnabledHistograms = true; }
  //! Disable the histogram filling
  static void   DisableHistograms() { Histograms.Disable(); EnabledHistograms = false; }
  //! Save histograms on a file
  static void   Save(TFile* file) { Histograms.Save(file,"TrCharge"); }
  /**@}*/


};

#endif
