// $Id: TrTasCluster.h,v 1.2 2009/12/21 20:46:57 shaino Exp $

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
///$Date: 2009/12/21 20:46:57 $
///
///$Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////////

#include "TrCluster.h"

class TF1;
class TDirectory;

class TrTasClusterR : public TrClusterR {

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
