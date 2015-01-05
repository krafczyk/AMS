//--------------------------------------------------------
//    
//   Library Functions to Calculate Stoermer Rigidity cutoff
//   http://www.ngdc.noaa.gov/geomag/geom_util/gmpole.shtml
//   Filename: GM_SubLibrary.C 
//   Creation date: 05/11/2012      
//
//   Implemented by : D. Grandi
//                    C. Consolandi
//                    S. Della Torre
//                    M. Tacconi
//
//--------------------------------------------------------

#include "typedefs.h"
#include <cmath>
#include <cstring>
#include <time.h>
#include "GM_SubLibrary.h"
        static GMtype_Data G10, G11, H11, G20, G21, H21, G22, H22;
        static int scanned=0;
#pragma omp threadprivate  (G10, G11, H11, G20, G21, H21, G22, H22,scanned) 


double GeoMagCutoff(time_t Utime, double Altitude , double thetaPart, double phiPart, double thetaISS, double phiISS, int pos  ){

/*
 *    Program GMPOLE 
 *       Model for 1900 to 2015 uses linear interpolation of Gauss coefficients
 *       Program finds location of North Geomagnetic Pole for input date
 *       Calculate Geomagnetic Coordinates in simple shifted tilted dipole model
 *       Compute Stoermer Rigidity cutoff calculation
 *                   
 *
 */


        double blat,blong,distd,elong,gam,a,b,d,e,fp,fn;
        double pcut2,pcutd=0,dp2;
        double Moment,Stormer;
        double Re = 6371.2;             //Earth radius in km
 

        GMtype_Date date;
        GMtype_Pole Pole;
        GMtype_Ellipsoid Ellip;
        GMtype_Model Model;
        GMtype_CoordSpherical CoordInput, MagSpherical, DipoleSpherical,GeoDir;
        GMtype_CoordDipole MagDir;

        //--------------------------Att!! path file IGRF.tab
        //...fill from IGRF.tab file:
        if(!scanned)
#pragma omp critical (scan)
{
         scanned=1;
         cout <<" GeoMagCutoff-I-IGRFtoBeRead "<<endl;
         GM_ScanIGRF(&G10, &G11, &H11, &G20, &G21, &H21, &G22, &H22 );
         cout <<" GeoMagCutoff-I-IGRFRead "<<endl;
        }
        //This function sets the WGS84 reference ellipsoid to its default values:
        GM_SetEllipsoid(&Ellip);

        //...Set input coordinateds in GTOD
        //ISS:
        CoordInput.phig=thetaISS;//LAT.
        CoordInput.lambda=phiISS;//LONG.
        CoordInput.r=Altitude;
        //Event direction:
        GeoDir.phig=thetaPart;//LAT.
        GeoDir.lambda=phiPart;//LONG.

        //...Convert Unix time into human time
        time_t utime=Utime;
        struct tm * newTime = gmtime( &utime ); //convert unixtime
                date.Year = newTime->tm_year +1900;// 0 is 1900 
                date.Month = newTime->tm_mon +1; // from 0 to 11
                date.Day = newTime->tm_mday;

             // Decimal year
                GM_DateToYear(&date);
                //linear interpolation of coeff
                GM_TimeAdjustCoefs(date, G10, G11, H11, G20, G21, H21, G22, H22, &Model);

                //Calculate Pole location
                GM_PoleLocation(Model, &Pole);

                Moment = Pole.M;
                //magnetic moment of the Earth A*m^2 (needed in Gauss*cm^3 there is a factor 1e3 
		//used below for Stormer Constant


                //......Stormer Constant:
                Stormer = Moment/Re/Re/1e5/1e5*300/1e9*1e3;             //constant for Rigidity cutoff


               // function for dipole center location
                GM_DipoleLocation(Model, &DipoleSpherical);

                // function for Geomagnetic coord
                GM_GeomagCoordLocation(CoordInput, DipoleSpherical, Pole, &MagSpherical);
                blat = RAD2DEG(MagSpherical.phig);
                blong = RAD2DEG(MagSpherical.lambda);
                distd = MagSpherical.r;



                GeoDir.r = CoordInput.r;
                // function for geomagnetic from direction
                GM_GeomagCoordDirection(GeoDir, DipoleSpherical, Pole, &MagDir);

                elong = RAD2DEG(MagDir.lambda);


                // function for Rigidity cutoff calculation

                // calculate gamma angle 
                gam = DEG2RAD(90+(elong - blong));

                //printf("GAMMA is %lf\n",gam);

/*Calculate the main Rigidity Cut-Off  at a distance from the center of the dipole equal to the earth radius
 * here different formula not original Stoermer
 * Pc=(M/R^2)*(cos(ThetaM)^4)/(sqrt(1-cos(Gamma)*cos(ThetaM)^3)+1)^2 
 * vald for all dir... also gamma = 0 */


                        a = cos(gam);
                        b = cos(DEG2RAD(blat));
                        d = pow(b,3);
                        e = a*d;

                        fp = sqrt(1-e);//positive particles
                        fn = sqrt(1+e); //negative 

               // distance from dipole in unit of Re   

                        dp2 = distd/Re;
// here different formula and check so Pc=(M/R^2)*(cos(ThetaM)^4)/(sqrt(1+cos(Gamma)*cos(ThetaM)^3))^2 
                  //valid for all dir... also gamma = 0 


			if(pos==1){
                        pcutd=(Stormer*pow(b,4))/pow((fp+1),2);    //    ONLY for positive particles
               //         pcutd=(mr2*pow(b,4))/pow((fp+1),2);   //ONLY for positive particles

			}else if( pos==0){
			
			pcutd=(Stormer*pow(b,4))/pow((fn+1),2) *(-1.); //    ONLY for negative particles
                       // pcutd=(mr2*pow(b,4))/pow((fn+1),2)*(-1.); //    ONLY for negative particles

			}	
                // correct here with 14.9/r/r or 59.6/r/r where r is dipole 
// 		center distance check east west angle with tests...  
                /// new cutoff related to distance from Dipole center... in unit of Re.. 


                        pcut2=pcutd/dp2/dp2;


                // test distance from Earth center...
                 //       galt = CoordInput.r + Re;
                 //       galt2 = galt/Re;                //MISSING galt
                 //       pcut3 = pcutd/galt2/galt2;


                  // printf("GEOM  LAT %lf GEOM LONG %lf DIPOLE DIST %lf \n",blat,blong,distd);

                 // printf(" \n");
                  //printf("RECALC CUTOFF %lf RECALC EARTH %lf CUTOFF %lf \n",pcut2,pcut3,pcutd); 
                 // printf("RECALC CUTOFF %lf  \n",pcut2);
                  //

return pcut2;





}
//---------------------------------------------------------------------------------------
double GM_GetThetaM(time_t Utime, double Altitude , double thetaISS, double phiISS){

        double GMC[3];
        GM_GeoMagSphericalCoo( Utime,  Altitude , thetaISS,  phiISS, GMC );
        double ThetaM = GMC[2];


return ThetaM;
}
//--------------------------------------------------------------------------------------
double GM_GetPhiM(time_t Utime, double Altitude , double thetaISS, double phiISS){

        double GMC[3];
        GM_GeoMagSphericalCoo( Utime,  Altitude , thetaISS,  phiISS, GMC);
        double PhiM = GMC[1];


return PhiM;
}
//--------------------------------------------------------------------------------------
void GM_GeoMagSphericalCoo(time_t Utime, double Altitude , double thetaISS, double phiISS, double GMC[3]){

        //check IGRF coefficients
        if(!scanned)
#pragma omp critical (scan)
{
         scanned=1;
         cout <<" GeoMagCutoff-I-IGRFtoBeRead "<<endl;
         GM_ScanIGRF(&G10, &G11, &H11, &G20, &G21, &H21, &G22, &H22 );
         cout <<" GeoMagCutoff-I-IGRFRead "<<endl;
        }
        //------------------
        //------------------
        GMtype_Date  date;
        //set human time date
        time_t utime=Utime;
        struct tm * newTime = gmtime( &utime ); //convert unixtime
                date.Year = newTime->tm_year +1900;// 0 is 1900 
                date.Month = newTime->tm_mon +1; // from 0 to 11
                date.Day = newTime->tm_mday;
        // Decimal year
        GM_DateToYear(&date);

        //--------------------
        GMtype_CoordSpherical CoordInput;
        //input spherical coordinates GTOD
        CoordInput.phig=thetaISS;//LAT. (deg)
        CoordInput.lambda=phiISS;//LONG. (deg)
        CoordInput.r=Altitude;//km

        //---------------
        GMtype_Model Model;
        //linear interpolation of coeff
        GM_TimeAdjustCoefs(date, G10, G11, H11, G20, G21, H21, G22, H22, &Model);

        //----------------
        GMtype_CoordSpherical DipoleSpherical;
        // function for dipole center location
        GM_DipoleLocation(Model, &DipoleSpherical);

        //-----------------
        GMtype_Pole Pole;
        //Calculate Pole location
        GM_PoleLocation(Model, &Pole);

        //--------------------------    
        GMtype_CoordSpherical MagSpherical;
        // function for Geomagnetic coord
        GM_GeomagCoordLocation(CoordInput, DipoleSpherical, Pole, &MagSpherical);

                //ThetaM, PhiM, rM:
           GMC[2]    = RAD2DEG(MagSpherical.phig);
           GMC[1]    = RAD2DEG(MagSpherical.lambda);
           GMC[0]    = MagSpherical.r;

}
//-------------------------------------------------------------------------------------------------------



void GM_CartesianToSpherical(GMtype_CoordCartesian CoordCartesian, GMtype_CoordSpherical *CoordSpherical)
{
	/*This function converts a point from Cartesian coordinates into spherical coordinates*/
	double X, Y, Z;
	
	X = CoordCartesian.x;
	Y = CoordCartesian.y;
	Z = CoordCartesian.z;

	CoordSpherical->r = sqrt(X * X + Y * Y + Z * Z);
	CoordSpherical->phig = RAD2DEG(asin(Z / (CoordSpherical->r)));
	CoordSpherical->lambda = RAD2DEG(atan2(Y, X));
} /*GM_CartesianToSpherical*/

void GM_CORD (GMtype_CoordGeodetic location, GMtype_Date *date, GMtype_Ellipsoid Ellip, GMtype_Data g10d, GMtype_Data g11d, GMtype_Data h11d, GMtype_Data g20d, GMtype_Data g21d,
GMtype_Data h21d, GMtype_Data g22d, GMtype_Data h22d, GMtype_CoordDipole *CoordDipole, GMtype_CoordSpherical CoordInput)
{
	/*This function can be used in a wrapper file to call all of the necessary functions to convert from geocentric coordinates to geomagnetic coordinates for a given time*/
	GMtype_CoordSpherical CoordSpherical, DipoleSpherical,  MagSpherical, ModDipoleSpherical; 

//------Not used:
//GeoDir; /*Added*/
// GMtype_CoordDipole MagDir;


	GMtype_CoordCartesian CoordCartesian, DipoleCartesian;
	GMtype_Model Model;
	GMtype_Pole Pole;
	GM_DateToYear(date);
	GM_GeodeticToSpherical(Ellip, location, &CoordSpherical);
	GM_SphericalToCartesian(CoordSpherical,  &CoordCartesian);
	GM_TimeAdjustCoefs(*date, g10d, g11d, h11d, g20d, g21d, h21d, g22d, h22d, &Model);
	GM_PoleLocation(Model, &Pole);
	GM_DipoleLocation(Model, &ModDipoleSpherical); /*Added*/
  	 GM_GeomagCoordLocation(CoordInput, ModDipoleSpherical, Pole,  &MagSpherical);
	GM_EarthCartToDipoleCartCD(Pole, CoordCartesian, &DipoleCartesian);
	GM_CartesianToSpherical(DipoleCartesian, &DipoleSpherical);
	CoordDipole->phi = DipoleSpherical.phig;
	CoordDipole->lambda = DipoleSpherical.lambda;	
} /*GM_CORD*/

int GM_DateToYear(GMtype_Date *CalendarDate)
{
	/*This function converts a Date into Decimal years.  It was taken from the WMM SubLibrary on the NGDC website*/
	int temp = 0; /*Total number of days */
	int MonthDays[13];
	int ExtraDay = 0;
	int i;
	if((CalendarDate->Year%4 == 0 && CalendarDate->Year%100 != 0) || CalendarDate->Year%400 == 0)
		ExtraDay = 1;
	MonthDays[0] = 0;
	MonthDays[1] = 31;
	MonthDays[2] = 28 + ExtraDay;
	MonthDays[3] = 31;
	MonthDays[4] = 30;
	MonthDays[5] = 31;
	MonthDays[6] = 30;
	MonthDays[7] = 31;
	MonthDays[8] = 31;
	MonthDays[9] = 30;
	MonthDays[10] = 31;
	MonthDays[11] = 30;
	MonthDays[12] = 31;

	for(i = 1; i <= CalendarDate->Month; i++)
		temp+=MonthDays[i-1];
	temp+=CalendarDate->Day;
	CalendarDate->DayNumber = temp;
	CalendarDate->DecimalYear = CalendarDate->Year + (temp-1)/(365.0 + ExtraDay);
	return 1;//TRUE;
}  /*GM_DateToYear*/ 

void GM_EarthCartToDipoleCartCD(GMtype_Pole Pole, GMtype_CoordCartesian EarthCoord, GMtype_CoordCartesian *DipoleCoords)
{
	/*This function converts from Earth centered cartesian coordinates to dipole centered cartesian coordinates*/
	double X, Y, Z, CosPhi, SinPhi, CosLambda, SinLambda;
	CosPhi = cos(DEG2RAD(Pole.phi));
	SinPhi = sin(DEG2RAD(Pole.phi));
	CosLambda = cos(DEG2RAD(Pole.lambda));
	SinLambda = sin(DEG2RAD(Pole.lambda));
	X = EarthCoord.x;
	Y = EarthCoord.y;
	Z = EarthCoord.z;
	
	/*These equations are taken from a document by Wallace H. Campbell*/
	DipoleCoords->x = X * CosPhi * CosLambda + Y * CosPhi * SinLambda - Z * SinPhi;
	DipoleCoords->y = -X * SinLambda + Y * CosLambda;
	DipoleCoords->z = X * SinPhi * CosLambda + Y * SinPhi * SinLambda + Z * CosPhi;
} /*GM_EarthCartToDipoleCartCD*/


void GM_GeodeticToSpherical(GMtype_Ellipsoid Ellip, GMtype_CoordGeodetic CoordGeodetic, GMtype_CoordSpherical *CoordSpherical)
{
	double CosLat, SinLat, rc, xp, zp; /*all local variables */
	/*
	** Convert geodetic coordinates, (defined by the WGS-84
	** reference ellipsoid), to Earth Centered Earth Fixed Cartesian
	** coordinates, and then to spherical coordinates.
	*/

	CosLat = cos(DEG2RAD(CoordGeodetic.phi));
	SinLat = sin(DEG2RAD(CoordGeodetic.phi));

	/* compute the local radius of curvature on the WGS-84 reference ellipsoid */

	rc = Ellip.a / sqrt(1.0 - Ellip.epssq * SinLat * SinLat);

	/* compute ECEF Cartesian coordinates of specified point (for longitude=0) */

	xp = (rc + CoordGeodetic.HeightAboveEllipsoid) * CosLat;
	zp = (rc*(1.0 - Ellip.epssq) + CoordGeodetic.HeightAboveEllipsoid) * SinLat;

	/* compute spherical radius and angle lambda and phi of specified point */

	CoordSpherical->r = sqrt(xp * xp + zp * zp);
	CoordSpherical->phig = RAD2DEG(asin(zp / CoordSpherical->r));     /* geocentric latitude */
	CoordSpherical->lambda = CoordGeodetic.lambda;                   /* longitude */
} /*GM_GeodeticToSpherical*/

void GM_GetUserInput(GMtype_CoordGeodetic *location, GMtype_Date *date)
{
	/*This function is used in the GMCORD program to get the users input data through the console*/
	char buffer[20];
	int flag; /*This is a loop control that allows the program to check the validity of the users input*/
	flag = 1;
	while(flag == 1)
	{
		printf("ENTER the geographic decimal latitude (+ North; - South)? ");
		fgets(buffer, 20, stdin);
		sscanf(buffer, "%lf", &location->phi);
		flag = 0;
		if(location->phi <-90 || location->phi > 90)
		{
			printf("Input must be in decimal degrees, between -90 and 90\n");
			flag = 1;
		}
		strcpy(buffer, "");
	}
	flag = 1;
	while(flag == 1)
	{
		printf("ENTER the geographic longitude (+ East; - West)? ");
		fgets(buffer, 20, stdin);
		sscanf(buffer, "%lf", &location->lambda);
		flag = 0;
		if(location->lambda < -180 || location->lambda > 360)
		{
			printf("Input must be in decimal degrees, between -180 and 360\n"); 
			flag = 1;
		}
		strcpy(buffer, "");
	}
	flag = 1;
	while(flag == 1)
	{
		printf("ENTER the analysis year (1900 to 2015)? ");
		fgets(buffer, 20, stdin);
		sscanf(buffer, "%d", &date->Year);
		flag = 0;
		if(date->Year < 1900 || date->Year > 2015)
		{
			printf("Input must be in integer years between 1900 and 2015\n");
			flag = 1;
		}
		strcpy(buffer, "");
	}
	flag = 1;
	while(flag == 1)
	{
		printf("Month number (1 to 12)? ");
		fgets(buffer, 20, stdin);
		sscanf(buffer, "%d", &date->Month);
		flag = 0;
		if(date->Month < 1 || date->Month > 12)
		{
			printf("Input must be an integer from 1 to 12.\n");
			flag = 1;
		}
		strcpy(buffer, "");
	}
	flag = 1;
	while(flag == 1)
	{
		printf("Day in month (1 to 31)? ");
		fgets(buffer, 20, stdin);
		sscanf(buffer, "%d", &date->Day);
		flag = 0;
		if(date->Day < 1 || date->Day > 31)
		{
			printf("Input must be an integer from 1 to 31.\n");
			flag = 1;
		}
		strcpy(buffer, "");
	}
} /*GM_GetUserInput*/

void GM_PoleLocation(GMtype_Model Model, GMtype_Pole *Pole)
{

	
	double Re = 6371.2;	/*Earth radius in km*/
	double B0,B02,Re3;
	double mu0 = 0.0000001*4*M_PI;
	
	/*This function finds the location of the north magnetic pole in spherical coordinates.  The equations are
	**from Wallace H. Campbell's Introduction to Geomagnetic Fields*/

	Pole->phi = RAD2DEG(-atan(sqrt(Model.h11 * Model.h11 + Model.g11 * Model.g11)/Model.g10)); /*colatitude*/
	Pole->lambda = RAD2DEG(atan(Model.h11/Model.g11));					   /*longitude*/

	
	B02 = Model.g10*Model.g10 + Model.g11*Model.g11 + Model.h11*Model.h11;
	B0 = sqrt(B02);
	Re3 = Re*Re*Re;
	Pole->M = 4*M_PI*B0*Re3/mu0;
	
	//printf("HERE Magnetic Moment %lf\n",M);
	
} /*GM_PoleLocation*/

/*DAV here insert calculation for position of the Dipole */


void GM_DipoleLocation(GMtype_Model Model, GMtype_CoordSpherical *DipoleSpherical)
{ 
	/* This function finds the location of the magnetic dipole center in spherical coordinates and its distance from the Earth center.  
	The equations are from Fraser-Smith Centered and Eccentric Geomagnetic Dipoles and Their Poles 1987*/
	
	double Re = 6371.2;
	
	double L0 = (2 * Model.g10 * Model.g20) + sqrt(3) * (Model.g11 * Model.g21 + Model.h11 * Model.h21); 
	double L1 = -Model.g11 * Model.g20 + sqrt(3) * (Model.g10 * Model.g21 + Model.g11 * Model.g22 + Model.h11 * Model.h22);
	double L2 = -Model.h11 * Model.g20 + sqrt(3) * (Model.g10 * Model.h21 - Model.h11 * Model.g22 + Model.g11 * Model.h22);
	double B02 = (Model.g10 * Model.g10 + Model.g11 * Model.g11 + Model.h11 * Model.h11);
	double E = (L0 * Model.g10 + L1 * Model.g11 + L2 * Model.h11)/4/B02; 
	
	double Zd = Re * (L0 - Model.g10 * E)/3/B02;
	double Xd = Re * (L1 - Model.g11 * E)/3/B02;
	double Yd = Re * (L2 - Model.h11 * E)/3/B02;
	
	
	DipoleSpherical->r = sqrt(Xd*Xd + Yd*Yd + Zd*Zd);

	DipoleSpherical->phig = RAD2DEG(asin(Zd/DipoleSpherical->r));	/*colatitude DEG*/
	DipoleSpherical->lambda = 90 + RAD2DEG(atan(-Xd/Zd)); 		/*longitude DEG*/ 

	//-----------------------NEW
//       DipoleSpherical->lambda = 90 + RAD2DEG(atan2(-Xd,Zd));                 /*longitude DEG*/

	
//	CoordSpherical->phig = asin(Zd/CoordSpherical->r);		/*colatitude RAD*/
//	CoordSpherical->lambda = 3.141/2. + atan(-Xd/Zd);		/*longitude RAD*/ 
	
	
	
}/*GM_DipoleLocation*/

/*DAV here insert calculation for geomagnetic Coordinates */

void GM_GeomagCoordLocation(GMtype_CoordSpherical CoordInput, GMtype_CoordSpherical DipoleSpherical, GMtype_Pole Pole, GMtype_CoordSpherical *MagSpherical)
{
 /*This is my function to calculate the Geomagnetic Cood in simple shifted tilted dipole*/
 double phi_in,lambda_in,r_in;
 double Re = 6371.2;
 double phig2,lambdag2,rg2,phin2,lambdan2;
 double xx,yy,zz,dx,dy,dz,x,y,z,rx,ry,rz,rdx,rdy,rdz;
 double plambdas,pphis,a,b,c,d,e,f,p,q,enx,eny,enz;





//............Not Used:
// double distd;

 
 /*input position*/
 phi_in = DEG2RAD(CoordInput.phig);
 lambda_in = DEG2RAD(CoordInput.lambda);
 r_in = CoordInput.r;
 r_in = Re+r_in;
// printf("LAT INPUT %lf in rad LONG %lf in rad RADIUS %lf in km\n",phi_in,lambda_in,r_in);
 
 /*	calculate the cartesian components of the point */
  x = r_in * cos(phi_in) * cos(lambda_in);	  
  y = r_in * cos(phi_in) * sin(lambda_in);
  z = r_in * sin(phi_in);
	

 /*Data from Dipole position RAD*/
 phig2 = DEG2RAD(DipoleSpherical.phig);
 lambdag2 = DEG2RAD(DipoleSpherical.lambda);
 
 /*Data from Dipole position DEG*/
// phig2 = DipoleSpherical.phig;
// lambdag2 = DipoleSpherical.lambda;
 
 rg2 = DipoleSpherical.r;			/*in km*/
 //rg2 = DipoleSpherical.r*100000;		/*in cm*/
 
 //printf("\nDIPOLE LAT %lf in rad LONG %lf in rad RADIUS %lf in km \n",phig2,lambdag2,rg2); 
 //printf("\nDIPOLE LAT %lf in deg LONG %lf in deg RADIUS %lf in cm \n",phig2,lambdag2,rg2);
  
 /*	calculate the cartesian components of the dipole center */
 xx = rg2 * cos(lambdag2) * cos(phig2); 
 yy = rg2 * sin(lambdag2) * cos(phig2);
 zz = rg2* sin(phig2);		        

/*	calculate distance from the center of the dipole*/	   
 dx = x - xx;	    	      
 dy = y - yy;	    
 dz = z - zz;	    
 
	 
 /*north pole in RAD*/
 phin2 = -DEG2RAD(90 - Pole.phi);
 lambdan2 = DEG2RAD(Pole.lambda + 180.);
 /*north pole in DEG*/
// phin2 = -(90 - Pole.phi);
// lambdan2 = (Pole.lambda + 180.);
 
 //printf("\nThe  NORTH POLE is:\n%.6lf N latitude and %.6lf E longitude in RAD\n", phin2, lambdan2);
 	 
 /*	REAL north  magnetic pole position (use for transformation) just get Colatitude..*/
 plambdas = DEG2RAD(90.) - phin2 ;	      
 pphis = lambdan2;

 /*	set trigonometry */
 a = cos(plambdas);
 b = cos(pphis);
 c = sin(plambdas);
 d = sin(pphis);
 e = a * b;
 f = a *d;
 p = c * b;
 q = c * d;
	
	
 /*	transformations , rotation of the point */
 rx = e * x + f * y - c * z;
 ry = b * y - d * x; 
 rz = p * x + q * y + a * z;
	
	
	
 /*	rotation of the dipole center */
 rdx = e * xx + f * yy - c * zz;
 rdy = b * yy - d * xx;
 rdz = p * xx + q * yy + a * zz;

 /*	transformation from geographic coordinates to geomagnetic coordinates
	shift of the dipole...new cartesian coordinates. */
 enx = rx - rdx;
 eny = ry - rdy;
 enz = rz - rdz; 

 /*	new geomagnetic spherical coordinates in the shifted-rotated system */
 	
 MagSpherical->r = sqrt(dx * dx + dy * dy + dz * dz);
 MagSpherical->phig = asin(enz/MagSpherical->r);
 MagSpherical->lambda = atan2(eny, enx);

}/*GM_GeomagCoordLocation*/


void GM_GeomagCoordDirection(GMtype_CoordSpherical GeoDir, GMtype_CoordSpherical DipoleSpherical, GMtype_Pole Pole, GMtype_CoordDipole *MagDir) // not used DipoleSpherical

{
 double phidir_in,lambdadir_in,rdir_in;
 double Re =6371.2;
 double phin,lambdan;
 double rx,ry,rz,x,y,z;
 double plambdas,pphis,a,b,c,d,e,f,p,q;
 

//-----------No Used
// rdx, rdy,rdz;
//,enx,eny,enz;


 /*Particle's direction*/
 
 lambdadir_in =  DEG2RAD(GeoDir.lambda);
 phidir_in = DEG2RAD(GeoDir.phig);

 rdir_in = GeoDir.r+Re; 

 //printf("\nHERE IS DIRECTION %lf LAT %lf LONG e %lf DIST\n",GeoDir.phig,GeoDir.lambda,GeoDir.r);
 
 /*	calculate the cartesian components of the point */
  x = rdir_in * cos(phidir_in) * cos(lambdadir_in);	  
  y = rdir_in * cos(phidir_in) * sin(lambdadir_in);
  z = rdir_in * sin(phidir_in);

 /*north pole in RAD*/
 
 phin = -DEG2RAD(90 - Pole.phi);
 lambdan = DEG2RAD(Pole.lambda + 180.);
 
 //printf("\nThe  NORTH POLE is:\n%.6lf N latitude and %.6lf E longitude in RAD\n", phin, lambdan);
 	 
 /*	REAL north  magnetic pole position (use for transformation) just get colatitude..*/
 plambdas = DEG2RAD(90.) - phin ;	      
 pphis = lambdan;	

 /*	set trigonometry */

 a = cos(plambdas);
 b = cos(pphis);
 c = sin(plambdas);
 d = sin(pphis);
 e = a * b;
 f = a * d;
 p = c * b;
 q = c * d;
 
 /*	transformations , rotation of the point */
 rx = e * x + f * y - c * z;
 ry = b * y - d * x ;
 rz = p * x + q * y + a * z;
 /*	new geomagnetic spherical direction in the shifted-rotated system */
 	
 MagDir->phi = asin(rz/rdir_in);
 MagDir->lambda = atan2(ry, rx);
	
	
 
}/*GM_GeomagCoordDirection*/


void GM_PrintUserData(GMtype_CoordGeodetic location, GMtype_Date date, GMtype_CoordDipole DipLocation)
{//---not used GMtype_Date date,

	/*This function prints the users data to the console*/
	printf("Centered Geomagnetic Dipole location at %lf, %lf:\n", location.phi, location.lambda);
	if(DipLocation.phi >= 0)
		printf("\t+%.2lf deg geomag lat. ", DipLocation.phi);
	else
		printf("\t%.2lf deg geomag lat. ", DipLocation.phi);
	if(DipLocation.lambda >= 0)
		printf("and +%.2lf deg E. geomag long.\n", DipLocation.lambda);
	else
		printf("and %.2lf deg E. geomag long.\n", DipLocation.lambda);
} /*GM_PrintUserData*/

void GM_ScanIGRF(GMtype_Data *G10, GMtype_Data *G11, GMtype_Data *H11, GMtype_Data *G20, GMtype_Data *G21, GMtype_Data *H21, GMtype_Data *G22, GMtype_Data *H22)
{
	/*This function scans inputs G10, G11, and H11 of the IGRF table into 3 data arrays*/
	int i;
	double temp;
	char buffer[200]; 		/*check buffer!*/
	FILE *IGRF;
        char pathFile[2048];
        sprintf( pathFile,"%s/v5.00/%s", getenv("AMSDataDir"), "IGRF.tab" );
        IGRF = fopen( pathFile, "r");

	if(IGRF==NULL) printf("GM_SubLibrary::GM_ScanIGRF The file IGRF.tab does not exist!\n");
	else{
	G10->size = 25;
	G11->size = 25;
	H11->size = 25;
	G20->size = 25;
	G21->size = 25;
	H21->size = 25;
	G22->size = 25;
	H22->size = 25;
	
	for( i = 0; i < 4; i++)
	{
		fgets(buffer, 200, IGRF);		/*first 4 lines...*/
	}
	
	fscanf(IGRF, "g 1 0 %lf ", &G10->element[0]); 	/*G10*/
	for(i = 1; i <= 22; i++)
	{
		fscanf(IGRF ,"%lf ", &G10->element[i]);
	}
	fscanf(IGRF ,"%lf\n", &temp);
	G10->element[23] = temp * 5 + G10->element[22];
	G10->element[24] = G10->element[23] + 5 * temp;
	
	fscanf(IGRF, "g 1 1 %lf ", &G11->element[0]);	/*G11*/
	for(i = 1; i <= 22; i++)
	{
		fscanf( IGRF, "%lf ", &G11->element[i]);
	}
	fscanf(IGRF, "%lf\n", &temp);
	G11->element[23] = temp * 5 + G11->element[22];
	G11->element[24] = temp * 5 + G11->element[23];
	
	fscanf(IGRF, "h 1 1 %lf ", &H11->element[0]);	/*H11*/
	for(i = 1; i <= 22; i++)
	{
		fscanf( IGRF, "%lf ", &H11->element[i]);
	}
	fscanf(IGRF, "%lf\n", &temp);
	H11->element[23] = temp * 5 + H11->element[22];
	H11->element[24] = temp * 5 + H11->element[23];
	
	fscanf(IGRF, "g 2 0 %lf ", &G20->element[0]);	/*G20*/
	for(i = 1; i <= 22; i++)
	{
		fscanf( IGRF, "%lf ", &G20->element[i]);
	}
	fscanf(IGRF, "%lf\n", &temp);
	G20->element[23] = temp * 5 + G20->element[22];
	G20->element[24] = temp * 5 + G20->element[23];
	
	fscanf(IGRF, "g 2 1 %lf ", &G21->element[0]);	/*G21*/
	for(i = 1; i <= 22; i++)
	{
		fscanf( IGRF, "%lf ", &G21->element[i]);
	}
	fscanf(IGRF, "%lf\n", &temp);
	G21->element[23] = temp * 5 + G21->element[22];
	G21->element[24] = temp * 5 + G21->element[23];
	
	fscanf(IGRF, "h 2 1 %lf ", &H21->element[0]);	/*H21*/
	for(i = 1; i <= 22; i++)
	{
		fscanf( IGRF, "%lf ", &H21->element[i]);
	}
	fscanf(IGRF, "%lf\n", &temp);
	H21->element[23] = temp * 5 + H21->element[22];
	H21->element[24] = temp * 5 + H21->element[23];
	
	fscanf(IGRF, "g 2 2 %lf ", &G22->element[0]);	/*G22*/
	for(i = 1; i <= 22; i++)
	{
		fscanf( IGRF, "%lf ", &G22->element[i]);
	}
	fscanf(IGRF, "%lf\n", &temp);
	G22->element[23] = temp * 5 + G22->element[22];
	G22->element[24] = temp * 5 + G22->element[23];
	
	fscanf(IGRF, "h 2 2 %lf ", &H22->element[0]);	/*H22*/
	for(i = 1; i <= 22; i++)
	{
		fscanf( IGRF, "%lf ", &H22->element[i]);
	}
	fscanf(IGRF, "%lf\n", &temp);
	H22->element[23] = temp * 5 + H22->element[22];
	H22->element[24] = temp * 5 + H22->element[23];

	fclose(IGRF); 

	}// end else
	
} /*GM_ScanIGRF*/

void GM_SetEllipsoid(GMtype_Ellipsoid *Ellip)
{
	/*This function sets the WGS84 reference ellipsoid to its default values*/
	Ellip->a	=			6378.137; /*semi-major axis of the ellipsoid in */
	Ellip->b	=			6356.7523142;/*semi-minor axis of the ellipsoid in */
	Ellip->fla	=			1/298.257223563;/* flattening */
	Ellip->eps	=			sqrt(1- ( Ellip->b *	Ellip->b) / (Ellip->a * Ellip->a ));  /*first eccentricity */
	Ellip->epssq	=			(Ellip->eps * Ellip->eps);   /*first eccentricity squared */
	Ellip->re	=			6371.2;/* Earth's radius */
} /*GM_SetEllipsoid*/

void GM_SphericalToCartesian(GMtype_CoordSpherical CoordSpherical, GMtype_CoordCartesian *CoordCartesian)
{
	/*This function converts spherical coordinates into Cartesian coordinates*/
	double CosPhi = cos(DEG2RAD(CoordSpherical.phig));
	double SinPhi = sin(DEG2RAD(CoordSpherical.phig));
	double CosLambda = cos(DEG2RAD(CoordSpherical.lambda));
	double SinLambda = sin(DEG2RAD(CoordSpherical.lambda));
	
	CoordCartesian->x = CoordSpherical.r * CosPhi * CosLambda;
	CoordCartesian->y = CoordSpherical.r * CosPhi * SinLambda;
	CoordCartesian->z = CoordSpherical.r * SinPhi;
} /*GM_SphericalToCartesian*/

void GM_TimeAdjustCoefs(GMtype_Date Date, GMtype_Data g10d, GMtype_Data g11d, GMtype_Data h11d, GMtype_Data g20d, GMtype_Data g21d, GMtype_Data h21d, GMtype_Data g22d,
GMtype_Data h22d, GMtype_Model *Model)
{
	/*This function calls GM_LinearInterpolation for the coefficients to estimate the value of the 
	**coefficient for the given date*/
	int index;
	double x;
	index = (Date.Year - GM_STARTYEAR) / 5;
	x = (Date.DecimalYear - GM_STARTYEAR) / 5;
	Model->g10 = GM_LinearInterpolation(index, index+1, g10d.element[index], g10d.element[index+1], x);
	Model->g11 = GM_LinearInterpolation(index, index+1, g11d.element[index], g11d.element[index+1], x);
	Model->h11 = GM_LinearInterpolation(index, index+1, h11d.element[index], h11d.element[index+1], x);
	Model->g20 = GM_LinearInterpolation(index, index+1, g20d.element[index], g20d.element[index+1], x);
	Model->g21 = GM_LinearInterpolation(index, index+1, g21d.element[index], g21d.element[index+1], x);
	Model->h21 = GM_LinearInterpolation(index, index+1, h21d.element[index], h21d.element[index+1], x);
	Model->g22 = GM_LinearInterpolation(index, index+1, g22d.element[index], g22d.element[index+1], x);
	Model->h22 = GM_LinearInterpolation(index, index+1, h22d.element[index], h22d.element[index+1], x);
} /*GM_TimeAdjustCoefs*/

//--------------------------------------------------------------------------------------------
//General math functions
//--------------------------------------------------------------------------------------------

double GM_DotProduct(GMtype_Data VectorA, GMtype_Data VectorB)
{
	/*This function takes the dot product of the elements stored in VectorA and VectorB*/
	double Total = 0;
	int i = 0;
	
	/*SUM(A_i * B_i)*/
	for(i = 0; i < VectorA.size; i++)
	{
		Total += VectorA.element[i] * VectorB.element[i];
	}
	return Total;
} /*GM_DotProduct*/

double GM_LinearInterpolation(double x1, double x2, double y1, double y2, double x)
{
	/*This function takes a linear interpolation between two given points for x*/
	double weight, y;
	weight  = (x - x1) / (x2 - x1);
	y = y1 * (1 - weight) + y2 * weight;
	return y;
}/*GM_LinearInterpolation*/

void GM_LUDecomposition(GMtype_Matrix A, GMtype_Matrix *L, GMtype_Matrix *U, GMtype_Matrix *P)
{
	/*This function LU decomposes a given matrix A and stores it in two new matricies L and U.
	**It uses the Gaussian Elimination with partial pivoting algorithm from Golub & Van Loan, Matrix Computations, **Algorithm 3.4.1.*/
	int i, j, pivot, k, N;
	double max;
	
	N = A.rows;
	L->rows = A.rows;
	L->columns = A.columns;
	U->rows = A.rows;
	U->columns = A.columns;
	for(i = 0; i < N - 1; i++)
	{
		max  = fabs(A.element[i][i]);
		pivot = i;
		
		for(j = i+1; j < N; j++)
		{
			if(fabs(A.element[j][i]) > max)
			{
				max = fabs(A.element[j][i]);
				pivot = j;
			}
		}
		
		if (pivot != i)
		{
			GM_SwapRows (&A, i, pivot);
			GM_SwapRows (P, i, pivot);
		}
		
		if(A.element[i][i] != 0.0)
		{
			for (j = i + 1; j < N; j++)
			{
				A.element[j][i] = A.element[j][i] / A.element[i][i];
				for (k = i + 1; k < N; k++)
				{
					A.element[j][k] = A.element[j][k] - A.element[j][i] * A.element[i][k];
				}
			}
		}
	}
	for(i = 0; i < N; i++)
	{
		L->element[i][i] = 1;
		for(j = 0; j < N; j++)
		{
			if(j < i)
			{	
				L->element[i][j] = A.element[i][j];
			}
			else
			{
				U->element[i][j] = A.element[i][j];
			}
		}
	}
} /*GM_LUDecomposition*/

void GM_LUSolve(GMtype_Matrix L, GMtype_Matrix U, GMtype_Matrix P, GMtype_Matrix *x, GMtype_Matrix b)
{
	/*This function solves an LU decomposed matrix equation LUx = b for x*/
	GMtype_Matrix y;
	int N, i, j;
	/*Apply permutation*/
	GM_MatMultiply(P, b, &b);
	N = U.rows;
	x->rows = b.rows;
	x->columns = 1;
	/*y: = Ux, thus Ly = b*/
	/*solve for y by forward substitution*/
	y.rows = b.rows;
	y.columns = 1;
	y.element[0][0] = b.element[0][0] / L.element[0][0];
	for(i = 1; i < N; i++)
	{
		y.element[i][0] = b.element[i][0] / L.element[i][i];
		for(j = 0; j < i; j++)
		{
			y.element[i][0] += -L.element[i][j] * y.element[j][0] / L.element[i][i];
		}
	}
	/*Ux = y*/
	/*Solve for x by backward substitution*/
	
	x->element[N-1][0] = y.element[N-1][0] / U.element[N-1][N-1];
	for(i = N - 2; i >= 0; i--)
	{
		x->element[i][0] = y.element[i][0] / U.element[i][i];
		for(j = i+1; j < N; j++)
		{
			x->element[i][0] += -U.element[i][j] * x->element[j][0] / U.element[i][i];
		}
	}
} /*GM_LUSolve*/

double GM_MatDet(GMtype_Matrix Matrix)
{
	/*This is a recursive function that calculates the determinant of the matrix*/
	double value = 0, SubDet;
	GMtype_Matrix TempMatrix;
	int i, j, k, l, m;
	
	if(Matrix.rows != Matrix.columns)
	{
		return 0;
	}
	if(Matrix.rows == 1)
	{
		value = Matrix.element[0][0];
		return value;
	}
	else if(Matrix.rows == 2)
	{
		value = Matrix.element[0][0] * Matrix.element[1][1] - Matrix.element[0][1] * Matrix.element[1][0];
		return value;
	}
	else if(Matrix.rows > 2)
	{
		TempMatrix.rows = Matrix.rows - 1;
		TempMatrix.columns = Matrix.columns - 1;
		for(m=0; m < Matrix.columns; m++) 
		{
			/*This loop finds the subdeterminant of the matrix, multiplies it by a the element 
			**in a column and sums or subtracts it to/from the total*/
			k = 0;
			for(i=0; i < Matrix.rows; i++)
			{
				l = 0;
				for(j=0; j < Matrix.columns; j++)
				{
					if(i!=0 && j!=m) 
					{
						TempMatrix.element[k][l] = Matrix.element[i][j];
						l++;
					}
				}
				if(i!=0) k++;
			}
			SubDet = GM_MatDet(TempMatrix); 
			if (m%2 == 1) SubDet = SubDet * -1;
			value += SubDet * Matrix.element[0][m];
		}
		return value;
	}
	else
	{
		return 0;
	}
} /*GM_MatDet*/

void GM_MatInverse(GMtype_Matrix Matrix, GMtype_Matrix *InvertedMatrix)
{
	/*This function calculates the inverse of an input matrix.  Error starts to become noticeable around 6x6 matricies.
	**The function starts to take very large amounts of time beyond 8x8 matricies.  LU decomposition is probably a
	**better approach for large matricies*/
	int x, y, m, n, i, j;
	double MatDet;
	GMtype_Matrix Temp;
	
	MatDet = GM_MatDet(Matrix);
	Temp.rows = Matrix.rows - 1;
	Temp.columns = Matrix.columns - 1;
	InvertedMatrix->rows = Matrix.rows;
	InvertedMatrix->columns = Matrix.columns;
	for(m = 0; m < Matrix.rows; m++)
	{
		for(n = 0; n < Matrix.columns; n++)
		{
			x = 0;
			for(i = 0; i < Matrix.rows; i++)
			{
				y = 0;
				for(j = 0; j < Matrix.columns; j++)
				{
					if(i != m && j != n)
					{
						Temp.element[x][y] = Matrix.element[i][j];
						y++;
					}
				}
				if (i!=m) x++;
			}
			InvertedMatrix->element[n][m] = GM_Pow(-1.0, n+m) * GM_MatDet(Temp) / MatDet;
		}
	}
} /*GM_MatInverse*/

void GM_MatMultiply(GMtype_Matrix MatrixA, GMtype_Matrix MatrixB, GMtype_Matrix *MatrixC)
{
	/*This function multiplies two input matricies A and B, and stores their product in C*/
	int i, j, k;
	GMtype_Matrix TMatrixB;
	GMtype_Data Row, Column;


	GM_MatTranspose(MatrixB, &TMatrixB);
	MatrixC->rows = MatrixA.rows;
	MatrixC->columns = MatrixB.columns;
	for(i = 0; i < MatrixA.rows; i++)
	{
		for(j = 0; j < MatrixB.columns; j++)
		{
			Row.size = MatrixA.columns;
			for(k = 0; k < MatrixA.columns; k++)
			{
				Row.element[k] = MatrixA.element[i][k];
			}
			Column.size = TMatrixB.columns;
			for(k = 0; k < TMatrixB.columns; k++)
			{
				Column.element[k] = TMatrixB.element[j][k];
			}
			MatrixC->element[i][j] = GM_DotProduct(Row, Column);
		}
	}
} /*GM_MatMultiply*/

void GM_MatTranspose(GMtype_Matrix Matrix, GMtype_Matrix *TMatrix)
{
	/*This function takes the transpose of the input matrix*/
	int i, j;
	TMatrix->rows = Matrix.columns;
	TMatrix->columns = Matrix.rows;
	for(i = 0; i < Matrix.rows; i++)
	{
		for(j = 0; j < Matrix.columns; j++)
		{
			TMatrix->element[j][i] = Matrix.element[i][j];
		}
	}
} /*GM_MatTranspose*/

double GM_Mean(GMtype_Data Data)
{
	/*This function average the elements of the input, Data*/
	int i;
	double total = 0, average;
	for(i = 0; i <= Data.size; i++)
	{
		total+=Data.element[i];
	}
	average = total / Data.size;
	return average;
} /*GM_Mean*/

void GM_Median(GMtype_Data Data, double *upper, double *lower)
{
	/*This function finds the median of the input, Data*/
	GM_Sort(&Data);
	if(Data.size%2 == 0) /*If the number of elements is even then there is an upper and lower bound for the median*/
	{
		*upper = Data.element[Data.size/2];
		*lower = Data.element[Data.size/2 - 1];
	}
	else /*If the number of elements is odd then the upper and lower bound are equivalent*/
	{
		*upper = Data.element[Data.size/2];
		*lower = Data.element[Data.size/2];
	}
} /*GM_Meadian*/

void GM_PolyFit(GMtype_Data DataX, GMtype_Data DataY, GMtype_Polynomial *Polynomial)
{
	/*This finds a best polynomial fit of the input degree for the input data set.  Polynomial must have 
	**a value in degree, otherwise the function will not know what degree to fit to.*/
	GMtype_Matrix X, TX, XSquared, b, Final, Y, L, U, P;
	int i, j;
	for(i = 0; i < DataX.size; i++)
	{
		for(j = 0; j <= Polynomial->degree; j++)
		{
			X.element[i][j] = GM_Pow(DataX.element[i],j);
		}
	}
	X.rows = DataX.size;
	X.columns = Polynomial->degree + 1;
	for(i = 0; i<= DataY.size; i++)
	{
		Y.element[i][0] = DataY.element[i];
	}
	Y.rows = DataY.size;
	Y.columns = 1;
	GM_MatTranspose(X, &TX);
	GM_MatMultiply(TX, X, &XSquared);
	P.rows = XSquared.rows;
	P.columns = XSquared.columns;
	for(i = 0; i < XSquared.rows; i++)
	{
		for(j = 0; j < XSquared.columns; j++)
		{
			if(i == j) P.element[i][j] = 1;
			else P.element[i][j] = 0;
		}
	}
	GM_LUDecomposition(XSquared, &L, &U, &P);
	GM_MatMultiply(TX, Y, &b);
	GM_LUSolve(L, U, P, &Final, b);
	for(i = 0; i <= Polynomial->degree; i++){
		Polynomial->coef[i] = Final.element[i][0];
	}
} /*GM_PolyFit*/

double GM_Pow(double x, int y)
{
	/*This function raises x to the y power, only works for positive exponents*/
	double value = 1;
	int i;
	if (x == 0)
		return 0;
	if(y<0)
	{
		for(i = 0; i < -1 * y; i++)
		{
			value = value / x;
		}
	}
	else
	{ 
		for(i = 0; i < y; i++)
		{
			value = value * x;
		}
	}
	
	return value;
} /*GM_Pow*/

void GM_PrintMatrix(GMtype_Matrix X)
{
	/*This function is used to print a matrix to the console*/
	int i, j;
	printf("\nRows = %d\nColumns = %d\n", X.rows, X.columns);
	for(i = 0; i < X.rows; i++)
	{
		printf("\n");
		for(j = 0; j < X.columns; j++)
		{
			printf("%lf   ", X.element[i][j]);
		}
	}
} /*GM_PrintMatrix*/

double GM_SolvePolynomial(GMtype_Polynomial Polynomial, double x)
{
	/*This function solves a polynomial for a given x*/
	int i;
	double y = 0;
	for(i = 0; i <= Polynomial.degree; i++)
	{
		if(x != 0)		
			y += Polynomial.coef[i] * GM_Pow(x, i);
	}
	return y;
} /*GM_SolvePolynomial*/

void GM_Sort(GMtype_Data *Data)
{
	/*This function sorts the array of elements in the input Data*/
	double lowest;
	int i, m, LowestIndex;
	lowest = Data->element[0];
	for(m = 0; m < Data->size; m++)
	{
		lowest = Data->element[m];
		LowestIndex = m;
		for(i = m; i < Data->size; i++)
		{
			if(lowest > Data->element[i])
			{
				lowest = Data->element[i];
				LowestIndex = i;
			}
		}
		GM_Swap(&Data->element[m], &Data->element[LowestIndex]);
	}
} /*GM_Sort*/

double GM_StandardDeviation(GMtype_Data Data)
{
	/*This function takes the standard deviation of the input Data*/
	double stdev, mean, variance;
	int i;
	mean = GM_Mean(Data);
	variance = 0;
	for(i = 0; i < Data.size; i++)
	{
		variance += (Data.element[i] - mean) * (Data.element[i] - mean) / (Data.size-1);
	}
	stdev = sqrt(variance);	
	return stdev;	
} /*GM_StandardDeviation*/

void GM_Swap(double *x, double *y)
{
	/*This function swaps two numbers*/
	double temp;
	temp = *x;
	*x = *y;
	*y = temp;
} /*GM_Swap*/

void GM_SwapRows(GMtype_Matrix *Matrix, int Row1, int Row2)
{
	int i;
	double a;
	for(i = 0; i < Matrix->columns; i++)
	{	
		a = Matrix->element[Row1][i];
		Matrix->element[Row1][i] = Matrix->element[Row2][i];
		Matrix->element[Row2][i] = a;
	}
} /*GM_SwapRows*/






