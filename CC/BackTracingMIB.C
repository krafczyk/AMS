//-------------------------------------------------
//	date 10/01/2013	
//	Authors: MIlano-Bicocca Group
//
//	File name: BackTracingMIB.C		 		
//	Class to interface Backtracing.F in AMSroot 
//
//      for comments and help:
//      davide.grandi@bim.infn.it
//      cristina.consolandi@cern.ch
//--------------------------------------------------


#include "BackTracingMIB.h"
#include "FrameTrans.h"
#include <cstdio>
#include <iostream>




//=====================================

 BACKTRACINGPAR_DEF BACKTRACINGPAR;
#pragma omp threadprivate(BACKTRACINGPAR)
//=====================================


extern "C" void mib_ascoor_();
//----> extern function to Backtracing.F

BackTracingMIB* BackTracingMIB::fBT=0;
//----> static self pointer to the class

int BackTracingMIB::LoadBTParametersFile=0;// 0:no, 1:ok , -1:err
//----> Loading BTparameters from files



//---------------------------------------------------
BackTracingMIB::BackTracingMIB(){

//	fBT = this;
	

}
//----------------------------------------------------
BackTracingMIB* BackTracingMIB::GetfBT(){

 if (!fBT) {
    fBT = new BackTracingMIB();
  }
  return fBT;


}
//----------------------------------------------------
BackTracingMIB::~BackTracingMIB(){


}
//----------------------------------------------------
void BackTracingMIB::Initialize(){

 
    	 dim_parmod=0;
         dim_OMNI=0;

         YearNow=0;
         doyNow=0;
         hhNow=0;

        //--erroes counters:    
         err1=0;
         err2=0;
         err3=0;

        //------------Load parameters file:
         LoadBTParametersFile= LoadParmodFile();
        //--- -1 err
        //---  1 ok




}
//----------------------------------------------------
int BackTracingMIB::CheckParameters(time_t Unix ,int ext){



	if(ext==0){//----------only internal filed no need to load par.
		for(int i=0 ; i < 11 ; i++){
		BACKTRACINGPAR.PAR[i]=0;
		}
	return 2; 	
		}

	//------------file does not exists
	if( LoadBTParametersFile==-1 ){
 		if(err1<=10){
                err1++;
        cerr<< " BackTracingMIB::CheckParameters (this message will be printed only 10 times) Cannot update parameters : the file does not exists " <<endl;
                }

	return -1; 
	}
	int check = CheckPaticleTime( Unix ,ext );

        if ( check == -2 ) {
 		if(err2<10){
        	err2++;

 if(ext==1)       cerr<< " BackTracingMIB::CheckParameters (this message will be printed only 10 times) Cannot update parameters T96, time  out of range from: "<<UnixF1  << " to "<< UnixL1 <<endl;
		

 if(ext==2)       cerr<< " BackTracingMIB::CheckParameters (this message will be printed only 10 times) Cannot update parameters T96 for T05 model , time  out of range from: "<<UnixF1  << " to "<< UnixL1 <<endl;
                }

        return check;
		
        }
        if ( check == -3 ) {
             	if(err3<10){
                err3++;
        cerr<< " BackTracingMIB::CheckParameters (this message will be printed only 10 times)  Cannot update parameters T05, time  out of range from: "<<UnixF2  << " to "<< UnixL2 <<endl;
		}
        return check;
        }
 	//---if (check == -3 )  Cannot update parameters : ext ==2 out of range 
        //---if (check == -2 )  Cannot update parameters : ext ==1 out of range 
        //---if (check == -1 )  Cannot update parameters : the file does not exists --->return -1;


        check=   UpdateParameters( Unix ,ext);

       if ( check == 0 ) {
        //cout<< " BackTracingMIB::CheckParameters not necessary to update parameters "<< check <<endl;
        return check;
        }
       if ( check == 1 ) {
        //cout<< " BackTracingMIB::CheckParameters the parameters were updated "<< check <<endl;
        return check;
        }


        //---if (check ==  0 )  not necessary to update parameters (the time has not changed)
        //---if (check ==  1 )  the parameters were updated


return check;
}


//----------------------------------------------------
int BackTracingMIB::LoadParmodFile(){


//------------------- file -----------------------------

        char path[100];
        sprintf(path, "%s/v5.00", getenv("AMSDataDir"));

        char parmod_File[300]; 
         sprintf(parmod_File,"%s/%s" ,path, "parmod_new.dat");
        cout << " BackTracingMIB::LoadParmodFile --- Load: " <<parmod_File<<endl;
        ifstream File1;

                File1.open(parmod_File);
        if(!File1){
        cerr<<" BackTracingMIB::LoadParmodFile the File "<< parmod_File << " does not exists!"<< endl;
        return -1 ;
        }
        while(File1.good() && !File1.eof()){

        int IYEAR1,IMONTH1,IDAY1,IHR1,IDOY1;
        double byimf,bzimf,pdyn,dst;

        File1 >> IYEAR1 >>IMONTH1 >> IDAY1 >>IHR1 >> IDOY1 >>byimf >>bzimf >>pdyn>> dst;

        IYEAR1V.push_back(IYEAR1);
        IDOY1V.push_back( IDOY1);
        IHR1V.push_back(IHR1);

        byimfV.push_back(byimf);
        bzimfV.push_back(bzimf);
        pdynV.push_back(pdyn);
        dstV.push_back( dst);

        }//...............end while     

      File1.close();

        dim_parmod=IYEAR1V.size();
        //cout<< "  dim_parmod " <<  dim_parmod <<endl;
               //---first dim_parmod
                int yearF1 =  IYEAR1V[0];
                int doyF1  =  IDOY1V[0];
                int hourF1 =  IHR1V[0];
        UnixF1 = FormYearDoyHourGetUnix(yearF1,doyF1,hourF1);

                //----last
                int yearL1 =  IYEAR1V[dim_parmod-1];
                int doyL1  =  IDOY1V[dim_parmod-1];
                int hourL1 =  IHR1V[dim_parmod-1];
        UnixL1 = FormYearDoyHourGetUnix(yearL1,doyL1,hourL1);


//cout <<" BackTracingMIB::LoadParmodFile == yy-doi-hh first "<<yearF1<<"-"<<doyF1<<"-"<<hourF1<< " --> UnixF1 "<<UnixF1<<endl;
//cout <<" BackTracingMIB::LoadParmodFile == yy-doi-hh last "<<yearL1<<"-"<<doyL1<<"-"<<hourL1<< " --> UnixL1 "<<UnixL1<<endl;
 
	//if(ext==2){
        char OMNI_File[300];
         sprintf(OMNI_File,"%s/%s" ,path , "OMNI_W1_W6.dat");
        cout << " BackTracingMIB::LoadParmodFile --- Load: "<<OMNI_File<<endl;
        ifstream File2;

        File2.open(OMNI_File);
        if(!File2){
        cerr<<" BackTracingMIB::LoadParmodFile the File "<< OMNI_File << " does not exists!"<< endl;
        return -1;
        }


        while(File2.good() && !File2.eof()){
        int IYEAR2, IDOY2, IHR2;
        double W1 ,  W2, W3 , W4 , W5, W6;
        File2>> IYEAR2 >> IDOY2 >> IHR2>> W1>> W2>> W3>> W4 >> W5>> W6;

        IYEAR2V.push_back(IYEAR2);
        IDOY2V.push_back( IDOY2);
        IHR2V.push_back(IHR2);

        W1V.push_back(W1);
        W2V.push_back(W2);
        W3V.push_back(W3);
        W4V.push_back(W4);
        W5V.push_back(W5);
        W6V.push_back(W6);

        }

        File2.close();
        dim_OMNI = IYEAR2V.size();
              //---first dim_OMNI
                int yearF2 =  IYEAR2V[0];
                int doyF2  =  IDOY2V[0];
                int hourF2 =  IHR2V[0];
         UnixF2 = FormYearDoyHourGetUnix(yearF2,doyF2,hourF2);

                //----last
                int yearL2 =  IYEAR2V[dim_OMNI-1];
                int doyL2  =  IDOY2V[dim_OMNI-1];
                int hourL2 =  IHR2V[dim_OMNI-1];
         UnixL2 = FormYearDoyHourGetUnix(yearL2,doyL2,hourL2);

//cout <<" BackTracingMIB::LoadParmodFile == yy-doi-hh first "<<yearF2<<"-"<<doyF2<<"-"<<hourF2<< " --> UnixF2 "<<UnixF2<<endl;
//cout <<" BackTracingMIB::LoadParmodFile == yy-doi-hh last "<<yearL2<<"-"<<doyL2<<"-"<<hourL2<< " --> UnixL2 "<<UnixL2<<endl;
	//--------------------------------------------now it is loaded!!!



return 1; 

}
//----------------------------------------------------
int BackTracingMIB::CheckPaticleTime( time_t Unix, int ext  ){


	if(ext!=0){

		//---------T96 out of range 
                if( Unix <= UnixF1 || Unix >= UnixL1 ) return -2;


	//-----------------------T05
	if(ext==2){

			//-------------T05 out of range
			if( Unix <=UnixF2 || Unix >= UnixL2 ) return -3;
		
		}//...end if (ext==2)

	}//...end if (ext!=0)



return 0; //ok!
}
//----------------------------------------------------
int BackTracingMIB::UpdateParameters(time_t Unix ,int ext){

	
	int human[7];
	FromUnixToHuman(Unix,  human );
	int iyC = human[0];
	int idC = human[3];
	int ihC = human[4];


//cout<< " BackTracingMIB::UpdateParameters Unix "<< Unix << " human : GMT "<< iy << "-"<< id<< " hh "<< ih<<endl;  
//cout<< " BackTracingMIB::UpdateParameters YearNow doyNow hhNow " << YearNow <<"-"<<doyNow<<" hh "<<hhNow<<endl;
        BACKTRACINGPAR.iy =human[0];
        BACKTRACINGPAR.mes=human[1];
        BACKTRACINGPAR.ide=human[2];
        BACKTRACINGPAR.id= human[3];
        BACKTRACINGPAR.ih= human[4];
        BACKTRACINGPAR.min=human[5];
        BACKTRACINGPAR.is= human[6];

	 if(YearNow==iyC && doyNow==idC &&  hhNow==ihC && ext==BACKTRACINGPAR.PAR[10] ){
	//cout<< "--------------------------------->Parameter already loaded :"<<endl;
        //cout<< " BackTracingMIB::UpdateParameters YearNow doyNow hhNow " << YearNow <<"-"<<doyNow<<" hh "<<hhNow<<endl;

	 return 0;//Parameter already loaded
	 }

     //---iupdate if the time has changed:
                   YearNow=iyC;
                   doyNow= idC ;
                   hhNow= ihC;

	//	cout<< " BackTracingMIB::UpdateParameters ......   Update ..... " << endl;
//---Scan                       
        //      cout<< " BackTracingMIB::UpdateParameters --- New GMT "<<  iy <<"-"<<id <<" hh: "<<ih <<endl;


	//......................EXTERNAL T96 ext ==1
               for(int i =0 ; i <  dim_parmod ; i++){
                        if(IYEAR1V[i] == YearNow && 
			   IDOY1V[i] ==doyNow    && 
                           IHR1V[i]==hhNow ){
                        //>byimf >>bzimf >>pdyn>> dst
                                BACKTRACINGPAR.PAR[0]= pdynV[i];
                                BACKTRACINGPAR.PAR[1]= dstV[i];
                                BACKTRACINGPAR.PAR[2]= byimfV[i];
                                BACKTRACINGPAR.PAR[3]= bzimfV[i];
                                break;
                                }

                        }//---end scan par 1

	//......................EXTERNAL T05 ext ==2	

		if(ext==2){

                for(int i =0 ; i <  dim_OMNI ; i++){

                        if(IYEAR2V[i] == YearNow && 
			   IDOY2V[i] ==doyNow    && 
			   IHR2V[i]==hhNow ){

                                BACKTRACINGPAR.PAR[4]= W1V[i];
                                BACKTRACINGPAR.PAR[5]= W2V[i];
                                BACKTRACINGPAR.PAR[6]= W3V[i];
                                BACKTRACINGPAR.PAR[7]= W4V[i];
                                BACKTRACINGPAR.PAR[8]= W5V[i];
                                BACKTRACINGPAR.PAR[9]= W6V[i];
                                break;
                               }

			}//---end scan par 2
		}else if(ext!=2){
 			        BACKTRACINGPAR.PAR[4]= 0.;
                                BACKTRACINGPAR.PAR[5]= 0.;
                                BACKTRACINGPAR.PAR[6]= 0.;
                                BACKTRACINGPAR.PAR[7]= 0.;
                                BACKTRACINGPAR.PAR[8]= 0.;
                                BACKTRACINGPAR.PAR[9]= 0.;

		     }


                         BACKTRACINGPAR.PAR[10]=(double)ext;


return 1; // --> the time has changed

}
//----------------------------------------------------
time_t BackTracingMIB::FromHumanToUnix( int time[6] ){
        tm gmtTime;
           gmtTime.tm_sec = time[5] ;
           gmtTime.tm_min = time[4];
           gmtTime.tm_hour = time[3];
           gmtTime.tm_mday = time[2];
           gmtTime.tm_mon= time[1] -1 ;
           gmtTime.tm_year = time[0] -1900;

        time_t Unix  = timegm(& gmtTime);


return Unix;
}
//----------------------------------------------------
void  BackTracingMIB::FromUnixToHuman(time_t Unix, int human[7] ){

	time_t time =Unix;
        tm* tmtime= gmtime(&time);

         human[0]= tmtime->tm_year +1900;// 0 is 1900
         human[1]= tmtime->tm_mon +1; // from 0 to 11
         human[2] = tmtime->tm_mday;
         human[3] = tmtime->tm_yday +1; //from 0 to 364/65
         human[4] = tmtime->tm_hour;
         human[5]= tmtime->tm_min;
         human[6] = tmtime->tm_sec;


}
//----------------------------------------------------
void  BackTracingMIB::FromYearDoyGetMonthDay( int Year, int Doy, int &Month, int& DayOfMonth){


	int DaysInMonth[12];
        for(int i=0; i<12;i++ ){
                int d=i+1;
                if(d<=7){
                        if(d==2){
                                if(Year%4==0) DaysInMonth[i]=29;
                                if(Year%4!=0) DaysInMonth[i]=28;
                        }else if(d!=2){
                        if(d%2!=0)DaysInMonth[i]=31;
                        if(d%2==0)DaysInMonth[i]=30;
                        }
                }else if(d>7){
                        if(d%2!=0)DaysInMonth[i]=30;
                        if(d%2==0)DaysInMonth[i]=31;
                        }
                }//.....end for


 	int TotDyas=0;
         Month=0;
        //---find month
        for(int i=0; i<12;i++ ){
         TotDyas+=DaysInMonth[i];
                if( Doy <= TotDyas ){

                        Month = i+1;
                        //cout << "Month "<< Month <<endl;
                                break;
                }
        }


        //...find day of month:

 	DayOfMonth=0;
        int doy =0;
        if( Month!=1)doy =TotDyas - DaysInMonth[Month-1];
        if( Month==1)doy = DayOfMonth;


        for (int j=0 ; j < DaysInMonth[ Month -1] ; j++){
                                DayOfMonth ++;
                                doy ++;
                                if(Doy ==doy){
                        // cout << " DayOfMonth "<< DayOfMonth<<endl;
                                break;
                                }
                        }
}

//----------------------------------------------------
time_t BackTracingMIB::FormYearDoyHourGetUnix(int Year ,int Doy ,int Hour){


	int Month;
	int DayOfMonth;
	FromYearDoyGetMonthDay( Year,  Doy, Month, DayOfMonth);

	int time[6];  
	time[0]= Year;
        time[1]= Month;
        time[2]= DayOfMonth;
        time[3]= Hour;
        time[4]= 0;//min
        time[5] =0;//sec


	time_t Unix= FromHumanToUnix( time );


return Unix;

}
//----------------------------------------------------
void BackTracingMIB::PartDirGTOD_INTL( double &thetaGTOD, double & phiGTOD ,double YPR[3], double time ,double thetaAMS,double phiAMS){


	//---particle direction:
	AMSDir ams(thetaAMS, phiAMS);
  	double x = -ams.x();
	double y = -ams.y();
	double z = -ams.z();

	  FT_AMS2Body(x, y, z);
	  FT_Body_to_J2000(x, y, z, YPR[0], YPR[1], YPR[2]);
 	  FT_Equat2GTOD(x, y, z, time);

          double theta=0;
          double phi=0;
	  double r=1;	
	  FT_Cart2Angular(x,y,z , r,theta,phi);

	thetaGTOD = theta;
	phiGTOD = phi;

}
//----------------------------------------------------
void BackTracingMIB::PartDirGTOD_LVLH( double &thetaGTOD, double & phiGTOD , double RPT[3] , double YPR[3] ,double VPT[3],double time ,double thetaAMS,double phiAMS){

	//---particle direction:
        AMSDir ams(thetaAMS, phiAMS);
        double x = -ams.x();
        double y = -ams.y();
        double z = -ams.z();
	
	//---particle dir AMS-->ISSBody-->LVLH... 
          FT_AMS2Body(x, y, z);
	  FT_Body2LVLH(x, y, z, YPR[0], YPR[1], YPR[2]);

	//----ISS position and velocity in GTOD
	double issx, issy, issz, vx, vy, vz;

    	FT_Angular2Cart(       RPT[0], RPT[2], RPT[1], issx, issy, issz);
    	FT_Angular2Cart(VPT[0]*RPT[0], VPT[2], VPT[1], vx, vy, vz);
        // ISS GTOD --> Equat 
    	FT_GTOD2Equat(issx, issy, issz, vx, vy, vz, time);

	//....LVLH-->Equat-->GTOD
    	FT_LVLH2ECI  (x, y, z, issx, issy, issz, vx, vy, vz);
    	FT_Equat2GTOD(x, y, z, time);

          double theta=0;
          double phi=0;
          double r=1;
        FT_Cart2Angular(x,y,z , r,theta,phi);

        thetaGTOD = theta;
        phiGTOD = phi;


}
//----------------------------------------------------
void BackTracingMIB::CallExternBacktracing(){


	//-----extern call to FORTRAN Backtracing.F
	mib_ascoor_();

}
//------------------------------------------------------
void BackTracingMIB::FromGTODtoGAL(double rIn ,double thetaIn, double phiIn, double timeIn,double &lOut,double &bOut,bool gal_coo){
//----Using FrameTrans.h functions

 //---from angular to cartesian (GTOD)
	//---in
	double r = rIn;
	double theta =thetaIn;
	double phi = phiIn;
	//---out
	double x=0;
	double y=0; 
	double z=0;
                               // in<--|-->out	
 	FT_Angular2Cart( r, theta, phi , x,y,z );

 //---From GTOD to Equatorial:
	double time = timeIn;
                   //| in==out|  
	FT_GTOD2Equat( x, y, z   , time);

 //---From Cartesian to angular:
                       // in<--|-->out
        FT_Cart2Angular(x,y,z,r,theta,phi);


 //---From Equatorial to Galactic:
        double azimut = phi;
        double elev=theta;

	if(gal_coo==true){
  		               //|in==out|
	FT_Equat2Gal(azimut, elev);
	}

	lOut=azimut;
	bOut=elev;


}
//----------------------------------------------------

