///   \example run.stlv.C
///   This is an example of how to load stlv.C (to run: root;  .x run.stlv.C)   
{
      gROOT->Reset();

// Set include path
      TString path = gSystem->GetIncludePath();
      path.Append(" -I/afs/ams.cern.ch/Offline/vdev/include ");
      gSystem->SetIncludePath(path.Data());

// Load AMS shared library
// For Linux load
      gSystem->Load("/afs/ams.cern.ch/Offline/vdev/lib/linux/ntuple.so");
// For Digital Unix, i.e. ams.cern.ch load 
      //gSystem->Load("/afs/ams.cern.ch/Offline/vdev/lib/osf1/ntuple.so");

// Add all AMS Root files to be analyzed 
      TChain chain("AMSRoot");
// Usual ROOT file
      chain.Add("/f2users/choutko/g3v1g3.root");
// Add a ROOT file on the WeB
      //chain.Add("http://pcamsf0.cern.ch/f2dah1/MC/AMS02/2004A/protons/el.pl1.10200/738197524.0000001.root");
// Add a ROOT file on Castor
      //chain.Add("rfio:/castor/cern.ch/amsfMC/AMS02/2004A/protons/el.pl1.10200/738197524.0000001.root");

// Process the file and loop over events
// Inside stlv.C, you have to implement the class "stlv"
// Everything works OK if you change all "stlv" by "ANYTHING" ==>
//     "ANYTHING.C" + "class ANYTHING" + chain.Process("ANYTHING.C+")
      chain.Process("stlv.C+");
// Write created histograms in file "outputfilename");
      //chain.Process("stlv.C+","outputfilename");
// Force recompilation of library at any time
      //chain.Process("stlv.C++");

// Open Root browser (optional; interactive use only)
      // new TBrowser;
      // gStyle->SetOptFit(1111);
      // ...

}
