/// \example ana.root.C
/// This is another example to analyze an AMS Root file in an interactive 
/// way, without code compilation. It just requires to load the Root library,
/// which you should create with "cd $AMSDir/install; gmake shared"
///
/// (to run: root;  .x ana.root.C)   
/// 
{
      gROOT->Reset();
      // for linux load
      gSystem->Load("$AMSDir/lib/linux/icc/ntuple.so");
      //
      //  for dunix aka ams.cern.ch load 
      //  gSystem->Load("$AMSDir/lib/osf1/ntuple.so");
      //

      AMSChain ams;
      ams.Add("/f2users/choutko/g3v1g3.root");

      TH1F* hrig = new TH1F ("hrig", "Momentum (GeV)", 50, -10., 10.);
      
      int ndata = ams.GetEntries();
      cout << "We will process: " << ndata << " events" << endl;
      for (int entry=0; entry<ndata; entry++) {
            AMSEventR *pev = ams.GetEvent();
            if (pev==NULL) break;
            AMSEventR &ev = *pev;
            for (int i=0; i<ev.nParticle(); i++) {
                  ParticleR part = ev.Particle(i);
                  hrig->Fill(part.Momentum);
            }
      }

      hrig->Draw();
      cout << "We have processed: " << ndata << " events" << endl;
}
