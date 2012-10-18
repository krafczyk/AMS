//  $Id: GeoMagTrace.C,v 1.2 2012/10/18 18:13:53 shaino Exp $

#include "GeoMagTrace.h"
#include "GeoMagField.h"
#include "GeoMagCoord.h"
#include "FrameTrans.h"
#include "TMath.h"

#include <iostream>

int GeoMagTrace::UTime = GeoMagTrace::T2012;
int GeoMagTrace::DEBUG = 0;

double GeoMagTrace::Re = 6378.137;              // Earth equator radius in km
double GeoMagTrace::Rp = 6356.7523142;          // Earth polar   radius in km
double GeoMagTrace::Ha = 100;                   // Atmospheric boundary in km
double GeoMagTrace::Rs = GeoMagTrace::Re*20;    // Escape limit         in km

double GeoMagTrace::Clight = 2.99792458e+08;    // Speed of light in m/s


GeoMagTrace::GeoMagTrace(double pos[3], double vel[2], double ypr[3],
			 double theta,  double phi,    double rigidity,
			 double charge, double beta,   int stat)
/*
 * pos[3] = position of ISS in r(cm), longitude(rad), latitude(rad)
 * vel[2] = velocity of ISS in phi, theta (rad)
 * ypr[3] = attitude of ISS in yaw, pitch, roll (rad)
 * theta  = track theta
 * phi    = track phi
 */
{
  double pp[3] = { pos[0]*1e-5, pos[1], pos[2] };
  Init(pp, vel, ypr, theta, phi, rigidity, charge, beta, stat, false);
}

GeoMagTrace::GeoMagTrace(double lng,    double lat,  double alt,
			 double theta,  double phi,  double rigidity,
			 double charge, double beta, int stat)
{
  double pos[3] = { alt+Re, lng, lat };
  Init(pos, 0, 0, theta, phi, rigidity, charge, beta, stat, true);
}

void GeoMagTrace::Init(double pos[3], double vel[2], double ypr[3],
		       double theta,  double phi,    double rigidity,
		       double charge, double beta, int stat, bool deg)
{
  AngToCart(pos[0], pos[2], pos[1], _x, _y, _z, deg);

  if (vel && ypr) {
    //ISSAtt::GetGTOD(theta, phi, pos, vel, ypr, _dx, _dy, _dz);

    // theta, phi are defined as of ParticleR::Theta and Phi
    AMSDir dams(theta, phi);
    double x = -dams.x(), y = -dams.y(), z = -dams.z();
    FT_AMS2Body (x, y, z);
    FT_Body2LVLH(x, y, z, ypr[0], ypr[1], ypr[2]);
    FT_LVLH2GTOD(x, y, z, pos[0], pos[1], pos[2] , vel[0], vel[1]);
    _dx = x; _dy = y; _dz = z;
  }

  else {
    SetDir(pos[2], pos[1], deg);
    AMSDir vz = AMSDir(_dx, _dy, _dz);
    AMSDir vy = vz.cross(AMSDir(0, 0, 1));
    AMSDir vx = vy.cross(vz);
    AMSDir vt = AMSDir(theta, phi);
    AMSDir vd = vx*vt.x()+vy*vt.y()+vz*vt.z();
    _dx = vd.x();
    _dy = vd.y();
    _dz = vd.z();
  }

  _tof      = 0;
  _beta     = beta;
  _charge   = charge;
  _rigidity = rigidity;
  _stat     = (1 <= stat && stat <= Nstat) ? stat : TRACE;
}

double GeoMagTrace::StepCv  = 0.1;
double GeoMagTrace::MinStep = 0.1*GeoMagTrace::Re;
double GeoMagTrace::MaxStep = 5.0*GeoMagTrace::Re;

int GeoMagTrace::Propagate(double step)
{
  if (!IsValid()) return _stat;

  double sign = (_stat == FRWRD) ? 1 : -1;
  double vin [7] = { _x, _y, _z, _dx, _dy, _dz, _charge*_rigidity*sign };
  double vout[7] = { 0, 0, 0, 1, 0, 0, 0 };

  if (step == 0) step = GetStep();

  int ret = Rkuta(_charge, step, vin, vout);
   _x = vout[0];  _y = vout[1];  _z = vout[2];
  _dx = vout[3]; _dy = vout[4]; _dz = vout[5];

  double beta = TMath::Abs(_beta);
  if (beta < 0.1) beta = 0.1;
  _tof += step*1e3/Clight/beta;

  if      (ret == 1)                    _stat = TRAPP;
  else if (GeoBoundary(_x, _y, _z, Ha)) _stat = ATMOS;
  else if (GetRadi() > Rs)              _stat = SPACE;
  
  return _stat;
}

int GeoMagTrace::Propagate(int nmax, int verb, double step)
{
  if (verb == -1) verb = (DEBUG > 0) ? DEBUG : -1;

  for (int i = 0; i < nmax && IsValid(); i++) {
    if (verb >= 0 && (i < 20 || i%10 == 0)) Print(verb);
    Propagate(step);
  }
  if (verb >= 0) Print(verb);

  return _stat;
}

void GeoMagTrace::Print(int option) const
{
  char ss[Nstat+2] = "TFAPSU";
  int is = (1 <= _stat && _stat <= Nstat) ? _stat-1 : Nstat;

  double r, th, ph, dr, dth, dph;
  CartToAng( _x,  _y,  _z,  r,  th,  ph, true);
  CartToAng(_dx, _dy, _dz, dr, dth, dph, true);

  static int first = 1;
  if (first) {
    first = 0;
    std::cout << "St";

    if ((option/ 100)%10 > 0) std::cout << "( x/Re  y/Re  z/Re)";
    else                      std::cout << "(Rad/Re  Long   Lat)";
    if ((option/ 100)%10 > 0) std::cout << " (   dx    dy    dz)";
    else                      std::cout << " ( dLong  dLat)";
    if ( option      %10 > 0) std::cout << " (BN/uT BE/uT BD/uT)";
    if ((option/  10)%10 > 0) std::cout << " Cv/Re Angle Step";
    if ((option/1000)%10 > 0) std::cout << "  ToF";
    std::cout << std::endl;
  }

  std::cout << ss[_stat-1];

  if ((option/100)%10 > 0) {
    double x = _x/Re, y = _y/Re, z = _z/Re;
    std::cout << " (";
    std::cout << Form(((x <= -10) ? "%5.1f" : "%5.2f"), x) << " ";
    std::cout << Form(((y <= -10) ? "%5.1f" : "%5.2f"), y) << " ";
    std::cout << Form(((z <= -10) ? "%5.1f" : "%5.2f"), z) << ")";
    std::cout << Form(" (%5.2f %5.2f %5.2f)", _dx, _dy, _dz);
  }
  else {
    std::cout << Form(" (%5.2f %6.1f %5.1f)", r/Re, ph, th);
    std::cout << Form(" (%6.1f %5.1f)", dph, dth);
  }
  if (option%10 > 0) {
    double b[3];
    GuFld(_x, _y, _z, b, false);
    std::cout << " (";
    std::cout << Form(((b[0] <= -10) ? "%5.1f" : "%5.2f"), b[0]) << " ";
    std::cout << Form(((b[1] <= -10) ? "%5.1f" : "%5.2f"), b[1]) << " ";
    std::cout << Form(((b[2] <= -10) ? "%5.1f" : "%5.2f"), b[2]) << ")";
  }
  if ((option/10)%10 > 0) {
    double cv = GetCurvature()/Re;
    std::cout << "  ";
    if      (cv <  10) std::cout << Form("%4.2f", cv);
    else if (cv < 100) std::cout << Form("%4.1f", cv);
    else if (cv < 1e4) std::cout << Form("%4.0f", cv);
    else {
      TString str = Form( "%5.0E", cv);
      str.ReplaceAll("E+", "E");
      std::cout << str.Data();
    }
    std::cout << Form(" %5.1f %4.1f", GetAngle(true), GetStep()/Re);
  }
  if ((option/1000)%10 > 0) {
    std::cout << " ";
    if      (_tof <  0.1) std::cout << Form("%4.1f ms",  _tof*1e3);
    else if (_tof <    1) std::cout << Form("%3.0f. ms", _tof*1e3);
    else if (_tof <  100) std::cout << Form("%4.1f s",   _tof);
    else                  std::cout << Form("%4.0f s",   _tof);
  }

  std::cout << std::endl;
}

const char *GeoMagTrace::GetStatString() const
{
  static TString str;
  if (_stat == TRACE) str = "Backward tracing";
  if (_stat == FRWRD) str = "Forward  tracing";
  if (_stat == ATMOS) str = "Hit in the atmosphere";
  if (_stat == TRAPP) str = "Trapped in field";
  if (_stat == SPACE) str = "Escaped in space";

  return str.Data();
}

double GeoMagTrace::GetStep() const
{
  double cv   = GetCurvature();
  double step = (cv > 0) ? cv*StepCv : MaxStep;
  if (step > MaxStep) step = MaxStep;
  if (step < MinStep) step = MinStep;

  return step;
}

double GeoMagTrace::GetCurvature() const
{
  double b[3];
  GuFld(_x, _y, _z, b);

  double ba = TMath::Sqrt(b[0]*b[0]+b[1]*b[1]+b[2]*b[2]);
  if (ba == 0) return 0;

  double ct = (_dx*b[0]+_dy*b[1]+_dz*b[2])/ba;
  double bt = ba*TMath::Sqrt(1-ct*ct);
  double ec = 2.9979251e-4;
  return (bt > 0) ? TMath::Abs(_rigidity)/ec/bt : 0;
}

double GeoMagTrace::GetAngle(bool deg) const
{
  double b[3];
  GuFld(_x, _y, _z, b);

  double ba = TMath::Sqrt(b[0]*b[0]+b[1]*b[1]+b[2]*b[2]);
  if (ba == 0) return 0;

  double ct = (_dx*b[0]+_dy*b[1]+_dz*b[2])/ba;
  double th = TMath::ACos(ct);

  if (deg) th *= TMath::RadToDeg();

  return th;
}

void GeoMagTrace::SetDir(double dx, double dy, double dz)
{
  _dx = dx; _dy = dy; _dz = dz;

  double s = TMath::Sqrt(_dx*_dx+_dy*_dy+_dz*_dz);

  if (s > 0) { _dx /= s; _dy /= s; _dz /= s; }
  else       { _dx  = 1; _dy  = 0; _dz  = 0; }
}

void GeoMagTrace::SetDir(double th, double ph, bool deg)
{
  if (deg) {
    ph *= TMath::DegToRad();
    th *= TMath::DegToRad();
  }
  _dx = TMath::Sin(TMath::Pi()/2-th)*TMath::Cos(ph);
  _dy = TMath::Sin(TMath::Pi()/2-th)*TMath::Sin(ph);
  _dz = TMath::Cos(TMath::Pi()/2-th);
}

double GeoMagTrace::GetLong(bool deg) const
{
  double r = 1, th = 0, ph = 0;
  CartToAng(_x, _y, _z, r, th, ph, deg);
  return ph;
}

double GeoMagTrace::GetLati(bool deg) const
{
  double r = 1, th = 0, ph = 0;
  CartToAng(_x, _y, _z, r, th, ph, deg);
  return th;
}

double GeoMagTrace::GetLongM(bool deg) const
{
  double r = 1, th = 0, ph = 0;
  CartToAng(_x, _y, _z, r, th, ph, deg);

  double thm = 0, phm = 0;
  GeoMagCoord::GeogToMag(r, th, ph, thm, phm, deg);
  return phm;
}

double GeoMagTrace::GetLatiM(bool deg) const
{
  double r = 1, th = 0, ph = 0;
  CartToAng(_x, _y, _z, r, th, ph, deg);

  double thm = 0, phm = 0;
  GeoMagCoord::GeogToMag(r, th, ph, thm, phm, deg);
  return thm;
}

double GeoMagTrace::GetDlong(bool deg) const
{
  double r = 1, th = 0, ph = 0;
  CartToAng(_dx, _dy, _dz, r, th, ph, deg);
  return ph;
}

double GeoMagTrace::GetDlati(bool deg) const
{
  double r = 1, th = 0, ph = 0;
  CartToAng(_dx, _dy, _dz, r, th, ph, deg);
  return th;
}

double GeoMagTrace::GetRadi(void) const
{
  return TMath::Sqrt(_x*_x+_y*_y+_z*_z);
}

AMSDir GeoMagTrace::GetVloc(void) const
{
  double r = 1, th = 0, ph = 0;
  CartToAng(_x, _y, _z, r, th, ph, false);

  double x = _dx, y = _dy, z = _dz;
  GeoToLoc(x, y, z, th, ph, false);

  return AMSDir(x, y, z);
}

void GeoMagTrace::CartToAng(double  x, double   y, double   z,
			    double &r, double &th, double &ph, bool deg)
{
  r  = TMath::Sqrt (x*x+y*y+z*z);
  ph = TMath::ATan2(y, x);
  th = TMath::Pi()/2-TMath::ACos(z/r);

  if (deg) {
    ph *= TMath::RadToDeg();
    th *= TMath::RadToDeg();
  }
}
 
void GeoMagTrace::AngToCart(double  r, double  th, double  ph,
			    double &x, double  &y, double  &z, bool deg)
{
  if (deg) {
    ph *= TMath::DegToRad();
    th *= TMath::DegToRad();
  }
  x = r*TMath::Sin(TMath::Pi()/2-th)*TMath::Cos(ph);
  y = r*TMath::Sin(TMath::Pi()/2-th)*TMath::Sin(ph);
  z = r*TMath::Cos(TMath::Pi()/2-th);
}

void GeoMagTrace::LocToGeo(double  &x, double  &y, double &z,
			   double lat, double lng, bool deg)
{
  double mtx[9];
  GeoMatrix(mtx, lat, lng, deg);

  double x0 = x, y0 = y, z0 = z;

  x = mtx[0]*x0 +mtx[3]*y0 +mtx[6]*z0;
  y = mtx[1]*x0 +mtx[4]*y0 +mtx[7]*z0;
  z = mtx[2]*x0 +mtx[5]*y0 +mtx[8]*z0;
}

void GeoMagTrace::GeoToLoc(double  &x, double  &y, double &z,
			   double lat, double lng, bool deg)
{
  double mtx[9];
  GeoMatrix(mtx, lat, lng, deg);

  double x0 = x, y0 = y, z0 = z;

  x = mtx[0]*x0 +mtx[1]*y0 +mtx[2]*z0;
  y = mtx[3]*x0 +mtx[4]*y0 +mtx[5]*z0;
  z = mtx[6]*x0 +mtx[7]*y0 +mtx[8]*z0;
}

void GeoMagTrace::GeoMatrix(double *m, double lat, double lng, bool deg)
{
  double th = (deg) ? (90-lat)*TMath::DegToRad() : TMath::Pi()/2-lat;
  double ph = (deg) ?     lng *TMath::DegToRad() :               lng;
  double st = TMath::Sin(th), ct = TMath::Cos(th);
  double sp = TMath::Sin(ph), cp = TMath::Cos(ph);

  *(m++) = -ct*cp; *(m++) = -ct*sp; *(m++) =  st;
  *(m++) =    -sp; *(m++) =     cp; *(m++) =   0;
  *(m++) = -st*cp; *(m++) = -st*sp; *(m++) = -ct;
}

bool GeoMagTrace::GeoBoundary(double x, double y, double z, double h)
{
  double a  = Re+h;
  double b  = Rp+h;
  double r2 = x*x+y*y;
  double z2 = z*z;
  return (z2 < b*b*(1-r2/a/a));
}

double GeoMagTrace::GetThetaG(int time)
{
/* The function ThetaG calculates the Greenwich Mean Sidereal Time *
 * for an epoch specified in the format used in the NORAD two-line *
 * element sets. It has now been adapted for dates beyond the year *
 * 1999, as described above. The function ThetaG_JD provides the   *
 * same calculation except that it is based on an input in the     *
 * form of a Julian Date.                                          *
 *                                                                 *
 * Reference:  The 1992 Astronomical Almanac, page B6.             */

  double jd     = (time/86400.0-3651.0)+2444238.5;
  double UT     = (jd+0.5)-TMath::Floor(jd+0.5); jd = jd-UT;
  double TU     = (jd-2451545.0)/36525;
  double GMST   = 24110.54841+TU*(8640184.812866+TU*(0.093104-TU*6.2E-6));
  double omegaE = 1.00273790934; // Earth rotations/siderial day
  double secday	= 8.6400E4;      // Seconds per day

  GMST += secday*omegaE*UT;
  while (GMST > secday) GMST -= secday;

  return TMath::TwoPi()*GMST/secday;
}

void GeoMagTrace::GuFld(double *p, double *b, bool global)
{
  b[0] = b[1] = b[2] = 0;

  double r = 1, th = 0, ph = 0;
  CartToAng(p[0], p[1], p[2], r, th, ph, true);

  GeoMagField *mag = GeoMagField::GetHead();
  mag->Calc(th, ph, r-Re);

  double bu = 1e-3;  // nT(1e-9)/kG(1e-1) * km(1e5)/cm

  double bx = mag->GetX()*bu;
  double by = mag->GetY()*bu;
  double bz = mag->GetZ()*bu;
  if (global) LocToGeo(bx, by, bz, th, ph, true);

  b[0] = bx; b[1] = by; b[2] = bz;
}

void GeoMagTrace::GuFld(double x, double y, double z, double *b, bool global)
{
  double pp[3] = { x, y, z };
  GuFld(pp, b, global);
}

int GeoMagTrace::Rkuta(double charge, double step, double *vect, double *vout)
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

	  return 0;
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

  return 1;
}
