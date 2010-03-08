//  $Id: event_tk.C,v 1.16 2010/03/08 15:38:06 shaino Exp $
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
  static int countcerr=0;
  Trigger2LVL1 *ptr1=(Trigger2LVL1*)getheadC("TriggerLVL1",0);
    
  TrRecon* rec= new TrRecon();

  int nraw = AMSEvent::gethead()->getC("AMSTrRawCluster")->getnelem();
  AMSgObj::BookTimer.start("RETKEVENT");

  bool lowdt = (ptr1->gettrtime(4) <= TrRecon::RecPar.lowdt);
  hman.Fill((lowdt ? "TrNrawLt" : "TrNrawHt"), getEvent(), nraw);

  trstat = 0;
  if (nraw >=TrRecon::RecPar.MaxNrawCls) trstat |= 1;

  //RAW Clusters -->  Clusters
  if(TRCLFFKEY.recflag>0 &&     // Wanted from TkDCards
     ptr1 &&                    // LVL1 exists
     nraw <TrRecon::RecPar.MaxNrawCls) // Not too many RAW Clusters
  {
    AMSgObj::BookTimer.start("TrCluster");
    int retr=rec->BuildTrClusters();
    int ncls=AMSEvent::gethead()->getC("AMSTrCluster")->getnelem();
    AMSgObj::BookTimer.stop("TrCluster");
    hman.Fill((lowdt ? "TrNclsLt" : "TrNclsHt"), getEvent(), ncls);

    if ( lowdt && ncls>=TrRecon::RecPar.MaxNtrCls_ldt) trstat |= 2;
    if (!lowdt && ncls>=TrRecon::RecPar.MaxNtrCls)     trstat |= 4;

    // Clusters --> Hits
    if(TRCLFFKEY.recflag>10 && // Wanted from TkDCards
       retr>=0 &&              // Clusters are built
       (( lowdt && ncls<TrRecon::RecPar.MaxNtrCls_ldt)|| // at small dt
	(!lowdt && ncls<TrRecon::RecPar.MaxNtrCls)))     // at large dt
    {
      AMSgObj::BookTimer.start("TrRecHit");
      int retr2=rec->BuildTrRecHits();
      int nhit =AMSEvent::gethead()->getC("AMSTrRecHit")->getnelem();
      AMSgObj::BookTimer.stop("TrRecHit");
      hman.Fill((lowdt ? "TrNhitLt" : "TrNhitHt"), getEvent(), nhit);

      if (nhit>=TrRecon::RecPar.MaxNtrHit) trstat |= 8;

      // Hits --> Tracks
      int tflg = ptr1->gettoflag1();
      if(TRCLFFKEY.recflag>110 &&          // Wanted from TkDCards
	 retr2>=0&&                        // Hits are built
	 nhit<TrRecon::RecPar.MaxNtrHit && // Not to many Hits
	 tflg>=0&&tflg<9)            // TOF trigger pattern at least 1U and 1L
      {
	AMSgObj::BookTimer.start("TrTrack");
	TrRecon::RecPar.NbuildTrack++;
	int retr3=rec->BuildTrTracks();
	AMSgObj::BookTimer.stop("TrTrack");

#pragma omp critical (trcpulim)
	if (rec->CpuTimeUp() || TrRecon::SigTERM) {
	  trstat |= 16;
	  cerr << "TrRecon::BuildTrTracks: "
	       << ((TrRecon::SigTERM) ? "SIGTERM detected: "
		                      : "Cpulimit Exceeded: ") << dec
	       << rec->GetCpuTime() << " at Event: " << getEvent() << endl;
	}
      } // Hits --> Tracks

      // Purge "ghost" hits and assign hit index to tracks
      rec->PurgeGhostHits();

    } // Clusters --> Hits
      
  } //RAW Clusters -->  Clusters

  AMSgObj::BookTimer.stop("RETKEVENT");

#pragma omp critical (trtrstat)
  {
    static int nfill = 0, ntevt = 0, ifirst = 1;
    nfill++;
    int ntrk = AMSEvent::gethead()->getC("AMSTrTrack")->getnelem();
    if (ntrk > 0) ntevt++;
    if (trstat > 0) {
         static int qprint=0;
      if (qprint++<100 && !lowdt) {
	cout << "AMSEvent::_retkevent-I-Trstat= ";
	for (int i = 0; i < 5; i++) cout << ((trstat>>i)&1);
	cout << " at Event: " << getEvent() << endl;
      }
      if (trstat &  1) TrRecon::RecPar.NcutRaw++;
      if (trstat &  2) TrRecon::RecPar.NcutLdt++;
      if (trstat &  4) TrRecon::RecPar.NcutCls++;
      if (trstat &  8) TrRecon::RecPar.NcutHit++;
      if (trstat & 16) TrRecon::RecPar.NcutCpu++;
    }
    if (nfill%10000 == 0) {
    if (ifirst) {
	cout << "AMSEvent::_retkevent-I-Report: "
	     << "  Nfill  NevTrk  Rtrk  Rldt Rhcut  Rcpu TrTime" << endl;
	ifirst = 0;
      }
    float trtime = (hman.Get("TrTimH"))
      ? ((TH2D *)hman.Get("TrTimH"))->GetMean(2) : 0;
      cout << "AMSEvent::_retkevent-I-Report: "
	   << Form("%7d %7d %5.3f %5.3f %5.3f %5.3f %6.4f",
		   nfill, ntevt, 1.*ntevt/nfill,
		   1.* TrRecon::RecPar.NcutLdt /nfill,
		   1.*(TrRecon::RecPar.NcutCls+
		       TrRecon::RecPar.NcutHit)/nfill,
		   1.* TrRecon::RecPar.NcutCpu /nfill,
		   trtime) << endl;
    }
  }

  // Fill histograms
  int nhit = AMSEvent::gethead()->getC("AMSTrRecHit")->getnelem();
  int ntrk = AMSEvent::gethead()->getC("AMSTrTrack" )->getnelem();
  hman.Fill("TrSizeDt", ptr1->gettrtime(4), rec->GetTrackerSize());
  hman.Fill("TrTimH", nhit, rec->GetCpuTime());
  hman.Fill("TrTimT", ntrk, rec->GetCpuTime());
  hman.Fill("TrRecon", trstat);

  AMSTrCluster *cls 
    = (AMSTrCluster *)AMSEvent::gethead()->getC("AMSTrCluster")->gethead();
  for (int i = 0; i < nraw && cls; i++) {
    hman.Fill((cls->GetSide() == 1) ? "TrClsSigP" 
	                            : "TrClsSigN", 0, cls->GetTotSignal());
    cls = (AMSTrCluster *)cls->next();
  }

  AMSTrTrack *trk 
    = (AMSTrTrack *)AMSEvent::gethead()->getC("AMSTrTrack")->gethead();
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

    int pate = TrRecon::GetHitPatternAllow(trk->GetPattern());
    if (i == 0 && pate == 0 && trk->GetNhitsY () >= 6 && 
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
