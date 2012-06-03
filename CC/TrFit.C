//  $Id: TrFit.C,v 1.67.4.3 2012/06/03 15:52:34 shaino Exp $


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
///\date  2010/03/03 SH  ChikanianFit added
///$Date: 2012/06/03 15:52:34 $
///
///$Revision: 1.67.4.3 $
///
//////////////////////////////////////////////////////////////////////////

#include "TrFit.h"
#include "TkDBc.h"
#include "MagField.h"

#include <cmath>
#include <iostream>     // for min and max

ClassImp(TrProp);
ClassImp(TrFit);


TrFit::TrFit(void) : TrProp()
{
  Clear();
}

TrFit::~TrFit()
{
}

void TrFit::Clear()
{
  TrProp::Clear();
  _nhit = _nhitx = _nhity = _nhitxy = 0;
  for (int i = 0; i < PMAX; i++) _param[i] = 0;
  for (int i = 0; i < LMAX; i++){
    _xh[i] = _yh[i] = _zh[i] = 0;
    _xs[i] = _ys[i] = _zs[i] = 0;
    _xr[i] = _yr[i] = _zr[i] = 0;
    _bx[i] = _by[i] = _bz[i] = 0;
  }
  _chisqx = _chisqy = _chisq = -1;
  _ndofx  = _ndofy  =  0;
  _errrinv = 0;
  _mscat = 0;
  _eloss = 0;
}

int TrFit::Add(double x,  double y,  double z,
  	       double ex, double ey, double ez, int at)
{
  double bf[3] = { 0, 0, 0 };
  GuFld(x, y, z, bf);
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

double TrFit::DoFit(int method, int mscat, int eloss, 
		    float charge, float mass, float beta)
{
  // Check number of hits
  if (_nhit < 3) return -1;

  // Estimate mass from given beta
  if (mass == 0 && beta != 0 && -1 < beta && beta < 1) {
    if (_rigidity == 0 && SimpleFit() < 0) return -21;

    if (beta < -1 || 1 < beta) beta = 1;
    double p  = _rigidity*charge;
    double mm = p*p*(1/beta/beta-1);
    mass = (mm > 0) ? std::sqrt(mm) : 0;
  }

  SetMultScat(mscat, eloss);
  SetMassChrg(mass, charge);

  double ret = 0;
  if (method ==    LINEAR)  ret = LinearFit();
  if (method ==    CIRCLE)  ret = CircleFit();
  if (method ==    SIMPLE)  ret = SimpleFit();
  if (method ==   ALCARAZ)  ret = AlcarazFit();
  if (method ==   CHOUTKO)  ret = ChoutkoFit();
  if (method == CHIKANIANC) ret = ChikanianFitCInt(1);
  if (method == CHIKANIANF) ret = ChikanianFitF();

  ParLimits();
  return ret;
}

double TrFit::GetBeta(){
  double bb=1;
  if(_mass>0){
    if(_rigidity==0) {
      int lmscat=_mscat;
      _mscat=0;
      SimpleFit();
      _mscat=lmscat;
    }
    double p=0;
    if(_chrg!=0) 
    p   = _rigidity*_chrg;
    float bi2 = (p != 0) ? 1+_mass*_mass/p/p : 0;
    bb= (bi2 > 0) ? 1/std::sqrt(bi2) : 0;
  }
  return bb;
}


double TrFit::LinearFit(void)
{
  /// Linear fitting in X-Z and Y-Z planes

  if (LinearFit(1) < 0) return -1;
  if (LinearFit(2) < 0) return -1;

  _p0x = _param[0]; _dxdz = _param[1];
  _p0y = _param[2]; _dydz = _param[3];
  _p0z = 0;

  _rigidity = 100000; 
  _errrinv = 0;

  _chisq = (_ndofx+_ndofy > 0) ? (_chisqx+_chisqy)/(_ndofx+_ndofy) : -1;
  return _chisq;
}

double TrFit::CircleFit(void)
{
  /// Circlar fitting in Y-Z plane and liear fitting in S-Z plane

  if (!_fieldon()) return LinearFit();

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

  double bf[3];
  GuFld(_p0x, _p0y, _p0z, bf);

  double cosd = -1/std::sqrt(1+_param[1]*_param[1]);
  _rigidity =             1e-12*Clight*0.5/kappa*bf[0]/cosd;
  _errrinv  = std::abs(1/(1e-12*Clight*0.5/kappe*bf[0]/cosd));

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

#include <iostream>
#define PAR_LIMITS(P, V1, V2) { \
    double pold = P;	        \
    bool r = ParLimits(P,V1,V2);\
    if (r && nmsg++ < 100) {    \
      std::cout << "TrFit::ParLimits applied to: "#P" "	\
	        << pold << " -> " << P << std::endl;}	\
    nlim += r; \
}

int TrFit::ParLimits(void)
{
  static int nmsg = 100;
  int nlim = 0;
  PAR_LIMITS(_p0x,      1e-9, 1e4);
  PAR_LIMITS(_p0y,      1e-9, 1e4);
  PAR_LIMITS(_p0z,      1e-9, 1e4);
  PAR_LIMITS(_dxdz,     1e-9, 1e4);
  PAR_LIMITS(_dydz,     1e-9, 1e4);
  PAR_LIMITS(_rigidity, 1e-6, 1e6);
  PAR_LIMITS(_chisqx,   1e-9, 1e9);
  PAR_LIMITS(_chisqy,   1e-9, 1e9);
  PAR_LIMITS(_chisq,    1e-9, 1e9);
  PAR_LIMITS(_errrinv,  1e-9, 1e6);
  return nlim;
}

bool TrFit::ParLimits(double &par, double min, double max)
{
  if (par == 0) return false;
  
  double pabs = std::fabs(par);
  double sign = par/pabs;

  bool ret = false;
  if (pabs < min) { par = sign*min; ret = true; }
  if (pabs > max) { par = sign*max; ret = true; }

  return ret;
}

int TrFit::GetLayer(double z)
{
  int ilay = 0;
  double dzmin = 2.5;
  for (int i = 0; i < TkDBc()->nlay(); i++) 
    if (std::abs(z-TkDBc()->GetZlayer(i+1)) < dzmin) {
      dzmin = std::abs(z-TkDBc()->GetZlayer(i+1));
      ilay = i+1;
    }

  return ilay;
}

void TrFit::PropagateFast(int ih0, int ih1, int ndiv)
{
  if (ih0 < 0 || _nhit <= ih0 ||
      ih1 < 0 || _nhit <= ih1) return;

  double dir[3] = { -GetD0x(), -GetD0y(), -GetD0z() };
  double pos[3] = {  GetP0x(),  GetP0y(),  GetP0z() };

  int i1 = (ih0 < ih1) ? ih0+1 : ih0-1;
  int i2 = (ih0 < ih1) ? ih1+1 : ih1-1;
  int di = (ih0 < ih1) ? 1     : -1;

  for (int i = i1; i != i2 && 0 <= i && i < _nhit; i += di) {
    double len[3] = { _xh[i]-_xr[i]-pos[0], 
		      _yh[i]-_yr[i]-pos[1], _zh[i]-pos[2] };
    Propagate(pos, len, dir, ndiv);
  }
  if (dir[2] == 0) dir[2] = 1;

  _p0x  = pos[0];
  _p0y  = pos[1];
  _p0z  = pos[2];
  _dxdz = dir[0]/dir[2];
  _dydz = dir[1]/dir[2];
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
  if (_nhitx < 2 || _nhity < 3) return -1;

  double pc[3];
  int ic = GetPcen(pc);
  if (ic <= 0) return -1;

  double len  [LMAX];
  double pintx[LMAX][2];
  double pintu[LMAX][2];
  double d  [2*LMAX][NDIM];

  for (int s = 0; s <= 1; s++) {
   int i1 = (s == 0) ? ic-1 :    ic;
   int i2 = (s == 0) ?   -1 : _nhit;
   int di = (s == 0) ?   -1 :     1;
   if ((i2-i1)/di < 0) return -1;

   for (int i = i1; i != i2; i += di) {

    int ix = i, iy = i+_nhit;
    for (int j = 0; j < NDIM; j++) d[ix][j] = d[iy][j] = 0;
    d[ix][0] = d[iy][1] = 1;

    if (_xs[i] <= 0 && _ys[i] <= 0 && _zs[i] <= 0) continue;

    double pos[3], dif[3], bbf[3];
    if (i == i1) {
      pos[0] = pc[0];
      pos[1] = pc[1];
      pos[2] = pc[2];
      GuFld(pos[0], pos[1], pos[2], bbf);
    }
    else {
      pos[0] = _xh[i-di];
      pos[1] = _yh[i-di];
      pos[2] = _zh[i-di];
      bbf[0] = _bx[i-di];
      bbf[1] = _by[i-di];
      bbf[2] = _bz[i-di];
    }
    dif[0] = _xh[i]-pos[0];
    dif[1] = _yh[i]-pos[1];
    dif[2] = _zh[i]-pos[2];

    len[i] = std::sqrt(dif[0]*dif[0]+dif[1]*dif[1]+dif[2]*dif[2]);
    if (dif[2] > 0) len[i] = -len[i];

    double u[3];
    u[0] = dif[0]/len[i];
    u[1] = dif[1]/len[i];
    u[2] = dif[2]/len[i];

    double bax = (bbf[0]+_bx[i])/2;
    double bay = (bbf[1]+_by[i])/2;
    double baz = (bbf[2]+_bz[i])/2;

    pintx[i][0] = (u[1]*bbf[2]-u[2]*bbf[1])/2;
    pintx[i][1] = (u[2]*bbf[0]-u[0]*bbf[2])/2;

    pintu[i][0] = u[1]*baz-u[2]*bay;
    pintu[i][1] = u[2]*bax-u[0]*baz;

    for (int j = i1; j != i+di; j += di) {
      d[ix][2] += len[j];
      d[iy][3] += len[j];

      for (int k = i1; k != j+di; k += di) {
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
  if (Inv55(cov)) return -2;

  // Solution
  for (int k = 0; k < NDIM; k++) {
    _param[k] = 0;
    for (int i = 0; i < NDIM; i++) _param[k] += cov[k][i]*dx[i];
  }
  if (_param[2]*_param[2]+_param[3]*_param[3] > 1) return -3;

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
  _chisq = (_ndofx+_ndofy > 0) ? (_chisqx+_chisqy)/(_ndofx+_ndofy) : 0;

  double dz = std::sqrt(1-_param[2]*_param[2]-_param[3]*_param[3]);
  _p0x = _param[0]; _dxdz = -_param[2]/dz;
  _p0y = _param[1]; _dydz = -_param[3]/dz;
  _p0z = 0;

  _rigidity = (_param[4] != 0) ? Clight*1e-12/_param[4] : 0;

  return _chisq;
}

double TrFit::SimpleFitTop(void)
{
  if (_nhitx < 2 || _nhity < 3) return -1;

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
      for (int j = 0; j < 3; j++) pintx[i][j] = pintu[i][j] = 0;
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
  if (Inv55(cov)) return -2;

  // Solution
  for (int k = 0; k < NDIM; k++) {
    _param[k] = 0;
    for (int i = 0; i < NDIM; i++) _param[k] += cov[k][i]*dx[i];
  }
  if (_param[2]*_param[2]+_param[3]*_param[3] > 1) return -3;

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

  double dz = std::sqrt(1-_param[2]*_param[2]-_param[3]*_param[3]);
  _p0x = _param[0]; _dxdz = -_param[2]/dz;
  _p0y = _param[1]; _dydz = -_param[3]/dz;
  _p0z = _zh[0];

  _rigidity = (_param[4] != 0) ? Clight*1e-12/_param[4] : 0;

  return _chisq;
}

// ***********************************************************************
//
//     Fitting NPOINTS (consecutive) (maximum 8 points)
//
// ***********************************************************************
//
//     Things to play with (lengths are in cm):
//        WLEN = number of radiation lengths traversed for each plane
//
//     The multiple scattering uses the formula:
//        (Deviation_rms_x)^2 = (Deviation_rms_y)^2 =
//          (0.0118/(rigidity(GV)//beta))^2 * (WLAD/RLEN)/cos(theta)
//
// ***********************************************************************
//
//     Written for AMS-01, J. Alcaraz (1999)
//     Updated and improved for AMS-02, J. Alcaraz (February 2003)
//
//     Ref: J.Alcaraz NIMA 533 (2005) 613
//
// ***********************************************************************
//
// Inported from Fortran (tkfit.F) by SH
//
double TrFit::AlcarazFit(int fixr)
{
  // Initialize fit
  int ret = JAInitPar(fixr);
  if (ret < 0) return (double)ret;

  double fmtx[LMAX*3],    gmtx[LMAX*3];     // Transportation matrices
  double vmtx[LMAX*LMAX], wmtx[LMAX*LMAX];  // Inv. of covariance matrices
  double len [LMAX];
  double cosz[LMAX];
  int    ilay[LMAX];

  // Estimate layer number
  for (int i = 0; i < _nhit; i++) {
    ilay[i] = GetLayer(_zh[i]);
    if (ilay[i] == 8) ilay[i] = 0;
    if (ilay[i] == 9) ilay[i] = 8;

    if (i > 0 && ilay[i] == ilay[i-1]) {
      cout << "TrFit::AlcarazFit-E-Duplicated layer "
           << ilay[i] << " " << ilay[i-1] << " nhit= " << _nhit << endl;
      for (int j = 0; j < _nhit; j++)
        cout << ilay[j] << " " << _zh[j] << endl;
      return -9;
    }
  }

  // Fill transportation matrices
  if (JAFillFGmtx(fmtx, gmtx, len, cosz, 20) < 0) return -1;

  // Fill inv. of covariance matrices
  if (JAFillVWmtx(vmtx, wmtx, len, cosz, ilay) < 0) return -1;

  // Obtain minimized parameters for Y and rigidity
  if (JAMinParams(gmtx, wmtx, 1, fixr) < 0) return -1;

  // Obtain minimized parameters for X (rigidity fixed)
  if (JAMinParams(fmtx, vmtx, 0, 1) < 0) return -1;

  // Get residuals
  for (int i = 0; i < _nhit; i++) {
    _xr[i] = _xh[i]-fmtx[i*3+2]*_param[4];
    _yr[i] = _yh[i]-gmtx[i*3+2]*_param[4];
    for (int j = 0; j < 2; j++) {
      _xr[i] -= fmtx[i*3+j]*_param[j*2];
      _yr[i] -= gmtx[i*3+j]*_param[j*2+1];
    }
  }

  // Estimate chisquares
  _chisqx = _chisqy = 0;
  for (int i = 0; i < _nhit; i++)
    for (int j = 0; j < _nhit; j++) {
      _chisqx += _xr[i]*vmtx[i*LMAX+j]*_xr[j];
      _chisqy += _yr[i]*wmtx[i*LMAX+j]*_yr[j];
    }
  _chisq = (_ndofx+_ndofy > 0) ? (_chisqx+_chisqy)/(_ndofx+_ndofy) : 0;

  // Fill parameters
  double dzarg = 1-_param[2]*_param[2]-_param[3]*_param[3];
  double dz = 1;
  if (dzarg > 0) dz = std::sqrt(dzarg);
  _p0x = _param[0]; _dxdz = -_param[2]/dz;
  _p0y = _param[1]; _dydz = -_param[3]/dz;
  _p0z = 0;

  _rigidity = (_param[4] != 0) ? 1e-12*Clight/_param[4] : 0;

  return _chisq;
}

int TrFit::GetPcen(double *pos, double *dir)
{
  int ic = -1;
  for (int i = 1; i < _nhit; i++)
    if (_zh[i-1]*_zh[i] < 0) { ic = i; break; }

  if (ic <= 0) {
    double zc = 0;
    for (int i = 1; i < _nhit; i++) {
      if (zc == 0 || std::abs(_zh[i]) < zc) {
	zc = std::abs(_zh[i]);
	ic = i;
      }
    }
  }

  if (ic <= 0 || _zh[ic]-_zh[ic-1] == 0) return -1;

  if (pos) {
    pos[0] = _xh[ic-1]-(_xh[ic]-_xh[ic-1])/(_zh[ic]-_zh[ic-1])*_zh[ic-1];
    pos[1] = _yh[ic-1]-(_yh[ic]-_yh[ic-1])/(_zh[ic]-_zh[ic-1])*_zh[ic-1];
    pos[2] = 0;
  }
  if (dir) {
    dir[0] = (_xh[ic]-_xh[ic-1])/(_zh[ic]-_zh[ic-1]);
    dir[1] = (_yh[ic]-_yh[ic-1])/(_zh[ic]-_zh[ic-1]);
    dir[2] = 1;
  }

  return ic;
}

int TrFit::FillDmsc(double *dmsc, double fact, 
		    double *len,  double *cosz, int *ilay)
{
  // Scattering factor tuned with muon MC
  double WLEN[LMAX] = { 0.1080,  // 1:L1N (Si+HC+TRD+TOF)
			0.0006,  // 2:L1  (Si)
			0.0012,  // 3:L2  (Si+HC)
			0.0006,  // 4:L3  (Si)
			0.0006,  // 5:L4  (Si+HC)/2
			0.0006,  // 6:L5  (Si+HC)/2
			0.0006,  // 7:L6  (Si)
			0.0012,  // 8:L7  (Si+HC)
			0.0600   // 9:L9  (Si+TOF+RICH)
                      };

  static bool first = true;
#pragma omp critical (filldmsc)
  if (first && _mscat) {
    std::cout << "TrFit::FillDmsc-I-WLEN(%)=";
    for (int i = 0; i < 9; i++)
      std::cout << Form(" %4.2f", WLEN[i]*100);
    std::cout << endl;
    first = false;
  }

  for (int i = 0; i < _nhit; i++) dmsc[i] = 0;
  for (int i = 1; i <     8; i++) WLEN[i] *= fact;

  if (_rigidity == 0) return -1;

  // Calculate pbi2 = (pbeta)^(-2)
  double pbi2 = 0;
  if (_mass > 0 && _chrg != 0) {
    double ri2 = (_rigidity != 0) ? 1/_rigidity/_rigidity : 0;
    double bi2 = 1/GetBeta()/GetBeta();
    pbi2 = ri2*bi2;
  }
  if (pbi2 <= 0) return -1;

  int ic = GetPcen(0);
  if (ic <= 0) return -1;

  double ltmp[LMAX], ctmp[LMAX];
  int    itmp[LMAX];

  if (!len || !cosz) {
    JAFillFGmtx(0, 0, ltmp, ctmp, 20);
    if (!len)  len  = ltmp;
    if (!cosz) cosz = ctmp;
  }
  if (!ilay) {
    ilay = itmp;
    for (int i = 0; i < _nhit; i++) {
      ilay[i] = GetLayer(_zh[i]);
      if (ilay[i] == 8) ilay[i] = 0;
      if (ilay[i] == 9) ilay[i] = 8;
    }
  }

  for (int i = 0; i < _nhit; i++) {
    dmsc[i] = 0;

    if (_mscat && pbi2 > 0) {
      int k1 = (i < ic) ? ic-1 : ic;
      int dk = (i < ic) ?   -1 :  1;
      if (k1+dk < 0 || _nhit <= k1+dk) continue;

      int l1 = ilay[k1+dk], l2 = ilay[i], dl = (l1 < l2) ? 1 : -1;
      for (int k = k1+dk; k != i+dk; k += dk) {
	double wl = 0, ln = 0;
	l2 = ilay[k];
	for (int l =  k; l !=  i+dk; l += dk) ln += len[l];
	for (int l = l1; l != l2+dl; l += dl) wl += WLEN[l];

	dmsc[i] += ln*ln*0.0118*0.0118*pbi2*wl/cosz[k];
	l1 = l2+dl;
      }
    }
  }

  return 0;
}

int TrFit::JAInitPar(int fixr)
{
  // Check number of hits
  if (_nhitx < 2 || _nhity < 3) return -(10+_nhit);

  // MagField check
  bool magf = true;
  for (int i = 0; i < _nhit; i++) 
    if (_bx[i]*_bx[i]+_by[i]*_by[i]+_bz[i]*_bz[i] == 0) magf = false;

  // Get initial paramters
  if (!fixr && magf && _rigidity == 0) {
    if (SimpleFit() < 0) return -20;
  }
  else {
    double pos[3], dir[3];
    GetPcen(pos, dir);
    _param[0] = pos[0];
    _param[1] = pos[1];
    _param[2] = dir[0];
    _param[3] = dir[1];
  }

  _param[4] = (magf && _rigidity != 0) ? 1e-12*Clight/_rigidity : 0;

  return 0;
}


int TrFit::JAFillFGmtx(double *fmtx, double *gmtx, 
                       double *len,  double *cosz, int ndiv)
{
  if (_nhit < 3) return -1;

  // Initialize F and G matrices
  if (fmtx && gmtx) {
    for (int i = 0; i < LMAX; i++)
      for (int j = 0; j < 3; j++)
	fmtx[i*3+j] = gmtx[i*3+j] = (j == 0) ? 1 : 0;
  }

  double u0 = _dxdz, v0 = _dydz, uv = u0*u0+v0*v0;
  double w0 = (uv < 1) ? -std::sqrt(1-uv) : -1e-9;

  double dir[3] = { u0, v0, w0 };

  double rpar = (_rigidity != 0) ? 1e-12*Clight/_rigidity : 0;

  len [0] = 0;
  cosz[0] = std::abs(w0);

  double pc[3];
  int ic = GetPcen(pc);
  if (ic <= 0) return -1;

  // Fill F and G matrices
  for (int s = 0; s <= 1; s++) {
   int i1 = (s == 0) ? ic-1 :    ic;
   int i2 = (s == 0) ?   -1 : _nhit;
   int di = (s == 0) ?   -1 :     1;
   if ((i2-i1)/di < 0) return -1;

   double mem[6] = { 1, 1, 0, 0, 0, 0 }, mel[4];

   for (int i = i1; i != i2; i += di) {
    double pos[3];
    pos[0] = (i == i1) ? pc[0] : _xh[i-di];
    pos[1] = (i == i1) ? pc[1] : _yh[i-di];
    pos[2] = (i == i1) ? pc[2] : _zh[i-di];

    double dif[3];
    dif[0] = _xh[i]-pos[0];
    dif[1] = _yh[i]-pos[1];
    dif[2] = _zh[i]-pos[2];

    // Estimate path integral
    JAStepPin(pos, dif, dir, mel, mem, rpar, ndiv);

    len [i] = std::sqrt(dif[0]*dif[0]+dif[1]*dif[1]+dif[2]*dif[2]);
    cosz[i] = std::abs(dir[2]);

    if (fmtx && gmtx) {
      for (int j = i; j != i2; j += di) {
	fmtx[j*3+1] += mel[0];
	gmtx[j*3+1] += mel[1];
	fmtx[j*3+2] += mel[2];
	gmtx[j*3+2] += mel[3];
      }
    }
   }
  }

  return 0;
}


int TrFit::JAFillVWmtx(double *vmtx, double *wmtx,
                       double *len,  double *cosz, int *ilay)
{
  // Initialize V and W matrices
  for (int i = 0; i < LMAX*LMAX; i++) vmtx[i] = wmtx[i] = 0;

  double dmsc[LMAX];
  if (_mscat && FillDmsc(dmsc, 1, len, cosz, ilay) < 0) return -1;

  // Fill V and W matrices
  for (int i = 0; i < _nhit; i++)
    for (int j = 0; j <= i; j++) {
      if (i == j) {
        vmtx[i*LMAX+j] += (_xs[i] > 0) ? _xs[i]*_xs[i] : 0;
        wmtx[i*LMAX+j] += (_ys[i] > 0) ? _ys[i]*_ys[i] : 0;
      }

      if (_mscat && i == j) {
	vmtx[i*LMAX+j] += (_xs[i] > 0) ? dmsc[i]*2.5 : 0;
	wmtx[i*LMAX+j] += (_ys[i] > 0) ? dmsc[i]     : 0;
      }
    }

  for (int i = 0; i < _nhit; i++)
    for (int j = 0; j < _nhit; j++) {
      if (vmtx[i*LMAX+j] != 0) vmtx[i*LMAX+j] = 1/vmtx[i*LMAX+j];
      if (wmtx[i*LMAX+j] != 0) wmtx[i*LMAX+j] = 1/wmtx[i*LMAX+j];
    }

  return 0;
}

int TrFit::JAMinParams(double *F, double *V, int side, int fix)
{
  double M[3][3];
  double vec[3];
  for (int i = 0; i < 3; i++) {
    vec[i] = 0;
    for (int j = 0; j < 3; j++) M[i][j] = 0;
  }

  double *x = (side == 0) ? _xh : _yh;
  double *e = (side == 0) ? _xs : _ys;

  // Fill vector and matrix
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      for (int k = 0; k < _nhit; k++)
        for (int l = 0; l < _nhit; l++) {
          if (j == 0)
           vec[i] += F[k*3+i]*V[k*LMAX+l]*x[l];
          M[i][j] += F[k*3+i]*V[k*LMAX+l]*F[l*3+j];
        }

  // For no magnetic field
  if (!fix && vec[2] == 0 && M[2][2] == 0) {
    fix = 1;
    _param[4] = 0;
  }

  // Fix par[4]
  if (fix) {
    vec[0] -= M[0][2]*_param[4]; M[0][2] = M[2][0] = 0;
    vec[1] -= M[1][2]*_param[4]; M[1][2] = M[2][1] = 0;
    vec[2]  =         _param[4]; M[2][2] = 1;
  }

  // Invert matrix
  if (Inv33(M) < 0) return -1;

  // Minimized parameters
  double p1 = M[0][0]*vec[0]+M[0][1]*vec[1]+M[0][2]*vec[2];
  double p2 = M[1][0]*vec[0]+M[1][1]*vec[1]+M[1][2]*vec[2];

  // Number of digrees of freedom
  int ndof = (fix) ? -(3-1) : -3;
  for (int i = 0; i < _nhit; i++) if (e[i] > 0) ndof++;
  if (ndof < 0) ndof = 0;

  // Minimized parameters
  if (side == 0) {
    _param[0] = p1;
    _param[2] = p2;
    _ndofx    = ndof;
  } else {
    _param[1] = p1;
    _param[3] = p2;
    _param[4] = M[2][0]*vec[0]+M[2][1]*vec[1]+M[2][2]*vec[2];
    _ndofy    = ndof;
  }
  if (!fix && M[2][2] > 0) _errrinv = std::sqrt(M[2][2])*1e12/Clight;

  return 0;
}

//#include "HistoMan.h"

double TrFit::ChoutkoFit(void)
{
  if(_nhit > TkDBc()->nlay() || 2*_nhit <= 5 || _chrg == 0) return -1;

  // Set initial parameters with SimpleFit
  double ssf = SimpleFit();
  if (ssf < 0) return -20+(int)ssf;

  _param[0] = _p0x; _param[1] = _dxdz;
  _param[2] = _p0y; _param[3] = _dydz;
  _param[4] = 1/_rigidity;

  _ndofx = -2;
  _ndofy = -3;
  for (int i = 0; i < _nhit; i++) {
    if (0 < _xs[i]) _ndofx++;
    if (0 < _ys[i]) _ndofy++;
    if (_zs[i] <= 0) _zs[i] = 300e-4;
    if (_xs[i] <= 0) _xs[i] = _zs[i]*1e8;
    if (_ys[i] <= 0) _ys[i] = _zs[i]*1e8;
  }

  int maxcal = 20;

  double tol = 0.1;

  // Multiple scattering constant
//double xls = 300.e-4;
//double rls = 9.36;
//double sms = 13.6e-3*std::sqrt(xls/rls)*1.5;

  double chisqb  = -1;
  double chisqbb = -1;

  int ifail = 2;

  double init[7], out[7];
  for (int i = 0; i < 7; i++) out[i] = 0;

  for (int kiter = 0; kiter < maxcal; kiter++) {
    double mm[NDIM][NDIM], gg[NDIM][NDIM], g[NDIM];
    for (int i = 0; i < NDIM*NDIM; i++) mm[i/NDIM][i%NDIM] = 0;
    for (int i = 0; i < NDIM*NDIM; i++) gg[i/NDIM][i%NDIM] = 0;
    for (int i = 0; i < NDIM; i++) g[i] = 0;
    for (int i = 0; i < NDIM; i++) mm[i][i] = 1;

    double xms[LMAX], fckx[LMAX], fcky[LMAX];

    double dmsc[LMAX];
    for (int i = 0; i < _nhit; i++) dmsc[i] = 0;
    if (_mscat && _param[4] != 0) {
      _rigidity = 1/_param[4];
      if (FillDmsc(dmsc) < 0) return -1;
    }

    double len [LMAX];
    for (int i = 0; i < _nhit; i++)
      len[i] = (i > 0) ? _zh[i]-_zh[i-1] : 0;

    // Loop for each point
    //for (int i = 0; i < _nhit; i++) {

    double pc[3];
    int ic = GetPcen(pc);
    if (ic <= 0) return -1;

    // Special treatment for Inner lower half
    if (_zh[0] < 50) ic = 0;

    for (int s = 0; s <= 1; s++) {
     int i1 = (s == 0) ? ic-1 :    ic;
     int i2 = (s == 0) ?   -1 : _nhit;
     int di = (s == 0) ?   -1 :     1;

     double dnorm = 1./std::sqrt(1+_param[1]*_param[1]+_param[3]*_param[3]);
     if (_zh[i1] < _zh[i2-di]) dnorm = -dnorm;

     init[0] = _param[0];
     init[1] = _param[2];
     init[2] =  0;
     init[3] = -_param[1]*dnorm;
     init[4] = -_param[3]*dnorm;
     init[5] = -dnorm;
     init[6] = (_param[4] != 0) ? _chrg/_param[4] : 1.e10;
     if (dnorm < 0) init[6] = -init[6];

     for (int i = 0; i < NDIM*NDIM; i++) mm[i/NDIM][i%NDIM] = 0;
     for (int i = 0; i < NDIM; i++) mm[i][i] = 1;

     for (int i = i1; i != i2; i += di) {
      double point[6] = { 0, 0, _zh[i], 0, 0, -1 };

      if (dnorm*init[5] > 0) {
	init[3] = -init[3];
	init[4] = -init[4];
	init[5] = -init[5];
      }

      // Transportation
      int clear = (i == i1 || i == i1+di) ? 0 : 1;
      //if (VCFitPar(init, out, point, mm, clear) < 0) return -3;
      VCFitPar(init, out, point, mm, clear);

      // Multiple scattering matrix
      xms[i] = (_mscat) ? dmsc[i] : 0;

      // Error matrix
      double aa[5];
      VCErrMtx(i, xms[i], out, aa, fckx[i], fcky[i]);

      // Fill residual
      _xr[i] = -aa[3];
      _yr[i] = -aa[4];

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
    }

    // Chisquare and residual
    _chisqx = _chisqy = _chisq = 0;
    for (int i = 0; i < _nhit; i++) {
      _chisqx += std::abs(fckx[i]);
      _chisqy += std::abs(fcky[i]);
      _chisq  += std::abs(fckx[i])+std::abs(fcky[i]);
    }

    // Check chisquare difference
    double dlt  = std::fabs((_chisq-chisqb )/(_chisq+1.e-3));
    double dltb = std::fabs((_chisq-chisqbb)/(_chisq+1.e-3));

    if (kiter >= 5) tol = 0.5;

  //if (kiter > 0) hman.Fill("TrChfit", kiter, dlt);
    if (kiter >= 2 && (dlt <= tol || dltb <= tol)) {
      ifail = 0;
      break;
    }
    chisqbb = chisqb;
    chisqb  = _chisq;

    // Invert covariance matrix
    if (TrFit::Inv55(gg) < 0) return -4;
    //PZ FPE Fix
    if(gg[4][4]<0) return -5;
    // Error of rigidity
    _errrinv = std::sqrt(2*gg[4][4]);

    // Solved parameters
    for (int i = 0; i < NDIM; i++) {
      double sol = (gg[0][i]*g[0]+gg[1][i]*g[1]+
		    gg[2][i]*g[2]+gg[3][i]*g[3]+gg[4][i]*g[4]);
      _param[i] += (i <= 3) ? -sol : sol;
    }

    // Check parameter range
    if (_param[4] <= -1e3) _param[4] = -1e3;
    if (_param[4] >=  1e3) _param[4] =  1e3;
  }

  if (ifail) return -6;

  _p0x = _param[0]; _dxdz = _param[1];
  _p0y = _param[2]; _dydz = _param[3];
  _p0z = 0;

  _param[4] = _param[4]*_chrg/std::fabs(_chrg);
  _rigidity = (_param[4] != 0) ? 1/_param[4] : 0;

  _chisq = (_ndofx+_ndofy > 0) ? _chisq/(_ndofx+_ndofy) : 0;

  return _chisq;
}

void TrFit::VCErrMtx(int ih, double xms, double *out, double *aa,
		     double &fckx, double &fcky)
{
  double mtmp[3];
  mtmp[0] = _xs[ih]*_xs[ih];
  mtmp[2] = _ys[ih]*_ys[ih];
  mtmp[1] = 0;

  if (_mscat != 0) {
    double vv = xms/out[5]/out[5]/out[5]/out[5];
    mtmp[0] += vv*(1-out[4]*out[4])*2.5;
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
  xperp[0] = out[0];
  xperp[1] = out[1];
  xperp[2] = _xh[ih];
  xperp[3] = _yh[ih];

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


int TrFit::RkmsDebug = -1;

#ifndef __ROOTSHAREDLIBRARY__
#include "amsgobj.h"
#endif

double TrFit::ChikanianFitCInt(int type)
/*
*-----------------------------------------------------------------------
* A.Chikanian, Yale, May-June 2010   (Revised version of 2003)
*-----------------------------------------------------------------------
* Rigidity reconstruction program for AMS02 with permanet magnet
* Taking care of the Multipole Scattering in AMS Tracking detector:
* Reconstruct track's kinematical parameters with full (nondiagonal)
* Covar.Error Metrix due to Multipole Scattering in Si tracking planes
* as well as in TRD, TOF and RICH.
*------------------------------------------------------

*     WARNING ========================================================
*     Due to Error Matrix depends on Rini, pick the best possible Rini
*     OR do final (additional) minimization loop with Rini~Rrec

   Imported to C++ by SH
 Feb 2012 Interface modified by PZ -- C Algo is probably outdated
*/
{
  _ndofx = -2;
  _ndofy = -3;
  for (int i = 0; i < _nhit; i++) {
    if (0 < _xs[i]) _ndofx++;
    if (0 < _ys[i]) _ndofy++;
    if (_zs[i] <= 0) _zs[i] = 300e-4;
    if (_xs[i] <= 0) _xs[i] = _zs[i]*1e4;
    if (_ys[i] <= 0) _ys[i] = _zs[i]*1e4;
  }

  double out[9];

#ifndef __ROOTSHAREDLIBRARY__

  // Imported C++ version
    AMSgObj::BookTimer.start("TrFitRkmsT");
    RkmsFit(out);
    AMSgObj::BookTimer.stop("TrFitRkmsT");
#else

  RkmsFit(out);
#endif

  if (RkmsDebug >= 1)
    cout << "RkmsFit: rini,rgt,chi2= "
	 << _rigidity << " " << out[5] << " " << out[6] << endl;

  _p0x = out[0]; 
  _p0y = out[1]; 
  _p0z = out[2];

  AMSDir dir(out[3], out[4]);
  _dxdz = -dir.x();
  _dydz = -dir.y();

  _rigidity = out[5];
  _chisq    = (_ndofx+_ndofy > 0) ? out[6]/(_ndofx+_ndofy) : out[6];
  _errrinv  = out[8];

  if (_chisqx < 0) _chisqx = out[6];
  if (_chisqy < 0) _chisqy = out[6];

  //-------------------------------------------- A.Ch. Apr.2011
  //After dropping MINUIT rkms flagging over out[7] insted chisq
  //return _chisq;
  float flag=1.;
  if(out[7]!=0.) flag=-1.;
  //cout<<"flag "<<flag<<" rigidity "<<_rigidity<<endl;
  return flag;
}

extern "C" void rkms_rig_(int*,int*,float*,float*,float*,float*,float*,float*);
extern "C" void rkmsinit_(float*);

double TrFit::ChikanianFitF()
/*
*-----------------------------------------------------------------------
* A.Chikanian, Yale, May-June 2010   (Revised version of 2003)
*-----------------------------------------------------------------------
* Rigidity reconstruction program for AMS02 with permanet magnet
* Taking care of the Multipole Scattering in AMS Tracking detector:
* Reconstruct track's kinematical parameters with full (nondiagonal)
* Covar.Error Metrix due to Multipole Scattering in Si tracking planes
* as well as in TRD, TOF and RICH.
*------------------------------------------------------

*     WARNING ========================================================
*     Due to Error Matrix depends on Rini, pick the best possible Rini
*     OR do final (additional) minimization loop with Rini~Rrec

   Imported to C++ by SH
Feb 2012 PZ update interface to new RKMS.F format
*/
{
  static int initialized=0;
  static float zpos[trconst::maxlay];
  int npo = 0, npl[NPma], ipa = 14;
  float xyz[NPma*3], dxyz[NPma*3];
  int   lay [trconst::maxlay] = { 8, 1, 2, 3, 4, 5, 6, 7, 9 };
  float out[41];

#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.start("TrFitRkmsF");
#endif

  _ndofx = -2;
  _ndofy = -3;
  for (int i = 0; i < _nhit; i++) {
    if (0 < _xs[i]) _ndofx++;
    if (0 < _ys[i]) _ndofy++;
    if (_zs[i] <= 0) _zs[i] = 300e-4;
    if (_xs[i] <= 0) _xs[i] = _zs[i]*1e4;
    if (_ys[i] <= 0) _ys[i] = _zs[i]*1e4;
  }

  if(!initialized){
    initialized=1;
    for (int i = 0; i < trconst::maxlay; i++)
      zpos[i] = TkDBc::Head->GetZlayer(lay[i]);
    rkmsinit_(zpos);
  }

  for (int i = 0; i < NPma; i++) {
    xyz[i*3] =  xyz[i*3+1] =  xyz[i*3+2] = 0;
    dxyz[i*3] = dxyz[i*3+1] = dxyz[i*3+2] = 0;
    npl[i] = 0;
  }

  for (int i = 0; i < GetNhit(); i++) {
    xyz[i*3  ] = GetXh(i);
    xyz[i*3+1] = GetYh(i);
    xyz[i*3+2] = GetZh(i);
    dxyz[i*3  ] = GetXs(i);
    dxyz[i*3+1] = GetYs(i);
    dxyz[i*3+2] = GetZs(i);
    
    if (dxyz[i*3  ] < 1e-4) dxyz[i*3  ] = 0.9999;
    if (dxyz[i*3+1] < 1e-4) dxyz[i*3+1] = 0.9999;
    if (dxyz[i*3+2] < 1e-4) dxyz[i*3+2] = 0.9999;
    
    int layer = GetLayer(GetZh(i));
    if (layer == 8) layer = 0;
    if (layer == 9) layer = 8;
    npl[i] = layer+1;
    npo++;
  }
  
  float rini = 10000.00;// GetRigidity(); // Compatible with TrackFit_Utils
  if(_rigidity==0) {
    int lmscat=_mscat;
    _mscat=0;
    SimpleFit();
    _mscat=lmscat;
  }
  rini = _rigidity;
  float beta   = GetBeta();
  float charge =_chrg;

//   if (_mass > 0 && _chrg != 0) {
//     float p   = _rigidity*_chrg;
//     float bi2 = (p != 0) ? 1+_mass*_mass/p/p : 0;
//     beta = (bi2 > 0) ? 1/std::sqrt(bi2) : 0;
//   }

  rkms_rig_(&npo, npl, xyz, dxyz, &beta, &charge, &rini, out);
  
  if (RkmsDebug >= 1)
    cout << "RkmsFit: rini,rgt,chi2= "
	 << _rigidity << " " << out[5] << " " << out[6] << endl;
  
  _p0x = out[0]; 
  _p0y = out[1]; 
  _p0z = out[2];

  AMSDir dir(out[3], out[4]);
  _dxdz = -dir.x();
  _dydz = -dir.y();

  _rigidity = out[5];
  _chisq    = (_ndofx+_ndofy > 0) ? out[6]/(_ndofx+_ndofy) : out[6];
  _errrinv  = out[8];

  if (_chisqx < 0) _chisqx = out[9];
  if (_chisqy < 0) _chisqy = out[10];

  //-------------------------------------------- A.Ch. Apr.2011
  //After dropping MINUIT rkms flagging over out[7] insted chisq
  //return _chisq;
  float flag=out[6];
  if(out[7]!=0.) flag=-1.;
  //cout<<"flag "<<flag<<" rigidity "<<_rigidity<<endl;
#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.stop("TrFitRkmsF");
#endif
  
  return flag;
}



#include "TMinuit.h"

namespace TrFit_RKMS {
  static TrFit *ptr = 0;
#pragma omp threadprivate(ptr)

  static TMinuit *fMinuit = 0;
#pragma omp threadprivate(fMinuit)

  void RkmsFCN(int &n, double *grad, double &chisq, double *par, int iflag)
  {
    if (ptr) ptr->RkmsFCN(n, grad, chisq, par, iflag);
  }
};

void TrFit::RkmsFit(double *out)
{
/*    subroutine rkms_rig(npo,npl,xyz,dxyz,ipa,Rini ,Out)
 *---------------------------------------------------------------------
 * A.Chikanian, Yale, 2003
 * Reconstruct track's kinematical parameters with full (nondiagonal)
 * Covar.Error Metrix due to Multipole Scattering in Si tracking planes.
 * Supposed to be used when the Mult.Scatt. is dominate.
 *---------------------------------------------------------------------
 * Input:   npo        - # points
 *          npl        - plane #, associated with each point
 *          xyz(3,npl) - 3d coordinates
 *          dxyz(npl)  - errors (intrinsic Si planes resolution)
 *          ipa        - particle Geant ID#
 *          Rini       - initial Rigidity value (try the best !!!)

 * Output:  Out(*)     - reconstructed parameters
 *          Out(1-3)   - x, y, z 
 *          Out(4-5)   - Theta, Phi
 *          Out(6)     - Rigidity
 *          Out(7)     - Chi2
 *          Out(8)     - not used
 *          Out(9)     - err(1/Rigidity)=err(R)/(R*R)
 *---------------------------------------------------
 *
   Imported to C++/ROOT by SH
 */

  if (_nhit < 3) return;

  // Set initial rigidity
  double rini = _rigidity;

  if (std::fabs(rini) < 0.3) {
    if (SimpleFit() < 0) return;
    rini = _rigidity;
  }

/*     subroutine rkms_fit(npo,npl,x,y,z,dx,dy,ipa,Rini,Xout,Er,Chi2) ! MINUIT
 *   A.Chikanian, Yale, 2003
 *
   Imported to C++/ROOT by SH
 */

/*--------------------------------------------------------------------------
 *                    
 * Initial MIGRAD steps dPx      dPy      dPz(GeV) dx     dy(cm) */
//double St[NDIM]   = { .000100, .000100, .002000, .0005, .0005 }; //! 8.98*min
// Initial MIGRAD steps dX/dZ  dY/dZ  1/R (1/GV)   dx     dy(cm)  // SH
  double St[NDIM]   = { .0001,   .0001,   .00001,  .0005, .0005 }; //! 8.98*min
/* see:/group/yaug1/prj/shik/AMS/MYAMS/TRKRKRES/MSMATR/RKMSFIT/derivstep.opt
 *--------------------------------------------------------------------------
 */

  TrFit_RKMS::ptr = this;

  if (!TrFit_RKMS::fMinuit) {
#pragma omp critical (tminuit)
    TrFit_RKMS::fMinuit = new TMinuit(NDIM+2);
    std::cout << "TrFit::RkmsFit-I-TMinuit object created" << std::endl;
  }

//TMinuit minuit(NDIM+2);
  TMinuit *minuit = TrFit_RKMS::fMinuit;
  minuit->mninit(5, 6, 7);
  minuit->SetFCN(TrFit_RKMS::RkmsFCN);

  double args[3] = { 0, 0, 0 };
  int narg = 1, ierr;

  //! suppr. output
  if (RkmsDebug == -2) {
    args[0] = 1; minuit->mnexcm("SET PRINT",  args, narg, ierr);
  }
  else {
    args[0] = -1; minuit->mnexcm("SET PRINT",  args, narg, ierr);
    args[0] = -1; minuit->mnexcm("SET NOWARN", args, narg, ierr);
  }
  if (ierr) cerr << "MNEXCM: Print ierflg=" << ierr << endl;

  args[0]= 1; minuit->mnexcm("SET GRAD", args, narg, ierr);
  if (ierr) cerr << "MNEXCM: Grad ierflg=" << ierr << endl;

  args[0]= 1; minuit->mnexcm("SET ERRo", args, narg, ierr);
  if(ierr) cerr << "MNEXCM: ERRo ierflg=" << ierr << endl;

  double z1  = _zh[0]; // ! if so Par(4)=x(1), Par[5]=y(1)
//double rad = std::sqrt((_xh[1]-_xh[0])*(_xh[1]-_xh[0])+
//			 (_yh[1]-_yh[0])*(_yh[1]-_yh[0])+
//			 (_zh[1]-_zh[0])*(_zh[1]-_zh[0]));

/*=============================
 *#include "../RKMSFIT/debug1.h"
 *=============================
 * First TWO passes in loop below are sign of charge checking
 * Third pass is iteration with right MS-matrix (mostly for correct Xi2)
 */

  double chi2b = 0;
  double chisq = 0;
  double xbuf[NDIM];
  double err [NDIM];

  int    npa = NDIM;
  double par[NDIM+2];
  double step = 10; //5;// ! cm // SH
  double sign = 1;

  enum { NitMa = 1 }; //3 }; // SH
  for (int it = 1; it <= NitMa; it++) {

    // Initial values
    if (it == NitMa) step = 2.;// ! cm
    if (it <= 2) {
                   sign =  1;
      if (it == 2) sign = -1;
// SH
      par[0] = (_xh[1]-_xh[0])/(_zh[1]-_zh[0]);                     // ! Px0
      par[1] = (_yh[1]-_yh[0])/(_zh[1]-_zh[0]);                     // ! Py0
      par[2] = 1/rini;                                              // ! Pz0
/* SH
      par[0] = rini*(_xh[1]-_xh[0])/rad;                            // ! Px0
      par[1] = rini*(_yh[1]-_yh[0])/rad;                            // ! Py0
      par[2] = rini*(_zh[1]-_zh[0])/rad;                            // ! Pz0
*/
      par[3] = _xh[0] + (z1-_zh[0])/(_zh[1]-_zh[0])*(_xh[1]-_xh[0]);// !  x0
      par[4] = _yh[0] + (z1-_zh[0])/(_zh[1]-_zh[0])*(_yh[1]-_yh[0]);// !  y0
    }
    else {
      for (int j = 0; j < npa; j++) par[j] = out[j];
    }

/*---------------------------- Initial values setup
 *----------------------------------------------- Minimization  
 *       ======================================+
 */
    RkmsMtx(rini); // Fill the error matrix

/*       ======================================+
 *=============================
 *#include "../RKMSFIT/debug2.h"
 *=============================
 *       ================================================
 */

//!$OMP CRITICAL (RKMS)  // TMinuit is thread-safe
    for (int i = 0; i < npa; i++) {
      minuit->mnparm(i, "par", par[i], St[i], 0, 0, ierr);//! Init.Val.Setup
      if(ierr) cerr << "### MNPARM: i,ierflg=" << i << " " << ierr << endl;
    }
    int np = npa;
    minuit->mnparm(np++, "step", step, 0, 0, 0, ierr);
    minuit->mnparm(np++, "sign", sign, 0, 0, 0, ierr);

    if (RkmsDebug >= 2) cout <<" MIGRAD it= " << it << endl;
    args[0] = 200;
    minuit->mnexcm("MIGRAD", args, 1, ierr);
    if (RkmsDebug >= 2) cout <<" MIGRAD ierflg= " << ierr << endl;

/*       ================================================
 cer        if(ierflg.ne.0.and.iercou.le.1) then        !
 cer          write(6,*)'MNEXCM: ierflg=',ierflg,iercou ! always 4 !!!???
 cer          iercou=iercou+1                           !
 cer        endif                                       !
 */

    TString str;
    for (int i = 0; i < npa; i++) {
      double bnd1, bnd2;
      int ivar;
      minuit->mnpout(i, str, out[i], err[i], bnd1, bnd2, ivar); //! Extraction
      par[i] = out[i];
    }
    par[npa]   = step;
    par[npa+1] = sign;

//!$OMP END CRITICAL (RKMS)  // TMinuit is thread-safe

    double chi2 = minuit->fAmin;
    if (chi2 > 1.e30) chisq = 1.e30;
    else chisq = chi2;
//c    if(2*npo-npa.gt.0) Chi2=Dhi2/(2.*npo-npa)      ! Chi2/d.f.

    if (it == 1) {
      chi2b = chisq;
      for (int i = 0; i < npa; i++) xbuf[i] = out[i];
    }
    if (it == 2 && chisq > chi2b) {
      sign = 1.;
      for (int i = 0; i < npa; i++) out[i] = xbuf[i];
    }

  }// ! Iteration loop

  // Set residuals
  RkmsFun(npa, par, true);

/*     ========================================================== */
/* SH
  double rgt = std::sqrt(out[0]*out[0]+out[1]*out[1]+out[2]*out[2]);
  double dr  = std::sqrt((out[0]*err[0])*(out[0]*err[0]) + 
			 (out[1]*err[1])*(out[1]*err[1]) +
			 (out[2]*err[2])*(out[2]*err[2]))/rgt;
*/
// SH
  double rgt = 1/out[2];
  double dr  = err[2]*rgt*rgt;

/*     ============================================= */
  double dd    = std::sqrt(out[0]*out[0]+out[1]*out[1]+1); // SH
  double theta = std::acos (1/dd);                         // SH
//double theta = std::acos (out[2]/rgt);                   // SH
  double phi   = std::atan2(out[1], out[0]);
  out[0] = out[3];
  out[1] = out[4];
  out[2] = _zh[0];
  out[3] = theta;
  out[4] = phi;
  out[5] = rgt*sign;
  out[6] = chisq;
  out[7] = 0.;
  out[8] = dr/(rgt*rgt);
}

#include "TMatrixDSym.h"

void TrFit::RkmsMtx(double rini)
{
/*    subroutine wxy_matr9(npo,npl,dx,dy,ipa,R)   !
 *-------------------------------------------------
 * Preparing Cov.Err.Matrix with Mult.Scattering
 * A.Chikanian Feb,2003.
 *     Revised June,2010
 *------------------------------------------------------------
 * Missing planes carring by putting dX,dY --> INF.(~22cm)
 * COS(Th) - is INDIV. for each planes
 * Symmetry of all matrix assumed
 *===========================================================
 *
   Imported to C++/ROOT by SH
 */

//double SiThick = 0.0300;    // ! plane thicknes in cm. (300mkm Si)
//double SiRlen  = 0.0032054; // ! plane thicknes in R.L.(300mkm Si) zz0
  double Rmin    = 0.300;     // ! Minimum rigidity, GV
//double xHC     = 1;         // ! Amount of HanyComb mater.in Si-pl.r.l

/*=== AMS01 Nov.29,1996
 c01      data trkZ0/50.95,29.15,7.75,-7.75,-29.15,-50.95/
 *=== AMS02 (Aug.25,04)
 c      data trkZ0/54.7,29.2,25.8,1.7,-1.7,-25.8,-29.2,-54.7/
 *   taken from MC (Sep.04)
 c      data trkZ0/54.51,29.18,25.32,1.68,-2.18,-25.32,-29.18,-54.51/
*/
//  TrkZ0 (now as RkmsZ0) to be filled from outside
/*
  double TrkZ0[NPma] = { 52.985, 29.185, 25.215,  1.685,
			 -2.285,-25.215,-29.185,-52.985 };// -122. };
*/
/*-------------------------------------------------------
 *   pl#   1-2   2-3   3-4   4-5   5-6   6-7  7-8
 *   dZ   21.8  24.4  15.5  24.4  21.8   -    -    AMS01
 *   dZ   25.5   3.4  24.1   3.4  24.1  3.4  25.5  AMS02
 *-------------------------------------------------------
 */

  int lay[trconst::maxlay] = { 8, 1, 2, 3, 4, 5, 6, 7, 9 };
  for (int i = 0; i < trconst::maxlay; i++)
    RkmsZ0[i] = TkDBc()->GetZlayer(lay[i]);

  double dZ[NPma][NPma];
  for (int i = 0; i < NPma-1; i++) {
    dZ[i][i] = 0.;
    for (int j = i+1; j < NPma; j++)
      dZ[i][j] = dZ[j][i] = RkmsZ0[j]-RkmsZ0[i];
  }
  dZ[NPma-1][NPma-1] = 0.;

  if (RkmsDebug >= 3){
    cout << "dZ" << endl;
    for (int i=0; i<NPma;i++) {
      for(int j=0;j<8;j++) cout<<Form("%10.2f",dZ[i][j]);
      cout<<endl;
    }
  }

  int npo = _nhit;
  int   npl[NPma];
  double xc[NPma], yc[NPma];
  for (int i = 0; i < npo; i++) {
    xc[i] = _xh[i];
    yc[i] = _yh[i];

    int layer = GetLayer(_zh[i]);
    if (layer == 8) layer = 0;
    if (layer == 9) layer = 8;
    npl[i] = layer+1;
  }

//double dTh0 = std::sqrt(2.)*13.6e-3*std::sqrt(SiRlen)
//                           *(1.+.038*std::log (SiRlen));

/*========================================= checkimg # of planes > 3 */
  if(_nhit < 3) {
    cerr << "### wxy_matr: # of planes " << _nhit << endl;
    return;
  }
/*================================== Initialization to 0 */
  double dms[NPma][NPma];
  double vx [NPma][NPma];
  double vy [NPma][NPma];
  double oco[NPma];
  for (int i=0; i < NPma*NPma; i++) {
    dms[i/NPma][i%NPma] 
      =       vx[i/NPma][i%NPma] =       vy[i/NPma][i%NPma]
      = _rkms_wx[i/NPma][i%NPma] = _rkms_wy[i/NPma][i%NPma] = 0;
    if (i < NPma) oco[i]=0.;
  }

  int np1 = npl[0];

  if (np1 > npl[npo-1]) {      // ! Down --> Up
    int np = np1;
    RkmsMscN1();
    if (3 <= np1 && np1 <= 9) {
      for (int i = 0; i < np; i++)
	for (int j = 0; j < np; j++)
	  dms[i][j] = _rkms_dmsN1[i][j][NPma-np1];
    }
  }
  else {                      //  ! Up --> Down
    int np = NPma-np1+1;
    RkmsMscN9();
    if (1 <= np1 && np1 <= 7) {
      for (int i = 0; i < np; i++)
	for (int j = 0; j < np; j++)
	  dms[i][j] = _rkms_dmsN9[i][j][np1-1];
    }
  }

 if (RkmsDebug >= 3) {
    cout<<"ZZ"<<endl;
    for(int i=0;i<8;i++) {
      for (int j=0;j<8;j++) cout<<Form("%10.2f",dms[i][j]);
      cout<<endl;
    }
  }

/*===========================================*/
//int NMSpl = abs(npl[npo-1]-npl[0]+np3);// ! # Mult.Scatt.planes
  int NMSpl = abs(npl[npo-1]-npl[0]+1);// ! # Mult.Scatt.planes
  //gfpart(ipa,pname,itr,ama,cha,tl,ub,nw);
  double cha  = _chrg; 
  double ama  = _mass;
  double RR   =  max(rini, Rmin);
  double P    =  RR*cha;
  double E    =  std::sqrt(P*P+ama*ama);
  double be   =  P/E;// ! or be from TOF
//double dTc2 = (dTh0/(be*RR))*(dTh0/(be*RR));// ! space factor 2 in dTh0
  double dTc2 = 1/(be*RR)/(be*RR);  // ! scatt.angle correction

//c      sigcha = 1.D0
//c      if(cha.lt.0.) sigcha = -1.D0 ! for GRKUTA 
/*=============================
 *#include "../RKMSFIT/debug7.h"
 *=============================
 *     ________________________________ Diagonal elements */

  double dx[NPma], dy[NPma], dz[NPma];
  for (int i = 0; i < npo; i++) {
    dz[i] = (_zs[i] > 0) ? _zs[i] : 300e-4;
    dx[i] = (_xs[i] > 0) ? _xs[i] : dz[i]*1e4;
    dy[i] = (_ys[i] > 0) ? _ys[i] : dz[i]*1e4;
  }

  vx[0][0] = dx[0]*dx[0];
  vy[0][0] = dy[0]*dy[0];
  
  int ii = 0;
  for (int i = 1; i < npo; i++) { // !_____________________Diagonal elements
    if (abs(npl[i]-npl[i-1]) > 1) {
      for (int is = 0; is < abs(npl[i]-npl[i-1])-1; is++) {
	ii++;
	oco[ii] = std::sqrt(1.+((xc[i]-xc[i-1])*(xc[i]-xc[i-1])+
				(yc[i]-yc[i-1])*(yc[i]-yc[i-1]))
			       /dZ[npl[i]-1][npl[i-1]-1]
			       /dZ[npl[i]-1][npl[i-1]-1]);
	vx[ii][ii] = 
	vy[ii][ii] = dms[ii][ii]*dTc2 + 1000.;// ! dX,Y~22cm if missing plane
      }
    }
    ii++;
    oco[ii]=std::sqrt(1.+((xc[i]-xc[i-1])*(xc[i]-xc[i-1])+
			  (yc[i]-yc[i-1])*(yc[i]-yc[i-1]))
		         /dZ[npl[i]-1][npl[i-1]-1]
		         /dZ[npl[i]-1][npl[i-1]-1]);
    vx[ii][ii] = dms[ii][ii]*dTc2*oco[ii] + dx[i]*dx[i];
    vy[ii][ii] = dms[ii][ii]*dTc2*oco[ii] + dy[i]*dy[i];
  }
  if (RkmsDebug >= 3){
    cout << "WW" << endl;
    for(int i=0;i<NPma;i++) {
      for (int j=0;j<8;j++) {
	if (std::abs(vy[i][j]) < 1) cout<<Form("%10.7f",vy[i][j]);
	else                        cout<<Form("%10.2f",vy[i][j]);
      }
      cout<<endl;
    }
  }

/*=============================
 *#include "../RKMSFIT/debug8.h"
 *=============================  */
  if (RkmsDebug >= 3){
    cout << "WW n= "<<NMSpl<<endl;
    for (int i = 0; i < NMSpl; i++) {
      for (int j=0;j<8;j++) {
	if (std::abs(vy[i][j]) < 1) cout<<Form("%10.7f",vy[i][j]);
	else                        cout<<Form("%10.1f",vy[i][j]);
      }
      cout<<endl;
    }
  }

/*=============================
 *#include "../RKMSFIT/debug9.h"
 *============================= */

  //rsinv(NMSpl,ww,NPma,ifail);// ! F012 Symm.Matr.Inversion
  TMatrixDSym mtx(NMSpl);
  TMatrixDSym mty(NMSpl);
  for (int i = 0; i < NMSpl; i++) 
    for (int j = 0; j <= i; j++) {
      mtx[i][j] = vx[i][j];
      mty[i][j] = vy[i][j];
    }

  double detx, dety;
  mtx.Invert(&detx);
  mty.Invert(&dety);
  if (detx == 0 || dety == 0) {
    if (RkmsDebug >= 1) cerr << "### rsinv: ifail= -1" << endl;
    return;
  }

  for (int i = 0; i < NMSpl; i++) 
    for (int j = 0; j < NMSpl; j++) {
      vx[i][j] = mtx[i][j];
      vy[i][j] = mty[i][j];
    }

  if (RkmsDebug >= 3){
    cout << "WW inv"<<endl;
    for (int i = 0; i < NMSpl; i++) {
      for (int j = 0; j < 8; j++) cout<<Form("%9.1f",vy[i][j]);
      cout<<endl;
    }
  }

/*==============================
 *#include "../RKMSFIT/debug10.h"
 *==============================
 *--------------------- Matr.Shrinking */
  _rkms_wx[0][0] = vx[0][0];
  _rkms_wy[0][0] = vy[0][0];
  for (int j = 1; j < npo; j++) {
    int jj = std::abs(npl[j]-npl[0]);
    for (int i = j; i < npo; i++) {
//    _rkms_wx[i][j] = vx[abs(npl[i]-npl[0]+np3)-1][jj];
//    _rkms_wy[i][j] = vy[abs(npl[i]-npl[0]+np3)-1][jj];
      int ii = std::abs(npl[i]-npl[0]);//+np3)-1;
      _rkms_wx[i][j] = vx[ii][jj];
      _rkms_wy[i][j] = vy[ii][jj];
    }
  }

  if (RkmsDebug >= 3){
    cout << "WY npo= "<<npo<<endl;
    for (int i = 0; i < npo; i++) {
      for (int j = 0; j < 8; j++) cout<<Form("%9.1f",_rkms_wy[i][j]);
      cout<<endl;
    }
  }

/*==============================
 *#include "../RKMSFIT/debug11.h"
 *============================== */
}

extern "C" void getdmsn9_(int*,int*,int*,float*);
extern "C" void getdmsn1_(int*,int*,int*,float*);

void TrFit::RkmsMscN9(void)
{
/* Cov.Err.Matrix with Mult.Scattering for Up --> Down track
 * A.Chikanian May,2010.
 *---------------------------------
 * Input:  npl1 - first plane
 * Output: dms  - scattering matrix
 *---------------------------------
 */

//  integer npl,npl1,N,i,j,k,jj,ii
  double sq3 =1.73205078;
/*  --------------------------------------------------------- Geometry
 *  L8(1N) TRD   TOF  HC2 L1 L2 L3 L4 L5 L6 L7  TOF  RICH  L9
 *    z1  p1-p2 p3-p4  p5 z2 z3-z4 z5-z6 z7-z8 p6-p7 p8-p9 z9
 *  ---------------------------------------------------------
 */
  double p[9] = { 156.15, 78.95  //!  TRD  p1,p2
	        ,  65.5 , 61.0   //!  TOF  p3,p4
	        ,  56.46         //!  HC2  p5
	        , -61.0 ,-65.5   //!  TOF2 p6,p7
	        , -74.5 ,-75.0}; //!  RICH p8,p9

  double z[9] = { 166.92         //!  z1   PL8 (1N)
	        ,  53.06         //!  z2   PL1
	        ,  29.2          //!  z3   PL2
		,  27.5          //!  z4   PL3
		,   1.7          //!  z5   PL4
		,  -1.7          //!  z6   PL5
		, -27.5          //!  z7   PL6
	        , -29.2          //!  z8   PL7
		,-135.48};       //!  z9   PL9

/*   -----------------------------------------------------
 *   X_Si = X_HC = 0.32054%
 *   X0_Rad = X_pl*den_pl/den_read=42.4*1.032/0.06=729cm
 *   X_TRD  = X_HC+X_rad+X_gaz=X_HC = 1.+ 6.+ 0.64 = 7.64%
 *   X_TOF  = 2cm/42.4cm =0.047 = 4.7%  (2cm of scintilator)
 *   X_RICH = 0.5cm/14.91cm = 0.0335 = 3.35%  (0.5cm of NaF)
 *   ----------------------------------------------------------------
 */

  double xRL[16] 
    //     Si      TRD    TOF
    = {0.0032054,0.0764,0.047,
    //   Si or HC
       0.0032054, 0.0032054, 0.0032054, 0.0032054, 0.0032054,
       0.0032054, 0.0032054, 0.0032054, 0.0032054, 0.0032054, 0.0032054,
    //  TOF   RICH
       0.047,0.0335};

  double  DT[16] = {0, 0, 0, 0, 0, 0, 0, 0 };
  double  dz[9][16];
  int     IS[9] = {0,4,5,7,8,10,11,13,16}; // ! right
  double  beta  = 1;
  double  rigid = 1; // ! 1 GeV/c

  for (int i = 0; i < 9; i++)
    for (int j = 0; j < 16; j++) dz[i][j] = 0;
		       
  // ! calculating and storring dmsN9 array
  for (int k = 0; k < NPma; k++) z[k] = RkmsZ0[k];

  for (int i = 0; i < NPma; i++)
    for (int j = 0; j < NPma; j++)
      for (int k = 0; k < 7; k++) _rkms_dmsN9[i][j][k] = 0;

  /*       --------------- scattering angles */
  for (int i = 0; i < 16; i++) {
    if(xRL[i] > 0.0001)
      DT[i]=xRL[i]*(0.0136/(beta*rigid)*(1.+0.038*std::log(xRL[i])))*
	           (0.0136/(beta*rigid)*(1.+0.038*std::log(xRL[i])));
  }
  /*       ---------------  dz matrix */
  for (int i = 1; i < 9; i++) { // do i=2,9
    dz[i][0] = z[i]-z[0];
    dz[i][1] =(p[1]-p[0])/sq3+z[i]-p[1];
    dz[i][2] =(p[3]-p[2])/sq3+z[i]-p[3];
    dz[i][3] =(z[1]-p[4])/sq3;
    if (i >= 2) {
      dz[i][4] = z[i]-z[1];
      if (i >= 3) {
	dz[i][5] = z[i]-z[2];
	dz[i][6] =(z[3]-z[2])/sq3;
	if (i >= 4) {
	  dz[i][7] = z[i]-z[3];
	  if (i >= 5) {
	    dz[i][8] = z[i]-z[4];
	    dz[i][9] =(z[5]-z[4])/sq3;
	    if (i >= 6) {
	      dz[i][10] = z[i]-z[5];
	      if (i >= 7) {
		dz[i][11] = z[i]-z[6];
		dz[i][12] =(z[7]-z[6])/sq3;
		if (i >= 8) {
		  dz[i][13]= z[i]-z[7];
		  dz[i][14]=(p[6]-p[5])/sq3+z[i]-p[6];
		  dz[i][15]=(p[8]-p[7])/sq3+z[i]-p[8];
		}
	      }
	    }
	  }
	}
      }
    }
  }
  /*       -------------------- dmsN9 filling */

  for (int N = 0; N < 7; N++) {  //  do N=1,7
    for (int i = N+1; i < NPma; i++) { // do i=N+1,NPma ! toge
      int ii=i-N;//+1;
      for (int j = i; j < NPma; j++) { // do j=i,NPma
	int jj=j-N;//+1;
	_rkms_dmsN9[ii][jj][N]=0.;
	for (int k = IS[N]; k < IS[i]; k++) //do k=IS(N)+1,IS(i)
	  _rkms_dmsN9[ii][jj][N] += dz[i][k]*dz[j][k]*DT[k];
	if(ii != jj) _rkms_dmsN9[jj][ii][N]=_rkms_dmsN9[ii][jj][N];
      }
    }
  }

//cout<<" ---- matr_scatN9 initialisation completed ----"<<endl;
}

void TrFit::RkmsMscN1(void)
{
/* Cov.Err.Matrix with Mult.Scattering for Down -->Up track
 * A.Chikanian May,2010.
 *---------------------------------
 * Input:  npl1 - first plane
 * Output: dms  - scattering matrix
 *---------------------------------
 */

//  integer npl,npl1,N,i,j,k,jj,ii
  double sq3 =1.73205078;
/*  --------------------------------------------------------- Geometry
 *  L8(1N) TRD   TOF  HC2 L1 L2 L3 L4 L5 L6 L7  TOF  RICH  L9
 *    z1  p1-p2 p3-p4  p5 z2 z3-z4 z5-z6 z7-z8 p6-p7 p8-p9 z9
 *  ---------------------------------------------------------
 */
  double p[9] = { 156.15, 78.95  //!  TRD  p1,p2
	        ,  65.5 , 61.0   //!  TOF  p3,p4
	        ,  56.46         //!  HC2  p5
	        , -61.0 ,-65.5   //!  TOF2 p6,p7
	        , -74.5 ,-75.0}; //!  RICH p8,p9

  double z[9] = { 166.92         //!  z1   PL8 (1N)
	        ,  53.06         //!  z2   PL1
	        ,  29.2          //!  z3   PL2
		,  27.5          //!  z4   PL3
		,   1.7          //!  z5   PL4
		,  -1.7          //!  z6   PL5
		, -27.5          //!  z7   PL6
	        , -29.2          //!  z8   PL7
		,-135.48};       //!  z9   PL9

/*   -----------------------------------------------------
 *   X_Si = X_HC = 0.32054%
 *   X0_Rad = X_pl*den_pl/den_read=42.4*1.032/0.06=729cm
 *   X_TRD  = X_HC+X_rad+X_gaz=X_HC = 1.+ 6.+ 0.64 = 7.64%
 *   X_TOF  = 2cm/42.4cm =0.047 = 4.7%  (2cm of scintilator)
 *   X_RICH = 0.5cm/14.91cm = 0.0335 = 3.35%  (0.5cm of NaF)
 *   ----------------------------------------------------------------
 */

  double xRL[16] 
    //       Si    RICH   TOF   
    = {0.0032054,0.0335,0.047,
    //   Si or HC
       0.0032054, 0.0032054, 0.0032054, 0.0032054, 0.0032054,
       0.0032054, 0.0032054, 0.0032054, 0.0032054, 0.0032054, 0.0032054,
    //   TOF    TRD
       0.047,0.0764};

  double  DT[16] = {0, 0, 0, 0, 0, 0, 0, 0 };
  double  dz[9][16];
  int     IS[9] = {0,3,5,6,8,9,11,12,16};
  double  beta  = 1;
  double  rigid = 1; // ! 1 GeV/c

  for (int i = 0; i < 9; i++)
    for (int j = 0; j < 16; j++) dz[i][j] = 0;
		       
  // ! calculating and storring dmsN1 array
  for (int k = 0; k < NPma; k++) z[k] = RkmsZ0[k];

  for (int i = 0; i < NPma; i++)
    for (int j = 0; j < NPma; j++)
      for (int k = 0; k < 7; k++) _rkms_dmsN1[i][j][k] = 0;

  /*       --------------- scattering angles */
  for (int i = 0; i < 16; i++) {
    if(xRL[i] > 0.0001)
      DT[i]=xRL[i]*(0.0136/(beta*rigid)*(1.+0.038*std::log(xRL[i])))*
	           (0.0136/(beta*rigid)*(1.+0.038*std::log(xRL[i])));
  }
  /*       ---------------  dz matrix */
  for (int i = 1; i < 9; i++) { // do i=2,9
    int j = 9-i-1;
    dz[i][0] = z[j]-z[8];
    dz[i][1] =(p[7]-p[8])/sq3+z[j]-p[7];
    dz[i][2] =(p[5]-p[6])/sq3+z[j]-p[5];
    if (i >= 2) {
      dz[i][3] = z[j]-z[7];
      dz[i][4] =(z[6]-z[7])/sq3;
      if (i >= 3) {
	dz[i][5] = z[j]-z[6];
	if (i >= 4) {
	  dz[i][6] = z[j]-z[5];
	  dz[i][7] =(z[4]-z[5])/sq3;
	  if (i >= 5) {
	    dz[i][8] = z[j]-z[4];
	    if (i >= 6) {
	      dz[i][9]  = z[j]-z[3];
	      dz[i][10] =(z[2]-z[3])/sq3;
	      if (i >= 7) {
		dz[i][11] = z[j]-z[2];
		if (i == 8) {
		  dz[i][12]= z[0]-z[1];
		  dz[i][13]=(p[4]-z[1])/sq3+z[0]-p[4];
		  dz[i][14]=(p[2]-p[3])/sq3+z[0]-p[2];
		  dz[i][15]=(p[0]-p[1])/sq3+z[0]-p[0];
		}
	      }
	    }
	  }
	}
      }
    }
  }
  /*       -------------------- dmsN1 filling */
  for (int N = 0; N < 7; N++) {  //  do N=1,7
    for (int i = N+1; i < NPma; i++) { // do i=N+1,NPma ! toge
      int ii=i-N;//+1;
      for (int j = i; j < NPma; j++) { // do j=i,NPma
	int jj=j-N;//+1;
	_rkms_dmsN1[ii][jj][N]=0.;
	for (int k = IS[N]; k < IS[i]; k++) //do k=IS(N)+1,IS(i)
	  _rkms_dmsN1[ii][jj][N] += dz[i][k]*dz[j][k]*DT[k];
	if(ii != jj) _rkms_dmsN1[jj][ii][N]=_rkms_dmsN1[ii][jj][N];
      }
    }
  }

//cout<<" ---- matr_scatN1 initialisation completed ----"<<endl;
}

void TrFit::RkmsFCN(int &npa, double *grad, double &fu, double *par, int iflag)
{
/*     Subroutine FunMI(Npa,grad,Fu,par,iflag) ! ,futil)
 *   A.Chikanian, Yale, Aug,2003
 *----------------------------------- */
//c      write(6,'(    a3,5f10.5)')'Par',par ! debug ###

  if (iflag == 2) {
/*    subroutine FunDer(npa,par,der)
 *   A.Chikanian, Yale, Aug,2003
 *---   numerical calculation of Fun and derivatives.
 *-------------------------------------------------- */
/*================================================== */
    double dp0[5] = { .000005,.000005,.000100,.0002,.0002 }; //! Optimal Steps
    /*                 GeV     GeV     GeV    cm    cm  !
     *================================================== */

  //double step = par[5];
  //double sign = par[6];

    double xx[7];
    for (int j = 0; j < npa+2 && j < 7; j++) xx[j] = par[j];

    for (int j = 0; j < npa; j++) {
      double dp = dp0[j];// ! /2.
      double x0 = xx [j];

      double f1, f2;
      xx[j] = x0-dp; f1 = RkmsFun(npa, xx);
      xx[j] = x0+dp; f2 = RkmsFun(npa, xx);

      grad[j] = .5*(f2-f1)/dp;
    }
    if (RkmsDebug >= 3){
      cout<<Form(" par  %12.4e%12.4e%12.4e%12.4e%12.4e",
		 par[0], par[1], par[2], par[3], par[4])<<endl;
      cout<<Form(" der  %12.4e%12.4e%12.4e%12.4e%12.4e",
		 grad[0], grad[1], grad[2], grad[3], grad[4])<<endl;
    }
  } // if (iflag == 2)
//c      write(6,'(a6,5(1pe12.4))')' der  ',der ! debug ###
//c      write(6,'(a6,5(1pe12.4))')'   x  ',xp  ! debug ###

  fu = RkmsFun(npa, par);

  if (RkmsDebug >= 2)
    cout<<Form("Par%2d%10.5f%10.5f%10.5f%10.5f%10.5f %10.5f",
	       iflag, par[0], par[1], par[2], par[3], par[4], fu) << endl;
//c      write(6,*)'iflag,Fu',iflag,Fu       ! debug ###
}

double TrFit::RkmsFun(int npa, double *par, bool res)
{
/*    double precision Function Fun(Npa,par)
 *   A.Chikanian, Yale, Aug,2003
 *   Function for Minimization
 */
  enum { NPma = LMAX };

  double pin[3], xin[3], x0[NPma], y0[NPma];

// SH
  double dd = -std::sqrt(par[0]*par[0]+par[1]*par[1]+1); // down-going
  double pp = (par[2] != 0) ? abs(1/par[2]) : 0;         // abs.momentum
  pin[0] = par[0]/dd*pp; // ! Px
  pin[1] = par[1]/dd*pp; // ! Py
  pin[2] =      1/dd*pp; // ! Pz
/* SH
  pin[0] = par[0]; // ! Px or cx
  pin[1] = par[1]; // ! Py or cy
  pin[2] = par[2]; // ! Pz or R
*/
  xin[0] = par[3]; // ! x
  xin[1] = par[4]; // ! y
  xin[2] = _zh[0]; // ! z

  for (int i = 0; i < NPma; i++) {
    x0[i] = -999.;
    y0[i] = -999.;
  }
/*     ========================== */

/*    subroutine rk_trk(pin,xin,x0,y0,w0)
 *---------------------------------------------------
 * Runge-Kutta integration for particle in Magn.field
 * A.Chikanian, Yale, Feb.,2003
 *----------------------------------------------------
 *  Input:  pin(3) - initial Rigidity Px,Py,Pz (GeV/c)
 *          xin(3) - initial position  x, y, z (cm)
 *  Output: pou(3) -   final Rigidity Px,Py,Pz (GeV/c)
 *          xou(3) -   final position  x, y, z (cm)
 *------------------------------------------------- */

/*==================================
 *#include "../RKMSFIT/instedgeant.h"
 *================================== */
  double vect[7], vout[7];
/*                           vect(7) is  x,y,z,px,py,pz,P
 *                                   or  x,y,z,cx,cy,cz,P
 *                                       1 2 3  4  5  6 7 */
  double Stot = 350.; // ! Max.range(cm) in Path Length
/*------------------------------------------------------- */

  double step = par[5];
  double sign = par[6];
  double vni  = 1.;

  int Nst  = int(Stot/step); // ! Max.# of steps
  int npoc = _nhit;

  double xc[NPma], yc[NPma], zc[NPma];
  for (int i = 0; i < npoc; i++) {
    xc[i] = _xh[i];
    yc[i] = _yh[i];
    zc[i] = _zh[i];
  }

  if (zc[0] < zc[1]) vni = -1.; // ! back tracking

  sign = (par[2] > 0) ? 1 : -1;
  vect[6] = std::sqrt(pin[0]*pin[0]+pin[1]*pin[1]+pin[2]*pin[2]); //SH

//cxy3 ---
/* SH
  if (pin[2] > 0) {
    vect[6] = -vect[6];
    sign    = -sign;
  }
*/

  for (int i = 0; i < 3; i++) {
    vect[i  ] = xin[i];//         !  xyz
    vect[i+3] = pin[i]/vect[6];// ! cxyz
  }
/*----------------------------------------------- Runge Kutta */
  if (RkmsDebug >= 3) cout<<" rk_trk "<<Nst<<" "<<Stot<<" "<<step<<endl;

  double w0[3][NPma];

  int np = 1;
  for (int i = 0; i < Nst; i++) {
    if (RkmsDebug >= 3) 
      cout<<Form(" %2d %9.4f%9.4f%9.3f%9.4f%9.4f%9.4f",
		 i, vect[0], vect[1], vect[2],
		    vect[3], vect[4], vect[5])<<endl;
    TrFit::Rkuta(sign, step, vect, vout);

    for (int j = 0; j < 2; j++) { // !!!! fixed close planes problem(AMS02)
      if (vni*vect[2] >= vni*zc[np-1] && 
	  vni*vout[2] <  vni*zc[np-1]) {
	double w = (zc[np-1]-vout[2])/(vect[2]-vout[2]);
	x0   [np-1] = vect[0]*w + vout[0]*(1.-w);
	y0   [np-1] = vect[1]*w + vout[1]*(1.-w);
	w0[0][np-1] = vect[3]*w + vout[3]*(1.-w);
	w0[1][np-1] = vect[4]*w + vout[4]*(1.-w);
	w0[2][np-1] = vect[5]*w + vout[5]*(1.-w);

	if (RkmsDebug >= 3) {
	  cout<<Form(" np %2d", np) << endl;
	  cout<<Form("%9.4f%9.4f%9.4f%9.3f"
		     "%9.4f%9.4f%9.4f%9.4f",
		     x0[0], x0[1], x0[2], x0[3], x0[4], x0[5], x0[6], x0[7])
	      <<endl;
	  cout<<Form("%9.4f%9.4f%9.4f%9.3f"
		     "%9.4f%9.4f%9.4f%9.4f",
		     y0[0], y0[1], y0[2], y0[3], y0[4], y0[5], y0[6], y0[7])
	      <<endl;
	}
//c          write(6,'(a3,i2,a3,8f10.5)')' np',np,' x0',x0 ! debug ###
//c          write(6,'(   5x,a3,8f10.5)')         ' y0',y0 ! debug ###
	if (np > npoc) break; //  !!!!!!!!! Done
	np = np+1;
      }
    } //  !### fixing closed planes probl.(or run with small STEP)

    if (np > npoc) break; //  !!!!!!!!! Done

    for (int j = 0; j < 7; j++) // ! x,y,z,cx,cy,cz,P
      vect[j] = vout[j];
  } // ! Nst 
/*==============================
 *#include "../RKMSFIT/debug5a.h"
 *============================== */

  if (RkmsDebug >= 3) {
    cout << " i      xc       yc       zc       x0       y0" << endl;
    for (int i = 0; i < npoc; i++)
      cout<<Form("%2d%9.3f%9.3f%9.3f%9.3f%9.3f",
		 i, xc[i], yc[i], zc[i], x0[i], y0[i])<<endl;
  }


/*     double precision function Chi2Su(x0,y0)
 *   A.Chikanian, Yale, Feb,2003
 *             Revised June,2010
 *---------------------------------------------
 * Input    x0     - x-coord. for current iteration
 *          y0     - y-coord. for current iteration
 * Output:  Chi2Su - chi2 value
 *--------------------------------------------- */

/*-------------------------*  xc,yc - hits coordinates */
//double chi2su = _rkms_err[0][0]*((xc[0]-x0[0])*(xc[0]-x0[0])+
//		   (yc[0]-y0[0])*(yc[0]-y0[0]));
  double chi2su = _rkms_wx[0][0]*(xc[0]-x0[0])*(xc[0]-x0[0])+
                  _rkms_wy[0][0]*(yc[0]-y0[0])*(yc[0]-y0[0]);
  if (RkmsDebug >= 3) cout<<"Chi2Su"<<endl;
  for (int i = 1; i < npoc; i++) {
    double xi = xc[i]-x0[i];
    double yi = yc[i]-y0[i];
    if (RkmsDebug >= 3) 
      cout<<Form("%2d%9.3f%9.1f%9.3f%9.3f%9.3f%9.3f%9.3f%9.3f",
		 i, chi2su, _rkms_wx[i][i], xi, yi,
		 xc[i], x0[i], yc[i], y0[i])<<endl;
  //chi2su += _rkms_err[i][i]*(xi*xi+yi*yi);
    chi2su += _rkms_wx[i][i]*xi*xi+
              _rkms_wx[i][i]*yi*yi;
    if (i < npoc-1) {
      for (int j = i+1; j < npoc; j++)
      //chi2su += 2.*_rkms_err[j][i]*(xi*(xc[j]-x0[j])+yi*(yc[j]-y0[j]));
	chi2su += 2.*(_rkms_wx[j][i]*xi*(xc[j]-x0[j])+
		      _rkms_wy[j][i]*yi*(yc[j]-y0[j]));
    }
  }

  // Fill residual, _chisqx and _chisqy
  if (res) {
    _chisqx = 0;
    _chisqy = 0;
    for (int i = 0; i < npoc; i++) {
      _xr[i] = xc[i]-x0[i];
      _yr[i] = yc[i]-y0[i];

      _chisqx += _rkms_wx[i][i]*_xr[i]*_xr[i];
      _chisqy += _rkms_wy[i][i]*_yr[i]*_yr[i];

      for (int j = i+1; j < npoc; j++) {
	_chisqx += 2.*_rkms_wx[j][i]*_xr[i]*(xc[j]-x0[j]);
	_chisqy += 2.*_rkms_wy[j][i]*_yr[i]*(yc[j]-y0[j]); 
      }
      if (RkmsDebug >= 2) 
	cout<<Form("Residual %d %8.4f %8.4f %8.4f %8.4f %8.4f %8.4f",
		   i, xc[i], x0[i], yc[i], y0[i], _xr[i], _yr[i])<<endl;
    }
    if (RkmsDebug >= 2) 
      cout<<"chisq= "<<_chisqx<<" "<<_chisqy<<endl;
  }

  return chi2su;
/*=============================
 c#include "../RKMSFIT/debug5.h"
 *============================= */
/*     ========================== */
/*=============================
 *#include "../RKMSFIT/debug4.h"
 *============================= */
}


int TrFit::InvSM(double **M, int ndim)
{
  TMatrixDSym mtx(ndim);

  for (int i = 0; i < ndim; i++) 
    for (int j = 0; j <= i; j++) mtx[i][j] = M[i][j];

  double det;
  mtx.Invert(&det);
  if (det == 0) return 0;

  for (int i = 0; i < ndim; i++) 
    for (int j = 0; j < ndim; j++) M[i][j] = M[j][i] = mtx[i][j];

  return 1;
}

int TrFit::InvSM(double *M, int ndim)
{
  TMatrixDSym mtx(ndim);

  for (int i = 0; i < ndim; i++) 
    for (int j = 0; j <= i; j++) mtx[i][j] = M[i*ndim+j];

  double det;
  mtx.Invert(&det);
  if (det == 0) return 0;

  for (int i = 0; i < ndim; i++) 
    for (int j = 0; j < ndim; j++) M[i*ndim+j] = M[j*ndim+i] = mtx[i][j];

  return 1;
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

int TrFit::Inv66(double M[6][6])
{
  // Find all NECESSARY 2x2 dets:  (45 of them)
  double d2_34_01 = M[3][0]*M[4][1] - M[3][1]*M[4][0];
  double d2_34_02 = M[3][0]*M[4][2] - M[3][2]*M[4][0];
  double d2_34_03 = M[3][0]*M[4][3] - M[3][3]*M[4][0];
  double d2_34_04 = M[3][0]*M[4][4] - M[3][4]*M[4][0];
  double d2_34_05 = M[3][0]*M[4][5] - M[3][5]*M[4][0];
  double d2_34_12 = M[3][1]*M[4][2] - M[3][2]*M[4][1];
  double d2_34_13 = M[3][1]*M[4][3] - M[3][3]*M[4][1];
  double d2_34_14 = M[3][1]*M[4][4] - M[3][4]*M[4][1];
  double d2_34_15 = M[3][1]*M[4][5] - M[3][5]*M[4][1];
  double d2_34_23 = M[3][2]*M[4][3] - M[3][3]*M[4][2];
  double d2_34_24 = M[3][2]*M[4][4] - M[3][4]*M[4][2];
  double d2_34_25 = M[3][2]*M[4][5] - M[3][5]*M[4][2];
  double d2_34_34 = M[3][3]*M[4][4] - M[3][4]*M[4][3];
  double d2_34_35 = M[3][3]*M[4][5] - M[3][5]*M[4][3];
  double d2_34_45 = M[3][4]*M[4][5] - M[3][5]*M[4][4];
  double d2_35_01 = M[3][0]*M[5][1] - M[3][1]*M[5][0];
  double d2_35_02 = M[3][0]*M[5][2] - M[3][2]*M[5][0];
  double d2_35_03 = M[3][0]*M[5][3] - M[3][3]*M[5][0];
  double d2_35_04 = M[3][0]*M[5][4] - M[3][4]*M[5][0];
  double d2_35_05 = M[3][0]*M[5][5] - M[3][5]*M[5][0];
  double d2_35_12 = M[3][1]*M[5][2] - M[3][2]*M[5][1];
  double d2_35_13 = M[3][1]*M[5][3] - M[3][3]*M[5][1];
  double d2_35_14 = M[3][1]*M[5][4] - M[3][4]*M[5][1];
  double d2_35_15 = M[3][1]*M[5][5] - M[3][5]*M[5][1];
  double d2_35_23 = M[3][2]*M[5][3] - M[3][3]*M[5][2];
  double d2_35_24 = M[3][2]*M[5][4] - M[3][4]*M[5][2];
  double d2_35_25 = M[3][2]*M[5][5] - M[3][5]*M[5][2];
  double d2_35_34 = M[3][3]*M[5][4] - M[3][4]*M[5][3];
  double d2_35_35 = M[3][3]*M[5][5] - M[3][5]*M[5][3];
  double d2_35_45 = M[3][4]*M[5][5] - M[3][5]*M[5][4];
  double d2_45_01 = M[4][0]*M[5][1] - M[4][1]*M[5][0];
  double d2_45_02 = M[4][0]*M[5][2] - M[4][2]*M[5][0];
  double d2_45_03 = M[4][0]*M[5][3] - M[4][3]*M[5][0];
  double d2_45_04 = M[4][0]*M[5][4] - M[4][4]*M[5][0];
  double d2_45_05 = M[4][0]*M[5][5] - M[4][5]*M[5][0];
  double d2_45_12 = M[4][1]*M[5][2] - M[4][2]*M[5][1];
  double d2_45_13 = M[4][1]*M[5][3] - M[4][3]*M[5][1];
  double d2_45_14 = M[4][1]*M[5][4] - M[4][4]*M[5][1];
  double d2_45_15 = M[4][1]*M[5][5] - M[4][5]*M[5][1];
  double d2_45_23 = M[4][2]*M[5][3] - M[4][3]*M[5][2];
  double d2_45_24 = M[4][2]*M[5][4] - M[4][4]*M[5][2];
  double d2_45_25 = M[4][2]*M[5][5] - M[4][5]*M[5][2];
  double d2_45_34 = M[4][3]*M[5][4] - M[4][4]*M[5][3];
  double d2_45_35 = M[4][3]*M[5][5] - M[4][5]*M[5][3];
  double d2_45_45 = M[4][4]*M[5][5] - M[4][5]*M[5][4];

  // Find all NECESSARY 3x3 dets:  (80 of them)
  double d3_234_012 = M[2][0]*d2_34_12 - M[2][1]*d2_34_02 + M[2][2]*d2_34_01;
  double d3_234_013 = M[2][0]*d2_34_13 - M[2][1]*d2_34_03 + M[2][3]*d2_34_01;
  double d3_234_014 = M[2][0]*d2_34_14 - M[2][1]*d2_34_04 + M[2][4]*d2_34_01;
  double d3_234_015 = M[2][0]*d2_34_15 - M[2][1]*d2_34_05 + M[2][5]*d2_34_01;
  double d3_234_023 = M[2][0]*d2_34_23 - M[2][2]*d2_34_03 + M[2][3]*d2_34_02;
  double d3_234_024 = M[2][0]*d2_34_24 - M[2][2]*d2_34_04 + M[2][4]*d2_34_02;
  double d3_234_025 = M[2][0]*d2_34_25 - M[2][2]*d2_34_05 + M[2][5]*d2_34_02;
  double d3_234_034 = M[2][0]*d2_34_34 - M[2][3]*d2_34_04 + M[2][4]*d2_34_03;
  double d3_234_035 = M[2][0]*d2_34_35 - M[2][3]*d2_34_05 + M[2][5]*d2_34_03;
  double d3_234_045 = M[2][0]*d2_34_45 - M[2][4]*d2_34_05 + M[2][5]*d2_34_04;
  double d3_234_123 = M[2][1]*d2_34_23 - M[2][2]*d2_34_13 + M[2][3]*d2_34_12;
  double d3_234_124 = M[2][1]*d2_34_24 - M[2][2]*d2_34_14 + M[2][4]*d2_34_12;
  double d3_234_125 = M[2][1]*d2_34_25 - M[2][2]*d2_34_15 + M[2][5]*d2_34_12;
  double d3_234_134 = M[2][1]*d2_34_34 - M[2][3]*d2_34_14 + M[2][4]*d2_34_13;
  double d3_234_135 = M[2][1]*d2_34_35 - M[2][3]*d2_34_15 + M[2][5]*d2_34_13;
  double d3_234_145 = M[2][1]*d2_34_45 - M[2][4]*d2_34_15 + M[2][5]*d2_34_14;
  double d3_234_234 = M[2][2]*d2_34_34 - M[2][3]*d2_34_24 + M[2][4]*d2_34_23;
  double d3_234_235 = M[2][2]*d2_34_35 - M[2][3]*d2_34_25 + M[2][5]*d2_34_23;
  double d3_234_245 = M[2][2]*d2_34_45 - M[2][4]*d2_34_25 + M[2][5]*d2_34_24;
  double d3_234_345 = M[2][3]*d2_34_45 - M[2][4]*d2_34_35 + M[2][5]*d2_34_34;
  double d3_235_012 = M[2][0]*d2_35_12 - M[2][1]*d2_35_02 + M[2][2]*d2_35_01;
  double d3_235_013 = M[2][0]*d2_35_13 - M[2][1]*d2_35_03 + M[2][3]*d2_35_01;
  double d3_235_014 = M[2][0]*d2_35_14 - M[2][1]*d2_35_04 + M[2][4]*d2_35_01;
  double d3_235_015 = M[2][0]*d2_35_15 - M[2][1]*d2_35_05 + M[2][5]*d2_35_01;
  double d3_235_023 = M[2][0]*d2_35_23 - M[2][2]*d2_35_03 + M[2][3]*d2_35_02;
  double d3_235_024 = M[2][0]*d2_35_24 - M[2][2]*d2_35_04 + M[2][4]*d2_35_02;
  double d3_235_025 = M[2][0]*d2_35_25 - M[2][2]*d2_35_05 + M[2][5]*d2_35_02;
  double d3_235_034 = M[2][0]*d2_35_34 - M[2][3]*d2_35_04 + M[2][4]*d2_35_03;
  double d3_235_035 = M[2][0]*d2_35_35 - M[2][3]*d2_35_05 + M[2][5]*d2_35_03;
  double d3_235_045 = M[2][0]*d2_35_45 - M[2][4]*d2_35_05 + M[2][5]*d2_35_04;
  double d3_235_123 = M[2][1]*d2_35_23 - M[2][2]*d2_35_13 + M[2][3]*d2_35_12;
  double d3_235_124 = M[2][1]*d2_35_24 - M[2][2]*d2_35_14 + M[2][4]*d2_35_12;
  double d3_235_125 = M[2][1]*d2_35_25 - M[2][2]*d2_35_15 + M[2][5]*d2_35_12;
  double d3_235_134 = M[2][1]*d2_35_34 - M[2][3]*d2_35_14 + M[2][4]*d2_35_13;
  double d3_235_135 = M[2][1]*d2_35_35 - M[2][3]*d2_35_15 + M[2][5]*d2_35_13;
  double d3_235_145 = M[2][1]*d2_35_45 - M[2][4]*d2_35_15 + M[2][5]*d2_35_14;
  double d3_235_234 = M[2][2]*d2_35_34 - M[2][3]*d2_35_24 + M[2][4]*d2_35_23;
  double d3_235_235 = M[2][2]*d2_35_35 - M[2][3]*d2_35_25 + M[2][5]*d2_35_23;
  double d3_235_245 = M[2][2]*d2_35_45 - M[2][4]*d2_35_25 + M[2][5]*d2_35_24;
  double d3_235_345 = M[2][3]*d2_35_45 - M[2][4]*d2_35_35 + M[2][5]*d2_35_34;
  double d3_245_012 = M[2][0]*d2_45_12 - M[2][1]*d2_45_02 + M[2][2]*d2_45_01;
  double d3_245_013 = M[2][0]*d2_45_13 - M[2][1]*d2_45_03 + M[2][3]*d2_45_01;
  double d3_245_014 = M[2][0]*d2_45_14 - M[2][1]*d2_45_04 + M[2][4]*d2_45_01;
  double d3_245_015 = M[2][0]*d2_45_15 - M[2][1]*d2_45_05 + M[2][5]*d2_45_01;
  double d3_245_023 = M[2][0]*d2_45_23 - M[2][2]*d2_45_03 + M[2][3]*d2_45_02;
  double d3_245_024 = M[2][0]*d2_45_24 - M[2][2]*d2_45_04 + M[2][4]*d2_45_02;
  double d3_245_025 = M[2][0]*d2_45_25 - M[2][2]*d2_45_05 + M[2][5]*d2_45_02;
  double d3_245_034 = M[2][0]*d2_45_34 - M[2][3]*d2_45_04 + M[2][4]*d2_45_03;
  double d3_245_035 = M[2][0]*d2_45_35 - M[2][3]*d2_45_05 + M[2][5]*d2_45_03;
  double d3_245_045 = M[2][0]*d2_45_45 - M[2][4]*d2_45_05 + M[2][5]*d2_45_04;
  double d3_245_123 = M[2][1]*d2_45_23 - M[2][2]*d2_45_13 + M[2][3]*d2_45_12;
  double d3_245_124 = M[2][1]*d2_45_24 - M[2][2]*d2_45_14 + M[2][4]*d2_45_12;
  double d3_245_125 = M[2][1]*d2_45_25 - M[2][2]*d2_45_15 + M[2][5]*d2_45_12;
  double d3_245_134 = M[2][1]*d2_45_34 - M[2][3]*d2_45_14 + M[2][4]*d2_45_13;
  double d3_245_135 = M[2][1]*d2_45_35 - M[2][3]*d2_45_15 + M[2][5]*d2_45_13;
  double d3_245_145 = M[2][1]*d2_45_45 - M[2][4]*d2_45_15 + M[2][5]*d2_45_14;
  double d3_245_234 = M[2][2]*d2_45_34 - M[2][3]*d2_45_24 + M[2][4]*d2_45_23;
  double d3_245_235 = M[2][2]*d2_45_35 - M[2][3]*d2_45_25 + M[2][5]*d2_45_23;
  double d3_245_245 = M[2][2]*d2_45_45 - M[2][4]*d2_45_25 + M[2][5]*d2_45_24;
  double d3_245_345 = M[2][3]*d2_45_45 - M[2][4]*d2_45_35 + M[2][5]*d2_45_34;
  double d3_345_012 = M[3][0]*d2_45_12 - M[3][1]*d2_45_02 + M[3][2]*d2_45_01;
  double d3_345_013 = M[3][0]*d2_45_13 - M[3][1]*d2_45_03 + M[3][3]*d2_45_01;
  double d3_345_014 = M[3][0]*d2_45_14 - M[3][1]*d2_45_04 + M[3][4]*d2_45_01;
  double d3_345_015 = M[3][0]*d2_45_15 - M[3][1]*d2_45_05 + M[3][5]*d2_45_01;
  double d3_345_023 = M[3][0]*d2_45_23 - M[3][2]*d2_45_03 + M[3][3]*d2_45_02;
  double d3_345_024 = M[3][0]*d2_45_24 - M[3][2]*d2_45_04 + M[3][4]*d2_45_02;
  double d3_345_025 = M[3][0]*d2_45_25 - M[3][2]*d2_45_05 + M[3][5]*d2_45_02;
  double d3_345_034 = M[3][0]*d2_45_34 - M[3][3]*d2_45_04 + M[3][4]*d2_45_03;
  double d3_345_035 = M[3][0]*d2_45_35 - M[3][3]*d2_45_05 + M[3][5]*d2_45_03;
  double d3_345_045 = M[3][0]*d2_45_45 - M[3][4]*d2_45_05 + M[3][5]*d2_45_04;
  double d3_345_123 = M[3][1]*d2_45_23 - M[3][2]*d2_45_13 + M[3][3]*d2_45_12;
  double d3_345_124 = M[3][1]*d2_45_24 - M[3][2]*d2_45_14 + M[3][4]*d2_45_12;
  double d3_345_125 = M[3][1]*d2_45_25 - M[3][2]*d2_45_15 + M[3][5]*d2_45_12;
  double d3_345_134 = M[3][1]*d2_45_34 - M[3][3]*d2_45_14 + M[3][4]*d2_45_13;
  double d3_345_135 = M[3][1]*d2_45_35 - M[3][3]*d2_45_15 + M[3][5]*d2_45_13;
  double d3_345_145 = M[3][1]*d2_45_45 - M[3][4]*d2_45_15 + M[3][5]*d2_45_14;
  double d3_345_234 = M[3][2]*d2_45_34 - M[3][3]*d2_45_24 + M[3][4]*d2_45_23;
  double d3_345_235 = M[3][2]*d2_45_35 - M[3][3]*d2_45_25 + M[3][5]*d2_45_23;
  double d3_345_245 = M[3][2]*d2_45_45 - M[3][4]*d2_45_25 + M[3][5]*d2_45_24;
  double d3_345_345 = M[3][3]*d2_45_45 - M[3][4]*d2_45_35 + M[3][5]*d2_45_34;
 
  // Find all NECESSARY 4x4 ds:  (75 of them)
  double d4_1234_0123 = M[1][0]*d3_234_123 - M[1][1]*d3_234_023 
                      + M[1][2]*d3_234_013 - M[1][3]*d3_234_012;
  double d4_1234_0124 = M[1][0]*d3_234_124 - M[1][1]*d3_234_024 
                      + M[1][2]*d3_234_014 - M[1][4]*d3_234_012;
  double d4_1234_0125 = M[1][0]*d3_234_125 - M[1][1]*d3_234_025 
                      + M[1][2]*d3_234_015 - M[1][5]*d3_234_012;
  double d4_1234_0134 = M[1][0]*d3_234_134 - M[1][1]*d3_234_034 
                      + M[1][3]*d3_234_014 - M[1][4]*d3_234_013;
  double d4_1234_0135 = M[1][0]*d3_234_135 - M[1][1]*d3_234_035
                      + M[1][3]*d3_234_015 - M[1][5]*d3_234_013;
  double d4_1234_0145 = M[1][0]*d3_234_145 - M[1][1]*d3_234_045
                      + M[1][4]*d3_234_015 - M[1][5]*d3_234_014;
  double d4_1234_0234 = M[1][0]*d3_234_234 - M[1][2]*d3_234_034 
                      + M[1][3]*d3_234_024 - M[1][4]*d3_234_023;
  double d4_1234_0235 = M[1][0]*d3_234_235 - M[1][2]*d3_234_035 
                      + M[1][3]*d3_234_025 - M[1][5]*d3_234_023;
  double d4_1234_0245 = M[1][0]*d3_234_245 - M[1][2]*d3_234_045 
                      + M[1][4]*d3_234_025 - M[1][5]*d3_234_024;
  double d4_1234_0345 = M[1][0]*d3_234_345 - M[1][3]*d3_234_045 
                      + M[1][4]*d3_234_035 - M[1][5]*d3_234_034;
  double d4_1234_1234 = M[1][1]*d3_234_234 - M[1][2]*d3_234_134 
                      + M[1][3]*d3_234_124 - M[1][4]*d3_234_123;
  double d4_1234_1235 = M[1][1]*d3_234_235 - M[1][2]*d3_234_135 
                      + M[1][3]*d3_234_125 - M[1][5]*d3_234_123;
  double d4_1234_1245 = M[1][1]*d3_234_245 - M[1][2]*d3_234_145 
                      + M[1][4]*d3_234_125 - M[1][5]*d3_234_124;
  double d4_1234_1345 = M[1][1]*d3_234_345 - M[1][3]*d3_234_145 
                      + M[1][4]*d3_234_135 - M[1][5]*d3_234_134;
  double d4_1234_2345 = M[1][2]*d3_234_345 - M[1][3]*d3_234_245 
                      + M[1][4]*d3_234_235 - M[1][5]*d3_234_234;
  double d4_1235_0123 = M[1][0]*d3_235_123 - M[1][1]*d3_235_023 
                      + M[1][2]*d3_235_013 - M[1][3]*d3_235_012;
  double d4_1235_0124 = M[1][0]*d3_235_124 - M[1][1]*d3_235_024 
                      + M[1][2]*d3_235_014 - M[1][4]*d3_235_012;
  double d4_1235_0125 = M[1][0]*d3_235_125 - M[1][1]*d3_235_025 
                      + M[1][2]*d3_235_015 - M[1][5]*d3_235_012;
  double d4_1235_0134 = M[1][0]*d3_235_134 - M[1][1]*d3_235_034 
                      + M[1][3]*d3_235_014 - M[1][4]*d3_235_013;
  double d4_1235_0135 = M[1][0]*d3_235_135 - M[1][1]*d3_235_035 
                      + M[1][3]*d3_235_015 - M[1][5]*d3_235_013;
  double d4_1235_0145 = M[1][0]*d3_235_145 - M[1][1]*d3_235_045 
                      + M[1][4]*d3_235_015 - M[1][5]*d3_235_014;
  double d4_1235_0234 = M[1][0]*d3_235_234 - M[1][2]*d3_235_034 
                      + M[1][3]*d3_235_024 - M[1][4]*d3_235_023;
  double d4_1235_0235 = M[1][0]*d3_235_235 - M[1][2]*d3_235_035 
                      + M[1][3]*d3_235_025 - M[1][5]*d3_235_023;
  double d4_1235_0245 = M[1][0]*d3_235_245 - M[1][2]*d3_235_045 
                      + M[1][4]*d3_235_025 - M[1][5]*d3_235_024;
  double d4_1235_0345 = M[1][0]*d3_235_345 - M[1][3]*d3_235_045 
                      + M[1][4]*d3_235_035 - M[1][5]*d3_235_034;
  double d4_1235_1234 = M[1][1]*d3_235_234 - M[1][2]*d3_235_134 
                      + M[1][3]*d3_235_124 - M[1][4]*d3_235_123;
  double d4_1235_1235 = M[1][1]*d3_235_235 - M[1][2]*d3_235_135 
                      + M[1][3]*d3_235_125 - M[1][5]*d3_235_123;
  double d4_1235_1245 = M[1][1]*d3_235_245 - M[1][2]*d3_235_145 
                      + M[1][4]*d3_235_125 - M[1][5]*d3_235_124;
  double d4_1235_1345 = M[1][1]*d3_235_345 - M[1][3]*d3_235_145 
                      + M[1][4]*d3_235_135 - M[1][5]*d3_235_134;
  double d4_1235_2345 = M[1][2]*d3_235_345 - M[1][3]*d3_235_245 
                      + M[1][4]*d3_235_235 - M[1][5]*d3_235_234;
  double d4_1245_0123 = M[1][0]*d3_245_123 - M[1][1]*d3_245_023 
                      + M[1][2]*d3_245_013 - M[1][3]*d3_245_012;
  double d4_1245_0124 = M[1][0]*d3_245_124 - M[1][1]*d3_245_024 
                      + M[1][2]*d3_245_014 - M[1][4]*d3_245_012;
  double d4_1245_0125 = M[1][0]*d3_245_125 - M[1][1]*d3_245_025 
                      + M[1][2]*d3_245_015 - M[1][5]*d3_245_012;
  double d4_1245_0134 = M[1][0]*d3_245_134 - M[1][1]*d3_245_034 
                      + M[1][3]*d3_245_014 - M[1][4]*d3_245_013;
  double d4_1245_0135 = M[1][0]*d3_245_135 - M[1][1]*d3_245_035 
                      + M[1][3]*d3_245_015 - M[1][5]*d3_245_013;
  double d4_1245_0145 = M[1][0]*d3_245_145 - M[1][1]*d3_245_045 
                      + M[1][4]*d3_245_015 - M[1][5]*d3_245_014;
  double d4_1245_0234 = M[1][0]*d3_245_234 - M[1][2]*d3_245_034 
                      + M[1][3]*d3_245_024 - M[1][4]*d3_245_023;
  double d4_1245_0235 = M[1][0]*d3_245_235 - M[1][2]*d3_245_035 
                      + M[1][3]*d3_245_025 - M[1][5]*d3_245_023;
  double d4_1245_0245 = M[1][0]*d3_245_245 - M[1][2]*d3_245_045 
                      + M[1][4]*d3_245_025 - M[1][5]*d3_245_024;
  double d4_1245_0345 = M[1][0]*d3_245_345 - M[1][3]*d3_245_045 
                      + M[1][4]*d3_245_035 - M[1][5]*d3_245_034;
  double d4_1245_1234 = M[1][1]*d3_245_234 - M[1][2]*d3_245_134 
                      + M[1][3]*d3_245_124 - M[1][4]*d3_245_123;
  double d4_1245_1235 = M[1][1]*d3_245_235 - M[1][2]*d3_245_135 
                      + M[1][3]*d3_245_125 - M[1][5]*d3_245_123;
  double d4_1245_1245 = M[1][1]*d3_245_245 - M[1][2]*d3_245_145 
                      + M[1][4]*d3_245_125 - M[1][5]*d3_245_124;
  double d4_1245_1345 = M[1][1]*d3_245_345 - M[1][3]*d3_245_145 
                      + M[1][4]*d3_245_135 - M[1][5]*d3_245_134;
  double d4_1245_2345 = M[1][2]*d3_245_345 - M[1][3]*d3_245_245 
                      + M[1][4]*d3_245_235 - M[1][5]*d3_245_234;
  double d4_1345_0123 = M[1][0]*d3_345_123 - M[1][1]*d3_345_023 
                      + M[1][2]*d3_345_013 - M[1][3]*d3_345_012;
  double d4_1345_0124 = M[1][0]*d3_345_124 - M[1][1]*d3_345_024 
                      + M[1][2]*d3_345_014 - M[1][4]*d3_345_012;
  double d4_1345_0125 = M[1][0]*d3_345_125 - M[1][1]*d3_345_025 
                      + M[1][2]*d3_345_015 - M[1][5]*d3_345_012;
  double d4_1345_0134 = M[1][0]*d3_345_134 - M[1][1]*d3_345_034 
                      + M[1][3]*d3_345_014 - M[1][4]*d3_345_013;
  double d4_1345_0135 = M[1][0]*d3_345_135 - M[1][1]*d3_345_035 
                      + M[1][3]*d3_345_015 - M[1][5]*d3_345_013;
  double d4_1345_0145 = M[1][0]*d3_345_145 - M[1][1]*d3_345_045 
                      + M[1][4]*d3_345_015 - M[1][5]*d3_345_014;
  double d4_1345_0234 = M[1][0]*d3_345_234 - M[1][2]*d3_345_034 
                      + M[1][3]*d3_345_024 - M[1][4]*d3_345_023;
  double d4_1345_0235 = M[1][0]*d3_345_235 - M[1][2]*d3_345_035 
                      + M[1][3]*d3_345_025 - M[1][5]*d3_345_023;
  double d4_1345_0245 = M[1][0]*d3_345_245 - M[1][2]*d3_345_045 
                      + M[1][4]*d3_345_025 - M[1][5]*d3_345_024;
  double d4_1345_0345 = M[1][0]*d3_345_345 - M[1][3]*d3_345_045 
                      + M[1][4]*d3_345_035 - M[1][5]*d3_345_034;
  double d4_1345_1234 = M[1][1]*d3_345_234 - M[1][2]*d3_345_134 
                      + M[1][3]*d3_345_124 - M[1][4]*d3_345_123;
  double d4_1345_1235 = M[1][1]*d3_345_235 - M[1][2]*d3_345_135 
                      + M[1][3]*d3_345_125 - M[1][5]*d3_345_123;
  double d4_1345_1245 = M[1][1]*d3_345_245 - M[1][2]*d3_345_145 
                      + M[1][4]*d3_345_125 - M[1][5]*d3_345_124;
  double d4_1345_1345 = M[1][1]*d3_345_345 - M[1][3]*d3_345_145 
                      + M[1][4]*d3_345_135 - M[1][5]*d3_345_134;
  double d4_1345_2345 = M[1][2]*d3_345_345 - M[1][3]*d3_345_245 
                      + M[1][4]*d3_345_235 - M[1][5]*d3_345_234;
  double d4_2345_0123 = M[2][0]*d3_345_123 - M[2][1]*d3_345_023 
                      + M[2][2]*d3_345_013 - M[2][3]*d3_345_012;
  double d4_2345_0124 = M[2][0]*d3_345_124 - M[2][1]*d3_345_024 
                      + M[2][2]*d3_345_014 - M[2][4]*d3_345_012;
  double d4_2345_0125 = M[2][0]*d3_345_125 - M[2][1]*d3_345_025 
                      + M[2][2]*d3_345_015 - M[2][5]*d3_345_012;
  double d4_2345_0134 = M[2][0]*d3_345_134 - M[2][1]*d3_345_034 
                      + M[2][3]*d3_345_014 - M[2][4]*d3_345_013;
  double d4_2345_0135 = M[2][0]*d3_345_135 - M[2][1]*d3_345_035 
                      + M[2][3]*d3_345_015 - M[2][5]*d3_345_013;
  double d4_2345_0145 = M[2][0]*d3_345_145 - M[2][1]*d3_345_045 
                      + M[2][4]*d3_345_015 - M[2][5]*d3_345_014;
  double d4_2345_0234 = M[2][0]*d3_345_234 - M[2][2]*d3_345_034 
                      + M[2][3]*d3_345_024 - M[2][4]*d3_345_023;
  double d4_2345_0235 = M[2][0]*d3_345_235 - M[2][2]*d3_345_035 
                      + M[2][3]*d3_345_025 - M[2][5]*d3_345_023;
  double d4_2345_0245 = M[2][0]*d3_345_245 - M[2][2]*d3_345_045 
                      + M[2][4]*d3_345_025 - M[2][5]*d3_345_024;
  double d4_2345_0345 = M[2][0]*d3_345_345 - M[2][3]*d3_345_045 
                      + M[2][4]*d3_345_035 - M[2][5]*d3_345_034;
  double d4_2345_1234 = M[2][1]*d3_345_234 - M[2][2]*d3_345_134 
                      + M[2][3]*d3_345_124 - M[2][4]*d3_345_123;
  double d4_2345_1235 = M[2][1]*d3_345_235 - M[2][2]*d3_345_135 
                      + M[2][3]*d3_345_125 - M[2][5]*d3_345_123;
  double d4_2345_1245 = M[2][1]*d3_345_245 - M[2][2]*d3_345_145 
                      + M[2][4]*d3_345_125 - M[2][5]*d3_345_124;
  double d4_2345_1345 = M[2][1]*d3_345_345 - M[2][3]*d3_345_145 
                      + M[2][4]*d3_345_135 - M[2][5]*d3_345_134;
  double d4_2345_2345 = M[2][2]*d3_345_345 - M[2][3]*d3_345_245 
                      + M[2][4]*d3_345_235 - M[2][5]*d3_345_234;

  // Find all NECESSARY 5x5 ds:  (36 of them)
  double d5_01234_01234 = M[0][0]*d4_1234_1234 - M[0][1]*d4_1234_0234 
                        + M[0][2]*d4_1234_0134 - M[0][3]*d4_1234_0124
                        + M[0][4]*d4_1234_0123;
  double d5_01234_01235 = M[0][0]*d4_1234_1235 - M[0][1]*d4_1234_0235
                        + M[0][2]*d4_1234_0135 - M[0][3]*d4_1234_0125
                        + M[0][5]*d4_1234_0123;
  double d5_01234_01245 = M[0][0]*d4_1234_1245 - M[0][1]*d4_1234_0245
                        + M[0][2]*d4_1234_0145 - M[0][4]*d4_1234_0125
                        + M[0][5]*d4_1234_0124;
  double d5_01234_01345 = M[0][0]*d4_1234_1345 - M[0][1]*d4_1234_0345
                        + M[0][3]*d4_1234_0145 - M[0][4]*d4_1234_0135
                        + M[0][5]*d4_1234_0134;
  double d5_01234_02345 = M[0][0]*d4_1234_2345 - M[0][2]*d4_1234_0345
                        + M[0][3]*d4_1234_0245 - M[0][4]*d4_1234_0235
                        + M[0][5]*d4_1234_0234;
  double d5_01234_12345 = M[0][1]*d4_1234_2345 - M[0][2]*d4_1234_1345
                        + M[0][3]*d4_1234_1245 - M[0][4]*d4_1234_1235
                        + M[0][5]*d4_1234_1234;
  double d5_01235_01234 = M[0][0]*d4_1235_1234 - M[0][1]*d4_1235_0234
                        + M[0][2]*d4_1235_0134 - M[0][3]*d4_1235_0124
                        + M[0][4]*d4_1235_0123;
  double d5_01235_01235 = M[0][0]*d4_1235_1235 - M[0][1]*d4_1235_0235
                        + M[0][2]*d4_1235_0135 - M[0][3]*d4_1235_0125
                        + M[0][5]*d4_1235_0123;
  double d5_01235_01245 = M[0][0]*d4_1235_1245 - M[0][1]*d4_1235_0245
                        + M[0][2]*d4_1235_0145 - M[0][4]*d4_1235_0125
                        + M[0][5]*d4_1235_0124;
  double d5_01235_01345 = M[0][0]*d4_1235_1345 - M[0][1]*d4_1235_0345
                        + M[0][3]*d4_1235_0145 - M[0][4]*d4_1235_0135
                        + M[0][5]*d4_1235_0134;
  double d5_01235_02345 = M[0][0]*d4_1235_2345 - M[0][2]*d4_1235_0345
                        + M[0][3]*d4_1235_0245 - M[0][4]*d4_1235_0235
                        + M[0][5]*d4_1235_0234;
  double d5_01235_12345 = M[0][1]*d4_1235_2345 - M[0][2]*d4_1235_1345
                        + M[0][3]*d4_1235_1245 - M[0][4]*d4_1235_1235
                        + M[0][5]*d4_1235_1234;
  double d5_01245_01234 = M[0][0]*d4_1245_1234 - M[0][1]*d4_1245_0234
                        + M[0][2]*d4_1245_0134 - M[0][3]*d4_1245_0124
                        + M[0][4]*d4_1245_0123;
  double d5_01245_01235 = M[0][0]*d4_1245_1235 - M[0][1]*d4_1245_0235
                        + M[0][2]*d4_1245_0135 - M[0][3]*d4_1245_0125
                        + M[0][5]*d4_1245_0123;
  double d5_01245_01245 = M[0][0]*d4_1245_1245 - M[0][1]*d4_1245_0245
                        + M[0][2]*d4_1245_0145 - M[0][4]*d4_1245_0125
                        + M[0][5]*d4_1245_0124;
  double d5_01245_01345 = M[0][0]*d4_1245_1345 - M[0][1]*d4_1245_0345
                        + M[0][3]*d4_1245_0145 - M[0][4]*d4_1245_0135
                        + M[0][5]*d4_1245_0134;
  double d5_01245_02345 = M[0][0]*d4_1245_2345 - M[0][2]*d4_1245_0345
                        + M[0][3]*d4_1245_0245 - M[0][4]*d4_1245_0235
                        + M[0][5]*d4_1245_0234;
  double d5_01245_12345 = M[0][1]*d4_1245_2345 - M[0][2]*d4_1245_1345
                        + M[0][3]*d4_1245_1245 - M[0][4]*d4_1245_1235
                        + M[0][5]*d4_1245_1234;
  double d5_01345_01234 = M[0][0]*d4_1345_1234 - M[0][1]*d4_1345_0234
                        + M[0][2]*d4_1345_0134 - M[0][3]*d4_1345_0124
                        + M[0][4]*d4_1345_0123;
  double d5_01345_01235 = M[0][0]*d4_1345_1235 - M[0][1]*d4_1345_0235
                        + M[0][2]*d4_1345_0135 - M[0][3]*d4_1345_0125
                        + M[0][5]*d4_1345_0123;
  double d5_01345_01245 = M[0][0]*d4_1345_1245 - M[0][1]*d4_1345_0245
                        + M[0][2]*d4_1345_0145 - M[0][4]*d4_1345_0125
                        + M[0][5]*d4_1345_0124;
  double d5_01345_01345 = M[0][0]*d4_1345_1345 - M[0][1]*d4_1345_0345
                        + M[0][3]*d4_1345_0145 - M[0][4]*d4_1345_0135
                        + M[0][5]*d4_1345_0134;
  double d5_01345_02345 = M[0][0]*d4_1345_2345 - M[0][2]*d4_1345_0345
                        + M[0][3]*d4_1345_0245 - M[0][4]*d4_1345_0235
                        + M[0][5]*d4_1345_0234;
  double d5_01345_12345 = M[0][1]*d4_1345_2345 - M[0][2]*d4_1345_1345
                        + M[0][3]*d4_1345_1245 - M[0][4]*d4_1345_1235
                        + M[0][5]*d4_1345_1234;
  double d5_02345_01234 = M[0][0]*d4_2345_1234 - M[0][1]*d4_2345_0234
                        + M[0][2]*d4_2345_0134 - M[0][3]*d4_2345_0124
                        + M[0][4]*d4_2345_0123;
  double d5_02345_01235 = M[0][0]*d4_2345_1235 - M[0][1]*d4_2345_0235
                        + M[0][2]*d4_2345_0135 - M[0][3]*d4_2345_0125
                        + M[0][5]*d4_2345_0123;
  double d5_02345_01245 = M[0][0]*d4_2345_1245 - M[0][1]*d4_2345_0245
                        + M[0][2]*d4_2345_0145 - M[0][4]*d4_2345_0125
                        + M[0][5]*d4_2345_0124;
  double d5_02345_01345 = M[0][0]*d4_2345_1345 - M[0][1]*d4_2345_0345
                        + M[0][3]*d4_2345_0145 - M[0][4]*d4_2345_0135
                        + M[0][5]*d4_2345_0134;
  double d5_02345_02345 = M[0][0]*d4_2345_2345 - M[0][2]*d4_2345_0345
                        + M[0][3]*d4_2345_0245 - M[0][4]*d4_2345_0235
                        + M[0][5]*d4_2345_0234;
  double d5_02345_12345 = M[0][1]*d4_2345_2345 - M[0][2]*d4_2345_1345
                        + M[0][3]*d4_2345_1245 - M[0][4]*d4_2345_1235
                        + M[0][5]*d4_2345_1234;
  double d5_12345_01234 = M[1][0]*d4_2345_1234 - M[1][1]*d4_2345_0234
                        + M[1][2]*d4_2345_0134 - M[1][3]*d4_2345_0124
                        + M[1][4]*d4_2345_0123;
  double d5_12345_01235 = M[1][0]*d4_2345_1235 - M[1][1]*d4_2345_0235
                        + M[1][2]*d4_2345_0135 - M[1][3]*d4_2345_0125
                        + M[1][5]*d4_2345_0123;
  double d5_12345_01245 = M[1][0]*d4_2345_1245 - M[1][1]*d4_2345_0245
                        + M[1][2]*d4_2345_0145 - M[1][4]*d4_2345_0125
                        + M[1][5]*d4_2345_0124;
  double d5_12345_01345 = M[1][0]*d4_2345_1345 - M[1][1]*d4_2345_0345
                        + M[1][3]*d4_2345_0145 - M[1][4]*d4_2345_0135
                        + M[1][5]*d4_2345_0134;
  double d5_12345_02345 = M[1][0]*d4_2345_2345 - M[1][2]*d4_2345_0345
                        + M[1][3]*d4_2345_0245 - M[1][4]*d4_2345_0235
                        + M[1][5]*d4_2345_0234;
  double d5_12345_12345 = M[1][1]*d4_2345_2345 - M[1][2]*d4_2345_1345
                        + M[1][3]*d4_2345_1245 - M[1][4]*d4_2345_1235
                        + M[1][5]*d4_2345_1234;

  // Find the derminant 
  double det = M[0][0]*d5_12345_12345 - M[0][1]*d5_12345_02345 
             + M[0][2]*d5_12345_01345 - M[0][3]*d5_12345_01245 
             + M[0][4]*d5_12345_01235 - M[0][5]*d5_12345_01234;
  if (det == 0) return -1;

  double oneOverDet = 1.0/det;
  double mn1OverDet = - oneOverDet;
  M[0][0] =  d5_12345_12345*oneOverDet;
  M[0][1] =  d5_02345_12345*mn1OverDet;
  M[0][2] =  d5_01345_12345*oneOverDet;
  M[0][3] =  d5_01245_12345*mn1OverDet;
  M[0][4] =  d5_01235_12345*oneOverDet;
  M[0][5] =  d5_01234_12345*mn1OverDet;

  M[1][0] =  d5_12345_02345*mn1OverDet;
  M[1][1] =  d5_02345_02345*oneOverDet;
  M[1][2] =  d5_01345_02345*mn1OverDet;
  M[1][3] =  d5_01245_02345*oneOverDet;
  M[1][4] =  d5_01235_02345*mn1OverDet;
  M[1][5] =  d5_01234_02345*oneOverDet;

  M[2][0] =  d5_12345_01345*oneOverDet;
  M[2][1] =  d5_02345_01345*mn1OverDet;
  M[2][2] =  d5_01345_01345*oneOverDet;
  M[2][3] =  d5_01245_01345*mn1OverDet;
  M[2][4] =  d5_01235_01345*oneOverDet;
  M[2][5] =  d5_01234_01345*mn1OverDet;

  M[3][0] =  d5_12345_01245*mn1OverDet;
  M[3][1] =  d5_02345_01245*oneOverDet;
  M[3][2] =  d5_01345_01245*mn1OverDet;
  M[3][3] =  d5_01245_01245*oneOverDet;
  M[3][4] =  d5_01235_01245*mn1OverDet;
  M[3][5] =  d5_01234_01245*oneOverDet;

  M[4][0] =  d5_12345_01235*oneOverDet;
  M[4][1] =  d5_02345_01235*mn1OverDet;
  M[4][2] =  d5_01345_01235*oneOverDet;
  M[4][3] =  d5_01245_01235*mn1OverDet;
  M[4][4] =  d5_01235_01235*oneOverDet;
  M[4][5] =  d5_01234_01235*mn1OverDet;

  M[5][0] =  d5_12345_01234*mn1OverDet;
  M[5][1] =  d5_02345_01234*oneOverDet;
  M[5][2] =  d5_01345_01234*mn1OverDet;
  M[5][3] =  d5_01245_01234*oneOverDet;
  M[5][4] =  d5_01235_01234*mn1OverDet;
  M[5][5] =  d5_01234_01234*oneOverDet;

  return 0;
}

//--------------------------------------------------------------------------
//          TRPROP IMPLEMENTATION 
//--------------------------------------------------------------------------
double TrProp::Mproton = 0.938272297;     // Proton  mass in GeV/c^2
double TrProp::Mhelium = 3.727379240;     // Helium4 mass in GeV/c^2
double TrProp::Mmuon   = 0.105658367;     // Muon    mass in GeV/c^2
double TrProp::Clight  = 2.99792458e+08;  // Speed of light in m/s

TrProp::TrProp(double p0x,   double p0y, double p0z, 
               double theta, double phi, double rigidity)
  : _p0x(p0x), _p0y(p0y), _p0z(p0z), _rigidity(rigidity)
{
  _mass = Mproton;
  _chrg = 1;

  AMSDir dir(theta, phi);
  _dxdz = (dir.z() != 0) ? dir.x()/dir.z() : 0;
  _dydz = (dir.z() != 0) ? dir.y()/dir.z() : 0;
}

TrProp::TrProp(AMSPoint p0, AMSDir dir, double rigidity)
  : _p0x(p0.x()), _p0y(p0.y()), _p0z(p0.z()), _rigidity(rigidity)
{
  _mass = Mproton;
  _chrg = 1;

  _dxdz = (dir.z() != 0) ? dir.x()/dir.z() : 0;
  _dydz = (dir.z() != 0) ? dir.y()/dir.z() : 0;
}

void TrProp::Clear()
{
  _mass = Mproton;
  _chrg = 1;
  _rigidity = 0;
  _p0x = _p0y = _p0z = _dxdz = _dydz = 0;
}

double TrProp::Propagate(double zpl)
{
  AMSPoint pnt(0, 0, zpl);
  AMSDir   dir(0, 0,   1);

  double len = Interpolate(pnt, dir);
  _p0x  = pnt[0];
  _p0y  = pnt[1];
  _p0z  = pnt[2];
  _dxdz = dir[0]/dir[2];
  _dydz = dir[1]/dir[2];

  return len;
}

void TrProp::Interpolate(int nz, double *zpl,
                         AMSPoint *plist, AMSDir *dlist, double *llist)
{
  AMSPoint psave(_p0x, _p0y, _p0z);
  AMSDir   dsave(_dxdz, _dydz,  1);

  for (int i = 0; i < nz; i++) {
    double len   = Propagate(zpl[i]);
    double dnorm = std::sqrt(_dxdz*_dxdz+_dydz*_dydz+1);
    if (plist) plist[i].setp(_p0x, _p0y, _p0z);
    if (dlist) dlist[i].setp(_dxdz/dnorm, _dydz/dnorm, 1/dnorm);
    if (llist) llist[i] = len;
  }

  _p0x  = psave[0]; _p0y  = psave[1]; _p0z  = psave[2];
  _dxdz = dsave[0]/dsave[2]; _dydz = dsave[1]/dsave[2];
}

double TrProp::Interpolate(AMSPoint &pnt, AMSDir &dir)
{
  // Check on dir
  if (dir[0] == 0 && dir[1] == 0 && dir[2] == 0) return -1;

  // Linear track case
  if ( _chrg*_rigidity == 0) {
    double z = (dir[0]*(pnt[0]-_p0x)+dir[1]*(pnt[1]-_p0y)+dir[2]*(pnt[2]-_p0z))
              /(dir[0]*_dxdz        +dir[1]*_dydz        +dir[2]);
    AMSPoint pnt0 = pnt;
    double dnorm = std::sqrt(_dxdz*_dxdz+_dydz*_dydz+1);
    pnt.setp(_dxdz*z+_p0x, _dydz*z+_p0y, _p0z+z);
    dir.setp(_dxdz/dnorm,  _dydz/dnorm, 1/dnorm);
    return pnt.dist(AMSPoint(_p0x, _p0y, _p0z));
  }

  // Curved track case
  AMSPoint ptrk(_p0x, _p0y, _p0z);
  double sign = (pnt.z() < _p0z) ? 1 : -1;
  double d0   = -sign/std::sqrt(_dxdz*_dxdz+_dydz*_dydz+1);
  double init[7] 
    = { _p0x, _p0y, _p0z, _dxdz*d0, _dydz*d0, d0, sign*_chrg*_rigidity };
  double point[6]
    = { pnt.x(), pnt.y(), pnt.z(), dir.x(), dir.y(), dir.z() };
  double out[7] = { 0, 0, 0, 0, 0, 0, 0 };

  double len = VCFitPar(init, out, point, 0, 1);
  pnt.setp(out[0], out[1], out[2]);
  dir.setp(out[3], out[4], out[5]);
  if (dir.z() < 0) dir = dir*(-1);
  return len;
}

double TrProp::InterpolateCyl(AMSPoint &pnt, AMSDir &dir, 
			      double radius, int idir)
{
  // Check on dir
  if (dir[0] == 0 && dir[1] == 0 && dir[2] == 0) return -1;

  // Linear track case
  if ( _chrg*_rigidity == 0) {
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

void TrProp::InterpolateSimple(int nz, AMSPoint *pnt, AMSDir *dir)
{
  enum { LMAX = 100 };

  if (_rigidity == 0) return;
  if (nz+1 > LMAX) return;

  double dd = std::sqrt(_dxdz*_dxdz+_dydz*_dydz+1);

  double param[NDIM];
  param[0] =  _p0x;
  param[1] =  _p0y;
  param[2] = -_dxdz/dd;
  param[3] = -_dydz/dd;
  param[4] = Clight*1e-12/_rigidity;

  AMSPoint pnt0(_p0x, _p0y, _p0z);

  // Length
  double len[LMAX];
  len[0] = 0;
  for (int i = 0; i < nz; i++)
    len[i+1] = (i > 0) ? (pnt[i]-pnt[i-1]).norm() : (pnt[i]-pnt0).norm();

  double bx[LMAX], by[LMAX], bz[LMAX];
  double xh[LMAX], yh[LMAX], zh[LMAX];

  double bf[3];
  GuFld(xh[0] = _p0x, yh[0] = _p0y, zh[0] = _p0z, bf);
  bx[0] = bf[0]; by[0] = bf[1]; bz[0] = bf[2];

  for (int i = 0; i < nz; i++) {
    GuFld(xh[i+1] = pnt[i].x(), 
	  yh[i+1] = pnt[i].y(), 
	  zh[i+1] = pnt[i].z(), bf);
    bx[i+1] = bf[0]; by[i+1] = bf[1]; bz[i+1] = bf[2];
  }

  double pintx[LMAX][3];
  double pintu[LMAX][3];

  // Calculate path integrals
  for (int i = 0; i < nz+1; i++) {
    if (i == 0) {
      for (int j = 0; j < 3; j++) pintx[i][j] = pintu[i][j] = 0;
      continue;
    }

    double u[3];
    u[0] = (xh[i]-xh[i-1])/len[i];
    u[1] = (yh[i]-yh[i-1])/len[i];
    u[2] = (zh[i]-zh[i-1])/len[i];

    double bax = (bx[i-1]+bx[i])/2;
    double bay = (by[i-1]+by[i])/2;
    double baz = (bz[i-1]+bz[i])/2;

    pintx[i][0] = (u[1]*bz[i-1]-u[2]*by[i-1])/2;
    pintx[i][1] = (u[2]*bx[i-1]-u[0]*bz[i-1])/2;
    pintx[i][2] = (u[0]*by[i-1]-u[1]*bx[i-1])/2;

    pintu[i][0] = u[1]*baz-u[2]*bay;
    pintu[i][1] = u[2]*bax-u[0]*baz;
    pintu[i][2] = u[0]*bay-u[1]*bax;
  }

  // F and G matrices
  double d[2*LMAX][NDIM];
  for (int i = 0; i < nz+1; i++) {
    int ix = i, iy = i+nz;

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

  for (int i = 0; i < nz+1; i++) {
    double x = 0, y = 0;

    for (int k = 0; k < NDIM; k++) {
      int ix = i, iy = i+nz;
      x += d[ix][k]*param[k];
      y += d[iy][k]*param[k];
    }

    if (i > 0) pnt[i-1].setp(x, y, zh[i]);
  }
}

double TrProp::VCFitPar(double *init, double *out, double *point, 
			double m55[][NDIM], int clear)
{
  double steps   = 0.03;
  double smax    = 20;
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
  double vin[7];


  for (int i = 0; i < 7; i++) vin[i] = out[i] = init[i];

 
  double za = out[2];
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

    double bb[3];
    GuFld(out[0], out[1], out[2], bb);
    cfld    = 3333.*pii/180./(bb[0]+1.e-10)*tmaxf;
    sfield  = fabs(cfld*init[6]/_chrg);

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
  if (imat == 0) return -1;

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
#pragma omp threadprivate(der)
  if (clear == 1) {
    for (int i = 0; i < 10; i++) der[0][i] = der[1][i];
  }

  double xx[3], h[3], hxy[3][3], dx, dy;

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
    GuFld(xx, h);
    TkFld(xx, hxy);

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

void TrProp::Propagate(double *x, double *d, double *u, int ndiv)
{
  double par = (_rigidity != 0) ? 1e-12*Clight/_rigidity : 0;
  double mem[6] = { 1, 1, 0, 0, 0, 0 }, mel[4];
  JAStepPin(x, d, u, mel, mem, par, ndiv);
}

TkDBc *TrProp::TkDBc()
{
  if (TkDBc::Head) return TkDBc::Head;
#pragma omp critical (tkdbccheck)
 {
  if (!TkDBc::Head) {
    TkDBc::CreateTkDBc();
    TkDBc::Head->init(3);
  }
 }
  return TkDBc::Head;
}

void TrProp::GuFld(double *p, double *b)
{
  b[0] = b[1] = b[2] = 0;

  if (!MagFieldOn()) return;

  MagField *mfp = MagField::GetPtr();
  if (!mfp->GetMap()) {
    int err = 0;
#pragma omp critical (magchk)
    {
      static int magerr = 0;
      if (!mfp->GetMap() && !magerr) {
	char name[200];
	sprintf(name, "%s/v5.00/MagneticFieldMapPermanent_NEW.bin",
		getenv("AMSDataDir"));
	if ((mfp->Read(name)) < 0) {
	  std::cerr << "Magnetic Field map not found: " << name << std::endl;
	  magerr = -1;
	  err = 1;
	}
	mfp->SetMagstat(1);
	mfp->SetScale(1.);
      }
    }
    if (err) return;
  }

  float pp[3] = { p[0], p[1], p[2] };
  float bb[3];
  GUFLD(pp, bb);

  b[0] = bb[0];
  b[1] = bb[1];
  b[2] = bb[2];
}

void TrProp::GuFld(double x, double y, double z, double *b)
{
  double pp[3] = { x, y, z };
  GuFld(pp, b);
}

void TrProp::TkFld(double *p, double hxy[][3])
{
  float pp[3] = { p[0], p[1], p[2] };
  float hh[2][3];
  TKFLD(pp, hh);

  hxy[0][0] = hh[0][0];
  hxy[0][1] = hh[0][1];
  hxy[0][2] = hh[0][2];
  hxy[1][0] = hh[1][0];
  hxy[1][1] = hh[1][1];
  hxy[1][2] = hh[1][2];
}

int TrProp::JAStepPin(double *x, double *l, double *u,
                      double *mel, double *mem, double par, int ndiv)
{
   for (int i = 0; i < 4; i++) mel[i] = 0;

   double dl = std::sqrt(l[0]*l[0]+l[1]*l[1]+l[2]*l[2]);
   if (dl == 0) return -1;

   double pint[4];
   for (int i = 0; i < 4; i++) pint[i] = 0;

   if (l[2] > 0) dl = -dl;

   if (ndiv < 4) ndiv = 4;
   for (int j = 0; j <= ndiv; j++) {
      double e = (double) j/ndiv, w = 2;
      if (j == 0 || j == ndiv) w = 1;
      else if (j%2 == 1) w = 4;

      double b[3];
      GuFld(x[0]+e*l[0], x[1]+e*l[1], x[2]+e*l[2], b);

      pint[0] += w*(l[1]*b[2]-l[2]*b[1])/dl*(1-e);
      pint[1] += w*(l[1]*b[2]-l[2]*b[1])/dl;
      pint[2] += w*(l[2]*b[0]-l[0]*b[2])/dl*(1-e);
      pint[3] += w*(l[2]*b[0]-l[0]*b[2])/dl;
   }
   for (int i = 0; i < 4; i++) pint[i] /= 3*ndiv;

   u[0] += par*pint[1]*dl;
   u[1] += par*pint[3]*dl;
   double uu = u[0]*u[0]+u[1]*u[1];
   u[2] = (uu < 1) ? -std::sqrt(1-uu) : -1e-9;

   x[0] += l[0];
   x[1] += l[1];
   x[2] += l[2];

   mel[0] = dl; mel[2] = dl*dl*pint[0]+dl*mem[2]; mem[2] += dl*pint[1];
   mel[1] = dl; mel[3] = dl*dl*pint[2]+dl*mem[3]; mem[3] += dl*pint[3];

   return 0;
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
  double F[4], X, Y, Z, XYZT[3];
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
    GuFld(vout[0], vout[1], vout[2], F);
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
      GuFld(XYZT, F);


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
	GuFld(XYZT, F);

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

