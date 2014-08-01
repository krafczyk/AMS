// $Id: TofRecon.C,v 1.5 2012/05/08 19:35:36 pzuccon Exp $

#include "TofRecon.h"

#include "point.h"
#include "root.h"
#include "TofTrack.h"
#include "TrCharge.h"
#include "HistoMan.h"

ClassImp(TofCandidate);
ClassImp(TofRecon);

// reconstruction parameters defaults
int   TofRecon::MinNHits = 3;
float TofRecon::MaxDistance = 20; // cm
float TofRecon::OffsetCharge1 = 3; // sqrt(MeV) offset
float TofRecon::OffsetCharge2 = 1; // sqrt(MeV) offset
float TofRecon::MaxRedChiSqT = 1000;
float TofRecon::MaxRedChiSqXY = 1000;
float TofRecon::MaxRelErrAnode = 1;
float TofRecon::MaxRelErrDynode = 1;

float TofRecon::MindYForGoodMatch = 10;
float TofRecon::MindQForGoodMatch = 1000;

// tracks list
vector<TofTrack*> TofRecon::TofTracksList;
vector< vector<TrTrackR*> > TofRecon::TrTracksList;

// ancillary
bool      TofRecon::EnabledHistograms = false;
HistoMan  TofRecon::TofReconHisto;
bool      TofRecon::EnabledNtuple = false;
TTree*    TofRecon::TofReconTree = 0;
TOF_TRACK TofRecon::Tof_Track;


void TofRecon::Print(int verbosity) {
  int n[5] = {0,0,0,0,0};
  for (int itrack = 0; itrack<TofRecon::NTofTrack(); itrack++) {
    TofTrack* track = TofRecon::pTofTrack(itrack);
    int nhits = track->GetNhits();   
    if (nhits<=1) nhits=1;
    if (nhits>=5) nhits=5;
    n[nhits-1]++;
  }
  printf("TofRecon::Print-V nTofTracks %2d    nHits<2 %2d    nHits==2 %2d   nHits==3 %2d   nHits=4 %2d   nHits>4 %2d\n",
         TofRecon::NTofTrack(),n[0],n[1],n[2],n[3],n[4]);
  if (verbosity>0) {
    for (int itrack = 0; itrack<TofRecon::NTofTrack(); itrack++) {
    printf("                  TofTrack %2d infos follow:\n",itrack);
      TofTrack* track = TofRecon::pTofTrack(itrack);
      track->Print(verbosity);
    }
  }
}


void TofRecon::DumpTofClustersContainer(AMSEventR* event) {
  printf("TofRecon::DumpTofClustersContainer-V n. clusters %2d\n",event->NTofCluster());
  for (int icluster=0; icluster<event->NTofCluster(); icluster++) {
    TofClusterR* cluster = (TofClusterR*) event->pTofCluster(icluster);
    if (cluster==0) continue;
    printf("    cluster %2d  status %7d  layer %1d   bar %2d  edep %7.2f  edepd %7.2f  t %7.2f  et %7.2f  coo (%7.2f,%7.2f,%7.2f) ecoo (%7.2f,%7.2f,%7.2f)\n",
      icluster,cluster->Status,cluster->Layer,cluster->Bar,cluster->Edep,cluster->Edepd,cluster->Time*1e9,cluster->ErrTime*1e9,
      cluster->Coo[0],cluster->Coo[1],cluster->Coo[2],cluster->ErrorCoo[0],cluster->ErrorCoo[1],cluster->ErrorCoo[2]);
  }
}


void TofRecon::DumpTofRawClustersContainer(AMSEventR* event) {
  printf("TofRecon::DumpTofRawClustersContainer-V n. raw clusters %2d\n",event->NTofRawCluster());
  for (int icluster=0; icluster<event->NTofRawCluster(); icluster++) {
    TofRawClusterR* cluster = (TofRawClusterR*) event->pTofRawCluster(icluster);
    if (cluster==0) continue;
    printf("    raw %2d  status %7d  layer %1d   bar %2d  edepa %8.2f  edepd %8.2f  t %8.2f  cool %8.2f\n",
      icluster,cluster->Status,cluster->Layer,cluster->Bar,cluster->edepa,cluster->edepd,cluster->time,cluster->cool);
    for (int iside=0; iside<2; iside++) {
      printf("      side %1d  adca %8.2f  adcd %8.2f  adcd_raw (%8.2f,%8.2f,%8.2f)  tdc %8.2f\n",
        iside,cluster->adca[iside],cluster->adcd[iside],cluster->adcdr[iside][0],cluster->adcdr[iside][1],cluster->adcdr[iside][2],cluster->sdtm[iside]);
    }
  }
}


void TofRecon::EnableHistograms() { 
  TofReconHisto.Enable();  
  EnabledHistograms = true; 
}


void TofRecon::DisableHistograms() { 
  TofReconHisto.Disable(); 
  EnabledHistograms = false; 
}


void TofRecon::Write(TFile* file) {
  // check
  if ( (file==0)||(file->IsOpen()==0) ) {
    printf("TofRecon::Write-W the file does not exist or is already closed.\n");
    return;
  }
  // init
  TDirectory* saveddir = gDirectory;
  // histo
  if (EnabledHistograms) {
    file->cd();
    printf("TofRecon::Write-V writing TOF reconstruction histograms on dir TofRecon in file %s\n",file->GetName());
    TofReconHisto.Save(file,"TofRecon");
  }
  // ntuple
  if (EnabledNtuple) {
    file->cd();
    printf("TofRecon::Write-V writing TOF reconstruction ntuple on dir TofRecon in file %s\n",file->GetName());
    if (!file->cd("TofRecon")) {
      TDirectory* dir = gDirectory->mkdir("TofRecon","TofRecon"); 
      dir->cd();
    }
    if (TofReconTree) TofReconTree->Write();
  }
  saveddir->cd();
}


bool TofRecon::GoodTofTrack(TofTrack* tof_track, Level1R* lvl1, char* add_name) { 
  // int minnumber of hits
  int   min_number_hits = 3;

  // histogram definitions
  char  name[200];    sprintf(name,"GoodTofTrackCounters_%s",add_name);
  char  title[200];   sprintf(title,"; Counters");
  int   nbin = 11;
  float xmin = -0.5;
  float xmax = 10.5;

  int   ndrop = 0;

  // number of function calling
  Histogram(name,title,nbin,xmin,xmax,0);

  // number of TofTracks with valid pointers
  if (!tof_track) return false;
  Histogram(name,title,nbin,xmin,xmax,1);

  // number of TofTracks with more than 2 hits
  if (!(tof_track->GetNhits()>=min_number_hits)) return false;
  Histogram(name,title,nbin,xmin,xmax,2);

  // number of TofTracks with good TofClusters and TofRawClusters
  if (GoodTofTrackClusters(tof_track,lvl1,add_name)) 
    Histogram(name,title,nbin,xmin,xmax,3);

  // drop bad guys (improves efficiency of the GoodTofTrackClusters cut)
  ndrop += DropBadTofTrackClusters(tof_track,lvl1,add_name);
  if (!(tof_track->GetNhits()>=min_number_hits)) return false;
  Histogram(name,title,nbin,xmin,xmax,4);

  // number of TofTracks with an associated TrTrack
  if (!tof_track->AnAssociatedTrTrackExist()) return false;
  Histogram(name,title,nbin,xmin,xmax,5);

  // tracker match 
  TrTrackR* trk_track = tof_track->GetAssociatedTrTrack();
  int       id        = tof_track->GetAssociatedTrTrackFitId();

  // track match with tof
  if (!GoodTofTrackTrTrackMatch(tof_track,trk_track,id,add_name)) return false;
  Histogram(name,title,nbin,xmin,xmax,6);

  // track inside tof cluster paddles
  if (IsInsidePaddles(tof_track,trk_track,id)) 
    Histogram(name,title,nbin,xmin,xmax,7);

  // drop outside clusters (improves efficiency of the IsInsidePaddles cut)
  // is possible to improve more this cut only dropping the TofRawCluster outside of the road
  ndrop += DropOutsideTofTrackClusters(tof_track,trk_track,id);
  if (!(tof_track->GetNhits()>=min_number_hits)) return false;
  Histogram(name,title,nbin,xmin,xmax,8);

  return true;
}


bool TofRecon::GoodTofRawCluster(TofRawClusterR* rawcluster, Level1R* lvl1, char* add_name) {
  // pointer check
  if (rawcluster==0) { 
    printf("TofRecon::GoodTofRawCluster-W an invalid tof raw cluster pointer.\n");
    return false;
  }
  // status check
  // 8: bad history, 9: 1-side meas., 10:bad from DB
  int status = rawcluster->Status;
  if ( (status&0x700)!=0 ) return false;
  /* 
   * If lvl1 pointer is available perform the global HT-LT compatibility test:
   * - Paddle trigger signal is an OR of the two sides HT. Instead signal and time 
   *   measurement are the combination of the signals coming from the two sides LT.
   *   If we have a signal we must have also the trigger.  
   * Atention: this test gives inefficiencies in case of single side HT-LT match. 
   * - We can have HT from OR, but no trigger from one side. Conversely 
   *   we could have signal from both sides or just one. 
   *   Requiring HT-LT match for single sides brings to beta measurement
   *   inefficiencies (3 layers instead of 4).  
   */
  int ilayer  = rawcluster->Layer - 1;
  int ipaddle = rawcluster->Bar - 1;
  if (lvl1!=0) {
    int pattern = lvl1->TofPatt1[ilayer];
    if (((pattern>>ipaddle)&0x1)==0) return false;
  }
  // time compatibility test
  float L_on_v = rawcluster->sdtm[0] + rawcluster->sdtm[1];
  int   index = ilayer*10 + ipaddle;
  // printf("ilayer: %2d  ipaddle: %2d  t1: %10.4f  t2:%10.4f  T: %10.4f  Coo: %10.4f  L/v: %10.4f\n",
  //        ilayer,ipaddle,rawcluster->sdtm[0],rawcluster->sdtm[1],rawcluster->time,rawcluster->cool,L_on_v); 
  Histogram(
    Form("L_on_v_vs_PaddleIndex_%s",add_name),
    "; Paddle Index; L/v (ns)",
    41,-0.5,40.5,100,0,1000,
    index,L_on_v
  );
  if ( (L_on_v<150)||(L_on_v>350) ) return false;
  return true;
}


bool TofRecon::GoodTofCluster(TofClusterR* tofcluster, Level1R* lvl1, char* add_name) {
  // pointer check
  if (tofcluster==0) {
    printf("TofRecon::GoodTofCluster-W an invalid tof cluster pointer.\n");
    return false;
  }
  // loop on raw clusters 
  for (int iraw=0; iraw<tofcluster->NTofRawCluster(); iraw++) {
    TofRawClusterR* rawcluster = tofcluster->pTofRawCluster(iraw);
    if (!TofRecon::GoodTofRawCluster(rawcluster,lvl1,add_name)) return false;
  }
  return true;
}


bool TofRecon::GoodTofTrackClusters(TofTrack* tof_track, Level1R* lvl1, char* add_name) {
  if ( (!lvl1)||(!tof_track) ) return false;
  for (int itof=0; itof<tof_track->GetNhits(); itof++) {
    TofClusterR* cluster = tof_track->GetHit(itof);
    if (cluster==0) return false; 
    if (!GoodTofCluster(cluster,lvl1,add_name)) return false;
  }
  return true;
}


int TofRecon::DropBadTofTrackClusters(TofTrack* tof_track, Level1R* lvl1, char* add_name) {
  if ( (!lvl1)||(!tof_track) ) return 0;
  int ndrop = 0;
  for (int itof=tof_track->GetNhits()-1; itof>=0; itof--) {
    TofClusterR* cluster = tof_track->GetHit(itof);
    if (cluster==0) { 
      ndrop++; 
      tof_track->DropHit(itof); 
      continue; 
    }
    if (!GoodTofCluster(cluster,lvl1,add_name)) { 
      ndrop++; 
      tof_track->DropHit(itof);
      continue; 
    }
  }   
  return ndrop;
}


bool TofRecon::NoPreviousFT(AMSEventR* event, float time) {
  if (!event) return false; // no event
  // first TofRawSide used as reference 
  int nrawside = event->NTofRawSide();
  if (nrawside<1) return false; // no FT
  TofRawSideR* raw_ref = (TofRawSideR*) event->pTofRawSide(0);
  int          nft_ref = raw_ref->nftdc; 
  float        lft_ref = 0.0244*1e-3*raw_ref->getftdc(nft_ref-1);
  bool         bad_ref = false;
  for (int ift=0; ift<nft_ref-1; ift++) {
    float dft = lft_ref - 0.0244*1e-3*raw_ref->getftdc(ift);
    if (dft<time) bad_ref = true; 
  }
  // I know this is stupid but I will check that other channels behave in the same way of reference one
  bool bad_check = false;
  for (int itof=1; itof<event->NTofRawSide(); itof++) {
    TofRawSideR* raw = (TofRawSideR*) event->pTofRawSide(itof);
    int          nft = raw->nftdc; 
    float        lft = 0.0244*1e-3*raw->getftdc(nft-1);
    bool         bad = false;
    /* 
    // it happens: when the FT is at the edge of the time history
    if (nft!=nft_ref) { 
      printf("TofRecon::NoPreviousFT-W different number of FT in TOF channels detected. This is impossible! Check run %10d and event %10d.\n",
      event->fHeader.Run,event->fHeader.Event); 
      bad_check = true; 
    }
    */
    for (int ift=0; ift<nft-1; ift++) {
      float dft = lft - 0.0244*1e-3*raw->getftdc(ift);
      if (dft<time) bad = true;
    }
    if (bad!=bad_ref) { 
      printf("TofRecon::NoPreviousFT-W previous FT inconsistency between TOF channels detected. This is impossible! Check run %10d and event %10d.\n",
        event->fHeader.Run,event->fHeader.Event);
      bad_check = true; 
    }
  }
  return (!bad_check)&&(!bad_ref);
} 



void TofRecon::ClearTofTracks() {
  for (int i=0; i<NTofTrack(); i++) {
    TofTrack* track = pTofTrack(i);
    if (track) delete track; 
  }
  TofTracksList.clear();
}


int TofRecon::BuildTofTracks(AMSEventR* event, int level) {
  // start reconstruction
  ClearTofTracks();
  // level 1 is needed
  if (event->NLevel1()<1) {
    printf("TofRecon::BuildTofTracks-W no level 1.\n");
    return 0;
  }
  Level1R* lvl1 = event->pLevel1(0);

  ///////////////////////////////////////////////
  // BuildTofTracks: my reconstruction 
  ///////////////////////////////////////////////

  if ( (level==0)||(level==2) ) {
    // ntuple
    if ( (EnabledNtuple)&&(!TofReconTree) ) InitNtuple();

    ///////////////////////////////////////////////
    // Cluster list by layer: use only the good TOF clusters
    ///////////////////////////////////////////////

    vector<TofClusterR*> good_tof_clusters_list[4];
    for (int itof=0; itof<event->NTofCluster(); itof++) {
      TofClusterR* cluster = event->pTofCluster(itof);
      if (cluster==0) continue; 
      if (GoodTofCluster(cluster,lvl1,"build")) good_tof_clusters_list[cluster->Layer-1].push_back(cluster);
    }
 
    ///////////////////////////////////////////////
    // Create TOF Candidate List
    // 1. Candidate is a couple of good clusters on two different layers of UTOF (or LTOF)
    // 2. Eliminate bad candidates (> minimum distance, bad charge compatibility)
    // 4. Sort by distance
    // 5. Purge the list by candidates with already used clusters (from the closest to the most far)
    // 6. Add to the list all the spare clusters 
    ///////////////////////////////////////////////

    vector<TofCandidate*> candidates_list[2];
    char couple_name[2] = {'U', 'L'}; 
    // create list of 2 cluster candidtaes
    for (int icouple=0; icouple<2; icouple++) {
      int index_upper = icouple*2;
      int index_lower = icouple*2+1;
      int nupper = int(good_tof_clusters_list[index_upper].size());
      int nlower = int(good_tof_clusters_list[index_lower].size());
      for (int iupper=0; iupper<nupper; iupper++) {
        TofClusterR* cl_upper = good_tof_clusters_list[index_upper].at(iupper);
        for (int ilower=0; ilower<nlower; ilower++) {
          TofClusterR* cl_lower = good_tof_clusters_list[index_lower].at(ilower);
          TofCandidate* candidate = new TofCandidate(couple_name[icouple],cl_upper,cl_lower);
          // bad match
          if (!GoodTofCandidate(candidate,"build")) {
            if (candidate) delete candidate;
            continue;
          }
          // good match
          candidates_list[icouple].push_back(candidate);
        }
      }
      // sorting list
      sort(candidates_list[icouple].begin(),candidates_list[icouple].end(),CompareTofCandidateDistance);
      // searching for already used candidates
      vector<TofClusterR*> used_clusters;
      for (int icand=0; icand<int(candidates_list[icouple].size()); icand++) {
        vector<int> index_of_candidates_to_be_erased;
        TofCandidate* candidate = candidates_list[icouple].at(icand);
        TofClusterR* cl_upper = candidate->GetCluster(0);
        TofClusterR* cl_lower = candidate->GetCluster(1);
        // already used?
        bool one_of_the_two_is_used = false;
        for (vector<TofClusterR*>::iterator it_used=used_clusters.begin(); it_used!=used_clusters.end(); it_used++) {
          if ( (cl_upper==(*it_used))||(cl_lower==(*it_used)) ) one_of_the_two_is_used = true;
        }
        if (one_of_the_two_is_used) {
          // increase list of candidates to be deleted
          index_of_candidates_to_be_erased.push_back(icand);
        }
        else {
          // increase used list 
          used_clusters.push_back(cl_upper);
          used_clusters.push_back(cl_lower);
        }
        // purging
        for (int ierase=0; ierase<int(index_of_candidates_to_be_erased.size()); ierase++) {
          // erase from memory
          TofCandidate* erase_it = candidates_list[icouple].at(index_of_candidates_to_be_erased.at(ierase));
          if (erase_it) delete erase_it;
          // erase from vector
          candidates_list[icouple].erase(candidates_list[icouple].begin() + index_of_candidates_to_be_erased.at(ierase));
        }
        index_of_candidates_to_be_erased.clear();
      }
      // add cluster excluded from combinations (one-cluster-candidates)
      vector<TofClusterR*> solo_clusters;
      for (int iupper=0; iupper<nupper; iupper++) {
        TofClusterR* cl_upper = good_tof_clusters_list[index_upper].at(iupper);
        if (find(used_clusters.begin(),used_clusters.end(),cl_upper)==used_clusters.end()) solo_clusters.push_back(cl_upper);
      }
      for (int ilower=0; ilower<nlower; ilower++) {
        TofClusterR* cl_lower = good_tof_clusters_list[index_lower].at(ilower);
        if (find(used_clusters.begin(),used_clusters.end(),cl_lower)==used_clusters.end()) solo_clusters.push_back(cl_lower);
      }
      for (int iclu=0; iclu<int(solo_clusters.size()); iclu++) {
        TofClusterR* cluster = solo_clusters.at(iclu);
        TofCandidate* candidate = new TofCandidate(couple_name[icouple],cluster);
        candidates_list[icouple].push_back(candidate);   
      }
    }
    Histogram("UnCand_vs_LnCand_build","; Upper n. Candidates; Lower n. Candidates",15,-0.5,14.5,15,-0.5,14.5,int(candidates_list[0].size()),int(candidates_list[1].size()));

    ///////////////////////////////////////////////
    // Create TofTracks 
    // 1. all combinations of valid upper and lower candidates 
    // 2. upper lower charge compatibility test
    // 3. shared candidates are possible (secondaries)
    // 4. at least three clusters per track
    ///////////////////////////////////////////////
  
    int nupper = int(candidates_list[0].size());
    int nlower = int(candidates_list[1].size());
    for (int iupper=0; iupper<nupper; iupper++) {
      TofCandidate* upper = candidates_list[0].at(iupper);
      for (int ilower=0; ilower<nlower; ilower++) {
        TofCandidate* lower = candidates_list[1].at(ilower);
        if (!GoodTofCandidateCombination(upper,lower,"build")) continue;
        // track construction  
        vector<TofClusterR*> track_tof_cluster;
        for (int ilay=0; ilay<upper->GetNClusters(); ilay++) track_tof_cluster.push_back(upper->GetCluster(ilay));
        for (int ilay=0; ilay<lower->GetNClusters(); ilay++) track_tof_cluster.push_back(lower->GetCluster(ilay));
        TofTrack* tof_track = new TofTrack(track_tof_cluster);

        ////////////////////////////////////////////////////////////////////////////////////       
        // Loose TOF track cuts (I don't want to bias reconstruction in case of uncalibrated data)
        ////////////////////////////////////////////////////////////////////////////////////
 
        if (!GoodTofTrackLooseSelection(tof_track,"build")) {
          if (tof_track) delete tof_track; 
          continue;
        }
        // default
        tof_track->MakeDefaultFits();
        // add
        TofTracksList.push_back(tof_track);
        // histograms 
        FillTofTrackResiduals(tof_track,"build"); 
        FillTofTrackHistograms(tof_track,"build");
        // ntuple
        if (EnabledNtuple) FillTofTrackNtuple(tof_track);
      }
    }
    // clean up TofCandidate list  
    for (int icouple=0; icouple<2; icouple++) {
      for (int ierase=0; ierase<int(candidates_list[0].size()); ierase++) {
        TofCandidate* erase_it = candidates_list[icouple].at(ierase);
        if (erase_it) delete erase_it;
      }
      candidates_list[icouple].clear();
    }

    ReorderTofTracks();
    // END of my reconstruction 
  }

  ///////////////////////////////////////////////
  // BuildTofTracks: Convert all BetaR  
  ///////////////////////////////////////////////

  // convert 
  if ( (level==1)||(level==2) ) {
    int nbeta = event->NBeta();
    for (int ibeta=0; ibeta<nbeta; ibeta++) {
      BetaR* beta = event->pBeta(ibeta);
      if (beta==0) continue;
      TofTrack* tof_track = new TofTrack(beta);
      TofTracksList.push_back(tof_track);
    }
    // END of conversion
  }
  // histogram
  Histogram("NTracks_build","; N. Tracks",21,-0.5,20.5,int(TofTracksList.size()));
  // return vector
  return int(TofTracksList.size());
}


/* 
// TO BE REMOVED events with some charge recon. problem
if (tof_track->GetMean(TofTrack::kAnode)<(-1+tof_track->GetMean(TofTrack::kDynode))) { 
cout << sqrt(upper.GetAverageSignal()) << " " << sqrt(lower.GetAverageSignal()) << endl;  
tof_track->Print(10);
if (!TofReconHisto.Get("RelErr_vs_TM_Anode_bad"))    
TofReconHisto.Add(new TH2D("RelErr_vs_TM_Anode_bad","; TM Anode (#sqrt{ADC}); TM RMS / TM",600,0,30,200,0,1));
if (TofReconHisto.Get("RelErr_vs_TM_Anode_bad"))     
TofReconHisto.Fill("RelErr_vs_TM_Anode_bad",tof_track->GetMean(TofTrack::kAnode),tof_track->GetRelErr(TofTrack::kAnode));
if (!TofReconHisto.Get("RelErr_vs_TM_Dynode_bad"))   
TofReconHisto.Add(new TH2D("RelErr_vs_TM_Dynode_bad","; TM Dynode (#sqrt{ADC}); TM RMS / TM",600,0,30,200,0,1));
if (TofReconHisto.Get("RelErr_vs_TM_Dynode_bad"))    
TofReconHisto.Fill("RelErr_vs_TM_Dynode_bad",tof_track->GetMean(TofTrack::kDynode),tof_track->GetRelErr(TofTrack::kDynode));
}
*/


void TofRecon::ReorderTofTracks() { 
  sort(TofTracksList.begin(),TofTracksList.end(),CompareTofTracksNHits);
  sort(TofTracksList.begin(),TofTracksList.end(),CompareTofTracksCharge);
}


bool TofRecon::GoodTofCandidate(TofCandidate* candidate, char* add_name) {  
  if (!candidate) return false;
  if (candidate->GetNClusters()==1) return true;
  char name = candidate->GetType();
  Histogram(
    Form("%cTOF_dY_vs_dX_%s",name,add_name),
    "; #DeltaX (cm); #DeltaY (cm)",
    120,-60,60,120,-60,60,
    candidate->GetdX(),candidate->GetdY()
  );
  Histogram(
    Form("%cTOF_UEdep_vs_LEdep_%s",name,add_name),
    Form("; %cTOF Lower E_{dep} (#sqrt{MeV}); %cTOF Upper E_{dep} (#sqrt{MeV})",name,name),
    200,0,20,200,0,20,
    sqrt(candidate->GetLowerSignal()),sqrt(candidate->GetUpperSignal())
  );
  // cl_upper and cl_lower good spatial match in x and y
  if ( (fabs(candidate->GetdX())>MaxDistance) ||
       (fabs(candidate->GetdY())>MaxDistance) ) return false;
  // cl_upper and cl_lower have a good signal match 
  if ( (sqrt(candidate->GetUpperSignal())>sqrt(candidate->GetLowerSignal())+OffsetCharge1) ||
       (sqrt(candidate->GetUpperSignal())<sqrt(candidate->GetLowerSignal())-OffsetCharge1) ) return false;
  return true;
}


bool TofRecon::GoodTofCandidateCombination(TofCandidate* upper, TofCandidate* lower, char* add_name) {
  if      ( ( upper)&&(!lower) ) return true;
  else if ( (!upper)&&( lower) ) return true;
  else if ( (!upper)&&(!lower) ) return false;
  // histogram
  Histogram(
    Form("UTOFPlaneEdep_vs_LTOFPlaneEdep_%s",add_name),
    "; LTOF E_{dep} (#sqrt{MeV}); UTOF E_{dep} (#sqrt{MeV})",
    200,0,20,200,0,20,
    sqrt(lower->GetAverageSignal()),sqrt(upper->GetAverageSignal())
  );
  // good compatibility between upper and lower charge 
  if ( (sqrt(upper->GetAverageSignal())>sqrt(lower->GetAverageSignal())+OffsetCharge2) ||
       (sqrt(upper->GetAverageSignal())<sqrt(lower->GetAverageSignal())-OffsetCharge2) ) return false;
  return true;
}


bool TofRecon::GoodTofTrackLooseSelection(TofTrack* tof_track, char* add_name) {
  if (!tof_track) return false;
  // eliminate tracks with a poor number of hits (2 is too low the fits will be definitions!)
  if (tof_track->GetNhits()<MinNHits) return false;
  // unbiased fitting 
  if (!tof_track->MakeSpatialFitInGaussianApprox()) return false;
  if (!tof_track->MakeSpatialResiduals()) return false;
  if (!tof_track->MakeTimeFit()) return false;
  if (!tof_track->MakeTimeResiduals()) return false;
  if (!tof_track->MakeMean(TofTrack::kAnode,TofTrack::kSqrt,TofTrack::kMIP)) return false;
  tof_track->MakeMean(TofTrack::kDynode,TofTrack::kSqrt,TofTrack::kMIP); // can fail 
  // some histograms for quality
  Histogram(
    Form("logChiSqXY_vs_Beta_%s",add_name),
    "; #beta; log_{10}(#chi_{xy}^{2})",
    240,-1.2,1.2,100,-3,3,
    tof_track->GetBeta(),log10(tof_track->GetRedChiSqXY())
  );
  Histogram(
    Form("logChiSqT_vs_Beta_%s",add_name),
    "; #beta; log_{10}(#chi_{xy}^{2})",
    240,-1.2,1.2,100,-3,3,
    tof_track->GetBeta(),log10(tof_track->GetRedChiSqT())
  );
  Histogram(
    Form("RelErr_vs_TM_Anode_%s",add_name),
    "; TM Anode (#sqrt{ADC}); TM RMS / TM",
    600,0,30,200,0,1,
    tof_track->GetMean(TofTrack::kAnode),tof_track->GetRelErr(TofTrack::kAnode)
  );
  Histogram(
    Form("RelErr_vs_TM_Dynode_%s",add_name),
    "; TM Dynode (#sqrt{ADC}); TM RMS / TM",
    600,0,30,200,0,1,
    tof_track->GetMean(TofTrack::kDynode),tof_track->GetRelErr(TofTrack::kDynode)
  );
  Histogram(
    Form("TMAnode_vs_TMDynode_%s",add_name),
    "; TM Dynode (#sqrt{ADC}); TM Anode (#sqrt{ADC})",
    600,0,30,600,0,30,
    tof_track->GetMean(TofTrack::kDynode),tof_track->GetMean(TofTrack::kAnode)
  );
  Histogram(
    Form("RelErrAnode_vs_RelErrDynode_%s",add_name),
    "; TM RMS / TM Dynode; TM RMS / TM Anode",
    200,0,1,200,0,1,
    tof_track->GetRelErr(TofTrack::kDynode),tof_track->GetRelErr(TofTrack::kAnode)
  );
  // very bad temporal fit (could bias at low beta, but the cut is very loose) 
  if (log10(tof_track->GetRedChiSqT())>log10(MaxRedChiSqT)) return false;
  // very bad chisquares (could bias at low beta, but the cut is very loose)
  if (log10(tof_track->GetRedChiSqXY())>log10(MaxRedChiSqXY)) return false;
  // very bad charge recon. (could bias at low beta, but the cut is very loose)
  if (tof_track->GetRelErr(TofTrack::kAnode)>MaxRelErrAnode) return false;
  // very bad charge recon. (could bias at low beta, but the cut is very loose)
  if (tof_track->GetRelErr(TofTrack::kDynode)>MaxRelErrDynode) return false;
  // return   
  return true;
}

 
bool TofRecon::GoodTofTrackWithBuildRules(TofTrack* tof_track, Level1R* lvl1) { 
  if ( (!lvl1)||(!tof_track) ) return false;
  // good hits 
  if (!GoodTofTrackClusters(tof_track,lvl1,"goodtoftr")) return false;
  // good candidates upper and lower cluster compatibilities
  bool bad = false;
  TofCandidate* candidate[2] = {0,0};
  char plane_name[2] = {'U', 'L'};
  for (int iplane=0; iplane<2; iplane++) { 
    TofClusterR* cl_upper = tof_track->GetHitLayer(1+iplane*2);
    TofClusterR* cl_lower = tof_track->GetHitLayer(2+iplane*2);
    if      ( (!cl_upper)&&(!cl_lower) ) continue;
    else if ( ( cl_upper)&&(!cl_lower) ) candidate[iplane] = new TofCandidate(plane_name[iplane],cl_upper); 
    else if ( (!cl_upper)&&( cl_lower) ) candidate[iplane] = new TofCandidate(plane_name[iplane],cl_lower); 
    else if ( ( cl_upper)&&( cl_lower) ) candidate[iplane] = new TofCandidate(plane_name[iplane],cl_upper,cl_lower);
    TofCandidate* candidate = new TofCandidate(plane_name[iplane],cl_upper,cl_lower);
    // bad match
    if (!GoodTofCandidate(candidate,"goodtoftr")) bad = true;
  } 
  if (bad) {
    if (candidate[0]) delete candidate[0];
    if (candidate[1]) delete candidate[1];
    return false;
  }
  // up-down compatibilities
  if ( (candidate[0])&&(candidate[1]) ) {
    TofCandidate* upper = candidate[0];
    TofCandidate* lower = candidate[1];  
    if (!GoodTofCandidateCombination(upper,lower,"goodtoftr")) bad = true;
  }
  if (candidate[0]) delete candidate[0];
  if (candidate[1]) delete candidate[1];
  if (bad) return false;
  // loose cuts 
  if (!GoodTofTrackLooseSelection(tof_track,"goodtoftr")) return false;
  // default
  tof_track->MakeDefaultFits();
  return true;
}


TofTrack* TofRecon::GetHighestChargeTofTrack(int nminclusters) {
  float     q = 0;
  TofTrack* tof_track = 0;
  for (int itrack=0; itrack<NTofTrack(); itrack++) {
    TofTrack* track = pTofTrack(itrack);
    // minimum hits
    if (track->GetNhits()<nminclusters) continue;
    // then higher one
    float mean = track->GetMean(TofTrack::kAnode);
    if (q<mean) {
      q = mean;
      tof_track = track;
    }
  }
  if (!tof_track) return 0;
  Histogram("Nhits_high","; N. hits",10,-0.5,10.5,tof_track->GetNhits());
  Histogram("RelErr_vs_TM_Anode_high","; TM Anode (#sqrt{ADC}); TM RMS / TM",600,0,30,200,0,1,
    tof_track->GetMean(TofTrack::kAnode),tof_track->GetRelErr(TofTrack::kAnode));
  Histogram("RelErr_vs_TM_Dynode_high","; TM Dynode (#sqrt{ADC}); TM RMS / TM",600,0,30,200,0,1,
    tof_track->GetMean(TofTrack::kDynode),tof_track->GetRelErr(TofTrack::kDynode));
  Histogram("TMAnode_vs_TMDynode_high","; TM Dynode (#sqrt{ADC}); TM Anode (#sqrt{ADC})",600,0,30,600,0,30,
    tof_track->GetMean(TofTrack::kDynode),tof_track->GetMean(TofTrack::kAnode));
  return tof_track;
}


TofTrack* TofRecon::BuildATofTrackFromTrTrack(AMSEventR* event, TrTrackR* trk_track) {
  if (!trk_track) return 0;
  if (event->NLevel1()!=1) return 0;
  Level1R* lvl1 = event->pLevel1(0);
  // loop on all clusters
  // - get only the good ones
  // - test if the track is passing through the paddle 
  TofClusterR* clusters[4] = {0,0,0,0};
  for (int itof=0; itof<event->NTofCluster(); itof++) {
    TofClusterR* cluster = event->pTofCluster(itof);
    if (cluster==0) continue;
    if (!GoodTofCluster(cluster,lvl1,"fromtrtrack")) continue;
    // loop on raw cluster and ask if the track passes at least onto one of them 
    bool through = false;
    float z = cluster->Coo[2];
    for (int iraw=0; iraw<cluster->NTofRawCluster(); iraw++) { 
      TofRawClusterR* rawcluster = cluster->pTofRawCluster(iraw);
      int   il = rawcluster->Layer - 1;
      int   ib = rawcluster->Bar - 1;
      // only inner ... 
      AMSPoint trk_point;
      AMSDir   trk_dir; 
      trk_track->Interpolate(z,trk_point,trk_dir);
      bool inside = TofGeometry::HitCounter(trk_point.x(),trk_point.y(),il,ib,1);
      cout << il << " " << ib << " " << trk_point.x() << " " << trk_point.y() << " " <<  trk_point.z() << " " << inside << endl;
      if (inside) through = true;
    }
    if (through) clusters[cluster->Layer-1] = cluster;
  }
  // create track
  vector<TofClusterR*> list;
  for (int ilayer=0; ilayer<4; ilayer++) {
    if (clusters[ilayer]!=0) list.push_back(clusters[ilayer]);
  }
  TofTrack* tof_track = new TofTrack(list);
  // APPLY CUTS?
  tof_track->MakeDefaultFits();
  tof_track->Print(100);
  return tof_track;
}


bool TofRecon::AssociateTrTracksToTofTracks(AMSEventR* event, int id) { 
  TrTracksList.clear();
  if (!event) return false;
  for (int itoftr=0; itoftr<NTofTrack(); itoftr++) {
    TofTrack* tof_track = pTofTrack(itoftr);
    vector<TrTrackR*> TrTracksOkList;
    TrTracksOkList.clear();

    ///////////////////////////////////////////////
    // Create the list of TrTracks that can be associated
    // 1. inside Y paddles
    // 2. good position and direction match at jlayer 2 
    // 3. good charge match
    // 4. reorder the list on base of Y chisq
    ///////////////////////////////////////////////

    for (int itrktr=0; itrktr<event->NTrTrack(); itrktr++) {
      TrTrackR* trk_track = (TrTrackR*) event->pTrTrack(itrktr);
      // compatibility
      if (!trk_track) continue;
      // if (!IsInsidePaddlesYZ(tof_track,trk_track,id)) continue;
      if (!IsAGoodSpatialMatchAlongY(tof_track,trk_track,id)) continue;
      if (!IsAGoodChargeMatch(tof_track,trk_track,id)) continue;
      // good match list 
      TrTracksOkList.push_back(trk_track);
    }
    // reorder list by chisqY
    int size = int(TrTracksOkList.size());
    for (int i=1; i<size; i++) {
      for (int j=0; j<size-i; j++) {
        cout << i << " " << j << endl; 
        TrTrackR* trk_track1 = TrTracksOkList.at(i);
        TrTrackR* trk_track2 = TrTracksOkList.at(j);
        float chisq1 = CalculateRedChiSqY(tof_track,trk_track1,id);
        float chisq2 = CalculateRedChiSqY(tof_track,trk_track2,id);
        if (chisq2<chisq1) swap(TrTracksOkList.at(i),TrTracksOkList.at(j));
      }
    }
    TrTracksList.push_back(TrTracksOkList);
  }
  if (int(TrTracksList.size())!=NTofTrack()) {
    printf("TofRecon::AssociateTrTracksToTofTracks-W the number of created association is different from number of TofTracks. This is not supposed to happen!\n");
    return false;
  }
  // associate to a TofTrack to the first TrTrack 
  for (int i=0; i<NTofTrack(); i++) {
    if (int(TrTracksList.at(i).size())>0) {
      TofTrack* tof_track = (TofTrack*) pTofTrack(i);
      TrTrackR* trk_track = TrTracksList.at(i).at(0);
      tof_track->SetAssociatedTrTrack(trk_track,id);
      tof_track->MakeDefaultFits();
    }
  }
  return true;
}


void TofRecon::Histogram(char* name, char* title, int nxbin, float xmin, float xmax, int nybin, float ymin, float ymax, float x, float y) {
  if (!EnabledHistograms) return;
  if (!TofReconHisto.Get(name)) TofReconHisto.Add(new TH2D(name,title,nxbin,xmin,xmax,nybin,ymin,ymax));
  if (TofReconHisto.Get(name)) TofReconHisto.Fill(name,x,y);
}


void TofRecon::Histogram(char* name, char* title, int nxbin, float xmin, float xmax, float x) {
  if (!EnabledHistograms) return;
  if (!TofReconHisto.Get(name)) TofReconHisto.Add(new TH1D(name,title,nxbin,xmin,xmax));
  if (TofReconHisto.Get(name)) TofReconHisto.Fill(name,x);
}


void TofRecon::FillTofTrackHistograms(TofTrack* tof_track, char* add_name) {
  Histogram(Form("Theta_vs_Phi_%s",add_name),"; #varphi; #theta",100,-3.2,3.2,100,0,1,tof_track->GetDir().getphi(),tof_track->GetDir().gettheta());
  Histogram(Form("Y_vs_X_%s",add_name),"; X (cm); Y (cm)",120,-60,60,120,-60,60,tof_track->GetPoint().x(),tof_track->GetPoint().y());
  Histogram(Form("logChiSqXY_vs_Pattern_%s",add_name),"; Pattern; log_{10}(#chi_{XY}^{2})",16,0.5,16.5,100,-3,3,tof_track->GetPattern(),log10(tof_track->GetRedChiSqXY()));  
  Histogram(Form("Beta_vs_Pattern_%s",add_name),"; Pattern; #beta",16,0.5,16.5,240,-1.2,1.2,tof_track->GetPattern(),tof_track->GetBeta());
  Histogram(Form("logChiSqT_vs_Pattern_%s",add_name),"; Pattern; log_{10}(#chi_{t}^{2})",16,0.5,16.5,100,-3,3,tof_track->GetPattern(),log10(tof_track->GetRedChiSqT()));
  Histogram(Form("TMAnode_vs_TMDynode_%s",add_name),"; TM Dynode (#sqrt{ADC}); TM Anode (#sqrt{ADC})",600,0,30,600,0,30,tof_track->GetMean(TofTrack::kDynode),tof_track->GetMean(TofTrack::kAnode));
  /*
  float uppersig_a = sqrt(tof_track->GetAverageSignalPlane(0,TofTrack::kAnode,TofTrack::kAngle|TofTrack::kBeta));
  float lowersig_a = sqrt(tof_track->GetAverageSignalPlane(1,TofTrack::kAnode,TofTrack::kAngle|TofTrack::kBeta));
  Histogram(Form("Upper_vs_Lower_Anode_%s",add_name),"; Lower Signal (#sqrt{ADC}); Upper Signal (#sqrt{ADC})",200,0,20,200,0,20,lowersig_a,uppersig_a);
  float uppersig_d = sqrt(tof_track->GetAverageSignalPlane(0,TofTrack::kDynode,TofTrack::kAngle|TofTrack::kBeta));
  float lowersig_d = sqrt(tof_track->GetAverageSignalPlane(1,TofTrack::kDynode,TofTrack::kAngle|TofTrack::kBeta));
  Histogram(Form("Upper_vs_Lower_Dynode_%s",add_name),"; Lower Signal (#sqrt{ADC}); Upper Signal (#sqrt{ADC})",200,0,20,200,0,20,lowersig_d,uppersig_d);
  */
}


void TofRecon::FillTofTrackNtuple(TofTrack* tof_track) {
  Tof_Track.nPoints = tof_track->GetNhits();
  Tof_Track.Pattern = tof_track->GetPattern();
  Tof_Track.X = tof_track->GetPoint().x();
  Tof_Track.Y = tof_track->GetPoint().y();
  Tof_Track.Theta = tof_track->GetDir().getphi();
  Tof_Track.Phi = tof_track->GetDir().gettheta();
  Tof_Track.ChiSqXY = tof_track->GetRedChiSqXY();
  Tof_Track.Beta = tof_track->GetBeta();
  Tof_Track.ChiSqT = tof_track->GetRedChiSqT();
  Tof_Track.AnodeTruncMean = tof_track->GetMean(TofTrack::kAnode);
  Tof_Track.AnodeRMS = tof_track->GetRMS(TofTrack::kAnode);
  Tof_Track.DynodeTruncMean = tof_track->GetMean(TofTrack::kDynode);
  Tof_Track.DynodeRMS = tof_track->GetRMS(TofTrack::kDynode);
  if (TofReconTree) TofReconTree->Fill();
}


void TofRecon::FillTofTrackResiduals(TofTrack* tof_track, char* add_name) {
  float beta = tof_track->GetBeta();
  for (int itof=0; itof<tof_track->GetNhits(); itof++) {
    TofClusterR* cluster = (TofClusterR*) tof_track->GetHit(itof);
    if (!cluster) continue;
    int      layer = cluster->Layer;
    AMSPoint residual = tof_track->GetSpatialResidual(layer);
    int      index = (cluster->Layer-1)*10 + cluster->Bar-1;
    bool     paddle_along_y = (cluster->Layer==1)||(cluster->Layer==4);
    float    dtrasv = (paddle_along_y) ? residual.y() : residual.x();
    float    dlongi = (paddle_along_y) ? residual.x() : residual.y();
    bool     multi_cluster_hit = (cluster->NTofRawCluster()>1);
    if (!multi_cluster_hit)  { 
      Histogram(Form("dTrasversa_vs_PaddleIndex_%s",add_name),"; Paddle Index; #DeltaX trasversal coordinate (cm)",41,-0.5,40.5,400,-20,20,index,dtrasv);
      Histogram(Form("dLongitudi_vs_PaddleIndex_%s",add_name),"; Paddle Index; #DeltaX longitudinal coordinate (cm)",41,-0.5,40.5,400,-20,20,index,dlongi);
    }
    else {
      Histogram(Form("dTrasvMult_vs_PaddleIndex_%s",add_name),"; Paddle Index; #DeltaX trasversal for multi-paddle (cm)",41,-0.5,40.5,400,-20,20,index,dtrasv);
      Histogram(Form("dLongiMult_vs_PaddleIndex_%s",add_name),"; Paddle Index; #DeltaX longitudinal for multi-paddle (cm)",41,-0.5,40.5,400,-20,20,index,dlongi);
    }
    float dt = tof_track->GetTimeResidual(layer);
    Histogram(Form("dTime_vs_PaddleIndex_%s",add_name),"; Paddle Index; #Deltat (ns)",41,-0.5,40.5,1000,-10,10,index,dt);
    if (tof_track->GetNhits()==4) 
      Histogram(Form("dTime_vs_Beta_Layer%1d_%s",layer,add_name),"; #beta; #Deltat (ns)",120,-1.2,1.2,100,-0.5,0.5,beta,dt);
  }
  if (tof_track->MakeTimeFitInner()) {
    float betainn = tof_track->GetBeta();
    Histogram(Form("Beta_vs_BetaInn_%s",add_name),"; #beta; #beta_{Inner}",120,-1.2,1.2,120,-1.2,1.2,beta,betainn);
  }
  tof_track->MakeTimeFit();
}


bool TofRecon::GoodTofTrackTrTrackMatch(TofTrack* tof_track, TrTrackR* trk_track, int id, char* add_name) {
  if ( (!trk_track)||(!tof_track)||(id<0) ) return false;
  float chi_x = 0;
  float chi_y = 0;
  int   n = 0;
  for (int itof=0; itof<tof_track->GetNhits(); itof++) {
    TofClusterR* cluster = (TofClusterR*) tof_track->GetHit(itof);
    if (!cluster) continue;
    int index = (cluster->Layer - 1)*10 + (cluster->Bar - 1);
    AMSPoint tof_point(cluster->Coo[0],cluster->Coo[1],cluster->Coo[2]);
    AMSPoint trk_point;
    AMSDir   trk_dir;
    trk_track->Interpolate(tof_point.z(),trk_point,trk_dir,id);
    float dx = tof_point.x()-trk_point.x();
    float dy = tof_point.y()-trk_point.y();
    float ex = cluster->ErrorCoo[0];
    float ey = cluster->ErrorCoo[1];
    Histogram(Form("dX_vs_PaddleIndex_%s",add_name),"; Index; #DeltaX (cm)",40,-0.5,39.5,200,-10,10,index,dx);
    Histogram(Form("dY_vs_PaddleIndex_%s",add_name),"; Index; #DeltaY (cm)",40,-0.5,39.5,200,-10,10,index,dy);
    chi_x += dx*dx/ex/ex;
    chi_y += dy*dy/ey/ey;
    n++;
  }
  float chi = (chi_x + chi_y)/(2*n);
  chi_x /= n;
  chi_y /= n;
  Histogram(Form("RedChiSqX_vs_RedChiSqY_%s",add_name),"; log_{10}(#chi^{2}_{X}); log_{10}(#chi^{2}_{Y})",400,-4,4,400,-4,4,log10(chi_x),log10(chi_y));
  Histogram(Form("RedChiSqXY_%s",add_name),"; log_{10}(#chi^{2}_{XY})",400,-4,4,log10(chi));
  if (chi>100) return false;
  return true;
}


void TofRecon::InitNtuple() {
  TDirectory* saveddir = gDirectory;
  gROOT->cd();
  TofReconTree = new TTree("TofReconTree","TOF Reconstruction Tree");
  TofReconTree->Branch("TofTrack",&Tof_Track,
    "nPoints/I:Pattern/I:X/F:Y/F:Theta/F:Phi/F:ChiSqXY/F:Beta/F:ChiSqT/F:AnodeTruncMean/F:AnodeRMS/F:DynodeTruncMean/F:DynodeRMS/F");
  saveddir->cd();
}


void TofRecon::FillComparisonBetweenBetaAndTofTrackStatistics(AMSEventR* event) {
  if (!EnabledHistograms) return; // in this case everything is pointless
  // number of beta/tracks 
  int nbeta = event->NBeta();
  int nbeta4 = 0;
  int ntracks = NTofTrack(); 
  int ntracks4 = 0;
  for (int ibeta=0; ibeta<nbeta; ibeta++) 
    if (event->pBeta(ibeta)->NTofCluster()==4) nbeta4++;
  for (int itrack=0; itrack<ntracks; itrack++) 
    if (pTofTrack(itrack)->GetNhits()==4) ntracks4++;
  // check the TOF tracks inside Beta (Beta > TOF tracks)
  int ntoftrack_in_beta = 0;
  // loop on beta 
  for (int ibeta=0; ibeta<event->NBeta(); ibeta++) {
    BetaR* beta = event->pBeta(ibeta);
    // loop on tof tracks
    for (int itrack=0; itrack<NTofTrack(); itrack++) {
      TofTrack* track = pTofTrack(itrack);
      float diff  = 0;
      int   nhits = 0;
      // loop on beta clusters
      for (int itof=0; itof<beta->NTofCluster(); itof++) {
        TofClusterR* beta_clu = beta->pTofCluster(itof);
        if (!beta_clu) continue;
        int layer = beta_clu->Layer;
        // ask for a specific layer on track
        TofClusterR* track_clu = track->GetHitLayer(layer);
        if (!track_clu) continue;
        // difference
        float dx = beta_clu->Coo[0] - track_clu->Coo[0];
        float dy = beta_clu->Coo[1] - track_clu->Coo[1];
        diff += sqrt(dx*dx + dy*dy);
        nhits++;
      }
      if ((diff/nhits)>1.) continue; // no match
      ntoftrack_in_beta++;
    } 
  }
  // check the Beta inside TOF tracks (TOF tracks > Beta)
  int nbeta_in_toftrack = 0;
  // loop on tof tracks
  for (int itrack=0; itrack<NTofTrack(); itrack++) {
    TofTrack* track = pTofTrack(itrack);
    BetaR* beta = GetIncludedBeta(track,event);
    if (beta==0) continue; // no match
    nbeta_in_toftrack++;
  }
  // histograms
  Histogram("NTracks_vs_NBeta","; n. #beta; n. TOF tracks",11,-0.5,10.5,11,-0.5,10.5,nbeta,ntracks);
  Histogram("NTracks4_vs_NBeta4","; n. #beta; n. TOF tracks",11,-0.5,10.5,11,-0.5,10.5,nbeta4,ntracks4);
  Histogram("NTracksInBeta_vs_NBetaInTracks","; n. TOF tracks recon. inside beta; n. beta recon. inside TOF tracks",11,-0.5,10.5,11,-0.5,10.5,nbeta_in_toftrack,ntoftrack_in_beta);
}


void TofRecon::FillComparisonBetweenBetaAndTofTrack(TofTrack* tof_track, BetaR* beta) {
  if (!EnabledHistograms) return; // in this case everything is pointless
  if (tof_track==0) {
    printf("TofRecon::FillComparisonBetweenBetaAndTofTrack-W no valid pointer to TofTrack.\n");
    return;
  }
  if (beta==0) {
    // printf("TofRecon::FillComparisonBetweenBetaAndTofTrack-W no valid pointer to BetaR.\n"); // it could happen
    return;
  }
  Histogram("Beta_vs_Beta","; GBATCH #beta; TOF Track #beta",120,-1.2,1.2,120,-1.2,1.2,beta->Beta,tof_track->GetBeta());
  Histogram("logChiSqT_vs_logChiSqT","; GBATCH log(#chi^{2}_{T}); TOF Track log(#chi^{2}_{T})",100,-3,3,100,-3,3,
    log10(TMath::Max(1e-9,(Double_t)beta->Chi2)),
    log10(TMath::Max(1e-9,(Double_t)tof_track->GetRedChiSqT()))
  );
  Histogram("logChiSqS_vs_logChiSqS","; GBATCH log(#chi^{2}_{S}); TOF Track log(#chi^{2}_{S})",100,-3,3,100,-3,3,
    log10(TMath::Max(1e-9,(Double_t)beta->Chi2S)),
    log10(TMath::Max(1e-9,(Double_t)tof_track->GetRedChiSqXY()))
  );
}


BetaR* TofRecon::GetIncludedBeta(TofTrack* tof_track, AMSEventR* event) {
  if (!event) {
    printf("TofRecon::GetIncludedBeta-W no valid pointer to AMSEvent.\n");
    return 0;
  }
  if (!tof_track) {
    printf("TofRecon::GetIncludedBeta-W no valid pointer to TofTrack.\n");
    return 0;
  }
  // loop on beta 
  for (int ibeta=0; ibeta<event->NBeta(); ibeta++) {
    BetaR* beta = event->pBeta(ibeta);
    float diff  = 0;
    int   nhits = 0;
    // loop on tof clusters 
    for (int iclu=0; iclu<tof_track->GetNhits(); iclu++) {
      TofClusterR* track_clu = tof_track->GetHit(iclu);
      if (!track_clu) continue;
      int layer = track_clu->Layer;
      // ask for a specific layer on beta
      TofClusterR* beta_clu = 0;
      for (int itof=0; itof<beta->NTofCluster(); itof++)
        if (beta->pTofCluster(itof)->Layer==layer) beta_clu = beta->pTofCluster(itof);
      if (!beta_clu) continue;
      // difference
      float dx = beta_clu->Coo[0] - track_clu->Coo[0];
      float dy = beta_clu->Coo[1] - track_clu->Coo[1];
      diff += sqrt(dx*dx + dy*dy);
      nhits++;
    }
    if ((diff/nhits)>1.) continue; // no match
    return beta; // the first match is ok
  }
  return 0;
}


/*
bool TofRecon::IsInsidePaddlesYZ(TofTrack* tof_track, TrTrackR* trk_track, int id) {
  if ( (!trk_track)||(!tof_track)||(id<0) ) return false;
  bool isinside = true;
  for (int itof=0; itof<tof_track->GetNhits(); itof++) {
    TofClusterR* cluster = tof_track->GetHit(itof);
    if (!cluster) continue;
    // judge only Y side
    if (TofGeometry::IsTransverseX(cluster->Layer-1)) continue; 
    bool isinside_cluster = false;
    for (int iraw=0; iraw<cluster->NTofRawCluster(); iraw++) {
      TofRawClusterR* rawcluster = cluster->pTofRawCluster(iraw);
      int index1 = (rawcluster->Layer-1)*10 + rawcluster->Bar-1;
      AMSPoint trk_point;
      AMSDir   trk_dir;
      float z = TofGeometry::GetTofPaddleZ(rawcluster->Layer-1,rawcluster->Bar-1);
      trk_track->Interpolate(z,trk_point,trk_dir,id); 
      int index2 = TofGeometry::GetTofPaddleIndexYZ(trk_point.y(),trk_point.z()); 
      if (fabs(index1-index2)<=1) isinside_cluster = true;
    }
    if (!isinside_cluster) isinside = false; 
  } 
  return isinside;
}
*/


bool TofRecon::IsInsidePaddles(TofTrack* tof_track, TrTrackR* trk_track, int id) {
  if ( (!trk_track)||(!tof_track)||(id<0) ) return false;  
  // loop on clusters
  for (int ihit=0; ihit<tof_track->GetNhits(); ihit++) {
    TofClusterR* cluster = (TofClusterR*) tof_track->GetHit(ihit);
    if (!cluster) return false;
    float z = cluster->Coo[2];
    // loop on paddles (rawclusters)
    for (int irc=0; irc<cluster->NTofRawCluster(); irc++){
      TofRawClusterR* rawcluster = cluster->pTofRawCluster(irc);
      int ibar = rawcluster->Bar - 1;
      int ilayer = rawcluster->Layer - 1;
      AMSPoint hit_point;
      AMSDir hit_dir;
      trk_track->Interpolate(z,hit_point,hit_dir,id); 
      float length = TofGeometry::PathLengthInAPaddle(ilayer,ibar,hit_point,hit_dir);
      if (length<=1e-6) return false; 
    }
  }
  return true;
}


int TofRecon::DropOutsideTofTrackClusters(TofTrack* tof_track, TrTrackR* trk_track, int id) { 
  if ( (!trk_track)||(!tof_track)||(id<0) ) return 0;
  int ndrop = 0;
  // loop on clusters
  for (int itof=tof_track->GetNhits()-1; itof>=0; itof--) {
    TofClusterR* cluster = tof_track->GetHit(itof);
    if (!cluster) {
      ndrop++;
      tof_track->DropHit(itof);
      continue;
    }
    bool  bad = false;
    float z = cluster->Coo[2];
    // loop on paddles 
    for (int irc=0; irc<cluster->NTofRawCluster(); irc++){
      TofRawClusterR* rawcluster = cluster->pTofRawCluster(irc);
      int ibar = rawcluster->Bar - 1;
      int ilayer = rawcluster->Layer - 1;
      AMSPoint hit_point;
      AMSDir hit_dir;
      trk_track->Interpolate(z,hit_point,hit_dir,id);
      float length= TofGeometry::PathLengthInAPaddle(ilayer,ibar,hit_point,hit_dir);
      if (length<=1e-6) bad = true;
    }
    // drop 
    if (bad) {
      ndrop++;
      tof_track->DropHit(itof);
      continue;
    }
  }
  return ndrop;
}


bool TofRecon::IsAGoodSpatialMatchAlongY(TofTrack* tof_track, TrTrackR* trk_track, int id) {
  if ( (!trk_track)||(!tof_track)||(id<0) ) return false;
  float dx = 0;
  float dy = 0;
  float costheta = 0;
  float z_match = TkDBc::Head->GetZlayerJ(2);
  if (!CalculateSpatialDifferenceAtZ(tof_track,trk_track,z_match,dx,dy,costheta,id)) return false; 
  if (dy>MindYForGoodMatch) return false;
  return true;
}


bool TofRecon::CalculateSpatialDifferenceAtZ(TofTrack* tof_track, TrTrackR* trk_track, float z_match, float& dx, float& dy, float& costheta, int id) {
  if ( (!tof_track)||(!trk_track)||(id<0) ) return false; 
  if ( (z_match<-200)||(z_match>200) ) return false;
  AMSPoint tof_point = tof_track->GetPoint(z_match);
  AMSDir   tof_dir   = tof_track->GetDir(); 
  AMSPoint trk_point;
  AMSDir   trk_dir;
  trk_track->Interpolate(z_match,trk_point,trk_dir,id);
  costheta = trk_dir.prod(tof_dir);
  dx = (trk_point-tof_point).x();
  dy = (trk_point-tof_point).y();
  return true;
}


bool TofRecon::CalculateChargeDifference(TofTrack* tof_track, TrTrackR* trk_track, float& dQ, int id) {

  /////////////////////////////////
  ////////// TO BE FIXED //////////
  /////////////////////////////////

  if ( (!tof_track)||(!trk_track)||(id<0) ) return false;
  // tracker temporary charge estimation
  float q_trk_x = TrCharge::GetMean(TrCharge::kInner|TrCharge::kTruncMean|TrCharge::kSqrt,trk_track,TrCharge::kX,1,-1,
    TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kMIP).Mean;
  float q_trk_y = TrCharge::GetMean(TrCharge::kInner|TrCharge::kTruncMean|TrCharge::kSqrt,trk_track,TrCharge::kY,1,-1,
    TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kMIP).Mean;
  float q_trk = q_trk_x;
  if ( (q_trk_x>12.5)&&(q_trk_y>12.5) ) q_trk = q_trk_y;
  float q_tof = tof_track->GetQ();
  dQ = fabs(q_trk-q_tof);
  return true; 
}


bool TofRecon::IsAGoodChargeMatch(TofTrack* tof_track, TrTrackR* trk_track, int id) {
  if ( (!trk_track)||(!tof_track)||(id<0) ) return false;
  float dQ = 100;
  if (!CalculateChargeDifference(tof_track,trk_track,dQ,id)) return false;
  if (dQ>MindQForGoodMatch) return false;
  return true;
}


float TofRecon::CalculateRedChiSqY(TofTrack* tof_track, TrTrackR* trk_track, int id) {
  if ( (!trk_track)||(!tof_track)||(id<0) ) return false;
  float chisq = 0;
  int ndof = 0;
  for (int itof=0; itof<tof_track->GetNhits(); itof++) {
    TofClusterR* cluster = tof_track->GetHit(itof);
    AMSPoint trk_point;
    AMSDir   trk_dir;
    trk_track->Interpolate(cluster->Coo[2],trk_point,trk_dir,id);
    float value = (cluster->Coo[1]-trk_point.y())/cluster->ErrorCoo[1];
    chisq += value*value;
    ndof++;
  } 
  return chisq/ndof;
}


TofTrack* TofRecon::BuildAll(AMSEventR* event, int id) {
  BuildTofTracks(event);
  if (TofRecon::NTofTrack()<1) return 0;
  if (event->NTrTrack()<1) return 0;
  AssociateTrTracksToTofTracks(event,id);
  TofTrack* tof_track = pTofTrack(0);
  if (!tof_track) return 0;
  TrTrackR* trk_track = tof_track->GetAssociatedTrTrack();
  if (!trk_track) return 0;
  return tof_track;
}


/*
TrTrackR* TofRecon::GetClosestTrTrack(TofTrack* tof_track, AMSEventR* event) {
  // tracker-tof match (at trk layer 1)
  float z_match = TkDBc::Head->GetZlayerJ(2);
  // quantities on witch is possible to cut (afterwords)
  float min_distance = 10;
  float min_costheta = 0;
  float min_dq = 15;
  float min_q_tof = -1;
  TrTrackR* trk_track = 0;
  AMSPoint tof_point = tof_track->GetPoint(z_match);
  AMSDir tof_dir = tof_track->GetDir(); 
  for (int itrack=0; itrack<event->NTrTrack(); itrack++) {
    TrTrackR* track = event->pTrTrack(itrack);
    // track inside Y-transverse coordinate paddles (x could be wrong by multiplicity)
    // if (!IsInsidePaddlesYZ(tof_track,track)) continue;
    // track-tof matching (direction and charge)
    AMSPoint trk_point;
    AMSDir   trk_dir;
    track->Interpolate(z_match,trk_point,trk_dir);
    float beta = tof_track->GetBeta(); 
    float costheta = trk_dir.prod(tof_dir);
    float dx = (trk_point-tof_point).x();
    float dy = (trk_point-tof_point).y();
    // only-y, because dx could be wrong by multiplicity
    float distance = fabs(dy); 
    Histogram("TofTrkMatch_dY_vs_dX","; #DeltaX (cm); #DeltaY (cm)",200,-20,20,200,-20,20,dx,dy);
    Histogram("TofTrkMatch_Dist_vs_CosTheta","; cos(#theta); Distance (cm)",200,0.9,1.0,200,0,20,costheta,distance);
    Histogram("TofTrkMatch_dX_vs_Beta","; #beta; #DeltaX (cm)",120,-1.2,1.2,200,-20,20,beta,dx);
    Histogram("TofTrkMatch_dY_vs_Beta","; #beta; #DeltaY (cm)",120,-1.2,1.2,200,-20,20,beta,dy);
    // spatial match (no angular cut, or x cut, due to multiplicity)
    //cout << distance << endl; 
    if (distance>min_distance) continue;
    // tracker TMP charge estimation
    float q_trk_x = TrCharge::GetMean(TrCharge::kInner|TrCharge::kTruncMean|TrCharge::kSqrt,track,TrCharge::kX,1,-1,
      TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kLoss|TrClusterR::kMIP).Mean;
    float q_trk_y = TrCharge::GetMean(TrCharge::kInner|TrCharge::kTruncMean|TrCharge::kSqrt,track,TrCharge::kY,1,-1,
      TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kLoss|TrClusterR::kMIP).Mean;
    Histogram("TofTrkMatch_Qx_vs_Qy","; Q_{Y}; Q_{X}",300,0,30,300,0,30,q_trk_y,q_trk_x);
    float q_trk = q_trk_x;
    if ( (q_trk_x>12.5)&&(q_trk_y>12.5) ) q_trk = q_trk_y;
    tof_track->MakeMean(TofTrack::kAnode,TofTrack::kSqrt,TofTrack::kMIP);
    float q_tof = tof_track->GetMean(TofTrack::kAnode);
    float dq = fabs(q_trk-q_tof);
    Histogram("TofTrkMatch_Q_vs_Q","; Q_{TOF}; Q_{TRK}",300,0,30,300,0,30,q_tof,q_trk); 
    Histogram("TofTrkMatch_dQ_vs_Q","; Q_{TOF}; #Delta Q",300,0,30,300,0,30,q_tof,dq);
    // take good track with minimum distance
    if (distance<min_distance) {
      min_distance = distance;
      min_costheta = costheta;
      trk_track = track;
      // cout << min_distance << " " << min_costheta << " " << dq << endl;
    }
    if (dq<min_dq) {
      min_dq = dq; 
      min_q_tof = q_tof;
      trk_track = track; 
    }
    // cout << itrack << " " << distance << " " << dq << endl; 
  }
  Histogram("TofTrkMatch_dQ_vs_Q_min","; Q_{TOF}; #Delta Q",300,0,30,300,0,30,min_q_tof,min_dq);

  //if (trk_track) cout << "OK" << endl;
  //else           cout << "KO" << endl;

  return trk_track;
}
*/

/*
bool TofRecon::GoodMatchTofTrackAndTrTrack(TofTrack* tof_track, TrTrackR* trk_track) {
  return true;
  if ( (tof_track==0)||(trk_track==0) ) return false; 
  // tracker-tof match (at trk layer 1)
  float z_match = TkDBc::Head->GetZlayer(2);
  // quantities on witch is possible to cut (afterwords)
  float min_distance = 10000;
  float min_costheta = 0;
  AMSPoint tof_point = tof_track->GetPoint(z_match);
  AMSDir   tof_dir = tof_track->GetDir();
  AMSPoint trk_point;
  AMSDir   trk_dir;
  trk_track->Interpolate(z_match,trk_point,trk_dir);
  float beta = tof_track->GetBeta();
  float costheta = trk_dir.prod(tof_dir);
  float dx = (trk_point-tof_point).x();
  float dy = (trk_point-tof_point).y();
  float distance = sqrt(dx*dx + dy*dy);
  distance = fabs(dy); // only-y
  /////////////////////////////////////////////
  // - dx is wrong of 7% for inner tracker tracks
  //   this is due to wrong multiplicity leading to wrong inclination
  //   Y projection is OK, X and Y of k89 tracks are OK.
  // - decided to use only on dy, following the idea that the
  //   dx could be solved in the next release B530  
  /////////////////////////////////////////////
  if ( (distance<10)&&(costheta>0.9) ) return true;
  return false;
}
*/

EcalShowerR* TofRecon::GetClosestEcalShower(TofTrack* tof_track, AMSEventR* event) {
  EcalShowerR* ecal_shower = 0;
  float min_distance = 10000;
  float min_costheta = 0;
  // match at calorimeter entry (-142 or -157 ... ?) 
  for (int ishower=0; ishower<event->NEcalShower(); ishower++) {
    EcalShowerR* shower = event->pEcalShower(ishower);
    AMSPoint ecal_point(shower->Entry);
    AMSDir   ecal_dir(shower->Dir);
    AMSPoint tof_point = tof_track->GetPoint(ecal_point.z());
    AMSDir   tof_dir = tof_track->GetDir();
    float    costheta = fabs(ecal_dir.prod(tof_dir)); // could be negative ... 
    float    dx = (ecal_point-tof_point).x();
    float    dy = (ecal_point-tof_point).y();
    float    distance = sqrt(dx*dx + dy*dy);
    Histogram("TofEcalMatch_dY_vs_dX","; #DeltaX (cm); #DeltaY (cm)",200,-20,20,200,-20,20,dx,dy);
    Histogram("TofEcalMatch_Dist_vs_CosTheta","; cos(#theta); Distance (cm)",200,0.9,1.0,200,0,20,costheta,distance);
    if (distance<min_distance) {
      min_distance = distance;
      min_costheta = costheta;
      ecal_shower = shower;
    }
  }
  // minimum request
  if ( (min_costheta>0.9)&&(min_distance<20) ) return ecal_shower;
  return 0;
}


TrdTrackR* TofRecon::GetClosestTrdTrack(TofTrack* tof_track, AMSEventR* event) {
  TrdTrackR* trd_track = 0;
  float min_distance = 10000;
  float min_costheta = 0;
  // match at one of the beginning layers (around 110 cm)  
  for (int itrack=0; itrack<event->NTrdTrack(); itrack++) {
    TrdTrackR* track = event->pTrdTrack(itrack);
    AMSPoint trd_point(track->Coo);
    AMSDir   trd_dir(track->Theta,track->Phi);
    AMSPoint tof_point = tof_track->GetPoint(trd_point.z());
    AMSDir   tof_dir = tof_track->GetDir();
    float    costheta = fabs(trd_dir.prod(tof_dir)); // is negative ... 
    float    dx = (trd_point-tof_point).x();
    float    dy = (trd_point-tof_point).y();
    float    distance = sqrt(dx*dx + dy*dy);
    Histogram("TofTrdMatch_dY_vs_dX","; #DeltaX (cm); #DeltaY (cm)",200,-20,20,200,-20,20,dx,dy);
    Histogram("TofTrdMatch_Dist_vs_CosTheta","; cos(#theta); Distance (cm)",200,0.9,1.0,200,0,20,costheta,distance);
    if (distance<min_distance) {
      min_distance = distance;
      min_costheta = costheta;
      trd_track = track;
    }
  }
  // minimum request
  if ( (min_costheta>0.98)&&(min_distance<10) ) return trd_track;
  return 0;
}


RichRingR* TofRecon::GetClosestRichRing(TofTrack* tof_track, AMSEventR* event) {
  RichRingR* rich_ring = 0;
  float min_distance = 10000;
  float min_costheta = 0;
  // match at the top of the radiator 
  for (int iring=0; iring<event->NRichRing(); iring++) {
    RichRingR* ring = event->pRichRing(iring);
    AMSPoint rich_point(ring->AMSTrPars[0],ring->AMSTrPars[1],ring->AMSTrPars[2]);
    AMSDir   rich_dir(ring->AMSTrPars[3],ring->AMSTrPars[4]);
    AMSPoint tof_point = tof_track->GetPoint(rich_point.z());
    AMSDir   tof_dir = tof_track->GetDir();
    float    costheta = fabs(rich_dir.prod(tof_dir)); // is negative ... 
    float    dx = (rich_point-tof_point).x();
    float    dy = (rich_point-tof_point).y();
    float    distance = sqrt(dx*dx + dy*dy);
    Histogram("TofRichMatch_dY_vs_dX","; #DeltaX (cm); #DeltaY (cm)",200,-20,20,200,-20,20,dx,dy);
    Histogram("TofRichMatch_Dist_vs_CosTheta","; cos(#theta); Distance (cm)",200,0.9,1.0,200,0,20,costheta,distance);
    if (distance<min_distance) {
      min_distance = distance;
      min_costheta = costheta;
      rich_ring = ring;
    }
  }
  // minimum request
  if ( (min_costheta>0.98)&&(min_distance<10) ) return rich_ring;
  return 0;
}



////////////////////////////////
// TofTimeCalibration
////////////////////////////////

ClassImp(TofTimeCalibration);

float TofTimeCalibration::slewing_par[4][10][2]={
  { 
    {17.96291,16.83554},{14.0314,12.99512},{15.61507,16.43706},{17.04257,12.80316},{14.72713,13.87814},
    {14.67123,13.62868},{12.5161,15.16941},{15.94649,12.65001},{0.,0.},{0.,0.}
  },
  {
    {13.4411,15.00852},{16.40132,16.68143},{15.5744,15.93634},{13.96471,18.62774},{13.37814,18.22032},
    {15.08228,21.13302},{11.45179,17.04846},{17.42891,15.19894},{0.,0.},{0.,0.}
  },
  {
    {18.39408,9.263855},{13.89204,15.32413},{16.83036,12.15041},{13.89913,12.17081},{16.5926,10.46456},
    {13.83534,11.05082},{13.37524,9.290484},{11.34494,10.92213},{16.35789,9.630091},{13.94434,11.06138}
  },
  {
    {16.37876,18.50454},{13.52378,11.42957},{13.45374,14.93331},{15.82747,10.52073},{13.6899,16.47858},
    {14.73641,11.42947},{19.08128,12.82591},{15.60377,11.66079},{0.,0.},{0.,0.}
  }
};      
int          TofTimeCalibration::max_calibration_run = 0;
unsigned int TofTimeCalibration::calibration_run[2000];
int          TofTimeCalibration::calibration_run_file[2000];
float        TofTimeCalibration::t_zero[4][10][2000];
string       TofTimeCalibration::calibration_folder = ".";
bool         TofTimeCalibration::init_calibrations = false;

void TofTimeCalibration::LoadCalibrations() {
  printf("TofTimeCalibration::LoadCalibration-V initialize TOF calibrations.\n");
  init_calibrations = true;
  FILE *tzero_in;
  string line;
  ifstream inf;
  inf.open(Form("%s/TOF_calibration/calibration_run_list.txt",calibration_folder.c_str()),ifstream::in);
  if (!inf.is_open()) {
    printf("CalibrationLoad-W file %s/TOF_calibration/calibration_run_list.txt not found. Skip.\n",calibration_folder.c_str());
    return;
  }
  printf("CalibrationLoad-V reading file %s/TOF_calibration/calibration_run_list.txt\n",calibration_folder.c_str());
  int ncal=0;
  while (inf.good()) {
    getline(inf,line);
    sscanf (line.c_str(),"%d %d",&calibration_run[ncal],&calibration_run_file[ncal]);
    tzero_in = fopen(Form("%s/TOF_calibration/tof_tzero_pars_Z2_def_%d.txt",calibration_folder.c_str(),calibration_run_file[ncal]),"r");
    if (tzero_in==0) {
      printf("CalibrationLoad-W file %s/TOF_calibration/tof_tzero_pars_Z2_def_%d.txt not found. Skip.\n",calibration_folder.c_str(),calibration_run_file[ncal]);
      continue;
    }
    for (int il=0;il<4;il++) {
      for (int ic=0;ic<10;ic++) {
        if (il!=2 && ic>7) continue;
        fscanf(tzero_in,"%e\n",&t_zero[il][ic][calibration_run_file[ncal]]);
      }
    }
    fclose(tzero_in);
    ncal++;
  }
  printf("CalibrationLoad-V %d calibrations read.\n",ncal);
  max_calibration_run = ncal;
  inf.close();
}

void TofTimeCalibration::RecalculateTofClusterTimes(AMSEventR* event) {
  if (event==0) return;
  if (!init_calibrations) LoadCalibrations();
  // take calibration file
  unsigned int run = event->Run();
  int cal_file = calibration_run_file[max_calibration_run-1]; // default is last one
  for (int i=0; i<max_calibration_run-1; i++) 
    if ( (run>=calibration_run[i]) && (run<calibration_run[i+1]) ) 
      cal_file=calibration_run_file[i];
  // improve
  for (int icluster=0; icluster<event->NTofCluster(); icluster++) {
    TofClusterR* cluster = event->pTofCluster(icluster);
    float timesum=0;
    for (int irawcluster=0; irawcluster<cluster->NTofRawCluster(); irawcluster++) {
      TofRawClusterR* rawcluster = cluster->pTofRawCluster(irawcluster);
      int ilayer = rawcluster->Layer - 1;
      int ibar   = rawcluster->Bar - 1;
      float anode_adc[2] = {-1000,-1000};
      float anode_tdc[2] = {-2000,-2000};
      for(int iside=0; iside<2; iside++){ 
        anode_adc[iside] = rawcluster->adca[iside];
        anode_tdc[iside] = rawcluster->sdtm[iside];
      }
      float times = ( 
        anode_tdc[0] + slewing_par[ilayer][ibar][0]/sqrt(anode_adc[0]) +
        anode_tdc[1] + slewing_par[ilayer][ibar][1]/sqrt(anode_adc[1]) ) / 2 
        - t_zero[ilayer][ibar][cal_file];
      rawcluster->time = times;
      timesum += times;
    }
    cluster->Time = -timesum*1e-9/cluster->NTofRawCluster(); 
  } 
}


