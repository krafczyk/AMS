///   \example run.MyAnal.C
///   Macro to process MyAnal.C (To run it: root -b -q run.MyAnal.C)   
{
      gROOT->Reset();
      TString path = gSystem->GetIncludePath();
      path.Append(" -I$AMSDir/include");
      gSystem->SetIncludePath(path.Data());
      gSystem->Load("$AMSDir/lib/linux/ntuple.so");

      TChain chain("AMSRoot");
      chain.Add("/f2users/choutko/g3v1g3.root");

      // Either define the output ROOT file ("select.root") here:
      chain.Process("MyAnal.C+","select.root");
      // Or play with output files inside the code:
      //chain.Process("MyAnal.C+");
}
