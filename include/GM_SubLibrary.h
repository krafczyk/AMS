//--------------------------------------------------------
//    
//   Library Functions to Calculate Stoermer Rigidity cutoff
//   http://www.ngdc.noaa.gov/geomag/geom_util/gmpole.shtml
//   Filename: GM_SubLibrary.h 
//   Creation date: 05/11/2012      
//
//   Implemented by : D. Grandi
//                    C. Consolandi
//                    S. Della Torre
//                    M. Tacconi
//
//--------------------------------------------------------

#ifndef GM_SubLibrary_H
#define GM_SubLibrary_H
#endif

#ifndef M_PI
#define M_PI	((2)*(acos(0.0)))
#endif

#define GM_STARTYEAR 	1900
#define RAD2DEG(rad)	((rad)*(180.0L/M_PI))
#define DEG2RAD(deg)	((deg)*(M_PI/180.0L))
#define ATanH(x)	(0.5 * log((1 + x) / (1 - x)))
#define MU_0		4*M_PI / 10000000
#define R_e		6.371 * 1000000



typedef struct {
			int Day;
			int Month;
			int Year;
			double DecimalYear;
			int DayNumber;
			} GMtype_Date;

typedef struct {
			double lambda;// longitude
			double phi; // geodetic latitude
			double HeightAboveEllipsoid; // height above the ellipsoid (HaE)
			} GMtype_CoordGeodetic;

typedef struct {
			double r; //radial distance
			double phig; //latitude
			double lambda; //longitude
			} GMtype_CoordSpherical;
			
typedef struct {
			double lambda;
			double phi;
			} GMtype_CoordDipole;

typedef struct {
			double x;
			double y;
			double z;
			} GMtype_CoordCartesian;

typedef struct {
			double a; /*semi-major axis of the ellipsoid*/
			double b; /*semi-minor axis of the ellipsoid*/
			double fla; /* flattening */
			double epssq; /*first eccentricity squared */
			double eps; /* first eccentricity */
			double re;/* mean radius of  ellipsoid*/
			} GMtype_Ellipsoid;

typedef struct {
			int degree;
			double coef[30];
			} GMtype_Polynomial;

typedef struct {
			double h11;
			double g11;
			double g10;
			double h21;
			double g21;
			double g20;
			double h22;
			double g22;			
			} GMtype_Model;

typedef struct {
			double M; /*Magnetic Moment*/
			double phi;
			double lambda;
			} GMtype_Pole;
			
typedef struct{
			int rows;
			int columns;
			double element[30][30];/*rows then columns*/
			} GMtype_Matrix;

typedef struct{
			int size;
			double element[30];
			} GMtype_Data;


// -----------  GMPOLE_last--> Geomagnetic Cut off with IGRF pos ==0 negative  pos=1 possitive
double GeoMagCutoff(time_t Utime, double Altitude , double thetaPart, double phiPart, double thetaISS, double phiISS, int pos );


//GM Cord functions
void GM_CartesianToSpherical(GMtype_CoordCartesian CoordCartesian, GMtype_CoordSpherical *CoordSpherical);
void GM_CORD (GMtype_CoordGeodetic location, GMtype_Date *date, GMtype_Ellipsoid Ellip, GMtype_Data g10d, GMtype_Data g11d, GMtype_Data h11d, GMtype_Data g20d, GMtype_Data g21d,
GMtype_Data h21d, GMtype_Data g22d, GMtype_Data h22d, GMtype_CoordDipole *CoordDipole, GMtype_CoordSpherical CoordInput);//...GTOD thetaISS,phiISS,coordinates and altitude of ISS
int GM_DateToYear(GMtype_Date *Date);
void GM_EarthCartToDipoleCartCD(GMtype_Pole Pole, GMtype_CoordCartesian EarthCoord, GMtype_CoordCartesian *DipoleCoords);
void GM_GeodeticToSpherical(GMtype_Ellipsoid Ellip, GMtype_CoordGeodetic CoordGeodetic, GMtype_CoordSpherical *CoordSpherical);
void GM_GetUserInput(GMtype_CoordGeodetic *location, GMtype_Date *date);
void GM_PoleLocation(GMtype_Model Model, GMtype_Pole *Pole);
void GM_DipoleLocation(GMtype_Model Model, GMtype_CoordSpherical *DipoleSpherical); 			/*Added for pole position*/
void GM_GeomagCoordLocation(GMtype_CoordSpherical CoordInput, GMtype_CoordSpherical DipoleSpherical, GMtype_Pole Pole, GMtype_CoordSpherical *MagSpherical); 	/*Added for geomag coord*/
void  GM_GeomagCoordDirection(GMtype_CoordSpherical GeoDir, GMtype_CoordSpherical DipoleSpherical, GMtype_Pole Pole, GMtype_CoordDipole *MagDir);
void GM_PrintUserData(GMtype_CoordGeodetic location, GMtype_Date date, GMtype_CoordDipole DipLocation);
void GM_ScanIGRF(GMtype_Data *G10, GMtype_Data *G11, GMtype_Data *H11, GMtype_Data *G20, GMtype_Data *G21, GMtype_Data *H21, GMtype_Data *G22, GMtype_Data *H22 );
void GM_SetEllipsoid(GMtype_Ellipsoid *Ellip);
void GM_SphericalToCartesian(GMtype_CoordSpherical CoordSpherical, GMtype_CoordCartesian *CoordCartesian);
void GM_TimeAdjustCoefs(GMtype_Date Date, GMtype_Data g10d, GMtype_Data g11d, GMtype_Data h11d, GMtype_Data g20d, GMtype_Data g21d, GMtype_Data h21d, GMtype_Data g22d,
GMtype_Data h22d, GMtype_Model *Model);

//Math Functions
double GM_DotProduct(GMtype_Data VectorA, GMtype_Data VectorB);
double GM_LinearInterpolation(double x1, double x2, double y1, double y2, double x);
void GM_LUDecomposition(GMtype_Matrix A, GMtype_Matrix *L, GMtype_Matrix *U, GMtype_Matrix *P);
void GM_LUSolve(GMtype_Matrix L, GMtype_Matrix U, GMtype_Matrix P, GMtype_Matrix *x, GMtype_Matrix b);
double GM_MatDet(GMtype_Matrix Matrix);
void GM_MatMultiply(GMtype_Matrix MatrixA, GMtype_Matrix MatrixB, GMtype_Matrix *MatrixC);
void GM_MatInverse(GMtype_Matrix Matrix, GMtype_Matrix *InvertedMatrix);
void GM_MatTranspose(GMtype_Matrix Matrix, GMtype_Matrix *TMatrix);
double GM_Mean(GMtype_Data Data);
void GM_Median(GMtype_Data Data, double *upper, double *lower);
void GM_PolyFit(GMtype_Data DataX, GMtype_Data DataY, GMtype_Polynomial *Polynomial);
double GM_Pow(double x, int y);
void GM_PrintMatrix(GMtype_Matrix X);
double GM_SolvePolynomial(GMtype_Polynomial Polynomial, double x);
void GM_Sort(GMtype_Data *Data);
double GM_StandardDeviation(GMtype_Data Data);
void GM_Swap(double *x, double *y);
void GM_SwapRows(GMtype_Matrix *Matrix, int Row1, int Row2);
