//  $Id: geant3.C,v 1.54 2001/05/23 14:37:21 choutko Exp $

#include <typedefs.h>
#include <cern.h>
#include <mceventg.h>
#include <amsgobj.h>
#include <commons.h>
#include <math.h>
#include <trid.h>
#include <new.h>
#include <limits.h>
#include <extC.h>
#include <trigger3.h>
#include <job.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>
#include <gvolume.h>
#include <gmat.h>
#include <mccluster.h>
#include <event.h>
#include <cont.h>
#include <trrec.h>
#include <daqevt.h>
#include <iostream.h>
#include <richdbc.h>
#include <producer.h>
#include <geantnamespace.h>         
#include <status.h>
#include <ntuple.h>
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

extern "C" void simde_(int&);
extern "C" void trphoton_(int&);
extern "C" void simtrd_(int& );
extern "C" void getscanfl_(int &scan);
extern "C" void gustep_(){

//AMSmceventg::SaveSeeds();
//cout <<" in gustep "<<GCFLAG.NRNDM[0]<<" "<<GCFLAG.NRNDM[1]<<endl;
//if (GCTRAK.istop ){
// cout <<GCKINE.ipart<<" "<<GCTRAK.vect[2]<<" "<<GCTRAK.getot<<" "<<GCTMED.numed<<endl;
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
  if(trig==0 && freq>1)AMSgObj::BookTimer.start("GUSTEP");
  if(trig==0 && AMSgObj::BookTimer.check("GEANTTRACKING")>AMSFFKEY.CpuLimit){
    if(freq>1)AMSgObj::BookTimer.stop("GUSTEP");
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
    //saveliev
    simtrd_(TRDMCFFKEY.g3trd);
    if(TRDMCFFKEY.mode<2)trphoton_(TRDMCFFKEY.g3trd);
    if(TRDMCFFKEY.mode==0)simde_(TRDMCFFKEY.g3trd);
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


  try{
{
   // TRD
 int lvl=GCVOLU.nlevel-1;
 if(GCTRAK.destep != 0 && GCTMED.isvol != 0 && GCVOLU.names[lvl][0]=='T' && GCVOLU.names[lvl][1]=='R'  && GCVOLU.names[lvl][2]=='D' && GCVOLU.names[lvl][3]=='T'){
      if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");

     AMSTRDMCCluster::sitrdhits(GCVOLU.number[lvl],GCTRAK.vect,
        GCTRAK.destep,GCTRAK.gekin,GCTRAK.step,GCKINE.ipart);   

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
     AMSTrMCCluster::sitkhits(GCVOLU.number[lvl],GCTRAK.vect,
     GCTRAK.destep,GCTRAK.step,GCKINE.ipart);   
      if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");

#ifdef __AMSDEBUG__
  if( globalbadthinghappened){
    cerr<<" d " <<AMSgObj::BookTimer.check("GEANTTRACKING")<<endl;
  }
#endif
  }
  // TOF

  char name[4];
  char media[20];
  geant t,x,y,z;
  geant de,dee,dtr2,div,tof;
  static geant xpr(0.),ypr(0.),zpr(0.),tpr(0.);
  geant trcut2(0.25);// Max. transv.shift (0.5cm)**2
  geant vect[3],dx,dy,dz,dt; 
  int i,nd,numv,iprt,numl,numvp;
  static int numvo(-999),iprto(-999);
//
  if(lvl >1 && GCVOLU.names[lvl][0]== 'T' && GCVOLU.names[lvl][1]=='O' &&
  GCVOLU.names[lvl][2]=='F' && GCVOLU.names[lvl][3]=='S'){// in "TOFS"
  if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");
    iprt=GCKINE.ipart;
    numv=GCVOLU.number[lvl];
    x=GCTRAK.vect[0];
    y=GCTRAK.vect[1];
    z=GCTRAK.vect[2];
    t=GCTRAK.tofg;
    de=GCTRAK.destep;
//    if(iprt>40)cout <<iprt<<" "<<de<<" "<<GCTRAK.inwvol<<" "<<z<<" "<<GCTRAK.istop<<endl;
    if(GCTRAK.inwvol==1){// new volume or track : store param.
      iprto=iprt;
      numvo=numv;
      xpr=x;
      ypr=y;
      zpr=z;
      tpr=t;
    }
    else{
      if(iprt==iprto && numv==numvo && de!=0.){// same part. in the same volume
        dx=(x-xpr);
        dy=(y-ypr);
        dz=(z-zpr);
        dt=(t-tpr);
        dtr2=dx*dx+dy*dy;
//
        if(dtr2>trcut2){// too big transv. shift: subdivide step
          nd=integer(sqrt(dtr2/trcut2));
          nd+=1;
          dx=dx/geant(nd);
          dy=dy/geant(nd);
          dz=dz/geant(nd);
          dt=dt/geant(nd);
          GCTRAK.destep=de/geant(nd);
          for(i=1;i<=nd;i++){//loop over subdivisions
            vect[0]=xpr+dx*(i-0.5);
            vect[1]=ypr+dy*(i-0.5);
            vect[2]=zpr+dz*(i-0.5);
            tof=tpr+dt*(i-0.5);
            dee=GCTRAK.destep;
            if(TOFMCFFKEY.birks)GBIRK(dee);
            AMSTOFMCCluster::sitofhits(numv,vect,dee,tof);
          }
        }
        else{
          vect[0]=xpr+0.5*dx;
          vect[1]=ypr+0.5*dy;
          vect[2]=zpr+0.5*dz;
          tof=tpr+0.5*dt;
          dee=GCTRAK.destep;
          if(TOFMCFFKEY.birks)GBIRK(dee);
          AMSTOFMCCluster::sitofhits(numv,vect,dee,tof);
        }// end of "big step" test
//
        xpr=x;
        ypr=y;
        zpr=z;
        tpr=t;
      }// end of "same part/vol, de>0"
    }// end of new volume test
  if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");
  }// end of "in TOFS"
//-------------------------------------

#ifdef __AMSDEBUG__
  if( globalbadthinghappened){
    cerr<<" e " <<AMSgObj::BookTimer.check("GEANTTRACKING")<<endl;
  }
#endif

  // CTC


  if(lvl >2 && GCKINE.charge != 0  && GCTRAK.destep != 0 
      &&   GCTMED.isvol != 0 ){ 
    if((GCVOLU.names[lvl][0]== 'A' && GCVOLU.names[lvl][1]=='G' && 
        GCVOLU.names[lvl][2]=='L') || (GCVOLU.names[lvl][0]== 'P' && GCVOLU.names[lvl][1]=='T' && 
           GCVOLU.names[lvl][2]=='F' ) || (GCVOLU.names[lvl][0]== 'P' && GCVOLU.names[lvl][1]=='M' && 
          GCVOLU.names[lvl][2]=='T' )){
      if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");
      AMSCTCMCCluster::sictchits(GCVOLU.number[lvl],GCTRAK.vect,
      GCKINE.charge,GCTRAK.step, GCTRAK.getot,GCTRAK.destep,GCTRAK.tofg);
      if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");
     }
  }

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
  int manti(0);
  if(lvl==3 && GCVOLU.names[lvl][0]== 'A' && GCVOLU.names[lvl][1]=='N'
                                       && GCVOLU.names[lvl][2]=='T')manti=1;
  if(GCTRAK.destep != 0  && GCTMED.isvol != 0 && manti==1){
      if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");
     dee=GCTRAK.destep; 
     if(TOFMCFFKEY.birks)GBIRK(dee);
     AMSAntiMCCluster::siantihits(GCVOLU.number[lvl],GCTRAK.vect,dee,GCTRAK.tofg);
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
               ){
//               && GCVOLU.names[lvl][2]=='F' && GCVOLU.names[lvl][3]=='C'){
//       
//       cout<<"lev/vol="<<numl<<" "<<numv<<" name="<<name<<" x/y="<<x<<" "<<y<<" z="<<z<<" de="<<de<<endl;
//       for(i=0;i<4;i++)name[i]=GCVOLU.names[numl-2][i];
//       cout<<"vol(lev-1)="<<numvp<<" name="<<name<<endl;
        if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");
        dee=GCTRAK.destep; 
        if(TOFMCFFKEY.birks)GBIRK(dee);
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
    if(GCKINE.ipart==Cerenkov_photon){
     if(GCVOLU.names[lvl][0]=='R' && GCVOLU.names[lvl][1]=='I' &&
       GCVOLU.names[lvl][2]=='C' && GCVOLU.names[lvl][3]=='H'){
      if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");
      for(integer i=0;i<GCTRAK.nmec;i++){
        if(GCTRAK.lmec[i]==106) RICHDB::numrefm++;
      }
      if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");
    }


    if(GCVOLU.names[lvl][0]=='R' && GCVOLU.names[lvl][1]=='A' &&
     GCVOLU.names[lvl][2]=='D' && GCVOLU.names[lvl][3]==' '){
      if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");

      if(GCTRAK.nstep!=0){
       for(integer i=0;i<GCTRAK.nmec;i++)
          if(GCTRAK.lmec[i]==1999) RICHDB::numrayl++;
      }        
      else{
	RICHDB::numrayl=0;
	RICHDB::numrefm=0;

//        if(!RICHDB::detcer(GCTRAK.vect[6])) GCTRAK.istop=1; 
//          else RICHDB::nphgen++; 
         RICHDB::nphgen++;
      }          
      if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");

     }

    }

//    if(GCTRAK.inwvol==1 && GCVOLU.names[lvl][0]=='P' &&
//       GCVOLU.names[lvl][1]=='M' && GCVOLU.names[lvl][2]=='T' &&
//       GCVOLU.names[lvl][3]=='B'){
//      if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");
//      if(trig==0 && freq>1)AMSgObj::BookTimer.stop("AMSGUSTEP");
//    }



    if(GCVOLU.names[lvl][0]=='C' && GCVOLU.names[lvl][1]=='A' &&
       GCVOLU.names[lvl][2]=='T' && GCVOLU.names[lvl][3]=='O' && 
       GCTRAK.inwvol==1){
      if(trig==0 && freq>1)AMSgObj::BookTimer.start("AMSGUSTEP");


      if(GCKINE.ipart==Cerenkov_photon && GCTRAK.nstep==0){

        GCTRAK.istop=1;

//        if(RICHDB::detcer(GCTRAK.vect[6])) {
//          GCTRAK.istop=2;
          AMSRichMCHit::sirichhits(GCKINE.ipart,
                                   GCVOLU.number[lvl-1]-1,
                                   GCTRAK.vect,
                                   GCKINE.vert,
                                   GCKINE.pvert,
                                   Status_Window-
                                   (GCKINE.itra!=1?100:0));
//        }
      }



      if(GCKINE.ipart==Cerenkov_photon && GCTRAK.nstep!=0){
        GCTRAK.istop=2; // Absorb it
	if(GCKINE.vert[2]<RICradpos-RICHDB::rad_height-RICHDB::height)
	  AMSRichMCHit::sirichhits(GCKINE.ipart,
				   GCVOLU.number[lvl-1]-1,
				   GCTRAK.vect,
				   GCKINE.vert,
				   GCKINE.pvert,
				   Status_LG_origin-
				   (GCKINE.itra!=1?100:0));	  
        else
	  AMSRichMCHit::sirichhits(GCKINE.ipart,
				   GCVOLU.number[lvl-1]-1,
				   GCTRAK.vect,
				   GCKINE.vert,
				   GCKINE.pvert,
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

   

#ifdef __AMSDEBUG__
  if( globalbadthinghappened){
    cerr<<" h " <<AMSgObj::BookTimer.check("GEANTTRACKING")<<endl;
  }
#endif

//  if(trig==0 && freq>1)AMSgObj::BookTimer.start("SYSGUSTEP");
  AMSmceventg::FillMCInfo();

  if(RICCONTROL.iflgk_flag){
   for(int i=0;i<GCKING.ngkine;i++){
     if(GCKING.iflgk[i]==0) GCKING.iflgk[i]=1;
//     GCKING.iflgk[i]=1;
   }
  }
  GCTRAK.upwght=0;
  if(GCNUMX.NALIVE+GCKING.ngkine>2000)throw AMSTrTrackError("SecondaryStackOverflows"); 
  GSKING(0);
  GCTRAK.upwght=1; //cerenkov tracked first  
  for(integer i=0;i<GCKIN2.ngphot;i++){
    if(RICHDB::detcer(GCKIN2.xphot[i][6])){
       if(GCNUMX.NALIVE>2000)throw AMSTrTrackError("SecondaryPhStackOverflows");
       GSKPHO(i+1);
       //cout << " NALIVE " <<GCNUMX.NALIVE<<endl;
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
//  GSXYZ();
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
     AMSEvent::gethead()->seterror(2);
   }
  if(trig==0 && freq>1)AMSgObj::BookTimer.stop("GUSTEP");
   //  cout <<" gustep out"<<endl;

}
//-----------------------------------------------------------------------
extern "C" void guout_(){
  RICHDB::Nph()=0;
   try{
   if(AMSJob::gethead()->isSimulation()){
      number tt=AMSgObj::BookTimer.stop("GEANTTRACKING");
      AMSTrTrack::TimeLimit()=AMSFFKEY.CpuLimit-tt;
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
          if(AMSEvent::gethead()->HasNoErrors())AMSEvent::gethead()->event();
   }
   catch (AMSuPoolError e){
     cerr << e.getmessage()<<endl;
     AMSEvent::gethead()->seterror(2);
#ifdef __CORBA__
     AMSProducer::gethead()->AddEvent();
#endif
     AMSEvent::gethead()->Recovery();
      return;
   }
   catch (AMSaPoolError e){
     cerr << e.getmessage()<<endl;
     AMSEvent::gethead()->seterror(2);
#ifdef __CORBA__
     AMSProducer::gethead()->AddEvent();
#endif
     AMSEvent::gethead()->Recovery();
      return;
   }
   catch (AMSTrTrackError e){
     cerr << e.getmessage()<<endl;
     cerr <<"Event dump follows"<<endl;
     AMSEvent::gethead()->_printEl(cerr);
     AMSEvent::gethead()->seterror(2);
/*
#ifdef __CORBA__
     AMSProducer::gethead()->AddEvent();
#endif
     UPool.Release(0);
     AMSEvent::gethead()->remove();
     UPool.Release(1);
     AMSEvent::sethead(0);
      UPool.erase(512000);
      return;
*/
   }
   catch (amsglobalerror e){
     cerr << e.getmessage()<<endl;
     cerr <<"Event dump follows"<<endl;
     AMSEvent::gethead()->_printEl(cerr);
     AMSEvent::gethead()->seterror(e.getlevel());
     if(e.getlevel()>2)throw e; 
     
/*
#ifdef __CORBA__
     AMSProducer::gethead()->AddEvent();
#endif
     UPool.Release(0);
     AMSEvent::gethead()->remove();
     UPool.Release(1);
     AMSEvent::sethead(0);
      UPool.erase(512000);
      return;
*/
   }
#ifdef __CORBA__
     AMSProducer::gethead()->AddEvent();
#endif
      AMSgObj::BookTimer.start("GUOUT");
      if(GCFLAG.IEVENT%abs(GCFLAG.ITEST)==0 ||     GCFLAG.IEORUN || GCFLAG.IEOTRI || 
         GCFLAG.IEVENT==GCFLAG.NEVENT)
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
   if(trig ){ 
     AMSEvent::gethead()->copy();
   }
     AMSEvent::gethead()->write(trig);
#ifdef __DB_All__
   if(trig) {
    if ( eventW > DBWriteGeom) {
     integer run   = AMSEvent::gethead() -> getrun();
     char* jobname = AMSJob::gethead()->getname();
     integer event = AMSEvent::gethead() -> getEvent();
     time_t  time  = AMSEvent::gethead() -> gettime();
     integer rtype = AMSEvent::gethead() -> getruntype();
     number pole, stationT, stationP;
     AMSEvent::gethead() -> GetGeographicCoo(pole, stationT, stationP);
     integer WriteStartEnd = 0;
     if (trigEvents == 0 && AMSFFKEY.Read < 10)     WriteStartEnd = 1;
      if(dbout.recoevents() || dbout.mcevents() || dbout.mceventg())
        dbout.AddEvent(run, event, time, rtype, pole, stationT, stationP, 
                       WriteStartEnd);
      trigEvents++;
    } 
    if (dbg_prtout != 0 && eventW < DBWriteGeom) cout <<
         "GUOUT - I - AMSFFKEY.Write = 0, no database action "<<endl;
   } else {
     notrigEvents++;
   }
#endif
     UPool.Release(0);
   AMSEvent::gethead()->remove();
     UPool.Release(1);
   AMSEvent::sethead(0);
   UPool.erase(2000000);
   AMSgObj::BookTimer.stop("GUOUT");

// allow termination via time via datacard
{  
   float xx;
   TIMEX(xx);   
   if(GCTIME.TIMEND < xx){
    GCTIME.ITIME=1;
   }
}
}

extern "C" void abinelclear_();
extern "C" void gukine_(){
AMSgObj::BookTimer.start("GUKINE");
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
   if(IOPA.mode !=1 ){
    AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(
    new AMSEvent(AMSID("Event",GCFLAG.IEVENT),CCFFKEY.run,0,0,0)));
    for(integer i=0;i<CCFFKEY.npat;i++){
     GRNDMQ(GCFLAG.NRNDM[0],GCFLAG.NRNDM[1],0,"G");
     AMSmceventg* genp=new AMSmceventg(GCFLAG.NRNDM);
    if(genp){
     AMSEvent::gethead()->addnext(AMSID("AMSmceventg",0), genp);
     genp->run(GCKINE.ikine);
     //genp->_printEl(cout);

    }
   }
   }
   else {
    AMSIO io;
    if(io.read()){
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
  else {
    //
    // read daq    
    //
    DAQEvent * pdaq=0;
    DAQEvent::InitResult res=DAQEvent::init();
    for(;;){
     if(res==DAQEvent::OK){ 
        AMSJob::gethead()->gettimestructure(AMSEvent::getTDVStatus());
       pdaq = new DAQEvent();
       uinteger run;
       uinteger event;
        time_t tt;
       if(pdaq->read()){
         run=pdaq->runno();
         event=pdaq->eventno();
         tt=pdaq->time();   
        AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(
        new AMSEvent(AMSID("Event",pdaq->eventno()),pdaq->runno(),
        pdaq->runtype(),pdaq->time(),pdaq->usec())));
        AMSEvent::gethead()->addnext(AMSID("DAQEvent",pdaq->GetBlType()), pdaq);
        if(SELECTFFKEY.Run==SELECTFFKEY.RunE && SELECTFFKEY.EventE && AMSEvent::gethead()->getid()>=SELECTFFKEY.EventE){
         pdaq->SetEOFIn();    
        } 
        if(GCFLAG.IEORUN==2){
      // if production 
      // try to update the badrun list
         if(AMSJob::gethead()->isProduction() && AMSJob::gethead()->isRealData()){
           char fname[256];
           char * logdir = getenv("ProductionLogDirLocal");
           if(logdir){
            strcpy(fname,logdir);
           }
           else {
             cerr<<"gukine-E-NoProductionLogDirLocalFound"<<endl;
             strcpy(fname,"/Offline/local/logs");
           }
           strcat(fname,"/BadRunsList");
           ofstream ftxt;
           ftxt.open(fname,ios::app);
           if(ftxt){
            ftxt <<pdaq->runno()<<" "<<pdaq->eventno()<<endl;
            ftxt.close();
           }
           else{
            cerr<<"gukine-S-CouldNotOPenFile "<<fname<<endl;
            exit(1);
           }
           
         }
        pdaq->SetEOFIn();    
        GCFLAG.IEORUN=-2;
      }
      else if (GCFLAG.IEORUN==-2){
        GCFLAG.IEORUN=0;
      //  AMSJob::gethead()->uhinit(pdaq->runno(),pdaq->eventno());
      }
      guout_();
      if(GCFLAG.IEOTRI || GCFLAG.IEVENT >= GCFLAG.NEVENT)break;
      GCFLAG.IEVENT++;
    }
    else{
#ifdef __CORBA__
    try{
     AMSJob::gethead()->uhend(run,event,tt);
     AMSID tdvs=AMSEvent::getTDVStatus();
      AMSTimeID *ptdv=AMSJob::gethead()->gettimestructure(tdvs);
  if(AMSFFKEY.Update && AMSStatus::isDBWriteR()  ){
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
      cout <<" Event Status info  info has been updated for "<<*ptdv;
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
      AMSProducer::gethead()->sendEventTagEnd(ptdv->getname(),p->getrun(),insert,begin,end,first,last,p->getnelem(),fail);       
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
     break;
#endif
    }
   }
   else if (res==DAQEvent::Interrupt)break;
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
     GCFLAG.IEORUN=1;
     GCFLAG.IEOTRI=1;
      AMSgObj::BookTimer.stop("GUKINE");
     return; 
  }
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


