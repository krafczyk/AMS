#ifndef __TrCharge__
#define __TrCharge__

/*! 
 \file  TrCharge.h
 \brief Header file of TrCharge class

 \class TrCharge
 \brief A static class for the Tracker charge reconstruction

 $Date: 2011/05/13 19:28:09 $

 $Revision: 1.4.2.1 $
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
  //! Approximate charge value, charge units (if mean is in MIP units)
  float GetQ() { return sqrt(Mean); }
  //! Error on charge value
  float GetQErr() { return RMS/(2*sqrt(Mean)); }
  //! Rounded value (if mean is in MIP units)
  int   GetZ() { return int(floor(GetZ()+0.5)); }
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
  //! Probability
  float  Prob;
  //! Mean
  float  Mean;
  //! Constructor
  like_t() { clear(); }
  //! Constructor
  like_t(int t, int s = 0, int n = 0, float ll = 0, float p = 0, float m = 0) { clear(); Type = t; Side = s; NPoints = n; LogLike = ll; Prob = p; Mean = m; }
  //! Constructor
  //! Destructor
  ~like_t() { clear(); }
  //! Clear
  void clear() { Type = 0; Side = 0; NPoints = 0; LogLike = 0; Prob = 0; Mean = 0; }
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

  //! AdcVsBeta: A
  static float A_AdcVsBeta;
  //! AdcVsBeta: B
  static float B_AdcVsBeta;
  //! AdcVsBeta: Beta_0
  static float b0_AdcVsBeta;
  //! AdcVsBeta: k
  static float k_AdcVsBeta;

  /////////////////////////
  // Enumerators
  /////////////////////////

  //! TrRecHit convention, 0: only-x, 1: only-y, 2: xy weighted mean, 3: xy not weighted
  enum SideType { kX = 0, kY = 1, kXY = 2, kXYUnWei = 3 };
  //! Plane combinations
  enum CombType { kInner = 0x01, kTop = 0x02, kLower = 0x04 };
  //! Available mean algorithms
  enum MeanType { kPlainMean = 0x100, kTruncMean = 0x200, kGaussMean = 0x400 };
  //! Available likelihood calculations
  enum LikeType { kTruncatedMean = 0x100, kAllPoints = 0x200, kDropOne = 0x400, kBestFour = 0x800 };

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
  //! Simple function to describe the energy loss versus beta
  static float AdcVsBeta(float beta);
  //! The rigidity could be used to estimate beta  
  static float GetBetaFromRigidity(float rigidity, int Z, float mass);
  //! Get the signal 
  static float GetSignalWithBetaCorrection(TrRecHitR* hit, int iside, float beta = 1, int opt = TrClusterR::DefaultCorrOpt);
  //! Get the signal
  static float GetSignalWithBetaCorrection(TrClusterR* cluster, float beta = 1, int opt = TrClusterR::DefaultCorrOpt);
  //! Get the probability 
  static float GetProbToBeZ(TrRecHitR* hit, int iside, int Z, float beta = 1); 

  /////////////////////////
  // Averaging methods
  /////////////////////////

  //! Track mean generic method
  static mean_t GetMean(int type, TrTrackR* track, int iside, float beta = 1, int jlayer = -1, int opt = TrClusterR::DefaultCorrOpt);

  //! Mean of n signals
  static mean_t GetPlainMean(vector<float> signal);
  //! Truncated mean of n signals
  static mean_t GetTruncMean(vector<float> signal);
  //! Gaussianized mean of n signals (discarding out-of-3-sigma signals)
  static mean_t GetGaussMean(vector<float> signal);

  //! Truncated mean probability (inner tracker)
  static like_t GetTruncMeanProbToBeZ(TrTrackR* track, int Z, float beta = 1);
  //! Truncated mean charge (inner tracker)
  static int    GetTruncMeanCharge(TrTrackR* track, float beta = 1);
  //! Best available Q evaluation (truncated mean, charge units, all tracker) [0: x, 1: y, 2: xy weigh, 3: xy plain]
  static float  GetQ(TrTrackR* track, int iside, float beta = 1);

  /////////////////////////
  // Likelihood methods
  /////////////////////////

  //! Likelihood computation generic method   
  static like_t  GetLogLikelihoodToBeZ(int type, TrTrackR* track, int iside, int Z, float beta = 1);

};

#endif
