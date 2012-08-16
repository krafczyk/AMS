/////////////////////
///   Library to convert from/to AMS reference 
///   it is supposed that the coordinate in AMS frame is of arrival direction.
//////////////////////
 /*  
 *  Authors :
 *           SDT - Stefano Della Torre - INFN Milano Bicocca - stefano.dellatorre@mib.infn.it
 *  Date: Aug 2012 - first version (SDT)
 *  ---- report here main changes ----
 *  SDT 16 aug 2012 - add FT_modulus function
 */

#include <iostream>
#include <stdio.h>
#include <string>
#include <cstdlib>
#include <fstream>
#include <math.h>
using namespace std;



/** *************** TO BE USED BY USER ********************/


int get_ams_ra_dec_fromGTOD(double PosISS[3], double VelISS[2], double rpy[3], double & ra, double &dec, double xtime);
///    generic trasformation from AMS frame to J2000 frame passing via GTOD (or ~ctrs)
///    PosISS[3] = posizion of ISS in r,azimut[i.e. longitude],elev[i.e. latitude] 
///    VelISS[2] = Velocity of ISS in  ISSVelPhi, ISSVelTheta
///    rpy[3]    = attitude wtr LVLH in ISSyaw,  ISSpitch,  ISSroll 
///    ra, dec   = azimut and elevation direction of arrival particle -> RA and DEC direction of arrival particle
///    xtime     = UTC time (in unix format)

int get_ams_l_b_fromGTOD(double PosISS[3], double VelISS[2], double rpy[3], double & Azim, double &Elev, double xtime);
///  generic trasformation from AMS frame to galactic frame passing via GTOD (or ~ctrs)
///  PosISS[3] = posizion of ISS in r,azimut[i.e. longitude],elev[i.e. latitude] 
///  VelISS[2] = Velocity of ISS in  ISSVelPhi, ISSVelTheta
///  rpy[3]    = attitude wtr LVLH in ISSyaw,  ISSpitch,  ISSroll 
///  ra, dec   = azimut and elevation direction of arrival particle -> RA and DEC direction of arrival particle
///  xtime     = UTC time (in unix format)


/** **************** STAR TRACKER *************************/

/* To Be Check */

/** **************** GROUND SEGMENT   ************************/

void	FT_Cart2Angular(double x, double y, double z, double& r, double& theta, double& phi);
void	FT_Angular2Cart(double r, double theta, double phi, double& x, double& y, double& z );
double  FT_Modulus(double arg1, double arg2);
/*****************************************************************************/
void	FT_AMS2Body(double &x, double &y, double &z);
void	FT_Body2AMS(double &x, double &y, double &z);
/*****************************************************************************/
void	FT_LVLH2Body(double &x, double &y, double &z, double ISSyaw, double ISSpitch, double ISSroll);
void	FT_Body2LVLH(double &x, double &y, double &z, double ISSyaw, double ISSpitch, double ISSroll);
/*****************************************************************************/
void	FT_GTOD2LVLH(double &x, double &y, double &z, double ISSaltitude,double ISSasc,double ISSdec,double ISSVelPhi, double ISSVelTheta );
void	FT_LVLH2GTOD(double &x, double &y, double &z, double ISSaltitude,double ISSasc,double ISSdec,double ISSVelPhi, double ISSVelTheta );
/*****************************************************************************/
double  FT_GMST_rad(double timeUnix);
void	FT_GTOD2Equat(double &x, double &y, double &z, double time);
void	FT_Equat2GTOD(double &x, double &y, double &z, double time);
/**************************************************************/
double  FT_GPS_JD(double itime);
double  FT_UTC_JD(double itime);
/**************************************************************/
int  FT_Equat2Gal(double &azimut, double &elev);
int  FT_Gal2Equat(double &azimut, double &elev);





