///   \example run.daqe.C
///   This is an example of how to load daqe.C (to run: root;  .x run.daqe.C)   
{
// Global ROOT Reset
      gROOT->Reset();

// Set AMSWD directory
     gSystem->Setenv("AMSWD","/Offline/vdev");

// Set include path
      gInterpreter->AddIncludePath(gSystem->ExpandPathName("$AMSWD/include"));

// Load AMS shared library
// For Linux 2.6 load
//      gSystem->Load("$AMSWD/lib/linux/icc2/ntuple.so");
    gSystem->Load("$AMSWD/lib/linux/ntuple_slc4_icc.so");

       //gDebug=5;

// Add all AMS Root files to be analyzed 
      TChain chain("AMSRoot");
chain.Add("/s0fc00/Data/AMS02/2006A/data2008/cern.cosmics.sci.4p/*.root");
chain.Add("/r0fc00/Data/AMS02/2006A/data2008/cern.cosmics.sci.4p/*.root"); 
       chain.Process("daqe.C+","daqe.all.4p.root");


}
