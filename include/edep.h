// $Id: edep.h,v 1.3 2012/04/20 07:14:25 oliva Exp $

#ifndef __AMSEnergyLoss__
#define __AMSEnergyLoss__

#include "Rtypes.h"

#include <iostream>
#include <vector>
#include <cmath>

#define MAXBETA 0.999999

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

  //! More detailed correction (April 2012)
  /* 
   *  Effective BetaGamma is the value of the betagamma needed to respect the dE/dx law for protons
   *  Then additional correction depending on Q itself should be added 
   */

  //! Parametrization for the effective \f$\beta\gamma\f$ calculation 
  static Double_t line_to_line(Double_t *x, Double_t *par);

  //! Get \f$\beta\gamma\f$ from \f$\beta\f$ (a maxbeta is imposed to avoid inf)
  static Double_t GetLogBetaGammaFromBeta(Double_t beta);
  //! Get \f$\beta\gamma\f$ from rigidity (mass/Z guess needed, proton by default)
  static Double_t GetLogBetaGammaFromRigidity(Double_t rigidity /*GV*/, Double_t mass_on_Z = 0.938);

  //! Effective \f$\beta\gamma\f$ coming from rigidity for TOF
  static Double_t GetTofLayerEffLogBetaGammaFromRigidity(Int_t layer, Double_t rigidity /*GV*/, Double_t mass_on_Z = 0.938);
  //! Effective \f$\beta\gamma\f$ coming from beta for TOF 
  static Double_t GetTofLayerEffLogBetaGammaFromBeta(Int_t layer, Double_t beta);
  //! Effective \f$\beta\gamma\f$ coming from rigidity and beta (best description of one of the two) for TOF
  static Double_t GetTofLayerEffLogBetaGamma(Int_t layer, Double_t beta, Double_t rigidity /*GV*/, Double_t mass_on_Z = 0.938);
  //! Effective \f$\beta\gamma\f$ coming from rigidity for Tracker
  static Double_t GetTrackerLayerEffLogBetaGammaFromRigidity(Int_t layer, Double_t rigidity /*GV*/, Double_t mass_on_Z = 0.938); 
  //! Effective \f$\beta\gamma\f$ coming from beta for Tracker
  static Double_t GetTrackerLayerEffLogBetaGammaFromBeta(Int_t layer, Double_t beta);
  //! Effective \f$\beta\gamma\f$ coming from rigidity and beta (best description of one of the two) for Tracker
  static Double_t GetTrackerLayerEffLogBetaGamma(Int_t layer, Double_t beta, Double_t rigidity /*GV*/, Double_t mass_on_Z = 0.938);
  
  /*
  //! Get the effective \f$\beta\gamma\f$ needed to have the theretical energy deposition release
  static Double_t GetTofEffectiveLogBetaGamma(Double_t betagamma, Int_t layer) { return 0; }
  //! Get the effective \f$\beta\gamma\f$ needed to have the theretical energy deposition release
  static Double_t GetTrackerEffectiveLogBetaGamma(Double_t betagamma, Int_t jlayer) { return 0; }
  */

  ClassDef(AMSEnergyLoss,1); 
};

#endif
