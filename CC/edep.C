// $Id: edep.C,v 1.4 2012/04/21 09:29:29 oliva Exp $

#include "edep.h"


ClassImp(AMSEnergyLoss);


/* 
// From PDG                   g/cm2   <Z/A>    eV     x0     x1   Cbar       a  k (m)   d0  
Double_t pars_Si[9]   = {0.03*1.330,0.49848,173.0,0.2014,2.8715,4.4351,0.14921,3.2546,0.14}; // 300 um of Si
Double_t pars_Poly[9] = {   1*1.032,0.54141, 64.7,0.1464,2.4855,3.1997,0.16101,3.2393,0.00}; // 1 cm of Polyvinyl. 
*/

// From a fit on proton data: delta0 derived from function match
//
//                          g/cm2   <Z/A>        eV      x0      x1    Cbar       a   k (m)     MIP      
Double_t pars_Si[9]   = {0.06990,0.49848,173.00000,0.20140,4.05905,4.56882,0.00121,5.93325,0.70847};
Double_t pars_Poly[9] = {1.03200,0.54141, 64.70000,0.14640,0.73322,4.72874,6.47414,1.89853,2.00000}; // no MIP


Double_t AMSEnergyLoss::csi_logbetagamma(Double_t* x, Double_t* par) {
  // x[0] is log10(betagamma) 
  Double_t eta       = x[0];
  Double_t betagamma = pow(10.,eta);
  Double_t beta      = sqrt(1./(1./betagamma/betagamma + 1.));
  Double_t K         = 0.307075; // MeV
  Double_t grammage  = par[0];   // g/cm2
  Double_t Z_on_A    = par[1];   // <Z/A>
  return 0.5*K*Z_on_A*grammage/beta/beta; // MeV
}

Double_t AMSEnergyLoss::density_correction_logbetagamma(Double_t* x, Double_t* par) {
  // x[0] is log10(betagamma)
  Double_t eta    = x[0];
  Double_t x0     = par[0];
  Double_t x1     = par[1];
  Double_t Cbar   = par[2];
  Double_t a      = par[3];
  Double_t k      = par[4];
  Double_t delta0 = par[5];
  if      (eta>=x1) return 2*log(10)*eta - Cbar;
  else if (eta>=x0) return 2*log(10)*eta - Cbar + a*pow(x1-eta,k);
  else              return (2*log(10)*x0 - Cbar + a*pow(x1-x0,k))*pow(10.,2*(eta-x0)); // delta0*pow(10.,2*(eta-x0));
}

Double_t AMSEnergyLoss::delta_logbetagamma(Double_t *x, Double_t *par) {
  // Most Probable Value for Energy Deposition
  Double_t grammage  = par[0]; // g/cm2
  Double_t Z_on_A    = par[1]; // <Z/A>
  Double_t I         = par[2]; // eV
  Double_t x0        = par[3]; // density correction
  Double_t x1        = par[4]; // density correction
  Double_t Cbar      = par[5]; // density correction
  Double_t a         = par[6]; // density correction
  Double_t k         = par[7]; // density correction
  // Double_t delta0    = par[8]; // density correction
  // x is log10(betagamma) 
  Double_t eta       = x[0]; 
  Double_t betagamma = pow(10.,eta);
  Double_t beta      = sqrt(1./(1./betagamma/betagamma + 1.));
  // csi 
  Double_t csi_par[2] = {grammage,Z_on_A};
  Double_t csi        = csi_logbetagamma(&eta,csi_par);
  // density correction
  Double_t density_correction_par[] = {x0,x1,Cbar,a,k}; // ,delta0};
  Double_t density_correction       = density_correction_logbetagamma(&eta,density_correction_par);
  // everything together 
  Double_t term1 = log(2*0.511*1e6/I) + 2*log(betagamma) - density_correction;
  Double_t term2 = log(csi*1e6/I); 
  Double_t term3 = 0.2 - beta*beta;
  Double_t delta = csi*(term1 + term2 + term3);
  return 1e+3*delta; // keV
}

Double_t AMSEnergyLoss::delta_beta(Double_t *x, Double_t *par) {
  // x[0] is beta
  Double_t beta         = x[0];
  Double_t logbetagamma = log10(beta/sqrt(1-beta*beta));
  return delta_logbetagamma(&logbetagamma,par);
}

Double_t AMSEnergyLoss::delta_beta_fit(Double_t *x, Double_t *par) {
  Double_t beta = x[0];
  if (beta<par[9]) return par[10]*delta_beta(&beta,par);
  else             return par[10]*delta_beta(&par[9],par);
}

Double_t AMSEnergyLoss::beta_tol_on_beta(Double_t *x, Double_t *par) {
  Double_t xx = x[0]; 
  Double_t x0 = par[0];
  Double_t x1 = par[1]; 
  if (xx>x0) return 1;
  Double_t e = par[4];
  Double_t d = par[3];
  Double_t c = par[2];
  Double_t b = -2*c*x0-3*d*pow(x0,2)-4*e*pow(x0,3);
  Double_t a = 1-b*x0-c*pow(x0,2)-d*pow(x0,3)-e*pow(x0,4);
  if (xx>x1) return a+b*xx+c*pow(xx,2)+d*pow(xx,3)+e*pow(xx,4);
  Double_t m = b+2*c*x1+3*d*pow(x1,2)+4*e*pow(x1,3);
  Double_t q = -m*x1+a+b*x1+c*pow(x1,2)+d*pow(x1,3)+e*pow(x1,4);
  return m*xx + q;
}


Double_t AMSEnergyLoss::GetMostProbEDepIn300umSi(Double_t logbetagamma) {
  return delta_logbetagamma(&logbetagamma,pars_Si); 
}


Double_t AMSEnergyLoss::GetMostProbEDepIn1cmPoly(Double_t logbetagamma) {
  return delta_logbetagamma(&logbetagamma,pars_Poly);
}


static Double_t beta_tol_on_beta_pars_trk[9][5] = {
  {  0.9350,  0.3450, 37.6902,-33.5698, 11.1962},
  {  0.9350,  0.3450,  1.4487, -0.0270, -0.3583},
  {  0.9350,  0.3450,  1.1215, -0.0188, -0.2737},
  {  0.9350,  0.3450,  0.8643, -0.0130, -0.2124},
  {  0.9350,  0.3450,  0.6114, -0.0080, -0.1447},
  {  0.9350,  0.3450,  0.5071, -0.0078, -0.1264},
  {  0.9350,  0.3450,  0.8398, -0.0198, -0.2140},
  {  0.9350,  0.3450,  0.4033, -0.0170, -0.1116},
  {  0.9350,  0.4150, -4.9400,  0.0182,  1.0674},
};
Double_t AMSEnergyLoss::GetBetaTopOfTrackerLayer(Int_t jlayer, Double_t beta) {
  //////// MISSING NEGATIVE BETA TREATMENT ///////// 
  // check
  if ( (jlayer<1)||(jlayer>9) ) return 0;
  if (beta<0) return 0;
  if (beta>1) return beta;
  Double_t beta_tol = beta*beta_tol_on_beta(&beta,beta_tol_on_beta_pars_trk[jlayer-1]);
  if (beta_tol<0) return 0;
  return beta_tol;
}


// charge one parameters to normalize theoric dE/dx to MIP^2 scale
Double_t AMSEnergyLoss::GetBetaCorrectionTrackerLayer(Int_t jlayer, Double_t beta) {
  if ( (jlayer<1)||(jlayer>9) ) return 0;
  // after beta correction to the top of tracker layer the correction becomes universal
  Double_t beta_tol = fabs(GetBetaTopOfTrackerLayer(jlayer,beta));
  // then we use simply the landau formula, the only parameter is the regime change
  if (beta_tol<=0) return 0.;
  Double_t beta0 = 0.9332318902;
  if (beta_tol>=beta0) return 1.; // beta "saturation" region  
  return delta_beta(&beta_tol,pars_Si)/delta_beta(&beta0,pars_Si);
}


static Double_t beta_tol_on_beta_pars_tof[4][5] = {
  {  0.9650,  0.3450,  1.8971,  0.0488, -0.3247},
  {  0.9650,  0.3450,  1.4774,  0.0395, -0.2542},
  {  0.9650,  0.3450, -1.3131,  0.0695,  0.3346},
  {  0.9650,  0.3650,-42.9125, 36.8601,-11.6073}
};
Double_t AMSEnergyLoss::GetBetaTopOfTofLayer(Int_t layer, Double_t beta) { 
  if ( (layer<1)||(layer>4) ) return 0;
  if (beta<0) return 0;
  if (beta>1) return beta;
  Double_t beta_tol = beta*beta_tol_on_beta(&beta,beta_tol_on_beta_pars_tof[layer-1]);
  if (beta_tol<0) return 0;
  return beta_tol;
}


Double_t AMSEnergyLoss::GetBetaCorrectionTofLayer(Int_t layer, Double_t beta) {
  if ( (layer<1)||(layer>4) ) return 0;
  Double_t beta_tol = fabs(GetBetaTopOfTofLayer(layer,beta));
  if (beta_tol<=0) return 0.;
  Double_t beta0 = 0.9632582664;
  if (beta_tol>=beta0) return 1.;  
  return delta_beta(&beta_tol,pars_Poly)/delta_beta(&beta0,pars_Poly);
}


///////////////////////////////////////////////
// More detailed correction (April 2012)
///////////////////////////////////////////////


Double_t pars_line_to_line_tof[2][4][10] = {
 {  
   {0.91372,   0.04339,  -0.45000,  -0.03276,   0.63974,  -0.17041,  -0.09822,  -0.02005,   0.22642,  -1.46110},
   {0.92646,   0.03558,  -0.45000,  -0.00409,   0.56637,  -0.00097,  -0.23722,   0.01625,   0.70780,  -3.34371},
   {0.96608,   0.01511,  -0.45000,   0.08939,   0.30528,   0.07360,  -0.02429,  -0.18239,   0.51042,  -1.15171},
   {0.95533,   0.02055,  -0.35000,  -0.00789,   0.54394,   0.13982,  -2.57996,   3.72169,   2.83606, -43.66837}
 },
 {
   {0.00000,   0.74209,  -0.40000,   0.63455,   0.85468,   1.86918,  -1.22962,  -7.11909,   1.97813,   3.43273},
   {0.00000,   0.73994,  -0.40000,   0.63473,   0.84812,   1.89650,  -1.48377,  -6.71730,   1.77799,   3.46623},
   {0.00000,   0.78818,  -0.40000,   0.91822,   0.45222,  -0.05861,  -0.72485,  -0.72204,   0.33732,   0.40407},
   {0.00000,   0.77906,  -0.40000,   0.82362,   0.33812,  -0.63379,  -0.16073,  -0.07061,  -0.00543,   0.01793}
 }
};

Double_t pars_line_to_line_trk[2][9][10] = {
  {
    {1.00000,   0.00000,  -0.50000,   0.40000,   0.43483,  -0.75777,  -1.49649,  -1.06304,   6.39697,   9.65724},
    {1.00000,   0.00000,  -0.50000,   0.40000,  -0.08619,  -1.59852,   3.83886,   9.39149, -10.76993, -23.62349},
    {1.00000,   0.00000,  -0.50000,   0.40000,  -0.09729,  -1.68035,   3.65058,   9.65652,  -9.82645, -23.91057},
    {1.00000,   0.00000,  -0.50000,   0.40000,  -0.09968,  -1.56721,   3.55177,   8.97145,  -9.48035, -22.43871},
    {1.00000,   0.00000,  -0.50000,   0.40000,  -0.09746,  -1.53554,   3.53525,   8.67784,  -9.42347, -21.76621},
    {1.00000,   0.00000,  -0.50000,   0.40000,  -0.10042,  -1.41231,   3.08069,   7.67724,  -7.74999, -18.79811},
    {1.00000,   0.00000,  -0.50000,   0.40000,  -0.08050,  -1.39769,   3.01911,   7.54782,  -7.75470, -18.44399},
    {1.00000,   0.00000,  -0.50000,   0.40000,  -0.12681,  -1.40559,   3.59825,   8.14137,  -9.61972, -20.94694},
    {1.00000,   0.00000,  -0.35000,   0.40000,  -0.31971,  -1.41184,   3.14494,  12.50434,  -3.34747, -46.27081}
  },
  {
    {0.00000,   0.50762,  -0.40000,   0.47948,   0.91377,  -0.09681,  -1.70135,  -0.59973,  -4.69414,  -1.68149},
    {0.00000,   0.50463,  -0.45000,   0.49271,   0.52604,   0.67790,  -1.26153,  -2.65564,  -1.01788,  -4.66670},
    {0.00000,   0.50894,  -0.45000,   0.49274,   0.51781,   0.62564,  -1.07816,  -2.43755,  -1.55594,  -4.86708},
    {0.00000,   0.50700,  -0.45000,   0.49247,   0.44706,   0.69581,  -0.93254,  -2.44756,  -1.22166,  -5.95336},
    {0.00000,   0.50918,  -0.45000,   0.49361,   0.48870,   0.65234,  -1.08773,  -2.42101,  -1.40045,  -4.96818},
    {0.00000,   0.51192,  -0.45000,   0.50660,   0.39666,   0.07289,  -0.54138,  -0.91698,  -2.45352,  -3.89920},
    {0.00000,   0.50152,  -0.45000,   0.48394,   0.46596,   0.75831,  -1.08248,  -2.59937,  -1.67778,  -6.06286},
    {0.00000,   0.50759,  -0.45000,   0.49326,   0.43827,   0.72768,  -1.03087,  -2.50387,  -1.37119,  -5.21595},
    {0.00000,   0.50393,  -0.30000,   0.55721,   0.20881,   6.56003, -11.57354, -37.18428,  89.71840, -49.47042}
  }
};


#define DEGREE 7 
/* Parametrization of the effective betagamma computation 
          ( M*x + Q                 x>x1
   f(x) = < sum_{i}^{N}  a_i x^i    x0<x<=x1
          ( m*x + q                 x<=x0 
   - pars (M,m,Q,q,a_0,...,a_N,x0,x1) = 2 + 2 + (N+1) + 2 = N + 7
   - match in continuity and derivative continuity, 4 conditions -> N+3 pars.
   - pars let free (M,Q,a_2,...,a_N,x0,x1)  
*/
Double_t AMSEnergyLoss::line_to_line(Double_t *x, Double_t *par) {
  Double_t xx = x[0];
  Double_t M  = par[0];
  Double_t Q  = par[1]; 
  Double_t x0 = par[2];
  Double_t x1 = par[3];
  Double_t va = M*xx + Q;
  if (xx>x1) return va;
  Double_t a[DEGREE+1];
  for (int i=2; i<=DEGREE; i++) a[i] = par[i+2];
  a[1] = M;      for (int i=2; i<=DEGREE; i++) a[1] += -i*pow(x1,i-1)*a[i];
  a[0] = M*x1+Q; for (int i=1; i<=DEGREE; i++) a[0] += -pow(x1,i)*a[i];
  va = 0;        for (int i=0; i<=DEGREE; i++) va   += pow(xx,i)*a[i];
  if (xx>x0)     return va;
  Double_t m = 0;     for (int i=1; i<=DEGREE; i++) m += i*pow(x0,i-1)*a[i];
  Double_t q = -m*x0; for (int i=0; i<=DEGREE; i++) q += pow(x0,i)*a[i];
  va = m*xx + q;
  return va;
}


Double_t AMSEnergyLoss::GetLogBetaGammaFromBeta(Double_t beta) { 
  beta = fabs(beta);
  if (beta<1e-6) return -100;
  if (beta>MAXBETA) beta = MAXBETA; 
  return log10(beta/sqrt(1-beta*beta)); 
}


Double_t AMSEnergyLoss::GetLogBetaGammaFromRigidity(Double_t rigidity, Double_t mass_on_Z) { 
  rigidity = fabs(rigidity);
  if (rigidity<1e-6) return -100;
  return log10(rigidity/mass_on_Z); 
}


Double_t AMSEnergyLoss::GetTofLayerEffLogBetaGammaFromRigidity(Int_t layer, Double_t rigidity, Double_t mass_on_Z) {
  if ( (layer<1)||(layer>4) ) return -100;
  Double_t logbetagamma = GetLogBetaGammaFromRigidity(rigidity,mass_on_Z);
  return line_to_line(&logbetagamma,&pars_line_to_line_tof[0][layer-1][0]); 
}


Double_t AMSEnergyLoss::GetTofLayerEffLogBetaGammaFromBeta(Int_t layer, Double_t beta) {
  if ( (layer<1)||(layer>4) ) return -100;
  Double_t logbetagamma = GetLogBetaGammaFromBeta(beta);
  return line_to_line(&logbetagamma,&pars_line_to_line_tof[1][layer-1][0]);
}


Double_t AMSEnergyLoss::GetTofLayerEffLogBetaGamma(Int_t layer, Double_t beta, Double_t rigidity, Double_t mass_on_Z) {
  if ( (layer<1)||(layer>4) ) return -100;
  Double_t logbetagamma_beta = GetLogBetaGammaFromBeta(beta);
  Double_t logbetagamma_rigi = GetLogBetaGammaFromRigidity(rigidity,mass_on_Z);
  Double_t effbetagamma_beta = GetTofLayerEffLogBetaGammaFromBeta(layer,beta);
  Double_t effbetagamma_rigi = GetTofLayerEffLogBetaGammaFromRigidity(layer,rigidity,mass_on_Z);
  // both determination failed
  if ( (effbetagamma_beta<=-100)&&(effbetagamma_rigi<=-100) ) return -100;
  // beta determination failed
  if (effbetagamma_beta<=-100) return effbetagamma_rigi; 
  // rigidity determination failed
  if (effbetagamma_rigi<=-100) return effbetagamma_beta; 
  // if the two determinations did not fail then decide wich one is best
  if (logbetagamma_beta<0.185) return effbetagamma_beta;
  return effbetagamma_rigi;
}


Double_t AMSEnergyLoss::GetTofLayerLogBetaGammaCorrectionFromRigidity(Int_t layer, Double_t rigidity, Double_t mass_on_Z) {
  Double_t logbetagamma = GetTofLayerEffLogBetaGammaFromRigidity(layer,rigidity,mass_on_Z);
  return delta_logbetagamma(&logbetagamma,&pars_Poly[0])/delta_logbetagamma(&pars_Poly[8],&pars_Poly[0]);            
} 


Double_t AMSEnergyLoss::GetTofLayerLogBetaGammaCorrectionFromBeta(Int_t layer, Double_t beta) {
  Double_t logbetagamma = GetTofLayerEffLogBetaGammaFromBeta(layer,beta);
  return delta_logbetagamma(&logbetagamma,&pars_Poly[0])/delta_logbetagamma(&pars_Poly[8],&pars_Poly[0]);            
}


Double_t AMSEnergyLoss::GetTofLayerLogBetaGammaCorrection(Int_t layer, Double_t beta, Double_t rigidity, Double_t mass_on_Z) {
  Double_t logbetagamma = GetTofLayerEffLogBetaGamma(layer,beta,rigidity,mass_on_Z);
  return delta_logbetagamma(&logbetagamma,&pars_Poly[0])/delta_logbetagamma(&pars_Poly[8],&pars_Poly[0]);  
}


Double_t AMSEnergyLoss::GetTrackerLayerEffLogBetaGammaFromRigidity(Int_t layer, Double_t rigidity, Double_t mass_on_Z) {
  if ( (layer<1)||(layer>9) ) return -100;
  Double_t logbetagamma = GetLogBetaGammaFromRigidity(rigidity,mass_on_Z);
  return line_to_line(&logbetagamma,&pars_line_to_line_trk[0][layer-1][0]);
}


Double_t AMSEnergyLoss::GetTrackerLayerEffLogBetaGammaFromBeta(Int_t layer, Double_t beta) {
  if ( (layer<1)||(layer>9) ) return -100;
  Double_t logbetagamma = GetLogBetaGammaFromBeta(beta);
  return line_to_line(&logbetagamma,&pars_line_to_line_trk[1][layer-1][0]);
}


Double_t AMSEnergyLoss::GetTrackerLayerEffLogBetaGamma(Int_t layer, Double_t beta, Double_t rigidity, Double_t mass_on_Z) {
  if ( (layer<1)||(layer>9) ) return -100;
  Double_t logbetagamma_beta = GetLogBetaGammaFromBeta(beta);
  Double_t logbetagamma_rigi = GetLogBetaGammaFromRigidity(rigidity,mass_on_Z);
  Double_t effbetagamma_beta = GetTrackerLayerEffLogBetaGammaFromBeta(layer,beta);
  Double_t effbetagamma_rigi = GetTrackerLayerEffLogBetaGammaFromRigidity(layer,rigidity,mass_on_Z);
  // both determination failed
  if ( (effbetagamma_beta<=-100)&&(effbetagamma_rigi<=-100) ) return -100;
  // beta determination failed
  if (effbetagamma_beta<=-100) return effbetagamma_rigi; 
  // rigidity determination failed
  if (effbetagamma_rigi<=-100) return effbetagamma_beta; 
  // if the two determinations did not fail then decide wich one is best
  if (logbetagamma_beta<0.135) return effbetagamma_beta;
  return effbetagamma_rigi;
}


Double_t AMSEnergyLoss::GetTrackerLayerLogBetaGammaCorrectionFromRigidity(Int_t layer, Double_t rigidity, Double_t mass_on_Z) {
  Double_t logbetagamma = GetTrackerLayerEffLogBetaGammaFromRigidity(layer,rigidity,mass_on_Z);
  return delta_logbetagamma(&logbetagamma,&pars_Si[0])/delta_logbetagamma(&pars_Si[8],&pars_Si[0]);
}


Double_t AMSEnergyLoss::GetTrackerLayerLogBetaGammaCorrectionFromBeta(Int_t layer, Double_t beta) {
  Double_t logbetagamma = GetTrackerLayerEffLogBetaGammaFromBeta(layer,beta);
  return delta_logbetagamma(&logbetagamma,&pars_Si[0])/delta_logbetagamma(&pars_Si[8],&pars_Si[0]);
}


Double_t AMSEnergyLoss::GetTrackerLayerLogBetaGammaCorrection(Int_t layer, Double_t beta, Double_t rigidity, Double_t mass_on_Z) {
  Double_t logbetagamma = GetTrackerLayerEffLogBetaGamma(layer,beta,rigidity,mass_on_Z);
  return delta_logbetagamma(&logbetagamma,&pars_Si[0])/delta_logbetagamma(&pars_Si[8],&pars_Si[0]);            
}


