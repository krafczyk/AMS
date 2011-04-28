#ifndef __TrCharge__
#define __TrCharge__

/*! 
 \file  TrCharge.h
 \brief Header file of TrCharge class

 \class TrCharge
 \brief A static class for the Tracker charge reconstruction

 $Date: 2011/04/28 11:19:04 $

 $Revision: 1.2 $
*/

#include "TrCluster.h"
#include "TrRecHit.h"
#include "TrTrack.h"
#include "TrPdf.h"
#include "TrPdfDB.h"

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
  //! Mean computation type
  int   Type;
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
  mean_t(int t, int s = 0, int n = 0, float m = 0, float r = 0) { clear(); Type = t; Side = s; NPoints = n; Mean = m; RMS = r; }
  //! Copy constructor
  mean_t(const mean_t &that) { copy(that); } 
  //! Assignment operator
  mean_t& operator=(const mean_t& that) { if (this!=&that) copy(that); return *this; }
  //! Destructor
  ~mean_t() { clear(); }
  //! Clear
  void clear() { Type = 0; Side = 0; NPoints = 0; Mean = 0; RMS = 0; }
  //! Copy
  void copy(const mean_t &that) { Type = that.Type; Side = that.Side; NPoints = that.NPoints; Mean = that.Mean; RMS = that.RMS; }
  //! Approximate charge value (if mean is in MIP units)
  float GetCharge() { return sqrt(Mean); }
  //! Error on charge value
  float GetChargeErr() { return RMS/(2*sqrt(Mean)); }
  //! Rounded value (if mean is in MIP units)
  int   GetZ() { return floor(GetZ()+0.5); }
};


//! Class used to retrieve results of a likelihood calculation
class like_t {
 public:
  //! Loglikelihood computation type
  int    Type;
  //! Side 
  int    Side;
  //! Number of points 
  int    NPoints;
  //! Likelihood
  float  LogLike; 
  //! Mean estimation
  mean_t Mean;
  //! Constructor
  like_t() { clear(); }
  //! Constructor
  like_t(int t, int s = 0, int n = 0, float ll = 0) { clear(); Type = t; Side = s; NPoints = n; LogLike = ll; }
  //! Constructor
  like_t(int t, int s, int n, float ll, mean_t m) { clear(); Type = t; Side = s; NPoints = n; LogLike = ll; Mean = m; }
  //! Destructor
  ~like_t() { clear(); }
  //! Clear
  void clear() { Type = 0; Side = 0; NPoints = 0; LogLike = 0; Mean.clear(); }
};


//! Class for the Tracker charge reconstruction
class TrCharge {

 public:

  /////////////////////////
  // Data Members
  /////////////////////////

  //! Maximum Z analyzed
  static int MaxCharge; 

  //! 1st parameter of the pseudo-Bethe-Block
  static float A_BetaBetheBlock;
  //! 2nd parameter of the pseudo-Bethe-Block
  static float B_BetaBetheBlock;
  //! Default beta value (MIP)
  static float MipBetaValue;

  /////////////////////////
  // Enumerators
  /////////////////////////
  
  //! Available mean algorithms
  enum MeanType { kPlainMean = 0, kTruncMean = 1, kGaussMean = 2 };
  //! Available likelihood calculations
  enum LikeType { kFromMean = 0, kAllPoints = 1, kDropOne = 2, kBestFour = 3, kInner = 4, kTop = 5, kLower = 6 };
  //! Following the TrRecHit convention, 0: only-x, 1: only-y, 2: xy weighted mean, 3: xy not weighted
  enum SideType { kX = 0, kY = 1, kXY = 2, kXYUnWei = 3 };

  /////////////////////////
  // Cluster/Hit methods
  /////////////////////////

  //! Good cluster for charge reconstruction
  static bool  GoodChargeReconCluster(TrClusterR* cluster);
  //! Good hit for charge reconstruction
  static bool  GoodChargeReconHit(TrRecHitR* hit);

  /// Check: other signal corrections (redo the impact point correction?)
  
  //! The Bethe-Block formula in the limit of negligible mass term (no electrons) 
  static float BetaBetheBlock(float beta);
  //! The rigidity could be used to estimate beta  
  static float GetBetaFromRigidity(float rigidity, int Z, float mass);
  //! Get the signal 
  static float GetSignalWithBetaCorrection(TrRecHitR* hit, int iside, float beta = MipBetaValue, int opt = TrClusterR::DefaultCorrOpt);
  //! Get the probability 
  static float GetProbToBeZ(TrRecHitR* hit, int iside, int Z, float beta = MipBetaValue, int opt = TrClusterR::DefaultCorrOpt); 

  /////////////////////////
  // Averaging methods
  /////////////////////////

  //! Track mean generic method
  static mean_t GetMeanByType(int type, TrTrackR* track, int iside, float beta = MipBetaValue, int jlayer = -1, int opt = TrClusterR::DefaultCorrOpt);

  //! Mean of n signals
  static mean_t GetMean(vector<float> signal);
  //! Track X mean signal (beta correction, jlayer = 1, ..., 9 indicates an excluded layer)
  static mean_t GetMean(TrTrackR* track, int iside, float beta = MipBetaValue, int jlayer = -1, int opt = TrClusterR::DefaultCorrOpt);

  //! Truncated mean of n signals
  static mean_t GetTruncatedMean(vector<float> signal);
  //! Track X truncated mean signal (beta correction, jlayer = 1, ..., 9 indicates an excluded layer)
  static mean_t GetTruncatedMean(TrTrackR* track, int iside, float beta = MipBetaValue, int jlayer = -1, int opt = TrClusterR::DefaultCorrOpt);

  //! Gaussianized mean of n signals (discarding out-of-3-sigma signals)
  static mean_t GetGaussianizedMean(vector<float> signal);
  //! Track X gaussianized mean of n signals (beta correction, layer=(0-8) indicates an excluded layer)
  static mean_t GetGaussianizedMean(TrTrackR* track, int iside, float beta = MipBetaValue, int jlayer = -1, int opt = TrClusterR::DefaultCorrOpt);

  //! Mean probability (only truncated mean has been implemented ...)
  static like_t GetMeanProbToBeZ(mean_t mean, int Z);
  //! Get charge from mean probability (only truncated mean has been implemented ...) 
  static int    GetMeanCharge(mean_t mean);

  /////////////////////////
  // Likelihood methods
  /////////////////////////

  //! Likelihood computation methods (different likelihood methods with different type, 
  //! 0 = all, 1 = drop 1, 2 = use best 5, 3 use inner only 
  static like_t  GetLogLikelihoodToBeZ(int type, TrTrackR* track, int iside, int Z, float beta = MipBetaValue);

  /////////////////////////
  // Charge reconstruction
  /////////////////////////

  //! Charge reconstruction call (beta needed)
  static void reaxtkcharge(float beta);

  /* 
     interface to AMSCharge: what will be used? 
     how to interface?: AMS Charge can call the loop or TrCharge can use the AMSCharge pointer?
     
   number  _TrMeanTracker;
   number  _ProbTracker[maxzin];   // prob to be e,p,He,...,F
   number  _LkhdTracker[maxzin];   // loglikhd value for e,p, ...
   integer _IndxTracker[maxzin];   // index 0,...,9 from most to least prob charge
   integer _iTracker;              // index of most probable charge
   number  _ProbAllTracker;        // prob of maximum charge using all tracker clusters
  */

};

#endif
