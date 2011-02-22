///   \example r.daqePG.C 
///   This is an example of how to load daqePG.C (to run: root;  .x r.daqePG.C)   
{
  // Global ROOT Reset
  gROOT->Reset();

  // Set AMSWD directory
  //  gSystem->Setenv("AMSWD","/AMS");
  //     gSystem->Setenv("AMSWD","/Offline/vdev");

  // Set include path
  gInterpreter->AddIncludePath(gSystem->ExpandPathName("$AMSWD/include"));

  // Load AMS shared library
  // For Linux 2.6 load
  gSystem->Load("$AMSWD/lib/linux/ntuple_slc4_PG.so");

  TString cmd=gSystem->GetMakeSharedLib();
  cmd.ReplaceAll("-m64","-m32");
  gSystem->SetMakeSharedLib(cmd);


  //gDebug=5;
  // For Linux 2.4

  // Add all AMS Root files to be analyzed 
  TChain chain("AMSRoot");
  //chain.Add("/r0fc00//Data/AMS02/2009B/bt.aug2010.v5/pr400/1281704189.*.root");
  //chain.Add("/fcdat1/Data/AMS02/2009B/bt.aug2010.v5.p2/pr400.416/*.root");
  chain.Add("/data7/DATA/bt.aug2010.v5.p3/pr400.416/*.root"); 
  chain.Process("daqePG.C+","daqePG_pr400.root");


}
