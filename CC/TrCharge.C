#include "TrCharge.h"


// Maximum charge to be analyzed
int   TrCharge::MaxCharge = 26; 


/*
  - Beta Correction Parameters:
    KSC 2010, muons + proton signal   
    plane-by-plane vs beta TOF normalized
    three values for 1st plane, anyone of the inner, 9th plane
*/
float TrCharge::A_BetaCorr[3]  = { 1.14505, 0.73906, -0.39480}; 
float TrCharge::B_BetaCorr[3]  = { 0.67118, 0.05288, -1.47569};
float TrCharge::b0_BetaCorr[3] = { 0.85387, 0.89147,  0.93449}; 


float TrCharge::BetaCorrection(float beta, int jlayer) {
  /*
     - Maximum Probability Energy Loss:
       MPEL(300 um of Si) = 53.6614 eV / beta^2 * { 12.1489 - 2 log(beta) - beta^2 - 0.1492 * 
                            * [max(0,2.8716-log(beta gamma)/log(10))]^3.2546}, beta > 0.20
     - Simplified fitting function:
       f(beta) = beta<beta0, A/beta^2 + B*log(beta)/beta^2 + C
                 beta>beta0, k (the TOF beta "saturates")     
       Continuity imposed on beta0 (no derivative continuity)
       C = k - A/beta0^2 - B*log(beta0)/beta0^2
     - Beta Correction:
       g(beta) = beta<beta0, A/beta^2 + B*log(beta)/beta^2 + 1 - A/beta0^2 - B*log(beta0)/beta0^2
                 beta>beta0, 1
  */
  if ( (jlayer<1)||(jlayer>9) ) {
    printf("TrCharge::BetaCorrection-W invalid layer index number (%d), returning 1.\n",index);
    return 1.;
  }
  int index  = 0;        // 0 for the layer over TRD
  if (jlayer>1) index++; // 1 for any inner tracker layer 
  if (jlayer>8) index++; // 2 for the layer over ECAL
  if (beta>=b0_BetaCorr[index]) return 1.; // beta "saturation" region
  return A_BetaCorr[index]/pow(beta,2) + 
         B_BetaCorr[index]*log(beta)/pow(beta,2) + 
         1 - 
         A_BetaCorr[index]/pow(b0_BetaCorr[index],2) - 
         B_BetaCorr[index]*log(b0_BetaCorr[index])/pow(b0_BetaCorr[index],2);
}


float TrCharge::GetBetaFromRigidity(float rigidity, int Z, float mass) {
  return 1./sqrt(1.-pow(1.*mass/(rigidity*Z),2) ); 
}


float TrCharge::GetSignalWithBetaCorrection(TrRecHitR* hit, int iside, float beta, int opt) {
  // pointer check
  if (hit==0) {
    printf("TrCharge::GetSignalWithBetaCorrection-W hit with NULL pointer, returning 1\n");
    return 1;
  }
  // beta over 1?
  if (fabs(beta)>=1) return hit->GetSignalCombination(iside,opt);
  int jlayer = hit->GetLayerJ();
  return hit->GetSignalCombination(iside,opt)/TrCharge::BetaCorrection(beta,jlayer);
}


float TrCharge::GetSignalWithBetaCorrection(TrClusterR* cluster, float beta, int opt) {
  // pointer check
  if (cluster==0) {
    printf("TrCharge::GetSignalWithBetaCorrection-W cluster with NULL pointer, returning 1\n");
    return 1;
  }
  // beta over 1?
  if (fabs(beta)>=1) return cluster->GetTotSignal(opt);
  int jlayer = cluster->GetLayerJ();
  return cluster->GetTotSignal(opt)/TrCharge::BetaCorrection(beta,jlayer);
}


float TrCharge::GetProbToBeZ(TrRecHitR* hit, int iside, int Z, float beta) {
  // pointer check
  if (hit==0) {
    printf("TrCharge::GetPropToBeZ-W hit with NULL pointer, returning 0\n");
    return 0; 
  }
  // Z = 0 means electrons (approximately no rise)
  if (Z==0) { 
    Z = 1; 
    beta = 1; 
  }
  /* ATTENTION: 
     the requested PDF should be in MIP scale,
     for the time being it is not! 
  */
  TrPdf* pdf = TrPdfDB::GetHead()->Get(Z,iside,TrPdfDB::kSingleLayer);
  if (pdf==0) {
    printf("TrCharge::GetProbToBeZ-W requesting a not-existing pdf (iside=%d,Z=%d,type=%d), returning -1.\n",
           iside,Z,TrPdfDB::kSingleLayer);
    return 0;
  }
  float signal = GetSignalWithBetaCorrection(hit,iside,beta,TrClusterR::DefaultMipCorrOpt);
  return pdf->Eval(signal);
}


bool TrCharge::GoodChargeReconCluster(TrClusterR* cluster) {
  // pointer check
  if (cluster==0) return false;
  int ndead = 0;
  int nbad  = 0;
  // loop on cluster strips + one before + one after
  for (int i=-1; i<cluster->GetNelem()+1; i++) {
    int iside   = cluster->GetSide();
    int address = i + cluster->GetAddress();
    if (cluster->IsK7()) address = cluster->GetAddress(i);   // cycl.
    if ( (iside==0)&&(address<640)&&(address>1023) ) continue; // out-of-bounds
    if ( (iside==1)&&(address<  0)&&(address> 639) ) continue; // out-of-bounds
    short status = cluster->GetStatus(i); 
    ndead += ( (status>>0)&0x1 ) | ( (status>>2)&0x1 ); // dead strips
    nbad  += status!=0;
  }
  // dead strip  
  bool dead = (ndead>0);
  // no noisy cluster 
  bool bad  = false; // (nbad>3); // good
  // good cluster shape: not implemented for now
  bool badshape = false;
  // return
  return !(dead||bad||badshape);
}


bool TrCharge::GoodChargeReconHit(TrRecHitR* hit) {
  // pointer check
  if (hit==0) return false;
  TrClusterR* clx = hit->GetXCluster();
  TrClusterR* cly = hit->GetYCluster();
  // no one of the two
  if ( (clx==0)||(cly==0) ) return false;
  // one of the two is not good
  if ( (!GoodChargeReconCluster(clx)) ||
       (!GoodChargeReconCluster(cly)) ) return false; 
  // bad combination: not implemented for now
  return true;
}


mean_t TrCharge::GetPlainMean(vector<float> signal) {
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
  return mean_t(kPlainMean,-1,n,mean,rms);
}


mean_t TrCharge::GetTruncMean(vector<float> signal) {
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
  return mean_t(kTruncMean,-1,n-1,mean,rms);
}


mean_t TrCharge::GetGaussMean(vector<float> signal) {
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
  return mean_t(kGaussMean,-1,gaussn,gaussmean,gaussrms);
}


mean_t TrCharge::GetMean(int type, TrTrackR* track, int iside, float beta, int layerj, int opt) {
  // Signal Selection
  vector<float> signal;  
  for (int ihit=0; ihit<track->GetNhits(); ihit++) {
    TrRecHitR* hit = track->GetHit(ihit);
    if (hit==0) {
      printf("TrCharge::GetMean-W hit in the track with NULL pointer, skipping it.\n");
      continue;
    }
    // requested inner
    int LayerJ = hit->GetLayerJ();
    if ( (type&kInner)&&( (LayerJ==1)||(LayerJ==9) ) ) continue;
    // if hit analysis
    if ( (iside>1)&&(!GoodChargeReconHit(hit)) ) continue;
    // if cluster analysis
    TrClusterR* cluster = (iside==0) ? hit->GetXCluster() : hit->GetYCluster();
    if ( (iside<=1)&&(!GoodChargeReconCluster(cluster)) ) continue;
    // an excluded layer 
    if (LayerJ==layerj) continue;
    // add 
    signal.push_back(GetSignalWithBetaCorrection(hit,iside,beta,opt));
  }
  // Mean Computation
  mean_t mean;
  if      (type&kPlainMean) mean = GetPlainMean(signal);
  else if (type&kTruncMean) mean = GetTruncMean(signal);
  else if (type&kGaussMean) mean = GetGaussMean(signal);
  // set side  
  mean.Side = iside;
  return mean;
}   


like_t TrCharge::GetTruncMeanProbToBeZ(TrTrackR* track, int Z, float beta) { 
  /* calculating it every time: TO BE FIXED */
  // Z=0 means electrons (approximately no rise)
  if (Z==0) { Z = 1; beta = 1; }
  // Truncated mean computation (X side)
  mean_t mean = GetMean(kTruncMean|kInner,track,kX,beta,-1,TrClusterR::DefaultCorrOpt);
  // Take the corresponding PDF
  TrPdf* pdf = TrPdfDB::GetHead()->Get(Z,kX,TrPdfDB::kTruncatedMean);
  if (pdf==0) {
    // printf("TrCharge::GetMeanProbToBeZ-W requesting a not-existing pdf (Z=%d,type=%d), returning -1.\n",
    //        Z,TrPdfDB::kTruncatedMean);
    return like_t();
  }
  // Evaluate
  float value = sqrt(mean.Mean);
  float prob  = pdf->Eval(value);
  return like_t(kTruncMean|kInner,kX,mean.NPoints,prob,prob,mean.Mean);
}


int TrCharge::GetTruncMeanCharge(TrTrackR* track, float beta) {
  int bestz = -1;
  float maxprob = 0;
  for (int z=1; z<=MaxCharge; z++) {
    float prob = GetTruncMeanProbToBeZ(track,z,beta).LogLike;
    if (prob>maxprob) bestz = z;
  } 
  return bestz;
}


float TrCharge::GetQ(TrTrackR* track, int iside, float beta) {
  // To be keep updated
  mean_t mean = GetMean(kTruncMean,track,iside,beta,-1,TrClusterR::DefaultMipCorrOpt);
  return mean.GetQ();
}


like_t TrCharge::GetLogLikelihoodToBeZ(int type, TrTrackR* track, int iside, int Z, float beta) {
  like_t likelihood;
  for (int ihit=0; ihit<track->GetNhits(); ihit++) {
    TrRecHitR* hit = track->GetHit(ihit);
    if (hit==0) {
      printf("TrCharge::GetLogLikelihoodToBeZ-W hit in the track with NULL pointer, skipping it.\n");
      continue;
    }
    // requested inner
    int LayerJ = hit->GetLayerJ();
    if ( (type&kInner!=0)&&( (LayerJ==1)||(LayerJ==9) ) ) continue;
    // if hit analysis
    if ( (iside>1)&&(!GoodChargeReconHit(hit)) ) continue;
    // if cluster analysis
    TrClusterR* cluster = (iside==0) ? hit->GetXCluster() : hit->GetYCluster();
    if ( (iside<=1)&&(!GoodChargeReconCluster(cluster)) ) continue;
    // calculate probability 
    float logprob = GetProbToBeZ(hit,iside,Z,beta);
    
    likelihood.LogLike += logprob;
    // vector for the mean ... 
  }
  return likelihood;
}


