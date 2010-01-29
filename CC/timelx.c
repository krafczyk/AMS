/*
 * $Id: timelx.c,v 1.2 2010/01/29 14:46:36 pzuccon Exp $
 *
 * $Log: timelx.c,v $
 * Revision 1.2  2010/01/29 14:46:36  pzuccon
 *  FPE bug fix
 *
 * Revision 1.1  2008/12/08 15:15:18  choutko
 * *** empty log message ***
 *
 * Revision 1.2  1997/02/04 17:34:47  mclareni
 * Merge Winnt and 97a versions
 *
 * Revision 1.1.1.1.2.1  1997/01/21 11:29:45  mclareni
 * All mods for Winnt 96a on winnt branch
 *
 * Revision 1.1.1.1  1996/02/15 17:49:27  mclareni
 * Kernlib
 *
 */
/*>    ROUTINE TIMEL
  CERN PROGLIB# Z007    TIMEST          .VERSION KERNFOR  4.39  940228
  ORIG. 01/03/85  FCA, mod 03/11/93 GF
*/
#include <sys/types.h>
#include <time.h>
#include <sys/times.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <values.h>
#ifndef CLOCKS_PER_SEC
#define  CLOCKS_PER_SEC CLK_TCK
#endif

#ifndef RLIMIT_CPU
#define RLIMIT_CPU 0    /* For HP-UX... */
#endif
#ifndef RLIM_INFINITY
#define RLIM_INFINITY 0x7fffffff    /* For HP-UX... */
#endif

#if defined(CERNLIB_QSYSBSD)||defined(CERNLIB_QMVMI)||defined(CERNLIB_QMVAOS)
#define HZ 60.;
#endif

#ifndef HZ
#ifdef __GNUC__
#define HZ 1
#else
#define HZ 1./CLOCKS_PER_SEC
#endif
#endif

static struct tms tps;
static float timlim;
static time_t timstart, timlast;
static int tml_init = 0;
#pragma omp threadprivate(timstart,timlast,tml_init,timlim,tps)
float deftim = 999.;


                   /*  local routine called by timst, and time_init */
static void time_st(timl)
float timl;
{
    times(&tps);
    timlim = timl;
    timstart =  tps.tms_utime+tps.tms_cutime+tps.tms_stime+tps.tms_cstime;
    timlast  = timstart;
    tml_init = 1;
    return;
}
                   /*  local routine to start by default  */
static void time_init()
{
	struct rlimit rlimit;
	float  maxtime;

	maxtime=deftim;

	if (getrlimit(RLIMIT_CPU, &rlimit)==0) {
		if ( rlimit.rlim_cur != RLIM_INFINITY )
		   maxtime = (float) rlimit.rlim_cur;
	}	

	time_st(maxtime);
	return;
}
#include <fenv.h>
void timest_(timl)
float *timl;
{
 struct rlimit rlimit;
 float  maxtime;

 if (!tml_init ) {

/*  get maximum time allowed by system, and do not allow more */
    maxtime = *timl;
    if (getrlimit(RLIMIT_CPU, &rlimit)==0) {
      if ( rlimit.rlim_cur != RLIM_INFINITY )
	maxtime =  rlimit.rlim_cur*1.;
      else
	maxtime = MAXFLOAT;
      if ( maxtime > *timl ) maxtime= *timl;
    }
    time_st(maxtime);
 }
 return;
}
void timex_(tx)
/*
C
  CERN PROGLIB# Z007    TIMEX           .VERSION KERNFOR  4.39  940228
C
*/
float *tx;
{
   time_t timnow;
   if (!tml_init) {
       time_init();
       *tx = 0.;
   }
   else {
       times(&tps);
       timnow = tps.tms_utime+tps.tms_cutime+tps.tms_stime+tps.tms_cstime;
       *tx = (float) (timnow - timstart) / HZ;
   }
   return;
}

void timed_(td)
/*
C
  CERN PROGLIB# Z007    TIMED           .VERSION KERNFOR  4.39  940228
C
*/
float *td;
{
   time_t timnow;
   if (!tml_init) {
       time_init();
       *td = timlim;
   }
   else {
       times(&tps);
       timnow = tps.tms_utime+tps.tms_cutime+tps.tms_stime+tps.tms_cstime;
       *td = (float) (timnow - timlast) / HZ;
       timlast = timnow;
   }
   return;
}

void timel_(tl)
/*
C
  CERN PROGLIB# Z007    TIMEL           .VERSION KERNFOR  4.39  940228
C
*/
float *tl;
{
//   timlim=2e6
   time_t timnow;
   if (!tml_init) {
       time_init();
       *tl = timlim;
   }
   else {
       times(&tps);
       timnow = tps.tms_utime+tps.tms_cutime+tps.tms_stime+tps.tms_cstime;
       *tl = timlim - (float) (timnow - timstart) / HZ;
   }
   return;
}
#ifdef __GNUC__
#undef time_t
#endif
