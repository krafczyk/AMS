// $Id$

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


  /** @name Most Probable Energy Loss description (Landau, 1944). 
    * 
    * The treatment involves also the Sternheimer density correction
    * modified to have a continuous behaviour (delta0 suppressed). 
    */
  /**@{*/
  //! Csi as function of log10(betagamma) (par = {grammage,Z_on_A})
  static Double_t csi_logbetagamma(Double_t* x, Double_t* par);
  //! Sternheimer density correction as function of log10(betagamma) (par = {x0,x1,Cbar,a,k,delta0}) 
  static Double_t density_correction_logbetagamma(Double_t* x, Double_t* par);
  //! Most Probable Energy loss (Landau formula) function of log10(betagamma) [keV] (par = {grammage,Z_on_A,I,x0,x1,Cbar,a,k,delta0})
  static Double_t delta_logbetagamma(Double_t *x, Double_t *par);
  //! Most Probable Energy loss (Landau formula) function of beta [keV] (par = {grammage,Z_on_A,I,x0,x1,Cbar,a,k,delta0})
  static Double_t delta_beta(Double_t *x, Double_t *par);

  //! Most probable energy deposition in 300 um Silicon (keV)
  static Double_t GetMostProbEDepIn300umSi(Double_t logbetagamma);
  //! Most probable energy deposition in 1 cm Polyvinyltoluene (keV)
  static Double_t GetMostProbEDepIn1cmPoly(Double_t logbetagamma);
  /**@}*/


  /** @name Energy loss correction (preliminary correction)
    */
  /**@{*/
  //! Function for the beta transformation, used in charge deposition fitting 
  static Double_t beta_tol_on_beta(Double_t *x, Double_t *par);

  //! Beta at top of a Tracker layer (jlayer = 1, ..., 9; beta = beta measured in ToF)
  static Double_t GetBetaTopOfTrackerLayer(Int_t jlayer, Double_t beta);
  //! Low energy beta correction (jlayer = 1, ..., 9; beta = beta measured in ToF) 
  static Double_t GetBetaCorrectionTrackerLayer(Int_t jlayer, Double_t beta);

  //! Beta at top of a ToF layer (layer = 1, ..., 4)
  static Double_t GetBetaTopOfTofLayer(Int_t layer, Double_t beta);
  //! Low energy beta correction (jlayer = 1, ..., 4; beta = beta measured in ToF) 
  static Double_t GetBetaCorrectionTofLayer(Int_t layer, Double_t beta);
  /**@}*/


  /** @name More detailed energy loss correction (April 2012)
    *
    * Both Beta and Rigidity (A/Z guess) dependence implemented.
    */
  /**@{*/
  //! Parametrization for the effective \f$\beta\gamma\f$ calculation 
  static Double_t line_to_line(Double_t *x, Double_t *par);

  //! Get \f$\beta\gamma\f$ from \f$\beta\f$ (a maxbeta is imposed to avoid inf)
  static Double_t GetLogBetaGammaFromBeta(Double_t beta);
  //! Get \f$\beta\gamma\f$ from rigidity (mass/Z guess needed, proton by default)
  static Double_t GetLogBetaGammaFromRigidity(Double_t rigidity /*GV*/, Double_t mass_on_Z = 0.938);

  //! Effective \f$\beta\gamma\f$ coming from rigidity for TOF (layer = 1, ..., 4)
  static Double_t GetTofLayerEffLogBetaGammaFromRigidity(Int_t layer, Double_t rigidity /*GV*/, Double_t mass_on_Z = 0.938);
  //! Effective \f$\beta\gamma\f$ coming from beta for TOF (layer = 1, ..., 4)
  static Double_t GetTofLayerEffLogBetaGammaFromBeta(Int_t layer, Double_t beta);
  //! Effective \f$\beta\gamma\f$ coming from rigidity and beta (best description of one of the two) for TOF (layer = 1, ..., 4)
  static Double_t GetTofLayerEffLogBetaGamma(Int_t layer, Double_t beta, Double_t rigidity /*GV*/, Double_t mass_on_Z = 0.938);

  //! TOF betagamma correction using rigidity  
  static Double_t GetTofLayerLogBetaGammaCorrectionFromRigidity(Int_t layer, Double_t rigidity /*GV*/, Double_t mass_on_Z = 0.938);
  //! TOF betagamma correction using beta
  static Double_t GetTofLayerLogBetaGammaCorrectionFromBeta(Int_t layer, Double_t beta);
  //! TOF betagamma correction using beta and rigidity 
  static Double_t GetTofLayerLogBetaGammaCorrection(Int_t layer, Double_t beta, Double_t rigidity /*GV*/, Double_t mass_on_Z = 0.938);

  //! Effective \f$\beta\gamma\f$ coming from rigidity for Tracker (layer = 1, ..., 9)
  static Double_t GetTrackerLayerEffLogBetaGammaFromRigidity(Int_t layer, Double_t rigidity /*GV*/, Double_t mass_on_Z = 0.938); 
  //! Effective \f$\beta\gamma\f$ coming from beta for Tracker (layer = 1, ..., 9)
  static Double_t GetTrackerLayerEffLogBetaGammaFromBeta(Int_t layer, Double_t beta);
  //! Effective \f$\beta\gamma\f$ coming from rigidity and beta (best description of one of the two) for Tracker (layer = 1, ..., 9)
  static Double_t GetTrackerLayerEffLogBetaGamma(Int_t layer, Double_t beta, Double_t rigidity /*GV*/, Double_t mass_on_Z = 0.938);

  //! Tracker betagamma correction using rigidity  
  static Double_t GetTrackerLayerLogBetaGammaCorrectionFromRigidity(Int_t layer, Double_t rigidity /*GV*/, Double_t mass_on_Z = 0.938);
  //! Tracker betagamma correction using beta
  static Double_t GetTrackerLayerLogBetaGammaCorrectionFromBeta(Int_t layer, Double_t beta);
  //! Tracker betagamma correction using beta and rigidity 
  static Double_t GetTrackerLayerLogBetaGammaCorrection(Int_t layer, Double_t beta, Double_t rigidity /*GV*/, Double_t mass_on_Z = 0.938);
  /**@}*/

  virtual ~AMSEnergyLoss() { }
  ClassDef(AMSEnergyLoss,1); 
};

#endif
