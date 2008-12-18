//  $Id: TrFit.C,v 1.1 2008/12/18 11:19:31 pzuccon Exp $


//////////////////////////////////////////////////////////////////////////
///
///\file  TrFit.C
///\brief Source file of TrFit class
///
///\date  2007/12/12 SH  First import (SimpleFit)
///\date  2007/12/14 SH  First import (tkfitg)
///\date  2007/12/20 SH  First stable version after a refinement
///\date  2007/12/20 SH  All the parameters are defined in double
///\date  2007/12/21 SH  New methods (LINEAR, CIRCLE) added, not yet tested
///\date  2008/01/20 SH  Imported to tkdev (test version)
///\date  2008/11/25 SH  Splitted into TrProp and TrFit
///\date  2008/12/02 SH  Fits methods debugged and checked
///$Date: 2008/12/18 11:19:31 $
///
///$Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////

#include "TrFit.h"
#include "MagField.h"

#include <cmath>
#include <iostream>     // for min and max


int TrFit::_ims = 0;

TrFit::TrFit(void):TrProp()
{

  Clear();

}

TrFit::~TrFit()
{
}

void TrFit::Clear()
{
  _nhit = _nhitx = _nhity = _nhitxy = 0;
  for (int i = 0; i < PMAX; i++) _param[i] = 0;
  for (int i = 0; i < LMAX; i++) _xh[i] = _yh[i] = _zh[i] = 0;
  for (int i = 0; i < LMAX; i++) _xs[i] = _ys[i] = _zs[i] = 0;
  for (int i = 0; i < LMAX; i++) _xr[i] = _yr[i] = _zr[i] = 0;
  for (int i = 0; i < LMAX; i++) _bx[i] = _by[i] = _bz[i] = 0;
  _chisqx = _chisqy = _chisq = -1;
  _ndofx  = _ndofy  =  0;
  _errrinv = 0;
}

int TrFit::Add(double x,  double y,  double z,
               double ex, double ey, double ez, int at)
{
  float pos[3] = { x, y, z }, bf[3] = { 0, 0, 0 };
  if (MAGSFFKEY.magstat) MagField::GetPtr()->GuFld(pos, bf);
  return Add(x, y, z, ex, ey, ez, bf[0], bf[1], bf[2], at);
}

int TrFit::Add(double x,  double y,  double z,
	       double ex, double ey, double ez, 
	       double bx, double by, double bz, int i)
{
  if (i < 0) i = _nhit;
  if (i < 0 || LMAX <= i) return -1;

  _xh[i] = x; _xs[i] = ex; _bx[i] = bx;
  _yh[i] = y; _ys[i] = ey; _by[i] = by;
  _zh[i] = z; _zs[i] = ez; _bz[i] = bz;

  if (_xs[i] > 0) _nhitx++;
  if (_ys[i] > 0) _nhity++;
  if (_xs[i] > 0 && _ys[i] > 0) _nhitxy++;

  if (i >= _nhit) _nhit = i+1;
  return _nhit;
}

double TrFit::Fit(int method)
{
  // Check number of hits
  if (_nhit < 3) return -1;

  if (method == LINEAR) return LinearFit();
  if (method == CIRCLE) return CircleFit();
  if (method == SIMPLE) return SimpleFit();
  if (method == NORMAL) return NormalFit();

  return 0;
}

double TrFit::LinearFit(void)
{
  /// Linear fitting in X-Z and Y-Z planes

  if (LinearFit(1) < 0) return -1;
  if (LinearFit(2) < 0) return -1;

  _p0x = _param[0]; _dxdz = _param[1];
  _p0y = _param[2]; _dydz = _param[3];
  _p0z = 0;

  _rigidity = _errrinv = 0;

  _chisq = (_ndofx+_ndofy > 0) ? (_chisqx+_chisqy)/(_ndofx+_ndofy) : -1;
  return _chisq;
}

double TrFit::CircleFit(void)
{
  /// Circlar fitting in Y-Z plane and liear fitting in S-Z plane

  if (MAGSFFKEY.magstat == 0) return LinearFit();

  if (CircleFit(2) < 0) return -1;
  if (LinearFit(3) < 0) return -1;

  // _p0x, _p0y and _p0z are defined as 
  // Point of Closest Approach (PCA) to Origin (0, 0, 0)
  double kappa = _param[2];
  double d0    = _param[3];
  double phi0  = _param[4];
  double kappe = _param[5];
  _p0x = _param[0];
  _p0y = -d0*std::sin(phi0);
  _p0z =  d0*std::cos(phi0);

  // _dxdz and _dydz are defined as a track direction at PCA
  _dydz = 1/std::tan(phi0);
  _dxdz = -_param[1]/std::sin(phi0);

  float p0[3] = { _p0x, _p0y, _p0z };
  float bf[3];
  MagField::GetPtr()->GuFld(p0, bf);

  double cosd    = -1/std::sqrt(1+_param[1]*_param[1]);
  double c_light = 2.99792458e+08;
  _rigidity =             1e-12*c_light*0.5/kappa*bf[0]/cosd;
  _errrinv  = std::abs(1/(1e-12*c_light*0.5/kappe*bf[0]/cosd));

  _chisq = (_ndofx+_ndofy > 0) ? (_chisqx+_chisqy)/(_ndofx+_ndofy) : -1;
  return _chisq;
}

double TrFit::LinearFit(int side)
{
  // Analytic linear fitting method

  double sh[LMAX];
  double *x   = (side == 2) ? _yh : _xh;
  double *sig = (side == 2) ? _ys : _xs;
  double *res = (side == 2) ? _yr : _xr;
  double *par = (side == 2) ? _param+2 : _param;
  double *y   = (side == 3) ? sh : _zh;

  // Fill S coordinate
  if (side == 3) {
    double kappa = _param[2];
    double d0    = _param[3];
    double phi0  = _param[4];
    if (kappa == 0) return -1;

    double cx = -(0.5/kappa+d0)*std::sin(phi0);
    double cy =  (0.5/kappa+d0)*std::cos(phi0);
    double ra = std::fabs(0.5/kappa);

    for (int i = 0; i < _nhit; i++) {
      double phi = std::atan2(-(_yh[i]-cx), _zh[i]-cy);
      double sgn = -1;
      if (phi < 0) { phi += M_PI; sgn = 1; }
      sh[i] = sgn*ra*(phi-(phi0-M_PI));
    }
  }

  // Set 2x2-matrix and 2-vector
  double mtx[2][2] = { { 0, 0 }, { 0, 0 } }, minv[2][2];
  double vec[2] = { 0, 0 };
   for (int i = 0; i < _nhit; i++) {
      double w = (sig[i] > 0) ? 1/sig[i]/sig[i] : 0;
      mtx[0][0] += w;      mtx[0][1] += w*y[i];
      mtx[1][0] += w*y[i]; mtx[1][1] += w*y[i]*y[i];
      vec[0]    += w*x[i]; vec[1]    += w*x[i]*y[i];
   }

   // Invert matrix
   double det = mtx[0][0]*mtx[1][1]-mtx[0][1]*mtx[1][0];
   if (det == 0) return -1;
   minv[0][0] = mtx[1][1]/det; minv[0][1] = -mtx[0][1]/det;
   minv[1][1] = mtx[0][0]/det; minv[1][0] = -mtx[1][0]/det;

   // Solution parameters
   par[0] = minv[0][0]*vec[0]+minv[0][1]*vec[1];
   par[1] = minv[1][0]*vec[0]+minv[1][1]*vec[1];

   // Fitting chisquare
   double chisq = 0;
   for (int i = 0; i < _nhit; i++) {
     res[i] = x[i]-(par[0]+par[1]*y[i]);
     chisq += (sig[i] > 0) ? res[i]*res[i]/sig[i]/sig[i] : 0;
   }

   int ndof = _nhit-2;
   if (side == 2) {
     _ndofy  = _nhity-2;
     _chisqy = chisq;
   } else {
     _ndofx  = _nhitx-2;
     _chisqx = chisq;
   }

   return chisq;
}

double TrFit::CircleFit(int side)
{
//////////////////////////////////////////////////////////////////////////
//  Effective circle fit - Karimaki's method
//  Nuclear Instruments and Methods
//          A305(1991) 187-191 by V.Karimaki
//
//  Note : The output values might have large truncation errors 
//         when the center of the track is near( <= 1% of R ) the origin.
//////////////////////////////////////////////////////////////////////////

  double *x   = (side == 1) ? _xh : _yh;
  double *sig = (side == 1) ? _xs : _ys;
  double *res = (side == 1) ? _xr : _yr;
  double *par = (side == 1) ? _param : _param+2;
  double *y   = _zh;

  double r[LMAX], w[LMAX];
  double sw = 0;
  for (int i = 0; i < _nhit; i++) {
    r[i] = x[i]*x[i]+y[i]*y[i];
    w[i] = (sig[i] > 0) ? 1/sig[i]/sig[i] : 1;
    sw += w[i];
  }
  if (sw <= 0) return -1;

  // Weighted average of x, y, r
  double xav  = VKAverage(_nhit, x, 0, w);
  double yav  = VKAverage(_nhit, y, 0, w);
  double rav  = VKAverage(_nhit, r, 0, w);
  double xxav = VKAverage(_nhit, x, x, w);
  double xyav = VKAverage(_nhit, x, y, w);
  double yyav = VKAverage(_nhit, y, y, w);
  double xrav = VKAverage(_nhit, x, r, w);
  double yrav = VKAverage(_nhit, y, r, w);
  double rrav = VKAverage(_nhit, r, r, w);

  // Covariances
  double cxx = xxav-xav*xav;
  double cxy = xyav-xav*yav;
  double cyy = yyav-yav*yav;
  double cxr = xrav-xav*rav;
  double cyr = yrav-yav*rav;
  double crr = rrav-rav*rav;
  if (crr == 0) return -1;

  // Track parameters in the zero-th order
  double q1    = crr*cxy-cxr*cyr;
  double q2    = crr*(cxx-cyy)-cxr*cxr+cyr*cyr;
  double phi   = std::atan2(2*q1, q2)/2.;
  double sphi  = std::sin(phi);
  double cphi  = std::cos(phi);
  double kappa = (sphi*cxr-cphi*cyr)/crr;
  double delta = -kappa*rav+sphi*xav-cphi*yav;
  if (1-4*delta*kappa <= 0 || kappa == 0) return -1;

  double dksqr = std::sqrt(1-4*delta*kappa);
  double rho   = 2.*kappa/dksqr;
  double d     = 2.*delta/(1+dksqr);
  double u     = 1+rho*d;
  double chisq = sw*u*u*(sphi*sphi*cxx-2*sphi*cphi*cxy+
			 cphi*cphi*cyy-kappa*kappa*crr);

  // Inversed covariance matrix Vinv
  double vinv[6];
  double sa = sphi*xav-cphi*yav;
  double sb = cphi*xav+sphi*yav;
  double sc = (sphi*sphi-cphi*cphi)*xyav+sphi*cphi*(xxav-yyav);
  double sd = sphi*xrav-cphi*yrav;
  double se = sphi*sphi*xxav-2*sphi*cphi*xyav+cphi*cphi*yyav;

  vinv[0] = rrav/4.-d*(sd-d*(se+rav/2.-d*(sa-d/4.)));
  vinv[1] = -u*((cphi*xrav+sphi*yrav)/2.-d*(sc-d*sb/2.));
  vinv[2] = u*u*(cphi*cphi*xxav+2*sphi*cphi*xyav+sphi*sphi*yyav);
  vinv[3] = rho*(-sd/2.+d*se)+u*rav/2.-d*((2*u+rho*d)*sa-u*d)/2.;
  vinv[4] = u*(rho*sc-u*sb);
  vinv[5] = rho*(rho*se-2*u*sa)+u*u;
  for (int i = 0; i < 6; i++) vinv[i] *= sw;

  // Invert vinv
  double det = vinv[0]*vinv[2]*vinv[5]+vinv[1]*vinv[3]*vinv[4]
              +vinv[1]*vinv[3]*vinv[4]-vinv[0]*vinv[4]*vinv[4]
              -vinv[1]*vinv[1]*vinv[5]-vinv[2]*vinv[3]*vinv[3];
  if (det == 0) return -1;

  double vmtx[6];
  vmtx[0] = (vinv[2]*vinv[5]-vinv[4]*vinv[4])/det;
  vmtx[1] = (vinv[3]*vinv[4]-vinv[1]*vinv[5])/det;
  vmtx[2] = (vinv[0]*vinv[5]-vinv[3]*vinv[3])/det;
  vmtx[3] = (vinv[1]*vinv[4]-vinv[2]*vinv[3])/det;
  vmtx[4] = (vinv[1]*vinv[3]-vinv[0]*vinv[4])/det;
  vmtx[5] = (vinv[0]*vinv[2]-vinv[1]*vinv[1])/det;
  if (vmtx[0] < 0) return -1;

  // First ordered corrections of parameters
  double sigma = -rho*sd+2*u*se-d*(1+u)*sa;
  rho -= (vmtx[0]*d*sigma+vmtx[3]*rho*sigma)/2.;
  phi -= (vmtx[1]*d*sigma+vmtx[4]*rho*sigma)/2.;
  d   -= (vmtx[3]*d*sigma+vmtx[5]*rho*sigma)/2.;

  if (phi > 0) {
    par[0] = rho/2;
    par[1] = d;
    par[2] = phi+M_PI;
  } else {
    par[0] = -rho/2;
    par[1] = -d;
    par[2] = phi+2*M_PI;
  }

  // Error of kappa
  par[3] = std::sqrt(vmtx[0])/2;

  // Fitting residual
  for (int i = 0; i < _nhit; i++) res[i] = VKResidual(x[i], y[i], par);

  int ndof = _nhit-3;
  if (side == 1) {
    _ndofx  = ndof;
    _chisqx = chisq;
  } else {
    _ndofy  = ndof;
    _chisqy = chisq;
  }

  return chisq;
}

double TrFit::PolynomialFit(int side, int ndim)
{
  // Analytic polynominal fitting, ndim=2,3,4 are available

  // check number of dimensions
  if (ndim <= 0) return -1;
  if (ndim >  4) return -1;
  if (ndim == 1) return LinearFit(side);

  // check number of hits
  int npar = ndim+1;
  if (_nhit < npar) return -1;

  double *x   = (side == 1) ? _xh : _yh;
  double *sig = (side == 1) ? _xs : _ys;
  double *res = (side == 1) ? _xr : _yr;
  double *par = _param;
  double *y   = _zh;

  // Define [npar][npar] matrix and [npar] vector
  double mtx[npar*npar], vec[npar];
  for (int i = 0; i < npar; i++) {
    for (int j = 0; j < npar; j++) mtx[i*npar+j] = 0;
    vec[i] = 0;
  }

  // Fill matrix and vector
  for (int i = 0; i < _nhit; i++) {
    double m0 = (sig[i] > 0) ? 1/sig[i]/sig[i] : 0;
    for (int j = 0; j < npar; j++) {
      double m = m0;
      for (int k = j; k < npar; k++) {
        mtx[j*npar+k] += m;
        if (j != k) mtx[k*npar+j] += m;
        if (j == 0) vec[k] += m*x[i];
        m *= y[i];
      }
      m0 *= y[i]*y[i];
    }
  }

  // Invert matrix
  int ret = -1;
  if (npar == 3) ret = Inv33((double(*)[3])mtx);
  if (npar == 4) ret = Inv44((double(*)[4])mtx);
  if (npar == 5) ret = Inv55((double(*)[5])mtx);
  if (ret < 0) return -1;

  // Get the parameters
  for (int i = 0; i < npar; i++) {
    par[i] = 0;
    for (int j = 0; j < npar; j++)
      par[i] += mtx[i*npar+j]*vec[j];
  }

  // Get chisquare
  double chisq = 0;
  int ndof = 0;
  for (int i = 0; i < _nhit; i++) {
    double xx = 0, yy = 1;
    for (int j = 0; j < npar; j++) {
      xx += yy*par[j];
      yy *= y[i];
    }
    res[i] = x[i]-xx;
    if (sig[i] > 0) {
      chisq += res[i]*res[i]/sig[i]/sig[i];
      ndof++;
    }
  }

  ndof -= npar;
  if (side == 1) {
    _ndofx  = ndof;
    _chisqx = chisq;
  }
  else {
    _ndofy  = ndof;
    _chisqy = chisq;
  }
  
  return chisq;
}

double TrFit::VKAverage(int n, double *x, double *y, double *w)
{
   double s = 0, sw = 0;
   for (int i = 0; i < n; i++) {
      s  += (y) ? x[i]*y[i]*w[i] : x[i]*w[i];
      sw += w[i];
   }
   return (sw <= 0) ? 0 : s/sw;
}

double TrFit::VKResidual(double x, double y, double *par)
{
   if (par[0] == 0 || par[2] == 0) return 0;

   double cx = -(0.5/par[0]+par[1])*std::sin(par[2]);
   double cy =  (0.5/par[0]+par[1])*std::cos(par[2]);
   double dx = x-cx;
   double dy = y-cy;
   double gr = -dy/dx;
   double a  = -1/gr;

   double rr = std::fabs(0.5/par[0]);
   double aa = a*a+1;
   double bb = a*dx-a*a*y-cy;
   double cc = (a*y-dx)*(a*y-dx)+cy*cy-rr*rr;
   double dd = bb*bb-aa*cc;
   if (dd <= 0) return 0;

   double sg[2] = { 0, 0 }, rs[2] = { 0, 0 };
   for (int i = 0; i < 2; i++) {
     double yy = (-bb+(i*2-1)*std::sqrt(dd))/aa;
     double xx = a*(yy-y)+x;
     sg[i] = (x > xx) ? 1 : -1;
     rs[i] = std::sqrt((xx-x)*(xx-x)+(yy-y)*(yy-y));
   }

   return (rs[0] < rs[1]) ? rs[0]*sg[0] : rs[1]*sg[1];
}

double TrFit::SimpleFit(void)
{
  double len[LMAX];

  // Length
  for (int i = 0; i < _nhit; i++) {
    len[i] = -1;
    if (_xs[i] <= 0 && _ys[i] <= 0 && _zs[i] <= 0) continue;

    len[i] = (i == 0) ? 0 : std::sqrt((_xh[i]-_xh[i-1])*(_xh[i]-_xh[i-1])
				     +(_yh[i]-_yh[i-1])*(_yh[i]-_yh[i-1])
				     +(_zh[i]-_zh[i-1])*(_zh[i]-_zh[i-1]));
  }

  double pintx[LMAX][3];
  double pintu[LMAX][3];

  // Calculate path integrals
  for (int i = 0; i < _nhit; i++) {
    if (i == 0 || len[i] <= 0) {
      for (int j = 0; j < 3; j++) pintx[0][j] = pintu[0][j] = 0;
      continue;
    }
     
    double u[3];
    u[0] = (_xh[i]-_xh[i-1])/len[i];
    u[1] = (_yh[i]-_yh[i-1])/len[i];
    u[2] = (_zh[i]-_zh[i-1])/len[i];

    double bax = (_bx[i-1]+_bx[i])/2;
    double bay = (_by[i-1]+_by[i])/2;
    double baz = (_bz[i-1]+_bz[i])/2;

    pintx[i][0] = (u[1]*_bz[i-1]-u[2]*_by[i-1])/2;
    pintx[i][1] = (u[2]*_bx[i-1]-u[0]*_bz[i-1])/2;
    pintx[i][2] = (u[0]*_by[i-1]-u[1]*_bx[i-1])/2;

    pintu[i][0] = u[1]*baz-u[2]*bay;
    pintu[i][1] = u[2]*bax-u[0]*baz;
    pintu[i][2] = u[0]*bay-u[1]*bax;
  }

  // F and G matrices
  double d[2*LMAX][NDIM];
  for (int i = 0; i < _nhit; i++) {
    int ix = i, iy = i+_nhit;

    for (int j = 0; j < NDIM; j++) d[ix][j] = d[iy][j] = 0;
    d[ix][0] = d[iy][1] = 1;
    
    for (int j = 0; j <= i; j++) {
      d[ix][2] += len[j];
      d[iy][3] += len[j];

      for (int k = 0; k <= j; k++) {
	if (k == j) {
	  d[ix][4] += len[j]*len[j]*pintx[j][0];
	  d[iy][4] += len[j]*len[j]*pintx[j][1];
	} else {
	  d[ix][4] += len[j]*len[k]*pintu[k][0];
	  d[iy][4] += len[j]*len[k]*pintu[k][1];
	}
      }
    }
  }

  // dx = F*S_x*x + G*S_y*y
  double dx[NDIM];
  for (int j = 0; j < NDIM; j++) {
    dx[j] = 0;
    for (int i = 0; i < _nhit; i++) {
      int ix = i, iy = i+_nhit;
      if (_xs[i] > 0) dx[j] += d[ix][j]/_xs[i]/_xs[i]*_xh[i]*1e-8;
      if (_ys[i] > 0) dx[j] += d[iy][j]/_ys[i]/_ys[i]*_yh[i]*1e-8;
    }
  }

  // cov = F*S_x*F + G*S_y*G
  double cov[NDIM][NDIM];
  for (int j = 0; j < NDIM; j++) {
    for (int k = 0; k < NDIM; k++) {
      cov[j][k] = 0.;
      for (int i = 0; i < _nhit; i++) {
        int ix = i, iy = i+_nhit;
        if (_xs[i] > 0) cov[j][k] += d[ix][j]/_xs[i]/_xs[i]*d[ix][k]*1e-8;
        if (_ys[i] > 0) cov[j][k] += d[iy][j]/_ys[i]/_ys[i]*d[iy][k]*1e-8;
      } 
    }
  }
        
  // cov^{-1}
  if (Inv55(cov)) return -1;

  // Solution
  for (int k = 0; k < NDIM; k++) {
    _param[k] = 0;
    for (int i = 0; i < NDIM; i++) _param[k] += cov[k][i]*dx[i];
  }

  // Chisquare
  _chisqx = _chisqy = 0;
  _ndofx = _ndofy = 0;
  for (int i = 0; i <_nhit; i++) {
    _xr[i] = _xh[i];
    _yr[i] = _yh[i];

    for (int k = 0; k < NDIM; k++) {
      int ix = i, iy = i+_nhit;
      _xr[i] -= d[ix][k]*_param[k];
      _yr[i] -= d[iy][k]*_param[k];
    }
    if (_xs[i] > 0) {
      _chisqx += _xr[i]*_xr[i]/_xs[i]/_xs[i]; _ndofx++;
    }
    if (_ys[i] > 0) {
      _chisqy += _yr[i]*_yr[i]/_ys[i]/_ys[i]; _ndofy++;
    }
  }

  _ndofx -= 2;
  _ndofy -= 3;
  _chisq = (_ndofx+_ndofy > 0) ? (_chisqx+_chisqy)/(_ndofx+_ndofy) : -1;

  _p0x = _param[0]; _dxdz = -_param[2];
  _p0y = _param[1]; _dydz = -_param[3];
  _p0z = _zh[0];

  _rigidity = (_param[4] != 0) ? 2.997E-4/_param[4] : 0;

  return _chisq;
}

double TrFit::NormalFit(void)
{
  if(_nhit > LMAX || 2*_nhit <= 5 || _chrg == 0) return -1;

  for (int i = 0; i < _nhit; i++) {
    if (_xs[i] <= 0) _xs[i] = _zs[i]*1e8;
    if (_ys[i] <= 0) _ys[i] = _zs[i]*1e8;
  }

  // Set initial parameters with SimpleFit
  SimpleFit();
  _param[0] = _p0x; _param[1] = _dxdz;
  _param[2] = _p0y; _param[3] = _dydz;
  _param[4] = 1/_rigidity;

  int maxcal = 100;

  double tol = 1.e-2;

  // Multiple scattering constant
  double xls = 300.e-4;
  double rls = 9.36;
  double sms = 13.6e-3*std::sqrt(xls/rls);

  double chisqb  = -1;
  double chisqbb = -1;
  double resmy;

  int ifail = 2;

  double init[7], out[7];

  for (int kiter = 0; kiter < maxcal; kiter++) {
    double dnorm = 1./std::sqrt(1+_param[1]*_param[1]+_param[3]*_param[3]);
    init[0] =  _param[0];
    init[1] =  _param[2];
    init[2] =  _zh[0]+1e-4;
    init[3] = -_param[1]*dnorm;
    init[4] = -_param[3]*dnorm;
    init[5] = -dnorm;
    init[6] = (_param[4] != 0) ? _chrg/_param[4] : 1.e10;

    double mm[NDIM][NDIM], gg[NDIM][NDIM], g[NDIM];
    for (int i = 0; i < NDIM*NDIM; i++) mm[i/NDIM][i%NDIM] = 0;
    for (int i = 0; i < NDIM*NDIM; i++) gg[i/NDIM][i%NDIM] = 0;
    for (int i = 0; i < NDIM; i++) g[i] = 0;
    for (int i = 0; i < NDIM; i++) mm[i][i] = 1;

    double fact[LMAX], xmsr[LMAX][LMAX], xms[LMAX], fckx[LMAX], fcky[LMAX];
    resmy = 0;

    // Loop for each point
    for (int i = 0; i < _nhit; i++) {

      double point[6] = { 0, 0, _zh[i], 0, 0, -1 };

      // Transportation
      if (i > 0) VCFitPar(init, out, point, mm, (i <= 1) ? 0 : 1);
      else for (int j = 0; j < 7; j++) out[j] = init[j];

      // Multiple scattering factor
      double dnm = init[5];
      double mmt = init[6];
      if (_ims == 0 || i <= 1) fact[i] = 0;
      else {
	double beta = std::max(fabs(mmt/sqrt(mmt*mmt+_xmass*_xmass)), 0.1);
	fact[i] = (sms*_chrg/mmt/beta)*(sms*_chrg/mmt/beta)/fabs(dnm);
      }

      // Multiple scattering matrix
      for (int im = i; im >= 0; im--) {
	xmsr[i][im] = 0;
	for (int il = 1; il <= std::min(i,im)-1; il++)
	  xmsr[i][im] += (_zh[i]-_zh[il])*(_zh[im]-_zh[il])*fact[il+1];

	if (im == i) xms[i] = xmsr[i][i];
	else {
	  double tmp1 = 2*xms[i]+(_xs[i]*_xs[i]+
				  _ys[i]*_ys[i])*dnm*dnm
	             +(1-dnm*dnm)*_zs[i]*_zs[i]+1.e-10;
	  double tmp2 = 2*xms[im]+(_xs[im]*_xs[im]+
				   _ys[im]*_ys[im])*dnm*dnm
	              +(1-dnm*dnm)*_zs[im]*_zs[im]+1.e-10;
	  xmsr[i][im] = 2*xmsr[i][im]/std::sqrt(tmp1*tmp2);
	}
      }

      // Error matrix
      double aa[5];
      VCErrMtx(i, xms[i], out, aa, fckx[i], fcky[i]);

      // Fill residual
      _xr[i] = -aa[3];
      _yr[i] = -aa[4];
      if (_ys[i] > 0) resmy += _yr[i]/_nhit;

      // Covariance matrix
      for (int j = 0; j < NDIM; j++) {
        g[j] += 2*(mm[j][0]*(aa[0]*aa[3]+aa[1]*aa[4])+
		   mm[j][2]*(aa[2]*aa[4]+aa[1]*aa[3]));

	for (int k = 0; k < NDIM; k++)
	  gg[j][k] += 2*(aa[0]* mm[k][0]*mm[j][0]+
			 aa[2]* mm[k][2]*mm[j][2]+
			 aa[1]*(mm[k][0]*mm[j][2]+mm[k][2]*mm[j][0]));
      }

      for (int j = 0; j < 7; j++) init[j] = out[j];
    }

    // Chisquare and residual
    _chisqx = _chisqy = _chisq = 0;
    for (int i = 0; i < _nhit; i++) {
      _chisqx += std::abs(fckx[i]);
      _chisqy += std::abs(fcky[i]);
      _chisq  += std::abs(fckx[i])+std::abs(fcky[i]);
      for (int k = i+1; k < _nhit; k++) {
        _chisqx += std::sqrt(fabs( fckx[i]*fckx[k]) )*xmsr[k][i];
        _chisqy += std::sqrt(fabs( fcky[i]*fcky[k]) )*xmsr[k][i];
	_chisq  += std::sqrt(fabs((fckx[i]+fcky[i])
				 *(fckx[k]+fcky[k])))*xmsr[k][i];

      }
    }

    // Check chisquare difference
    double dlt  = (_chisq-chisqb )/(chisqb +2*_nhit-5+1.e-3);
    double dltb = (_chisq-chisqbb)/(chisqbb+2*_nhit-5+1.e-3);

    if (fabs(dlt) <= tol || fabs(dltb) <= tol) {
      ifail = 0;
      break;
    }
    chisqbb = chisqb;
    chisqb  = _chisq;

    // Invert covariance matrix
    if (TrFit::Inv55(gg) < 0) return -1;

    // Error of rigidity
    _errrinv = std::sqrt(2*gg[4][4]);

    // Solved parameters
    for (int i = 0; i < NDIM; i++) {
      double sol = (gg[0][i]*g[0]+gg[1][i]*g[1]+
		    gg[2][i]*g[2]+gg[3][i]*g[3]+gg[4][i]*g[4]);
      _param[i] += (i <= 3) ? -sol : sol;
    }

    // Check parameter range
    if (_param[4] <= -10) _param[4] = -10;
    if (_param[4] >=  10) _param[4] =  10;
  }

  // A small tuning
  if (std::abs(resmy) < 0.01) _param[2] -= resmy;
  for (int i = 0; i < _nhit; i++) {
    if (_ys[i] > 0) {
      _chisq  -= (_yr[i]*_yr[i]-(_yr[i]-resmy)*(_yr[i]-resmy))/_ys[i]/_ys[i];
      _chisqy -= (_yr[i]*_yr[i]-(_yr[i]-resmy)*(_yr[i]-resmy))/_ys[i]/_ys[i];
    }
    _yr[i] -= resmy;
  }

  if (ifail) return -1;

  _p0x = _param[0]; _dxdz = _param[1];
  _p0y = _param[2]; _dydz = _param[3];
  _p0z = _zh[0];

  _param[4] = _param[4]*_chrg/std::fabs(_chrg);
  _rigidity = (_param[4] != 0) ? 1/_param[4] : 0;

  _ndofx = _nhit-2;
  _ndofy = _nhit-3;
  _chisq = (_ndofx+_ndofy > 0) ? _chisq/(_ndofx+_ndofy) : -1;

  return _chisq;
}

void TrFit::VCErrMtx(int ih, double xms, double *out, double *aa,
		     double &fckx, double &fcky)
{
  double mtmp[3];
  mtmp[0] = _xs[ih]*_xs[ih]+out[3]*out[3]*_zs[ih]*_zs[ih]/out[5]/out[5];
  mtmp[2] = _ys[ih]*_ys[ih]+out[4]*out[4]*_zs[ih]*_zs[ih]/out[5]/out[5];
  mtmp[1] = 0;

  if (_ims != 0) {
    double vv = xms/out[5]/out[5]/out[5]/out[5];
    mtmp[0] += vv*(1-out[4]*out[4]);
    mtmp[2] += vv*(1-out[3]*out[3]);
    mtmp[1] += vv*out[3]*out[4];
  }

  double det = mtmp[0]*mtmp[2]-mtmp[1]*mtmp[1];
  if (det != 0) {
    aa[0] =  mtmp[2]/det; 
    aa[1] = -mtmp[1]/det;
    aa[2] =  mtmp[0]/det; 
  }
  else for (int i = 0; i < 3; i++) aa[i] = 0;

  double xperp[4];
  xperp[0] = out[0]-out[3]/out[5]*out[2];
  xperp[1] = out[1]-out[4]/out[5]*out[2];
  xperp[2] = _xh[ih]-out[3]/out[5]*_zh[ih];
  xperp[3] = _yh[ih]-out[4]/out[5]*_zh[ih];

  aa[3] = xperp[0]-xperp[2];
  aa[4] = xperp[1]-xperp[3];

  fckx = fcky = 0;
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 2; j++) {
      double fck = (xperp[i]-xperp[i+2])*aa[i+j]*(xperp[j]-xperp[j+2]);
      if (i == 0) fckx += fck;
      else        fcky += fck;
    }
}


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TMatrixTCramerInv                                                    //
//                                                                      //
// Authors: Fons Rademakers, Eddy Offermann  Jan 2004                   //
//                                                                      //
// Encapsulate templates of Cramer Inversion routines.                  //
//                                                                      //
// Adapted from routines written by                                     //
// Mark Fischler and Steven Haywood as part of the CLHEP package        //
//                                                                      //
// Although for sizes <= 6x6 the Cramer Inversion has a gain in speed   //
// compared to factorization schemes (like LU) , one pays a price in    //
// accuracy                                                             //
//                                                                      //
// For Example:                                                         //
//  H * H^-1 = U, where H is a 5x5 Hilbert matrix                       //
//                      U is a 5x5 Unity matrix                         //
//                                                                      //
// LU    : |U_jk| < 10e-13 for  j!=k                                    //
// Cramer: |U_jk| < 10e-7  for  j!=k                                    //
//                                                                      //
//  however Cramer algorithm is about 10 (!) times faster               //
//////////////////////////////////////////////////////////////////////////
//
// Imported by SH

int TrFit::Inv33(double M[3][3])
{
  double c00 = M[1][1] * M[2][2] - M[1][2] * M[2][1];
  double c01 = M[1][2] * M[2][0] - M[1][0] * M[2][2];
  double c02 = M[1][0] * M[2][1] - M[1][1] * M[2][0];
  double c10 = M[2][1] * M[0][2] - M[2][2] * M[0][1];
  double c11 = M[2][2] * M[0][0] - M[2][0] * M[0][2];
  double c12 = M[2][0] * M[0][1] - M[2][1] * M[0][0];
  double c20 = M[0][1] * M[1][2] - M[0][2] * M[1][1];
  double c21 = M[0][2] * M[1][0] - M[0][0] * M[1][2];
  double c22 = M[0][0] * M[1][1] - M[0][1] * M[1][0];

  double t0 = std::abs(M[0][0]);
  double t1 = std::abs(M[1][0]);
  double t2 = std::abs(M[2][0]);
  double det, tmp;
  if (t0 >= t1) {
    if (t2 >= t0) {
      tmp = M[2][0];
      det = c12*c01-c11*c02;
    } else {
      tmp = M[0][0];
      det = c11*c22-c12*c21;
    }
  } else if (t2 >= t1) {
    tmp = M[2][0];
    det = c12*c01-c11*c02;
  } else {
    tmp = M[1][0];
    det = c02*c21-c01*c22;
  }
  if ( det == 0 || tmp == 0) return -1;

  double s = tmp/det;
  M[0][0] = s*c00;
  M[0][1] = s*c10;
  M[0][2] = s*c20;
  M[1][0] = s*c01;
  M[1][1] = s*c11;
  M[1][2] = s*c21;
  M[2][0] = s*c02;
  M[2][1] = s*c12;
  M[2][2] = s*c22;
  
  return 0;
}

int TrFit::Inv44(double M[4][4])
{
  // Find all NECESSARY 2x2 dets:  (18 of them)
  double d2_12_01 = M[1][0]*M[2][1] - M[1][1]*M[2][0];
  double d2_12_02 = M[1][0]*M[2][2] - M[1][2]*M[2][0];
  double d2_12_03 = M[1][0]*M[2][3] - M[1][3]*M[2][0];
  double d2_12_13 = M[1][1]*M[2][3] - M[1][3]*M[2][1];
  double d2_12_23 = M[1][2]*M[2][3] - M[1][3]*M[2][2];
  double d2_12_12 = M[1][1]*M[2][2] - M[1][2]*M[2][1];
  double d2_13_01 = M[1][0]*M[3][1] - M[1][1]*M[3][0];
  double d2_13_02 = M[1][0]*M[3][2] - M[1][2]*M[3][0];
  double d2_13_03 = M[1][0]*M[3][3] - M[1][3]*M[3][0];
  double d2_13_12 = M[1][1]*M[3][2] - M[1][2]*M[3][1];
  double d2_13_13 = M[1][1]*M[3][3] - M[1][3]*M[3][1];
  double d2_13_23 = M[1][2]*M[3][3] - M[1][3]*M[3][2];
  double d2_23_01 = M[2][0]*M[3][1] - M[2][1]*M[3][0];
  double d2_23_02 = M[2][0]*M[3][2] - M[2][2]*M[3][0];
  double d2_23_03 = M[2][0]*M[3][3] - M[2][3]*M[3][0];
  double d2_23_12 = M[2][1]*M[3][2] - M[2][2]*M[3][1];
  double d2_23_13 = M[2][1]*M[3][3] - M[2][3]*M[3][1];
  double d2_23_23 = M[2][2]*M[3][3] - M[2][3]*M[3][2];

  // Find all NECESSARY 3x3 dets:   (16 of them)
  double d3_012_012 = M[0][0]*d2_12_12 - M[0][1]*d2_12_02 + M[0][2]*d2_12_01;
  double d3_012_013 = M[0][0]*d2_12_13 - M[0][1]*d2_12_03 + M[0][3]*d2_12_01;
  double d3_012_023 = M[0][0]*d2_12_23 - M[0][2]*d2_12_03 + M[0][3]*d2_12_02;
  double d3_012_123 = M[0][1]*d2_12_23 - M[0][2]*d2_12_13 + M[0][3]*d2_12_12;
  double d3_013_012 = M[0][0]*d2_13_12 - M[0][1]*d2_13_02 + M[0][2]*d2_13_01;
  double d3_013_013 = M[0][0]*d2_13_13 - M[0][1]*d2_13_03 + M[0][3]*d2_13_01;
  double d3_013_023 = M[0][0]*d2_13_23 - M[0][2]*d2_13_03 + M[0][3]*d2_13_02;
  double d3_013_123 = M[0][1]*d2_13_23 - M[0][2]*d2_13_13 + M[0][3]*d2_13_12;
  double d3_023_012 = M[0][0]*d2_23_12 - M[0][1]*d2_23_02 + M[0][2]*d2_23_01;
  double d3_023_013 = M[0][0]*d2_23_13 - M[0][1]*d2_23_03 + M[0][3]*d2_23_01;
  double d3_023_023 = M[0][0]*d2_23_23 - M[0][2]*d2_23_03 + M[0][3]*d2_23_02;
  double d3_023_123 = M[0][1]*d2_23_23 - M[0][2]*d2_23_13 + M[0][3]*d2_23_12;
  double d3_123_012 = M[1][0]*d2_23_12 - M[1][1]*d2_23_02 + M[1][2]*d2_23_01;
  double d3_123_013 = M[1][0]*d2_23_13 - M[1][1]*d2_23_03 + M[1][3]*d2_23_01;
  double d3_123_023 = M[1][0]*d2_23_23 - M[1][2]*d2_23_03 + M[1][3]*d2_23_02;
  double d3_123_123 = M[1][1]*d2_23_23 - M[1][2]*d2_23_13 + M[1][3]*d2_23_12;

  // Find the 4x4 det:
  double det = M[0][0]*d3_123_123 - M[0][1]*d3_123_023 
             + M[0][2]*d3_123_013 - M[0][3]*d3_123_012;
  if (det == 0) return -1;

  double oneOverDet = 1.0/det;
  double mn1OverDet = -oneOverDet;

  M[0][0] =  d3_123_123 * oneOverDet;
  M[0][1] =  d3_023_123 * mn1OverDet;
  M[0][2] =  d3_013_123 * oneOverDet;
  M[0][3] =  d3_012_123 * mn1OverDet;

  M[1][0] =  d3_123_023 * mn1OverDet;
  M[1][1] =  d3_023_023 * oneOverDet;
  M[1][2] =  d3_013_023 * mn1OverDet;
  M[1][3] =  d3_012_023 * oneOverDet;

  M[2][0] =  d3_123_013 * oneOverDet;
  M[2][1] =  d3_023_013 * mn1OverDet;
  M[2][2] =  d3_013_013 * oneOverDet;
  M[2][3] =  d3_012_013 * mn1OverDet;

  M[3][0] =  d3_123_012 * mn1OverDet;
  M[3][1] =  d3_023_012 * oneOverDet;
  M[3][2] =  d3_013_012 * mn1OverDet;
  M[3][3] =  d3_012_012 * oneOverDet;

  return 0;
}

int TrFit::Inv55(double M[5][5])
{
  // Find all NECESSARY 2x2 dets:  (30 of them)
  double d2_23_01 = M[2][0]*M[3][1] - M[2][1]*M[3][0];
  double d2_23_02 = M[2][0]*M[3][2] - M[2][2]*M[3][0];
  double d2_23_03 = M[2][0]*M[3][3] - M[2][3]*M[3][0];
  double d2_23_04 = M[2][0]*M[3][4] - M[2][4]*M[3][0];
  double d2_23_12 = M[2][1]*M[3][2] - M[2][2]*M[3][1];
  double d2_23_13 = M[2][1]*M[3][3] - M[2][3]*M[3][1];
  double d2_23_14 = M[2][1]*M[3][4] - M[2][4]*M[3][1];
  double d2_23_23 = M[2][2]*M[3][3] - M[2][3]*M[3][2];
  double d2_23_24 = M[2][2]*M[3][4] - M[2][4]*M[3][2];
  double d2_23_34 = M[2][3]*M[3][4] - M[2][4]*M[3][3];
  double d2_24_01 = M[2][0]*M[4][1] - M[2][1]*M[4][0];
  double d2_24_02 = M[2][0]*M[4][2] - M[2][2]*M[4][0];
  double d2_24_03 = M[2][0]*M[4][3] - M[2][3]*M[4][0];
  double d2_24_04 = M[2][0]*M[4][4] - M[2][4]*M[4][0];
  double d2_24_12 = M[2][1]*M[4][2] - M[2][2]*M[4][1];
  double d2_24_13 = M[2][1]*M[4][3] - M[2][3]*M[4][1];
  double d2_24_14 = M[2][1]*M[4][4] - M[2][4]*M[4][1];
  double d2_24_23 = M[2][2]*M[4][3] - M[2][3]*M[4][2];
  double d2_24_24 = M[2][2]*M[4][4] - M[2][4]*M[4][2];
  double d2_24_34 = M[2][3]*M[4][4] - M[2][4]*M[4][3];
  double d2_34_01 = M[3][0]*M[4][1] - M[3][1]*M[4][0];
  double d2_34_02 = M[3][0]*M[4][2] - M[3][2]*M[4][0];
  double d2_34_03 = M[3][0]*M[4][3] - M[3][3]*M[4][0];
  double d2_34_04 = M[3][0]*M[4][4] - M[3][4]*M[4][0];
  double d2_34_12 = M[3][1]*M[4][2] - M[3][2]*M[4][1];
  double d2_34_13 = M[3][1]*M[4][3] - M[3][3]*M[4][1];
  double d2_34_14 = M[3][1]*M[4][4] - M[3][4]*M[4][1];
  double d2_34_23 = M[3][2]*M[4][3] - M[3][3]*M[4][2];
  double d2_34_24 = M[3][2]*M[4][4] - M[3][4]*M[4][2];
  double d2_34_34 = M[3][3]*M[4][4] - M[3][4]*M[4][3];

  // Find all NECESSARY 3x3 dets:   (40 of them)
  double d3_123_012 = M[1][0]*d2_23_12 - M[1][1]*d2_23_02 + M[1][2]*d2_23_01;
  double d3_123_013 = M[1][0]*d2_23_13 - M[1][1]*d2_23_03 + M[1][3]*d2_23_01;
  double d3_123_014 = M[1][0]*d2_23_14 - M[1][1]*d2_23_04 + M[1][4]*d2_23_01;
  double d3_123_023 = M[1][0]*d2_23_23 - M[1][2]*d2_23_03 + M[1][3]*d2_23_02;
  double d3_123_024 = M[1][0]*d2_23_24 - M[1][2]*d2_23_04 + M[1][4]*d2_23_02;
  double d3_123_034 = M[1][0]*d2_23_34 - M[1][3]*d2_23_04 + M[1][4]*d2_23_03;
  double d3_123_123 = M[1][1]*d2_23_23 - M[1][2]*d2_23_13 + M[1][3]*d2_23_12;
  double d3_123_124 = M[1][1]*d2_23_24 - M[1][2]*d2_23_14 + M[1][4]*d2_23_12;
  double d3_123_134 = M[1][1]*d2_23_34 - M[1][3]*d2_23_14 + M[1][4]*d2_23_13;
  double d3_123_234 = M[1][2]*d2_23_34 - M[1][3]*d2_23_24 + M[1][4]*d2_23_23;
  double d3_124_012 = M[1][0]*d2_24_12 - M[1][1]*d2_24_02 + M[1][2]*d2_24_01;
  double d3_124_013 = M[1][0]*d2_24_13 - M[1][1]*d2_24_03 + M[1][3]*d2_24_01;
  double d3_124_014 = M[1][0]*d2_24_14 - M[1][1]*d2_24_04 + M[1][4]*d2_24_01;
  double d3_124_023 = M[1][0]*d2_24_23 - M[1][2]*d2_24_03 + M[1][3]*d2_24_02;
  double d3_124_024 = M[1][0]*d2_24_24 - M[1][2]*d2_24_04 + M[1][4]*d2_24_02;
  double d3_124_034 = M[1][0]*d2_24_34 - M[1][3]*d2_24_04 + M[1][4]*d2_24_03;
  double d3_124_123 = M[1][1]*d2_24_23 - M[1][2]*d2_24_13 + M[1][3]*d2_24_12;
  double d3_124_124 = M[1][1]*d2_24_24 - M[1][2]*d2_24_14 + M[1][4]*d2_24_12;
  double d3_124_134 = M[1][1]*d2_24_34 - M[1][3]*d2_24_14 + M[1][4]*d2_24_13;
  double d3_124_234 = M[1][2]*d2_24_34 - M[1][3]*d2_24_24 + M[1][4]*d2_24_23;
  double d3_134_012 = M[1][0]*d2_34_12 - M[1][1]*d2_34_02 + M[1][2]*d2_34_01;
  double d3_134_013 = M[1][0]*d2_34_13 - M[1][1]*d2_34_03 + M[1][3]*d2_34_01;
  double d3_134_014 = M[1][0]*d2_34_14 - M[1][1]*d2_34_04 + M[1][4]*d2_34_01;
  double d3_134_023 = M[1][0]*d2_34_23 - M[1][2]*d2_34_03 + M[1][3]*d2_34_02;
  double d3_134_024 = M[1][0]*d2_34_24 - M[1][2]*d2_34_04 + M[1][4]*d2_34_02;
  double d3_134_034 = M[1][0]*d2_34_34 - M[1][3]*d2_34_04 + M[1][4]*d2_34_03;
  double d3_134_123 = M[1][1]*d2_34_23 - M[1][2]*d2_34_13 + M[1][3]*d2_34_12;
  double d3_134_124 = M[1][1]*d2_34_24 - M[1][2]*d2_34_14 + M[1][4]*d2_34_12;
  double d3_134_134 = M[1][1]*d2_34_34 - M[1][3]*d2_34_14 + M[1][4]*d2_34_13;
  double d3_134_234 = M[1][2]*d2_34_34 - M[1][3]*d2_34_24 + M[1][4]*d2_34_23;
  double d3_234_012 = M[2][0]*d2_34_12 - M[2][1]*d2_34_02 + M[2][2]*d2_34_01;
  double d3_234_013 = M[2][0]*d2_34_13 - M[2][1]*d2_34_03 + M[2][3]*d2_34_01;
  double d3_234_014 = M[2][0]*d2_34_14 - M[2][1]*d2_34_04 + M[2][4]*d2_34_01;
  double d3_234_023 = M[2][0]*d2_34_23 - M[2][2]*d2_34_03 + M[2][3]*d2_34_02;
  double d3_234_024 = M[2][0]*d2_34_24 - M[2][2]*d2_34_04 + M[2][4]*d2_34_02;
  double d3_234_034 = M[2][0]*d2_34_34 - M[2][3]*d2_34_04 + M[2][4]*d2_34_03;
  double d3_234_123 = M[2][1]*d2_34_23 - M[2][2]*d2_34_13 + M[2][3]*d2_34_12;
  double d3_234_124 = M[2][1]*d2_34_24 - M[2][2]*d2_34_14 + M[2][4]*d2_34_12;
  double d3_234_134 = M[2][1]*d2_34_34 - M[2][3]*d2_34_14 + M[2][4]*d2_34_13;
  double d3_234_234 = M[2][2]*d2_34_34 - M[2][3]*d2_34_24 + M[2][4]*d2_34_23;

  // Find all NECESSARY 4x4 dets:   (25 of them)
  double d4_0123_0123 = M[0][0]*d3_123_123 - M[0][1]*d3_123_023 
                      + M[0][2]*d3_123_013 - M[0][3]*d3_123_012;
  double d4_0123_0124 = M[0][0]*d3_123_124 - M[0][1]*d3_123_024 
                      + M[0][2]*d3_123_014 - M[0][4]*d3_123_012;
  double d4_0123_0134 = M[0][0]*d3_123_134 - M[0][1]*d3_123_034 
                      + M[0][3]*d3_123_014 - M[0][4]*d3_123_013;
  double d4_0123_0234 = M[0][0]*d3_123_234 - M[0][2]*d3_123_034 
                      + M[0][3]*d3_123_024 - M[0][4]*d3_123_023;
  double d4_0123_1234 = M[0][1]*d3_123_234 - M[0][2]*d3_123_134 
                      + M[0][3]*d3_123_124 - M[0][4]*d3_123_123;
  double d4_0124_0123 = M[0][0]*d3_124_123 - M[0][1]*d3_124_023 
                      + M[0][2]*d3_124_013 - M[0][3]*d3_124_012;
  double d4_0124_0124 = M[0][0]*d3_124_124 - M[0][1]*d3_124_024 
                      + M[0][2]*d3_124_014 - M[0][4]*d3_124_012;
  double d4_0124_0134 = M[0][0]*d3_124_134 - M[0][1]*d3_124_034 
                      + M[0][3]*d3_124_014 - M[0][4]*d3_124_013;
  double d4_0124_0234 = M[0][0]*d3_124_234 - M[0][2]*d3_124_034 
                      + M[0][3]*d3_124_024 - M[0][4]*d3_124_023;
  double d4_0124_1234 = M[0][1]*d3_124_234 - M[0][2]*d3_124_134 
                      + M[0][3]*d3_124_124 - M[0][4]*d3_124_123;
  double d4_0134_0123 = M[0][0]*d3_134_123 - M[0][1]*d3_134_023 
                      + M[0][2]*d3_134_013 - M[0][3]*d3_134_012;
  double d4_0134_0124 = M[0][0]*d3_134_124 - M[0][1]*d3_134_024 
                      + M[0][2]*d3_134_014 - M[0][4]*d3_134_012;
  double d4_0134_0134 = M[0][0]*d3_134_134 - M[0][1]*d3_134_034 
                      + M[0][3]*d3_134_014 - M[0][4]*d3_134_013;
  double d4_0134_0234 = M[0][0]*d3_134_234 - M[0][2]*d3_134_034 
                      + M[0][3]*d3_134_024 - M[0][4]*d3_134_023;
  double d4_0134_1234 = M[0][1]*d3_134_234 - M[0][2]*d3_134_134 
                      + M[0][3]*d3_134_124 - M[0][4]*d3_134_123;
  double d4_0234_0123 = M[0][0]*d3_234_123 - M[0][1]*d3_234_023 
                      + M[0][2]*d3_234_013 - M[0][3]*d3_234_012;
  double d4_0234_0124 = M[0][0]*d3_234_124 - M[0][1]*d3_234_024 
                      + M[0][2]*d3_234_014 - M[0][4]*d3_234_012;
  double d4_0234_0134 = M[0][0]*d3_234_134 - M[0][1]*d3_234_034 
                      + M[0][3]*d3_234_014 - M[0][4]*d3_234_013;
  double d4_0234_0234 = M[0][0]*d3_234_234 - M[0][2]*d3_234_034 
                      + M[0][3]*d3_234_024 - M[0][4]*d3_234_023;
  double d4_0234_1234 = M[0][1]*d3_234_234 - M[0][2]*d3_234_134 
                      + M[0][3]*d3_234_124 - M[0][4]*d3_234_123;
  double d4_1234_0123 = M[1][0]*d3_234_123 - M[1][1]*d3_234_023 
                      + M[1][2]*d3_234_013 - M[1][3]*d3_234_012;
  double d4_1234_0124 = M[1][0]*d3_234_124 - M[1][1]*d3_234_024 
                      + M[1][2]*d3_234_014 - M[1][4]*d3_234_012;
  double d4_1234_0134 = M[1][0]*d3_234_134 - M[1][1]*d3_234_034 
                      + M[1][3]*d3_234_014 - M[1][4]*d3_234_013;
  double d4_1234_0234 = M[1][0]*d3_234_234 - M[1][2]*d3_234_034 
                      + M[1][3]*d3_234_024 - M[1][4]*d3_234_023;
  double d4_1234_1234 = M[1][1]*d3_234_234 - M[1][2]*d3_234_134 
                      + M[1][3]*d3_234_124 - M[1][4]*d3_234_123;

  // Find the 5x5 det:

  double det = M[0][0]*d4_1234_1234
             - M[0][1]*d4_1234_0234
             + M[0][2]*d4_1234_0134
             - M[0][3]*d4_1234_0124
             + M[0][4]*d4_1234_0123;
  if (det == 0) return -1;

  M[0][0] =  d4_1234_1234/det;
  M[0][1] = -d4_0234_1234/det;
  M[0][2] =  d4_0134_1234/det;
  M[0][3] = -d4_0124_1234/det;
  M[0][4] =  d4_0123_1234/det;

  M[1][0] = -d4_1234_0234/det;
  M[1][1] =  d4_0234_0234/det;
  M[1][2] = -d4_0134_0234/det;
  M[1][3] =  d4_0124_0234/det;
  M[1][4] = -d4_0123_0234/det;

  M[2][0] =  d4_1234_0134/det;
  M[2][1] = -d4_0234_0134/det;
  M[2][2] =  d4_0134_0134/det;
  M[2][3] = -d4_0124_0134/det;
  M[2][4] =  d4_0123_0134/det;

  M[3][0] = -d4_1234_0124/det;
  M[3][1] =  d4_0234_0124/det;
  M[3][2] = -d4_0134_0124/det;
  M[3][3] =  d4_0124_0124/det;
  M[3][4] = -d4_0123_0124/det;

  M[4][0] =  d4_1234_0123/det;
  M[4][1] = -d4_0234_0123/det;
  M[4][2] =  d4_0134_0123/det;
  M[4][3] = -d4_0124_0123/det;
  M[4][4] =  d4_0123_0123/det;

  return 0;
}


double TrProp::Mproton = 0.938272297;

TrProp::TrProp(double p0x,   double p0y, double p0z, 
	       double theta, double phi, double rigidity)
  : _p0x(p0x), _p0y(p0y), _p0z(p0z), _rigidity(rigidity)
{
  _xmass = Mproton;
  _chrg  = 1;

  AMSDir dir(theta, phi);
  _dxdz = dir.x();
  _dydz = dir.y();
}

TrProp::TrProp(AMSPoint p0, AMSDir dir, double rigidity)
  : _p0x(p0.x()), _p0y(p0.y()), _p0z(p0.z()), _rigidity(rigidity)
{
  _xmass = Mproton;
  _chrg  = 1;

  _dxdz = (dir.z() != 0) ? dir.x()/dir.z() : 0;
  _dydz = (dir.z() != 0) ? dir.y()/dir.z() : 0;
}

void TrProp::Clear()
{
  _rigidity = 0;
  _p0x = _p0y = _p0z = _dxdz = _dydz = 0;
}

double TrProp::Interpolate(AMSPoint &pnt, AMSDir &dir)
{
  // Check on dir
  if (dir[0] == 0 && dir[1] == 0 && dir[2] == 0) return -1;

  // Linear track case
  if (MAGSFFKEY.magstat == 0 || _chrg*_rigidity == 0) {
    double z = (dir[0]*(pnt[0]-_p0x)+dir[1]*(pnt[1]-_p0y)+dir[2]*(pnt[2]-_p0z))
              /(dir[0]*_dxdz        +dir[1]*_dydz        +dir[2]);
    AMSPoint pnt0 = pnt;
    pnt.setp(_p0x+_dxdz*z, _p0y+_dydz*z, _p0z+z);
    dir.setp(     _dxdz,        _dydz,        1);
    return pnt.dist(pnt0);
  }

  // Curved track case
  double d0 = -1/std::sqrt(_dxdz*_dxdz+_dydz*_dydz+1);
  double init[7] 
    = { _p0x, _p0y, _p0z, _dxdz*d0, _dydz*d0, d0, _chrg*_rigidity };
  double point[6]
    = { pnt.x(), pnt.y(), pnt.z(), dir.x(), dir.y(), dir.z() };

  double out[7];
  double len = VCFitPar(init, out, point, 0, 1);
  pnt[0] = out[0]; pnt[1] = out[1]; pnt[2] = out[2];
  dir[0] = out[3]; dir[1] = out[4]; dir[2] = out[5];
  return len;
}

double TrProp::InterpolateCyl(AMSPoint &pnt, AMSDir &dir, 
                              double radius, int idir)
{
  // Check on dir
  if (dir[0] == 0 && dir[1] == 0 && dir[2] == 0) return -1;

  // Linear track case
  if (MAGSFFKEY.magstat == 0 || _chrg*_rigidity == 0) {
    double dx = dir[0], dy = dir[1], dz = dir[2], z0 = _p0z-pnt[2];
    double p0 = _p0x-pnt[0], p1 = _dxdz, p2 = _p0y-pnt[1], p3 = _dydz;
    double aa = (1-dx*dx)*p1*p1     +(1-dy*dy)*p3*p3  +(1-dz*dz)
                -2*dx*dy *p1*p3      -2*dy*dz *p3      -2*dz*dx*p1;
    double bb = (1-dx*dx)*p1*p0     +(1-dy*dy)*p3*p2  +(1-dz*dz)*z0
               -dx*dy*(p3*p0+p1*p2) -dx*dz*(p0+z0*p1) -dy*dz*(p2+z0*p3);
    double cc = (1-dx*dx)*p0*p0     +(1-dy*dy)*p2*p2  +(1-dz*dz)*z0*z0
                -2*dx*dy *p0*p2      -2*dy*dz *z0*p2   -2*dx*dz *z0*p0    
                -radius*radius;
    double dd = bb*bb-aa*cc;
    if (dd < 0) return -1;

    double z = (-bb+idir*std::sqrt(dd))/aa;
    AMSPoint pnt0 = pnt;
    pnt.setp(_p0x+_dxdz*z, _p0y+_dydz*z, _p0z+z);
    dir.setp(     _dxdz,        _dydz,        1);
    return pnt.dist(pnt0);
  }

  // Curved track case
  double point[7] 
    = { pnt.x(), pnt.y(), pnt.z(), dir.x(), dir.y(), dir.z(), radius };
  double init[7];

  AMSDir   dir0(-_dxdz, -_dydz, -1);
  AMSPoint pnt0(_p0x, _p0y, _p0z);
  double s2    = (pnt-pnt0).prod(pnt-pnt0);
  double s1    = (pnt-pnt0).prod(dir);
  double sdist = radius-std::sqrt(s2-s1*s1);

  // Interpolate to inside the cylinder
  if (sdist < 0) {
    double t = -((pnt0[0]-pnt[0])*dir0[0]+
                 (pnt0[1]-pnt[1])*dir0[1])/(dir0[0]*dir0[0]+dir0[1]*dir0[1]);
    pnt0 = pnt0+dir0*t;
    Interpolate(pnt0, dir0);
    s1      = (pnt-pnt0).prod(pnt-pnt0);
    s2      = (pnt-pnt0).prod(dir);
    sdist   = radius-std::sqrt(s1-s2*s2);
    if (sdist < 0) return -1;
  }

  dir0    = dir0*idir;
  init[0] = pnt0[0]; init[1] = pnt0[1]; init[2] = pnt0[2];
  init[3] = dir0[0]; init[4] = dir0[1]; init[5] = dir0[2];
  init[6] = _rigidity*_chrg;

  double out[7];
  double len = VCFitParCyl(init, out, point);
  pnt[0] = out[0]; pnt[1] = out[1]; pnt[2] = out[2];
  dir[0] = out[3]; dir[1] = out[4]; dir[2] = out[5];
  return len;
}

double TrProp::VCFitPar(double *init, double *out, double *point, 
			double m55[][NDIM], int clear)
{
  double steps   = 0.03;
  double smax    = 50;
  double cconv   = 5.e-4;
  double fieldmm = 20;
  double tmaxf   = 0.2;
  double pii     = 3.1415927;
  double cfld    = 3333.*pii/180./(fieldmm+1.e-10)*tmaxf;
  double sfield  = cfld*fabs(init[6]/_chrg);
  double stept   = 0;
  double sdir    = 1;

  int nitm = 1000;

  double xmat[NDIM][NDIM];

  int ich = 0;
  double za = out[2];
  double vin[7];
  for (int i = 0; i < 7; i++) vin[i] = out[i] = init[i];

  double sdist = (point[0]-out[0])*point[3]+
                 (point[1]-out[1])*point[4]+
                 (point[2]-out[2])*point[5];

  if (sdist < 0) {
    sdir   = -1;
    vin[3] = out[3] = -init[3];
    vin[4] = out[4] = -init[4];
    vin[5] = out[5] = -init[5];
  }

  double pini = _chrg/init[6];

  int nit = 0, imat = 0;
  while (++nit <= nitm && imat == 0) {
    sdist = (point[0]-out[0])*point[3]+
            (point[1]-out[1])*point[4]+
            (point[2]-out[2])*point[5];

    double sd2 = out[3]*point[3]+out[4]*point[4]+out[5]*point[5];
    sdist = sdist/(sd2+1.e-10);

    if (std::min(sfield, smax) <= 0 || fabs(sdist) < cconv) {
      double h = out[2]-za;

      if (fabs(h) > steps && imat++ == 0) {
	double der[10];
	VCFuncXY(vin, out, der, clear);

	double dfx  = der[0]*h*pini, dfy  = der[1]*h*pini;
	double dfxp = der[2]*h*pini, dfyp = der[3]*h*pini;
	double dgx  = der[4]*h*pini, dgy  = der[5]*h*pini;
	double dgxp = der[6]*h*pini, dgyp = der[7]*h*pini;
	double fm = der[8], gm = der[9];

	xmat[0][0] = 1+0.5*h*dfx;  xmat[2][0] = 0.5*h*dfy;
	xmat[1][0] = h+0.5*h*dfxp; xmat[3][0] = 0.5*h*dfyp;
	xmat[4][0] = fm*h*h/2.;

	xmat[0][1] = dfx;    xmat[2][1] = dfy;
	xmat[1][1] = 1+dfxp; xmat[3][1] = dfyp;
	xmat[4][1] = h*fm;

	xmat[0][2] = 0.5*h*dgx;  xmat[2][2] = 1+0.5*h*dgy;
	xmat[1][2] = 0.5*h*dgxp; xmat[3][2] = h+0.5*h*dgyp;
	xmat[4][2] = 0.5*h*h*gm;

	xmat[0][3] = dgx;  xmat[2][3] = dgy;
	xmat[1][3] = dfxp; xmat[3][3] = 1+dgyp;
	xmat[4][3] = h*gm;

	xmat[0][4] = xmat[1][4] = xmat[2][4] = xmat[3][4] = 0;
	xmat[4][4] = 1;
      }
      else {
	for (int i = 0; i < NDIM*NDIM; i++) 
	  xmat[i/NDIM][i%NDIM] = (i/NDIM == i%NDIM) ? 1 : 0;
      }

      za = out[2];
    }

    for (int i = 0; i < 7; i++) vin[i] = out[i];

    if (fabs(sdist) < cconv) break;
    if (sdist < 0) {
      sdir  = -sdir;
      sdist = -sdist;
      ich++;
      for (int k = 3; k < 6; k++) vin[k] =- vin[k];
    }

    double step = std::min(std::min(smax, sfield), sdist);
    stept += step*sdir;

    if (step > steps) Rkuta(_chrg, step, vin, out);
    else {
      out[0] = vin[0]+step*vin[3];
      out[1] = vin[1]+step*vin[4];
      out[2] = vin[2]+step*vin[5];
      for (int i = 0; i < 3; i++) out[3+i] = vin[3+i];
    }
  }

  if (ich%2 != 0) for (int k = 3; k < 6; k++) out[k] =- out[k];

  if (m55) {
    double dum[NDIM][NDIM];
    Dmm55(xmat, m55, dum);

    for (int i = 0; i < NDIM*NDIM; i++) 
      m55[i/NDIM][i%NDIM] = dum[i/NDIM][i%NDIM];
  }

  return stept;
}

double TrProp::VCFitParCyl(double *init, double *out, double *point)
{
  double steps   = 0.03;
  double smax    = 50;
  double cconv   = 5.e-4;
  double fieldmm = 20;
  double tmaxf   = 0.2;
  double pii     = 3.1415927;
  double cfld    = 3333.*pii/180./(fieldmm+1.e-10)*tmaxf;
  double sfield  = cfld*fabs(init[6]/_chrg);
  double stept   = 0;
  double sdir    = 1;

  double vin[7];
  for (int i = 0; i < 7; i++) vin[i] = out[i] = init[i];

  int ich = 0, nit = 0, nitm = 1000;
  while (++nit <= nitm) {
    double s1 = (point[0]-out[0])*(point[0]-out[0])+
                (point[1]-out[1])*(point[1]-out[1])+
                (point[2]-out[2])*(point[2]-out[2]);
    double s2 = (point[0]-out[0])*point[3]+
                (point[1]-out[1])*point[4]+
                (point[2]-out[2])*point[5];
    double sdist = point[6]-std::sqrt(s1-s2*s2);
    double sd2   = out[3]*point[3]+out[4]*point[4]+out[5]*point[5];
    double sd1   = std::sqrt(abs(1-sd2*sd2));
    sdist = sdist/(sd1+1.e-10);

    if (nit == 1 && sdist < 0) {
      std::cerr << "tkfitparcyl-E-outside of cyl " 
                << sdist << " " << point[6] << std::endl;
      return -1;
    }

    for (int i = 0; i < 7; i++) vin[i] = out[i];

    if (fabs(sdist) < cconv) break;
    if (sdist < 0) {
      sdir  = -sdir;
      sdist = -sdist;
      ich++;
      for (int k = 3; k < 6; k++) vin[k] =- vin[k];
    }

    double step = std::min(std::min(smax, sfield), sdist);
    stept += step*sdir;

    if (step > steps) Rkuta(_chrg, step, vin, out);
    else {
      for (int i = 0; i < 3; i++) out[i] = vin[i]+step*vin[3+i];
      for (int i = 3; i < 6; i++) out[i] = vin[i];
    }
  }
  if (ich%2 != 0) for (int k = 3; k < 6; k++) out[k] =- out[k];

  return stept;
}

void TrProp::VCFuncXY(double *in, double *out, double *derl, int clear)
{
  static double der[2][10];

  if (clear == 1) {
    for (int i = 0; i < 10; i++) der[0][i] = der[1][i];
  }

  float xx[3], h[3], hxy[3][3], dx, dy;

  for (int k = clear; k < 2; k++) {
    if (k == 0){
      for (int i = 0; i < 3; i++) xx[i] = in[i];
      dx = in[3]/in[5];
      dy = in[4]/in[5];
    }
    else {
      for (int i = 0; i < 3; i++) xx[i] = out[i];
      dx = out[3]/out[5];
      dy = out[4]/out[5];
    }

    double s = std::sqrt(1+dx*dx+dy*dy);
    MagField::GetPtr()->GuFld(xx, h);
    MagField::GetPtr()->TkFld(xx, hxy);
    
    der[k][0] = s*(dx*dy*hxy[0][0]-(1+dx*dx)*hxy[0][1]+dy*hxy[0][2]);
    der[k][1] = s*(dx*dy*hxy[1][0]-(1+dx*dx)*hxy[1][1]+dy*hxy[1][2]);

    der[k][2] = dx/s*(dx*dy*h[0]-(1+dx*dx)*h[1]+dy*h[2])+s*(dy*h[0]-2*dx*h[1]);
    der[k][3] = dy/s*(dx*dy*h[0]-(1+dx*dx)*h[1]+dy*h[2])+s*(dx*h[0]+h[2]);

    der[k][4] = s*((1+dy*dy)*hxy[0][0]-dx*dy*hxy[0][1]-dx*hxy[0][2]);
    der[k][5] = s*((1+dy*dy)*hxy[1][0]-dx*dy*hxy[1][1]-dx*hxy[1][2]);

    der[k][6] = dx/s*((1+dy*dy)*h[0]-dx*dy*h[1]-dx*h[2])-s*(dy*h[1]+h[2]);
    der[k][7] = dy/s*((1+dy*dy)*h[0]-dx*dy*h[1]-dx*h[2])+s*(2*dy*h[0]-dx*h[1]);

    der[k][8] = s*( dx*dy*h[0]-(1+dx*dx)*h[1]+dy*h[2]);
    der[k][9] = s*(-dx*dy*h[1]+(1+dy*dy)*h[0]-dx*h[2]);
  }

  for (int i = 0; i < 10; i++)
    derl[i] = 2.997e-4*(der[0][i]+der[1][i])/2;
}

void TrProp::Dmm55(double AA[][5], double BB[][5], double CC[][5])
{
/*
*** ---------------------------------------------                     05/07/81
*** MULTIPLICATION OF 5X5 MATRICES: A*B -> C.
*** FOR:I,J=1,5   C(I,J)=SUM:K=1,5(A(I,K)B(K,J))
*** NB: B AND C MAY WELL BE THE SAME MATRIX

        Author: A. Haas (Freiburg University)

C *** ---------------------------------------------

   Imported to C++ by SH
*/

  double A[25], B[25], C[25];
  for (int i = 0; i < 25; i++) {
    A[i] = AA[i/5][i%5];
    B[i] = BB[i/5][i%5];
  }

  for (int J5 = 4; J5 < 25; J5 += 5) {
    double B5J = B[J5]; int J4 = J5-1;
    double B4J = B[J4]; int J3 = J4-1;
    double B3J = B[J3]; int J2 = J3-1;
    double B2J = B[J2]; int J1 = J2-1;
    double B1J = B[J1];

    C[J1] = A[0]*B1J+A[5]*B2J+A[10]*B3J+A[15]*B4J+A[20]*B5J;
    C[J2] = A[1]*B1J+A[6]*B2J+A[11]*B3J+A[16]*B4J+A[21]*B5J;
    C[J3] = A[2]*B1J+A[7]*B2J+A[12]*B3J+A[17]*B4J+A[22]*B5J;
    C[J4] = A[3]*B1J+A[8]*B2J+A[13]*B3J+A[18]*B4J+A[23]*B5J;
    C[J5] = A[4]*B1J+A[9]*B2J+A[14]*B3J+A[19]*B4J+A[24]*B5J;
  }

  for (int i = 0; i < 25; i++) CC[i/5][i%5] = C[i];
}

void TrProp::Rkuta(double charge, double step, double *vect, double *vout)
{
/*
C.
C.    ******************************************************************
C.    *                                                                *
C.    *  Runge-Kutta method for tracking a particle through a magnetic *
C.    *  field. Uses Nystroem algorithm (See Handbook Nat. Bur. of     *
C.    *  Standards, procedure 25.5.20)                                 *
C.    *                                                                *
C.    *  Input parameters                                              *
C.    *       CHARGE    Particle charge                                *
C.    *       STEP      Step size                                      *
C.    *       VECT      Initial co-ords,direction cosines,momentum     *
C.    *  Output parameters                                             *
C.    *       VOUT      Output co-ords,direction cosines,momentum      *
C.    *  User routine called                                           *
C.    *       CALL GUFLD(X,F)                                          *
C.    *                                                                *
C.    *    ==>Called by : <USER>, GUSWIM                               *
C.    *       Authors    R.Brun, M.Hansroul  *********                 *
C.    *                  V.Perevoztchikov (CUT STEP implementation)    *
C.    *                                                                *
C.    *                                                                *
C.    ******************************************************************
C.

   Imported to C++ by SH
*/
  float F[4], X, Y, Z, XYZT[3];
  double SECXS[4], SECYS[4], SECZS[4], HXP[3];

  int MAXIT = 1992, MAXCUT = 11;
  double EC  = 2.9979251e-4;
  double DLT = 1e-4, DLT32 = DLT/32;
  double PISQUA = .986960440109e+01;

  int ITER = 0, NCUT = 0;

  for (int j = 0; j < 7; j++) vout[j] = vect[j];

  double PINV = EC*charge/vect[6], TL = 0, H = step;

  do {
    double REST = step-TL;
    if (std::fabs(H) > std::fabs(REST)) H = REST;
    float vvout[3];
    vvout[0]=vout[0];
    vvout[1]=vout[1];
    vvout[2]=vout[2];
    MagField::GetPtr()->GuFld(vvout, F);
//
//             Start of integration
//
    X = vout[0];
    Y = vout[1];
    Z = vout[2];

    double A = vout[3];
    double B = vout[4];
    double C = vout[5];

    double H2  = H/2;
    double H4  = H2/2;
    double PH  = PINV*H;
    double PH2 = PH/2;

    SECXS[0] = (B*F[2]-C*F[1])*PH2;
    SECYS[0] = (C*F[0]-A*F[2])*PH2;
    SECZS[0] = (A*F[1]-B*F[0])*PH2;

    double ANG2 = (SECXS[0]*SECXS[0]+SECYS[0]*SECYS[0]+SECZS[0]*SECZS[0]);
    if (ANG2 > PISQUA) break;

    double DXT = H2*A+H4*SECXS[0];
    double DYT = H2*B+H4*SECYS[0];
    double DZT = H2*C+H4*SECZS[0];

    XYZT[0] = X+DXT;
    XYZT[1] = Y+DYT;
    XYZT[2] = Z+DZT;
//
//              Second intermediate point
//
    double EST = std::fabs(DXT)+std::fabs(DYT)+std::fabs(DZT);
    if (EST <= H) {
    MagField::GetPtr()->GuFld(XYZT, F);


      double AT = A+SECXS[0];
      double BT = B+SECYS[0];
      double CT = C+SECZS[0];

      SECXS[1] = (BT*F[2]-CT*F[1])*PH2;
      SECYS[1] = (CT*F[0]-AT*F[2])*PH2;
      SECZS[1] = (AT*F[1]-BT*F[0])*PH2;

      AT = A+SECXS[1];
      BT = B+SECYS[1];
      CT = C+SECZS[1];

      SECXS[2] = (BT*F[2]-CT*F[1])*PH2;
      SECYS[2] = (CT*F[0]-AT*F[2])*PH2;
      SECZS[2] = (AT*F[1]-BT*F[0])*PH2;

      DXT = H*(A+SECXS[2]);
      DYT = H*(B+SECYS[2]);
      DZT = H*(C+SECZS[2]);

      XYZT[0] = X+DXT;
      XYZT[1] = Y+DYT;
      XYZT[2] = Z+DZT;

      AT = A+2*SECXS[2];
      BT = B+2*SECYS[2];
      CT = C+2*SECZS[2];

      EST = std::fabs(DXT)+std::fabs(DYT)+std::fabs(DZT);

      if (EST <= 2.*std::fabs(H)) {
        MagField::GetPtr()->GuFld(XYZT, F);

        Z += (C+(SECZS[0]+SECZS[1]+SECZS[2])/3)*H;
        Y += (B+(SECYS[0]+SECYS[1]+SECYS[2])/3)*H;
	X += (A+(SECXS[0]+SECXS[1]+SECXS[2])/3)*H;

	SECXS[3] = (BT*F[2]-CT*F[1])* PH2;
	SECYS[3] = (CT*F[0]-AT*F[2])* PH2;
	SECZS[3] = (AT*F[1]-BT*F[0])* PH2;

	A += (SECXS[0]+SECXS[3]+2*(SECXS[1]+SECXS[2]))/3;
	B += (SECYS[0]+SECYS[3]+2*(SECYS[1]+SECYS[2]))/3;
	C += (SECZS[0]+SECZS[3]+2*(SECZS[1]+SECZS[2]))/3;

	EST = (std::fabs(SECXS[0]+SECXS[3]-(SECXS[1]+SECXS[2])))+
	      (std::fabs(SECYS[0]+SECYS[3]-(SECYS[1]+SECYS[2])))+
	      (std::fabs(SECZS[0]+SECZS[3]-(SECZS[1]+SECZS[2])));

	if (EST <= DLT && std::fabs(H) > 1.E-4) {
	  NCUT = 0;

//               If too many iterations, go to HELIX
	  if (++ITER > MAXIT) break;

	  TL = TL+H;
	  if (EST < (DLT32)) H *= 2;

	  double CBA = 1/std::sqrt(A*A+B*B+C*C);
	  vout[0] = X;
	  vout[1] = Y;
	  vout[2] = Z;
	  vout[3] = CBA*A;
	  vout[4] = CBA*B;
	  vout[5] = CBA*C;

	  REST = step-TL;
	  if (step < 0.) REST = -REST;
	  if (REST > 1.E-5*std::fabs(step)) continue;

	  return;
	}
      }
    }
//              CUT step
//               If too many cuts , go to HELIX
    H /= 2;
    NCUT++;
  } while (NCUT <= MAXCUT);


//              ANGLE TOO BIG, USE HELIX
  double F1  = F[0];
  double F2  = F[1];
  double F3  = F[2];
  double F4  = std::sqrt(F1*F1+F2*F2+F3*F3);
  double RHO = -F4*PINV;
  double TET = RHO*step;

  if (TET != 0.) {
    double HNORM = 1/F4;
    F1 = F1*HNORM;
    F2 = F2*HNORM;
    F3 = F3*HNORM;

    HXP[0] = F2*vect[2]-F3*vect[1];
    HXP[1] = F3*vect[0]-F1*vect[2];
    HXP[2] = F1*vect[1]-F2*vect[0];

    double HP = F1*vect[0]+F2*vect[1]+F3*vect[2];

    double RHO1 = 1/RHO;
    double SINT = std::sin(TET);
    double COST = 2*std::sin(TET/2)*std::sin(TET/2);

    double G1 = SINT*RHO1;
    double G2 = COST*RHO1;
    double G3 = (TET-SINT)*HP*RHO1;
    double G4 = -COST;
    double G5 = SINT;
    double G6 = COST*HP;
 
    vout[0] = vect[0]+(G1*vect[0]+G2*HXP[0]+G3*F1);
    vout[1] = vect[1]+(G1*vect[1]+G2*HXP[1]+G3*F2);
    vout[2] = vect[2]+(G1*vect[2]+G2*HXP[2]+G3*F3);
 
    vout[0] = vect[0]+(G4*vect[0]+G5*HXP[0]+G6*F1);
    vout[1] = vect[1]+(G4*vect[1]+G5*HXP[1]+G6*F2);
    vout[2] = vect[2]+(G4*vect[2]+G5*HXP[2]+G6*F3);
  }
  else {
    vout[0] = vect[0]+step*vect[0];
    vout[1] = vect[1]+step*vect[1];
    vout[2] = vect[2]+step*vect[2];
  }
}

