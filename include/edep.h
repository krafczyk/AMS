#ifndef __AMSEnergyLoss__
#define __AMSEnergyLoss__

#include "Rtypes.h"

#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

/* 
 * Library for the treatment of the energy depositions formulas.
 * Implemented the Landau approach, as function of beta or betagamma.
 */
class AMSEnergyLoss { 

 public:

  //! Parameters for 300 um Silicon
  static Double_t pars_Si[9];
  //! Parameters for 1 cm Polyvinyltoluene
  static Double_t pars_Poly[9];

  //! Csi as function of log10(betagamma) (par = {grammage,Z_on_A})
  static Double_t csi_logbetagamma(Double_t* x, Double_t* par);
  //! Sternheimer density correction as function of log10(betagamma) (par = {x0,x1,Cbar,a,k,delta0}) 
  static Double_t density_correction_logbetagamma(Double_t* x, Double_t* par);
  //! Most Probable Energy loss (Landau formula) function of log10(betagamma) [keV] (par = {grammage,Z_on_A,I,x0,x1,Cbar,a,k,delta0})
  static Double_t delta_logbetagamma(Double_t *x, Double_t *par);
  //! Most Probable Energy loss (Landau formula) function of beta [keV] (par = {grammage,Z_on_A,I,x0,x1,Cbar,a,k,delta0})
  static Double_t delta_beta(Double_t *x, Double_t *par);





  //! Most Probable Energy loss function of beta, connected with a plateau (good for dE/dx vs beta ToF fit) 
  static Double_t delta_beta_fit(Double_t *x, Double_t *par);
  //! Function for the beta transformation, used in charge deposition fitting 
  static Double_t beta_tol_on_beta(Double_t *x, Double_t *par);


  //! Most probable energy deposition in 300 um Silicon (keV)
  static Double_t GetMostProbEDepIn300umSi(Double_t logbetagamma);
  //! Most probable energy deposition in 1 cm Polyvinyltoluene (keV)
  static Double_t GetMostProbEDepIn1cmPoly(Double_t logbetagamma);


  //! Beta at top of a Tracker layer (jlayer = 1, ..., 9; beta = beta measured in ToF)
  static Double_t GetBetaTopOfTrackerLayer(Int_t jlayer, Double_t beta);
  //! Low energy beta correction (jlayer = 1, ..., 9; beta = beta measured in ToF) 
  static Double_t GetBetaCorrectionTrackerLayer(Int_t jlayer, Double_t beta);


  //! Beta at top of a ToF layer (layer = 1, ..., 4; beta = beta measured in ToF)
  static Double_t GetBetaTopOfTofLayer(Int_t layer, Double_t beta);
  //! Low energy beta correction (jlayer = 1, ..., 9; beta = beta measured in ToF) 
  static Double_t GetBetaCorrectionTofLayer(Int_t layer, Double_t beta);

};

#endif
