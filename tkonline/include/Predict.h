#ifndef PREDICT_H
#define PREDICT_H

struct QTH {
  char callsign[17];
  double stnlat;
  double stnlong;
  int stnalt;
};

struct	SAT {
  char line1[70];
  char line2[70];
  char name[25];
  long catnum;
  long setnum;
  char designator[10];
  int year;
  double refepoch;
  double incl;
  double raan;
  double eccn;
  double argper;
  double meanan;
  double meanmo;
  double drag;
  double nddot6;
  double bstar;
  long orbitnum;
};

struct	SAT_DB {
  char name[25];
  long catnum;
  char squintflag;
  double alat;
  double alon;
  unsigned char transponders;
  char transponder_name[10][80];
  double uplink_start[10];
  double uplink_end[10];
  double downlink_start[10];
  double downlink_end[10];
  unsigned char dayofweek[10];
  int phase_start[10];
  int phase_end[10];
};

struct GeoCoo{
  double Lat;
  double Lon;
};

/* Two-line-element satellite orbital data
   structure used directly by the SGP4/SDP4 code. */
typedef struct {
  double  epoch, xndt2o, xndd6o, bstar, xincl,
    xnodeo, eo, omegao, xmo, xno;
  int	   catnr, elset, revnum;
  char	   sat_name[25], idesg[9];
}  tle_t;

// /* General three-dimensional vector structure used by SGP4/SDP4 code. */
typedef struct {
  double x, y, z, w;
}  vector_t;

/* Geodetic position structure used by SGP4/SDP4 code. */
typedef struct	{
  double lat, lon, alt, theta;
}  geodetic_t;

typedef struct	{
  /* Used by dpinit part of Deep() */
  double  eosq, sinio, cosio, betao, aodp, theta2,
    sing, cosg, betao2, xmdot, omgdot, xnodot, xnodp;
  
  /* Used by dpsec and dpper parts of Deep() */
  double  xll, omgadf, xnode, em, xinc, xn, t;
  
  /* Used by thetg and Deep() */
  double  ds50;
}  deep_arg_t;

void ISSPosition(double time, GeoCoo& pos);
char CopyFile(char* source, char* destination);
char ReadDataFiles();
char Decayed(int x, double time);
char KepCheck(char* line1, char* line2);
void InternalUpdate(int x);
double CurrentDaynum();
long DayNum(int m, int d, int y);
char *SubString(char* string, char start, char end);
void SingleTrack(int x, char speak, double time, GeoCoo& pos);
void PreCalc(int x);
char Geostationary(int x);
char *Abbreviate(char* string,int n);
char *Daynum2String(double daynum);
void Calc();
void TrackDataOut(int antfd, double elevation, double azimuth);
void FindSun(double daynum);
void ClearFlag(int flag);
void select_ephemeris(tle_t *tle);
double Julian_Date_of_Epoch(double epoch);
double Julian_Date_of_Year(double year);
void SetFlag(int flag);
int isFlagSet(int flag);
void SDP4(double tsince, tle_t * tle, vector_t * pos, vector_t * vel);
void SGP4(double tsince, tle_t * tle, vector_t * pos, vector_t * vel);
void Convert_Sat_State(vector_t *pos, vector_t *vel);
void Magnitude(vector_t *v);
void Calculate_Obs(double time, vector_t *pos, vector_t *vel, geodetic_t *geodetic, vector_t *obs_set);
void Calculate_LatLonAlt(double time, vector_t *pos,  geodetic_t *geodetic);
void Calculate_Solar_Position(double time, vector_t *solar_vector);
int Sat_Eclipsed(vector_t *pos, vector_t *sol, double *depth);
double Degrees(double arg);
double Radians(double arg);
double FixAngle( double x);
void bailout(char* string);
void Calculate_RADec(double time, vector_t *pos, vector_t *vel, geodetic_t *geodetic, vector_t *obs_set);
int isFlagClear(int flag);
void Deep(int ientry, tle_t * tle, deep_arg_t * deep_arg);
double FMod2p(double x);
double AcTan(double sinx, double cosx);
void Scale_Vector(double k, vector_t *v);
double Sqr(double arg);
void Calculate_User_PosVel(double time, geodetic_t *geodetic, vector_t *obs_pos, vector_t *obs_vel);
double ArcSin(double arg);
double Dot(vector_t *v1, vector_t *v2);
double Delta_ET(double year);
double Modulus(double arg1, double arg2);
void Vec_Sub(vector_t *v1, vector_t *v2, vector_t *v3);
void Scalar_Multiply(double k, vector_t *v1, vector_t *v2);
double Angle(vector_t *v1, vector_t *v2);
double ThetaG(double epoch, deep_arg_t *deep_arg);
double ThetaG_JD(double jd);
int Sign(double arg);
double ArcCos(double arg);
double Frac(double arg);

#endif
