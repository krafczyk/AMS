{     
// Resetting and loading AMS library
      gROOT->Reset();
      gSystem->Load("$AMSDir/lib/linux/ntuple.so");   
      
// Input AMS ROOT Chain
      AMSChain *ams = new AMSChain;              
      ams->Add("/f2users/choutko/g3v1g3.root");

// Output file and histograms
      char* chfile = "amstest.root";
      TFile* hfile = new TFile (chfile, "RECREATE"); 
      TH1F* hrig = new TH1F ("hrig", "New-Old Rigidity (GV)", 50, -1., 1.);
      
// Instantiate your track
      AMSMyTrack new_track;
      // AMSMyTrack new_track(0); // assume B=0 => straight line fit

// Loop to analyze entries
      int ndata = ams->GetEntries();
      for (int entry=0; entry<ndata; entry++) {
            AMSEventR* pev = ams->GetEvent();
            if (pev==NULL) break;
            for (int i=0; i<pev->nTrTrack(); i++) {
                  TrTrackR track = pev->TrTrack(i);
                  new_track.use_hits_from(&track);
                  for (int j=5; j<track.NTrRecHit(); j++) {
                        new_track.del_hit(track.pTrRecHit(j));
                  }
                  if (new_track.Fit()) {
                    hrig->Fill(new_track.Rigidity-track.RigidityWithoutMS);
                    //printf("Track number= %d, Old rig %f, New Rig %f\n"
                    //        , i, track.RigidityWithoutMS, new_track.Rigidity);
                  }
            }
      }

      hrig->Draw();
      hfile->Write();
      printf("We have processed %d events\n", ndata);
      printf("Histograms saved in '%s'\n", chfile);
}
