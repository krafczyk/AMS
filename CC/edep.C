#include "edep.h"

/* 
// From PDG                                  g/cm2   <Z/A>    eV     x0     x1   Cbar       a  k (m)   d0
Double_t AMSEnergyLoss::pars_Si[9]   = {0.03*1.330,0.49848,173.0,0.2014,2.8715,4.4351,0.14921,3.2546,0.14}; // 300 um of Si
Double_t AMSEnergyLoss::pars_Poly[9] = {   1*1.032,0.54141, 64.7,0.1464,2.4855,3.1997,0.16101,3.2393,0.00}; // 1 cm of Polyvinyl. 
*/
// From a fit on proton data 
Double_t AMSEnergyLoss::pars_Si[9]   = {0.06990,0.49848,173.00000,0.17490,3.81233,4.45019,0.00196,5.84622,0.01152};
Double_t AMSEnergyLoss::pars_Poly[9] = {1.03200,0.54141, 64.70000,0.14640,0.71240,4.72266,6.40222,1.74375,0.00053};


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
  else              return delta0*pow(10.,2*(eta-x0));
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
  Double_t delta0    = par[8]; // density correction
  // x is log10(betagamma) 
  Double_t eta       = x[0]; 
  Double_t betagamma = pow(10.,eta);
  Double_t beta      = sqrt(1./(1./betagamma/betagamma + 1.));
  // csi 
  Double_t csi_par[2] = {grammage,Z_on_A};
  Double_t csi        = csi_logbetagamma(&eta,csi_par);
  // density correction
  Double_t density_correction_par[6] = {x0,x1,Cbar,a,k,delta0};
  Double_t density_correction        = density_correction_logbetagamma(&eta,density_correction_par);
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
