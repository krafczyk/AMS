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
extern "C" void gustep_(){
//if(GCKINE.ipart==50)cout <<"vlalalalalalalala"<<endl;
//    cout <<"    ----> in gustep "<<endl;
  try{

  //  Tracker
  int lvl= GCVOLU.nlevel-1;  

  if(GCVOLU.nlevel>2 && GCTRAK.destep != 0 && GCTMED.isvol != 0 && 
  GCVOLU.names[2][0]== 'S' &&     GCVOLU.names[2][1]=='T' && 
  GCVOLU.names[2][2]=='K')
     AMSTrMCCluster::sitkhits(GCVOLU.number[lvl],GCTRAK.vect,
     GCTRAK.destep,GCTRAK.step,GCKINE.ipart);   

  // TOF

  geant t,x,y,z;
  char name[5]="dumm";
  char media[21]="dummy_media         ";
  geant de,dee,dtr2,div,tof;
  static geant xpr(0.),ypr(0.),zpr(0.),tpr(0.);
  geant trcut2(0.1);// Max. transv.shift (0.316cm)**2
  geant vect[3],dx,dy,dz,dt; 
  int i,nd,numv,iprt,numl,numvp;
  static int numvo(-999),iprto(-999);
  if(lvl >1 && GCVOLU.names[lvl][0]== 'T' && GCVOLU.names[lvl][1]=='O' &&
  GCVOLU.names[lvl][2]=='F' && GCVOLU.names[lvl][3]=='S'){// in "TOFS"
    iprt=GCKINE.ipart;
    numv=GCVOLU.number[lvl];
    x=GCTRAK.vect[0];
    y=GCTRAK.vect[1];
    z=GCTRAK.vect[2];
    t=GCTRAK.tofg;
    de=GCTRAK.destep;
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
  }// end of "in TOFS"
//-------------------------------------

  // CTC


  if(lvl >2 && GCKINE.charge != 0  && GCTRAK.destep != 0 
      &&   GCTMED.isvol != 0 ){ 
    if((GCVOLU.names[lvl][0]== 'A' && GCVOLU.names[lvl][1]=='G' && 
        GCVOLU.names[lvl][2]=='L'))
      AMSCTCMCCluster::sictchits(GCVOLU.number[lvl],GCTRAK.vect,
      GCKINE.charge,GCTRAK.step, GCTRAK.getot,GCTRAK.destep,GCTRAK.tofg);

       if((GCVOLU.names[lvl][0]== 'P' && GCVOLU.names[lvl][1]=='T' && 
           GCVOLU.names[lvl][2]=='F' )){
         AMSCTCMCCluster::sictchits(GCVOLU.number[lvl],GCTRAK.vect,
         GCKINE.charge,GCTRAK.step, GCTRAK.getot,GCTRAK.destep,GCTRAK.tofg);
       }

      if((GCVOLU.names[lvl][0]== 'P' && GCVOLU.names[lvl][1]=='M' && 
          GCVOLU.names[lvl][2]=='T' ))
        AMSCTCMCCluster::sictchits(GCVOLU.number[lvl],GCTRAK.vect,
        GCKINE.charge,GCTRAK.step, GCTRAK.getot,GCTRAK.destep,GCTRAK.tofg);
  }

  // ANTI,  mod. by E.C.
//  numl=GCVOLU.nlevel;
//  numv=GCVOLU.number[numl-1];
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
  int manti(0);
  if(lvl==3 && GCVOLU.names[lvl][0]== 'A' && GCVOLU.names[lvl][1]=='N'
                                       && GCVOLU.names[lvl][2]=='T')manti=1;
  if(GCTRAK.destep != 0  && GCTMED.isvol != 0 && manti==1){
     GBIRK(dee);
     AMSAntiMCCluster::siantihits(GCVOLU.number[lvl],GCTRAK.vect,dee,GCTRAK.tofg);
//     HF1(1510,geant(iprt),1.);
  }
//
// -----> ECAL 1.0-version by E.C.
    if(GCTRAK.destep != 0.){
      if(lvl==6 && GCVOLU.names[lvl][0]== 'E' && GCVOLU.names[lvl][1]=='C'
               ){
//               && GCVOLU.names[lvl][2]=='F' && GCVOLU.names[lvl][3]=='C'){
//       
//       numvp=GCVOLU.number[numl-2];
//       cout<<"lev/vol="<<numl<<" "<<numv<<" name="<<name<<" x/y="<<x<<" "<<y<<" z="<<z<<" de="<<de<<endl;
//       for(i=0;i<4;i++)name[i]=GCVOLU.names[numl-2][i];
//       cout<<"vol(lev-1)="<<numvp<<" name="<<name<<endl;
       GBIRK(dee);
       AMSEcalMCHit::siecalhits(GCVOLU.number[lvl-1],GCTRAK.vect,dee,GCTRAK.tofg);
      }
    }
//

    if(GCVOLU.names[lvl][0]=='C' && GCVOLU.names[lvl][1]=='A' &&
     GCVOLU.names[lvl][2]=='T' && GCVOLU.names[lvl][3]=='O' && 
     GCTRAK.inwvol==1)
    {
      switch(GCKINE.ipart)
	{
	case 0:
        case 50: // Cerenkov photons
{	  // Do all the job: compute if the particle is detected
	  // using the detection eff(iciency) array and put it in the
	  // mccluster
          integer i;
	  geant wl=2*3.1415926*197.327e-9/GCTRAK.vect[6];
	  if(wl<RICHDB::wave_length[0] && 
	     wl>RICHDB::wave_length[RICHDB::entries]){
	    for(i=0;i<RICHDB::entries;i++)
	      if(RICHDB::wave_length[i]>wl && 
		 RICHDB::wave_length[i+1]<wl) break;
	    
	    // linear interpolation of eff: it's good because the bining is 
	    // small enough

	    geant dummy=0;
	    geant ieff=(RICHDB::eff[i+1]-RICHDB::eff[i])*
	      (wl-RICHDB::wave_length[i])/
	      (RICHDB::wave_length[i+1]-RICHDB::wave_length[i])
	      +RICHDB::eff[i];
	    geant rnumber=RNDM(dummy);

	    if(100*rnumber<ieff) // Detected!!!
	      { 
//		cout << "IPART " << GCKINE.ipart << "detected at:" << endl;
//		cout << GCVOLU.names[lvl][0] << GCVOLU.names[lvl][1] <<
//		  GCVOLU.names[lvl][2] << GCVOLU.names[lvl][3] << " copy number " <<
//		  GCVOLU.number[lvl] <<endl;
//		cout << GCVOLU.names[lvl-1][0] << GCVOLU.names[lvl-1][1] <<
//		  GCVOLU.names[lvl-1][2] << GCVOLU.names[lvl-1][3] << " copy number " <<
//		  GCVOLU.number[lvl-1] <<endl;
		AMSRichMCHit::sirichhits(GCVOLU.number[lvl]+10000*GCVOLU.number[lvl-1],
					 GCTRAK.vect,
					 GCTRAK.getot,
					 0); // 0 means gamma
					 
	      }
	  }
	  GCTRAK.istop=1; // Absorb it
	  break;
}
	default:
	  AMSRichMCHit::sirichhits(GCVOLU.number[lvl]+10000*GCVOLU.number[lvl-1],
				   GCTRAK.vect,
				   GCTRAK.getot,
				   GCKINE.ipart);
	  break;
   	}
    }
   

  GSKING(0);
  GSKPHO(0);
#ifndef __BATCH__
  GSXYZ();
#endif
  }
  
   catch (AMSuPoolError e){
    cerr << "GUSTEP  "<< e.getmessage();
    GCTRAK.istop =1;
     AMSEvent::gethead()->Recovery();
     return;
   }
   catch (AMSaPoolError e){
    cerr << "GUSTEP  "<< e.getmessage();
    GCTRAK.istop =1;
    AMSEvent::gethead()->Recovery();
      return;
   }
   catch (AMSTrTrackError e){
    cerr << "GUSTEP  "<< e.getmessage();
    GCTRAK.istop =1;
   }
   //  cout <<" gustep out"<<endl;
}
//-----------------------------------------------------------------------
extern "C" void guout_(){

#ifdef __DB__
   if (dbg_prtout != 0 && eventR > DBWriteGeom) {
     cout <<"guout_: read event of type "<<AMSJob::gethead() -> eventRtype()
          <<" from dbase"<<endl;
    if (AMSJob::gethead() -> isMCBanks()) cout <<"guout_: MCBanks exist"<<endl;
    if (AMSJob::gethead() -> isRecoBanks()) 
                                        cout <<"guout_: RecoBanks exist"<<endl;
   }
#endif

   if(AMSJob::gethead()->isSimulation())
   AMSgObj::BookTimer.stop("GEANTTRACKING");

   try{
          if(AMSEvent::gethead()->HasNoErrors())AMSEvent::gethead()->event();
   }
   catch (AMSuPoolError e){
     cerr << e.getmessage()<<endl;
     AMSEvent::gethead()->Recovery();
      return;
   }
   catch (AMSaPoolError e){
     cerr << e.getmessage()<<endl;
     AMSEvent::gethead()->Recovery();
      return;
   }
   catch (AMSTrTrackError e){
     cerr << e.getmessage()<<endl;
     cerr <<"Event dump follows"<<endl;
     AMSEvent::gethead()->_printEl(cerr);
     UPool.Release(0);
     AMSEvent::gethead()->remove();
     UPool.Release(1);
     AMSEvent::sethead(0);
      UPool.erase(0);
      return;
   }
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
// try to manipulate the conditions for writing....
   if(trig ){ 
     AMSEvent::gethead()->copy();
   }
     AMSEvent::gethead()->write(trig);
#ifdef __DB_All__
//+
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
//-
#endif

     UPool.Release(0);
   AMSEvent::gethead()->remove();
     UPool.Release(1);
   AMSEvent::sethead(0);
   UPool.erase(2000000);
}

extern "C" void gukine_(){

static integer event=0;

#ifdef __DB_All__
  if (AMSFFKEY.Read > 1) {
    readDB();
     return;
  }
#endif
// create new event & initialize it
  if(AMSJob::gethead()->isSimulation()){
    AMSgObj::BookTimer.start("GEANTTRACKING");
   if(IOPA.mode !=1 ){
    AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(
    new AMSEvent(AMSID("Event",GCFLAG.IEVENT),CCFFKEY.run,0,0,0)));
    for(integer i=0;i<CCFFKEY.npat;i++){
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
     return;
    }   
   }
  }
  else {
    //
    // read daq    
    //
    DAQEvent * pdaq=0;
    for(;;){
      pdaq = new DAQEvent();
      if(!(pdaq->read()))break;
      AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(
      new AMSEvent(AMSID("Event",pdaq->eventno()),pdaq->runno(),
      pdaq->runtype(),pdaq->time(),pdaq->usec())));
//      pdaq->runtype(),tm,pdaq->usec()))); // tempor introduced to read PC-made files
//<------      
      AMSEvent::gethead()->addnext(AMSID("DAQEvent",pdaq->GetBlType()), pdaq);
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
      //  AMSJob::gethead()->uhend();
      //  AMSJob::gethead()->uhinit(pdaq->runno(),pdaq->eventno());
      }
      guout_();
      if(GCFLAG.IEOTRI || GCFLAG.IEVENT >= GCFLAG.NEVENT)break;
      GCFLAG.IEVENT++;
    }
     GCFLAG.IEORUN=1;
     GCFLAG.IEOTRI=1;
     return; 
  }   
}


