//  $Id: GeoMagCoord.C,v 1.2 2012/11/06 21:55:37 shaino Exp $

#include "GeoMagCoord.h"
#include "GeoMagTrace.h"
#include "TMath.h"

static const double DtR  = 0.0174532925199432955;

double GeoMagCoord::DSr  = 567.946;         // Dipole Shift Distance  (km)
double GeoMagCoord::DSth =  22.596*DtR;     //              Latitude  (rad)
double GeoMagCoord::DSph = 151.365*DtR;     //              Longitude (rad)
double GeoMagCoord::PDth = -80.130*DtR;     // Dipole Direction Lat.  (rad)
double GeoMagCoord::PDph = 107.622*DtR;     //                  Lon.  (rad) 

AMSDir GeoMagCoord::GMx;              // Geomagnetic X-axis in GTOD coord.
AMSDir GeoMagCoord::GMy;              // Geomagnetic Y-axis in GTOD coord.
AMSDir GeoMagCoord::GMz;              // Geomagnetic Z-axis in GTOD coord.


#include <iostream>

void GeoMagCoord::InitAxes()
{
#pragma omp critical (geomagaxis)
 {
  if (GMz.norm() < 1) {
    // Dipole direction in GTOD coord.
    double dx, dy, dz;
    GeoMagTrace::AngToCart(1, PDth, PDph, dx, dy, dz);

    // Three vectors for geomagnetic axes
    GMz = AMSDir(-dx, -dy, -dz);
    GMy = GMz.cross(AMSDir(0, 0, -1));
    GMx = GMy.cross(GMz);

    std::cout << "GeoMagCoord::InitAxes" << std::endl;
  }
 }
}

void GeoMagCoord::GeogToMag(double   r, double  th, double  ph,
			    double &tm, double &pm, bool deg)
{
  if (GMz.norm() < 1) InitAxes();
  
  double sx, sy, sz;
  GeoMagTrace::AngToCart(1, th, ph, sx, sy, sz, deg);

  // Dipole shift in GTOD coord.
  double px, py, pz;
  GeoMagTrace::AngToCart(DSr, DSth, DSph, px, py, pz);
  
  // Station GTOD coordinates referred to dipole center
  AMSDir dsp(sx*r-px, sy*r-py, sz*r-pz);

  double rm;
  GeoMagTrace::CartToAng(GMx.prod(dsp), 
			 GMy.prod(dsp),
			 GMz.prod(dsp), rm, tm, pm, deg);
}

void GeoMagCoord::MagToGeog(double rs,  double  tm, double  pm,
			    double &th, double &ph, bool deg)
{
  if (GMz.norm() < 1) InitAxes();

  double mx, my, mz;
  GeoMagTrace::AngToCart(1, tm, pm, mx, my, mz, deg);

  // Station GTOD coordinates referred to dipole center
  AMSDir dpc = GMx*mx+GMy*my+GMz*mz;

  // Dipole shift in GTOD coord.
  double px, py, pz;
  GeoMagTrace::AngToCart(DSr, DSth, DSph, px, py, pz);

  double c = px*px+py*py+pz*pz-rs*rs;
  double b = dpc.x()*px+dpc.y()*py+dpc.z()*pz;
  double a = dpc.prod(dpc);
  double r = (-b+TMath::Sqrt(b*b-a*c))/a;

  AMSPoint ps = AMSPoint(px, py, pz)+dpc*r;
  GeoMagTrace::CartToAng(ps.x(), ps.y(), ps.z(), rs, th, ph, deg);
}
