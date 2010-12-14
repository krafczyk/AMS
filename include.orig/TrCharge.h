#ifndef __TrCharge__
#define __TrCharge__

#include "TrCluster.h"
#include "TrRecHit.h"
#include "TrTrack.h"

#include "TMath.h"

#include <cmath>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

class TrCharge {

 public:
 
  //! 1st parameter of the pseudo-Bethe-Block
  static float A_BetaBetheBlock;
  //! 2nd parameter of the pseudo-Bethe-Block
  static float B_BetaBetheBlock;
  //! Default beta value (MIP)
  static float MipBetaValue;

  /////////////////////////
  // Cluster methods
  /////////////////////////
  
  //! The Bethe-Block formula in the limit of negligible mass term (no electrons) 
  /* pseudo Bethe-Block: BB(beta) = A/beta^2  *  [ B + ln[beta^2/(1-beta^2)] - beta^2 ] */
  static float BetaBetheBlock(float beta);
  //! Signal beta Correction (normalization to MIP) 
  static float GetSignalWithBetaCorrection(float signal, float beta = MipBetaValue);
  //! Cluster signal beta correction 
  static float GetSignalWithBetaCorrection(TrClusterR* cluster, float beta = MipBetaValue, int opt = TrClusterR::DefaultCorrOpt) {
    float signal = cluster->GetTotSignal(opt); 
    return GetSignalWithBetaCorrection(signal,beta); 
  }
  //! Signal rigidity correction 
  static float GetSignalWithRigidityCorrection(float signal, float rigidity, float mass);
  //! Cluster signal rigidity correction
  static float GetSignalWithRigidityCorrection(TrClusterR* cluster, float rigidity, float mass, int opt = TrClusterR::DefaultCorrOpt) {
    float signal = cluster->GetTotSignal(opt); 
    return GetSignalWithRigidityCorrection(signal,rigidity,mass); 
  }

  /////////////////////////
  // Track methods
  /////////////////////////

  //! Mean of n signals
  static float GetMean(vector<float> signal);
  //! Track average signal (beta correction, layer=(0-8) indicates an excluded layer)
  static float GetMeanX(TrTrackR* track, float beta = MipBetaValue, int layer = -1, int opt = TrClusterR::DefaultCorrOpt) {
    return GetMeanByType(0, track, 0, beta, layer, opt);
  }
  static float GetMeanY(TrTrackR* track, float beta = MipBetaValue, int layer = -1, int opt = TrClusterR::DefaultCorrOpt) {
    return GetMeanByType(0, track, 1, beta, layer, opt);
  }
  //! Truncated mean of n signals
  static float GetTruncatedMean(vector<float> signal);
  //! Track truncated mean signal (beta correction, layer=(0-8) indicates an excluded layer)
  static float GetTruncatedMeanX(TrTrackR* track, float beta = MipBetaValue, int layer = -1, int opt = TrClusterR::DefaultCorrOpt) {
    return GetMeanByType(1, track, 0, beta, layer, opt);
  }
  static float GetTruncatedMeanY(TrTrackR* track, float beta = MipBetaValue, int layer = -1, int opt = TrClusterR::DefaultCorrOpt) {
    return GetMeanByType(1, track, 1, beta, layer, opt);
  }
  //! Gaussianized mean of n signals (discarding out-of-3-sigma signals)
  static float GetGaussianizedMean(vector<float> signal);
  //! Track gaussianized mean of n signals (beta correction, layer=(0-8) indicates an excluded layer)
  static float GetGaussianizedMeanX(TrTrackR* track, float beta = MipBetaValue, int layer = -1, int opt = TrClusterR::DefaultCorrOpt) {
    return GetMeanByType(2, track, 0, beta, layer, opt);
  }
  static float GetGaussianizedMeanY(TrTrackR* track, float beta = MipBetaValue, int layer = -1, int opt = TrClusterR::DefaultCorrOpt) {
    return GetMeanByType(2, track, 1, beta, layer, opt);
  }
  //! Track mean generic method (0 = plain mean, 1 = truncated mean, 2 = gaussianized mean)
  static float GetMeanByType(int type, TrTrackR* track, int side, float beta = MipBetaValue, int layer = -1, int opt = TrClusterR::DefaultCorrOpt);

};

#endif
