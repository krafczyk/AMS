///   \example run.trd_likelihood.C
///   Macro to process trd_likelihood.C (To run it: root -b -q run.trd_likelihood.C)   
{
      gROOT->Reset();
      gInterpreter->AddIncludePath(gSystem->ExpandPathName("${AMSWD}/include"));
      gSystem->Load(gSystem->ExpandPathName("${AMSWD}/lib/linuxx8664icc/ntuple_slc4_PG.so"));

      TChain chain("AMSRoot");
      chain.Add("root://castorpublic.cern.ch///castor/cern.ch/ams/Data/AMS02/2011B/ISS.B530/std/1313111683.00000001.root");
      
      chain.Process("trd_likelihood.C+");
}
