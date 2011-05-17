//  $Id: TrAlignFit.h,v 1.5 2011/05/17 09:19:37 shaino Exp $
#ifndef __TrAlignFit__
#define __TrAlignFit__

#ifndef _MINUIT2_
#include "TObject.h"

class TrExtAlignFit : public TObject {
public:
  ClassDef(TrExtAlignFit, 1);
};

#else

//////////////////////////////////////////////////////////////////////////
///
///\file  TrAlignFit.h
///\brief Header file of TrAlignFit class
///
///\class TrAlignFit
///\brief Tracker alignment fit
///\ingroup gbint
///
///\date  2007/04/02 SH  First test version
///\date  2011/05/13 SH  Dynamic ext.planes alignment
///$Date: 2011/05/17 09:19:37 $
///
///$Revision: 1.5 $
///
//////////////////////////////////////////////////////////////////////////

#include "TFcnAdapter.h"
#include "TFitterMinuit.h"
#include "TObjArray.h"
#include "TrExtAlignDB.h"

#include <vector>

class TH2;
class TH3;
class TDirectory;

class TrExtAlignFit : public TFcnAdapter {

protected:
  /// Layer number (8/9)
  Int_t fLayer;

  /// Fitting parameters
  mutable TrExtAlignPar fPar;

  /// Array of histograms for the fitting
  TObjArray fHarray;

  /// Base working directory
  static TDirectory *fWorkDir;

  /// Minuit2 engine
  static TFitterMinuit *fMinfit;

public:
  static Double_t fRange;

public:
  /// Default constructor
  TrExtAlignFit() : TFcnAdapter(0), fLayer(0) {}

  /// Constructor
  TrExtAlignFit(Int_t layer) : TFcnAdapter(0), fLayer(layer) {}

  /// Get Layer number
  Int_t GetLayer() const { return fLayer; }

  /// Get parameter
  const TrExtAlignPar &GetPar() const { return fPar; }

public:
  /// Main method: Alignment fitting on layer 8/9 and insert to database
  static Int_t Fit(Int_t layer, uint time, 
		   TDirectory *dir = 0,
		   const char *name = "TrAlg%d%d",
		   Int_t prof_mode = 0,
		   Int_t fix = 0x38, Int_t verb = 0, Int_t nfcn = 1000);

  /// Write working directroy into current file
  static Int_t WriteWorkDir(const char *name = "ExtAlign");

  typedef std::vector<Double_t> PVec;

  /// Virtual function in TFcnAdapter : FCNGradientBase : FCNBase
  Double_t operator() (const PVec &par) const { return Chisq(par); }

  /// Function called for chisquare calculation in the minimization
  Double_t Chisq(const PVec &par, Bool_t pset = kFALSE) const;

  /// Calculate chisquare with parameters in fMinfit
  Double_t Chisq(Bool_t pset = kFALSE) const;

  /// Initiate the minimization
  Int_t Minimize(Int_t fix = 0, Int_t verb = 0, Int_t nfcn = 1000);

  /// Copy 3D histograms from TDirectory, dir=0: from hman
  Int_t CopyHists(TDirectory *dir = 0, const char *name = "TrAlg%d%d");

  /// Profile 3D histograms in fHarray
  Int_t ProfHists(Int_t mode = 0);

  /// Profile 3D histogram
  static TH2 *Prof(TH3 *hist, const char *name, 
		   Int_t mode, Int_t wthd = 500,
		   Double_t pmax = 500e-4, Double_t emax = 50e-4,
		   Double_t emin = 0.5e-4,
		   Double_t sig1 = 0.03,   Double_t sig2 = 0.1, 
		   Double_t rpk  = 1);

  ClassDef(TrExtAlignFit, 1);
};

#endif
#endif
