/*! \example ana.root.C
 *  This is another example to analyze an AMS Root file in an interactive 
 *  way, without code compilation. It just requires to load the Root library,
 *  which you should create with "cd $AMSDir/install; gmake shared"
 * 
 *  (to run: root;  .x ana.root.C)   
 */ 
{
gROOT->Reset();

// for linux load
gSystem->Load("$AMSDir/lib/linux/icc/ntuple.so");
//
//  for dunix aka ams.cern.ch load 
//  gSystem->Load("$AMSDir/lib/osf1/ntuple.so");
//

// Pile-up ROOT files
TChain chain("AMSRoot");
chain.Add("/f2users/choutko/g3v1g3.root");
chain.SetMakeClass(1);

// We need: a) An AMSEventR as object receptor 
//          b) To set the addresses for all branches
AMSEventR Event;
Event.SetBranchA(&chain);

// Finally, your fast analysis

// Init
TH1F* h_ang = new TH1F ("h_ang", "Delta angle (rad)", 50, -0.05, 0.05);
gRandom->SetSeed();

int Nentries = (int)chain.GetEntries();

// Loop on events
for (int entry=0; entry<Nentries; entry++) {
    chain.GetEntry(entry);

    for (int i=0; i<Event.nTrTrack(); i++) {
      TrTrackR tr = Event.TrTrack(i);
	float phi = tr.Phi;
	float theta = tr.Theta;
      for (int j=0; j<Event.nMCEventg(); j++) {
	  MCEventgR mcevent = Event.MCEventg(j);
	  if (mcevent.Particle==14) {
	    float dotprod = sin(theta)*cos(phi)*mcevent.Dir[0]
	                  + sin(theta)*sin(phi)*mcevent.Dir[1]
	                  + cos(theta)         *mcevent.Dir[2];
	    if (gRandom->Rndm()>0.5) {
	      h_ang->Fill(acos(dotprod));
	    } else {
	      h_ang->Fill(-acos(dotprod));
	    }
	    break;
	  }
      }
    }
    
}

// Terminate
h_ang->Draw();
cout << "We have processed: " << Nentries << " events" << endl;
cout << "Bye" << endl;

}
