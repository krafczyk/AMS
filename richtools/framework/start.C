{
  // Global ROOT Reset
  gROOT->Reset();

  // Set AMSWD directory 
  //  gSystem->Setenv("AMSWD","/Offline/vdev");

  // Set include path
  gInterpreter->AddIncludePath(gSystem->ExpandPathName("$AMSWD/include"));
  
  // Load AMS shared library
  gSystem->Load("$AMSWD/lib/linuxx8664icc/ntuple_slc4_PG.so");

  gInterpreter->Load("dst.C++");
  gInterpreter->Load("RichMass.C++");
  using namespace Framework;

  AMSChain chain;
  chain.Add("/r0fc00/Data/AMS02/2009B/data2010/cosmics.ksc.p3/*.root");

  RichMass example;
  chain.Process(&example,"cosilla.root");
}
