///   \example r.daqePG.C 
///   This is an example of how to load daqePG.C (to run: root;  .x r.daqePG.C)   
{
  // Global ROOT Reset
  gROOT->Reset();

  // Set AMSWD directory
  //  gSystem->Setenv("AMSWD","/Offline/vdev");
  //  gSystem->Setenv("AMSWD","/f2users/mdelgado/fresh/test/AMS");

  // Set include path
  gInterpreter->AddIncludePath(gSystem->ExpandPathName("$AMSWD/include"));
  
  // Load AMS shared library
  gSystem->Load("$AMSWD/lib/linuxx8664icc/ntuple_slc4_PG.so");
  
  // Add all AMS Root files to be analyzed 
  TChain chain("AMSRoot");
  //  chain.Add("/r0fc00//Data/AMS02/2009B/bt.aug2010.v5/pr400/1281704189.*.root");
  //chain.Add("/fcdat1/Data/AMS02/2009B/bt.aug2010.v5.p2/pr400.416/*.root");
  //chain.Add("/fcdat1/Data/AMS02/2009B/data2010/cosmics.ksc/*.root"); 
  chain.Add("/r0fc00/Data/AMS02/2009B/data2010/cosmics.ksc.p3/*.root"); 

  chain.Process("Analysis.C+","",6554)

  /*
  gInterpreter->Load("Analysis.C++");
  using namespace Framework;


  Analysis example;
  chain.Process(&example);
  */
}

