/// \example run.root.C
/// This is an example of how see ams variables interactively in root session
///
/// (to run: root;  .x run.root.C)   
/// 
/// for complete listing of var functions see root_methods.h

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
chain.Draw("ev.trtrack_rigidity(0)", "ev.trtrack_rigidity(0)>0");
}


