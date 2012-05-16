//  $Id: geant3.C,v 1.150.16.1 2012/05/16 11:24:36 choutko Exp $

#include "typedefs.h"
#include "cern.h"
#include "mceventg.h"
#include "amsgobj.h"
#include "commons.h"
#include <math.h>
#ifdef _PGTRACK_
#include "TrMCCluster.h"
#include "TrRecon.h"
#include "TrSim.h"
#else
#include "trmccluster.h"
#include "trid.h"
#endif
//#include <new.h>
#include <limits.h>
#include "extC.h"
//#include <trigger3.h>
#include "job.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>
#include "gvolume.h"
#include "gmat.h"
#include "event.h"

#include "mccluster.h"
#include "cont.h"
#include "trrec.h"
#include "daqevt.h"
#include "richdbc.h"
#include "richid.h"
#include "producer.h"
#include "geantnamespace.h"         
#include "status.h"
#include "ntuple.h"
#ifdef _PGTRACK_
#include "MagField.h"
#endif
#ifdef __AMSDEBUG__
static integer globalbadthinghappened=0;

void DumpG3Commons(ostream & o){
  o<< " DumpG3Commons  "<<endl;
  o<< GCKINE.ipart<<endl;
  for(int i=0;i<7;i++)o<<GCTRAK.vect[i]<<" ";
  o<<endl;
  o<<GCTRAK.gekin<<" "<<GCKING.ngkine<<" "<<GCTMED.numed<<endl;
}
#endif

extern "C" void simde_(int&,float&,float&,float&,float&,float&);
extern "C" void trphoton_(int&,float&);
extern "C" void simtrd_(int& ,float&,float&,float&,float&);
extern "C" void getscanfl_(int &scan);
extern "C" void gustep_(){
if(    !AMSEvent::gethead()->HasNoCriticalErrors())return;



//#define __BETACLOTH__
#ifdef  __BETACLOTH__
{
int lvl=GCVOLU.nlevel-1;
 if(GCVOLU.names[lvl][0]=='B' && GCVOLU.names[lvl][1]=='E'  && GCVOLU.names[lvl][2]=='T' && GCVOLU.names[lvl][3]=='A' ){
        cout <<"***MCTRACK***"<<endl;
       AMSmctrack* genp=new AMSmctrack(GCTRAK.step,GCKINE.ipart,GCTRAK.vect,GCVOLU.names[lvl]);
       AMSEvent::gethead()->addnext(AMSID("AMSmctrack",0), genp);

}
}
#endif



//AMSmceventg::SaveSeeds();
//cout <<" in gustep "<<GCFLAG.NRNDM[0]<<" "<<GCFLAG.NRNDM[1]<<endl;
//if (GCTRAK.istop ){
//cout <<GCKINE.ipart<<" "<<GCTRAK.vect[2]<<" "<<GCTRAK.getot<<" "<<GCTMED.numed<<endl;
//}


#ifdef __AMSDEBUG__
  if( globalbadthinghappened){
    cerr<<" a " <<AMSgObj::BookTimer.check("GEANTTRACKING")<<endl;
    DumpG3Commons(cerr);
  }
#endif
  if(!AMSEvent::gethead()->HasNoCriticalErrors()){
   GCTRAK.istop=1;
   return;
  }
  static integer freq=10;
  static integer trig=0;
  trig=(trig+1)%freq;
//  if(trig==0 && freq>1)AMSgObj::BookTimer.start("GUSTEP");
  if(trig==0 && AMSgObj::BookTimer.check("GEANTTRACKING")>AMSFFKEY.CpuLimit){
//    if(freq>1)AMSgObj::BookTimer.stop("GUSTEP");
    freq=1;
    GCTRAK.istop =1;
    return;
  }
  else if(freq<10)freq=10;

//
// TRD here
//
//  if(trig==0 && freq>1)AMSgObj::BookTimer.start("TrdRadiationGen");
   int scan=0;
   getscanfl_(scan);
  if(!scan){
  if(TRDMCFFKEY.mode <3 && TRDMCFFKEY.mode >=0) {

    double mmassCO2 =  44.0095; //! g/mol
    double mmassXe  = 131.293;  //! g/mol
    double Rgas     =   8.31447;//! J/mol/K

    float rho_co2= TRDMCFFKEY.CO2Fraction * TRDMCFFKEY.Pmean * mmassCO2 / Rgas / TRDMCFFKEY.Tmean / 1.e1; //! mg/cm3                                             
    float rho_xe = TRDMCFFKEY.XenonFraction * TRDMCFFKEY.Pmean * mmassXe  / Rgas / TRDMCFFKEY.Tmean / 1.e1; //! mg/cm3                                           
    //saveliev
    simtrd_(TRDMCFFKEY.g3trd,rho_xe,rho_co2,TRDMCFFKEY.ntrcor,TRDMCFFKEY.etrcor);
    if(TRDMCFFKEY.mode<2){
         if(GCTRAK.gekin != GCTRAK.gekin){
          cerr <<"  gekin problem "<<endl;
          GCTRAK.istop =1;
          AMSEvent::gethead()->seterror(2);
         }
         trphoton_(TRDMCFFKEY.g3trd,rho_xe);
    }
    if(TRDMCFFKEY.mode==0)simde_(TRDMCFFKEY.g3trd,rho_xe,rho_co2,TRDMCFFKEY.Pecut,TRDMCFFKEY.ndecor,TRDMCFFKEY.edecor);
  }
  else if(TRDMCFFKEY.mode ==3){
    // garibyan
   
  }
  }
#ifdef __AMSDEBUG__
  if( globalbadthinghappened){
    cerr<<" b " <<AMSgObj::BookTimer.check("GEANTTRACKING")<<endl;
  }
#endif
//  if(trig==0 && freq>1)AMSgObj::BookTimer.stop("TrdRadiationGen");
/*
{
cout << "gustep "<<GCTRAK.vect[0]<<" "<<GCTRAK.vect[1]<<" "<<GCTRAK.vect[2]<<endl;
 int lvl=GCVOLU.nlevel-1;
 cout <<lvl <<" "<<GCVOLU.names[lvl][0]<<GCVOLU.names[lvl][1]<<GCVOLU.names[lvl][2]<<GCVOLU.names[lvl][3]<<endl;
}
*/

  try{
{
#ifdef __AMSDEV__
 // special rads
 int lvl=GCVOLU.nlevel-1;
 if(GCVOLU.names[lvl][0]=='R' &&
GCVOLU.names[lvl][1]=='A'  && GCVOLU.names[lvl][3]=='T' && (
GCVOLU.names[lvl][2]=='3' )){
static AMSgvolume *pvol3=0;
if(!pvol3)pvol3=AMSJob::gethead()->getgeomvolume(AMSID("RA3T",1));
AMSPoint loc=pvol3->gl2loc(AMSPoint(GCTRAK.vect));
AMSEvent::gethead()->addnext(AMSID("Test",0),new Test(GCKINE.ipart,loc));

}
else  if(GCVOLU.names[lvl][0]=='R' &&
GCVOLU.names[lvl][1]=='A'  && GCVOLU.names[lvl][3]=='T' && (
GCVOLUVOLU.names[lvl][2]=='4' )){
static AMSgvolume *pvol4=0;
if(!pvol4)pvol4=AMSJob::gethead()->getgeomvolume(AMSID("RA4T",1));
AMSPoint loc=pvol4->gl2loc(AMSPoint(GCTRAK.vect));
AMSEvent::gethead()->addnext(AMSID("Test",0),new Test(GCKINE.ipart,loc));

}

#endif
}






{
   // TRD
 int lvl=GCVOLU.nlevel-1;
 if(GCTRAK.destep != 0 && GCTMED.isvol != 0 && GCVOLU.names[lvl][0]=='T' && GCVOLU.names[lvl][1]=='R'  && GCVOLU.names[lvl][2]=='D' && GCVOLU.names[lvl][3]=='T'){
      if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");


      // Secondary particles are tagged with - for ipart
            AMSTRDMCCluster::sitrdhits(GCVOLU.number[lvl],GCTRAK.vect,
      	   GCTRAK.destep,GCTRAK.gekin,GCTRAK.step,GCKINE.istak?-GCKINE.ipart:GCKINE.ipart,GCKINE.itra);   

      if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");
} 
}
#ifdef __AMSDEBUG__
  if( globalbadthinghappened){
    cerr<<" c " <<AMSgObj::BookTimer.check("GEANTTRACKING")<<endl;
  }
#endif

  //  Tracker
  int lvl= GCVOLU.nlevel-1;  

  if(GCVOLU.nlevel>2 && GCTRAK.destep != 0 && GCTMED.isvol != 0 && 
  GCVOLU.names[2][0]== 'S' &&     GCVOLU.names[2][1]=='T' && 
  GCVOLU.names[2][2]=='K'){
      if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");
#ifdef _PGTRACK_
      TrSim::sitkhits(GCVOLU.number[lvl],GCTRAK.vect,
		      GCTRAK.destep,GCTRAK.step,GCKINE.ipart);   
#else
      AMSTrMCCluster::sitkhits(GCVOLU.number[lvl],GCTRAK.vect,
			       GCTRAK.destep,GCTRAK.step,GCKINE.ipart);   
#endif

      if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");

#ifdef __AMSDEBUG__
  if( globalbadthinghappened){
    cerr<<" d " <<AMSgObj::BookTimer.check("GEANTTRACKING")<<endl;
  }
#endif
  }
  // TOF

  int tflv;  
  char name[4];
  char media[20];
  geant t,x,y,z;
  geant de,dee,dtr2,div,tof,prtq,pstep;
  geant coo[3],dx,dy,dz,dt,stepel;
  geant wvect[6],snext,safety;
  geant origstep,origedep;
  int i,nd,numv,iprt,numl,numvp,tfprf(0);
  tflv=GCVOLU.nlevel-1;  
//---> print some GEANT standard values(for debugging):
//  if(GCFLAG.IEVENT==3118)tfprf=1;
//  if(GCFLAG.IEVENT==3118)TOF2DBc::debug=1;
  numl=GCVOLU.nlevel;
  numv=GCVOLU.number[numl-1];
  numvp=GCVOLU.number[numl-2];
//  for(i=0;i<4;i++)name[i]=GCVOLU.names[numl-1][i];
//  UHTOC(GCTMED.natmed,4,media,20);
//
  if(GCVOLU.nlevel==3 && GCTMED.isvol != 0 && GCTRAK.destep > 0
            && GCVOLU.names[tflv][0]=='T' && GCVOLU.names[tflv][1]=='F'){// <=== charged part. in "TFnn"
//
    if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");
    iprt=GCKINE.ipart;
    prtq=GCKINE.charge;
    pstep=GCTRAK.step;
    numv=GCVOLU.number[tflv];
    x=GCTRAK.vect[0];
    y=GCTRAK.vect[1];
    z=GCTRAK.vect[2];
    t=GCTRAK.tofg;
    de=GCTRAK.destep;
// cout<<"=====>In TOFsensvol: part="<<iprt<<" numv="<<numv<<" z="<<z<<" t/de="<<t<<" "<<de<<endl;
    coo[0]=x;
    coo[1]=y;
    coo[2]=z;
    tof=t;
    dee=de;
//  if(tfprf)cout<<"   x/y/z="<<coo[0]<<" "<<coo[1]<<" "<<coo[2]<<"  step="<<pstep<<endl;
    if(TFMCFFKEY.birks){
//cout<<"  ----->TOF-Bef.Birks:Edep="<<dee<<"  Q="<<GCKINE.charge<<" step="<<GCTRAK.step<<endl;
      GBIRK(dee);
//cout<<"  ----->Aft.Birks:Edep="<<dee<<endl;
    }
    AMSTOFMCCluster::sitofhits(numv,coo,dee,tof);
  }// ===> endof "in TFnn"
//-------------------------------------

#ifdef __AMSDEBUG__
  if( globalbadthinghappened){
    cerr<<" e " <<AMSgObj::BookTimer.check("GEANTTRACKING")<<endl;
  }
#endif


  // =======>  ANTI,  mod. by E.C.
//---> read some GEANT standard values(for debugging): 
//  numl=GCVOLU.nlevel;
//  numv=GCVOLU.number[numl-1];
//  numvp=GCVOLU.number[numl-2];
//  for(i=0;i<4;i++)name[i]=GCVOLU.names[numl-1][i];
//  cout<<"Volume "<<name<<" number="<<numv<<" level="<<numl<<" sens="<<GCTMED.isvol<<endl;
//  iprt=GCKINE.ipart;
//  x=GCTRAK.vect[0];
//  y=GCTRAK.vect[1];
//  z=GCTRAK.vect[2];
//  t=GCTRAK.tofg;
//  de=GCTRAK.destep;
//  cout<<"Part="<<iprt<<" x/y/z="<<x<<" "<<y<<" "<<z<<" de="<<de<<endl;
//  UHTOC(GCTMED.natmed,4,media,20);
//  cout<<" Media "<<media<<endl;
//--->
  integer manti(0),isphys,islog;
  if(lvl==3 && GCVOLU.names[lvl][0]== 'A' && GCVOLU.names[lvl][1]=='N'
                                       && GCVOLU.names[lvl][2]=='T')manti=1;
  if(GCTRAK.destep != 0  && GCTMED.isvol != 0 && manti==1){
      if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");
     dee=GCTRAK.destep; 
     if(TFMCFFKEY.birks)GBIRK(dee);
     isphys=GCVOLU.number[lvl];
     islog=floor(0.5*(isphys-1))+1;//not used now
     AMSAntiMCCluster::siantihits(isphys,GCTRAK.vect,dee,GCTRAK.tofg);
//     HF1(1510,geant(iprt),1.);
      if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");
  }
//
#ifdef __AMSDEBUG__
  if( globalbadthinghappened){
    cerr<<" f " <<AMSgObj::BookTimer.check("GEANTTRACKING")<<endl;
  }
#endif
//
// ---------------> ECAL 1.0-version by E.C.
//
// lines below are for check of TRK imp.point accuracy:
//    if(GCTRAK.inwvol==1
//     && GCVOLU.names[lvl][0]=='F' && GCVOLU.names[lvl][1]=='L'
//     && GCVOLU.names[lvl][2]=='0'&& GCVOLU.names[lvl][3]=='5'
//    ){
//      if(GCKINE.ipart==47){//"47" if He-calibration
//        AMSEcalMCHit::impoint[0]=GCTRAK.vect[0];
//        AMSEcalMCHit::impoint[1]=GCTRAK.vect[1];
//      }
//    }
    if(GCTRAK.destep != 0.){
      if(lvl==6 && GCVOLU.names[lvl][0]== 'E' && GCVOLU.names[lvl][1]=='C'
               && GCVOLU.names[lvl][2]=='F' && GCVOLU.names[lvl][3]=='C'){
//       
//       cout<<"lev/vol="<<numl<<" "<<numv<<" name="<<name<<" x/y="<<x<<" "<<y<<" z="<<z<<" de="<<de<<endl;
//       for(i=0;i<4;i++)name[i]=GCVOLU.names[numl-2][i];
//       cout<<"vol(lev-1)="<<numvp<<" name="<<name<<endl;
        if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");
        dee=GCTRAK.destep;
	if(GCTRAK.destep<0)cout<<"----> destep<0 "<<GCTRAK.destep<<endl;
//cout<<"--->  Ec-birkfl="<<TFMCFFKEY.birks<<"   dee="<<dee<<endl;
//        if(TFMCFFKEY.birks)GBIRK(dee);
//
//      number rkb=0.0011;//Birks by hands
//      number c=0.52;
//      number dedxcm=1000*dee/GCTRAK.step;
//      dee=dee/(1+c*atan(rkb/c*dedxcm));
//cout<<"     aft.birk dee="<<dee<<endl;
//
	if(dee<=0)cout<<"---->Birks:dee<0 "<<dee<<" step="<<GCTRAK.step<<" destep="<<GCTRAK.destep
	<<" ipart="<<GCKINE.ipart<<endl;
        //cout << GCVOLU.names[lvl][0]<< GCVOLU.names[lvl][1]<< GCVOLU.names[lvl][2]<< GCVOLU.names[lvl][3]<<endl;
        //cout <<" ecal "<<GCVOLU.number[lvl-1]<< " "<<dee<<endl;
//  if(GCKINE.ipart==1 || GCKINE.ipart==2 || GCKINE.ipart==3)dee=dee*1.24;//<-- crazy idea
        AMSEcalMCHit::siecalhits(GCVOLU.number[lvl-1],GCTRAK.vect,dee,GCTRAK.tofg);

        if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");
      }
    }
//

#ifdef __AMSDEBUG__
  if( globalbadthinghappened){
    cerr<<" g " <<AMSgObj::BookTimer.check("GEANTTRACKING")<<endl;
  }
#endif

    // RICH simulation code
    static int fscatcounter;
    AMSPoint local_position;
    if(GCKINE.ipart==Cerenkov_photon){
// Cut by hand 
     if(GCTRAK.nstep>6000) GCTRAK.istop=1;
     if(GCVOLU.names[lvl][0]=='R' && GCVOLU.names[lvl][1]=='I' &&
       GCVOLU.names[lvl][2]=='C' && GCVOLU.names[lvl][3]=='H'){
      if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");
      for(integer i=0;i<GCTRAK.nmec;i++){
        if(GCTRAK.lmec[i]==106) RICHDB::numrefm++;
        if(GCTRAK.lmec[i]==2000) fscatcounter=1;
      }
      if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");
     }



     // Added the counters for the NAF

     if((GCVOLU.names[lvl][0]=='R' && GCVOLU.names[lvl][1]=='A' &&
	 GCVOLU.names[lvl][2]=='D' && GCVOLU.names[lvl][3]==' ')||
	(GCVOLU.names[lvl][0]=='N' && GCVOLU.names[lvl][1]=='A' &&
	 GCVOLU.names[lvl][2]=='F' && GCVOLU.names[lvl][3]==' ')){
       if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");

      if(GCTRAK.nstep!=0){
       for(integer i=0;i<GCTRAK.nmec;i++)
          if(GCTRAK.lmec[i]==1999) RICHDB::numrayl++;
      }        
      else{
	RICHDB::numrayl=0;
	RICHDB::numrefm=0;
	fscatcounter=0;
         RICHDB::nphgen++;
      }          
      if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");

     }

}

        if(GCTRAK.inwvol==2 && GCVOLU.names[lvl][0]=='F' &&
           GCVOLU.names[lvl][1]=='O' && GCVOLU.names[lvl][2]=='I' &&
           GCVOLU.names[lvl][3]=='L' && GCTRAK.nstep>1 && GCTRAK.vect[5]<0 && RICHDB::numrayl==0){
	  RICHDB::nphrad++;
	}
        if(GCTRAK.inwvol==1 && GCVOLU.names[lvl][0]=='S' &&
           GCVOLU.names[lvl][1]=='L' && GCVOLU.names[lvl][2]=='G' &&
           GCVOLU.names[lvl][3]=='C' && GCTRAK.nstep>1 && GCTRAK.vect[5]<0 && RICHDB::numrayl==0){
	  RICHDB::nphbas++;
	}


    if(GCVOLU.names[lvl][0]=='C' && GCVOLU.names[lvl][1]=='A' &&
       GCVOLU.names[lvl][2]=='T' && GCVOLU.names[lvl][3]=='O' && 
       GCTRAK.inwvol==1){
      if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");
#ifdef __AMSDEBUG__
      if(GCKINE.vert[2]>-85. && GCKINE.ipart==Cerenkov_photon ){
	cout << "Emited at "<<GCKINE.vert[2]<<endl;
      }
#endif

      if(GCKINE.ipart==Cerenkov_photon && GCTRAK.nstep==0){

        GCTRAK.istop=1;

//        if(RICHDB::detcer(GCTRAK.vect[6])) {
//          GCTRAK.istop=2;


        local_position=AMSPoint(GCTRAK.vect);
        local_position=RichAlignment::AMSToRich(local_position);
	//	geant xl=(RichPMTsManager::GetAMSPMTPos(GCVOLU.number[lvl-1]-1,2)-RICHDB::cato_pos()-RICotherthk/2-GCTRAK.vect[2])/GCTRAK.vect[5];
	geant xl=(RichPMTsManager::GetAMSPMTPos(GCVOLU.number[lvl-1]-1,2)-RICHDB::cato_pos()-RICotherthk/2-local_position[2])/GCTRAK.vect[5];

          geant vect[3];
          vect[0]=GCTRAK.vect[0]+xl*GCTRAK.vect[3];
          vect[1]=GCTRAK.vect[1]+xl*GCTRAK.vect[4];
          vect[2]=GCTRAK.vect[2]+xl*GCTRAK.vect[5];

          AMSRichMCHit::sirichhits(GCKINE.ipart,
                                   GCVOLU.number[lvl-1]-1,
                                   vect,
                                   GCKINE.vert,
                                   GCKINE.pvert,
                                   Status_Window-
                                   (GCKINE.itra!=1?100:0));
      }



      if(GCKINE.ipart==Cerenkov_photon && GCTRAK.nstep!=0){
        GCTRAK.istop=2; // Absorb it
        local_position=AMSPoint(GCTRAK.vect);
        local_position=RichAlignment::AMSToRich(local_position);
	//	geant xl=(RichPMTsManager::GetAMSPMTPos(GCVOLU.number[lvl-1]-1,2)-RICHDB::cato_pos()-RICotherthk/2-GCTRAK.vect[2])/GCTRAK.vect[5];
	geant xl=(RichPMTsManager::GetAMSPMTPos(GCVOLU.number[lvl-1]-1,2)-RICHDB::cato_pos()-RICotherthk/2-local_position[2])/GCTRAK.vect[5];

        geant vect[3];
        vect[0]=GCTRAK.vect[0]+xl*GCTRAK.vect[3];
        vect[1]=GCTRAK.vect[1]+xl*GCTRAK.vect[4];
        vect[2]=GCTRAK.vect[2]+xl*GCTRAK.vect[5];

        if(GCKINE.vert[2]<RICHDB::RICradpos()-RICHDB::rad_height-RICHDB::rich_height-
           RICHDB::foil_height-RICradmirgap-RIClgdmirgap // in LG
           || (GCKINE.vert[2]<RICHDB::RICradpos()-RICHDB::rad_height &&
               GCKINE.vert[2]>RICHDB::RICradpos()-RICHDB::rad_height-RICHDB::foil_height))
	  AMSRichMCHit::sirichhits(GCKINE.ipart,
				   GCVOLU.number[lvl-1]-1,
                                   vect,
				   GCKINE.vert,
				   GCKINE.pvert,
				   Status_LG_origin-
				   (GCKINE.itra!=1?100:0));	  
        else
	  AMSRichMCHit::sirichhits(GCKINE.ipart,
				   GCVOLU.number[lvl-1]-1,
				   //GCTRAK.vect,
                                   vect,
				   GCKINE.vert,
				   GCKINE.pvert,
				   fscatcounter*1000+
				   (GCKINE.itra!=1?100:0)+
				   RICHDB::numrefm*10+
				   (RICHDB::numrayl>0?Status_Rayleigh:0));
      }else if(GCTRAK.nstep!=0){	 
        AMSRichMCHit::sirichhits(GCKINE.ipart,
				 GCVOLU.number[lvl-1]-1,
				 GCTRAK.vect,
				 GCKINE.vert,
				 GCKINE.pvert,
                                 Status_No_Cerenkov-
                                 (GCKINE.itra!=1?100:0));
      }				   
      if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");
    }

   
    // More RICH information added for MC
    if(GCKINE.itra==1 && GCVOLU.names[lvl][0]=='R' && 
      GCVOLU.names[lvl][1]=='A' &&GCVOLU.names[lvl][2]=='D' && 
      GCVOLU.names[lvl][3]==' ' && GCKINE.ipart!=50 &&
      GCTRAK.inwvol==1 && GCKINE.charge!=0 && GCTRAK.vect[5]<0)
      AMSRichMCHit::sirichhits(GCKINE.ipart,
                               0,
                               GCTRAK.vect,
                               GCTRAK.vect,
                               GCTRAK.vect+3,
                               Status_primary_rad);



    if(GCKINE.itra==1 && GCVOLU.names[lvl][0]=='N' && 
      GCVOLU.names[lvl][1]=='A' &&GCVOLU.names[lvl][2]=='F' && 
      GCVOLU.names[lvl][3]==' ' && GCKINE.ipart!=50 &&
      GCTRAK.inwvol==1 && GCKINE.charge!=0 && GCTRAK.vect[5]<0)
      AMSRichMCHit::sirichhits(GCKINE.ipart,
                               0,
                               GCTRAK.vect,
                               GCTRAK.vect,
                               GCTRAK.vect+3,
                               Status_primary_rad);


    if(RICCONTROLFFKEY.iflgk_flag){  // This to be checked


      if(GCKINE.itra==1 &&GCVOLU.names[lvl][0]=='R' && 
	 GCVOLU.names[lvl][1]=='A' &&GCVOLU.names[lvl][2]=='D' && 
	 GCVOLU.names[lvl][3]=='B' && GCKINE.ipart!=50 &&
	 GCTRAK.inwvol==1){
	AMSRichMCHit::sirichhits(GCKINE.ipart,
				 0,
				 GCTRAK.vect,
				 GCTRAK.vect,
				 GCTRAK.vect+3,
				 Status_primary_radb);
      }
    
    if(GCKINE.itra==1 &&GCVOLU.names[lvl][0]=='S' && 
       GCVOLU.names[lvl][1]=='T' &&GCVOLU.names[lvl][2]=='K' 
       && GCKINE.ipart!=50 && GCTRAK.inwvol==1) //Tracker
      AMSRichMCHit::sirichhits(GCKINE.ipart,
                               0,
                               GCTRAK.vect,
                               GCTRAK.vect,
                               GCTRAK.vect+3,
                               Status_primary_tracker);

    if(GCKINE.itra==1 &&GCVOLU.names[lvl][0]=='T' && 
       GCVOLU.names[lvl][1]=='O' &&GCVOLU.names[lvl][2]=='F' && 
       GCVOLU.names[lvl][2]=='H'&& GCKINE.ipart!=50 
       && GCTRAK.inwvol==1) //TOF
      AMSRichMCHit::sirichhits(GCKINE.ipart,
                               0,
                               GCTRAK.vect,
                               GCTRAK.vect,
                               GCTRAK.vect+3,
                               Status_primary_tof); 

    }



#ifdef __AMSDEBUG__
  if( globalbadthinghappened){
    cerr<<" h " <<AMSgObj::BookTimer.check("GEANTTRACKING")<<endl;
  }
#endif

//  if(trig==0 && freq>1)AMSgObj::BookTimer.start("SYSGUSTEP");
  AMSmceventg::FillMCInfo();

  if(RICCONTROLFFKEY.iflgk_flag){
   for(int i=0;i<GCKING.ngkine;i++){
     if(GCKING.iflgk[i]==0) GCKING.iflgk[i]=1;
//     GCKING.iflgk[i]=1;
   }
  }
  GCTRAK.upwght=0;
 //cout <<" nalive "<<GCNUMX.NALIVE+GCKING.ngkine<<endl;

//  if(GCNUMX.NALIVE+GCKING.ngkine>2000)throw AMSTrTrackError("SecondaryStackOverflows"); 
  GSKING(0);
  GCTRAK.upwght=1; //cerenkov tracked first  
  for(integer i=0;i<GCKIN2.ngphot;i++){
    if(RICHDB::detcer(GCKIN2.xphot[i][6])){
       if(GCNUMX.NALIVE>5000)throw AMSTrTrackError("SecondaryPhStackOverflows");
       GSKPHO(i+1);
//       cout << " NALIVE " <<GCNUMX.NALIVE<< " "<<GCKIN2.ngphot<<endl;
    }
  }
  GCTRAK.upwght=0;  
//  if(trig==0 && freq>1)AMSgObj::BookTimer.stop("SYSGUSTEP");
#ifdef __AMSDEBUG__
  if( globalbadthinghappened){
    cerr<<" i " <<AMSgObj::BookTimer.check("GEANTTRACKING")<<endl;
  }
#endif
#ifndef __BATCH__
//GSXYZ();
GDCXYZ();
#endif
  }
  
   catch (AMSuPoolError e){
    cerr << "GUSTEP  "<< e.getmessage();
    GCTRAK.istop =1;
    AMSEvent::gethead()->seterror(2);
//     AMSEvent::gethead()->Recovery();
    }
   catch (AMSaPoolError e){
    cerr << "GUSTEP  "<< e.getmessage();
    GCTRAK.istop =1;
    AMSEvent::gethead()->seterror(2);
//    AMSEvent::gethead()->Recovery();
    }
   catch (AMSTrTrackError e){
    cerr << "GUSTEP  "<< e.getmessage()<<endl;
     AMSEvent::gethead()->seterror();
   }
//  if(trig==0 && freq>1)AMSgObj::BookTimer.stop("GUSTEP");
   //  cout <<" gustep out"<<endl;

}
//-----------------------------------------------------------------------
extern "C" void guout_(){
  AMSgObj::BookTimer.start("GUOUT");
  //if(GCFLAG.IEVENT==10)new int[2000000000];
  if(    AMSEvent::gethead()->HasNoCriticalErrors()){
    RICHDB::Nph()=0;

    try{
      CCFFKEY.curtime=AMSEvent::gethead()->gettime();
      if(AMSJob::gethead()->isSimulation()){
	if(GCFLAG.IEOTRI)AMSJob::gethead()->uhend(AMSEvent::gethead()->getrun(),
						  AMSEvent::gethead()->getid(),AMSEvent::gethead()->gettime());
	number tt=AMSgObj::BookTimer.stop("GEANTTRACKING");
#ifdef _PGTRACK_
//		AMSTrTrack::SetTimeLimit(AMSFFKEY.CpuLimit-tt);
#else
	AMSTrTrack::TimeLimit()=AMSFFKEY.CpuLimit-tt;
#endif
	//        cout <<  "  tt   " <<tt<<endl;
#ifdef __AMSDEBUG__
	globalbadthinghappened=0;
#endif
	if(tt > AMSFFKEY.CpuLimit){
	  int nsec=(AMSEvent::gethead()->getC("AMSmceventg",0))->getnelem();
	  cerr<<" GEANTTRACKING Time Spent"<<tt<<" "<<nsec<<" Secondaries Generated"<<endl;
	  if(nsec==1 && tt>AMSFFKEY.CpuLimit*1.2 ){
	    // bad thing
	    (AMSEvent::gethead()->getC("AMSmceventg",0))->printC(cerr);
#ifdef __AMSDEBUG__
	    globalbadthinghappened=1;
#endif
	  }
	  throw AMSTrTrackError("SimCPULimit exceeded");
	}
      }
      // special trick to simulate/reconstruct with different mag field

      // special trick to simulate/reconstruct with different mag field
      if(TKFIELD.iniok==2)TKFIELD.iniok=3;
      //          cout <<"  iniok "<<TKFIELD.iniok<<endl;
      if(AMSEvent::gethead()->HasNoErrors())AMSEvent::gethead()->event();
      if(TKFIELD.iniok==3)TKFIELD.iniok=2;
      //          cout <<"  iniok "<<TKFIELD.iniok<<endl;


    }
    catch (AMSuPoolError e){
      AMSgObj::BookTimer.stop("GUOUT");
      cerr << e.getmessage()<<endl;
      AMSEvent::gethead()->seterror(2);
#ifdef __CORBA__
#pragma omp critical (g1)
      AMSProducer::gethead()->AddEvent();
#endif
      AMSEvent::gethead()->Recovery();
      return;
    }
    catch (AMSaPoolError e){
      AMSgObj::BookTimer.stop("GUOUT");
      cerr << "Event "<<AMSEvent::gethead()->getid()<<" Thread "<<AMSEvent::get_thread_num()<<" "<<e.getmessage()<<endl;
      AMSEvent::gethead()->seterror(2);
#ifdef __CORBA__
//#pragma omp critical (g1)
//      AMSProducer::gethead()->AddEvent();
#endif
#pragma omp critical (g1)
      AMSEvent::gethead()->Recovery();
    if(MISCFFKEY.NumThreads>2 || MISCFFKEY.NumThreads<0)MISCFFKEY.NumThreads=2;
    else MISCFFKEY.NumThreads=1;
    cerr<<"  AMSaPool-W-MemoryRecovered "<<AMSEvent::TotalSize()<<" Thread "<<AMSEvent::get_thread_num()<<" will be idled and number of thread will be reduced to "<<MISCFFKEY.NumThreads<<endl;
   AMSEvent::ThreadWait()=1;
    AMSEvent::ThreadSize()=UPool.size();
      return;
    }
    catch(std::bad_alloc a){
     AMSgObj::BookTimer.stop("GUOUT");
      cerr << "Event "<<AMSEvent::gethead()->getid()<<" Thread "<<AMSEvent::get_thread_num()<<" "<<"  memory exhausted "<<endl;
      AMSEvent::gethead()->seterror(2);
#ifdef __CORBA__
//#pragma omp critical (g1)
//      AMSProducer::gethead()->AddEvent();
#endif
#pragma omp critical (g1)
      AMSEvent::gethead()->Recovery();
    if(MISCFFKEY.NumThreads>2 || MISCFFKEY.NumThreads<0)MISCFFKEY.NumThreads=2;
    else MISCFFKEY.NumThreads=1;
    cerr<<"  AMSaPool-W-MemoryRecovered "<<AMSEvent::TotalSize()<<" Thread "<<AMSEvent::get_thread_num()<<" will be idled and number of thread will be reduced to "<<MISCFFKEY.NumThreads<<endl;
   AMSEvent::ThreadWait()=1;
    AMSEvent::ThreadSize()=UPool.size();
      return;

   }
    catch (AMSTrTrackError e){
      cerr << e.getmessage()<<endl;
      AMSEvent::gethead()->_printEl(cerr);
      AMSEvent::gethead()->seterror();
      /*
	#ifdef __CORBA__
	#pragma omp critical (g1)
	AMSProducer::gethead()->AddEvent();
	#endif
	UPool.Release(0);
	#pragma omp critical (g2)
	AMSEvent::gethead()->remove();
	UPool.Release(7);
	delete AMSEvent::gethead();
	AMSEvent::sethead(0);
	UPool.erase(512000);
	return;
      */
    }
    catch (amsglobalerror e){
      AMSgObj::BookTimer.stop("GUOUT");
      cerr << e.getmessage()<<endl;
      cerr <<"Event dump follows"<<endl;
      AMSEvent::gethead()->_printEl(cerr);
      AMSEvent::gethead()->seterror(e.getlevel());
      if(e.getlevel()>2)throw e; 
     
      /*
	#ifdef __CORBA__
	#pragma omp critical (g1)
	AMSProducer::gethead()->AddEvent();
	#endif
	UPool.Release(0);
	#pragma omp critical (g2)
	AMSEvent::gethead()->remove();
	UPool.Release(1);
	delete AMSEvent::gethead();
	AMSEvent::sethead(0);
	UPool.erase(512000);
	return;
      */
    }
#ifdef __CORBA__
#pragma omp critical (g1)
    AMSProducer::gethead()->AddEvent();
#endif
#pragma omp critical (g3)
    if(GCFLAG.IEVENT%abs(GCFLAG.ITEST)==0 ||     GCFLAG.IEORUN || GCFLAG.IEOTRI || 
       GCFLAG.IEVENT>=GCFLAG.NEVENT)
      AMSEvent::gethead()->printA(AMSEvent::debug);
    integer trig;
    if(AMSJob::gethead()->gettriggerOr()){
      trig=0;
      integer ntrig=AMSJob::gethead()->gettriggerN();
      for(int n=0;n<ntrig;n++){
        for(int i=0; ;i++){
	  AMSContainer *p=AMSEvent::gethead()->
	    getC(AMSJob::gethead()->gettriggerC(n),i);
	  if(p)trig+=p->getnelem();
	  else break;
        }
      }
    }
    else{
      trig=1;
      integer ntrig=AMSJob::gethead()->gettriggerN();
      for(int n=0;n<ntrig;n++){
        integer trigl=0;
        for(int i=0; ;i++){
	  AMSContainer *p=AMSEvent::gethead()->
	    getC(AMSJob::gethead()->gettriggerC(n),i);
	  if(p)trigl+=p->getnelem();
	  else break;
        }
        if(trigl==0)trig=0;
      }
    }
    //   if(trig ){ 
    //     AMSEvent::gethead()->copy();
    //   }
    AMSEvent::gethead()->write(trig);
  }
#pragma omp critical (g2)
  AMSEvent::ThreadSize()=UPool.size();
  UPool.Release(0);
  AMSEvent::gethead()->remove();
  delete AMSEvent::gethead();
  UPool.Release(1);
  AMSEvent::sethead(0);
  UPool.erase(2000000);
  if( AMSEvent::Waitable() && AMSEvent::TotalSize()>AMSEvent::MaxMem()){
    cerr<<"  AMSaPool-W-MemoryTooBig "<<AMSEvent::TotalSize()<<" Thread "<<AMSEvent::get_thread_num()<<" will be idled "<<UPool.size()<<endl;
    AMSEvent::ThreadWait()=1;
    AMSEvent::ThreadSize()=UPool.size();
  }
  AMSgObj::BookTimer.stop("GUOUT");

  // allow termination via time via datacard
  {  
    float xx,yy;
    TIMEX(xx);   
    TIMEL(yy);   
    if(GCTIME.TIMEND < xx || (yy>0 && yy<AMSFFKEY.CpuLimit) ){
      GCTIME.ITIME=1;
    }
  }
}
extern "C" void abinelclear_();
//--------------------------- begin gukine ----------------------------------------
extern "C" void gukine_(){
AMSgObj::BookTimer.start("GUKINE");

// Set update flag to zero, as geant3 sometimes doesn;t do it itself
 GCTMED.iupd=0;

abinelclear_();
static integer event=0;

#ifdef __DB_All__
  if (AMSFFKEY.Read > 1) {
    readDB();
     AMSgObj::BookTimer.stop("GUKINE");
     return;
  }
#endif
try{


// create new event & initialize it
  if(AMSJob::gethead()->isSimulation()){
    AMSgObj::BookTimer.start("GEANTTRACKING");
	   if(IOPA.mode%10 !=1 ){
	    AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(
    new AMSEvent(AMSID("Event",GCFLAG.IEVENT),CCFFKEY.run,0,0,0)));
	    
    for(integer i=0;i<CCFFKEY.npat;i++){
     GRNDMQ(GCFLAG.NRNDM[0],GCFLAG.NRNDM[1],0,"G");
     AMSmceventg* genp=new AMSmceventg(GCFLAG.NRNDM);

    if(genp){
     AMSEvent::gethead()->addnext(AMSID("AMSmceventg",0), genp);
     genp->run(GCKINE.ikine);
     if (CCFFKEY.npat>1 && TRDMCFFKEY.multiples==1){
	 if (i>0){
	     genp->gendelay();
	   }
       }
     //genp->_printEl(cout);

    }
    }
	   }
   else {
    AMSIO io;
    if(IOPA.mode/10?io.readA():io.read()){
     AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(
     new AMSEvent(AMSID("Event",io.getevent()),io.getrun(),0,io.gettime(),io.getnsec(),
     io.getpolephi(),io.getstheta(),io.getsphi(),io.getveltheta(),
     io.getvelphi(),io.getrad(),io.getyaw(),io.getpitch(),io.getroll(),io.getangvel())));
     AMSmceventg* genp=new AMSmceventg(io);
     if(genp){
      AMSEvent::gethead()->addnext(AMSID("AMSmceventg",0), genp);
      genp->run();
      //genp->_printEl(cout);
     }
    }
    else{
     GCFLAG.IEORUN=1;
     GCFLAG.IEOTRI=1;
     AMSgObj::BookTimer.stop("GUKINE");
     return;
    }   
   }

  }
// ---> endof simulation section
//
// <--- reconstruction section
  else {
    //
    // read daq    
    //
    DAQEvent * pdaq=0;
        static bool Waiting=false; 
        double cpulimit=AMSFFKEY.CpuLimit;

    static int forcemaxthread=0;
   for(;;){
#ifdef __CORBA__
    AMSProducer::gethead()->Transfer()=true;
#endif
    DAQEvent::InitResult res=DAQEvent::init();
#ifdef __CORBA__
    AMSProducer::gethead()->Transfer()=false;
#endif
     if(res==DAQEvent::OK){ 
       uinteger run;
       uinteger event=0;
       time_t tt=0;
       time_t oldtime=0;
       int count=0;
       
    while(AMSEvent::Barrier() || (GCFLAG.IEVENT<GCFLAG.NEVENT && !GCFLAG.IEOTRI) || (AMSFFKEY.Update && AMSStatus::isDBWriteR() && AMSJob::gethead()->getstatustable() && tt==oldtime)){
#ifdef _OPENMP
if(MISCFFKEY.NumThreads>0 && MISCFFKEY.NumThreads<omp_get_num_procs())
omp_set_num_threads(MISCFFKEY.NumThreads);
else
omp_set_num_threads(omp_get_num_procs());
omp_set_dynamic(MISCFFKEY.DynThreads);
//kmp_set_blocktime(200);
//kmp_set_stacksize_s(32000000);
#endif

const int maxt=32;
long long ia[maxt*16];
#ifdef _OPENMP
int nchunk=(MISCFFKEY.NumThreads>0?MISCFFKEY.NumThreads:omp_get_num_procs())*MISCFFKEY.ChunkThreads;        
if(MISCFFKEY.DivideBy)AMSFFKEY.CpuLimit=cpulimit*(MISCFFKEY.NumThreads>0?MISCFFKEY.NumThreads:omp_get_num_procs());
#else
int nchunk=MISCFFKEY.ChunkThreads;
#endif
#ifdef _OPENMP
if(forcemaxthread){
forcemaxthread=0;
omp_set_num_threads(omp_get_num_procs());
nchunk=omp_get_num_procs()*10;
}
#endif
for(int ik=0;ik<maxt;ik++)ia[ik*16]=0; 
//cout <<"  new chunk "<<nchunk<<endl;

#pragma omp parallel  default(none),shared(cpulimit,std::cout,std::cerr,amsffkey_,selectffkey_,gcflag_,run,event,tt,oldtime,count,nchunk,ia,Waiting), private(pdaq), copyin(LIPC2F) 
{

   AMSEvent::ResetThreadWait(1);
#pragma omp for schedule(dynamic) nowait
    for(int  kevt=0;kevt<nchunk;kevt++){
//      try{
#pragma omp critical (g3)
      if(GCFLAG.IEOTRI){
        if(!count++)oldtime=tt;
      }
      if((GCFLAG.IEOTRI || GCFLAG.IEVENT >= GCFLAG.NEVENT) &&  !(AMSFFKEY.Update && AMSStatus::isDBWriteR() && AMSJob::gethead()->getstatustable() && tt==oldtime) && !AMSEvent::Barrier()){
      continue;
      }


        //cout <<" beforebefore "<<kevt<<" "<<omp_get_thread_num()<<" "<<endl;
       //AMSTimeID*  he=AMSJob::gethead()->gettimestructure(AMSEvent::getTDVStatus());
   int isleep=cpulimit+0.501;
   while(AMSEvent::ThreadWait()){
    sleep(isleep);
    if(AMSEvent::TotalSize()<AMSEvent::MaxMem()/2){
   cout<<"  AMSaPool-I-Thread "<<AMSEvent::get_thread_num()<<" will be resumed "<<AMSEvent::TotalSize()<<endl;
    AMSEvent::ThreadSize()=AMSEvent::AvMem();
    AMSEvent::ThreadWait()=0;
    }
   }
try{
       pdaq = new DAQEvent();
}
    catch (AMSaPoolError e){
#pragma omp critical (g1)
      cerr << "DAQEvent "<<" Thread "<<AMSEvent::get_thread_num()<<" "<<e.getmessage()<<endl;
      continue;
    }
        bool ok;
again:
// intel cooperative mode bug workaround
//
if(AMSJob::gethead()->isMonitoring()){
 while(Waiting){
  usleep(1000);
}
}
 #pragma omp critical (g4)      
{
       Waiting=true;
       ok=pdaq->read();
       Waiting=false;
// set runev here
    uint64 runev=pdaq->runno(); 
   runev=pdaq->eventno() | runev<<32;
   AMSEvent::runev(AMSEvent::get_thread_num())=runev;
}
//cout << "  a "<<kevt<<endl;
       if(ok){
         
#pragma omp critical (g3)      
{
         run=pdaq->runno();
         if(pdaq->eventno()>event)event=pdaq->eventno();
         if(pdaq->time()>tt)tt=pdaq->time();   
}
         AMSEvent *pn=0;
try{
        pn=new AMSEvent(AMSID("Event",pdaq->eventno()),pdaq->runno(),
        pdaq->runtype(),pdaq->time(),pdaq->usec());
         pn->_init(pdaq);
}
    catch (AMSaPoolError e){
#pragma omp critical (g1)
      cerr << "AMSEvent "<<" Thread "<<AMSEvent::get_thread_num()<<" "<<e.getmessage()<<endl;
      continue;
    }
    catch(std::bad_alloc a){
      cerr << "new Event "<<AMSEvent::gethead()->getid()<<" Thread "<<AMSEvent::get_thread_num()<<" "<<"  memory exhausted "<<endl;
      AMSEvent::gethead()->seterror(2);
#pragma omp critical (g1)
      AMSEvent::gethead()->Recovery();
      continue;

   }

          AMSEvent::sethead(pn);
          pn->addnext(AMSID("DAQEvent",pdaq->GetBlType()), pdaq); 
//#pragma omp critical
         //cout << " hhzz "<<endl;
//#pragma omp critical
//AMSJob::gethead()->add(pn,false);
//        AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(pn,false));
//        AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(pn));
#ifdef __AMSP__
        //cout <<" 1kevt "<<kevt<<" "<<omp_get_thread_num()<<" "<<AMSEvent::gethead()<<" "<<&UPool<<" "<<pdaq->eventno()<<endl;
#endif
#pragma omp critical (g3)
{
        if(SELECTFFKEY.Run==SELECTFFKEY.RunE && SELECTFFKEY.EventE && AMSEvent::gethead()->getid()>=SELECTFFKEY.EventE){
         pdaq->SetEOFIn();    
        } 
        if(GCFLAG.IEORUN==2){
        pdaq->SetEOFIn();    
        GCFLAG.IEORUN=-2;
      }
      else if (GCFLAG.IEORUN==-2){
        GCFLAG.IEORUN=0;
      }
}
     //delete pdaq;
/*
     UPool.Release(0);
#pragma omp critical (g2)
   AMSEvent::gethead()->remove();
     UPool.Release(1);
	delete AMSEvent::gethead();
   AMSEvent::sethead(0);
   UPool.erase(2000000);

#pragma omp critical (g3)
      GCFLAG.IEVENT++;
      if(GCFLAG.IEVENT%10000==1)cout <<" events "<<GCFLAG.IEVENT<<endl;
      continue;
*/
      guout_();
#pragma omp critical (g3)
      GCFLAG.IEVENT++;
    }
    else if(AMSJob::gethead() && AMSJob::gethead()->isMonitoring()){
      sleep(1);
      goto again;
    }
// ---> endof "readDaq=ok" check
    else{
#pragma omp critical (g3)
{
     GCFLAG.IEOTRI=1;
     GCFLAG.IEORUN=1;
     count++;
     cout << "  file end "<<endl;
}
     continue;
    }
//    }
//    catch (AMSaPoolError e){
//#pragma omp critical (g1)
//      cerr << "Oops "<<" Thread "<<AMSEvent::get_thread_num()<<" "<<e.getmessage()<<endl;
//      continue;
//    }
   }
// ---> endof "kevt<nchunk" for-loop
#pragma omp critical (g2)
{

   AMSEvent::ThreadSize()=UPool.size();
   AMSEvent::ThreadWait()=-1;
   AMSEvent::ResetThreadWait(0);
}
#ifdef _OPENMP        
//  this clause is because intel throutput mode doesn;t work
//   so simulating it
           ia[omp_get_thread_num()*16]=1;
           for(;;){
           bool work=false;
           for(int j=0;j<omp_get_num_threads();j++){
              if(!ia[j*16]){
                 work=true;
                 break;
              }
            }
            if(work)usleep(1000);  
            else break;
          }
#endif
 



}
//<------- endof omp parallel
}
// ---> endof "while(Neventcollected<NeventRequested ||...)" loop
#ifdef __CORBA__
    try{
     AMSJob::gethead()->uhend(run,event,tt);
     AMSID tdvs=AMSEvent::getTDVStatus();
      AMSTimeID *ptdv=AMSJob::gethead()->gettimestructure(tdvs);
  if(AMSFFKEY.Update && AMSStatus::isDBWriteR()  ){
      AMSJob::gethead()->getstatustable()->Sort();
     AMSID tdvs=AMSEvent::getTDVStatus();
      AMSTimeID *ptdv=AMSJob::gethead()->gettimestructure(tdvs);
      ptdv->UpdateMe()=1;
      ptdv->UpdCRC();
      time_t begino,endo,inserto;
      ptdv->gettime(inserto,begino,endo);
      time_t begin,end,insert;
      begin=AMSJob::gethead()->getstatustable()->getbegin();
      end=AMSJob::gethead()->getstatustable()->getend();
      time(&insert);
      ptdv->SetTime(insert,begin,end);
      cout <<" Event Status info  geant33 has been updated for "<<*ptdv;
      ptdv->gettime(insert,begin,end);
      cout <<" Time Insert "<<ctime(&insert);
      cout <<" Time Begin "<<ctime(&begin);
      cout <<" Time End "<<ctime(&end);
      cout << " Starting to update "<<*ptdv; 
      bool fail=false;
      if(  !ptdv->write(AMSDATADIR.amsdatabase)){
         cerr <<"AMSEvent::_init-S-ProblemtoUpdate "<<*ptdv;
          fail=true;
      }
      AMSStatus *p=AMSJob::gethead()->getstatustable();
      uinteger first,last;
      p->getFL(first,last);
//      AMSProducer::gethead()->sendEventTagEnd(ptdv->getname(),p->getrun(),insert,begin,end,first,last,p->getnelem(),fail);       
      ptdv->SetTime(inserto,begino,endo);
      AMSJob::gethead()->getstatustable()->reset();      
  }

     AMSProducer::gethead()->sendRunEnd(res);
     AMSProducer::gethead()->getRunEventInfo();
     res=DAQEvent::init();
    }
    catch (AMSClientError a){
     cerr<<a.getMessage()<<endl;
     break;
    }
#else
     if(AMSJob::gethead() && AMSJob::gethead() -> isMonitoring())continue;
     else break;
#endif

}
// ---> endof "res=DaqEvent::OK"
else if (res==DAQEvent::Interrupt){
#ifdef __CORBA__
  AMSClientError ab("Process Interrupted",DPS::Client::CInAbort);
  cerr<<" CORBA-ProcessInterruped"<<endl;
 if(AMSProducer::gethead()){
  AMSProducer::gethead()->Error()=ab;
  AMSProducer::gethead()->sendRunEnd(res);
 }
#endif
     break;
   }
   else{
#ifdef __CORBA__
    try{
     AMSJob::gethead()->uhend();
     AMSProducer::gethead()->sendRunEnd(res);
     AMSProducer::gethead()->getRunEventInfo();
     res=DAQEvent::init();
    }
    catch (AMSClientError a){
     cerr<<a.getMessage()<<endl;
     break;
    }
#else
     break;
#endif
   }
}
// ---> endof for(;;)-loop
     GCFLAG.IEORUN=1;
     GCFLAG.IEOTRI=1;
      AMSgObj::BookTimer.stop("GUKINE");
     return; 
  }
// ---> endof reco-section
}
catch (amsglobalerror & a){
 cerr<<a.getmessage()<< endl;
#ifdef __CORBA__
  if(a.getlevel()>2){
  AMSClientError ab((const char*)a.getmessage(),DPS::Client::CInAbort);
 if(AMSProducer::gethead()){
  cerr<<"setting errror"<< endl;
  AMSProducer::gethead()->Error()=ab;
 }
 }
#endif
    gams::UGLAST(a.getmessage());
    exit(1);
}
    AMSgObj::BookTimer.stop("GUKINE");

}
//-------------------- endof gukine -------------------------------------------

