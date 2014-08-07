//  $Id$

#include "GeoMagField.h"
#include "TSystem.h"     // For ExpandPathName
#include <iostream>

/// Singleton pointer (protected)
GeoMagField *GeoMagField::fHead = 0;

/// Model file name
TString GeoMagField::fModelFile = "$AMSDataDir/v5.00/IGRF11.COF";

/// Model time in year
double GeoMagField::fModelYear = 2012;

/// Status of the InitModel 0:Not yet, 1:Done, -1:Error
int GeoMagField::fInitStat = 0;

GeoMagField::GeoMagField()
{
  fHead = this;
  Reset();
}

GeoMagField *GeoMagField::GetHead()
{
  if (!fHead) {
    fHead = new GeoMagField;
    fHead->InitModel();
  }
  return fHead;
}

int GeoMagField::InitModel(const char *mdfile, double date)
{
  if (fInitStat != 0) return fInitStat;

  if (fModelFile.Contains("$"))
    fModelFile = gSystem->ExpandPathName(fModelFile.Data());

  if (!mdfile) mdfile = fModelFile.Data();

  int ret = readmdf(mdfile);
  if (ret < 0) {
    fInitStat = -1;
    return ret;
  }

  sdate = date;

  if (max2[modelI] == 0) {
    getshc(mdfile, 1,     irec_pos[modelI],   max1[modelI],   1);
    getshc(mdfile, 1,     irec_pos[modelI+1], max1[modelI+1], 2);
    nmax = interpsh(sdate,   yrmin[modelI],   max1[modelI],
		             yrmin[modelI+1], max1[modelI+1], 3);
    nmax = interpsh(sdate+1, yrmin[modelI] ,  max1[modelI],
                             yrmin[modelI+1], max1[modelI+1], 4);
  } 
  else {
    getshc(mdfile, 1,     irec_pos[modelI], max1[modelI], 1);
    getshc(mdfile, 0,     irec_pos[modelI], max2[modelI], 2);
    nmax = extrapsh(sdate,   epoch[modelI], max1[modelI], max2[modelI], 3);
    nmax = extrapsh(sdate+1, epoch[modelI], max1[modelI], max2[modelI], 4);
  }

  static bool first = true;
  if (first) {
#pragma omp critical (initmodel)
  {if (first) {
    std::cout << "GeoMagField::InitModel-I-Read model from : "
	      << mdfile << " N= "
	      << nmodel << " " << max2[modelI] << " " << nmax << " "
	      << "Year= " << sdate << std::endl;
    first = 0;
   }
  }}

  Reset();

  fInitStat = 1;
  return 0;
}

void GeoMagField::Reset()
{
  reset();
}

int GeoMagField::Calc(double lat, double lng, double alt, int igdgc)
{
  reset();
  return calcxyz(igdgc, lat, lng, alt);
}

int GeoMagField::CalcAll(double lat, double lng, double alt, int igdgc)
{
  reset();
  calcall(igdgc, lat, lng, alt);

  if (warn_P || warn_H || warn_H_strong) {
#pragma omp critical (warnmsg)
    std::cout << "warn= " << warn_P << " " << warn_H << " " << warn_H_strong 
	      << std::endl;
    return 1;
  }

  return 0;
}

void GeoMagField::Print(Option_t *option) const
{

  if (dec != 0 && inc != 0 && hol != 0 && tot != 0) {
    print_header();
    print_result(sdate, dec, inc, hol, xcmp, ycmp, zcmp, tot);
  }
  else {
    print_header_cp();
    print_result_cp(sdate, xcmp, ycmp, zcmp);
  }

  if (ddot != 0 && idot != 0 && hdot != 0 &&
      xdot != 0 && ydot != 0 && zdot != 0 && fdot != 0) {
    print_long_dashed_line();
    print_header_sv();
    print_result_sv(sdate,ddot,idot,hdot,xdot,ydot,zdot,fdot);
  }
  print_dashed_line();
}


#include <cstdio>
#include <cmath>

int GeoMagField::readmdf(const char *mdfile)
{
  FILE *stream = NULL;

  if (!(stream = fopen(mdfile, "rt"))) {
    printf("\nError opening file %s.", mdfile);
    return -1;
  }

  rewind(stream);

  enum { RECL = 81, MAXINBUFF = RECL+14,
	            MAXREAD   = MAXINBUFF-2 };
  char inbuff[MAXINBUFF];
  char  model[MAXMOD][9];          

  int fileline = 0;                        /* First line will be 1 */
  modelI = -1;                             /* First model will be 0 */
  while (fgets(inbuff,MAXREAD,stream))     /* While not end of file
					    * read to end of line or buffer */
  {
    fileline++;                            /* On new line */

    if (strlen(inbuff) != RECL)       /* IF incorrect record size */
    {
      printf("Corrupt record in file %s on line %d.\n", mdfile, fileline);
      fclose(stream);
      return -5;
    }

    /* New statement Dec 1999 changed by wmd  required by year 2000 models */
    if (!strncmp(inbuff,"   ",3))         /* If 1st 3 chars are spaces */
    {
      modelI++;                           /* New model */

      if (modelI > MAXMOD)                /* If too many headers */
      {
	printf("Too many models in file %s on line %d.", mdfile, fileline);
	fclose(stream);
	return -6;
      }

      irec_pos[modelI]=ftell(stream);
      /* Get fields from buffer into individual vars.  */
      sscanf(inbuff, "%s%lg%d%d%d%lg%lg%lg%lg", model[modelI], &epoch[modelI],
	     &max1[modelI], &max2[modelI], &max3[modelI], &yrmin[modelI],
	     &yrmax[modelI], &altmin[modelI], &altmax[modelI]);

      /* Compute date range for all models */
      if (modelI == 0)                    /*If first model */
      {
	minyr=yrmin[0];
	maxyr=yrmax[0];
      } 
      else 
      {
	if (yrmin[modelI]<minyr)
	{
	  minyr=yrmin[modelI];
	}
	if (yrmax[modelI]>maxyr){
	  maxyr=yrmax[modelI];
	}
      } /* if modelI != 0 */
      
    } /* If 1st 3 chars are spaces */
    
  } /* While not end of model file */

  nmodel = modelI + 1;
  fclose(stream);

  return 0;
}

void GeoMagField::reset()
{
  inc   = dec   = tot   = hol   = 0;
  xcmp  = ycmp  = zcmp  = 0;
  xtemp = ytemp = ztemp = 0;
  dtemp = ftemp = htemp = itemp = 0;
  ddot  = idot  = fdot  = hdot  = 0;
  xdot  = ydot  = zdot  = 0;
}

double GeoMagField::getf()
{
  if (tot == 0) tot = sqrt(xcmp*xcmp+ycmp*ycmp+zcmp*zcmp);
  return tot;
}

double GeoMagField::geth()
{
  if (hol == 0) hol = sqrt(xcmp*xcmp+ycmp*ycmp);
  return hol;
}

int GeoMagField::calcall(int igdgc, double latitude, double longitude,
			            double alt)
{
  enum { IEXT = 0 };
  const double EXT_COEFF1 = 0;
  const double EXT_COEFF2 = 0;
  const double EXT_COEFF3 = 0;

        double NaN     = log(-1.0);
  const double PI      = 3.141592654;
  const double RAD2DEG = (180.0/PI);

  /* Do the first calculations */
  shval3(igdgc, latitude, longitude, alt, nmax, 3,
	 IEXT, EXT_COEFF1, EXT_COEFF2, EXT_COEFF3);
  dihf(3);
  shval3(igdgc, latitude, longitude, alt, nmax, 4,
	 IEXT, EXT_COEFF1, EXT_COEFF2, EXT_COEFF3);
  dihf(4);

  double d = dec,  i = inc;
  double h = hol,  f = tot;
  double x = xcmp, y = ycmp, z = zcmp;

  ddot = ((dtemp - d)*RAD2DEG);
  if (ddot > 180.0) ddot -= 360.0;
  if (ddot <= -180.0) ddot += 360.0;
  ddot *= 60.0;
      
  idot = ((itemp - i)*RAD2DEG)*60;
  d = d*(RAD2DEG);   i = i*(RAD2DEG);
  hdot = htemp - h;   xdot = xtemp - x;
  ydot = ytemp - y;   zdot = ztemp - z;
  fdot = ftemp - f;

  /* deal with geographic and magnetic poles */

  warn_H = 0;
  warn_H_val = 99999.0;
  warn_H_strong = 0;
  warn_H_strong_val = 99999.0;
  warn_P = 0;
      
  if (h < 100.0) /* at magnetic poles */
  {
    d = NaN;
    ddot = NaN;
    /* while rest is ok */
  }
      
  if (h < 1000.0) 
  {
    warn_H = 0;
    warn_H_strong = 1;
    if (h<warn_H_strong_val) warn_H_strong_val = h;
  }
  else if (h < 5000.0 && !warn_H_strong) 
  {
    warn_H = 1;
    if (h<warn_H_val) warn_H_val = h;
  }
      
  if (90.0-fabs(latitude) <= 0.001) /* at geographic poles */
  {
    x = NaN;
    y = NaN;
    d = NaN;
    xdot = NaN;
    ydot = NaN;
    ddot = NaN;
    warn_P = 1;
    warn_H = 0;
    warn_H_strong = 0;
    /* while rest is ok */
  }

  dec  = d;
  inc  = i;
  hol  = h;
  tot  = f;
  xcmp = x;
  ycmp = y;
  zcmp = z;
      
  return 0;
}


int GeoMagField::calcxyz(int igdgc, double latitude, double longitude,
			            double alt)
{
  return shval3(igdgc, latitude, longitude, alt, nmax, 3, 0, 0, 0, 0);
}

/****************************************************************************/
/*                                                                          */
/*                           Subroutine julday                              */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*     Computes the decimal day of year from month, day, year.              */
/*     Supplied by Daniel Bergstrom                                         */
/*                                                                          */
/* References:                                                              */
/*                                                                          */
/* 1. Nachum Dershowitz and Edward M. Reingold, Calendrical Calculations,   */
/*    Cambridge University Press, 3rd edition, ISBN 978-0-521-88540-9.      */
/*                                                                          */
/* 2. Claus Tøndering, Frequently Asked Questions about Calendars,          */
/*    Version 2.9, http://www.tondering.dk/claus/calendar.html              */
/*                                                                          */
/****************************************************************************/

double GeoMagField::julday(int month, int day, int year)
{
  int days[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

  int leap_year = (((year % 4) == 0) &&
                   (((year % 100) != 0) || ((year % 400) == 0)));

  double day_in_year = (days[month - 1] + day + (month > 2 ? leap_year : 0));

  return ((double)year + (day_in_year / (365.0 + leap_year)));
}


/****************************************************************************/
/*                                                                          */
/*                           Subroutine getshc                              */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*     Reads spherical harmonic coefficients from the specified             */
/*     model into an array.                                                 */
/*                                                                          */
/*     Input:                                                               */
/*           stream     - Logical unit number                               */
/*           iflag      - Flag for SV equal to ) or not equal to 0          */
/*                        for designated read statements                    */
/*           strec      - Starting record number to read from model         */
/*           nmax_of_gh - Maximum degree and order of model                 */
/*                                                                          */
/*     Output:                                                              */
/*           gh1 or 2   - Schmidt quasi-normal internal spherical           */
/*                        harmonic coefficients                             */
/*                                                                          */
/*     FORTRAN                                                              */
/*           Bill Flanagan                                                  */
/*           NOAA CORPS, DESDIS, NGDC, 325 Broadway, Boulder CO.  80301     */
/*                                                                          */
/*     C                                                                    */
/*           C. H. Shaffer                                                  */
/*           Lockheed Missiles and Space Company, Sunnyvale CA              */
/*           August 15, 1988                                                */
/*                                                                          */
/****************************************************************************/

int GeoMagField::getshc(const char *file, int iflag, long int strec, 
		   int nmax_of_gh, int gh)
{
  enum { RECL = 81, MAXINBUFF = RECL+14,
	            MAXREAD   = MAXINBUFF-2 };

  char  inbuff[MAXINBUFF];
  char irat[9];
  int ii = 0,m,n,mm,nn;
  int ios = 0;
  int line_num;
  double g,hh;
  double trash;

  FILE *stream = NULL;
  stream = fopen(file, "rt");
  if (stream == NULL)
    {
      printf("\nError on opening file %s", file);
    }
  else
    {
      fseek(stream,strec,SEEK_SET);
      for ( nn = 1; nn <= nmax_of_gh; ++nn)
        {
          for (mm = 0; mm <= nn; ++mm)
            {
              if (iflag == 1)
                {
                  fgets(inbuff, MAXREAD, stream);
                  sscanf(inbuff, "%d%d%lg%lg%lg%lg%s%d",
                         &n, &m, &g, &hh, &trash, &trash, irat, &line_num);
                }
              else
                {
                  fgets(inbuff, MAXREAD, stream);
                  sscanf(inbuff, "%d%d%lg%lg%lg%lg%s%d",
                         &n, &m, &trash, &trash, &g, &hh, irat, &line_num);
                }
              if ((nn != n) || (mm != m))
                {
                  ios = -2;
                  fclose(stream);
                  return(ios);
                }
              ii = ii + 1;
              switch(gh)
                {
                case 1:  gh1[ii] = g;
                  break;
                case 2:  gh2[ii] = g;
                  break;
                default: printf("\nError in subroutine getshc");
                  break;
                }
              if (m != 0)
                {
                  ii = ii+ 1;
                  switch(gh)
                    {
                    case 1:  gh1[ii] = hh;
                      break;
                    case 2:  gh2[ii] = hh;
                      break;
                    default: printf("\nError in subroutine getshc");
                      break;
                    }
                }
            }
        }
    }
  fclose(stream);
  return(ios);
}


/****************************************************************************/
/*                                                                          */
/*                           Subroutine extrapsh                            */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*     Extrapolates linearly a spherical harmonic model with a              */
/*     rate-of-change model.                                                */
/*                                                                          */
/*     Input:                                                               */
/*           date     - date of resulting model (in decimal year)           */
/*           dte1     - date of base model                                  */
/*           nmax1    - maximum degree and order of base model              */
/*           gh1      - Schmidt quasi-normal internal spherical             */
/*                      harmonic coefficients of base model                 */
/*           nmax2    - maximum degree and order of rate-of-change model    */
/*           gh2      - Schmidt quasi-normal internal spherical             */
/*                      harmonic coefficients of rate-of-change model       */
/*                                                                          */
/*     Output:                                                              */
/*           gha or b - Schmidt quasi-normal internal spherical             */
/*                    harmonic coefficients                                 */
/*           nmax   - maximum degree and order of resulting model           */
/*                                                                          */
/*     FORTRAN                                                              */
/*           A. Zunde                                                       */
/*           USGS, MS 964, box 25046 Federal Center, Denver, CO.  80225     */
/*                                                                          */
/*     C                                                                    */
/*           C. H. Shaffer                                                  */
/*           Lockheed Missiles and Space Company, Sunnyvale CA              */
/*           August 16, 1988                                                */
/*                                                                          */
/****************************************************************************/

int GeoMagField::extrapsh(double date, double dte1, int nmax1,
			                            int nmax2, int gh)
{
  int   nmax;
  int   k, l;
  int   ii;
  double factor;
  
  factor = date - dte1;
  if (nmax1 == nmax2)
    {
      k =  nmax1 * (nmax1 + 2);
      nmax = nmax1;
    }
  else
    {
      if (nmax1 > nmax2)
        {
          k = nmax2 * (nmax2 + 2);
          l = nmax1 * (nmax1 + 2);
          switch(gh)
            {
            case 3:  for ( ii = k + 1; ii <= l; ++ii)
                {
                  gha[ii] = gh1[ii];
                }
              break;
            case 4:  for ( ii = k + 1; ii <= l; ++ii)
                {
                  ghb[ii] = gh1[ii];
                }
              break;
            default: printf("\nError in subroutine extrapsh");
              break;
            }
          nmax = nmax1;
        }
      else
        {
          k = nmax1 * (nmax1 + 2);
          l = nmax2 * (nmax2 + 2);
          switch(gh)
            {
            case 3:  for ( ii = k + 1; ii <= l; ++ii)
                {
                  gha[ii] = factor * gh2[ii];
                }
              break;
            case 4:  for ( ii = k + 1; ii <= l; ++ii)
                {
                  ghb[ii] = factor * gh2[ii];
                }
              break;
            default: printf("\nError in subroutine extrapsh");
              break;
            }
          nmax = nmax2;
        }
    }
  switch(gh)
    {
    case 3:  for ( ii = 1; ii <= k; ++ii)
        {
          gha[ii] = gh1[ii] + factor * gh2[ii];
        }
      break;
    case 4:  for ( ii = 1; ii <= k; ++ii)
        {
          ghb[ii] = gh1[ii] + factor * gh2[ii];
        }
      break;
    default: printf("\nError in subroutine extrapsh");
      break;
    }
  return(nmax);
}


/****************************************************************************/
/*                                                                          */
/*                           Subroutine interpsh                            */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*     Interpolates linearly, in time, between two spherical harmonic       */
/*     models.                                                              */
/*                                                                          */
/*     Input:                                                               */
/*           date     - date of resulting model (in decimal year)           */
/*           dte1     - date of earlier model                               */
/*           nmax1    - maximum degree and order of earlier model           */
/*           gh1      - Schmidt quasi-normal internal spherical             */
/*                      harmonic coefficients of earlier model              */
/*           dte2     - date of later model                                 */
/*           nmax2    - maximum degree and order of later model             */
/*           gh2      - Schmidt quasi-normal internal spherical             */
/*                      harmonic coefficients of internal model             */
/*                                                                          */
/*     Output:                                                              */
/*           gha or b - coefficients of resulting model                     */
/*           nmax     - maximum degree and order of resulting model         */
/*                                                                          */
/*     FORTRAN                                                              */
/*           A. Zunde                                                       */
/*           USGS, MS 964, box 25046 Federal Center, Denver, CO.  80225     */
/*                                                                          */
/*     C                                                                    */
/*           C. H. Shaffer                                                  */
/*           Lockheed Missiles and Space Company, Sunnyvale CA              */
/*           August 17, 1988                                                */
/*                                                                          */
/****************************************************************************/

int GeoMagField::interpsh(double date, double dte1, int nmax1,
		                       double dte2, int nmax2, int gh)
{
  int   nmax;
  int   k, l;
  int   ii;
  double factor;
  
  factor = (date - dte1) / (dte2 - dte1);
  if (nmax1 == nmax2)
    {
      k =  nmax1 * (nmax1 + 2);
      nmax = nmax1;
    }
  else
    {
      if (nmax1 > nmax2)
        {
          k = nmax2 * (nmax2 + 2);
          l = nmax1 * (nmax1 + 2);
          switch(gh)
            {
            case 3:  for ( ii = k + 1; ii <= l; ++ii)
                {
                  gha[ii] = gh1[ii] + factor * (-gh1[ii]);
                }
              break;
            case 4:  for ( ii = k + 1; ii <= l; ++ii)
                {
                  ghb[ii] = gh1[ii] + factor * (-gh1[ii]);
                }
              break;
            default: printf("\nError in subroutine extrapsh");
              break;
            }
          nmax = nmax1;
        }
      else
        {
          k = nmax1 * (nmax1 + 2);
          l = nmax2 * (nmax2 + 2);
          switch(gh)
            {
            case 3:  for ( ii = k + 1; ii <= l; ++ii)
                {
                  gha[ii] = factor * gh2[ii];
                }
              break;
            case 4:  for ( ii = k + 1; ii <= l; ++ii)
                {
                  ghb[ii] = factor * gh2[ii];
                }
              break;
            default: printf("\nError in subroutine extrapsh");
              break;
            }
          nmax = nmax2;
        }
    }
  switch(gh)
    {
    case 3:  for ( ii = 1; ii <= k; ++ii)
        {
          gha[ii] = gh1[ii] + factor * (gh2[ii] - gh1[ii]);
        }
      break;
    case 4:  for ( ii = 1; ii <= k; ++ii)
        {
          ghb[ii] = gh1[ii] + factor * (gh2[ii] - gh1[ii]);
        }
      break;
    default: printf("\nError in subroutine extrapsh");
      break;
    }
  return(nmax);
}


/****************************************************************************/
/*                                                                          */
/*                           Subroutine shval3                              */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*     Calculates field components from spherical harmonic (sh)             */
/*     models.                                                              */
/*                                                                          */
/*     Input:                                                               */
/*           igdgc     - indicates coordinate system used; set equal        */
/*                       to 1 if geodetic, 2 if geocentric                  */
/*           latitude  - north latitude, in degrees                         */
/*           longitude - east longitude, in degrees                         */
/*           elev      - WGS84 altitude above ellipsoid (igdgc=1), or       */
/*                       radial distance from earth's center (igdgc=2)      */
/*           a2,b2     - squares of semi-major and semi-minor axes of       */
/*                       the reference spheroid used for transforming       */
/*                       between geodetic and geocentric coordinates        */
/*                       or components                                      */
/*           nmax      - maximum degree and order of coefficients           */
/*           iext      - external coefficients flag (=0 if none)            */
/*           ext1,2,3  - the three 1st-degree external coefficients         */
/*                       (not used if iext = 0)                             */
/*                                                                          */
/*     Output:                                                              */
/*           x         - northward component                                */
/*           y         - eastward component                                 */
/*           z         - vertically-downward component                      */
/*                                                                          */
/*     based on subroutine 'igrf' by D. R. Barraclough and S. R. C. Malin,  */
/*     report no. 71/1, institute of geological sciences, U.K.              */
/*                                                                          */
/*     FORTRAN                                                              */
/*           Norman W. Peddie                                               */
/*           USGS, MS 964, box 25046 Federal Center, Denver, CO.  80225     */
/*                                                                          */
/*     C                                                                    */
/*           C. H. Shaffer                                                  */
/*           Lockheed Missiles and Space Company, Sunnyvale CA              */
/*           August 17, 1988                                                */
/*                                                                          */
/****************************************************************************/

int GeoMagField::shval3(int igdgc, double flat, double flon, double elev,
			int nmax, int gh, int iext,
			double ext1, double ext2, double ext3)
{
  double x = xcmp, y = ycmp, z = zcmp;

  double earths_radius = 6371.2;
  double dtr = 0.01745329;
  double slat;
  double clat;
  double ratio;
  double aa, bb, cc, dd;
  double sd;
  double cd;
  double r;
  double a2;
  double b2;
  double rr=0;
  double fm,fn=0;
  double sl[14];
  double cl[14];
  double p[119];
  double q[119];
  int ii,j,k,l,m,n;
  int npq;
  int ios;
  double argument;
  double power;
  a2 = 40680631.59;            /* WGS84 */
  b2 = 40408299.98;            /* WGS84 */
  ios = 0;
  r = elev;
  argument = flat * dtr;
  slat = sin( argument );
  if ((90.0 - flat) < 0.001)
    {
      aa = 89.999;            /*  300 ft. from North pole  */
    }
  else
    {
      if ((90.0 + flat) < 0.001)
        {
          aa = -89.999;        /*  300 ft. from South pole  */
        }
      else
        {
          aa = flat;
        }
    }
  argument = aa * dtr;
  clat = cos( argument );
  argument = flon * dtr;
  sl[1] = sin( argument );
  cl[1] = cos( argument );
  switch(gh)
    {
    case 3:  x = 0;
      y = 0;
      z = 0;
      break;
    case 4:  xtemp = 0;
      ytemp = 0;
      ztemp = 0;
      break;
    default: printf("\nError in subroutine shval3");
      break;
    }
  sd = 0.0;
  cd = 1.0;
  l = 1;
  n = 0;
  m = 1;
  npq = (nmax * (nmax + 3)) / 2;
  if (igdgc == 1)
    {
      aa = a2 * clat * clat;
      bb = b2 * slat * slat;
      cc = aa + bb;
      argument = cc;
      dd = sqrt( argument );
      argument = elev * (elev + 2.0 * dd) + (a2 * aa + b2 * bb) / cc;
      r = sqrt( argument );
      cd = (elev + dd) / r;
      sd = (a2 - b2) / dd * slat * clat / r;
      aa = slat;
      slat = slat * cd - clat * sd;
      clat = clat * cd + aa * sd;
    }
  ratio = earths_radius / r;
  argument = 3.0;
  aa = sqrt( argument );
  p[1] = 2.0 * slat;
  p[2] = 2.0 * clat;
  p[3] = 4.5 * slat * slat - 1.5;
  p[4] = 3.0 * aa * clat * slat;
  q[1] = -clat;
  q[2] = slat;
  q[3] = -3.0 * clat * slat;
  q[4] = aa * (slat * slat - clat * clat);
  for ( k = 1; k <= npq; ++k)
    {
      if (n < m)
        {
          m = 0;
          n = n + 1;
          argument = ratio;
          power =  n + 2;
          rr = pow(argument,power);
          fn = n;
        }
      fm = m;
      if (k >= 5)
        {
          if (m == n)
            {
              argument = (1.0 - 0.5/fm);
              aa = sqrt( argument );
              j = k - n - 1;
              p[k] = (1.0 + 1.0/fm) * aa * clat * p[j];
              q[k] = aa * (clat * q[j] + slat/fm * p[j]);
              sl[m] = sl[m-1] * cl[1] + cl[m-1] * sl[1];
              cl[m] = cl[m-1] * cl[1] - sl[m-1] * sl[1];
            }
          else
            {
              argument = fn*fn - fm*fm;
              aa = sqrt( argument );
              argument = ((fn - 1.0)*(fn-1.0)) - (fm * fm);
              bb = sqrt( argument )/aa;
              cc = (2.0 * fn - 1.0)/aa;
              ii = k - n;
              j = k - 2 * n + 1;
              p[k] = (fn + 1.0) * (cc * slat/fn * p[ii] - bb/(fn - 1.0) * p[j]);
              q[k] = cc * (slat * q[ii] - clat/fn * p[ii]) - bb * q[j];
            }
        }
      switch(gh)
        {
        case 3:  aa = rr * gha[l];
          break;
        case 4:  aa = rr * ghb[l];
          break;
        default: printf("\nError in subroutine shval3");
          break;
        }
      if (m == 0)
        {
          switch(gh)
            {
            case 3:  x = x + aa * q[k];
              z = z - aa * p[k];
              break;
            case 4:  xtemp = xtemp + aa * q[k];
              ztemp = ztemp - aa * p[k];
              break;
            default: printf("\nError in subroutine shval3");
              break;
            }
          l = l + 1;
        }
      else
        {
          switch(gh)
            {
            case 3:  bb = rr * gha[l+1];
              cc = aa * cl[m] + bb * sl[m];
              x = x + cc * q[k];
              z = z - cc * p[k];
              if (clat > 0)
                {
                  y = y + (aa * sl[m] - bb * cl[m]) *
                    fm * p[k]/((fn + 1.0) * clat);
                }
              else
                {
                  y = y + (aa * sl[m] - bb * cl[m]) * q[k] * slat;
                }
              l = l + 2;
              break;
            case 4:  bb = rr * ghb[l+1];
              cc = aa * cl[m] + bb * sl[m];
              xtemp = xtemp + cc * q[k];
              ztemp = ztemp - cc * p[k];
              if (clat > 0)
                {
                  ytemp = ytemp + (aa * sl[m] - bb * cl[m]) *
                    fm * p[k]/((fn + 1.0) * clat);
                }
              else
                {
                  ytemp = ytemp + (aa * sl[m] - bb * cl[m]) *
                    q[k] * slat;
                }
              l = l + 2;
              break;
            default: printf("\nError in subroutine shval3");
              break;
            }
        }
      m = m + 1;
    }
  if (iext != 0)
    {
      aa = ext2 * cl[1] + ext3 * sl[1];
      switch(gh)
        {
        case 3:   x = x - ext1 * clat + aa * slat;
          y = y + ext2 * sl[1] - ext3 * cl[1];
          z = z + ext1 * slat + aa * clat;
          break;
        case 4:   xtemp = xtemp - ext1 * clat + aa * slat;
          ytemp = ytemp + ext2 * sl[1] - ext3 * cl[1];
          ztemp = ztemp + ext1 * slat + aa * clat;
          break;
        default:  printf("\nError in subroutine shval3");
          break;
        }
    }
  switch(gh)
    {
    case 3:   aa = x;
		x = x * cd + z * sd;
		z = z * cd - aa * sd;
		break;
    case 4:   aa = xtemp;
		xtemp = xtemp * cd + ztemp * sd;
		ztemp = ztemp * cd - aa * sd;
		break;
    default:  printf("\nError in subroutine shval3");
		break;
    }

  xcmp = x;
  ycmp = y;
  zcmp = z;

  return(ios);
}

/****************************************************************************/
/*                                                                          */
/*                           Subroutine dihf                                */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*     Computes the geomagnetic d, i, h, and f from x, y, and z.            */
/*                                                                          */
/*     Input:                                                               */
/*           x  - northward component                                       */
/*           y  - eastward component                                        */
/*           z  - vertically-downward component                             */
/*                                                                          */
/*     Output:                                                              */
/*           d  - declination                                               */
/*           i  - inclination                                               */
/*           h  - horizontal intensity                                      */
/*           f  - total intensity                                           */
/*                                                                          */
/*     FORTRAN                                                              */
/*           A. Zunde                                                       */
/*           USGS, MS 964, box 25046 Federal Center, Denver, CO.  80225     */
/*                                                                          */
/*     C                                                                    */
/*           C. H. Shaffer                                                  */
/*           Lockheed Missiles and Space Company, Sunnyvale CA              */
/*           August 22, 1988                                                */
/*                                                                          */
/****************************************************************************/

int GeoMagField::dihf(int gh)
{
  double d = dec,  i = inc,  h = hol, f = tot;
  double x = xcmp, y = ycmp, z = zcmp;

  double NaN = log(-1.0);
  double PI  = 3.141592654;

  int ios;
  int j;
  double sn;
  double h2;
  double hpx;
  double argument, argument2;

  ios = gh;
  sn = 0.0001;
  
  switch(gh)
    {
    case 3:   for (j = 1; j <= 1; ++j)
        {
          h2 = x*x + y*y;
          argument = h2;
          h = sqrt(argument);       /* calculate horizontal intensity */
          argument = h2 + z*z;
          f = sqrt(argument);      /* calculate total intensity */
          if (f < sn)
            {
              d = NaN;        /* If d and i cannot be determined, */
              i = NaN;        /*       set equal to NaN         */
            }
          else
            {
              argument = z;
              argument2 = h;
              i = atan2(argument,argument2);
              if (h < sn)
                {
                  d = NaN;
                }
              else
                {
                  hpx = h + x;
                  if (hpx < sn)
                    {
                      d = PI;
                    }
                  else
                    {
                      argument = y;
                      argument2 = hpx;
                      d = 2.0 * atan2(argument,argument2);
                    }
                }
            }
        }
		break;
    case 4:   for (j = 1; j <= 1; ++j)
        {
          h2 = xtemp*xtemp + ytemp*ytemp;
          argument = h2;
          htemp = sqrt(argument);
          argument = h2 + ztemp*ztemp;
          ftemp = sqrt(argument);
          if (ftemp < sn)
            {
              dtemp = NaN;    /* If d and i cannot be determined, */
              itemp = NaN;    /*       set equal to 999.0         */
            }
          else
            {
              argument = ztemp;
              argument2 = htemp;
              itemp = atan2(argument,argument2);
              if (htemp < sn)
                {
                  dtemp = NaN;
                }
              else
                {
                  hpx = htemp + xtemp;
                  if (hpx < sn)
                    {
                      dtemp = PI;
                    }
                  else
                    {
                      argument = ytemp;
                      argument2 = hpx;
                      dtemp = 2.0 * atan2(argument,argument2);
                    }
                }
            }
        }
		break;
    default:  printf("\nError in subroutine dihf");
		break;
    }

  dec = d;
  inc = i;
  hol = h;
  tot = f;

  return(ios);
}

void GeoMagField::print_dashed_line() const
{
  printf(" -------------------------------------------------------------------------------\n");
}

void GeoMagField::print_long_dashed_line() const
{
  printf(" - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
}

void GeoMagField::print_header() const
{
  print_dashed_line();
  printf("   Date          D           I           H        X        Y        Z        F\n");
  printf("   (yr)      (deg min)   (deg min)     (nT)     (nT)     (nT)     (nT)     (nT)\n");
  return;
}

void GeoMagField::print_header_cp() const
{
  print_dashed_line();
  printf("   Date          X        Y        Z \n");
  printf("   (yr)        (nT)     (nT)     (nT)\n");
  return;
}

void GeoMagField::print_header_sv() const
{
  printf("   Date         dD          dI           dH       dX       dY       dZ       dF\n");
  printf("   (yr)      (min/yr)    (min/yr)    (nT/yr)  (nT/yr)  (nT/yr)  (nT/yr)  (nT/yr)\n");
}

void GeoMagField::print_result(double date, double d, double i, double h,
			                    double x, double y, double z,
			                    double f) const
{
  int   ddeg,ideg;
  double dmin,imin;

      /* Change d and i to deg and min */
      
  ddeg=(int)d;
  dmin=(d-(double)ddeg)*60;
  if (d > 0 && dmin >= 59.5)
    {
      dmin -= 60.0;
      ddeg++;
    }
  if (d < 0 && dmin <= -59.5)
    {
      dmin += 60.0;
      ddeg--;
    }

  if (ddeg!=0) dmin=fabs(dmin);
  ideg=(int)i;
  imin=(i-(double)ideg)*60;
  if (i > 0 && imin >= 59.5)
    {
      imin -= 60.0;
      ideg++;
    }
  if (i < 0 && imin <= -59.5)
    {
      imin += 60.0;
      ideg--;
    }
  if (ideg!=0) imin=fabs(imin);


  if (my_isnan(d))
    {
      if (my_isnan(x))
        printf("  %4.2f       NaN    %4dd %3.0fm  %8.1f      NaN      NaN %8.1f %8.1f\n",date,ideg,imin,h,z,f);
      else
        printf("  %4.2f       NaN    %4dd %3.0fm  %8.1f %8.1f %8.1f %8.1f %8.1f\n",date,ideg,imin,h,x,y,z,f);
    }
  else 
    printf("  %4.2f  %4dd %3.0fm  %4dd %3.0fm  %8.1f %8.1f %8.1f %8.1f %8.1f\n",date,ddeg,dmin,ideg,imin,h,x,y,z,f);
  return;
}

void GeoMagField::print_result_cp(double date,
				  double x, double y, double z) const
{
  printf("  %4.2f  %8.1f %8.1f %8.1f\n",date,x,y,z);
  return;
}

void GeoMagField::print_result_sv(double date, double ddot,
				  double idot, double hdot,
				  double xdot, double ydot,
				  double zdot, double fdot) const
{
  if (my_isnan(ddot))
    {
      if (my_isnan(xdot))
        printf("  %4.2f        NaN   %7.1f     %8.1f      NaN      NaN %8.1f %8.1f\n",date,idot,hdot,zdot,fdot);
      else
        printf("  %4.2f        NaN   %7.1f     %8.1f %8.1f %8.1f %8.1f %8.1f\n",date,idot,hdot,xdot,ydot,zdot,fdot);
    }
  else 
    printf("  %4.2f   %7.1f    %7.1f     %8.1f %8.1f %8.1f %8.1f %8.1f\n",date,ddot,idot,hdot,xdot,ydot,zdot,fdot);
  return;
}
