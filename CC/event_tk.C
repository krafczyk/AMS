//  $Id: event_tk.C,v 1.41 2011/03/29 15:48:45 pzuccon Exp $
#include "TrRecon.h"
#include "TrSim.h"
#include "TkSens.h"


void AMSEvent::_sitkinitevent(){
  // printf("AMSEvent::_sitkinitevent()\n");
  AMSEvent::gethead()->add(new AMSContainer(AMSID("AMSContainer:AMSTrMCCluster",0),0));
}
 
void AMSEvent::_retkinitevent(){
  AMSEvent::gethead()->add(new AMSContainer(AMSID("AMSContainer:AMSTrRawCluster",0),0));
  AMSEvent::gethead()->add(new AMSContainer(AMSID("AMSContainer:AMSTrCluster",0),0));
  AMSEvent::gethead()->add(new AMSContainer(AMSID("AMSContainer:AMSTrRecHit",0),0));
  AMSEvent::gethead()->add(new AMSContainer(AMSID("AMSContainer:AMSTrTrack",0),0));
  AMSEvent::gethead()->add(new AMSContainer(AMSID("AMSContainer:AMSVtx",0),0));
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
    
  TrRecon rec;

  int nraw = AMSEvent::gethead()->getC("AMSTrRawCluster")->getnelem();
  AMSgObj::BookTimer.start("RETKEVENT");
  bool lowdt = false;
  if(ptr1) lowdt=(ptr1->gettrtime(4) <= TrRecon::RecPar.lowdt);
  hman.Fill((lowdt ? "TrNrawLt" : "TrNrawHt"), getEvent(), nraw);

  trstat = 0;
  if (nraw >=TrRecon::RecPar.MaxNrawCls) trstat |= 1;

  //RAW Clusters -->  Clusters
  if(TRCLFFKEY.recflag>0 &&     // Wanted from TkDCards
     //   ptr1 &&                    // LVL1 exists
     nraw <TrRecon::RecPar.MaxNrawCls) // Not too many RAW Clusters
  {
    AMSgObj::BookTimer.start("TrCluster");
    int retr=rec.BuildTrClusters();
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
      int retr2=rec.BuildTrRecHits();
      int nhit =AMSEvent::gethead()->getC("AMSTrRecHit")->getnelem();
      AMSgObj::BookTimer.stop("TrRecHit");
      hman.Fill((lowdt ? "TrNhitLt" : "TrNhitHt"), getEvent(), nhit);

      if (nhit>=TrRecon::RecPar.MaxNtrHit) trstat |= 8;

      // Hits --> Tracks
      int tflg =0;
      if(ptr1)ptr1->gettoflag1();
      if(TRCLFFKEY.recflag>110 &&          // Wanted from TkDCards
	 retr2>=0&&                        // Hits are built
	 nhit<TrRecon::RecPar.MaxNtrHit  // Not to many Hits
	 &&	 tflg>=0&&tflg<9            // TOF trigger pattern at least 1U and 1L
	 )
	{
	  AMSgObj::BookTimer.start("TrTrack");
	  TrRecon::RecPar.NbuildTrack++;
	  rec.BuildTrTracks();
	  AMSgObj::BookTimer.stop("TrTrack");
	  
#pragma omp critical (trcpulim)
	  if (rec.CpuTimeUp() || TrRecon::SigTERM) {
	    trstat |= 16;
	    cerr << "TrRecon::BuildTrTracks: "
		 << ((TrRecon::SigTERM) ? "SIGTERM detected: "
		     : "Cpulimit Exceeded: ") << dec
		 << rec.GetCpuTime() << " at Event: " << getEvent() << endl;
	  }
	} // Hits --> Tracks
      
      // Purge "ghost" hits and assign hit index to tracks
      rec.PurgeGhostHits();

      // Check hit indexes
      VCon* contT = GetVCon()->GetCont("AMSTrTrack");
      int ntrack=contT->getnelem();
      for (int i = 0; i < ntrack; i++) {
	TrTrackR *track = (TrTrackR*)contT->getelem(i);

	for(int j=0;j<track->GetNhits();j++)
	  if (track->iTrRecHit(j) < 0) {
	    static int nerr = 0;
	    if (nerr++ < 100)
	      cerr << "TrRecon::PurgeGhostHits-W- wrong hit index: "
		   << "at Event " << getEvent() 
		   << " track " << i << " hit " << j << " " << endl;
	  }
      }
      delete contT;


    } // Clusters --> Hits
      
  } //RAW Clusters -->  Clusters

  AMSgObj::BookTimer.stop("RETKEVENT");

#pragma omp critical (trtrstat)
  {
    static int nfill = 0, ntevt = 0, ntful =0, ifirst = 1;
    nfill++;
    int ntrk = AMSEvent::gethead()->getC("AMSTrTrack")->getnelem();
    if (ntrk > 0) {
      ntevt++;
      AMSTrTrack *trk 
	= (AMSTrTrack *)AMSEvent::gethead()->getC("AMSTrTrack")->gethead();
      if(TkDBc::Head->GetSetup()==3){
	if (trk->ParExists(TrTrackR::kChoutko | TrTrackR::kFitLayer8
			                      | TrTrackR::kFitLayer9)) 
	  ntful++;
      }else
	if (trk->ParExists(TrTrackR::kChoutko )) 
	  ntful++;
      
    }
    if (trstat > 0) {
         static int qprint=0;
      if (0 && qprint++<100 && !lowdt) {
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
    int intv = 10000;
    if (nfill < 10000) intv = 1000;
    if (nfill%intv == 0) {
      if (ifirst) {
	cout << "_retkevent-I-Report: "
	     << "  Nfill  NevTrk  NevT89  Rtrk  Rldt Rhcut  Rcpu TrTime"
	     << endl;
	ifirst = 0;
      }
      float trtime = (hman.Get("TrTimH"))
	? ((TH2D *)hman.Get("TrTimH"))->GetMean(2) : 0;
      cout << "_retkevent-I-Report: "
	   << Form("%7d %7d %7d %5.3f %5.3f %5.3f %5.3f %6.4f",
		   nfill, ntevt, ntful, 1.*ntevt/nfill,
		   1.* TrRecon::RecPar.NcutLdt /nfill,
		   1.*(TrRecon::RecPar.NcutCls+
		       TrRecon::RecPar.NcutHit)/nfill,
		   1.* TrRecon::RecPar.NcutCpu /nfill,
		   trtime) << endl;
    }
  } // #pragma omp critical (trtrstat)

#pragma omp critical (trhist)
 {
  // Fill histograms
  int nhit = AMSEvent::gethead()->getC("AMSTrRecHit")->getnelem();
  int ntrk = AMSEvent::gethead()->getC("AMSTrTrack" )->getnelem();
  if(ptr1)  hman.Fill("TrSizeDt", ptr1->gettrtime(4), rec.GetTrackerSize());
  hman.Fill("TrTimH", nhit, rec.GetCpuTime());
  hman.Fill("TrTimT", ntrk, rec.GetCpuTime());
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

  AMSmceventg *mcg = 0;
  if (AMSJob::gethead()->isSimulation()) {
    mcg = (AMSmceventg *)AMSEvent::gethead()->getC("AMSmceventg")->gethead();
    if (mcg && mcg->getcharge() == 0) mcg = 0;
  }

  for (int i = 0; i < ntrk && trk; i++) {
    double sigp[9], sign[9];
    for (int j = 0; j < TkDBc::Head->nlay(); j++) {
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
	TkSens tks(play,0);
	if (tks.LadFound())
	  hman.Fill("TrLadTrk", tks.GetLadTkID()%100,
		                abs(tks.GetLadTkID())/100);
      }
    }

    hman.Fill("TrNhit", trk->GetNhitsY(), trk->GetNhitsXY());

    int pate = patt->GetHitPatternAllow(trk->GetPattern());
    int mfs  = TrTrackR::DefaultFitID;
    if (i == 0 && trk->ParExists(mfs) &&
	pate == 0 && trk->GetNhitsY () >= 6 && 
	             trk->GetNhitsXY() >= 5) {
      double argt = fabs(trk->GetRigidity(mfs));
      hman.Fill("TrCsqX", argt, trk->GetChisqX(mfs));
      hman.Fill("TrCsqY", argt, trk->GetChisqY(mfs));

      double ssump = 0, ssumn = 0, smaxp = 0, smaxn = 0;
      int    nsump = 0, nsumn = 0;
      for (int j = 0; j < TkDBc::Head->nlay(); j++) {
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

    if (i == 0 && TkDBc::Head->GetSetup()==3) {
      int mf0 = TrTrackR::DefaultFitID;
      int mfs[4] = { mf0, mf0 | TrTrackR::kFitLayer8,
		          mf0 | TrTrackR::kFitLayer9,
		          mf0 | TrTrackR::kFitLayer8
		              | TrTrackR::kFitLayer9 };
      int mf8 = mfs[1];
      int mf9 = mfs[2];

      AMSPoint pl8, pl9;
      AMSDir   dr8, dr9;
      trk->InterpolateLayer(7, pl8, dr8, mf0);
      trk->InterpolateLayer(8, pl9, dr9, mf0);
      AMSPoint p0;

      double rrgt = (!mcg) ? trk->GetRigidity() 
	                   : mcg->getmom()/mcg->getcharge();
      double argt = TMath::Abs(rrgt);


     if (pl8.dist(p0) > 1e-3 && pl9.dist(p0) > 1e-3) {
      hman.Fill("TrPtkL8", pl8.x(), pl8.y());
      hman.Fill("TrPtkL9", pl9.x(), pl9.y());

      if (trk->ParExists(mf8) && trk->ParExists(mf0) && 
	  trk->GetChisqX(mf8) < 100 && trk->GetChisqY(mf8) < 100) {
	AMSPoint   res = trk->GetResidual(7, mf0);
	TrRecHitR *hit = trk->GetHitL(7);
	hman.Fill("TrPftL8", pl8.x(), pl8.y());

	if (hit && !hit->OnlyY()) {
	  hman.Fill("TrAlg81", pl8.x(), dr8.x()/dr8.z(), res.x());
	  hman.Fill("TrAlg82", pl8.y(), dr8.x()/dr8.z(), res.x());
	}
	if (hit && !hit->OnlyX()) {
	  hman.Fill("TrAlg83", pl8.x(), dr8.y()/dr8.z(), res.y());
	  hman.Fill("TrAlg84", pl8.y(), dr8.y()/dr8.z(), res.y());
	}
      }

      if (trk->ParExists(mf9) && trk->ParExists(mf0) && 
	  trk->GetChisqX(mf9) < 100 && trk->GetChisqY(mf9) < 100) {
	AMSPoint   res = trk->GetResidual(8, mf0);
	TrRecHitR *hit = trk->GetHitL(8);
	hman.Fill("TrPftL9", pl9.x(), pl9.y());

	if (hit && !hit->OnlyY()) {
	  hman.Fill("TrAlg91", pl9.x(), dr9.x()/dr9.z(), res.x());
	  hman.Fill("TrAlg92", pl9.y(), dr9.x()/dr9.z(), res.x());
	}
	if (hit && !hit->OnlyX()) {
	  hman.Fill("TrAlg93", pl9.x(), dr9.y()/dr9.z(), res.y());
	  hman.Fill("TrAlg94", pl9.y(), dr9.y()/dr9.z(), res.y());
	}
      }

     }

     // residual vs layer
     for (int ilayer=0; ilayer<9; ilayer++) {
       if (trk->GetHitL(ilayer)==0) continue;
       if (trk->GetHitL(ilayer)->GetXCluster()!=0) 
         hman.Fill("TrResLayx",ilayer,trk->GetResidual(ilayer,TrTrackR::DefaultFitID).x()*1.e+04);
       if (trk->GetHitL(ilayer)->GetYCluster()!=0) 
         hman.Fill("TrResLayy",ilayer,trk->GetResidual(ilayer,TrTrackR::DefaultFitID).y()*1.e+04);
     }  
    }

    if ( (i==0) && AMSJob::gethead()->isSimulation()
	        && (TkDBc::Head->GetSetup()==3) ) {
      TrSim::fillreso(trk);

      int mf0 = TrTrackR::DefaultFitID;
      int mfit[4] = { mf0, mf0 | TrTrackR::kFitLayer8,
		           mf0 | TrTrackR::kFitLayer9,
		           mf0 | TrTrackR::kFitLayer8 | TrTrackR::kFitLayer9 };

      if (mcg && mcg->getcharge() != 0) {
	double pmc = mcg->getmom();
	double rmc = mcg->getmom()/mcg->getcharge();

	if (trk->ParExists(mfit[0])) {
	  if (trk->ParExists(mfit[1]))
	    hman.Fill("TrDtyL81", rmc, trk->GetResidual(7, mfit[0]).y());
	  if (trk->ParExists(mfit[2]))
	    hman.Fill("TrDtyL91", rmc, trk->GetResidual(8, mfit[0]).y());
	}
	if (trk->ParExists(mfit[3])) {
	  if (trk->ParExists(mfit[2]))
	    hman.Fill("TrDtyL82", rmc, trk->GetResidual(7, mfit[2]).y());
	  if (trk->ParExists(mfit[1]))
	    hman.Fill("TrDtyL92", rmc, trk->GetResidual(8, mfit[1]).y());
	}

	for (int j = 0; j < 4; j++) {
	  if (!trk->ParExists(mfit[j])) continue;
	  double rgt = trk->GetRigidity(mfit[j]);
	  if (rgt != 0 && rmc != 0) {
	    hman.Fill(Form("TrRres%d1", j+1), pmc, 1e3*(1/rgt-1/rmc));
	    hman.Fill(Form("TrRres%d2", j+1), pmc, rmc*(1/rgt-1/rmc));
	  }
	}
      }
    }   
    trk = (AMSTrTrack *)trk->next();
  }
 } // #pragma omp critical (trhist)
}



void AMSEvent::_sitkinitrun(){
  // printf("AMSEvent::_sitkinitrun()\n");

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
  // printf("AMSEvent:: _sitkevent()\n");
  TrSim::sitkdigi();

  //PZ FIXME NOISE SIMULATION bool fastrigger= TOF2RawSide::GlobFasTrigOK();
  //if(TRMCFFKEY.NoiseOn && fastrigger )AMSTrMCCluster::sitknoise();
  //AMSTrMCCluster::sitkcrosstalk();
}
