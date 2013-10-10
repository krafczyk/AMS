///   \example run.stlv.C
///   This is an example of how to load stlv.C (to run: root;  .x run.stlv.C)   
{



// Global ROOT Reset
      gROOT->Reset();

// Set AMSWD directory
      gSystem->Setenv("AMSWD","/afs/cern.ch/ams/Offline/vdev");

// Set include path
      gInterpreter->AddIncludePath(gSystem->ExpandPathName("$AMSWD/include"));

// Load AMS shared library
// For Linux load
      gSystem->Load("$AMSWD/lib/linuxx8664icc/ntuple_slc4_PG.so");



// Add all AMS Root files to be analyzed 
      TChain chain("AMSRoot");

// Usual ROOT file
//chain.Add("/f0dah3/MC/AMS02/2005A/deuterons/el.pl1.0510/1355.0*.root");
//      chain.Add("/f2users/choutko/g3v1g3.5.root");
//      chain.Add("/f2users/alcaraz/ams/gamma/protons_el.pl1.10200.root");

// Add a ROOT file on the eos
      chain.Add("root://eosams.cern.ch///eos/ams/Data/AMS02/2011B/ISS.B620/pass4/1310617621.00000001.root");
// Add a ROOT file on Castor
      //chain.Add("root://castorpublic.cern.ch///castor/cern.ch/amsfMC/AMS02/2004A/protons/el.pl1.10200/738197524.0000001.root");

// Process the file and loop over events
// Inside stlv.C, you have to implement the class "stlv"
// Everything works OK if you change all "stlv" by "ANYTHING" ==>
//     "ANYTHING.C" + "class ANYTHING" + chain.Process("ANYTHING.C+")
      chain.Process("stlv.C+");
// Write created histograms in file "outputfilename");
      //chain.Process("stlv.C+","outputfilename");
// Do not compile, interpret it interactively
      //chain.Process("stlv.C");
// Force recompilation of library at any time
      //chain.Process("stlv.C++");

// Open Root browser (optional; interactive use only)
      // new TBrowser;
      // gStyle->SetOptFit(1111);
      // ...

}
