//  $Id: TrFit.h,v 1.11 2010/05/28 11:17:01 pzuccon Exp $
#ifndef __TrFit__
#define __TrFit__

//////////////////////////////////////////////////////////////////////////
///
///\class TrProp
///\brief A class to manage general track propagation
///\ingroup tkrec
///
/// Track propagation routines are extracted from the old TrFit 
/// (originally in TrackFit_utils by V.Choutko) and collected in TrProp.
/// So for the purpose of, for example, interpolation, 
/// only TrProp can be used.
///
///\date  2008/11/25 SH  Splitted into TrProp and TrFit
///
///
///\class TrFit
///\brief A class to manage general track fitting 
///\ingroup tkrec
///
/// Track fitting routines imported from TrackFit_utils.F
///
/// Usage: 
///  TrFit fit;   // Create an instance
///  fit.Add(x, y, z, ex, ey, ez);   // Add a hit point
///  ...
///  fit.Fit(method);    // Do fitting
/// 
/// Fitting methods (shown in EMethods)
/// LINEAR    : Linear fitting in X-Z and Y-Z planes (used in no-B)
/// CIRCLE    : Circlar fitting in Y-Z and linear in X-S (used in const.B)
/// SIMPLE    : Simple fitting, no multiple scattering, and coarse B field
///             (used for pattern scan)
/// ALCARAZ   : Fitting method by J.Alcaraz (NIMA 533 (2005) 613)
/// CHOUTKO   : Fitting method by V.Choutko (default method in tkfitg)
/// CHIKANIAN : Fitting method by A.Chikanian using TMinuit
///
///
///\date  2007/12/12 SH  First import (SimpleFit)
///\date  2007/12/14 SH  First import (tkfitg)
///\date  2007/12/20 SH  First stable version after a refinement
///\date  2007/12/20 SH  All the parameters are defined in double
///\date  2007/12/21 SH  New methods (LINEAR, CIRCLE) added, not yet tested
///\date  2008/01/20 SH  Imported to tkdev (test version)
///\date  2008/11/25 SH  Splitted into TrProp and TrFit
///\date  2008/12/01 SH  Fits methods debugged and checked
///\date  2008/12/11 SH  NORMAL renamed as CHOUTKO, and ALCARAZ fit added
///\date  2010/03/03 SH  ChikanianFit added
///
///$Date: 2010/05/28 11:17:01 $
///
///$Revision: 1.11 $
///
//////////////////////////////////////////////////////////////////////////

#include "point.h"
#include "TkDBc.h"

/// Particle propagation manager
/*!
 *  Propagation algorithm imported by SH from TrackFit_utils by V.Choutko
 */
class TrProp {

public:
  enum { 
    /// Number of dimensions for the propagation matrix
    NDIM = 5
  };

  /// Proton mass in GeV
  static double Mproton; 

  /// Speed of light in m/s
  static double Clight;

protected:
  // Track parameters
  double _p0x;           ///< Track reference point P0(X)
  double _p0y;           ///< Track reference point P0(Y)
  double _p0z;           ///< Track reference point P0(Z)
  double _dxdz;          ///< Track direction (dX/dZ) at P0
  double _dydz;          ///< Track direction (dY/dZ) at P0
  double _rigidity;      ///< Rigidity (GV)

  // Particle information used for propagation and multiple scattering
  double _mass;          ///< Particle mass (GeV/c2)
  double _chrg;          ///< Particle charge (e)

public:
  /// Constructor with default values
  TrProp(double p0x   = 0, double p0y = 0, double p0z = 0, 
	 double theta = 0, double phi = 0, double rigidity = 0);
  /// Constructor with AMSPoint and AMSDir
  TrProp(AMSPoint p0, AMSDir dir, double rigidity = 0);

  // Access functions
  double GetP0x (void) const { return _p0x;  }
  double GetP0y (void) const { return _p0y;  }
  double GetP0z (void) const { return _p0z;  }
  double GetDxDz(void) const { return _dxdz; }
  double GetDyDz(void) const { return _dydz; }
  // PZ BugFix NEVER use atan for phi angle use always atan2
  double GetPhi  (void) const { return atan2(_dydz,_dxdz);}

  double GetTheta(void) const { return atan(sqrt(_dydz*_dydz+_dxdz*_dxdz));}

  double GetRigidity(void) const { return _rigidity; }

  AMSPoint GetP0 (void) const { return AMSPoint(_p0x, _p0y, _p0z); }
  AMSDir   GetDir(void) const { return AMSDir(_dxdz, _dydz, 1); }

  /// Set particle mass and charge (Z)
  void SetMassChrg(double mass, double chrg) { _mass = mass; _chrg = chrg; }
  void SetRigidity(double rigidity)          { _rigidity = rigidity; }

  /// Clear data members
  void Clear();

  /// Propagete on the track up to Z=zpl plane
  /*
   *  _p0x,_p0y,_p0z,_dxdz,_dydz are changed as of _p0z = zpl */
  double Propagate(double zpl);

  /// Build interpolation lists onto Z=zpl[i] (0<=i<nz) planes given
  void Interpolate(int nz, double *zpl,
                   AMSPoint *plist, AMSDir *dlist = 0, double *llist = 0);

  /// Track interpolation (onto general plane)
  /*
   *  Another Interpolate method is recomended to use 
   *  for the interpolation onto Z=const. planes.
   */
  double Interpolate(AMSPoint &pnt, AMSDir &dir);

  /// Track interpolation (onto cylindrical surface)
  double InterpolateCyl(AMSPoint &pnt, AMSDir &dir, double radius, int idir);

protected:
  /// Transportation (for VC's method)
  double VCFitPar(double*, double*, double*, double[][5], int);

  /// Transportation for cylinder (for VC's method)
  double VCFitParCyl(double*, double*, double*);

  /// Transportation (for VC's method)
  void VCFuncXY(double*, double*, double*, int);

  /// Transportation (for JA's method)
  int JAStepPin(double*, double*, double*, double*, double*, double, int);

public:
  /// 5x5 matrix multiplication imported from Fortran
  static void Dmm55(double[][5], double[][5], double[][5]);

  /// Runge-Kutta transportation imported from Fortran
  static void Rkuta(double, double, double*, double*);
};

class TrFit : public TrProp {

public:
  enum ENums{ 
    /// Maximum number of layer
    // LMAX = 8, PZ Do not use this! 
    /// Parameter buffer size
    PMAX = 10,
    MaxHits=20
  };
  enum EMethods{ LINEAR, CIRCLE, SIMPLE, ALCARAZ, CHOUTKO, CHIKANIAN };

  /// Multiple scattering switch
  static int _mscat;
#pragma omp threadprivate(_mscat)
  /// Energy loss correction switch
  static int _eloss;
#pragma omp threadprivate(_eloss)
protected:
  int    _nhit;          ///< Number of hits
  int    _nhitx;         ///< Number of hits in X
  int    _nhity;         ///< Number of hits in Y
  int    _nhitxy;        ///< Number of hits in X and Y

  double _xh[MaxHits];      ///< Hit position in X
  double _yh[MaxHits];      ///< Hit position in Y
  double _zh[MaxHits];      ///< Hit position in Z
  double _xs[MaxHits];      ///< Fitting error in X
  double _ys[MaxHits];      ///< Fitting error in Y
  double _zs[MaxHits];      ///< Fitting error in Z
  double _xr[MaxHits];      ///< Fitting residual in X
  double _yr[MaxHits];      ///< Fitting residual in Y
  double _zr[MaxHits];      ///< Fitting residual in Z

  double _bx[MaxHits];      ///< Magnetic field in X
  double _by[MaxHits];      ///< Magnetic field in Y
  double _bz[MaxHits];      ///< Magnetic field in Z

  double _chisqx;        ///< Fitting chisquare in X (Not normalized)
  double _chisqy;        ///< Fitting chisquare in Y (Not normalized)
  int    _ndofx;         ///< Number of degree of freedom in X
  int    _ndofy;         ///< Number of degree of freedom in Y

  // Normalized chisquare
  double _chisq;         ///< Fitting chisquare, _chisq(x+y)/_ndof(x+y)
  double _errrinv;       ///< Error of 1/Rigidity (1/GV)

  /// Fitting parameters temporary buffer
  double _param[PMAX];   //!

public:
  /// Default constructor
  TrFit(void);

  /// Destructor
 ~TrFit();

  int    GetNhit  (void) const { return _nhit;   }
  int    GetNhitX (void) const { return _nhitx;  }
  int    GetNhitY (void) const { return _nhity;  }
  int    GetNhitXY(void) const { return _nhitxy; }

  int    GetNdofX (void) const { return _ndofx;  }
  int    GetNdofY (void) const { return _ndofy;  }
  double GetChisqX(void) const { return _chisqx; }
  double GetChisqY(void) const { return _chisqy; }

  double GetChisq   (void) const { return _chisq; }
  double GetErrRinv (void) const { return _errrinv;  }

  double GetXh(int i) const { return (0 <= i && i < MaxHits)? _xh[i] : 0; }
  double GetYh(int i) const { return (0 <= i && i < MaxHits)? _yh[i] : 0; }
  double GetZh(int i) const { return (0 <= i && i < MaxHits)? _zh[i] : 0; }

  double GetXs(int i) const { return (0 <= i && i < MaxHits)? _xs[i] : 0; }
  double GetYs(int i) const { return (0 <= i && i < MaxHits)? _ys[i] : 0; }
  double GetZs(int i) const { return (0 <= i && i < MaxHits)? _zs[i] : 0; }

  double GetXr(int i) const { return (0 <= i && i < MaxHits)? _xr[i] : 0; }
  double GetYr(int i) const { return (0 <= i && i < MaxHits)? _yr[i] : 0; }
  double GetZr(int i) const { return (0 <= i && i < MaxHits)? _zr[i] : 0; }

  /// Recomended to use GetP0x(), GetDxDz(),. etc. instead of Getparam
  double GetParam(int i) const {
    return (0 <= i && i < PMAX)? _param[i] : 0;
  }

  /// Clear data members
  void Clear();

  /// Add a new hit with a position by AMSPoint and its error (ex,ey,ez)
  int Add(AMSPoint pos, double ex, double ey, double ez, int at = -1) {
    return Add(pos.x(), pos.y(), pos.z(), ex, ey, ez, at);
  }

  /// Add a new hit with a position and its error by AMSPoint
  int Add(AMSPoint pos, AMSPoint err, int at = -1) {
    return Add(pos.x(), pos.y(), pos.z(), err.x(), err.y(), err.z(), at);
  }

  /// Add a new hit with a position (x,y,z) and its error (ex,ey,ez)
  int Add(double xh, double y,  double z,
	  double ex, double ey, double ez, int at = -1);

  /// Add a new hit with a position, its error, and B field
  int Add(double x,  double y,  double z,
	  double ex, double ey, double ez, 
          double bx, double by, double bz, int at = -1);

  /// Do fit, fitting algorithm to be selected
  double Fit(int method = CHOUTKO);

  /// Linear fitting in X-Z and Y-Z planes
  double LinearFit(void);

  /// Circlar fitting in Y-Z plane and liear fitting in X-S plane
  double CircleFit(void);

  /// Simple fit (no scattering, coarse field)
  double SimpleFit(void);

  /// Alcaraz fit  fixr!=0 : fix rigidity as _rigidity
  double AlcarazFit(int fixr = 0);

  /// Choutko fit
  double ChoutkoFit(void);

  /// Chikanian fit
  double ChikanianFit(void);

  /// Linear fitting for X-Z (side=1), Y-Z (side=2) or X-S (side=3)
  double LinearFit(int side);

  /// Circlar fitting for X-Z (side=1) or Y-Z (side=2)
  double CircleFit(int side);

  /// Analytic polynominal fitting, ndim=2,3,4 are available
  double PolynomialFit(int side, int ndim);

  /// Apply ParLimits after fitting
  int ParLimits(void);

  /// Estimate layer number [0:trconst::maxlay-1] from the hit Z-coodinates
  int GetLayer(double z);

protected:
  /// Apply limits as min<abs(par)<max
  bool ParLimits(double &par, double min, double max);

  /// Weighted sum of vectors (for CircleFit)
  double VKAverage(int, double*, double*, double*);

  /// Fitting residual (for CircleFit)
  double VKResidual(double, double, double*);

  /// Initialize JA's method
  int JAInitPar(int);

  /// Fill F and G matrices (for JA's method)
  int JAFillFGmtx(double*, double*, double*, double*, int);

  /// Fill V and W matrices (for JA's method)
  int JAFillVWmtx(double*, double*, double*, double*, int*);

  /// Minimize parameters (for JA's method)
  int JAMinParams(double*, double*, int, int);

  /// Get error matrix (for VC's method)
  void VCErrMtx(int, double, double*, double*, double &, double &);

public:
  /// Startup routine (for Chikanian fit)
  void RkmsFit(double *out);

  // Internal parameters for Chikanian fit
  double _rkms_err[MaxHits][MaxHits]; ///< Error matrix

  /// Get error matrix (for Chikanian fit)
  void RkmsMtx(double rini);

  /// FCN function main part (for Chikanian fit)
  double RkmsFun(int npa, double *par, bool res = false);

public:
  /// FCN function for MINUIT (for Chikanian fit)
  void RkmsFCN(int&, double*, double&, double*, int);

  /// Debug switch for ChikanianFit
  static int RkmsDebug;


public:
  /// 3x3 Matrix inversion imported from ROOT
  static int Inv33(double mtx[3][3]);
  /// 4x4 Matrix inversion imported from ROOT
  static int Inv44(double mtx[4][4]);
  /// 5x5 Matrix inversion imported from ROOT
  static int Inv55(double mtx[5][5]);
  /// 5x5 Matrix inversion imported from ROOT
  static int Inv66(double mtx[6][6]);
};

#endif
