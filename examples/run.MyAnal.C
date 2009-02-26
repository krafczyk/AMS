///   \example run.MyAnal.C
///   Macro to process MyAnal.C (To run it: root -b -q run.MyAnal.C)   
{
      gROOT->Reset();

      gInterpreter->AddIncludePath(gSystem->ExpandPathName("$AMSDir/include"));
      gSystem->Load("$AMSDir/lib/linux/ntuple_slc4_icc.so");

      TChain chain("AMSRoot");
      chain.Add("/f2users/fgiovacc/NewNewCommit/AMS/MCprod/muons/101/*.root");

      // Either define the output ROOT file ("select.root") here:
      chain.Process("MyAnal.C+","select.root");
      // Or play with output files inside the code:
      //chain.Process("MyAnal.C+");

      // Other options: 
      //chain.Process("MyAnal.C"); // not compiled
      //chain.Process("MyAnal.C++"); // force compilation
      //chain.Process("MyAnal.C","select.root");
      //chain.Process("MyAnal.C++","select.root");
}
