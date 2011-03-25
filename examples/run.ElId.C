///   \example run.ElId.C
///   Macro to process ElectronIdentification.C (To run it: root -b -q run.ElId.C)   
{
      gROOT->Reset();

      gInterpreter->AddIncludePath(gSystem->ExpandPathName("${AMSDir}/include"));
      gSystem->Load(gSystem->ExpandPathName("${AMSDir}/lib/linuxx8664icc/ntuple_slc4_PG.so"));

      TChain chain("AMSRoot");
      chain.Add("/fcdat1/Data/AMS02/2009B/bt.aug2010.v5.p3/el100/1282150717.00000001.root");
      
      // Either define the output ROOT file ("select.root") here:
      chain.Process("ElectronIdentification.C+","select.root");
}
