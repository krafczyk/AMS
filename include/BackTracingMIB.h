//-------------------------------------------------
//      date 10/01/2013 
//      Authors: MIlano-Bicocca Group
//
//      File name: BackTracingMIB.h                             
//      Class to interface Backtracing.F in AMSroot 
//
//	for comments and help:
//	davide.grandi@bim.infn.it
//	cristina.consolandi@cern.ch
//--------------------------------------------------


#ifndef BachTracingMIB_h
#define BachTracingMIB_h 1

#include "typedefs.h"
#include <time.h>



class BackTracingMIB {


	protected:

         BackTracingMIB();
         static BackTracingMIB * fBT;
       //--->  static self pointer:

	public:
        static BackTracingMIB *GetfBT();
       //---> retruns static self pointer (fBT)

	virtual ~BackTracingMIB(); 	

	void Initialize();//--> called in root.C "void AMSEventR::InitDB();"

	static int GetStatusBT(){return LoadBTParametersFile;};


	int CheckParameters( time_t Unix ,int ext);
	//---- -1 the file does not exists
	//----- 0 ok  
        int UpdateParameters(time_t Unix, int ext);
	//---- -1 the file does not exists
	//----  0 not necessary to update parameters 
	//----  1 parameters have been updated 


	void PartDirGTOD_INTL( double &thetaGTOD, double & phiGTOD ,double YPR[3], double time ,double thetaAMS,double phiAMS);
	//---------->  particle direction in GTOD with INTL att.
	void PartDirGTOD_LVLH( double &thetaGTOD, double & phiGTOD , double RPT[3] , double YPR[3] ,double VPT[3],double time ,double thetaAMS,double phiAMS);
	//---------->  particle direction in GTOD with LVLH att.
	
	void CallExternBacktracing();
	//---Extern call to FORTRAN Backtracing.F

	void FromGTODtoGAL(double rIn ,double thetaIn, double phiIn, double timeIn,double &lOut,double &bOut,bool gal_coo);
	//---> conversion from GTOD to GAL reference frame
	private:
	int LoadParmodFile();
	//---- -1 the file does not exists
        //----- 1 file loaded     
	int CheckPaticleTime( time_t Unix, int ext  );

	//---Time conversion functions:
	time_t FromHumanToUnix( int time[6] );
	// in:
	//time[0]= Year;
        //time[1]= Month;
        //time[2]= Day Of Month;
        //time[3]= hour
        //time[4]= min
        //time[5] =sec
	//out: Unix GMT
	void FromYearDoyGetMonthDay( int Year, int Doy, int &Month, int& DayOfMonth);
	//in : Year , Doy
	//out: Month , Day Of Month  
	public:
	void FromUnixToHuman(time_t Unix, int human[7] );
	//in: Unix GMT
	//out: human time:
	//human[0]= year
        //human[1]= month
        //human[2] = day of month
        //human[3] = doy 
        //human[4] = hour
        //human[5]=  min
        //human[6] = sec
	private:
	time_t FormYearDoyHourGetUnix(int Year ,int Doy ,int Hour); 



	private:
//----------------Parameters for BackTracing:------
 	static int LoadBTParametersFile;

        //---form parmod_new.dat
         vector <int>   IYEAR1V;
         vector <int>   IDOY1V;
         vector <int>   IHR1V;

         vector <double> byimfV ;
         vector <double> bzimfV;
         vector <double> pdynV;
         vector <double> dstV;
         int dim_parmod;
	 time_t UnixF1 ;//first parmod
	 time_t UnixL1;//last parmod

        //---form OMNI_W1_W6.dat
          vector <int>   IYEAR2V;
          vector <int>   IDOY2V;
          vector <int>   IHR2V;

          vector <double> W1V ;
          vector <double> W2V ;
          vector <double> W3V ;
          vector <double> W4V ;
          vector <double> W5V ;
          vector <double> W6V ;
          int dim_OMNI;
          time_t UnixF2 ;//first OMNI
          time_t UnixL2;//last OMNI

	//---present time to update par.
         int YearNow;
         int doyNow;
         int hhNow;
	
	//--error counters
	 int err1;
         int err2;
         int err3;



};
//----------------------------------------------------


#ifndef __FAST__
#ifdef __ALPHA__
#define DECFortran
#else
#define mipsFortran
#endif
#include "cfortran.h"
#endif

class BACKTRACINGPAR_DEF{
        public:

        //-----input:
        double PAR[11] ;
// no #pragma omp threadprivate(PAR)
        double r0 ;
        double the0;
        double fi0 ;
        double the1;
        double fi1;

	double rig;
	double zn;
	double amass;
	double dflag;
	double rkflag;

        //----output:
        double res;
        double r;
        double td;
        double fei;
        double ast;
        double asf;
        double time;
        double alast;

	//.....in
        int iy;
        int mes;
        int ide;
        int id;
        int ih;
        int min;
        int is ;

// no #pragma omp threadprivate(PAR,r0,the0,fi0,the1, fi1,rig,zn,amass,dflag,rkflag, res,r,td,fei,ast,asf,time,alast,iy,mes,ide,id,ih,min,is)

};
#ifndef __FAST__
#define BACKTRACINGPAR COMMON_BLOCK(BACKTRACINGPAR,backtracingpar)
COMMON_BLOCK_DEF(BACKTRACINGPAR_DEF, BACKTRACINGPAR);
#endif
#endif
