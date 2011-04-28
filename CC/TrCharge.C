#include "TrCharge.h"


float TrCharge::A_BetaBetheBlock = 1.6;
float TrCharge::B_BetaBetheBlock = 15;
float TrCharge::MipBetaValue = 0.935;   // MIP value
int   TrCharge::MaxCharge = 26; // maximum  


float TrCharge::BetaBetheBlock(float beta) {
  /* pseudo Bethe-Block: BB(beta) = A/beta^2  *  [ B + ln[beta^2/(1-beta^2)] - beta^2 ] */
  if (beta>=1) {
    // printf("TrCharge::BetaBetheBlock-Error Beta is greater-equal 1 (beta=%f).\n",beta);
    // return 0;
  }
  float corrbeta = TMath::Min(TrCharge::MipBetaValue,beta);   
  float b2 = pow(corrbeta,2);
  // return A_BetaBetheBlock/b2 * (B_BetaBetheBlock + log(b2/(1-b2)) - b2);
  return A_BetaBetheBlock/b2;
}  


float TrCharge::GetBetaFromRigidity(float rigidity, int Z, float mass) {
  return 1./sqrt(1.-pow(1.*mass/(rigidity*Z),2) ); 
}


float TrCharge::GetSignalWithBetaCorrection(TrRecHitR* hit, int iside, float beta, int opt) {
  return hit->GetSignalCombination(iside,opt)*TrCharge::BetaBetheBlock(TrCharge::MipBetaValue)/TrCharge::BetaBetheBlock(beta);  
}


float TrCharge::GetProbToBeZ(TrRecHitR* hit, int iside, int Z, float beta, int opt) {
  TrPdf* pdf = TrPdfDB::GetHead()->Get(Z,iside,0);
  if (pdf==0) {
    printf("TrCharge::GetProbToBeZ-W requesting a not-existing pdf (iside=%d,Z=%d,type=0), returning -1.\n",iside,Z);
    return -1;
  }
  float signal = GetSignalWithBetaCorrection(hit,iside,beta,opt);
  return pdf->Eval(signal);
  return -1;
}


bool TrCharge::GoodChargeReconCluster(TrClusterR* cluster) {
  if (cluster==0) return false;
  // dead strips in the cluster or before/after?
  bool dead = false;
  int ndead = 0;
  for (int i=-1; i<cluster->GetNelem()+1; i++) {
    int iside   = cluster->GetSide();
    int address = i + cluster->GetAddress();
    if (cluster->IsK7()) address = cluster->GetAddress(i);   // cycl.
    if ( (iside==0)&&(address<640)&&(address>1023) ) continue; // out-of-bounds
    if ( (iside==1)&&(address<  0)&&(address> 639) ) continue; // out-of-bounds
    short status = cluster->GetStatus(i); 
    // this cut could be too tight (... maybe only if is near to seed strip ...) 
    ndead += ( (status>>0)&0x1 ) | ( (status>>2)&0x1 ); // dead strips
  }
  dead = (ndead>0);
  // good cluster shape? not implemented for now
  bool badshape = false;
  // return
  return !(dead||badshape);
}


bool TrCharge::GoodChargeReconHit(TrRecHitR* hit) {
  if (hit==0) return false;
  TrClusterR* clx = hit->GetXCluster();
  TrClusterR* cly = hit->GetYCluster();
  // no one of the two
  if ( (clx==0)||(cly==0) ) return false;
  // one of the two is not good
  if ( (!GoodChargeReconCluster(clx)) ||
       (!GoodChargeReconCluster(cly)) ) return false; 
  // bad combination? not implemented for now
  return true;
}


mean_t TrCharge::GetMean(vector<float> signal) {
  int   n    = 0;
  float mean = 0;
  float rms  = 0;
  for (int ii=0; ii<(int)signal.size(); ii++) {
    if (signal.at(ii)<0) continue;
    mean += signal.at(ii);
    rms  += pow(signal.at(ii),2); 
    n++;
  }
  mean /= n;
  rms  /= n;
  rms = sqrt(rms - mean*mean);
  if (n==0) return mean_t();
  return mean_t(0,-1,n,mean,rms);
}


mean_t TrCharge::GetTruncatedMean(vector<float> signal) {
  int   n       = 0;
  float mean    = 0;
  float maxampl = 0;
  float rms     = 0;
  for (int ii=0; ii<(int)signal.size(); ii++) {
    if (signal.at(ii)<0) continue;
    if (signal.at(ii)>maxampl) maxampl = signal.at(ii);
    mean += signal.at(ii);
    rms  += pow(signal.at(ii),2);
    n++;
  }
  if (n<=1) return mean_t();
  mean = (mean - maxampl)/(n-1);
  rms  = (rms - pow(maxampl,2))/(n-1);
  rms  = sqrt(rms - mean*mean);    
  return mean_t(1,-1,n-1,mean,rms);
}


mean_t TrCharge::GetGaussianizedMean(vector<float> signal) {
  // calculate mean and rms
  int   n    = 0;
  float mean = 0;
  float rms  = 0;
  for (int ii=0; ii<(int)signal.size(); ii++) {
    if (signal.at(ii)<0) continue;
    mean += signal.at(ii);
    rms += pow(signal.at(ii),2);
    n++;
  }
  if (n==0) return mean_t();
  // iterate selecting clusters
  mean /= n;
  rms  /= n;
  rms = sqrt(rms - mean*mean);
  int   gaussn    = 0;
  float gaussmean = 0;
  float gaussrms  = 0;
  for (int ii=0; ii<(int)signal.size(); ii++) {
    if (signal.at(ii)<0) continue;
    if ((signal[ii]-mean)>3*rms) continue; // 3 sigma
    gaussmean += signal.at(ii);
    gaussrms  += pow(signal.at(ii),2);
    gaussn++;
  }
  if (gaussn==0) return mean_t();
  gaussmean /= gaussn;
  gaussrms  /= gaussn;
  gaussrms = sqrt(gaussrms - gaussmean*gaussmean);
  return mean_t(2,-1,gaussn,gaussmean,gaussrms);
}


mean_t TrCharge::GetMeanByType(int type, TrTrackR* track, int iside, float beta, int layerj, int opt) {
  vector<float> signal;  
  for (int ihit=0; ihit<track->GetNhits(); ihit++) {
    TrRecHitR* hit = track->GetHit(ihit);
    int LayerJ = hit->GetLayerJ();
    if (hit==0) continue;
    // hit analysis
    if ( (iside>1)&&(!GoodChargeReconHit(hit)) ) continue;
    // cluster analysis
    TrClusterR* cluster = (iside==0) ? hit->GetXCluster() : hit->GetYCluster();
    if ( (iside<=1)&&(!GoodChargeReconCluster(cluster)) ) continue;
    // excluded layer 
    if (LayerJ==layerj) continue;
    signal.push_back(GetSignalWithBetaCorrection(hit,iside,beta,opt));
  }
  mean_t mean; 
  switch (type) {
    case kPlainMean:
      mean = GetMean(signal);
      break;
    case kTruncMean:
      mean = GetTruncatedMean(signal);
      break;
    case kGaussMean:
      mean = GetGaussianizedMean(signal);
      break;
  }
  // set side and other ... 
  mean.Side = iside;
  return mean;
}   


mean_t TrCharge::GetMean(TrTrackR* track, int iside, float beta, int layer, int opt) {
  return GetMeanByType(kPlainMean, track, iside, beta, layer, opt);
}


mean_t TrCharge::GetTruncatedMean(TrTrackR* track, int iside, float beta, int layer, int opt) {
  return GetMeanByType(kTruncMean, track, iside, beta, layer, opt);
}


mean_t TrCharge::GetGaussianizedMean(TrTrackR* track, int iside, float beta, int layer, int opt) {
  return GetMeanByType(kGaussMean, track, iside, beta, layer, opt);
}


like_t TrCharge::GetMeanProbToBeZ(mean_t mean, int Z) { 
  // implemented only for truncated mean, in ADC scale
  if (mean.Type!=kTruncMean) {
    printf("TrCharge::GetTruncatedMeanProbToBeZ-W algo. implemented only for truncated mean.\n");
    return like_t();
  }

  // some kind of selection:
  // - minimum number of hits?
  // - (inner tracker ... maybe not at this level)
 
  // take the corresponding pdf
  TrPdf* pdf = TrPdfDB::GetHead()->Get(Z,mean.Side,1); // put the correct distributions ... 
  if (pdf==0) {
    printf("TrCharge::GetTruncatedMeanProbToBeZ-W requesting a not-existing pdf (iside=%d,Z=%d,type=1), returning nothing.\n",mean.Side,Z);
    return like_t();
  }

  // evaluate
  return like_t(0,mean.Side,mean.NPoints,pdf->Eval(mean.Mean),mean);
}


int TrCharge::GetMeanCharge(mean_t mean) {
  int bestz = -1;
  float maxprob = 0;
  for (int z=1; z<=MaxCharge; z++) {
    float prob = GetMeanProbToBeZ(mean,z).LogLike;
    if (prob>maxprob) bestz = z;
  } 
  return bestz;
}


like_t TrCharge::GetLogLikelihoodToBeZ(int type, TrTrackR* track, int iside, int Z, float beta) {
  // loop on hits belonging to the track
  // decide if good or not
  // calculate likelihood 
  // drop one hit? 
  return like_t();
}


void TrCharge::reaxtkcharge(float beta) {
  // one-th recon:
  // Truncated mean distributions (gaussians) in ADC scale <<< only inner? 
  // Return to AMSCharge probs

  // two-th recon:
  // conversion to MIP scale
  // likelihood calculation

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
}

