#include "TrRecon.h"
#include "TkSens.h"


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

#pragma omp critical (trcpulim)
{
              if (retr3 == -2) {
		if (TrRecon::SigTERM)
		  cerr << "TrRecon::BuildTrTracks: SIGTERM detected: ";
		else
		  cerr << "TrRecon::BuildTrTracks: Cpulimit Exceeded: ";
		cerr << rec->GetCpuTime()
		     << " at Event: " << dec << getEvent() << endl;
	      }
}
	      //PZDEBUG 	printf("\nReconstructed %d tracks time\n",AMSEvent::gethead()->getC("AMSTrTrack")->getnelem()  );	      //PZDEBUG 	AMSgObj::BookTimer.print("TrTrack");
	    }
		}
      
      
      AMSgObj::BookTimer.stop("RETKEVENT");
    }

  // Fill histograms
  int nraw = AMSEvent::gethead()->getC("AMSTrRawCluster")->getnelem();
  int ncls = AMSEvent::gethead()->getC("AMSTrCluster"   )->getnelem();
  int nhit = AMSEvent::gethead()->getC("AMSTrRecHit"    )->getnelem();
  int ntrk = AMSEvent::gethead()->getC("AMSTrTrack"     )->getnelem();
  hman.Fill("TrSizeDt", ptr1->gettrtime(4), rec->GetTrackerSize());
  hman.Fill("TrNrawEv", getEvent(), nraw);
  hman.Fill("TrNclsEv", getEvent(), ncls);
  hman.Fill("TrNhitEv", getEvent(), nhit);

  hman.Fill("TrTime", nhit, rec->GetCpuTime()+(getEvent()%100)*1e-3);

  AMSTrCluster *cls = (AMSTrCluster *)AMSEvent::gethead()->getC("AMSTrCluster")->gethead();
  for (int i = 0; i < nraw && cls; i++) {
    hman.Fill((cls->GetSide() == 1) ? "TrClsSigP" 
	                            : "TrClsSigN", 0, cls->GetTotSignal());
    cls = (AMSTrCluster *)cls->next();
  }

  AMSTrTrack *trk = (AMSTrTrack *)AMSEvent::gethead()->getC("AMSTrTrack")->gethead();
  for (int i = 0; i < ntrk && trk; i++) {
    double sigp[8], sign[8];
    for (int j = 0; j < 8; j++) {
      sigp[j] = sign[j] = 0;

      TrRecHitR *hit = trk->GetHitL(j);
      if (hit) {
	int slot  = hit->GetTkId()%100;
	int layer = hit->GetLayer();
	TrClusterR *clx = hit->GetXCluster();
	TrClusterR *cly = hit->GetYCluster();

	hman.Fill("TrLadTrk", slot, layer);
	if (cly) { hman.Fill("TrLadYh",  slot, layer);
	  if (clx) hman.Fill("TrLadXYh", slot, layer);
	}
	if (clx) hman.Fill("TrClsSigN", 1, (sigp[j] = clx->GetTotSignal()));
	if (cly) hman.Fill("TrClsSigP", 1, (sign[j] = cly->GetTotSignal()));
      }
      else {
	AMSPoint play = trk->GetPlayer(j);
	TkSens tks(play);
	if (tks.LadFound()) {
	  int slot  = tks.GetLadTkID()%100;
	  int layer = abs(tks.GetLadTkID())/100;
	  hman.Fill("TrLadTrk", slot, layer);
	}
      }
    }

    hman.Fill("TrNhit", trk->GetNhitsY(), trk->GetNhitsXY());

    if (i == 0 && trk->GetNhitsY () >= 6 && 
	          trk->GetNhitsXY() >= 5) {
      double argt = fabs(trk->GetRigidity());
      hman.Fill("TrCsqX", argt, trk->GetNormChisqX());
      hman.Fill("TrCsqY", argt, trk->GetNormChisqY());

      double ssump = 0, ssumn = 0, smaxp = 0, smaxn = 0;
      int    nsump = 0, nsumn = 0;
      for (int j = 0; j < 8; j++) {
	TrRecHitR *hit = trk->GetHitL(j);
	AMSPoint   res = trk->GetResidual(j);
	if (hit && !hit->OnlyY()) hman.Fill("TrResX", argt, res.x()*1e4);
	if (hit && !hit->OnlyX()) hman.Fill("TrResY", argt, res.y()*1e4);

	if (sigp[j] > 0) { ssump += sigp[j]; nsump++; }
	if (sign[j] > 0) { ssumn += sign[j]; nsumn++; }
	if (sigp[j] > smaxp) smaxp = sigp[j];
	if (sign[j] > smaxn) smaxn = sign[j];
      }

      // Fill truncated mean of charge
      if (nsump-1 > 0) hman.Fill("TrChgP", argt, (ssump-smaxp)/(nsump-1));
      if (nsumn-1 > 0) hman.Fill("TrChgN", argt, (ssumn-smaxn)/(nsumn-1));
    }

    trk = (AMSTrTrack *)trk->next();
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
