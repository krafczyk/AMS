#include "TrRecon.h"


void AMSEvent::_sitkinitevent(){

  AMSEvent::gethead()->add (
			    new AMSContainer(AMSID("AMSContainer:AMSTrMCCluster",0),0));
}
 
void AMSEvent::_retkinitevent(){




    AMSEvent::gethead()->add (
			      new AMSContainer(AMSID("AMSContainer:AMSTrRawCluster",0),0));

 
    AMSEvent::gethead()->add (
			      new AMSContainer(AMSID("AMSContainer:AMSTrCluster",0),0));
    
    AMSEvent::gethead()->add (
			      new AMSContainer(AMSID("AMSContainer:AMSTrRecHit",0),0));
    
    AMSEvent::gethead()->add (
			      new AMSContainer(AMSID("AMSContainer:AMSTrTrack",0),0));

    AMSEvent::gethead()->add (
 			      new AMSContainer(AMSID("AMSContainer:AMSVtx",0),0));

}
 


void AMSEvent::_catkinitevent(){

//PZ FIXME CALIB  if(TRCALIB.CalibProcedureNo == 2){
//     AMSEvent::gethead()->add (
// 			      new AMSContainer(AMSID("AMSContainer:AMSTrCalibration",0),0));
//   }
}


void AMSEvent::_catkevent(){
//PZ FIXME CALIB  AMSgObj::BookTimer.start("CalTrFill");
//   if(TRALIG.UpdateDB){
//     if(!TRALIG.GlobalFit)AMSTrAligFit::Test();
//     else AMSTrAligFit::Testgl();
//   }
//   if(TRCALIB.CalibProcedureNo == 1){
//     AMSTrIdCalib::check();
//   }
//   else if(TRCALIB.CalibProcedureNo == 2){
//     int i,j;
//     for(i=0;i<nalg;i++){
//       if(TRCALIB.Method==0 || TRCALIB.Method==i){
// 	int ps=TRCALIB.PatStart;
// 	int pe=tkcalpat;
// 	if(TRCALIB.MultiRun)pe=ps+2;
// 	for(j=ps;j<pe;j++){
// 	  if(AMSTrCalibFit::getHead(i,j)->Test()){
// 	    AMSgObj::BookTimer.start("CalTrFit");
// 	    AMSTrCalibFit::getHead(i,j)->Fit();
// 	    AMSgObj::BookTimer.stop("CalTrFit");
// 	  }
// 	}
//       }
//     }
//   }
//   AMSgObj::BookTimer.stop("CalTrFill");

}



void AMSEvent::_retkevent(integer refit){

  Trigger2LVL1 *ptr1=(Trigger2LVL1*)getheadC("TriggerLVL1",0);
    
  TrRecon* rec= new TrRecon();
  //PZDEBUG  printf("\nFound %d Raw cluster \n",AMSEvent::gethead()->getC("AMSTrRawCluster")->getnelem()  );
  //  if(TRCLFFKEY.recflag>0 && ptr1 && (!LVL3FFKEY.Accept ||  (ptr1 && ptr && (ptr302 && ptr302->LVL3OK())))) //tempor
  
  //RAW Clusters -->  Clusters
  if(
     TRCLFFKEY.recflag>0 && // Wanted from TkDCards
     ptr1 &&                // LVL1 exists
     AMSEvent::gethead()->getC("AMSTrRawCluster")->getnelem() <TrRecon::MaxNrawCls // Not too many RAW Clusters
     )
    { 
      AMSgObj::BookTimer.start("RETKEVENT");
      AMSgObj::BookTimer.start("TrCluster");
      int retr=rec->BuildTrClusters();
      int ncls=AMSEvent::gethead()->getC("AMSTrCluster")->getnelem();
      AMSgObj::BookTimer.stop("TrCluster");
      
      
      //PZDEBUG        printf("\nReconstructed %d cluster time\n",AMSEvent::gethead()->getC("AMSTrCluster")->getnelem()  );
      //PZDEBUG       AMSgObj::BookTimer.print("TrCluster");
      

      // Clusters --> Hits
      if(
	 TRCLFFKEY.recflag>10 && // Wanted from TkDCards
	 retr>=0 &&  //Clusters are built
	 (
	  (ptr1->gettrtime(4)<= TrRecon::lowdt &&ncls<TrRecon::MaxNtrCls_ldt)||  //Not to many Cls at small dt
	  (ptr1->gettrtime(4)>  TrRecon::lowdt &&ncls<TrRecon::MaxNtrCls)        //Not to many Cls at large dt
	  )
	 
	 )
	{
	  AMSgObj::BookTimer.start("TrRecHit");
	  int retr2=rec->BuildTrRecHits();
	  AMSgObj::BookTimer.stop("TrRecHit");
	  //PZDEBUG       printf("\nReconstructed %d hits time\n",AMSEvent::gethead()->getC("AMSTrRecHit")->getnelem()  );
	  //PZDEBUG       AMSgObj::BookTimer.print("TrRecHit");
	  
	  
	  // Hits --> Tracks
	  if(
	     TRCLFFKEY.recflag>110 && // Wanted from TkDCards
	     retr2>=0&& AMSEvent::gethead()->getC("AMSTrRecHit")->getnelem()<TrRecon::MaxNtrHit && //Not to many Hits
	     ptr1->gettoflag1()>=0&&ptr1->gettoflag1()<9
	     )
	    {
	      AMSgObj::BookTimer.start("TrTrack");
	      int retr3=rec->BuildTrTracks();
	      double bb=AMSgObj::BookTimer.stop("TrTrack");
	      
	      //PZDEBUG 	printf("\nReconstructed %d tracks time\n",AMSEvent::gethead()->getC("AMSTrTrack")->getnelem()  );	      //PZDEBUG 	AMSgObj::BookTimer.print("TrTrack");

	      	      hman.Fill("TrTime",bb);
	    }
	
	}
      
      
      AMSgObj::BookTimer.stop("RETKEVENT");
    }
  //  else throw AMSLVL3Error("LVL3NotCreated");  
  if(rec) delete rec;
}



void AMSEvent::_sitkinitrun(){
//PZFIXME   NOISE SIMULATION int l;
//   for(l=0;l<2;l++){
//     for (int i=0;i<TKDBc::nlay();i++){
//       for (int j=0;j<TKDBc::nlad(i+1);j++){
// 	for (int s=0;s<2;s++){
// 	  AMSTrIdSoft id(i+1,j+1,s,l,0);
// 	  if(id.dead())continue;
// 	  number oldone=0;
// 	  for(int k=0;k<TKDBc::NStripsDrp(i+1,l);k++){
// 	    id.upd(k);
// 	    geant d;
// 	    id.setindnoise()=oldone+
// 	      AMSTrMCCluster::sitknoiseprob(id ,id.getsig()*TRMCFFKEY.thr1R[l]);
// 	    oldone=id.getindnoise();
// 	  }
// 	}
//       }
//     }
//   }
//   // Fill the histos
//   for(l=0;l<2;l++){
//     geant p0=AMSTrMCCluster::sitknoiseprobU(TRMCFFKEY.thr1R[l],AMSTrMCCluster::step());
//     for(int i=0;i<AMSTrMCCluster::ncha();i++){
//       geant x=TRMCFFKEY.thr1R[l]+i*AMSTrMCCluster::step();
//       geant prob=AMSTrMCCluster::sitknoiseprobU(x,AMSTrMCCluster::step());
//       HF1(AMSTrMCCluster::hid(l),x,prob/p0);
//     }
// #ifdef __AMSDEBUG__
//     HPRINT(AMSTrMCCluster::hid(l));
// #endif
// #ifdef __G4AMS__
//     if(MISCFFKEY.G4On)AMSRandGeneral::book(AMSTrMCCluster::hid(l));
// #endif
//   }
}



void AMSEvent::_retkinitrun(){
//PZ FIXME JUST PRINTOUT  // Warning if TRFITFFKEY.FastTracking is on...
//   if(TRFITFFKEY.FastTracking){
//     cout <<" "<<endl<<
//       "AMSEvent::Retkinitrun-W-TRFITFFKEY.FastTracking selected. "<<TRFITFFKEY.FastTracking<<endl;
//   }
//   for (int half=0;half<2;half++){
//     for(int side=0;side<2;side++){
//       cout <<"AMSEvent::_retkevent-I-"<<AMSTrIdCalib::CalcBadCh(half,side)<<
// 	" bad channels found for half "<<half<<" and side "<<side<<endl;
//     }
//   }
}



void AMSEvent:: _sitkevent(){
  //PZ FIXME NOISE SIMULATION bool fastrigger= TOF2RawSide::GlobFasTrigOK();
  //if(TRMCFFKEY.NoiseOn && fastrigger )AMSTrMCCluster::sitknoise();
  //AMSTrMCCluster::sitkcrosstalk();

  TrSim::sitkdigi();

}
