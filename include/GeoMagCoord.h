//  $Id: GeoMagCoord.h,v 1.1 2012/10/16 19:37:49 shaino Exp $
#ifndef __GeoMagCoord__
#define __GeoMagCoord__

#include "point.h"

class GeoMagCoord {

public:
  static double DSr;      ///< Dipole Shift Distance  (km)
  static double DSth;     ///<              Latitude  (rad)
  static double DSph;     ///<              Longitude (rad)
  static double PDth;     ///< Dipole Direction Lat.  (rad)
  static double PDph;     ///<                  Lon.  (rad) 

  static AMSDir GMx;      ///< Geomagnetic X-axis in GTOD coord.
  static AMSDir GMy;      ///< Geomagnetic Y-axis in GTOD coord.
  static AMSDir GMz;      ///< Geomagnetic Z-axis in GTOD coord.

  static void InitAxes();

  static void GeogToMag(double  r, double  th, double  ph,
			           double &tm, double &pm, bool deg = false);
  static void MagToGeog(double  r, double  tm, double  pm,
			           double &th, double &ph, bool deg = false);

  virtual ~GeoMagCoord() { }
  ClassDef(GeoMagCoord, 1)
};

#endif
