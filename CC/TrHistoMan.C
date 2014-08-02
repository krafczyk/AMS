#define __TrHistoMan_C__

///////////////////////////////////////////////////////////////////////////
///
///\file  TrHistoMan.h
///\brief Source file of TrHistoMan class
///
///\date  2008/02/16 AO  First version
///
//////////////////////////////////////////////////////////////////////////

#include "TrHistoMan.h"

#include <iostream>

using namespace std;

ClassImp(TrHistoManHeader);
ClassImp(TrHistoMan);
ClassImp(TrOnlineMonitor);


/////////////////////////////////////////////////
// TrOnlineMonitor 
/////////////////////////////////////////////////


TIME_EVENT TrOnlineMonitor::time_event;
TrOnlineMonitor* ptrman = 0;


TrOnlineMonitor::~TrOnlineMonitor() {
   if (fOwnerItself) { 
     if (fTimeNtuple) delete fTimeNtuple; 
   }
   fFlag = 0; 
   fDtMin = 0; 
   fTimeNtuple = 0;
}


TrOnlineMonitor::TrOnlineMonitor(const TrOnlineMonitor &orig) : TrHistoMan(orig) {
  fFlag = orig.fFlag; 
  fDtMin = orig.fDtMin;
}


void TrOnlineMonitor::Book() {

  char histoname[100];
  char histotitle[100];

  // Size and DT
  DefineTracker("DT","; #Deltat (us)",1000,0,10000,0);
  DefineTracker("Size","; Tracker size (byte)",2400,0.,24000.,0);
  DefineTracker("Size_vs_DT","; #Deltat (us); Tracker Size (byte)",450,0.,500.,300,0.,24000.,0);   
  DefineTracker("Size_vs_Crate","; Crate; Crate Segment Size (byte)",8,-0.5,7.5,1000,0.,24000.,0);      
  for (int icrate=0; icrate<8; icrate++) {
    sprintf(histoname,"T%1dSize_vs_DT",icrate);
    sprintf(histotitle,"; #Deltat (us); Crate T\\%1d Size (byte)",icrate);
    DefineTracker(histoname,histotitle,450,50.,500.,400,0.,10000.,0);
  }

  // RawClusters
  DefineTracker("nRawClusters","; n. of Raw Clusters",501,-0.5,500.5,0);
  DefineTracker("nRawClusters_vs_Ladder","; iCrate*24 + iTDR; n. of Raw Clusters",192,-0.5,191.5,101,-0.5,100.5,1);
  DefineTracker("nRawClusters_vs_DT","; #Deltat (us); n. of Raw Clusters",450,50.,500.,501,-0.5,500.5,0);

  // Clusters
  DefineTracker("nClusters","; n. of Clusters",401,-0.5,400.5,0);
  DefineTracker("nClusters_vs_DT","; #Deltat (us); n. of Clusters",450,0.,500.,401,-0.5,400.5,0);

  // Hits
  DefineTracker("nRecHits","; n. of Rec. Hits",201,-0.5,200.5,0);
  DefineTracker("nRecHits_vs_DT","; #Deltat (us); n. of Rec. Hits",450,0.,500.,201,-0.5,200.5,0);
  
  // Tracks
  DefineTracker("nTracks","; n. of Tracks",11,-0.5,10.5,0);
  DefineTracker("nTracks_vs_DT","; #Deltat (us); n. of Tracks",450,0.,500.,11,-0.5,10.5,0);

  // Signal
  DefineTracker("Signal","; Amplitude (ADC)",100,0.,250.,1);                                                           // beta>0.9
  DefineTracker("SignalOnTrack","; Amplitude (ADC)",100,0.,250.,1);                                                    // beta>0.9
  DefineTracker("SignalOnTrackY_vs_X","; Y Amp. (ADC); X Amp. (ADC)",100,0.,250.,100,0.,250.,kFALSE);                  // beta>0.9
  DefineTracker("sqrtSignal","; Amplitude (#sqrt{ADC})",300,0.,70.,kTRUE);                                             // beta>0.9
  DefineTracker("sqrtSignalOnTrack","; Amplitude (#sqrt{ADC})",300,0.,70.,kTRUE);                                      // beta>0.9
  DefineTracker("sqrtSignalOnTrackY_vs_X","; Y Amp. (#sqrt{ADC}); X Amp. (#sqrt{ADC})",300,0.,70.,300,0.,70.,kFALSE);  // beta>0.9
  DefineTracker("sqrtSignalOnTrack_vs_Beta","; #beta; Amplitude (#sqrt{ADC})",300,-1.2,1.2,300,0.,70.,kTRUE);

  // Ladder Summary Plots  
  DefineEntries("RawEntries",kTRUE);
  DefineEntries("TrackEntries",kTRUE);
  DefineTracker("Size_vs_Ladder","; iCrate*24 + iTDR; Ladder Segment Size (byte)",192,-0.5,191.5,300,0.,600.,1);
  DefineTracker("SizeLowDT_vs_Ladder","; iCrate*24 + iTDR; Ladder Segment Size for #Deltat<200 us (byte)",192,-0.5,191.5,300,0.,600.,1); 
  DefineTracker("Signal_vs_Ladder","; iCrate*24 + iTDR; Amplitude (ADC)",192,-0.5,191.5,100,0.,250.,1);
  DefineTracker("SignalLowDT_vs_Ladder","; iCrate*24 + iTDR; Amplitude for #Deltat<200 us (ADC)",192,-0.5,191.5,100,0.,250.,1);
  DefineTracker("NElement_vs_Ladder","; iCrate*24 + iTDR; n. of Strips",192,-0.5,191.5,100,0.,100.,1);
  DefineTracker("NElementLowDT_vs_Ladder","; iCrate*24 + iTDR; n. of Strips for #Deltat<200 us",192,-0.5,191.5,100,0.,100.,1);

  // Cluster and hit occupancy (raw beam profile)
  DefineLayers("ClusterX","; 0-multiplicity X coordinate (cm)",200,-70.,70.,kFALSE);
  DefineLayers("ClusterY","; Y coordinate (cm)",200,-70.,70.,kFALSE);
  DefineLayers("RecHitY_vs_X","; 0-multiplicity X coordinate (cm); Y coordinate (cm)",200,-70.,70.,200,-70.,70.,kFALSE);
  DefineTracker("RecHitLayer_vs_Slot","; Slot; Layer",31,-15.5,15.5,TkDBc::Head->nlay(),0.5,TkDBc::Head->nlay()+0.5,kFALSE);

  // Layer track-occupancy
  DefineLayers("Occupancy","; X (cm); Y (cm)",200,-70,70,200,-70,70,kFALSE);
  DefineLayers("SeedAddress","; Channel",1024,0,1024,kFALSE); 
  DefineTracker("SeedAddress_vs_Ladder","; iCrate*24 + iTDR; Channel",192,-0.5,191.5,1024,0,1024,kFALSE); 

  // Track infos 
  DefineTracker("Theta_vs_Phi","; #phi (rad); #theta (rad)",500,-3.2,3.2,200,-0.1,3.2,kFALSE);
  DefineTracker("Y_vs_X","; X (cm); Y (cm)",500,-70.,70.,500,-70.,70.,kFALSE);
  DefineTracker("logRigidityPlus","; log(R/GV)",100,-2,4.,kFALSE);
  DefineTracker("logRigidityMinus","; log(-R/GV)",100,-2,4.,kFALSE);
  DefineTracker("logChiSq_vs_logRigidity","; log(|R|/GV); log(#chi^{2})",100,-2.,4.,100,-3,5.,kFALSE);
  DefineTracker("InvRigidity","; 1/R",500,-0.5,0.5,kFALSE);
  DefineTracker("ErrInvRigidity","; log(|R|/GV); #sigma_{1/R}*|R|",100,-2.,4.,200,-0.,2.,kFALSE);
  DefineTracker("nClustersOnTrack_vs_Ladder","; iCrate*24 + iTDR; n. of Clusters On Track",192,-0.5,191.5,11,-0.5,10.5,kTRUE);
  DefineTracker("nClustersOnTrack","; n. of Clusters On Track",11,-0.5,10.5,kTRUE);
  DefineTracker("nRecHitsOnTrack","; n. of Rec. Hits On Track",11,-0.5,10.5,kFALSE);

  // Full Output: ladder-by-ladder 
  if (fFlag>0) {
    DefineLadders("Size","; fragment size (byte)",200,0.,400.,kTRUE); 
    DefineLadders("Size","; fragment size (byte)",200,0.,400.,kFALSE);
    DefineLadders("SeedAddress","; Channel",1024,0,1024,kFALSE); 
    DefineLadders("Signal","; Amplitude (ADC)",100,0.,250.,kTRUE);
    DefineLadders("SignalToNoise","; Seed S/N",100,0.,100.,kTRUE);
    DefineLadders("NElement","; n. of Strips",40,0.,40.,kTRUE);
    DefineLadders("SizeLowDT","; fragment size (byte)",200,0.,400.,kTRUE);
    DefineLadders("SizeLowDT","; fragment size (byte)",200,0.,400.,kFALSE); 
    DefineLadders("SeedAddressLowDT","; Channel",1024,0,1024,kFALSE); 
    DefineLadders("SignalLowDT","; Amplitude for #Deltat<200 us (ADC)",100,0.,500.,kTRUE);
    DefineLadders("NElementLowDT","; n. of Strips for #Deltat<200 us",128,0.,128.,kTRUE);
    DefineLadders("SignalToNoiseLowDT","; Seed S/N",100,0.,100.,kTRUE);
  }

  // Truncated mean 
  DefineTracker("sqrtTM_vs_Beta","; #beta; Truncated Mean (#sqrt{ADC})",300,-1.2,1.2,300,0.,70.,kFALSE);
  DefineTracker("sqrtTM_vs_logRigidity","; log_{10}(|R|/GV); X Truncated Mean (#sqrt(ADC))",100,-2,4,300,0,70,kFALSE);
  DefineTracker("sqrtTM_vs_invRigidity","; 1/Rigidity (1/GV); X Truncated Mean (#sqrt{ADC})",200,-20,20,300,0,70,kFALSE);

  Sumw2();

  // Time ntuple
  if (fFlag>1) {
    TDirectory* saveddir = gDirectory;
    fDir->cd();
    fTimeNtuple = new TTree("timentuple","Time Dependent Quantities");
    fTimeNtuple->Branch("timebranch",&time_event,
      "Time/i:FineTime/F:dT/F:LiveTime/F:Rigidity/F:logChiSq/F:TruncMean/F:Beta/F:Size/s:nRaw/s:nClu/s:nHit/s:nTrk/s");
    saveddir->cd();
  }
}


void TrOnlineMonitor::Fill(AMSEventR* event){

  if (event==0) return;

  char histoname[100];

  if (event==0) { printf("TrOnlineMonitor::Fill-W void event, skipping!"); return; }
  if (event->NLevel1()!=1) { printf("TrOnlineMonitor::Fill-W a good Level1 is not available, skipping!"); return; }
  Level1R* lvl1 = event->pLevel1(0);

  // global statistics
  double dt   = lvl1->TrigTime[4]; // time diff in mksec
  int    nraw = event->NTrRawCluster();
  int    nclu = event->NTrCluster();
  int    nhit = event->NTrRecHit();
  int    ntrk = event->NTrTrack();

  // ntuple vars
  time_event.Time      = (UInt_t) event->UTime();
  time_event.FineTime  = (Float_t) 0.001*0.001*0.64*((float) lvl1->TrigTime[3]*pow(2.,32.) + (float) lvl1->TrigTime[2]);
  time_event.dT        = (Float_t) dt;
  time_event.LiveTime  = (Float_t) lvl1->LiveTime; 
  time_event.nRaw      = (UShort_t) nraw;
  time_event.nClu      = (UShort_t) nclu;
  time_event.nHit      = (UShort_t) nhit;
  time_event.nTrk      = (UShort_t) ntrk;
  time_event.TruncMean = (Float_t) 0.;
  time_event.Beta      = (Float_t) 2.; 
  time_event.Rigidity  = (Float_t) 0.;
  time_event.logChiSq  = (Float_t) 1000.;

  // global
  FillTracker("DT",dt);  
  FillTracker("nRawClusters",nraw);
  FillTracker("nRawClusters_vs_DT",dt,nraw);
  FillTracker("nClusters",nclu);
  FillTracker("nClusters_vs_DT",dt,nclu);
  FillTracker("nRecHits",nhit);
  FillTracker("nRecHits_vs_DT",dt,nhit);
  FillTracker("nTracks",ntrk);
  FillTracker("nTracks_vs_DT",dt,ntrk);

  // init
  int TrackerSize = 2; 
  int CrateSize[8] = {2,2,2,2,2,2,2,2}; 
  int TdrSize[8][24];
  for (int icrate=0; icrate<8; icrate++) 
    for (int itdr=0; itdr<24; itdr++) 
      TdrSize[icrate][itdr] = 2;
  int LadderSize[2][192]; 
  int LadderClusters[2][192];
  for (int iside=0; iside<2; iside++) { 
    for (int iladder=0; iladder<192; iladder++) {
      LadderSize[iside][iladder] = 0;
      LadderClusters[iside][iladder] = 0;
    }
  }

  // loop on raw
  for (int iraw=0; iraw<nraw; iraw++) {
    TrRawClusterR* cluster = event->pTrRawCluster(iraw);
    int   iside  = cluster->GetSide();
    int   tkid   = cluster->GetTkId();
    TkLadder* ladder = TkDBc::Head->FindTkId(tkid);
    int   icrate = ladder->GetCrate();
    int   itdr   = ladder->GetTdr();
    int   entry  = icrate*24 + itdr;
    int   clsize = cluster->GetNelem()+2;
    // int   ncl    = cluster->GetNelem();
    float seed   = cluster->GetSeedSN(3.);
    int   width  = cluster->GetLength(3.);
    float signal = cluster->GetTotSignal(3.,1.);
    int   addr   = cluster->GetSeedAddress(3.);
    LadderSize[iside][entry] += clsize;
    LadderClusters[iside][entry]++;
    TdrSize[icrate][itdr] += clsize;
    FillEntry(iside,tkid,"RawEntries");
    FillTracker("SeedAddress_vs_Ladder",entry,addr);
    FillLayer(kFALSE,cluster,"SeedAddress",addr);
    FillTracker(iside,"Signal",signal);
    FillTracker(iside,"sqrtSignal",sqrt(fabs(signal)));
    FillTracker(iside,"Signal_vs_Ladder",entry,signal);
    FillTracker(iside,"NElement_vs_Ladder",entry,width);
    if (dt<fDtMin) {
      FillTracker(iside,"SignalLowDT_vs_Ladder",entry,signal);
      FillTracker(iside,"NElementLowDT_vs_Ladder",entry,width);
    }
    if (fFlag>0) { 
      FillLadder(kFALSE,cluster,"SeedAddress",addr);
      FillLadder(kTRUE,cluster,"Signal",signal);
      FillLadder(kTRUE,cluster,"NElement",width);
      FillLadder(kTRUE,cluster,"SignalToNoise",seed);
      if (dt<fDtMin) {
        FillLadder(kFALSE,cluster,"SeedAddressLowDT",addr);
        FillLadder(kTRUE,cluster,"SignalLowDT",signal);
        FillLadder(kTRUE,cluster,"NElementLowDT",width);
        FillLadder(kTRUE,cluster,"SignalToNoiseLowDT",seed);
      }
    }
  }

  // loop on tdrs 
  for (int icrate=0; icrate<8; icrate++)
    for (int itdr=0; itdr<24; itdr++) 
      if (TdrSize[icrate][itdr]>2) CrateSize[icrate] += TdrSize[icrate][itdr];
  // loop on crates
  for (int icrate=0; icrate<8; icrate++) {
    TrackerSize += CrateSize[icrate];
    FillTracker("Size_vs_Crate",icrate,(float)2*CrateSize[icrate]);
    sprintf(histoname,"T%1dSize_vs_DT",icrate);
    FillTracker(histoname,dt,(float)2*CrateSize[icrate]);
    for (int itdr=0; itdr<24; itdr++) {
      int hwid = icrate*100 + itdr;
      int tkid = TkDBc::Head->FindHwId(hwid)->GetTkId();
      FillLadder(tkid,"Size",(float)2*TdrSize[icrate][itdr]);
      if (dt<fDtMin) FillLadder(tkid,"SizeLowDT",(float)2*TdrSize[icrate][itdr]);
    }
  }
  FillTracker("Size",(float)2*TrackerSize);
  FillTracker("Size_vs_DT",dt,(float)2*TrackerSize);

  // two sides separated histograms
  for (int iside=0; iside<2; iside++) {
    for (int icrate=0; icrate<8; icrate++) {
      for (int itdr=0; itdr<24; itdr++) {
        int hwid = icrate*100 + itdr;
        TkLadder* ladder = TkDBc::Head->FindHwId(hwid);
        if ( (ladder==0)||(ladder->GetHwId()<0) ) continue;
        int entry = icrate*24 + itdr;
        int tkid  = ladder->GetTkId();
        FillTracker(iside,"Size_vs_Ladder",entry,(float)2*LadderSize[iside][entry]);
        FillTracker(iside,"nRawClusters_vs_Ladder",entry,LadderClusters[iside][entry]);
        if (dt<fDtMin) FillTracker(iside,"SizeLowDT_vs_Ladder",entry,(float)2*LadderSize[iside][entry]);
        if (fFlag>0) {
          FillLadder(iside,tkid,"Size",(float)2*LadderSize[iside][entry]);
          if (dt<fDtMin) FillLadder(iside,tkid,"SizeLowDT",(float)2*LadderSize[iside][entry]);   
        }
      }
    }
  }
  
  // beam histograms
  for (unsigned int iclus=0; iclus<event->NTrCluster(); iclus++) {
    TrClusterR* cluster = event->pTrCluster(iclus);
    int side = cluster->GetSide();
    if      (side==0) FillLayer(kFALSE,cluster,"ClusterX",cluster->GetGCoord(0)); // max-multiplicity
    else if (side==1) FillLayer(kFALSE,cluster,"ClusterY",cluster->GetGCoord(0));
  }
  for (unsigned int ihit=0; ihit<event->NTrRecHit(); ihit++) {
    TrRecHitR* hit = event->pTrRecHit(ihit);
    if ( (hit->GetXCluster()!=0)&&(hit->GetYCluster()!=0) ) {
      AMSPoint point = hit->GetCoord(0); // max-multiplicity
      FillLayer(0,hit->GetXCluster(),"RecHitY_vs_X",point.x(),point.y());
      int layer = hit->GetLayer();
      int slot = hit->GetSlot();
      if (hit->GetTkId()<0) slot = -slot; 
      FillTracker("RecHitLayer_vs_Slot",slot,layer);
    } 
  }


  // loop on tracks
  for (unsigned int itrack=0; itrack<event->NTrTrack(); itrack++) {

    TrTrackR*  track    = event->pTrTrack(itrack);
    TrTrackPar par      = track->GetPar();
    float      rigidity = par.Rigidity;
    float      errinv   = par.ErrRinv; 
    float      chisq    = (par.ChisqX+par.ChisqY)/(par.NdofX+par.NdofY);

    if ( (errinv<1e6)&&(fabs(rigidity)>1e-6)&&(chisq>1e-6) ) { 

      AMSPoint global(par.P0);
      AMSDir   direction(par.Dir);

      // rigidity plots
      if (rigidity<0) FillTracker("logRigidityMinus",log10(-rigidity)); 
      else            FillTracker("logRigidityPlus",log10(rigidity));
      FillTracker("logChiSq_vs_logRigidity",log10(fabs(rigidity)),log10(chisq));
      FillTracker("InvRigidity",1./rigidity);
      FillTracker("ErrInvRigidity",log10(fabs(rigidity)),errinv*fabs(rigidity));
 
      // position and direction at Z=0
      FillTracker("Y_vs_X",global.x(),global.y());
      float phi = direction.getphi(); 
      float theta = direction.gettheta();
      FillTracker("Theta_vs_Phi",phi,theta);

      // points on track 
      int nclus[2] = {0,0};
      for (int iside=0; iside<2; iside++)
        for (int iladder=0; iladder<192; iladder++)
          LadderClusters[iside][iladder] = 0;

      int nhit = track->GetNhits();
      FillTracker("nRecHitsOnTrack",nhit);

      // loop on hits
      for (int ihit=0; ihit<track->GetNhits(); ihit++) {
        TrRecHitR* hit = track->GetHit(ihit);
        if (hit==0) continue; 
        TrClusterR* clx = hit->GetXCluster();
        TrClusterR* cly = hit->GetYCluster();
        int tkid = hit->GetTkId();
        TkLadder* ladder = TkDBc::Head->FindTkId(tkid);
        int ientry = ladder->GetCrate()*24 + ladder->GetTdr();
        float signalx = 0; // default
        float signaly = 0; // default
        if (clx!=0) { 
          nclus[0]++;
          LadderClusters[0][ientry]++;
          FillEntry(0,tkid,"TrackEntries");
          signalx = hit->GetXCluster()->GetTotSignal(); 
          if (rigidity>6.8) FillTracker(0,"SignalOnTrack",signalx);
          if (rigidity>6.8) FillTracker(0,"sqrtSignalOnTrack",sqrt(fabs(signalx)));
        }
        if (cly!=0) {
          nclus[1]++;
          LadderClusters[1][ientry]++;
          FillEntry(1,tkid,"TrackEntries");
          signaly = hit->GetYCluster()->GetTotSignal(); 
          if (rigidity>6.8) FillTracker(1,"SignalOnTrack",signaly);
          if (rigidity>6.8) FillTracker(1,"sqrtSignalOnTrack",sqrt(fabs(signaly)));
        }
        if ( (clx!=0)&&(cly!=0) ) {
          if (rigidity>6.8) FillTracker("SignalOnTrackY_vs_X",signalx,signaly);
          if (rigidity>6.8) FillTracker("sqrtSignalOnTrackY_vs_X",sqrt(fabs(signalx)),sqrt(fabs(signaly)));
        }
        FillLayer(hit->GetLayer(),"Occupancy",hit->GetCoord().x(),hit->GetCoord().y());
      } 

      // number of cluster on track
      for (int iside=0; iside<2; iside++) {
        FillTracker(iside,"nClustersOnTrack",nclus[iside]);
        for (int ientry=0; ientry<192; ientry++) {
          FillTracker(iside,"nClustersOnTrack_vs_Ladder",ientry,(float)LadderClusters[iside][ientry]);        
        }
      }

      // truncated mean
      float truncmean = TrCharge::GetMean(TrCharge::kTruncMean|TrCharge::kInner,track,TrCharge::kX).Mean;
      FillTracker("sqrtTM_vs_logRigidity",log10(rigidity),sqrt(fabs(truncmean)));
      FillTracker("sqrtTM_vs_invRigidity",1./rigidity,sqrt(fabs(truncmean)));
    } 
  }  


  // beta + track   
  if (event->nParticle()!=0) {
    ParticleR* particle = event->pParticle(0);
    if ( (particle->iBeta()!=-1)&&(particle->iTrTrack()!=-1) ) {
      float      beta     = event->pBeta(particle->iBeta())->Beta;
      TrTrackR*  track    = event->pTrTrack(particle->iTrTrack());
      TrTrackPar par      = track->GetPar();
      float      rigidity = par.Rigidity;
      float      errinv   = par.ErrRinv; 
      float      chisq    = (par.ChisqX+par.ChisqY)/(par.NdofX+par.NdofY);
      if ( (errinv<1e6)&&(fabs(rigidity)>1e-6)&&(chisq>1e-6) ) { 
        // ntuple storing
        time_event.Rigidity = (Float_t) rigidity;
        time_event.logChiSq = (Float_t) log10(chisq);
        time_event.Beta     = (Float_t) beta; 
        // loop on hits
        for (int ihit=0; ihit<track->GetNhits(); ihit++) {
          TrRecHitR* hit = track->GetHit(ihit);
          if (hit==0) continue; 
          TrClusterR* clx = hit->GetXCluster();
          TrClusterR* cly = hit->GetYCluster();
          if (clx!=0) FillTracker(0,"sqrtSignalOnTrack_vs_Beta",beta,sqrt(fabs(hit->GetXCluster()->GetTotSignal())));
          if (cly!=0) FillTracker(1,"sqrtSignalOnTrack_vs_Beta",beta,sqrt(fabs(hit->GetYCluster()->GetTotSignal())));
        }
        // truncated mean
        float truncmean = TrCharge::GetMean(TrCharge::kTruncMean|TrCharge::kInner,track,TrCharge::kX).Mean;
        FillTracker("sqrtTM_vs_Beta",beta,sqrt(fabs(truncmean)));
        time_event.TruncMean = (Float_t) truncmean;
      }
    }
  }

  // time ntuple storing
  time_event.Size = (UShort_t) 2*TrackerSize;
  if (fFlag>1) fTimeNtuple->Fill();
  
}


/////////////////////////////////////////////////
// TrHistoMan 
/////////////////////////////////////////////////


void TrHistoManHeader::Clear(Option_t* oo) {
  TObject::Clear(oo);
  fRunList.clear();  
  fFileList.SetOwner(kTRUE);
  fFileList.Clear();
}


TrHistoManHeader::TrHistoManHeader(const TrHistoManHeader &orig) : TNamed(orig) {
  fRunList = orig.fRunList;
  fFileList = orig.fFileList; // Clone ...
}


Long64_t TrHistoManHeader::Merge(TCollection* list) {
  if (!list) return 0;
  TList inlist;
  inlist.AddAll(list);
  TIter next(&inlist);
  while (TObject *o = next()) {
    TrHistoManHeader* header = (TrHistoManHeader*) o;
    // all files names
    for (int ifile=0; ifile<header->GetNFileNames(); ifile++) AddFileName(header->GetFileName(ifile)); 
    // only not equal runs  
    for (int irun=0;  irun<header->GetNRunNumbers(); irun++) {
      int runnumber = header->GetRunNumber(irun);
      bool alreadyin = false; 
      for (int jrun=0; jrun<GetNRunNumbers(); jrun++) {  
        int runcheck = GetRunNumber(jrun); 
        if (runcheck==runnumber) alreadyin = true;
      }
      if (!alreadyin) AddRunNumber(runnumber);
    }
  }
  return 1;
}

 
/////////////////////////////////////////////////
// TrHistoMan 
/////////////////////////////////////////////////


TrHistoMan::TrHistoMan(TFile* file, const char* name, const char* title) {
  // init
  TDirectory* saveddir = gDirectory;
  fHashTable.clear();
  fFile = file;
  if ( (fFile!=0)&&(fFile->IsOpen()!=0) ) { // fFile is the owner
    fFile->cd(); 
    fOwnerItself = kFALSE;
  }
  else { // owner is itself
    printf("TrHistoMan::Ctor-W invalid/absent output file specified, writing disabled\n");
    fOwnerItself = kTRUE;
  }     
  fDir = gDirectory->mkdir(name,title);
  fDir->cd();
  fHeader = new TrHistoManHeader(Form("%sHeader",name),Form("%s Header",title));
  fDir->Append(fHeader);

  // Directory tree:
  // |
  // +-- layer 1 -+- ladder +103
  // |            +- ladder +104
  // |            +- ...
  // ...

  // layers
  char dirname[80];
  char dirtitle[80];
  for (int ll=0; ll<TkDBc::Head->nlay(); ll++) {
    int layer = ll+1;
    sprintf(dirname,"layer%1d",layer);
    sprintf(dirtitle,"Layer %d",layer);
    fDirLayers[ll] = fDir->mkdir(dirname,dirtitle);
  }
  // slot
  char subdirname[80];
  char subdirtitle[80];
  char sidename[2] = {'M','P'};
  for (int ii=0; ii<TkDBc::Head->GetEntries(); ii++) {
    TkLadder* ladder = TkDBc::Head->GetEntry(ii);
    if (ladder->GetHwId()<0) continue;
    int ll = ladder->GetLayer() - 1;
    sprintf(subdirname,"%+03d",ladder->GetTkId());
    sprintf(subdirtitle,"Slot %d, Side %c",ladder->GetSlot(),sidename[ladder->GetSide()]);
    fDirLayers[ll]->mkdir(subdirname,subdirtitle);
  }
  saveddir->cd();
}


TrHistoMan::TrHistoMan(const TrHistoMan &orig) : TObject(orig) {
  fHeader = orig.fHeader;
  fFile = orig.fFile;
  fDir = orig.fDir;
  if (orig.fHashTable.empty()) {
    fHashTable.clear();
    fHashTable.insert(orig.fHashTable.begin(),orig.fHashTable.end());
  }
  fOwnerItself = orig.fOwnerItself;
}


TrHistoMan::~TrHistoMan() {
  if (fOwnerItself) { // owner is itself, destroy members
    if (fDir!=0) {
      fDir->cd();
      fDir->DeleteAll();
    }
    if (!fHashTable.empty()) fHashTable.clear();
    if (!fHeader) fHeader->Clear();
  }
  // fFile is the owner, no deletion
  map<uint32_t,TNamed*>::iterator it;
  for (it=fHashTable.begin(); it!=fHashTable.end(); it++) (*it).second = 0;
  fHashTable.clear();
  fDir = 0;
  for (int ll=0; ll<TkDBc::Head->nlay(); ll++) fDirLayers[ll] = 0;
  fHeader = 0;
  fFile = 0;
  
}


int TrHistoMan::Write(const char* name , Int_t option, Int_t bufsize) const {
  if ( (fFile==0)||(fFile->IsOpen()==0) ) {
    printf("TrHistoMan::Write-W invalid/absent output file, writing disabled\n");
    return -1;
  }
  fDir->cd();
  TList* list = fDir->GetList();
  TIter next(list);
  TObject *obj;
  int siz=0;
  while ( (obj=next()) ) {
    siz+=obj->Write(); 
  }
  return siz;
}


int TrHistoMan::Write(const char* name , Int_t option, Int_t bufsize)  {
 return ((const TrHistoMan*)this)->Write(name, option, bufsize);
}


void TrHistoMan::Sumw2(){
  // Sets all histograms to have a correct statistic error calculation
  map<uint32_t,TNamed*>::iterator it;
  for (it=fHashTable.begin(); it!=fHashTable.end(); it++) {
    TNamed* histo = it->second;
    if      (strcmp(histo->ClassName(),"TH1D")!=0) ((TH1D*) histo)->Sumw2();
    else if (strcmp(histo->ClassName(),"TH2D")!=0) ((TH2D*) histo)->Sumw2();
    else if (strcmp(histo->ClassName(),"TH3D")!=0) ((TH3D*) histo)->Sumw2();
  }
}


void TrHistoMan::Add(TNamed* obj) {
  uint32_t ihash = CalcHash(obj->GetName());
  // repetition check
  if (fHashTable.find(ihash)!=fHashTable.end()) {
    printf("TrHistoMan::Add-W the object named %s has the same hash %d of the previously stored object %s.\n",
           obj->GetName(),ihash,fHashTable.find(ihash)->second->GetName());
    printf("TrHistoMan::Add-W I WILL SKIP THIS OBJECT, THE BEST SOLUTION WILL BE TO CHANGE THE OBJECT NAME.\n");
    return; 
  }
  fHashTable.insert(pair<uint32_t,TNamed*>(ihash,obj));
}


TNamed* TrHistoMan::Get(const char* name){
  uint32_t ihash = CalcHash(name);
  // existence check
  if (fHashTable.find(ihash)==fHashTable.end()) {
    printf("TrHistoMan::Get-W object named %s not found. Returning 0.\n",name);
    return 0; 
  }
  return fHashTable.find(ihash)->second;
}


uint32_t TrHistoMan::CalcHash(const char *str) {
  /* 
    // ROOT hashing, some problem has arisen with this!
    TString istring(str);
    int ihash = istring.Hash();
  */
  // The FNV-1 hashing algorithm
  // http://www.isthe.com/chongo/src/fnv/hash_32.c
  uint32_t ihash = ((uint32_t)0x811c9dc5);
  unsigned char *s = (unsigned char *)str; // unsigned string
  //  FNV-1 hash each octet in the buffer
  while (*s) {
    ihash *= ((uint32_t)0x01000193);
    // xor the bottom with the current octet
    ihash ^= (uint32_t)*s++;
  }
  return ihash;
}


void TrHistoMan::DefineTracker(const char* name, const char*title, Int_t binx, Double_t lowx, Double_t upx, Bool_t bothsides) {
  /* Appends 1 or 2 TH1F histos in the main directory */
  TDirectory* saveddir = gDirectory;
  char  histoname[80];
  TH1D* histo;
  fDir->cd();
  if (!bothsides) {
    sprintf(histoname,"%s_all",name);
    histo = new TH1D(histoname,title, binx, lowx, upx);
    Add(histo);
  }
  else {
    sprintf(histoname,"%s_all_P",name);
    histo = new TH1D(histoname, title, binx, lowx, upx);
    Add(histo);
    sprintf(histoname,"%s_all_N",name);
    histo = new TH1D(histoname, title, binx, lowx, upx);
    Add(histo);
  }
  saveddir->cd();
  return;
}


void TrHistoMan::DefineTracker(const char* name, const char* title, 
		Int_t binx, Double_t lowx, Double_t upx, 
		Int_t biny, Double_t lowy, Double_t upy, Bool_t bothsides) {
  /* Appends 1 or 2 TH1F histos in the main directory */
  TDirectory* saveddir = gDirectory;
  char  histoname[80];
  TH2D* histo;
  fDir->cd();
  if (!bothsides) {
    sprintf(histoname,"%s_all",name);
    histo = new TH2D(histoname, title, binx, lowx, upx, biny, lowy, upy);
    Add(histo);
  }
  else {
    sprintf(histoname,"%s_all_P",name);
    histo = new TH2D(histoname, title, binx, lowx, upx, biny, lowy, upy);
    Add(histo);
    sprintf(histoname,"%s_all_N",name);
    histo = new TH2D(histoname, title, binx, lowx, upx, biny, lowy, upy);
    Add(histo);
  }
  saveddir->cd();
  return;
}


void TrHistoMan::DefineTracker(const char* name, const char* title, 
		Int_t binx, Double_t lowx, Double_t upx,
		Int_t biny, Double_t lowy, Double_t upy, 
		Int_t binz, Double_t lowz, Double_t upz, Bool_t bothsides) {
  /* Appends 1 or 2 TH1F histos in the main directory */
  TDirectory* saveddir = gDirectory;
  char  histoname[80];
  TH3D* histo;
  fDir->cd();
  if (!bothsides) {
    sprintf(histoname,"%s_all",name);
    histo = new TH3D(histoname, title, binx, lowx, upx, biny, lowy, upy, binz, lowz, upz);
    Add(histo);
  }
  else {
    sprintf(histoname,"%s_all_P",name);
    histo = new TH3D(histoname, title, binx, lowx, upx, biny, lowy, upy, binz, lowz, upz);
    Add(histo);
    sprintf(histoname,"%s_all_N",name);
    histo = new TH3D(histoname, title, binx, lowx, upx, biny, lowy, upy, binz, lowz, upz);
    Add(histo);
  }
  saveddir->cd();
  return;
}


void TrHistoMan::DefineLayers(const char* name, const char* title, 
		Int_t binx, Double_t lowx, Double_t upx, Bool_t bothsides) {
  /* Appends nlay or nlay*2 TH1F histos in the layer directories */
  TDirectory* saveddir = gDirectory;
  char  histoname[80];
  TH1D* histo;
  char  dirname[80];
  for (int ll=0; ll<TkDBc::Head->nlay(); ll++) {
    int layer = ll+1;
    sprintf(dirname,"layer%1d",layer);
    fDir->cd(dirname);
    if (!bothsides) {
      sprintf(histoname,"%s_layer%1d",name,layer);
      histo = new TH1D(histoname,title, binx, lowx, upx);
      Add(histo);
    }
    else {
      sprintf(histoname,"%s_layer%1d_P",name,layer);
      histo = new TH1D(histoname, title, binx, lowx, upx);
      Add(histo);
      sprintf(histoname,"%s_layer%1d_N",name,layer);
      histo = new TH1D(histoname, title, binx, lowx, upx);
      Add(histo);
    }
  }
  saveddir->cd();
  return;
}


void TrHistoMan::DefineLayers(const char* name, const char* title, 
		Int_t binx, Double_t lowx, Double_t upx,
		Int_t biny, Double_t lowy, Double_t upy, Bool_t bothsides){
  /* Appends nlay or nlay*2 TH2F histos in the layer directories */
  TDirectory* saveddir = gDirectory;
  char  histoname[80];
  TH2D* histo;
  char  dirname[80];
  for (int ll=0; ll<TkDBc::Head->nlay(); ll++) {
    int layer = ll+1;
    sprintf(dirname,"layer%1d",layer);
    fDir->cd(dirname);
    if(!bothsides){
      sprintf(histoname,"%s_layer%1d",name,layer);
      histo = new TH2D (histoname, title, binx, lowx, upx, biny, lowy, upy);
      Add(histo);
    }
    else {
      sprintf(histoname,"%s_layer%1d_P",name,layer);
      histo = new TH2D(histoname, title, binx, lowx, upx, biny, lowy, upy);
      Add(histo);
      sprintf(histoname,"%s_layer%1d_N",name,layer);
      histo = new TH2D(histoname, title, binx, lowx, upx, biny, lowy, upy);
      Add(histo);
    }
  }
  saveddir->cd();
  return;
}


void TrHistoMan::DefineLayers(const char* name, const char* title, 
		Int_t binx, Double_t lowx, Double_t upx,
		Int_t biny, Double_t lowy, Double_t upy, 
		Int_t binz, Double_t lowz, Double_t upz, Bool_t bothsides){
  /* Appends nlay or nlay*2 TH2F histos in the layer directories */
  TDirectory* saveddir = gDirectory;
  char  histoname[80];
  TH3D* histo;
  char  dirname[80];
  for (int ll=0; ll<TkDBc::Head->nlay(); ll++) {
    int layer = ll+1;
    sprintf(dirname,"layer%1d",layer);
    fDir->cd(dirname);
    if(!bothsides){
      sprintf(histoname,"%s_layer%1d",name,layer);
      histo = new TH3D (histoname, title, binx, lowx, upx, biny, lowy, upy, binz, lowz, upz);
      Add(histo);
    }
    else {
      sprintf(histoname,"%s_layer%1d_P",name,layer);
      histo = new TH3D(histoname, title, binx, lowx, upx, biny, lowy, upy, binz, lowz, upz);
      Add(histo);
      sprintf(histoname,"%s_layer%1d_N",name,layer);
      histo = new TH3D(histoname, title, binx, lowx, upx, biny, lowy, upy, binz, lowz, upz);
      Add(histo);
    }
  }
  saveddir->cd();
  return;
}


void TrHistoMan::DefineLadders(const char* name, const char* title, 
		Int_t binx, Double_t lowx, Double_t upx, Bool_t bothsides) {
  /* Appends 192 or 384 TH1F histos in the ladders subdirectories */
  TDirectory* saveddir = gDirectory;
  char  histoname[80];
  TH1D* histo;
  char  subdirname[80];
  for (int ii=0; ii<TkDBc::Head->GetEntries(); ii++) {
    TkLadder* ladder = TkDBc::Head->GetEntry(ii);
    if (ladder->GetHwId()<0) continue;
    int TkId = ladder->GetTkId(); 
    int ll   = ladder->GetLayer() - 1; 
    sprintf(subdirname,"%+03d",TkId);
    fDirLayers[ll]->cd(subdirname);
    if(!bothsides){
      sprintf(histoname,"%s_%+03d",name,TkId);
      histo = new TH1D(histoname, title, binx, lowx, upx);
      Add(histo);
    }
    else {
      sprintf(histoname,"%s_%+03d_P",name,TkId);
      histo = new TH1D(histoname, title, binx, lowx, upx);
      Add(histo);
      sprintf(histoname,"%s_%+03d_N",name,TkId);
      histo = new TH1D(histoname, title, binx, lowx, upx);
      Add(histo);
    }
  }
  saveddir->cd();
  return;
}


void TrHistoMan::DefineLadders(const char* name, const char* title, 
		Int_t binx, Double_t lowx, Double_t upx,
 		Int_t biny, Double_t lowy, Double_t upy, Bool_t bothsides) {
  /* Appends 192 or 384 TH1F histos in the ladders subdirectories */
  TDirectory* saveddir = gDirectory;
  char  histoname[80];
  TH2D* histo;
  char  subdirname[80];
  for (int ii=0; ii<TkDBc::Head->GetEntries(); ii++) {
    TkLadder* ladder = TkDBc::Head->GetEntry(ii);
    if (ladder->GetHwId()<0) continue;
    int TkId = ladder->GetTkId(); 
    int ll   = ladder->GetLayer() - 1; 
    sprintf(subdirname,"%+03d",TkId);
    fDirLayers[ll]->cd(subdirname);
    if(!bothsides){
      sprintf(histoname,"%s_%+03d",name,TkId);
      histo = new TH2D(histoname, title, binx, lowx, upx, biny, lowy, upy);
      Add(histo);
    }
    else {
      sprintf(histoname,"%s_%+03d_P",name,TkId);
      histo = new TH2D(histoname, title, binx, lowx, upx, biny, lowy, upy);
      Add(histo);
      sprintf(histoname,"%s_%+03d_N",name,TkId);
      histo = new TH2D(histoname, title, binx, lowx, upx, biny, lowy, upy);
      Add(histo);
    }
  }
  saveddir->cd();
  return;
}


void TrHistoMan::DefineLadders(const char* name, const char* title, 
		Int_t binx, Double_t lowx, Double_t upx,
		Int_t biny, Double_t lowy, Double_t upy, 
		Int_t binz, Double_t lowz, Double_t upz, Bool_t bothsides){
  /* Appends 192 or 384 TH1F histos in the ladders subdirectories */
  TDirectory* saveddir = gDirectory;
  char  histoname[80];
  TH3D* histo;
  char  subdirname[80];
  for (int ii=0; ii<TkDBc::Head->GetEntries(); ii++) {
    TkLadder* ladder = TkDBc::Head->GetEntry(ii);
    if (ladder->GetHwId()<0) continue;
    int TkId = ladder->GetTkId(); 
    int ll   = ladder->GetLayer() - 1; 
    sprintf(subdirname,"%+03d",TkId);
    fDirLayers[ll]->cd(subdirname);
    if(!bothsides){
      sprintf(histoname,"%s_%+03d",name,TkId);
      histo = new TH3D(histoname, title, binx, lowx, upx, biny, lowy, upy, binz, lowz, upz);
      Add(histo);
    }
    else {
      sprintf(histoname,"%s_%+03d_P",name,TkId);
      histo = new TH3D(histoname, title, binx, lowx, upx, biny, lowy, upy, binz, lowz, upz);
      Add(histo);
      sprintf(histoname,"%s_%+03d_N",name,TkId);
      histo = new TH3D(histoname, title, binx, lowx, upx, biny, lowy, upy, binz, lowz, upz);
      Add(histo);
    }
  }
  saveddir->cd();
  return;
}


void TrHistoMan::DefineEntries(const char* name, Bool_t bothsides) { 
  /* defines an entry histogram */
  TDirectory* saveddir = gDirectory;
  fDir->cd();
  char  histoname[80];
  TH2D* histo; 
  if(!bothsides){
    sprintf(histoname,"%s_entries",name);
    histo = new TH2D(histoname,"; slot; layer",31,-15.5,15.5,TkDBc::Head->nlay(),0.5,TkDBc::Head->nlay()+0.5);
    Add(histo);
  }
  else {
    sprintf(histoname,"%s_entries_P",name);
    histo = new TH2D(histoname,"; slot; layer",31,-15.5,15.5,TkDBc::Head->nlay(),0.5,TkDBc::Head->nlay()+0.5);
    Add(histo);
    sprintf(histoname,"%s_entries_N",name);
    histo = new TH2D(histoname,"; slot; layer",31,-15.5,15.5,TkDBc::Head->nlay(),0.5,TkDBc::Head->nlay()+0.5);
    Add(histo);
  }
  saveddir->cd();
  return;
}


void TrHistoMan::Define(const char* name, const char*title, 
		Int_t binx, Double_t lowx, Double_t upx, Bool_t bothsides) {
  TDirectory* saveddir = gDirectory;
  DefineEntries(name, bothsides);
  DefineTracker(name, title, binx, lowx, upx, bothsides);
  DefineLayers (name, title, binx, lowx, upx, bothsides);
  DefineLadders(name, title, binx, lowx, upx, bothsides);
  saveddir->cd();
  return;
}


void TrHistoMan::Define(const char* name, const char* title, 
		Int_t binx, Double_t lowx, Double_t upx,
		Int_t biny, Double_t lowy, Double_t upy, Bool_t bothsides){
  TDirectory* saveddir = gDirectory;
  DefineEntries(name, bothsides);
  DefineTracker(name, title, binx, lowx, upx, biny, lowy, upy, bothsides);
  DefineLayers (name, title, binx, lowx, upx, biny, lowy, upy, bothsides);
  DefineLadders(name, title, binx, lowx, upx, biny, lowy, upy, bothsides);
  saveddir->cd();
  return;
}
 

void TrHistoMan::Define(const char* name, const char* title, 
		Int_t binx, Double_t lowx, Double_t upx,
		Int_t biny, Double_t lowy, Double_t upy, 
		Int_t binz, Double_t lowz, Double_t upz, Bool_t bothsides) {
  TDirectory* saveddir = gDirectory;
  DefineEntries(name, bothsides);
  DefineTracker(name, title, binx, lowx, upx, biny, lowy, upy, binz, lowz, upz, bothsides);
  DefineLayers (name, title, binx, lowx, upx, biny, lowy, upy, binz, lowz, upz, bothsides);
  DefineLadders(name, title, binx, lowx, upx, biny, lowy, upy, binz, lowz, upz, bothsides);
  saveddir->cd();
  return;
}
 

void TrHistoMan::FillTracker(Bool_t bothsides, TrRawClusterR* cluster, const char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {
  char histoname[80];
  sprintf(histoname,"%s_all",name);
  if (bothsides) {
    if (cluster->GetSide()==1) {
      sprintf(histoname,"%s_all_P",name);
    }
    else if (cluster->GetSide()==0) {  
      sprintf(histoname,"%s_all_N",name);
    }
  }
  TNamed* tmp = Get(histoname);
  if (!tmp) {
    cout << "TrHistoMan::FillTracker-W " << histoname << " does not exist, ignoring for the moment ..." << endl;
  }
  else {
    if      (strcmp("TH1D",tmp->ClassName())==0) {     // is a TH1D
      TH1D* hh = (TH1D*) tmp;
      hh->Fill(X1,X2);
    }
    else if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
      TH2D* hh = (TH2D*) tmp;
      hh->Fill(X1,X2,X3);
    }
    else if (strcmp("TH3D",tmp->ClassName())==0) {     // is a TH3D
      TH3D* hh = (TH3D*) tmp;
      hh->Fill(X1,X2,X3,w);
    }
    else {
      cout <<"TrHistoMan:FillTracker-W " << histoname << " type " << tmp->ClassName() << " not supported" << endl;
    }
  }
}


void TrHistoMan::FillTracker(Bool_t bothsides, TrClusterR* cluster, const char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {
  char histoname[80];
  sprintf(histoname,"%s_all",name);
  if (bothsides) {
    if (cluster->GetSide()==1) {
      sprintf(histoname,"%s_all_P",name);
    }
    else if (cluster->GetSide()==0) {  
      sprintf(histoname,"%s_all_N",name);
    }
  }
  TNamed* tmp = Get(histoname);
  if (!tmp) {
    cout << "TrHistoMan::FillTracker-W " << histoname << " does not exist, ignoring for the moment ..." << endl;
  }
  else {
    if      (strcmp("TH1D",tmp->ClassName())==0) {     // is a TH1D
      TH1D* hh = (TH1D*) tmp;
      hh->Fill(X1,X2);
    }
    else if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
      TH2D* hh = (TH2D*) tmp;
      hh->Fill(X1,X2,X3);
    }
    else if (strcmp("TH3D",tmp->ClassName())==0) {     // is a TH3D
      TH3D* hh = (TH3D*) tmp;
      hh->Fill(X1,X2,X3,w);
    }
    else {
      cout <<"TrHistoMan:FillTracker-W " << histoname << " type " << tmp->ClassName() << " not supported" << endl;
    }
  }
}


void TrHistoMan::FillTracker(Int_t side, const char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {
  char histoname[80];
  if (side==1) sprintf(histoname,"%s_all_P",name);
  else         sprintf(histoname,"%s_all_N",name);
  TNamed* tmp = Get(histoname);
  if (!tmp) {
    cout << "TrHistoMan::FillTracker-W " << histoname << " does not exist, ignoring for the moment ..." << endl;
  }
  else {
    if      (strcmp("TH1D",tmp->ClassName())==0) {     // is a TH1D
      TH1D* hh = (TH1D*) tmp;
      hh->Fill(X1,X2);
    }
    else if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
      TH2D* hh = (TH2D*) tmp;
      hh->Fill(X1,X2,X3);
    }
    else if (strcmp("TH3D",tmp->ClassName())==0) {     // is a TH3D
      TH3D* hh = (TH3D*) tmp;
      hh->Fill(X1,X2,X3,w);
    }
    else {
      cout <<"TrHistoMan:FillTracker-W " << histoname << " type " << tmp->ClassName() << " not supported" << endl;
    }
  }
}


void TrHistoMan::FillTracker(const char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {
  char histoname[80];
  sprintf(histoname,"%s_all",name);
  TNamed* tmp = Get(histoname);
  if (!tmp) {
    cout << "TrHistoMan::FillTracker-W " << histoname << " does not exist, ignoring for the moment ..." << endl;
  }
  else {
    if      (strcmp("TH1D",tmp->ClassName())==0) {     // is a TH1D
      TH1D* hh = (TH1D*) tmp;
      hh->Fill(X1,X2);
    }
    else if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
      TH2D* hh = (TH2D*) tmp;
      hh->Fill(X1,X2,X3);
    }
    else if (strcmp("TH3D",tmp->ClassName())==0) {     // is a TH3D
      TH3D* hh = (TH3D*) tmp;
      hh->Fill(X1,X2,X3,w);
    }
    else {
      cout <<"TrHistoMan:FillTracker-W " << histoname << " type " << tmp->ClassName() << " not supported" << endl;
    }
  }
}


void TrHistoMan::FillLayer(Bool_t bothsides, TrRawClusterR* cluster, const char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {
  // fill the layer histograms (p/n)
  int TkId         = cluster->GetTkId(); 
  TkLadder* ladder = TkDBc::Head->FindTkId(TkId);
  int layer        = ladder->GetLayer(); 
  char histoname[80];
  sprintf(histoname,"%s_layer%1d",name,layer);
  if (bothsides) {
    if (cluster->GetSide()==1) {
      sprintf(histoname,"%s_layer%1d_P",name,layer);
    }
    else if (cluster->GetSide()==0) {  
      sprintf(histoname,"%s_layer%1d_N",name,layer);
    }
  }
  TNamed* tmp = Get(histoname);
  if (!tmp) {
    cout << "TrHistoMan::FillLayer-W " << histoname << " does not exist, ignoring for the moment ..." << endl;
  }
  else {
    if      (strcmp("TH1D",tmp->ClassName())==0) {     // is a TH1D
      TH1D* hh = (TH1D*) tmp;
      hh->Fill(X1,X2);
    }
    else if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
      TH2D* hh = (TH2D*) tmp;
      hh->Fill(X1,X2,X3);
    }
    else if (strcmp("TH3D",tmp->ClassName())==0) {     // is a TH3D
      TH3D* hh = (TH3D*) tmp;
      hh->Fill(X1,X2,X3,w);
    }
    else {
      cout <<"TrHistoMan:FillLayer-W " << histoname << " type " << tmp->ClassName() << " not supported" << endl;
    }
  }
}


void TrHistoMan::FillLayer(Bool_t bothsides, TrClusterR* cluster, const char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {
  // fill the layer histograms (p/n)
  int TkId         = cluster->GetTkId(); 
  TkLadder* ladder = TkDBc::Head->FindTkId(TkId);
  int layer        = ladder->GetLayer(); 
  char histoname[80];
  sprintf(histoname,"%s_layer%1d",name,layer);
  if (bothsides) {
    if (cluster->GetSide()==1) {
      sprintf(histoname,"%s_layer%1d_P",name,layer);
    }
    else if (cluster->GetSide()==0) {  
      sprintf(histoname,"%s_layer%1d_N",name,layer);
    }
  }
  TNamed* tmp = Get(histoname);
  if (!tmp) {
    cout << "TrHistoMan::FillLayer-W " << histoname << " does not exist, ignoring for the moment ..." << endl;
  }
  else {
    if      (strcmp("TH1D",tmp->ClassName())==0) {     // is a TH1D
      TH1D* hh = (TH1D*) tmp;
      hh->Fill(X1,X2);
    }
    else if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
      TH2D* hh = (TH2D*) tmp;
      hh->Fill(X1,X2,X3);
    }
    else if (strcmp("TH3D",tmp->ClassName())==0) {     // is a TH3D
      TH3D* hh = (TH3D*) tmp;
      hh->Fill(X1,X2,X3,w);
    }
    else {
      cout <<"TrHistoMan:FillLayer-W " << histoname << " type " << tmp->ClassName() << " not supported" << endl;
    }
  }
}


void TrHistoMan::FillLayer(Int_t side, Int_t layer, const char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {
  // fill the layer histograms (p/n)
  char histoname[80];
  if (side==1) sprintf(histoname,"%s_layer%1d_P",name,layer);
  else         sprintf(histoname,"%s_layer%1d_N",name,layer);
  TNamed* tmp = Get(histoname);
  if (!tmp) {
    cout << "TrHistoMan::FillLayer-W " << histoname << " does not exist, ignoring for the moment ..." << endl;
  }
  else {
    if      (strcmp("TH1D",tmp->ClassName())==0) {     // is a TH1D
      TH1D* hh = (TH1D*) tmp;
      hh->Fill(X1,X2);
    }
    else if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
      TH2D* hh = (TH2D*) tmp;
      hh->Fill(X1,X2,X3);
    }
    else if (strcmp("TH3D",tmp->ClassName())==0) {     // is a TH3D
      TH3D* hh = (TH3D*) tmp;
      hh->Fill(X1,X2,X3,w);
    }
    else {
      cout <<"TrHistoMan:FillLayer-W " << histoname << " type " << tmp->ClassName() << " not supported" << endl;
    }
  }
}


void TrHistoMan::FillLayer(Int_t layer, const char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {
  // fill the layer histograms (p/n)
  char histoname[80];
  sprintf(histoname,"%s_layer%1d",name,layer);
  TNamed* tmp = Get(histoname);
  if (!tmp) {
    cout << "TrHistoMan::FillLayer-W " << histoname << " does not exist, ignoring for the moment ..." << endl;
  }
  else {
    if      (strcmp("TH1D",tmp->ClassName())==0) {     // is a TH1D
      TH1D* hh = (TH1D*) tmp;
      hh->Fill(X1,X2);
    }
    else if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
      TH2D* hh = (TH2D*) tmp;
      hh->Fill(X1,X2,X3);
    }
    else if (strcmp("TH3D",tmp->ClassName())==0) {     // is a TH3D
      TH3D* hh = (TH3D*) tmp;
      hh->Fill(X1,X2,X3,w);
    }
    else {
      cout <<"TrHistoMan:FillLayer-W " << histoname << " type " << tmp->ClassName() << " not supported" << endl;
    }
  }
}


void TrHistoMan::FillLadder(Bool_t bothsides, TrRawClusterR* cluster, const char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {
  // fill the ladder histograms (p/n)
  int  TkId = cluster->GetTkId(); 
  char histoname[80];
  sprintf(histoname,"%s_%+03d",name,TkId);
  if (bothsides) {
    if (cluster->GetSide()==1) {
      sprintf(histoname,"%s_%+03d_P",name,TkId);
    }
    else if (cluster->GetSide()==0) {  
      sprintf(histoname,"%s_%+03d_N",name,TkId);
    }
  }
  TNamed* tmp = Get(histoname);
  if (!tmp) {
    cout << "TrHistoMan::FillLadder-W " << histoname << " does not exist, ignoring for the moment ..." << endl;
  }
  else {
    if      (strcmp("TH1D",tmp->ClassName())==0) {     // is a TH1D
      TH1D* hh = (TH1D*) tmp;
      hh->Fill(X1,X2);
    }
    else if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
      TH2D* hh = (TH2D*) tmp;
      hh->Fill(X1,X2,X3);
    }
    else if (strcmp("TH3D",tmp->ClassName())==0) {     // is a TH3D
      TH3D* hh = (TH3D*) tmp;
      hh->Fill(X1,X2,X3,w);
    }
    else {
      cout <<"TrHistoMan:FillLadder-W " << histoname << " type " << tmp->ClassName() << " not supported" << endl;
    }
  }
}


void TrHistoMan::FillLadder(Bool_t bothsides, TrClusterR* cluster, const char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {
  // fill the ladder histograms (p/n)
  int  TkId = cluster->GetTkId(); 
  char histoname[80];
  sprintf(histoname,"%s_%+03d",name,TkId);
  if (bothsides) {
    if (cluster->GetSide()==1) {
      sprintf(histoname,"%s_%+03d_P",name,TkId);
    }
    else  {  
      sprintf(histoname,"%s_%+03d_N",name,TkId);
    }
  }
  TNamed* tmp = Get(histoname);
  if (!tmp) {
    cout << "TrHistoMan::FillLadder-W " << histoname << " does not exist, ignoring for the moment ..." << endl;
  }
  else {
    if      (strcmp("TH1D",tmp->ClassName())==0) {     // is a TH1D
      TH1D* hh = (TH1D*) tmp;
      hh->Fill(X1,X2);
    }
    else if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
      TH2D* hh = (TH2D*) tmp;
      hh->Fill(X1,X2,X3);
    }
    else if (strcmp("TH3D",tmp->ClassName())==0) {     // is a TH3D
      TH3D* hh = (TH3D*) tmp;
      hh->Fill(X1,X2,X3,w);
    }
    else {
      cout <<"TrHistoMan:FillLadder-W " << histoname << " type " << tmp->ClassName() << " not supported" << endl;
    }
  }
}


void TrHistoMan::FillLadder(Int_t side, Int_t TkId, const char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {
  // fill the ladder histograms (p/n)
  char histoname[80];
  if (side==1) sprintf(histoname,"%s_%+03d_P",name,TkId);
  else         sprintf(histoname,"%s_%+03d_N",name,TkId);
  TNamed* tmp = Get(histoname);
  if (!tmp) {
    cout << "TrHistoMan::FillLadder-W " << histoname << " does not exist, ignoring for the moment ..." << endl;
  }
  else {
    if      (strcmp("TH1D",tmp->ClassName())==0) {     // is a TH1D
      TH1D* hh = (TH1D*) tmp;
      hh->Fill(X1,X2);
    }
    else if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
      TH2D* hh = (TH2D*) tmp;
      hh->Fill(X1,X2,X3);
    }
     else if (strcmp("TH3D",tmp->ClassName())==0) {     // is a TH3D
      TH3D* hh = (TH3D*) tmp;
      hh->Fill(X1,X2,X3,w);
    }
   else {
      cout <<"TrHistoMan:FillLadder-W " << histoname << " type " << tmp->ClassName() << " not supported" << endl;
    }
  }
}


void TrHistoMan::FillLadder(Int_t TkId, const char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {
  // fill the ladder histograms (p/n)
  char histoname[80];
  sprintf(histoname,"%s_%+03d",name,TkId);
  TNamed* tmp = Get(histoname);
  if (!tmp) {
    cout << "TrHistoMan::FillLadder-W " << histoname << " does not exist, ignoring for the moment ..." << endl;
  }
  else {
    if      (strcmp("TH1D",tmp->ClassName())==0) {     // is a TH1D
      TH1D* hh = (TH1D*) tmp;
      hh->Fill(X1,X2);
    }
    else if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
      TH2D* hh = (TH2D*) tmp;
      hh->Fill(X1,X2,X3);
    }
    else if (strcmp("TH3D",tmp->ClassName())==0) {     // is a TH3D
      TH3D* hh = (TH3D*) tmp;
      hh->Fill(X1,X2,X3,w);
    }
    else {
      cout <<"TrHistoMan:FillLadder-W " << histoname << " type " << tmp->ClassName() << " not supported" << endl;
    }
  }
}


void TrHistoMan::Fill(Bool_t bothsides, TrRawClusterR* cluster, const char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {
  FillEntry  (bothsides, cluster, name); 
  FillTracker(bothsides, cluster, name, X1, X2, X3, w); 
  FillLayer  (bothsides, cluster, name, X1, X2, X3, w);
  FillLadder (bothsides, cluster, name, X1, X2, X3, w);
}


void TrHistoMan::Fill(Bool_t bothsides, TrClusterR* cluster, const char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {
  FillEntry  (bothsides, cluster, name); 
  FillTracker(bothsides, cluster, name, X1, X2, X3, w); 
  FillLayer  (bothsides, cluster, name, X1, X2, X3, w);
  FillLadder (bothsides, cluster, name, X1, X2, X3, w);
}


void TrHistoMan::Fill(Int_t side, Int_t tkid, const char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {
  FillEntry  (side, tkid, name); 
  FillTracker(side, name, X1, X2, X3, w); 
  FillLayer  (side, abs(tkid/100), name, X1, X2, X3, w);
  FillLadder (side, tkid, name, X1, X2, X3, w);
}


void TrHistoMan::Fill(Int_t tkid, const char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {
  FillEntry  (tkid, name); 
  FillTracker(name, X1, X2, X3, w); 
  FillLayer  (abs(tkid/100), name, X1, X2, X3, w);
  FillLadder (tkid, name, X1, X2, X3, w);
}


void TrHistoMan::FillEntry(Bool_t bothsides, TrRawClusterR* cluster, const char *name){
  char histoname[80];
  int TkId         = cluster->GetTkId();
  TkLadder* ladder = TkDBc::Head->FindTkId(TkId);
  int layer        = ladder->GetLayer();
  int slot         = ladder->GetSlot();
  sprintf(histoname,"%s_entries",name);
  if (bothsides) {
    if (cluster->GetSide()==1) {
      sprintf(histoname,"%s_entries_P",name);
    }
    else if (cluster->GetSide()==0) {  
      sprintf(histoname,"%s_entries_N",name);
    }
  }
  TNamed* tmp = Get(histoname);
  if(!tmp){
    cout <<"TrHistoMan:FillEntry-W " << histoname << " not found" << endl;
    return;
  }
    
  if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
    TH2D* hh = (TH2D*) tmp;
    hh->Fill((float)slot,(float)layer,1.);
  }
  else {
    cout <<"TrHistoMan:FillEntry-W " << histoname << " type " << tmp->ClassName() << " not supported" << endl;
  }
}


void TrHistoMan::FillEntry(Bool_t bothsides, TrClusterR* cluster, const char *name){
  char histoname[80];
  int TkId         = cluster->GetTkId();
  TkLadder* ladder = TkDBc::Head->FindTkId(TkId);
  int layer        = ladder->GetLayer();
  int slot         = ladder->GetSlot();
  sprintf(histoname,"%s_entries",name);
  if (bothsides) {
    if (cluster->GetSide()==1) {
      sprintf(histoname,"%s_entries_P",name);
    }
    else if (cluster->GetSide()==0) {  
      sprintf(histoname,"%s_entries_N",name);
    }
  }
  TNamed* tmp = Get(histoname);
  if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
    TH2D* hh = (TH2D*) tmp;
    hh->Fill((float)slot,(float)layer,1.);
  }
  else {
    cout <<"TrHistoMan:FillEntry-W " << histoname << " type " << tmp->ClassName() << " not supported" << endl;
  }
}


void TrHistoMan::FillEntry(Int_t side, Int_t tkid, const char *name){
  char histoname[80];
  TkLadder* ladder = TkDBc::Head->FindTkId(tkid);
  int layer        = ladder->GetLayer();
  int slot         = ladder->GetSlot();
  if (side==1) sprintf(histoname,"%s_entries_P",name);
  if (side==0) sprintf(histoname,"%s_entries_N",name);
  TNamed* tmp = Get(histoname);
  if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
    TH2D* hh = (TH2D*) tmp;
    hh->Fill((float)slot,(float)layer,1.);
  }
  else {
    cout <<"TrHistoMan:FillEntry-W " << histoname << " type " << tmp->ClassName() << " not supported" << endl;
  }
}


void TrHistoMan::FillEntry(Int_t tkid, const char *name){
  char histoname[80];
  TkLadder* ladder = TkDBc::Head->FindTkId(tkid);
  int layer        = ladder->GetLayer();
  int slot         = ladder->GetSlot();
  sprintf(histoname,"%s_entries",name);
  TNamed* tmp = Get(histoname);
  if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
    TH2D* hh = (TH2D*) tmp;
    hh->Fill((float)slot,(float)layer,1.);
  }
  else {
    cout <<"TrHistoMan:FillEntry-W " << histoname << " type " << tmp->ClassName() << " not supported" << endl;
  }
}


void TrHistoMan::ResetHisto(TNamed* object) {
  if (!object->InheritsFrom("TH1")) return;
  if      (strcmp("TH1D",object->ClassName())==0) {     // is a TH1D
    TH1D* histogram = (TH1D*) object;
    histogram->Reset();
  }
  else if (strcmp("TH2D",object->ClassName())==0) {     // is a TH2D
    TH2D* histogram = (TH2D*) object;
    histogram->Reset();
  }
  else if (strcmp("TH3D",object->ClassName())==0) {     // is a TH3D
    TH3D* histogram = (TH3D*) object;
    histogram->Reset();
  }
}


void TrHistoMan::ResetTracker(const char* name) {
  char histoname[80];
  sprintf(histoname,"%s_all",name);
  TNamed* tmp = Get(histoname);
  ResetHisto(tmp);
}

