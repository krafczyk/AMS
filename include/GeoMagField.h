//  $Id$
#ifndef __GeoMagField__
#define __GeoMagField__

//////////////////////////////////////////////////////////////////////////
///
///\class GeoMagField
///\brief A class to manage geomagnetic field with IGRF and WMM
///
/// NGDC's Geomagnetic Field Modeling software for the IGRF and WMM
/// imported to C++ code by SH
///
///\date  2012/09/23 SH  Imported to C++
///
///$Date$
///
///$Revision$
///
//////////////////////////////////////////////////////////////////////////

/****************************************************************************/
/*                                                                          */
/*     NGDC's Geomagnetic Field Modeling software for the IGRF and WMM      */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*     Disclaimer: This program has undergone limited testing. It is        */
/*     being distributed unoffically. The National Geophysical Data         */
/*     Center does not guarantee it's correctness.                          */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*     Version 7.0:                                                         */
/*     - input file format changed to                                       */
/*            -- accept new DGRF2005 coeffs with 0.01 nT precision          */
/*            -- make sure all values are separated by blanks               */
/*            -- swapped n and m: first is degree, second is order          */
/*     - new my_isnan function improves portablility                        */
/*     - corrected feet to km conversion factor                             */
/*     - fixed date conversion errors for yyyy,mm,dd format                 */
/*     - fixed lon/lat conversion errors for deg,min,sec format             */
/*     - simplified leap year identification                                */
/*     - changed comment: units of ddot and idot are arc-min/yr             */
/*     - added note that this program computes the secular variation as     */
/*            the 1-year difference, rather than the instantaneous change,  */
/*            which can be slightly different                               */
/*     - clarified that height is above ellipsoid, not above mean sea level */
/*            although the difference is negligible for magnetics           */
/*     - changed main(argv,argc) to usual definition main(argc,argv)        */
/*     - corrected rounding of angles close to 60 minutes                   */
/*     Thanks to all who provided bug reports and suggested fixes           */
/*                                                                          */
/*                                          Stefan Maus Jan-25-2010         */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*     Version 6.1:                                                         */
/*     Included option to read coordinates from a file and output the       */
/*     results to a new file, repeating the input and adding columns        */
/*     for the output                                                       */
/*                                          Stefan Maus Jan-31-2008         */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*     Version 6.0:                                                         */
/*     Bug fixes for the interpolation between models. Also added warnings  */
/*     for declination at low H and corrected behaviour at geogr. poles.    */
/*     Placed print-out commands into separate routines to facilitate       */
/*     fine-tuning of the tables                                            */
/*                                          Stefan Maus Aug-24-2004         */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*      This program calculates the geomagnetic field values from           */
/*      a spherical harmonic model.  Inputs required by the user are:       */
/*      a spherical harmonic model data file, coordinate preference,        */
/*      altitude, date/range-step, latitude, and longitude.                 */
/*                                                                          */
/*         Spherical Harmonic                                               */
/*         Model Data File       :  Name of the data file containing the    */
/*                                  spherical harmonic coefficients of      */
/*                                  the chosen model.  The model and path   */
/*                                  must be less than PATH chars.           */
/*                                                                          */
/*         Coordinate Preference :  Geodetic (WGS84 latitude and altitude   */
/*                                  above ellipsoid (WGS84),                */
/*                                  or geocentric (spherical, altitude      */
/*                                  measured from the center of the Earth). */
/*                                                                          */
/*         Altitude              :  Altitude above ellipsoid (WGS84). The   */
/*                                  program asks for altitude above mean    */
/*                                  sea level, because the altitude above   */
/*                                  ellipsoid is not known to most users.   */
/*                                  The resulting error is very small and   */
/*                                  negligible for most practical purposes. */
/*                                  If geocentric coordinate preference is  */
/*                                  used, then the altitude must be in the  */
/*                                  range of 6370.20 km - 6971.20 km as     */
/*                                  measured from the center of the earth.  */
/*                                  Enter altitude in kilometers, meters,   */
/*                                  or feet                                 */
/*                                                                          */
/*         Date                  :  Date, in decimal years, for which to    */
/*                                  calculate the values of the magnetic    */
/*                                  field.  The date must be within the     */
/*                                  limits of the model chosen.             */
/*                                                                          */
/*         Latitude              :  Entered in decimal degrees in the       */
/*                                  form xxx.xxx.  Positive for northern    */
/*                                  hemisphere, negative for the southern   */
/*                                  hemisphere.                             */
/*                                                                          */
/*         Longitude             :  Entered in decimal degrees in the       */
/*                                  form xxx.xxx.  Positive for eastern     */
/*                                  hemisphere, negative for the western    */
/*                                  hemisphere.                             */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*      Subroutines called :  degrees_to_decimal,julday,getshc,interpsh,    */
/*                            extrapsh,shval3,dihf,safegets                 */
/*                                                                          */
/****************************************************************************/

#include "TObject.h"
#include "TString.h"


/// NGDC's Geomagnetic Field Modeling software for the IGRF and WMM
/*!
 *  Imported by SH from geomag70.c
 */
class GeoMagField : public TObject {

protected:
  GeoMagField();
  static GeoMagField *fHead;
#pragma omp threadprivate(fHead)

  /// Status of the InitModel 0:Not yet, 1:Done, -1:Error
  static int fInitStat;
#pragma omp threadprivate(fInitStat)

public:
 ~GeoMagField() {}

  /// Get the singleton pointer
  static GeoMagField *GetHead();
  /// Get the status of InitModel
  static int GetInitStat() { return fInitStat; }
  /// Field model file name
  static TString fModelFile;
  /// Field model time in year
  static double fModelYear;

public:
  /// Get northward component (nT)
  double GetX() const { return xcmp; }

  /// Get eastward component (nT)
  double GetY() const { return ycmp; }

  /// Get vertical downward component (nT)
  double GetZ() const { return zcmp; }

  /// Get total intensity (nT)
  double GetF() { return getf(); }

  /// Get horizontal intensity (nT)
  double GetH() { return geth(); }

  /// Initialize model
  int InitModel(const char *mdfile = 0, double date = fModelYear);

  /// Reset variables
  void Reset();

  /// Calculate field components (x,y,z)
  int Calc(double latitude, double longitude, double alt, 
	   int igdgc = 1);  // 1: Geodetic or 2: Geocentric altitude

  /// Calculate components and 1-year variations
  int CalcAll(double latitude, double longitude, double alt, 
	      int igdgc = 1);  // 1: Geodetic or 2: Geocentric altitude

  /// Print results with CalcAll
  void Print(Option_t *option = "") const;

protected:
  enum { MAXDEG   = 13,
	 MAXCOEFF = (MAXDEG*(MAXDEG+2)+1),
	 MAXMOD   = 30 }; // Max number of models in a file

  // Schmidt quasi-normal internal
  double gh1[MAXCOEFF];   // spherical harmonic coeff.
  double gh2[MAXCOEFF];   // spherical harmonic coeff.
  double gha[MAXCOEFF];   // Coefficients of resulting model
  double ghb[MAXCOEFF];   // Coefficients of rate of change model

  int    modelI;          // Which model (Index)
  int    nmodel;          // Number of models in file
  int    max1  [MAXMOD];  // Integer array of MAXMOD Main field coeff.
  int    max2  [MAXMOD];  // Integer array of MAXMOD Secular variation coeff.
  int    max3  [MAXMOD];  // Integer array of MAXMOD Acceleration coeff.
  long irec_pos[MAXMOD];  // Integer array of MAXMOD Record counter for header
  double epoch [MAXMOD];  // Double  array of MAXMOD epoch of model
  double altmin[MAXMOD];  // Double  array of MAXMOD Minimum height of model
  double altmax[MAXMOD];  // Double  array of MAXMOD Maximum height of model
  double yrmin [MAXMOD];  // Double  array of MAXMOD Min year of model
  double yrmax [MAXMOD];  // Double  array of MAXMOD Max year of model
  double minyr;           // Min year of all models
  double maxyr;           // Max year of all models
  int    nmax;

  // Geomag global variables
  double sdate;         // Start date inputted
  double inc;           // Inclination (deg)
  double dec;           // Declination of the field from the geogr.north (deg)
  double tot;           // Total intensity (nT)
  double hol;           // Horizontal intensity (nT)
  double xcmp;          // Northward component (nT)
  double ycmp;          // Eastward  component (nT)
  double zcmp;          // Vertically-downward component (nT)

  // 1-year variations
  double xtemp, ytemp, ztemp;
  double dtemp, ftemp, htemp, itemp;
  double ddot, idot, fdot, hdot;
  double xdot, ydot, zdot;

  // Warning flags
  int    warn_H, warn_H_strong, warn_P;
  double warn_H_val, warn_H_strong_val;

  // Subroutines

  void reset();

  int readmdf(const char *mdfile);
  int calcall(int igdgc, double lat, double lng, double alt);
  int calcxyz(int igdgc, double lat, double lng, double alt);

  double getf();
  double geth();

  int my_isnan(double d) const {
    return (d != d);              /* IEEE: only NaN is not equal to itself */
  }

  double julday(int month, int day, int year);

  int getshc (const char *mdfile, int iflag, long int strec, 
	                          int nmax_of_gh, int gh);

  int extrapsh(double date, double dte1, int nmax1, int nmax2, int gh);
  int interpsh(double date, double dte1, int nmax1,
	                    double dte2, int nmax2, int gh);

  int shval3(int igdgc, double flat, double flon, double elev,
	     int nmax, int gh, int iext,
	     double ext1, double ext2, double ext3);

  int dihf(int gh);

  void print_dashed_line     () const;
  void print_long_dashed_line() const;
  void print_header   () const;
  void print_header_cp() const;
  void print_header_sv() const;
  void print_result   (double date, double d, double i, double h,
		                    double x, double y, double z,
		                                        double f) const;
  void print_result_cp(double date, double d, double i, double h) const;
  void print_result_sv(double date, double ddot,
		       double idot, double hdot,
		       double xdot, double ydot, double zdot,
		                                 double fdot) const;

public:
  ClassDef(GeoMagField, 1)
};

#endif
