#include "TrCharge.h"

// Histogramming stuff
bool     TrCharge::EnabledHistograms = TRCHAFFKEY.EnableHisto;
HistoMan TrCharge::Histograms;


float TrCharge::GetBetaFromRigidity(float rigidity, int Z, float mass) {
  return 1./sqrt(1.-pow(1.*mass/(rigidity*Z),2) ); 
}


double TrCharge::GetProbToBeZ(TrRecHitR* hit, int iside, int Z, float beta) {
  // pointer check
  if (hit==0) {
    printf("TrCharge::GetPropToBeZ-W hit with NULL pointer, returning 0\n");
    return 0; 
  }
  // Z = 0 means electrons (approximately no rise)
  if (Z==0) { Z = 1; beta = 1; }
  // choose the pdf version 
  switch (TRCHAFFKEY.PdfVersion) { 
    case 1: { 
      TrPdf* pdf = TrPdfDB::GetHead()->GetPdf(Z,iside,TrPdfDB::kPdf01_SingleLayer);
      if (pdf==0) {
        // this frequently happens ... maybe is better to put a counter
        // printf("TrCharge::GetProbToBeZ-W requesting a not-existing pdf for a single measurement (iside=%d, Z=%d, version=%d), returning 0.\n",
        //   iside,Z,TRCHAFFKEY.PdfVersion);
        return 0;
      }
      // in this version the pdf are in sqrt(ADC) scale
      double signal = hit->GetSignalCombination(iside,TrClusterR::DefaultCorrOpt|TrClusterR::kBeta,beta);
      return pdf->Eval(sqrt(signal));
      break;
    }
    case 2: {
      TrPdf* pdf = TrPdfDB::GetHead()->GetPdf(Z,iside,TrPdfDB::kPdf02_SingleLayer,hit->GetLayerJ());
      // TMP (MAYBE BETTER)
      // TrPdf* pdf = (Z<2) ? TrPdfDB::GetHead()->GetPdf(1,iside,TrPdfDB::kPdf02_SingleLayer,hit->GetLayerJ()) : 
      //                      TrPdfDB::GetHead()->GetPdf(2,iside,TrPdfDB::kPdf02_SingleLayer,hit->GetLayerJ()) ;
      // TrPdf* pdf = TrPdfDB::GetHead()->GetPdf(2,iside,TrPdfDB::kPdf02_SingleLayer,hit->GetLayerJ());
      // TAKING JUST ONE PDF FOR EACH CHARGE AND EACH LAYER ... SEEMS BETTER!!!
      // TrPdf* pdf = TrPdfDB::GetHead()->GetPdf(2,iside,TrPdfDB::kPdf02_SingleLayer,2);
      if (pdf==0) {
        // this frequently happens ... maybe is better to put a counter
        // printf("TrCharge::GetProbToBeZ-W requesting a not-existing pdf for a single measurement (iside=%d, Z=%d, version=%d, layer=%d), returning 0.\n",
        //    iside,Z,TRCHAFFKEY.PdfVersion,hit->GetLayerJ());
        return 0;
      }
      // in this version the pdf are in ADC/Z^2 scale
      double signal = hit->GetSignalCombination(iside,TrClusterR::DefaultCorrOpt|TrClusterR::kBeta,beta);
      return pdf->Eval(signal/pow(Z,2.),false,true); // interpolation x-logy (better for tails)  
      break;
    }
    default: {
      printf("TrCharge::GetProbToBeZ-W requesting a not-existing pdf version for a single measurement (iside=%d, Z=%d, version=%d), returning 0.\n",
        iside,Z,TRCHAFFKEY.PdfVersion);
      return 0;
      break;
    }
  }
  // default return 
  return 0;
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
  return (!dead)&&(!bad)&&(!badshape);
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
  return mean_t(kPlainMean,-1,-1,n,mean,rms);
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
  return mean_t(kTruncMean,-1,-1,n-1,mean,rms);
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
  return mean_t(kGaussMean,-1,-1,gaussn,gaussmean,gaussrms);
}


mean_t TrCharge::GetMean(int type, TrTrackR* track, int iside, float beta, int layerj, int opt, int fit_id, float mass_on_Z) {
  // check
  if (track==0) {
    printf("TrCharge::GetMean-W track with NULL pointer, return empty mean_t.\n");
    return mean_t();
  }

  // rigidity
//  cout << "rigidity...fit_id=" << fit_id << flush;
  float rigidity = track->GetRigidity(fit_id);
 // cout << "rigidity=" << rigidity << endl;

  // track hit loop
  vector<float> signal;
  for (int ihit=0; ihit<track->GetNhits(); ihit++) {
    TrRecHitR* hit = track->GetHit(ihit);
    if (hit==0) {
      printf("TrCharge::GetMean-W hit in the track with NULL pointer, skipping it.\n");
      continue;
    }

    // requested configuration (upper/inner/lower)
    int LayerJ = hit->GetLayerJ();
    if ( !( ((type&kInner)&&(LayerJ>1)&&(LayerJ<9))||
            ((type&kLower)&&(LayerJ==9))||
            ((type&kUpper)&&(LayerJ==1)) )
    ) continue;
    // if excluded layer 
    if (LayerJ==layerj) continue;

    // selection
    // - if hit analysis
    if ( (iside>1)&&(!GoodChargeReconHit(hit)) ) continue;
    // - if cluster analysis
    TrClusterR* cluster = (iside==0) ? hit->GetXCluster() : hit->GetYCluster();
    if ( (iside<=1)&&(!GoodChargeReconCluster(cluster)) ) continue;

    // add signal to the vector 
    float asignal = hit->GetSignalCombination(iside,opt,beta,rigidity,mass_on_Z); 
    if (type&kSqrt) asignal = (asignal>0) ? sqrt(asignal) : 0;
    signal.push_back(asignal);
  }
  //cout << "signal size=" << signal.size() << endl;

  // computation
  mean_t mean = GetMean(type,signal);

  // set additional infos
  mean.Type = type;
  mean.Opt  = opt;
  mean.Side = iside;

  return mean;
}   


like_t TrCharge::GetTruncMeanProbToBeZ(TrTrackR* track, int Z, float beta) { 
  // Z=0 means electrons (approximately no rise)
  if (Z==0) { Z = 1; beta = 1; }
  /* calculating this mean every time: TO BE FIXED */
  // Take the corresponding PDF
  switch (TRCHAFFKEY.PdfVersion) {
    case 1: {
      // in this PDF version scale is sqrt(ADC) 
      int    type = kTruncMean|kInner|kSqrt;
      int    opt  = TrClusterR::DefaultCorrOpt|TrClusterR::kBeta;
      mean_t mean = GetMean(type,track,kX,beta,-1,opt);
      TrPdf* pdf = TrPdfDB::GetHead()->GetPdf(Z,kX,TrPdfDB::kPdf01_TruncatedMean);
      if (pdf==0) {
        // printf("TrCharge::GetTruncMeanProbToBeZ-W requesting a not-existing pdf (Z=%d,type=%d), returning empty like_t.\n",
        //       Z,TrPdfDB::kPdf01_TruncatedMean);
        return like_t();
      }
      // Evaluate
      float value = mean.Mean;
      float prob  = pdf->Eval(value);
      return like_t(type,opt,kX,Z,mean.NPoints,prob,prob,mean.Mean);
      break;
    }
    default: {
      printf("TrCharge::GetTruncMeanProbToBeZ-W requesting a not-existing pdf version (version=%d), returning empty like_t.\n",TRCHAFFKEY.PdfVersion);
      return like_t(); 
      break;
    }
  }
  return like_t();
}


int TrCharge::GetTruncMeanCharge(TrTrackR* track, float beta) {
  // if the probability of electron/proton is exactly the same return proton
  int bestz = -1;
  float maxprob = 0;
  for (int z=0; z<=TRCHAFFKEY.MaxCharge; z++) {
    float prob = GetTruncMeanProbToBeZ(track,z,beta).LogLike;
    if (prob>=maxprob) {
      bestz = z; 
      maxprob = prob; 
    }
  } 
  return bestz;
}


float TrCharge::GetQ(TrTrackR* track, int iside, float beta) {  
  mean_t mean = GetMean(kTruncMean|kInner|kSqrt,track,iside,beta,-1,TrClusterR::DefaultChargeCorrOpt);
  if ( (iside==kX)||(iside==kY) ) return mean.Mean;
  // temporary solution (January 2012) for mixing the two measurements
  if (iside==kXY) {
    float q_x = GetMean(kTruncMean|kInner|kSqrt,track,kX,beta,-1,TrClusterR::DefaultChargeCorrOpt).Mean;
    float q_y = GetMean(kTruncMean|kInner|kSqrt,track,kY,beta,-1,TrClusterR::DefaultChargeCorrOpt).Mean;
    // first simple guess
    float q = q_x;
    if ( (q>12.5)&&(q_y>0.) ) q = q_y;
    for (int iiter=0; iiter<1; iiter++) {
      // get sigma (parametrization from preliminary charge analysis)
      float sigma_x = 0.0958899 - 0.0232748*q + 0.00753501*q*q;
      float sigma_y = 0.135193 + 0.0646057*q;
      // evaluate weights
      float w_x = 1./(sigma_x*sigma_x)/(1./(sigma_x*sigma_x) + 1./(sigma_y*sigma_y));
      float w_y = 1./(sigma_y*sigma_y)/(1./(sigma_x*sigma_x) + 1./(sigma_y*sigma_y));
      // update     
      q = q_x*w_x+ q_y*w_y;
    }
    return q;
  }
  return -1;
}


like_t TrCharge::GetLogLikelihoodToBeZ(int type, TrTrackR* track, int iside, int Z, float beta) {
  // check
  if (track==0) {
    printf("TrCharge::GetLogLikelihoodToBeZ-W track with NULL pointer, return empty mean_t.\n");
    return like_t();
  }
  // loop
  like_t likelihood;
  for (int ihit=0; ihit<track->GetNhits(); ihit++) {
    TrRecHitR* hit = track->GetHit(ihit);
    if (hit==0) {
      printf("TrCharge::GetLogLikelihoodToBeZ-W hit in the track with NULL pointer, skipping it.\n");
      continue;
    }
    // requested configuration (upper/inner/lower)
    int LayerJ = hit->GetLayerJ();
    if ( !( ((type&kInner)&&(LayerJ>1)&&(LayerJ<9))||
            ((type&kLower)&&(LayerJ==9))||
            ((type&kUpper)&&(LayerJ==1)) )
    ) continue;
    /* could be useful ... 
       if excluded layedd 
       if (LayerJ==layerj) continue; */
    // selection
    // - if hit analysis
    if ( (iside>1)&&(!GoodChargeReconHit(hit)) ) continue;
    // - if cluster analysis
    TrClusterR* cluster = (iside==0) ? hit->GetXCluster() : hit->GetYCluster();
    if ( (iside<=1)&&(!GoodChargeReconCluster(cluster)) ) continue;
    // calculate probability 
    double prob = GetProbToBeZ(hit,iside,Z,beta);
    double logprob = (prob>1e-300) ? log10(prob) : -300; // double minimum 
    /*
    cout << ihit << " " << hit->GetLayerJ() << " "  
         << hit->GetSignalCombination(iside,TrClusterR::DefaultCorrOpt|TrClusterR::kBeta,beta) << " " 
         << hit->GetSignalCombination(iside,TrClusterR::DefaultCorrOpt|TrClusterR::kBeta,beta)/Z/Z << " " 
         << Z << " " << " " << prob << endl;
    */
    likelihood.NPoints++;
    likelihood.LogLike += logprob;
    likelihood.Mean = hit->GetSignalCombination(iside,TrClusterR::DefaultChargeCorrOpt,beta);
  }
  // calculation and update exit values
  likelihood.Side = iside;
  likelihood.Z    = Z;
  likelihood.Ver  = TRCHAFFKEY.PdfVersion;
  likelihood.Type = type;
  likelihood.Mean = likelihood.Mean/likelihood.NPoints; 
  return likelihood;
}


like_t TrCharge::GetLogLikelihoodCharge(int type, TrTrackR* track, int iside, float beta) {
  if (track==0) {
    printf("TrCharge::GetLogLikelihoodCharge-W NULL track pointer. Return an empty like_t.\n");
    return like_t();
  }
  // if the probability of electron/proton is exactly the same return proton
  like_t bestlikelihood;
  for (int z=0; z<=TRCHAFFKEY.MaxCharge; z++) {
    like_t likelihood = GetLogLikelihoodToBeZ(type,track,iside,z,beta); 
    if (likelihood>=bestlikelihood) {
      bestlikelihood = likelihood;
    }	
  }
  return bestlikelihood;
}


mean_t TrCharge::GetMean(int type, vector<float> signal) {
  if      ( ( (type&kPlainMean))&&(!(type&kTruncMean))&&(!(type&kGaussMean)) ) return GetPlainMean(signal);
  else if ( (!(type&kPlainMean))&&( (type&kTruncMean))&&(!(type&kGaussMean)) ) return GetTruncMean(signal);
  else if ( (!(type&kPlainMean))&&(!(type&kTruncMean))&&( (type&kGaussMean)) ) return GetGaussMean(signal);
  else    printf("TrCharge::GetMean-W not a valid mean type selected, returning empty mean_t, %2x.\n",type);
  return mean_t();
}


mean_t TrCharge::GetMeanHighestFourClusters(int type, int iside, int opt) {
  // TrCluster container
  VCon* cont = GetVCon()->GetCont("AMSTrCluster");
  if (!cont) {
    printf("TrCharge::GetMeanHighestFourClusters-W no AMSTrCluster container available. Return empty mean_t.\n");
    return mean_t();
  }
  // Highest clusters per layer
  TrClusterR* max_clu[9] = {0,0,0,0,0,0,0,0,0};
  float       max_sig[9] = {0,0,0,0,0,0,0,0,0};
  for (int icont = 0; icont<cont->getnelem(); icont++){
    TrClusterR* cluster = (TrClusterR*) cont->getelem(icont);
    if (cluster==0) {
      printf("TrCharge::GetMeanHighestFourClusters-W invalid TrClusterR pointer. Skipping.\n");                    
      continue;
    }
    int   jside   = cluster->GetSide();
    if (jside!=iside) continue;
    int   jlayer = cluster->GetLayerJ() - 1;
    float signal = cluster->GetTotSignal(opt);
    if (type&kSqrt) signal = (signal>0) ? sqrt(signal) : 0;
    if (signal>max_sig[jlayer]) {
      max_sig[jlayer] = signal;
      max_clu[jlayer] = cluster;
    }
  }
  if (cont) delete cont;
  // order signals
  vector<float> signal;
  for (int jlayer=0; jlayer<9; jlayer++) {
    if (max_clu[jlayer]!=0) signal.push_back(max_sig[jlayer]);
  }
  if (int(signal.size()<4)) return mean_t();
  sort(signal.begin(),signal.end());
  reverse(signal.begin(),signal.end());
  signal.erase(signal.begin()+4,signal.end());
  mean_t result = GetMean(type,signal);
  // monitoring plots
  if (EnabledHistograms) {
    if (iside==0) {
      if (!Histograms.Get("HighFourMean_RMS_vs_Mean_X")) Histograms.Add(new TH2F("HighFourMean_RMS_vs_Mean_X","; Mean; RMS",500,0,5000,200,0,1000));
      if (Histograms.Get("HighFourMean_RMS_vs_Mean_X"))  Histograms.Fill("HighFourMean_RMS_vs_Mean_X",result.Mean,result.RMS);
    }
    else {
      if (!Histograms.Get("HighFourMean_RMS_vs_Mean_Y")) Histograms.Add(new TH2F("HighFourMean_RMS_vs_Mean_Y","; Mean; RMS",500,0,5000,200,0,1000));
      if (Histograms.Get("HighFourMean_RMS_vs_Mean_Y"))  Histograms.Fill("HighFourMean_RMS_vs_Mean_Y",result.Mean,result.RMS);
    }
  }
  // update infos
  result.Type = type;
  result.Opt  = opt;
  result.Side = iside;
  return result;
}

