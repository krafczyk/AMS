#include "TrCharge.h"


// Histogramming stuff
bool     TrCharge::EnabledHistograms = TRCHAFFKEY.EnableHisto;
HistoMan TrCharge::Histograms;


// Clean members
void TrCharge::Clear() {
  PattType = -1;
  for(int ll=0; ll<9; ll++) {
    ClsCat[ll] = -1000;
    for(int ss=0; ss<2; ss++) {
      ClsSig[ss][ll]= -1.; // means not stored
    }
  }
}


////////////////////////////////////////////
// Floating point charge estimator 
// A. Oliva and P. Saouter 2012    
////////////////////////////////////////////


bool TrCharge::GoodChargeReconHit(TrRecHitR* hit, int iside) {
  // pointer check
  if (hit==0) return false;
  TrClusterR* clx = hit->GetXCluster();
  TrClusterR* cly = hit->GetYCluster();
  // no one of the two
  if ( (!clx)&&(!cly) ) return false;
  // requested side exists
  if ( (iside==0)&&(!clx) ) return false;
  if ( (iside==1)&&(!cly) ) return false;
  // existence of the clusters is taken into account, as well as multiplicity, in the TrRecHit::GetQStatus method
  int status = hit->GetQStatus(); 
  if      (iside==0) return ((status&0x1001FD)==0);
  else if (iside==1) return ((status&0x1FD100)==0);
  else               return ((status&0x1FD1FD)==0);
  return true;
}


mean_t TrCharge::GetMean(int type, vector<float> signal) {
  if      ( ( (type&kPlainMean))&&(!(type&kTruncMean))&&(!(type&kGaussMean)) ) return GetPlainMean(signal);
  else if ( (!(type&kPlainMean))&&( (type&kTruncMean))&&(!(type&kGaussMean)) ) return GetTruncMean(signal);
  else if ( (!(type&kPlainMean))&&(!(type&kTruncMean))&&( (type&kGaussMean)) ) return GetGaussMean(signal);
  else    printf("TrCharge::GetMean-W not a valid mean type selected, returning empty mean_t, %2x.\n",type);
  return mean_t();
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
  rms  = sqrt(fabs(rms - mean*mean)); // avoids problem in case of zeroes    
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
  float rigidity = (fit_id>=0) ? track->GetRigidity(fit_id) : 0;
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
    if ( !( ((type&kInner)&&(LayerJ>1)&&(LayerJ<9))||((type&kLower)&&(LayerJ==9))||((type&kUpper)&&(LayerJ==1)) ) ) continue;
    // if excluded layer 
    if (LayerJ==layerj) continue;
    // selection
    if (!GoodChargeReconHit(hit,iside)) continue;   
    // add signal to the vector 
    float asignal = hit->GetSignalCombination(iside,opt,beta,rigidity,mass_on_Z);
    if (type&kSqrt) asignal = (asignal>0) ? sqrt(asignal) : 0;
    signal.push_back(asignal);
  }
  // computation
  mean_t mean = GetMean(type,signal);
  // set additional infos
  mean.Type = type;
  mean.Opt  = opt;
  mean.Side = iside;
  return mean;
}


// Weight factors for combined truncated mean 
// - 2012 calibration: 
// Database of sigma values come from truncated mean (very rough algorithm) 
static float z_sigma_trunc[14]   = {1,2,3,4,5,6,7,8,9,10,12,14,16,26};
static float sigma_trunc_x_6[14] = {0.0598,  0.0762,  0.0881,  0.0947,  0.1091,  0.1303,  0.1357,  0.1721,  0.2264,  0.2467,  0.3958,  0.5622,  1.1560,  0.7984};
static float sigma_trunc_y_6[14] = {0.0618,  0.0798,  0.0950,  0.1361,  0.2534,  0.5171,  1.2854,  0.8704,  0.7563,  0.4421,  0.4702,  0.5122,  1.0341,  0.9487};
static TGraph* sigma_trunc_spline[2] = {0,0};
// Number of points from 0 to 8 (truncated mean is nmax-1, 0 if impossible or undetermined) 
// - assumed to be independent from Z (approximately true)
// - extrapolated for n_x = 1 and for for n_y = 1, 2 
static float sigma_scale_x[9] = {0., 1.6777, 1.4973,  1.2724,  1.1441,  1.0612,  1.0000,  0.9658,  0.9162};
static float sigma_scale_y[9] = {0., 1.4584, 1.3345,  1.2285,  1.1301,  1.0629,  1.0000,  0.9569,  0.9291};
// - 2013 calibration 
static double z_sigma_trunc_2013[14] = {1,2,3,4,5,6,7,8,10,12,14,16,20,26}; 
static double sigma_trunc_x_6_2013[14] = {
  0.0582148,0.0767001,0.0869924,0.0945005,0.100936,0.108567,0.122822,
  0.134392,0.209511,0.315384,0.42982,0.531301,0.61243,0.677021
};
static double sigma_trunc_y_6_2013[14] = {
  0.0590583,0.0784018,0.0970621,0.153183,0.298174,0.422098,0.629828,
  0.472116,0.406461,0.390868,0.395945,0.490878,0.641071,0.653275  
};
mean_t TrCharge::GetCombinedMean(int type, TrTrackR* track, float beta, int jlayer, int opt, int fit_id, float mass_on_Z) {
  // check
  // if the requested type does not require TruncatedMean is not valid 
  if (!(type&kTruncMean)) return 0;
  // if the requested type is not kSqrt is not valid
  if (!(type&kSqrt)) return 0;
  // init spline if needed
  if (!sigma_trunc_spline[0]) sigma_trunc_spline[0] = new TGraph(14,z_sigma_trunc,sigma_trunc_x_6);
  if (!sigma_trunc_spline[1]) sigma_trunc_spline[1] = new TGraph(14,z_sigma_trunc,sigma_trunc_y_6);
  // calculate truncated mean
  mean_t mean_x = TrCharge::GetMean(type,track,TrCharge::kX,beta,jlayer,opt,fit_id,mass_on_Z);
  mean_t mean_y = TrCharge::GetMean(type,track,TrCharge::kY,beta,jlayer,opt,fit_id,mass_on_Z);
  float q_x = mean_x.Mean;
  float q_y = mean_y.Mean;
  int   n_x = mean_x.NPoints;
  int   n_y = mean_y.NPoints;
  float r_x = (n_x>0) ? mean_x.RMS : 0;
  float r_y = (n_y>0) ? mean_y.RMS : 0;
  // normalization at the number of points  
  // in the old version I was determining factors from data
  float factor_x = ( (n_x>0)&&(n_x<9) ) ? sigma_scale_x[n_x] : 0;
  float factor_y = ( (n_y>0)&&(n_y<9) ) ? sigma_scale_y[n_y] : 0;
  if (!(opt&TrClusterR::kOld)) {
    // in new version I use just point normalization 
    factor_x = sqrt(1.*n_x)/sqrt(6);
    factor_y = sqrt(1.*n_y)/sqrt(6);
  }
  // make a first approximate charge guess: is X, if existing, otherwise Y
  float q_pre = (q_x>0) ? q_x : q_y;
  // very rough integer charge estimator
  int   z_pre = floor(q_pre + 0.5);
  if (z_pre<1) z_pre = 1;
  // now do the combination
  // old calibration used the sline 
  float sigma_x = sigma_trunc_spline[0]->Eval(z_pre);
  float sigma_y = sigma_trunc_spline[1]->Eval(z_pre);
  if (!(opt&TrClusterR::kOld)) {
    // new calibration uses the monotonic spline 
    sigma_x = monotonic_cubic_interpolation(1.*z_pre,14,z_sigma_trunc_2013,sigma_trunc_x_6_2013);
    sigma_y = monotonic_cubic_interpolation(1.*z_pre,14,z_sigma_trunc_2013,sigma_trunc_y_6_2013);
  }
  float w_x = (sigma_x*factor_x>0) ? 1.0/pow(sigma_x*factor_x,2) : 0;
  float w_y = (sigma_y*factor_y>0) ? 1.0/pow(sigma_y*factor_y,2) : 0;
  float q_glb = ((w_x + w_y)>0) ? (q_x*w_x + q_y*w_y)/(w_x + w_y) : 0;
  // calculate the RMS combination from error propagation 
  float q_rms = ((w_x + w_y)>0) ? (r_x*w_x + r_y*w_y)/(w_x + w_y) : 0;
  // result
  return mean_t(type,opt,-1,n_x+n_y,q_glb,q_rms);
}


//////////////////////////////////////////////
// Integer charge estimator related methods  
// A. Oliva and N. Tomassetti 2013
//////////////////////////////////////////////


// Remark
//   almost equivalent to TrCharge::GoodChargeReconHit
//   a part the request of being "gold". Maybe too strict.
bool TrCharge::GoodHitForLogProb(TrRecHitR* hit, int iside) {
  // pointer check
  if (hit==0) return false;
  iside = iside%3;
  // cluster exist
  TrClusterR* cls;
  if (iside==0) cls = hit->GetXCluster();
  if (iside==1) cls = hit->GetYCluster();
  if (!cls) return false;
  // good va
  int iVA  = (int)(cls->GetAddress()/64);
  bool isgold = (bool) TrGainDB::GetHead()->FindGainTkId(cls->GetTkId())->IsGold(iVA);
  if (!isgold) return false;
  // quality
  int Mult = hit->GetResolvedMultiplicity();
  if ( (cls->GetQStatus(1,Mult)&0x1FD)!=0 ) return false;
  // other-side cluster
  TrClusterR* clo;
  if (iside==0) clo = hit->GetYCluster();
  if (iside==1) clo = hit->GetXCluster();
  if ( (clo) && (clo->GetQStatus(1,Mult)&0x100)!=0 ) return false;
  return true;
}


// Remark: check the mass_on_Z choice 
bool TrCharge::StoreClusterInfo(TrTrackR* track, int type, int iside, double beta, double rigidity, double mass_on_Z) {
  // reset 
  Clear();     
  // check pointer
  if (track==0) return false; 
  // init
  double qtot_x= 0.;
  double qtot_y= 0.;
  int default_opt = TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kAngle|TrClusterR::kMIP|TrClusterR::kLoss|TrClusterR::kBeta|TrClusterR::kRigidity;
  // loop on hits
  for (int ihit=0; ihit<track->GetNhits(); ihit++) {
    TrRecHitR* hit = track->GetHit(ihit);
    if(!hit) continue;
    int LayerJ = hit->GetLayerJ();
    // store only the requested configuration 
    if ( (type<8)&&(!( ((type&kInner)&&(LayerJ>1)&&(LayerJ<9))||((type&kLower)&&(LayerJ==9))||((type&kUpper)&&(LayerJ==1)))) ) continue;
    if ( (type>100)&&((type%100)!=LayerJ) ) continue;
    // store
    ClsCat[LayerJ-1]= 0;
    if ( (hit->GetXCluster())&&(!hit->GetXCluster()->IsK7()) ) ClsCat[LayerJ-1]= 1;
    // use 2D hits when available - 1D otherwise
    if(iside==2) {
      if ( (GoodHitForLogProb(hit,0))&&(GoodHitForLogProb(hit,1)) ) {
        ClsSig[0][LayerJ-1] = sqrt(hit->GetSignalCombination(0,default_opt,beta,rigidity,mass_on_Z));
        ClsSig[1][LayerJ-1] = sqrt(hit->GetSignalCombination(1,default_opt,beta,rigidity,mass_on_Z));
      }
      else {
        if (GoodHitForLogProb(hit,0)) { // x-side 1D
          ClsSig[0][LayerJ-1]= sqrt(hit->GetSignalCombination(0,default_opt,beta,rigidity,mass_on_Z));
        }
        if (GoodHitForLogProb(hit,1)) { // y-side 1D
          ClsSig[1][LayerJ-1]= sqrt(hit->GetSignalCombination(1,default_opt,beta,rigidity,mass_on_Z));
        }
      }
    }
    // only one side used
    if (iside==0) {
      if (GoodHitForLogProb(hit,0)) { // x-side 1D
        ClsSig[0][LayerJ-1]= sqrt(hit->GetSignalCombination(0,default_opt,beta,rigidity,mass_on_Z));
      }
    }
    if (iside==1) {
      if (GoodHitForLogProb(hit,1)) { // y-side 1D
        ClsSig[1][LayerJ-1]= sqrt(hit->GetSignalCombination(1,default_opt,beta,rigidity,mass_on_Z));
      }
    }
  }
  PattType = type;
  return true;
}


void TrCharge::PrintClusterInfo() {
  printf("TrCharge::PrintfClusterInfo-V:\n");
  for (int ss=0; ss<2; ss++) {
    for (int ll=0; ll<9; ll++) { 
      printf("%7.3f ",ClsSig[ss][ll]);
    }
    printf("\n");
  }
  for (int ll=0; ll<9; ll++) {
    printf("%7d ",ClsCat[ll]);
  }
  printf("\n");
}


double TrCharge::GetMeanCharge(double qtot_x, double qtot_y, int nq_x, int nq_y, int Z){
  // check
  if (Z<1) return -2.;
  if (nq_x<=0 && nq_y<=0) return -1.;
  if (nq_y<=0) return qtot_x/nq_x; // x-side only
  if (nq_x<=0) return qtot_y/nq_y; // y-side only
  // measured resolutions
  double sigma6_x[28]={0.0598, 0.0762, 0.0881, 0.0947, 0.1091, 0.1303, 0.1357,
                       0.1721, 0.2264, 0.2467, 0.32125, 0.3958, 0.479, 0.5622,
                       0.8591, 1.156, 1.12024, 1.08448, 1.04872, 1.01296, 0.9772,
                       0.94144, 0.90568, 0.86992, 0.83416, 0.7984, 0.7984, 0.7984};
  double sigma6_y[28]={0.0618, 0.0798, 0.095, 0.1361, 0.2534, 0.5171, 1.2854,
                       0.8704, 0.7563, 0.4421, 0.45615, 0.4702, 0.4912, 0.5122,
                       0.77315, 1.0341, 1.02556, 1.01702, 1.00848, 0.99994, 0.9914,
                       0.98286, 0.97432, 0.96578, 0.95724, 0.9487, 0.9487, 0.9487};
  // do something reasonable when out of range
  if (Z>=28) Z = 28;
  double sigma_x = sigma6_x[Z-1]*sqrt(6./nq_x);
  double sigma_y = sigma6_y[Z-1]*sqrt(6./nq_y);
  double mean_q = (qtot_x/nq_x/sigma_x/sigma_x + qtot_y/nq_y/sigma_y/sigma_y)/(1./sigma_x/sigma_x + 1./sigma_y/sigma_y);
  return mean_q;
}


like_t TrCharge::GetLogLikelihoodToBeZ(int Z, int iside){
  // check
  if (Z<1) return like_t(PattType,10,iside,Z,0,TrLikeDB::default_logprob,0,0);
  // init
  double likelihood = 0.;
  int npoints_x     = 0;
  int npoints_y     = 0;
  int npoints_xy    = 0;
  double qtot_x     = 0.;
  double qtot_y     = 0.;
  // loop 
  int  side = iside%3;
  for(int ll=0;ll<9;ll++){
    // cluster values
    double xi_x = ClsSig[0][ll]/Z;
    double xi_y = ClsSig[1][ll]/Z;
    int    icat = ClsCat[ll];
    double logprob = TrLikeDB::default_logprob;
    // fiducial intervals
    float MinXi=0.55;
    float MaxXi=3.0;
    if(Z>2)  MaxXi=2.3;
    if(Z>6)  MaxXi=1.8;
    if(Z>10) MaxXi=1.5;
    bool kX = (xi_x>MinXi && xi_x<MaxXi && icat>=0 && icat<=1);
    bool kY = (xi_y>MinXi && xi_y<MaxXi && icat>=0 && icat<=1);
    // switch
    if (side==2) { // LOOK AT BOTH SIDES
      if (kX && kY) { // 2D COMBINED SIGNALS
        logprob = -TrLikeDB::Interpolate(TrLikeDB::GetHead()->GetPdf(0,Z,icat,0,1),xi_x,xi_y); // 2D X vs Y 
        if(logprob>TrLikeDB::default_logprob){
          qtot_x += ClsSig[0][ll];
          qtot_y += ClsSig[1][ll];
          npoints_xy++;
          likelihood+= logprob;
        }
      }
      else{ // ONLY 1D SIGNAL
        if (kX){ // IT's X
          logprob = -TrLikeDB::Interpolate(TrLikeDB::GetHead()->GetPdf(1,Z,icat,0,1),xi_x); // 1D X
          if (logprob>TrLikeDB::default_logprob) {
            qtot_x += ClsSig[0][ll];
            npoints_x++;
            likelihood+= logprob;
          }
        }
        if (kY) {// IT's Y
          logprob = -TrLikeDB::Interpolate(TrLikeDB::GetHead()->GetPdf(1,Z,icat,1,1),xi_y); // 1D Y
          if (logprob>TrLikeDB::default_logprob) {
            qtot_y += ClsSig[1][ll];
            npoints_y++;
            likelihood+= logprob;
          }
        }
      }
    }
    if ((side==0) && kX) {
      logprob = -TrLikeDB::Interpolate(TrLikeDB::GetHead()->GetPdf(1,Z,icat,0,1),xi_x); // 1D X   
      if (logprob>TrLikeDB::default_logprob){ 
        qtot_x += ClsSig[0][ll];
        npoints_x++;
        likelihood+= logprob;
      }
    }
    if ((side==1) && kY) {
       logprob = -TrLikeDB::Interpolate(TrLikeDB::GetHead()->GetPdf(1,Z,icat,1,1),xi_y); // 1D Y 
      if (logprob>TrLikeDB::default_logprob) {
        qtot_y += ClsSig[1][ll];
        npoints_y++;
        likelihood+= logprob;
      }
    }
  }
  // eval mean floating charge q and likelihood dof
  int nq_x = npoints_x + npoints_xy;
  int nq_y = npoints_y + npoints_xy;
  double qmean = GetMeanCharge(qtot_x,qtot_y,nq_x,nq_y,Z);
  int npoints = npoints_x + npoints_y + 2*npoints_xy;
  return (npoints<1) ? 
    like_t(PattType,10,iside,Z,0,TrLikeDB::default_logprob,0,0) : 
    like_t(PattType,10,iside,Z,npoints,likelihood,pow(10,likelihood),qmean);
}


int TrCharge::GetZ(vector<like_t>& likelihood, TrTrackR* track, int type, int iside, double beta, double rigidity, double mass_on_Z) {

  // clean
  likelihood.clear();

  // check 
  if (!track) return -2;
  int Zbest = -1;
  int NPoints = 0;

  // min npoints required at first iteration
  int MinNP = 6;
  if (iside<2) MinNP = 5;
  if (type&kAll) MinNP++;
  if ( type&kLower || type&kUpper ) MinNP=1;
  if (type>8) MinNP=1;
  int NITER = MinNP;

  // store in memory relevant infos
  if (!StoreClusterInfo(track,type,iside,beta,rigidity,mass_on_Z)) return -3;

  // standard loop between 1-28, but extend to int(Q)+10 from max Q between inner or first tracker layer floating charge, max boundary is however 150.
  int Zinner = int(track->GetInnerQ(beta,0,mass_on_Z)+0.5);
  int Zl1 = int(track->GetLayerJQ(1,beta,0,mass_on_Z)+0.5);
  int Zmax = max(TrLikeDB::fZmax,min(150,10+max(Zinner,Zl1)));

  // iteration loop 
  for (int iter=0; iter<NITER; iter++) { // iteration loop (to avoid that lower number of points could dominate)

    // init before Z search 
    likelihood.clear();
    Zbest = -1;
    double LogLike = TrLikeDB::default_logprob;

    // Z search
    for (int Z=1; Z<=Zmax; Z++) {

      // calculate 
      like_t like = GetLogLikelihoodToBeZ(Z,iside);
      double logl = like.GetNormLogLike();
      int npoints = like.NPoints; 

      // drop too bad evaluations
      if ( (logl<=TrLikeDB::default_logprob)||(npoints<1) ) continue; 
      likelihood.push_back(like);

      if (like.NPoints<MinNP-iter) continue;
      if (logl>LogLike) {
        LogLike = logl;
        Zbest = like.Z;
      }

    }
    if(Zbest>0) break; // Z found
  } 

  // reorder
  sort(likelihood.begin(),likelihood.end()); 
  reverse(likelihood.begin(),likelihood.end());

  // consistency check 
  if ( (Zbest>0)&&(likelihood.empty()) ) printf("TrCharge::GetZ()-W good Z found but empty likelihood container. This is very strange. Go to check.\n");

  // end iteration loop
  return Zbest;
}


//////////////////////////////////////////////
// Old methods for floating point charge and integer charge estimators 
// A. Oliva and P. Saouter 2011
//////////////////////////////////////////////


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
      if (pdf==0) {
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
    if (!GoodChargeReconHit(hit,iside)) continue;
    // calculate probability 
    double prob = GetProbToBeZ(hit,iside,Z,beta);
    double logprob = (prob>1e-300) ? log10(prob) : -300; // double minimum 
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


//////////////////////////////////////////////
// Charge-related methods for reconstruction
// A. Oliva 2012   
//////////////////////////////////////////////


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

