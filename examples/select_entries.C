{     
      gROOT->Reset();
      gSystem->Load("$AMSDir/lib/linux/icc/ntuple.so");   
      
      AMSChain *ams = new AMSChain;              
      ams->Add("/f2users/choutko/g3v1g3.root");

      // BEGIN of block: How to select only some branches ->
      ams->SetBranchStatus("*",0); // this disables all branches by default
      ams->SetBranchStatus("ev.",1); // mother branch
      ams->SetBranchStatus("ev.fHeader",1); // HeaderR class information IN
      ams->SetBranchStatus("ev.fParticle",1); // ParticleR class information IN
      // END of block

      TFile* hfile = new TFile ("amstest.root", "RECREATE"); 
      TTree *amsnew = ams->CloneTree(0);

      int ndata = ams->GetEntries();
      int nnew = 0;
      for (int entry=0; entry<ndata; entry++) {
            AMSEventR* pev = ams->GetEvent();
            if (pev==NULL) break;
            if (pev->nParticle()==1) {
               amsnew->Fill();
               nnew++;
            }
      }
      
      hfile->Write();
      cout << "Events in the old tree: " << ndata << endl;
      cout << "Events in the new tree: " << nnew << endl;
}
