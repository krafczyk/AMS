///   \example run.MyAnal.C
///   Macro to process MyAnal.C (To run it: root -b -q run.MyAnal.C)   
{
      gROOT->Reset();
      gSystem->Load("$AMSDir/lib/linux/ntuple.so");

      TChain chain("AMSRoot");
      chain.Add("/f2users/choutko/g3v1g3.root");
      chain.Process("MyAnal.C+");
}
