/// \example run.stlv.C
/// This is an example of how to load stlv.C
/// (to run: root;  .x run.stlv.C)   
/// 

{
gROOT->Reset();
gSystem->Load("/offline/vdev/lib/linux/icc/ntuple.so");
TChain chain("AMSRoot");
chain.Add("/f2users/choutko/root/dst/g3v1g3.root");
chain.Add("/f2users/choutko/root/dst/g3v1g3.full.root");
chain.Add("/f2users/choutko/root/dst/pl1/14/*.root");
//chain.Process("stlv.C+","outputfilename"); ///<  write histos to file
chain.Process("stlv.C+");
}


