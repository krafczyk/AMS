//  $Id: TrFit.h,v 1.32 2013/02/02 16:43:52 shaino Exp $
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
///$Date: 2013/02/02 16:43:52 $
///
///$Revision: 1.32 $
///
//////////////////////////////////////////////////////////////////////////

#include "point.h"
class TkDBc;

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

  /// Helium4 mass in GeV
  static double Mhelium;

  /// Muon mass in GeV
  static double Mmuon;

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

  double GetD0x (void) const { return _dxdz*GetD0z(); }
  double GetD0y (void) const { return _dydz*GetD0z(); }
  double GetD0z (void) const { return 1/sqrt(_dxdz*_dxdz+_dydz*_dydz+1); }

  double GetPhi  (void) const { return atan2(_dydz, _dxdz);}
  double GetTheta(void) const { return atan(sqrt(_dydz*_dydz+_dxdz*_dxdz));}
  double GetMass(){return _mass;}
  double GetCharge(){return _chrg;}

  double GetRigidity(void) const { return _rigidity; }

  AMSPoint GetP0 (void) const { return AMSPoint(_p0x, _p0y, _p0z); }
  AMSDir   GetDir(void) const { return AMSDir(_dxdz, _dydz, 1); }

  /// Set particle mass and charge (Z)
  void SetMass    (double mass) { _mass = mass; }
  void SetChrg    (double chrg) { _chrg = chrg; }
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

  /// Propagation with JAStepPin
  void Propagate(double *x, double *d, double *u, int ndiv = 20);

  /// Build interpolation lists onto Z=plist[i].z() (0<=i<nz) planes given
  void InterpolateSimple(int nz, AMSPoint *plist, AMSDir *dlist = 0);

public:
  /// Wrapper to TkDBc, null pointer check included
  static ::TkDBc *TkDBc();

  /// Wrapper to MagField::GUFLD, field map check included
  static void GuFld(double *p, double *b);

  /// Wrapper to GuFld(double*, double*)
  static void GuFld(double x, double y, double z, double *b);

  /// Wrapper to GuFld(double*, double*)
  static AMSPoint GuFld(AMSPoint);

protected:
  /// Wrapper to MagField::TKFLD
  static void TkFld(double *p, double hxy[][3]);

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

  ClassDef(TrProp,1);
};

class TrFit : public TrProp {

public:
  enum ENums{ 
    /// Maximum number of layer
    LMAX = 20,
    /// Parameter buffer size
    PMAX = 10
  };
  enum EMethods{ LINEAR, CIRCLE, SIMPLE, ALCARAZ, CHOUTKO, CHIKANIANC,
		 CHIKANIANF };

protected:
  /// Multiple scattering switch
  int _mscat;

  /// Energy loss correction switch
  int _eloss;

  int    _nhit;          ///< Number of hits
  int    _nhitx;         ///< Number of hits in X
  int    _nhity;         ///< Number of hits in Y
  int    _nhitxy;        ///< Number of hits in X and Y

  double _xh[LMAX];      ///< Hit position in X
  double _yh[LMAX];      ///< Hit position in Y
  double _zh[LMAX];      ///< Hit position in Z
  double _xs[LMAX];      ///< Fitting error in X
  double _ys[LMAX];      ///< Fitting error in Y
  double _zs[LMAX];      ///< Fitting error in Z
  double _xr[LMAX];      ///< Fitting residual in X
  double _yr[LMAX];      ///< Fitting residual in Y
  double _zr[LMAX];      ///< Fitting residual in Z

  double _bx[LMAX];      ///< Magnetic field in X
  double _by[LMAX];      ///< Magnetic field in Y
  double _bz[LMAX];      ///< Magnetic field in Z

  double _chisqx;        ///< Fitting chisquare in X (Not normalized)
  double _chisqy;        ///< Fitting chisquare in Y (Not normalized)
  int    _ndofx;         ///< Number of degree of freedom in X
  int    _ndofy;         ///< Number of degree of freedom in Y

  // Normalized chisquare
  double _chisq;         ///< Fitting chisquare, _chisq(x+y)/_ndof(x+y)
  double _errrinv;       ///< Error of 1/Rigidity (1/GV)

  /// Fitting parameters temporary buffer
  double _param[PMAX];   //!

  /// Checks if the magfield is on
  bool _fieldon(){
    double bb = 0;
    for(int i = 0; i <_nhit; i++)
      bb += _bx[i]*_bx[i]+_by[i]*_by[i]+_bz[i]*_bz[i];
    return (bb > 0);
  }

public:
  /// Default constructor
  TrFit(void);

  /// Destructor
  ~TrFit();
  double GetBeta();
  
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

  double GetXh(int i) const { return (0 <= i && i < LMAX)? _xh[i] : 0; }
  double GetYh(int i) const { return (0 <= i && i < LMAX)? _yh[i] : 0; }
  double GetZh(int i) const { return (0 <= i && i < LMAX)? _zh[i] : 0; }

  double GetXs(int i) const { return (0 <= i && i < LMAX)? _xs[i] : 0; }
  double GetYs(int i) const { return (0 <= i && i < LMAX)? _ys[i] : 0; }
  double GetZs(int i) const { return (0 <= i && i < LMAX)? _zs[i] : 0; }

  double GetXr(int i) const { return (0 <= i && i < LMAX)? _xr[i] : 0; }
  double GetYr(int i) const { return (0 <= i && i < LMAX)? _yr[i] : 0; }
  double GetZr(int i) const { return (0 <= i && i < LMAX)? _zr[i] : 0; }

  void SetErr(int i, double xs, double ys, double zs) {
    if (0 <= i && i < LMAX) { _xs[i] = xs; _ys[i] = ys; _zs[i] = zs; }
  }

  /// Set multiple scattring and energy loss switch
  void SetMultScat(int msc, int els = 0) { _mscat = msc; _eloss = els; }

  /// Recomended to use GetP0x(), GetDxDz(),. etc. instead of Getparam
  double GetParam(int i) const {
    return (0 <= i && i < PMAX)? _param[i] : 0;
  }

  /// Clear data members
  void Clear();

  /// Add a new hit with a position by AMSPoint and its error (ex,ey,ez)
  int Add(AMSPoint pos, double ex, double ey, double ez,
	  double bx, double by, double bz, int at = -1) {
    return Add(pos.x(), pos.y(), pos.z(), ex, ey, ez, bx, by, bz,at);
  }

  /// Add a new hit with a position and its error (ex, ey, ez)
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

  /*!
   \brief perform  fit, fitting algorithm to be selected 
   \param method Fitting Algoritm=
   \li LINEAR
   \li CIRCLE
   \li SIMPLE
   \li ALCARAZ
   \li CHOUTKO
   \li CHIKANIANC
   \li CHIKANIANF
   \param mscat  (0/1) activate the multiple scattering treatment
   \param eloss  (0/1) activate the energy loss treatment (to be implemented)
   \param charge charge of the fitted particle
   \param mass   mass of the particle (calculated if mass==0 && beta!=0)
   \param beta   used to calculate mass if mass==0 && beta!=0
   \param fixrig fix the rigidity for the fit (available only with ALCARAZ algo)
  */
  double DoFit(int method = CHOUTKO, int mscat = 1, int eloss = 1,
	       float charge = 1, float mass = Mproton, float beta = 0,float fixrig=0);

  /// Linear fitting in X-Z and Y-Z planes
  double LinearFit(void);

  /// Circlar fitting in Y-Z plane and liear fitting in X-S plane
  double CircleFit(void);

  /// Simple fit (no scattering, coarse field)
  double SimpleFit(void);

  /// Simple fit (no scattering, coarse field, parameter defined at 1st layer)
  double SimpleFitTop(void);

  /// Alcaraz fit  fixr!=0 : fix rigidity as _rigidity
  double AlcarazFit(int fixr = 0);

  /// Choutko fit
  double ChoutkoFit(void);

  /// Chikanian fit C++ (may be outdated)  
  double ChikanianFitCInt(int type = 1);

  /// Chikanian fit Fortran
  double ChikanianFitF();

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

  /// Fast propagation with JAStepPin
  void PropagateFast(int ih0, int ih1, int ndiv = 20);

  /// Multiple scattering tuning factor
  static double DmscFact;

protected:
  /// Apply limits as min<abs(par)<max
  bool ParLimits(double &par, double min, double max);

  /// Weighted sum of vectors (for CircleFit)
  double VKAverage(int, double*, double*, double*);

  /// Fitting residual (for CircleFit)
  double VKResidual(double, double, double*);

  /// Estimate track position and direction at z=0
  int GetPcen(double *pos, double *dir = 0);

  /// Fill multiple scattering factor, dmsc
  int FillDmsc(double *dmsc, double fact = 1,
	       double *len = 0, double *cosz = 0, int *ilay = 0);

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

  /// Maximum number of layers for Chikanian fit
  enum { NPma = 9 };

  // Internal parameters for Chikanian fit
  double _rkms_wx[NPma][NPma]; ///< Error matrix
  double _rkms_wy[NPma][NPma]; ///< Error matrix

  /// Get error matrix (for Chikanian fit)
  void RkmsMtx(double rini);

  double _rkms_dmsN1[NPma][NPma][7];
  double _rkms_dmsN9[NPma][NPma][7];

  /// Cov.Err.Matrix with Mult.Scattering for Up --> Down track
  void RkmsMscN9(void);

  /// Cov.Err.Matrix with Mult.Scattering for Down --> Up track
  void RkmsMscN1(void);

  /// FCN function main part (for Chikanian fit)
  double RkmsFun(int npa, double *par, bool res = false);

public:
  /// FCN function for MINUIT (for Chikanian fit)
  void RkmsFCN(int&, double*, double&, double*, int);

  /// Debug switch for ChikanianFit
  static int RkmsDebug;

  /// Tracker Z position
  double RkmsZ0[NPma];

public:
  /// 3x3 Matrix inversion imported from ROOT
  static int Inv33(double mtx[3][3]);
  /// 4x4 Matrix inversion imported from ROOT
  static int Inv44(double mtx[4][4]);
  /// 5x5 Matrix inversion imported from ROOT
  static int Inv55(double mtx[5][5]);
  /// 5x5 Matrix inversion imported from ROOT
  static int Inv66(double mtx[6][6]);

  /// Symmetric Matrix inversion with TMatrixDSym
  static int InvSM(double **mtx, int ndim);

  /// Symmetric Matrix inversion with TMatrixDSym
  static int InvSM(double *mtx, int ndim);

  ClassDef(TrFit, 2);
};

#endif
