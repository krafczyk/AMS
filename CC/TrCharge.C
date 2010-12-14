#include "TrCharge.h"

float TrCharge::A_BetaBetheBlock = 1.6;
float TrCharge::B_BetaBetheBlock = 15;
float TrCharge::MipBetaValue = 0.935;   // MIP value


float TrCharge::BetaBetheBlock(float beta) {
  if (beta>=1) {
    printf("TrCharge::BetaBetheBlock-Error Beta is greater-equal 1 (beta=%f), returning 0\n",beta);
    return 0;
  }
  float corrbeta = TMath::Min((float)0.99999,beta);   
  float b2 = pow(corrbeta,2);
  return A_BetaBetheBlock/b2 * (B_BetaBetheBlock + log(b2/(1-b2)) - b2);
}  


float TrCharge::GetSignalWithBetaCorrection(float signal, float beta) {
  return signal*TrCharge::BetaBetheBlock(TrCharge::MipBetaValue)/TrCharge::BetaBetheBlock(beta);
}


float TrCharge::GetSignalWithRigidityCorrection(float signal, float rigidity, float mass) {
  // To be implemented 
  return 0;
}


float TrCharge::GetMean(vector<float> signal) {
  int   n    = 0;
  float mean = 0;
  for (int ii=0; ii<signal.size(); ii++) {
    if (signal.at(ii)<0) continue;
    mean += signal.at(ii);
    n++;
  }
  if (n==0) return 0.;
  return mean/n;
}


float TrCharge::GetTruncatedMean(vector<float> signal) {
  int   n    = 0;
  float mean = 0;
  float maxampl = 0;
  for (int ii=0; ii<signal.size(); ii++) {
    if (signal.at(ii)<0) continue;
    if (signal.at(ii)>maxampl) maxampl = signal.at(ii);
    mean += signal.at(ii);
    n++;
  }
  if (n<=1) return 0.;
  return (mean - maxampl)/(n-1);
}


float TrCharge::GetGaussianizedMean(vector<float> signal) {
  // calculate mean and rms
  int   n    = 0;
  float mean = 0;
  float rms = 0;
  for (int ii=0; ii<signal.size(); ii++) {
    if (signal.at(ii)<0) continue;
    mean += signal.at(ii);
    rms += pow(signal.at(ii),2);
    n++;
  }
  if (n==0) return 0.;
  mean /= n;
  rms /= n;
  rms = sqrt(fabs(rms - mean*mean));
  // iterate selecting clusters
  int   gaussn = 0;
  float gaussmean = 0;
  for (int ii=0; ii<signal.size(); ii++) {
    if (signal.at(ii)<0) continue;
    if ((signal[ii]-mean)>3*rms) continue; // 3 sigma
    gaussmean += signal.at(ii);
    gaussn++;
  }
  if (gaussn==0) return 0.;
  return gaussmean/gaussn;
}


float TrCharge::GetMeanByType(int type, TrTrackR* track, int side, float beta, int layer, int opt) {
  vector<float> signal;
  for (int ihit=0; ihit<track->GetNhits(); ihit++) {
    TrRecHitR*  hit = track->GetHit(ihit);
    if (hit==0) continue;
    TrClusterR* cluster = (side==0) ? hit->GetXCluster() : hit->GetYCluster();
    if (cluster==0) continue;
    if (cluster->GetLayer()==layer) continue;
    signal.push_back(GetSignalWithBetaCorrection(cluster,beta,opt));
  }
  switch (type) {
    case 0:
      return GetMean(signal);
      break;
    case 1:
      return GetTruncatedMean(signal);
      break;
    case 2:
      return GetGaussianizedMean(signal);
      break;
  }
  return 0;
}    
