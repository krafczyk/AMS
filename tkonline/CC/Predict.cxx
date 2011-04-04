#include "Predict.h"

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <curses.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

/* Constants used by SGP4/SDP4 code */
#define	km2mi		0.621371		/* km to miles */
#define deg2rad		1.745329251994330E-2	/* Degrees to radians */
#define pi		3.14159265358979323846	/* Pi */
#define pio2		1.57079632679489656	/* Pi/2 */
#define x3pio2		4.71238898038468967	/* 3*Pi/2 */
#define twopi		6.28318530717958623	/* 2*Pi  */
#define e6a		1.0E-6
#define tothrd		6.6666666666666666E-1	/* 2/3 */
#define xj2		1.0826158E-3		/* J2 Harmonic (WGS '72) */
#define xj3		-2.53881E-6		/* J3 Harmonic (WGS '72) */   
#define xj4		-1.65597E-6		/* J4 Harmonic (WGS '72) */
#define xke		7.43669161E-2
#define xkmper		6.378137E3		/* WGS 84 Earth radius km */
#define xmnpda		1.44E3			/* Minutes per day */
#define ae		1.0
#define ck2		5.413079E-4
#define ck4		6.209887E-7
#define f		3.35281066474748E-3	/* Flattening factor */
#define ge		3.986008E5 	/* Earth gravitational constant (WGS '72) */
#define S		1.012229
#define qoms2t		1.880279E-09
#define secday		8.6400E4	/* Seconds per day */
#define omega_E		1.00273790934	/* Earth rotations/siderial day */
#define omega_ER	6.3003879	/* Earth rotations, rads/siderial day */
#define zns		1.19459E-5
#define c1ss		2.9864797E-6
#define zes		1.675E-2
#define znl		1.5835218E-4
#define c1l		4.7968065E-7
#define zel		5.490E-2
#define zcosis		9.1744867E-1
#define zsinis		3.9785416E-1
#define zsings		-9.8088458E-1
#define zcosgs		1.945905E-1
#define zcoshs		1
#define zsinhs		0
#define q22		1.7891679E-6
#define q31		2.1460748E-6
#define q33		2.2123015E-7
#define g22		5.7686396
#define g32		9.5240898E-1
#define g44		1.8014998
#define g52		1.0508330
#define g54		4.4108898
#define root22		1.7891679E-6
#define root32		3.7393792E-7
#define root44		7.3636953E-9
#define root52		1.1428639E-7
#define root54		2.1765803E-9
#define thdt		4.3752691E-3
#define rho		1.5696615E-1
#define mfactor		7.292115E-5
#define sr		6.96000E5	/* Solar radius - km (IAU 76) */
#define AU		1.49597870691E8	/* Astronomical unit - km (IAU 76) */

/* Entry points of Deep() */
#define dpinit   1 /* Deep-space initialization code */
#define dpsec    2 /* Deep-space secular code        */
#define dpper    3 /* Deep-space periodic code       */

/* Flow control flag definitions */
#define ALL_FLAGS              -1
#define SGP_INITIALIZED_FLAG   0x000001	/* not used */
#define SGP4_INITIALIZED_FLAG  0x000002
#define SDP4_INITIALIZED_FLAG  0x000004
#define SGP8_INITIALIZED_FLAG  0x000008	/* not used */
#define SDP8_INITIALIZED_FLAG  0x000010	/* not used */
#define SIMPLE_FLAG            0x000020
#define DEEP_SPACE_EPHEM_FLAG  0x000040
#define LUNAR_TERMS_DONE_FLAG  0x000080
#define NEW_EPHEMERIS_FLAG     0x000100	/* not used */
#define DO_LOOP_FLAG           0x000200
#define RESONANCE_FLAG         0x000400
#define SYNCHRONOUS_FLAG       0x000800
#define EPOCH_RESTART_FLAG     0x001000
#define VISIBLE_FLAG           0x002000
#define SAT_ECLIPSED_FLAG      0x004000

/* Global variables for sharing data among functions... */
double	tsince, jul_epoch, jul_utc, eclipse_depth=0,
  sat_azi, sat_ele, sat_range, sat_range_rate,
  sat_lat, sat_lon, sat_alt, sat_vel, phase,
  sun_azi, sun_ele, daynum, fm, fk, age, aostime,
  lostime, ax, ay, az, rx, ry, rz, squint, alat, alon,
  sun_ra, sun_dec, sun_lat, sun_lon, sun_range, sun_range_rate,
  moon_az, moon_el, moon_dx, moon_ra, moon_dec, moon_gha, moon_dv;

char	qthfile[50], tlefile[50], dbfile[50], temp[80], output[25],
  serial_port[15], resave=0, reload_tle=0, netport[6],
  once_per_second=0, ephem[5], sat_sun_status, findsun,
  calc_squint, database=0, xterm, io_lat='N', io_lon='W';

char	visibility_array[24], tracking_mode[30];

float	az_array[24], el_array[24], long_array[24], lat_array[24],
	footprint_array[24], range_array[24], altitude_array[24],
	velocity_array[24], eclipse_depth_array[24], phase_array[24],
	squint_array[24];

double	doppler[24], nextevent[24];

long	aos_array[24], orbitnum_array[24];

long rv, irk;

unsigned char val[256];

int indx, antfd, iaz, iel, ma256, isplat, isplong, socket_flag=0, Flags=0;

/* Two-line Orbital Elements for the satellite used by SGP4/SDP4 code. */
tle_t tle;

geodetic_t obs_geodetic;

SAT_DB sat_db[24];
SAT sat[24];
QTH qth;

void ISSPosition(double time, GeoCoo& pos){

  int x, key=0;
  char updatefile[80], quickstring[40], outputfile[42],
    tle_cli[50], qth_cli[50], interactive=0;
  char env[255];;
  FILE *db;
  
  /* Set up translation table for computing TLE checksums */
  
  for (x=0; x<=255; val[x]=0, x++);
  for (x='0'; x<='9'; val[x]=x-'0', x++);
  
  val['-']=1;
  
  updatefile[0]=0;
  quickstring[0]=0;
  outputfile[0]=0;
  temp[0]=0;
  tle_cli[0]=0;
  qth_cli[0]=0;
  dbfile[0]=0;
  netport[0]=0;
  serial_port[0]=0;
  once_per_second=0;
  
  antfd=-1;

  sprintf(env,"%s/tkonline/data/",getenv("AMSWD"));
  
  if (qth_cli[0]==0)
    sprintf(qthfile,"%s/predict.qth",env);
  else
    sprintf(qthfile,"%s%c",qth_cli,0);
  
  if (tle_cli[0]==0)
    sprintf(tlefile,"%s/predict.tle",env);
  else
    sprintf(tlefile,"%s%c",tle_cli,0);
  
  /* Test for interactive/non-interactive mode of operation
     based on command-line arguments given to PREDICT. */
  
  interactive=1;

  if (interactive){
    sprintf(dbfile,"%s/predict.db",env);
    
    /* If the transponder database file doesn't already
       exist under $HOME/.predict, and a working environment
       is available, place a default copy from the PREDICT
       distribution under $HOME/.predict. */
    
    db=fopen(dbfile,"r");
    
    if (db==NULL){
      sprintf(temp,"%s/predict.db",env);
      CopyFile(temp,dbfile);
    }
    
    else
      fclose(db);
  }
  
  x=ReadDataFiles();
  
  if (x!=3){//x=3 -> Both TLE and QTH files were loaded successfully
    if (tle_cli[0] || qth_cli[0]){
      /* "Houston, we have a problem..." */
      
      printf("\n%c",7);
      
      if (x^1)
	printf("*** ERROR!  Your QTH file \"%s\" could not be loaded!\n",qthfile);
      
      if (x^2)
	printf("*** ERROR!  Your TLE file \"%s\" could not be loaded!\n",tlefile);
      
      printf("\n");
      
      exit(-1);
    }
  }
  
  if (x==3){
    indx=22;//the ISS position on tle file (startinf from 0)
    
    if (indx!=-1 && sat[indx].meanmo!=0.0 && Decayed(indx,0.0)==0)
      SingleTrack(indx,key, time, pos);
  }
  
  return;
}

char CopyFile(char* source, char* destination){
  /* This function copies file "source" to file "destination"
     in 64k chunks.  The permissions on the destination file
     are set to rw-r--r--  (0644).  A 0 is returned if no
     errors are encountered.  A 1 indicates a problem writing
     to the destination file.  A 2 indicates a problem reading
     the source file.  */
  
  int x, sd, dd;
  char error=0, buffer[65536];
  
  sd=open(source,O_RDONLY);
  
  if (sd!=-1){
    dd=open(destination,O_WRONLY | O_CREAT| O_TRUNC, 0644);
    
    if (dd!=-1){
      x=read(sd,&buffer,65536);
      
      while (x){
	write(dd,&buffer,x);
	x=read(sd,&buffer,65536);
      }
      
      close(dd);
    }
    else
      error=1;
    
    close(sd);
  }
  else
    error+=2;
  
  return error;
}  

char ReadDataFiles(){
  /* This function reads "predict.qth", "predict.tle",
     and (optionally) "predict.db" files into memory.
     Return values are as follows:
     
     0: Neither the qth nor the tle files were loaded
     1: Only the qth file was loaded
     2: Only the tle file was loaded
     3: The qth and tle files were loaded successfully */
  
  FILE *fd;
  long catnum;
  unsigned char dayofweek;
  int x=0, y, entry=0, max_entries=10, transponders=0;
  char flag=0, match, name[80], line1[80], line2[80];
  
  fd=fopen(qthfile,"r");

  //  printf("Opening %s = %d\n", qthfile, fd);
  
  if (fd!=NULL){
    fgets(qth.callsign,16,fd);
    qth.callsign[strlen(qth.callsign)-1]=0;
    fscanf(fd,"%lf", &qth.stnlat);
    fscanf(fd,"%lf", &qth.stnlong);
    fscanf(fd,"%d", &qth.stnalt);
    fclose(fd);
    
    obs_geodetic.lat=qth.stnlat*deg2rad;
    obs_geodetic.lon=-qth.stnlong*deg2rad;
    obs_geodetic.alt=((double)qth.stnalt)/1000.0;
    obs_geodetic.theta=0.0;
    
    flag=1;
  }
  
  fd=fopen(tlefile,"r");
  
  if (fd!=NULL){
    while (x<24 && feof(fd)==0){
      /* Initialize variables */
      
      name[0]=0;
      line1[0]=0;
      line2[0]=0;
      
      /* Read element set */
      
      fgets(name,75,fd);
      fgets(line1,75,fd);
      fgets(line2,75,fd);
      
      if (KepCheck(line1,line2) && (feof(fd)==0)){
	/* We found a valid TLE! */
	
	/* Some TLE sources left justify the sat
	   name in a 24-byte field that is padded
	   with blanks.  The following lines cut
	   out the blanks as well as the line feed
	   character read by the fgets() function. */
	
	y=strlen(name);
	
	while (name[y]==32 || name[y]==0 || name[y]==10 || name[y]==13 || y==0){
	  name[y]=0;
	  y--;
	}
	
	/* Copy TLE data into the sat data structure */
	
	strncpy(sat[x].name,name,24);
	strncpy(sat[x].line1,line1,69);
	strncpy(sat[x].line2,line2,69);
	
	/* Update individual parameters */
	
	InternalUpdate(x);
	
	x++;
      }
    }
    
    fclose(fd);
    flag+=2;
    resave=0;
    
    /* Load satellite database file */
    
    fd=fopen(dbfile,"r");
    
    if (fd!=NULL){
      database=1;
      
      fgets(line1,40,fd);
      
      while (strncmp(line1,"end",3)!=0 && line1[0]!='\n' && feof(fd)==0){
	/* The first line is the satellite
	   name which is ignored here. */
	
	fgets(line1,40,fd);
	sscanf(line1,"%ld",&catnum);
	
	/* Search for match */
	
	for (y=0, match=0; y<24 && match==0; y++){
	  if (catnum==sat[y].catnum)
	    match=1;
	}
	
	if (match){
	  transponders=0;
	  entry=0;
	  y--;
	}
	
	fgets(line1,40,fd);
	
	if (match){
	  if (strncmp(line1,"No",2)!=0){
	    sscanf(line1,"%lf, %lf",&sat_db[y].alat, &sat_db[y].alon);
	    sat_db[y].squintflag=1;
	  }

	  else
	    sat_db[y].squintflag=0;
	}
	
	fgets(line1,80,fd);
	
	while (strncmp(line1,"end",3)!=0 && line1[0]!='\n' && feof(fd)==0){
	  if (entry<max_entries){
	      if (match){
		if (strncmp(line1,"No",2)!=0){
		  line1[strlen(line1)-1]=0;
		  strcpy(sat_db[y].transponder_name[entry],line1);
		}
		else
		  sat_db[y].transponder_name[entry][0]=0;
	      }
	      
	      fgets(line1,40,fd);
	      
	      if (match)
		sscanf(line1,"%lf, %lf", &sat_db[y].uplink_start[entry], &sat_db[y].uplink_end[entry]);
	      
	      fgets(line1,40,fd);
	      
	      if (match)
		sscanf(line1,"%lf, %lf", &sat_db[y].downlink_start[entry], &sat_db[y].downlink_end[entry]);
	      
	      fgets(line1,40,fd);
	      
	      if (match){
		if (strncmp(line1,"No",2)!=0){
		  dayofweek=(unsigned char)atoi(line1);
		  sat_db[y].dayofweek[entry]=dayofweek;
		}
		else
		  sat_db[y].dayofweek[entry]=0;
	      }
	      
	      fgets(line1,40,fd);
	      
	      if (match){
		if (strncmp(line1,"No",2)!=0)
		  sscanf(line1,"%d, %d",&sat_db[y].phase_start[entry], &sat_db[y].phase_end[entry]);
		else
		  {
		    sat_db[y].phase_start[entry]=0;
		    sat_db[y].phase_end[entry]=0;
		  }
		
		if (sat_db[y].uplink_start[entry]!=0.0 || sat_db[y].downlink_start[entry]!=0.0)
		  transponders++;
		
		entry++;
	      }
	  }
	  fgets(line1,80,fd);
	}
	fgets(line1,80,fd);
	
	if (match)
	  sat_db[y].transponders=transponders;
	
	entry=0;
	transponders=0;
      }
      
      fclose(fd);
    }
  }
  
  return flag;
}

char Decayed(int x, double time){
  /* This function returns a 1 if it appears that the
     satellite pointed to by 'x' has decayed at the
     time of 'time'.  If 'time' is 0.0, then the
     current date/time is used. */
  
  double satepoch;

  if (time==0.0)
    time=CurrentDaynum();
  
  satepoch=DayNum(1,0,sat[x].year)+sat[x].refepoch;
  
  if (satepoch+((16.666666-sat[x].meanmo)/(10.0*fabs(sat[x].drag))) < time)
    return 1;
  else
    return 0;
}

char KepCheck(char* line1, char* line2){
  /* This function scans line 1 and line 2 of a NASA 2-Line element
     set and returns a 1 if the element set appears to be valid or
     a 0 if it does not.  If the data survives this torture test,
     it's a pretty safe bet we're looking at a valid 2-line
     element set and not just some random text that might pass
     as orbital data based on a simple checksum calculation alone. */
  
  int x;
  unsigned sum1, sum2;
  
  /* Compute checksum for each line */
  
  for (x=0, sum1=0, sum2=0; x<=67; sum1+=val[(int)line1[x]], sum2+=val[(int)line2[x]], x++);
  
  /* Perform a "torture test" on the data */
  
  x=(val[(int)line1[68]]^(sum1%10)) | (val[(int)line2[68]]^(sum2%10)) |
    (line1[0]^'1')  | (line1[1]^' ')  | (line1[7]^'U')  |
    (line1[8]^' ')  | (line1[17]^' ') | (line1[23]^'.') |
    (line1[32]^' ') | (line1[34]^'.') | (line1[43]^' ') |
    (line1[52]^' ') | (line1[61]^' ') | (line1[62]^'0') |
    (line1[63]^' ') | (line2[0]^'2')  | (line2[1]^' ')  |
    (line2[7]^' ')  | (line2[11]^'.') | (line2[16]^' ') |
    (line2[20]^'.') | (line2[25]^' ') | (line2[33]^' ') |
    (line2[37]^'.') | (line2[42]^' ') | (line2[46]^'.') |
    (line2[51]^' ') | (line2[54]^'.') | (line1[2]^line2[2]) |
    (line1[3]^line2[3]) | (line1[4]^line2[4]) |
    (line1[5]^line2[5]) | (line1[6]^line2[6]) |
    (isdigit(line1[68]) ? 0 : 1) | (isdigit(line2[68]) ? 0 : 1) |
    (isdigit(line1[18]) ? 0 : 1) | (isdigit(line1[19]) ? 0 : 1) |
    (isdigit(line2[31]) ? 0 : 1) | (isdigit(line2[32]) ? 0 : 1);
  
  return (x ? 0 : 1);
}

void InternalUpdate(int x){
  /* Updates data in TLE structure based on
     line1 and line2 stored in structure. */
  
  double tempnum;
  
  strncpy(sat[x].designator,SubString(sat[x].line1,9,16),8);
  sat[x].designator[9]=0;
  sat[x].catnum=atol(SubString(sat[x].line1,2,6));
  sat[x].year=atoi(SubString(sat[x].line1,18,19));
  sat[x].refepoch=atof(SubString(sat[x].line1,20,31));
  tempnum=1.0e-5*atof(SubString(sat[x].line1,44,49));
  sat[x].nddot6=tempnum/pow(10.0,(sat[x].line1[51]-'0'));
  tempnum=1.0e-5*atof(SubString(sat[x].line1,53,58));
  sat[x].bstar=tempnum/pow(10.0,(sat[x].line1[60]-'0'));
  sat[x].setnum=atol(SubString(sat[x].line1,64,67));
  sat[x].incl=atof(SubString(sat[x].line2,8,15));
  sat[x].raan=atof(SubString(sat[x].line2,17,24));
  sat[x].eccn=1.0e-07*atof(SubString(sat[x].line2,26,32));
  sat[x].argper=atof(SubString(sat[x].line2,34,41));
  sat[x].meanan=atof(SubString(sat[x].line2,43,50));
  sat[x].meanmo=atof(SubString(sat[x].line2,52,62));
  sat[x].drag=atof(SubString(sat[x].line1,33,42));
  sat[x].orbitnum=atof(SubString(sat[x].line2,63,67));
}

double CurrentDaynum(){
  /* Read the system clock and return the number
     of days since 31Dec79 00:00:00 UTC (daynum 0) */
  
  int x;
  struct timeval tptr;
  double usecs, seconds;
  
  x=gettimeofday(&tptr,NULL);
  
  usecs=0.000001*(double)tptr.tv_usec;
  seconds=usecs+(double)tptr.tv_sec;
  
  return ((seconds/86400.0)-3651.0);
}

long DayNum(int m, int d, int y){
  /* This function calculates the day number from m/d/y. */
  
  long dn;
  double mm, yy;
  
  if (m<3){ 
    y--; 
    m+=12; 
  }
  
  if (y<57)
    y+=100;
  
  yy=(double)y;
  mm=(double)m;
  dn=(long)(floor(365.25*(yy-80.0))-floor(19.0+yy/100.0)+floor(4.75+yy/400.0)-16.0);
  dn+=d+30*m+(long)floor(0.6*mm-0.3);
  return dn;
}

char *SubString(char* string, char start, char end){
  /* This function returns a substring based on the starting
     and ending positions provided.  It is used heavily in
     the AutoUpdate function when parsing 2-line element data. */
  
  unsigned x, y;
  
  if (end>=start){
    for (x=start, y=0; x<=end && string[x]!=0; x++)
      if (string[x]!=' '){
	temp[y]=string[x];
	y++;
      }
    
    temp[y]=0;
    return temp;
  }
  else
    return NULL;
}

void SingleTrack(int x, char speak, double time, GeoCoo& pos){
  /* This function tracks a single satellite in real-time
     until 'Q' or ESC is pressed.  x represents the index
     of the satellite being tracked.  If speak=='T', then
     the speech routines are enabled. */

  char decayed=0;
  double doppler100=0.0, delay;
  
  //  printf("Satellite Track..\n");

  PreCalc(x);
  indx=x;
  
  //  daynum=CurrentDaynum();
  daynum=time;
  decayed=Decayed(indx,0.0);
  
  //  do{
  daynum=time;
  //  printf("%s    ",Daynum2String(daynum));
  Calc();
  
  pos.Lat=sat_lat;
  //  printf("Latitude: %f", pos.Lat);
  //  printf("Latitude: %-6.2f (%.2f) ",(io_lat=='N'?+1:-1)*sat_lat, sat_lat);
  //  printf("%s  ",(io_lat=='N'?"N":"S"));
  //  printf("  ");
  pos.Lon=180.0-sat_lon;
  //  printf("Longitude: %f", pos.Lon);
  //  printf("Longitude: %-7.2f (%.2f) ",(io_lon=='W'?360.0-sat_lon:sat_lon), sat_lon);
  //  printf("%s  ",(io_lon=='W'?"W":"E"));
  //  printf("  ");
  
  fk=12756.33*acos(xkmper/(xkmper+sat_alt));
  fm=fk*km2mi;
  
  //    printf("Alt: %0.f ",sat_alt*km2mi);
  //  printf("Alt: %0.f km   ",sat_alt);
  //    printf("Range: %-5.0f",sat_range*km2mi);
  //    printf("Range: %-5.0f ",sat_range);
  //    printf("Azimut: %-7.2f ",sat_azi);
  //    printf("Elevation: %+-6.2f ",sat_ele);
  //    printf("Velocity: %0.f ",(3600.0*sat_vel)*km2mi);
  //  printf("Velocity: %0.f km/h   ",3600.0*sat_vel);
  //    printf("Eclipse: %+6.2f%c ",eclipse_depth/deg2rad,176);
  //    printf("Phase: %5.1f ",256.0*(phase/twopi));
  //    printf("Model: %s",ephem);
  //  printf("\n");
  //  usleep(100000);
  
  doppler100=-100.0e06*((sat_range_rate*1000.0)/299792458.0);
  delay=1000.0*((1000.0*sat_range)/299792458.0);
  
  //} while (ans!='q' && ans!=27);
  
  sprintf(tracking_mode, "NONE\n%c",0);
}

void PreCalc(int x){
  /* This function copies TLE data from PREDICT's sat structure
     to the SGP4/SDP4's single dimensioned tle structure, and
     prepares the tracking code for the update. */
  
  strcpy(tle.sat_name,sat[x].name);
  strcpy(tle.idesg,sat[x].designator);
  tle.catnr=sat[x].catnum;
  tle.epoch=(1000.0*(double)sat[x].year)+sat[x].refepoch;
  tle.xndt2o=sat[x].drag;
  tle.xndd6o=sat[x].nddot6;
  tle.bstar=sat[x].bstar;
  tle.xincl=sat[x].incl;
  tle.xnodeo=sat[x].raan;
  tle.eo=sat[x].eccn;
  tle.omegao=sat[x].argper;
  tle.xmo=sat[x].meanan;
  tle.xno=sat[x].meanmo;
  tle.revnum=sat[x].orbitnum;
  
  if (sat_db[x].squintflag){
    calc_squint=1;
    alat=deg2rad*sat_db[x].alat;
    alon=deg2rad*sat_db[x].alon;
  }
  else
    calc_squint=0;
  
  /* Clear all flags */
  
  ClearFlag(ALL_FLAGS);
  
  /* Select ephemeris type.  This function will set or clear the
     DEEP_SPACE_EPHEM_FLAG depending on the TLE parameters of the
     satellite.  It will also pre-process tle members for the
     ephemeris functions SGP4 or SDP4, so this function must
     be called each time a new tle set is used. */
  
  select_ephemeris(&tle);
}

char Geostationary(int x){
  /* This function returns a 1 if the satellite pointed
     to by "x" appears to be in a geostationary orbit */
  
  if (fabs(sat[x].meanmo-1.0027)<0.0002) 
    
    return 1;
  else
    return 0;
}

char *Abbreviate(char* string,int n){
  /* This function returns an abbreviated substring of the original,
     including a '~' character if a non-blank character is chopped
     out of the generated substring.  n is the length of the desired
     substring.  It is used for abbreviating satellite names. */
  
  strncpy(temp,string,79);
  
  if (temp[n]!=0 && temp[n]!=32){
    temp[n-2]='~';
    temp[n-1]=temp[strlen(temp)-1];
  }
  
  temp[n]=0;
  
  return temp;
}

char *Daynum2String(double daynum){
  /* This function takes the given epoch as a fractional number of
     days since 31Dec79 00:00:00 UTC and returns the corresponding
     date as a string of the form "Tue 12Oct99 17:22:37". */
  
  char timestr[26];
  time_t t;
  int x;
  
  /* Convert daynum to Unix time (seconds since 01-Jan-70) */
  t=(time_t)(86400.0*(daynum+3651.0));
  
  sprintf(timestr,"%s",asctime(gmtime(&t)));
  
  if (timestr[8]==' ')
    timestr[8]='0';
  
  for (x=0; x<=3; output[x]=timestr[x], x++);
  
  output[4]=timestr[8];
  output[5]=timestr[9];
  output[6]=timestr[4];
  output[7]=timestr[5];
  output[8]=timestr[6];
  output[9]=timestr[22];
  output[10]=timestr[23];
  output[11]=' ';
  
  for (x=12; x<=19; output[x]=timestr[x-1], x++);
  
  output[20]=0;
  return output;
}

void Calc(){
  /* This is the stuff we need to do repetitively while tracking. */
  
  /* Zero vector for initializations */
  vector_t zero_vector={0,0,0,0};
  
  /* Satellite position and velocity vectors */
  vector_t vel=zero_vector;
  vector_t pos=zero_vector;
  
  /* Satellite Az, El, Range, Range rate */
  vector_t obs_set;
  
  /* Solar ECI position vector  */
  vector_t solar_vector=zero_vector;
  
  /* Solar observed azi and ele vector  */
  vector_t solar_set;
  
  /* Satellite's predicted geodetic position */
  geodetic_t sat_geodetic;
  
  jul_utc=daynum+2444238.5;
  
  /* Convert satellite's epoch time to Julian  */
  /* and calculate time since epoch in minutes */
  
  jul_epoch=Julian_Date_of_Epoch(tle.epoch);
  tsince=(jul_utc-jul_epoch)*xmnpda;
  age=jul_utc-jul_epoch;
  
  /* Copy the ephemeris type in use to ephem string. */
  
  if (isFlagSet(DEEP_SPACE_EPHEM_FLAG))
    strcpy(ephem,"SDP4");
  else
    strcpy(ephem,"SGP4");
  
  /* Call NORAD routines according to deep-space flag. */
  
  if (isFlagSet(DEEP_SPACE_EPHEM_FLAG))
    SDP4(tsince, &tle, &pos, &vel);
  else
    SGP4(tsince, &tle, &pos, &vel);
  
  /* Scale position and velocity vectors to km and km/sec */
  
  Convert_Sat_State(&pos, &vel);
  
  /* Calculate velocity of satellite */
  
  Magnitude(&vel);
  sat_vel=vel.w;
  
  /** All angles in rads. Distance in km. Velocity in km/s **/
  /* Calculate satellite Azi, Ele, Range and Range-rate */
  
  Calculate_Obs(jul_utc, &pos, &vel, &obs_geodetic, &obs_set);
  
  /* Calculate satellite Lat North, Lon East and Alt. */
  
  Calculate_LatLonAlt(jul_utc, &pos, &sat_geodetic);
  
  /* Calculate squint angle */
  
  if (calc_squint)
    squint=(acos(-(ax*rx+ay*ry+az*rz)/obs_set.z))/deg2rad;
  
  /* Calculate solar position and satellite eclipse depth. */
  /* Also set or clear the satellite eclipsed flag accordingly. */
  
  Calculate_Solar_Position(jul_utc, &solar_vector);
  Calculate_Obs(jul_utc, &solar_vector, &zero_vector, &obs_geodetic, &solar_set);
  
  if (Sat_Eclipsed(&pos, &solar_vector, &eclipse_depth))
    SetFlag(SAT_ECLIPSED_FLAG);
  else
    ClearFlag(SAT_ECLIPSED_FLAG);
  
  if (isFlagSet(SAT_ECLIPSED_FLAG))
    sat_sun_status=0;  /* Eclipse */
  else
    sat_sun_status=1; /* In sunlight */
  
  /* Convert satellite and solar data */
  sat_azi=Degrees(obs_set.x);
  sat_ele=Degrees(obs_set.y);
  sat_range=obs_set.z;
  sat_range_rate=obs_set.w;
  sat_lat=Degrees(sat_geodetic.lat);
  sat_lon=Degrees(sat_geodetic.lon);
  sat_alt=sat_geodetic.alt;
  
  fk=12756.33*acos(xkmper/(xkmper+sat_alt));
  fm=fk/1.609344;
  
  rv=(long)floor((tle.xno*xmnpda/twopi+age*tle.bstar*ae)*age+tle.xmo/twopi)+tle.revnum;
  
  sun_azi=Degrees(solar_set.x); 
  sun_ele=Degrees(solar_set.y);
  
  irk=(long)rint(sat_range);
  isplat=(int)rint(sat_lat);
  isplong=(int)rint(360.0-sat_lon);
  iaz=(int)rint(sat_azi);
  iel=(int)rint(sat_ele);
  ma256=(int)rint(256.0*(phase/twopi));
  
  if (sat_sun_status){
    if (sun_ele<=-12.0 && rint(sat_ele)>=0.0)
      findsun='+';
    else
      findsun='*';
  }
  else
    findsun=' ';
}

void TrackDataOut(int antfd, double elevation, double azimuth){
  /* This function sends Azimuth and Elevation data
     to an antenna tracker connected to the serial port */
  
  int n, port;
  char message[30]="\n";
  
  port=antfd;
  
  sprintf(message, "AZ%3.1f EL%3.1f \x0D\x0A", azimuth,elevation);
  n=write(port,message,strlen(message));
  
  if (n<0){
    bailout("Error Writing To Antenna Port");
    exit(-1);
  }
}

void FindSun(double daynum){
  /* This function finds the position of the Sun */
  
  /* Zero vector for initializations */
  vector_t zero_vector={0,0,0,0};
  
  /* Solar ECI position vector  */
  vector_t solar_vector=zero_vector;
  
  /* Solar observed azi and ele vector  */
  vector_t solar_set=zero_vector;
  
  /* Solar right ascension and declination vector */
  vector_t solar_rad=zero_vector;
  
  /* Solar lat, long, alt vector */
  geodetic_t solar_latlonalt;
  
  jul_utc=daynum+2444238.5;
  
  Calculate_Solar_Position(jul_utc, &solar_vector);
  Calculate_Obs(jul_utc, &solar_vector, &zero_vector, &obs_geodetic, &solar_set);
  sun_azi=Degrees(solar_set.x); 
  sun_ele=Degrees(solar_set.y);
  sun_range=1.0+((solar_set.z-AU)/AU);
  sun_range_rate=1000.0*solar_set.w;
  
  Calculate_LatLonAlt(jul_utc, &solar_vector, &solar_latlonalt);
  
  sun_lat=Degrees(solar_latlonalt.lat);
  sun_lon=360.0-Degrees(solar_latlonalt.lon);
  
  Calculate_RADec(jul_utc, &solar_vector, &zero_vector, &obs_geodetic, &solar_rad);
  
  sun_ra=Degrees(solar_rad.x);
  sun_dec=Degrees(solar_rad.y);
}

void ClearFlag(int flag)
{
	Flags&=~flag;
}

void select_ephemeris(tle_t *tle){
  /* Selects the apropriate ephemeris type to be used */
  /* for predictions according to the data in the TLE */
  /* It also processes values in the tle set so that  */
  /* they are apropriate for the sgp4/sdp4 routines   */
  
  double ao, xnodp, dd1, dd2, delo, temp, a1, del1, r1;
  
  /* Preprocess tle set */
  tle->xnodeo*=deg2rad;
  tle->omegao*=deg2rad;
  tle->xmo*=deg2rad;
  tle->xincl*=deg2rad;
  temp=twopi/xmnpda/xmnpda;
  tle->xno=tle->xno*temp*xmnpda;
  tle->xndt2o*=temp;
  tle->xndd6o=tle->xndd6o*temp/xmnpda;
  tle->bstar/=ae;
  
  /* Period > 225 minutes is deep space */
  dd1=(xke/tle->xno);
  dd2=tothrd;
  a1=pow(dd1,dd2);
  r1=cos(tle->xincl);
  dd1=(1.0-tle->eo*tle->eo);
  temp=ck2*1.5f*(r1*r1*3.0-1.0)/pow(dd1,1.5);
  del1=temp/(a1*a1);
  ao=a1*(1.0-del1*(tothrd*.5+del1*(del1*1.654320987654321+1.0)));
  delo=temp/(ao*ao);
  xnodp=tle->xno/(delo+1.0);
  
  /* Select a deep-space/near-earth ephemeris */
  
  if (twopi/xnodp/xmnpda>=0.15625)
    SetFlag(DEEP_SPACE_EPHEM_FLAG);
  else
    ClearFlag(DEEP_SPACE_EPHEM_FLAG);
}

double Julian_Date_of_Year(double year){
  /* The function Julian_Date_of_Year calculates the Julian Date  */
  /* of Day 0.0 of {year}. This function is used to calculate the */
  /* Julian Date of any date by using Julian_Date_of_Year, DOY,   */
  /* and Fraction_of_Day. */
  
  /* Astronomical Formulae for Calculators, Jean Meeus, */
  /* pages 23-25. Calculate Julian Date of 0.0 Jan year */
  
  long A, B, i;
  double jdoy;
  
  year=year-1;
  i=year/100;
  A=i;
  i=A/4;
  B=2-A+i;
  i=365.25*year;
  i+=30.6001*14;
  jdoy=i+1720994.5+B;
  
  return jdoy;
}

double Julian_Date_of_Epoch(double epoch){ 
  /* The function Julian_Date_of_Epoch returns the Julian Date of     */
  /* an epoch specified in the format used in the NORAD two-line      */
  /* element sets. It has been modified to support dates beyond       */
  /* the year 1999 assuming that two-digit years in the range 00-56   */
  /* correspond to 2000-2056. Until the two-line element set format   */
  /* is changed, it is only valid for dates through 2056 December 31. */
  
  double year, day;
  
  /* Modification to support Y2K */
  /* Valid 1957 through 2056     */
  
  day=modf(epoch*1E-3, &year)*1E3;
  
  if (year<57)
    year=year+2000;
  else
    year=year+1900;
  
  return (Julian_Date_of_Year(year)+day);
}

int isFlagSet(int flag){
  return (Flags&flag);
}

void SetFlag(int flag){
  Flags|=flag;
}

void SDP4(double tsince, tle_t * tle, vector_t * pos, vector_t * vel){
  /* This function is used to calculate the position and velocity */
  /* of deep-space (period > 225 minutes) satellites. tsince is   */
  /* time since epoch in minutes, tle is a pointer to a tle_t     */
  /* structure with Keplerian orbital elements and pos and vel    */
  /* are vector_t structures returning ECI satellite position and */
  /* velocity. Use Convert_Sat_State() to convert to km and km/s. */
  
  int i;
  
  static double x3thm1, c1, x1mth2, c4, xnodcf, t2cof, xlcof,
    aycof, x7thm1;
  
  double a, axn, ayn, aynl, beta, betal, capu, cos2u, cosepw, cosik,
    cosnok, cosu, cosuk, ecose, elsq, epw, esine, pl, theta4, rdot,
    rdotk, rfdot, rfdotk, rk, sin2u, sinepw, sinik, sinnok, sinu,
    sinuk, tempe, templ, tsq, u, uk, ux, uy, uz, vx, vy, vz, xinck, xl,
    xlt, xmam, xmdf, xmx, xmy, xnoddf, xnodek, xll, a1, a3ovk2, ao, c2,
    coef, coef1, x1m5th, xhdot1, del1, r, delo, eeta, eta, etasq,
    perigee, psisq, tsi, qoms24, s4, pinvsq, temp, tempa, temp1,
    temp2, temp3, temp4, temp5, temp6, bx, by, bz, cx, cy, cz;
  
  static deep_arg_t deep_arg;
  
  /* Initialization */
  
  if (isFlagClear(SDP4_INITIALIZED_FLAG)){
    SetFlag(SDP4_INITIALIZED_FLAG);
    
    /* Recover original mean motion (xnodp) and   */
    /* semimajor axis (aodp) from input elements. */
    
    a1=pow(xke/tle->xno,tothrd);
    deep_arg.cosio=cos(tle->xincl);
    deep_arg.theta2=deep_arg.cosio*deep_arg.cosio;
    x3thm1=3*deep_arg.theta2-1;
    deep_arg.eosq=tle->eo*tle->eo;
    deep_arg.betao2=1-deep_arg.eosq;
    deep_arg.betao=sqrt(deep_arg.betao2);
    del1=1.5*ck2*x3thm1/(a1*a1*deep_arg.betao*deep_arg.betao2);
    ao=a1*(1-del1*(0.5*tothrd+del1*(1+134/81*del1)));
    delo=1.5*ck2*x3thm1/(ao*ao*deep_arg.betao*deep_arg.betao2);
    deep_arg.xnodp=tle->xno/(1+delo);
    deep_arg.aodp=ao/(1-delo);
    
    /* For perigee below 156 km, the values */
    /* of s and qoms2t are altered.         */
    
    s4=S;
    qoms24=qoms2t;
    perigee=(deep_arg.aodp*(1-tle->eo)-ae)*xkmper;
    
    if (perigee<156.0){
      if (perigee<=98.0)
	s4=20.0;
      else
	s4=perigee-78.0;
      
      qoms24=pow((120-s4)*ae/xkmper,4);
      s4=s4/xkmper+ae;
    }
    
    pinvsq=1/(deep_arg.aodp*deep_arg.aodp*deep_arg.betao2*deep_arg.betao2);
    deep_arg.sing=sin(tle->omegao);
    deep_arg.cosg=cos(tle->omegao);
    tsi=1/(deep_arg.aodp-s4);
    eta=deep_arg.aodp*tle->eo*tsi;
    etasq=eta*eta;
    eeta=tle->eo*eta;
    psisq=fabs(1-etasq);
    coef=qoms24*pow(tsi,4);
    coef1=coef/pow(psisq,3.5);
    c2=coef1*deep_arg.xnodp*(deep_arg.aodp*(1+1.5*etasq+eeta*(4+etasq))+0.75*ck2*tsi/psisq*x3thm1*(8+3*etasq*(8+etasq)));
    c1=tle->bstar*c2;
    deep_arg.sinio=sin(tle->xincl);
    a3ovk2=-xj3/ck2*pow(ae,3);
    x1mth2=1-deep_arg.theta2;
    c4=2*deep_arg.xnodp*coef1*deep_arg.aodp*deep_arg.betao2*(eta*(2+0.5*etasq)+tle->eo*(0.5+2*etasq)-2*ck2*tsi/(deep_arg.aodp*psisq)*(-3*x3thm1*(1-2*eeta+etasq*(1.5-0.5*eeta))+0.75*x1mth2*(2*etasq-eeta*(1+etasq))*cos(2*tle->omegao)));
    theta4=deep_arg.theta2*deep_arg.theta2;
    temp1=3*ck2*pinvsq*deep_arg.xnodp;
    temp2=temp1*ck2*pinvsq;
    temp3=1.25*ck4*pinvsq*pinvsq*deep_arg.xnodp;
    deep_arg.xmdot=deep_arg.xnodp+0.5*temp1*deep_arg.betao*x3thm1+0.0625*temp2*deep_arg.betao*(13-78*deep_arg.theta2+137*theta4);
    x1m5th=1-5*deep_arg.theta2;
    deep_arg.omgdot=-0.5*temp1*x1m5th+0.0625*temp2*(7-114*deep_arg.theta2+395*theta4)+temp3*(3-36*deep_arg.theta2+49*theta4);
    xhdot1=-temp1*deep_arg.cosio;
    deep_arg.xnodot=xhdot1+(0.5*temp2*(4-19*deep_arg.theta2)+2*temp3*(3-7*deep_arg.theta2))*deep_arg.cosio;
    xnodcf=3.5*deep_arg.betao2*xhdot1*c1;
    t2cof=1.5*c1;
    xlcof=0.125*a3ovk2*deep_arg.sinio*(3+5*deep_arg.cosio)/(1+deep_arg.cosio);
    aycof=0.25*a3ovk2*deep_arg.sinio;
    x7thm1=7*deep_arg.theta2-1;
    
    /* initialize Deep() */
    
    Deep(dpinit,tle,&deep_arg);
  }
  
  /* Update for secular gravity and atmospheric drag */
  xmdf=tle->xmo+deep_arg.xmdot*tsince;
  deep_arg.omgadf=tle->omegao+deep_arg.omgdot*tsince;
  xnoddf=tle->xnodeo+deep_arg.xnodot*tsince;
  tsq=tsince*tsince;
  deep_arg.xnode=xnoddf+xnodcf*tsq;
  tempa=1-c1*tsince;
  tempe=tle->bstar*c4*tsince;
  templ=t2cof*tsq;
  deep_arg.xn=deep_arg.xnodp;
  
  /* Update for deep-space secular effects */
  deep_arg.xll=xmdf;
  deep_arg.t=tsince;
  
  Deep(dpsec, tle, &deep_arg);
  
  xmdf=deep_arg.xll;
  a=pow(xke/deep_arg.xn,tothrd)*tempa*tempa;
  deep_arg.em=deep_arg.em-tempe;
  xmam=xmdf+deep_arg.xnodp*templ;
  
  /* Update for deep-space periodic effects */
  deep_arg.xll=xmam;
  
  Deep(dpper,tle,&deep_arg);
  
  xmam=deep_arg.xll;
  xl=xmam+deep_arg.omgadf+deep_arg.xnode;
  beta=sqrt(1-deep_arg.em*deep_arg.em);
  deep_arg.xn=xke/pow(a,1.5);
  
  /* Long period periodics */
  axn=deep_arg.em*cos(deep_arg.omgadf);
  temp=1/(a*beta*beta);
  xll=temp*xlcof*axn;
  aynl=temp*aycof;
  xlt=xl+xll;
  ayn=deep_arg.em*sin(deep_arg.omgadf)+aynl;
  
  /* Solve Kepler's Equation */
  capu=FMod2p(xlt-deep_arg.xnode);
  temp2=capu;
  i=0;
  
  do{
    sinepw=sin(temp2);
    cosepw=cos(temp2);
    temp3=axn*sinepw;
    temp4=ayn*cosepw;
    temp5=axn*cosepw;
    temp6=ayn*sinepw;
    epw=(capu-temp4+temp3-temp2)/(1-temp5-temp6)+temp2;
    
    if (fabs(epw-temp2)<=e6a)
      break;
    
    temp2=epw;
    
  } while (i++<10);
  
  /* Short period preliminary quantities */
  ecose=temp5+temp6;
  esine=temp3-temp4;
  elsq=axn*axn+ayn*ayn;
  temp=1-elsq;
  pl=a*temp;
  r=a*(1-ecose);
  temp1=1/r;
  rdot=xke*sqrt(a)*esine*temp1;
  rfdot=xke*sqrt(pl)*temp1;
  temp2=a*temp1;
  betal=sqrt(temp);
  temp3=1/(1+betal);
  cosu=temp2*(cosepw-axn+ayn*esine*temp3);
  sinu=temp2*(sinepw-ayn-axn*esine*temp3);
  u=AcTan(sinu,cosu);
  sin2u=2*sinu*cosu;
  cos2u=2*cosu*cosu-1;
  temp=1/pl;
  temp1=ck2*temp;
  temp2=temp1*temp;
  
  /* Update for short periodics */
  rk=r*(1-1.5*temp2*betal*x3thm1)+0.5*temp1*x1mth2*cos2u;
  uk=u-0.25*temp2*x7thm1*sin2u;
  xnodek=deep_arg.xnode+1.5*temp2*deep_arg.cosio*sin2u;
  xinck=deep_arg.xinc+1.5*temp2*deep_arg.cosio*deep_arg.sinio*cos2u;
  rdotk=rdot-deep_arg.xn*temp1*x1mth2*sin2u;
  rfdotk=rfdot+deep_arg.xn*temp1*(x1mth2*cos2u+1.5*x3thm1);
  
  /* Orientation vectors */
  sinuk=sin(uk);
  cosuk=cos(uk);
  sinik=sin(xinck);
  cosik=cos(xinck);
  sinnok=sin(xnodek);
  cosnok=cos(xnodek);
  xmx=-sinnok*cosik;
  xmy=cosnok*cosik;
  ux=xmx*sinuk+cosnok*cosuk;
  uy=xmy*sinuk+sinnok*cosuk;
  uz=sinik*sinuk;
  vx=xmx*cosuk-cosnok*sinuk;
  vy=xmy*cosuk-sinnok*sinuk;
  vz=sinik*cosuk;
  
  /* Position and velocity */
  pos->x=rk*ux;
  pos->y=rk*uy;
  pos->z=rk*uz;
  vel->x=rdotk*ux+rfdotk*vx;
  vel->y=rdotk*uy+rfdotk*vy;
  vel->z=rdotk*uz+rfdotk*vz;
  
  /* Calculations for squint angle begin here... */
  
  if (calc_squint){
    bx=cos(alat)*cos(alon+deep_arg.omgadf);
    by=cos(alat)*sin(alon+deep_arg.omgadf);
    bz=sin(alat);
    cx=bx;
    cy=by*cos(xinck)-bz*sin(xinck);
    cz=by*sin(xinck)+bz*cos(xinck);
    ax=cx*cos(xnodek)-cy*sin(xnodek);
    ay=cx*sin(xnodek)+cy*cos(xnodek);
    az=cz;
  }
  
  /* Phase in radians */
  phase=xlt-deep_arg.xnode-deep_arg.omgadf+twopi;
  
  if (phase<0.0)
    phase+=twopi;
  
  phase=FMod2p(phase);
}

void SGP4(double tsince, tle_t * tle, vector_t * pos, vector_t * vel){
  /* This function is used to calculate the position and velocity */
  /* of near-earth (period < 225 minutes) satellites. tsince is   */
  /* time since epoch in minutes, tle is a pointer to a tle_t     */
  /* structure with Keplerian orbital elements and pos and vel    */
  /* are vector_t structures returning ECI satellite position and */ 
  /* velocity. Use Convert_Sat_State() to convert to km and km/s. */
  
  static double aodp, aycof, c1, c4, c5, cosio, d2, d3, d4, delmo,
    omgcof, eta, omgdot, sinio, xnodp, sinmo, t2cof, t3cof, t4cof,
    t5cof, x1mth2, x3thm1, x7thm1, xmcof, xmdot, xnodcf, xnodot, xlcof;
  
  double cosuk, sinuk, rfdotk, vx, vy, vz, ux, uy, uz, xmy, xmx, cosnok,
    sinnok, cosik, sinik, rdotk, xinck, xnodek, uk, rk, cos2u, sin2u,
    u, sinu, cosu, betal, rfdot, rdot, r, pl, elsq, esine, ecose, epw,
    cosepw, x1m5th, xhdot1, tfour, sinepw, capu, ayn, xlt, aynl, xll,
    axn, xn, beta, xl, e, a, tcube, delm, delomg, templ, tempe, tempa,
    xnode, tsq, xmp, omega, xnoddf, omgadf, xmdf, a1, a3ovk2, ao,
    betao, betao2, c1sq, c2, c3, coef, coef1, del1, delo, eeta, eosq,
    etasq, perigee, pinvsq, psisq, qoms24, s4, temp, temp1, temp2,
    temp3, temp4, temp5, temp6, theta2, theta4, tsi;
  
  int i;
  
  /* Initialization */
  
  if (isFlagClear(SGP4_INITIALIZED_FLAG)){
    SetFlag(SGP4_INITIALIZED_FLAG);
    
    /* Recover original mean motion (xnodp) and   */
    /* semimajor axis (aodp) from input elements. */
    
    a1=pow(xke/tle->xno,tothrd);
    cosio=cos(tle->xincl);
    theta2=cosio*cosio;
    x3thm1=3*theta2-1.0;
    eosq=tle->eo*tle->eo;
    betao2=1.0-eosq;
    betao=sqrt(betao2);
    del1=1.5*ck2*x3thm1/(a1*a1*betao*betao2);
    ao=a1*(1.0-del1*(0.5*tothrd+del1*(1.0+134.0/81.0*del1)));
    delo=1.5*ck2*x3thm1/(ao*ao*betao*betao2);
    xnodp=tle->xno/(1.0+delo);
    aodp=ao/(1.0-delo);
    
    /* For perigee less than 220 kilometers, the "simple"     */
    /* flag is set and the equations are truncated to linear  */
    /* variation in sqrt a and quadratic variation in mean    */
    /* anomaly.  Also, the c3 term, the delta omega term, and */
    /* the delta m term are dropped.                          */
    
    if ((aodp*(1-tle->eo)/ae)<(220/xkmper+ae))
      SetFlag(SIMPLE_FLAG);
    
    else
      ClearFlag(SIMPLE_FLAG);
    
    /* For perigees below 156 km, the      */
    /* values of s and qoms2t are altered. */
    
    s4=S;
    qoms24=qoms2t;
    perigee=(aodp*(1-tle->eo)-ae)*xkmper;
    
    if (perigee<156.0){
      if (perigee<=98.0)
	s4=20;
      else
	s4=perigee-78.0;
      
      qoms24=pow((120-s4)*ae/xkmper,4);
      s4=s4/xkmper+ae;
    }
    
    pinvsq=1/(aodp*aodp*betao2*betao2);
    tsi=1/(aodp-s4);
    eta=aodp*tle->eo*tsi;
    etasq=eta*eta;
    eeta=tle->eo*eta;
    psisq=fabs(1-etasq);
    coef=qoms24*pow(tsi,4);
    coef1=coef/pow(psisq,3.5);
    c2=coef1*xnodp*(aodp*(1+1.5*etasq+eeta*(4+etasq))+0.75*ck2*tsi/psisq*x3thm1*(8+3*etasq*(8+etasq)));
    c1=tle->bstar*c2;
    sinio=sin(tle->xincl);
    a3ovk2=-xj3/ck2*pow(ae,3);
    c3=coef*tsi*a3ovk2*xnodp*ae*sinio/tle->eo;
    x1mth2=1-theta2;
    
    c4=2*xnodp*coef1*aodp*betao2*(eta*(2+0.5*etasq)+tle->eo*(0.5+2*etasq)-2*ck2*tsi/(aodp*psisq)*(-3*x3thm1*(1-2*eeta+etasq*(1.5-0.5*eeta))+0.75*x1mth2*(2*etasq-eeta*(1+etasq))*cos(2*tle->omegao)));
    c5=2*coef1*aodp*betao2*(1+2.75*(etasq+eeta)+eeta*etasq);
    
    theta4=theta2*theta2;
    temp1=3*ck2*pinvsq*xnodp;
    temp2=temp1*ck2*pinvsq;
    temp3=1.25*ck4*pinvsq*pinvsq*xnodp;
    xmdot=xnodp+0.5*temp1*betao*x3thm1+0.0625*temp2*betao*(13-78*theta2+137*theta4);
    x1m5th=1-5*theta2;
    omgdot=-0.5*temp1*x1m5th+0.0625*temp2*(7-114*theta2+395*theta4)+temp3*(3-36*theta2+49*theta4);
    xhdot1=-temp1*cosio;
    xnodot=xhdot1+(0.5*temp2*(4-19*theta2)+2*temp3*(3-7*theta2))*cosio;
    omgcof=tle->bstar*c3*cos(tle->omegao);
    xmcof=-tothrd*coef*tle->bstar*ae/eeta;
    xnodcf=3.5*betao2*xhdot1*c1;
    t2cof=1.5*c1;
    xlcof=0.125*a3ovk2*sinio*(3+5*cosio)/(1+cosio);
    aycof=0.25*a3ovk2*sinio;
    delmo=pow(1+eta*cos(tle->xmo),3);
    sinmo=sin(tle->xmo);
    x7thm1=7*theta2-1;
    
    if (isFlagClear(SIMPLE_FLAG)){
      c1sq=c1*c1;
      d2=4*aodp*tsi*c1sq;
      temp=d2*tsi*c1/3;
      d3=(17*aodp+s4)*temp;
      d4=0.5*temp*aodp*tsi*(221*aodp+31*s4)*c1;
      t3cof=d2+2*c1sq;
      t4cof=0.25*(3*d3+c1*(12*d2+10*c1sq));
      t5cof=0.2*(3*d4+12*c1*d3+6*d2*d2+15*c1sq*(2*d2+c1sq));
    }
  }
  
  /* Update for secular gravity and atmospheric drag. */
  xmdf=tle->xmo+xmdot*tsince;
  omgadf=tle->omegao+omgdot*tsince;
  xnoddf=tle->xnodeo+xnodot*tsince;
  omega=omgadf;
  xmp=xmdf;
  tsq=tsince*tsince;
  xnode=xnoddf+xnodcf*tsq;
  tempa=1-c1*tsince;
  tempe=tle->bstar*c4*tsince;
  templ=t2cof*tsq;
  
  if (isFlagClear(SIMPLE_FLAG)){
    delomg=omgcof*tsince;
    delm=xmcof*(pow(1+eta*cos(xmdf),3)-delmo);
    temp=delomg+delm;
    xmp=xmdf+temp;
    omega=omgadf-temp;
    tcube=tsq*tsince;
    tfour=tsince*tcube;
    tempa=tempa-d2*tsq-d3*tcube-d4*tfour;
    tempe=tempe+tle->bstar*c5*(sin(xmp)-sinmo);
    templ=templ+t3cof*tcube+tfour*(t4cof+tsince*t5cof);
  }
  
  a=aodp*pow(tempa,2);
  e=tle->eo-tempe;
  xl=xmp+omega+xnode+xnodp*templ;
  beta=sqrt(1-e*e);
  xn=xke/pow(a,1.5);
  
  /* Long period periodics */
  axn=e*cos(omega);
  temp=1/(a*beta*beta);
  xll=temp*xlcof*axn;
  aynl=temp*aycof;
  xlt=xl+xll;
  ayn=e*sin(omega)+aynl;
  
  /* Solve Kepler's Equation */
  capu=FMod2p(xlt-xnode);
  temp2=capu;
  i=0;
  
  do{
    sinepw=sin(temp2);
    cosepw=cos(temp2);
    temp3=axn*sinepw;
    temp4=ayn*cosepw;
    temp5=axn*cosepw;
    temp6=ayn*sinepw;
    epw=(capu-temp4+temp3-temp2)/(1-temp5-temp6)+temp2;
    
    if (fabs(epw-temp2)<= e6a)
      break;
    
    temp2=epw;
    
  } while (i++<10);
  
  /* Short period preliminary quantities */
  ecose=temp5+temp6;
  esine=temp3-temp4;
  elsq=axn*axn+ayn*ayn;
  temp=1-elsq;
  pl=a*temp;
  r=a*(1-ecose);
  temp1=1/r;
  rdot=xke*sqrt(a)*esine*temp1;
  rfdot=xke*sqrt(pl)*temp1;
  temp2=a*temp1;
  betal=sqrt(temp);
  temp3=1/(1+betal);
  cosu=temp2*(cosepw-axn+ayn*esine*temp3);
  sinu=temp2*(sinepw-ayn-axn*esine*temp3);
  u=AcTan(sinu,cosu);
  sin2u=2*sinu*cosu;
  cos2u=2*cosu*cosu-1;
  temp=1/pl;
  temp1=ck2*temp;
  temp2=temp1*temp;
  
  /* Update for short periodics */
  rk=r*(1-1.5*temp2*betal*x3thm1)+0.5*temp1*x1mth2*cos2u;
  uk=u-0.25*temp2*x7thm1*sin2u;
  xnodek=xnode+1.5*temp2*cosio*sin2u;
  xinck=tle->xincl+1.5*temp2*cosio*sinio*cos2u;
  rdotk=rdot-xn*temp1*x1mth2*sin2u;
  rfdotk=rfdot+xn*temp1*(x1mth2*cos2u+1.5*x3thm1);
  
  /* Orientation vectors */
  sinuk=sin(uk);
  cosuk=cos(uk);
  sinik=sin(xinck);
  cosik=cos(xinck);
  sinnok=sin(xnodek);
  cosnok=cos(xnodek);
  xmx=-sinnok*cosik;
  xmy=cosnok*cosik;
  ux=xmx*sinuk+cosnok*cosuk;
  uy=xmy*sinuk+sinnok*cosuk;
  uz=sinik*sinuk;
  vx=xmx*cosuk-cosnok*sinuk;
  vy=xmy*cosuk-sinnok*sinuk;
  vz=sinik*cosuk;
  
  /* Position and velocity */
  pos->x=rk*ux;
  pos->y=rk*uy;
  pos->z=rk*uz;
  vel->x=rdotk*ux+rfdotk*vx;
  vel->y=rdotk*uy+rfdotk*vy;
  vel->z=rdotk*uz+rfdotk*vz;
  
  /* Phase in radians */
  phase=xlt-xnode-omgadf+twopi;
  
  if (phase<0.0)
    phase+=twopi;
  
  phase=FMod2p(phase);
}

void Convert_Sat_State(vector_t *pos, vector_t *vel){
  /* Converts the satellite's position and velocity  */
  /* vectors from normalized values to km and km/sec */ 
  Scale_Vector(xkmper, pos);
  Scale_Vector(xkmper*xmnpda/secday, vel);
}

void Magnitude(vector_t *v){
  /* Calculates scalar magnitude of a vector_t argument */
  v->w=sqrt(Sqr(v->x)+Sqr(v->y)+Sqr(v->z));
}

void Calculate_Obs(double time, vector_t *pos, vector_t *vel, geodetic_t *geodetic, vector_t *obs_set){
  /* The procedures Calculate_Obs and Calculate_RADec calculate         */
  /* the *topocentric* coordinates of the object with ECI position,     */
  /* {pos}, and velocity, {vel}, from location {geodetic} at {time}.    */
  /* The {obs_set} returned for Calculate_Obs consists of azimuth,      */
  /* elevation, range, and range rate (in that order) with units of     */
  /* radians, radians, kilometers, and kilometers/second, respectively. */
  /* The WGS '72 geoid is used and the effect of atmospheric refraction */
  /* (under standard temperature and pressure) is incorporated into the */
  /* elevation calculation; the effect of atmospheric refraction on     */
  /* range and range rate has not yet been quantified.                  */
  
  /* The {obs_set} for Calculate_RADec consists of right ascension and  */
  /* declination (in that order) in radians.  Again, calculations are   */
  /* based on *topocentric* position using the WGS '72 geoid and        */
  /* incorporating atmospheric refraction.                              */
  
  double sin_lat, cos_lat, sin_theta, cos_theta, el, azim, top_s, top_e, top_z;
  
  vector_t obs_pos, obs_vel, range, rgvel;
  
  Calculate_User_PosVel(time, geodetic, &obs_pos, &obs_vel);
  
  range.x=pos->x-obs_pos.x;
  range.y=pos->y-obs_pos.y;
  range.z=pos->z-obs_pos.z;
  
  /* Save these values globally for calculating squint angles later... */
  
  rx=range.x;
  ry=range.y;
  rz=range.z;
  
  rgvel.x=vel->x-obs_vel.x;
  rgvel.y=vel->y-obs_vel.y;
  rgvel.z=vel->z-obs_vel.z;
  
  Magnitude(&range);
  
  sin_lat=sin(geodetic->lat);
  cos_lat=cos(geodetic->lat);
  sin_theta=sin(geodetic->theta);
  cos_theta=cos(geodetic->theta);
  top_s=sin_lat*cos_theta*range.x+sin_lat*sin_theta*range.y-cos_lat*range.z;
  top_e=-sin_theta*range.x+cos_theta*range.y;
  top_z=cos_lat*cos_theta*range.x+cos_lat*sin_theta*range.y+sin_lat*range.z;
  azim=atan(-top_e/top_s); /* Azimuth */
  
  if (top_s>0.0) 
    azim=azim+pi;
  
  if (azim<0.0)
    azim=azim+twopi;
  
  el=ArcSin(top_z/range.w);
  obs_set->x=azim;	/* Azimuth (radians)   */
  obs_set->y=el;		/* Elevation (radians) */
  obs_set->z=range.w;	/* Range (kilometers)  */
  
  /* Range Rate (kilometers/second) */
  
  obs_set->w=Dot(&range,&rgvel)/range.w;
  
  /* Corrections for atmospheric refraction */
  /* Reference:  Astronomical Algorithms by Jean Meeus, pp. 101-104    */
  /* Correction is meaningless when apparent elevation is below horizon */
  
  /*** The following adjustment for
       atmospheric refraction is bypassed ***/
  
  /* obs_set->y=obs_set->y+Radians((1.02/tan(Radians(Degrees(el)+10.3/(Degrees(el)+5.11))))/60); */
  
  obs_set->y=el;
  
  /**** End bypass ****/
  
  if (obs_set->y>=0.0)
    SetFlag(VISIBLE_FLAG);
  else{
    obs_set->y=el;  /* Reset to true elevation */
    ClearFlag(VISIBLE_FLAG);
  }
}

void Calculate_Solar_Position(double time, vector_t *solar_vector){
  /* Calculates solar position vector */
  
  double mjd, year, T, M, L, e, C, O, Lsa, nu, R, eps;
  
  mjd=time-2415020.0;
  year=1900+mjd/365.25;
  T=(mjd+Delta_ET(year)/secday)/36525.0;
  M=Radians(Modulus(358.47583+Modulus(35999.04975*T,360.0)-(0.000150+0.0000033*T)*Sqr(T),360.0));
  L=Radians(Modulus(279.69668+Modulus(36000.76892*T,360.0)+0.0003025*Sqr(T),360.0));
  e=0.01675104-(0.0000418+0.000000126*T)*T;
  C=Radians((1.919460-(0.004789+0.000014*T)*T)*sin(M)+(0.020094-0.000100*T)*sin(2*M)+0.000293*sin(3*M));
  O=Radians(Modulus(259.18-1934.142*T,360.0));
  Lsa=Modulus(L+C-Radians(0.00569-0.00479*sin(O)),twopi);
  nu=Modulus(M+C,twopi);
  R=1.0000002*(1.0-Sqr(e))/(1.0+e*cos(nu));
  eps=Radians(23.452294-(0.0130125+(0.00000164-0.000000503*T)*T)*T+0.00256*cos(O));
  R=AU*R;
  solar_vector->x=R*cos(Lsa);
  solar_vector->y=R*sin(Lsa)*cos(eps);
  solar_vector->z=R*sin(Lsa)*sin(eps);
  solar_vector->w=R;
}

void Calculate_LatLonAlt(double time, vector_t *pos,  geodetic_t *geodetic){
  /* Procedure Calculate_LatLonAlt will calculate the geodetic  */
  /* position of an object given its ECI position pos and time. */
  /* It is intended to be used to determine the ground track of */
  /* a satellite.  The calculations  assume the earth to be an  */
  /* oblate spheroid as defined in WGS '72.                     */
  
  /* Reference:  The 1992 Astronomical Almanac, page K12. */
  
  double r, e2, phi, c;
  
  geodetic->theta=AcTan(pos->y,pos->x); /* radians */
  geodetic->lon=FMod2p(geodetic->theta-ThetaG_JD(time)); /* radians */
  r=sqrt(Sqr(pos->x)+Sqr(pos->y));
  e2=f*(2-f);
  geodetic->lat=AcTan(pos->z,r); /* radians */
  
  do{
    phi=geodetic->lat;
    c=1/sqrt(1-e2*Sqr(sin(phi)));
    geodetic->lat=AcTan(pos->z+xkmper*c*e2*sin(phi),r);
    
  } while (fabs(geodetic->lat-phi)>=1E-10);
  
  geodetic->alt=r/cos(geodetic->lat)-xkmper*c; /* kilometers */
  
  if (geodetic->lat>pio2)
    geodetic->lat-=twopi;
}

int Sat_Eclipsed(vector_t *pos, vector_t *sol, double *depth){
  /* Calculates satellite's eclipse status and depth */
  
  double sd_sun, sd_earth, delta;
  vector_t Rho, earth;
  
  /* Determine partial eclipse */
  
  sd_earth=ArcSin(xkmper/pos->w);
  Vec_Sub(sol,pos,&Rho);
  sd_sun=ArcSin(sr/Rho.w);
  Scalar_Multiply(-1,pos,&earth);
  delta=Angle(sol,&earth);
  *depth=sd_earth-sd_sun-delta;
  
  if (sd_earth<sd_sun)
    return 0;
  else
    if (*depth>=0)
      return 1;
    else
      return 0;
}

double Radians(double arg){
  /* Returns angle in radians from argument in degrees */
  return (arg*deg2rad);
}

double Degrees(double arg){
  /* Returns angle in degrees from argument in radians */
  return (arg/deg2rad);
}

double FixAngle( double x){
  /* This function reduces angles greater than
     two pi by subtracting two pi from the angle */
  
  while (x>twopi)
    x-=twopi;
  
  return x;
}

void bailout(char* string){
  /* This function quits ncurses, resets and "beeps"
     the terminal, and displays an error message (string)
     when we need to bail out of the program in a hurry. */
  
  //  beep();	
  //  curs_set(1);
  //  bkgdset(COLOR_PAIR(1));
  //  clear();
  //  refresh();
  //  endwin();
  fprintf(stderr,"*** predict: %s!\n",string);
}

void Calculate_RADec(double time, vector_t *pos, vector_t *vel, geodetic_t *geodetic, vector_t *obs_set){
  /* Reference:  Methods of Orbit Determination by  */
  /*             Pedro Ramon Escobal, pp. 401-402   */
  
  double	phi, theta, sin_theta, cos_theta, sin_phi, cos_phi, az, el,
    Lxh, Lyh, Lzh, Sx, Ex, Zx, Sy, Ey, Zy, Sz, Ez, Zz, Lx, Ly,
    Lz, cos_delta, sin_alpha, cos_alpha;
  
  Calculate_Obs(time,pos,vel,geodetic,obs_set);
  
  az=obs_set->x;
  el=obs_set->y;
  phi=geodetic->lat;
  theta=FMod2p(ThetaG_JD(time)+geodetic->lon);
  sin_theta=sin(theta);
  cos_theta=cos(theta);
  sin_phi=sin(phi);
  cos_phi=cos(phi);
  Lxh=-cos(az)*cos(el);
  Lyh=sin(az)*cos(el);
  Lzh=sin(el);
  Sx=sin_phi*cos_theta;
  Ex=-sin_theta;
  Zx=cos_theta*cos_phi;
  Sy=sin_phi*sin_theta;
  Ey=cos_theta;
  Zy=sin_theta*cos_phi;
  Sz=-cos_phi;
  Ez=0.0;
  Zz=sin_phi;
  Lx=Sx*Lxh+Ex*Lyh+Zx*Lzh;
  Ly=Sy*Lxh+Ey*Lyh+Zy*Lzh;
  Lz=Sz*Lxh+Ez*Lyh+Zz*Lzh;
  obs_set->y=ArcSin(Lz);  /* Declination (radians) */
  cos_delta=sqrt(1.0-Sqr(Lz));
  sin_alpha=Ly/cos_delta;
  cos_alpha=Lx/cos_delta;
  obs_set->x=AcTan(sin_alpha,cos_alpha); /* Right Ascension (radians) */
  obs_set->x=FMod2p(obs_set->x);
}

int isFlagClear(int flag){
  return (~Flags&flag);
}

void Deep(int ientry, tle_t * tle, deep_arg_t * deep_arg){
  /* This function is used by SDP4 to add lunar and solar */
  /* perturbation effects to deep-space orbit objects.    */
  
  static double thgr, xnq, xqncl, omegaq, zmol, zmos, savtsn, ee2, e3,
    xi2, xl2, xl3, xl4, xgh2, xgh3, xgh4, xh2, xh3, sse, ssi, ssg, xi3,
    se2, si2, sl2, sgh2, sh2, se3, si3, sl3, sgh3, sh3, sl4, sgh4, ssl,
    ssh, d3210, d3222, d4410, d4422, d5220, d5232, d5421, d5433, del1,
    del2, del3, fasx2, fasx4, fasx6, xlamo, xfact, xni, atime, stepp,
    stepn, step2, preep, pl, sghs, xli, d2201, d2211, sghl, sh1, pinc,
    pe, shs, zsingl, zcosgl, zsinhl, zcoshl, zsinil, zcosil;
  
  double a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, ainv2, alfdp, aqnv,
    sgh, sini2, sinis, sinok, sh, si, sil, day, betdp, dalf, bfact, c,
    cc, cosis, cosok, cosq, ctem, f322, zx, zy, dbet, dls, eoc, eq, f2,
    f220, f221, f3, f311, f321, xnoh, f330, f441, f442, f522, f523,
    f542, f543, g200, g201, g211, pgh, ph, s1, s2, s3, s4, s5, s6, s7,
    se, sel, ses, xls, g300, g310, g322, g410, g422, g520, g521, g532,
    g533, gam, sinq, sinzf, sis, sl, sll, sls, stem, temp, temp1, x1,
    x2, x2li, x2omi, x3, x4, x5, x6, x7, x8, xl, xldot, xmao, xnddt,
    xndot, xno2, xnodce, xnoi, xomi, xpidot, z1, z11, z12, z13, z2,
    z21, z22, z23, z3, z31, z32, z33, ze, zf, zm, zmo, zn, zsing,
    zsinh, zsini, zcosg, zcosh, zcosi, delt=0, ft=0;
  
  switch (ientry){
  case dpinit:  /* Entrance for deep space initialization */
    thgr=ThetaG(tle->epoch,deep_arg);
    eq=tle->eo;
    xnq=deep_arg->xnodp;
    aqnv=1/deep_arg->aodp;
    xqncl=tle->xincl;
    xmao=tle->xmo;
    xpidot=deep_arg->omgdot+deep_arg->xnodot;
    sinq=sin(tle->xnodeo);
    cosq=cos(tle->xnodeo);
    omegaq=tle->omegao;
    
    /* Initialize lunar solar terms */
    day=deep_arg->ds50+18261.5;  /* Days since 1900 Jan 0.5 */
    
    if (day!=preep){
      preep=day;
      xnodce=4.5236020-9.2422029E-4*day;
      stem=sin(xnodce);
      ctem=cos(xnodce);
      zcosil=0.91375164-0.03568096*ctem;
      zsinil=sqrt(1-zcosil*zcosil);
      zsinhl=0.089683511*stem/zsinil;
      zcoshl=sqrt(1-zsinhl*zsinhl);
      c=4.7199672+0.22997150*day;
      gam=5.8351514+0.0019443680*day;
      zmol=FMod2p(c-gam);
      zx=0.39785416*stem/zsinil;
      zy=zcoshl*ctem+0.91744867*zsinhl*stem;
      zx=AcTan(zx,zy);
      zx=gam+zx-xnodce;
      zcosgl=cos(zx);
      zsingl=sin(zx);
      zmos=6.2565837+0.017201977*day;
      zmos=FMod2p(zmos);
    }
    
    /* Do solar terms */
    savtsn=1E20;
    zcosg=zcosgs;
    zsing=zsings;
    zcosi=zcosis;
    zsini=zsinis;
    zcosh=cosq;
    zsinh= sinq;
    cc=c1ss;
    zn=zns;
    ze=zes;
    zmo=zmos;
    xnoi=1/xnq;
    
    /* Loop breaks when Solar terms are done a second */
    /* time, after Lunar terms are initialized        */
    
    for (;;){
      /* Solar terms done again after Lunar terms are done */
      a1=zcosg*zcosh+zsing*zcosi*zsinh;
      a3=-zsing*zcosh+zcosg*zcosi*zsinh;
      a7=-zcosg*zsinh+zsing*zcosi*zcosh;
      a8=zsing*zsini;
      a9=zsing*zsinh+zcosg*zcosi*zcosh;
      a10=zcosg*zsini;
      a2=deep_arg->cosio*a7+deep_arg->sinio*a8;
      a4=deep_arg->cosio*a9+deep_arg->sinio*a10;
      a5=-deep_arg->sinio*a7+deep_arg->cosio*a8;
      a6=-deep_arg->sinio*a9+deep_arg->cosio*a10;
      x1=a1*deep_arg->cosg+a2*deep_arg->sing;
      x2=a3*deep_arg->cosg+a4*deep_arg->sing;
      x3=-a1*deep_arg->sing+a2*deep_arg->cosg;
      x4=-a3*deep_arg->sing+a4*deep_arg->cosg;
      x5=a5*deep_arg->sing;
      x6=a6*deep_arg->sing;
      x7=a5*deep_arg->cosg;
      x8=a6*deep_arg->cosg;
      z31=12*x1*x1-3*x3*x3;
      z32=24*x1*x2-6*x3*x4;
      z33=12*x2*x2-3*x4*x4;
      z1=3*(a1*a1+a2*a2)+z31*deep_arg->eosq;
      z2=6*(a1*a3+a2*a4)+z32*deep_arg->eosq;
      z3=3*(a3*a3+a4*a4)+z33*deep_arg->eosq;
      z11=-6*a1*a5+deep_arg->eosq*(-24*x1*x7-6*x3*x5);
      z12=-6*(a1*a6+a3*a5)+deep_arg->eosq*(-24*(x2*x7+x1*x8)-6*(x3*x6+x4*x5));
      z13=-6*a3*a6+deep_arg->eosq*(-24*x2*x8-6*x4*x6);
      z21=6*a2*a5+deep_arg->eosq*(24*x1*x5-6*x3*x7);
      z22=6*(a4*a5+a2*a6)+deep_arg->eosq*(24*(x2*x5+x1*x6)-6*(x4*x7+x3*x8));
      z23=6*a4*a6+deep_arg->eosq*(24*x2*x6-6*x4*x8);
      z1=z1+z1+deep_arg->betao2*z31;
      z2=z2+z2+deep_arg->betao2*z32;
      z3=z3+z3+deep_arg->betao2*z33;
      s3=cc*xnoi;
      s2=-0.5*s3/deep_arg->betao;
      s4=s3*deep_arg->betao;
      s1=-15*eq*s4;
      s5=x1*x3+x2*x4;
      s6=x2*x3+x1*x4;
      s7=x2*x4-x1*x3;
      se=s1*zn*s5;
      si=s2*zn*(z11+z13);
      sl=-zn*s3*(z1+z3-14-6*deep_arg->eosq);
      sgh=s4*zn*(z31+z33-6);
      sh=-zn*s2*(z21+z23);
      
      if (xqncl<5.2359877E-2)
	sh=0;
      
      ee2=2*s1*s6;
      e3=2*s1*s7;
      xi2=2*s2*z12;
      xi3=2*s2*(z13-z11);
      xl2=-2*s3*z2;
      xl3=-2*s3*(z3-z1);
      xl4=-2*s3*(-21-9*deep_arg->eosq)*ze;
      xgh2=2*s4*z32;
      xgh3=2*s4*(z33-z31);
      xgh4=-18*s4*ze;
      xh2=-2*s2*z22;
      xh3=-2*s2*(z23-z21);
      
      if (isFlagSet(LUNAR_TERMS_DONE_FLAG))
	break;
      
      /* Do lunar terms */
      sse=se;
      ssi=si;
      ssl=sl;
      ssh=sh/deep_arg->sinio;
      ssg=sgh-deep_arg->cosio*ssh;
      se2=ee2;
      si2=xi2;
      sl2=xl2;
      sgh2=xgh2;
      sh2=xh2;
      se3=e3;
      si3=xi3;
      sl3=xl3;
      sgh3=xgh3;
      sh3=xh3;
      sl4=xl4;
      sgh4=xgh4;
      zcosg=zcosgl;
      zsing=zsingl;
      zcosi=zcosil;
      zsini=zsinil;
      zcosh=zcoshl*cosq+zsinhl*sinq;
      zsinh=sinq*zcoshl-cosq*zsinhl;
      zn=znl;
      cc=c1l;
      ze=zel;
      zmo=zmol;
      SetFlag(LUNAR_TERMS_DONE_FLAG);
    }
    
    sse=sse+se;
    ssi=ssi+si;
    ssl=ssl+sl;
    ssg=ssg+sgh-deep_arg->cosio/deep_arg->sinio*sh;
    ssh=ssh+sh/deep_arg->sinio;
    
    /* Geopotential resonance initialization for 12 hour orbits */
    ClearFlag(RESONANCE_FLAG);
    ClearFlag(SYNCHRONOUS_FLAG);
    
    if (!((xnq<0.0052359877) && (xnq>0.0034906585))){
      if ((xnq<0.00826) || (xnq>0.00924))
	return;
      
      if (eq<0.5)
	return;
      
      SetFlag(RESONANCE_FLAG);
      eoc=eq*deep_arg->eosq;
      g201=-0.306-(eq-0.64)*0.440;
      
      if (eq<=0.65){
	g211=3.616-13.247*eq+16.290*deep_arg->eosq;
	g310=-19.302+117.390*eq-228.419*deep_arg->eosq+156.591*eoc;
	g322=-18.9068+109.7927*eq-214.6334*deep_arg->eosq+146.5816*eoc;
	g410=-41.122+242.694*eq-471.094*deep_arg->eosq+313.953*eoc;
	g422=-146.407+841.880*eq-1629.014*deep_arg->eosq+1083.435 * eoc;
	g520=-532.114+3017.977*eq-5740*deep_arg->eosq+3708.276*eoc;
      }
      
      else
	{
	  g211=-72.099+331.819*eq-508.738*deep_arg->eosq+266.724*eoc;
	  g310=-346.844+1582.851*eq-2415.925*deep_arg->eosq+1246.113*eoc;
	  g322=-342.585+1554.908*eq-2366.899*deep_arg->eosq+1215.972*eoc;
	  g410=-1052.797+4758.686*eq-7193.992*deep_arg->eosq+3651.957*eoc;
	  g422=-3581.69+16178.11*eq-24462.77*deep_arg->eosq+12422.52*eoc;
	  
	  if (eq<=0.715)
	    g520=1464.74-4664.75*eq+3763.64*deep_arg->eosq;
	  
	  else
	    g520=-5149.66+29936.92*eq-54087.36*deep_arg->eosq+31324.56*eoc;
	}
      
      if (eq<0.7){
	g533=-919.2277+4988.61*eq-9064.77*deep_arg->eosq+5542.21*eoc;
	g521=-822.71072+4568.6173*eq-8491.4146*deep_arg->eosq+5337.524*eoc;
	g532=-853.666+4690.25*eq-8624.77*deep_arg->eosq+5341.4*eoc;
      }
      
      else{
	g533=-37995.78+161616.52*eq-229838.2*deep_arg->eosq+109377.94*eoc;
	g521 =-51752.104+218913.95*eq-309468.16*deep_arg->eosq+146349.42*eoc;
	g532 =-40023.88+170470.89*eq-242699.48*deep_arg->eosq+115605.82*eoc;
      }
      
      sini2=deep_arg->sinio*deep_arg->sinio;
      f220=0.75*(1+2*deep_arg->cosio+deep_arg->theta2);
      f221=1.5*sini2;
      f321=1.875*deep_arg->sinio*(1-2*deep_arg->cosio-3*deep_arg->theta2);
      f322=-1.875*deep_arg->sinio*(1+2*deep_arg->cosio-3*deep_arg->theta2);
      f441=35*sini2*f220;
      f442=39.3750*sini2*sini2;
      f522=9.84375*deep_arg->sinio*(sini2*(1-2*deep_arg->cosio-5*deep_arg->theta2)+0.33333333*(-2+4*deep_arg->cosio+6*deep_arg->theta2));
      f523=deep_arg->sinio*(4.92187512*sini2*(-2-4*deep_arg->cosio+10*deep_arg->theta2)+6.56250012*(1+2*deep_arg->cosio-3*deep_arg->theta2));
      f542=29.53125*deep_arg->sinio*(2-8*deep_arg->cosio+deep_arg->theta2*(-12+8*deep_arg->cosio+10*deep_arg->theta2));
      f543=29.53125*deep_arg->sinio*(-2-8*deep_arg->cosio+deep_arg->theta2*(12+8*deep_arg->cosio-10*deep_arg->theta2));
      xno2=xnq*xnq;
      ainv2=aqnv*aqnv;
      temp1=3*xno2*ainv2;
      temp=temp1*root22;
      d2201=temp*f220*g201;
      d2211=temp*f221*g211;
      temp1=temp1*aqnv;
      temp=temp1*root32;
      d3210=temp*f321*g310;
      d3222=temp*f322*g322;
      temp1=temp1*aqnv;
      temp=2*temp1*root44;
      d4410=temp*f441*g410;
      d4422=temp*f442*g422;
      temp1=temp1*aqnv;
      temp=temp1*root52;
      d5220=temp*f522*g520;
      d5232=temp*f523*g532;
      temp=2*temp1*root54;
      d5421=temp*f542*g521;
      d5433=temp*f543*g533;
      xlamo=xmao+tle->xnodeo+tle->xnodeo-thgr-thgr;
      bfact=deep_arg->xmdot+deep_arg->xnodot+deep_arg->xnodot-thdt-thdt;
      bfact=bfact+ssl+ssh+ssh;
    }
    
    else{
      SetFlag(RESONANCE_FLAG);
      SetFlag(SYNCHRONOUS_FLAG);
      
      /* Synchronous resonance terms initialization */
      g200=1+deep_arg->eosq*(-2.5+0.8125*deep_arg->eosq);
      g310=1+2*deep_arg->eosq;
      g300=1+deep_arg->eosq*(-6+6.60937*deep_arg->eosq);
      f220=0.75*(1+deep_arg->cosio)*(1+deep_arg->cosio);
      f311=0.9375*deep_arg->sinio*deep_arg->sinio*(1+3*deep_arg->cosio)-0.75*(1+deep_arg->cosio);
      f330=1+deep_arg->cosio;
      f330=1.875*f330*f330*f330;
      del1=3*xnq*xnq*aqnv*aqnv;
      del2=2*del1*f220*g200*q22;
      del3=3*del1*f330*g300*q33*aqnv;
      del1=del1*f311*g310*q31*aqnv;
      fasx2=0.13130908;
      fasx4=2.8843198;
      fasx6=0.37448087;
      xlamo=xmao+tle->xnodeo+tle->omegao-thgr;
      bfact=deep_arg->xmdot+xpidot-thdt;
      bfact=bfact+ssl+ssg+ssh;
    }
    
    xfact=bfact-xnq;
    
    /* Initialize integrator */
    xli=xlamo;
    xni=xnq;
    atime=0;
    stepp=720;
    stepn=-720;
    step2=259200;
    
    return;
    
  case dpsec:  /* Entrance for deep space secular effects */
    deep_arg->xll=deep_arg->xll+ssl*deep_arg->t;
    deep_arg->omgadf=deep_arg->omgadf+ssg*deep_arg->t;
    deep_arg->xnode=deep_arg->xnode+ssh*deep_arg->t;
    deep_arg->em=tle->eo+sse*deep_arg->t;
    deep_arg->xinc=tle->xincl+ssi*deep_arg->t;
    
    if (deep_arg->xinc<0){
      deep_arg->xinc=-deep_arg->xinc;
      deep_arg->xnode=deep_arg->xnode+pi;
      deep_arg->omgadf=deep_arg->omgadf-pi;
    }
    
    if (isFlagClear(RESONANCE_FLAG))
      return;
    
    do{
      if ((atime==0) || ((deep_arg->t>=0) && (atime<0)) || ((deep_arg->t<0) && (atime>=0))){
	/* Epoch restart */
	
	if (deep_arg->t>=0)
	  delt=stepp;
	else
	  delt=stepn;
	
	atime=0;
	xni=xnq;
	xli=xlamo;
      }
      
      else{
	if (fabs(deep_arg->t)>=fabs(atime)){
	  if (deep_arg->t>0)
	    delt=stepp;
	  else
	    delt=stepn;
	}
      }
      
      do{
	if (fabs(deep_arg->t-atime)>=stepp){
	  SetFlag(DO_LOOP_FLAG);
	  ClearFlag(EPOCH_RESTART_FLAG);
	}
	
	else{
	  ft=deep_arg->t-atime;
	  ClearFlag(DO_LOOP_FLAG);
	}
	
	if (fabs(deep_arg->t)<fabs(atime)){
	  if (deep_arg->t>=0)
	    delt=stepn;
	  else
	    delt=stepp;
	  
	  SetFlag(DO_LOOP_FLAG | EPOCH_RESTART_FLAG);
	}
	
	/* Dot terms calculated */
	if (isFlagSet(SYNCHRONOUS_FLAG)){
	  xndot=del1*sin(xli-fasx2)+del2*sin(2*(xli-fasx4))+del3*sin(3*(xli-fasx6));
	  xnddt=del1*cos(xli-fasx2)+2*del2*cos(2*(xli-fasx4))+3*del3*cos(3*(xli-fasx6));
	}
	
	else{
	  xomi=omegaq+deep_arg->omgdot*atime;
	  x2omi=xomi+xomi;
	  x2li=xli+xli;
	  xndot=d2201*sin(x2omi+xli-g22)+d2211*sin(xli-g22)+d3210*sin(xomi+xli-g32)+d3222*sin(-xomi+xli-g32)+d4410*sin(x2omi+x2li-g44)+d4422*sin(x2li-g44)+d5220*sin(xomi+xli-g52)+d5232*sin(-xomi+xli-g52)+d5421*sin(xomi+x2li-g54)+d5433*sin(-xomi+x2li-g54);
	  xnddt=d2201*cos(x2omi+xli-g22)+d2211*cos(xli-g22)+d3210*cos(xomi+xli-g32)+d3222*cos(-xomi+xli-g32)+d5220*cos(xomi+xli-g52)+d5232*cos(-xomi+xli-g52)+2*(d4410*cos(x2omi+x2li-g44)+d4422*cos(x2li-g44)+d5421*cos(xomi+x2li-g54)+d5433*cos(-xomi+x2li-g54));
	}
	
	xldot=xni+xfact;
	xnddt=xnddt*xldot;
	
	if (isFlagSet(DO_LOOP_FLAG)){
	  xli=xli+xldot*delt+xndot*step2;
	  xni=xni+xndot*delt+xnddt*step2;
	  atime=atime+delt;
	}
      } while (isFlagSet(DO_LOOP_FLAG) && isFlagClear(EPOCH_RESTART_FLAG));
    } while (isFlagSet(DO_LOOP_FLAG) && isFlagSet(EPOCH_RESTART_FLAG));
    
    deep_arg->xn=xni+xndot*ft+xnddt*ft*ft*0.5;
    xl=xli+xldot*ft+xndot*ft*ft*0.5;
    temp=-deep_arg->xnode+thgr+deep_arg->t*thdt;
    
    if (isFlagClear(SYNCHRONOUS_FLAG))
      deep_arg->xll=xl+temp+temp;
    else
      deep_arg->xll=xl-deep_arg->omgadf+temp;
    
    return;
    
  case dpper:	 /* Entrance for lunar-solar periodics */
    sinis=sin(deep_arg->xinc);
    cosis=cos(deep_arg->xinc);
    
    if (fabs(savtsn-deep_arg->t)>=30){
      savtsn=deep_arg->t;
      zm=zmos+zns*deep_arg->t;
      zf=zm+2*zes*sin(zm);
      sinzf=sin(zf);
      f2=0.5*sinzf*sinzf-0.25;
      f3=-0.5*sinzf*cos(zf);
      ses=se2*f2+se3*f3;
      sis=si2*f2+si3*f3;
      sls=sl2*f2+sl3*f3+sl4*sinzf;
      sghs=sgh2*f2+sgh3*f3+sgh4*sinzf;
      shs=sh2*f2+sh3*f3;
      zm=zmol+znl*deep_arg->t;
      zf=zm+2*zel*sin(zm);
      sinzf=sin(zf);
      f2=0.5*sinzf*sinzf-0.25;
      f3=-0.5*sinzf*cos(zf);
      sel=ee2*f2+e3*f3;
      sil=xi2*f2+xi3*f3;
      sll=xl2*f2+xl3*f3+xl4*sinzf;
      sghl=xgh2*f2+xgh3*f3+xgh4*sinzf;
      sh1=xh2*f2+xh3*f3;
      pe=ses+sel;
      pinc=sis+sil;
      pl=sls+sll;
    }
    
    pgh=sghs+sghl;
    ph=shs+sh1;
    deep_arg->xinc=deep_arg->xinc+pinc;
    deep_arg->em=deep_arg->em+pe;
    
    if (xqncl>=0.2){
      /* Apply periodics directly */
      ph=ph/deep_arg->sinio;
      pgh=pgh-deep_arg->cosio*ph;
      deep_arg->omgadf=deep_arg->omgadf+pgh;
      deep_arg->xnode=deep_arg->xnode+ph;
      deep_arg->xll=deep_arg->xll+pl;
    }
    
    else{
      /* Apply periodics with Lyddane modification */
      sinok=sin(deep_arg->xnode);
      cosok=cos(deep_arg->xnode);
      alfdp=sinis*sinok;
      betdp=sinis*cosok;
      dalf=ph*cosok+pinc*cosis*sinok;
      dbet=-ph*sinok+pinc*cosis*cosok;
      alfdp=alfdp+dalf;
      betdp=betdp+dbet;
      deep_arg->xnode=FMod2p(deep_arg->xnode);
      xls=deep_arg->xll+deep_arg->omgadf+cosis*deep_arg->xnode;
      dls=pl+pgh-pinc*deep_arg->xnode*sinis;
      xls=xls+dls;
      xnoh=deep_arg->xnode;
      deep_arg->xnode=AcTan(alfdp,betdp);
      
      /* This is a patch to Lyddane modification */
      /* suggested by Rob Matson. */
      
      if (fabs(xnoh-deep_arg->xnode)>pi){
	if (deep_arg->xnode<xnoh)
	  deep_arg->xnode+=twopi;
	else
	  deep_arg->xnode-=twopi;
      }
      
      deep_arg->xll=deep_arg->xll+pl;
      deep_arg->omgadf=xls-deep_arg->xll-cos(deep_arg->xinc)*deep_arg->xnode;
    }
    return;
  }
}

double FMod2p(double x){
  /* Returns mod 2PI of argument */
  
  int i;
  double ret_val;
  
  ret_val=x;
  i=ret_val/twopi;
  ret_val-=i*twopi;
  
  if (ret_val<0.0)
    ret_val+=twopi;
  
  return ret_val;
}

double AcTan(double sinx, double cosx){
  /* Four-quadrant arctan function */
  
  if (cosx==0.0){
    if (sinx>0.0)
      return (pio2);
    else
      return (x3pio2);
  }
  
  else{
    if (cosx>0.0){
      if (sinx>0.0)
	return (atan(sinx/cosx));
      else
	return (twopi+atan(sinx/cosx));
    }
    
    else
      return (pi+atan(sinx/cosx));
  }
}

void Scale_Vector(double k, vector_t *v){ 
  /* Multiplies the vector v1 by the scalar k */
  v->x*=k;
  v->y*=k;
  v->z*=k;
  Magnitude(v);
}

double Sqr(double arg){
  /* Returns square of a double */
  return (arg*arg);
}

void Calculate_User_PosVel(double time, geodetic_t *geodetic, vector_t *obs_pos, vector_t *obs_vel){
  /* Calculate_User_PosVel() passes the user's geodetic position
     and the time of interest and returns the ECI position and
     velocity of the observer.  The velocity calculation assumes
     the geodetic position is stationary relative to the earth's
     surface. */
  
  /* Reference:  The 1992 Astronomical Almanac, page K11. */
  
  double c, sq, achcp;
  
  geodetic->theta=FMod2p(ThetaG_JD(time)+geodetic->lon); /* LMST */
  c=1/sqrt(1+f*(f-2)*Sqr(sin(geodetic->lat)));
  sq=Sqr(1-f)*c;
  achcp=(xkmper*c+geodetic->alt)*cos(geodetic->lat);
  obs_pos->x=achcp*cos(geodetic->theta); /* kilometers */
  obs_pos->y=achcp*sin(geodetic->theta);
  obs_pos->z=(xkmper*sq+geodetic->alt)*sin(geodetic->lat);
  obs_vel->x=-mfactor*obs_pos->y; /* kilometers/second */
  obs_vel->y=mfactor*obs_pos->x;
  obs_vel->z=0;
  Magnitude(obs_pos);
  Magnitude(obs_vel);
}

double ArcSin(double arg){
  /* Returns the arcsine of the argument */
  
  if (fabs(arg)>=1.0)
    return(Sign(arg)*pio2);
  else
    
    return(atan(arg/sqrt(1.0-arg*arg)));
}

double Dot(vector_t *v1, vector_t *v2){
  /* Returns the dot product of two vectors */
  return (v1->x*v2->x+v1->y*v2->y+v1->z*v2->z);
}

double Delta_ET(double year){
  /* The function Delta_ET has been added to allow calculations on   */
  /* the position of the sun.  It provides the difference between UT */
  /* (approximately the same as UTC) and ET (now referred to as TDT).*/
  /* This function is based on a least squares fit of data from 1950 */
  /* to 1991 and will need to be updated periodically. */
  
  /* Values determined using data from 1950-1991 in the 1990 
     Astronomical Almanac.  See DELTA_ET.WQ1 for details. */
  
  double delta_et;
  
  delta_et=26.465+0.747622*(year-1950)+1.886913*sin(twopi*(year-1975)/33);
  
  return delta_et;
}

double Modulus(double arg1, double arg2){
  /* Returns arg1 mod arg2 */
  
  int i;
  double ret_val;
  
  ret_val=arg1;
  i=ret_val/arg2;
  ret_val-=i*arg2;
  
  if (ret_val<0.0)
    ret_val+=arg2;
  
  return ret_val;
}

void Vec_Sub(vector_t *v1, vector_t *v2, vector_t *v3){
  /* Subtracts vector v2 from v1 to produce v3 */
  v3->x=v1->x-v2->x;
  v3->y=v1->y-v2->y;
  v3->z=v1->z-v2->z;
  Magnitude(v3);
}

void Scalar_Multiply(double k, vector_t *v1, vector_t *v2){
  /* Multiplies the vector v1 by the scalar k to produce the vector v2 */
  v2->x=k*v1->x;
  v2->y=k*v1->y;
  v2->z=k*v1->z;
  v2->w=fabs(k)*v1->w;
}

double Angle(vector_t *v1, vector_t *v2){
  /* Calculates the angle between vectors v1 and v2 */
  Magnitude(v1);
  Magnitude(v2);
  return(ArcCos(Dot(v1,v2)/(v1->w*v2->w)));
}

double ThetaG(double epoch, deep_arg_t *deep_arg){
  /* The function ThetaG calculates the Greenwich Mean Sidereal Time */
  /* for an epoch specified in the format used in the NORAD two-line */
  /* element sets. It has now been adapted for dates beyond the year */
  /* 1999, as described above. The function ThetaG_JD provides the   */
  /* same calculation except that it is based on an input in the     */
  /* form of a Julian Date. */
  
  /* Reference:  The 1992 Astronomical Almanac, page B6. */
  
  double year, day, UT, jd, TU, GMST, ThetaG;
  
  /* Modification to support Y2K */
  /* Valid 1957 through 2056     */
  
  day=modf(epoch*1E-3,&year)*1E3;
  
  if (year<57)
    year+=2000;
  else
    year+=1900;
  
  UT=modf(day,&day);
  jd=Julian_Date_of_Year(year)+day;
  TU=(jd-2451545.0)/36525;
  GMST=24110.54841+TU*(8640184.812866+TU*(0.093104-TU*6.2E-6));
  GMST=Modulus(GMST+secday*omega_E*UT,secday);
  ThetaG=twopi*GMST/secday;
  deep_arg->ds50=jd-2433281.5+UT;
  ThetaG=FMod2p(6.3003880987*deep_arg->ds50+1.72944494);
  
  return ThetaG;
}

double ThetaG_JD(double jd){
  /* Reference:  The 1992 Astronomical Almanac, page B6. */
  
  double UT, TU, GMST;
  
  UT=Frac(jd+0.5);
  jd=jd-UT;
  TU=(jd-2451545.0)/36525;
  GMST=24110.54841+TU*(8640184.812866+TU*(0.093104-TU*6.2E-6));
  GMST=Modulus(GMST+secday*omega_E*UT,secday);
  
  return (twopi*GMST/secday);
}

int Sign(double arg){
  /* Returns sign of a double */
  
  if (arg>0)
    return 1;
  
  else if (arg<0)
    return -1;
  
  else
    return 0;
}

double ArcCos(double arg){
  /* Returns arccosine of argument */
  return(pio2-ArcSin(arg));
}

double Frac(double arg){
  /* Returns fractional part of double argument */
  return(arg-floor(arg));
}
