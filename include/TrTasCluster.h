// $Id$

#ifndef __TrTasCluster__
#define __TrTasCluster__

//////////////////////////////////////////////////////////////////////////
///
///\file  TrTasCluster.h
///\brief Header file of TrTasCluster class
///
///\class TrTasCluster
///\brief TAS cluster
///\ingroup tkrec
///
///\date  2009/12/10 SH  First version
///\date  2009/12/17 SH  First Gbatch version
///\date  2014/07/29 SH  Update for ISS data
///$Date$
///
///$Revision$
///
//////////////////////////////////////////////////////////////////////////

#include "TrCluster.h"

class TF1;
class TH1;
class TDirectory;

class TrTasClusterR : public TrClusterR {

////////////////// New version for ISS //////////////////
public:
  /*!
   *! Fit TAS histogram
   * @param[in]  hist   Pointer of histogram (Y only)
   * @param[in]  ibeam  Beam index (0 or 1)
   * @param[in]  mode   Fitting mode (0:Def. 1:Gaus 2:SGaus >=100: CofG)
   * @param[out] par    0:Const 1:Mean 2:sigma 3:Saturation 4:Coordinate
   * @param[out] perr   Fitting errors
   * @return     Chisquare/NDF, <0 for errors
   */
  static double Fit(TH1 *hist, int ibeam, int mode, double *par,
		                                    double *perr = 0);

////////////////// New version for ISS //////////////////

public:
  enum { GAUS  = 1, ///< Normal Gaussian
	 SGAUS = 2  ///< Saturated Gaussian
       };
  /// Fitting mode
  static int fitmode;

  /// Directory for histograms
  static TDirectory *HistDir;

public:
  /// Constructor with data
  TrTasClusterR(int tkid, int side, int addr, int nelem, float *adc);
  /// Default constructor
  TrTasClusterR() {}
  /// Destructor
 ~TrTasClusterR() {}

  /// Get CofG by fitting "Gaus" or "SGaus" function
  float GetXCofGTas();

public:
  /// Returns saturated Gaussian function
  static TF1 *SGaus(double xmin, double xmax, const char *name = "func");
  /// Core of the saturated Gaussian function
  static double SGaus(double *x, double *par);

  /// TAS alignment correction
  static AMSPoint Align(TrClusterR *xcls, TrClusterR *ycls);

  ClassDef(TrTasClusterR, 1);
};

#endif
