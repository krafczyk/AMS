/// \example run.stlv.C
/// This is an example of how to load stlv.C
///  (to run: root;  .x run.stlv.C)   
/// 
{
gROOT->Reset();

// for linux load
gSystem->Load("/offline/vdev/lib/linux/icc/ntuple.so");
//
//  for dunix aka ams.cern.ch load 
//  gSystem->Load("/offline/vdev/lib/osf1/ntuple.so");
//
TChain chain("AMSRoot");
chain.Add("/f2users/choutko/g3v1g3.root");
//chain.Process("stlv.C+","outputfilename"); ///<  write histos to file
chain.Process("stlv.C+");
}
