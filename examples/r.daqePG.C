///   \example r.daqePG.C 
///   This is an example of how to load daqePG.C (to run: root;  .x r.daqePG.C)   
{
  // Global ROOT Reset
  gROOT->Reset();

  // Set AMSWD directory
       gSystem->Setenv("AMSWD","/afs/cern.ch/ams//Offline/vdev");

  // Set include path
  gInterpreter->AddIncludePath(gSystem->ExpandPathName("$AMSWD/include"));

  // Load AMS shared library
  // For Linux 2.6 load
  gSystem->Load("$AMSWD/lib/linuxx8664icc/ntuple_slc4_PG.so");

  //gDebug=5;
  // For Linux 2.4

  // Add all AMS Root files to be analyzed 
  TChain chain("AMSRoot");
  //chain.Add("/r0fc00//Data/AMS02/2009B/bt.aug2010.v5/pr400/1281704189.*.root");
  //chain.Add("/fcdat1/Data/AMS02/2009B/bt.aug2010.v5.p2/pr400.416/*.root");
chain.Add("root://eosams.cern.ch///eos/ams/Data/AMS02/2011B/ISS.B620/pass4/1310617621.00000001.root");
 chain.Process("daqePG_simple.C+","daqePG.root",100000);



}

