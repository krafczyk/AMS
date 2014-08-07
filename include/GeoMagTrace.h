//  $Id$
#ifndef __GeoMagTrace__
#define __GeoMagTrace__

#include "point.h"

class GeoMagTrace {

public:
  enum EUTimes { T2011 = 1293840000,  // Sat Jan  1 00:00:00 2011
		 T2012 = 1325376000,  // Sun Jan  1 00:00:00 2012
		 T2013 = 1356998400,  // Tue Jan  1 00:00:00 2013
		 YrSec = 365*24*3600 };

  static int UTime;       ///< Current unix time

  static double Re;       ///< Equatorial axis (a) of earth ellipsoid
  static double Rp;       ///< Polar      axis (b) of earth ellipsoid
  static double Ha;       ///< Atmospheric boundary altitude
  static double Rs;       ///< Propagation limit for space

  static double StepCv;   ///< Step size per curvature
  static double MinStep;  ///< Minimum step size
  static double MaxStep;  ///< Maximum step size
  static double Clight;   ///< Speed of light in m/s
  static int    NmaxStep; ///< Maximum number of steps
  static int    DEBUG;    ///< Debug switch

  /// Constructor with ISS pos,vel,ypr and track dir,rigidity
  /*
   * pos[3] = position of ISS in r(cm), longitude(rad), latitude(rad)
   * vel[2] = velocity of ISS in phi, theta (rad)
   * ypr[3] = attitude of ISS in yaw, pitch, roll (rad)
   * xtime  = UTC time in sec
   * theta  = track theta
   * phi    = track phi
   */
  GeoMagTrace(double pos[3], double vel[3], double ypr[3], double xtime,
	      double theta,  double phi,    double rigidity,
	                                    double charge = 1,
	                                    double beta   = 1,
	                                    int    stat   = 0);

  /// Constructor with ISS pos(GTOD), ypr(INTL) and track dir,rigidity
  /*
   * pos[3] = position of ISS in r(cm), longitude(rad), latitude(rad)
   * ypr[3] = attitude of ISS in yaw, pitch, roll(rad) in J2000 frame
   * xtime  = UTC time in sec
   * theta  = track theta
   * phi    = track phi
   */
  GeoMagTrace(double pos[3], double ypr[3], double xtime, 
	      double theta,  double phi,    double rigidity,
	                                    double charge = 1,
	                                    double beta   = 1,
	                                    int    stat   = 0);

  /// Constructor with ISS lng(deg), lat(deg), alt(km) and track direction
  GeoMagTrace(double lng,   double lat, double alt,
	      double theta, double phi, double rigidity,
	                                double charge = 1,
	                                double beta   = 1,
	                                int stat = 0);

 virtual ~GeoMagTrace() {}

  /// Initialize with GTOD pos,vel and att.
  void Init(double pos[3], double vel[3], double ypr[3], double xtime, 
	    double theta,  double phi,    double rigidity,
	    double charge = 1, double beta = 1,
	    int stat = 0, bool deg = false);

  /// Initialize with GTOD pos and INTL att.
  void Init(double pos[3], double ypr[3], double xtime,
	    double theta,  double phi,    double rigidity,
	    double charge = 1, double beta = 1,
	    int stat = 0, bool deg = false);

  double GetX () const { return _x;  }
  double GetY () const { return _y;  }
  double GetZ () const { return _z;  }
  double GetDx() const { return _dx; }
  double GetDy() const { return _dy; }
  double GetDz() const { return _dz; }

  AMSPoint GetPos() const { return AMSPoint(_x, _y, _z);  }
  AMSDir   GetDir() const { return AMSDir(_dx, _dy, _dz); }

  void SetPos(double  x, double  y, double  z) { _x = x; _y = y; _z = z; }
  void SetDir(double dx, double dy, double dz);
  void SetDir(double th, double ph, bool deg = false);

  void SetBeta  (double   beta) { _beta   = beta;   }
  void SetCharge(double charge) { _charge = charge; }

  double GetTof()      const { return _tof; }
  double GetBeta()     const { return _beta; }
  double GetCharge()   const { return _charge; }
  double GetRigidity() const { return _rigidity; }

  double GetLong (bool deg = true) const;
  double GetLati (bool deg = true) const;
  double GetLongM(bool deg = true) const;
  double GetLatiM(bool deg = true) const;
  double GetDlong(bool deg = true) const;
  double GetDlati(bool deg = true) const;
  double GetRadi() const;
  double GetAlt () const { return GetRadi()-Re; }
  AMSDir GetVloc() const;

  enum EStat { TRACE = 1,  // Backward tracing
	       FRWRD = 2,  // Forward  tracing
	       ATMOS = 3,  // Hit in the atmosphere
	       TRAPP = 4,  // Trapped in field
	       SPACE = 5,  // Escaped in space
	       Nstat = 5
             };

  /// Get status
  int GetStat() const { return _stat; }

  /// Check if the status is valid 
  bool IsValid() const { return (_stat == TRACE || _stat == FRWRD); }

  /// Propagete one step returns the new status
  int Propagate(double step = 0);

  /// Propagete until the status changes
  int Propagate(int nmax, int verb = -1, double step = 0);

  /// Print out data
  void Print(int option = 11) const;

  /// Get status string
  const char *GetStatString() const;

public:
  /// Get optimized step size
  double GetStep() const;

  /// Get curvature with current field and direction
  double GetCurvature() const;

  /// Get angle between direction and field
  double GetAngle(bool deg = false) const;

protected:
  int _stat;             ///< Status defined with enum EStat

  // Track parameters
  double _x;             ///< Track point X (km)
  double _y;             ///< Track point Y (km)
  double _z;             ///< Track point Z (km)
  double _dx;            ///< Track direction dX
  double _dy;            ///< Track direction dY
  double _dz;            ///< Track direction dZ
  double _tof;           ///< Time of flight (s)
  double _beta;          ///< Velocity /c
  double _charge;        ///< Partile signed charge
  double _rigidity;      ///< Rigidity (GV)

  /// Get magnetic field
  static void GuFld(double *p, double *b, bool global = true);

  /// Get magnetic field
  static void GuFld(double x, double y, double z, double *b,
		                                  bool global = true);

  /// Runge-Kutta transportation returns 1 if helix
  static int Rkuta(double, double, double*, double*);

public:
  static void CartToAng(double  x, double   y, double   z,
		        double &r, double &th, double &ph, bool deg = false);
  static void AngToCart(double  r, double  th, double  ph,
		        double &x, double  &y, double  &z, bool deg = false);
  static void LocToGeo (double &x, double  &y, double  &z,
		                   double lat, double lng, bool deg = false);
  static void GeoToLoc (double &x, double  &y, double  &z,
		                   double lat, double lng, bool deg = false);
  static void GeoMatrix(double *m, double lat, double lng, bool deg = false);

  static double GetThetaG(int time = UTime);

  /// Return true if inside the Reference ellipsoid (+ specific height)
  static bool GeoBoundary(double x, double y, double z, double h = 0);

  ClassDef(GeoMagTrace, 1)
};

#endif
